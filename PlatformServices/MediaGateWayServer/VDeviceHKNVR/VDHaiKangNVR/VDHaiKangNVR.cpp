#include "stdafx.h"
#include "VDHaiKangNVR.h"
#include "Markup.h"

bool g_bInitSDK = false;
hash_map<string, VDHaiKangNVR*> g_VPHaiKangNVR;
map<long,DWORD> g_PTZCommand_map;
map<DWORD,string> g_LastError_map;
Poco::FastMutex g_sdkLock;

//#define USE_VOICE_TALK

static void CALLBACK MessageCallBack(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, 
									 DWORD dwBufLen, void* pUser)
{
	try
	{
		string sIP = pAlarmer->sDeviceIP;
		VDHaiKangNVR * p;
		{
			Poco::FastMutex::ScopedLock locker(g_sdkLock);
			hash_map<string, VDHaiKangNVR*>::iterator pos;
			pos = g_VPHaiKangNVR.find(sIP);

			if ( sIP.empty() || pos == g_VPHaiKangNVR.end() )
			{
				return;
			}

			p = pos->second;
		}

		

		p->MessageNotify(lCommand,pAlarmInfo,dwBufLen);
	}
	catch(...)
	{

	}
}

bool VDHaiKangNVR::InitPTZCommand()
{
	DWORD dwStop = 1<<16;

	g_PTZCommand_map[1] = TILT_UP;
	g_PTZCommand_map[2] = TILT_DOWN;
	g_PTZCommand_map[3] = PAN_LEFT;
	g_PTZCommand_map[4] = PAN_RIGHT;

	g_PTZCommand_map[5] = IRIS_OPEN;
	g_PTZCommand_map[6] = IRIS_CLOSE;
	g_PTZCommand_map[7] = ZOOM_IN;
	g_PTZCommand_map[8] = ZOOM_OUT;
	g_PTZCommand_map[9] = FOCUS_NEAR;
	g_PTZCommand_map[10] = FOCUS_FAR;

	g_PTZCommand_map[11] = PAN_AUTO;
	g_PTZCommand_map[12] = PAN_AUTO|dwStop;
	g_PTZCommand_map[13] = LIGHT_PWRON;
	g_PTZCommand_map[14] = LIGHT_PWRON|dwStop;
	g_PTZCommand_map[15] = WIPER_PWRON;
	g_PTZCommand_map[16] = WIPER_PWRON|dwStop;
	
	return true;
}

bool VDHaiKangNVR::InitPicCfg(long lChannel)
{
	DWORD dwReturned = 0;
	if(!NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_PICCFG_V30, lChannel+1, &video_Para_records[lChannel].m_dvr_pic_cfg, 
		sizeof(NET_DVR_PICCFG_V30), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_TRACE,"%s  视频通道%ld 获取图像参数失败!",m_Config.serverIP,lChannel+1);
		return false;
	}

	if(!NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_PICCFG_EX, lChannel+1, &video_Para_records[lChannel].m_dvr_pic_cfg_ex, 
		sizeof(NET_DVR_PICCFG_EX), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_TRACE,"%s  视频通道%ld 获取扩展图像参数失败!", m_Config.serverIP,lChannel+1);
		return false;
	}

	m_Config.vp_log->log(Message::PRIO_TRACE,"NET_DVR_GetDVRConfig %ld ok", lChannel);
	
	video_Para_records[lChannel].m_bInitPicCfg = true;

	m_Config.vp_log->log(Message::PRIO_NOTICE,"InitPicCfg %ld OK", lChannel);

	return true;
}

bool VDHaiKangNVR::InitCompressCfg(long lChannel)
{
	DWORD dwReturned = 0;
	if (!NET_DVR_GetDVRConfig(m_lUserID,NET_DVR_GET_COMPRESSCFG_V30,lChannel, &video_Para_records[lChannel].m_dvr_compress_cfg,
		sizeof(NET_DVR_COMPRESSIONCFG_V30),&dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_NOTICE,"%s  视频通道%ld 获取视频参数失败!", m_Config.serverIP,lChannel+1);
		return false;
	}
	video_Para_records[lChannel].m_bInitCompressCfg = true;
	return true;
}

//全局初始化、注销
void sdk_global_destroy()
{
	NET_DVR_Cleanup();
	g_bInitSDK = false;
}

VDHaiKangNVR::VDHaiKangNVR()
{
    m_sVersion = VPHaiKangNVR_Version;
	m_dwLastCommand=0;
    m_lUserID = -1;
	sAlarmInfo alarmInfo;
	alarmInfo.lastTime=0;
	for(int i=0;i<64;++i)
	{
		alarmInfo.channelno=i;
		alarmInfo.alarmType=alarm_camera;
		alarmInfoMap.insert(make_pair(alarmInfo.alarmType*1000+i,alarmInfo));
		alarmInfo.alarmType=alarm_in;
		alarmInfoMap.insert(make_pair(alarmInfo.alarmType*1000+i,alarmInfo));
	}
}

VDHaiKangNVR::~VDHaiKangNVR()
{
	Destroy();
}

static void CALLBACK fVoiceDataCallBack(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, 
										BYTE byAudioFlag, DWORD dwUser)
{
	//HxTrace("fVoiceDataCallBack handle:%d flag:%d size:%d",lVoiceComHandle,byAudioFlag,dwBufSize);
	
#ifdef USE_VOICE_TALK
	VDHaiKangNVR * p = (VDHaiKangNVR*)dwUser;
	if ( byAudioFlag == 1 )
	{
		if ( p->m_PlayChannel.empty() )
		{
			return;
		}

		p->m_pCallBack->NotifyStreamData(*(p->m_PlayChannel.begin()), video_stream, unknown_frame, (PBYTE)pRecvDataBuffer,dwBufSize);
	}
#endif


}

