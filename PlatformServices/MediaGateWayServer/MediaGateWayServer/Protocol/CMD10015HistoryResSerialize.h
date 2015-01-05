#ifndef CMD10015RESSERIALIZE_H 
#define  CMD10015RESSERIALIZE_H

#include <vector>
#include "CMDRequestSerialize.h"
/**********************历史视频查询10015************************************/
/*
<Information>
	<file start_time ="DT" end_time ="DT" file_len="N+"/>
	<file start_time ="DT" end_time ="DT" file_len="N+"/>
</Information>

*/
/************************************************************************/
class StreamInfor
{
public:
	StreamInfor()
	{

	}
	~StreamInfor()
	{

	}
public:
	std::string m_strBeginDT;
	std::string m_strEndDT;
	int m_nLength;
};

class CMD10015HistoryResSerialize:public CMDRequestSerialize
{
public:
	CMD10015HistoryResSerialize()
	{
	}
	~CMD10015HistoryResSerialize()
	{

	}
	virtual std::string Serialize()
	{
		m_xml.AddElem("Information");
		m_xml.IntoElem();
		if (m_cmdHead.success != 0)
		{
			m_xml.AddElem("ErrDesctription", err_description);
		}
		else
		{
			for (int i=0;i<(int)m_streamInfoVec.size();i++)
			{
				m_xml.AddElem("file");
				m_xml.AddAttrib("start_time",m_streamInfoVec[i].m_strBeginDT);
				m_xml.AddAttrib("end_time",m_streamInfoVec[i].m_strEndDT);
				m_xml.AddAttrib("file_len",m_streamInfoVec[i].m_nLength);

			}
		}

		m_cmdHead.cmdnum = CMD_DEVHISTROY_RESP;
		m_cmdHead.length = m_cmdHead.success != 0 ? m_xml.GetDoc().length() + 1 : 0;

		char* buffer = new char[sizeof(m_cmdHead) + m_cmdHead.length];
		memcpy(buffer, &m_cmdHead, sizeof(m_cmdHead));
		if (m_cmdHead.length)
			memcpy(buffer + sizeof(m_cmdHead), m_xml.GetDoc().c_str(), m_xml.GetDoc().length() + 1);
		std::string xml;
		xml.append(buffer, sizeof(m_cmdHead) + m_xml.GetDoc().length() + 1);
		delete[] buffer;
		return xml;
	}

	virtual void UnSerialize(const std::string& strMessage){}

public:
	std::vector<StreamInfor> m_streamInfoVec;
	std::string err_description;
};

#endif