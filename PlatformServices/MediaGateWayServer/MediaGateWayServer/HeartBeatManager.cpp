#include "stdafx.h"
#include "HeartBeatManager.h"
#include "Poco/ScopedLock.h"
#include <algorithm>
#include "VDeviceServer.h"
#include "DispatchServer/DistributionManager.h"
#include "Megaeyes40000ReqSerialize.h"
#include "Megaeyes2ReqSerialize.h"

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
    SocketAddress Address(m_strSvrIp,m_nSvrPort);

    std::string strMsg= "";
    char buffer[1024];
	MegaeyesResponseHead *pResponseHead;
	
	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

    while(!is_stop_)
    {

		vector<VDeviceConfig> deviceOffLine;
        Megaeyes40000ReqSerialize devOnlineHeart;
        for (int i=0;i<(int)_deviceConfigsCopy.size();i++)
        {
            if(_deviceConfigsCopy[i].status == VDeviceConfig::Success) //注册成功的设备；
            {   
                bool ret;
                ret = CheckDeviceOnline(_deviceConfigsCopy[i]);//检查前端设备是否在线；
                if(ret == true)
                {
                    devOnlineHeart.m_OnlineVec.push_back(_deviceConfigsCopy[i]);
                }
				else
				{
					_deviceConfigsCopy[i].status = VDeviceConfig::False;
					deviceOffLine.push_back(_deviceConfigsCopy[i]);
				}
            }
        }

		if (devOnlineHeart.m_OnlineVec.size() == 0)
		{
			CmdServerModule::Instance().NotifyFromHeartCtrl(deviceOffLine);
			Sleep(100);
			continue;
		}

        if(CmdServerModule::Instance().m_config.supportPlatform == 0) //new heart;
        {
			StreamSocket  socket;
			try
			{
				StreamSocket  socket_imp(Address);	
				socket = socket_imp;
			}
			catch (...)
			{
				transLog.log(Message::PRIO_ERROR,"接入断线了!");
				Sleep(500);
				continue;
			}
            string content =  devOnlineHeart.Serialize();
            if((content.length()==NetUtility::SafeSendData(socket,(char*)content.c_str(),content.length())) && 
				NetUtility::Receive(socket,(char *)buffer,sizeof(MegaeyesResponseHead)))
            {
                //
            }
			else
            {
                //
				for (int i=0;i<(int)devOnlineHeart.m_OnlineVec.size();i++)
				{
					deviceOffLine.push_back(devOnlineHeart.m_OnlineVec[i]);
				}
            }
        }

        if(CmdServerModule::Instance().m_config.supportPlatform == 1) //old heart;
        {
            for(int i=0;i< devOnlineHeart.m_OnlineVec.size();i++)
            {   
				StreamSocket  socket;
				try
				{
					StreamSocket  socket_imp(Address);	
					socket = socket_imp;
				}
				catch (...)
				{
					transLog.log(Message::PRIO_ERROR,"接入断线了!");
					Sleep(500);
					continue;
				}
                Megaeyes2ReqSerialize devOnlineOldHeart;
                devOnlineOldHeart.onlineDeviceWorkID = devOnlineHeart.m_OnlineVec[i].workID;
                string content =  devOnlineOldHeart.Serialize();
                if((content.length()==NetUtility::SafeSendData(socket,(char*)content.c_str(),content.length())) &&
					NetUtility::Receive(socket,(char *)buffer,sizeof(MegaeyesResponseHead)))
                {
                }
				else
				{
                    deviceOffLine.push_back(devOnlineHeart.m_OnlineVec[i]);
                }
            }
        }

		CmdServerModule::Instance().NotifyFromHeartCtrl(deviceOffLine);

		Sleep(3000);
	}
}
