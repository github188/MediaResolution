#ifndef MEGAEYES_VIDEO_PARAM_H
#define MEGAEYES_VIDEO_PARAM_H
#include "MegaeyesProtocol.h"
#include <vector>
class CMegaeyesVideMainPara:public CMegaeyesRequestProtocol
{
public:
	enum _Para
	{
		Frame=1,
		Level=2,
		MaxBPS=3,
		PicFormat=4,
		Shard=5,
		Log=6,
		VideoPara=7,
		Detect=8,

	};
	CMegaeyesVideMainPara()
	{
		Clear();
	}
	~CMegaeyesVideMainPara()
	{

	}
	void Clear()
	{
		m_strStreamType="MainStream";
		m_nPara=0;
	}
public:

	//反序列化
	 void UnSerialize(const std::string& strMessage)
	 {
		 m_xml.SetDoc(strMessage);
		 if (m_xml.FindElem("Message")==false)
		 {
			 throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5034);
		 }
		m_strStreamType=m_xml.GetAttrib("Stream");
		m_xml.IntoElem();
		if (m_xml.FindElem("Para")==false)
		{
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5034);
		}
		m_nPara=atoi(m_xml.GetData().c_str());
	 }

public:
	std::string m_strStreamType;
	int m_nPara;
};

class CVidedRect
{
public:
 int m_nLeft;
 int m_nTop;
 int m_nWidth;
 int m_nHeight;

};

class CMegaeyesSetVideoPara: public CMegaeyesResponseProtocol
{
public:
	CMegaeyesSetVideoPara()
	{
	}
	~CMegaeyesSetVideoPara()
	{
	}
	 const std::string Serialize()
	 {
		 m_RespinseCmdHead.m_nLength=0;
		 m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgSetVidePara+1;
		 std::string str="";
		 return str;
	 }
};
class CMegaeyesGetVideoPara: public CMegaeyesResponseProtocol, public CMegaeyesRequestProtocol
{
public:

