#ifndef LOGINFO_H
#define LOGINFO_H

#include <string>

using namespace std;

class LogInfo
{
public:
	static LogInfo& Instance(){return instance_;}
	virtual ~LogInfo() {}

	string ProtocolDoorGateway();


protected:
	LogInfo() {}
	LogInfo(const LogInfo&);
	LogInfo& operator=(const LogInfo&);

private:
	static LogInfo instance_;
};

#endif