#include "stdafx.h"
#include "CmdServerModule.h"
#include "VDeviceServer.h"
#include "AlarmServer.h"
#include "LogInfo.h"
#include "LogSys.h"
#include "VDeviceManager.h"

VDeviceServer::VDeviceServer(VDeviceManager* pManager):m_pPlatform(NULL),pVPManager(pManager)
{

}

VDeviceServer::~VDeviceServer()
{
	if(m_pPlatform!=NULL)
	{
		try
		{
			m_pPlatform->Destroy();
			//ReleaseMegaObject(m_pPlatform);
			m_pPlatform=NULL;
		}
		catch(...)
		{
			m_pPlatform=NULL;
		}

	}
}

int VDeviceServer::AddVirtualDevice(VDeviceConfig& rConfig)
{
    try
    {
	    if (m_pPlatform!=NULL)
	    {
		    return 0;
	    }

		//CreateMegaObject("VirtualDevice", rConfig.m_sClassName.c_str(), (void**)&m_pPlatform);
		m_pPlatform = pVPManager->vd_factory_.MakeVirtualDevice(rConfig.m_sClassName);

        if (m_pPlatform==NULL)
        {
            return -1;
        }
		
		m_rConfig=rConfig;           //保存配置信息

        VDeviceInfo info;
		info.deviceID = rConfig.m_sDeviceId;
        info.serverIP = rConfig.m_sServerIP;
        info.port = rConfig.m_lPort;
        info.user = rConfig.m_sUser;
        info.passwd = rConfig.m_sPasswd;
        info.extendPara1=rConfig.m_sExtendPara1;
        info.deviceType = rConfig.m_sClassName;
		info.vp_log = &LogSys::getLogSys(LogInfo::Instance().VirtualDevice());

        m_pPlatform->BaseInit(this,info);
		
		return 0;
	}
	catch (...)
	{
        ReleaseVirtualDevice();
        return -1;	
	}	
}

void VDeviceServer::ReleaseVirtualDevice()
{
	if(m_pPlatform!=NULL)
	{
		try
		{
			//ReleaseMegaObject(m_pPlatform);
			//pVPManager->vd_factory_.
			m_pPlatform=NULL;
		}
		catch(...)
		{
			m_pPlatform=NULL;
		}

	}
}

int VDeviceServer::InitVirtualDevice(VDeviceConfig& rConfig)
{
	if (m_pPlatform==NULL)
	{
		return false;
	}

	m_pPlatform->Destroy();

	int ret = m_pPlatform->Init(rConfig);
	m_rConfig = rConfig;
	return ret;
}

bool VDeviceServer::ListenAlarmInfo()
{
	if (m_pPlatform==NULL)
	{
		return false;
	}

	return m_pPlatform->SetupAlarmChannel();
}

void VDeviceServer::NotifyStreamData(long lChannel, long lStreamType, long lFrameType, PBYTE pBuffer, DWORD dwSize, void* dm)
{   
	long key;
	string streamType;  
	if(lStreamType == video_stream)
	{
		key = GetChannelPlayId(lChannel,true);
		streamType ="MainStream";
	}
	else
	{
		key = GetChannelPlayId(lChannel,false);
		streamType ="SubStream1";
	}

	DistributionManager* distr_mgr = (DistributionManager*)dm;
	if (lFrameType == head_frame)
	{
		CmdHead hi;
		hi.length = dwSize;
		hi.cmdnum = CMD_DEVREALSTREAM_RESP;
		hi.success = 0; 
		memcpy(distr_mgr->memory_pool_.extradata_,&hi,sizeof(CmdHead));
		memcpy(distr_mgr->memory_pool_.extradata_+sizeof(CmdHead),pBuffer,dwSize);
		distr_mgr->memory_pool_.extradata_length_ = dwSize + sizeof(hi);
	}
	else
	{       
		long lPacketSize = dwSize;
		long lFragSize = PACK_LENGTH - SECOND_FRAME_HEADER_LEN;
		
		if ( lPacketSize <= lFragSize )
		{
			QueuePacket2(true, SeqNumGenerator::Instance().Generate(), 0, lFrameType, pBuffer, dwSize, distr_mgr);
		}
		else
		{
			long lSeq = SeqNumGenerator::Instance().Generate();
			long lPacketNum = lPacketSize / lFragSize ;
			long lCopySize = 0;
			bool bFirst=true;
			while ( lPacketSize > 0 )
			{
				long lFrameSize = lPacketSize > lFragSize ? lFragSize : lPacketSize;
				QueuePacket2(bFirst,lSeq, lPacketNum , lFrameType, pBuffer + lCopySize,lFrameSize, distr_mgr);
				bFirst=false;
				lPacketSize -= lFrameSize;
				lCopySize += lFrameSize;
				lPacketNum-- ;
			}
		}
	}
}

