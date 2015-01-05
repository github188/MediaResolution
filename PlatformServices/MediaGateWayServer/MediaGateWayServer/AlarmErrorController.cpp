//报警错误控制器实现
//mlcai 2014-03-06

#include "AlarmErrorController.h"
#include "VirtualDevice.h"

AlarmErrorController::AlarmErrorController()
{
}

AlarmErrorController::~AlarmErrorController()
{
}

bool AlarmErrorController::PostAlarm(const VDeviceConfig& vpconfig, long channel, int alarm_type)
{
	/*
	CMegaeyesErrorNotify errorNotify;

	Poco::LocalDateTime time;
	switch(alarm_type)
	{
	case alarm_video_lost:
		errorNotify.m_nDeviceType = TYPE_CAMERA;
		errorNotify.m_nFaultType= 4;
		errorNotify.m_strFaultID = "004";

		break;
	case alarm_disk_full:
		errorNotify.m_nDeviceType = TYPE_TERMINAL;
		errorNotify.m_nFaultType= 1;
		errorNotify.m_strFaultID = "502";
		break;
	case alarm_disk_not_format:
		errorNotify.m_nDeviceType = TYPE_TERMINAL;
		errorNotify.m_nFaultType= 1;
		errorNotify.m_strFaultID = "007";
		break;
	case alarm_disk_fault:
		errorNotify.m_nDeviceType = TYPE_TERMINAL;
		errorNotify.m_nFaultType= 1;
		errorNotify.m_strFaultID = "008";

		break;
	default:
		return;
	}
	errorNotify.m_strNaming = vpconfig.m_sPlatformName;
	memcpy(errorNotify.m_cmdHead.m_cSrcID,registerResponse.m_strID.c_str(),sizeof(errorNotify.m_cmdHead.m_cSrcID));
	errorNotify.m_strGatherTime = Poco::DateTimeFormatter::format(time,Poco::DateTimeFormat::SORTABLE_FORMAT);
	errorNotify.m_strIP = m_pPlatform->m_Config.m_sServerIP;
	errorNotify.m_strFaultTime = errorNotify.m_strGatherTime;
	//errorNotify.m_strDeviceID = camera->m_strDeviceId;
	*/
	return true;
}

void AlarmErrorController::run()
{
	char buffer[1024];
	MegaeyesResponseHead *pResponseHead;
	while (true)
	{
		CMegaeyesErrorNotify alarm_message;

		_error_queue.Pop(alarm_message);

		Net::SocketAddress address(_access_ip, _access_port);

		Net::StreamSocket socket;

		try
		{
			StreamSocket  socket_imp(address);	
			socket = socket_imp;
		}
		catch (...)
		{
			break;
		}
		string content =  alarm_message.Serialize();
		if(content.length()!=NetUtility::SafeSendData(socket,(char*)content.c_str(),content.length()))
		{
			//g_log<<"MediaGateWayServer send heart(new) failed\n";
		}
		pResponseHead = (MegaeyesResponseHead*)buffer;
		if(!NetUtility::Receive(socket,(char *)buffer,sizeof(MegaeyesResponseHead)))
		{
			//g_log<<"MediaGateWayServer receive heart(new) failed\n";
		}
		if(pResponseHead->success != 0)
		{
			//g_log<<"MediaGateWayServer heart(new) failed\n";
			continue;
		}     
		if(!NetUtility::Receive(socket,(char *)buffer,pResponseHead->length))
		{
			//g_log<<"MediaGateWayServer receive heart(new) failed\n";
		}
	}
}

AlarmType AlarmErrorController::GetAlarmType()
{
	return (AlarmType)-1;
}