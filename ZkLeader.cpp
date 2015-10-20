#include  "ZkBase.h"
#include  "ZkLeader.h"
#include  "ZkClient.h"

ZkLeader::ZkLeader(const string &name, const string &addr, const string &serstring):ZkBase(name, addr, serstring)
{
	_LdPath = getLeaderPath()+ name;
	_LdNodeName =addr;
	_LdcreateFlag = false;
	init();
}
ZkLeader::ZkLeader(const string &name, const string &addr, const string &serstring, const string &leadername):ZkBase(name, addr, serstring)
{
	_LdPath = getLeaderPath()+ name;
	_LdNodeName =leadername;
	_LdcreateFlag = false;
	init();
	string fullpath = _LdPath + "/" +_LdNodeName;
	getClientPtr()->createPersistent( _LdPath, true);
	
	getClientPtr()->createEphemeral(fullpath);
	_LdcreateFlag = true;
}
ZkLeader::~ZkLeader()
{
	if(_LdcreateFlag)
	{
		string fullpath = _LdPath + "/" +_LdNodeName;
		getClientPtr()->deleteRecursive(fullpath);
	}
}

bool ZkLeader::isLeader()
{
	list<string> result ;
	try{
		result = getClientPtr()->getChildren(_LdPath, false);
		result.sort();

		list<string>::iterator it =result.begin();
		if ((*it) == _LdNodeName)
		{
			return true;
		}
		return false;
	}
	catch (...)
	{
		return false;
	}
}
list<ZkNode>&  ZkLeader::follower()
{
	list<string> result;
	result = getClientPtr()->getChildren(_LdPath, false);
	result.sort();
	
	list<string>::iterator it =result.begin();
	if (*(it)  != _LdNodeName)
	{
		_follower.clear();
		return _follower;
	}

	result.erase(it);
	if (result.empty())
	{
		_follower.clear();
	}
	for (it =result.begin();it!=result.end();++it)
	{
		ZkNode node(_LdPath,(*it));
		_follower.push_back(node);
	}
	return _follower;
}

void ZkLeader::leader(list<ZkNode> &follow)
{

}




