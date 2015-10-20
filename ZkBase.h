#ifndef __ZKCLIENT_ZKBASE
#define __ZKCLIENT_ZKBASE

#include <string>
#include  <pthread.h>    
#include  "boost/shared_ptr.hpp"

class ZkClient;
using namespace  std;
using boost::shared_ptr;

class ZkNode
{
public:
	ZkNode(const string &name);
	ZkNode(const string &name, const string &addr);
	~ZkNode();

	string &getAddr();
	string &getName();
	string &getId();
private:
	string         _addr;
	string         _name;
	string         _id;
};

class ZkBase: public ZkNode
{
#define     ZKBASE_SESSIONTIMEOUT        (30000)   
#define     ZKBASE_CONNECTIONTIMEOUT      (30)  

public:
	ZkBase(const string &name, const string &serstring);
	ZkBase(const string &name, const string &addr, const string &serstring);
	~ZkBase();
	bool init();
	bool setServer(string &ser);

	shared_ptr<ZkClient>  getClientPtr(){return _zkc;}
	string getSerRegPath(){return _serRegPath;}
	string getDLockPath(){return _serDLockPath;}
	string getLeaderPath(){return _serLeaderPath;}

private:
	string 					_serstring; 
	static pthread_mutex_t  _mutex;
	static shared_ptr<ZkClient> _zkc; 
	static  string     _serRegPath; 
	static  string 	_serDLockPath;
	static  string 	_serLeaderPath;
	static  string 	_serSessionPath;
};


#endif 


