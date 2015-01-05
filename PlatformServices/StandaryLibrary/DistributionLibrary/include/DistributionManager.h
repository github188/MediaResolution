#pragma once
#include <string>
#include <map>
#include <list>
#include "INotify.h"
#include "MemoryPool.h"
#include "DistributionTask.h"
#include "DistributionLibrary.h"
typedef enum
{   
    COLSE_CONNECT     = 40,
    MEM_POOL_COVER    = 60,
    NET_CONNECT_ERROR = 80,
    DSTASK_QUIT       = 81,
	DESTROY = 82,
}emNOTIFY;

class DistributionLibrary_API DistributionManager:public INotify
{
public:
	DistributionManager(INotify  *notify_obj, int queue_num_ =3,int _perSize = 1024*1024*1);
	~DistributionManager();
	
	/***********************************************************************
	����һ���������ݵ�ָ���ͻ��˵ķַ�����
	socket_		Ϊ��ͻ��˽�������ͨ�ŵ�socket
	��ע�����������ڴ��ȡ�����ݷ��͵��ͻ��ˣ�����ڴ���б����˸������ݣ����Ҹ�ģ��
		  ����Ϊ��Ҫ���������ݷ��͵��ͻ��ˣ���ô������һ�ν��������ݷ��͸��ͻ��ˣ�Ȼ
		  �����η����ڴ�����滺�������
	***********************************************************************/
	bool AddClient(StreamSocket &socket_);

	/***********************************************************************
	���÷ַ���������keyֵ
	key			Ҫ���ø��ַ���������keyֵ
	��ע����Ӧ�ò��洴���˶���ַ���������ʱ�򣬵�һ���ַ�������û���κηַ���ʱ���֪ͨ
		  Ӧ�ò������ٸ÷ַ���������֪ͨ��ʱ��ͻὫkeyֵ���ݵ�Ӧ�ò��棬��ʱӦ�ò���Ҫ
		  ���ٷַ�����������
	***********************************************************************/
	void SetKey(long key);
	
	/***********************************************************************
	���ø÷ַ��������µķַ������Ƿ��͸�������
	bSend		true:�ַ��̷߳��͸������ݣ�false:�����͸�������
	��ע�����øú������óɹ�֮���Ѿ������ķַ����񲻻���Ч��ֻ�������óɹ�֮�󴴽��ķַ�
		  �������Ч���ַ������������ɹ�Ĭ������£������ķַ������ǲ����͸������ݵ��ͻ���
	***********************************************************************/
	void SetIsSendExtraData(bool bSend);

	//�õ��÷ַ����������ڴ�ض��󣬷��㽫��Ҫ���͵�����push���ڴ��
	MemoryPool* GetMemoryPool();

	//�û���������Ҫʹ��dm����Я������ʱ,��ʹ�øñ���,�ɴ��ָ��
	long dwUser;

protected:
	string Notify(int type, char* frame, int length_);

private:
	void stop();

private:
    int task_id_;
    MemoryPool memory_pool_;
    map<int,DistributionTask*> task_record_;
    Poco::FastMutex   mutex_;
    bool task_need_extradata_;
	INotify  *notify_object_;
	long key_;					
	Poco::Timestamp create_time_;            //����ʱ��
	bool has_client_;
	
private:
	int dm_key_;

private:
	static std::map<int,DistributionManager*> dm_map_;
	static Poco::Mutex dm_mutex_;
	static int dm_ind_;
	static bool dm_run_;
	static Thread scan_thr_;

	class ScanTask:public Poco::Runnable
	{
	private:
		void run();
	};

};