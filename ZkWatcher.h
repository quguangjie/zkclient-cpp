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


