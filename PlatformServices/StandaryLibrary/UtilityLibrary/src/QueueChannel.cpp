//实现异步日志

#include "QueueChannel.h"
#include "Poco/Message.h"

class StopNotification : public Poco::Notification
{
public:
	StopNotification() {}
	~StopNotification() {}
};

class MessageNotification : public Poco::Notification
{
public:
	MessageNotification(const Poco::Message& msg) : msg_(msg)
	{
	}
	~MessageNotification()
	{
	}

	const Poco::Message& message()
	{
		return msg_;
	}

private:
	Poco::Message msg_;
};

QueueChannel::QueueChannel(Poco::Channel* chl)
{
	asyn_logging_ = new AsynLogging(chl, this);
	open();
}

QueueChannel::~QueueChannel()
{
	close();
	delete asyn_logging_;
}

void QueueChannel::open()
{
	asyn_logging_->start();
}

void QueueChannel::close()
{
	asyn_logging_->stop();
}

void QueueChannel::log(const Poco::Message& msg)
{
	message_queue_.enqueueNotification(new MessageNotification(msg));
}

AsynLogging::AsynLogging(Poco::Channel* chl, QueueChannel* que_chl) : 
pChannel_(chl), queue_channel_(que_chl)
{
	chl->duplicate();
}

AsynLogging::~AsynLogging()
{
}

void AsynLogging::start()
{
	logging_thr.start(*this);
}

void AsynLogging::stop()
{
	queue_channel_->message_queue_.enqueueNotification(new StopNotification);
	logging_thr.join();
}

void AsynLogging::run()
{
	StopNotification* stop_notification = 0;
	while (!stop_notification)
	{
		Poco::Notification* notification = queue_channel_->message_queue_.waitDequeueNotification();
		MessageNotification* msg_notification = dynamic_cast<MessageNotification*>(notification);
		if (msg_notification)
		{
			pChannel_->log(msg_notification->message());
		}
		else
			stop_notification = dynamic_cast<StopNotification*>(notification);
		notification->release();
	}
}
