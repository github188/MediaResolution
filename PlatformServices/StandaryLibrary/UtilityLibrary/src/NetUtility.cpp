#include "NetUtility.h"


int NetUtility::SafeSendData( StreamSocket &act_sock, char * sMsg, int nMsgLen,int timeout)
{
	int bRet=0;
	int number = 0;
	try
	{
		if(timeout!=act_sock.getSendTimeout().totalSeconds()) act_sock.setSendTimeout(Timespan(timeout,0));
	}
	catch(...)
	{
		return false;
	}
	while(number<nMsgLen)
	{
		try
		{
			int nRecv = act_sock.sendBytes(sMsg+number,nMsgLen-number);
			number=number+nRecv;
			bRet=number;
		}
		catch(Exception& ex)
		{
			if(POCO_ESYSNOTREADY==ex.code())   //10054 错误码
				bRet=-1;
			else if(POCO_ETIMEDOUT==ex.code()) //10060 错误码
				bRet=-2;
			else                               //其他  错误码
				bRet=-3;
			
			break;
		}
	}
	return bRet;
}

bool NetUtility::SendData(StreamSocket &act_sock, char * sMsg, int nMsgLen,int timeout)
{
	bool bRet = true;
	int number = 0;
	try
	{
		if(timeout!=act_sock.getReceiveTimeout().totalSeconds()) act_sock.setReceiveTimeout(Timespan(timeout,0));
	}
	catch (...)
	{
		return false;
	}

	while(number<nMsgLen)
	{
		try
		{
			int nRecv = act_sock.sendBytes(sMsg+number,nMsgLen-number);
			number+=nRecv;
		}
		catch (...)
		{
			bRet = false;
			break;
		}
	}
	return bRet;
}

bool NetUtility::Receive(StreamSocket &act_sock, char *sMsg,int nMsgLen,int timeout)
{
	bool bRet = true;
	int number = 0;
	try
	{
		if(timeout!=act_sock.getReceiveTimeout().totalSeconds()) act_sock.setReceiveTimeout(Timespan(timeout,0));
	}
	catch (...)
	{
		return false;
	}

	while(number<nMsgLen)
	{
		try
		{
			int nRecv = act_sock.receiveBytes(sMsg+number,nMsgLen-number);
			if(nRecv==0)	//客户端主动关闭连接
			{
				bRet = false;
				break;
			}
			number+=nRecv;
		}
		catch (...)
		{
			bRet = false;
			break;
		}
	}
	return bRet;
}

void NetUtility::GetAddr(StreamSocket &act_sock,std::string &ipStr,int &nPort)
{
	ipStr=act_sock.peerAddress().host().toString();
	nPort=act_sock.peerAddress().port();
}
void NetUtility::GetIpPort(StreamSocket &act_sock,std::string &ipStr)
{
	ipStr=act_sock.peerAddress().toString();
}
