#ifndef BussinessServer_H
#define BussinessServer_H
#include "Global.h"
#include "MsgHandler.h"
#include "QueueTask.h"
#include "StaticVar.h"
namespace ty
{
	class BussinessServer
	{
	public:
		BussinessServer(BussinessEvent *event,int nthdnum=10):m_Timer(2*2000,2*2000)
		{
			this->event=event;
			nThreadNum=nthdnum;
		}
		~BussinessServer()
		{
		}
	public:
		bool start();
		void StartBasicTask(int nNum);
		void OnTimer(Poco::Timer& timer);
	private:
		Poco::Timer m_Timer;
		int nThreadNum;
		BussinessEvent *event;
		Poco::Thread m_thd;
	};


}
#endif
