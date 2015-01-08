#ifndef _H_USERHEART_H_
#define _H_USERHEART_H_
#include "Handler.h"
#include "Common.h"

class UserHeart : public CHandler
{
public:
	UserHeart(void);
	virtual~UserHeart(void);

public:
	int Run(Operator& op, CmdHead& head, char* body);
};

#endif
