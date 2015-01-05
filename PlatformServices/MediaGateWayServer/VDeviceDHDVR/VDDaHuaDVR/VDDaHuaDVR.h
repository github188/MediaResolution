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
	// �ӿ�
public:
	//��ʼ��
	virtual int Init(VDeviceConfig& dev_config);
	virtual bool Destroy();

	//ʵʱ��Ƶ
	virtual bool PlayVideo(long lChannel,void* dm, bool bMainStrean = 0);
	//ֹͣʵʱ���
	virtual bool StopVideo(long lChannel,bool bMainStrean = 0);

	//��ȡ�����������
	bool GetNetParam(std::string& sIPAddr, std::string& sNetMask, std::string& sGateway);

	//��̨����
	virtual bool PTZControl(long lChannel,long lCommand, long lValue, void* pParam = NULL);

	//���������ϴ�ͨ��(����)
	bool SetupAlarmChannel();

	//�رձ����ϴ�ͨ����������
	void CloseAlarmChannel();

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
	bool SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,std::string& sWord);
	bool GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,std::string& sWord);

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


	//�������
	//bDectecting��Ϊ1��0�ֱ�Ϊ�򿪺͹ر��ƶ���⡣DetectGrade��Ϊ���������ȡ���СΪ0-6��0��ٶۣ�6��������
	//Rect��Ϊ�������Left��Top��Width��Height��С��352��288֮�䣬�����Ƶ�ֱ��ʲ�����352��288����Ƶ������Ӧ�Զ��������������š�
	//Ĭ�������û�������ƶ���⡣
	bool SetDetectRect(long lChannel,std::vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade, long nMaxRectNum);
	bool GetDetectRect(long lChannel,std::vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade, long& nMaxRectNum);

	//�ڵ�����
	//bLogoing��ֵΪ1��0�ֱ�Ϊ���ú�ȡ��ͼ���ڵ���
	//Rect����ʾ�ڵ�����Left��Top��Width��Height��С��352��288֮�䣬�����Ƶ�ֱ��ʲ�����352��288����Ƶ������Ӧ�Զ��������������š�
	//��������˶���ڵ����򣬽����ֶ��Rect��㡣Ĭ��û�������ڵ���
	bool SetShadeRect(long lChannel,std::vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum);
	bool GetShadeRect(long lChannel,std::vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum);

	//��ѯ¼��
	//rd: �ο�Record˵��
	bool QueryRecord(long lChannel, Poco::LocalDateTime& tStart, Poco::LocalDateTime& tEnd,vector<Record>& rds);

	//����¼��
	//lRequestType: �������ͣ�0��ʾֹͣ���أ�1��ʾ����
	//pos�������ص�λ�� mlcai 2014-05-07
	bool RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext, long& lData, int& pos);

	//��ȡ¼��ͷ
	//�������¼���ļ�����¼��ͷ����ֻ�践��ͷ����С����������sHeaderΪ��Ӧ¼��Э��ͷ�������ΪDHPT
	bool GetRecordHeader(long lChannel, long& lSize, string& sHeader)
	{ 
		// ��ʵʱ��Ƶ�ص���û����Ƶͷ��α��һ����Ƶͷ����
		lSize = 0;
		sHeader = "DHPT";
		return true;
	}

	//��ʼ����̨��ӳ��
	bool InitPTZCommand();


	//����
	BOOL MessageNotify(LONG lCommand, char *pBuf,LDWORD dwUser);
	
	//�����ӿ�
	BOOL ReceiveMessage(LONG lCommand,char* pBuf,LDWORD dwUser);
	//������չ�ӿ�
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

	//��¼ע���û����豸
	void RegisterObject();
	//�����ע�����û�
	void UnRegisterObject();

	void GetDHRect(DH_RECT& rc1, Rect& rc2);
	void GetHXRect(DH_RECT& rc1,Rect& rc2);

private:
	LONG user_id_;//�����豸ID

	Poco::Mutex rec_mutex_;
	std::map<long,VPlatformChannel> records;
	DWORD last_ptz_command_;//��̨�����

	bool m_bNewVer;

	NET_DEVICEINFO m_dev_info;

	//add by sunminping   ������̨���ƺ�Ԥ��λ����
private:
	BOOL PtzControl(long lChannel,int type,BOOL stop,int param);
	BOOL Extptzctrl(long lChannel,int nPresetPoint,DWORD dwCommand, DWORD dwParam);     //��չ�ӿ�
	//end add
};

//�ṩ���󹤳�
class VDDaHuaDVRMaker : public VirtualDeviceMaker
{
public:
	VDDaHuaDVRMaker();
	~VDDaHuaDVRMaker();

	VirtualDevice* Make();

	void Reclaim(VirtualDevice* vd);
};