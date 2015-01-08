#ifndef USERSESSION_H
#define USERSESSION_H
#include "Common.h"
#include "Device.h"

class UserInfo
{
public:
	std::string mac_;
	std::string client_ip_;
	std::string priority_;
	StreamSocket sock_ ;

	bool operator==(const UserInfo& info)
	{
		return mac_ == info.mac_ && client_ip_ == info.client_ip_ && sock_ == info.sock_;
	}

	void operator=(const UserInfo& info)
	{
		mac_ = info.mac_;
		client_ip_ = info.client_ip_;
		priority_ = info.priority_;
		sock_ = info.sock_;
	}
};

class User : public Device
{
public:
	User();
	virtual ~User();

public:
	std::string id;
	std::string user_name_;
	UserInfo user_info_;
	Poco::Mutex status_lock_;
	bool is_active_;
	Poco::Timestamp update_time_;
	void SetDevStatus(bool is_active);
	void UpdateTime();
};

class UserManager : public Poco::Runnable
{
public:
	bool Finduser(const std::string &user_name, User* &user);
	bool Finduser(const std::string &user_name, vector<UserInfo>& users);
	void Adduser(User* user);

	bool UpdateTime(const std::string& user_name, const UserInfo& info);

	virtual void run();

private:
	std::multimap<std::string, User*> users_map_;
	Poco::FastMutex user_lock_;
};

#endif
