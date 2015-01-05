
#include "VDeviceRecordTask.h"
#include "windows.h"

VDeviceRecordTask::VDeviceRecordTask(VDeviceServer* pVPServer,string key, const CmdHead& reques_head, VirtualDevice* pVPlatform,char *pBuf,int channel)
{
	isStop = false;
	pVPlatformRecord = pVPlatform;
	_key=key;
	pVPlatformServer=pVPServer;
	pBuffer = pBuf;
	lChannel = channel;
	lSeq=0;
	memcpy(&request_head_, &reques_head, sizeof(CmdHead));
}

VDeviceRecordTask::~VDeviceRecordTask()
{
	ReleaseVPlatform();
}

bool VDeviceRecordTask::NotifyRecordData(long lRecordType, int success, PBYTE pBuffer, DWORD dwSize)
{
	bool result = true;

	//cmd header
	CmdHead hi;
	hi.cmdseq = lSeq++;
	hi.success = success; 

	char* m_Frame;

	hi.length = dwSize;
	m_Frame = new char[sizeof(hi) + dwSize];
	memcpy(m_Frame, &hi, sizeof(hi));
	if (dwSize)
		memcpy(m_Frame + sizeof(hi), pBuffer, dwSize);
	
	if ( lRecordType == record_header )
	{
		hi.cmdnum = CMD_DEVRECORDSTREAM_RESP;
	}
	else if ( lRecordType == record_stream )
	{
		hi.cmdnum = CMD_HISTORY_STREAMING; 
	}
	else
	{
		hi.cmdnum = CMD_HISTORY_STREAMING_END;
	}
	

	if(NetUtility::SafeSendData(socket, m_Frame, sizeof(hi) + hi.length)<0)
		result = false;
	delete[] m_Frame;
	return result;
}

int VDeviceRecordTask::AddVPlatform(VDeviceConfig& rConfig)
{
	try
	{
		if (pVPlatformRecord!=NULL)
		{
			return 0;
		}

		//CreateMegaObject("VirtualDevice", rConfig.m_sClassName.c_str(), (void**)&pVPlatformRecord);

		if (pVPlatformRecord==NULL)
		{
			return -1;
		}

		VDeviceInfo info;
		info.deviceID = rConfig.m_sDeviceId;
		info.serverIP = rConfig.m_sServerIP;
		info.port = rConfig.m_lPort;
		info.user = rConfig.m_sUser;
		info.passwd = rConfig.m_sPasswd;
		info.extendPara1=rConfig.m_sExtendPara1;
		info.deviceType = rConfig.m_sClassName;  //ligc++ 2014-06-23 用于解决宇视NVR录像下载扩展名必须为.ts问题
		info.vp_log = &LogSys::getLogSys(LogInfo::Instance().VirtualDevice());

		pVPlatformRecord->BaseInit(NULL,info);

		return 0;
	}
	catch (...)
	{
		ReleaseVPlatform();
		return -1;	
	}	
}

void VDeviceRecordTask::ReleaseVPlatform()
{
	if(pVPlatformRecord!=NULL)
	{
		try
		{
			pVPlatformRecord->Destroy();
			//ReleaseMegaObject(pVPlatformRecord);
			pVPlatformRecord=NULL;
		}
		catch(...)
		{
			pVPlatformRecord=NULL;
		}

	}
}

int VDeviceRecordTask::InitVPlatform(VDeviceConfig& rConfig)
{
	if (pVPlatformRecord==NULL)
	{
		return false;
	}

	pVPlatformRecord->Destroy();

	return pVPlatformRecord->Init(rConfig);
}

bool VDeviceRecordTask::NewVPlatform(VDeviceConfig& rConfig)
{
	AddVPlatform(rConfig);
	InitVPlatform(rConfig);
	return true;
}

