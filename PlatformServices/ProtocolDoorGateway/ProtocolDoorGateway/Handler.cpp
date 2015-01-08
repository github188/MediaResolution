#include "Handler.h"
#include "AccessModule.h"
#include "Common.h"

CHandler::CHandler()
{

}

CHandler::~CHandler()
{

}

bool CHandler::OperateLongSockEvent(StreamSocket& sock, Device* dev, char* send_buffer, int send_len, char* recv_buffer)
{
	LocalDateTime trylock_time;
	FastMutex::ScopedLock lock(dev->status_mtx_);
	LocalDateTime getlock_time;

	Timespan span = getlock_time - trylock_time;

	LogSys& access_log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);

	if (span.totalSeconds() >= 5)
	{
		access_log.log(Message::PRIO_ERROR, "get lock device:%s too long time, waiting %d seconds", dev->id_, span.totalSeconds());
		return false;
	}

	{
		FastMutex::ScopedLock lock(dev->nat_sock_mtx_);
		if (!NetUtility::SendData(sock, send_buffer, send_len))
		{
			access_log.log(Message::PRIO_ERROR, "发送操作指令到前端%s设备失败", dev->id_);
			return false;
		}
	}

	Event wait_event;
	AccessModule::Instance().InsertAsynMsg(dev->nat_addr_, &wait_event, recv_buffer);
	if (!wait_event.tryWait(3000))
	{
		access_log.log(Message::PRIO_ERROR, "等待前端设备:%s响应超时", dev->id_);
		AccessModule::Instance().RemoveAsynMsg(dev->nat_addr_);
		return false;
	}

	return true;
}

int CHandler::AnswerCorrectRes(StreamSocket& sock, CmdHead& head, string body)
{
	CmdHead res_head;
	res_head.cmdnum = head.cmdnum + 1;
	res_head.success = 0;
	res_head.length = body.length();

	int send_len = sizeof(CmdHead) + body.length();
	char* send_buf = new char[send_len];
	memcpy(send_buf, &res_head, sizeof(CmdHead));
	
	if (!body.empty())
	{
		memcpy(send_buf + sizeof(CmdHead), body.c_str(), body.length());
	}

	int nSucc = AccessModule::Instance().SendMsg(sock, send_buf, send_len);

	delete[] send_buf;

	return nSucc;
}

int CHandler::AnswerErrorRes(StreamSocket& sock, CmdHead& head, int nRet, string body)
{
	CMarkup xml;
	xml.AddElem("Information");
	xml.AddChildElem("ErrorDescription", body);

	string res_body = xml.GetDoc();

	CmdHead res_head;
	res_head.cmdnum = head.cmdnum + 1;
	res_head.success = nRet;
	res_head.length = res_body.length();

	int send_len = sizeof(CmdHead) + res_body.length();
	char* send_buf = new char[send_len];
	memcpy(send_buf, &res_head, sizeof(CmdHead));
	memcpy(send_buf + sizeof(CmdHead), res_body.c_str(), res_body.length());

	int nSucc = AccessModule::Instance().SendMsg(sock, send_buf, send_len);

	delete[] send_buf;

	return nSucc;
}