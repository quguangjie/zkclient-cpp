#ifndef __ZKCLENT_ZKEVENTTHREAD  
#define __ZKCLENT_ZKEVENTTHREAD   

#include <string>
using namespace  std;

template<typename T> class ZkEventThread;  


template<typename T>  
class ZkThread_Event
{  
	friend class ZkEventThread<T> ; //注意此处友元类的声明形式  
	ZkThread_Event(const T& i):_event(i), _next(0) {  
	}  
	T _event;  
	ZkThread_Event *_next;  
};  

template<typename T>  
class ZkEventThread
{

public:
    ZkEventThread(); 
    ZkEventThread(string &n); 
	virtual ~ZkEventThread();


public:  
	void send(const T &i);  
	T    recv();   
	bool empty();
	long size();
	void clear();  
	virtual void process(T &i) = 0;

private:
	pthread_mutex_t _mutex;
	pthread_cond_t  _cond;

private:  
	ZkThread_Event<T> *_head;  
	ZkThread_Event<T> *_tail;  
	long               _n;  
};  

#include "ZkEventThread.hpp"

#endif 


