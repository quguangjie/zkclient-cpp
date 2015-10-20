#ifndef __ZKCLIENT_ZKNODECONN
#define __ZKCLIENT_ZKNODECONN

#include <map>
#include <list>
using namespace  std; 

class ZkNode;
class ZkRequest;
class ZkAccept;
class ZkLeader;


template<typename T> class ZkNodeConn;  

template<typename T>
class ZkNodeConn: public ZkRequest, public ZkAccept  
{
public:
	ZkNodeConn(string &name, string &addr, string &serstring);   
	virtual ~ZkNodeConn();  

	virtual void resAlloc(list<ZkNode> &follower);      
	virtual void updateRouter(list<ZkNode> &follower);  

	void   setNodeResource(ZkNode &node, T &i);   
	void   getNodeResource(ZkNode &node, list<T> &l);    
	bool   syncResource(); 
private:  
	void leader(list<ZkNode> &follow);   

private:  
	map<string, list<T> > _resource;   
};

#endif 


