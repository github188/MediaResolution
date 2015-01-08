#ifndef AccessEvent_H
#define AccessEvent_H
#include "Global.h"
#include "BussinessEvent.h"
#include "NetUtility.h"
#include "Handler.h"
#include "MsgDispatcher.h"
#include "Const.h"

class AccessEvent : BussinessEvent
{
public:
	int onReadable(Operator &op);
	int OnRecvMsg(Operator &op, CmdHead& head, char* body);
};

#endif
