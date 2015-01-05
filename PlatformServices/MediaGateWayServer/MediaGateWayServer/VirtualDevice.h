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
// 厂商虚拟设备接口
//
//////////////////////////////////////////////////////////////////////////

class Rect;
class Time;
class Record;

#pragma pack(push)
#pragma pack(1)
typedef struct _RESULT2
{
	unsigned short nCode;		// 消息值
	unsigned short nSource:13;	// 消息来源(对应nFacility)
	unsigned short bUser: 1;	// 是否为用户自定义的消息码
	unsigned short nGrade:2;	//  0表示成功，指定的命令已经运行完毕
	//  1表示通知，指定的命令尚未运行完毕
	//  2表示警告（命令运行过程中出现了一些问题，但还可以继续运行得出结果）
	//  3表示错误（命令已经出错退出，因为在运行过程中出现了致命问题，无法完成指定的任务）
	//此处的定义与HRESULT的定义是一致的：
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
	string deviceID;		//设备ID
    string deviceType;      //设备类型，用来区分宇视科技NVR的录像扩展名
	string serverIP;		//IP地址
	long port;				//端口
	string user;			//用户名
	string passwd;			//密码
	string extendPara1;  //扩展
	LogSys* vp_log;         //vp日志        

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
// 每个DVR有多个通道，每个通道可以支持多路
// 在虚拟视频服务器中，只需要保持每个通道1路连接即可，由上层进行多路分发
//
//////////////////////////////////////////////////////////////////////////

//管理各厂商SDK的虚拟设备
class VirtualDevice
{
public:
	class ICallBack
	{
	public:
		//流数据通知(包括音视频等)
		//lStreamType，数据流类型，视频头、视频流、主子码流、录像文件头、录像文件流等
		//lFrameType，帧类型，IBP帧等，分两种情况，如果无法获取类型则全部设为unknown_frame
		virtual void NotifyStreamData(long lChannel,long lStreamType,long lFrameType,PBYTE pBuffer,DWORD dwSize, void* dm) = 0;

		//告警通知, szAlarmMessage保留用于有可能呈现厂家特殊扩展信息
		virtual bool NotifyAlarm(long lChannel,long lType,LPCTSTR szAlarmMessage = NULL) = 0;

		//本地处理消息通知
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
	//必须实现的接口

	//初始化
	virtual int Init(VDeviceConfig& dev_config) = 0;
	virtual bool Destroy() = 0;

	//实时视频
	virtual bool PlayVideo(long lChannel,void* dm, bool bMainStrean = 0) = 0;
	virtual bool StopVideo(long lChannel,bool bMainStrean = 0) = 0;

	virtual void GetVersion(string& version, string& build_time)
	{
		version = "";
		build_time = "";
	}

	//查询录像
	//rd: 参考Record说明
	virtual bool QueryRecord(long lChannel, const LocalDateTime& tStart, const LocalDateTime& tEnd,vector<Record>& rds)
	{
		return false;
	}

	//请求录像
	//lRequestType: 请求类型，0表示停止下载，1表示下载，2获取下载状态
	//lContext: 下载上下文句柄
	//lData: 当类型为2时表示下载状态，其余保留
	//pos返回下载的位置 mlcai 2014-05-07
	virtual bool RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext, long& lData, int& pos)
	{
		return false;
	}

	//获取视频头
	//如果DVR录像文件本身具有录像头，则只需返回头部大小
	//否则设置lSize为0，同时设置sHeader为相应录像协议头，比如大华为"DHPT"
	virtual bool GetRecordHeader(long lChannel, long& lSize, string& sHeader)
	{
		return false;
	}

	//获取设置网络参数
	virtual bool GetNetParam(string& sIPAddr, string& sNetMask, string& sGateway)
	{
		return false;
	}

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
	1003 #转到预置点       1004 #三维定位(x,y,z)

	lValue：当命令码为(1-12,101,1003)时，代表操作的速度，数值为(1~255)
			当命令码为(1001-1003)时，代表预置点索引。如果lCommand=1002且值为-1时，代表清除所有预置点

	pParam：扩展参数，默认为NULL。
			当命令为设置预置点时该参数代表预置点名称 ( char* sName = (char*)pParam )
			当命令为调用预置点时该参数代表速度( long lSpeed = atoi((char*)pParam) )
	*/
	virtual bool PTZControl(long lChannel,long lCommand, long lValue, void* pParam = NULL) = 0;


	////////////////////////////////////////////////////////////////////////////
	////可选接口


	//设置时间OSD
	//virtual bool SetTime(Time& t);
	//设置OSD
	//virtual bool SetOSD(string& sOSD) = 0;


	//建立报警上传通道(布防)
	virtual bool SetupAlarmChannel()
	{
		return false;
	}

	//关闭报警上传通道（撤防）
	virtual void CloseAlarmChannel()
	{
		
	}

	//报警状态
	//lType:  0摄像头，1报警输入，2报警输出
	//lState: 状态 0为关 1为开;
	virtual bool SetAlarmState(long lChannel, long lType, long lState)
	{
		return false;
	}
	virtual bool GetAlarmState(long lChannel, long lType, long& lState)
	{
		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	////图像及视频参数
	////如果某一项值是 －1，表示要设置默认值。

	//OSD
	//bShowOsd：为1或0分别为打开和关闭时间和Logo设置。
	//nOSDPlace：表示时间的显示位置。1－8分别为左上，中上，右上，中左，中右，左下，中下，右下八个位置。
	//nWordPlace：表示Logo的显示位置。也是1－8。
	//sWord：为Logo 的文字。
	//默认显示时间和Logo，时间左上显示，Logo右下显示，内容为“视频位置”。
	virtual bool SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,string& sWord)
	{
		return false;
	}
	virtual bool GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,string& sWord)
	{
		return false;
	}

	//帧率
	//FrameRate：范围3－25。KeyFrameIntervals：的范围10－100。默认值自定义。
	virtual bool SetFrameRate(long lChannel,long nFrameRate, long nKeyFrameIntervals,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetFrameRate(long lChannel,long& nFrameRate, long& nKeyFrameIntervals,bool bMainStream=true)
	{
		return false;
	}

	//图像质量
	//Level：为等级。范围：最好、较好、好、一般。分别用 0,1,2,3代替。默认值自定义。
	virtual bool SetPictureQuality(long lChannel,long nLevel,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetPictureQuality(long lChannel,long& nLevel,bool bMainStream=true)
	{
		return false;
	}

	//最大码流
	//nMaxBPS：为码流值。单位是1000bit/sec范围：大于50。 vbr：是否为变码流
	virtual bool SetMaxBPS(long lChannel,long nMaxBPS, bool vbr,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetMaxBPS(long lChannel,long& nMaxBPS, bool& vbr,bool bMainStream=true)
	{
		return false;
	}

	//图像格式
	//PicFormat：CIF、QCIF、2CIF、4CIF/D1。分别用0,1,2,3代替。
	//SupportFormat：为支持的视频格式。如支持CIF、QCIF、2CIF为012。如支持QCIF、2CIF为13。默认值自定义。
	virtual bool SetPictureFormat(long lChannel,long nPicFormat, string& sSupportFormat,bool bMainStream=true)
	{
		return false;
	}
	virtual bool GetPictureFormat(long lChannel,long& nPicFormat, string& sSupportFormat,bool bMainStream=true)
	{
		return false;
	}

	//视频参数
	//对比度、色度、亮度、饱和度的大小范围都是0-255。
	virtual bool SetVideoPara(long lChannel,long nBrightness, long nContrast, long nSaturation /*饱和度*/, 
		long nHue/*色调*/)
	{
		return false;
	}

	virtual bool GetVideoPara(long lChannel,long& nBrightness, long& nContrast, long& nSaturation /*饱和度*/, 
		long& nHue/*色调*/)
	{
		return false;
	}

	////////////////////////////////////////////////////////////////////////
	//Rect(352*288) 上层保证参数正确，下层不用判断，如果下层无法获取精确区域，应返回false交由上层处理

	//侦测区域
	//bDectecting：为1或0分别为打开和关闭移动侦测。DetectGrade：为侦测的灵敏度。大小为0-6。0最迟钝，6最灵敏。
	//Rect：为侦测区域，Left、Top、Width、Height大小在352×288之间，如果视频分辨率不等于352×288，视频服务器应自动按比例进行缩放。
	//默认情况下没有设置移动侦测。

	virtual bool SetDetectRect(long lChannel,vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade,long nMaxRectNum)
	{
		return false;
	}
	virtual bool GetDetectRect(long lChannel,vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade,long& nMaxRectNum)
	{
		return false;
	}

	//遮挡区域
	virtual bool SetShadeRect(long lChannel,vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum)
	{
		return false;
	}
	virtual bool GetShadeRect(long lChannel,vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum)
	{
		return false;
	}

	////重启设备
	//virtual bool Reboot()
	//{
	//	return false;
	//}

	////
	////语音

	////获取虚拟平台采用的音频参数
	////lAudioChannels: 语音通道数
	////lAudioBit: 采样深度(位数)
	////lSampleRate: 采样速率
	////sFormat: 压缩格式，参考HX_AUDIONAME_G722HK等定义
	//virtual bool GetAudioPara(long& lAudioChannels, long& lAudioBit, long& lSampleRate, string& sFormat)
	//{
	//	return false;
	//}

	////开始语音呼叫
	//virtual bool StartAudioCall()
	//{
	//	return false;
	//}

	////停止语音呼叫
	//virtual bool StopAudioCall()
	//{
	//	return false;
	//}

	////转发音频数据到设备
	//virtual bool SendAudioData(void * pBuffer, long lSize)
	//{
	//	return false;
	//}

	////获取协议类型
	//virtual bool GetProtocol(string& sProtocol)
	//{
	//	return false;
	//}

	// 获取当前预置点列表
	virtual bool GetPresetList(long lChannel, string& sRet)
	{
		return false;
	}

public:
	//获得对应的playId
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
		//虚拟平台配置
		VDeviceInfo m_Config;
};


//实时视频通道记录信息，用于实时视频回调使用
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
	long play_handle_;	//播放句柄(一般来说，和索引值一致，为了避免不一致的情况，分开处理)
	int channelno_;
	bool is_mainstream_;
	VirtualDevice* vplatform_;
	void* distribution_manager_;
    char  play_handle_str_[128]; //宇视科技nvr的播放句柄

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
	Poco::LocalDateTime m_Start;			//开始时间
	Poco::LocalDateTime m_End;				//结束时间
	long m_lSize;			//大小
	long m_lType;			//类型( 0表示普通录像，1表示报警录像 )
	std::string m_sName;	//文件名( 当下载时表示保存的文件全路径，当查询时表示DVR录像文件名 )
};

enum StreamType
{
	video_header = 0,	//视频头
	video_stream,		//视频流
	video_main_stream,	//主码流( 网络发送 ) 
	video_sub_stream,	//子码流( 存储 )
	audio_stream,		//音频
};

enum FrameType
{
	unknown_frame = 0,  //未知(默认)
	i_frame,			//I帧
	b_frame,			//B帧
	p_frame,			//P帧
	audio_frame,		//Audio帧
	other_frame,		//其他类型
	head_frame          //视频头
};

enum AlarmType
{
	alarm_camera = 0,	//移动侦测
	alarm_in,			//报警输入
	alarm_out,			//报警输出
	alarm_video_lost,	//视频丢失
	alarm_disk_full,	//磁盘满
	alarm_disk_not_format,	//磁盘未格式化
	alarm_disk_fault,	//磁盘故障
	alarm_hide,			//视频遮挡
};

enum RecordStreamType
{
	record_header,		//录像码流流头
	record_stream,		//录像码流
	record_end			//录像码流结束
};

enum DownloadState
{
	e_download_init,		//初始化下载
	e_download_more,		//下载到多的数据 //mlcai 2014-05-07
	e_download_wait,		//等待下载数据 //mlcai 2014-05-07
	e_download_complete,	//下载完成
	e_download_error,		//下载失败
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
