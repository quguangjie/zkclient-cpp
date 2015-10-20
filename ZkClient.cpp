/*
 *  Author: GuangJie Qu <qgjie456@163.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, 
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ZkClient.h"
#include "ZkConnection.h"
#include "ZkEventThread.h"
#include "ZkLock.h"
#include "pthread.h"
#include "zookeeper.h"




/**
 * Create a client
 *
 * @param connectstring     zookeeper connection string
 *                          comma separated host:port pairs, each corresponding to a zk
 *                          server. e.g. "127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002" If
 *                          the optional chroot suffix is used the example would look
 *                          like: "127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002/app/a"
 *                          where the client would be rooted at "/app/a" and all paths
 *                          would be relative to this root - ie getting/setting/etc...
 *                          "/foo/bar" would result in operations being run on
 *                          "/app/a/foo/bar" (from the server perspective).
 * @param sessionTimeout    session timeout in milliseconds
 * @param connectionTimeout connection timeout in milliseconds
 */
ZkClient::ZkClient(const string &connectstring, long sessionTimeout, long connectionTimeout):ZkWatcher()       
{
	_childListener.clear();
	_dataListener.clear();
	_stateListener.clear();
	_currentState = ZkConnection::NOTCONNECTED;
	_connection = new ZkConnection(connectstring, sessionTimeout); 
	connect(connectionTimeout, this);  
	cout << "ZkClient::ZkClient" << endl;
}

ZkClient::~ZkClient()
{
	if(_currentState != ZkConnection::NOTCONNECTED)
		close();
	delete _connection;  
}


list<string> ZkClient::subscribeChildChanges(const string &path, shared_ptr<IZkChildListener> listener) 
{
	getListenerLock().lock();
	map<string, list<shared_ptr<IZkChildListener> > >::iterator iter = _childListener.find(path);  
	if (iter == _childListener.end()) 
	{
		list<shared_ptr<IZkChildListener> > l;
		_childListener[path] = l;
	}
	(_childListener[path]).push_back(listener);
	getListenerLock().unlock();
	return watchForChilds(path);
}

void ZkClient::unsubscribeChildChanges(const string &path, shared_ptr<IZkChildListener> childListener) 
{
	getListenerLock().lock();
	map<string, list<shared_ptr<IZkChildListener> > >::iterator iter = _childListener.find(path);  
	if (iter != _childListener.end()) 
	{
		(_childListener[path]).remove(childListener);
	}
	getListenerLock().unlock();
}

void ZkClient::subscribeDataChanges(const string &path, shared_ptr<IZkDataListener>  listener) 
{
	getListenerLock().lock();
	map<string, list<shared_ptr<IZkDataListener> > >::iterator iter = _dataListener.find(path);  
	if (iter == _dataListener.end()) 
	{
		list<shared_ptr<IZkDataListener> > l;
		_dataListener[path] = l;
	}
	_dataListener[path].push_back(listener);
	getListenerLock().unlock();
	watchForData(path);
}

void ZkClient::unsubscribeDataChanges(const string &path, shared_ptr<IZkDataListener> dataListener) 
{
	getListenerLock().lock();
	map<string, list<shared_ptr<IZkDataListener> > >::iterator iter = _dataListener.find(path);  
	if (iter != _dataListener.end()) 
	{
		_dataListener[path].remove(dataListener);
	}
	getListenerLock().unlock();
}

void ZkClient::subscribeStateChanges(shared_ptr<IZkStateListener> listener) 
{
	getListenerLock().lock();
	_stateListener.push_back(listener);
	getListenerLock().unlock();
}

void ZkClient::unsubscribeStateChanges(shared_ptr<IZkStateListener> stateListener) 
{
	getListenerLock().lock();
	_stateListener.remove(stateListener);
	getListenerLock().unlock();
}

void ZkClient::unsubscribeAll() 
{
	getListenerLock().lock();
	_childListener.clear();
	_dataListener.clear();
	_stateListener.clear();
	getListenerLock().unlock();
}


void ZkClient::createPersistent(const string &path) 
{
		createPersistent(path, false);
}

