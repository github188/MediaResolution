#ifndef ALARM_SERVER_H
#define ALARM_SERVER_H
#include "Poco/Timer.h"
#include "Poco/Mutex.h"
#include "Config.h"
#include "AlarmDetectController.h"
#include "AlarmInController.h"
#include "AlarmErrorController.h"

class Megaeyes5214ReqSerialize;
class AlarmServer
{
public:
	static AlarmServer& Instance(){return instance_;}
	~AlarmServer();
	void Start(const Config& config);

	bool NotifyAlarm(const VDeviceConfig& vpconfig, long channel, long alarm_type, LPCTSTR alarm_message = 0);

protected:
	AlarmServer();
	AlarmServer(const AlarmServer&);
	AlarmServer& operator=(const AlarmServer&);

private:
	static AlarmServer instance_;
	AlarmDetectController _alarm_detect_controller;
	AlarmInController _alarm_in_controller;
	AlarmErrorController _alarm_error_controller;
	DWORD m_StartTime;
};
#endif