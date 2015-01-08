#ifndef _H_Config_H
#define _H_Config_H

#include "Global.h"
#include "StrUtility.h"
#include "define.h"

class Configure
{
public:
	Configure();
	virtual ~Configure();

	bool Parse(std::string file_name);

	std::string center_ip_;
	int center_port_;

	std::string local_ip_;

	//  本地监听端口
	int user_port_;
	int term_port_;

	//超时时间
	int heart_timeout_;
};

#endif
