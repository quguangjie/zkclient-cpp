#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>  

#include "ZkAccept.h"
#include "ZkRequest.h"
#include "ZkDistributedLock.h"


struct testLock
{  
public:
	testLock(int id) : id(id) {}  

	void operator()()  
	{  
		string  serstring = "127.0.0.1:2181";
		string sid("hospitalVisitor");
		sid.append(boost::lexical_cast<string>(10101020));

		std::cerr << "inter id:" << id << std::endl;
		shared_ptr<ZkDistributedLock> l(new ZkDistributedLock(sid, serstring));
		l->Dlock();
		std::cout << "lock id:" << id << std::endl;
		sleep(3);
		l->unDlock();
		std::cout << "unlock id:" << id << std::endl;
	}  

	int id;  
};  	

void hello1()  
{  
	string  serstring = "127.0.0.1:2181";
	string sid("hospitalVisitor");
	sid.append(boost::lexical_cast<string>(10101032));

	std::cerr << "inter "  << std::endl;
	shared_ptr<ZkDistributedLock> l(new ZkDistributedLock(sid, serstring));
	l->Dlock();
	std::cout << "lock id:" << std::endl;
	usleep(100000);
	l->unDlock();
	std::cout << "unlock id:" << std::endl;
}  
void hello2()  
{  
	string  serstring = "127.0.0.1:2181";
	string sid("hospitalVisitor");
	sid.append(boost::lexical_cast<string>(10101033));

	std::cerr << "inter "  << std::endl;
	shared_ptr<ZkDistributedLock> l(new ZkDistributedLock(sid, serstring));
	l->Dlock();
	std::cout << "lock id:" << std::endl;
	usleep(100000);
	l->unDlock();
	std::cout << "unlock id:" << std::endl;
}  
void hello3()  
{  
	string  serstring = "127.0.0.1:2181";
	string sid("hospitalVisitor");
	sid.append(boost::lexical_cast<string>(10101035));

	std::cerr << "inter "  << std::endl;
	shared_ptr<ZkDistributedLock> l(new ZkDistributedLock(sid, serstring));
	l->Dlock();
	std::cout << "lock id:" << std::endl;
	usleep(100000);
	l->unDlock();
	std::cout << "unlock id:" << std::endl;
}  
void hello6()  
{  
	string  serstring = "127.0.0.1:2181";
	string sid("hospitalVisitor");
	sid.append(boost::lexical_cast<string>(10101036));

	std::cerr << "inter "  << std::endl;
	shared_ptr<ZkDistributedLock> l(new ZkDistributedLock(sid, serstring));
	l->Dlock();
	std::cout << "lock id:" << std::endl;
	usleep(100000);
	l->unDlock();
	std::cout << "unlock id:" << std::endl;
}  


int main()
{
#if 0
	string  selfname = "testname";
	string  addr = "111111111";
	string  serstring = "127.0.0.1:2181";


	ZkAccept   zkcli(selfname, addr, serstring, 1);
	zkcli.serRegister();


	string  	name = "npackserver";  
	string  	serstring = "127.0.0.1:2181";
	long        ver = 1;
	int         port;
	string      ip;

	shared_ptr<ZkRequest>   m_zkcli(new ZkRequest(name, serstring, ver));
	m_zkcli->discovery();
	if(!m_zkcli->getServer(ip, port))
	{
		return -1;
	}
	std::cerr << ip << " error  " << port << std::endl;
	return 0; 
#endif


#if 1 
	for(int i=0; i < 1000; ++i)
	{
		std::cout << "id:" << i << std::endl;
		if(i%4 == 0)
			boost::thread thrd(&hello1);  
		if(i%4 == 1)
			boost::thread thrd(&hello1);  
		if(i%4 == 2)
			boost::thread thrd(&hello1);  
		else
			boost::thread thrd(&hello2);  
	//	boost::thread thrd(testLock(i)); 
	}

#endif
	while(1) sleep(1);
}






