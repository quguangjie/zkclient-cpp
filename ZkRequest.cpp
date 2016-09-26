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

#include "ZkBase.h"
#include "ZkRequest.h"
#include "ZkClient.h"
#include <boost/lexical_cast.hpp>


const string ZkRequest::null_string = NULL_STRING;
ZkRequest::ZkRequest(const string &name, const string &serstring, long ver):ZkBase(name, serstring),_reqName(name)
{
	_reqName = name;
	_reqSerFullPath =  getSerRegPath()  + "/" + name + "/v" + boost::lexical_cast<string>(ver);
	cout << _reqSerFullPath << endl;
	setParentPath(_reqSerFullPath);

	pthread_mutex_init(&mutex,NULL);
	init();
}
ZkRequest::~ZkRequest()
{
		_zkclient->unsubscribeChildChanges( _reqSerFullPath, shared_from_this());
		pthread_mutex_destroy(&mutex);

}
void ZkRequest::handleChildChange(const string &parentPath, const list<string> &currentChildren)
{
	cerr << "handleChildChange[" << parentPath << "]" << endl;   
	pthread_mutex_lock(&mutex);
	setList(currentChildren);
	pthread_mutex_unlock(&mutex);
}
void ZkRequest::handleParentChange(const string &parentPath)
{
}

bool ZkRequest::checkNodeExist()
{
	try{
		return  _zkclient->exists(_reqSerFullPath,false);
	}
	catch (ZkExceptionNoNode &e)
	{
		return false;
	}
	catch(ZkException &e)
	{
		return false;
	}
}
void ZkRequest::changeListToMap()
{
	list<string>::iterator it_list;
	map<string, vector<string> >::iterator it_map;
	_reqSermap.clear();
	for (it_list = _reqSerlist.begin();it_list !=_reqSerlist.end();it_list++)
	{
		string IDstr,IPstr;
		int  index = (*it_list).find_first_of("@");
		if(index > 0)
		{
			IDstr = (*it_list).substr(0, index);
			IPstr = (*it_list).substr(index +1, (*it_list).size());
		}
		else
		{
			IPstr = (*it_list);
		}
		_reqSermap.insert(make_pair(IDstr, IPstr));  
#if 0
		it_map =_reqSermap.find(IDstr);
		if(it_map == _reqSermap.end())
		{
			vector<string>	tmp_vec;
			tmp_vec.push_back(IPstr);
			_reqSermap[IDstr] = tmp_vec;
		}
		else
		{
			(*it_map).second.push_back(IPstr);
		}
#endif 
	}
	
}
bool ZkRequest::discovery()
{
	if (!checkNodeExist())
		return false;
	try{
		_zkclient->subscribeChildChanges( _reqSerFullPath, shared_from_this());
		_reqSerlist= _zkclient->getChildren(_reqSerFullPath);
	}catch (ZkExceptionNoNode &e){
		cerr <<"subscribeDataChanges"<<e.what()<<endl;
		return false;
	}
	return true;
}

bool    ZkRequest::getServer(string &ip, int &port, const string &ID)
{
	string serStr; 
	if(!getServer(serStr, ID))
		return false;
	ip = serStr.substr(0,  serStr.find_first_of(":")); 
	port = atoi((serStr.substr( serStr.find_first_of(":") +1, serStr.size())).c_str());  
	return true;
}

bool  ZkRequest::getServer(string &s, const string &ID)
{
	int 		size;
	map<string, string >::iterator it_map;
	pthread_mutex_lock(&mutex);
	changeListToMap();
	size = _reqSermap.count(ID);
	it_map = _reqSermap.find(ID);
	if (it_map == _reqSermap.end())
	{
		pthread_mutex_unlock(&mutex);
		return false;
	}
	srand((unsigned int )time(NULL));
	int randI = rand() % size;
	int i = 0;
	for(it_map = _reqSermap.begin(); i != randI; it_map++)
	{
		i++;	
	}
	s = (*it_map).second;
	pthread_mutex_unlock(&mutex);
	return true;
}




