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
	������Push���ڴ��
	bsplit			true:��ʾdata��һ��I֡���ݵĿ�ʼ�����඼Ϊfalse
	type			1:��ʾpush����������ΪI֡����,0:��I֡����
	head			��ͻ�������ͨ��Э��
	head_length     Э������ݳ���
	data			ͨ������
	data_length     ���ݳ���
	��ע��������͵�������һ������I֡û�зְ�,��ôbsplitΪtrue��������͵�������һ��I֡���ݷְ��ĵ�һ����
	      bsplitΪtrue,����������ݼ�ʹ��I֡�ķְ��������ǵ�һ������Ϊfalse�����ڷ�I֡���ݣ�Ϊfalse
	********************************************************************************************/
	bool Push(bool bsplit,int type,char* head,int head_length, char *data,int data_length);


	/******************************************************************************************
	���ڴ��Pop����
	sequence		��Ҫpop�������ݵ����
	pBuf            pop�����ݵĵ�ַ
	len	            pop�����ݵĳ���
	*******************************************************************************************/
	bool Pop(int sequence, char** pBuf,int& len);

	/******************************************************************************************
	���ø�������
	pExtraData		�������ݵ�ͷָ��
	len				�������ݵĳ���
	��ע:����ε��øýӿ����ø�������֮�󣬱���ĸ�������Ϊ���һ�����õĸ������ݣ�
		 ����ͨ��GetExtraData()��GetExtraDataLength()�ӿ�ȡ���������ݺ͸������ݳ���
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
	int max_iframe_sequence_;   //����I֡�����,���ڽ���ְ�������
	char* extradata_;
	int extradata_length_;
	INotify  *notyfy_object_;
	map<int,MemoryFlag>record_; //���ݰ������ģ�
	map<int,SequenceFlag>recordex_;//��¼��������ֹ����ţ�
	int current_memory_block_num_;
	bool is_cover_;
	vector<char*> buffer_;//�ܹ����ڴ�ռ䣻
	int queue_num_;//����������
	int per_queue_size_;//ÿһ�����е��ڴ��С��
	Poco::FastMutex   mutex_;

};