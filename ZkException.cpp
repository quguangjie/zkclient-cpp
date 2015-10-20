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



