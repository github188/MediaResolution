#include "stdafx.h"
#include "dhconfigsdk.h"
#include "VDDaHuaDVR.h"
#include "Markup.h"

bool g_bInitSDK = false;
Poco::FastMutex g_sdkLock;
hash_map<string, VDDaHuaDVR*> g_VPDaHua;
int g_sequence=0;


//云台命令映射 (lCommand -> wStop<<16|PTZCommand )
std::map<long,DWORD> ptz_command_map_;

VDDaHuaDVR::VDDaHuaDVR()
{
	user_id_ = -1;
	g_bInitSDK = false;
	m_bNewVer=true;
	memset(&m_dev_info,0,sizeof(m_dev_info));
	last_ptz_command_=0;
}

VDDaHuaDVR::~VDDaHuaDVR()
{
	Destroy();
}

//记录注册到设备的用户
void VDDaHuaDVR::RegisterObject()
{
	Poco::ScopedLock<Poco::FastMutex> lock(g_sdkLock);
	g_sequence++;
	char buf[100];
	sprintf(buf,"%s_%d",m_Config.serverIP.c_str(),g_sequence);
	_key.assign(buf);
	g_VPDaHua[ _key ] = this;
}

//清除已注销的用户
void VDDaHuaDVR::UnRegisterObject()
{
	Poco::ScopedLock<Poco::FastMutex> lock(g_sdkLock);
	g_VPDaHua.erase(_key );
}

//初始化
int VDDaHuaDVR::Init(VDeviceConfig& dev_config) 
{
	// Poco::ScopedLock<Poco::FastMutex> lock(g_sdkLock);
	g_sdkLock.lock();
	if (!g_bInitSDK)
	{
		if( !CLIENT_Init( 0, 0 ) )
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR CLIENT_Init失败!");
            g_sdkLock.unlock();
			return -1;
		}
		g_bInitSDK = true;
	}
	//初始化报警类型映射 0-信号量报警,1-硬盘满,2-信号丢失,3－移动侦测,4－硬盘未格式化,5-读写硬盘出错,6-遮挡报警,7-制式不匹配, 8-非法访问
	//alarm_type_map_.insert(std::hash_map<DWORD, long>::value_type(0, alarm_in));
	//alarm_type_map_.insert(std::hash_map<DWORD, long>::value_type(1, alarm_disk_full));
	//alarm_type_map_.insert(std::hash_map<DWORD, long>::value_type(2, alarm_video_lost));
	//alarm_type_map_.insert(std::hash_map<DWORD, long>::value_type(3, alarm_camera));
	//alarm_type_map_.insert(std::hash_map<DWORD, long>::value_type(4, alarm_disk_not_format));
	//alarm_type_map_.insert(std::hash_map<DWORD, long>::value_type(5, alarm_disk_fault));
	g_sdkLock.unlock();

	InitPTZCommand();

	int err_code;
	user_id_ = CLIENT_Login((char*)m_Config.serverIP.c_str(), m_Config.port, (char*)m_Config.user.c_str(), (char*)m_Config.passwd.c_str(), &m_dev_info, &err_code );// 注册用户到设备
	if( user_id_ == 0 )
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_Login失败!", m_Config.deviceID);
		return -1;
	}

	//判断设备版本
	int nProtoVer;
	int nRet;
	BOOL bSuccess = CLIENT_QueryDevState(user_id_, DH_DEVSTATE_PROTOCAL_VER, (char*)&nProtoVer, sizeof(int), &nRet, 1000);
	if (bSuccess)
	{
		if (nProtoVer >= 5)		//Current device is V2.6 baseline or higher.
			m_bNewVer = true;
		else
			m_bNewVer = false;
	}

	AV_CFG_ChannelName cfg_name;
	memset(&cfg_name, 0, sizeof(cfg_name));
	cfg_name.nStructSize = sizeof(cfg_name);
	
	int nerror;
	char *szOutBuffer = new char[32 * 1024];
	for (int i = 0; i < (int)m_dev_info.byChanNum; ++i)
	{
		memset(szOutBuffer, 0, 32* 1024);
		BOOL bSuccess = CLIENT_GetNewDevConfig(user_id_, CFG_CMD_CHANNELTITLE, i, szOutBuffer, 32*1024, &nerror, 3000);
		if(bSuccess)
		{
			int nRetLen = 0;
			BOOL bRet = CLIENT_ParseData(CFG_CMD_CHANNELTITLE, szOutBuffer, &cfg_name, sizeof(AV_CFG_ChannelName),&nRetLen);
			if (!bRet)
			{
				m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetNewDevConfig->CLIENT_ParseData 获取通道%d参数失败!", m_Config.deviceID, i + 1);
				dev_config.MediaChannls.insert(map<int, string>::value_type(i + 1, ""));
			}
			else
				dev_config.MediaChannls.insert(map<int, string>::value_type(i + 1, cfg_name.szName));
		}
		else
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetNewDevConfig 获取通道%d参数失败!", m_Config.deviceID, i + 1);
			dev_config.MediaChannls.insert(map<int, string>::value_type(i + 1, ""));
		}
	}


	RegisterObject();
	return 0;
}

