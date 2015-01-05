#ifndef StaticVar_H
#define StaticVar_H
#include "Global.h"
#include "Queue.h"
#include "Operator.h"
#include "StandaryLibrary.h"
using namespace ty;
namespace ty
{
	class StandaryLibrary_API StaticVar
	{
	public:
		static Queue<Operator> g_MsgQueue;
		static ThreadPool* g_MsgThreadPool;
		static ThreadPool& getMsgThrPool();
		static FastMutex g_instMutex;
        static void PushToQueue(Operator op);
	};

}

#endif
