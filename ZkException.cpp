#include <zookeeper.h>    
#include <string>
#include <stdexcept> 

#include "ZkException.h"

#if 0 
ZkException *  ZkException::create() 
{
	switch (_errCode) {
		// case ZDATAINCONSISTENCY:
		// return new ZkExceptionDataInconsistency();
		// case ZCONNECTIONLOSS:
		// return new ZkExceptionConnectionLoss();
		case ZNONODE:
			return new ZkExceptionNoNode(_errCode, _message);
			// case NOAUTH:
			// return new ZkNoAuthException();
		case ZBADVERSION:
			return new ZkExceptionBadVersion(_errCode, _message);
			// case NOCHILDRENFOREPHEMERALS:
			// return new ZkExceptionNoChildrenForEphemerals();
		case ZNODEEXISTS:
			return new ZkExceptionNodeExists(_errCode, _message);
			// case ZINVALIDACL:
			// return new ZkExceptionInvalidACL();
			// case ZAUTHFAILED:
			// return new ZkExceptionAuthFailed();
			// case ZNOTEMPTY:
			// return new ZkExceptionNotEmpty();
			// case ZSESSIONEXPIRED:
			// return new ZkExceptionSessionExpired();
			// case ZINVALIDCALLBACK:
			// return new ZkExceptionInvalidCallback();

		default:
			return new ZkException(_errCode, _message);
	}
}
#endif