//云台初始化，映射
bool VDDaHuaDVR::InitPTZCommand()
{
	DWORD is_top = 1<<16;

	ptz_command_map_[1] = DH_PTZ_UP_CONTROL;
	ptz_command_map_[2] = DH_PTZ_DOWN_CONTROL;
	ptz_command_map_[3] = DH_PTZ_LEFT_CONTROL;
	ptz_command_map_[4] = DH_PTZ_RIGHT_CONTROL;

	ptz_command_map_[5] = DH_PTZ_APERTURE_ADD_CONTROL;
	ptz_command_map_[6] = DH_PTZ_APERTURE_DEC_CONTROL;
	ptz_command_map_[7] = DH_PTZ_ZOOM_ADD_CONTROL;
	ptz_command_map_[8] = DH_PTZ_ZOOM_DEC_CONTROL;
	ptz_command_map_[9] = DH_PTZ_FOCUS_ADD_CONTROL;
	ptz_command_map_[10] = DH_PTZ_FOCUS_DEC_CONTROL;

	ptz_command_map_[11] = DH_EXTPTZ_STARTLINESCAN;
	ptz_command_map_[12] = DH_EXTPTZ_CLOSELINESCAN;

	ptz_command_map_[13] = DH_PTZ_LAMP_CONTROL;
	ptz_command_map_[14] = DH_PTZ_LAMP_CONTROL|is_top;//灯光关
	ptz_command_map_[15] = DH_PTZ_LAMP_CONTROL;
	ptz_command_map_[16] = DH_PTZ_LAMP_CONTROL|is_top;//雨刷关

	ptz_command_map_[33] = DH_EXTPTZ_RIGHTTOP;
	ptz_command_map_[34] = DH_EXTPTZ_LEFTTOP;
	ptz_command_map_[35] = DH_EXTPTZ_RIGHTDOWN;
	ptz_command_map_[36] = DH_EXTPTZ_LEFTDOWN;

	ptz_command_map_[101] = DH_EXTPTZ_REVERSECOMM;

	ptz_command_map_[1001] = DH_PTZ_POINT_SET_CONTROL;
	ptz_command_map_[1002] = DH_PTZ_POINT_DEL_CONTROL;
	ptz_command_map_[1003] = DH_PTZ_POINT_MOVE_CONTROL;

	return true;
}

bool VDDaHuaDVR::Destroy()
{
	if ( user_id_ != -1 )
	{
		CLIENT_Logout( user_id_ );
		user_id_ = -1;
		UnRegisterObject();
		if( g_VPDaHua.empty() )
		{
			CLIENT_Cleanup();
			g_bInitSDK = false;
		}
	}
	return true;
}


void CALLBACK RealDataCallBack(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser)
{
	try
	{
		VPlatformChannel * pChannel = (VPlatformChannel*)dwUser;
		VDDaHuaDVR* pPlatform = (VDDaHuaDVR*)pChannel->vplatform_;

		long lType;
		if(pChannel->is_mainstream_)
		{
			lType = video_stream;
		}
		else
		{
			lType = video_sub_stream;
		}
		if (pChannel->is_send_videoheader_==false)
		{
			long size;
			string head;

			pPlatform->GetRecordHeader(pChannel->channelno_,size,head);

			CMarkup head_xml;
			head_xml.AddElem("Information");
			head_xml.AddElem("Protocol", head);
			const string& head_str = head_xml.GetDoc();

			pPlatform->m_pCallBack->NotifyStreamData(pChannel->channelno_,lType,head_frame,(PBYTE)head.c_str(),head.size(), pChannel->distribution_manager_);

			pChannel->is_send_videoheader_ = true;
		}
		//通知上层接收视频数据
		pPlatform->m_pCallBack->NotifyStreamData(pChannel->channelno_,lType,unknown_frame,pBuffer,dwBufSize, pChannel->distribution_manager_);
	}
	catch (...)
	{

	}

}

//实时视频
 bool VDDaHuaDVR::PlayVideo(long lChannel, void* dm, bool bMainStrean)
{
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
	
	DH_RealPlayType  playType;
	if (bMainStrean)
	{
		playType = DH_RType_Realplay_0;
	}else
	{
		playType = DH_RType_Realplay_1;
	}
	LONG lHandle = CLIENT_RealPlayEx(user_id_,lChannel,NULL,playType);

	if (lHandle <= 0)
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_RealPlayEx fail", m_Config.deviceID);
		return false;	
	}

	vp_channel->play_handle_ = lHandle;
	vp_channel->channelno_ = lChannel;
	vp_channel->is_mainstream_ = bMainStrean;
	vp_channel->vplatform_ = this;
	vp_channel->distribution_manager_ = dm;

	BOOL result = CLIENT_SetRealDataCallBack(lHandle, RealDataCallBack,(DWORD)vp_channel);

	return true;
}

//停止实时监控
bool VDDaHuaDVR::StopVideo(long lChannel,bool bMainStrean)
{
	long lPlayChannelId = GetChannelPlayId(lChannel,bMainStrean);

// 	std::map<long,VPlatformChannel>::iterator pos;
// 	pos = records.find(lPlayChannelId);
// 	BOOL result = TRUE;
// 	if(pos != records.end())
// 	{
// 		VPlatformChannel& channel = pos->second;
// 		if(channel.play_handle_!=-1)
// 		{
// 			result  = CLIENT_StopRealPlayEx(channel.play_handle_);
// 			channel.is_send_videoheader_=false;
// 			channel.channelno_ = -1;
// 			channel.is_mainstream_ = -1;
// 			channel.vplatform_ = NULL;
// 			channel.play_handle_ = -1;
// 			channel.distribution_manager_ = NULL;
// 		}
//	}

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
		result = CLIENT_StopRealPlayEx(vp_channel->play_handle_);
		vp_channel->is_send_videoheader_=false;
		vp_channel->channelno_ = -1;
		vp_channel->is_mainstream_ = -1;
		vp_channel->vplatform_ = NULL;
		vp_channel->play_handle_ = -1;
		vp_channel->distribution_manager_ = NULL;
	}

	return result==TRUE?true:false;
}