BOOL VDHaiKangNVR::MessageNotify(LONG lCommand, char *pBuf,DWORD dwBufLen)
{
	try
	{
		if ( lCommand != COMM_ALARM_V30 && lCommand != COMM_ALARM )
		{
			return FALSE;
		}

		long channel=-1;
		DWORD dwAlarmType = -1;

		//dwAlarmType = 0-信号量报警,1-硬盘满,2-信号丢失,3－移动侦测,4－硬盘未格式化,5-读写硬盘出错,6-遮挡报警,7-制式不匹配, 8-非法访问
		if ( lCommand == COMM_ALARM_V30 )
		{
			NET_DVR_ALARMINFO_V30* info = (NET_DVR_ALARMINFO_V30*)pBuf;
			dwAlarmType = info->dwAlarmType;
			if(dwAlarmType==3)
			{
				long alarm_type;
				string strAlarmType="";
				{
					Poco::Mutex::ScopedLock locker(alarm_mutex_);
					std::hash_map<DWORD, long>::iterator it = _alarm_type_map.find(dwAlarmType);
					alarm_type = it->second;
					strAlarmType=(alarm_type==alarm_in)?"alarm_in":"alarm_camera";
				}

				for(int i=0;i<16;i++)
				{
					if (info->byChannel[i]==1)
					{
						channel=i;

						if(m_pCallBack->IsExistsIp(m_Config.serverIP))
							m_Config.vp_log->log(Message::PRIO_DEBUG,"HaiKangDVR %s 报警:%ld，报警类型:%s ",m_Config.serverIP,channel+1,strAlarmType);

						map<int,sAlarmInfo>::iterator it=alarmInfoMap.find(alarm_type*1000+channel);
						if(it!=alarmInfoMap.end())
						{
							time_t now,lastTime;
							time(&now);
							lastTime=it->second.lastTime;
							it->second.lastTime=now;
							if(now-lastTime>60)
							{
								if(!m_pCallBack->NotifyAlarm(channel,alarm_type))
									it->second.lastTime=lastTime;
							}
							return TRUE;
						}
					}
				}
			}
			else if(dwAlarmType==0) 
			{

				channel=info->dwAlarmInputNumber;

				long alarm_type;
				string strAlarmType="";
				{
					Poco::Mutex::ScopedLock locker(alarm_mutex_);
					std::hash_map<DWORD, long>::iterator it = _alarm_type_map.find(dwAlarmType);
					alarm_type = it->second;
					strAlarmType=(alarm_type==alarm_in)?"alarm_in":"alarm_camera";
				}

				if(m_pCallBack->IsExistsIp(m_Config.serverIP))
					m_Config.vp_log->log(Message::PRIO_DEBUG,"HaiKangDVR %s 报警:%ld，报警类型:%s ",m_Config.serverIP,channel+1,strAlarmType);

				map<int,sAlarmInfo>::iterator it=alarmInfoMap.find(alarm_type*1000+channel);
				if(it!=alarmInfoMap.end())
				{
					time_t now,lastTime;
					time(&now);
					lastTime=it->second.lastTime;
					it->second.lastTime=now;
					if(now-lastTime>60)
					{
						if(!m_pCallBack->NotifyAlarm(channel,alarm_type))
							it->second.lastTime=lastTime;
					}
					return TRUE;
				}

			}
		}
		else if( lCommand == COMM_ALARM )
		{
			NET_DVR_ALARMINFO* info = (NET_DVR_ALARMINFO*)pBuf;
			dwAlarmType = info->dwAlarmType;
			if(dwAlarmType==3)
			{
				long alarm_type;
				string strAlarmType="";
				{
					Poco::Mutex::ScopedLock locker(alarm_mutex_);
					std::hash_map<DWORD, long>::iterator it = _alarm_type_map.find(dwAlarmType);
					alarm_type = it->second;
					strAlarmType=(alarm_type==alarm_in)?"alarm_in":"alarm_camera";
				}

				for(int i=0; i<16; i++)
				{
					if (info->dwChannel[i] == 1)
					{
						channel=i;

						if(m_pCallBack->IsExistsIp(m_Config.serverIP))
							m_Config.vp_log->log(Message::PRIO_DEBUG,"HaiKangDVR %s 报警:%ld，报警类型:%s ",m_Config.serverIP,channel+1,strAlarmType);

						map<int,sAlarmInfo>::iterator it=alarmInfoMap.find(alarm_type*1000+channel);
						if(it!=alarmInfoMap.end())
						{
							time_t now,lastTime;
							time(&now);
							lastTime=it->second.lastTime;
							it->second.lastTime=now;
							if(now-lastTime>60)
							{
								if(!m_pCallBack->NotifyAlarm(channel,alarm_type))
									it->second.lastTime=lastTime;
							}
							return TRUE;
						}

					}
				}
			}
			else if(0==dwAlarmType)
			{
				channel=info->dwAlarmInputNumber;

				long alarm_type;
				string strAlarmType="";
				{
					Poco::Mutex::ScopedLock locker(alarm_mutex_);
					std::hash_map<DWORD, long>::iterator it = _alarm_type_map.find(dwAlarmType);
					alarm_type = it->second;
					strAlarmType=(alarm_type==alarm_in)?"alarm_in":"alarm_camera";
				}

				if(m_pCallBack->IsExistsIp(m_Config.serverIP))
					m_Config.vp_log->log(Message::PRIO_DEBUG,"HaiKangDVR %s 报警:%ld，报警类型:%s ",m_Config.serverIP,channel+1,strAlarmType);

				map<int,sAlarmInfo>::iterator it=alarmInfoMap.find(alarm_type*1000+channel);
				if(it!=alarmInfoMap.end())
				{
					time_t now,lastTime;
					time(&now);
					lastTime=it->second.lastTime;
					it->second.lastTime=now;
					if(now-lastTime>60)
					{
						if(!m_pCallBack->NotifyAlarm(channel,alarm_type))
							it->second.lastTime=lastTime;
					}
					return TRUE;
				}
			}
		}
	}
	catch(...)
	{

	}

	return FALSE;
	
}

//实时视频数据回调
void CALLBACK RealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
	try
	{
        VPlatformChannel * pChannel = (VPlatformChannel*)dwUser;
        VDHaiKangNVR* pPlatform = (VDHaiKangNVR*)pChannel->vplatform_;

        long lType;

        if(pChannel->is_mainstream_)
        {
            lType = video_stream;
        }
        else
        {
            lType = video_sub_stream;
        }


		if (dwDataType == NET_DVR_SYSHEAD && pChannel->is_send_videoheader_==false)
		{
            if (dwBufSize > 40)
            {
				//"Get HK Video Header  error!!
            }

			pChannel->is_send_videoheader_ = true;

			CMarkup head_xml;
			head_xml.AddElem("Information");
			head_xml.AddElem("Protocol", (char*)pBuffer);
			const string& head_str = head_xml.GetDoc();

			pPlatform->m_pCallBack->NotifyStreamData(pChannel->channelno_,lType,head_frame,pBuffer,dwBufSize, pChannel->distribution_manager_);

            Sleep(10);

            return;
		}
		
		//通知上层接收视频数据
		pPlatform->m_pCallBack->NotifyStreamData(pChannel->channelno_,lType,unknown_frame,pBuffer,dwBufSize, pChannel->distribution_manager_);

	}
	catch (...)
	{

	}
	
}

