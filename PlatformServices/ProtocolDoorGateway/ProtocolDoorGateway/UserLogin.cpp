#include "UserLogin.h"
#include "Common.h"
#include "User.h"
#include "AccessModule.h"

UserLogin::UserLogin(void)
{
}


UserLogin::~UserLogin(void)
{
}

int UserLogin::Run(Operator& op, CmdHead& head, char* body)
{
	std::string protocol, user_id;
	protocol.append(body, head.length);
	user_id.append(head.src, 20);

	CMarkup xml;
	StrUtility utility;
	if (!xml.SetDoc(protocol) || !utility.ForeachXml(xml))
	{
		return -1;
	}

	string user_name;
	UserInfo info;
	utility.GetxmlValue("Information/UserName", user_name);
	utility.GetxmlValue("Information/ClientIP", info.client_ip_);
	utility.GetxmlValue("Information/Mac", info.mac_);

	info.sock_ = op.hand->_socket;
	info.user_id_ = user_id;
	AccessModule::Instance().user_manager_.AddUser(user_name, info);

	Poco::LocalDateTime now;
	xml.SetDoc("");
	xml.AddElem("Information");
	xml.AddAttrib("HeartTime", 30);
	xml.AddAttrib("Time", DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));
	
	return AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc()) > 0 ? -1 : 0;
}