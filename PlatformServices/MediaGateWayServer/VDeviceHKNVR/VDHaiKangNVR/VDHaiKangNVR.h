#pragma once

#include "HCNetSDK.h"
#include "../MediaGateWayServer/VirtualDevice.h"
#include "../Typedef.h"
#include <map>
#include "OBMCtrl.h"
#include "Poco/Mutex.h"
#include "Poco/ScopedLock.h"
#include "LogInfo.h"
#include "LogSys.h"

#ifdef WIN32
	#ifdef _DEBUG
		#pragma comment(lib, "PocoFoundationd.lib")
	#else
		#pragma comment(lib, "PocoFoundation.lib")
	#endif
#endif
#pragma comment(lib,"./VDHaiKangNVR/HCNetSDK.lib")

#define INTFACE VPlatform
#define INTFACE_NAME "VPlatform"
#define CLASS VDHaiKangNVR
#define CLASS_NAME "VDHaiKangNVR"

#define VPHaiKangNVR_Version	"v3.1.1 Build20091110"

extern const char * sAttribTable[][2];

//ȫ�ֳ�ʼ����ע��
void sdk_global_destroy();

//add by sunminping
struct sAlarmInfo
{
	int channelno;     //ͨ����
	int alarmType;     //��������
	time_t lastTime;   //�ϴα���ʱ��
};
//end add


//add by sunminping 2014-8-22
class HKVPlatformChannel
{
public:
	HKVPlatformChannel()
	{
		memset(&m_dvr_pic_cfg,0,sizeof(m_dvr_pic_cfg));
		memset(&m_dvr_compress_cfg,0,sizeof(m_dvr_compress_cfg));
		m_bInitVideoPara = false;
		m_bInitPicCfg = false;
		m_bInitCompressCfg = false;
		m_bInitRsCfg = false;
		m_nHue = 5;
	}
public:
	//ͼ�����
	NET_DVR_PICCFG_V30 m_dvr_pic_cfg;
	//��Ƶ����
	NET_DVR_COMPRESSIONCFG_V30 m_dvr_compress_cfg;
	//RS485����
	NET_DVR_DECODERCFG_V30 m_rs485_cfg;
	//��չͼ�����
	NET_DVR_PICCFG_EX m_dvr_pic_cfg_ex;

	//������Ƶ������Χ��һ�£�������б���
	Platform::VideoPara m_VideoPara;

	bool m_bInitVideoPara;

	bool m_bInitPicCfg;
	bool m_bInitCompressCfg;
	bool m_bInitRsCfg;
	bool m_bInitMotion;

	//ɫ����������(bug)
	DWORD m_nHue;
};
//end add


class VDHaiKangNVR : public VirtualDevice
{
public:
	VDHaiKangNVR();
	~VDHaiKangNVR();

	//add by sunminping
	map<int,sAlarmInfo> alarmInfoMap;   //���κ���һֱ����������
	//end add


//�ӿ�
public:

	//��ʼ��
	int Init(VDeviceConfig& dev_config);
	bool Destroy();

	//����ʱ��OSD
	bool SetTime(Time& t);
	//����OSD
	//virtual bool SetOSD(string& sOSD) = 0;

	//�����豸
	bool Reboot();

	//ʵʱ��Ƶ
	bool PlayVideo(long lChannel,void* dm, bool bMainStrean);
	bool StopVideo(long lChannel,bool bMainStrean);

	//��ȡ�����������
	bool GetNetParam(string& sIPAddr, string& sNetMask, string& sGateway);

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
	1003 #ת��Ԥ�õ�    

	lValue����������Ϊ(1-12,101,1003)ʱ������������ٶȣ���ֵΪ(1~255)
	��������Ϊ(1001-1003)ʱ������Ԥ�õ����������lCommand=1002��ֵΪ-1ʱ�������������Ԥ�õ�

	pParam����չ������Ĭ��ΪNULL��
	������Ϊ����Ԥ�õ�ʱ�ò�������Ԥ�õ����� ( char* sName = (char*)pParam )
	������Ϊ����Ԥ�õ�ʱ�ò��������ٶ�( long lSpeed = (long)pParam )
	�²����ж��Ƿ�ΪNULL�����н��������ΪNULL����Ըò�����
	*/
	bool PTZControl(long lChannel,long lCommand, long lValue, void* pParam = NULL);

