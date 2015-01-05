#ifndef CMD_10006ABIPOSTREQ_SERIALIZE_H
#define CMD_10006ABIPOSTREQ_SERIALIZE_H

#include <map>
#include "CMDRequestSerialize.h"

using namespace std;

/**********************设备能力上报10006************************************/
/*
<Information>
	<VideoChannels>
		<VideoChannel Num="",Name=""/>
	</VideoChannels>
	<AlarmInChannel Max=""/>
	<AlarmOutChannel Max=""/>
</Information>
*/
/*********************************************************************/

class CMD10006AbiPostReqSerialize:public CMDRequestSerialize
{
public:
	CMD10006AbiPostReqSerialize()
	{
	}
	~CMD10006AbiPostReqSerialize()
	{

	}
public:
   virtual std::string Serialize()
   {  
       m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
	   m_xml.AddElem("Information");
	   
	   m_xml.IntoElem();

	   m_xml.AddElem("VideoChannels");

	   m_xml.IntoElem();
	   map<int, string>::iterator it;
	   for (it = MediaChannls.begin(); it != MediaChannls.end(); ++it)
	   {
		    m_xml.AddElem("VideoChannel");
			m_xml.AddAttrib("Num", it->first);
			m_xml.AddAttrib("Name", it->second);
	   }
	   m_xml.OutOfElem();
       
	   m_xml.AddElem("AlarmInChannel");
	   m_xml.AddAttrib("Max", max_in_);
	   m_xml.AddElem("AlarmOutChannel");
	   m_xml.AddAttrib("Max", max_out_);

	   m_xml.OutOfElem();
     
	   m_cmdHead.cmdnum = CMD_TERMABILITY;
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
	map<int, string> MediaChannls;
	int max_in_;
	int max_out_;
};

#endif
