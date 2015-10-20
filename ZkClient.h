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

#ifndef __ZKCLIENT_IZKCLIENT_H
#define __ZKCLIENT_IZKCLIENT_H

#include <list>
#include <map>
#include <unistd.h>
#include "zookeeper.h"
#include "ZkConnection.h"
#include "boost/shared_ptr.hpp"

#include  "ZkException.h"
#include  "ZkWatcher.h"


using namespace std;
using boost::shared_ptr;

class ZkLock;

class IZkStateListener 
{
public:  
	IZkStateListener(){
		int 	i1, i2;
		int 	len = sizeof(_randStr);

		for(i1=0; i1<len-1; ++i1) 
		{
			i2 = rand()%26 + 65 + rand()%2*32;  
			_randStr[i1]=(char)i2;
		}
	}

		virtual ~IZkStateListener(){};
    /**
     * Called when the zookeeper connection state has changed.
     *
     * @param state The new state.
     * @throws Exception On any error.
     */
      virtual void handleStateChanged(int state){};

    /**
     * Called after the zookeeper session has expired and a new session has been created. You would have to re-create
     * any ephemeral nodes here.
     *
     * @throws Exception On any error.
     */
	 virtual void handleNewSession(){};
private:
	 char 	_randStr[32];
};


/**
 * An {@link IZkDataListener} can be registered at a {@link ZkClient} for listening on zk data changes for a given path.
 * <p/>
 * Node: Also this listener re-subscribes it watch for the path on each zk event (zk watches are one-timers) is is not
 * guaranteed that events on the path are missing (see http://zookeeper.apache.org/doc/current/zookeeperProgrammers.html#ch_zkWatches). An
 * implementation of this class should take that into account.
 */
class IZkDataListener {
public:
	IZkDataListener(const string &path) 
	{ 
		int 	i1, i2;
		int 	len = sizeof(_randStr);
		reInstallWatchChildren = false;
		_dataPath = path; 

		for(i1=0; i1<len-1; ++i1) 
		{
			i2 = rand()%26 + 65 + rand()%2*32;  
			_randStr[i1]=(char)i2;
		}
	}
	virtual ~IZkDataListener(){};  
    virtual void handleDataChange(const string &dataPath, const string &data)
		{ };

	virtual void handleDataCreate(const string &dataPath)
		{};
	virtual void handleDataDeleted(const string &dataPath){};

	string &dataPath(){ return _dataPath;}
	bool reInstallWatchChildren;
private:
	string  _dataPath; 
	 char 	_randStr[32];
};



/**
 * An {@link IZkChildListener} can be registered at a {@link ZkClient} for listening on zk child changes for a given
 * path.
 * <p/>
 * Node: Also this listener re-subscribes it watch for the path on each zk event (zk watches are one-timers) is is not
 * guaranteed that events on the path are missing (see http://zookeeper.apache.org/doc/current/zookeeperProgrammers.html#ch_zkWatches). An
 * implementation of this class should take that into account.
 */
class IZkChildListener {
public:  
	IZkChildListener(const string &parentPath)
	{
		int 	i1, i2;
		int 	len = sizeof(_randStr);
		_path = parentPath;

		for(i1=0; i1<len-1; ++i1) 
		{
			i2 = rand()%26 + 65 + rand()%2*32;  
			_randStr[i1]=(char)i2;
		}
	}
	virtual ~IZkChildListener(){};
    /**
     * Called when the children of the given path changed.
     * <p>
     * NOTICE: if subscribing a node(not exists), the event will be triggered while the node(parent) were created.
     * </p>
     *
     * @param parentPath      The parent path
     * @param currentChildren The children or null if the root node (parent path) was deleted.
     * @throws Exception
     */
    virtual void handleChildChange(const string &parentPath, list<string> &currentChildren){};
	virtual void handleParentChange(const string &parentPath){};
	string &parentPath(){return _path;};
private:
	string _path;
	 char 	_randStr[32];
};


/**
 * zookeeper client wrapper
 * @since 2.0
 */
class ZkClient: public ZkWatcher
{
public:
#define   ZKCLIENT_WAITTIME_USEC   (1000) 

	ZkClient(const string &connectstring, long sessionTimeout, long connectionTimeout);
	~ZkClient();

	
    /**
     * Close the client.
     *
     * @throws ZkInterruptedException if interrupted while closing
     */
     void close();

	

    /**
     * Connect to ZooKeeper.
     *���ӵ�ZooKeeper������
     * @param timeout max waiting time(ms) until connected
     *����timeout ���ӳ�ʱʱ��(����)
     * @param watcher default watcher
     * @throws ZkInterruptedException if the connection timed out due to thread interruption
     * @throws ZkTimeoutException     if the connection timed out
     * @throws IllegalStateException  if the connection timed out due to thread interruption
     */
    void connect(long timeout, ZkWatcher *watcher);

	