int VDHaiKangNVR::Login(VDeviceConfig& dev_config)
{
	NET_DVR_DEVICEINFO_V30 m_dvr_info;

	m_lUserID = NET_DVR_Login_V30((char*)m_Config.serverIP.c_str(),(WORD)m_Config.port,
		(char*)m_Config.user.c_str(),(char*)m_Config.passwd.c_str(),&m_dvr_info);
	
	NET_DVR_USER_V30 usr;

	if(m_lUserID < 0)
	{
		//登录失败
		DWORD dwError = NET_DVR_GetLastError();  
		m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_Login_V30失败!", m_Config.deviceID);
		if(dwError == NET_DVR_PASSWORD_ERROR)
		{
			return -2; //用户名密码错误!
		}
		else
		{
			return -1; //由于网络原因或DVR忙, 注册失败!
		}
	}

	NET_DVR_PICCFG_V30 pic_cfg;
	DWORD dwReturned = 0;
	for (int i = 0; i < (int)m_dvr_info.byChanNum; ++i)
	{
		memset(&pic_cfg, 0, sizeof(pic_cfg));
		if (!NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_PICCFG_V30, i + 1, &pic_cfg, sizeof(NET_DVR_PICCFG_V30), &dwReturned))
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangDVR device id %s NET_DVR_GetDVRConfig获取通道%d参数失败!", m_Config.deviceID, i + 1);
			dev_config.MediaChannls.insert(map<int, string>::value_type(i + 1, ""));
		}
		else
			dev_config.MediaChannls.insert(map<int, string>::value_type(i + 1, (char*)pic_cfg.sChanName));
	}

	RegisterObject();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//接口

//初始化
int VDHaiKangNVR::Init(VDeviceConfig& dev_config) 
{
	g_sdkLock.lock();
	if (!g_bInitSDK)
	{
		if(!NET_DVR_Init())
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR NET_DVR_Init失败!");
            g_sdkLock.unlock();
			return false;
		}
		g_bInitSDK=true;

		//初始化云台命令
		InitPTZCommand();
	}

	//初始化报警类型映射 0-信号量报警,1-硬盘满,2-信号丢失,3－移动侦测,4－硬盘未格式化,5-读写硬盘出错,6-遮挡报警,7-制式不匹配, 8-非法访问
	_alarm_type_map.insert(std::hash_map<DWORD, long>::value_type(0, alarm_in));
	_alarm_type_map.insert(std::hash_map<DWORD, long>::value_type(1, alarm_disk_full));
	_alarm_type_map.insert(std::hash_map<DWORD, long>::value_type(2, alarm_video_lost));
	_alarm_type_map.insert(std::hash_map<DWORD, long>::value_type(3, alarm_camera));
	_alarm_type_map.insert(std::hash_map<DWORD, long>::value_type(4, alarm_disk_not_format));
	_alarm_type_map.insert(std::hash_map<DWORD, long>::value_type(5, alarm_disk_fault));

	g_sdkLock.unlock();


    return Login(dev_config);
}

void VDHaiKangNVR::RegisterObject()
{
	Poco::ScopedLock<Poco::FastMutex> lock(g_sdkLock);
	g_VPHaiKangNVR[ m_Config.serverIP ] = this;
}

void VDHaiKangNVR::UnRegisterObject()
{
	Poco::ScopedLock<Poco::FastMutex> lock(g_sdkLock);
	g_VPHaiKangNVR.erase(m_Config.serverIP );
}

bool VDHaiKangNVR::SetupAlarmChannel()
{
	//SDK中收到DVR发过来的信息需要上传的通过回调函数实现，比如报警信息，日志信息等。
	NET_DVR_SetDVRMessageCallBack_V30(MessageCallBack, NULL);

	//建立报警上传通道
	long m_lAlarmChannel = NET_DVR_SetupAlarmChan_V30(m_lUserID);
	if ( m_lAlarmChannel < 0 )
	{
		DWORD dError = NET_DVR_GetLastError();
		return false;
	}

	return true;
}

void VDHaiKangNVR::CloseAlarmChannel()
{
	//if ( m_lAlarmChannel != -1 )
	//{
	//	NET_DVR_CloseAlarmChan_V30(m_lAlarmChannel);
	//	m_lAlarmChannel = -1;
	//}
}

bool VDHaiKangNVR::SetAlarmState(long lChannel, long lType, long lState) 
{
	//HxTrace("VPHaiKang::SetAlarmState Channe:%d Type:%d State:%d",lChannel,lType,lState);
	//lChannel += 33; 不用+33 mlcai 2014-05-07
	bool bSuccess = false;
	switch (lType)
	{
	case alarm_camera:
		{
			// 			if (!m_VideoChannels[lChannel].m_bInitPicCfg)
			// 			{
			// 				if (!InitPicCfg(lChannel))
			// 				{
			// 					break;
			// 				}
			// 			}
			// 			//bSuccess = true;
			// 			m_VideoChannels[lChannel].m_dvr_pic_cfg.struMotion.byEnableHandleMotion = (BYTE)lState;
			// 			bSuccess = SetPicturInfo(lChannel);
		}
		break;
	case alarm_in:
		{
			// 			if (!m_AlarmInChannels[lChannel].m_bInit)
			// 			{
			// 				if (!InitAlarmIn(lChannel))
			// 				{
			// 					break;
			// 				}
			// 			}
			// 			m_AlarmInChannels[lChannel].m_dvr_AlarmIn_cfg.byAlarmInHandle = lState ;
			// 			bSuccess = SetAlarmInCfg(lChannel);	
		}
		break;
	case alarm_out:
		{
			if (NET_DVR_SetAlarmOut(m_lUserID,lChannel,lState))
				bSuccess = true;
			else
				m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_SetAlarmOut失败!", m_Config.deviceID);
		}
		break;
	default:
		break;
	}

	return bSuccess; 
}

bool VDHaiKangNVR::GetAlarmState(long lChannel, long lType, long& lState) 
{
	//lChannel += 33; 不用+33 mlcai 2014-05-07
	bool bSuccess = false;

	switch (lType)
	{
	case alarm_camera:
		{
			// 			if (!m_VideoChannels[lChannel].m_bInitPicCfg)
			// 			{
			// 				if (!InitPicCfg(lChannel))
			// 				{
			// 					break;
			// 				}
			// 			}
			// 			lState = m_VideoChannels[lChannel].m_dvr_pic_cfg.struMotion.byEnableHandleMotion;
			// 			bSuccess = true;
		}
		break;
	case alarm_in:
		{
			// 			if (!m_AlarmInChannels[lChannel].m_bInit)
			// 			{
			// 				if (!InitAlarmIn(lChannel))
			// 				{
			// 					break;
			// 				}
			// 			}
			// 			lState = (m_AlarmInChannels[lChannel].m_dvr_AlarmIn_cfg.byAlarmType == 0) ? 1 : 0;
			// 			bSuccess = true;
		}
		break;
	case alarm_out:
		{
			NET_DVR_ALARMOUTSTATUS_V30 status;
			memset(&status,0,sizeof(status));
			if (!NET_DVR_GetAlarmOut_V30(m_lUserID,&status))
			{
				m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_GetAlarmOut_V30失败!", m_Config.deviceID);
				break;
			}
			lState = status.Output[lChannel];
			bSuccess = true;
		}
		break;
	default:
		break;
	}

	return bSuccess; 
}