	bool SetupAlarmChannel();
	void CloseAlarmChannel();
	//////////////////////////////////////////////////////////////////////////
	//ͼ����Ƶ����
	//���ÿһ��ֵ�� ��1����ʾҪ����Ĭ��ֵ��

	//����״̬
	//lType:  0����ͷ��1�������룬2�������
	//lState: ״̬ 0Ϊ�� 1Ϊ��
	bool SetAlarmState(long lChannel, long lType, long lState);
	bool GetAlarmState(long lChannel, long lType, long& lState);

	//OSD
	//bShowOsd��Ϊ1��0�ֱ�Ϊ�򿪺͹ر�ʱ���Logo���á�
	//nOSDPlace����ʾʱ�����ʾλ�á�1��8�ֱ�Ϊ���ϣ����ϣ����ϣ��������ң����£����£����°˸�λ�á�
	//nWordPlace����ʾLogo����ʾλ�á�Ҳ��1��8��
	//sWord��ΪLogo �����֡�
	//Ĭ����ʾʱ���Logo��ʱ��������ʾ��Logo������ʾ������Ϊ����Ƶλ�á���
	bool SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,string& sWord);
	bool GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,string& sWord);

	//֡��
	//FrameRate����Χ3��25��KeyFrameIntervals���ķ�Χ10��100��Ĭ��ֵ�Զ��塣
	bool SetFrameRate(long lChannel,long nFrameRate, long nKeyFrameIntervals,bool bMainStream=true);
	bool GetFrameRate(long lChannel,long& nFrameRate, long& nKeyFrameIntervals,bool bMainStream=true);

	//ͼ������
	//Level��Ϊ�ȼ�����Χ����á��Ϻá��á�һ�㡣�ֱ��� 0,1,2,3���档Ĭ��ֵ�Զ��塣
	bool SetPictureQuality(long lChannel,long nLevel,bool bMainStream=true);
	bool GetPictureQuality(long lChannel,long& nLevel,bool bMainStream=true);

	//�������
	//nMaxBPS��Ϊ����ֵ����λ��1000bit/sec��Χ������50�� vbr���Ƿ�Ϊ������
	bool SetMaxBPS(long lChannel,long nMaxBPS, bool vbr,bool bMainStream=true);
	bool GetMaxBPS(long lChannel,long& nMaxBPS, bool& vbr,bool bMainStream=true);

	//ͼ���ʽ
	//PicFormat��CIF��QCIF��2CIF��4CIF/D1���ֱ���0,1,2,3���档
	//SupportFormat��Ϊ֧�ֵ���Ƶ��ʽ����֧��CIF��QCIF��2CIFΪ012����֧��QCIF��2CIFΪ13��Ĭ��ֵ�Զ��塣
	bool SetPictureFormat(long lChannel,long nPicFormat, string& sSupportFormat,bool bMainStream=true);
	bool GetPictureFormat(long lChannel,long& nPicFormat, string& sSupportFormat,bool bMainStream=true);

	//��Ƶ����
	//�Աȶȡ�ɫ�ȡ����ȡ����ͶȵĴ�С��Χ����0-255��
	bool SetVideoPara(long lChannel,long nBrightness, long nContrast, long nSaturation /*���Ͷ�*/, long nHue/*ɫ��*/);
	bool GetVideoPara(long lChannel,long& nBrightness, long& nContrast, long& nSaturation /*���Ͷ�*/, long& nHue/*ɫ��*/);

	//////////////////////////////////////////////////////////////////////////

	//�������
	//bDectecting��Ϊ1��0�ֱ�Ϊ�򿪺͹ر��ƶ���⡣DetectGrade��Ϊ���������ȡ���СΪ0-6��0��ٶۣ�6��������
	//Rect��Ϊ�������Left��Top��Width��Height��С��352��288֮�䣬�����Ƶ�ֱ��ʲ�����352��288����Ƶ������Ӧ�Զ��������������š�
	//Ĭ�������û�������ƶ���⡣

	bool SetDetectRect(long lChannel,vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade, long nMaxRectNum);
	bool GetDetectRect(long lChannel,vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade, long& nMaxRectNum);
	
	//�ڵ�����
	//bLogoing��ֵΪ1��0�ֱ�Ϊ���ú�ȡ��ͼ���ڵ���
	//Rect����ʾ�ڵ�����Left��Top��Width��Height��С��352��288֮�䣬�����Ƶ�ֱ��ʲ�����352��288����Ƶ������Ӧ�Զ��������������š�
	//��������˶���ڵ����򣬽����ֶ��Rect��㡣Ĭ��û�������ڵ���

	bool SetShadeRect(long lChannel,vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum);
	bool GetShadeRect(long lChannel,vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum);

	//////////////////////////////////////////////////////////////////////////
	//��ѯ¼��
	//rd: �ο�Record˵��
	bool QueryRecord(long lChannel, const Poco::LocalDateTime& tStart, const Poco::LocalDateTime& tEnd,vector<Record>& rds);

	//����¼��
	//lRequestType: �������ͣ�0��ʾֹͣ���أ�1��ʾ����
	//down_pos�������ص�λ�� mlcai 2014-05-07
	bool RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext, long& lData, int& down_pos);

	//��ȡ¼��ͷ
	//�������¼���ļ�����¼��ͷ����ֻ�践��ͷ����С����������sHeaderΪ��Ӧ¼��Э��ͷ�������ΪDHPT
	bool GetRecordHeader(long lChannel, long& lSize, string& sHeader)
	{ 
		lSize = 40; 
		return true;
	}

	//����

	//��ȡ����ƽ̨���õ���Ƶ����
	//lAudioChannels: ����ͨ����
	//lAudioBit: �������(λ��)
	//lSampleRate: ��������
	//sFormat: ѹ����ʽ��Ŀǰ֧��G711A.HX , G771U.HX , G722.HX
	bool GetAudioPara(long& lAudioChannels, long& lAudioBit, long& lSampleRate, string& sFormat);

	//��ʼ��������
	bool StartAudioCall();

	//ֹͣ��������
	bool StopAudioCall();

	//ת����Ƶ���ݵ��豸
	bool SendAudioData(void * pBuffer, long lSize);

	bool GetProtocol(string& sProtocol)
	{
		sProtocol = "HIKMP4.2.5";
		return true;
	}

