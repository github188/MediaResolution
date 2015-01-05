#pragma once

#include "Poco/Mutex.h"
#include "BussinessServer.h"
#include "BussinessEvent.h"
#include "Operator.h"
#include "Global.h"
#include "NetUtility.h"
#include "LogInfo.h"
#include "LogSys.h"

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
				if(memory_pool_->extradata_length_!=NetUtility::SafeSendData(socket_,memory_pool_->extradata_,memory_pool_->extradata_length_))
				{
					notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));
				}
				need_extradata_ = false;
				fail_num = 0;
			}
			else
			{
				if (fail_num++>40)
				{
					notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));
				}
				Thread::sleep(50);
			}

            continue;
        }

        int  length_ = 0;
        bRet = memory_pool_->Pop(sequence_,&pMemPool,length_);
        if(bRet == true)
        {
			int ret=NetUtility::SafeSendData(socket_,pMemPool,length_);
            if(ret<0)
			{
				notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));
				LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
				transLog.log(Message::PRIO_ERROR, "数据发送失败导致分发到客户端%s关闭,ret=%d", socket_.peerAddress().toString(),ret);
				continue;
			}
            sequence_++;
			sendfail_num = 0;
			fail_num=0;
        }
		else
		{
			if (fail_num++ > 200)
			{
				notyfy_object_->Notify(NET_CONNECT_ERROR,(char*)&id_,sizeof(id_));
				//记录服务端取数据失败主动掉线的日志
				LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
				transLog.log(Message::PRIO_ERROR, "取不到数据,导致分发到客户端%s关闭！", socket_.peerAddress().toString());
				//sunminping 2014-06-03
			}
			else
				Thread::sleep(30); //ligc 20140702,减小睡眠时间，和增加‘fail_num’重试次数，避免在拿不到数据包时睡眠时间过长导致客户端视频卡顿问题。
		}
    }

    notyfy_object_->Notify(DSTASK_QUIT,(char*)&id_,sizeof(id_));
}

