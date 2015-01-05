#include "stdafx.h"
#include "AlarmServer.h"
#include "Poco/ScopedLock.h"
#include "../VirtualDevice.h"

AlarmServer AlarmServer::instance_;

AlarmServer::AlarmServer()
{
	m_StartTime=0;
}

AlarmServer::~AlarmServer()
{

}

void AlarmServer::Start(const Config& config)
{    
	//add by sunmining 记录下报警模块的启动时间   
	m_StartTime=GetTickCount();
	//end add 

    //移动侦测报警
	_alarm_detect_controller.Start(config.accessIP, config.accessPort);
    //输入报警
	_alarm_in_controller.Start(config.accessIP, config.accessPort);
    //错误报警
//	_alarm_error_controller.Start(config.accessIP, config.accessPort);
}

bool AlarmServer::NotifyAlarm(const VDeviceConfig& vpconfig, long channel, long alarm_type, LPCTSTR alarm_message)
{
	//add by sunminping 进程启动五分钟之内不处理报警信息
	if(GetTickCount()-m_StartTime<=5*60*1000)
		return true;
	//end add

	if (alarm_camera == alarm_type)
	{
		return _alarm_detect_controller.PostAlarm(vpconfig, channel, alarm_type);
	}
	else if (alarm_in == alarm_type)
	{
		return _alarm_in_controller.PostAlarm(vpconfig, channel, alarm_type);
	}
	//else
	//	_alarm_error_controller.PostAlarm(vpconfig, channel ,alarm_type);
}

