#include "stdafx.h"
#include "LogInfo.h"
#include "LogSys.h"
#include "Poco/Format.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "StrUtility.h"
#include "CmdServerModule.h"
#include "RegisterManager.h"
#include "HeartBeatManager.h"

using namespace Poco::Net;

CmdServerModule CmdServerModule::instance_;

CmdServerModule::CmdServerModule()
{
	heart_manager_ = new HeartBeatManager;
	register_manager_ = new RegisterManager;
	pShareObject=NULL;
}

CmdServerModule::~CmdServerModule()
{
}

bool CmdServerModule::Start(std::string strFilePath, int index, smp::ShareMemory* share)
{
	pShareObject=share;
	LogSys& failLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	typedef Poco::HashMap<std::string, std::pair<std::string,std::string>> ProtocolMap;
	ProtocolMap protocolMap;

	std::string full_xml_path(strFilePath);
	full_xml_path += Poco::format("MediaGateWayServerCfg_%d.xml", index);

	CMarkup xml;
	if (xml.Load(full_xml_path) && xml.FindElem("Config"))
	{
	}
	else
	{
		failLog.log(Message::PRIO_INFORMATION, "load TranslatorServerConfig.xml failed");
		return false;
	}

	m_config.localIP1 = xml.GetAttrib("LocalIP");
	m_config.localIP2 = xml.GetAttrib("LocalIP2");
	m_config.startPort = atol(xml.GetAttrib("StartPort").c_str());  
	m_config.rebootTime = xml.GetAttrib("RebootTime");
	m_config.rebootCycle = atol(xml.GetAttrib("RebootCycle").c_str());
	m_config.supportAlarm = atol(xml.GetAttrib("SupportAlarm").c_str());
	m_config.supportPlatform = atol(xml.GetAttrib("SupportPlatform").c_str());

	xml.IntoElem();

	if (xml.FindElem("Center") == false)
	{
		failLog.log(Message::PRIO_ERROR, "FindElem Center failed");
		return false;
	}

	m_config.center_ip_ = xml.GetAttrib("IP");
	m_config.center_port_ = atoi(xml.GetAttrib("Port").c_str());
	m_config.center_uri_ = xml.GetAttrib("Uri");

	if (xml.FindElem("DeviceModule") == false)
	{
		failLog.log(Message::PRIO_ERROR, "FindElem DeviceModule failed");
		return false;
	}

	xml.IntoElem();

	while (xml.FindElem("Module"))
	{

		std::string sName =  xml.GetAttrib("Name");
		//std::string sPlatformName =  xml.GetAttrib("PlatformName");
		std::string sProtocol = xml.GetAttrib("Protocol");
		std::string module_path = xml.GetAttrib("Path");

		std::pair<std::string,std::string> pa;
		pa.first = sName;
		pa.second = sProtocol;

		protocolMap[sName] = pa;

		std::string module_full_path = strFilePath;
		module_full_path += module_path;
		VDeviceManager::Instance().instance_.vd_factory_.LoadVDLibrary(module_full_path);
	}


	xml.OutOfElem();

	std::string dev_info;
	StrUtility dev_strutility;
	CMarkup dev_xml;
	string err_code;
	if (!(QueryDeviceInfoFromCenter(m_config.server_id_, dev_info) && dev_xml.SetDoc(dev_info) && 
		dev_strutility.ForeachXml(dev_xml) && dev_strutility.GetxmlValue("Information_code", err_code) && err_code.compare("000000")))
	{
		failLog.log(Message::PRIO_ERROR, "从中心获取设备列表失败");
		return false;
	}

	dev_strutility.GetxmlValue("Information/GateWayServer_Ip", m_config.accessIP);
	dev_strutility.GetxmlValue("Information/GateWayServer_Port", m_config.accessPort);

	//先连接信令网关服务
	try
	{
		SocketAddress addr(m_config.accessIP, m_config.accessPort);
		conn_socket_.connect(addr);
	}
	catch (...)
	{
		failLog.log(Message::PRIO_ERROR, "连接信令网关服务失败");
		return false;
	}

	deque<string> dev_names;
	deque<string> dev_ids;
	deque<string> dev_ips;
	deque<string> dev_ports;
	deque<string> dev_users;
	deque<string> dev_passwds;
	dev_strutility.GetxmlValue("Information/DeviceInfos/DeviceInfo_DeviceName", dev_names);
	dev_strutility.GetxmlValue("Information/DeviceInfos/DeviceInfo_DeviceID", dev_ids);
	dev_strutility.GetxmlValue("Information/DeviceInfos/DeviceInfo_IP", dev_ips);
	dev_strutility.GetxmlValue("Information/DeviceInfos/DeviceInfo_Port", dev_ports);
	dev_strutility.GetxmlValue("Information/DeviceInfos/DeviceInfo_User", dev_users);
	dev_strutility.GetxmlValue("Information/DeviceInfos/DeviceInfo_Passwd", dev_passwds);
	
	int i = 0;
	for (; i < dev_names.size(); ++i)
	{
		VDeviceConfig vplatform;

		vplatform.m_sPlatformName = dev_names[i];
		vplatform.m_sDeviceId = dev_ids[i];
		vplatform.m_sServerIP = dev_ips[i];
		vplatform.m_lPort = atol(dev_ports[i].c_str());
		vplatform.m_sUser = dev_users[i];
		vplatform.m_sPasswd = dev_passwds[i];

		vplatform.m_sClassName= protocolMap[vplatform.m_sPlatformName].first;
		vplatform.m_sProtocol= protocolMap[vplatform.m_sPlatformName].second;

		//add by sunminping 2014-10-22 解决没有注册的vp提示信息
		if(vplatform.m_sClassName.empty() || vplatform.m_sProtocol.empty())
		{
			failLog.log(Message::PRIO_NOTICE,"%s 对应的VirtualDevice 没有注册!",vplatform.m_sPlatformName);
			continue;
		}
		//end add 

		_deviceConfigs.insert(std::make_pair(vplatform.m_sDeviceId,vplatform));         
		register_manager_->deviceConfigsCopy.push_back(vplatform);
	}

	conn_thr_.start(*this);

	register_manager_->Start();
	heart_manager_->Start(m_config.accessIP,m_config.accessPort);

	return true;
}

