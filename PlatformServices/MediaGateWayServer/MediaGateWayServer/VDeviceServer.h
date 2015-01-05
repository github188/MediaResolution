
#pragma once

#include  "../VirtualDevice.h"
#include <sys/timeb.h>
#include <time.h>
#include "Poco/Timestamp.h"
#include "Poco/HashMap.h"
#include "Poco/Mutex.h"
#include "INotify.h"
#include "MegaeyesAlarmOut.h"
#include "VDeviceRecordTask.h"
#include "NetUtility.h"
#include "MegaeyesHead.h"
#include "MsgHandler.h"
#include "Operator.h"
#include "VDeviceConfig.h"
#include "CmdServerModule.h"
#include "./Protocol/MegaeyesProtocolType.h"
#include "DispatchServer/DistributionManager.h"
#include "CMD10008RealReqSerialize.h"
#include "CMD10009RealResSerialize.h"
#include "CMD10014HistoryReqSerialize.h"
#include "CMD10015HistoryResSerialize.h"
#include "VDeviceRecordTask.h"
#include <string>
#include <map>
#include <set>
#include <io.h>
#include <direct.h>

using namespace std;
using namespace ty;
using ty::MsgHander;
using namespace std;
using Poco::Mutex;
using Poco::FastMutex;

struct FrameHead
{
	int frame_type_;
	Timestamp::TimeVal time_stamp_;
	int leftpack_size_;
	int seq_;
	FrameHead() :
	frame_type_(0),
		time_stamp_(0),
		leftpack_size_(0),
		seq_(0)
	{
	}
};

class VDeviceManager;
class DistributionManager;
class VDeviceRecordTask;

class VDeviceServer:public VirtualDevice::ICallBack, public INotify
{
public:
	typedef std::set<string> video_conn_set;
public:
	VDeviceServer(VDeviceManager* pManager);
    ~VDeviceServer();
	 int AddVirtualDevice(VDeviceConfig& rConfig);
	 void ReleaseVirtualDevice();
	 int InitVirtualDevice(VDeviceConfig& rConfig);
	 bool ListenAlarmInfo();

public:
	int OnRealVideoRequest(const CmdHead& head, const unsigned char* pBuffer, StreamSocket& sock);
	//处理10010指令
	int OnRealVideoStreamRequest(long lChannel, int stream_type, StreamSocket& sock);
	//处理10014指令
	int OnRecordVideoRequest(const CmdHead& head, const unsigned char* pBuffer, StreamSocket& sock);
	//处理10016指令
	int OnRecordVideoStreamRequest(const CmdHead& head, char* pBuffer, StreamSocket& sock);

	void OnGetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel);

	void OnSetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel);

	void OnSetAlarmOut(const MegaeyesRequestHead& head,CMegaeyesAlarmOut& alarmOut,int& nLength,int& nChannel,Operator &op);

	void OnGetAlarmOut(const MegaeyesRequestHead& head,CMegaeyesAlarmOut& alarmOut,int& nLength,int& nChannel,Operator &op);
	//处理5046指令
	void OnNetParam(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);

	void OnSetAlarmPlan(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel);

	void OnGetPtzStatus(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel);

	void OnPtzCtrl(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel);

	void PTZPresetControl(const int& nCmd,const int& nPreset,void* strPresetName);

	bool FastPTZControl(long lChannel,std::string strParam);

	bool FindToken(const string& token);

