#ifndef _H_USERREALTIMEREQUEST_H_
#define _H_USERREALTIMEREQUEST_H_

#include "Handler.h"

class UserRealTimeRequest : public CHandler
{
public:
	UserRealTimeRequest();
	virtual~UserRealTimeRequest();

	int Run(Operator& op, CmdHead& head, char* body);
};

#endif