void CmdServerModule::Stop()
{
	delete register_manager_;
	delete heart_manager_;
}

void CmdServerModule::Notify(vector<VDeviceConfig> &deviceCFGCopy)
{
    for (int i=0;i<(int)deviceCFGCopy.size();i++)
    {  
       if(deviceCFGCopy[i].status == VDeviceConfig::Success) //注册成功的设备
       {	
           map<string,VDeviceConfig>::iterator pos;
		   _mutex.lock();
           pos = _deviceConfigs.find(deviceCFGCopy[i].m_sDeviceId);

		   if(pos != _deviceConfigs.end()) 
		   {
				VDeviceConfig& vplatform_config = pos->second;

				device_map_.insert(map<string, string>::value_type(deviceCFGCopy[i].workID, deviceCFGCopy[i].m_sDeviceId));

				map<int,string>::iterator iter;
				for (iter = deviceCFGCopy[i].MediaChannls.begin();iter != deviceCFGCopy[i].MediaChannls.end(); iter++) 
				{
					vplatform_config.MediaChannls.insert(map<int, string>::value_type(iter->first, iter->second));//pChnInfo));
				}

				vplatform_config.max_in_ = deviceCFGCopy[i].max_in_;
				vplatform_config.max_out_ = deviceCFGCopy[i].max_out_;
				
				vplatform_config.status = deviceCFGCopy[i].status;//Success;
				vplatform_config.workID = deviceCFGCopy[i].workID;

				//统计点位信息
				//add by sunminping 2014-8-29 统计在线点位的个数
				//cout<<"pShareObject="<<pShareObject<<"  通道个数为:"<<vplatform_config.MediaChannls.size()<<endl;
				if(pShareObject!=NULL && vplatform_config.m_nCameraCount!=vplatform_config.MediaChannls.size())
				{
					//cout<<"当前点位数:"<<vplatform_config.m_nCameraCount<<";注册点位数"<<vplatform_config.MediaChannls.size()<<endl;
					pShareObject->nCameraCount-=vplatform_config.m_nCameraCount;
					pShareObject->nCameraCount+=vplatform_config.MediaChannls.size();
					vplatform_config.m_nCameraCount=vplatform_config.MediaChannls.size();
				}
				//通知到父进程
				//end add 
		   }
		   
		   _mutex.unlock();
       }
	   else
	   {
		   LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
		   transLog.log(Message::PRIO_ERROR, "device %s register fail......", deviceCFGCopy[i].m_sDeviceId);
	   }
    }

    deviceCFGCopy.clear();
 
	_mutex.lock();
	map<string, VDeviceConfig> deviceConfigs_Copy = _deviceConfigs;
	_mutex.unlock();

	register_manager_->deviceConfigsCopy.clear();

    map<string,VDeviceConfig>::iterator iter;
    for (iter = deviceConfigs_Copy.begin();iter != deviceConfigs_Copy.end(); iter++) 
    {   
       if(iter->second.status == VDeviceConfig::False) //失败后重新注册；
       {
            register_manager_->deviceConfigsCopy.push_back(iter->second);
       }
    }

}

void CmdServerModule::Copy(vector<VDeviceConfig> &deviceCFGCopy)
{
	_mutex.lock();
	map<string, VDeviceConfig> deviceConfigs_Copy = _deviceConfigs;
	_mutex.unlock();

	map<string,VDeviceConfig>::iterator iter;
	for (iter = deviceConfigs_Copy.begin(); iter != deviceConfigs_Copy.end(); iter++) 
	{   
		deviceCFGCopy.push_back(iter->second);
	}
}

void CmdServerModule::SetDeviceConfigStatus(string device_id)
{
	_mutex.lock();
	_deviceConfigs[device_id].status = VDeviceConfig::False;
	_mutex.unlock();
}