private:
	int Login(VDeviceConfig& dev_config);
	bool InitPTZCommand();

	void RegisterObject();
	void UnRegisterObject();

public:
	BOOL MessageNotify(LONG lCommand,char *pBuf,DWORD dwBufLen);

private:
	void TimeConvert(Time& t1, NET_DVR_TIME& t2, long lType)
	{
		if ( lType == 0 )
		{
			t2.dwYear = t1.year;
			t2.dwMonth = t1.month;
			t2.dwDay = t1.day;
			t2.dwHour = t1.hour;
			t2.dwMinute = t1.minute;
			t2.dwSecond = t1.second;
		}
		else
		{
			t1.year = t2.dwYear;
			t1.month = t2.dwMonth;
			t1.day = t2.dwDay;
			t1.hour = t2.dwHour;
			t1.minute = t2.dwMinute;
			t1.second = t2.dwSecond;
		}
	}

	NET_DVR_COMPRESSION_INFO_V30& GetCompressInfo(long lChannel)
	{
		return video_Para_records[lChannel].m_dvr_compress_cfg.struNormHighRecordPara;
	}
	NET_DVR_PICCFG_V30& GetPictureInfo(long lChannel)
	{
		return video_Para_records[lChannel].m_dvr_pic_cfg;
	}
	NET_DVR_PICCFG_EX& GetExPictureInfo(long lChannel)
	{
		return video_Para_records[lChannel].m_dvr_pic_cfg_ex;
	}
	NET_DVR_DECODERCFG_V30& Get485Param(long lChannel)
	{
		return video_Para_records[lChannel].m_rs485_cfg;
	}

	bool SetCompressInfo(long lChannel)
	{
		if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_COMPRESSCFG_V30,lChannel+1,
			&video_Para_records[lChannel].m_dvr_compress_cfg,sizeof(NET_DVR_COMPRESSIONCFG_V30)) )
			return true;

		m_Config.vp_log->log(Message::PRIO_NOTICE,"����ѹ������ʧ��, Channel = %ld, Error = %lu", lChannel+1, NET_DVR_GetLastError());
		return false; 
	}
	bool SetPicturInfo(long lChannel)
	{
		if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_PICCFG_V30,lChannel+1,
			&video_Para_records[lChannel].m_dvr_pic_cfg,sizeof(NET_DVR_PICCFG_V30)))
			return true;

		DWORD err=NET_DVR_GetLastError();
		m_Config.vp_log->log(Message::PRIO_NOTICE,"����ͼ�����ʧ��, Channel = %ld, Error= %lu", lChannel+1,err );
		return false;
	}

	bool SetExPicturInfo(long lChannel)
	{
		video_Para_records[lChannel].m_dvr_pic_cfg_ex;
		NET_DVR_VIDEOPARA_V40 struVideoPara;
		memset(&struVideoPara, 0, sizeof(NET_DVR_VIDEOPARA_V40));
		struVideoPara.dwChannel=lChannel+1;
		DWORD DvrVal[4]={0};
		DvrVal[0]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.byBrightness;     //����
		DvrVal[1]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.byContrast;       //�Աȶ�
		DvrVal[2]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.bySaturation;     //���Ͷ�
		DvrVal[3]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.byHue;            //ɫ��
		BOOL bRet=TRUE;
		for(int i=0;i<sizeof(DvrVal)/sizeof(DvrVal[0]) && bRet;++i)
		{
			struVideoPara.dwVideoParamType = i;
			struVideoPara.dwVideoParamValue = DvrVal[i];
			bRet=NET_DVR_RemoteControl(m_lUserID, NET_DVR_SET_VIDEO_EFFECT, &struVideoPara, sizeof(NET_DVR_VIDEOPARA_V40));
		}

		if (!bRet)
		{
			//ʧ���ˣ�ʹ���Ͻӿ�����
			bRet = NET_DVR_ClientSetVideoEffect(m_lUserID, DvrVal[0],DvrVal[1],DvrVal[2],DvrVal[3]);
			if (!bRet)
				m_Config.vp_log->log(Message::PRIO_NOTICE,"ͨ��%ld ������Ƶ����ʧ��",lChannel+1);
		}
		return bRet;


		/*if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_PICCFG_EX,lChannel+1,
			&video_Para_records[lChannel].m_dvr_pic_cfg_ex,sizeof(NET_DVR_PICCFG_EX)) )
			return true;

		DWORD err=NET_DVR_GetLastError();
		m_Config.vp_log->log(Message::PRIO_NOTICE,"����ͼ��չ�����ʧ��, Channel = %ld, Error = %lu", lChannel+1,err);
		return false;*/        //delete by sunminping  ���ְ汾�����豸�ϲ�����

	}
	bool Set485Param(long lChannel)
	{
		if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_DECODERCFG_V30,lChannel+1,
			&video_Para_records[lChannel].m_rs485_cfg,sizeof(NET_DVR_DECODERCFG_V30)) )
			return true;
		m_Config.vp_log->log(Message::PRIO_NOTICE,"����485����ʧ��, Channel = %ld, Error = %lu", lChannel+1, NET_DVR_GetLastError());
		return false;
	}

	string GetChannelName(long lChannel)
	{
		return (char*)(video_Para_records[lChannel].m_dvr_pic_cfg.sChanName);
	}

	void SetFullSchemeTime(NET_DVR_SCHEDTIME ds[MAX_DAYS][MAX_TIMESEGMENT_V30])
	{
		for (int i = 0; i < MAX_DAYS; i++)
		{
			for (int j = 0; j < MAX_TIMESEGMENT_V30; j++)
			{
				NET_DVR_SCHEDTIME& ts = ds[i][j];
				ts.byStartHour = 0;
				ts.byStartMin = 0;
				ts.byStopHour = 0;
				ts.byStopMin = 0;
			}

			NET_DVR_SCHEDTIME& ts = ds[i][0];
			ts.byStartHour = 0;
			ts.byStartMin = 0;
			ts.byStopHour = 23;
			ts.byStopMin = 59;
		}

	}

	void GetPointForPlace(WORD& x, WORD& y, WORD lPlace);
	void GetPlaceForPoint(WORD x, WORD y, WORD& lPlace);

	bool InitPicCfg(long lChannel);
	bool InitCompressCfg(long lChannel);

private:
	static bool m_bInitOK;

	//�û�ID
	LONG m_lUserID;

	//�汾
	string m_sVersion;

	Poco::Mutex rec_mutex_;
    std::map<long,VPlatformChannel> records;

	std::map<long,HKVPlatformChannel> video_Para_records;
	
	Poco::Mutex alarm_mutex_;
	std::hash_map<DWORD, long> _alarm_type_map;

	DWORD m_dwLastCommand;          //��һ�ε���̨��������
};

//�ṩ���󹤳�
class VDHaiKangNVRMaker : public VirtualDeviceMaker
{
public:
	VDHaiKangNVRMaker();
	~VDHaiKangNVRMaker();

	VirtualDevice* Make();

	void Reclaim(VirtualDevice* vd);
};
