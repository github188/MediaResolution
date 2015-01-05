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
	创建一个发送数据到指定客户端的分发任务
	socket_		为与客户端进行网络通信的socket
	备注：该任务会从内存池取出数据发送到客户端，如果内存池中保存了附加数据，并且该模块
		  设置为需要将附加数据发送到客户端，那么任务会第一次将附加数据发送给客户端，然
		  后依次发送内存池里面缓存的数据
	***********************************************************************/
	bool AddClient(StreamSocket &socket_);

	/***********************************************************************
	设置分发管理器的key值
	key			要设置给分发管理器的key值
	备注：当应用层面创建了多个分发管理器的时候，当一个分发管理器没有任何分发的时候会通知
		  应用层面销毁该分发管理器，通知的时候就会将key值传递到应用层面，这时应用层需要
		  销毁分发管理器对象
	***********************************************************************/
	void SetKey(long key);
	
	/***********************************************************************
	设置该分发管理器下的分发任务是否发送附加数据
	bSend		true:分发线程发送附加数据，false:不发送附加数据
	备注：调用该函数设置成功之后，已经创建的分发任务不会生效，只有在设置成功之后创建的分发
		  任务才有效，分发管理器创建成功默认情况下，创建的分发任务是不发送附加数据到客户端
	***********************************************************************/
	void SetIsSendExtraData(bool bSend);

	//得到该分发管理器的内存池对象，方便将需要发送的数据push到内存池
	MemoryPool* GetMemoryPool();

	//用户层面上需要使用dm对象携带数据时,可使用该变量,可存放指针
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
	Poco::Timestamp create_time_;            //创建时间
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