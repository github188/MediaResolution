#ifndef LOGINFO_H
#define LOGINFO_H

#include <string>

using namespace std;

class LogInfo
{
public:
	static LogInfo& Instance(){return instance_;}
	~LogInfo() {}

	string MediaGateWayServer();
	string VirtualDevice();
	string AlarmIn();
	string AlarmDetect();

	int _pid;

protected:
	LogInfo() {}
	LogInfo(const LogInfo&);
	LogInfo& operator=(const LogInfo&);

private:
	static LogInfo instance_;
};

#endif