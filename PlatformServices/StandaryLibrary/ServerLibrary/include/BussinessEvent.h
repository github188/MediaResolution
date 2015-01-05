#ifndef BussinessEvent_H
#define BussinessEvent_H

#include "Operator.h"
#include "StandaryLibrary.h"

using namespace ty;
namespace ty
{
	class StandaryLibrary_API BussinessEvent
	{
	public:
		virtual int onReadable(Operator &op)
		{
			return -1;
		}
	};

}

#endif
