// MediaGateWayServer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
//#include "vld.h"
#include "Poco/Path.h"
using Poco::Path;
#include <Windows.h>
#include "Poco/Path.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/File.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Environment.h"
#include <iostream>
#include "Poco/Timestamp.h"
#include "Poco/SharedMemory.h"
#include "Poco/Timer.h"
#include "LogSys.h"
#include "CmdServerModule.h"
#include "VDeviceManager.h"
#include "MediaGateWayServer.h"
#include "AlarmServer.h"
#include "LogInfo.h"
#include <Windows.h>
#include <imagehlp.h>
#include "../common/ShareMemory.h"

#ifdef WIN32
	#ifdef _DEBUG
		#pragma comment(lib, "ServerLibraryd.lib")
		#pragma comment(lib, "PocoFoundationd.lib")
		#pragma comment(lib, "PocoNetd.lib")
	#else
		#pragma comment(lib, "ServerLibrary.lib")
		#pragma comment(lib, "PocoFoundation.lib")
		#pragma comment(lib, "PocoNet.lib")
	#endif
    //#pragma comment(lib, "MegaTypeLib.lib")
#endif

#pragma comment(lib, "dbghelp.lib")

LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp)
{
	printf("crash!\n");
	CString file_name;
	file_name.Format(_T("c:\\error_trans%d.dmp"), ::GetCurrentProcessId());
	::GetCurrentProcessId();
	HANDLE hFile = ::CreateFileA(
		file_name, 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId			= ::GetCurrentThreadId();
		einfo.ExceptionPointers	= pExp;
		einfo.ClientPointers	= FALSE;

		::MiniDumpWriteDump(
			::GetCurrentProcess(), 
			::GetCurrentProcessId(), 
			hFile, 
			MiniDumpWithFullMemory, 
			&einfo, 
			NULL, 
			NULL);
		::CloseHandle(hFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void StartUnhandledExceptionFilter()
{
	::SetUnhandledExceptionFilter(ExpFilter);
}

#ifdef WIN32
static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	TerminateProcess(GetCurrentProcess(), 0);
	return TRUE;
}

void InitConsole()
{

	SetConsoleCtrlHandler(HandlerRoutine, TRUE);

	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE); 

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hOutput, &info);
	COORD cr;
	cr.X = info.dwSize.X;
	cr.Y = 5000;
	SetConsoleScreenBufferSize(hOutput, cr);

//	SetConsoleTitle(_T("PasswordServer( 提示:避免长按滚动条，以免导致程序输出阻塞 )"));

}
#endif


bool OpenGlobalShare(const char * name,HANDLE& _hShareMapping,smp::ShareMemory*& _pShareData)
{
	_hShareMapping = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, name);
	if (_hShareMapping == NULL)
	{
		return false;
	}
	_pShareData = (smp::ShareMemory*)MapViewOfFile(_hShareMapping, FILE_MAP_WRITE, 0, 0, sizeof(smp::ShareMemory));
	return _pShareData != NULL;
}

void CloseGlobalShare(HANDLE& _hShareMapping,smp::ShareMemory*& _pShareData)
{
	cout<<"调用函数CloseGlobalShare!!!"<<endl;
	if (_pShareData != NULL)
	{
		UnmapViewOfFile((LPVOID)_pShareData);
		_pShareData = NULL;
	}

	if (_hShareMapping != NULL)
	{
		CloseHandle(_hShareMapping);
		_hShareMapping = NULL;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	StartUnhandledExceptionFilter(); //产生dump文件 mlcai 2014-0428
#ifdef WIN32
	InitConsole();
#endif

	std::string strFilePath;
	std::string strShareName;
	std::string strCameraCount;
	int nPid=1;

	if (strncmp(argv[1],"-c",2)==0)
	{
		strShareName = argv[2];
	}

	if (strncmp(argv[3],"-t",2)==0)
	{
		strFilePath = argv[4];
	}

	if (strncmp(argv[5],"-p",2)==0)
	{
		nPid = atoi(argv[6]);
	}

	if(strncmp(argv[7],"-cc",2)==0)
	{
		strCameraCount=argv[8];
	}

	strFilePath.clear();
	char fullFileName[1024];
	::GetModuleFileNameA(0, fullFileName, sizeof(fullFileName));
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(fullFileName, drive, dir, NULL, NULL);
	strFilePath += drive;
	strFilePath += dir;

	LogInfo::Instance()._pid = nPid;

	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer(), true);
	transLog.setLogLevel("debug");

	LogSys& vp_log = LogSys::getLogSys(LogInfo::Instance().VirtualDevice(), true);
	vp_log.setLogLevel("debug");

	LogSys& alarm_in_Log = LogSys::getLogSys(LogInfo::Instance().AlarmIn(), true);
	alarm_in_Log.setLogLevel("debug");

	LogSys& alarm_detect_log = LogSys::getLogSys(LogInfo::Instance().AlarmDetect(), true);
	alarm_detect_log.setLogLevel("debug");

	//add by sunminping 2014-8-29
	HANDLE hShareHandle;
	smp::ShareMemory* pShareData=NULL;

	if(!OpenGlobalShare(strCameraCount.c_str(),hShareHandle,pShareData))
		transLog.log(Message::PRIO_ERROR,"打开共享内存%s失败!!!",strCameraCount);
	else
	{
		transLog.log(Message::PRIO_ERROR,"打开共享内存%s成功!!!",strCameraCount);
		pShareData->nCameraCount=0;
	}
	//end add

	if (CmdServerModule::Instance().Start(strFilePath, nPid, pShareData)==false)
	{
		transLog.log(Message::PRIO_ERROR, "CmdServerModule start failed");
		return false;
	}
	transLog.log(Message::PRIO_INFORMATION, "CmdServerModule start ok......");

	AlarmServer::Instance().Start(CmdServerModule::Instance().m_config);

	transLog.log(Message::PRIO_INFORMATION, "AlarmServer start ok......");

	MediaGateWayServer::Instance().Start(CmdServerModule::Instance().m_config);

	transLog.log(Message::PRIO_INFORMATION, "MediaGateWayServer start ok......");
	
	while(true)
	{
		std::string str;
		cin>>str;

		if (str=="quit")
		{
			break;
		}
	}
	
	CmdServerModule::Instance().Stop();
	Logger::shutdown();
	CloseGlobalShare(hShareHandle,pShareData);
	return 0;
}
