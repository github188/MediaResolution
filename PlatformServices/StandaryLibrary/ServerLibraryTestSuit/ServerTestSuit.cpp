// testreactor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Poco/Mutex.h"
#include "BussinessServer.h"
#include "BussinessEvent.h"
#include "Operator.h"
#include "Global.h"
#include "NetUtility.h"

#ifdef _DEBUG
	#pragma comment(lib,"ServerLibraryd.lib")
#else
	#pragma comment(lib,"ServerLibrary.lib")
#endif 

class Logic : public BussinessEvent
{
public:

	//返回值等于-1，代表服务器需要主动释放客户端套接字
	//返回值等于-2，代表服务器不需再监听该套接字可读事件及也不需要释放客户端套接字
	int onReadable(Operator &op)
	{

		char tt[20] = "";
		if(!NetUtility::Receive(op.hand->_socket,(char*)tt,20))
		{
			return -1;
		}

		printf("receive message %s\n", tt);

		return -1;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{

	Logic logicevent;
	BussinessServer bsever((BussinessEvent *)&logicevent);
	bsever.start();
	Thread testThr;
	SocketReactor _reactor;
	SocketAddress aa("0.0.0.0",9300);
	ServerSocket ss_msg(aa);
	SocketAcceptor<MsgHander> acceptor(ss_msg, _reactor);

	_reactor.run();

	getchar();

	return 0;
}

