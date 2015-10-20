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

#include <iostream>
#include "ZkConnection.h"
#include "ZkWatcher.h"

/*  ***********
 *   *
 *    ZOO_EXPIRED_SESSION_STATE = -112;
 *    ZOO_AUTH_FAILED_STATE = -113
 *    ZOO_CONNECTING_STATE = 1;
 *    ZOO_ASSOCIATING_STATE = 2;
 *    ZOO_CONNECTED_STATE = 3;
 */
ZkWatchedEvent::EventType wtype2etype(int wtype)
{
	ZkWatchedEvent::EventType e = ZkWatchedEvent::NoNodeEvent;
	switch(wtype)
	{
		case 1:
			e = ZkWatchedEvent::NodeCreated;
				break;	
		case 2:
			e = ZkWatchedEvent::NodeDeleted;
			break;
		case 3:
			e = ZkWatchedEvent::NodeDataChanged;
			break;
		case 4:
			e = ZkWatchedEvent::NodeChildrenChanged;
			break;
		case -2:
			e = ZkWatchedEvent::NodeNoWaching;
			break;
		case -1:
			e = ZkWatchedEvent::StateChanged;
			break;;
		case -112:
			e = ZkWatchedEvent::StateChanged;
			break;;
		default:
			e = ZkWatchedEvent::NoNodeEvent;
			break;
	}	
	printf("wtype %d etype %d\n",wtype, e);
	return e;
} 

void watch_init(zhandle_t *zh, int type, int state, const char *path, void* context) 
{
	string p = path;
	ZkWatchedEvent event(p,wtype2etype(type));
	((ZkConnection*)context)->_watcher->send(event); 
	if (((ZkConnection*)context)->_zkstat != state)
	{
		ZkWatchedEvent event(p,ZkWatchedEvent::StateChanged);
		((ZkConnection*)context)->_watcher->send(event);
		((ZkConnection*)context)->_zkstat = state;
	}
}

void watch_exist(zhandle_t *zh, int type, int state, const char *path, void* context)
{

	string p = path;
	ZkWatchedEvent event(p,wtype2etype(type));  
	((ZkConnection*)context)->_watcher->send(event);  
	if (((ZkConnection*)context)->_zkstat != state)  
	{
		ZkWatchedEvent event(p,ZkWatchedEvent::StateChanged);  
		((ZkConnection*)context)->_watcher->send(event);
		((ZkConnection*)context)->_zkstat = state;
	}
	zoo_wexists(zh, path, watch_exist,context, NULL);
}

void watch_getchild(zhandle_t *zh, int type, int state, const char *path, void* context) 
{
	string p = path;
	ZkWatchedEvent event(p,wtype2etype(type));
	((ZkConnection*)context)->_watcher->send(event);
	if (((ZkConnection*)context)->_zkstat != state)
	{
		ZkWatchedEvent event(p,ZkWatchedEvent::StateChanged);
		((ZkConnection*)context)->_watcher->send(event);
		((ZkConnection*)context)->_zkstat = state;
	}
	zoo_wget_children(zh, path, watch_getchild,context, NULL);
}

void watch_readData(zhandle_t *zh, int type, int state, const char *path, void* context) 
{
	string p = path;
	ZkWatchedEvent event(p,wtype2etype(type));
	((ZkConnection*)context)->_watcher->send(event);
	if (((ZkConnection*)context)->_zkstat != state)
	{
		ZkWatchedEvent event(p,ZkWatchedEvent::StateChanged);
		((ZkConnection*)context)->_watcher->send(event);
		((ZkConnection*)context)->_zkstat = state;
	}
	zoo_wget(zh, path, watch_readData,context, 0,0,0);
}
/**
 * build a zookeeper connection
 * @param zkServers      zookeeper connection string
 * @param sessionTimeOut session timeout in milliseconds
 */
ZkConnection::	ZkConnection(const string &zkServers, int sessionTimeOut) 
{
	_zk = NULL;
	_servers = zkServers;
	_sessionTimeOut = sessionTimeOut;
	_zkstat = 999; //#define NOTCONNECTED_STATE_DEF 999
}
ZkConnection::~ZkConnection()
{
	_zk = NULL;
	_servers.clear();
	_sessionTimeOut = 0;
	_zkstat = 999;
}
void ZkConnection::connect(ZkWatcher *watcher) 
{
	_zookeeperLock.lock();
	if (_zk != NULL) 
	{
		return ;
	}

	_zk = zookeeper_init(_servers.c_str(), watch_init, _sessionTimeOut, 0, (void*)this, 0); 
	if(_zk == NULL)
	{
		string err = "zookeeper_init";
		throw  ZkExceptionConnectionLoss(0,err);
	}
	zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

	_watcher = watcher;
	_zookeeperLock.unlock();
}

void ZkConnection::close() 
{
	_zookeeperLock.lock();
	if (_zk != NULL) 
	{
		zookeeper_close(_zk);
		_zk = NULL;
	}
	_zookeeperLock.unlock();
}

