//±¨¾¯ÊäÈë¿ØÖÆÆ÷
//mlcai 2014-03-07

#ifndef ALARMINCONTROLLER_H
#define ALARMINCONTROLLER_H

#include "AlarmController.h"

class AlarmInController : public AlarmController
{
public:
	AlarmInController();
	~AlarmInController();

	bool PostAlarm(const VDeviceConfig& vpconfig, long channel, int alarm_type);
	AlarmType GetAlarmType();
};

#endif