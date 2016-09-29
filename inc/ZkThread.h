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

#ifndef __ZKCLIENT_ZKTHREAD
#define __ZKCLIENT_ZKTHREAD


#include <string>
#include <vector>
using namespace  std; 

class ZkThread
{
public:
	ZkThread();
	ZkThread(int  n);
	virtual ~ZkThread(){};  

	bool  start();
	void  shutdown();
	long  getId();
	bool  isShutdown();
	void  setName(string &name);

	static void* threadRun(void *p);

	virtual void run() = 0;

private:
	int 				_thNum;
	vector<pthread_t>   _ptids;
	string         _name;
	bool           _shutdown;
};


#endif 


