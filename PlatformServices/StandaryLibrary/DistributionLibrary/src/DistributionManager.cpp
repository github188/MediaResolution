#pragma once

#include "Poco/Mutex.h"
#include "NetUtility.h"
#include "DistributionManager.h"

map<int,DistributionManager*> DistributionManager::dm_map_;
Poco::Mutex DistributionManager::dm_mutex_;
int DistributionManager::dm_ind_=0;
bool DistributionManager::dm_run_=false;
Thread DistributionManager::scan_thr_;

void DistributionManager::ScanTask::run()
{
	dm_mutex_.lock();
	dm_run_=true;
	dm_mutex_.unlock();
	do 
	{
		dm_mutex_.lock();
		map<int,DistributionManager*> map_copy_=dm_map_;
		dm_mutex_.unlock();

		map<int,DistributionManager*>::iterator itStart,itEnd;
		itStart=map_copy_.begin();
		itEnd=map_copy_.end();
		for(;itStart!=itEnd;++itStart)
		{
			Poco::Timestamp now;
			if((itStart->second)->has_client_)                    //已经有分发任务
			{
				map<int,DistributionManager*>::iterator itDel;
				dm_mutex_.lock();
				itDel=dm_map_.find(itStart->first);	
				dm_map_.erase(itDel);
				dm_mutex_.unlock();
			}
			else if(now-(itStart->second)->create_time_>10*1000)     //没有分发任务，但是已经超时
			{
				(itStart->second)->notify_object_->Notify(DESTROY,
					(char*)(&(itStart->second)->key_),sizeof((itStart->second)->key_));
				dm_mutex_.lock();
				map<int,DistributionManager*>::iterator itDel;
				itDel=dm_map_.find(itStart->first);	
				dm_map_.erase(itDel);
				dm_mutex_.unlock();
			}
		}
		Thread::sleep(5000);
	} while (dm_map_.size());

	//线程结束
	dm_mutex_.lock();
	dm_run_=false;
	dm_mutex_.unlock();
	delete this;
}

DistributionManager::DistributionManager(INotify  *notify_obj, int queue_num_,int _perSize): 
notify_object_(notify_obj), memory_pool_(this,queue_num_,_perSize),create_time_(Poco::Timestamp())
{
	task_need_extradata_ = false;
	has_client_=false;
	task_id_ = 0;
	dwUser=0;
	this->dm_key_=dm_ind_++;
	bool bRun=false;

	dm_mutex_.lock();
	dm_map_.insert(make_pair(this->dm_key_,this));
	bRun=dm_run_;
	dm_mutex_.unlock();
	if(!bRun)
		scan_thr_.start(*new ScanTask());
}

DistributionManager::~DistributionManager()
{
	stop();
}

void DistributionManager::SetKey(long key)
{
	key_=key;
}

void DistributionManager::SetIsSendExtraData(bool bSend)
{
	task_need_extradata_=bSend;
}

MemoryPool* DistributionManager::GetMemoryPool()
{
	return &memory_pool_;
}


bool DistributionManager::AddClient(StreamSocket &socket_)
{   
    DistributionTask * dsTask = new DistributionTask(this,&memory_pool_);
    dsTask->need_extradata_ = task_need_extradata_;
    dsTask->socket_ = socket_;

    mutex_.lock();
    dsTask->id_ = task_id_++;
    task_record_[dsTask->id_] = dsTask;
    mutex_.unlock();
 
    dsTask->Start();
	has_client_=true;
	
	printf("创建分发%d 到客户端%s\n",dsTask->id_,socket_.peerAddress().toString().c_str());

    return true;
}

void DistributionManager::stop()
{
    map<int,DistributionTask*>::iterator pos;
    mutex_.lock();
    pos = task_record_.begin();
    for(pos;pos != task_record_.end();pos++)
    {   
        DistributionTask* pDS_T = pos->second; 
        pDS_T->is_stop_ = true;
    }

    while(task_record_.size() !=0)
    {   
        mutex_.unlock();
        Sleep(20);
    }
    mutex_.unlock();
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
            MemoryPool::SequenceFlag *pSQFlag;
            pSQFlag = (MemoryPool::SequenceFlag *)frame;
            map<int,DistributionTask*>::iterator pos;

            int iFrameSEQ = memory_pool_.getiFrameSequenceNum(); //获取I帧包序号；
            mutex_.lock();
            for (pos = task_record_.begin();pos != task_record_.end();pos++)
            {   
                DistributionTask* pDS_T = pos->second;  
                if(pDS_T->sequence_ <= pSQFlag->end_index_) //跳帧处理;
                {  
                    printf("tasking %d skip %d frames\n",pDS_T->id_,pSQFlag->end_index_-pDS_T->sequence_+1); 
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
				notify_object_->Notify(DESTROY, (char*)key_, sizeof(key_));
        }
        break;
    default:
        break;
	}
	return "success";
}
	

