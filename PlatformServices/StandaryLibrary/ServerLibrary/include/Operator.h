#ifndef Operator_H
#define Operator_H
#include "Global.h"
#include "StandaryLibrary.h"

typedef struct eXosip_event eXosip_event_t;
class SipServer;
namespace ty
{
	class MsgHander;
    
	class StandaryLibrary_API Operator
	{
	public:
		MsgHander * hand;
        eXosip_event_t* event;
        SipServer*      sipServer;
	};
}
#endif