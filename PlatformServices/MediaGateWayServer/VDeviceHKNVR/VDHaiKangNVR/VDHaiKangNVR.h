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

//全局初始化、注销
void sdk_global_destroy();

//add by sunminping
struct sAlarmInfo
{
	int channelno;     //通道号
	int alarmType;     //报警类型
	time_t lastTime;   //上次报警时间
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
	//图像参数
	NET_DVR_PICCFG_V30 m_dvr_pic_cfg;
	//视频参数
	NET_DVR_COMPRESSIONCFG_V30 m_dvr_compress_cfg;
	//RS485参数
	NET_DVR_DECODERCFG_V30 m_rs485_cfg;
	//扩展图像参数
	NET_DVR_PICCFG_EX m_dvr_pic_cfg_ex;

	//由于视频参数范围不一致，这里进行保存
	Platform::VideoPara m_VideoPara;

	bool m_bInitVideoPara;

	bool m_bInitPicCfg;
	bool m_bInitCompressCfg;
	bool m_bInitRsCfg;
	bool m_bInitMotion;

	//色调单独保存(bug)
	DWORD m_nHue;
};
//end add


class VDHaiKangNVR : public VirtualDevice
{
public:
	VDHaiKangNVR();
	~VDHaiKangNVR();

	//add by sunminping
	map<int,sAlarmInfo> alarmInfoMap;   //屏蔽海康一直报警的问题
	//end add


//接口
public:

	//初始化
	int Init(VDeviceConfig& dev_config);
	bool Destroy();

	//设置时间OSD
	bool SetTime(Time& t);
	//设置OSD
	//virtual bool SetOSD(string& sOSD) = 0;

	//重启设备
	bool Reboot();

	//实时视频
	bool PlayVideo(long lChannel,void* dm, bool bMainStrean);
	bool StopVideo(long lChannel,bool bMainStrean);

	//获取设置网络参数
	bool GetNetParam(string& sIPAddr, string& sNetMask, string& sGateway);

	//云台控制
	/*
	lCommand：命令码,定义如下：
	0 #停止 
	1  #云台上          2 #云台下 
	3  #云台左          4 #云台右
	5  #光圈+           6 #光圈- 
	7  #变倍+           8 #变倍- 
	9  #聚焦+          10 #聚焦-
	11 #云台自动扫描开 12 #云台自动扫描关
	13 #灯光开         14 #灯光关
	15 #雨刷开         16 #雨刷关
	17 #探头电源开     18 #探头电源关 
	19 #摄头电源开     20 #摄头电源关

	21 #背光补偿开			22 #背光补偿关
	23 #自动夜视切换开		24 #自动夜视切换关
	25 #手工转换为夜视显示	26 #手工转换为彩色显示
	27 #看守位功能开		28 #看守位功能关
	29 #自动光圈开			30 #自动光圈关
	31 #自动聚焦开			32 #自动聚焦关

	101 #180度翻转			102 #恢复球机的出厂参数设置

	1001 #设置预置点	   1002 #清除预置点
	1003 #转到预置点    

	lValue：当命令码为(1-12,101,1003)时，代表操作的速度，数值为(1~255)
	当命令码为(1001-1003)时，代表预置点索引。如果lCommand=1002且值为-1时，代表清除所有预置点

	pParam：扩展参数，默认为NULL。
	当命令为设置预置点时该参数代表预置点名称 ( char* sName = (char*)pParam )
	当命令为调用预置点时该参数代表速度( long lSpeed = (long)pParam )
	下层需判断是否为NULL来进行解析，如果为NULL则忽略该参数。
	*/
	bool PTZControl(long lChannel,long lCommand, long lValue, void* pParam = NULL);

	bool SetupAlarmChannel();
	void CloseAlarmChannel();
	//////////////////////////////////////////////////////////////////////////
	//图像及视频参数
	//如果每一项值是 －1，表示要设置默认值。

	//报警状态
	//lType:  0摄像头，1报警输入，2报警输出
	//lState: 状态 0为关 1为开
	bool SetAlarmState(long lChannel, long lType, long lState);
	bool GetAlarmState(long lChannel, long lType, long& lState);