int VDeviceServer::OnRealVideoRequest(const CmdHead& head, const unsigned char* pBuffer, StreamSocket& sock)
{

	std::string strMsg;
	strMsg.assign((char*)(pBuffer));

	CMD10008RealReqSerialize realRequest;
	CMD10009RealResSerialize realResponse;
	realResponse.m_cmdHead.success=0;
	try
	{
		realRequest.UnSerialize(strMsg);
	}
	catch (Poco::Exception& excep)
	{
		realResponse.m_cmdHead.success = 1;
		realResponse.err_description = excep.message();
	}

	if (0 == realResponse.m_cmdHead.success)
	{
		
		if (realRequest.channel_ > m_rConfig.MediaChannls.size())
		{
			realResponse.m_cmdHead.success = 1;
			realResponse.err_description = Poco::format("指令:%d 请求设备:%s错误的通道号%d", head.cmdnum, string(head.dst), realRequest.channel_);
		}
		else
		{
			FastMutex::ScopedLock lock(token_mtx_);
			tokens_.insert(realRequest.token_);
		}
	}

	memcpy(realResponse.m_cmdHead.src, head.dst, sizeof(head.dst));
	memcpy(realResponse.m_cmdHead.dst, head.src, sizeof(head.src));
	realResponse.m_cmdHead.cmdseq = head.cmdseq;
	string content =  realResponse.Serialize();

	int send_len=NetUtility::SafeSendData(sock,(char*)content.c_str(),content.length());
	if(content.length()!=send_len)
	{
		LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
		media_log.log(Message::PRIO_INFORMATION, "执行指令操作:%d,目的id %s, SendData failed! %d", realResponse.m_cmdHead.cmdnum, string(head.dst),send_len);
	}
	return -1;
}

int VDeviceServer::OnRealVideoStreamRequest(long lChannel, int stream_type, StreamSocket& sock)
{
    bool isMainStream;
    if(stream_type == 1)
        isMainStream = true;
    else
        isMainStream = false;

    //先查看通道是否存在
    long key = m_pPlatform->GetChannelPlayId(lChannel-1,isMainStream);

    map<long,DistributionManager*>::iterator pos;
    m_Mutex.lock(); 
    pos = videoSourceRecord.find(key);
    if(pos != videoSourceRecord.end())
    {
        pos->second->AddClient(sock);//创建分发任务；
		m_Mutex.unlock();
		return -2;
    }

    DistributionManager* distribution_manager = new DistributionManager(this, 3,1024*1024);
	distribution_manager->set(key);
    distribution_manager->task_need_extradata_ = true;
    videoSourceRecord.insert(std::make_pair(key,distribution_manager));
    StreamSocket Cltsock=sock;
	distribution_manager->AddClient(sock);//创建分发任务；
	m_Mutex.unlock();

	if (m_pPlatform->PlayVideo(lChannel - 1, distribution_manager, isMainStream))
	{
		return -2;
	}
	else
	{
		Cltsock.close();
		CmdServerModule::Instance().SetDeviceConfigStatus(m_pPlatform->m_Config.deviceID);
		return -1;
	}
}

