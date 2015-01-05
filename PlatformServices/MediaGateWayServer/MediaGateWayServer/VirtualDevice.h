#pragma once


#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include "winbase.h"
#include "Poco/LocalDateTime.h"
#include "LogSys.h"
#include "VDeviceConfig.h"
using namespace std;
using namespace stdext;

//////////////////////////////////////////////////////////////////////////
// 
// ���������豸�ӿ�
//
//////////////////////////////////////////////////////////////////////////

class Rect;
class Time;
class Record;

#pragma pack(push)
#pragma pack(1)
typedef struct _RESULT2
{
	unsigned short nCode;		// ��Ϣֵ
	unsigned short nSource:13;	// ��Ϣ��Դ(��ӦnFacility)
	unsigned short bUser: 1;	// �Ƿ�Ϊ�û��Զ������Ϣ��
	unsigned short nGrade:2;	//  0��ʾ�ɹ���ָ���������Ѿ��������
	//  1��ʾ֪ͨ��ָ����������δ�������
	//  2��ʾ���棨�������й����г�����һЩ���⣬�������Լ������еó������
	//  3��ʾ���������Ѿ������˳�����Ϊ�����й����г������������⣬�޷����ָ��������
	//�˴��Ķ�����HRESULT�Ķ�����һ�µģ�
	//  Severity  Definition  
	//  00        Success  
	//  01        Informational  
	//  10        Warning  
	//  11        Error   
	_RESULT2()
	{
		nGrade = 0;
		bUser = 0;
		nSource = 0;
		nCode = 0;
	}
	_RESULT2( unsigned int _nGrade, unsigned int _nSource, long _nCode, bool _bUser = true )
	{
		nGrade = _nGrade;
		bUser = _bUser;
		nSource = _nSource;
		nCode = ( unsigned short )_nCode;
	}
	_RESULT2( HRESULT _hr )
	{
		*(long*)(this) = (long)_hr;
	}
	operator HRESULT()
	{
		return *((HRESULT*)this);
	}
} _RESULT;
#pragma pack(pop)

class VDeviceInfo
{
public:
	string deviceID;		//�豸ID
    string deviceType;      //�豸���ͣ������������ӿƼ�NVR��¼����չ��
	string serverIP;		//IP��ַ
	long port;				//�˿�
	string user;			//�û���
	string passwd;			//����
	string extendPara1;  //��չ
	LogSys* vp_log;         //vp��־        

	VDeviceInfo()
	{
		port = 0;
	}

	VDeviceInfo(const VDeviceInfo& info)
	{
		Copy(info);
	}

	VDeviceInfo& operator =(const VDeviceInfo& info)
	{
		Copy(info);
		return *this;
	}

private:
	void Copy(const VDeviceInfo& info)
	{
		deviceID = info.deviceID;
		serverIP = info.serverIP;
		port = info.port;
		user = info.user;
		passwd = info.passwd;
		extendPara1 =info.extendPara1;
		vp_log = info.vp_log;
		deviceType = info.deviceType;
	}
};

//////////////////////////////////////////////////////////////////////////
//
// ÿ��DVR�ж��ͨ����ÿ��ͨ������֧�ֶ�·
// ��������Ƶ�������У�ֻ��Ҫ����ÿ��ͨ��1·���Ӽ��ɣ����ϲ���ж�·�ַ�
//
//////////////////////////////////////////////////////////////////////////

//���������SDK�������豸
class VirtualDevice
{
public:
	class ICallBack
	{
	public:
		//������֪ͨ(��������Ƶ��)
		//lStreamType�����������ͣ���Ƶͷ����Ƶ��������������¼���ļ�ͷ��¼���ļ�����
		//lFrameType��֡���ͣ�IBP֡�ȣ����������������޷���ȡ������ȫ����Ϊunknown_frame
		virtual void NotifyStreamData(long lChannel,long lStreamType,long lFrameType,PBYTE pBuffer,DWORD dwSize, void* dm) = 0;

		//�澯֪ͨ, szAlarmMessage���������п��ܳ��ֳ���������չ��Ϣ
		virtual bool NotifyAlarm(long lChannel,long lType,LPCTSTR szAlarmMessage = NULL) = 0;

		//���ش�����Ϣ֪ͨ
		virtual void NotifyMessage(_RESULT lCode,LPCTSTR szMessage) = 0;

