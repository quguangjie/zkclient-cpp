#include   "ZkBase.h"
#include   "ZkLeader.h"
#include   "ZkAccept.h"
#include   "ZkClient.h"
#include <boost/lexical_cast.hpp>



class watchServerRoute :public IZkDataListener
{
	shared_ptr<ZkClient> _zkcli;
	ZkAccept *_accpPtr;
	public:
	watchServerRoute(shared_ptr<ZkClient> cli, const string &filepath) : IZkDataListener(filepath)
	{
		_zkcli = cli;
	}
	~watchServerRoute(){}

	void handleDataDeleted(const string &filepath)
	{
		cout << "watchServerRoute::handleDataDeleted" << endl;
		try{
			_zkcli->createEphemeral(filepath);  
		}catch (ZkExceptionNodeExists &e){
			cout << "watchServerRoute::handleDataDeleted NodeExist "<< e.what() <<endl;
			return ;
		}catch (ZkExceptionNoNode &e){
			cout << "watchServerRoute::handleDataDeleted ZkException "<< e.what() <<endl;
			return; 
		}catch (ZkException &e){
			cout << "watchServerRoute::handleDataDeleted ZkException "<< e.what() <<endl;
			return ;
		}
		return;  
	}
};




ZkAccept::ZkAccept(const string &name, const string &addr, const string &serstring, const long ver):ZkBase(name, addr,serstring),ZkLeader(name, addr, serstring) 
{
	 _acPath =  getSerRegPath()  + "/" + name + "/v" + boost::lexical_cast<string>(ver);
	 cout << _acPath << endl;
	 _acNodeName = addr;

}
ZkAccept::ZkAccept(const string &name, const string &addr, const string &serstring, const string &leaderName, const long ver):ZkBase(name, addr,serstring),ZkLeader(name, addr, serstring,leaderName) 
{
	 _acPath =  getSerRegPath()  + "/" + name + "/v" + boost::lexical_cast<string>(ver);
	 _acNodeName = addr;
}
ZkAccept::~ZkAccept()
{
}

bool ZkAccept::serRegister() 
{
	shared_ptr<ZkClient> zk = getClientPtr();	
	getClientPtr()->createPersistent(_acPath,true);
	string fullPath = _acPath + "/" + _acNodeName;
	while(true)
	{
		try{
			try {
				getClientPtr()->deleteRecursive(fullPath); 
			} catch (...) {}
			getClientPtr()->createEphemeral(fullPath);
			shared_ptr<watchServerRoute> _reqMywc (new watchServerRoute(getClientPtr(), fullPath));  
			getClientPtr()->subscribeDataChanges(fullPath, _reqMywc);
			printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			break;
		}catch (ZkExceptionNodeExists &e){
			cout << "serRegister NodeExist "<< e.what() <<endl;
			break;
		}catch (ZkExceptionNoNode &e){
			cout << "serRegister ZkExceptionNoNode "<< e.what() <<endl;
		}catch (ZkException &e){
			cout << "serRegister ZkException "<< e.what() <<endl;
		}
	}
	return true;  
}
void ZkAccept::leader(list<ZkNode> &follow)
{
}


