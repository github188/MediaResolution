#include "BussinessServer.h"

bool BussinessServer::start()
{
	StartBasicTask(nThreadNum);
	Poco::TimerCallback<BussinessServer> timer(*this,&BussinessServer::OnTimer);
	m_Timer.start(timer);
	return true;
}
void BussinessServer::StartBasicTask(int nNum)
{
	for (int i=0;i<nNum;i++)
	{
		Queuetask *pTask=new Queuetask;
		pTask->event=event;
		try
		{
			ThreadPool& thrPool = StaticVar::getMsgThrPool();
			thrPool.start(*pTask,"MsgTask");
		}
		catch (...)
		{
			delete pTask;
			Poco::Thread::sleep(1000);//sleep 1s
		}
	}
}
void BussinessServer::OnTimer(Poco::Timer& timer)
{
	if (&timer==&m_Timer)
	{
		if (StaticVar::g_MsgQueue.used()<nThreadNum)
		{
			ThreadPool& thrPool = StaticVar::getMsgThrPool();
			thrPool.collect();
		}
		if (StaticVar::g_MsgQueue.used()>nThreadNum)
		{
			StartBasicTask(10);
		}
	}
}