		//add by sunmiping 
		virtual bool IsExistsIp(const string& ip)=0;
		//end add
	};

public:
    VirtualDevice():m_pCallBack(NULL){}

	virtual ~VirtualDevice(){};

	void BaseInit(ICallBack* pCallBack,VDeviceInfo config)
	{
		m_pCallBack = pCallBack;
        m_Config = config;
	}

	void Notify(_RESULT lCode, const char* lpszFormat)
	{
		m_pCallBack->NotifyMessage(lCode, lpszFormat);
	}

	void Notify(const char* lpszFormat,...)
	{
		if (m_pCallBack)
		{
			va_list args;
			va_start(args, lpszFormat);
			char buf[4096] = {0};
			vsnprintf_s(buf ,4096, 4096, lpszFormat, args);
			m_pCallBack->NotifyMessage(0, buf);
			va_end(args);			
		}
	}

	//add by sunminping  2014-10-10
protected:
	string _key;
	//end add 

public:
	//////////////////////////////////////////////////////////////////////////
	//����ʵ�ֵĽӿ�

	//��ʼ��
	virtual int Init(VDeviceConfig& dev_config) = 0;
	virtual bool Destroy() = 0;

	//ʵʱ��Ƶ
	virtual bool PlayVideo(long lChannel,void* dm, bool bMainStrean = 0) = 0;
	virtual bool StopVideo(long lChannel,bool bMainStrean = 0) = 0;

	virtual void GetVersion(string& version, string& build_time)
	{
		version = "";
		build_time = "";
	}

	//��ѯ¼��
	//rd: �ο�Record˵��
	virtual bool QueryRecord(long lChannel, const LocalDateTime& tStart, const LocalDateTime& tEnd,vector<Record>& rds)
	{
		return false;
	}

