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
#include "ZkClient.h"
#include <pthread.h>
#include <list>
#include <map>
#include <time.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using   namespace std;
using   boost::shared_ptr;

class ZkRequest:public enable_shared_from_this<ZkRequest>, virtual public ZkBase, public IZkChildListener
{
public:
	ZkRequest(const string &name, const string &serstring, long ver);   
	virtual ~ZkRequest();

	bool    discovery(); 
	bool    getServer(string &ip, int &port, const string &ID=null_string);  
	
	const list<string>& getList()const {return _reqSerlist;}
	void setList(const  list<string> &l){_reqSerlist = l;}

private:
#define    NULL_STRING    ""
	static const string   null_string;
	void handleChildChange(const string &parentPath, const list<string> &currentChildren);
	void handleParentChange(const string &parentPath);

	bool  getServer(string &s, const string &ID); 
	bool checkNodeExist();
	void changeListToMap();

	pthread_mutex_t mutex;
	string _reqName;
	string _reqSerFullPath;
	list<string> _reqSerlist;
	multimap<string ,string > _reqSermap;
};


#endif 


