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

#ifndef __ZKCLIENT_ZKLOCK
#define __ZKCLIENT_ZKLOCK
#include <pthread.h>

class ZkLock
{
public:
	ZkLock();
	~ZkLock();
	bool lock();   
	bool unlock();   
	bool signalAllStateChanged();
	bool siganlAllZNodeEvent();
	bool siganlAllDataChanged();

	bool awaitUntilStateChanged(long sec);
	bool awaitUntilZNodeEvent(long sec);
	bool awaitUntilDataChanged(long sec);
private:
	pthread_mutex_t   	_mutex;
	pthread_cond_t  	_statecond;
	pthread_cond_t  	_znodecond;
	pthread_cond_t  	_datacond;
};


#endif 


