#pragma once
#include <string>
#include <iostream>
#include "DistributionManager.h"

#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Net/StreamSocket.h"
using namespace Poco;
using namespace Poco::Net;

class DistributionTask:public Poco::Runnable  
{
	friend class DistributionManager;
public:
    DistributionTask(INotify  * inotyfyObj,MemoryPool * mempool) : notyfy_object_(inotyfyObj),memory_pool_(mempool)
    {
        sequence_ = 0;
        need_extradata_ =false;
        is_stop_ = false;
    }
    ~DistributionTask(){}

private:
	INotify  *notyfy_object_;
	MemoryPool *memory_pool_; 
	Poco::Thread work_thread_;

private:
    int id_;
    int sequence_;
	StreamSocket socket_;
    bool need_extradata_;
    bool is_stop_;

private:
	void run();
	void Start();
};