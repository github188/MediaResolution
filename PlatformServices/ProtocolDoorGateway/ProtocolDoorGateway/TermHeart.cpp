#include "TermHeart.h"
#include "Common.h"
#include "AccessModule.h"

TermHeart::TermHeart()
{

}


TermHeart::~TermHeart()
{

}

int TermHeart::Run(Operator& op, CmdHead& head, char* body)
{
	std::string protocol;
	protocol.append(body, head.length);

	CMarkup xml;

	LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);
	if (!xml.SetDoc(protocol))
	{
		log.log(Message::PRIO_ERROR, "设备心跳上报，前端发送XML信息错误");
		return -1;
	}

	while (xml.FindChildElem("Device"))
	{
		string id = xml.GetChildAttrib("Id");
		string line = xml.GetChildAttrib("Online");

		Term* pTerm = NULL;
		if (!AccessModule::Instance().term_manager_.FindTerm(id, pTerm) || !pTerm->is_active_)
		{
			log.log(Message::PRIO_ERROR, "设备心跳上报，前端离线", id);
			continue;
		}

		if (line == "true")
		{
			LocalDateTime now;
			pTerm->update_time_ = now;
		}
		else
		{
			pTerm->SetDevStatus(false);
		}
	}

	//回应视频服务器
	LocalDateTime now;
	xml.SetDoc("");
	xml.AddElem("Information");
	xml.AddChildElem("Time", DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));

	return AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc()) > 0 ? -1 : 0;
}