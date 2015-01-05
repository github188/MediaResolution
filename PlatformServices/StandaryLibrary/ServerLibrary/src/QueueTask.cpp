#include "QueueTask.h"
#include "Operator.h"
namespace ty
{

	Queuetask::Queuetask():_stop(false)
	{

	}
	Queuetask::~Queuetask()
	{
		_stop=true;
	}
	void Queuetask::run()
	{
		while(!_stop)
		{
			Operator op;
			if(StaticVar::g_MsgQueue.Pop(op,true))
			{
				try
				{
					if (op.hand != NULL && op.hand->is_delete_by_self == 1)
					{
						op.hand->_sockMutex.lock();
						if (op.hand->is_process_by_self == 1)
						{
							op.hand->is_process_by_self = 0;
							op.hand->Start();
							op.hand->_sockMutex.unlock();
							continue;
						}
					}

					int nRet = event->onReadable(op);

					if (op.hand != NULL && op.hand->is_delete_by_self == 1)
					{
						op.hand->_sockMutex.unlock();
					}

					if (nRet==-1)
					{
						if (NULL != op.hand && op.hand->is_delete_by_self == 0)
						{
							op.hand->Release();
						}
						continue;
					}
					if(nRet==-2)
					{
						continue;
					}
					else
					{
						op.hand->Start();
					}
				}
				catch(...)
				{
				}
			}
		}

	}
}
