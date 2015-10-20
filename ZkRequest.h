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

#ifndef __ZKCLIENT_ZKREQUEST
#define __ZKCLIENT_ZKREQUEST
#include "ZkBase.h"
#include <pthread.h>
#include <list>
#include <map>
#include <time.h>
#include "boost/shared_ptr.hpp"

using   namespace std;
using   boost::shared_ptr;

class watchReqNodeChild;
class ZkRequest:virtual public ZkBase
{
public:
	ZkRequest(const string &name, const string &serstring, const long ver);   
	virtual ~ZkRequest();

	bool    discovery(); 
	bool    getServer(string &ip, int &port, const string &ID=null_string);  
	
	pthread_mutex_t ReqMutex;
	const list<string> getList()const {return _reqSerlist;}
	void setList(list<string> &l){_reqSerlist = l;}
private:
#define    NULL_STRING    ""
	static const string   null_string;

	bool  getServer(string &s, const string &ID); 
	bool checkNodeExist();
	void changeListToMap();
	string _reqName;
	string _reqSerFullPath;
	shared_ptr<watchReqNodeChild> _reqMywc;
	list<string> _reqSerlist;
	multimap<string ,string > _reqSermap;
};


#endif 