bool VDDaHuaDVR::GetNetParam(std::string& sIPAddr, std::string& sNetMask, std::string& sGateway)
{
	DWORD dwReturned = 0;
	
	//网络参数
	DHDEV_NET_CFG net_cfg;

	if (!CLIENT_GetDevConfig(user_id_, DH_DEV_NETCFG, 0, &net_cfg, sizeof(net_cfg), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取网络参数失败!", m_Config.deviceID);
		return false;
	}

	sIPAddr = net_cfg.stEtherNet[0].sDevIPAddr;
	sNetMask = net_cfg.stEtherNet[0].sDevIPMask; 
	sGateway = net_cfg.stEtherNet[0].sGatewayIP;
	return true;
}

//add by sunminping
BOOL VDDaHuaDVR::PtzControl(long lChannel,int type, BOOL stop, int param)
{
	BOOL ret;
	BOOL upRet;

	switch(type) {
		//在主页面的控制
	case DH_PTZ_UP_CONTROL  :    //上           
	case DH_PTZ_DOWN_CONTROL:		//下
	case DH_PTZ_LEFT_CONTROL:	//左
	case DH_PTZ_RIGHT_CONTROL:		//右
	case DH_PTZ_ZOOM_ADD_CONTROL:		//变倍
	case DH_PTZ_ZOOM_DEC_CONTROL:		
	case DH_PTZ_FOCUS_ADD_CONTROL:	//调焦
	case DH_PTZ_FOCUS_DEC_CONTROL:	
	case DH_PTZ_APERTURE_ADD_CONTROL:	//光圈
	case DH_PTZ_APERTURE_DEC_CONTROL:
	case DH_EXTPTZ_LEFTTOP :
	case DH_EXTPTZ_RIGHTTOP :
	case DH_EXTPTZ_LEFTDOWN :
	case DH_EXTPTZ_RIGHTDOWN:
		break;
	default:
		return FALSE;
	}

	if(type >= DH_EXTPTZ_LEFTTOP)
	{
		ret = CLIENT_DHPTZControl(user_id_,lChannel,type,(BYTE)param,(BYTE)param,0,stop);
		if (!ret)
			m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_DHPTZControl line %d 失败!",m_Config.deviceID,__LINE__);
		else
			last_ptz_command_=type;
	}
	else
	{
		ret = CLIENT_DHPTZControl(user_id_,lChannel,type, 0, (BYTE)param,0, stop);
		if (!ret)
			m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_DHPTZControl line %d 失败!",m_Config.deviceID,__LINE__);
		else
			last_ptz_command_=type;
	}
	if(!ret)
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s PtzControl 失败!",m_Config.deviceID);
	return ret;
}

BOOL VDDaHuaDVR::Extptzctrl(long lChannel,int nPresetPoint,DWORD dwCommand,DWORD dwParam)     //扩展接口
{
	BOOL ret = FALSE;
	BYTE bParam[3] = {0};

	if(!lChannel)
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s Extptzctrl 通道为0!",m_Config.deviceID);

	switch(dwCommand) 
	{
	case DH_PTZ_POINT_MOVE_CONTROL:     //执行
	case DH_PTZ_POINT_SET_CONTROL:      //添加
	case DH_PTZ_POINT_DEL_CONTROL:      //删除
		bParam[1] = (BYTE)nPresetPoint;
		break;
	case DH_EXTPTZ_UP_TELE:
		bParam[0] = dwParam;
		break;
	default: 
		return FALSE;
	}
	ret = CLIENT_DHPTZControlEx(user_id_,lChannel,dwCommand ,bParam[0],bParam[1],bParam[2],FALSE);
	if(!ret)
		m_Config.vp_log->log(Message::PRIO_ERROR,"DaHuaDVR device id %s CLIENT_DHPTZControlEx line %d 执行失败!",m_Config.deviceID,__LINE__);
	return ret;
}
//end add

bool VDDaHuaDVR::PTZControl(long lChannel,long lCommand, long lValue, void* pParam)
{
	DWORD ptz_command = 0;
	if ( lCommand == 0 )
	{
		if (last_ptz_command_ == 0)
			return true;

		ptz_command = last_ptz_command_;
		if ( CLIENT_DHPTZControl(user_id_,lChannel,ptz_command,0,8,0,TRUE))
		{
			last_ptz_command_ = ptz_command;
			return true;
		}
		return false;
	}
	else
	{
		std::map<long,DWORD>::iterator pos;
		pos = ptz_command_map_.find(lCommand);

		//不支持的命令
		if ( pos == ptz_command_map_.end() )
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s 收到不支持的云台控制命令!!",m_Config.deviceID);
			return false;
		}

		ptz_command = (pos->second)&0xffff;

		int speed=lValue/9+1;         //大华的云台速度范围为[1,2,3,4,5,6,7,8],平台速度范围为1-63的整数闭区间.

		if(lCommand<1000)          //大于1000为预置位功能
			return PtzControl(lChannel,ptz_command,FALSE,speed);
		else
			return Extptzctrl(lChannel,lValue,ptz_command,0);
	}
}

