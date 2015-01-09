#include "Global.h"
#include "AccessEvent.h"
#include "AccessModule.h"
#include "Common.h"

int main(int argc, char *argv[])
{
//#ifdef WIN32
#if defined(WIN32)
#else
monitor:
	pid_t childpid = fork();
	if (childpid < 0)
	{
		exit(0);
	}
	else if (childpid > 0)
	{
		int status;
		pid_t pid = waitpid( childpid, &status, 0);
		if (pid < 0)
		{
			exit(0);
		}

		if (WIFEXITED(status))
		{
			int exitcode = WEXITSTATUS(status);
			if( 0 == exitcode )
			{
				printf("exit code");
				exit(0);
			}
		}

		sleep(3);
		goto monitor;
	}
#endif

	LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);
	log.setLogLevel("debug");

	if (!AccessModule::Instance().config_.Parse("./config.xml"))
	{
		log.log(Message::PRIO_ERROR, "Read configuration file failed");
		return 0;
	}

	ThreadPool::defaultPool().start(AccessModule::Instance().term_manager_);
	ThreadPool::defaultPool().start(AccessModule::Instance().user_manager_);

	AccessEvent logicevent;
	BussinessServer accesssever((BussinessEvent *)&logicevent, 100);
	accesssever.start();

	SocketReactor reactor;
	std::string local_ip = "0.0.0.0";
	try
	{
		SocketAddress user_addr(local_ip, AccessModule::Instance().config_.user_port_);
		SocketAddress term_addr(local_ip, AccessModule::Instance().config_.term_port_);

		ServerSocket user_server_sock(user_addr, 128);
		ServerSocket term_server_sock(term_addr, 128);

		SocketAcceptor<MsgHander> acceptor_user(user_server_sock, reactor);
		SocketAcceptor<MsgHander> acceptor_tem(term_server_sock, reactor);

		log.log(Message::PRIO_INFORMATION, "*******************************************************");
		log.log(Message::PRIO_INFORMATION, "        Start Protocol Door Gateway Server Success       ");
		log.log(Message::PRIO_INFORMATION, "*******************************************************");

		reactor.run();
	}
	catch (...)
	{
		return false;
	}
	return 0;
}
