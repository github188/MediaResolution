#include "Term.h"
#include "AccessModule.h"

Term::Term()
{
	is_active_ = false;
	is_nat_ = false ;
	msg_port_ = 0;
	video_port_ = 0;
	audio_port_ = 0;
	heart_cycle_ = 120;
}

Term::~Term()
{

}

void Term::SetDevStatus(bool isActive )
{
	Poco::ScopedLock<Mutex> lock(status_lock_);
	is_active_ = isActive;
}

void Term::UpdateTime()
{
	Poco::LocalDateTime now;
	update_time_ = now;
}

bool TermManager::FindTerm( const std::string &id, Term* &term)
{
	Poco::ScopedLock<FastMutex> lock(term_mutex_);
	std::map<std::string, Term*>::iterator iter = term_map_.find(id);
	if (iter != term_map_.end())
	{
		term = iter->second;
		return true;
	}
	return false;
}


void TermManager::AddTerm(Term* term)
{
	Poco::ScopedLock<FastMutex> lock(term_mutex_);
	term_map_.insert(std::make_pair<std::string, Term*>(term->term_id_, term));
}

void TermManager::run()
{
	std::map<std::string, Term*> copy_map;
	while (true)
	{
		term_mutex_.lock();
		copy_map = term_map_;
		term_mutex_.unlock();

		Poco::Timespan tm;
		for (std::map<std::string, Term*>::iterator iter = copy_map.begin(); iter != copy_map.end(); ++iter)
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

			//Downline
			iter->second->SetDevStatus(false);
		}
		Poco::Thread::sleep(5000);
	}
}
