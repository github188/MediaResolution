#ifndef MEGAEYES_2003RES_SERIALIZE_H
#define MEGAEYES_2003RES_SERIALIZE_H

#include "MegaeyesResponseSerialize.h"
#include "MegaeyesException.h"
/**********************Éè±¸×¢²á2003************************************/
/*
<Message ID="0000000000200000000000000350001" Naming="0000000000200000000000000350001:25.30.5.191:010001" HeartCycle="120" SysRebootTime="10:30" SysRebootCycle="1" DispatchId="" CurrentDateTime="022614342014.37">
<Camera DeviceID="0000000000200000000000000380000" ID="0000000000200000000000000380000" Naming="0000000000200000000000000380000:0000000000200000000000000350001:25.30.5.191:010001" ChannelId="1" HasPan="false" IsIPCamera="false" DomainName="" Port="0" Number="">
<StoreScheme IsLocaleSaved="true" LocalDiskFullOption="Overlay" StorePlanHourFlag="000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" SchemeCycle="week" StoreCycle="0" StreamType="MainStream" />
</Camera>
<Camera DeviceID="0000000000200000000000000380001" ID="0000000000200000000000000380001" Naming="0000000000200000000000000380001:0000000000200000000000000350001:25.30.5.191:010001" ChannelId="2" HasPan="false" IsIPCamera="false" DomainName="" Port="0" Number="">
<StoreScheme IsLocaleSaved="true" LocalDiskFullOption="Overlay" StorePlanHourFlag="000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" SchemeCycle="week" StoreCycle="0" StreamType="MainStream" />
</Camera>
<Camera DeviceID="0000000000200000000000000380002" ID="0000000000200000000000000380002" Naming="0000000000200000000000000380002:0000000000200000000000000350001:25.30.5.191:010001" ChannelId="3" HasPan="false" IsIPCamera="false" DomainName="" Port="0" Number="">
<StoreScheme IsLocaleSaved="true" LocalDiskFullOption="Overlay" StorePlanHourFlag="000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" SchemeCycle="week" StoreCycle="0" StreamType="MainStream" />
</Camera>
<Success>0</Success>
</Message>
*/
/************************************************************************/


class CCamera
{
public:
    CCamera()
    {
        Clear();
    }
    ~CCamera()
    {

    }
    void Clear()
    {
        m_strDeviceId="";
        m_strId="";
        m_strNaming="";
        m_nChannelId=0;
        m_bHasPan=false;
        m_bIsIPCamera=false;
        m_strDomainName="";
        m_nPort=0;
        m_nNumber=0;
        m_pUserData=NULL;

    }
    const CCamera& operator=(const CCamera& rParam)
    {
        if (this==&rParam)
        {
            return *this;
        }
        m_strDeviceId=rParam.m_strDeviceId;
        m_strId=rParam.m_strId;
        m_strNaming=rParam.m_strNaming;
        m_nChannelId=rParam.m_nChannelId;
        m_bHasPan=rParam.m_bHasPan;
        m_bIsIPCamera=rParam.m_bIsIPCamera;
        m_strDomainName=rParam.m_strDomainName;
        m_nPort=rParam.m_nPort;
        m_nNumber=rParam.m_nNumber;
        m_pUserData=rParam.m_pUserData;
        return *this;
    }
public:
    std::string m_strDeviceId;
    std::string m_strId;
    std::string m_strNaming;
    int m_nChannelId;
    bool m_bHasPan;
    bool m_bIsIPCamera;
    std::string m_strDomainName;
    int m_nPort;
    int m_nNumber;
    //CStoreScheme m_StoreScheme;
    void* m_pUserData;


};

class CAlarmIn
{
public:
    CAlarmIn()
    {
        Clear();
    }
    ~CAlarmIn()
    {

    }
    void Clear()
    {
        m_strId="";
        m_strNaming="";
        m_nChannelId=0;
        m_pUserData=NULL;

    }
    const CAlarmIn& operator=(const CAlarmIn& rParam)
    {
        if (this==&rParam)
        {
            return *this;
        }
        m_strNaming=rParam.m_strNaming;
        m_strId=rParam.m_strId;
        m_nChannelId=rParam.m_nChannelId;
        m_pUserData=rParam.m_pUserData;
        return *this;
    }
public:
    std::string m_strId;
    std::string m_strNaming;
    int m_nChannelId;
    void* m_pUserData;
};

class CAlarmOut
{
public:
    CAlarmOut()
    {
        Clear();
    }
    ~CAlarmOut()
    {

    }
    void Clear()
    {
        m_strId="";
        m_strNaming="";
        m_nChannelId=0;
        m_pUserData=NULL;

    }
    const CAlarmOut& operator=(const CAlarmOut& rParam)
    {
        if (this==&rParam)
        {
            return *this;
        }
        m_strNaming=rParam.m_strNaming;
        m_strId=rParam.m_strId;
        m_nChannelId=rParam.m_nChannelId;
        m_pUserData=rParam.m_pUserData;
        return *this;
    }
public:
    std::string m_strId;
    std::string m_strNaming;
    int m_nChannelId;
    void* m_pUserData;
};




