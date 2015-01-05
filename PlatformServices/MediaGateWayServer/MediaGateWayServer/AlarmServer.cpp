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
	//add by sunmining ��¼�±���ģ�������ʱ��   
	m_StartTime=GetTickCount();
	//end add 

    //�ƶ���ⱨ��
	_alarm_detect_controller.Start(config.accessIP, config.accessPort);
    //���뱨��
	_alarm_in_controller.Start(config.accessIP, config.accessPort);
    //���󱨾�
//	_alarm_error_controller.Start(config.accessIP, config.accessPort);
}

bool AlarmServer::NotifyAlarm(const VDeviceConfig& vpconfig, long channel, long alarm_type, LPCTSTR alarm_message)
{
	//add by sunminping �������������֮�ڲ���������Ϣ
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

