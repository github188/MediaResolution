#ifndef CMD_SERVER_H
#define CMD_SERVER_H

#include "Poco/Timer.h"
#include "Poco/SharedPtr.h"
#include "Poco/HashMap.h"
#include "Poco/Net/StreamSocket.h"
#include "Config.h"
#include "CmdHead.h"
#include "VDeviceConfig.h"
#include <vector>
#include "LogSys.h"
#include "../common/ShareMemory.h"

using namespace std;

class CmdResponseCallback
{
public:
	virtual ~CmdResponseCallback() {}

	virtual void OnResponse(const CmdHead& res_head, char* res_xml)  = 0;

protected:
	CmdResponseCallback() {}
};

template<class Callbacker>
class CmdResponseCallbacker : public CmdResponseCallback
{
public:
	typedef void (Callbacker::*OnCmdResponse)(const CmdHead& res_head, char* res_xml);

	CmdResponseCallbacker(Callbacker* caller, OnCmdResponse OnCall) : 
	caller_(caller), OnCall_(OnCall)
	{
	}

	void OnResponse(const CmdHead& res_head, char* res_xml)
	{
		if (caller_)
		{
			(caller_->*OnCall_)(res_head, res_xml);
		}
	}

private:
	Callbacker* caller_;
	OnCmdResponse OnCall_;
};

template<class Callbacker, class P>
class CmdResponseProcessor : public CmdResponseCallback
{
public:
	typedef void (Callbacker::*OnCmdResponse)(const CmdHead& res_head, char* res_xml, P param);

	CmdResponseProcessor(Callbacker* caller, OnCmdResponse OnCall, P param) : 
	caller_(caller), OnCall_(OnCall), call_param_(param)
	{
	}

	void OnResponse(const CmdHead& res_head, char* res_xml)
	{
		if (caller_)
		{
			(caller_->*OnCall_)(res_head, res_xml, call_param_);
		}
	}

private:
	Callbacker* caller_;
	OnCmdResponse OnCall_;
	P call_param_;
};

class RegisterManager;
class HeartBeatManager;

class CmdServerModule : public Runnable
{
public:
	static CmdServerModule& Instance(){return instance_;}
	~CmdServerModule();
	bool Start(std::string strFilePath, int index, smp::ShareMemory* share);
	void Stop();
protected:
	CmdServerModule();
	CmdServerModule(const CmdServerModule&);
	CmdServerModule& operator=(const CmdServerModule&);

private:
	static CmdServerModule instance_;
	HeartBeatManager* heart_manager_;
	RegisterManager* register_manager_;
	smp::ShareMemory* pShareObject;

	FastMutex callback_mtx_;
	HashMap< int, SharedPtr<CmdResponseCallback> > callbacks_;
public:
    map<string,VDeviceConfig> _deviceConfigs;
    void Notify(vector<VDeviceConfig> &deviceCFGCopy);
	void Copy(vector<VDeviceConfig> &deviceCFGCopy);

	void SetDeviceConfigStatus(string device_id);

	bool SendToCmdServer(char* content, int length, int cmd_seq, CmdResponseCallback* call_back);

	void AddCmdCallback(int cmd_seq, CmdResponseCallback* call_back);
	void RemoveCmdCallback(int cmd_seq);

    map<string,string> channelMap;
    map<string,string> inputMap;
    map<string,string> outputMap;
	map<string, string> device_map_;
    string FindByChannelID(string workID);
    string FindByInputID(string workID);
    string FindByOutputID(string workID);
	string FindByDeviceWorkID(string work_id);
	VDeviceConfig FindByDeviceID(string deviceID);
	
	bool QueryDeviceInfoFromCenter(const string& server_id, string& res_xml);

private:
	void run();
	Thread conn_thr_;
	bool is_reading_;

public:
    Config m_config;
	FastMutex _mutex;
	Net::StreamSocket conn_socket_;
};
#endif