//查询录像
bool VDDaHuaDVR::QueryRecord(long lChannel, Poco::LocalDateTime& tStart, Poco::LocalDateTime& tEnd,vector<Record>& rds)
{
	NET_TIME t1,t2;
	t1.dwYear = tStart.year();
	t1.dwMonth = tStart.month();
	t1.dwDay = tStart.day();
	t1.dwHour = tStart.hour();
	t1.dwMinute = tStart.minute();
	t1.dwSecond = tStart.second();

	t2.dwYear = tEnd.year();
	t2.dwMonth = tEnd.month();
	t2.dwDay = tEnd.day();
	t2.dwHour = tEnd.hour();
	t2.dwMinute = tEnd.minute();
	t2.dwSecond = tEnd.second();

	NET_RECORDFILE_INFO info[2000];
	int nMaxNum;
	int nMaxLen = 2000 * sizeof(NET_RECORDFILE_INFO);

	if ( CLIENT_QueryRecordFile(user_id_,lChannel,0,&t1,&t2,NULL,info,nMaxLen,&nMaxNum) )
	{
		for (int i = 0; i < nMaxNum; i++ )
		{
			Record rd;
			
			rd.m_Start.assign(info[i].starttime.dwYear, info[i].starttime.dwMonth, info[i].starttime.dwDay, info[i].starttime.dwHour, info[i].starttime.dwMinute, info[i].starttime.dwSecond);
			rd.m_End.assign(info[i].endtime.dwYear, info[i].endtime.dwMonth, info[i].endtime.dwDay, info[i].endtime.dwHour, info[i].endtime.dwMinute, info[i].endtime.dwSecond);

			rd.m_sName = info[i].filename;
			rd.m_lSize = info[i].size << 10;
			rd.m_lType = (info[i].nRecordFileType == 0) ? 0 : 1;
			rds.push_back(rd);
		}

		return true;
	}

	m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_QueryRecordFile fail", m_Config.deviceID);

	return false;
}

//请求录像
//lRequestType: 请求类型，0表示停止下载，1表示下载
bool VDDaHuaDVR::RecordRequest(long lChannel,long lRequestType, Record& rd, long& lContext,long& lData, int& pos)
{
	if ( lRequestType == 0 )
	{
		lData = e_download_complete;
		CLIENT_StopDownload(lContext);
	}

	if ( lRequestType == 1 )
	{
		NET_TIME t1,t2;
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

		lContext = CLIENT_DownloadByTime(user_id_, lChannel, 0, &t1, &t2, (char*)rd.m_sName.c_str(),  0, NULL );

		if(lContext == 0)
		{
			m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_DownloadByTime fail", m_Config.deviceID);
			return false;
		}	
	}

	if ( lRequestType == 2 )
	{
		int total_size = 0;
		int download_size = 0;
		if( CLIENT_GetDownloadPos(lContext, &total_size, &download_size))
		{
			if( download_size == total_size )
			{
				//down_pos为上次查询的大小
				if (download_size > pos)
				{
					lData = e_download_more;
					pos = download_size;
				}
				else
					lData = e_download_complete;
				//mlcai 2014-05-07
			}
			else if( download_size < total_size )
			{
				//pos为上次查询的大小
				if (download_size > pos)
				{
					lData = e_download_more;
					pos = download_size;
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
	}
	return true;
}

void VDDaHuaDVR::GetRectForPlace(DH_RECT& rc, WORD nPlace)
{
	rc.left = 8192;
	rc.top = 8192;
	rc.right = 8192;
	rc.bottom = 8192;

	//１－８分别为左上，中上，右上，中左，中右，左下，中下，右下八个位置。

	switch (nPlace)
	{
	case 1: rc.left = 20; rc.top = 32; break;
	case 2: rc.left = 3000; rc.top = 32; break;
	case 3: rc.left = 8000; rc.top = 32; break;
	case 4: rc.left = 20; rc.top = 3000; break;
	case 5: rc.left = 3000; rc.top = 3000; break;
	case 6: rc.left = 20; rc.top = 8000; break;
	case 7: rc.left = 3000; rc.top = 8000; break;
	case 8: rc.left = 8000; rc.top = 8000; break;
	default: rc.left = 20; rc.top = 32; break;
	}

}

void VDDaHuaDVR::GetPlaceForRect(DH_RECT& rc, WORD& nPlace)
{
	if ( rc.top <= 32 )
	{
		if ( rc.left <= 20 )
		{
			nPlace = 1;
		}
		else if ( rc.left <= 3000 )
		{
			nPlace = 2;
		}
		else
		{
			nPlace = 3;
		}
	}
	else if ( rc.top <= 3000 )
	{
		if ( rc.left <= 20 )
		{
			nPlace = 4;
		}
		else
		{
			nPlace = 5;
		}
	}
	else
	{
		if ( rc.left <= 20 )
		{
			nPlace = 6;
		}
		else if ( rc.left <= 3000 )
		{
			nPlace = 7;
		}
		else
		{
			nPlace = 8;
		}
	}

}

bool VDDaHuaDVR::SetOSD(long lChannel,BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,std::string& sWord)
{
	DWORD dwReturned;
	DHDEV_CHANNEL_CFG channel_cfg;
	long i = lChannel;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, i, &channel_cfg, sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取OSD参数失败!", m_Config.deviceID);
		return false;
	}

	DHDEV_CHANNEL_CFG& cfg = channel_cfg;
	memset( cfg.szChannelName, 0, DH_CHAN_NAME_LEN );
	strncpy(cfg.szChannelName,sWord.c_str(),sWord.size());

	cfg.stTimeOSD.bShow = bShowOsd;
	cfg.stChannelOSD.bShow = 1;

	GetRectForPlace(cfg.stTimeOSD.rcRect, nOSDPlace);
	GetRectForPlace(cfg.stChannelOSD.rcRect, nWordPlace);

	if (!(m_dev_info.byDVRType>8))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "不支持该型号设备参数设置");
		return false;
	}

	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &cfg, 
		sizeof(DHDEV_CHANNEL_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,  "DaHuaDVR device id %s CLIENT_GetDevConfig 设置OSD参数失败!", m_Config.deviceID);
		return false;
	}
	return true;
}

bool VDDaHuaDVR::GetOSD(long lChannel,BOOL& bShowOsd,WORD& nOSDPlace,WORD& nWordPlace,std::string& sWord)
{
	DWORD dwReturned = 0;
	DHDEV_CHANNEL_CFG cfg;
	long i = lChannel;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, i, &cfg, sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取OSD参数失败!", m_Config.deviceID);
		return false;
	}
 
	sWord = cfg.szChannelName;

	bShowOsd = cfg.stTimeOSD.bShow;

	GetPlaceForRect(cfg.stTimeOSD.rcRect, nOSDPlace);
	GetPlaceForRect(cfg.stChannelOSD.rcRect, nWordPlace);

	return true; 
}