class Megaeyes2003ResSerialize: public MegaeyesResponseSerialize
{
public:
    Megaeyes2003ResSerialize()
    {
        m_strID="";
        m_strNaming="";
        m_nHeartCycle=0;
        m_strSysRebootCycle="";
        m_strCurrentDateTime="";
        m_strDispatchID="";
        m_CameraVecInfo.clear();
        m_AlarmOutVecInfo.clear();
        m_AlarmInVecInfo.clear();
        m_nSuccess=0;
    }
    const Megaeyes2003ResSerialize& operator=(const Megaeyes2003ResSerialize& rParam)
    {
        if (this==&rParam)
        {
            return *this;
        }

        m_strID=rParam.m_strID;
        m_strNaming=rParam.m_strNaming;
        m_nHeartCycle=rParam.m_nHeartCycle;
        m_strSysRebootCycle=rParam.m_strSysRebootCycle;
        m_strCurrentDateTime=rParam.m_strCurrentDateTime;
        m_strDispatchID=rParam.m_strDispatchID;
        m_CameraVecInfo=rParam.m_CameraVecInfo;
        m_AlarmOutVecInfo=rParam.m_AlarmOutVecInfo;
        m_AlarmInVecInfo=rParam.m_AlarmInVecInfo;
        m_nSuccess=rParam.m_nSuccess;
        return *this;
    }
    ~Megaeyes2003ResSerialize()
    {

    }

    virtual std::string Serialize()
    {
        return NULL;
    }

    virtual void UnSerialize(const std::string& strMessage)
    {

        m_xml.SetDoc(strMessage);
        if (m_xml.FindElem("Message")==false)
        {
			throw CMegaEyesException("Megaeyes2003ResSerialize failed\n",2003);
		}
        m_strID=m_xml.GetAttrib("ID");
        m_strNaming=m_xml.GetAttrib("Naming");
        m_nHeartCycle=atoi(m_xml.GetAttrib("HeartCycle").c_str());
        m_strRebootTime=m_xml.GetAttrib("SysRebootTime");
        m_strSysRebootCycle=m_xml.GetAttrib("SysRebootCycle");
        m_strDispatchID=m_xml.GetAttrib("DispatchId");
        m_strCurrentDateTime=m_xml.GetAttrib("CurrentDateTime");
        m_xml.IntoElem();
        while(m_xml.FindElem("Camera"))
        {
            CCamera camera;
            camera.m_strDeviceId=m_xml.GetAttrib("DeviceID");
            camera.m_strNaming=m_xml.GetAttrib("Naming");
            camera.m_nChannelId=atoi(m_xml.GetAttrib("ChannelId").c_str());
            camera.m_bHasPan=((atoi(m_xml.GetAttrib("HasPan").c_str())==1)?true:false);
            camera.m_bIsIPCamera=((atoi(m_xml.GetAttrib("IsIPCamera").c_str()))?true:false);
            camera.m_strDomainName=m_xml.GetAttrib("DomainName");
            camera.m_nPort=atoi(m_xml.GetAttrib("Port").c_str());
            camera.m_nNumber=atoi(m_xml.GetAttrib("Number").c_str());
            m_CameraVecInfo.push_back(camera);
        }
        while(m_xml.FindElem("AlarmIn"))
        {
            CAlarmIn alarmIn;
            alarmIn.m_strId=m_xml.GetAttrib("ID");
            alarmIn.m_strNaming=m_xml.GetAttrib("Naming");
            alarmIn.m_nChannelId=atoi(m_xml.GetAttrib("ChannelId").c_str());
            m_AlarmInVecInfo.push_back(alarmIn);
        }
        while(m_xml.FindElem("AlarmOut"))
        {
            CAlarmOut alarmOut;
            alarmOut.m_strId=m_xml.GetAttrib("ID");
            alarmOut.m_strNaming=m_xml.GetAttrib("Naming");
            alarmOut.m_nChannelId=atoi(m_xml.GetAttrib("ChannelId").c_str());
            m_AlarmOutVecInfo.push_back(alarmOut);
        }
        m_nSuccess=atoi(m_xml.GetAttrib("Success").c_str());
    }

    const CCamera* GetCamera(int& nChannel) const
    {
        for (int i=0;i<(int)m_CameraVecInfo.size();i++)
        {
            if (nChannel==m_CameraVecInfo[i].m_nChannelId)
            {
                return &m_CameraVecInfo[i];
            }
        }
        return NULL;
    }

    const CAlarmIn* GetAlarmIn(int& nChannel)const
    {
        for (int i=0;i<(int)m_AlarmInVecInfo.size();i++)
        {
            if (nChannel==m_AlarmInVecInfo[i].m_nChannelId)
            {
                return &m_AlarmInVecInfo[i];
            }
        }
        return NULL;
    }

    const CAlarmOut* GetAlarmOut(int& nChannel)const
    {
        for (int i=0;i<(int)m_AlarmOutVecInfo.size();i++)
        {
            if (nChannel==m_AlarmOutVecInfo[i].m_nChannelId)
            {
                return &m_AlarmOutVecInfo[i];
            }
        }
        return NULL;
    }

public:
    std::string m_strID;
    std::string m_strNaming;
    int m_nHeartCycle;
    std::string m_strRebootTime;
    std::string m_strSysRebootCycle;
    std::string m_strCurrentDateTime;
    std::string m_strDispatchID;
    std::vector<CCamera> m_CameraVecInfo;
    std::vector<CAlarmOut> m_AlarmOutVecInfo;
    std::vector<CAlarmIn> m_AlarmInVecInfo;
    int m_nSuccess;
};

#endif
