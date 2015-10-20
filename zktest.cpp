#include "ZkClient.h"
#include "ZkException.h"
#include <stdexcept> 
#include <iostream>
#include <string>
#include <time.h>
#include <list>

using namespace std;
using boost::shared_ptr;
class watchData : public IZkDataListener
{

public:
	watchData(ZkClient &cli, string &path):IZkDataListener(path)
	{}
	~watchData(){}
void handleDataChange(string &dataPath, string &data);
void handleDataCreate(string &dataPath);
void   handleDataDeleted(string &dataPath);
	
};
void watchData ::handleDataChange(string &dataPath, string &data)
{
	cout<<"handleDataChange!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "<<dataPath<<"---"<<data<<endl;
}

void watchData ::handleDataCreate(string &dataPath)
{
	cout<<"handleDataCreate!!!!!!!!!!!!!!!!!!!!!!!!!!!! "<< dataPath<<endl;

}
void watchData :: handleDataDeleted(string &dataPath)
{
	cout<<"handleDataDeleted!!!!!!!!!!!!!!!!!!!!!!!!!!!! "<<dataPath<<endl;
}


class watchChild:public IZkChildListener
{
public:
	watchChild(ZkClient &cli, string &parentPath):IZkChildListener(parentPath){}
	~watchChild(){}
	void handleChildChange(string &parentPath, list<string> &currentChildren){
		cout <<"!!!!!!!!!!!!handleChildChange!!!!!!!!!!!!!!!!" <<parentPath <<endl;
		list<string>::iterator it;
		for(it=currentChildren.begin();it!=currentChildren.end();it++)
		{
				cout<<"now children is  "<<(*it)<<endl;
		}
		
	}
	void handleParentChange(string &parentPath){
		cout <<"!!!!!!!!!!!!handleParentChange!!!!!!!!!!!!!!!" <<parentPath <<endl;
	}

	
};

class watchStats :public IZkStateListener
{
	public:
		watchStats(ZkClient &cli, string &parentPath):IZkStateListener(){}
		~watchStats(){}
	 void handleStateChanged(int state)
	 {
		cout << state<<endl;
	
	 }

		
};	
void watchDatafunc(ZkClient &cli, string &path)
{
	shared_ptr<watchData>mywd(new watchData(cli,path));
	try{
	cli.subscribeDataChanges( path, mywd);
	}catch (ZkExceptionNoNode &e){
		cout <<"subscribeDataChanges"<<e.what()<<endl;
	}


}
int main()
{
	string hostname = "127.0.0.1:2181";
	string path = "/cliTtest1";
	string path2 = "/asncluster/";
	time_t ctime = 0;
	struct Stat mystat;
	long sessionTimeout = 10000;
	ZkClient cli(hostname,sessionTimeout,0);
#if 0
	shared_ptr<watchChild>mywc(new watchChild(cli,path));
	shared_ptr<watchStats>myws(new watchStats(cli,path));
	shared_ptr<watchData>mywd(new watchData(cli,path));
	try{
		cli.subscribeDataChanges( path, mywd);
	}catch (ZkExceptionNoNode &e){
		cout <<"subscribeDataChanges"<<e.what()<<endl;
	}


	try{
		cli.subscribeChildChanges( path, mywc);
	}catch (ZkExceptionNoNode &e){

		cout <<"subscribeChildChanges"<<e.what()<<endl;

	}
	try{
		cli.subscribeStateChanges(myws);
	}catch (ZkExceptionNoNode &e){

		cout <<"subscribeStateChanges"<<e.what()<<endl;

	}
#endif
	try {
		//cli.exists(path,true);
		cli.createPersistent(path,true);
		string epath = "/cliTtest1/111111";
		cli.createEphemeral(epath);
		string creatpath = cli.createEphemeralSequential( path2,NULL);
		cout <<"createEphemeralSequentia     	"<<creatpath<<endl;
		list<string> child =cli.getChildren(path, true);
		list<string>::iterator it;
		for(it=child.begin();it!=child.end();it++)
		{
				cout<<"now child is  "<<(*it)<<endl;
		}
		string path3("/1/2/3/4");
		cli.createPersistent( path3, true);
	//	cli.deleteRecursive( path);

	/******/

		struct tm tim_t;
		char timebuf[32] ={0};
		ctime = cli.getCreationTime(path);
		localtime_r(&ctime,&tim_t);
		strftime(timebuf,32,"%F",&tim_t);
		cout <<timebuf<<endl;

/*****/
	
		string data = cli.readData(path,&mystat);
		cout<<"readData"<<data<<endl;

/*****/
	
//	cli.writeData(path, "123456");
//	data = cli.readData(path);
//	cout<<"readData2" << data<< endl;
	
	}
	catch (ZkExceptionConnectionLoss & e) {
		cout << "ZkExceptionConnectionLoss"  <<endl;
	} catch (ZkExceptionSessionExpired &e) {
		cout << "ZkExceptionSessionExpired"  <<endl;
	} catch (ZkExceptionInterrupted &e) {
		cout << "ZkExceptionInterrupted"  <<endl;
	}
	catch (ZkExceptionNoNode &e)
	{
		cout  <<	"ZkExceptionNoNode [] "<<endl;	
	}
	catch (ZkExceptionNodeExists &e)
	{
		cout  <<	"ZkExceptionNodeExists [] "<<endl;
	}
	catch (ZkException &e)
	{
		cout  <<	"zxkException []  "<<e.what()<<endl;	
	}
	catch (exception &e)
	{
		cout << "Exceptione []" << e.what()<<endl;
	}
	catch (...)
	{
		cout << "....."  <<endl;
	}

	

	getchar();
	
	cli.close();
	return 0;
}