//帧率
bool VDDaHuaDVR::SetFrameRate(long lChannel,long nFrameRate, long nKeyFrameIntervals,bool bMainStream)
{
	DWORD dwReturned;
	DHDEV_CHANNEL_CFG channel_cfg;
	long i = lChannel;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, i, &channel_cfg, sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取OSD参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT& enc = channel_cfg.stMainVideoEncOpt[0];

	long a, b;
	a = enc.byFramesPerSec;
	b = enc.bIFrameInterval;

	enc.byFramesPerSec = nFrameRate;
	enc.bIFrameInterval = nKeyFrameIntervals;

	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,  "DaHuaDVR device id %s CLIENT_GetDevConfig 设置OSD参数失败!", m_Config.deviceID);
		return false;
	}

	return true; 
}

bool VDDaHuaDVR::GetFrameRate(long lChannel,long& nFrameRate, long& nKeyFrameIntervals,bool bMainStream)
{
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取帧率参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT enc;
	if (bMainStream)
	{
		enc = channel_cfg.stMainVideoEncOpt[0];
	}
	else
	{
		enc = channel_cfg.stAssiVideoEncOpt[0];
	}

	nFrameRate = enc.byFramesPerSec;
	nKeyFrameIntervals = enc.bIFrameInterval ;
	return true; 
}

//图像质量
bool VDDaHuaDVR::SetPictureQuality(long lChannel,long nLevel,bool bMainStream)
{
	DWORD dwReturned;
	DHDEV_CHANNEL_CFG channel_cfg;
	long i = lChannel;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, i, &channel_cfg, sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取OSD参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT& enc = channel_cfg.stMainVideoEncOpt[0];
	long a = enc.byImageQlty;

	switch (nLevel)
	{
	case 0: enc.byImageQlty = 1; break;
	case 1: enc.byImageQlty = 2; break;
	case 2: enc.byImageQlty = 4; break;
	case 3: enc.byImageQlty = 6; break;
	}

	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,  "DaHuaDVR device id %s CLIENT_GetDevConfig 设置OSD参数失败!", m_Config.deviceID);
		return false;
	}
	return true; 
}

bool VDDaHuaDVR::GetPictureQuality(long lChannel,long& nLevel,bool bMainStream)
{
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取质量参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT enc;
	if (bMainStream)
	{
		enc = channel_cfg.stMainVideoEncOpt[0];
	}
	else
	{
		enc = channel_cfg.stAssiVideoEncOpt[0];
	}

	switch (enc.byImageQlty)
	{
	case 1: nLevel = 0; break;
	case 2: nLevel = 1; break;
	case 3: nLevel = 1; break;
	case 4: nLevel = 2; break;
	case 5: nLevel = 2; break;
	case 6: nLevel = 3; break;
	default: nLevel = 0; break;
	}

	return true; 
}

//最大码流
bool VDDaHuaDVR::SetMaxBPS(long lChannel,long nMaxBPS, bool vbr,bool bMainStream)
{
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取码流参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT enc=channel_cfg.stMainVideoEncOpt[0];
	long a = enc.wLimitStream;
	long b = enc.byBitRateControl;

	enc.wLimitStream = nMaxBPS;
	enc.byBitRateControl = (BYTE)vbr;

	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,  "DaHuaDVR device id %s CLIENT_GetDevConfig 设置OSD参数失败!", m_Config.deviceID);
		return false;
	}

	return true; 
}

bool VDDaHuaDVR::GetMaxBPS(long lChannel,long& nMaxBPS, bool& vbr,bool bMainStream)
{	
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取码流参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT enc;
	if (bMainStream)
	{
		enc = channel_cfg.stMainVideoEncOpt[0];
	}
	else
	{
		enc = channel_cfg.stAssiVideoEncOpt[0];
	}

	nMaxBPS = enc.wLimitStream;
	vbr = enc.byBitRateControl == 1;
	return true; 
}

//图像格式
bool VDDaHuaDVR::SetPictureFormat(long lChannel,long nPicFormat, std::string& sSupportFormat,bool bMainStream)
{ 
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取格式参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT& enc = channel_cfg.stMainVideoEncOpt[0];

	long a = enc.byImageSize;

	switch (nPicFormat)
	{
	case 0: enc.byImageSize = 0; break;
	case 1: enc.byImageSize = 5; break;
	case 2: enc.byImageSize = 2; break;
	case 3: enc.byImageSize = 3; break;
	default: enc.byImageSize = 0; break;
	}

	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,  "DaHuaDVR device id %s CLIENT_GetDevConfig 设置OSD参数失败!", m_Config.deviceID);
		return false;
	}

	return true;
}

bool VDDaHuaDVR::GetPictureFormat(long lChannel,long& nPicFormat, std::string& sSupportFormat,bool bMainStream)
{
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取格式参数失败!", m_Config.deviceID);
		return false;
	}

	DH_VIDEOENC_OPT enc;
	if (bMainStream)
	{
		enc = channel_cfg.stMainVideoEncOpt[0];
	}
	else
	{
		enc = channel_cfg.stAssiVideoEncOpt[0];
	}
	switch (enc.byImageSize)
	{
	case 0: nPicFormat = 0; break;
	case 5: nPicFormat = 1; break;
	case 2: nPicFormat = 2; break;
	case 3: nPicFormat = 3; break;
	default: nPicFormat = 0; break;
	}
	return true; 
}

