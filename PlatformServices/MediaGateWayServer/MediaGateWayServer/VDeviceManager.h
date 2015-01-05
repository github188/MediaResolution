
#pragma once

#include "Poco/Timer.h"
#include "Poco/HashMap.h"
#include "Config.h"
#include "../VirtualDevice.h"
#include "MegaeyesHead.h"
#include "NetUtility.h"
#include "Operator.h"
#include "VDeviceServer.h"
#include "CmdServerModule.h"
#include "define.h"
#include "VPScanConfigFile.h"
using namespace ty;

class VDeviceServer;

class VDeviceManager
{
public:
	typedef std::map<std::string,VDeviceServer*> PlatformMap;
	typedef int (VDeviceManager::*RequestHandle)(const CmdHead& head,const unsigned char* pBuffer, StreamSocket& sock);

	static VDeviceManager& Instance(){return instance_;}
	~VDeviceManager();

	int AddVDeviceServer(VDeviceConfig& rConfig);
	void RemoveVDeviceServer(VDeviceConfig& rConfig);
	int InitVDeviceServer(VDeviceConfig& rConfig);
	VDeviceServer* Find(string deviceId);

protected:
	VDeviceManager();
	VDeviceManager(const VDeviceManager&);
	VDeviceManager& operator=(const VDeviceManager&);

public:
	//�����¼��ķַ�����
	int OnRecvRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock);

	int OnRealVideoRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock);
	//����10010ָ��
    int OnRealVideoStreamRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock);
	 //����10014ָ��
	int OnRecordVideoRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock);
	//����10016ָ��
	int OnRecordVideoStreamRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock);

	int OnGetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	int OnSetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	
	int OnGetAlarmOut(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	//����5064ָ��-��������ź�
	int OnSetAlarmOut(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	
	//����5046ָ��-����
	int OnNetParam(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	int OnSetVideoDectePlan(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	int OnSetAlarmPlan(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	int OnGetPtzStatus(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);
	//����5006ָ��
	int OnPtzCtrl(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op);

	//add by sunminping �ж�ָ�����豸IP�Ƿ���Ҫ��ӡ������־��Ϣ
	bool IsExistsIp(const string & ip);
	bool ListenAlarmInfo(string deviceid);
	//end add 

private:
	void InitHandle();

public:
	static VDeviceManager instance_;
	PlatformMap m_PlatformServerMap;
	std::map<int, RequestHandle> RequestHandles_;
	Poco::FastMutex m_Mutex;
	VPScanConfigFile m_ScanFile;
	VirtualDeviceFactory vd_factory_;
};
