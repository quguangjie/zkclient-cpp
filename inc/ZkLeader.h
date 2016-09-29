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
	virtual   void leader(list<shared_ptr<ZkNode> > &follow); 

	bool 		  					 isLeader();
	const list<shared_ptr<ZkNode> > &follower();

private:
	list<shared_ptr<ZkNode> > _follower;
	string 					_LdPath;
	string 					_LdNodeName;
	bool 				    _LdcreateFlag;
};


#endif 


