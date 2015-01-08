#ifndef _H_TERMABILITYREPORT_H_
#define _H_TERMABILITYREPORT_H_
#include "Handler.h"

class TermAbilityReport : public CHandler
{
public:
	TermAbilityReport(void);
	virtual~TermAbilityReport(void);

	int Run(Operator& op, CmdHead& head, char* body);
};

#endif

