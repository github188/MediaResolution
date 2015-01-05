#pragma once
#include <string>
#include <iostream>
#include "DistributionManager.h"


class DistributionTask:public Poco::Runnable  
{
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

public:
    int id_;
    int sequence_;
	StreamSocket socket_;
    bool need_extradata_;
    bool is_stop_;

public:
	void run();
	void Start();
};