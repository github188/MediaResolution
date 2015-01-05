//实现AlarmInController
//mlcai 2014-03-07

#include "AlarmInController.h"
#include "VDeviceManager.h"

AlarmInController::AlarmInController()
{
}

AlarmInController::~AlarmInController()
{
}

bool AlarmInController::PostAlarm(const VDeviceConfig& vpconfig, long channel, int alarm_type)
{
	int channel_no = channel + 1;

	//通过通道号找到Naming
	Megaeyes5214ReqSerialize alarm_message;
	

	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
	if(alarm_message.m_strDevNaming=="")
	{
		if(VDeviceManager::Instance().IsExistsIp(vpconfig.m_sServerIP))
			transLog.log(Message::PRIO_DEBUG,"%s %s 报警:%d，报警类型:alarm_in 对应的Naming为空",vpconfig.m_sPlatformName,vpconfig.m_sServerIP,channel_no);	
		return false;
	}

	memcpy(alarm_message.m_cmdHead.src, vpconfig.workID.c_str(), sizeof(alarm_message.m_cmdHead.src));
	alarm_message.m_nDevType = TYPE_ALARM;
	Poco::LocalDateTime time;
	alarm_message.m_strAlarmTime=Poco::DateTimeFormatter::format(time,Poco::DateTimeFormat::SORTABLE_FORMAT);
	char LTime[256];
	sprintf(LTime,"%d%02d%02d%02d%02d%02d",time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second());
	alarm_message.m_strNaming.assign(LTime);
	alarm_message.m_strNaming= alarm_message.m_strNaming + "_" +  "6" + "_" + alarm_message.m_strDevNaming;

	_alarm_queue.Push(alarm_message);
	return true;
}

AlarmType AlarmInController::GetAlarmType()
{
	return alarm_in;
}

