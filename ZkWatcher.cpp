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

#include "ZkWatcher.h"  


ZkWatchedEvent::ZkWatchedEvent()
{
	_vaild = false;
	_path.clear();   
	_type = NoNodeEvent;
}

ZkWatchedEvent::ZkWatchedEvent(EventType type)
{
	_vaild = true;
	_path.clear();  
	_type = type;  
}

ZkWatchedEvent::ZkWatchedEvent(string &path, EventType type)
{
	_vaild = true;
	_path = path;  
	_type = type;  
}

ZkWatchedEvent::~ZkWatchedEvent()
{
}
bool   ZkWatchedEvent::vaild()
{
	return _vaild;
}

const char *   ZkWatchedEvent::getPath()
{
	if(_path.empty())
		return NULL;
	return _path.c_str(); 
}
ZkWatchedEvent::EventType  ZkWatchedEvent::getType()
{
	return _type; 
}


ZkWatcher::ZkWatcher(): ZkEventThread<ZkWatchedEvent >(), ZkThread()
{
	start();
}
ZkWatcher::~ZkWatcher()
{
}
void ZkWatcher::run()
{
	while (!isShutdown())
	{
		ZkWatchedEvent t = recv();
		try {
			process(t);
		}
		catch (ZkExceptionInterrupted e) {
		}
	}
}

