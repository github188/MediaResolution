/***********************************************************************
Author:sunminping
Date:2014-6-19
Function:监视扫描配置文件，根据配置信息判断是否需要打印报警日志
***********************************************************************/

#ifndef _H_VPWATCHFILE_H
#define _H_VPWATCHFILE_H

#include "Poco/Timer.h"
#include "Poco/Mutex.h"
#include "Poco/Thread.h" 
#include <list>
using namespace std;

using Poco::Timer;
using Poco::TimerCallback;

class VPScanConfigFile
{

public:
	VPScanConfigFile();
	void Start();
	void Stop();
	bool IsExistsIp(const string& ip);

private:
	void OnTimer(Poco::Timer& timer);

	Poco::Timer m_Timer;
	list<string> m_IPContainer;
	Poco::Mutex m_Mutex;
};

#endif