
#include "StaticVar.h"
Queue<Operator> StaticVar::g_MsgQueue(10000);
ThreadPool* StaticVar::g_MsgThreadPool = 0;
FastMutex StaticVar::g_instMutex;

ThreadPool& StaticVar::getMsgThrPool()
{
	FastMutex::ScopedLock locker(g_instMutex);
	if (0 == g_MsgThreadPool)
		g_MsgThreadPool = new ThreadPool(10,100);
	return *g_MsgThreadPool;
};

void StaticVar::PushToQueue(Operator op)
{
    g_MsgQueue.Push(op);
}
