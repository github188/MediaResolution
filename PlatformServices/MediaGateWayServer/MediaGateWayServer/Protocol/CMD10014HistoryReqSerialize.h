#ifndef CMD_QUERY_HISTORY_VIDEO_PROTOCOL_H
#define CMD_QUERY_HISTORY_VIDEO_PROTOCOL_H
#include "CMDRequestSerialize.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormat.h"
/**********************历史视频查询10014************************************/
/*
<Information>
	<ChannelNum>S</ChannelNum>
	<BeginDT>DT</BeginDT>
	<EndDT>DT</EndDT>
</Information>
*/
/************************************************************************/
class CMD10014HistoryReqSerialize:public CMDRequestSerialize
{
public:
	//序列化
	virtual std::string Serialize()
	{
		return NULL;
	}
	//反序列化
	virtual void UnSerialize(const std::string& strMessage)
	{
		StrUtility utility;
		string str_begdt, str_enddt;
		if (!(m_xml.SetDoc(strMessage) && utility.ForeachXml(m_xml) && utility.GetxmlValue("Information/ChannelNum", channel_) && 
			utility.GetxmlValue("Information/BeginDT", str_begdt) && utility.GetxmlValue("Information/EndDT", str_enddt)))
		{
			throw Poco::Exception("Megaeyes5004ReqSerialize UnSerialize failed\n",5004);;
		}

		Poco::DateTime beg_dt, end_dt;
		int tzd;
		beg_dt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::SORTABLE_FORMAT, str_begdt, tzd);
		end_dt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::SORTABLE_FORMAT, str_enddt, tzd);

		begin_dt_ = beg_dt;
		end_dt_ = end_dt;
	}
public:
	int channel_;
	Poco::LocalDateTime begin_dt_;
	Poco::LocalDateTime end_dt_;
};
#endif
