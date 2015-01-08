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

	//  ���ؼ����˿�
	int user_port_;
	int term_port_;

	//��ʱʱ��
	int heart_timeout_;
};

#endif
