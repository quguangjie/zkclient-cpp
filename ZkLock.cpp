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

#include "ZkLock.h"




ZkLock::ZkLock()
{
	pthread_mutex_init(&_mutex, NULL);
	pthread_cond_init(&_statecond, NULL);
	pthread_cond_init(&_znodecond, NULL);
	pthread_cond_init(&_datacond, NULL);

}
ZkLock::~ZkLock()
{
	pthread_mutex_destroy(&_mutex);
	pthread_cond_destroy(&_statecond);
	pthread_cond_destroy(&_znodecond);
	pthread_cond_destroy(&_datacond);
}
bool ZkLock::lock()
{
	pthread_mutex_lock(&_mutex);
	return true;
}
bool ZkLock::unlock()
{
	pthread_mutex_unlock(&_mutex);
	return true;
}
bool ZkLock::signalAllStateChanged()
{
	int  ret;
	ret = pthread_cond_broadcast(&_statecond);
	if(ret !=  0)
		return false;
	return true;
}
bool ZkLock::siganlAllZNodeEvent()
{
	int  ret;
	ret = pthread_cond_broadcast(&_znodecond);
	if(ret !=  0)
		return false;
	return true;
}
bool ZkLock::siganlAllDataChanged()
{
	int  ret;
	ret = pthread_cond_broadcast(&_datacond);
	if(ret !=  0)
		return false;
	return true;
}

bool ZkLock::awaitUntilStateChanged(long sec)
{
	int 	ret;
	struct timespec timeout;

	timeout.tv_nsec = 0;
	timeout.tv_sec = time(NULL) + sec;
	ret = pthread_cond_timedwait(&_statecond, &_mutex, &timeout);
	if(ret !=  0)
		return false;
	return true;
}
bool ZkLock::awaitUntilZNodeEvent(long sec)
{
	int 	ret;
	struct timespec timeout;

	timeout.tv_nsec = 0;
	timeout.tv_sec = time(NULL) + sec;
	ret = pthread_cond_timedwait(&_znodecond, &_mutex, &timeout);
	if(ret !=  0)
		return false;
	return true;
}
bool ZkLock::awaitUntilDataChanged(long sec)
{
	int 	ret;
	struct timespec timeout;

	timeout.tv_nsec = 0;
	timeout.tv_sec = time(NULL) + sec;
	ret = pthread_cond_timedwait(&_datacond, &_mutex, &timeout);
	if(ret !=  0)
		return false;
	return true;
}






