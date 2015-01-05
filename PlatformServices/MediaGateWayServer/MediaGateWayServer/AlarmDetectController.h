//ÒÆ¶¯Õì²â±¨¾¯¿ØÖÆÆ÷
//mlcai 2014-03-7

#ifndef ALARMDETECTCONTROLLER_H
#define ALARMDETECTCONTROLLER_H

#include "AlarmController.h"

class AlarmDetectController : public AlarmController
{
public:
	AlarmDetectController();
	~AlarmDetectController();

	bool PostAlarm(const VDeviceConfig& vpconfig, long channel, int alarm_type);
	AlarmType GetAlarmType();
};

#endif
