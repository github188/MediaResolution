#include "stdafx.h"
#include "HeartBeatManager.h"
#include "Poco/ScopedLock.h"
#include <algorithm>
#include "VDeviceServer.h"
#include "DispatchServer/DistributionManager.h"
#include "CMD10002HeartReqSerialize.h"

class DistributionManager;
class DistributionTask;

HeartBeatManager::HeartBeatManager()
{
	is_stop_=false;
}

HeartBeatManager::~HeartBeatManager()
{
	is_stop_=true;
	workthread.join();
	Poco::ScopedLock<FastMutex> lock(m_HeartMutex);
	m_OnlineVec.clear();
}

void HeartBeatManager::Start(std::string strIp,int nPort)
{
    m_strSvrIp = strIp;
    m_nSvrPort = nPort;
    workthread.start(*this);
}

bool HeartBeatManager::CheckDeviceOnline(VDeviceConfig &vplatformCFG)
{
	int nPeriod = 1;
	int nNum = 0;
	Poco::Timestamp ts;
	while(nNum <3)
	{
		SocketAddress Address(vplatformCFG.m_sServerIP,vplatformCFG.m_lPort);
		StreamSocket  socket;
		Poco::Timespan timeout(0,nPeriod * 100*1000);
		try
		{  
			socket.connect(Address/*,timeout*/);
		}
		catch(...)
		{
			nPeriod+=2;
			nNum++;
			Poco::Thread::sleep(10);
			continue;
		}

		return true;
	}

	return false;
}


void HeartBeatManager::run()
{
	Sleep(10000);

    std::string strMsg= "";
    char buffer[1024];
	
	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

    while(!is_stop_)
    {
		vector<VDeviceConfig> deviceConfigsCopy;

		CmdServerModule::Instance().Copy(deviceConfigsCopy);

        CMD10002HeartReqSerialize devHeart;
        for (int i=0;i<(int)deviceConfigsCopy.size();i++)
        {
            if(deviceConfigsCopy[i].status == VDeviceConfig::Success) //注册成功的设备；
            {   
                bool ret;
                ret = CheckDeviceOnline(deviceConfigsCopy[i]);//检查前端设备是否在线；
                if(ret != true)
             		deviceConfigsCopy[i].status = VDeviceConfig::False;
				else
					deviceConfigsCopy[i].status = VDeviceConfig::Success;
            }
			else
				deviceConfigsCopy[i].status = VDeviceConfig::False;
			devHeart.m_deviceVec.push_back(deviceConfigsCopy[i]);
        }
		
		string content = devHeart.Serialize();

		CmdServerModule::Instance().SendToCmdServer((char*)content.c_str(), content.size(), devHeart.m_cmdHead.cmdseq, new CmdResponseCallbacker<HeartBeatManager>(this, &HeartBeatManager::OnHeartBeatResponse));

		Sleep(3000);
	}
}

void HeartBeatManager::OnHeartBeatResponse(const CmdHead& head, char* res_xml)
{
	CMarkup mark_xml;
	StrUtility utility;
	string success;

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	if (!(mark_xml.SetDoc(res_xml) && utility.ForeachXml(mark_xml) && utility.GetxmlValue("Information/Success", success)))
	{
		media_log.log(Message::PRIO_ERROR, "心跳上报返回xml格式错误:%s",  std::string(res_xml));
		return;
	}

	if (success != "0")
		media_log.log(Message::PRIO_ERROR, "心跳上报返回错误码:%s",  success);
}
