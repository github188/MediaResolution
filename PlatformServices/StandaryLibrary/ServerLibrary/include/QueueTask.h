#ifndef QUEUE_TASK_H
#define QUEUE_TASK_H
#include "Global.h"
#include "Queue.h"
#include "MsgHandler.h"
#include "BussinessEvent.h"
#include "StandaryLibrary.h"
namespace ty
{
	class StandaryLibrary_API Queuetask : public Poco::Runnable
	{
	public:
		explicit Queuetask();
		virtual ~Queuetask();
	public:
		BussinessEvent *event;
		void run();
	protected:
		bool _stop;
	};

}
#endif
