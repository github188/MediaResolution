
#pragma once

#include "Config.h"
#include "Poco/Thread.h"
#include "LogInfo.h"
#include "Poco/Runnable.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/HTTPClientSession.h"
#include "CmdServerModule.h"
#include "MsgHandler.h"
using Poco::Runnable;
using Poco::Thread;
using namespace Poco;
using namespace Poco::Net;
class MediaGateWayServer:public Runnable
{
public:
	static MediaGateWayServer& Instance(){return instance_;}
	~MediaGateWayServer();

protected:
	MediaGateWayServer();
	MediaGateWayServer(const MediaGateWayServer&);
	MediaGateWayServer& operator=(const MediaGateWayServer&);

public:
	void Start(Config& config);
	void run();

public:
	static MediaGateWayServer instance_;

private:
	bool is_stop_;
	Poco::Thread m_Thread;
	SocketReactor _reactor;

	SocketAcceptor<MsgHander>* acceptor1;
	SocketAcceptor<MsgHander>* acceptor2;
	SocketAcceptor<MsgHander>* acceptor3;
	SocketAcceptor<MsgHander>* acceptor4;
};