int VDeviceServer::OnRecordVideoRequest(const CmdHead& head,const unsigned char* pBuffer, StreamSocket& sock)
{
	std::string strMsg;
	strMsg.assign((char*)(pBuffer));

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	CMD10014HistoryReqSerialize query;
	CMD10015HistoryResSerialize historyResponse;
	try
	{
		query.UnSerialize(strMsg);
	}
	catch (Poco::Exception& excep)
	{
		historyResponse.m_cmdHead.success = 1;
		historyResponse.err_description = excep.message();
	}

	if (0 == historyResponse.m_cmdHead.success)
	{
		if (query.channel_ > m_rConfig.MediaChannls.size())
		{
			historyResponse.m_cmdHead.success = 1;
			historyResponse.err_description = Poco::format("指令:%d 请求设备:%s错误的通道号%d", head.cmdnum, string(head.dst), query.channel_);
			media_log.log(Message::PRIO_ERROR, historyResponse.err_description);
		}
		else
		{
			vector<Record> record;
			if (m_pPlatform->QueryRecord(query.channel_ - 1, query.begin_dt_, query.end_dt_, record)==false)
			{
				historyResponse.m_cmdHead.success = 1;
				historyResponse.err_description = Poco::format("指令:%d 请求设备:%s:%d查询录像记录失败", head.cmdnum, string(head.dst), query.channel_);
				media_log.log(Message::PRIO_ERROR, historyResponse.err_description);
			}
			else
			{
				for (int i=0;i<(int)record.size();i++)
				{
					StreamInfor stream;
					stream.m_strBeginDT= Poco::DateTimeFormatter::format(record[i].m_Start, Poco::DateTimeFormat::SORTABLE_FORMAT);
					stream.m_strEndDT = Poco::DateTimeFormatter::format(record[i].m_End, Poco::DateTimeFormat::SORTABLE_FORMAT);
					stream.m_nLength=record[i].m_lSize;
					historyResponse.m_streamInfoVec.push_back(stream);
				}
			}
		}
	}

	string content =  historyResponse.Serialize();

	NetUtility::SafeSendData(sock,(char*)content.c_str(),content.length());

	return 0;
}


int VDeviceServer::OnRecordVideoStreamRequest(const CmdHead& head, char* pBuffer, StreamSocket& sock)//, mega_packet * packet)
{
	CMarkup xml;
	StrUtility utility;
	int lChannel;

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	if (!(xml.SetDoc(pBuffer) && utility.ForeachXml(xml) && utility.GetxmlValue("Information/ChannelNum", lChannel)))
		media_log.log(Message::PRIO_ERROR, "指令:%d 请求设备%s xml格式错误", head.cmdnum, string(head.dst));

	char szChannel[10]={0};
	sprintf(szChannel,"%d",lChannel);

	string key=m_pPlatform->m_Config.deviceID;
	key.append(szChannel);
	
	VDeviceRecordTask * pVPTask = new VDeviceRecordTask(this,key, head, NULL,pBuffer,lChannel-1);
	if(pVPTask==NULL)
	{
		media_log.log(Message::PRIO_ERROR, "指令:%d 请求设备%s 创建任务VDeviceRecordTask失败", head.cmdnum, string(head.dst));
		return 0;
	}

	pVPTask->NewVPlatform(m_rConfig);
	
	
	memset(pVPTask->file_extend,0,sizeof(pVPTask->file_extend)); 
	if(m_pPlatform->m_Config.deviceType == "VPIMOS")
		memcpy(pVPTask->file_extend,"ts",strlen("ts"));
	else
		memcpy(pVPTask->file_extend,"mp4",strlen("mp4"));

	media_log.log(Message::PRIO_DEBUG,"%s 开始进行录像下载,已经被锁住!",key);
	pVPTask->socket = sock;
	pVPTask->Start();
	return 0;
}

