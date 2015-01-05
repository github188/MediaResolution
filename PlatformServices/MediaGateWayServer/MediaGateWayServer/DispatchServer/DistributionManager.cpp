#pragma once

#include "Poco/Mutex.h"
#include "BussinessServer.h"
#include "BussinessEvent.h"
#include "Operator.h"
#include "Global.h"
#include "NetUtility.h"
#include "DistributionManager.h"
#include "LogInfo.h"
#include "LogSys.h"

int DistributionManager::task_id_ = 1;

bool DistributionManager::AddClient(StreamSocket &socket_)
{   
    DistributionTask * dsTask = new DistributionTask(this,&memory_pool_);
    dsTask->need_extradata_ = task_need_extradata_;
    dsTask->socket_ = socket_;

    mutex_.lock();
    dsTask->id_ = DistributionManager::task_id_++;
    task_record_[dsTask->id_] = dsTask;
    mutex_.unlock();
    
    dsTask->Start();
	//查看是否创建分发
	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
	transLog.log(Message::PRIO_DEBUG, "创建分发到客户端%s", socket_.peerAddress().toString());
	//mlcai 2014-04-09

    return true;
}

void DistributionManager::Stop()
{
    map<int,DistributionTask*>::iterator pos;
    mutex_.lock();
	pos = task_record_.begin();
	for(pos;pos != task_record_.end();pos++)
	{   
		DistributionTask* pDS_T = pos->second; 
		pDS_T->is_stop_ = true;
	}
	mutex_.unlock();

	while(task_record_.size() !=0)
		Sleep(20);
	
	printf("dm deleted\n");
}

string DistributionManager::Notify(int type, char* frame, int length_)
{       
	switch (type)
	{
    case COLSE_CONNECT:
        break;
    case MEM_POOL_COVER:
        {
            SequenceFlag *pSQFlag;
            pSQFlag = (SequenceFlag *)frame;
            map<int,DistributionTask*>::iterator pos;

            int iFrameSEQ = memory_pool_.getiFrameSequenceNum(); //获取I帧包序号；
            mutex_.lock();
            for (pos = task_record_.begin();pos != task_record_.end();pos++)
            {   
                DistributionTask* pDS_T = pos->second;  
                if(pDS_T->sequence_ <= pSQFlag->end_index_) //跳帧处理;
                {  
                    cout <<"tasking " << pDS_T->id_<<" skip "<<pSQFlag->end_index_ - pDS_T->sequence_ + 1 <<" frames"<<endl; 
                    pDS_T->sequence_ = iFrameSEQ; //跳到找到的I帧序号包。
                }
            }
            mutex_.unlock();
        }
        break;
    case NET_CONNECT_ERROR:
        {
            int *pTid =(int *)frame;
            map<int,DistributionTask*>::iterator pos;
            mutex_.lock();
            pos = task_record_.find(*pTid);
            if(pos != task_record_.end())
            {   
                DistributionTask* pDS_T = pos->second; 
                pDS_T->is_stop_=true;
            } 
            mutex_.unlock();
        }
        break;
    case DSTASK_QUIT:
        {
            int *pTid =(int *)frame;
            map<int,DistributionTask*>::iterator pos;
            mutex_.lock();
            pos = task_record_.find(*pTid);
            if(pos != task_record_.end())
            {   
                DistributionTask* pDS_T = pos->second;
                delete pDS_T;
                task_record_.erase(pos);
            } 
			int task_size = task_record_.size();
			mutex_.unlock();
			if (0 == task_size)
				notify_object_->Notify(DESTROY, (char*)key_, 0);
        }
        break;
    default:
        break;
	}
	return "success";
}
	

