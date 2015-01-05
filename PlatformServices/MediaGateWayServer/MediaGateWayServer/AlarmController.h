//±¨¾¯¿ØÖÆÆ÷
//mlcai 2014-03-06

#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include <string>
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Queue.h"
#include "NetUtility.h"
#include "MegaeyesHead.h"
#include "Megaeyes5214ReqSerialize.h"
#include "VDeviceConfig.h"
#include "VirtualDevice.h"

class AlarmController : public Poco::Runnable
{
public:
	virtual ~AlarmController();

	void Start(const std::string& access_ip, int acess_port);

	virtual bool PostAlarm(const VDeviceConfig& vpconfig, long channel, int alarm_type) = 0;

	virtual AlarmType GetAlarmType()=0;

protected:
	AlarmController();

	virtual void run();

	void printFailedAlarmInfo(string fileName,string failedInfo);  //add by sunminping 2014-7-17

	Poco::Thread _post_thr;
	ty::Queue<Megaeyes5214ReqSerialize> _alarm_queue;
	std::string _access_ip;
	int _access_port;
private:
	bool is_stop_;
};

#endif
