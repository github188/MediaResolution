#ifndef CMD_10004REGREQ_SERIALIZE_H
#define CMD_10004REGREQ_SERIALIZE_H
#include "CMDRequestSerialize.h"
/**********************�豸ע��10004************************************/
/*
<Information>
	<DeviceId>�豸ID</DeviceId>
	<VideoPort>��Ƶ���Ӷ˿ڣ�N��</VideoPort>
	<AudioPort>�������Ӷ˿ڣ�N��</AudioPort>
	<Version>�豸�汾��Ϣ��Ns��</Version>
	<Nat>1/0</Nat>
</Information>

*/
/*********************************************************************/

class CMD10004RegReqSerialize:public CMDRequestSerialize
{
public:
	CMD10004RegReqSerialize()
	{
       m_strVersion="2";
	}
	~CMD10004RegReqSerialize()
	{

	}
public:
   virtual std::string Serialize()
   {  
       m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
	   m_xml.AddElem("Information");
	   m_xml.IntoElem();
       m_xml.AddElem("DeviceID",m_strDeviceID);
       m_xml.AddElem("VideoPort",m_nVideoPort);
       m_xml.AddElem("AudioPort",m_nAudioPort);
       m_xml.AddElem("Version",m_strVersion);
	   m_xml.AddElem("Nat", is_nat_);
       m_xml.OutOfElem();
     
	   m_cmdHead.cmdnum = CMD_REGTERM;
	   m_cmdHead.cmdseq = SeqNumGenerator::Instance().Generate();
	   m_cmdHead.length = m_xml.GetDoc().length() + 1;

	   char buffer[1024] = {0};
	   memcpy(buffer, &m_cmdHead, sizeof(m_cmdHead));
	   memcpy(buffer + sizeof(m_cmdHead), m_xml.GetDoc().c_str(), m_xml.GetDoc().length() + 1);
	   std::string xml;
	   xml.append(buffer, sizeof(m_cmdHead) + m_xml.GetDoc().length() + 1);
	   return xml;
   }
	virtual void UnSerialize(const std::string& strMessage){ }

public:
   std::string m_strDeviceID;
   int m_nVideoPort;
   int m_nAudioPort;
   std::string m_strVersion;
   int is_nat_;
};

#endif
