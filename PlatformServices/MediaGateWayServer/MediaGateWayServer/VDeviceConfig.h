#ifndef VDeviceConfig_H
#define VDeviceConfig_H

#include <map>
#include <string>
using namespace std;

class MediaChannel;
class InputChannel;
class OutputChannel;

class VDeviceConfig
{
public:
	map<int, string> MediaChannls;
	int max_in_;
	int max_out_;

	enum
	{   
		False,
		Success,
	};

public:
    VDeviceConfig()
    {
		status = VDeviceConfig::False;
        supportAlarm = 0;
        m_lSupportMutiStream = 0;
        m_lPort = 0;
		workID="";
		m_nCameraCount=0;
    }
public:
	string workID;
	int status; //注册是否成功
	string m_sClassName;
	string m_sProtocol;

    string m_sPlatformName;
    string m_sDeviceId;
    string m_sServerIP;			//IP地址
    long m_lPort;				//端口
    string m_sUser;				//用户名
    string m_sPasswd;			//密码
    long supportAlarm;			//是否支持报警
    long m_lSupportMutiStream;  //是否支持多码流
    string m_sExtendPara1;

	int m_nCameraCount;         //通道个数


    VDeviceConfig(const VDeviceConfig& config)
    {
        Copy(config);
    }

    VDeviceConfig& operator =(const VDeviceConfig& config)
    {
        Copy(config);
        return *this;
    }

    void Copy(const VDeviceConfig& config)
    {
        m_sPlatformName = config.m_sPlatformName;
        m_sDeviceId = config.m_sDeviceId;
        m_sServerIP = config.m_sServerIP;
        m_lPort = config.m_lPort;
        m_sUser = config.m_sUser;
        m_sPasswd = config.m_sPasswd;
        supportAlarm = config.supportAlarm;
        m_lSupportMutiStream = config.m_lSupportMutiStream;
		m_sExtendPara1=config.m_sExtendPara1;

		m_sClassName = config.m_sClassName;
		m_sProtocol = config.m_sProtocol;
		status = config.status;
		workID = config.workID;
		MediaChannls = config.MediaChannls;
		max_in_ = config.max_in_;
		max_out_ = config.max_out_;
		m_nCameraCount=config.m_nCameraCount;
    }

};

class MediaChannel
{ 
public:
	MediaChannel()
	{
		DeviceID = "";
		WorkID = ""; 
		Naming = "";
		ChannelID = 0;
	}
	~MediaChannel()
	{

	}

public:
	string DeviceID;
	string WorkID;
	string Naming;
	int ChannelID;
};

class InputChannel
{   
public:
	InputChannel()
	{
		DeviceID = "";
		WorkID = "";
		Naming = "";
		ChannelID = 0;
	}
	~InputChannel()
	{

	}

public:
	string DeviceID;
	string WorkID;
	string Naming;
	int ChannelID;
};  

class OutputChannel
{   
public:
	OutputChannel()
	{
		DeviceID = "";
		WorkID = "";
		Naming = "";
		ChannelID = 0;
	}
	~OutputChannel()
	{

	}
public:
	string DeviceID;
	string WorkID;
	string Naming;
	int ChannelID;
};

#endif