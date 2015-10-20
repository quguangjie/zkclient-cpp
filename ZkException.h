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

#ifndef  __ZKCLIENT_ZKEXCEPTION
#define  __ZKCLIENT_ZKEXCEPTION

#include <zookeeper.h>    
#include <string>
#include <stdexcept> 


using namespace std;  

class ZkExceptionNoNode;
class ZkExceptionBadVersion;
class ZkExceptionNodeExists;
class ZkErrorMarshalling;
class ZkExceptionTimeout;
class ZkExceptionInterrupted; 

class ZkException:public std::out_of_range
{
public:
    ZkException(string &message):out_of_range(message) 
	{
		_message = message;
	}
    ZkException(int code, string &message):out_of_range(message)
	{
		_errCode = code;
		_message = message;
	}  
    virtual ~ZkException() throw (){};
private:
	string    _message;
    int  	  _errCode;
};

class ZkExceptionNoNode:public ZkException
{
public:
	ZkExceptionNoNode(int code, string &message):ZkException(code, message){}
	~ZkExceptionNoNode()  throw (){};
};
class ZkExceptionBadVersion: public ZkException
{
public:
	ZkExceptionBadVersion(int code, string &message):ZkException(code, message){}
	~ZkExceptionBadVersion()  throw (){};
};
class ZkExceptionNodeExists: public ZkException
{
public:
	ZkExceptionNodeExists(int code, string &message):ZkException(code, message){}
	~ZkExceptionNodeExists()  throw (){};
};
class ZkErrorMarshalling: public ZkException
{
public:
	ZkErrorMarshalling(int code, string &message):ZkException(code, message){}
	~ZkErrorMarshalling()  throw (){};
};
class ZkExceptionTimeout:  public ZkException
{
public:
	ZkExceptionTimeout(int code, string &message):ZkException(code, message){}
	~ZkExceptionTimeout()  throw (){};
};
class ZkExceptionInterrupted:  public ZkException
{
public:
	ZkExceptionInterrupted(int code, string &message):ZkException(code, message){}
	~ZkExceptionInterrupted()  throw (){};
};
class ZkExceptionDataInconsistency:  public ZkException
{
public:
	ZkExceptionDataInconsistency(int code, string &message):ZkException(code, message){}
	~ZkExceptionDataInconsistency()  throw (){};
};
class ZkExceptionConnectionLoss:  public ZkException
{
public:
	ZkExceptionConnectionLoss(int code, string &message):ZkException(code, message){}
	~ZkExceptionConnectionLoss()  throw (){};
};

class ZkExceptionInvalidACL:  public ZkException
{
public:
	ZkExceptionInvalidACL(int code, string &message):ZkException(code, message){}
	~ZkExceptionInvalidACL()  throw (){};
};
class ZkExceptionAuthFailed:  public ZkException
{
public:
	ZkExceptionAuthFailed(int code, string &message):ZkException(code, message){}
	~ZkExceptionAuthFailed()  throw (){};
};
class ZkExceptionSessionExpired:  public ZkException
{
public:
	ZkExceptionSessionExpired(int code, string &message):ZkException(code, message){}
	~ZkExceptionSessionExpired()  throw (){};
};

#endif 


