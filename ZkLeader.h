#ifndef __ZKCLIENT_ZKLEADER  
#define __ZKCLIENT_ZKLEADER  

#include<string>
#include <list>
#include "ZkBase.h"

using namespace std;

class ZkNode;

class ZkLeader:virtual public ZkBase
{
public:
	ZkLeader(const string &name, const string &addr, const string &serstring);   
	ZkLeader(const string &name, const string &addr, const string &serstring, const string &leadername);

	virtual ~ZkLeader();  

	bool isLeader();
	list<ZkNode> &follower();

	virtual   void leader(list<ZkNode> &follow); 

private:
	list<ZkNode>            _follower;
	string 			_LdPath;
	string 			_LdNodeName;
	bool 				_LdcreateFlag;
};


#endif 


