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
	ZkBase(const string &name, const string &zkhosts);
	ZkBase(const string &name, const string &addr, const string &zkhosts);
	~ZkBase();
	bool init();

	const string& 			getSerRegPath(){return _serRegPath;}
	const string& 			getDLockPath(){return _serDLockPath;}
	const string& 			getLeaderPath(){return _serLeaderPath;}

protected:
	shared_ptr<ZkClient> 	_zkclient; 
private:
	string 					_zkhosts; 
	static  string      	_serRegPath; 
	static  string 			_serDLockPath;
	static  string 			_serLeaderPath;
	static  string 			_serSessionPath;
};


#endif 


