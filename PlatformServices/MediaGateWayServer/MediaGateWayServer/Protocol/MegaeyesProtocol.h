#ifndef MEGAEYES_PROTOCOL_H
#define MEGAEYES_PROTOCOL_H
#include "CmdHead.h"
#include "Markup.h"
#include "MegaeyesException.h"
class CMegaeyesProtocol
{
public:
	CMegaeyesProtocol()
	{

	}
	virtual ~CMegaeyesProtocol()
	{

	}
	//反序列化
	virtual void UnSerialize(const std::string& strMessage)=0;
	virtual const std::string Serialize()=0;
public:
	CMarkup m_xml;
};
class CMegaeyesRequestProtocol:virtual public CMegaeyesProtocol
{
public:

	CMegaeyesRequestProtocol()
	{
		
	}
	virtual ~CMegaeyesRequestProtocol()
	{

	}

	const CMegaeyesRequestProtocol& operator=(const CMegaeyesRequestProtocol& rParam)
	{
		if (this==&rParam)
		{
			return *this;
		}
		m_cmdHead=rParam.m_cmdHead;
		return *this;
	}

public:
	//反序列化
	virtual void UnSerialize(const std::string& strMessage)=0;
	
	//序列化
	virtual const std::string Serialize()
	{
		std::string str;
		return str;
	}
public:
	CMegaeyesQequestCmdHead m_cmdHead;
	
};

class CMegaeyesResponseProtocol:virtual public CMegaeyesProtocol
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
		m_RespinseCmdHead=rParam.m_RespinseCmdHead;
		return *this;
	}
public:
	//序列化
	virtual const std::string Serialize()=0;
	virtual void UnSerialize(const std::string& strMessage)
	{

	}

	CMegaeyesResponseCmdHead m_RespinseCmdHead;
};

#endif