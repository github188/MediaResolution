#ifndef CMDRequestSerialize_H
#define CMDRequestSerialize_H
#include "CmdHead.h"
#include "Markup.h"
#include <string>
#include "Poco/Exception.h"
#include "StrUtility.h"

class CMDRequestSerialize
{
public:

	CMDRequestSerialize()
	{
		
	}
	virtual ~CMDRequestSerialize()
	{

	}

public:
	//���л�
	virtual  std::string Serialize()=0;
	//�����л�
	virtual void UnSerialize(const std::string& strMessage)=0;
public:
	CmdHead m_cmdHead;
	CMarkup m_xml;
};
#endif