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

