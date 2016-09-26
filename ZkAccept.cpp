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

#include   "ZkBase.h"
#include   "ZkLeader.h"
#include   "ZkAccept.h"
#include   "ZkClient.h"
#include <boost/lexical_cast.hpp>


ZkAccept::ZkAccept(const string &name, const string &addr, const string &serstring, const long ver):ZkBase(name, addr,serstring),ZkLeader(name, addr, serstring) 
{
	 _acPath =  getSerRegPath()  + "/" + name + "/v" + boost::lexical_cast<string>(ver);
	 cout << _acPath << endl;
	 _acNodeName = addr;
	string fullPath = _acPath + "/" + _acNodeName;
	setDataPath(fullPath);
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
	shared_ptr<ZkClient> zk = _zkclient;	
	_zkclient->createPersistent(_acPath,true);
	string fullPath = _acPath + "/" + _acNodeName;
	while(true)
	{
		try{
			try {
				_zkclient->deleteRecursive(fullPath); 
			} catch (...) {}
			_zkclient->createEphemeral(fullPath);
			_zkclient->subscribeDataChanges(fullPath, shared_from_this());
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

void ZkAccept::handleDataDeleted(const string &filepath)
{
	cout << "watchServerRoute::handleDataDeleted" << endl;
	try{
		_zkclient->createEphemeral(filepath);  
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

