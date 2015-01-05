
#include <cstdlib>
#include "LogInfo.h"

LogInfo LogInfo::instance_;

string LogInfo::MediaGateWayServer()
{
	char buf[256];
	sprintf(buf,"MediaGateWayServer_%d", _pid);
	std::string str;
	str.assign(buf);
	return str;
}

string LogInfo::VirtualDevice()
{
	char buf[256];
	sprintf(buf,"VPlatform_%d", _pid);
	std::string str;
	str.assign(buf);
	return str;
}

string LogInfo::AlarmIn()
{
	char buf[256];
	sprintf(buf,"AlarmIn_Info");
	std::string str;
	str.assign(buf);
	return str;
}

string LogInfo::AlarmDetect()
{
	char buf[256];
	sprintf(buf,"AlarmDetect_Info");
	std::string str;
	str.assign(buf);
	return str;
}

