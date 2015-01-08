#ifndef TERMINAL_H
#define TERMINAL_H
#include "Common.h"
#include "Device.h"

// 前端信息结构定义
struct ChannelInfo
{
	std::string channel_num;
	std::string channel_name;
};

class Term : public Device
{
public:
	std::string term_id_;				//前端的数据库ID
	std::string local_ip_;             //前端本机IP地址 
	std::string nat_ip_ ;				//前端设备NAT IP 地址
	std::string version_;			    //前端设备协议
	int msg_port_;						//前端指令端口号
	int video_port_;					//前端视频端口号
	int audio_port_;					//前端音频端口号
	int heart_cycle_;
	bool is_active_;
	bool is_nat_;
	StreamSocket socket_ ;	   //前端注册模式下长连接
	Poco::Mutex status_lock_;
	Poco::LocalDateTime update_time_;

public:
	void SetDevStatus(bool isActive);
	void UpdateTime();

	Term();
	virtual ~Term();
};

class TermManager : public Poco::Runnable
{
public:
	bool FindTerm(const std::string &id, Term* &term);
	void AddTerm(Term* term);

	virtual void run();

private:
	std::map<std::string, Term*> term_map_;
	Poco::FastMutex term_mutex_;
};

#endif