string CmdServerModule::FindByChannelID(string workID)
{   
    string deviceId = "";
    map<string,string>::iterator pos;
	Poco::ScopedLock<FastMutex> lock(_mutex);
	pos = channelMap.find(workID);
    
    if(pos != channelMap.end())
    {
       deviceId =  pos->second;
    }
    return deviceId;
}

string CmdServerModule::FindByInputID(string workID)
{
    string deviceId = "";
    map<string,string>::iterator pos;
	Poco::ScopedLock<FastMutex> lock(_mutex);
	pos = inputMap.find(workID);

    if(pos != inputMap.end())
    {
         deviceId =  pos->second;
    }
    return deviceId;
}
string CmdServerModule::FindByOutputID(string workID)
{
    string deviceId = "";
    map<string,string>::iterator pos;
	Poco::ScopedLock<FastMutex> lock(_mutex);
	pos = outputMap.find(workID);

    if(pos != outputMap.end())
    {
        deviceId =  pos->second;
    }
    return deviceId;
}

string CmdServerModule::FindByDeviceWorkID(string work_id)
{
	string deviceId = "";
	map<string,string>::iterator pos;
	Poco::ScopedLock<FastMutex> lock(_mutex);
	pos = device_map_.find(work_id);

	if(pos != device_map_.end())
	{
		deviceId =  pos->second;
	}
	return deviceId;
}

VDeviceConfig CmdServerModule::FindByDeviceID(string deviceID)
{
	VDeviceConfig vplatform_config;
	map<string,VDeviceConfig>::iterator pos;
	_mutex.lock();
	pos = _deviceConfigs.find(deviceID);

	if(pos != _deviceConfigs.end())
	{
		vplatform_config = pos->second;
	}

	_mutex.unlock();

	return vplatform_config;
}

bool CmdServerModule::QueryDeviceInfoFromCenter(const string& server_id, string& res_xml)
{
	string msg = format("<Information><VideoServerID>%s</VideoServerID></Information>", server_id);

	string body(m_config.center_uri_);
	body.append("?xml=");
	body.append(msg);

	Net::SocketAddress address(m_config.center_ip_, m_config.center_port_);
	Net::HTTPClientSession client(address);
	Net::HTTPRequest request(Net::HTTPRequest::HTTP_POST, body);
	request.setHost(m_config.center_ip_, m_config.center_port_);
	request.setContentType("application/x-www-form-urlencoded");
	request.setVersion("HTTP/1.1");

	try
	{
		client.sendRequest(request);
		Poco::Net::HTTPResponse response;
		std::istream& istr = client.receiveResponse(response);
		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(istr, ostr);
		res_xml = ostr.str();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CmdServerModule::SendToCmdServer(char* content, int length, int cmd_seq, CmdResponseCallback* call_back)
{
	AddCmdCallback(cmd_seq, call_back);
	bool is_ok = NetUtility::SafeSendData(conn_socket_, content, length);
	if (!is_ok)
		RemoveCmdCallback(cmd_seq);
	return is_ok;
}

void CmdServerModule::AddCmdCallback(int cmd_seq, CmdResponseCallback* call_back)
{
	FastMutex::ScopedLock lock(callback_mtx_);
	if (callbacks_.find(cmd_seq) == callbacks_.end())
	{
		callbacks_.insert(HashMap< int, SharedPtr<CmdResponseCallback> >::ValueType(cmd_seq, SharedPtr<CmdResponseCallback>(call_back)));
	}
}

void CmdServerModule::RemoveCmdCallback(int cmd_seq)
{
	HashMap< int, SharedPtr<CmdResponseCallback> >::Iterator iter;
	FastMutex::ScopedLock lock(callback_mtx_);
	if ((iter = callbacks_.find(cmd_seq)) != callbacks_.end())
		callbacks_.erase(iter);
}

void CmdServerModule::run()
{
	is_reading_ = true;
	while (is_reading_)
	{
		Timespan span(3, 0);
		if (conn_socket_.poll(span, Net::Socket::SELECT_READ))
		{
			CmdHead head;
			if (NetUtility::Receive(conn_socket_, (char*)&head, sizeof(head)))
			{
				char* msg(0);
				if (head.length != 0)
				{
					msg = new char[head.length];
					if (!NetUtility::Receive(conn_socket_, msg, head.length))
					{
						delete[] msg;
						continue;
					}
				}

				if (head.cmdnum % 2)
				{
					SharedPtr<CmdResponseCallback> call_back;
					HashMap< int, SharedPtr<CmdResponseCallback> >::Iterator iter;
					{
						FastMutex::ScopedLock lock(callback_mtx_);
						if ((iter = callbacks_.find(head.cmdseq)) != callbacks_.end())
							call_back = iter->second;
					}

					if (call_back.get())
						call_back->OnResponse(head, msg);
				}
				else
				{
					VDeviceManager::Instance().OnRecvRequest(head, (const unsigned char*)msg, conn_socket_);
				}

				if (msg)
					delete[] msg;
			}
		}
	}
}