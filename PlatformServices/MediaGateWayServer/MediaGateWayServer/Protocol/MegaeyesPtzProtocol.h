#ifndef MEGAEYES_PTZ_PROTOCOL
#define MEGAEYES_PTZ_PROTOCOL
#include "MegaeyesProtocol.h"
class CMegaeyesPtzProtocol:public  CMegaeyesResponseProtocol
{
public:
	CMegaeyesPtzProtocol()
	{
		Clear();
	}
	~CMegaeyesPtzProtocol()
	{

	}
	void Clear()
	{
		m_strStatus = "11111";
		m_nPresetNum = 128;
		m_strPreset = "";
	}
	virtual const std::string Serialize()
	{
		char buf[2048] ={0};
		sprintf(buf,"<?xml version=\"1.0\" encoding=\"gb2312\"?><Message>\
			<Status>%s</Status>\
			<Aux>0</Aux>\
			<Preset>%d</Preset>\
			<Support Direct=\"y\" Bright=\"y\" Zoom=\"y\" Focus=\"y\" Flip180=\"y\" OriginalSetting=\"y\" EnterMenu=\"y\">\
			<Op>8300</Op>\
			<Op>8306</Op>\
			<Op>8302</Op>\
			<Op>8308</Op>\
			<Op>8200</Op>\
			<Op>8202</Op>\
			<Op>8204</Op>\
			<Op>8206</Op>\
			<Op>8208</Op>\
			<Op>8210</Op>\
			<Op>8212</Op>\
			<Op>8304</Op>\
			<Op>8310</Op>\
			</Support>",m_strStatus.c_str(),m_nPresetNum);

		std::string str;
		str.assign(buf);
		if (m_strPreset !="")
		{
			str = str+m_strPreset;
		}
		str = str +"</Message>";
		m_RespinseCmdHead.m_nCmdNum = 5043;
		m_RespinseCmdHead.m_nCmdSeq = 0;
		m_RespinseCmdHead.m_nLength = str.length();
		return str;
	}
public:
	std::string m_strStatus;
	int m_nPresetNum;
	std::string m_strPreset;
};

class CMegaeyesPtzCtrlProcotol:public CMegaeyesRequestProtocol
{
public:
	enum
	{
		PTZ_ZOOM=8302,
		PTZ_ZOOM_STOP=8308,
		PTZ_IRIS = 8300,
		PTZ_IRIS_STOP = 8306,
		PTZ_FOCUS = 8304,
		PTZ_FOCUS_STOP = 8310,
		PTZ_PRESET_ADD = 8206,
		PTZ_PRESET_DLL = 8208,
		PTZ_PRESET_DLL_ALL = 8212,
		PTZ_PRESET_EXCUTE = 8210,
		PTZ_CTR = 8200,
		PTZ_STOP = 8202,
		PTZ_FAST_GO = 1004,
	};
public:
	CMegaeyesPtzCtrlProcotol()
	{

	}
	~CMegaeyesPtzCtrlProcotol()
	{

	}

	virtual void UnSerialize(const std::string& strMessage)
	{

		m_xml.SetDoc(strMessage);
		if (m_xml.FindElem("Message")==false)
		{
		
			throw CMegaEyesException("CMegaeyesRequestRealVideo UnSerialize failed\n",5006);
		}
		std::string strMethod = m_xml.GetAttrib("Method");
		if (strMethod == "RectCenter")
		{
			m_xml.IntoElem();
			if (m_xml.FindElem("Rect"))
			{
				m_strRect = m_xml.GetData();
				m_nOpera = 1004;
				return;	
			}
			
			throw CMegaEyesException("CMegaeyesRequestRealVideo UnSerialize failed\n",5006);
		}
		if (strMethod == "PointCenter")
		{
			m_xml.IntoElem();
			if (m_xml.FindElem("Point"))
			{
				m_strRect = m_xml.GetData();
				m_nOpera = 1004;
				return;	
			}

			throw CMegaEyesException("CMegaeyesRequestRealVideo UnSerialize failed\n",5006);
		}
		m_xml.IntoElem();
		
		if (m_xml.FindElem("Operate")==false)
		{
			throw CMegaEyesException("CMegaeyesRequestRealVideo UnSerialize failed\n",5006);

		}
		m_nOpera = atoi(m_xml.GetData().c_str());
		if (m_xml.FindElem("Switch"))
		{
			m_nSwitch = atol(m_xml.GetData().c_str());
		}
		if (m_xml.FindElem("Num"))
		{
			m_nPresetNum = atoi(m_xml.GetData().c_str());
		}
		if (m_xml.FindElem("Name"))
		{
			m_strPresetName = m_xml.GetData();
		}
		if (m_xml.FindElem("Speed"))
		{
			m_nPrestSpeed = atol(m_xml.GetData().c_str());
		}
		if (m_xml.FindElem("Action"))
		{
			m_strDirect = m_xml.GetAttrib("Direct");
			if (m_strDirect == "R")
			{
				m_nPtzDirect = 4;
			}
			if (m_strDirect == "U")
			{
				m_nPtzDirect = 1;
			}
			if (m_strDirect == "L")
			{
				m_nPtzDirect = 3;

			}
			if (m_strDirect == "D")
			{
				m_nPtzDirect  = 2;
			}
			m_nPtzSpeed = atol(m_xml.GetAttrib("Speed").c_str());

		}
		
	}
	void Clear()
	{
		m_nOpera = 0;
		m_nSwitch = 0;
		m_nPresetNum = -1;
		m_nPrestSpeed = -1;
		m_nPtzSpeed = -1;
		m_strDirect = "";
		m_strPresetName = "";
		m_strRect = "";
	}
public:
	int m_nOpera;
	int m_nSwitch;
	int m_nPresetNum;
	std::string m_strPresetName;
	long m_nPrestSpeed;
	std::string m_strDirect;
	long m_nPtzSpeed;
	int m_nPtzDirect;
	std::string m_strRect;
};

class CMegaeyesPtzCtrlResponse:public CMegaeyesResponseProtocol
{
	public:
	const std::string Serialize()
	{
		m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgPtzCtrl+1;
	m_RespinseCmdHead.m_nLength = 0;
		std::string str;
		return str;
	}
};

#endif