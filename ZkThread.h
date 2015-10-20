#ifndef __ZKCLIENT_ZKTHREAD
#define __ZKCLIENT_ZKTHREAD


#include <string>
#include <vector>
using namespace  std; 

class ZkThread
{
public:
	ZkThread();
	ZkThread(int  n);
	virtual ~ZkThread(){};  

	bool  start();
	void  shutdown();
	long  getId();
	bool  isShutdown();
	void  setName(string &name);

	static void* threadRun(void *p);

	virtual void run() = 0;

private:
	int 				_thNum;
	vector<pthread_t>   _ptids;
	string         _name;
	bool           _shutdown;
};


#endif 


