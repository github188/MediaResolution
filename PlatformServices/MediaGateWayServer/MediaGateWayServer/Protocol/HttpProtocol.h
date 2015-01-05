#ifndef HTTP_PROTOCOL__H
#define HTTP_PROTOCOL__H
#include <vector>
#include "Markup.h"
#include "Poco/HashMap.h"
#include <algorithm>
#include "Poco/Mutex.h"
#include "Poco/ScopedLock.h"
#include "Poco/HashMap.h"
#include "VDeviceConfig.h"
#include "Markup.h"
using Poco::FastMutex;
using Poco::ScopedLock;
class CHttpDevInfoProtocol
{
public:
	std::vector<int> m_FaultChannelVec;
	std::vector<int> m_RealChannelVec;
	std::string m_strDevId;
	std::string Serialize()
	{
		CMarkup xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		xml.AddElem("Message");
		xml.IntoElem();
		xml.AddElem("Dev");
		xml.AddAttrib("Id",m_strDevId);
		xml.IntoElem();
		if (m_FaultChannelVec.size()!=0)
		{
			xml.AddElem("FaultChannel");
			xml.IntoElem();
			for (int j=0;j< m_FaultChannelVec.size();j++)
			{
				int nChannel =m_FaultChannelVec[j];
				xml.AddElem("Channel",nChannel+1);

			}
			xml.OutOfElem();
		}

		//ptr->second.m_FaultChannelVec.clear();
		if (m_RealChannelVec.size()!=0)
		{
			xml.AddElem("RealChannel");
			xml.IntoElem();
			for (int j=0;j< m_RealChannelVec.size();j++)
			{
				int nChannel = m_RealChannelVec[j];
				xml.AddElem("Channel",nChannel+1);
			}
			xml.OutOfElem();
		}
		xml.OutOfElem();
		xml.OutOfElem();
		return xml.GetDoc();
	}
};
class HttpServerInfoProtocol
{
public:
	HttpServerInfoProtocol()
	{

	}
	~HttpServerInfoProtocol()
	{

	}
	void AddFaultChannel(std::string strDevId,int nChannel)
	{
		ScopedLock<FastMutex> lock(m_Mutex);
		Poco::HashMap<std::string, CHttpDevInfoProtocol>::Iterator ptr = m_devVec.find(strDevId);
		if (ptr==m_devVec.end())
		{
			CHttpDevInfoProtocol dev;
			dev.m_strDevId=strDevId;
			dev.m_FaultChannelVec.push_back(nChannel);
			m_devVec[strDevId]=dev;
			return;
		}
		std::vector<int>::iterator itrChannel = find(ptr->second.m_FaultChannelVec.begin(),ptr->second.m_FaultChannelVec.end(),nChannel);
		if (itrChannel!=ptr->second.m_FaultChannelVec.end())
		{
			return;
		}
		ptr->second.m_FaultChannelVec.push_back(nChannel);
		
	}
	void AddRealChannel(std::string strDevId,int nChannel)
	{
		ScopedLock<FastMutex> lock(m_Mutex);
		Poco::HashMap<std::string,CHttpDevInfoProtocol>::Iterator ptr = m_devVec.find(strDevId);
		if (ptr==m_devVec.end())
		{
			CHttpDevInfoProtocol dev;
			dev.m_strDevId=strDevId;
			dev.m_RealChannelVec.push_back(nChannel);
			m_devVec[strDevId]=dev;
			return;
		}
		std::vector<int>::iterator itrChannel = find(ptr->second.m_RealChannelVec.begin(),ptr->second.m_RealChannelVec.end(),nChannel);
		if (itrChannel!=ptr->second.m_RealChannelVec.end())
		{
			return;
		}
		ptr->second.m_RealChannelVec.push_back(nChannel);
	}

