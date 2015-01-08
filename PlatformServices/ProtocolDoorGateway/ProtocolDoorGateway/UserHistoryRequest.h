#ifndef _H_USERHISTORYREQUEST_H_
#define _H_USERHISTORYREQUEST_H_

#include "Common.h"
#include "Handler.h"

class UserHistoryRequest : public CHandler
{
public:
	UserHistoryRequest(void);
	virtual ~UserHistoryRequest(void);

	int Run(Operator& op, CmdHead& head, char* body);
};

#endif
