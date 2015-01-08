//设备统一继承该基类
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
	string nat_addr_;		//长连接地址

	Poco::FastMutex status_mtx_;
	Poco::FastMutex nat_sock_mtx_;

protected:
	Device() {}
};

#endif
