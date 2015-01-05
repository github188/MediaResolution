#pragma once

#include "Poco/Mutex.h"
#include "DistributionManager.h"

/***********************************
 PUSH ���ݵ�ѭ�������У�push�����ݰ��������Ĵ��
 ��record_�У���Pop()ʱʹ�ã���ĳ����д����Ҫ����
 ��һ���и���дʱ��֪ͨ�ϲ���Ҫ���ǵ��������䣻
************************************/

MemoryPool::MemoryPool(INotify  * inotyfyObj,int queNum,int _perSize) : notyfy_object_(inotyfyObj)
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
	extradata_=NULL;
	extradata_length_ = 0;    
}

MemoryPool::~MemoryPool()
{
	for(int i = 0;i<queue_num_;i++)
	{  
		delete[] buffer_[i];
	}
	buffer_.clear();
	record_.clear();
	recordex_.clear();

	delete[] extradata_;
	extradata_length_=0;
}

bool MemoryPool::SetExtraData(char* pExtraData,int len)            //���ø�������
{
	if(pExtraData==NULL) return false;
	if(extradata_) delete[] extradata_;
	extradata_=new char[len];
	if(extradata_==NULL) return false; 
	memcpy(extradata_,pExtraData,len);
	extradata_length_=len;
	return true;
}

char* MemoryPool::GetExtraData()                                   //��ȡ��������
{
	return extradata_;
}

int MemoryPool::GetExtraDataLength()                               //��ȡ�������ݵĳ���
{
	return extradata_length_;
}

bool MemoryPool::Push(bool bsplit,int type,char* head,int head_length, char *data,int data_length)
{   
    if((data == NULL)||(data_length <= 0))
        return false;

    //�ܵ����ݳ��� = ͷ����+���ݳ���
    int total_length = head_length+data_length;

	map<int,SequenceFlag>::iterator pos;
	mutex_.lock(); //mlcai ���Ϊ���̰߳�ȫ
    pos = recordex_.find(current_memory_block_num_);
	mutex_.unlock(); //mlcai 2014-04-22

    //�ж��Ƿ񸲸�;
    if(pos->second.next_position + total_length > per_queue_size_)//����д
    {
        pos->second.end_index_ = max_sequence_ - 1;

        current_memory_block_num_ ++;  //������ţ�

        if(current_memory_block_num_ >= queue_num_)
        {
            current_memory_block_num_ = 0;
            is_cover_ = true;
        }

        if(is_cover_)
        {
			mutex_.lock(); //mlcai ���Ϊ���̰߳�ȫ
            pos = recordex_.find(current_memory_block_num_);
			mutex_.unlock(); //mlcai 2014-04-22

            SequenceFlag SqueFlag = pos->second;
            notyfy_object_->Notify(MEM_POOL_COVER,(char *)&SqueFlag,sizeof(SqueFlag)); //֪ͨ�ϲ�д����;

            map<int,MemoryFlag>::iterator first,last; 
            mutex_.lock();
            first = record_.find(SqueFlag.start_index_);
            last  = record_.find(SqueFlag.end_index_ + 1);
			record_.erase(first,last); //��map�������ǰҪʹ�õĶ��е� MemoryFlag;
            mutex_.unlock();

            pos->second.start_index_ = -1;
            pos->second.end_index_ = -1;
            pos->second.next_position = 0;
        }
        Push(bsplit,type,head,head_length,data,data_length);
    }
    else//������
    {
        if(pos->second.start_index_ == -1)
            pos->second.start_index_ = max_sequence_;

        MemoryFlag frameBufFlag;

        if(head!=NULL && head_length>0)
            memcpy(buffer_[current_memory_block_num_]+pos->second.next_position, head,head_length);

		memcpy(buffer_[current_memory_block_num_]+pos->second.next_position+head_length, data,data_length);
        frameBufFlag.type = type;

        frameBufFlag.length_ = total_length;
        frameBufFlag.used_memory_block_num_ = current_memory_block_num_;
        frameBufFlag.start_index_ =  pos->second.next_position;
        pos->second.next_position += total_length; //��һ���ݰ��Ĵ��λ�ã�

		Poco::FastMutex::ScopedLock locker(mutex_); //mlcai ���Ϊ���̰߳�ȫ2014-04-22
		record_.insert(map<int,MemoryFlag> :: value_type(max_sequence_,frameBufFlag)); 
		
		//�ж��Ƿ��Ƿְ��ĵ�һ��
		if(bsplit)
			max_iframe_sequence_=max_sequence_;

		max_sequence_++;
    }
    return true;
}



bool MemoryPool::Pop(int sequence_, char** pBuf,int& len)
{   
    map<int,MemoryFlag>::iterator pos;
    Poco::FastMutex::ScopedLock locker(mutex_); //mlcai ���Ϊ���̰߳�ȫ2014-04-22

    pos = record_.find(sequence_);

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
Ѱ��I֡�����û���ҵ�I֡�����ص�ǰ���֡��ţ����򷵻�I֡�����
******************************************/
int MemoryPool::getiFrameSequenceNum()
{   
	return max_iframe_sequence_;
}