#ifndef HEART_BEAT_CONTROLLER_H
#define HEART_BEAT_CONTROLLER_H
#include "Poco/Mutex.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include <vector>
#include "VDeviceConfig.h"
#include "CmdServerModule.h"
#include "VDeviceManager.h"


class HeartBeatManager:public Poco::Runnable  
{
public:
	HeartBeatManager();

	virtual ~HeartBeatManager();

	void Start(std::string strIp,int nPort);
	

protected:
	typedef std::vector<std::string> DevVec;//视频服务器业务ID数组
	DevVec m_OnlineVec;
	std::string m_strSvrIp;
	int m_nSvrPort;
	Poco::FastMutex m_HeartMutex;
	
public:
    void run();
    bool CheckDeviceOnline(VDeviceConfig &vplatformCFG);

	void OnHeartBeatResponse(const CmdHead& head, char* res_xml);

private:
	Poco::Thread workthread;
	bool is_stop_;
};

#endif