bool VDHaiKangNVR::Destroy() 
{
	if ( m_lUserID != -1 )
	{
		NET_DVR_Logout_V30(m_lUserID);
		m_lUserID = -1;
		UnRegisterObject();
		if (g_VPHaiKangNVR.empty())
		{
			sdk_global_destroy();
		}
	}
	return true;
}

//设置时间OSD
bool VDHaiKangNVR::SetTime(Time& t) 
{ 
	NET_DVR_TIME dt;
	memset(&dt,0,sizeof(dt));

	dt.dwYear = t.year;
	dt.dwMonth = t.month;
	dt.dwDay = t.day;
	dt.dwHour = t.hour;
	dt.dwMinute = t.minute;
	dt.dwSecond = t.second;

	if ( NET_DVR_SetDVRConfig(m_lUserID,NET_DVR_SET_TIMECFG,0,&dt,sizeof(dt)) )
	{
		return true;
	}

	return false;
}

bool VDHaiKangNVR::Reboot()
{
	if ( NET_DVR_RebootDVR(m_lUserID) )
	{
		return true;
	}
	return false; 
}


//实时视频
bool VDHaiKangNVR::PlayVideo(long lChannel,void* dm, bool bMainStrean)
{   
    lChannel += 33;
	long lPlayChannelId = GetChannelPlayId(lChannel,bMainStrean);

	VPlatformChannel* vp_channel = NULL;
	{
		Poco::Mutex::ScopedLock locker(rec_mutex_);
		std::map<long,VPlatformChannel>::iterator pos;
		pos = records.find(lPlayChannelId);
		if(pos == records.end())
		{
			VPlatformChannel hkVideoChn;
			records.insert(std::make_pair(lPlayChannelId,hkVideoChn));
		}
		pos = records.find(lPlayChannelId);
		vp_channel = &pos->second;
	}


	NET_DVR_PREVIEWINFO dvr_pre_info;
	memset(&dvr_pre_info, 0, sizeof(NET_DVR_PREVIEWINFO));

	dvr_pre_info.lChannel = lChannel;
	if (bMainStrean)
	{
		dvr_pre_info.dwStreamType = 0;
	}else
	{
		dvr_pre_info.dwStreamType = 1;
	}
	dvr_pre_info.hPlayWnd = NULL;
	dvr_pre_info.bBlocked = 1;

	vp_channel->channelno_ = lChannel;
	vp_channel->is_mainstream_ = bMainStrean;
	vp_channel->vplatform_ = this;
	vp_channel->distribution_manager_ = dm;

	LONG lHandle = NET_DVR_RealPlay_V40(m_lUserID,&dvr_pre_info, RealDataCallBack, vp_channel);
	if (lHandle < 0)
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_RealPlay_V40 fail", m_Config.deviceID);
		return false;
	}

	vp_channel->play_handle_ = lHandle;
	return true;
}


bool VDHaiKangNVR::StopVideo(long lChannel,bool bMainStrean)
{
	lChannel += 33;
	long lPlayChannelId = GetChannelPlayId(lChannel,bMainStrean);
	BOOL result = TRUE;
	VPlatformChannel* vp_channel = NULL;
	{
		Poco::Mutex::ScopedLock locker(rec_mutex_);
		std::map<long,VPlatformChannel>::iterator pos;
		pos = records.find(lPlayChannelId);
		if (pos != records.end())
			vp_channel = &pos->second;
	}

	if (vp_channel != NULL)
	{
		result = NET_DVR_StopRealPlay(vp_channel->play_handle_);
		vp_channel->is_send_videoheader_=false;
		vp_channel->channelno_ = -1;
		vp_channel->is_mainstream_ = -1;
		vp_channel->vplatform_ = NULL;
		vp_channel->play_handle_ = -1;
		vp_channel->distribution_manager_ = NULL;
	}

	return result==TRUE?true:false;
}

//////////////////////////////////////////////////////////////////////////
bool VDHaiKangNVR::QueryRecord(long lChannel, const Poco::LocalDateTime& tStart, const Poco::LocalDateTime& tEnd,vector<Record>& rds)
{
	//兼容板卡
	lChannel += 33;
	NET_DVR_FILECOND fd;
	memset( &fd, 0, sizeof(fd) );

	fd.dwFileType = 0xff;
	fd.lChannel = lChannel;

	fd.struStartTime.dwYear = tStart.year();
	fd.struStartTime.dwMonth = tStart.month();
	fd.struStartTime.dwDay = tStart.day();
	fd.struStartTime.dwHour = tStart.hour();
	fd.struStartTime.dwMinute = tStart.minute();
	fd.struStartTime.dwSecond = tStart.second();

	fd.struStopTime.dwYear = tEnd.year();
	fd.struStopTime.dwMonth = tEnd.month();
	fd.struStopTime.dwDay = tEnd.day();
	fd.struStopTime.dwHour = tEnd.hour();
	fd.struStopTime.dwMinute = tEnd.minute();
	fd.struStopTime.dwSecond = tEnd.second();

	LONG lFind = NET_DVR_FindFile_V30(m_lUserID,&fd);

	if(lFind == -1)
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_FindFile_V30 fail", m_Config.deviceID);
		DWORD rl = NET_DVR_GetLastError();
		return false;
	}

	while (true)
	{
		NET_DVR_FINDDATA_V30 fd;
		LONG lRet = NET_DVR_FindNextFile_V30(lFind,&fd);
		if ( lRet == NET_DVR_FILE_SUCCESS )
		{
			Record rd;
			rd.m_lSize = fd.dwFileSize;
			rd.m_Start.assign(fd.struStartTime.dwYear, fd.struStartTime.dwMonth, fd.struStartTime.dwDay, fd.struStartTime.dwHour, fd.struStartTime.dwMinute, fd.struStartTime.dwSecond);
			rd.m_End.assign(fd.struStopTime.dwYear, fd.struStopTime.dwMonth, fd.struStopTime.dwDay, fd.struStopTime.dwHour, fd.struStopTime.dwMinute, fd.struStopTime.dwSecond);
			rd.m_sName = fd.sFileName;
			rds.push_back(rd);
		}
		else if ( lRet == NET_DVR_ISFINDING )
		{
			Sleep(5);
			continue;
		}
		else if ( (lRet == NET_DVR_NOMOREFILE) || (lRet == NET_DVR_FILE_NOFIND) )
		{
			break;
		} 
		else
		{
			break;
		}

	}


	NET_DVR_FindClose_V30(lFind);

	return true;
}


