#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include "Poco/Mutex.h"
#include "INotify.h"
#include "DistributionManager.h"
#include "DistributionLibrary.h"
#include "DistributionLibrary.h"

using Poco::Mutex;

class DistributionLibrary_API MemoryPool
{
	friend class DistributionManager;
	friend class DistributionTask;
public:

	/*******************************************************************************************
	将数据Push到内存池
	bsplit			true:表示data是一个I帧数据的开始，其余都为false
	type			1:表示push的数据类型为I帧数据,0:非I帧数据
	head			与客户端网络通信协议
	head_length     协议的数据长度
	data			通信数据
	data_length     数据长度
	备注：如果发送的数据是一个完整I帧没有分包,那么bsplit为true；如果发送的数据是一个I帧数据分包的第一包，
	      bsplit为true,否则这个数据即使是I帧的分包，但不是第一包，都为false，对于非I帧数据，为false
	********************************************************************************************/
	bool Push(bool bsplit,int type,char* head,int head_length, char *data,int data_length);


	/******************************************************************************************
	从内存池Pop数据
	sequence		需要pop出的数据的序号
	pBuf            pop出数据的地址
	len	            pop出数据的长度
	*******************************************************************************************/
	bool Pop(int sequence, char** pBuf,int& len);

	/******************************************************************************************
	设置附加数据
	pExtraData		附加数据的头指针
	len				附加数据的长度
	备注:当多次调用该接口设置附加数据之后，保存的附加数据为最后一次设置的附加数据，
		 可以通过GetExtraData()和GetExtraDataLength()接口取出附加数据和附加数据长度
	*******************************************************************************************/
	bool SetExtraData(char* pExtraData,int len);            
	char* GetExtraData();
	int GetExtraDataLength();
private:
	int  getiFrameSequenceNum();
	MemoryPool(INotify  * inotyfyObj,int queNum,int _perSize);
	~MemoryPool();

private:
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

private:
	unsigned int max_sequence_;
	int max_iframe_sequence_;   //最大的I帧包序号,用于解决分包的问题
	char* extradata_;
	int extradata_length_;
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