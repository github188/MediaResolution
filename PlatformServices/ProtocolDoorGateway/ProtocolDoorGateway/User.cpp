#include "User.h"
#include "AccessModule.h"

User::User(const std::string& user_name) : user_name_(user_name), is_active_(true)
{

}

User::~User()
{

}

void User::UpdateTime()
{
	Poco::LocalDateTime now;
	update_time_ = now;
}

bool UserManager::FindUser(const std::string &id, User* &user)
{
	Poco::ScopedLock<FastMutex> lock(user_mutex_);
	std::multimap<std::string, User*>::iterator iter = users_map_.lower_bound(id);
	if (iter != users_map_.end())
	{
		user = iter->second;
		return true;
	}

	return false;
}

bool UserManager::FindUser(const std::string &user_name, vector<UserInfo>& users)
{
	Poco::ScopedLock<FastMutex> lock(user_mutex_);
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

void UserManager::AddUser(const std::string &user_name, const UserInfo& info)
{
	Poco::ScopedLock<FastMutex> lock(user_mutex_);
	{
		std::pair<std::multimap<std::string, User*>::iterator, std::multimap<std::string, User*>::iterator> pair;
		pair = users_map_.equal_range(user_name);

		if (pair.first == pair.second)
		{
			User* pUser = new User(user_name);
			pUser->user_info_ = info;
			users_map_.insert(std::make_pair<std::string, User*>(user_name, pUser));
			return;
		}

		std::multimap<std::string, User*>::iterator iter;
		for (iter = pair.first; iter != pair.second; iter++)
		{
			User* pUser = iter->second;
			if (pUser->user_info_ == info)
			{
				pUser->user_info_ = info;
				pUser->UpdateTime();
				return;
			}
		}
	}
}

void UserManager::RemoveUser(const std::string &user_name, const UserInfo& info)
{
	Poco::ScopedLock<FastMutex> lock(user_mutex_);
	{
		std::pair<std::multimap<std::string, User*>::iterator, std::multimap<std::string, User*>::iterator> pair;
		pair = users_map_.equal_range(user_name);

		if (pair.first == pair.second)
		{
			return;
		}

		std::multimap<std::string, User*>::iterator iter;
		for (iter = pair.first; iter != pair.second; iter++)
		{
			User* pUser = iter->second;
			if (pUser->user_info_ == info)
			{
				users_map_.erase(iter);
				delete pUser;
				return;
			}
		}

		return;
	}
}

bool UserManager::UpdateTime(const std::string& user_name, const UserInfo& info)
{
	Poco::ScopedLock<FastMutex> lock(user_mutex_);
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
		//copy
		{
			Poco::ScopedLock<FastMutex> lock(user_mutex_);
			copy_map = users_map_;
		}

		Poco::Timespan tm;
		for (std::multimap<std::string, User*>::iterator iter = copy_map.begin(); iter != copy_map.end(); ++iter)
		{
			if (!iter->second->is_active_)
			{
				continue;
			}

			Poco::LocalDateTime now;
			tm = now - iter->second->update_time_;
			if (tm.totalSeconds() < AccessModule::Instance().config_.heart_timeout_)
			{
				continue;
			}

			RemoveUser(iter->first, iter->second->user_info_);
		}
		Poco::Thread::sleep(5000);
	}
}