bool VDHaiKangNVR::RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext,long& lData, int& down_pos)
{
	lChannel += 33;
	if ( lRequestType == 0 )
	{
		NET_DVR_StopGetFile(lContext);
	}

	if ( lRequestType == 1 )
	{
		NET_DVR_TIME t1,t2;
		t1.dwYear = rd.m_Start.year();
		t1.dwMonth = rd.m_Start.month();
		t1.dwDay = rd.m_Start.day();
		t1.dwHour = rd.m_Start.hour();
		t1.dwMinute = rd.m_Start.minute();
		t1.dwSecond = rd.m_Start.second();

		t2.dwYear = rd.m_End.year();
		t2.dwMonth = rd.m_End.month();
		t2.dwDay = rd.m_End.day();
		t2.dwHour = rd.m_End.hour();
		t2.dwMinute = rd.m_End.minute();
		t2.dwSecond = rd.m_End.second();

		lContext = NET_DVR_GetFileByTime(m_lUserID,lChannel,&t1,&t2,(char*)rd.m_sName.c_str());

		if(lContext == -1)
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_GetFileByTime fail", m_Config.deviceID);
			return false;
		}

		if ( !NET_DVR_PlayBackControl(lContext, NET_DVR_PLAYSTART, 0, NULL) )
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_PlayBackControl fail", m_Config.deviceID);
			return false;
		}
	}

	if ( lRequestType == 2 )
	{
		int pos = NET_DVR_GetDownloadPos(lContext);
		if ( pos == 100 )
		{
			//down_pos为上次查询的大小
			if (pos > down_pos)
			{
				lData = e_download_more;
				down_pos = pos;
			}
			else
				lData = e_download_complete;
			//mlcai 2014-05-07
		}
		else if ( pos >=0 && pos < 100 )
		{
			//down_pos为上次查询的大小
			if (pos > down_pos)
			{
				lData = e_download_more;
				down_pos = pos;
			}
			else
				lData = e_download_wait;
			//mlcai 2014-05-07
		}
		else
		{
			lData = e_download_error;
		}
	}

	return true;
}

bool VDHaiKangNVR::GetNetParam(string& sIPAddr, string& sNetMask, string& sGateway)
{
	DWORD dwReturned = 0;

	//网络配置
	NET_DVR_NETCFG_V30 dvr_net_cfg;

	if (!NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_NETCFG_V30, 0, &dvr_net_cfg, sizeof(dvr_net_cfg), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "HaiKangNVR device id %s NET_DVR_GetDVRConfig 获取网络参数失败!", m_Config.deviceID);
		return false;
	}

	sIPAddr = dvr_net_cfg.struEtherNet[0].struDVRIP.sIpV4;
	sNetMask = dvr_net_cfg.struEtherNet[0].struDVRIPMask.sIpV4;
	sGateway = dvr_net_cfg.struGatewayIpAddr.sIpV4;
	return true;
}

//云台控制
bool VDHaiKangNVR::PTZControl(long lChannel,long lCommand, long lValue, void* pParam)
{ 
	lChannel += 33;
	long lPlayChannelId = GetChannelPlayId(lChannel,true);

	VPlatformChannel* vp_channel = NULL;
	{
		Poco::Mutex::ScopedLock locker(rec_mutex_);
		std::map<long,VPlatformChannel>::iterator pos;
		pos = records.find(lPlayChannelId);
		if(pos == records.end())
		{
			lPlayChannelId = GetChannelPlayId(lChannel, false);
			pos = records.find(lPlayChannelId);
			if(pos == records.end())
				return false;
		}
		vp_channel = &pos->second;
	}
	

	 if (vp_channel->play_handle_== -1)
		 return false;

	LONG lHKChnl = lChannel + 33;
	DWORD dwPTZCommand = 0;
	DWORD dwStop = 0;
	DWORD dwSpeed = 7;	// 1~7

#define VP_SET_PRESET	1001
#define VP_CLEAR_PRESET	1002
#define VP_CALL_PRESET	1003

	//预置点
	if(lCommand >= VP_SET_PRESET && lCommand <= VP_CALL_PRESET )
	{
		DWORD dwIndex = lValue;
		switch (lCommand)
		{
		case VP_SET_PRESET:
			dwPTZCommand = SET_PRESET;
			break;
		case VP_CLEAR_PRESET:
			dwPTZCommand = CLE_PRESET;
			break;
		case VP_CALL_PRESET:
			dwPTZCommand = GOTO_PRESET;
			break;
		default:
			break;
		}

		if (vp_channel->play_handle_!= -1 )
		{
			if ( NET_DVR_PTZPreset(vp_channel->play_handle_,dwPTZCommand,dwIndex) )
				return true;
		}
		else
		{
			if ( NET_DVR_PTZPreset_Other(m_lUserID, lHKChnl, dwPTZCommand, dwIndex) )
				return true;
		}
		return false;;
	}

	//0表示停止上次操作，1000以上为预置点设置
	if ( lCommand == 0 )
	{
		dwPTZCommand=m_dwLastCommand;
		dwStop = 1;
	}
	else
	{
		map<long,DWORD>::iterator pos;
		pos = g_PTZCommand_map.find(lCommand);

		//不支持的命令
		if(pos==g_PTZCommand_map.end())
			return false;

		dwPTZCommand = (pos->second)&0xffff;
		dwStop = (pos->second >> 16)&0xffff;

		m_dwLastCommand=dwPTZCommand;

		//WLL 2010.7.16
		dwSpeed = (lValue * 7 / 219) + 1;
		int rem = lValue % 219;

		if (rem == 0 && lValue != 0)
			dwSpeed = 7;
	}

	if (vp_channel->play_handle_!= -1 )
	{
		if ( NET_DVR_PTZControlWithSpeed(vp_channel->play_handle_, dwPTZCommand, dwStop, dwSpeed) )
			return true;
	}
	else
	{
		if ( NET_DVR_PTZControlWithSpeed_Other(m_lUserID, lHKChnl, dwPTZCommand, dwStop, dwSpeed) )
			return true;
	}

	return false; 
}

void VDHaiKangNVR::GetPointForPlace(WORD& x, WORD& y, WORD lPlace)
{
	//区域大小704*576

	//１－８分别为左上，中上，右上，中左，中右，左下，中下，右下八个位置。

	switch (lPlace)
	{
	case 1: x = 20; y = 32; break;
	case 2: x = 280; y = 32; break;
	case 3: x = 530; y = 32; break;
	case 4: x = 20; y = 200; break;
	case 5: x = 530; y = 200; break;
	case 6: x = 20; y = 520; break;
	case 7: x = 280; y = 520; break;
	case 8: x = 530; y = 520; break;
	default: x = 20; y = 32; break;
	}
}

