/***********************************************************************
Author:sunminping
Date:2014-6-19
Function:监视扫描配置文件，根据配置信息判断是否需要打印报警日志
***********************************************************************/


#include "VPScanConfigFile.h"
#include "Markup.h"


VPScanConfigFile::VPScanConfigFile():m_Timer(0,2000)
{
	
}


void VPScanConfigFile::Start()
{
	m_Timer.start(TimerCallback<VPScanConfigFile>(*this,&VPScanConfigFile::OnTimer));
}


void VPScanConfigFile::Stop()
{
	m_Timer.stop();
}


void VPScanConfigFile::OnTimer(Poco::Timer& timer)
{
	CMarkup xml;
	xml.Load("./Debug.xml");
	if(!xml.FindElem("Message")) return;

	m_IPContainer.clear();

	xml.IntoElem();
	string ip;
	m_Mutex.lock();
	while(xml.FindElem("IP"))
	{
		ip=xml.GetData();
		m_IPContainer.push_back(ip);
	}
	m_Mutex.unlock();
	xml.OutOfElem();

	return;
}


bool VPScanConfigFile::IsExistsIp(const string& ip)
{
	bool bRet=false;
	m_Mutex.lock();
	list<string> tmpContainer=m_IPContainer;
	m_Mutex.unlock();

	list<string>::const_iterator it=tmpContainer.begin();
	list<string>::iterator end=tmpContainer.end();
	while(it!=end)
	{
		if(*it==ip)
		{
			bRet=true;
			break;
		}
		++it;
	}
	return bRet;
}