#ifndef CMDHEAD_H
#define CMDHEAD_H

#include "define.h"

#include "Poco/Mutex.h"

using namespace Poco;

typedef struct CmdHead
{ 
	int version;
	int length;
	int cmdnum;
	int cmdseq;
	int exten;
	int success;
	char src[20];
	char dst[20];
	CmdHead():version(2056),length(0),cmdnum(0),cmdseq(0),
		exten(0),success(0)
	{
		memset(src, 0, sizeof(src));
		memset(dst, 0, sizeof(dst));
	}
} CmdHead;

class SeqNumGenerator
{
public:
	~SeqNumGenerator() {}

	static SeqNumGenerator& Instance();

	int Generate();

private:
	SeqNumGenerator() : num_(0) {}
	static SeqNumGenerator instance_;
	FastMutex num_mtx_;
	int num_;
};

struct CMegaeyesQequestCmdHead
{
	enum _Msg_Head
	{
		Length=80,
	};
	enum _MsgNum
	{
		MsgRegist=2002,
		MsgRealVideo = 5010,
		MsgRealVideoStream = 8000,

		MsgQueryHistoryVideo=5004,
		MsgRecordStream = 8300,

		MsgRealVideo2=6002,
		MsgStoreRegist=4002,
		MsgErrorNotify=1100,
		MsgAlarmNotify=5214,
		MsgTranslatorNotifyStaus=40002,
		MsgTranslatorHeart=40000,
		MsgSetVidePara = 5032,
		MsgGetVidePara = 5034,
		MsgSetAlarmOut = 5064,
		MsgGetAlarmOut = 5066,
		MsgNetParam = 5046,
		MsgSetVideDetectPlan = 5080,
		MsgSetAlarmPlan =5084,
		MsgGetPtzStatus = 5042,
		MsgPtzCtrl = 5006,

	};
	CMegaeyesQequestCmdHead()
	{
		Clear();
	}
	void Clear()
	{
		m_nVersion=257;
		m_nLength=0;
		m_nCmdNum=0;
		m_nCmdSeq=0;
		memset(m_cSrcID,0,sizeof(m_cSrcID));
		memset(m_cDesID,0,sizeof(m_cDesID));
	}
	const	CMegaeyesQequestCmdHead& operator=(const CMegaeyesQequestCmdHead& rParam)
	{
		if (this==&rParam)
		{
			return *this;
		}
		m_nVersion=rParam.m_nVersion;
		m_nLength=rParam.m_nLength;
		m_nCmdSeq=rParam.m_nCmdSeq;
		m_nCmdNum=rParam.m_nCmdNum;
		memcpy(m_cSrcID,rParam.m_cSrcID,sizeof(m_cSrcID));
		memcpy(m_cDesID,rParam.m_cDesID,sizeof(m_cDesID));
		return *this;
	}
public:

	int m_nVersion;
	int m_nLength;
	int m_nCmdNum;
	int m_nCmdSeq;
	char m_cSrcID[32];
	char m_cDesID[32];
};

class CMegaeyesResponseCmdHead
{
public:
	enum _Msg_Head
	{
		Length=20,
	};
	CMegaeyesResponseCmdHead()
	{
		Clear();
	}
	void Clear()
	{
		m_nVersion=257;
		m_nLength=0;
		m_nCmdNum=0;
		m_nCmdSeq=0;
		m_nSuccess=0;
	}
	const CMegaeyesResponseCmdHead operator=(const CMegaeyesResponseCmdHead& rParam)
	{
		if (this==&rParam)
		{
			return *this;
		}
		m_nVersion=rParam.m_nVersion;
		m_nLength=rParam.m_nLength;
		m_nCmdNum=rParam.m_nCmdNum;
		m_nCmdSeq=rParam.m_nCmdSeq;
		m_nSuccess=rParam.m_nSuccess;
		return *this;
	}
public:

	int m_nVersion;
	int m_nLength;
	int m_nCmdNum;
	int m_nCmdSeq;
	int m_nSuccess;
};

#define CMD_REGTERM 10004
#define CMD_TERMABILITY 10006
#define CMD_DEVREALTIME 10008	//向请求实时视频
#define CMD_DEVREALTIME_RESP 10009
#define CMD_DEVREALSTREAM 10010
#define CMD_DEVREALSTREAM_RESP 10011
#define CMD_REALTIME_STREAMING 10012 //实时视频流
#define CMD_DEVHISTROY 10014 //请求历史视频
#define CMD_DEVHISTROY_RESP 10015
#define CMD_DEVRECORDSTREAM 10016
#define CMD_DEVRECORDSTREAM_RESP 10017
#define CMD_HISTORY_STREAMING 10018 //历史视频流
#define CMD_HISTORY_STREAMING_END 10019 //历史视频流结束标志

#endif