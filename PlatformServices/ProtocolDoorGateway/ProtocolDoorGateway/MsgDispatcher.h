#ifndef MSG_DISPATCHER_H
#define MSG_DISPATCHER_H
#include "Global.h"
#include "Handler.h"

class FactoryHandler
{
public:
	static CHandler *GetHandler(int id);
};

#endif
