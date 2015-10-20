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

#ifndef __ZKCLIENT_ZKACCEPT  
#define __ZKCLIENT_ZKACCEPT  

#include "ZkLeader.h"

class ZkBase;
class ZkNode;
class ZkLeader;

class ZkAccept: virtual public ZkLeader  
{
public:  
	ZkAccept(const string &name, const string &addr, const string &serstring, const long ver);   
	ZkAccept(const string &name, const string &addr, const string &serstring, const string &leaderName, const long ver);   

	virtual ~ZkAccept();

	bool serRegister();  
	void leader(list<ZkNode> &follow);
private: 
	string _acPath;
	string _acNodeName;
};


#endif 