	void RemoveRealChannel(std::string strDevId,int nChannel)
	{
		ScopedLock<FastMutex> lock(m_Mutex);
		Poco::HashMap<std::string,CHttpDevInfoProtocol>::Iterator ptr = m_devVec.find(strDevId);
		if (ptr==m_devVec.end())
		{
			return;
		}
		std::vector<int>::iterator itrChannel = find(ptr->second.m_RealChannelVec.begin(),ptr->second.m_RealChannelVec.end(),nChannel);
		if (itrChannel!=ptr->second.m_RealChannelVec.end())
		{
			ptr->second.m_RealChannelVec.erase(itrChannel);
			if (ptr->second.m_RealChannelVec.size()==0&&ptr->second.m_FaultChannelVec.size()==0)
			{
				m_devVec.erase(ptr);
			}
		}
	}
	void RemoveFaultChannel()
	{
		for (Poco::HashMap<std::string,CHttpDevInfoProtocol>::Iterator ptr=m_devVec.begin();ptr!=m_devVec.end();ptr++)
		{
			ptr->second.m_FaultChannelVec.clear();
		}
	}
	  bool GetDevInfo(const std::string& strDevId,CHttpDevInfoProtocol& devInfo)
	{
		ScopedLock<FastMutex> lock(m_Mutex);
		Poco::HashMap<std::string, CHttpDevInfoProtocol>::Iterator ptr = m_devVec.find(strDevId);
		if (ptr==m_devVec.end())
		{
			return false;
		}
		devInfo = ptr->second;
		return true;
	}
public:
	std::string Serialize()
	{
		CMarkup xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		xml.AddElem("Message");
		xml.IntoElem();
		xml.AddElem("CPU");
		xml.AddAttrib("CpuType",m_strCPUType);
		xml.AddAttrib("UseRatio",m_strCPUUseRatio);
		xml.AddElem("Memory");
		xml.AddAttrib("Size",m_fMemorySize);
		xml.AddAttrib("UseRatio",m_strMmoryUseRatio);
		ScopedLock<FastMutex> lock(m_Mutex);
		for (Poco::HashMap<std::string,CHttpDevInfoProtocol>::Iterator ptr = m_devVec.begin();ptr!=m_devVec.end();ptr++)
		{
			xml.AddElem("Dev");
			xml.AddAttrib("Id",ptr->first);
			xml.IntoElem();
			if (ptr->second.m_FaultChannelVec.size()!=0)
			{
				xml.AddElem("FaultChannel");
				xml.IntoElem();
				for (int j=0;j< ptr->second.m_FaultChannelVec.size();j++)
				{
					int nChannel =ptr->second.m_FaultChannelVec[j];
					xml.AddElem("Channel",nChannel+1);

				}
				xml.OutOfElem();
			}
	
			//ptr->second.m_FaultChannelVec.clear();
			if ( ptr->second.m_RealChannelVec.size()!=0)
			{
				xml.AddElem("RealChannel");
				xml.IntoElem();
				for (int j=0;j< ptr->second.m_RealChannelVec.size();j++)
				{
					int nChannel = ptr->second.m_RealChannelVec[j];
					xml.AddElem("Channel",nChannel+1);
				}
				xml.OutOfElem();
			}
			xml.OutOfElem();
		
		}
		return xml.GetDoc();
	}
public:
	std::string strDevId;
	std::string m_strCPUType;
	std::string m_strCPUUseRatio;
	double m_fMemorySize;
	std::string m_strMmoryUseRatio;
	Poco::HashMap<std::string, CHttpDevInfoProtocol> m_devVec;
	FastMutex m_Mutex;
};

class CHttpAddVPlatform
{
public:
	CHttpAddVPlatform()
	{

	}
	~CHttpAddVPlatform()
	{

	}
	void UnSerialize(std::string& strMsg)
	{
		CMarkup xml;
		xml.SetDoc(strMsg);
		if(xml.FindElem("Message")==false)
		{
			return ;
		}
		xml.IntoElem();
		if (xml.FindElem("VPlatforms")==false)
		{
			return ;
		}
		xml.IntoElem();
		while(xml.FindElem("VirtualDevice"))
		{
			VDeviceConfig vplatform;
			vplatform.m_sDeviceId=xml.GetAttrib("DeviceID");
			vplatform.m_sExtendPara1=xml.GetAttrib("ExtendPara1");
			vplatform.m_sPasswd=xml.GetAttrib("Passwd");
			vplatform.m_sPlatformName=xml.GetAttrib("PlatformName");
			vplatform.m_sPtzParam=xml.GetAttrib("PtzParam");
			vplatform.m_sServerIP=xml.GetAttrib("IP");
			vplatform.m_sUser=xml.GetAttrib("User");
			vplatform.m_lPort=atol(xml.GetAttrib("Port").c_str());
			vplatform.m_lSupportNM=atol(xml.GetAttrib("SupportNM").c_str());
			vplatform.m_lSupportMutiStream=atol(xml.GetAttrib("SupportMutiStream").c_str());
			vplatform.m_lPtzType=atol(xml.GetAttrib("PtzType").c_str());
			vplatform.m_lMaxConnect=atol(xml.GetAttrib("DeviceMaxConnect").c_str());
			m_VPlatformVec.push_back(vplatform);
		}
	}
public:
	std::vector<VDeviceConfig> m_VPlatformVec;
};

class CHttpRemoveVPlatorm
{
public:
	CHttpRemoveVPlatorm()
	{

	}
	~CHttpRemoveVPlatorm()
	{

	}
	void UnSerialize(std::string& strMsg)
	{
		CMarkup xml;
		xml.SetDoc(strMsg);
		//if(xml.FindElem("Message")==false)
		//{
		//	return ;
		//}
		//xml.IntoElem();
		if (xml.FindElem("VPlatforms")==false)
		{
			return ;
		}
		xml.IntoElem();
		while(xml.FindElem("VirtualDevice"))
		{
			std::string strDevId=xml.GetAttrib("DeviceID");
			m_VPlatformVec.push_back(strDevId);
		}
	}
public:
	std::vector<std::string> m_VPlatformVec;
};


#endif