//视频参数
bool VDDaHuaDVR::SetVideoPara(long lChannel,long nBrightness, long nContrast, long nSaturation /*饱和度*/, long nHue/*色调*/)
{ 
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取格式参数失败!", m_Config.deviceID);
		return false;
	}

	DHDEV_CHANNEL_CFG& cfg = channel_cfg;

	cfg.stColorCfg[0].byBrightness = (BYTE)(nBrightness*100/256);
	cfg.stColorCfg[0].byContrast = (BYTE)(nContrast*100/256);
	cfg.stColorCfg[0].bySaturation = (BYTE)(nSaturation*100/256);

	cfg.stColorCfg[1].byBrightness = (BYTE)(nBrightness*100/256);
	cfg.stColorCfg[1].byContrast = (BYTE)(nContrast*100/256);
	cfg.stColorCfg[1].bySaturation = (BYTE)(nSaturation*100/256);

	if ( nHue > 0 )
	{
		cfg.stColorCfg[0].byHue = (BYTE)(nHue*100/256);
		cfg.stColorCfg[1].byHue = (BYTE)(nHue*100/256);
	}

	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,  "DaHuaDVR device id %s CLIENT_GetDevConfig 设置OSD参数失败!", m_Config.deviceID);
		return false;
	}

	return true; 
}


bool VDDaHuaDVR::GetVideoPara(long lChannel,long& nBrightness, long& nContrast, long& nSaturation /*饱和度*/, long& nHue/*色调*/)
{
	DWORD dwReturned = 0;
	//通道配置
	DHDEV_CHANNEL_CFG	channel_cfg;

	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_CHANNELCFG, lChannel, &channel_cfg, 
		sizeof(DHDEV_CHANNEL_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s CLIENT_GetDevConfig 获取图像参数失败!", m_Config.deviceID);
		return false;
	}

	nBrightness = channel_cfg.stColorCfg[0].byBrightness * 256 / 100;
	nContrast = channel_cfg.stColorCfg[0].byContrast * 256 / 100;
	nSaturation = channel_cfg.stColorCfg[0].bySaturation * 256 / 100;
	nHue = channel_cfg.stColorCfg[0].byHue * 256 / 100;

	return true; 
}

BOOL VDDaHuaDVR::ReceiveMessage(LONG lCommand,char* pBuf,LDWORD dwUser)
{
	WORD channelno;
	int i = 0;
	double x = 2;
	switch(lCommand) 
	{
	case DH_COMM_ALARM:			//General alarm information 
		{
			NET_CLIENT_STATE *pState = (NET_CLIENT_STATE *)pBuf;
			if(pState == NULL)
				return FALSE;
			int ChannelCount = pState->channelcount;
			int AlarmInputCount = pState->alarminputcount;
			//External alarm 
			for(i=0;i<ChannelCount;i++)
			{
				if(1 == pState->alarm[i])
				{
					WORD wAlarmChannel = (WORD)pow(x,i);
					channelno= channelno | wAlarmChannel;
				}
			}
			channelno=0;
			//Motion detection 
			for(i=0;i<ChannelCount;i++)
			{
				if(1 == pState->motiondection[i])
				{
					WORD wAlarmChannel = (WORD)pow(x,i);
					channelno=channelno | wAlarmChannel;
				}

			}
		}
		break;
	case DH_SHELTER_ALARM:		//Camera masking alarm 
		break;
	case DH_DISK_FULL_ALARM:	//HDD full alarm 
		break;
	case DH_DISK_ERROR_ALARM:	//HDD malfunction alarm 
		break;
	case DH_SOUND_DETECT_ALARM:		//Audio detection alarm 
		break;
	case DH_ALARM_DECODER_ALARM:	//Alarm decoder alarm 
		break;
	default:
		break;
	}




	return TRUE;
}

BOOL VDDaHuaDVR::ReceiveMessageEx(LONG lCommand,char* pBuf,LDWORD dwUser)
{
	try
	{
		long channel=-1;
		long alarm_type=-1;
		string strAlarmType="";
		switch(lCommand)
		{
		case DH_ALARM_ALARM_EX:
		case DH_MOTION_ALARM_EX:
			{
				
				alarm_type=(lCommand==DH_ALARM_ALARM_EX)?alarm_in:alarm_camera;
				
				strAlarmType=(alarm_type==alarm_in)?"alarm_in":"alarm_camera";
				for ( int i = 0; i < DH_MAX_VIDEO_IN_NUM; i++ )
				{
					if ( pBuf[i] == 1 )
					{
						channel=i;
						if(m_pCallBack->IsExistsIp(m_Config.serverIP))
							m_Config.vp_log->log(Message::PRIO_DEBUG,"DaHuaDVR %s 报警:%ld，报警类型:%s ",m_Config.serverIP,channel+1,strAlarmType);

						m_pCallBack->NotifyAlarm(channel,alarm_type);
					}
				}
			}	
			break;
		case DH_SHELTER_ALARM_EX:
		case DH_VIDEOLOST_ALARM_EX:
		case DH_DISKFULL_ALARM_EX:
		case DH_DISKERROR_ALARM_EX:
		case DH_ENCODER_ALARM_EX:
			return FALSE;
		default:
			return FALSE;
		}

		
		return TRUE;

	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL VDDaHuaDVR::MessageNotify(LONG lCommand, char *pBuf,LDWORD dwUser)
{
	if(m_bNewVer)
		return ReceiveMessageEx(lCommand,pBuf,dwUser);
	else
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, " #############################DaHuaDVR %s #############################！",m_Config.serverIP);
		return ReceiveMessage(lCommand,pBuf,dwUser);
	}
}

