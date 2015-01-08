#include "UserHeart.h"
#include "User.h"
#include "AccessModule.h"

UserHeart::UserHeart(void)
{
}


UserHeart::~UserHeart(void)
{
}

int UserHeart::Run(Operator& op, CmdHead& head, char* body)
{ 
	std::string protocol;
	protocol.append(body, head.length);

	CMarkup xml;
	StrUtility utility;
	if (!xml.SetDoc(protocol) || !utility.ForeachXml(xml))
	{
		AnswerErrorRes(op.hand->_socket, head, ERR_VAILD_XML, "客户端心跳上报，错误XML");
		return -1;
	}

	std::string user_name, client_ip, mac;
	utility.GetxmlValue("Information/UserName", user_name);
	utility.GetxmlValue("Information/ClientIp", client_ip);
	utility.GetxmlValue("Information/Mac", mac);

	UserInfo info;
	info.client_ip_ = client_ip;
	info.mac_ = mac;
	info.sock_ = op.hand->_socket;

	if (!AccessModule::Instance().user_manager_.UpdateTime(user_name, info))
	{
		AnswerErrorRes(op.hand->_socket, head, ERR_VAILD_XML, "客户端心跳上报，未知的用户");
		return -1;
	}

	LocalDateTime now;
	xml.SetDoc("");
	xml.AddElem("Information");
	xml.AddChildElem("Time", DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));

	return AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc()) > 0 ? -1 : 0;
}
