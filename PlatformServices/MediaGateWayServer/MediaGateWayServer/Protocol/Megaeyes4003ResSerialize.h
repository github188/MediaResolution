#ifndef _MEGAEYES4003RESSERIALIZE_H 
#define  _MEGAEYES4003RESSERIALIZE_H

#include "MegaeyesResponseSerialize.h"
/**********************Éè±¸×¢²á2002************************************/
/*
<TermRegister DeviceID="55555555555555555551" DeviceIP="25.30.5.220" DeviceLinkType="1" DeviceMaxConnect="0">
<VideoPort>4601</VideoPort>
<AudioPort>4602</AudioPort>
<MsgPort>4600</MsgPort>
<Version>2</Version>
<Protocol>HIKMP4.2.5</Protocol>
<SchemeSupport>0</SchemeSupport>
<PtzLockRet>1</PtzLockRet>
<IsSupportNAT>0</IsSupportNAT>
<Support NAT="false" Scheme="true" PtzLockRet="true" Video2="true" MainStream="true" SubStream1="true"/>
</TermRegister>
*/
/*********************************************************************/

class Megaeyes4003ResSerialize:public MegaeyesResponseSerialize
{
public:
	Megaeyes4003ResSerialize()
	{
       m_bIsProtocolTranslator=true;
       m_strVersion="2";
       m_nPtzLockRet=1;
       m_bMainStream=true;
       m_bSubStream1=true;
       m_nDeviceLinkType=1;
       m_nSchemeSupport=1;
       m_nPid=1;
	}
	~Megaeyes4003ResSerialize()
	{

	}
public:
   virtual std::string Serialize()
   {  
      std::string str;
	  return str;
   }
	virtual void UnSerialize(const std::string& strMessage)
	{ 
	}

public:
   int m_nPid;
   std::string m_strDeviceID;
   std::string m_strDeviceIP;
   int m_nDeviceLinkType;
   int m_nDevMaxConnect;
   bool m_bIsProtocolTranslator;
   int m_nVideoPort;
   int m_nAudioPort;
   int m_nMsgPort;
   std::string m_strVersion;
   std::string m_strProtocol;
   int m_nSchemeSupport;
   int m_nPtzLockRet;
   int m_nIsSupportNAT;
   bool m_bNAT;
   bool m_bScheme;
   bool m_bPtzLockRet;
   bool m_bVideo2;
   bool m_bMainStream;
   bool m_bSubStream1;
};

#endif
