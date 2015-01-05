#include "stdafx.h"
#include "VDeviceManager.h"
#include "LogInfo.h"

VDeviceManager VDeviceManager::instance_;

VDeviceManager::VDeviceManager()
{
    InitHandle();
	//add by sunminping 2014-6-19  扫描配置文件
	m_ScanFile.Start();
	//add end
}

VDeviceManager::~VDeviceManager()
{
	Poco::ScopedLock<FastMutex> lock(m_Mutex);

	for (std::map<std::string,VDeviceServer*>::iterator ptr=m_PlatformServerMap.begin();ptr!=m_PlatformServerMap.end();ptr++)
	{
		VDeviceServer* server=ptr->second;
		delete server;
	}

	m_PlatformServerMap.clear();

	m_ScanFile.Stop();
}

void VDeviceManager::InitHandle()
{
	RequestHandles_.insert(std::map<int, RequestHandle>::value_type(CMD_DEVREALTIME, &VDeviceManager::OnRealVideoRequest));
	RequestHandles_.insert(std::map<int, RequestHandle>::value_type(CMD_DEVREALSTREAM, &VDeviceManager::OnRealVideoStreamRequest));
	RequestHandles_.insert(std::map<int, RequestHandle>::value_type(CMD_DEVHISTROY, &VDeviceManager::OnRecordVideoRequest));
	RequestHandles_.insert(std::map<int, RequestHandle>::value_type(CMD_DEVRECORDSTREAM, &VDeviceManager::OnRecordVideoStreamRequest));
}

int VDeviceManager::AddVDeviceServer(VDeviceConfig& rConfig)
{
	VDeviceServer* server = Find(rConfig.m_sDeviceId);
	if (server !=NULL)
	{
		return 0;
	}

	server=new VDeviceServer(this);
	if (server->AddVirtualDevice(rConfig)==-1)
	{
		delete server;
		return -1;
	}

	Poco::ScopedLock<FastMutex> lock(m_Mutex);
	m_PlatformServerMap[rConfig.m_sDeviceId]=server;
	return 0;

}

int VDeviceManager::InitVDeviceServer(VDeviceConfig& rConfig)
{
	VDeviceServer* pServer = Find(rConfig.m_sDeviceId);
	if (pServer == NULL)
	{
		return -1;
	}
	int nRet= pServer->InitVirtualDevice(rConfig);

	return nRet;	
}

void VDeviceManager::RemoveVDeviceServer(VDeviceConfig& rConfig)
{
	Poco::ScopedLock<FastMutex> lock(m_Mutex);
	PlatformMap::iterator ptr=m_PlatformServerMap.find(rConfig.m_sDeviceId);
	if (ptr==m_PlatformServerMap.end())
	{
		return ;
	}
	VDeviceServer* server=ptr->second;
	delete server;
	m_PlatformServerMap.erase(ptr);
}

