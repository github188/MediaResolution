#pragma once

#include "Poco/Mutex.h"
#include "BussinessServer.h"
#include "BussinessEvent.h"
#include "Operator.h"
#include "Global.h"
#include "NetUtility.h"

#include "DistributionManager.h"

/***********************************
 PUSH 数据到循环队列中，push的数据包的上下文存放
 到record_中，供Pop()时使用，当某队列写满需要跳到
 下一队列覆盖写时，通知上层需要覆盖的数据区间；
************************************/
bool MemoryPool::Push(bool bsplit,int type,char* head,int head_length, char *data,int data_length)
{   
    if((data == NULL)||(data_length <= 0))
    {
        return false;
    }

    //总的数据长度 = 头长度+数据长度
    int total_length = head_length+data_length;

	map<int,SequenceFlag>::iterator pos;
	mutex_.lock(); //mlcai 添加为了线程安全
    pos = recordex_.find(current_memory_block_num_);
	mutex_.unlock(); //mlcai 2014-04-22

    //判断是否覆盖;
    if(pos->second.next_position + total_length > per_queue_size_)//覆盖写
    {
        pos->second.end_index_ = max_sequence_ - 1;

        current_memory_block_num_ ++;  //队列序号；

        if(current_memory_block_num_ >= queue_num_)
        {
            current_memory_block_num_ = 0;
            is_cover_ = true;
        }

        if(is_cover_)
        {
			mutex_.lock(); //mlcai 添加为了线程安全
            pos = recordex_.find(current_memory_block_num_);
			mutex_.unlock(); //mlcai 2014-04-22

            SequenceFlag SqueFlag = pos->second;
            notyfy_object_->Notify(MEM_POOL_COVER,(char *)&SqueFlag,sizeof(SqueFlag)); //通知上层写覆盖;

            map<int,MemoryFlag>::iterator first,last; 
            mutex_.lock();
            first = record_.find(SqueFlag.start_index_);
            last  = record_.find(SqueFlag.end_index_ + 1);
			//cout << "memory " << current_memory_block_num_ << " block have " << SqueFlag.end_index_ + 1 - SqueFlag.start_index_ << " data frames " << endl;
            record_.erase(first,last); //从map中清除当前要使用的队列的 MemoryFlag;
            mutex_.unlock();

            pos->second.start_index_ = -1;
            pos->second.end_index_ = -1;
            pos->second.next_position = 0;
        }
        
        Push(bsplit,type,head,head_length,data,data_length);
    }
    else//不覆盖
    {
        if(pos->second.start_index_ == -1)
        {
            pos->second.start_index_ = max_sequence_;
        }

        MemoryFlag frameBufFlag;

        if(head!=NULL && head_length>0)
        {
            memcpy(buffer_[current_memory_block_num_]+pos->second.next_position, head,head_length);
        }

		memcpy(buffer_[current_memory_block_num_]+pos->second.next_position+head_length, data,data_length);
        frameBufFlag.type = type;

        frameBufFlag.length_ = total_length;
        frameBufFlag.used_memory_block_num_ = current_memory_block_num_;
        frameBufFlag.start_index_ =  pos->second.next_position;//current_memory_block_num_ * per_queue_size_ + pos->second.next_position;
        pos->second.next_position += total_length; //下一数据包的存放位置；

		Poco::FastMutex::ScopedLock locker(mutex_); //mlcai 添加为了线程安全2014-04-22
		record_.insert(map<int,MemoryFlag> :: value_type(max_sequence_,frameBufFlag)); 
		//判断是否是分包的第一包
		if(bsplit)
			max_iframe_sequence_=max_sequence_;
		max_sequence_++;
    }
    return true;
}



bool MemoryPool::Pop(int sequence_, char** pBuf,int& len)
{   
    map<int,MemoryFlag>::iterator pos;
    Poco::FastMutex::ScopedLock locker(mutex_); //mlcai 添加为了线程安全2014-04-22
	//mutex_.lock();
    pos = record_.find(sequence_);
    //mutex_.unlock();

	if(pos != record_.end()) 
	{
		MemoryFlag bufFlag = pos->second;  
		len = bufFlag.length_;
		*pBuf = (char*)buffer_[bufFlag.used_memory_block_num_] + bufFlag.start_index_;

		return true;
	}
	return false;
}

/******************************************
寻找I帧，如果没有找到I帧，返回当前最大帧序号，否则返回I帧包序号
******************************************/
int MemoryPool::getiFrameSequenceNum()
{   
	return max_iframe_sequence_;

    int seq = max_sequence_;
    map<int,MemoryFlag>::iterator MemPos;   
    map<int,SequenceFlag>::iterator SeqPos;

    if(!seq)
    {
        return seq;
    }
    mutex_.lock();
    SeqPos = recordex_.find(current_memory_block_num_);
    mutex_.unlock();

    for(int i=seq;i>SeqPos->second.end_index_;i--)//从当前最大序号包中倒序寻找最近的I帧;
    {   
        //mutex_.lock();
		Poco::FastMutex::ScopedLock locker(mutex_); //mlcai 添加为了线程安全2014-04-22
        MemPos = record_.find(i);
        //mutex_.unlock();
        if(MemPos != record_.end())
        {
            if(MemPos->second.type == 0)//unknown_frame)
            {
                break;
            }
            else if(MemPos->second.type == 1)//i_frame)
            {
                seq = i;
                break;
            }
        }
    }

    return seq;
}