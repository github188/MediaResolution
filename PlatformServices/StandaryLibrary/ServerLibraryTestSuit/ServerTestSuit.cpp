// testreactor.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//����ֵ����-1�������������Ҫ�����ͷſͻ����׽���
	//����ֵ����-2����������������ټ������׽��ֿɶ��¼���Ҳ����Ҫ�ͷſͻ����׽���
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

