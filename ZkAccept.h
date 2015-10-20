#ifndef __ZKCLIENT_ZKACCEPT  
#define __ZKCLIENT_ZKACCEPT  

#include "ZkLeader.h"

class ZkBase;
class ZkNode;
class ZkLeader;

class ZkAccept: virtual public ZkLeader  
{
public:  
	ZkAccept(const string &name, const string &addr, const string &serstring, const long ver);   
	ZkAccept(const string &name, const string &addr, const string &serstring, const string &leaderName, const long ver);   

	virtual ~ZkAccept();

	bool serRegister();  
	void leader(list<ZkNode> &follow);
private: 
	string _acPath;
	string _acNodeName;
};


#endif 


