#include "stdafx.h"
#include "MediaGateWayServer.h"
#include "ServerTask.h"
#include "Poco/ThreadPool.h"
#include "MsgHandler.h"

#include "BussinessServer.h"
#include "BussinessEvent.h"
#include "Operator.h"

MediaGateWayServer MediaGateWayServer::instance_;

MediaGateWayServer::MediaGateWayServer()
{
	is_stop_=false;
}

MediaGateWayServer::~MediaGateWayServer()
{
	delete acceptor1;
	delete acceptor2;
	delete acceptor3;
	delete acceptor4;
	_reactor.stop();
	is_stop_=true;
	m_Thread.join();
}

void MediaGateWayServer::Start(Config& config)
{
    m_Thread.start(*this);
}

void MediaGateWayServer::run()
{
	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

    CMsgTask logicevent;
    BussinessServer bsever((BussinessEvent *)&logicevent);
    bsever.start();

    SocketAddress address1("0.0.0.0",CmdServerModule::Instance().m_config.startPort);
    ServerSocket skfd_msg(address1);
    acceptor1=new SocketAcceptor<MsgHander>(skfd_msg, _reactor);

	transLog.log(Message::PRIO_INFORMATION, "Message Listen: %d ......", CmdServerModule::Instance().m_config.startPort);

    SocketAddress address2("0.0.0.0",CmdServerModule::Instance().m_config.startPort+1);
    ServerSocket skfd_video(address2);
    acceptor2=new SocketAcceptor<MsgHander>(skfd_video, _reactor);

    transLog.log(Message::PRIO_INFORMATION, "Video Listen: %d ......", CmdServerModule::Instance().m_config.startPort + 1);

    SocketAddress address3("0.0.0.0",CmdServerModule::Instance().m_config.startPort+2);
    ServerSocket skfd_audio(address3);
    acceptor3=new SocketAcceptor<MsgHander>(skfd_audio, _reactor);

    transLog.log(Message::PRIO_INFORMATION, "Audio Listen: %d ......", CmdServerModule::Instance().m_config.startPort + 2);

    SocketAddress address4("0.0.0.0",CmdServerModule::Instance().m_config.startPort+3);
    ServerSocket skfd_record(address4);
    acceptor4=new SocketAcceptor<MsgHander>(skfd_record, _reactor);

    transLog.log(Message::PRIO_INFORMATION, "Record Listen: %d ......", CmdServerModule::Instance().m_config.startPort + 3);

	while(!is_stop_)
	{
		_reactor.run();
	}
}