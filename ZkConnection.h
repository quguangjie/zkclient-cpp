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

#ifndef  __ZKCLIENT_ZKCONNECTION
#define  __ZKCLIENT_ZKCONNECTION
#include <string.h>
#include <iostream>
#include <list>
#include "zookeeper.h"
#include "ZkException.h"
#include "ZkLock.h"
#include <time.h>
using namespace std;

#define DATABUFFSIZE 1024
typedef   zhandle_t*   zooKeeper; 
/* ***********
 *
 ZOO_EXPIRED_SESSION_STATE = -112;
 ZOO_AUTH_FAILED_STATE = -113
 ZOO_CONNECTING_STATE = 1;
 ZOO_ASSOCIATING_STATE = 2;
 ZOO_CONNECTED_STATE = 3;


 *
 * */
class ZkWatcher;

class ZkConnection 
{ 

	friend void watch_init(zhandle_t *zh, int type, int state, const char *path, void* context);
	friend void watch_exist(zhandle_t *zh, int type, int state, const char *path, void* context);
	friend void watch_getchild(zhandle_t *zh, int type, int state, const char *path, void* context);
	friend void watch_readData(zhandle_t *zh, int type, int state, const char *path, void* context);
		
public:  
	enum KeeperState
	{
		NOTCONNECTED=0,
		EXPIRED_SESSION,
		AUTH_FAILED,
		CONNECTING,
		ASSOCIATING,
		CONNECTED
	};
	/**
     * build a zookeeper connection
     * @param zkServers      zookeeper connection string
     * @param sessionTimeOut session timeout in milliseconds
     */
    ZkConnection(const string &zkServers, int sessionTimeOut);
	~ZkConnection();

    void connect(ZkWatcher *watcher);  

    void close() ; 

    string create(const string &path, const char data[], int mode) ;
    void deleteNode(const string &path) ;
    bool exists(const string &path, bool  watch) ; 

    list<string> getChildren(const string &path,  bool watch) ;
    string readData(const string &path, struct Stat* stat, bool watch);

    struct Stat writeData(const string &path,const string &data, int version) ;
    KeeperState getZookeeperState();

    time_t getCreateTime(const string& path) ;
    string getServers();
    zooKeeper getZooKeeper();

private:
	__inline void checkError(int err, string message)
	{
	if(err != ZNODEEXISTS)
		cout<< err<<"~~~~~~~~~~~"<<endl;
	switch (err) {
		case ZDATAINCONSISTENCY:
			throw ZkExceptionDataInconsistency(err, message);
		case ZCONNECTIONLOSS:
			throw ZkExceptionConnectionLoss(err, message);
		case ZNONODE:
			throw  ZkExceptionNoNode(err, message);
//		case NOAUTH:
//			throw ZkNoAuthException();
		case ZBADVERSION:
			throw  ZkExceptionBadVersion(err, message);
//		case NOCHILDRENFOREPHEMERALS:
//			throw ZkExceptionNoChildrenForEphemerals(err, message);
		case ZNODEEXISTS:
			throw  ZkExceptionNodeExists(err, message);
		case ZINVALIDACL:
			throw ZkExceptionInvalidACL(err, message);
		case ZAUTHFAILED:
			throw ZkExceptionAuthFailed(err, message);
//		case ZNOTEMPTY:
//			throw ZkExceptionNotEmpty(err, message);
		case ZSESSIONEXPIRED:
			throw ZkExceptionSessionExpired(err, message);
//		case ZINVALIDCALLBACK:
//			throw ZkExceptionInvalidCallback(err, message);
		case ZINVALIDSTATE:  //   Invliad zhandle state
			break;
		default:
				throw  ZkException(err, message);
		}
	}
   
private:
	zhandle_t* _zk ;
	ZkLock _zookeeperLock;
	ZkWatcher * _watcher;
	string _servers;
	int _sessionTimeOut;
	int _zkstat;
	char _bufferData[DATABUFFSIZE];
};



#endif 