	//OSD
	//bShowOsd：为1或0分别为打开和关闭时间和Logo设置。
	//nOSDPlace：表示时间的显示位置。1－8分别为左上，中上，右上，中左，中右，左下，中下，右下八个位置。
	//nWordPlace：表示Logo的显示位置。也是1－8。
	//sWord：为Logo 的文字。
	//默认显示时间和Logo，时间左上显示，Logo右下显示，内容为“视频位置”。
	bool SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,string& sWord);
	bool GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,string& sWord);

	//帧率
	//FrameRate：范围3－25。KeyFrameIntervals：的范围10－100。默认值自定义。
	bool SetFrameRate(long lChannel,long nFrameRate, long nKeyFrameIntervals,bool bMainStream=true);
	bool GetFrameRate(long lChannel,long& nFrameRate, long& nKeyFrameIntervals,bool bMainStream=true);

	//图像质量
	//Level：为等级。范围：最好、较好、好、一般。分别用 0,1,2,3代替。默认值自定义。
	bool SetPictureQuality(long lChannel,long nLevel,bool bMainStream=true);
	bool GetPictureQuality(long lChannel,long& nLevel,bool bMainStream=true);

	//最大码流
	//nMaxBPS：为码流值。单位是1000bit/sec范围：大于50。 vbr：是否为变码流
	bool SetMaxBPS(long lChannel,long nMaxBPS, bool vbr,bool bMainStream=true);
	bool GetMaxBPS(long lChannel,long& nMaxBPS, bool& vbr,bool bMainStream=true);

	//图像格式
	//PicFormat：CIF、QCIF、2CIF、4CIF/D1。分别用0,1,2,3代替。
	//SupportFormat：为支持的视频格式。如支持CIF、QCIF、2CIF为012。如支持QCIF、2CIF为13。默认值自定义。
	bool SetPictureFormat(long lChannel,long nPicFormat, string& sSupportFormat,bool bMainStream=true);
	bool GetPictureFormat(long lChannel,long& nPicFormat, string& sSupportFormat,bool bMainStream=true);

	//视频参数
	//对比度、色度、亮度、饱和度的大小范围都是0-255。
	bool SetVideoPara(long lChannel,long nBrightness, long nContrast, long nSaturation /*饱和度*/, long nHue/*色调*/);
	bool GetVideoPara(long lChannel,long& nBrightness, long& nContrast, long& nSaturation /*饱和度*/, long& nHue/*色调*/);

	//////////////////////////////////////////////////////////////////////////

	//侦测区域
	//bDectecting：为1或0分别为打开和关闭移动侦测。DetectGrade：为侦测的灵敏度。大小为0-6。0最迟钝，6最灵敏。
	//Rect：为侦测区域，Left、Top、Width、Height大小在352×288之间，如果视频分辨率不等于352×288，视频服务器应自动按比例进行缩放。
	//默认情况下没有设置移动侦测。

	bool SetDetectRect(long lChannel,vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade, long nMaxRectNum);
	bool GetDetectRect(long lChannel,vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade, long& nMaxRectNum);
	
	//遮挡区域
	//bLogoing：值为1或0分别为设置和取消图像遮挡。
	//Rect：表示遮挡区域，Left、Top、Width、Height大小在352×288之间，如果视频分辨率不等于352×288，视频服务器应自动按比例进行缩放。
	//如果设置了多个遮挡区域，将出现多个Rect结点。默认没有设置遮挡。

	bool SetShadeRect(long lChannel,vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum);
	bool GetShadeRect(long lChannel,vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum);

	//////////////////////////////////////////////////////////////////////////
	//查询录像
	//rd: 参考Record说明
	bool QueryRecord(long lChannel, const Poco::LocalDateTime& tStart, const Poco::LocalDateTime& tEnd,vector<Record>& rds);

	//请求录像
	//lRequestType: 请求类型，0表示停止下载，1表示下载
	//down_pos返回下载的位置 mlcai 2014-05-07
	bool RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext, long& lData, int& down_pos);

	//获取录像头
	//如果本身录像文件具有录像头，则只需返回头部大小，否则设置sHeader为相应录像协议头，比如大华为DHPT
	bool GetRecordHeader(long lChannel, long& lSize, string& sHeader)
	{ 
		lSize = 40; 
		return true;
	}

	//语音

	//获取虚拟平台采用的音频参数
	//lAudioChannels: 语音通道数
	//lAudioBit: 采样深度(位数)
	//lSampleRate: 采样速率
	//sFormat: 压缩格式，目前支持G711A.HX , G771U.HX , G722.HX
	bool GetAudioPara(long& lAudioChannels, long& lAudioBit, long& lSampleRate, string& sFormat);

	//开始语音呼叫
	bool StartAudioCall();

	//停止语音呼叫
	bool StopAudioCall();

	//转发音频数据到设备
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

		m_Config.vp_log->log(Message::PRIO_NOTICE,"设置压缩参数失败, Channel = %ld, Error = %lu", lChannel+1, NET_DVR_GetLastError());
		return false; 
	}
	bool SetPicturInfo(long lChannel)
	{
		if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_PICCFG_V30,lChannel+1,
			&video_Para_records[lChannel].m_dvr_pic_cfg,sizeof(NET_DVR_PICCFG_V30)))
			return true;

		DWORD err=NET_DVR_GetLastError();
		m_Config.vp_log->log(Message::PRIO_NOTICE,"设置图像参数失败, Channel = %ld, Error= %lu", lChannel+1,err );
		return false;
	}

	bool SetExPicturInfo(long lChannel)
	{
		video_Para_records[lChannel].m_dvr_pic_cfg_ex;
		NET_DVR_VIDEOPARA_V40 struVideoPara;
		memset(&struVideoPara, 0, sizeof(NET_DVR_VIDEOPARA_V40));
		struVideoPara.dwChannel=lChannel+1;
		DWORD DvrVal[4]={0};
		DvrVal[0]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.byBrightness;     //亮度
		DvrVal[1]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.byContrast;       //对比度
		DvrVal[2]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.bySaturation;     //饱和度
		DvrVal[3]=video_Para_records[lChannel].m_dvr_pic_cfg_ex.byHue;            //色调
		BOOL bRet=TRUE;
		for(int i=0;i<sizeof(DvrVal)/sizeof(DvrVal[0]) && bRet;++i)
		{
			struVideoPara.dwVideoParamType = i;
			struVideoPara.dwVideoParamValue = DvrVal[i];
			bRet=NET_DVR_RemoteControl(m_lUserID, NET_DVR_SET_VIDEO_EFFECT, &struVideoPara, sizeof(NET_DVR_VIDEOPARA_V40));
		}

		if (!bRet)
		{
			//失败了，使用老接口设置
			bRet = NET_DVR_ClientSetVideoEffect(m_lUserID, DvrVal[0],DvrVal[1],DvrVal[2],DvrVal[3]);
			if (!bRet)
				m_Config.vp_log->log(Message::PRIO_NOTICE,"通道%ld 设置视频参数失败",lChannel+1);
		}
		return bRet;


		/*if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_PICCFG_EX,lChannel+1,
			&video_Para_records[lChannel].m_dvr_pic_cfg_ex,sizeof(NET_DVR_PICCFG_EX)) )
			return true;

		DWORD err=NET_DVR_GetLastError();
		m_Config.vp_log->log(Message::PRIO_NOTICE,"设置图扩展像参数失败, Channel = %ld, Error = %lu", lChannel+1,err);
		return false;*/        //delete by sunminping  这种版本在新设备上不能用

	}
	bool Set485Param(long lChannel)
	{
		if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_DECODERCFG_V30,lChannel+1,
			&video_Para_records[lChannel].m_rs485_cfg,sizeof(NET_DVR_DECODERCFG_V30)) )
			return true;
		m_Config.vp_log->log(Message::PRIO_NOTICE,"设置485参数失败, Channel = %ld, Error = %lu", lChannel+1, NET_DVR_GetLastError());
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

	//用户ID
	LONG m_lUserID;

	//版本
	string m_sVersion;

	Poco::Mutex rec_mutex_;
    std::map<long,VPlatformChannel> records;

	std::map<long,HKVPlatformChannel> video_Para_records;
	
	Poco::Mutex alarm_mutex_;
	std::hash_map<DWORD, long> _alarm_type_map;

	DWORD m_dwLastCommand;          //上一次的云台控制命令
};

//提供对象工厂
class VDHaiKangNVRMaker : public VirtualDeviceMaker
{
public:
	VDHaiKangNVRMaker();
	~VDHaiKangNVRMaker();

	VirtualDevice* Make();

	void Reclaim(VirtualDevice* vd);
};
