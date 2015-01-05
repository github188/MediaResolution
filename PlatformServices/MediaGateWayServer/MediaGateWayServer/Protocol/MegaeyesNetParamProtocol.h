#ifndef MEGAEYES_NET_PARAM_H
#define MEGAEYES_NET_PARAM_H
#include "MegaeyesProtocol.h"

class CMegaeyesNetParamType:public CMegaeyesRequestProtocol,public CMegaeyesResponseProtocol
{
public:
	enum _TYPE
	{
		SetNetPara=1,
		Reboot=2,
		GetVersion=3,
		GetNetPara=4,

	};
	CMegaeyesNetParamType()
	{

	}
	~CMegaeyesNetParamType()
	{

	}
	 void UnSerialize(const std::string& strMessage)
	{
		m_xml.SetDoc(strMessage);
		if (m_xml.FindElem("Message")==false)
		{
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5046);
		}
		m_xml.IntoElem();
		if (m_xml.FindElem("Type")==false)
		{
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5046);
		}
		m_nType=atoi(m_xml.GetData().c_str());
	}
 const std::string Serialize()
{
	m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgNetParam+1;
	m_RespinseCmdHead.m_nLength=0;
	return "";
}

public:
	int m_nType;
};

class CMegaeyesNetParam:public CMegaeyesRequestProtocol,public CMegaeyesResponseProtocol
{
public:
	CMegaeyesNetParam()
	{

	}
	~CMegaeyesNetParam()
	{

	}
	 void UnSerialize(const std::string& strMessage)
	{
	
	}
	virtual const std::string Serialize()
	{
		m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		m_xml.AddElem("Message");
		m_xml.IntoElem();
		m_xml.AddElem("IP",m_strIp);
		m_xml.AddElem("NetMask",m_strNetMask);
		m_xml.AddElem("GateWay",m_strGetWay);
		m_xml.OutOfElem();
		std::string str;
		str =  m_xml.GetDoc();
		m_RespinseCmdHead.m_nLength=str.length();
		m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgNetParam+1;
		return str;
	}
public:
	std::string m_strIp;
	std::string m_strNetMask;
	std::string m_strGetWay;
};

class CMegaeyesGetVersion:public CMegaeyesResponseProtocol
{
public:
	CMegaeyesGetVersion()
	{

	}
	~CMegaeyesGetVersion()
	{

	}
	virtual const std::string Serialize()
	{
		m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		m_xml.AddElem("Message");
		m_xml.AddElem("Version",m_strVersion);
		m_xml.AddElem("BuildDateTime",m_strBuildDateTime);
		m_xml.OutOfElem();
		std::string str;
		str =  m_xml.GetDoc();
		m_RespinseCmdHead.m_nLength=str.length();
		m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgNetParam+1;
		return str;
	}
public:
	std::string m_strVersion;
	std::string m_strBuildDateTime;
};
#endif