#include  <pthread.h>    
#include  <exception>    
#include  "ZkClient.h"
#include  "ZkBase.h"


ZkNode::ZkNode(const string &name)
{
	_name = name;  
}
ZkNode::ZkNode(const string &name, const string &addr)
{
	_name = name;
	_addr = addr;
}
ZkNode::~ZkNode()
{
}
string &ZkNode::getId()
{
	return _id; 
}
string &ZkNode::getAddr()
{
	return _addr; 
}
string &ZkNode::getName()
{
	return _name; 
}

ZkBase::ZkBase(const string &name, const string &serstring):ZkNode(name)
{
	_serstring = serstring; 
}
ZkBase::ZkBase(const string &name, const string &addr, const string &serstring):ZkNode(name, addr)
{ 
	_serstring = serstring; 
}
ZkBase::~ZkBase()
{
}
bool ZkBase::init()
{
	try
	{
		pthread_mutex_lock(&_mutex);
		if(_zkc == NULL){
			_zkc = shared_ptr<ZkClient>(new ZkClient(_serstring, ZKBASE_SESSIONTIMEOUT, ZKBASE_CONNECTIONTIMEOUT));
			if(_zkc==NULL)
			{
				cout<< "_zkc(new ZkClient( err"<<endl;
				return false;
			}
			usleep(800000);
			_zkc->createPersistent(_serRegPath, true);
			_zkc->createPersistent(_serDLockPath, true);
			_zkc->createPersistent(_serLeaderPath, true);
			_zkc->createPersistent(_serSessionPath, true);
		}
		pthread_mutex_unlock(&_mutex);
		return true;
	}
	catch (ZkExceptionConnectionLoss e)
	{
		pthread_mutex_unlock(&_mutex);
		throw e;
	}
	catch (exception e)
	{
		pthread_mutex_unlock(&_mutex);
		throw e;
	}
	return false;
}
bool ZkBase::setServer(string &ser)
{
	return false;
}

pthread_mutex_t      ZkBase::_mutex = PTHREAD_MUTEX_INITIALIZER;
string      ZkBase::_serRegPath("/register");
string      ZkBase::_serDLockPath("/lock");
string      ZkBase::_serLeaderPath("/leader");
string      ZkBase::_serSessionPath("/session");
shared_ptr<ZkClient>      ZkBase::_zkc;




