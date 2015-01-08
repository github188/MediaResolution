#include "MsgDispatcher.h"
#include "define.h"
#include "TermLogin.h"
#include "TermAbilityReport.h"

CHandler *FactoryHandler::GetHandler(int id)
{
	CHandler* pHandler = NULL;
	switch (id)
	{
	case CMD_REGTERM:
		{
			pHandler = new TermLogin;
			break;
		}
    case CMD_TERMABILITY:
		{
			pHandler = new TermAbilityReport;
			break;
		}
	default:
		{

		}
	}
	return pHandler;
}
