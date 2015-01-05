#pragma once
#include <string>
#include <iostream>
#include <map>
#include "Poco/Mutex.h"

#include "INotify.h"
#include "DistributionManager.h"

using Poco::Mutex;

class MemoryFlag
{
public:
    MemoryFlag()
    {
        start_index_ = 0;
        used_memory_block_num_= 0;
    }

    ~MemoryFlag()
    {

    }
public:
    int length_;
    int type;
    int used_memory_block_num_;
    int start_index_;
};

class SequenceFlag
{
public:
    SequenceFlag()
    {
        start_index_ = -1;
        end_index_ = -1;
        next_position = 0;
    }

    ~SequenceFlag()
    {

    }

public:
    int start_index_;
    int end_index_;
    int next_position;
};

class MemoryPool
{
public:
    MemoryPool(INotify  * inotyfyObj,int queNum,int _perSize) : notyfy_object_(inotyfyObj)
    {  
        current_memory_block_num_ = 0;
        max_sequence_ = 0;
		max_iframe_sequence_ = 0;
        is_cover_ = false;

        queue_num_ = queNum;
        per_queue_size_ = _perSize;
        
        for(int i = 0;i<queue_num_;i++)
        {  
            SequenceFlag frameSeqFlag;
            buffer_.push_back(new char[per_queue_size_]);
            recordex_.insert(map<int,SequenceFlag> :: value_type(i,frameSeqFlag)); 
        }
        memset(extradata_,0,128);
        extradata_length_ = 0;    
    }
   
    ~MemoryPool()
    {
        for(int i = 0;i<queue_num_;i++)
        {  
            delete[] buffer_[i];
        }
      //  mutex_.lock();
        buffer_.clear();
        record_.clear();
        recordex_.clear();
       // mutex_.unlock();
    }

	 bool Push(bool bsplit,int type,char* head,int head_length, char *data,int data_length);
	 bool Pop(int sequence, char** pBuf,int& len);
     int  getiFrameSequenceNum();
protected:

public:
    int max_sequence_;
	//add by sunminping
	int max_iframe_sequence_;   //最大的I帧包序号,用于解决分包的问题
	//add end 
    char extradata_[256];
    int extradata_length_;
private:
	INotify  *notyfy_object_;
    map<int,MemoryFlag>record_; //数据包上下文；
    map<int,SequenceFlag>recordex_;//记录队列中起止包序号；
    int current_memory_block_num_;
    bool is_cover_;
    vector<char*> buffer_;//总共的内存空间；
    int queue_num_;//队列总数；
    int per_queue_size_;//每一个队列的内存大小；
    Poco::FastMutex   mutex_;
};