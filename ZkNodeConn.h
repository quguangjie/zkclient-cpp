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

#ifndef __ZKCLIENT_ZKNODECONN
#define __ZKCLIENT_ZKNODECONN

#include <map>
#include <list>
using namespace  std; 

class ZkNode;
class ZkRequest;
class ZkAccept;
class ZkLeader;


template<typename T> class ZkNodeConn;  

template<typename T>
class ZkNodeConn: public ZkRequest, public ZkAccept  
{
public:
	ZkNodeConn(string &name, string &addr, string &serstring);   
	virtual ~ZkNodeConn();  

	virtual void resAlloc(list<ZkNode> &follower);      
	virtual void updateRouter(list<ZkNode> &follower);  

	void   setNodeResource(ZkNode &node, T &i);   
	void   getNodeResource(ZkNode &node, list<T> &l);    
	bool   syncResource(); 
private:  
	void leader(list<ZkNode> &follow);   

private:  
	map<string, list<T> > _resource;   
};

#endif 


