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

#ifndef __ZKCLIENT_WATCHER  
#define __ZKCLIENT_WATCHER  
#include <string>
#include "ZkEventThread.h"
#include "ZkConnection.h"

using namespace std;


class ZkWatchedEvent
{
public:
	enum EventType
	{
		NoNodeEvent = 0,
		NodeDataChanged,
		NodeDeleted ,
		NodeCreated,
		NodeChildrenChanged,
		NodeNoWaching, 
		StateChanged  
	};
	ZkWatchedEvent();
	ZkWatchedEvent(string &path, EventType type);
	ZkWatchedEvent(EventType type);
	~ZkWatchedEvent();  
	const char *   getPath();  
	EventType  getType();
	bool       vaild();

private:
	string 		_path;
	bool 		_vaild; 
	EventType   _type;  
};

class ZkWatcher : public ZkEventThread<ZkWatchedEvent>, ZkThread
{
public:
	ZkWatcher();
	virtual ~ZkWatcher();

	void run();   // thread function
	virtual void process(ZkWatchedEvent &event)=0;  
};

#endif 