void ZkClient::createPersistent(const string &path, bool createParents) 
{
	try {
		create(path, NULL, 0);
	} catch (ZkExceptionNodeExists &e) {
		if (!createParents) {
			throw e;
		}
	} catch (ZkExceptionNoNode &e) {
		if (!createParents) {
			throw e;
		}
		string parentDir = path.substr(0, path. find_last_of('/'));
		createPersistent(parentDir, createParents);
		createPersistent(path, createParents);
	}
}


void ZkClient::createPersistent(const string &path, const char data[]) 
{
        create(path, data, 0);
}


string ZkClient::createPersistentSequential(const string &path, const char data[]) 
{
	return create(path, data,  ZOO_SEQUENCE);
}


void ZkClient::createEphemeral(const string &path) 
{
	create(path, NULL, ZOO_EPHEMERAL);
}

string ZkClient::create(const string &path, const char data[], int mode) 
{
	const char * bytes = data;

	while (true) {
		try {
			return _connection->create(path, bytes, mode);
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) { 
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Expired'
			usleep(ZKCLIENT_WAITTIME_USEC); 
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) { 
			throw e;
		} catch (ZkExceptionNodeExists &e) {			
			throw e;
		} catch (ZkExceptionNoNode &e) {			
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

void ZkClient::createEphemeral(const string &path, const char  data[])   
{
	create(path, data, ZOO_EPHEMERAL);
}

string ZkClient::createEphemeralSequential(const string &path, const char  data[]) 
{
	return create(path, data, ZOO_EPHEMERAL | ZOO_SEQUENCE);
}

void ZkClient::process(ZkWatchedEvent &event) 
{
	printf("event type[%d]\n", event.getType());
	bool stateChanged = event.getType() == ZkWatchedEvent::StateChanged;
	bool dataChanged  =  event.getType() == ZkWatchedEvent::NodeDataChanged || //
		event.getType() == ZkWatchedEvent::NodeDeleted ||
		event.getType() == ZkWatchedEvent::NodeCreated || //
		event.getType() == ZkWatchedEvent::NodeChildrenChanged;

	if (getShutdownTrigger()) {
		return ;   
	}
	getListenerLock().lock();
	if (stateChanged) {
		processStateChanged(event);
	}
	if (dataChanged) {
		// We might have to install child change event listener if a new node was created
		processDataOrChildChange(event);
	}
	getListenerLock().unlock();

	if (stateChanged) {
		getEventLock().signalAllStateChanged();

		// If the session expired we have to signal all conditions, because watches might have been removed and
		// there is no guarantee that those
		// conditions will be signaled at all after an Expired event
	 	if(_connection->getZookeeperState() == ZkConnection::EXPIRED_SESSION){  
			getEventLock().siganlAllZNodeEvent();
			getEventLock().siganlAllDataChanged();
			// We also have to notify all listeners that something might have changed
			fireAllEvents();
		}
	}
}

void ZkClient::fireAllEvents() 
{
	map<string, list<shared_ptr<IZkChildListener> > >::iterator citer;
	map<string, list<shared_ptr<IZkDataListener> > >::iterator  diter;
	getListenerLock().lock();
	for(citer = _childListener.begin(); citer != _childListener.end(); ++citer)
	{
		string path = (*citer).first;
		fireChildChangedEvents(path, (*citer).second);
	}
	for(diter = _dataListener.begin(); diter != _dataListener.end(); ++diter)
	{
		string path = (*diter).first;
		fireDataChangedEvents(path, diter->second);
	}
	getListenerLock().unlock();
}

list<string> ZkClient::getChildren(const string &path) 
{
	return getChildren(path, hasListeners(path));
}

list<string> ZkClient::getChildren(const string &path, bool watch) 
{
	while (true) {
		try {
			return _connection->getChildren(path, watch);
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Expired'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}


int ZkClient::countChildren(const string &path) 
{
	try {
		struct Stat stat;
		readData(path, &stat);
		return stat.numChildren;
	} catch (ZkExceptionNoNode e) {
		return -1;
	}
}

bool ZkClient::exists(const string &path,  bool watch) 
{
	while (true) {
		try {
			return _connection->exists(path, watch);
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionNoNode &e) {
			printf("exists ZkExceptionNoNode\n");
			return false;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

bool ZkClient::exists(const string  &path) 
{
	return exists(path, hasListeners(path));
}

void ZkClient::processStateChanged(ZkWatchedEvent &event) 
{
	setCurrentState(_connection->getZookeeperState());
	if (getShutdownTrigger()) {
		return;
	}
	try {
		fireStateChangedEvent(_connection->getZookeeperState());
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);

	 	if(_connection->getZookeeperState() == ZkConnection::EXPIRED_SESSION){  
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			reconnect();
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			fireNewSessionEvents();
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
		}
	} catch (ZkException e) {
		throw e;
	}
}

void ZkClient::fireNewSessionEvents() 
{	
	list<shared_ptr<IZkStateListener> >::iterator  it;
	for(it =  _stateListener.begin(); it !=  _stateListener.end(); ++it)
	{
		shared_ptr<IZkStateListener> stateListener = (*it);
		stateListener->handleNewSession();
	}
}

void ZkClient::fireStateChangedEvent(int state) 
{
	list<shared_ptr<IZkStateListener> >::iterator  it;
	for(it =  _stateListener.begin(); it !=  _stateListener.end(); ++it)
	{
		shared_ptr<IZkStateListener> stateListener = (*it);
		stateListener->handleStateChanged(state);
	}
}

bool ZkClient::hasListeners(const string &path) 
{
	map<string, list<shared_ptr<IZkDataListener> > >::iterator iter = _dataListener.find(path);  
	if (iter !=  _dataListener.end() &&  _dataListener[path].size() > 0) 
	{
		return true;
	}
	map<string, list<shared_ptr<IZkChildListener> > >::iterator citer = _childListener.find(path);  
	if (citer !=  _childListener.end() &&  _childListener[path].size() > 0) 
	{
		return true;
	}
	return false;
}

bool ZkClient::deleteRecursive(const string &path) 
{
	list<string> children;
	try {
		children = getChildren(path, false);
	} catch (ZkExceptionNoNode &e) {
		return true;
	}

	for(list <string>::iterator iter = children.begin(); iter != children.end(); iter++)
	{
		string subpath = path + "/" + *iter;
		if (!deleteRecursive(subpath)) {
			return false;
		}
	}
	return deletePath(path);
}

void ZkClient::processDataOrChildChange(ZkWatchedEvent &event) 
{
	 string path = event.getPath();

	if (event.getType() == ZkWatchedEvent::NodeChildrenChanged ||
			event.getType() == ZkWatchedEvent::NodeCreated ||
			event.getType() == ZkWatchedEvent::NodeDeleted) {
		map<string, list<shared_ptr<IZkChildListener> > >::iterator iter = _childListener.find(path);  
		if (iter != _childListener.end() && !_childListener[path].empty()) 
		{
			fireChildChangedEvents(path, _childListener[path]);
		}
	}

	if (event.getType() == ZkWatchedEvent::NodeDataChanged ||
			event.getType() == ZkWatchedEvent::NodeDeleted ||
			event.getType() == ZkWatchedEvent::NodeCreated) {
		map<string, list<shared_ptr<IZkDataListener> > >::iterator iter =  _dataListener.find(path);  
		if (iter != _dataListener.end() && !_dataListener[path].empty()) 
		{
			fireDataChangedEvents(path, _dataListener[path]);
		}
	}
}

void ZkClient::fireDataChangedEvents(const string &path, list<shared_ptr<IZkDataListener> > &listeners) 
{
	list<shared_ptr<IZkDataListener> >::iterator  it;
	for(it =  listeners.begin(); it !=  listeners.end(); ++it)
	{
		shared_ptr<IZkDataListener> listener = (*it);
		// reinstall watch
		try {
			exists(path, true);
			string data = readData(path, NULL, true);
			listener->handleDataChange(path, data);
			/*
			 * 当父节点创建后,注册关注子目录事件,并把父节点事件删除掉
			 */
			if(listener->reInstallWatchChildren && reInstallWatchChildren(path))
			{
				list<shared_ptr<IZkDataListener> >::iterator  it2 = it++;
				listeners.erase(it2);
			}
		} catch (ZkExceptionNoNode &e) {
			listener->handleDataDeleted(path);
		} catch (ZkExceptionSessionExpired &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

void ZkClient::fireChildChangedEvents(const string &path, list<shared_ptr<IZkChildListener> > &childListeners) 
{
	list<shared_ptr<IZkChildListener> >::iterator  it;
	for(it =  childListeners.begin(); it !=  childListeners.end(); ++it)
	{
		shared_ptr<IZkChildListener> listener = *(it);
		try {
			// if the node doesn't exist we should listen for the root node to reappear
			exists(path);
			list<string> children = getChildren(path);
			listener->handleChildChange(path, children);
		} catch (ZkExceptionNoNode &e) {
			// must do something. add by quguangjie
			#if 0
			listener->handleParentChange(path);  
			IZkDataListener parentListener(path);
			parentListener.reInstallWatchChildren = true;
			subscribeDataChanges(path, &parentListener);
			#endif 
		}
	}
}

bool ZkClient::waitUntilExists(const string &path, long timeUnit, long time)
{
	return false; 
}

bool ZkClient::waitUntilConnected()
{
	// wait 30 seconds  
	return waitUntilConnected(30);
}

bool ZkClient::waitUntilConnected(long time)
{
	return waitForKeeperState(ZkConnection::CONNECTED, time);
}

bool ZkClient::waitForKeeperState(ZkConnection::KeeperState keeperState, long timeout)
{
	getEventLock().lock();  
	bool stillWaiting = true;   
	while (_currentState != keeperState) {
		if (!stillWaiting) {
			getEventLock().unlock();
			return false;
		}
		stillWaiting = getEventLock().awaitUntilStateChanged(timeout);
	}
	getEventLock().unlock();
	return true;  
}


void ZkClient::setCurrentState(ZkConnection::KeeperState currentState) 
{
	_currentState = currentState;
}

/**
 * Returns a mutex all zookeeper events are synchronized aginst. So in case you need to do
 * something without getting any zookeeper event interruption synchronize against this
 * mutex. Also all threads waiting on this mutex object will be notified on an event.
 *
 * @return the mutex.
 */
ZkLock& ZkClient::getEventLock() 
{
	return _zkEventLock;
}
ZkLock& ZkClient::getListenerLock() 
{
	return _zkListenerLock;
}

bool ZkClient::deletePath(const string &path) 
{
	while (true) {
		try {
			_connection->deleteNode(path);
			return true;
		} catch (ZkExceptionNoNode &e) {
			throw e;
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

string ZkClient::readData(const string &path) 
{
	string data;
	try {
		data = readData(path, NULL);
	} catch (ZkExceptionNoNode &e) {
		throw e;
	}
	return data;
}

string ZkClient::readData(const string &path, struct Stat *stat) 
{
	return readData(path, stat, hasListeners(path));
}

string ZkClient::readData(const string &path, struct Stat *stat,  bool watch) 
{
	while (true) {
		try {
			return _connection->readData(path, stat, watch);
		} catch (ZkExceptionNoNode &e) {
			throw e;
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}
struct Stat ZkClient::writeData(const string &path, const string &data) 
{
	while (true) {
		try {
			return  _connection->writeData(path, data, -1);
		} catch (ZkExceptionNoNode &e) {
			throw e;
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

struct Stat ZkClient::writeData(const string &path, const char object[]) 
{
	return writeData(path, object, -1);
}

struct Stat ZkClient::writeData(const string &path, const char data[],  int expectedVersion) 
{
	while (true) {
		try {
			return  _connection->writeData(path, data, expectedVersion);
		} catch (ZkExceptionNoNode &e) {
			throw e;
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

void ZkClient::watchForData(const string &path) 
{
	while (true) {
		try {
			exists(path, true);
			return ;
		} catch (ZkExceptionNoNode &e) {
			return ;
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}

list<string> ZkClient::watchForChilds(const string &path) 
{
	list<string> tmpStr;
	tmpStr.clear();
	while (true) {
		try {
			exists(path, true);
			try {
				return getChildren(path, true);
			} catch (ZkExceptionNoNode &e) {
				// ignore, the "exists" watch will listen for the parent node to appear

				cout << path<<"  is no exist!!!!"<<endl;
			}
			
		} catch (ZkExceptionNoNode &e) {
			// watch for the parent node to appear
			return tmpStr;
			#if 0
			IZkDataListener parentListener(path);
			parentListener.reInstallWatchChildren = true;
			subscribeDataChanges(path, &parentListener);
			#endif 
		} catch (ZkExceptionConnectionLoss &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			// we give the event thread some time to update the status to 'Disconnected'
			usleep(ZKCLIENT_WAITTIME_USEC);
			waitUntilConnected();
		} catch (ZkExceptionSessionExpired &e) {
			ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
			send(evt);
			usleep(ZKCLIENT_WAITTIME_USEC);
			// we give the event thread some time to update the status to 'Expired'
			waitUntilConnected();
		} catch (ZkExceptionInterrupted &e) {
			throw e;
		} catch (ZkException &e) {
			throw e;
		}
	}
}


void ZkClient::connect(long maxMsToWaitUntilConnected, ZkWatcher *watcher) 
{
	try 
	{
		getEventLock().lock();
		setShutdownTrigger(false);
		_connection->connect(watcher);
		getEventLock().unlock();
		_currentState = ZkConnection::CONNECTED;
	} 
	catch (ZkExceptionConnectionLoss &e) 
	{
		getEventLock().unlock();
		throw e;
	} 
}    

long ZkClient::getCreationTime(const string &path) 
{
	getEventLock().lock();
	try {
		long ret;
		ret = _connection->getCreateTime(path);
		getEventLock().unlock();
		return ret;
	} catch (ZkExceptionInterrupted &e) {
		getEventLock().unlock();
		throw e;
	}  
}

void ZkClient::close() 
{
	getEventLock().lock();
	setShutdownTrigger(true);
	_currentState = ZkConnection::NOTCONNECTED;
	_connection->close();
	getEventLock().unlock();
}

void ZkClient::reconnect() 
{
	try {
		getEventLock().lock();
		_connection->close();
		_connection->connect(this);
		_currentState = ZkConnection::CONNECTED;
		getEventLock().unlock();
	} 
	catch (ZkExceptionConnectionLoss &e) 
	{
		ZkWatchedEvent evt(ZkWatchedEvent::StateChanged);
		send(evt);
		getEventLock().unlock();
		throw e;
	} 
}

void ZkClient::setShutdownTrigger(bool triggerState) 
{
	_shutdownTriggered = triggerState;
}

bool ZkClient::getShutdownTrigger() 
{
	return _shutdownTriggered;
}

int ZkClient::numberOfListeners() 
{
	int listeners = 0;
	map<string, list<shared_ptr<IZkChildListener> > >::iterator citer;
	map<string, list<shared_ptr<IZkDataListener> > >::iterator  diter;

	getListenerLock().lock();
	for(citer = _childListener.begin(); citer != _childListener.end(); ++citer)
	{
		listeners +=  (citer->second).size();
	}
	for(diter = _dataListener.begin(); diter != _dataListener.end(); ++diter)
	{
		listeners +=  (diter->second).size();
	}
	listeners += _stateListener.size();
	getListenerLock().unlock();

	return listeners;
}

bool ZkClient::isConnected() 
{
	return _currentState == ZkConnection::CONNECTED;
}

bool ZkClient::reInstallWatchChildren(const string &path)
{
	try {
		// if the node doesn't exist we should listen for the root node to reappear
	//	exists(path);
	//	list<string> children = getChildren(path);
		return true;
	} catch (ZkExceptionNoNode &e) {
		return false;
	}
}


