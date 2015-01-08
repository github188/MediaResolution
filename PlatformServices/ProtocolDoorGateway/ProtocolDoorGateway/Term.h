#ifndef TERMINAL_H
#define TERMINAL_H
#include "Common.h"
#include "Device.h"

// ǰ����Ϣ�ṹ����
struct ChannelInfo
{
	std::string channel_num;
	std::string channel_name;
};

class Term : public Device
{
public:
	std::string term_id_;				//ǰ�˵����ݿ�ID
	std::string local_ip_;             //ǰ�˱���IP��ַ 
	std::string nat_ip_ ;				//ǰ���豸NAT IP ��ַ
	std::string version_;			    //ǰ���豸Э��
	int msg_port_;						//ǰ��ָ��˿ں�
	int video_port_;					//ǰ����Ƶ�˿ں�
	int audio_port_;					//ǰ����Ƶ�˿ں�
	int heart_cycle_;
	bool is_active_;
	bool is_nat_;
	StreamSocket socket_ ;	   //ǰ��ע��ģʽ�³�����
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
