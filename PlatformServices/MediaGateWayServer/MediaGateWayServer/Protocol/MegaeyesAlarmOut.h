#ifndef MAEGAEYES_ALARMOUT_H
#define MAEGAEYES_ALARMOUT_H
#include "MegaeyesProtocol.h"
class CMegaeyesSetAlarmOutResponse:public CMegaeyesResponseProtocol
{
public:
	const std::string Serialize()
	{
		m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgSetAlarmOut+1;
		std::string str;
		return str;
	}
};
class CMegaeyesAlarmOut:public CMegaeyesRequestProtocol,public CMegaeyesResponseProtocol
{
public:
	CMegaeyesAlarmOut()
	{

	}
	~CMegaeyesAlarmOut()
	{

	}
	 void UnSerialize(const std::string& strMessage)
	 {
		 m_xml.SetDoc(strMessage);
		 if (m_xml.FindElem("Message")==false)
		 {
			 throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5064);
		 }
		 m_xml.IntoElem();
		 if (m_xml.FindElem("DeviceID")==false)
		 {
			 throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5064);
		 }
		 m_strAlarmOutId=m_xml.GetData();
		 if (m_xml.FindElem("Op")==true)
		 {
			  m_nOp=atoi(m_xml.GetData().c_str());
		 }
		
		 
	 }
	 const std::string Serialize()
	{
		 m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		 m_xml.AddElem("Message");
		 m_xml.IntoElem();
	//	 m_xml.AddElem("DeviceID",m_strAlarmOutId);
		 m_xml.AddElem("Op",m_nOp);
		 m_RespinseCmdHead.m_nCmdNum = CMegaeyesQequestCmdHead::MsgGetAlarmOut+1;
		 m_RespinseCmdHead.m_nLength = m_xml.GetDoc().length();
		 return m_xml.GetDoc();
	}
public:
	std::string m_strAlarmOutId;
	long m_nOp;
};
#endif