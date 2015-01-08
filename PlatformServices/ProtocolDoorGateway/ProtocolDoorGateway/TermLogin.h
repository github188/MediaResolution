#ifndef _H_TERMLOGIN_H_
#define _H_TERMLOGIN_H_

#include "Global.h"
#include "Handler.h"
#include "Operator.h"
#include "Const.h"

using namespace ty;

class TermLogin : public CHandler
{
public:
	TermLogin();
	virtual~TermLogin();

	int Run(Operator& op, CmdHead& head, char* body);

private:
	static FastMutex mutex_;
};

#endif