void VDHaiKangNVR::GetPlaceForPoint(WORD x, WORD y, WORD& lPlace)
{
	if ( y <= 32 )
	{
		if ( x <= 20 )
		{
			lPlace = 1;
		}
		else if ( x <= 300 )
		{
			lPlace = 2;
		}
		else
		{
			lPlace = 3;
		}
	}
	else if ( y <= 200 )
	{
		if ( x <= 20 )
		{
			lPlace = 4;
		}
		else
		{
			lPlace = 5;
		}
	}
	else
	{
		if ( x <= 20 )
		{
			lPlace = 6;
		}
		else if ( x <= 300 )
		{
			lPlace = 7;
		}
		else
		{
			lPlace = 8;
		}
	}
}

//OSD
bool VDHaiKangNVR::SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,string& sWord)
{
	if(!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
			return false;
	}

	NET_DVR_PICCFG_V30& dvr_pic_cfg = GetPictureInfo(lChannel);

	memset(dvr_pic_cfg.sChanName,0,NAME_LEN);

	strcpy_s((char*)dvr_pic_cfg.sChanName,NAME_LEN,sWord.c_str());

	dvr_pic_cfg.dwShowChanName = bShowOsd;
	dvr_pic_cfg.dwShowOsd = bShowOsd;

	WORD x, y;

	GetPointForPlace(x, y, nOSDPlace);
	dvr_pic_cfg.wOSDTopLeftX = x;
	dvr_pic_cfg.wOSDTopLeftY = y;

	GetPointForPlace(x, y, nWordPlace);
	dvr_pic_cfg.wShowNameTopLeftX = x;
	dvr_pic_cfg.wShowNameTopLeftY = y;

	return SetPicturInfo(lChannel);
}

bool VDHaiKangNVR::GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,string& sWord)
{	
	lChannel += 33;
if (!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
		{
			return false;
		}
	}

	NET_DVR_PICCFG_V30& dvr_pic_cfg = GetPictureInfo(lChannel);

	//区域大小704*576
	bShowOsd = dvr_pic_cfg.dwShowOsd;
	GetPlaceForPoint(dvr_pic_cfg.wOSDTopLeftX, dvr_pic_cfg.wOSDTopLeftY, nOSDPlace);
	GetPlaceForPoint(dvr_pic_cfg.wShowNameTopLeftX, dvr_pic_cfg.wShowNameTopLeftY, nWordPlace);
	sWord = (char*)dvr_pic_cfg.sChanName;

	return true;

}
//帧率
bool VDHaiKangNVR::SetFrameRate(long lChannel,long nFrameRate, long nKeyFrameIntervals,bool bMainStream) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
		{
			return false;
		}
	}

	if ( nFrameRate == -1 && nKeyFrameIntervals == -1 )
	{
		return true;
	}

	NET_DVR_COMPRESSION_INFO_V30& dvr_compress_cfg = GetCompressInfo(lChannel);
	if ( nFrameRate != -1 )
	{
		if (nFrameRate < 5)
		{
			dvr_compress_cfg.dwVideoFrameRate = 7;
		}
		else if (nFrameRate < 13)
		{
			dvr_compress_cfg.dwVideoFrameRate = nFrameRate/2 + 5;
		}
		else if (nFrameRate < 21)
		{
			dvr_compress_cfg.dwVideoFrameRate = nFrameRate/4 + 8;
		}
		else
		{
			dvr_compress_cfg.dwVideoFrameRate = 0;
		}
	}

	if ( nKeyFrameIntervals != -1 )
	{
		dvr_compress_cfg.byIntervalBPFrame = (BYTE)nKeyFrameIntervals;
	}

	return SetCompressInfo(lChannel); 
}
bool VDHaiKangNVR::GetFrameRate(long lChannel,long& nFrameRate, long& nKeyFrameIntervals,bool bMainStream) 
{ 
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
			return false;
	}

	NET_DVR_COMPRESSION_INFO_V30& dvr_compress_cfg = GetCompressInfo(lChannel);

	//帧率 0-全部; 1-1/16; 2-1/8; 3-1/4; 4-1/2; 5-1; 6-2; 7-4; 8-6; 9-8; 10-10; 11-12; 12-16; 13-20;
	DWORD dwVideoFrameRate = dvr_compress_cfg.dwVideoFrameRate;

	if (dwVideoFrameRate == 0)
	{
		nFrameRate = 25;
	}
	else if(dwVideoFrameRate < 7)
	{
		nFrameRate = 3;
	}
	else if (dwVideoFrameRate < 17)
	{
		nFrameRate = 2*(dwVideoFrameRate - 5);
	}
	else if (dwVideoFrameRate < 21)
	{
		nFrameRate = 4*(dwVideoFrameRate - 8);
	}
	else
	{
		nFrameRate = 25;	
	}

	nKeyFrameIntervals = dvr_compress_cfg.wIntervalFrameI;

	return true; 
}

//设置图像质量
bool VDHaiKangNVR::SetPictureQuality(long lChannel,long nLevel,bool bMainStream) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
			return false;
	}

	if ( nLevel == -1 )
		return true;

	GetCompressInfo(lChannel).byPicQuality = (BYTE)nLevel;

	return SetCompressInfo(lChannel); 
}

//获取图像质量
bool VDHaiKangNVR::GetPictureQuality(long lChannel,long& nLevel,bool bMainStream) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
		{
			return false;
		}
	}

	nLevel = GetCompressInfo(lChannel).byPicQuality;
	if (nLevel > 3)
	{
		nLevel = 3;
	}

	return true;
}

