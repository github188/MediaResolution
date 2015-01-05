#pragma once
#include <string>
#include <iostream>
#include <map>
#include "INotify.h"
#include "NetUtility.h"
#include "MemoryPool.h"
#include "DistributionTask.h"

typedef enum
{   
    COLSE_CONNECT     = 40,
    MEM_POOL_COVER    = 60,
    NET_CONNECT_ERROR = 80,
    DSTASK_QUIT       = 81,
	DESTROY = 82,
}emNOTIFY;

class DistributionManager:public INotify
{
public:
    DistributionManager(INotify  *notify_obj, int queue_num_ =3,int _perSize = 1024*1024*1): 
	  notify_object_(notify_obj), memory_pool_(this,queue_num_,_perSize)
    {
		m_lPacketSeq = 0;
       task_need_extradata_ = false;
    }
    ~DistributionManager(){Stop();}
private:
    static int task_id_;


public:
    static int get_task_id(){return task_id_;}

public:
    MemoryPool memory_pool_;
    map<int,DistributionTask*> task_record_;
    Poco::FastMutex   mutex_;
    bool task_need_extradata_;
	INotify  *notify_object_;
	long key_;
	DWORD       m_lPacketSeq;

public:
	bool AddClient(StreamSocket &socket_);

	void set(long key) { key_ = key; }

	string Notify(int type, char* frame, int length_);

	//»ñÈ¡°üÐòºÅ
	long GetPacketSeq()
	{ 
		m_lPacketSeq++;
		if ( m_lPacketSeq > 0xffff )
		{
			m_lPacketSeq = 0;
		}
		return m_lPacketSeq;
	}

private:
    void Stop();
};