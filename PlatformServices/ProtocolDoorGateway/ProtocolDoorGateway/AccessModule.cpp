#include "AccessModule.h"
#include "Const.h"

using namespace ty;

AccessModule AccessModule::instance_;

AccessModule::AccessModule()
{
}

AccessModule::~AccessModule()
{
}

void AccessModule::Stop()
{
}

void AccessModule::InsertAsynMsg(const string& addr, Event* event, char* msg)
{
	FastMutex::ScopedLock lock(asyn_mutx_);
	pair<Event*, char*> event_data(event, msg);
	asyn_mgs_map_.insert(map<string, pair< Event*, char* > >::value_type(addr, event_data));
}

void AccessModule::NotifyAsynMsg(const string& addr, char* msg, int length)
{
	FastMutex::ScopedLock lock(asyn_mutx_);
	map<string, pair< Event*, char* > >::iterator it = asyn_mgs_map_.find(addr);
	if (it != asyn_mgs_map_.end())
	{
		pair<Event*, char*>& event_data = it->second;
		memcpy(event_data.second, msg, length);
		event_data.first->set();
		asyn_mgs_map_.erase(it);
	}
}

void AccessModule::RemoveAsynMsg(const string& addr)
{
	FastMutex::ScopedLock lock(asyn_mutx_);
	map<string, pair< Event*, char* > >::iterator it = asyn_mgs_map_.find(addr);
	if (it != asyn_mgs_map_.end())
		asyn_mgs_map_.erase(it);
}

int AccessModule::SendMsgWithRet(StreamSocket& act_sock, const char* msg, int msg_len, char* ret_msg, int& ret_len)
{
	int nRet = 0;

	bool ok = false;

	ok = NetUtility::SendData(act_sock, (char*)msg, msg_len);

	if (!ok)
		return ERR_SEND;


	ok = NetUtility::Receive(act_sock, ret_msg, sizeof(CmdHead));
	if (!ok)
		return ERR_RECV;

	CmdHead* pHead = (CmdHead*)ret_msg;
	if (PROTOCOL_VERSION != pHead->version)
		return ERR_VERSION;

	if (!NetUtility::Receive(act_sock, ret_msg + sizeof(CmdHead), pHead->length))
	{
		return ERR_RECV;
	}

	ret_len = pHead->length + sizeof(CmdHead);

	return nRet;
}

int AccessModule::SendMsgWithRet(const string& ip, int port, const char* msg, int msg_len, char* ret_msg, int& ret_len)
{
	StreamSocket act_socket;
	SocketAddress sa(ip.c_str(), port);
	try
	{
		act_socket.connect(sa);
	}
	catch (...)
	{
		return ERR_CONNECT;
	}

	return SendMsgWithRet(act_socket, msg, msg_len, ret_msg, ret_len);
}

int AccessModule::SendMsg(StreamSocket& act_sock, const char* msg, int msg_len)
{
	return NetUtility::SendData(act_sock, (char*)msg, msg_len) ? 0 : ERR_SEND;
}

int AccessModule::SendMsg(const string& ip, int port, const char* msg, int msg_len)
{
	StreamSocket act_socket;
	SocketAddress sa(ip.c_str(), port);
	try
	{
		act_socket.connect(sa);
	}
	catch (...)
	{
		return ERR_CONNECT;
	}

	return NetUtility::SendData(act_socket, (char*)msg, msg_len) ? 0 : ERR_SEND;
}

void AccessModule::GetTicket( char * ticket, int length)
{
	static char characterSet[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for( int i = 0; i< length - 1; i++ )
	{
		int n = rand () % (sizeof(characterSet) - 1);
		ticket[i] = characterSet[n];
	}
	ticket[length - 1] = 0;
}

bool AccessModule::HttpPost(int msg_id, string msg, std::string &res)
{
	try
	{
		Poco::Net::SocketAddress address(config_.center_ip_,config_.center_port_);
		Poco::Net::HTTPClientSession session(address);
		Poco::Timespan ts(3 * 1000 * 1000);
		session.setTimeout(ts);
		Poco::Net::HTTPRequest request( Poco::Net::HTTPRequest::HTTP_POST,/*CMSURLPATH*/"");
		request.setHost(config_.center_ip_, config_.center_port_);
		request.setContentType("application/x-www-form-urlencoded");
		request.setVersion("HTTP/1.1");

		char buf[256] = {0};
		snprintf(buf, 256, "messageId=%d&accessId=%s&xml=", msg_id, config_.local_ip_.c_str());

		std::string protocol;
		protocol.append(buf);
		protocol.append(msg);

		request.setContentLength(protocol.length());
		session.sendRequest(request) << protocol;

		Poco::Net::HTTPResponse response;
		std::istream& istr = session.receiveResponse(response);
		std::string str;
		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(istr, ostr);
		res = ostr.str();
		session.abort();
	}
	catch(...)
	{
		return false;
	}
	return true;
}