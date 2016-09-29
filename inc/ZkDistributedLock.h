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

#ifndef __ZKCLIENT_ZKDISTRIBUTEDLOCK
#define __ZKCLIENT_ZKDISTRIBUTEDLOCK

#include <list>
#include <string>
#include <iostream>
#include <pthread.h>
#include <boost/enable_shared_from_this.hpp> 
#include "ZkBase.h"

using namespace std;

class watchDLockNode;
class ZkDistributedLock:public ZkBase, public boost::enable_shared_from_this<ZkDistributedLock>
{
	public:
		ZkDistributedLock(const string &name, const string &serstring);
		~ZkDistributedLock(){}
		bool IsNeedCreate(); 
		bool Dlock();
		bool unDlock();
		bool waitDlock(long timeout);
	private:
		friend class watchDLockNode;

		string findMinLock();
		string _dLockPath;			// eg. "/lock"
		string _dLockName;		// eg. " wlock"
		string _dLockFullPath;		//eg. "/lock/wlock"
		string _dLockNodeName;	//eg. "/lock/wlock/00001"

		int  DLockStats;
		pthread_mutex_t DLockmutex;
		pthread_cond_t DLockcond;

		list<string> _dLockList;
		int _dLockNoCreate;
};

#endif
