//继承自Poco Channel以队列方式写日志

#ifndef QUEUECHANNEL_H
#define QUEUECHANNEL_H

#include "Poco/Channel.h"
#include "Poco/AutoPtr.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

class AsynLogging;
class QueueChannel : public Poco::Channel
{
public:
	QueueChannel(Poco::Channel* chl);
	~QueueChannel();

	void open();
	void close();

	void log(const Poco::Message& msg);

private:
	Poco::NotificationQueue message_queue_;
	AsynLogging* asyn_logging_;

	friend class AsynLogging;
};

class AsynLogging : public Poco::Runnable
{
public:
	AsynLogging(Poco::Channel* chl, QueueChannel* que_chl);
	~AsynLogging();

	void start();
	void stop();

private:
	void run();

private:
	Poco::Thread logging_thr;
	
	QueueChannel* queue_channel_;
	Poco::AutoPtr<Poco::Channel> pChannel_;
};

#endif