    /**
     * Counts number of children for the given path.
     *
     * @param path zk path
     * @return number of children or 0 if path does not exist.
     */
    int countChildren(const string &path);

	

    /**
     * Create a node.
     *����һ���ڵ�path Ϊ�ڵ�·��data Ϊ�ڵ����mode Ϊ�ڵ�����
     * @param path zk path
     * @param data node data
     * @param mode create mode {@link CreateMode}
     * @return created path
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     */
     string create(const string &path, const char data[], int mode);

	

    /**
     * Create an ephemeral node with empty data
     *����һ��û�нڵ���ݵ���ʱ�ڵ�
     * @param path zk path
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     */
     void createEphemeral(const string &path);

	

    /**
     * Create an ephemeral node.
     *����һ����ʱ�ڵ�
     * @param path the path for the node
     * @param data node data
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     */
     void createEphemeral(const string &path, const char data[]);


	

    /**
     * Create an ephemeral, sequential node.
     *����һ����ʱ˳��ڵ㣬���Ѵ����ɹ���Ľڵ���(·��)����
     * @param path the path for the node
     * @param data the data for the node
     * @return created path
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     */
     string createEphemeralSequential(const string &path, const char data[]);

	

    /**
     * Create a persistent node with empty data (null)
     *����һ������ݵ� ���ýڵ�
     * @param path the path for the node
     * @throws ZkNodeExistsException    if the node exists
     * @throws ZkNoNodeException        if the parent node not exists
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     * @throws RuntimeException         any other exception
     * @see #createPersistent(string, bool)
     */
     void createPersistent(const string &path);


	

    /**
     * Create a persistent node with empty data (null)
     * <p>������������ýڵ㣬���createParents ΪTRUE ���𼶴���
     * If the createParents is true, neither {@link ZkNodeExistsException} nor {@link com.github.zkclient.exception.ZkNoNodeException} were throwed.
     * </p>
     *
     * @param path          the path for the node
     * @param createParents if true all parent dirs are created as well and no
     *                      {@link ZkNodeExistsException} is thrown in case the path already exists
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     * @throws RuntimeException         any other exception
     */
     void createPersistent(const string &path, bool createParents);


	

    /**
     * Create a persistent node.
     *����һ�������data�����ýڵ�
     * @param path the path for the node
     * @param data node data
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     * @throws RuntimeException         any other exception
     */
     void createPersistent(const string &path, const char data[]);


	

    /**
     * Create a persistent, sequental node.
     *����һ������˳��ڵ㣬�ڵ���ͨ���ֵ����
     * @param path the path for the node
     * @param data node data
     * @return create node's path
     * @throws ZkInterruptedException   if operation was interrupted, or a required reconnection
     *                                  got interrupted
     * @throws IllegalArgumentException if called from anything except the ZooKeeper event
     *                                  thread
     * @throws ZkException              if any ZooKeeper exception occurred
     * @throws RuntimeException         any other exception
     */
     string createPersistentSequential(const string &path, const char data[]);

	

    /**
     * delete a node
     *�ݹ�ɾ��ڵ㣬��ɾ��·���µ����нڵ�
     * @param path the path for the node
     * @return true if deleted; otherwise false
     */
     bool deleteRecursive(const string &path);

	

    /**
     * check the node exists
     *���ڵ��Ƿ���ڣ��Ƿ����ü��ӣ����ڷ����棬�������׳��쳣
     * @param path the path for the node
     * @return true if the node exists
     */
     bool exists(const string &path);
	 bool exists(const string &path,  bool watch);


	 

    /**
     * get the children for the node
     *��ȡpath�µ��ӽڵ㣬�������ӽڵ����֣��������Ƿ����ӽڵ�ı仯
     * @param path the path for the node
     * @return the children node names or null (then node not exists)
     */
     list<string> getChildren(const string &path);
	 list<string> getChildren(const string &path, bool watch);

	 

    /**
     * get the node creation time (unix milliseconds)
     *
     * @param path the path for the node
     * @return the unix milliseconds or -1 if node not exists
     */
     long getCreationTime(const string &path);

    /**
     * all watcher number in this connection
     *
     * @return watcher number
     */
     int numberOfListeners();

    /**
     * read the data for the node
     *
     * @param path the path for the node
     * @return the data for the node
     * @throws {@link com.github.zkclient.exception.ZkNoNodeException} if the node not exists
     * @see {@link #readData(string, bool)}
     */
     string readData(const string &path);


    /**
     * read the data and stat for the node
     *
     * @param path the path for the node
     * @param stat the stat for the node
     * @return the data for the node
     * @see {@link #readData(string, bool)}
     */
     string readData(const string &path, struct Stat *stat);
	 string readData(const string &path, struct Stat *stat,  bool watch);

    /**
     * subscribe the changing for children
     *
     * @param path     the path for the node
     * @param listener the listener
     * @return the children list or null if the node not exists
     * @see {@link IZkChildListener}
     */
     list<string> subscribeChildChanges(const string &path, shared_ptr<IZkChildListener> listener);