string ZkConnection::create(const string &path, const char data[], int mode)
{
	string buf;
	char buffer[512] = {0};
	int 	rc = ZOK; 
#if 0
	if (_zk == NULL) 
		return ;
#endif 
	if(data == NULL)
		rc =zoo_create(_zk, path.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE, mode, buffer, sizeof(buffer)-1);
	else
		rc =zoo_create(_zk, path.c_str(), data, sizeof(data) -1, &ZOO_OPEN_ACL_UNSAFE, mode, buffer, sizeof(buffer)-1);
	if (rc!=ZOK)
	{	
		string err = "zoo_create"+ path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
		checkError(rc, err);
	}
	buf = buffer;
	return buf;
}

void ZkConnection::deleteNode(const string &path) 
{
	if (_zk == NULL) 
		return ;
	int rd = zoo_delete(_zk, path.c_str(), -1);   //-1 not check version
	if(rd!=ZOK)
	{
		string err = "zoo_delete" + path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
		checkError(rd, err);
	}
}

bool ZkConnection::exists(const string &path, bool  watch)  
{
	int ret = 0;
	if (_zk == NULL) 
		return false;
	if(watch)
	{
		ret = zoo_wexists(_zk, path.c_str(), watch_exist,(void*)this, NULL) ;
		if(ret!=ZOK)
		{
			string err = "zoo_wexists" + path;
			printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			checkError(ret, err);
		}
		return true;
	}
	ret = zoo_exists(_zk, path.c_str(), 0, NULL);
	if(ret!=ZOK)
	{
		string err = "zoo_exists" + path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
		checkError(ret, err);

	}
	return true;
}

list<string> ZkConnection::getChildren(const string &path, bool watch)
{
	int i = 0;
	int ret =0;
	list<string> path_tree;
	struct String_vector str_vec ;
	memset(&str_vec,0,sizeof(str_vec));
#if 0
	if (_zk == NULL) 
		return ;
#endif 
	if(watch)
	{
		ret = zoo_wget_children(_zk, path.c_str(), watch_getchild, (void*)this, &str_vec);
		if(ret!=ZOK)
		{
			string err = "zoo_wget_children" + path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			checkError(ret, err);

		}
	}
	ret = zoo_get_children(_zk, path.c_str(), 0, &str_vec);	
	if(ret!=ZOK)
	{
		string err = "zoo_get_children" + path;
		checkError(ret, err);

	}
	path_tree.clear();

	_zookeeperLock.lock();
	for (i=0;i<str_vec.count;i++)
	{
		path_tree.push_back(str_vec.data[i]);
	}
	_zookeeperLock.unlock();

	return path_tree;
}

string ZkConnection::readData(const string& path, struct Stat* stat, bool watch)
{
	string stringData;
	int ret;
	stringData.clear();
	int bufferDataLen = DATABUFFSIZE;

#if 0
	if (_zk == NULL) 
		return ;
#endif 
	if(watch)
	{
		ret = zoo_wget(_zk, path.c_str(), watch_readData,(void*)this, _bufferData, &bufferDataLen,stat);
		if(ret!=ZOK)
		{
			string err = "zoo_wget" + path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			checkError(ret, err);
		}
	}
	ret = zoo_get(_zk,path.c_str(),0,_bufferData,&bufferDataLen,stat);
	if(ret!=ZOK)
	{
		string err = "zoo_get" + path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
		checkError(ret, err);
	}
	if (bufferDataLen >0)
	{
		stringData = _bufferData;
		memset(_bufferData,0,DATABUFFSIZE);
	}
	return stringData;
}

struct Stat  ZkConnection::writeData(const string &path, const string &data, int version)  {
	struct Stat stat;
	int ret;
	memset(&stat,0,sizeof(stat));
#if 0
	if (_zk == NULL) 
		return ;
#endif 
	ret = zoo_set2(_zk, path.c_str(),data.c_str(), data.length(), version, &stat);
	if(ret!=ZOK)
	{
		string err = "zoo_set2" + path;
		printf("[%s.%d]\n", __FUNCTION__,__LINE__);
		checkError(ret, err);
	}
	return stat;
}

ZkConnection::KeeperState ZkConnection:: getZookeeperState() {
	// zoo_state(_zk);
	// is_unrecoverable(_zk);
	KeeperState s = NOTCONNECTED;
	if(_zk!= NULL)
	{
		switch(_zkstat)
		{
			case 1:
				s = CONNECTING;
				break;
			case 2:
				s = ASSOCIATING;
				break;
			case 3:
				s = CONNECTED;
				break;
			case -112:
				s = EXPIRED_SESSION;
				break;
			case -113:
				s = AUTH_FAILED;
				break;
			default:
				s = NOTCONNECTED;
				break;
		}

	}
	return s;
}

time_t ZkConnection::getCreateTime(const string &path)  {
	struct Stat stat;
	if (_zk == NULL) 
		return -1;
	zoo_exists(_zk, path.c_str(), 0, &stat);
	if (stat.ctime) {
		return stat.ctime/1000;
	}
	return -1;
}

string ZkConnection::getServers() {
	return _servers;
}

zooKeeper ZkConnection::getZooKeeper() {
	return (zooKeeper)_zk;
}
