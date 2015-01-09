#ifndef ACCESS_SERVER_H
#define ACCESS_SERVER_H

#include "Global.h"
#include "Term.h"
#include "User.h"
#include "Config.h"

using namespace std;

class AccessModule
{
public:
	static AccessModule& Instance()
	{
		return instance_;
	}

	virtual ~AccessModule();
	void Stop();

	void InsertAsynMsg(const string& addr, Event* event, char* msg);
	void NotifyAsynMsg(const string& addr, char* msg, int length);
	void RemoveAsynMsg(const string& addr);

	int SendMsgWithRet(StreamSocket& act_sock, const char* msg, int msg_len, char* ret_msg, int& ret_len);
	int SendMsgWithRet(const string& ip, int port, const char* msg, int msg_len, char* ret_msg, int& ret_len);
	int SendMsg(const string& ip, int port, const char* msg, int msg_len);
	int SendMsg(StreamSocket& act_sock, const char* msg, int msg_len);

	void GetTicket(char * ticket, int length);
	bool HttpPost(string url, string message, std::string &res);

	Configure config_;
	TermManager  term_manager_;
	UserManager  user_manager_;

protected:
	AccessModule();
	AccessModule(const AccessModule&);
	AccessModule& operator=(const AccessModule&);

private:
	static AccessModule instance_;

public:
	FastMutex    _mutex;
	FastMutex asyn_mutx_;
	map<string, pair<Event*, char*> > asyn_mgs_map_;
};
#endif
