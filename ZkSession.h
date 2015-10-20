#ifndef __ZKCLIENT_ZKSESSION  
#define __ZKCLIENT_ZKSESSION  

#include<string>
#include <list>
#include <boost/serialization/singleton.hpp>
#include "ZkBase.h"

using namespace std;
using boost::serialization::singleton;


class ZkSession: virtual public ZkBase, public singleton<ZkSession>
{
public:
	ZkSession(){};
	~ZkSession(){};
	init(string &serstring);   

private:

};


#endif 


