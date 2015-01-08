//�豸ͳһ�̳иû���
//mlcai 2014-11-24

#ifndef DEVICE_H
#define DEVICE_H

#include "Global.h"
#include "NetUtility.h"

class Device
{
public:
	virtual ~Device() {}

	std::string id_;
	string nat_addr_;		//�����ӵ�ַ

	Poco::FastMutex status_mtx_;
	Poco::FastMutex nat_sock_mtx_;

protected:
	Device() {}
};

#endif
