#include "UserRealTimeRequest.h"
#include "AccessModule.h"
#include "LogInfo.h"
#include "LogSys.h"
#include "Operator.h"
#include "MsgHandler.h"

UserRealTimeRequest::UserRealTimeRequest()
{
}


UserRealTimeRequest::~UserRealTimeRequest()
{

}

int UserRealTimeRequest::Run(Operator& op, CmdHead& head, char* body)
{
	std::string term_id;
	term_id.append(head.dst, PROTOCOL_ID_LENGTH);

	LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);

	Term* pTerm = NULL;
	if (!AccessModule::Instance().term_manager_.FindTerm(term_id, pTerm) || !pTerm->is_active_)
	{
		log.log(Message::PRIO_ERROR, "10008->实时视频请求，设备离线[%d]", ERR_TERM_NOTEXIST);
		AnswerErrorRes(op.hand->_socket, head, ERR_TERM_NOTEXIST, "10008->实时视频请求，设备离线");
		return -1;
	}
	
	std::string protocol;
	protocol.append(body, head.length);
	CMarkup xml;
	if (!xml.SetDoc(protocol))
	{
		log.log(Message::PRIO_ERROR, "10008->实时视频请求，客户端XML错误[%d]", ERR_VAILD_XML);
		AnswerErrorRes(op.hand->_socket, head, ERR_VAILD_XML, "10008->实时视频请求，客户端XML错误");
		return -1;
	}

	char token[21] = {0};
	AccessModule::Instance().GetTicket(token, 20);
	xml.AddChildElem("Token", token);
	head.length = xml.GetDoc().length();

	char send_buf[1024] = {0};
	int send_length = sizeof(CmdHead) + xml.GetDoc().length();

	memcpy(send_buf, &head, sizeof(CmdHead));
	memcpy(send_buf + sizeof(CmdHead), xml.GetDoc().c_str(), xml.GetDoc().length());

	char rev_buf[3 * 1024] = {0};
	if (!OperateLongSockEvent(pTerm->socket_, pTerm, send_buf, send_length, rev_buf))
	{
		log.log(Message::PRIO_ERROR, "10008->实时视频请求，设备回复异常");
		AnswerErrorRes(op.hand->_socket, head, ERR_TERM_NOTRES, "10008->实时视频请求，设备回复异常");
		return -1;
	}

	CmdHead* res_head = (CmdHead*)rev_buf;

	if (res_head->success != 0 || res_head->version != PROTOCOL_VERSION)
	{
		//错误码
		AccessModule::Instance().SendMsg(op.hand->_socket, rev_buf, sizeof(CmdHead) + res_head->length);
		return -1;
	}

	xml.SetDoc("");
	xml.AddElem("Information");
	xml.AddChildElem("DeviceIP", pTerm->local_ip_);
	xml.AddChildElem("MediaServerIP", pTerm->nat_ip_);
	xml.AddChildElem("MediaServerPort", pTerm->video_port_);
	xml.AddChildElem("Token", token);

	AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc());
	
	return -1;
}