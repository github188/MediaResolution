
#pragma once

#include "VDeviceConfig.h"
#include "VDeviceManager.h"
#include "CmdServerModule.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "Poco/ScopedLock.h"

class RegisterManager : public Runnable
{
public:
  RegisterManager();
  ~RegisterManager();

public:
	void Start();
private:
	Poco::FastMutex m_Mutex;
	bool is_stop_;
	Poco::Thread workthread;

private:
	void OnRegisterResponse(const CmdHead& head, char* res_xml, int index);
	void OnAbilityPostResponse(const CmdHead& head, char* res_xml, int index);

public:
    void run();
    vector<VDeviceConfig> deviceConfigsCopy;

};

