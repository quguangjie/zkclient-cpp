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

ZkBase::ZkBase(const string &name, const string &zkhosts):ZkNode(name)
{
	_zkhosts = zkhosts; 
}
ZkBase::ZkBase(const string &name, const string &addr, const string &zkhosts):ZkNode(name, addr)
{ 
	_zkhosts = zkhosts; 
}
ZkBase::~ZkBase()
{
}
bool ZkBase::init()
{
	try {
		if(_zkclient == NULL){
			_zkclient = shared_ptr<ZkClient>(new ZkClient(_zkhosts, ZKBASE_SESSIONTIMEOUT, ZKBASE_CONNECTIONTIMEOUT));
			if(_zkclient==NULL)
			{
				cout<< "_zkclient(new ZkClient( err"<<endl;
				return false;
			}
			usleep(800000);
			_zkclient->createPersistent(_serRegPath, true);
			_zkclient->createPersistent(_serDLockPath, true);
			_zkclient->createPersistent(_serLeaderPath, true);
			_zkclient->createPersistent(_serSessionPath, true);
		}
		return true;
	} catch (ZkExceptionConnectionLoss e) {
		throw e;
	} catch (exception e) {
		throw e;
	}
	return false;
}

string      ZkBase::_serRegPath("/register");
string      ZkBase::_serDLockPath("/lock");
string      ZkBase::_serLeaderPath("/leader");
string      ZkBase::_serSessionPath("/session");




