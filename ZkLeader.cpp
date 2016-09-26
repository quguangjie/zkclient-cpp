/*
 *  Author: GuangJie Qu <qgjie456@163.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, 
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include  "ZkBase.h"
#include  "ZkLeader.h"
#include  "ZkClient.h"
#include  <boost/make_shared.hpp>

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
	_zkclient->createPersistent( _LdPath, true);
	
	_zkclient->createEphemeral(fullpath);
	_LdcreateFlag = true;
}
ZkLeader::~ZkLeader()
{
	if(_LdcreateFlag)
	{
		string fullpath = _LdPath + "/" +_LdNodeName;
		_zkclient->deleteRecursive(fullpath);
	}
}

bool ZkLeader::isLeader()
{
	list<string> result ;
	try{
		result = _zkclient->getChildren(_LdPath, false);
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
const list<shared_ptr<ZkNode> >&  ZkLeader::follower()
{
	list<string> result;
	result = _zkclient->getChildren(_LdPath, false);
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
		shared_ptr<ZkNode> p = ::boost::make_shared<ZkNode>(_LdPath,(*it));
		_follower.push_back(p);
	}
	return _follower;
}

void ZkLeader::leader(list<shared_ptr<ZkNode> > &follow)
{

}




