#ifndef _TERMHEAR_H_
#define _TERMHEAR_H_

#include "Handler.h"

class TermHeart : public CHandler
{
public:
	TermHeart();
	virtual~TermHeart();

	int Run(Operator& op, CmdHead& head, char* body);
};

#endif