//最大码流
bool VDHaiKangNVR::SetMaxBPS(long lChannel,long nMaxBPS, bool vbr,bool bMainStream) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
		{
			return false;
		}
	}

	if ( nMaxBPS == -1 )
	{
		return true;
	}

	BYTE byBitrateTypeOld = GetCompressInfo(lChannel).byBitrateType;
	DWORD dwVideoBitrateOld = GetCompressInfo(lChannel).dwVideoBitrate;

	//GetCompressInfo(lChannel).dwVideoBitrate = nMaxBPS|0x80000000;
	//视频码率 0-保留 1-16K(保留) 2-32K 3-48k 4-64K 5-80K 6-96K 7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
	// 13-384K 14-448K 15-512K 16-640K 17-768K 18-896K 19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
	//最高位(31位)置成1表示是自定义码流, 0-30位表示码流值(MIN-32K MAX-8192K)。

	DWORD dwVideoBitrate = 0;
	do 
	{
		if ( nMaxBPS <= 32 ){ dwVideoBitrate = 2; break; }
		if ( nMaxBPS <= 48 ){ dwVideoBitrate = 3; break; }
		if ( nMaxBPS <= 64 ){ dwVideoBitrate = 4; break; }
		if ( nMaxBPS <= 80 ){ dwVideoBitrate = 5; break; }
		if ( nMaxBPS <= 96 ){ dwVideoBitrate = 6; break; }
		if ( nMaxBPS <= 128 ){ dwVideoBitrate = 7; break; }
		if ( nMaxBPS <= 160 ){ dwVideoBitrate = 8; break; }
		if ( nMaxBPS <= 192 ){ dwVideoBitrate = 9; break; }
		if ( nMaxBPS <= 224 ){ dwVideoBitrate = 10; break; }
		if ( nMaxBPS <= 256 ){ dwVideoBitrate = 11; break; }
		if ( nMaxBPS <= 320 ){ dwVideoBitrate = 12; break; }
		if ( nMaxBPS <= 384 ){ dwVideoBitrate = 13; break; }
		if ( nMaxBPS <= 448 ){ dwVideoBitrate = 14; break; }
		if ( nMaxBPS <= 512 ){ dwVideoBitrate = 15; break; }
		if ( nMaxBPS <= 640 ){ dwVideoBitrate = 16; break; }
		if ( nMaxBPS <= 768 ){ dwVideoBitrate = 17; break; }
		if ( nMaxBPS <= 896 ){ dwVideoBitrate = 18; break; }
		if ( nMaxBPS <= 1024 ){ dwVideoBitrate = 19; break; }
		if ( nMaxBPS <= 1280 ){ dwVideoBitrate = 20; break; }
		if ( nMaxBPS <= 1536 ){ dwVideoBitrate = 21; break; }
		if ( nMaxBPS <= 1792 ){ dwVideoBitrate = 22; break; }
		if ( nMaxBPS <= 2048 ){ dwVideoBitrate = 23; break; }

		dwVideoBitrate = nMaxBPS * 1024;
		dwVideoBitrate |= 1 << 31;

	} while (0);

	GetCompressInfo(lChannel).byBitrateType = (BYTE)(!vbr);
	GetCompressInfo(lChannel).dwVideoBitrate = dwVideoBitrate;

	if ( !SetCompressInfo(lChannel) )
	{
		GetCompressInfo(lChannel).byBitrateType = byBitrateTypeOld;
		GetCompressInfo(lChannel).dwVideoBitrate = dwVideoBitrateOld;
		return false;
	}

	return true;
}
bool VDHaiKangNVR::GetMaxBPS(long lChannel,long& nMaxBPS, bool& vbr,bool bMainStream) 
{ 
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
		{
			return false;
		}
	}

	vbr = GetCompressInfo(lChannel).byBitrateType == 0;

	DWORD dwVideoBitrate = GetCompressInfo(lChannel).dwVideoBitrate;

	BOOL bCustom = dwVideoBitrate>>31;
	dwVideoBitrate = dwVideoBitrate&~(1<<31);
	if (bCustom)
	{
		nMaxBPS = dwVideoBitrate / 1024;
		return true;
	}


	//视频码率 0-保留 1-16K(保留) 2-32K 3-48k 4-64K 5-80K 6-96K 7-128K 8-160k 9-192K 10-224K 11-256K 12-320K
	// 13-384K 14-448K 15-512K 16-640K 17-768K 18-896K 19-1024K 20-1280K 21-1536K 22-1792K 23-2048K
	//最高位(31位)置成1表示是自定义码流, 0-30位表示码流值(MIN-32K MAX-8192K)。
	switch (dwVideoBitrate)
	{
	case 15:nMaxBPS = 512;break;
	case 17:nMaxBPS = 768;break;
	case 19:nMaxBPS = 1024;break;
	case 20:nMaxBPS = 1280;break;

	case 1:nMaxBPS = 16;break;
	case 2:nMaxBPS = 32;break;
	case 3:nMaxBPS = 48;break;
	case 4:nMaxBPS = 64;break;
	case 5:nMaxBPS = 80;break;
	case 6:nMaxBPS = 96;break;
	case 7:nMaxBPS = 128;break;
	case 8:nMaxBPS = 160;break;
	case 9:nMaxBPS = 192;break;
	case 10:nMaxBPS = 224;break;
	case 11:nMaxBPS = 256;break;
	case 12:nMaxBPS = 320;break;
	case 13:nMaxBPS = 384;break;
	case 14:nMaxBPS = 448;break;

	case 16:nMaxBPS = 640;break;

	case 18:nMaxBPS = 896;break;

	case 21:nMaxBPS = 1536;break;
	case 22:nMaxBPS = 1792;break;
	case 23:nMaxBPS = 2048;break;
	default:nMaxBPS = 512;break;
	}

	return true; 
}

//图像格式
bool VDHaiKangNVR::SetPictureFormat(long lChannel,long nPicFormat, string& sSupportFormat,bool bMainStream) 
{
	lChannel+=33; 
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
		{
			return false;
		}
	}

	if ( nPicFormat == -1 )
	{
		return true;
	}


	BYTE byResolutionOld = GetCompressInfo(lChannel).byResolution;
	BYTE byResolution;

	switch (nPicFormat)
	{
	case 1:byResolution=2;break;
	case 3:byResolution=3;break;
	case 2:byResolution=4;break;
	default:byResolution=1;break;
	}

	sSupportFormat = "0123";

	GetCompressInfo(lChannel).byResolution = byResolution;

	if ( !SetCompressInfo(lChannel) )
	{
		GetCompressInfo(lChannel).byResolution = byResolutionOld;
		return false;
	}
	return true; 
}

bool VDHaiKangNVR::GetPictureFormat(long lChannel,long& nPicFormat, string& sSupportFormat,bool bMainStream) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitCompressCfg)
	{
		if (!InitCompressCfg(lChannel))
		{
			return false;
		}
	}

	BYTE byResolution = GetCompressInfo(lChannel).byResolution;
	switch (byResolution)
	{
	case 2:nPicFormat=1;break;
	case 3:nPicFormat=3;break;
	case 4:nPicFormat=2;break;
	default:nPicFormat=0;break;
	}
	sSupportFormat = "0123";

	return true; 
}

//视频参数
bool VDHaiKangNVR::SetVideoPara(long lChannel,long nBrightness, long nContrast, long nSaturation /*饱和度*/, long nHue/*色调*/) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
		{
			return false;
		}
	}

	long a, b, c, d;
	NET_DVR_PICCFG_EX& ex_cfg = GetExPictureInfo(lChannel);
	a = ex_cfg.byBrightness;
	b = ex_cfg.byContrast;
	c = ex_cfg.bySaturation;
	d = ex_cfg.byHue;

	ex_cfg.byBrightness = nBrightness;
	ex_cfg.byContrast = nContrast;
	ex_cfg.bySaturation = nSaturation;
	ex_cfg.byHue = nHue;

	if (!SetExPicturInfo(lChannel))
	{
		ex_cfg.byBrightness = a;
		ex_cfg.byContrast = b;
		ex_cfg.bySaturation = c;
		ex_cfg.byHue = d;
		return false;
	}
	return true;
}
bool VDHaiKangNVR::GetVideoPara(long lChannel,long& nBrightness, long& nContrast, long& nSaturation /*饱和度*/, long& nHue/*色调*/) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
		{
			m_Config.vp_log->log(Message::PRIO_TRACE,"%s GetVideoPara InitPicCfg Failed",m_Config.serverIP);
			return false;
		}
	}

	NET_DVR_PICCFG_EX& ex_cfg = GetExPictureInfo(lChannel);
	nBrightness = ex_cfg.byBrightness;
	nContrast = ex_cfg.byContrast;
	nSaturation = ex_cfg.bySaturation;
	nHue = ex_cfg.byHue;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//侦测区域
