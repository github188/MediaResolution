#pragma once

#include "Poco/Mutex.h"
#include "NetUtility.h"
#include "DistributionManager.h"


void DistributionTask::Start() 
{
    work_thread_.start(*this);
}

void DistributionTask::run() 
{
    char* pMemPool;
    bool bRet = false;
    sequence_ = memory_pool_->getiFrameSequenceNum(); //获取I帧包序号；

	int fail_num = 0;
	int sendfail_num=0;
    while(!is_stop_)
    {   
        if(need_extradata_)
        {
			if(memory_pool_->extradata_length_ != 0)
			{
				if(!NetUtility::SendData(socket_,memory_pool_->extradata_,memory_pool_->extradata_length_))
					notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));

				need_extradata_ = false;
				fail_num = 0;
			}
			else
			{
				if (fail_num++>40)
					notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));

				Thread::sleep(50);
			}

            continue;
        }

        int  length_ = 0;
        bRet = memory_pool_->Pop(sequence_,&pMemPool,length_);
        if(bRet == true)
        {
			bool bRet=NetUtility::SendData(socket_,pMemPool,length_);
            if(!bRet)
            {
				if(sendfail_num++>40)
				{
					notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));
					printf("发送数据失败导致分发到客户端 %s 关闭！\n", socket_.peerAddress().toString().c_str());
				}
				else
					Thread::sleep(50);
				continue;
            }
            sequence_++;
			sendfail_num = 0;
			fail_num=0;
        }
		else
		{
			if (fail_num++ > 50)
			{
				notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));
				printf("取不到数据,导致分发到客户端%s关闭！\n", socket_.peerAddress().toString().c_str());
			}
			else
				Thread::sleep(30);
		}
    }

    notyfy_object_->Notify(DSTASK_QUIT,(char*)&id_,sizeof(id_));
}