//报警回调函数
BOOL CALLBACK MessCallBack( LONG lCommand, LLONG lLoginID, char* pBuf, DWORD dwBufLen, char* pchDVRIP, LONG nDVRPort, LDWORD dwUser )
{
	try{
		string sip = (char*)dwUser;
		VDDaHuaDVR * p;
		{	
			Poco::FastMutex::ScopedLock locker(g_sdkLock);
			hash_map<string, VDDaHuaDVR*>::iterator pos = g_VPDaHua.find( sip );
			if( sip.empty() || pos == g_VPDaHua.end() )
			{
				return false;
			}

			p = pos->second;
			if( p == NULL )
			{
				return false;
			}
		}

		p->MessageNotify( lCommand, pBuf, dwUser);

		return true;
	}
	catch(...)
	{

	}
}


//建立报警上传通道(布防)
bool VDDaHuaDVR::SetupAlarmChannel()
{
	CLIENT_SetDVRMessCallBack( MessCallBack,(DWORD)_key.c_str());
	if(m_bNewVer)
		CLIENT_StartListenEx( user_id_ );
	else
		CLIENT_StartListen(user_id_);
	
	return true;
}

//关闭报警上传通道（撤防）
//暂时需求不考虑撤防
void VDDaHuaDVR::CloseAlarmChannel()
{

}

//报警状态
bool VDDaHuaDVR::SetAlarmState(long lChannel, long lType, long lState)
{
	//HxTrace("SetAlarmState Channel:%d Type:%d State:%d",lChannel,lType,lState);
	//报警参数
	DHDEV_ALARM_SCHEDULE alarm_cfg;
	DWORD dwReturned = 0;

	if (!CLIENT_GetDevConfig(user_id_, DH_DEV_ALARMCFG, 0xFFFFFFFF, &alarm_cfg, sizeof(alarm_cfg), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR  device id %s CLIENT_GetDevConfig 失败！", m_Config.deviceID);
	}

	if ( lType == alarm_camera )
	{
		
		return false;
	}

	if ( lType == alarm_in)
	{
		return false;
	}

	ALARM_CONTROL ac;
	ac.index = lChannel;
	ac.state = lState;
	DH_IOTYPE dType = DH_ALARMOUTPUT;
	if ( CLIENT_IOControl(user_id_, dType, &ac, sizeof(ac)) )
	{
		return true;
	}

	m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR device id %s 设置报警输出状态失败! 失败码:%ul", m_Config.deviceID, CLIENT_GetLastError());

	return false; 
}

bool VDDaHuaDVR::GetAlarmState(long lChannel, long lType, long& lState)
{
	int lNum = 0;
	DH_IOTYPE dType = (lType == alarm_out) ? DH_ALARMOUTPUT :DH_ALARMINPUT;

	if ( !CLIENT_QueryIOControlState(user_id_,dType,NULL,0,&lNum) )
	{
		m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR  device id %s CLIENT_QueryIOControlState 失败！", m_Config.deviceID);
		return false;
	}

	ALARM_CONTROL aState[DH_MAX_ALARMOUT_NUM];

	if ( CLIENT_QueryIOControlState(user_id_,dType,&aState,sizeof(ALARM_CONTROL)*lNum,&lNum) )
	{
		lState = aState[lChannel].state;
		return true;
	}

	m_Config.vp_log->log(Message::PRIO_ERROR, "DaHuaDVR  device id %s CLIENT_QueryIOControlState 失败！", m_Config.deviceID);

	return false; 
}

//add by sunminping 从HXDVS上面拷贝过来的
//////////////////////////////////////////////////////////////////////////
//侦测区域
bool VDDaHuaDVR::SetDetectRect(long lChannel,std::vector<Rect>& Rects, BOOL bDectecting, long nDetectGrade, long nMaxRectNum)
{
	DWORD dwReturned;
	DHDEV_ALARM_SCHEDULE alarm_cfg;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_ALARMCFG, 0xFFFFFFFF, &alarm_cfg, sizeof(alarm_cfg), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,"%s 获取设备报警参数失败!", m_Config.serverIP);
		return false;
	}

	//区域遮挡配置
	DHDEV_VIDEOCOVER_CFG cover_cfg;
	if (!CLIENT_GetDevConfig(user_id_, DH_DEV_VIDEO_COVER,lChannel, &cover_cfg,
		sizeof(DHDEV_VIDEOCOVER_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,"%s 视频通道%d 获取区域遮挡参数失败!", m_Config.serverIP,lChannel);
		return false;
	}

	DH_MOTION_DETECT_CFG& cfg=alarm_cfg.struMotion[lChannel];
	cfg.byMotionEn = bDectecting;
	cfg.wSenseLevel = nDetectGrade;

	for (int i=0;i<DH_MOTION_ROW;i++)
	{
		for (int j=0;j<DH_MOTION_COL;j++)
		{
			cfg.byDetected[i][j] = 0;
		}
	}

	for (int i=0;i<(int)Rects.size();i++)
	{
		int col_start=Rects[i].Left * 32 / 288;
		int col_end=Rects[i].Right * 32 / 288;
		int row_start=Rects[i].Top * 32 / 352 ;
		int row_end=Rects[i].Bottom * 32 / 352;

		for (int j=row_start;j<row_end;j++)
		{
			for (int k=col_start;k<col_end;k++)
			{
				cfg.byDetected[j][k] = 1;
			}
		}
	}

	if(CLIENT_SetDevConfig(user_id_,DH_DEV_ALARMCFG,0xFFFFFFFF,&alarm_cfg,sizeof(DHDEV_ALARM_SCHEDULE)))
		return true;
	return false;
}

bool VDDaHuaDVR::GetDetectRect(long lChannel,std::vector<Rect>& Rects, BOOL& bDectecting, long& nDetectGrade, long& nMaxRectNum)
{
	return false;
}