//返回:3,表示需要继续收取视频文件头
//返回:2,表示发送视频头失败
//返回:1,表示发送视频数据失败
//返回:0,成功
int VDeviceRecordTask::doSuccess(BYTE* buf,DWORD nLen,DWORD nHeadLen,BOOL& bFirst)
{
	if(bFirst)
	{
		if(nLen<nHeadLen)
			return 3;
		else
		{
			CMarkup res_xml;
			res_xml.AddElem("Information");
			res_xml.AddElem("Protocol",(char*)buf);
			if(!NotifyRecordData(record_header, 0, (PBYTE)res_xml.GetDoc().c_str(), res_xml.GetDoc().size()))
				return 2;
			if(!NotifyRecordData(record_stream, 0, buf+nHeadLen, nLen-nHeadLen))
				return 1;
		}
		bFirst=FALSE;
	}
	else
	{
		if(!NotifyRecordData(record_stream,0, buf,nLen))
			return 1;		
	}
	return 0;
}
//add end 

bool VDeviceRecordTask::doRun()
{
	CMarkup xml;
	StrUtility utility;
	string start_td;
	string end_td;
	if (!(xml.SetDoc(pBuffer) && utility.ForeachXml(xml) 
		&& utility.GetxmlValue("Information/StartTD", start_td) && utility.GetxmlValue("Information/EndTD", end_td)))
		return false;

	int tzd;
	Poco::LocalDateTime tStart = Poco::DateTimeParser::parse(Poco::DateTimeFormat::SORTABLE_FORMAT, start_td, tzd);
	Poco::LocalDateTime tEnd = Poco::DateTimeParser::parse(Poco::DateTimeFormat::SORTABLE_FORMAT, end_td, tzd);
	Record rd;
	rd.m_Start = tStart;
	rd.m_End = tEnd;

	char sFileName[128];
	memset(sFileName,0,128);
    sprintf(sFileName,"C:\\%d_%p_tmp.%s", GetTickCount(), this,file_extend);
	rd.m_sName = sFileName;

	long _record_context = -1;
	long _record_state = 0;

	BOOL bFirst = TRUE;
	int tryNum = 0;

	BYTE buf[READ_RECORD_BUFFER];
	long lOffset = 0;
	memset(buf,'\0',READ_RECORD_BUFFER);
	DWORD dwRead = 0;
	m_hReadFile = NULL;
	int pos = 0;

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
	
	bool bReq=pVPlatformRecord->RecordRequest(lChannel,1,rd,_record_context,_record_state,pos);

	CMarkup res_xml;
	if(!bReq) 
	{
		res_xml.AddElem("Information");
		res_xml.AddElem("ErrDescription", Poco::format("请求到设备%s:%d的历史视频失败", pVPlatformRecord->m_Config.deviceID, lChannel));
		if(!NotifyRecordData(record_header, 254, (PBYTE)res_xml.GetDoc().c_str(), res_xml.GetDoc().size()))
		{
			media_log.log(Message::PRIO_ERROR,"请求录像文件发送10017数据到客户端失败!");
			return false;
		}
		media_log.log(Message::PRIO_ERROR, "请求到设备%s:%d的历史视频失败", pVPlatformRecord->m_Config.deviceID, lChannel);
		return false;
	}

	OVERLAPPED overlap;
	DWORD dwFileOffset=0;
	memset(&overlap, 0, sizeof(overlap));
    if(pVPlatformRecord->m_Config.deviceType == "VPIMOS")//ligc++ 2014-06-23宇视NVR创建文件太慢，随眠3秒等待文件准备好。
        Sleep(3000);

	m_hReadFile = ::CreateFileA( rd.m_sName.c_str(),
		GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL, OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);

	if( m_hReadFile == INVALID_HANDLE_VALUE )
	{
		media_log.log(Message::PRIO_ERROR, "请求到设备%s:%d的历史视频打开录像文件失败!", pVPlatformRecord->m_Config.deviceID, lChannel);
		pVPlatformRecord->RecordRequest(lChannel, 0, rd, _record_context, _record_state, pos); //停止前端下载
		res_xml.AddElem("Information");
		res_xml.AddElem("ErrDescription", Poco::format("请求到设备%s:%d的历史视频失败", pVPlatformRecord->m_Config.deviceID, lChannel));
		if(!NotifyRecordData(record_header, 254, (PBYTE)res_xml.GetDoc().c_str(), res_xml.GetDoc().size()))
		{
			media_log.log(Message::PRIO_ERROR,"请求录像文件发送10017数据到客户端失败!");
			return false;
		}
		media_log.log(Message::PRIO_ERROR, "请求到设备%s:%d的历史视频失败", pVPlatformRecord->m_Config.deviceID, lChannel);
		return false;
	}
	
	bFirst = TRUE;

	//获取录像视频文件头
	long nHeaderLen=0;
	string strHeader;
	if(!pVPlatformRecord->GetRecordHeader(lChannel,nHeaderLen,strHeader))
	{
		media_log.log(Message::PRIO_DEBUG, "获取录历史视频头长度失败!");
		pVPlatformRecord->RecordRequest(lChannel, 0, rd, _record_context, _record_state, pos); //停止前端下载
		return false;
	}

	if(nHeaderLen<=0)     //构造的录像文件视频头
	{
		res_xml.AddElem("Information");
		res_xml.AddElem("Protocol", strHeader);
		if(!NotifyRecordData(record_header, 0, (PBYTE)res_xml.GetDoc().c_str(), res_xml.GetDoc().size()))
		{
			media_log.log(Message::PRIO_DEBUG,"发送录像文件视频头到客户端失败!");
			pVPlatformRecord->RecordRequest(lChannel, 0, rd, _record_context, _record_state, pos); //停止前端下载
			return false;
		}
		bFirst=FALSE;
	}

	HANDLE hEvent=CreateEventA(NULL,TRUE,FALSE,NULL);
	if(hEvent==INVALID_HANDLE_VALUE)
	{
		media_log.log(Message::PRIO_DEBUG,"创建重叠IO需要的事件失败!");
		pVPlatformRecord->RecordRequest(lChannel, 0, rd, _record_context, _record_state, pos); //停止前端下载
		return false;
	}

	int nDataLen=0;
	int nRet=0;
	bool bRet=true;

	//请求视频数据.
	do 
	{
		overlap.hEvent=hEvent;
		overlap.OffsetHigh=0;
		overlap.Offset=dwFileOffset;
		bReq=pVPlatformRecord->RecordRequest(lChannel, 2, rd, _record_context, _record_state, pos);
		ReadFile(m_hReadFile, buf + lOffset, READ_RECORD_BUFFER-lOffset, &dwRead, &overlap);
		DWORD dwWaitRet=WaitForSingleObject(overlap.hEvent,250);
		GetOverlappedResult(m_hReadFile,&overlap,&dwRead,FALSE);
		DWORD err=GetLastError();
		if(err==ERROR_HANDLE_EOF)  //win7 退出
		{
			++tryNum;
			if(tryNum>5 || _record_state==e_download_complete || _record_state==e_download_error)
			{
				if(_record_state==e_download_complete)
					media_log.log(Message::PRIO_DEBUG,"前端(ip:%s,通道:%s)下载录像返回e_download_complete",pVPlatformRecord->m_Config.serverIP,lChannel);
				else if(_record_state==e_download_error)
					media_log.log(Message::PRIO_DEBUG,"前端(ip:%s,通道:%s)下载录像返回e_download_error",pVPlatformRecord->m_Config.serverIP,lChannel);
				else
					media_log.log(Message::PRIO_DEBUG,"前端(ip:%s,通道:%s)失败次数大于5",pVPlatformRecord->m_Config.serverIP,lChannel);
				break;                          //这里是正常退出流程
			}
			Sleep(250);
			continue;
		}

		if(dwWaitRet==WAIT_OBJECT_0)    
		{
			nDataLen=bFirst?lOffset+dwRead:dwRead;
			nRet=doSuccess(buf,nDataLen,nHeaderLen,bFirst);
			if(1==nRet)
			{
				media_log.log(Message::PRIO_DEBUG,"发送录像文件视频数据到客户端失败!");
				bRet=false;
				break;
			}
			else if(2==nRet)
			{
				media_log.log(Message::PRIO_DEBUG,"发送录像文件视频头到客户端失败!");
				bRet=false;
				break;
			}
			else if(3==nRet)
			{
				lOffset+=dwRead;
				dwFileOffset+=dwRead;
				continue;
			}
			dwFileOffset+=dwRead;
			dwRead=0;
			tryNum=0;
		}
		else if(dwWaitRet==WAIT_TIMEOUT)     //xp和server2003 退出
		{
			++tryNum;
			if(tryNum>5 || _record_state==e_download_complete || _record_state==e_download_error)
			{
				if(_record_state==e_download_complete)
					media_log.log(Message::PRIO_DEBUG,"line %d 前端(IP:%s,通道:%s)下载录像返回e_download_complete",__LINE__,pVPlatformRecord->m_Config.serverIP,lChannel);
				else if(_record_state==e_download_error)
					media_log.log(Message::PRIO_DEBUG,"line %d 前端(IP:%s,通道:%s)下载录像返回e_download_error",__LINE__,pVPlatformRecord->m_Config.serverIP,lChannel);
				else
					media_log.log(Message::PRIO_DEBUG,"line %d 前端(IP:%s,通道:%s)失败次数大于5",__LINE__,pVPlatformRecord->m_Config.serverIP,lChannel);
				break;                 //这里是正常退出流程
			}
			Sleep(250);
			continue;
		}

		// 解决录像下载0kb的问题
		Poco::Timespan  timeout(0,150);                                //150微秒超时机制    
		if(socket.poll(timeout,1))
		{
			MegaeyesRequestHead head;
			if(!NetUtility::Receive(socket,(char*)&head,sizeof(MegaeyesRequestHead))) //对于心跳数据不做处理
				break;

			char pBufferTmp[200];
			if(!NetUtility::Receive(socket,(char*)pBufferTmp,head.length))   
				break;
		}
	} while (bReq && !isStop);

	CloseHandle(hEvent);
	NotifyRecordData(record_end,0, NULL,0);
	pVPlatformRecord->RecordRequest(lChannel, 0, rd, _record_context, _record_state, pos);

	//删除文件
	CloseHandle(m_hReadFile);
	m_hReadFile = INVALID_HANDLE_VALUE;
	::DeleteFileA(rd.m_sName.c_str());

	return bRet;
}


