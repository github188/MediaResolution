#ifndef MEGAEYES_ERROR_NOTIFY_PROTOCOL_H
#define MEGAEYES_ERROR_NOTIFY_PROTOCOL_H
#include "MegaeyesProtocol.h"
class CMegaeyesErrorNotify:public CMegaeyesRequestProtocol
{
public:
	CMegaeyesErrorNotify()
	{
		Clear();
	}
	~CMegaeyesErrorNotify()
	{

	}
	//反序列化
	virtual void UnSerialize(const std::string& strMessage)
	{

	}
	//序列化
	  const std::string Serialize()
	  {
		  
		  m_xml.AddElem("Message");
		  m_xml.AddAttrib("Version", m_strVersion);
		  m_xml.IntoElem();
		  m_xml.AddElem("Header");
		  m_xml.AddAttrib("SequenceNumber", 0);
		  m_xml.AddAttrib("SessionID", "012345678901234");

		  m_xml.AddElem("Label");
		  m_xml.AddAttrib("HostType", "TERMINAL");
		  m_xml.AddAttrib("IP", m_strIP);
		  m_xml.AddAttrib("ItemType", "FAULT");
		  m_xml.AddAttrib("ItemTable", "DEVICE");
		  m_xml.AddElem("Body");
		  m_xml.IntoElem();
		  m_xml.AddElem("GatherTime", m_strGatherTime);
		  m_xml.AddElem("DeviceID", m_strDeviceID);
		  m_xml.AddElem("DeviceType", m_nDeviceType);
		  m_xml.AddElem("FaultType", m_nFaultType);
		  m_xml.AddElem("FaultID", m_strFaultID);
		  m_xml.AddElem("FaultTime",m_strFaultTime);
		  m_xml.AddElem("IsClear", m_nIsClear);
		  m_xml.AddElem("Naming",m_strNaming);
		  m_cmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgErrorNotify;
		  std::string str = m_xml.GetDoc();
		  m_cmdHead.m_nLength=str.length();
		  return str;
	  }
	 void Clear()
	 {
		 m_strVersion="1.0";
		 m_nIsClear=0;

	 }
	 const CMegaeyesErrorNotify& operator=(const CMegaeyesErrorNotify& rParam)
	 {
		 if (this==&rParam)
		 {
			 return *this;
		 }
		 CMegaeyesRequestProtocol::operator=(rParam);
		 m_strVersion = rParam.m_strVersion;
		 m_nSequenceNumber = rParam.m_nSequenceNumber;
		 m_strSessionID = rParam.m_strSessionID;
		m_strHostType = rParam.m_strHostType;
		m_strIP = rParam.m_strIP;
		m_strItemType = rParam.m_strItemType;
		m_strItemTable = rParam.m_strItemTable;
		m_strGatherTime = rParam.m_strGatherTime;
		m_strDeviceID = rParam.m_strDeviceID;
		m_nDeviceType = rParam.m_nDeviceType;
		m_nFaultType = rParam.m_nFaultType;
		m_strFaultID = rParam.m_strFaultID;
		m_strFaultTime = rParam.m_strFaultTime;
		m_nIsClear = rParam.m_nIsClear;
		m_strNaming = rParam.m_strNaming;
		return *this;
	 }
public:
	std::string m_strVersion;
	int m_nSequenceNumber;
	std::string m_strSessionID;
	std::string m_strHostType;
	std::string m_strIP;
	std::string m_strItemType;
	std::string m_strItemTable;
	std::string m_strGatherTime;
	std::string m_strDeviceID;
	int m_nDeviceType;
	int m_nFaultType;
	std::string m_strFaultID;
	std::string m_strFaultTime;
	int m_nIsClear;
	std::string m_strNaming;

};
#endif