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
		log.log(Message::PRIO_ERROR, "�豸�����ϱ���ǰ�˷���XML��Ϣ����");
		return -1;
	}

	while (xml.FindChildElem("Device"))
	{
		string id = xml.GetChildAttrib("Id");
		string line = xml.GetChildAttrib("Online");

		Term* pTerm = NULL;
		if (!AccessModule::Instance().term_manager_.FindTerm(id, pTerm) || !pTerm->is_active_)
		{
			log.log(Message::PRIO_ERROR, "�豸�����ϱ���ǰ������", id);
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

	//��Ӧ��Ƶ������
	LocalDateTime now;
	xml.SetDoc("");
	xml.AddElem("Information");
	xml.AddChildElem("Time", DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));

	return AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc()) > 0 ? -1 : 0;
}