#ifndef BussinessEvent_H
#define BussinessEvent_H
#include "Operator.h"
using namespace ty;
namespace ty
{
	class BussinessEvent
	{
	public:
		virtual int onReadable(Operator &op)
		{
			return -1;
		}
	};

}

#endif
