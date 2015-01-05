/***********************************************************************
Author:sunminping
Date:2014-6-19
Function:����ɨ�������ļ�������������Ϣ�ж��Ƿ���Ҫ��ӡ������־
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