#include "User.h"
#include "AccessModule.h"

User::User()
{

}

User::~User()
{

}

void User::SetDevStatus(bool is_active)
{
	is_active_ = is_active;
}

void User::UpdateTime()
{
	Poco::Timestamp now;
	update_time_ = now;
}

bool UserManager::Finduser(const std::string &id, User* &user)
{
	Poco::ScopedLock<FastMutex> lock(user_lock_);
	std::multimap<std::string, User*>::iterator iter = users_map_.lower_bound(id);
	if (iter != users_map_.end())
	{
		user = iter->second;
		return true;
	}

	return false;
}

bool UserManager::Finduser(const std::string &user_name, vector<UserInfo>& users)
{
	Poco::ScopedLock<FastMutex> lock(user_lock_);
	std::pair<std::multimap<std::string, User*>::iterator, std::multimap<std::string, User*>::iterator> pair;
	pair = users_map_.equal_range(user_name);

	if (pair.first == pair.second)
	{
		return false;
	}

	std::multimap<std::string, User*>::iterator iter;
	for (iter = pair.first; iter != pair.second; iter++)
	{
		users.push_back(iter->second->user_info_);
	}

	return true;
}

void UserManager::Adduser(User* user)
{
	Poco::ScopedLock<FastMutex> lock(user_lock_);
	users_map_.insert(std::make_pair<std::string, User*>(user->id_, user));
}

bool UserManager::UpdateTime(const std::string& user_name, const UserInfo& info)
{
	Poco::ScopedLock<FastMutex> lock(user_lock_);
	{
		std::pair<std::multimap<std::string, User*>::iterator, std::multimap<std::string, User*>::iterator> pair;
		pair = users_map_.equal_range(user_name);

		if (pair.first == pair.second)
		{
			return false;
		}

		std::multimap<std::string, User*>::iterator iter;
		for (iter = pair.first; iter != pair.second; iter++)
		{
			User* pUser = iter->second;
			if (pUser->user_info_ == info)
			{
				pUser->UpdateTime();
				return true;
			}
		}

		return false;
	}
}

void UserManager::run()
{
	std::multimap<std::string, User*> copy_map;
	while (true)
	{
		user_lock_.lock();
		copy_map = users_map_;
		user_lock_.unlock();

		Poco::Timespan tm;
		for (std::multimap<std::string, User*>::iterator iter = copy_map.begin(); iter != copy_map.end(); ++iter)
		{
			if (!iter->second->is_active_)
			{
				continue;
			}

			Poco::Timestamp now;
			tm = now - iter->second->update_time_;
			if (tm.totalSeconds() < AccessModule::Instance().config_.heart_timeout_)
			{
				continue;
			}

			iter->second->SetDevStatus(false);
		}
		Poco::Thread::sleep(5000);
	}
}

