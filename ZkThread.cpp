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

#include "ZkThread.h"


ZkThread::ZkThread():_thNum(1), _shutdown(true)
{
	_ptids.resize(1);
}
ZkThread::ZkThread(int n):_thNum(n), _shutdown(true)
{
	_ptids.resize(n);
}
bool  ZkThread::start()
{
	int ret;

	_shutdown = false;
	ret = pthread_create(&_ptids[0], NULL, threadRun, (void *)this);
	if(ret != 0)
	{
		_shutdown = true;
		return false;
	}
	return true;
}

void  ZkThread::shutdown()
{
	pthread_cancel(_ptids[0]);
	_shutdown = true;
}
long  ZkThread::getId()
{
	return 0;
}
bool  ZkThread::isShutdown()
{
	return _shutdown;
}
void  ZkThread::setName(string &name)
{
}

void* ZkThread::threadRun(void *arg)    
{
	pthread_detach(pthread_self());
	ZkThread   *p = (ZkThread *)arg;  
	p->run();   
	return NULL;  
}



