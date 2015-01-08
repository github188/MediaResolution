#include "TermLogin.h"
#include "Markup.h"
#include "StrUtility.h"
#include "NetUtility.h"
#include "Operator.h"
#include "MsgHandler.h"
#include "AccessModule.h"
#include "LogInfo.h"
#include "LogSys.h"

FastMutex TermLogin::mutex_;

TermLogin::TermLogin()
{
}

TermLogin::~TermLogin()
{
}

int TermLogin::Run(Operator& op, CmdHead& head, char* body)
{
	std::string protocol;
	protocol.append(body, head.length);

	std::string center_res;
	CMarkup xml;
	StrUtility utility;
	int nRet = ERR_CENTER_NOT_RES;

	LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);

	if (!(AccessModule::Instance().HttpPost(CMD_REGTERM, protocol, center_res)
		&& xml.SetDoc(center_res) && utility.ForeachXml(xml)
		&& utility.GetxmlValue("Information/Success", nRet) && nRet == 0))
	{
		log.log(Message::PRIO_ERROR, "10004->设备登录，请求中心失败[%d]", nRet);
		string errorDescription;
		utility.GetxmlValue("Information/ErrorDescription", errorDescription);
		errorDescription = (nRet == ERR_CENTER_NOT_RES ? "10004->设备登录，中心无应答" : errorDescription);
		AnswerErrorRes(op.hand->_socket, head, nRet, errorDescription);
		return -1;
	}

	utility.ClearMap();
	if (!xml.SetDoc(protocol) || !utility.ForeachXml(xml))
	{
		AnswerErrorRes(op.hand->_socket, head, ERR_VAILD_XML, "10004->设备登录，前端设备无效的XML");
		return -1;
	}

	// 回复设备
	nRet = AnswerCorrectRes(op.hand->_socket, head);
	if (nRet > 0)
	{
		return -1;
	}

	std::string nat_ip;
	int nat_port;
	NetUtility::GetAddr(op.hand->_socket, nat_ip, nat_port);

	int msg_port = 0, video_port = 0, audio_port = 0;
	std::string term_id, term_ver;
	utility.GetxmlValue("DeviceRegister/DeviceId", term_id);
	utility.GetxmlValue("DeviceRegister/Version", term_ver);
	utility.GetxmlValue("DeviceRegister/VideoPort", video_port);
	utility.GetxmlValue("DeviceRegister/AudioPort", audio_port);
	utility.GetxmlValue("DeviceRegister/MsgPort", msg_port);

	//lock
	Term* pTerm = NULL;

	mutex_.lock();
	if (!AccessModule::Instance().term_manager_.FindTerm(term_id, pTerm))
	{
		pTerm = new Term;
		pTerm->term_id_ = term_id;
		AccessModule::Instance().term_manager_.AddTerm(pTerm);
	}
	mutex_.unlock();

	pTerm->status_lock_.lock();
	pTerm->nat_ip_ = nat_ip;
	pTerm->msg_port_ = msg_port;
	pTerm->video_port_ = video_port;
	pTerm->audio_port_ = audio_port;
	pTerm->version_ = term_ver;	
	pTerm->is_active_ = true;
	pTerm->socket_ = op.hand->_socket;
	pTerm->UpdateTime();
	pTerm->status_lock_.unlock();
	return 0;
}