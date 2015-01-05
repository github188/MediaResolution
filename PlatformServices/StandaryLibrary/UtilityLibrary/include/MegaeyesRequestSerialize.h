#ifndef MegaeyesRequestSerialize_H
#define MegaeyesRequestSerialize_H
#include "MegaeyesHead.h"
#include "Markup.h"
#include <string>

class MegaeyesRequestSerialize
{
public:

	MegaeyesRequestSerialize()
	{
		
	}
	virtual ~MegaeyesRequestSerialize()
	{

	}

public:
	//���л�
	virtual  std::string Serialize()=0;
	//�����л�
	virtual void UnSerialize(const std::string& strMessage)=0;
public:
	MegaeyesRequestHead m_cmdHead;
	CMarkup m_xml;
};
#endif