#include "Config.h"

using namespace ty;

Configure::Configure()
{
	center_port_ = 0;
	user_port_ = 0;
	term_port_ = 0;
}

Configure::~Configure()
{

}

bool Configure::Parse(std::string file_name)
{
	CMarkup configxml;
	StrUtility strutility;
	if (!configxml.Load(file_name) || !strutility.ForeachXml(configxml))
	{
		return false;
	}

	strutility.GetxmlValue("Config/Center_Host", center_ip_);
	strutility.GetxmlValue("Config/Center_Port", center_port_);
	strutility.GetxmlValue("Config/Local_Host", local_ip_);
	strutility.GetxmlValue("Config/Local_UserPort", user_port_);
	strutility.GetxmlValue("Config/Local_TermPort", term_port_);
	strutility.GetxmlValue("Config/Timeout_HeartBeat", heart_timeout_);

	if (center_ip_.empty() || center_port_ <= 0 || user_port_ <= 0 || term_port_ <= 0)
	{
		return false;
	}

	heart_timeout_ = heart_timeout_ > 360 ? 360 : heart_timeout_;

	return true;
}
