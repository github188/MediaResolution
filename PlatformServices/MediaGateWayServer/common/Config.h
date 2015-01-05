#ifndef CONFIG_H
#define CONFIG_H
#include "Markup.h"
class Config
{
public:
	Config()
	{
        localIP1 = "";
        localIP2 = "";
        accessIP = "";
        accessPort = 6005;
        startPort = 0;
        rebootTime = "";
        rebootCycle = 0;
        supportAlarm = 0;
        supportPlatform = 0; //0: new platform,1: old platform
        nPid = 0;
	}

	~Config()
	{

	}


public:
	std::string localIP1;
	std::string localIP2;
	std::string accessIP;
	int accessPort;
	int startPort;
    std::string rebootTime;
    int rebootCycle;
	int supportAlarm;
    int supportPlatform;
    int nPid;
	std::string m_logLevel;
	std::string center_ip_;
	int center_port_;
	std::string center_uri_;
	std::string server_id_;
};
#endif