#include "AccessEvent.h"
#include "Common.h"
#include "AccessModule.h"

int AccessEvent::onReadable(Operator &op)
{
	CmdHead head;
	if (!NetUtility::Receive(op.hand->_socket, (char*)&head, sizeof(CmdHead)))
	{
		return -1;
	}

	if (head.version != PROTOCOL_VERSION || head.length > PROTOCOL_MAX_LENGTH)
	{
		LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);
		log.log(Message::PRIO_INFORMATION, "ip %s,%d->指令版本错误[%d]或包体长度[%d]过大", op.hand->_socket.peerAddress().toString(), head.cmdnum, head.version, head.length);
		return -1;
	}

	char buffer[PROTOCOL_MAX_LENGTH] = {0};

	if (!NetUtility::Receive(op.hand->_socket, (char*)buffer, head.length))
	{
		return -1;
	}

	if (head.cmdnum % 2 == 1)
	{
		char* res_buffer = new char[sizeof(CmdHead) + head.length];
		memcpy(res_buffer, &head, sizeof(CmdHead));
		memcpy(res_buffer + sizeof(CmdHead), buffer, head.length);
		AccessModule::Instance().NotifyAsynMsg(op.hand->_socket.peerAddress().toString(), res_buffer, sizeof(CmdHead) + head.length);
		delete[] res_buffer;
		return 0;
	}

	return OnRecvMsg(op, head, buffer);
}

int AccessEvent::OnRecvMsg(Operator &op, CmdHead& head, char* body)
{
	CHandler * pHandler = FactoryHandler::GetHandler(head.cmdnum);
	if (pHandler == NULL)
	{
		//无此消息
		LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);
		log.log(Message::PRIO_INFORMATION, "ip %s,%d->未识别的指令号", op.hand->_socket.peerAddress().toString(), head.cmdnum);
		return -1;
	}

	int nRet = pHandler->Run(op, head, body);
	delete pHandler;
	return nRet;
}

