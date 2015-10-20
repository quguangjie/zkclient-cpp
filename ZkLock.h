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