	explicit CMegaeyesGetVideoPara(int nPara=0):m_nPara(nPara)
	{

	}
	~CMegaeyesGetVideoPara()
	{

	}
	//反序列化
	 void UnSerialize(const std::string& strMessage)
	{

		m_xml.SetDoc(strMessage);
		if (m_xml.FindElem("Message")==false)
		{
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5034);
		}
		m_strStreamType=m_xml.GetAttrib("Stream");
		m_xml.IntoElem();
		if (m_xml.FindElem("Para")==false)
		{
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5034);
		}
		m_nPara=atoi(m_xml.GetData().c_str());
		if (m_xml.FindElem("Record")==false)
		{
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5034);
		}
		switch (m_nPara)
		{
		case 1:
			
			m_lFrameRate = atol(m_xml.GetAttrib("FrameRate").c_str());
			m_lKeyFrameInterval = atol(m_xml.GetAttrib("KeyFrameIntervals").c_str());
			break;
		case 2:
			m_lLevel = atol(m_xml.GetAttrib("Level").c_str());
			break;
		case 3:
			m_lMaxBps = atoi(m_xml.GetAttrib("MaxBPS").c_str());
			m_strStreamOption = m_xml.GetAttrib("StreamOption");
			break;
		case 4:
			m_lPicFormat =atol(m_xml.GetAttrib("PicFormat").c_str());
			break;
		case 5:
			m_bLoging = atoi(m_xml.GetAttrib("bLogoing").c_str());
			m_nLogingNum =atoi(m_xml.GetAttrib("Num").c_str());
			while(m_xml.FindChildElem("Rect"))
			{
				CVidedRect rect;
				rect.m_nLeft=atoi( m_xml.GetAttrib( "Left" ).c_str() );
				rect.m_nTop =atoi( m_xml.GetAttrib( "Top" ).c_str() );
				rect.m_nWidth=atoi( m_xml.GetAttrib( "Width" ).c_str() );
				rect.m_nHeight=atoi( m_xml.GetAttrib( "Height" ).c_str() );
				m_ShardRectVec.push_back(rect);
			}
			break;
		case 6:
			m_bOSDing = atoi(m_xml.GetAttrib("bOSDing").c_str());
			m_nOSDPlace =atoi(m_xml.GetAttrib("nOSDPlace").c_str());
			m_nWordPlace =atoi(m_xml.GetAttrib("nWordPlace").c_str());
			m_strWord=m_xml.GetAttrib("Word");
			break;
		case 7:
			m_nBrightness = atoi( m_xml.GetAttrib( "Brightness" ).c_str() );
			m_nContrast = atoi( m_xml.GetAttrib( "Contrast" ).c_str() );
			m_nSatuation = atoi( m_xml.GetAttrib( "Saturation" ).c_str() );
			m_nHue = atoi( m_xml.GetAttrib( "Hue" ).c_str() );
			break;
		case 8:
			m_bDetectting = atoi( m_xml.GetAttrib( "bDectecting" ).c_str() );
			m_nDectectGrade = atoi( m_xml.GetAttrib( "DetectGrade" ).c_str() );
			m_nDetectNum = atoi( m_xml.GetAttrib( "Num" ).c_str() );
			m_xml.IntoElem();
			while(m_xml.FindElem("Rect"))
			{
				CVidedRect rect;
				rect.m_nLeft=atoi( m_xml.GetAttrib( "Left" ).c_str() );
				rect.m_nTop =atoi( m_xml.GetAttrib( "Top" ).c_str() );
				rect.m_nWidth=atoi( m_xml.GetAttrib( "Width" ).c_str() );
				rect.m_nHeight=atoi( m_xml.GetAttrib( "Height" ).c_str() );
				m_DetectRectVec.push_back(rect);
			}
			break;
		default:
			throw CMegaEyesException("CMegaeyesSetVidePara failed\n",5034);
		}


	}
	 //序列化
	 const std::string Serialize()
	 {
		m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
		m_xml.AddElem("Message");
		m_xml.AddAttrib("Stream",m_strStreamType);
		m_xml.IntoElem();
		m_xml.AddElem("Para",m_nPara);
		m_xml.AddElem("Record");
		switch(m_nPara)
		{
		case 1:
			m_xml.AddAttrib("FrameRate",m_lFrameRate);
			m_xml.AddAttrib("KeyFrameIntervals",m_lKeyFrameInterval);
			break;
		case 2:
			m_xml.AddAttrib("Level",m_lLevel);
			break;
		case 3:
			m_xml.AddAttrib("MaxBPS", m_lMaxBps);
			m_xml.AddAttrib("StreamOption",m_strStreamOption);
			break;
		case 4:
			m_xml.AddAttrib("PicFormat",m_lPicFormat);
			m_xml.AddAttrib("SupportFormat",m_strSupportFormat);
			break;
		case 5:
			m_xml.AddAttrib("bLogoing",m_bLoging);
			m_xml.AddAttrib("Num",m_nLogingNum);
			m_xml.IntoElem();
			for (int i=0;i<(int)m_ShardRectVec.size();i++)
			{
				m_xml.AddElem("Rect");
				m_xml.AddAttrib("Left",m_ShardRectVec[i].m_nLeft);
				m_xml.AddAttrib("Top",m_ShardRectVec[i].m_nTop);
				m_xml.AddAttrib("Width",m_ShardRectVec[i].m_nWidth);
				m_xml.AddAttrib("Heiht",m_ShardRectVec[i].m_nHeight);
			}
			break;
		case 6:
			m_xml.AddAttrib("bOSDing",m_bOSDing);
			m_xml.AddAttrib("nOSDPlace",m_nOSDPlace);
			m_xml.AddAttrib("nWordPlace",m_nWordPlace);
			m_xml.AddAttrib("Word",m_strWord);
			break;
		case 7:
			m_xml.AddAttrib("Brightness",m_nBrightness);
			m_xml.AddAttrib("Contrast",m_nContrast);
			m_xml.AddAttrib("Saturation",m_nSatuation);
			m_xml.AddAttrib("Hue",m_nHue);
			break;
		case 8:
			m_xml.AddAttrib("bDectecting",m_bDetectting);
			m_xml.AddAttrib("DetectGrade",m_nDectectGrade);
			m_xml.AddAttrib("Num",m_nDetectNum);
			for (int i=0;i<(int)m_DetectRectVec.size();i++)
			{
				m_xml.AddElem("Rect");
				m_xml.AddAttrib("Left",m_DetectRectVec[i].m_nLeft);
				m_xml.AddAttrib("Top",m_DetectRectVec[i].m_nTop);
				m_xml.AddAttrib("Width",m_DetectRectVec[i].m_nWidth);
				m_xml.AddAttrib("Heiht",m_DetectRectVec[i].m_nHeight);
			}
			break;
		default:
			break;
		}
		m_RespinseCmdHead.m_nCmdNum=CMegaeyesQequestCmdHead::MsgGetVidePara+1;
		m_RespinseCmdHead.m_nLength = m_xml.GetDoc().length();
		return m_xml.GetDoc();
	 }
public:
	std::string m_strStreamType;
	int m_nPara;
	long m_lFrameRate;
	long m_lKeyFrameInterval;
	long m_lLevel;
	long m_lMaxBps;
	std::string m_strStreamOption;
	long m_lPicFormat;
	std::string m_strSupportFormat;
	int m_bLoging;
	long m_nLogingNum;
	std::vector<CVidedRect> m_ShardRectVec;
	int m_bOSDing;
	unsigned short m_nOSDPlace;
	unsigned short m_nWordPlace;
	std::string m_strWord;
	int m_nBrightness;
	int m_nContrast;
	int m_nSatuation;
	int m_nHue;
	int m_bDetectting;
	int m_nDectectGrade;
	int m_nDetectNum;
	std::vector<CVidedRect> m_DetectRectVec;


};

#endif