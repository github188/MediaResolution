#ifndef MEGAEYES_RESPONSE_PROTOCOL_H
#define MEGAEYES_RESPONSE_PROTOCOL_H
#include "CmdHead.h"
#include "Markup.h"
#include <string>
class CMegaeyesResponseProtocol
{
public:
	CMegaeyesResponseProtocol()
	{

	}

	virtual ~CMegaeyesResponseProtocol()
	{

	}
	const CMegaeyesResponseProtocol& operator=(const CMegaeyesResponseProtocol& rParam)
	{
		if (this==&rParam)
		{
			return *this;
		}
		m_cmdHead=rParam.m_cmdHead;
		return *this;
	}
public:
	//���л�
	virtual const std::string Serialize()=0;
	//�����л�
	virtual void UnSerialize(const std::string& strMessage)=0;
	CMegaeyesResponseCmdHead m_cmdHead;
	CMarkup m_xml;
};
#endif