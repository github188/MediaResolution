#ifndef MEGAEYES_ALARM_NOTIFY_H
#define MEGAEYES_ALARM_NOTIFY_H
#include "MegaeyesRequestSerialize.h"
class Megaeyes5214ReqSerialize:public MegaeyesRequestSerialize
{
public:

	Megaeyes5214ReqSerialize()
	{
		m_strNaming="";
		m_strDevNaming="";
		m_nDevType=5;
		m_strAlarmTime="";
	}
	~Megaeyes5214ReqSerialize()
	{

	}
	virtual void UnSerialize(const std::string& strMessage){ }
	//–Ú¡–ªØ
	virtual std::string Serialize()
	{
		char buf[1024] = {0};
		sprintf(buf, "<Message><Naming>%s</Naming><DevNaming>%s</DevNaming><DevType>%d</DevType><AlarmTime>%s</AlarmTime></Message>",m_strNaming.c_str(),m_strDevNaming.c_str(),m_nDevType,
			m_strAlarmTime.c_str());
		std::string str;
		str.assign(buf);
		

		m_cmdHead.cmdnum = MSG_ALARM_NOTIFY;
		m_cmdHead.cmdseq = 0;
		m_cmdHead.length = str.size();

		char buffer[1024] = {0};
		memcpy(buffer, &m_cmdHead, sizeof(MegaeyesRequestHead));
		memcpy(buffer + sizeof(MegaeyesRequestHead), str.c_str(), str.size());
		std::string xml;
		xml.append(buffer, sizeof(MegaeyesRequestHead) + str.size());
		return xml;
	}
public:
	std::string m_strNaming;
	std::string m_strDevNaming;
	int m_nDevType;
	std::string m_strAlarmTime;
};
#endif