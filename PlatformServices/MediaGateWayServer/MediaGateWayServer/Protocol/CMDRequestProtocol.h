#ifndef MEGAEYES_REQUEST_PROTOCOL_H
#define MEGAEYES_REQUEST_PROTOCOL_H
#include "CmdHead.h"
#include "Markup.h"

class CMegaeyesRequestProtocol
{
public:

	CMegaeyesRequestProtocol()
	{
		
	}
	virtual ~CMegaeyesRequestProtocol()
	{

	}

public:
	//序列化
	virtual  std::string Serialize()=0;
	//反序列化
	virtual void UnSerialize(const std::string& strMessage)=0;
public:
	CMegaeyesQequestCmdHead m_cmdHead;
	CMarkup m_xml;
};
#endif