public:
	string Notify(int type, char* buffer, int length);

	void QueuePacket2(bool bFirst,long lSeq, long lLeave, long lFrameType, PBYTE pBuffer, DWORD dwSize, DistributionManager* dm)
	{
		long lDataLen = 0;

		lDataLen = sizeof(FrameHead) + dwSize;

		//mega header
		CmdHead hi;
		memset(&hi, 0, sizeof(hi));
		hi.length = lDataLen;
		hi.cmdnum = CMD_REALTIME_STREAMING;
		hi.cmdseq = lSeq;
		hi.success = 0; 


		FrameHead fi;
		memset(&fi, 0 ,sizeof(fi));

		fi.seq_ = lSeq;

		//如果是未知帧，则默认设定为I帧
		fi.frame_type_ = lFrameType == unknown_frame ? i_frame : lFrameType;

		fi.leftpack_size_ = lLeave;


		Timestamp stamp;
		fi.time_stamp_ = stamp.epochMicroseconds(); 

		//拷贝帧头
		char head[60] ={0};
		memcpy(head, &hi, sizeof(CmdHead));
		memcpy(head + sizeof(CmdHead), &fi, sizeof(fi));
		int head_length = sizeof(CmdHead) + sizeof(fi);
		dm->memory_pool_.Push(bFirst,0,(char *)head, head_length, (char*)pBuffer,dwSize);
	}
	
	//获取时间戳
	int gettimeofday(struct timeval* tv) 
	{
		struct timeb tm;
		ftime (&tm);
		tv->tv_sec = (long)tm.time;
		tv->tv_usec = tm.millitm * 1000;

		return (0);
	}

	//判断文件是否存在
	bool Exsit(LPCTSTR lpszPath)
	{
		return (access(lpszPath,0)!=-1);
	}

	//创建目录
	bool MakeDir(LPCTSTR lpszPath)
	{
		if(Exsit(lpszPath))
			return true;
		
		if(mkdir(lpszPath))
			return true;

		return false;
	}

	//递归创建目录
	bool MakeDirEx(LPCTSTR lpszPath)
	{
		char strPath[MAX_PATH]={0},val;
		strcpy(strPath,lpszPath);
		char* start=strPath,*end=NULL;
		do
		{
			end=strchr(start,'\\');
			if(end==NULL) return true;
			val=*end;
			*end=0;
			if (!MakeDir(strPath))
				return FALSE;
			*end=val;
			start=end+1;
		} while (NULL!=end);
		return TRUE;
	}

private:
	void SendResponse(const MegaeyesRequestHead& head,CMegaeyesResponseProtocol& response,Operator &op)
	{
        std::string strMsg=response.Serialize();
		CMegaeyesResponseCmdHead& cmdHead=response.m_RespinseCmdHead;

		unsigned char* buffer = new unsigned char[CMegaeyesResponseCmdHead::Length + strMsg.size()];
		
		memcpy(buffer,&cmdHead,CMegaeyesResponseCmdHead::Length);
        memcpy(buffer+CMegaeyesResponseCmdHead::Length,strMsg.c_str(),strMsg.length());
        int nlen=CMegaeyesResponseCmdHead::Length + strMsg.length();
        if(nlen!=NetUtility::SafeSendData(op.hand->_socket,(char *)buffer,nlen))
        {
            //g_log<<"Send response failed"<<response.m_RespinseCmdHead.m_nCmdNum<<"\n";
        }

		delete[] buffer;
	}
 public:
      map<long,DistributionManager*> videoSourceRecord;
      Poco::FastMutex m_Mutex;
      VirtualDevice* m_pPlatform;


	  Poco::Mutex m_RecdTaskMutex;
	  map<string,VDeviceRecordTask*> VPRecordTasks;

	

private:
	VDeviceManager* pVPManager;

	VDeviceConfig m_rConfig;

	Poco::FastMutex token_mtx_;
	std::set<string> tokens_;


private:
	//流数据通知(包括音视频等)
	void NotifyStreamData(long lChannel, long lStreamType, long lFrameType, PBYTE pBuffer, DWORD dwSize, void* dm);
	bool IsExistsIp(const string& ip);
    long GetChannelPlayId(long channelId,bool isMainStream)
    {
        if(!isMainStream)
        {
            return channelId*1000+1;
        }

        return 	channelId *1000;
    }

    //告警通知
	bool NotifyAlarm(long lChannel, long lType, LPCTSTR szAlarmMessage = 0);
    void NotifyMessage(_RESULT lCode,LPCTSTR szMessage);

	FastMutex m_videoMutex;
	video_conn_set _video_conn_set;
};