	//����¼��
	//lRequestType: �������ͣ�0��ʾֹͣ���أ�1��ʾ���أ�2��ȡ����״̬
	//lContext: ���������ľ��
	//lData: ������Ϊ2ʱ��ʾ����״̬�����ౣ��
	//pos�������ص�λ�� mlcai 2014-05-07
	virtual bool RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext, long& lData, int& pos)
	{
		return false;
	}

	//��ȡ��Ƶͷ
	//���DVR¼���ļ��������¼��ͷ����ֻ�践��ͷ����С
	//��������lSizeΪ0��ͬʱ����sHeaderΪ��Ӧ¼��Э��ͷ�������Ϊ"DHPT"
	virtual bool GetRecordHeader(long lChannel, long& lSize, string& sHeader)
	{
		return false;
	}

	//��ȡ�����������
	virtual bool GetNetParam(string& sIPAddr, string& sNetMask, string& sGateway)
	{
		return false;
	}

	//��̨����
	/*
	lCommand��������,�������£�
	0 #ֹͣ 
	1  #��̨��          2 #��̨�� 
	3  #��̨��          4 #��̨��
	5  #��Ȧ+           6 #��Ȧ- 
	7  #�䱶+           8 #�䱶- 
	9  #�۽�+          10 #�۽�-
	11 #��̨�Զ�ɨ�迪 12 #��̨�Զ�ɨ���
	13 #�ƹ⿪         14 #�ƹ��
	15 #��ˢ��         16 #��ˢ��
	17 #̽ͷ��Դ��     18 #̽ͷ��Դ�� 
	19 #��ͷ��Դ��     20 #��ͷ��Դ��

	21 #���ⲹ����			22 #���ⲹ����
	23 #�Զ�ҹ���л���		24 #�Զ�ҹ���л���
	25 #�ֹ�ת��Ϊҹ����ʾ	26 #�ֹ�ת��Ϊ��ɫ��ʾ
	27 #����λ���ܿ�		28 #����λ���ܹ�
	29 #�Զ���Ȧ��			30 #�Զ���Ȧ��
	31 #�Զ��۽���			32 #�Զ��۽���

	101 #180�ȷ�ת			102 #�ָ�����ĳ�����������

	1001 #����Ԥ�õ�	   1002 #���Ԥ�õ�
	1003 #ת��Ԥ�õ�       1004 #��ά��λ(x,y,z)

	lValue����������Ϊ(1-12,101,1003)ʱ������������ٶȣ���ֵΪ(1~255)
			��������Ϊ(1001-1003)ʱ������Ԥ�õ����������lCommand=1002��ֵΪ-1ʱ�������������Ԥ�õ�

	pParam����չ������Ĭ��ΪNULL��
			������Ϊ����Ԥ�õ�ʱ�ò�������Ԥ�õ����� ( char* sName = (char*)pParam )
			������Ϊ����Ԥ�õ�ʱ�ò��������ٶ�( long lSpeed = atoi((char*)pParam) )
	*/
	virtual bool PTZControl(long lChannel,long lCommand, long lValue, void* pParam = NULL) = 0;


	////////////////////////////////////////////////////////////////////////////
	////��ѡ�ӿ�


	//����ʱ��OSD
	//virtual bool SetTime(Time& t);
	//����OSD
	//virtual bool SetOSD(string& sOSD) = 0;


	//���������ϴ�ͨ��(����)
	virtual bool SetupAlarmChannel()
	{
		return false;
	}

	//�رձ����ϴ�ͨ����������
	virtual void CloseAlarmChannel()
	{
		
	}

	//����״̬
	//lType:  0����ͷ��1�������룬2�������
	//lState: ״̬ 0Ϊ�� 1Ϊ��;
	virtual bool SetAlarmState(long lChannel, long lType, long lState)
	{
		return false;
	}
	virtual bool GetAlarmState(long lChannel, long lType, long& lState)
	{
		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	////ͼ����Ƶ����
	////���ĳһ��ֵ�� ��1����ʾҪ����Ĭ��ֵ��

	//OSD
	//bShowOsd��Ϊ1��0�ֱ�Ϊ�򿪺͹ر�ʱ���Logo���á�
	//nOSDPlace����ʾʱ�����ʾλ�á�1��8�ֱ�Ϊ���ϣ����ϣ����ϣ��������ң����£����£����°˸�λ�á�
	//nWordPlace����ʾLogo����ʾλ�á�Ҳ��1��8��
	//sWord��ΪLogo �����֡�
	//Ĭ����ʾʱ���Logo��ʱ��������ʾ��Logo������ʾ������Ϊ����Ƶλ�á���
	virtual bool SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,string& sWord)
	{
		return false;
	}
	virtual bool GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,string& sWord)
	{
		return false;
	}

	//֡��
	//FrameRate����Χ3��25��KeyFrameIntervals���ķ�Χ10��100��Ĭ��ֵ�Զ��塣
	virtual bool SetFrameRate(long lChannel,long nFrameRate, long nKeyFrameIntervals,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetFrameRate(long lChannel,long& nFrameRate, long& nKeyFrameIntervals,bool bMainStream=true)
	{
		return false;
	}

	//ͼ������
	//Level��Ϊ�ȼ�����Χ����á��Ϻá��á�һ�㡣�ֱ��� 0,1,2,3���档Ĭ��ֵ�Զ��塣
	virtual bool SetPictureQuality(long lChannel,long nLevel,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetPictureQuality(long lChannel,long& nLevel,bool bMainStream=true)
	{
		return false;
	}

	//�������
	//nMaxBPS��Ϊ����ֵ����λ��1000bit/sec��Χ������50�� vbr���Ƿ�Ϊ������
	virtual bool SetMaxBPS(long lChannel,long nMaxBPS, bool vbr,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetMaxBPS(long lChannel,long& nMaxBPS, bool& vbr,bool bMainStream=true)
	{
		return false;
	}

	//ͼ���ʽ
	//PicFormat��CIF��QCIF��2CIF��4CIF/D1���ֱ���0,1,2,3���档
	//SupportFormat��Ϊ֧�ֵ���Ƶ��ʽ����֧��CIF��QCIF��2CIFΪ012����֧��QCIF��2CIFΪ13��Ĭ��ֵ�Զ��塣
	virtual bool SetPictureFormat(long lChannel,long nPicFormat, string& sSupportFormat,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetPictureFormat(long lChannel,long& nPicFormat, string& sSupportFormat,bool bMainStream=true)
	{
		return false;
	}

	//��Ƶ����
	//�Աȶȡ�ɫ�ȡ����ȡ����ͶȵĴ�С��Χ����0-255��
	virtual bool SetVideoPara(long lChannel,long nBrightness, long nContrast, long nSaturation /*���Ͷ�*/, 
		long nHue/*ɫ��*/)
	{
		return false;
	}

	virtual bool GetVideoPara(long lChannel,long& nBrightness, long& nContrast, long& nSaturation /*���Ͷ�*/, 
		long& nHue/*ɫ��*/)
	{
		return false;
	}

	////////////////////////////////////////////////////////////////////////
	//Rect(352*288) �ϲ㱣֤������ȷ���²㲻���жϣ�����²��޷���ȡ��ȷ����Ӧ����false�����ϲ㴦��

	//�������
	//bDectecting��Ϊ1��0�ֱ�Ϊ�򿪺͹ر��ƶ���⡣DetectGrade��Ϊ���������ȡ���СΪ0-6��0��ٶۣ�6��������
	//Rect��Ϊ�������Left��Top��Width��Height��С��352��288֮�䣬�����Ƶ�ֱ��ʲ�����352��288����Ƶ������Ӧ�Զ��������������š�
	//Ĭ�������û�������ƶ���⡣

	virtual bool SetDetectRect(long lChannel,vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade,long nMaxRectNum)
	{
		return false;
	}
	virtual bool GetDetectRect(long lChannel,vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade,long& nMaxRectNum)
	{
		return false;
	}

	//�ڵ�����
	virtual bool SetShadeRect(long lChannel,vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum)
	{
		return false;
	}
	virtual bool GetShadeRect(long lChannel,vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum)
	{
		return false;
	}

	////�����豸
	//virtual bool Reboot()
	//{
	//	return false;
	//}

	////
	////����

	////��ȡ����ƽ̨���õ���Ƶ����
	////lAudioChannels: ����ͨ����
	////lAudioBit: �������(λ��)
	////lSampleRate: ��������
	////sFormat: ѹ����ʽ���ο�HX_AUDIONAME_G722HK�ȶ���
	//virtual bool GetAudioPara(long& lAudioChannels, long& lAudioBit, long& lSampleRate, string& sFormat)
	//{
	//	return false;
	//}

	////��ʼ��������
	//virtual bool StartAudioCall()
	//{
	//	return false;
	//}

	////ֹͣ��������
	//virtual bool StopAudioCall()
	//{
	//	return false;
	//}

	////ת����Ƶ���ݵ��豸
	//virtual bool SendAudioData(void * pBuffer, long lSize)
	//{
	//	return false;
	//}

	////��ȡЭ������
	//virtual bool GetProtocol(string& sProtocol)
	//{
	//	return false;
	//}

	// ��ȡ��ǰԤ�õ��б�
	virtual bool GetPresetList(long lChannel, string& sRet)
	{
		return false;
	}

public:
	//��ö�Ӧ��playId
	long GetChannelPlayId(long channelId,bool isMainStream)
	{
		if(!isMainStream)
		{
			return channelId*1000+1;
		}

		return 	channelId *1000;
	}

	public:
		ICallBack * m_pCallBack;
		//����ƽ̨����
		VDeviceInfo m_Config;
};


//ʵʱ��Ƶͨ����¼��Ϣ������ʵʱ��Ƶ�ص�ʹ��
class VPlatformChannel
{
public:
	VPlatformChannel()
	{
		is_send_videoheader_ = false;
		play_handle_ = -1;
		channelno_ = -1;
		vplatform_ = NULL;
		distribution_manager_ = NULL;    
        memset(play_handle_str_,0, sizeof(play_handle_str_));
	}

public:
	bool  is_send_videoheader_;
	long play_handle_;	//���ž��(һ����˵��������ֵһ�£�Ϊ�˱��ⲻһ�µ�������ֿ�����)
	int channelno_;
	bool is_mainstream_;
	VirtualDevice* vplatform_;
	void* distribution_manager_;
    char  play_handle_str_[128]; //���ӿƼ�nvr�Ĳ��ž��

};

class Rect
{
public:
	long Left;
	long Top;
	long Right;
	long Bottom;
public:
	Rect()
	{
		Left = 0;
		Top = 0;
		Right = 0;
		Bottom = 0;
	}
	Rect(long l,long t,long r,long b)
	{
		Left = l;
		Top = t;
		Right = r;
		Bottom = b;
	}
	Rect(const Rect& rc)
	{
		Left = rc.Left;
		Top = rc.Top;
		Right = rc.Right;
		Bottom = rc.Bottom;
	}
	long Width()
	{
		return Right - Left;
	}
	long Height()
	{
		return Bottom - Top;
	}


	void AjustCIF()
	{
		AjustLess(Left, 0);
		AjustLess(Top, 0);
		AjustGreater(Right, 352);
		AjustGreater(Bottom, 288);
	}

	Rect& operator =(const Rect& rc)
	{
		Left = rc.Left;
		Top = rc.Top;
		Right = rc.Right;
		Bottom = rc.Bottom;

		return *this;
	}

private:
	void AjustLess(long& lValue,long lValueCmp)
	{
		if(lValue < lValueCmp)
			lValue = lValueCmp;
	}
	void AjustGreater(long& lValue,long lValueCmp)
	{
		if(lValue > lValueCmp)
			lValue = lValueCmp;
	}
};

class Time
{
public:
	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;
	long millsecond;
public:
	Time()
	{
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
		millsecond = 0;
	}
	//Type = 0, 2009-08-10 12:30:00
	//Type = 1, 20090810
	//Type = 2, 071210072011.19
	Time(std::string& sTime, long lType = 0)
	{
		SetTime(sTime, lType);
	}

	Time(const char * sTime, long lType = 0)
	{
		std::string time(sTime);
		SetTime(time, lType);
	}

	Time(SYSTEMTIME& st)
	{
		Copy(st);
	}

	void Copy(SYSTEMTIME& st)
	{
		year = st.wYear;
		month = st.wMonth;
		day = st.wDay;
		hour = st.wHour;
		minute = st.wMinute;
		second = st.wSecond;
		millsecond = st.wMilliseconds;
	}

	void SetTime(std::string& sTime, long lType = 0)
	{
		millsecond = 0;

		if ( lType == 0 )
		{
			if (sTime.size() != 19)
			{
				return;
			}
			year = atoi(sTime.substr(0, 4).c_str());
			month = atoi(sTime.substr(5, 2).c_str());
			day = atoi(sTime.substr(8, 2).c_str());
			hour = atoi(sTime.substr(11, 2).c_str());
			minute = atoi(sTime.substr(14, 2).c_str());
			second = atoi(sTime.substr(17, 2).c_str());
		}
		else if ( lType == 1 )
		{
			if (sTime.size() != 8)
			{
				return;
			}
			year = atoi(sTime.substr(0, 4).c_str());
			month = atoi(sTime.substr(4, 2).c_str());
			day = atoi(sTime.substr(6, 2).c_str());
			hour = 0;
			minute = 0;
			second = 0;
		}
		else if ( lType == 2)
		{
			if (sTime.size() == 15)
			{
				month = atoi( sTime.substr( 0, 2 ).c_str() );
				day = atoi( sTime.substr( 2, 2 ).c_str() );
				hour = atoi( sTime.substr( 4, 2 ).c_str() );
				minute = atoi( sTime.substr( 6, 2 ).c_str() );
				year = atoi( sTime.substr( 8, 4 ).c_str() );
				second = atoi( sTime.substr( 13, 2 ).c_str() );
			}
		}
	}
	Time(const Time& dt)
	{
		year = dt.year;
		month = dt.month;
		day = dt.day;
		hour = dt.hour;
		minute = dt.minute;
		second = dt.second;
		millsecond = dt.millsecond;
	}
	Time& operator =(const Time& dt )
	{
		year = dt.year;
		month = dt.month;
		day = dt.day;
		hour = dt.hour;
		minute = dt.minute;
		second = dt.second;		
		millsecond = dt.millsecond;
		return *this;
	}

	void Clear()
	{
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;		
		millsecond = 0;
	}

	std::string ToString(bool ms = false, bool nospace = false)
	{
		char sTime[128];
		memset(sTime,0,128);
		if (ms)
		{
			if (nospace)
			{
				_snprintf(sTime,127,"%04d%02d%02d%02d%02d%02d%03d",year,month,day,hour,minute,second,millsecond);
			}
			else
			{
				_snprintf(sTime,127,"%04d/%02d/%02d %02d:%02d:%02d.%03d",year,month,day,hour,minute,second,millsecond);
			}

		}
		else
		{
			if (nospace)
			{
				_snprintf(sTime,127,"%04d%02d%02d%02d%02d%02d",year,month,day,hour,minute,second);
			}
			else
			{
				_snprintf(sTime,127,"%04d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);
			}

		}

		return sTime;
	}
};

class Record
{
public:
	Record()
	{
		m_lSize = 0;
		m_lType = 0;
	}
	Record(Poco::LocalDateTime& tStart, Poco::LocalDateTime& sEnd, long lSize, long lType, const char * sName)
	{
		m_Start = tStart;
		m_End = sEnd;
		m_lSize = lSize;
		m_lType = lType;
		m_sName = sName;
	}

	Record& operator=(const Record& rd)
	{
		m_Start = rd.m_Start;
		m_End = rd.m_End;
		m_lSize = rd.m_lSize;
		m_lType = rd.m_lType;
		m_sName = rd.m_sName;
		return *this;
	}
public:
	Poco::LocalDateTime m_Start;			//��ʼʱ��
	Poco::LocalDateTime m_End;				//����ʱ��
	long m_lSize;			//��С
	long m_lType;			//����( 0��ʾ��ͨ¼��1��ʾ����¼�� )
	std::string m_sName;	//�ļ���( ������ʱ��ʾ������ļ�ȫ·��������ѯʱ��ʾDVR¼���ļ��� )
};

enum StreamType
{
	video_header = 0,	//��Ƶͷ
	video_stream,		//��Ƶ��
	video_main_stream,	//������( ���緢�� ) 
	video_sub_stream,	//������( �洢 )
	audio_stream,		//��Ƶ
};

enum FrameType
{
	unknown_frame = 0,  //δ֪(Ĭ��)
	i_frame,			//I֡
	b_frame,			//B֡
	p_frame,			//P֡
	audio_frame,		//Audio֡
	other_frame,		//��������
	head_frame          //��Ƶͷ
};

enum AlarmType
{
	alarm_camera = 0,	//�ƶ����
	alarm_in,			//��������
	alarm_out,			//�������
	alarm_video_lost,	//��Ƶ��ʧ
	alarm_disk_full,	//������
	alarm_disk_not_format,	//����δ��ʽ��
	alarm_disk_fault,	//���̹���
	alarm_hide,			//��Ƶ�ڵ�
};

enum RecordStreamType
{
	record_header,		//¼��������ͷ
	record_stream,		//¼������
	record_end			//¼����������
};

enum DownloadState
{
	e_download_init,		//��ʼ������
	e_download_more,		//���ص�������� //mlcai 2014-05-07
	e_download_wait,		//�ȴ��������� //mlcai 2014-05-07
	e_download_complete,	//�������
	e_download_error,		//����ʧ��
};

class VirtualDeviceMaker
{
public:
	virtual ~VirtualDeviceMaker() {}

	virtual VirtualDevice* Make() = 0;

	virtual void Reclaim(VirtualDevice* vd) = 0;

protected:
	VirtualDeviceMaker() {}
};


class VirtualDeviceFactory
{
public:
	VirtualDeviceFactory()
	{
	}
	~VirtualDeviceFactory()
	{
	}

	void LoadVDLibrary(const std::string& file_name)
	{
		RegisterVDMaker* RegFunction = 0;
#if defined(WIN32)
		HMODULE handle = ::LoadLibraryEx(file_name.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!handle)
			return;
		RegFunction = (RegisterVDMaker*)::GetProcAddress((HMODULE)handle, "RegisterVDMaker");
#endif
		if (RegFunction != 0)
			(*RegFunction)(this);
	}

	void AddVirtualDeviceMaker(const std::string& device_name, VirtualDeviceMaker* vd_maker)
	{
		Poco::FastMutex::ScopedLock lock(mutex_);
		vd_makers_.insert(std::map<std::string, VirtualDeviceMaker*>::value_type(device_name, vd_maker));
	}

	VirtualDevice* MakeVirtualDevice(const std::string& device_name)
	{
		Poco::FastMutex::ScopedLock lock(mutex_);
		VirtualDeviceMaker* vd_maker(0);

		std::map<std::string, VirtualDeviceMaker*>::iterator it = vd_makers_.find(device_name);
		if (it != vd_makers_.end())
			vd_maker = it->second;

		VirtualDevice* vd(0);
		if (vd_maker)
			vd = vd_maker->Make();

		return vd;
	}

private:
	std::map<std::string, VirtualDeviceMaker*> vd_makers_;
	Poco::FastMutex mutex_;

	typedef void RegisterVDMaker(VirtualDeviceFactory* vd_factory);
};
