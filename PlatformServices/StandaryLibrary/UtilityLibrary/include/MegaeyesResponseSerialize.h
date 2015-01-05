#ifndef MegaeyesResponseSerialize_H
#define MegaeyesResponseSerialize_H
#include "MegaeyesHead.h"
#include "Markup.h"
#include <string>

class MegaeyesResponseSerialize
{
public:

	MegaeyesResponseSerialize()
	{
		
	}
	virtual ~MegaeyesResponseSerialize()
	{

	}

public:
	//序列化
	virtual  std::string Serialize()=0;
	//反序列化
	virtual void UnSerialize(const std::string& strMessage)=0;
public:
	MegaeyesResponseHead m_cmdHead;
	CMarkup m_xml;
};
#endif