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


