#include "UserHistoryRequest.h"
#include "Term.h"
#include "AccessModule.h"

UserHistoryRequest::UserHistoryRequest(void)
{

}


UserHistoryRequest::~UserHistoryRequest(void)
{

}

int UserHistoryRequest::Run(Operator& op, CmdHead& head, char* body)
{
	string protocol;
	protocol.append(body, head.length);

	CMarkup xml;
	StrUtility utility;
	if (!xml.SetDoc(protocol) || !utility.ForeachXml(xml))
	{
		//������ظ�
		return -1;
	}

	int storeType = 2;
	utility.GetxmlValue("Information/StoreType", storeType);

	LogSys& log = LogSys::getLogSys(LogInfo::Instance().ProtocolDoorGateway(), true);

	switch (storeType)
	{
	case PROTOCOL_SERVER_STORE:
		{
			break;
		}
	case PROTOCOL_TERM_STORE:
		{
			string term_id;
			term_id.append(head.dst, PROTOCOL_ID_LENGTH);

			Term* pTerm = NULL;
			if (!AccessModule::Instance().term_manager_.FindTerm(term_id, pTerm) || !pTerm->is_active_)
			{
				//����ظ�
				log.log(Message::PRIO_ERROR, "��ѯ��ʷ��Ƶʧ��, �豸����");
				AnswerErrorRes(op.hand->_socket, head, ERR_TERM_NOTEXIST, "��ѯ��ʷ��Ƶʧ��, �豸����");
				return -1;
			}

			//���͸�ǰ��
			char send_buf[1024] = {0};
			int send_len = head.length + sizeof(CmdHead);

			memcpy(send_buf, &head, sizeof(CmdHead));
			memcpy(send_buf + sizeof(CmdHead), body, head.length);

			char rev_buf[20 * 1024] = {0};

			if (!OperateLongSockEvent(pTerm->socket_, pTerm, send_buf, send_len, rev_buf))
			{
				//�ظ�
				log.log(Message::PRIO_ERROR, "��ѯ��ʷ��Ƶʧ��, ǰ���޻�Ӧ");
				AnswerErrorRes(op.hand->_socket, head, ERR_RECV, "��ѯ��ʷ��Ƶʧ��, ǰ���޻�Ӧ");
				return -1;
			}

			CmdHead res_head;
			memcpy(&res_head, rev_buf, sizeof(CmdHead));
			
			if (res_head.success != 0 || res_head.version != PROTOCOL_VERSION)
			{	
				AccessModule::Instance().SendMsg(op.hand->_socket, rev_buf, sizeof(CmdHead) + res_head.length);
				return -1;
			}

			string res;
			res.append(rev_buf + sizeof(CmdHead), res_head.length);
			xml.SetDoc(res);
			xml.AddChildElem("StoreIP", pTerm->nat_ip_);
			xml.AddChildElem("StorePort", pTerm->video_port_);

			AnswerCorrectRes(op.hand->_socket, head, xml.GetDoc());

			break;
		}
	default:
		{

		}
	}

	return -1;
}
