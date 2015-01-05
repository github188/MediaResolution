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
	//发送成功，返回发送的长度>0;-1:对方主动关闭或者网络掉线;-2:超时,-3:其他异常
	static int SafeSendData(StreamSocket &act_sock, char * sMsg, int nMsgLen,int timeout=6);
	//数据发送成功，返回true;发送失败,fanhui false
	static bool SendData(StreamSocket &dst,char* sMsg,int nMsgLen,int timeout=3);
	//接收数据成功,返回true,接收失败,返回false
	static bool Receive(StreamSocket &act_sock, char *sMsg,int nMsgLen,int timeout=3);
	static void GetAddr(StreamSocket &act_sock, string &ipStr,int &nPort);
	static void GetIpPort(StreamSocket &act_sock, string &ipStr);
};

#endif
