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



