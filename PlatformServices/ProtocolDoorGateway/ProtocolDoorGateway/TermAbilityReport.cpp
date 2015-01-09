#include "TermAbilityReport.h"
#include "define.h"
#include "AccessModule.h"
#include "Term.h"
#include "Markup.h"
#include "StrUtility.h"
#include "LogInfo.h"
#include "LogSys.h"

TermAbilityReport::TermAbilityReport(void)
{
}

TermAbilityReport::~TermAbilityReport(void)
{
}

int TermAbilityReport::Run(Operator& op, CmdHead& head, char* body)
{
	std::string term_id;
	term_id.append(head.src, PROTOCOL_ID_LENGTH);

	LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);

	Term* pTerm = NULL;
	if (!AccessModule::Instance().term_manager_.FindTerm(term_id, pTerm))
	{
		log.log(Message::PRIO_ERROR, "10006->设备能力上报，非法设备[%d]", ERR_TERM_NOTEXIST);
		string errorDescription = "10006->设备能力上报，非法设备";
		AnswerErrorRes(op.hand->_socket, head, ERR_TERM_NOTEXIST, errorDescription);
		return -1;
	}

	std::string protocol, center_res;
	protocol.append(body, head.length);

	CMarkup xml;
	StrUtility utility;
	int nRet = ERR_CENTER_NOT_RES;
	if (!(AccessModule::Instance().HttpPost(SERVER_URL_CAP, protocol, center_res)
		&& xml.SetDoc(center_res) && utility.ForeachXml(xml)
		&& utility.GetxmlValue("Information/Success", nRet) && nRet == 0))
	{
		log.log(Message::PRIO_ERROR, "10006->设备能力上报，中心交互失败备[%d]", nRet);
		string errorDescription;
		utility.GetxmlValue("Information/ErrorDescription", errorDescription);
		errorDescription = (nRet == ERR_CENTER_NOT_RES ? "10006->设备能力上报，中心无应答" : errorDescription);
		AnswerErrorRes(op.hand->_socket, head, nRet, errorDescription);
		return -1;
	}

	//回应视频服务器
	LocalDateTime now;
	xml.SetDoc("");
	xml.AddElem("Information");
	xml.AddChildElem("Time", DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));

	return AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc()) > 0 ? -1 : 0;
}