    /**
     * subscribe the data changing for the node
     *
     * @param path     the path for the node
     * @param listener the data changing listener
     * @see {@link IZkDataListener}
     */
     void subscribeDataChanges(const string &path, shared_ptr<IZkDataListener> listener);

    /**
     * subscribe the connection state
     *
     * @param listener the connection listener
     * @see {@link IZkStateListener}
     */
     void subscribeStateChanges(shared_ptr<IZkStateListener> listener);

    /**
     * unsubscribe all listeners for all path and connection state
     */
     void unsubscribeAll();

    /**
     * unsubscribe the child listener
     *
     * @param path          the path for the node
     * @param childListener the listener
     */
     void unsubscribeChildChanges(const string &path, shared_ptr<IZkChildListener> childListener);

    /**
     * unsubscribe the data changing for the node
     *
     * @param path         the path for the node
     * @param dataListener the data changing listener
     */
     void unsubscribeDataChanges(const string &path, shared_ptr<IZkDataListener> dataListener);

    /**
     * unsubscribe the connection state
     *
     * @param stateListener the connection listener
     */
     void unsubscribeStateChanges(shared_ptr<IZkStateListener> stateListener);


    /**
     * wait some time for the state
     *
     * @param keeperState the state
     * @param time        some time
     * @param timeUnit    the time unit
     * @return true if the connection state is the <code>keeperState</code> before the end time
     */
     bool waitForKeeperState(ZkConnection::KeeperState keeperState, long time, long timeUnit);

    /**
     * wait for the connected state.
     * <pre>
     *     waitForKeeperState(KeeperState.SyncConnected, Integer.MAX_VALUE, TimeUnit.MILLISECONDS);
     * </pre>
     *
     * @return true if the client connects the server
     * @throws ZkInterruptedException
     * @see {@link #waitForKeeperState(org.apache.zookeeper.Watcher.Event.KeeperState, long, java.util.concurrent.TimeUnit)}
     */
     bool waitUntilConnected();

    /**
     * wait for the connected state
     *
     * @param time     soem time
     * @param timeUnit the time unit
     * @return true if the client connects the server before the end time
     */
     bool waitUntilConnected(long time);

    /**
     * wait some unit until the node exists
     *
     * @param path     the path for the node
     * @param timeUnit the time unit
     * @param time     some time
     * @return true if the node exists
     */
     bool waitUntilExists(const string &path, long timeUnit, long time);

    /**
     * write the data for the node
     *
     * @param path the path for the node
     * @param data the data for the node
     * @return the stat for the node
     */
     struct Stat writeData(const string &path, const char  data[]);
     struct Stat writeData(const string &path, const string &data);

    /**
     * write the data for the node
     *
     * @param path            the path for the node
     * @param data            the data for the node
     * @param expectedVersion the version for the node
     * @return the stat for the node
     * @see {@link #cas(string, com.github.zkclient.IZkClient.DataUpdater)}
     */
     struct Stat writeData(const string &path, const char data[], int expectedVersion);

    /**
     * get the inner zookeeper client
     *
     * @return the inner zookeeper client
     */
     zooKeeper getZooKeeper();

    /**
     * check the connecting state of zookeeper client
     * @return true if connected
     */
     bool isConnected();
	 void process(ZkWatchedEvent &event);

public:
	#define DEFAULT_CONNECTION_TIMEOUT = 10000;
	#define  DEFAULT_SESSION_TIMEOUT = 30000;
protected :
	ZkConnection  *_connection;

private:
	void fireAllEvents();
	void fireNewSessionEvents();
	void fireStateChangedEvent(int state);
	void fireChildChangedEvents(const string &path, list<shared_ptr<IZkChildListener> > &childListeners);
	void fireDataChangedEvents(const string &path, list<shared_ptr<IZkDataListener> > &listeners);
	void statehandleNewSession() throw (ZkException);	
	list<string> watchForChilds(const string &path);
	void         watchForData(const string &path);
	bool getShutdownTrigger();
	ZkLock &getListenerLock();
	ZkLock &getEventLock();
	void processStateChanged(ZkWatchedEvent &event);
	void processDataOrChildChange(ZkWatchedEvent &event);
	bool hasListeners(const string &path);
	void setCurrentState(ZkConnection::KeeperState currentState);
	void reconnect();
    bool deletePath(const string &path);
	bool waitForKeeperState(ZkConnection::KeeperState keeperState, long timeout);
	void setShutdownTrigger(bool triggerState);
	bool reInstallWatchChildren(const string &path);

private:
	list<shared_ptr<IZkStateListener> > _stateListener;
	map<string, list<shared_ptr<IZkChildListener> > > _childListener; 
	map<string, list<shared_ptr<IZkDataListener > > > _dataListener;
	ZkLock           _zkEventLock;
	ZkLock           _zkListenerLock;
	bool _shutdownTriggered;
	ZkConnection::KeeperState 	_currentState;
};


#endif 


