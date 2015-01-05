//´íÎó±¨¾¯¿ØÖÆÆ÷
//mlcai 2014-03-06

#ifndef ALARMERRORCONTROLLER_H
#define ALARMERRORCONTROLLER_H

#include "AlarmController.h"
#include "MegaeyesErrorNotify.h"

class AlarmErrorController : public AlarmController
{
public:
	AlarmErrorController();
	~AlarmErrorController();

	bool PostAlarm(const VDeviceConfig& vpconfig, long channel, int alarm_type);
	AlarmType GetAlarmType();
private:
	void run();

	ty::Queue<CMegaeyesErrorNotify> _error_queue;
};

#endif