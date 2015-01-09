#ifndef USERSESSION_H
#define USERSESSION_H
#include "Common.h"
#include "Device.h"

class UserInfo
{
public:
	std::string mac_;
	std::string client_ip_;
	std::string user_id_;
	StreamSocket sock_ ;

	bool operator==(const UserInfo& info)
	{
		return mac_ == info.mac_ && client_ip_ == info.client_ip_;
	}

	void operator=(const UserInfo& info)
	{
		mac_ = info.mac_;
		client_ip_ = info.client_ip_;
		user_id_ = info.user_id_;
		sock_ = info.sock_;
	}
};

class User : public Device
{
public:
	User(const std::string& user_name);
	virtual ~User();

public:
	std::string user_name_;
	UserInfo user_info_;
	Poco::Mutex status_lock_;
	bool is_active_;
	Poco::LocalDateTime update_time_;
	void UpdateTime();
};

class UserManager : public Poco::Runnable
{
public:
	bool FindUser(const std::string &user_name, User* &user);
	bool FindUser(const std::string &user_name, vector<UserInfo>& users);
	void AddUser(const std::string &user_name, const UserInfo& info);
	void RemoveUser(const std::string &user_name, const UserInfo& info);
	bool UpdateTime(const std::string& user_name, const UserInfo& info);

	virtual void run();

private:
	std::multimap<std::string, User*> users_map_;
	Poco::FastMutex user_mutex_;
};

#endif
