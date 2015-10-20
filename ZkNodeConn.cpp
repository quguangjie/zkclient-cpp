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




