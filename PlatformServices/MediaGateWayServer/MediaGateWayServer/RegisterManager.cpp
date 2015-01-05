#include "stdafx.h"
#include "StrUtility.h"
#include "RegisterManager.h"

#include "CMD10004RegReqSerialize.h"
#include "CMD10006AbiPostReqSerialize.h"
#include "Megaeyes4002ReqSerialize.h"
#include "Megaeyes2003ResSerialize.h"

using Poco::Path;
using Poco::Environment;

RegisterManager::RegisterManager()
{
	is_stop_=false;
}

RegisterManager::~RegisterManager()
{
	is_stop_=true;
	workthread.join();
}

void RegisterManager::Start()
{
    workthread.start(*this);
}

void RegisterManager::run()
{
/*	Sleep(50);*/
    while(!is_stop_)
    {
        for(int i=0;i<(int)deviceConfigsCopy.size();i++)
        {
            int nRet = VDeviceManager::Instance().AddVDeviceServer(deviceConfigsCopy[i]);
            if(nRet == -1)
            {
                continue;
            }

            nRet = VDeviceManager::Instance().InitVDeviceServer(deviceConfigsCopy[i]);
            if(nRet == -1  || nRet == -2)
            {   
                 continue;
            }

            CMD10004RegReqSerialize deviceRequestRegister;
            deviceRequestRegister.m_strDeviceID = deviceConfigsCopy[i].m_sDeviceId;
            deviceRequestRegister.m_nVideoPort= deviceConfigsCopy[i].m_lPort;
            deviceRequestRegister.m_nAudioPort= CmdServerModule::Instance().m_config.startPort+2;
 
            string content =  deviceRequestRegister.Serialize();

			deviceConfigsCopy[i].status = VDeviceConfig::False;
			
            CmdServerModule::Instance().SendToCmdServer((char*)content.c_str(), content.size(), deviceRequestRegister.m_cmdHead.cmdseq, new CmdResponseProcessor<RegisterManager, int>(this, &RegisterManager::OnRegisterResponse, i));
        }
		
		Thread::sleep(2000);

		CmdServerModule::Instance().Notify(deviceConfigsCopy);
    }
}

void RegisterManager::OnRegisterResponse(const CmdHead& head, char* res_xml, int index)
{
	CMarkup mark_xml;
	StrUtility utility;
	string code;

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	if (!(mark_xml.SetDoc(res_xml) && utility.ForeachXml(mark_xml) && utility.GetxmlValue("Information_code", code)))
	{
		media_log.log(Message::PRIO_ERROR, "设备:%s注册返回xml格式错误",  deviceConfigsCopy[index].m_sDeviceId);
		return;
	}

	if (code != "000000")
	{
		string descript;
		utility.GetxmlValue("Information_descript", descript);
		media_log.log(Message::PRIO_ERROR, "设备:%s注册返回错误码[%s]和错误信息%s",  deviceConfigsCopy[index].m_sDeviceId, code, descript);
		return;
	}

	string ability, hearcycle;
	utility.GetxmlValue("Information/Ability", ability);
	utility.GetxmlValue("Information/HeartCycle", hearcycle);

	if (ability == "true")
	{
		CMD10006AbiPostReqSerialize ability_post;
		ability_post.MediaChannls = deviceConfigsCopy[index].MediaChannls;
		ability_post.max_in_ = deviceConfigsCopy[index].max_in_;
		ability_post.max_out_ = deviceConfigsCopy[index].max_out_;
		string content = ability_post.Serialize();
		CmdServerModule::Instance().SendToCmdServer((char*)content.c_str(), content.size(), ability_post.m_cmdHead.cmdseq, new CmdResponseProcessor<RegisterManager, int>(this, &RegisterManager::OnAbilityPostResponse, index));
	}

	deviceConfigsCopy[index].status = VDeviceConfig::Success;
	VDeviceManager::Instance().ListenAlarmInfo(deviceConfigsCopy[index].m_sDeviceId);
}

void RegisterManager::OnAbilityPostResponse(const CmdHead& head, char* res_xml, int index)
{
	CMarkup mark_xml;
	StrUtility utility;
	string success;

	LogSys& media_log = LogSys::getLogSys(LogInfo::Instance().MediaGateWayServer());

	if (!(mark_xml.SetDoc(res_xml) && utility.ForeachXml(mark_xml) && utility.GetxmlValue("Information/Success", success)))
	{
		media_log.log(Message::PRIO_ERROR, "设备:%s能力上报返回xml格式错误",  deviceConfigsCopy[index].m_sDeviceId);
		return;
	}

	if (success != "0")
		media_log.log(Message::PRIO_ERROR, "设备:%s能力上报返回错误[%s]",  deviceConfigsCopy[index].m_sDeviceId, success);
}