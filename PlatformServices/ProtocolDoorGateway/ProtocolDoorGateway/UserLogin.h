#ifndef _H_USERLOGIN_H_
#define _H_USERLOGIN_H_

#include "Handler.h"

class UserLogin : public CHandler
{
public:
	UserLogin(void);
	virtual~UserLogin(void);

	int Run(Operator& op, CmdHead& head, char* body);
};

#endif