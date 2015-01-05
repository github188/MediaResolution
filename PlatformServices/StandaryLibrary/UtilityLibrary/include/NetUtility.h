#ifndef NetUtility_H
#define NetUtility_H

#include "Poco/Timespan.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/NetException.h"

using namespace Poco;
using namespace Poco::Net;
using namespace std;

class NetUtility
{
public:
	//���ͳɹ������ط��͵ĳ���>0;-1:�Է������رջ����������;-2:��ʱ,-3:�����쳣
	static int SafeSendData(StreamSocket &act_sock, char * sMsg, int nMsgLen,int timeout=6);
	//���ݷ��ͳɹ�������true;����ʧ��,fanhui false
	static bool SendData(StreamSocket &dst,char* sMsg,int nMsgLen,int timeout=3);
	//�������ݳɹ�,����true,����ʧ��,����false
	static bool Receive(StreamSocket &act_sock, char *sMsg,int nMsgLen,int timeout=3);
	static void GetAddr(StreamSocket &act_sock, string &ipStr,int &nPort);
	static void GetIpPort(StreamSocket &act_sock, string &ipStr);
};

#endif