bool VDeviceServer::NotifyAlarm(long lChannel, long lType, LPCTSTR szAlarmMessage)
{
	VDeviceConfig vpconfig = CmdServerModule::Instance().FindByDeviceID(m_pPlatform->m_Config.deviceID);
	return AlarmServer::Instance().NotifyAlarm(vpconfig, lChannel, lType, szAlarmMessage);
}

 void VDeviceServer::NotifyMessage(_RESULT lCode,LPCTSTR szMessage)
 {
	// //g_log<<szMessage<<"\n";
	// //g_log<<"NotifyMessagetest\n";
 }

 void VDeviceServer::OnGetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel)
 {
	 std::string strMsg;
	 strMsg.assign((char*)(pBuffer));
	 CMegaeyesVideMainPara para;
	 nChannel -= 1;
	 try
	 {
		 para.UnSerialize(strMsg);
	 }
	 catch (...)
	 {
	 	//g_log<<"UnSerialize CMegaeyesVideMainPara failed\n";
		return;
	 }
	 CMegaeyesGetVideoPara response(para.m_nPara);
	 bool bSteady=false;
	 
	 bool bMainStream = true;

	 std::vector<Rect> rect;
	 BOOL bOsding;

	 if (para.m_strStreamType == "SubStream1")
	 {
		 bMainStream =false;
	 }
	 switch(para.m_nPara)
	 {
	 case CMegaeyesVideMainPara::Frame:
		 if (m_pPlatform->GetFrameRate(nChannel,response.m_lFrameRate,response.m_lKeyFrameInterval,bMainStream)==false)
		 {
			return;
		 }
		 break;
	 case CMegaeyesVideMainPara::Level:
		 if (m_pPlatform->GetPictureQuality(nChannel,response.m_lLevel,bMainStream)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::MaxBPS:
		 if (m_pPlatform->GetMaxBPS(nChannel,response.m_lMaxBps,bSteady,bMainStream)==false)
		 {
			 return;
		 }
		 response.m_strStreamOption=(bSteady==true?"steady":"wavy");
		 break;
	 case CMegaeyesVideMainPara::PicFormat:
		 if (m_pPlatform->GetPictureFormat(nChannel,response.m_lPicFormat,response.m_strSupportFormat,bMainStream)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::Shard: //获取遮挡区域 mlcai 2014-05-08
		 if (m_pPlatform->GetShadeRect(nChannel,rect,bOsding,response.m_nLogingNum)==false)
		 {
			 return;
		 }
		 response.m_bOSDing=(bOsding==TRUE?1:0);
		 for (int i=0;i<(int)rect.size();i++)
		 {
			 CVidedRect videoRect;
			 videoRect.m_nLeft=rect[i].Left;
			 videoRect.m_nTop=rect[i].Top;
			 videoRect.m_nWidth=rect[i].Width();
			 videoRect.m_nHeight=rect[i].Height();
			 response.m_ShardRectVec.push_back(videoRect);
		 }
		 break;
	 case CMegaeyesVideMainPara::Log: //获取OSD功能 mlcai 2014-05-08
		 if (m_pPlatform->GetOSD(nChannel,bOsding,response.m_nOSDPlace,response.m_nWordPlace,response.m_strWord)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::VideoPara:
		 if (m_pPlatform->GetVideoPara(nChannel,(long&)response.m_nBrightness,(long&)response.m_nContrast,(long&)response.m_nSatuation,(long&)response.m_nHue)==false)
		 {
			 return;
		 }
		 break;
	 default:
		 return;
	 }
	
	 unsigned char buffer[1024] ={0};
	 strMsg=response.Serialize();
	 CMegaeyesResponseCmdHead& cmdHead=response.m_RespinseCmdHead;
	 cmdHead.m_nCmdSeq=head.cmdseq+1;

	 memcpy(buffer,&cmdHead,CMegaeyesResponseCmdHead::Length);
	 memcpy(buffer+CMegaeyesResponseCmdHead::Length,strMsg.c_str(),strMsg.length());

	 NetUtility::SafeSendData(op.hand->_socket,(char *)buffer,CMegaeyesResponseCmdHead::Length + strMsg.length());
 }

 void VDeviceServer::OnSetVideoPara(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel)
 {
	 std::string strMsg;
	 strMsg.assign((char*)(pBuffer));
	 CMegaeyesGetVideoPara para;
	 nChannel -= 1;
	 try
	 {
		 para.UnSerialize(strMsg);
	 }
	 catch (...)
	 {
		 return;
	 }
	  std::vector<Rect> rect;
	  bool bMainStream=true;
	  if (para.m_strStreamType == "SubStream1")
	  {
		   bMainStream=false;
	  }
	 switch(para.m_nPara)
	 {
	 case CMegaeyesVideMainPara::Frame:
		 if (m_pPlatform->SetFrameRate(nChannel,para.m_lFrameRate,para.m_lKeyFrameInterval,bMainStream)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::Level:
		 if (m_pPlatform->SetPictureQuality(nChannel,para.m_lLevel,bMainStream)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::MaxBPS:
		 if (m_pPlatform->SetMaxBPS(nChannel,para.m_lMaxBps,para.m_strStreamOption=="steady",bMainStream)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::PicFormat:
		 para.m_strSupportFormat="0123";
		 if (m_pPlatform->SetPictureFormat(nChannel,para.m_lPicFormat,para.m_strSupportFormat,bMainStream)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::Shard:
		 for (int i=0;i<(int)para.m_ShardRectVec.size();i++)
		 {
			 Rect videoRect;
			 videoRect.Left=para.m_ShardRectVec[i].m_nLeft;
			 videoRect.Top=para.m_ShardRectVec[i].m_nTop;
			 videoRect.Right=para.m_ShardRectVec[i].m_nLeft+para.m_ShardRectVec[i].m_nWidth;
			 videoRect.Bottom=videoRect.Top+para.m_ShardRectVec[i].m_nHeight;
			rect.push_back(videoRect);
		 }
		 if (m_pPlatform->SetShadeRect(nChannel,rect,para.m_bLoging,para.m_nLogingNum)==false)
		 {
			 return;
		 }
	
		 break;
	 case CMegaeyesVideMainPara::Log:
		 if (m_pPlatform->SetOSD(nChannel,para.m_bOSDing,(WORD&)para.m_nOSDPlace,(WORD&)para.m_nWordPlace,para.m_strWord)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::VideoPara:
		 if (m_pPlatform->SetVideoPara(nChannel,(long&)para.m_nBrightness,(long&)para.m_nContrast,(long&)para.m_nSatuation,(long&)para.m_nHue)==false)
		 {
			 return;
		 }
		 break;
	 case CMegaeyesVideMainPara::Detect:
		 for (int i=0;i<(int)para.m_DetectRectVec.size();i++)
		 {
			 Rect videoRect;
			 videoRect.Left=para.m_DetectRectVec[i].m_nLeft;
			 videoRect.Top=para.m_DetectRectVec[i].m_nTop;
			 videoRect.Right=para.m_DetectRectVec[i].m_nLeft+para.m_DetectRectVec[i].m_nWidth;
			 videoRect.Bottom=videoRect.Top+para.m_DetectRectVec[i].m_nHeight;
			 rect.push_back(videoRect);
		 }
		 if (m_pPlatform->SetDetectRect(nChannel,rect,para.m_bDetectting,(long&)para.m_nDectectGrade,(long&)para.m_nDetectNum)==false)
		 {
			 return;
		 }
		 break;
	 default:
		 return;
	 }
	 //LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
	 //transLog.log(Message::PRIO_ERROR, "指令 5032  已经返回给接入了");
	 CMegaeyesSetVideoPara response;
	 SendResponse(head,response,op); 
 }

 void VDeviceServer::OnSetAlarmOut(const MegaeyesRequestHead& head,CMegaeyesAlarmOut& alarmOut,int& nLength,int& nChannel,Operator &op)
 {
	if (m_pPlatform->SetAlarmState(nChannel-1,2,alarmOut.m_nOp)==false)
	{
		return;
	}
	CMegaeyesSetAlarmOutResponse response;
	SendResponse(head,response,op);
 }

 void VDeviceServer::OnGetAlarmOut(const MegaeyesRequestHead& head,CMegaeyesAlarmOut& alarmOut,int& nLength,int& nChannel,Operator &op)
 {
	 if (m_pPlatform->GetAlarmState(nChannel,2,alarmOut.m_nOp)==false)
	 {
		 return;
	 }
	 SendResponse(head,alarmOut,op);
 }

 void VDeviceServer::OnNetParam(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op)
 {
	 CMegaeyesNetParamType netParam;
	 std::string str;
	 str.assign((char*)pBuffer);
	 try
	 {
		 netParam.UnSerialize(str);
	 }
	 catch (...)
	 {
	 	//g_log<<"5046 failed\n";
		return;
	 }
	 
	switch(netParam.m_nType)
	{
	case CMegaeyesNetParamType::GetVersion:
		{
			CMegaeyesGetVersion version;
			m_pPlatform->GetVersion(version.m_strVersion, version.m_strBuildDateTime);
			
			unsigned char buffer[1024] ={0};
			std::string strMsg=version.Serialize();
			CMegaeyesResponseCmdHead& cmdHead=version.m_RespinseCmdHead;
			cmdHead.m_nCmdSeq=head.cmdseq+1;

			memcpy(buffer,&cmdHead,CMegaeyesResponseCmdHead::Length);
			memcpy(buffer+CMegaeyesResponseCmdHead::Length,strMsg.c_str(),strMsg.length());

			NetUtility::SafeSendData(op.hand->_socket,(char *)buffer,CMegaeyesResponseCmdHead::Length + strMsg.length());
		}
		break;
	case CMegaeyesNetParamType::GetNetPara:
		{
			CMegaeyesNetParam net;
			m_pPlatform->GetNetParam(net.m_strIp,net.m_strNetMask,net.m_strGetWay);
			
			unsigned char buffer[1024] ={0};
			std::string strMsg=net.Serialize();
			CMegaeyesResponseCmdHead& cmdHead=net.m_RespinseCmdHead;
			cmdHead.m_nCmdSeq=head.cmdseq+1;

			memcpy(buffer,&cmdHead,CMegaeyesResponseCmdHead::Length);
			memcpy(buffer+CMegaeyesResponseCmdHead::Length,strMsg.c_str(),strMsg.length());

			NetUtility::SafeSendData(op.hand->_socket,(char *)buffer,CMegaeyesResponseCmdHead::Length + strMsg.length());
		}
		break;
	default:
		break;

	}
 }

 void VDeviceServer::OnSetAlarmPlan(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel)
 {
	 CMegaeyesSetAlarmOutResponse response;
	 response.m_RespinseCmdHead.m_nCmdNum=head.cmdnum+1;
	 unsigned char buffer[1024];
	 CMegaeyesResponseCmdHead& cmdHead=response.m_RespinseCmdHead;
	 cmdHead.m_nCmdSeq=head.cmdseq+1;

	 memcpy(buffer,&cmdHead,CMegaeyesResponseCmdHead::Length);
	 //发送指令头
	//if ((head._hand)->send(buffer,CMegaeyesResponseCmdHead::Length)==false)
	//	 return;
 }

 void VDeviceServer::OnGetPtzStatus(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel)
 {
	std::string preset;
	 CMegaeyesPtzProtocol response;
	 if (m_pPlatform->GetPresetList(nChannel - 1, preset))
		 response.m_strPreset = preset;

	 if(preset.empty())
	 {
 		CString  base_path,path;
 		GetModuleFileNameA( NULL,base_path.GetBuffer(MAX_PATH), MAX_PATH );
 		base_path.ReleaseBuffer();
 		base_path = base_path.Left( base_path.ReverseFind('\\'));
 		path.Format("%s\\Data\\%s_Presets.xml", base_path, m_pPlatform->m_Config.deviceID.c_str());
 
	 	 CMarkup xml;
 		 if (xml.Load(path))
 			response.m_strPreset = xml.GetDoc();
	 }

 	 SendResponse(head,response,op);
 }

 bool VDeviceServer::FastPTZControl(long lChannel,std::string strParam)
 {
	 std::string str = strParam;
	 int x = 0, y = 0, z = 0;
	 int i = str.find(',');
	 if (i == -1)
	 {
		 return false;
	 }
	 std::string substr = str.substr(0,i);
	 x = atoi(substr.c_str());
	 substr = str.substr(i+1,str.length());
	int j = substr.find(',');
	
	if (j == -1)
	{
		return false;
	}
	str = substr.substr(0,j);
	y = atoi(str.c_str());
	str = substr.substr(j+1,substr.find_last_of(',')-j-1);
	z = atoi(str.c_str());
	 if (x > 50)
	 {
		 //right
		 m_pPlatform->PTZControl(lChannel, 4, 255);
	 }
	 else
	 {
		 //left
		 m_pPlatform->PTZControl(lChannel, 3, 255);
	 }

	 Sleep( abs(x - 50) * 30 );

	 m_pPlatform->PTZControl(lChannel, 0, 0);

	 if (y > 50)
	 {
		 //bottom
		 m_pPlatform->PTZControl(lChannel, 2, 255);
	 }
	 else 
	 {
		 //top
		 m_pPlatform->PTZControl(lChannel, 1, 255);
	 }

	 Sleep( abs(y - 50) * 30 );

	 m_pPlatform->PTZControl(lChannel, 0, 0);

	 if (z == 0)
	 {
		 return true;
	 }

	 m_pPlatform->PTZControl(lChannel, z > 0 ? 7 : 8, 128);

	 Sleep(500);

	 m_pPlatform->PTZControl(lChannel, 0, 0);

	 return true;
 }

 bool VDeviceServer::FindToken(const string& token)
 {
	 FastMutex::ScopedLock lock(token_mtx_);
	 return tokens_.find(token) != tokens_.end();
 }

 void VDeviceServer::OnPtzCtrl(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int& nLength,Operator &op,int& nChannel)
 {

	std::string strMsg;
	strMsg.assign((char*)pBuffer);
	nChannel -= 1;
	CMegaeyesPtzCtrlProcotol ptzCtrl;
	try
	{
		if (m_pPlatform == NULL)
		{
			return ;
		}
		ptzCtrl.UnSerialize(strMsg);
	}
	catch (...)
	{
		return;
	}

	long lCmd = 0;

	switch(ptzCtrl.m_nOpera)
	{
	case CMegaeyesPtzCtrlProcotol::PTZ_FAST_GO:
		if (m_pPlatform->PTZControl(nChannel,CMegaeyesPtzCtrlProcotol::PTZ_FAST_GO,0,(void*)ptzCtrl.m_strRect.c_str())==false)
		{
			//不支持三维定位；
			 FastPTZControl(nChannel,ptzCtrl.m_strRect);
			 return;
		}
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_CTR:
		m_pPlatform->PTZControl(nChannel,ptzCtrl.m_nPtzDirect,ptzCtrl.m_nPtzSpeed);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_ZOOM:
		lCmd = ((ptzCtrl.m_nSwitch==1)?7:8);
		m_pPlatform->PTZControl(nChannel,lCmd,0,NULL);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_FOCUS:
		lCmd = ((ptzCtrl.m_nSwitch==0)?9:10);
		m_pPlatform->PTZControl(nChannel,lCmd,0,NULL);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_IRIS:
		lCmd = ((ptzCtrl.m_nSwitch==0)?5:6);
		m_pPlatform->PTZControl(nChannel,lCmd,0,NULL);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_PRESET_ADD:
		lCmd = 1001;
		m_pPlatform->PTZControl(nChannel,lCmd,ptzCtrl.m_nPresetNum,(void*)ptzCtrl.m_strPresetName.c_str());
		PTZPresetControl(CMegaeyesPtzCtrlProcotol::PTZ_PRESET_ADD,ptzCtrl.m_nPresetNum,(void*)ptzCtrl.m_strPresetName.c_str());
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_PRESET_DLL:
		lCmd = 1002;
		m_pPlatform->PTZControl(nChannel,lCmd,ptzCtrl.m_nPresetNum,NULL);
		PTZPresetControl(CMegaeyesPtzCtrlProcotol::PTZ_PRESET_DLL,ptzCtrl.m_nPresetNum,NULL);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_PRESET_DLL_ALL:
		lCmd = 1002;
		m_pPlatform->PTZControl(nChannel,lCmd,-1,NULL);
		PTZPresetControl(CMegaeyesPtzCtrlProcotol::PTZ_PRESET_DLL_ALL,ptzCtrl.m_nPresetNum,NULL);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_PRESET_EXCUTE:
		lCmd = 1003;
		m_pPlatform->PTZControl(nChannel,lCmd,ptzCtrl.m_nPresetNum,(void*)&ptzCtrl.m_nPrestSpeed);
		break;
	case CMegaeyesPtzCtrlProcotol::PTZ_IRIS_STOP:
	case CMegaeyesPtzCtrlProcotol::PTZ_STOP:
	case CMegaeyesPtzCtrlProcotol::PTZ_FOCUS_STOP:
	case CMegaeyesPtzCtrlProcotol::PTZ_ZOOM_STOP:
		m_pPlatform->PTZControl(nChannel,0,NULL);
		break;
	default:
		break;
	}

	CMegaeyesPtzCtrlResponse response;
	SendResponse(head,response,op);
 }

 void VDeviceServer::PTZPresetControl(const int& nCmd,const int& nPreset,void* pParam)
 {
	 CString  base_path,path;
	 GetModuleFileNameA( NULL,base_path.GetBuffer(MAX_PATH), MAX_PATH );
	 base_path.ReleaseBuffer();
	 base_path = base_path.Left( base_path.ReverseFind('\\'));

	 path.Format("%s\\Data\\%s_Presets.xml", base_path, m_pPlatform->m_Config.deviceID.c_str());
	 CMarkup xml;

	 if (!xml.Load(path))
	 {
		 xml.SetDoc("<PresetSetting Count=\"128\"></PresetSetting>");
	 }
	 else if (nCmd == CMegaeyesPtzCtrlProcotol::PTZ_PRESET_DLL_ALL)
	 {
		 xml.SetDoc("<PresetSetting Count=\"128\"></PresetSetting>");
	 }
	 xml.FindElem("PresetSetting");
	 xml.IntoElem();
	while(xml.FindElem("Point"))
	{
		if (nCmd ==CMegaeyesPtzCtrlProcotol::PTZ_PRESET_DLL_ALL)
			xml.RemoveElem();
		else
		{
			int nNum = atoi(xml.GetAttrib("Num").c_str());
			if (nPreset == nNum)
			{
				xml.RemoveElem();
				break;
			}
		}
	
	}
	if (nCmd == CMegaeyesPtzCtrlProcotol::PTZ_PRESET_ADD )
	{
		xml.AddElem("Point");
		xml.AddAttrib("Num",nPreset);
		xml.AddAttrib("Name",(char*)pParam);
		xml.OutOfElem();
	}
	
	MakeDirEx(path);
	xml.Save(path);
 }

string VDeviceServer::Notify(int type, char* buffer, int length)
{
	switch (type)
	{
	case DESTROY:
		{
			long key = (long)buffer;
			m_Mutex.lock();
			map<long,DistributionManager*>::iterator it = videoSourceRecord.find(key);
			if (it != videoSourceRecord.end())
			{
				DistributionManager* dm = it->second;
				if (0 == dm->task_record_.size())
				{
					videoSourceRecord.erase(it);
					m_pPlatform->StopVideo(key/1000,!(key%1000));
					cout << "Destroy DistributionManager " << key << endl;
					delete dm;
				}
			}
			m_Mutex.unlock();
		}
		break;
	}

	string str;
	return str;
}

//add by sunminping 2014-6-19
bool VDeviceServer::IsExistsIp(const string& ip)
{
	return pVPManager->IsExistsIp(ip);
}
//add end