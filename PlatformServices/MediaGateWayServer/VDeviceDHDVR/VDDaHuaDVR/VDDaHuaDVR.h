#pragma once

#include "dhnetsdk.h"
#include "Poco/Mutex.h"
#include "Poco/ScopedLock.h"
#include "LogInfo.h"
#include "LogSys.h"
#include "../MediaGateWayServer/VirtualDevice.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "PocoFoundationd.lib")
#else
#pragma comment(lib, "PocoFoundation.lib")
#endif
#endif

#pragma comment(lib,"./VDDaHuaDVR/dhnetsdk.lib")
#pragma comment(lib, "./VDDaHuaDVR/dhconfigsdk.lib")

#define VPDaHua_Version	"v3.4.1 Build20130826"

extern const char * sAttribTable[][2];

class VDDaHuaDVR : public VirtualDevice
{
public:
	VDDaHuaDVR();
	~VDDaHuaDVR();
	// 接口
public:
	//初始化
	virtual int Init(VDeviceConfig& dev_config);
	virtual bool Destroy();

	//实时视频
	virtual bool PlayVideo(long lChannel,void* dm, bool bMainStrean = 0);
	//停止实时监控
	virtual bool StopVideo(long lChannel,bool bMainStrean = 0);

	//获取设置网络参数
	bool GetNetParam(std::string& sIPAddr, std::string& sNetMask, std::string& sGateway);

	//云台控制
	virtual bool PTZControl(long lChannel,long lCommand, long lValue, void* pParam = NULL);

	//建立报警上传通道(布防)
	bool SetupAlarmChannel();

	//关闭报警上传通道（撤防）
	void CloseAlarmChannel();

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
	bool SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,std::string& sWord);
	bool GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,std::string& sWord);

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


	//侦测区域
	//bDectecting：为1或0分别为打开和关闭移动侦测。DetectGrade：为侦测的灵敏度。大小为0-6。0最迟钝，6最灵敏。
	//Rect：为侦测区域，Left、Top、Width、Height大小在352×288之间，如果视频分辨率不等于352×288，视频服务器应自动按比例进行缩放。
	//默认情况下没有设置移动侦测。
	bool SetDetectRect(long lChannel,std::vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade, long nMaxRectNum);
	bool GetDetectRect(long lChannel,std::vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade, long& nMaxRectNum);

	//遮挡区域
	//bLogoing：值为1或0分别为设置和取消图像遮挡。
	//Rect：表示遮挡区域，Left、Top、Width、Height大小在352×288之间，如果视频分辨率不等于352×288，视频服务器应自动按比例进行缩放。
	//如果设置了多个遮挡区域，将出现多个Rect结点。默认没有设置遮挡。
	bool SetShadeRect(long lChannel,std::vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum);
	bool GetShadeRect(long lChannel,std::vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum);

	//查询录像
	//rd: 参考Record说明
	bool QueryRecord(long lChannel, Poco::LocalDateTime& tStart, Poco::LocalDateTime& tEnd,vector<Record>& rds);

	//请求录像
	//lRequestType: 请求类型，0表示停止下载，1表示下载
	//pos返回下载的位置 mlcai 2014-05-07
	bool RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext, long& lData, int& pos);

	//获取录像头
	//如果本身录像文件具有录像头，则只需返回头部大小，否则设置sHeader为相应录像协议头，比如大华为DHPT
	bool GetRecordHeader(long lChannel, long& lSize, string& sHeader)
	{ 
		// 大华实时视频回调中没有视频头，伪造一个视频头即可
		lSize = 0;
		sHeader = "DHPT";
		return true;
	}

	//初始化云台，映射
	bool InitPTZCommand();


	//报警
	BOOL MessageNotify(LONG lCommand, char *pBuf,LDWORD dwUser);
	
	//报警接口
	BOOL ReceiveMessage(LONG lCommand,char* pBuf,LDWORD dwUser);
	//报警扩展接口
	BOOL ReceiveMessageEx(LONG lCommand,char* pBuf,LDWORD dwUser);

private:
	void TimeConvert(Time& t1, NET_TIME& t2, long lType)
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

	void GetRectForPlace(DH_RECT& rc, WORD nPlace);
	void GetPlaceForRect(DH_RECT& rc, WORD& nPlace);

	//记录注册用户到设备
	void RegisterObject();
	//清除已注销的用户
	void UnRegisterObject();

	void GetDHRect(DH_RECT& rc1, Rect& rc2);
	void GetHXRect(DH_RECT& rc1,Rect& rc2);

private:
	LONG user_id_;//保存设备ID

	Poco::Mutex rec_mutex_;
	std::map<long,VPlatformChannel> records;
	DWORD last_ptz_command_;//云台命令备份

	bool m_bNewVer;

	NET_DEVICEINFO m_dev_info;

	//add by sunminping   调整云台控制和预置位功能
private:
	BOOL PtzControl(long lChannel,int type,BOOL stop,int param);
	BOOL Extptzctrl(long lChannel,int nPresetPoint,DWORD dwCommand, DWORD dwParam);     //扩展接口
	//end add
};

//提供对象工厂
class VDDaHuaDVRMaker : public VirtualDeviceMaker
{
public:
	VDDaHuaDVRMaker();
	~VDDaHuaDVRMaker();

	VirtualDevice* Make();

	void Reclaim(VirtualDevice* vd);
};