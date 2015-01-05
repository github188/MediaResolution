#ifndef TRANSLATOR_SERVER_CONFIG_H
#define TRANSLATOR_SERVER_CONFIG_H
#include "Markup.h"
#include "VPlatformConfig.h"
class CTranslatorServerConfig
{
public:
	CTranslatorServerConfig()
	{
		Clear();
	}
	~CTranslatorServerConfig()
	{
		Destory();
	}
	void Clear()
	{
	
		m_strAccessIP="";
	
		m_VPlatformVecInfo.clear();
	}
	void Destory()
	{
		m_VPlatformVecInfo.clear();
	}
	bool OpenFile(std::string strFilePath)
	{
		CMarkup xml;
		if (xml.Load(strFilePath)==false)
		{
			//g_log<<"open vplatform.xml failed\n";
			return false;
		}
		if (xml.FindElem("Config")==false)
		{
			return false;
		}
		m_strAccessIP=xml.GetAttrib("AccessIP");
		m_nSupportParam=atoi(xml.GetAttrib("SupportParam").c_str());
		m_nSupportAlarm=atoi(xml.GetAttrib("SupportAlarm").c_str());
		m_nStartListenPort = atoi(xml.GetAttrib("StartPort").c_str());
		xml.IntoElem();
		if (xml.FindElem("VPlatforms")==false)
		{
			return false;
		}
		xml.IntoElem();
		while (xml.FindElem("VPlatform"))
		{
			//xml.IntoElem();
			VPlatformConfig vplatform;
			vplatform.m_sDeviceId=xml.GetAttrib("DeviceID");
			vplatform.m_sExtendPara1=xml.GetAttrib("ExtendPara1");
			vplatform.m_sPasswd=xml.GetAttrib("Passwd");
			vplatform.m_sPlatformName=xml.GetAttrib("PlatformName");
			vplatform.m_sServerIP=xml.GetAttrib("IP");
			vplatform.m_sUser=xml.GetAttrib("User");
			vplatform.m_lPort=atol(xml.GetAttrib("Port").c_str());
			vplatform.supportAlarm=atol(xml.GetAttrib("SupportAlarm").c_str());
			vplatform.m_lSupportMutiStream=atol(xml.GetAttrib("SupportMutiStream").c_str());
			m_VPlatformVecInfo.push_back(vplatform);
			//xml.OutOfElem();


		}

		return true;
				
	}
	bool SaveFile(std::string strFilePath)
	{
		CMarkup xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		xml.AddElem("Config");
		xml.AddAttrib("AccessIP",m_strAccessIP);
		xml.AddAttrib("StartPort",m_nStartListenPort);
		xml.AddAttrib("SupportParam",m_nSupportParam);
		xml.AddAttrib("SupportAlarm",m_nSupportAlarm);
		xml.IntoElem();
		xml.AddElem("VPlatforms");
		xml.IntoElem();
		for (int i=0;i<(int)m_VPlatformVecInfo.size();i++)
		{
			VPlatformConfig& platform=m_VPlatformVecInfo[i];
			xml.AddElem("VPlatform");
			xml.AddAttrib("PlatformName",platform.m_sPlatformName);
			xml.AddAttrib("DeviceID",platform.m_sDeviceId);
			xml.AddAttrib("IP",platform.m_sServerIP);
			xml.AddAttrib("Port",platform.m_lPort);
			xml.AddAttrib("User",platform.m_sUser);
			xml.AddAttrib("Passwd",platform.m_sPasswd);
			xml.AddAttrib("SupportAlarm",platform.supportAlarm);
			xml.AddAttrib("SupportMutiStream",platform.m_lSupportMutiStream);
			xml.AddAttrib("ExtendPara1",platform.m_sExtendPara1);

		}
		xml.OutOfElem();
		return xml.Save(strFilePath);
	}
public:

	std::string m_strAccessIP;
	int m_nStartListenPort;
	int m_nSupportParam;
	int m_nSupportAlarm;
	std::vector<VPlatformConfig> m_VPlatformVecInfo;
};

#endif

