#include "ZkEventThread.h"   
#include "ZkException.h"
#include "ZkThread.h"

using namespace std;

template<typename T>  
ZkEventThread<T>::ZkEventThread() :  _head(NULL), _tail(NULL), _n(0)  
{
	pthread_mutex_init(&_mutex, NULL);
	pthread_cond_init(&_cond, NULL);
}

template<typename T>  
ZkEventThread<T>::ZkEventThread(string &name): _head(NULL), _tail(NULL), _n(0)  
{
	pthread_mutex_init(&_mutex, NULL);
	pthread_cond_init(&_cond, NULL);
}
template<typename T>  
ZkEventThread<T>::~ZkEventThread()  
{
	pthread_mutex_destroy(&_mutex);
	pthread_cond_destroy(&_cond);
}



template<typename T>  
void ZkEventThread<T>::send(const T &i)
{
	{
		pthread_mutex_lock(&_mutex);
		ZkThread_Event<T> *item = new ZkThread_Event<T> (i);   

		if (_n == 0) {  
			_head = _tail = item;  
		} else {  
			_tail->_next = item;  
			_tail = item;  
		}  
		++_n;  
		pthread_mutex_unlock(&_mutex);
		pthread_cond_signal(&_cond);
	}
}

template<typename T>  
T    ZkEventThread<T>::recv()
{
	T t;

	struct timespec timeout;

	pthread_mutex_lock(&_mutex);
	while(_n <= 0)
	{
		timeout.tv_nsec = 0;
		timeout.tv_sec = time(NULL) + 30;
		pthread_cond_timedwait(&_cond, &_mutex, &timeout);
	}
	if (_n > 0) 
	{  
		ZkThread_Event<T> *temp = _head;  
		_head = _head->_next;  
		t = temp->_event;
		delete temp;  
		--_n;  
	} 
	pthread_mutex_unlock(&_mutex);
	return t;  
}

template<typename T>  
bool ZkEventThread<T>::empty()
{
	if (_n > 0)  
		return false;  
	else  
		return true;  
}
template<typename T>  
long ZkEventThread<T>::size()
{
	return _n;
}

template<typename T>  
void ZkEventThread<T>::clear()
{
	pthread_mutex_lock(&_mutex);
	while (_n > 0) 
	{  
		ZkThread_Event<T> *temp = _head;  
		_head = _head->next;  
		delete temp;  
		--_n;  
	} 
	pthread_mutex_unlock(&_mutex);
}



