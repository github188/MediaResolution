#pragma once
#include <string>
#include <iostream>
#include "Poco/Mutex.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

#include "NetUtility.h"
#include "VDeviceServer.h"
#include "MegaeyesHead.h"
#include "Markup.h"

#include "LogInfo.h"
#include "LogSys.h"
#include "VDeviceConfig.h"

class VDeviceServer;

#define READ_RECORD_BUFFER 64*1024
class VDeviceRecordTask:public Poco::Runnable  
{
public:
    VDeviceRecordTask(VDeviceServer* pVPServer, string key, const CmdHead& reques_head, VirtualDevice* pVPlatform, char *pBuf, int channel);
    ~VDeviceRecordTask();
    bool NotifyRecordData(long lRecordType, int success, PBYTE pBuffer, DWORD dwSize);
	bool NewVPlatform(VDeviceConfig& rConfig);
    void run();

	void Start();

	void Stop();                 //通知停止，然后开始释放

	VirtualDevice* pVPlatformRecord;

private:
	int AddVPlatform(VDeviceConfig& rConfig);
	void ReleaseVPlatform();
	int InitVPlatform(VDeviceConfig& rConfig);
private:
	bool doRun();

	int doSuccess(BYTE* buf,DWORD nLen,DWORD nHeadLen,BOOL& bFirst);
private:
	bool isStop;
	Poco::Thread work_thread_;
    HANDLE m_hReadFile;
    
    char *pBuffer;
    int lChannel;
	long lSeq;

	CmdHead request_head_;

	string _key;
	VDeviceServer* pVPlatformServer;      //add by sunmiping 2014-6-13
public:
    StreamSocket socket;
    char file_extend[5];  //ligc++ 2014-06-23 加入文件扩展名可配置，解决宇视科技扩展名只能是.ts问题
};