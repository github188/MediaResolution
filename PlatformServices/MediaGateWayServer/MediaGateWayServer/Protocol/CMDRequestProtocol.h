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
	//���л�
	virtual  std::string Serialize()=0;
	//�����л�
	virtual void UnSerialize(const std::string& strMessage)=0;
public:
	CMegaeyesQequestCmdHead m_cmdHead;
	CMarkup m_xml;
};
#endif