void VDeviceRecordTask::Stop()
{
	isStop=true;
}


void VDeviceRecordTask::run()
{   
	//modify by sunminping 2014-6-30
	do 
	{
		if(!doRun())                                  //下载单个文件!
		{
			isStop=true;
			break;
		}
		int nHeatBeat=0;
		char pBufferTmp[2000];
		while(!isStop)
		{
			MegaeyesRequestHead head;
			if(!NetUtility::Receive(socket,(char*)&head,80))   
			{
				isStop=true;
				break;
			}

			if(!NetUtility::Receive(socket,(char*)pBufferTmp,head.length))   //接收8302的数据
			{
				isStop=true;
				break;
			}

			if(0==head.cmdnum)
			{
				++nHeatBeat;
				if(nHeatBeat>4)
				{
					isStop=true;
					break;
				}
			}
			else if(head.cmdnum==8302)
			{
				pBuffer=pBufferTmp;
				break;
			}
		}
	} while (!isStop);
	//end modify


	LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
	transLog.log(Message::PRIO_DEBUG,"%s 录像下载完毕,已经解锁!",_key);

	socket.close();
	//end add

	//删除自身
	delete this;
}

void VDeviceRecordTask::Start()
{
	isStop=false;

	work_thread_.start(*this);
	//add by sunminping 
	if(!work_thread_.isRunning())       //线程启动失败
	{
		LogSys& transLog = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());
		transLog.log(Message::PRIO_DEBUG,"%s 录像下载线程启动失败！",_key);
		pVPlatformServer->m_RecdTaskMutex.lock();
		map<string,VDeviceRecordTask*>::iterator it=pVPlatformServer->VPRecordTasks.find(_key);
		if(it!=pVPlatformServer->VPRecordTasks.end())
		{
			pVPlatformServer->VPRecordTasks.erase(it);
			transLog.log(Message::PRIO_DEBUG,"%s 录像下载完毕,已经解锁!",_key);
		}
		pVPlatformServer->m_RecdTaskMutex.unlock();

		delete this;
	}
	//end add
}