bool VDHaiKangNVR::SetDetectRect(long lChannel,vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade, long nMaxRectNum) 
{ 	
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
		{
			return false;
		}
	}


	NET_DVR_MOTION_V30& mo = video_Para_records[lChannel].m_dvr_pic_cfg.struMotion;

	//侦测区域,共有22*18个小宏块,为1表示该宏块是移动侦测区域,0-表示不是

	//首先清空
	for (int i=0;i<64;i++)
	{
		for (int j=0;j<96;j++)
		{
			mo.byMotionScope[i][j] = 0;
		}
	}

	for (int i=0;i<(int)Rects.size();i++)
	{
		int col_start = Rects[i].Left * 18 / 288;
		int col_end = Rects[i].Right * 18 / 288 ;
		int row_start = Rects[i].Top * 22 / 352;
		int row_end = Rects[i].Bottom * 22 / 352;

		for (int j=row_start;j<row_end;j++)
		{
			for (int k=col_start;k<col_end;k++)
			{
				mo.byMotionScope[j][k] = 1;
			}
		}
	}

	mo.byEnableHandleMotion = (BYTE)bDectecting;
	mo.byMotionSensitive = (BYTE)(nDetectGrade*5/6);
	mo.struMotionHandleType.dwHandleType = 0x04;

	SetFullSchemeTime(mo.struAlarmTime);

	if (SetPicturInfo(lChannel))
	{
		return true;
	}

	return false; 
}

//海康侦测区域采用宏块的方式，不够精确，直接返回false
bool VDHaiKangNVR::GetDetectRect(long lChannel,vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade, long& nMaxRectNum) 
{
	return false; 
}

//遮挡区域
bool VDHaiKangNVR::SetShadeRect(long lChannel,vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
		{
			return false;
		}
	}

	NET_DVR_PICCFG_V30& dvr_pic_cfg = GetPictureInfo(lChannel);

	dvr_pic_cfg.dwEnableHide = bLogoing;

	memset(&dvr_pic_cfg.struShelter,0,sizeof(NET_DVR_SHELTER) * MAX_SHELTERNUM);
	for (int i=0;( i<(int)Rects.size() && i<MAX_SHELTERNUM );i++)
	{
		NET_DVR_SHELTER& shr = dvr_pic_cfg.struShelter[i];
		shr.wHideAreaTopLeftX = (WORD)Rects[i].Left * 2 + 2;
		shr.wHideAreaTopLeftY = (WORD)Rects[i].Top * 2 + 2;
		shr.wHideAreaWidth = (WORD)Rects[i].Width() * 2;
		shr.wHideAreaHeight = (WORD)Rects[i].Height() * 2;
	}

	if (SetPicturInfo(lChannel))
	{
		return true;
	}

	return false; 
}
bool VDHaiKangNVR::GetShadeRect(long lChannel,vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum) 
{
	lChannel+=33;
	if (!video_Para_records[lChannel].m_bInitPicCfg)
	{
		if (!InitPicCfg(lChannel))
		{
			return false;
		}
	}

	NET_DVR_PICCFG_V30& dvr_pic_cfg = GetPictureInfo(lChannel);
	bLogoing = dvr_pic_cfg.dwEnableHide;
	nMaxRectNum = MAX_SHELTERNUM;

	for (int i=0;i<MAX_SHELTERNUM;i++)
	{
		NET_DVR_SHELTER& shr = dvr_pic_cfg.struShelter[i];
		Rect rc;
		rc.Left = shr.wHideAreaTopLeftX / 2 - 2 ;
		rc.Top = shr.wHideAreaTopLeftY / 2  - 2;
		rc.Right = shr.wHideAreaWidth / 2 + rc.Left ;
		rc.Bottom = shr.wHideAreaHeight / 2 + rc.Top ;
		if ( rc.Width()>0 && rc.Height()>0 )
		{
			Rects.push_back(rc);
		}
	}
	return true; 
}


//开始语音呼叫
bool VDHaiKangNVR::StartAudioCall()
{
	StopAudioCall();

	//HxTrace("VDHaiKangNVR::StartAudioCall()");

	//m_lAudioID = NET_DVR_StartVoiceCom_MR_V30(m_lUserID, 1, NULL, NULL);
	//if ( m_lAudioID != -1 )
	//{
	//	return true;
	//}
	
//	HxTrace("NET_DVR_StartVoiceCom_MR failed : %d", NET_DVR_GetLastError());
	
	return false;
}

//停止语音呼叫
bool VDHaiKangNVR::StopAudioCall()
{
	//if ( m_lAudioID != -1 )
	//{
	//	//HxTrace("VDHaiKangNVR::StopAudioCall()");

	//	if ( NET_DVR_StopVoiceCom(m_lAudioID) )
	//	{
	//		m_lAudioID = -1;
	//		return true;
	//	}
	//}


	return false;
}

//转发音频数据到设备
bool VDHaiKangNVR::SendAudioData(void * pBuffer, long lSize)
{
	//HxTrace("HaiKang SendAudioData , lSize:%d, tick:%d", lSize, GetTickCount());
	//if ( m_lAudioID != -1 )
	//{
	//	if( NET_DVR_VoiceComSendData(m_lAudioID,(char*)pBuffer,lSize) )
	//		return true;
	//}
//	HxTrace("HaiKang SendAudioData failed, lSize:%d",lSize);

	return false;
}

//实现VD工厂
VDHaiKangNVRMaker::VDHaiKangNVRMaker()
{
}

VDHaiKangNVRMaker::~VDHaiKangNVRMaker()
{
}

VirtualDevice* VDHaiKangNVRMaker::Make()
{
	return new VDHaiKangNVR;
}

void VDHaiKangNVRMaker::Reclaim(VirtualDevice* vd)
{
	delete vd;
}

extern "C" __declspec(dllexport) void RegisterVDMaker(VirtualDeviceFactory* vd_factory)
{
	//factory->AddVPlatformCreator("RTSP", new VPRtspCreator);
	vd_factory->AddVirtualDeviceMaker("HaiKangNVR", new VDHaiKangNVRMaker);
}