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

#include <list>
#include "ZkRequest.h"
#include "ZkAccept.h"
#include "ZkNodeConn.h"


template<typename T>
ZkNodeConn<T>::ZkNodeConn(string &name, string &addr, string &serstring):ZkRequest(name, addr, serstring),ZkAccept(name, addr, serstring)
{
}
template<typename T>
ZkNodeConn<T>::~ZkNodeConn()
{
}

template<typename T>
void   ZkNodeConn<T>::setNodeResource(ZkNode &node, T &i)
{
}
template<typename T>
void   ZkNodeConn<T>::getNodeResource(ZkNode &node, list<T> &l)
{
}
template<typename T>
bool   ZkNodeConn<T>::syncResource()
{
	return true;
}

template<typename T>
void ZkNodeConn<T>::resAlloc(list<ZkNode> &follower)
{
}
template<typename T>
void ZkNodeConn<T>::updateRouter(list<ZkNode> &follower)
{
}

template<typename T>
void ZkNodeConn<T>::leader(list<ZkNode> &follow)
{
}