void VDDaHuaDVR::GetDHRect(DH_RECT& rc1, Rect& rc2)
{
	rc1.left = rc2.Left * 8192 / 352;
	rc1.top = rc2.Top * 8192 / 288;
	rc1.right = rc2.Right * 8192 / 352;
	rc1.bottom = rc2.Bottom * 8192 / 288;
}

bool VDDaHuaDVR::SetShadeRect(long lChannel,std::vector<Rect>& Rects, BOOL bLogoing, long nMaxRectNum)
{
	DWORD dwReturned;
	DHDEV_VIDEOCOVER_CFG cover_cfg;
	long i = lChannel;
	if (!CLIENT_GetDevConfig(user_id_, DH_DEV_VIDEO_COVER,lChannel, &cover_cfg,
		sizeof(DHDEV_VIDEOCOVER_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,"%s 视频通道%d 获取区域遮挡参数失败!", m_Config.serverIP,lChannel);
		return false;
	}

	DHDEV_ALARM_SCHEDULE alarm_cfg;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_ALARMCFG, 0xFFFFFFFF, &alarm_cfg, sizeof(alarm_cfg), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,"%s 获取设备报警参数失败!", m_Config.serverIP);
		return false;
	}

	long lNum = 0;
	if ( Rects.size() > 0 )
	{
		for ( int i = 0; i < Rects.size(); i++ )
		{
			if ( i >=  DH_MAX_VIDEO_COVER_NUM)
			{
				break;
			}
			GetDHRect(cover_cfg.CoverBlock[i].rcBlock, Rects[i]);
			m_Config.vp_log->log(Message::PRIO_NOTICE,"rect %d %d %d %d", cover_cfg.CoverBlock[i].rcBlock.left, cover_cfg.CoverBlock[i].rcBlock.top,
				cover_cfg.CoverBlock[i].rcBlock.right, cover_cfg.CoverBlock[i].rcBlock.bottom);
			cover_cfg.CoverBlock[i].bBlockType = 0;
			cover_cfg.CoverBlock[i].bEncode = 1;
			cover_cfg.CoverBlock[i].bPriview = 1;
			cover_cfg.CoverBlock[i].nColor = 0;
			lNum++;
		}
		cover_cfg.bCoverCount = lNum;
	}
	else
	{
		cover_cfg.bCoverCount = 0;
	}

	if(!(m_dev_info.byDVRType> 8))
	{
		m_Config.vp_log->log(Message::PRIO_NOTICE,"不支持该型号设备参数设置");
		return false;
	}
	
	if(!CLIENT_SetDevConfig(user_id_, DH_DEV_VIDEO_COVER, lChannel, &cover_cfg,
		sizeof(DHDEV_VIDEOCOVER_CFG)))
	{
		m_Config.vp_log->log(Message::PRIO_NOTICE,"%s 设置遮挡区域失败",m_Config.serverIP);
		return false;
	}
	return true; 

}

void VDDaHuaDVR::GetHXRect(DH_RECT& rc1,Rect& rc2)
{
	rc2.Left = rc1.left * 352 / 8192;
	rc2.Top = rc1.top * 288 / 8192;
	rc2.Right = rc1.right * 352 / 8192;
	rc2.Bottom = rc1.bottom * 288 / 8192;
}

bool VDDaHuaDVR::GetShadeRect(long lChannel,std::vector<Rect>& Rects, BOOL& bLogoing, long& nMaxRectNum)
{
	DWORD dwReturned;
	DHDEV_VIDEOCOVER_CFG cover_cfg;
	long i = lChannel;
	if (!CLIENT_GetDevConfig(user_id_, DH_DEV_VIDEO_COVER,lChannel, &cover_cfg,
		sizeof(DHDEV_VIDEOCOVER_CFG), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,"%s 视频通道%d 获取区域遮挡参数失败!", m_Config.serverIP,lChannel);
		return false;
	}

	DHDEV_ALARM_SCHEDULE alarm_cfg;
	if(!CLIENT_GetDevConfig(user_id_, DH_DEV_ALARMCFG, 0xFFFFFFFF, &alarm_cfg, sizeof(alarm_cfg), &dwReturned))
	{
		m_Config.vp_log->log(Message::PRIO_ERROR,"%s 获取设备报警参数失败!", m_Config.serverIP);
		return false;
	}

	if (cover_cfg.bCoverCount == 0)
	{
		bLogoing = 0;
	}
	else
	{
		bLogoing = 1;
		Rects.clear();
		for (int i = 0; i < cover_cfg.bCoverCount; i++)
		{
			Rect rc;
			GetHXRect(cover_cfg.CoverBlock[i].rcBlock, rc);
			m_Config.vp_log->log(Message::PRIO_NOTICE,"rect %d %d %d %d", cover_cfg.CoverBlock[i].rcBlock.left, cover_cfg.CoverBlock[i].rcBlock.top,
				cover_cfg.CoverBlock[i].rcBlock.right, cover_cfg.CoverBlock[i].rcBlock.bottom);
			Rects.push_back(rc);
		}
	}
	nMaxRectNum = DH_MAX_VIDEO_COVER_NUM;
	return true; 
}

//实现VD工厂
VDDaHuaDVRMaker::VDDaHuaDVRMaker()
{
}

VDDaHuaDVRMaker::~VDDaHuaDVRMaker()
{
}

VirtualDevice* VDDaHuaDVRMaker::Make()
{
	return new VDDaHuaDVR;
}

void VDDaHuaDVRMaker::Reclaim(VirtualDevice* vd)
{
	delete vd;
}

extern "C" __declspec(dllexport) void RegisterVDMaker(VirtualDeviceFactory* vd_factory)
{
	//factory->AddVPlatformCreator("RTSP", new VPRtspCreator);
	vd_factory->AddVirtualDeviceMaker("DaHuaDVR", new VDDaHuaDVRMaker);
}