VDeviceServer* VDeviceManager::Find(string deviceId)
{
	Poco::ScopedLock<FastMutex> lock(m_Mutex);
	PlatformMap::iterator itrVPServer=m_PlatformServerMap.find(deviceId);
	if (itrVPServer!=m_PlatformServerMap.end())
	{
		return itrVPServer->second;
	}
	else
	{
		return NULL;
	}
}

 int VDeviceManager::OnRecvRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket& sock)
 {  
	VDeviceManager::RequestHandle handle=NULL;
	map<int, RequestHandle>::iterator itr = RequestHandles_.find(head.cmdnum);

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

    if (itr == RequestHandles_.end() || itr->second ==NULL)
	{
		media_log.log(Message::PRIO_ERROR, "未知的操作指令： %d",head.cmdnum);
		return 0;
	}

	handle=itr->second;

	string dst_id;
	dst_id.assign(head.dst);
	media_log.log(Message::PRIO_INFORMATION, "操作指令： %d, 目的id %s", head.cmdnum, dst_id);

    return (this->*handle)(head, pBuffer, sock);
}

 int VDeviceManager::OnRealVideoRequest(const CmdHead& head, const unsigned char* pBuffer, StreamSocket& sock)
 {
	 std::string deviceid;
	 deviceid.assign(head.dst);
    
	 LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	 VDeviceServer* server=Find(deviceid);
	 if (server==NULL)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s不存在", head.cmdnum, deviceid);
		 return 0;
	 }

	 VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);
	 if(config.status==VDeviceConfig::False)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s未成功注册", head.cmdnum, deviceid);
		 return 0;
	 }

	 return server->OnRealVideoRequest(head, pBuffer, sock);
 }


 int VDeviceManager::OnRealVideoStreamRequest(const CmdHead& head, const unsigned char* pBuffer, StreamSocket& sock)
 {
     std::string deviceid;
     deviceid.assign(head.dst);
    
	 LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
	
	 VDeviceServer* server=Find(deviceid);
     if (server==NULL)
     {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s不存在", head.cmdnum, deviceid);
		 return 0;
     }

	 VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);
	 if(config.status==VDeviceConfig::False)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s未成功注册", head.cmdnum, deviceid);
		 return 0;
	 }

	 CMarkup mark_xml;
	 int channelNo = 0;
	 string token;
	 StrUtility utility;
	 int stream_type = 0;
	 if (!(mark_xml.SetDoc((const char*)pBuffer) && utility.ForeachXml(mark_xml) && utility.GetxmlValue("Information/ChannelNum", channelNo)
		 && utility.GetxmlValue("Information/Token", token) && utility.GetxmlValue("Information/StreamType", stream_type)))
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s错误的XML格式", head.cmdnum, deviceid);
		 return 0;
	 }

	 if (!server->FindToken(token))
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s非法的token: %s", head.cmdnum, deviceid, token);
		 return 0;
	 }

     return server->OnRealVideoStreamRequest(channelNo, stream_type, sock);
 }

 int VDeviceManager::OnRecordVideoRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock)
 {
     std::string deviceid;
     deviceid.assign(head.dst);

	 LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	 VDeviceServer* server=Find(deviceid);
	 if (server==NULL)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s不存在", head.cmdnum, deviceid);
		 return 0;
	 }

	 VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);
	 if(config.status==VDeviceConfig::False)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s未成功注册", head.cmdnum, deviceid);
		 return 0;
	 }

	 int channelNo = 0;
	 //end modify 
	 //int channelNo = config.MediaChannls[strId].ChannelID;

     return server->OnRecordVideoRequest(head, pBuffer, sock);
 }

 int VDeviceManager::OnRecordVideoStreamRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket &sock)
 {
	 std::string deviceid;
	 deviceid.assign(head.dst);

	 LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	 VDeviceServer* server=Find(deviceid);
	 if (server==NULL)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s不存在", head.cmdnum, deviceid);
		 return 0;
	 }

	 VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);
	 if(config.status==VDeviceConfig::False)
	 {
		 media_log.log(Message::PRIO_ERROR, "命令:%d请求设备:%s未成功注册", head.cmdnum, deviceid);
		 return 0;
	 }

	 int channelNo = 0;

	 return server->OnRecordVideoStreamRequest(head,(char*)pBuffer, sock);
 }

 int VDeviceManager::OnGetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
	 std::string strId;
	 strId.assign(head.dst);
     string deviceid = CmdServerModule::Instance().FindByChannelID(strId);
     VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);
	 
	 //add by sunminping 2014-7-1
	 if(config.status==VDeviceConfig::False)
	 {
		 return -1;
	 }

	 
	 int nChannel = 0;
	 //end add
	 
	 VDeviceServer* server=Find(deviceid);

	 if (server==NULL)
	 {
		 return -1;
	 }
	 server->OnGetVideoPara(head,pBuffer,nLength,op,nChannel);

     return -1;
 }

 int VDeviceManager::OnSetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strId;
     strId.assign(head.dst);
	 string deviceid = CmdServerModule::Instance().FindByChannelID(strId);
	 VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);

	 //add by sunminping 2014-7-1
	 if(config.status==VDeviceConfig::False)
	 {
		 return -1;
	 }


	 int nChannel = 0;
	 //end add

	 VDeviceServer* server=Find(deviceid);
     
     if (server==NULL)
     {
        return -1;
     }
     server->OnSetVideoPara(head,pBuffer,nLength,op,nChannel);
     return -1;
 }

 int VDeviceManager::OnGetAlarmOut(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strMsg;
     CMegaeyesAlarmOut alarmOut;
     try
     {
        alarmOut.UnSerialize(strMsg);
     }
     catch (...)
     {
        return -1;
     }
	 std::string strId;
	 strId.assign(head.dst);
	 string deviceid = CmdServerModule::Instance().FindByOutputID(strId);
	 VDeviceConfig config = CmdServerModule::Instance().FindByDeviceID(deviceid);
	 if(config.status==VDeviceConfig::False)
	 {
		 return -1;
	 }


	 int channelNo = 0;

     VDeviceServer* server=Find(deviceid);
     if (server==NULL)
     {
        return -1;
     }
     server->OnGetAlarmOut(head,alarmOut,nLength,channelNo,op);
	 return -1;
 }

 int VDeviceManager::OnSetAlarmOut(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strMsg;
     strMsg.assign((char*)pBuffer);
     strMsg[nLength]='\n';
     CMegaeyesAlarmOut alarmOut;
     try
     {
		 alarmOut.UnSerialize(strMsg);
	 }
     catch (...)
     {
        return -1;
     }

	 std::string strId;
	 strId.assign(head.dst);

	 //add by sunminping 控制输出报警的目的id为设备业务id 2014-06-19
	 int	nChannel=-1;
	 string deviceid=CmdServerModule::Instance().FindByOutputID(strId);
	 VDeviceConfig config=CmdServerModule::Instance().FindByDeviceID(deviceid);

	 VDeviceServer* server=Find(deviceid);

     if (server==NULL)
     {
        return -1;
     }
     server->OnSetAlarmOut(head,alarmOut,nLength,nChannel,op);

     return -1;
 }

 int VDeviceManager::OnNetParam(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strId;
     strId.assign(head.dst);
     string deviceid = CmdServerModule::Instance().FindByDeviceWorkID(strId);
     VDeviceServer* server=Find(deviceid);
     if (server==NULL)
     {
         return -1;
     }
     server->OnNetParam(head,pBuffer,nLength,op);

     return -1;
 }

 int VDeviceManager::OnSetVideoDectePlan(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strId;
     strId.assign(head.dst);
     //int nChannel=0;
     //VDeviceServer* server=FindVPServerByCameraId(strId);
     string deviceid = CmdServerModule::Instance().FindByChannelID(strId);
     int nChannel = 0;
     VDeviceServer* server=Find(deviceid);
     if (server==NULL)
     {
         return -1;
     }
     server->OnSetAlarmPlan(head,pBuffer,nLength,op,nChannel);
     
     return -1;
 }

 int VDeviceManager::OnSetAlarmPlan(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strId;
     strId.assign(head.dst);
     int nChannel=0;
     VDeviceServer* server=NULL;//FindVPServerByAlarmOutId(strId,nChannel);
     if (server==NULL)
     {
         return -1;
     }
     server->OnSetAlarmPlan(head,pBuffer,nLength,op,nChannel);
     return -1;
 }

 int VDeviceManager::OnGetPtzStatus(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strId;
     strId.assign(head.dst);
     //int nChannel=0;
     //VDeviceServer* server=FindVPServerByCameraId(strId,nChannel);
     string deviceid = CmdServerModule::Instance().FindByChannelID(strId);
     int nChannel = 0;
     VDeviceServer* server=Find(deviceid);
     if (server==NULL)
     {
         return -1;
     }
     server->OnGetPtzStatus(head,pBuffer,nLength,op,nChannel);

     return -1;
 }


 int VDeviceManager::OnPtzCtrl(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
     std::string strId;
     strId.assign(head.dst);

     string deviceid = CmdServerModule::Instance().FindByChannelID(strId);
     int nChannel = 0;

     VDeviceServer* server=Find(deviceid);
     if (server==NULL)
     {
         return -1;
     }
     server->OnPtzCtrl(head,pBuffer,nLength,op,nChannel);
     
     return -1;
 }

 //add by sunminping 判断指定的设备IP是否需要打印报警日志信息
 bool VDeviceManager::IsExistsIp(const string & ip)
 {
	 return m_ScanFile.IsExistsIp(ip);
 }

 bool VDeviceManager:: ListenAlarmInfo(string deviceid)
 {
	 VDeviceServer* server=Find(deviceid);
	 if (server==NULL)
	 {
		 return -1;
	 }
	return server->ListenAlarmInfo();
 }

 //end add 