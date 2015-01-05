#pragma once

//由于采集卡无法像DVR那样存储信息，所以必须在磁盘上建立相应的结构存储，与接口参数对应

namespace Platform
{

//字幕
class Osd
{
public:
	Osd()
	{
		m_bShowOsd = TRUE;
		m_nOSDPlace = 1;
		m_nWordPlace = 1;
	}
	Osd(BOOL bShowOsd,WORD nOSDPlace,WORD nWordPlace,std::string& sWord)
	{
		m_bShowOsd = bShowOsd;
		m_nOSDPlace = nOSDPlace;
		m_nWordPlace = nWordPlace;
		m_sWord = sWord;
	}
	Osd(const Osd& o)
	{
		m_bShowOsd = o.m_bShowOsd;
		m_nOSDPlace = o.m_nOSDPlace;
		m_nWordPlace = o.m_nWordPlace;
		m_sWord = o.m_sWord;
	}
public:
	BOOL m_bShowOsd;
	WORD m_nOSDPlace;
	WORD m_nWordPlace;
	std::string m_sWord;
};

class StreamPara
{
public:
	long m_nFrameRate;
	long m_nKeyFrameIntervals;
	long m_nLevel;

	long m_nMaxBPS;
	bool m_vbr;

	long m_nPicFormat;
	std::string m_sSupportFormat;
public:
	StreamPara()
	{
		m_nFrameRate = 25;
		m_nKeyFrameIntervals = 100;
		m_nLevel = 0;
		m_nMaxBPS = 768;
		m_vbr = false;
	}
	StreamPara(const StreamPara& para)
	{
		m_nFrameRate = para.m_nFrameRate;
		m_nKeyFrameIntervals = para.m_nKeyFrameIntervals;
		m_nLevel = para.m_nLevel;
		m_nMaxBPS = para.m_nMaxBPS;
		m_vbr = para.m_vbr;
		m_nPicFormat = para.m_nPicFormat;
		m_sSupportFormat = para.m_sSupportFormat;
	}
	StreamPara& operator=(const StreamPara& para)
	{
		m_nFrameRate = para.m_nFrameRate;
		m_nKeyFrameIntervals = para.m_nKeyFrameIntervals;
		m_nLevel = para.m_nLevel;
		m_nMaxBPS = para.m_nMaxBPS;
		m_vbr = para.m_vbr;
		m_nPicFormat = para.m_nPicFormat;
		m_sSupportFormat = para.m_sSupportFormat;
	}
};

//视频参数
class VideoPara
{
public:
	VideoPara()
	{	
		m_nBrightness = 160;
		m_nContrast = 128;
		m_nSaturation = 140;
		m_nHue = 128;
	}
	VideoPara(const VideoPara& para)
	{
		m_nBrightness = para.m_nBrightness;
		m_nContrast = para.m_nContrast;
		m_nSaturation = para.m_nSaturation;
		m_nHue = para.m_nHue;

	}
	VideoPara& operator=(const VideoPara& para)
	{
		m_nBrightness = para.m_nBrightness;
		m_nContrast = para.m_nContrast;
		m_nSaturation = para.m_nSaturation;
		m_nHue = para.m_nHue;
		return *this;
	}

public:
	long m_nBrightness;
	long m_nContrast;
	long m_nSaturation;
	long m_nHue;
};

//双码流参数
class DoubleStreamPara
{
public:
	DoubleStreamPara()
	{	
		m_StreamPara0.m_nFrameRate = 25;
		m_StreamPara0.m_nKeyFrameIntervals = 100;
		m_StreamPara0.m_nLevel = 0;
		m_StreamPara0.m_nMaxBPS = 1024;
		m_StreamPara0.m_vbr = false;
		m_StreamPara0.m_nPicFormat = 3;
		m_StreamPara0.m_sSupportFormat = "0123";
		m_StreamPara1.m_nFrameRate = 25;
		m_StreamPara1.m_nKeyFrameIntervals = 100;
		m_StreamPara1.m_nLevel = 0;
		m_StreamPara1.m_nMaxBPS = 35;
		m_StreamPara1.m_vbr = false;
		m_StreamPara1.m_nPicFormat = 0;
		m_StreamPara1.m_sSupportFormat = "0123";

	}
	DoubleStreamPara(const DoubleStreamPara& para)
	{
		m_StreamPara0.m_nFrameRate = para.m_StreamPara0.m_nFrameRate;
		m_StreamPara0.m_nKeyFrameIntervals = para.m_StreamPara0.m_nKeyFrameIntervals;
		m_StreamPara0.m_nLevel = para.m_StreamPara0.m_nLevel;
		m_StreamPara0.m_nMaxBPS = para.m_StreamPara0.m_nMaxBPS;
		m_StreamPara0.m_vbr = para.m_StreamPara0.m_vbr;
		m_StreamPara0.m_nPicFormat = para.m_StreamPara0.m_nPicFormat;
		m_StreamPara0.m_sSupportFormat = para.m_StreamPara0.m_sSupportFormat;

		m_StreamPara1.m_nFrameRate = para.m_StreamPara1.m_nFrameRate;
		m_StreamPara1.m_nKeyFrameIntervals = para.m_StreamPara1.m_nKeyFrameIntervals;
		m_StreamPara1.m_nLevel = para.m_StreamPara1.m_nLevel;
		m_StreamPara1.m_nMaxBPS = para.m_StreamPara1.m_nMaxBPS;
		m_StreamPara1.m_vbr = para.m_StreamPara1.m_vbr;
		m_StreamPara1.m_nPicFormat = para.m_StreamPara1.m_nPicFormat;
		m_StreamPara1.m_sSupportFormat = para.m_StreamPara1.m_sSupportFormat;

	}
	DoubleStreamPara& operator=(const DoubleStreamPara& para)
	{
		m_StreamPara0.m_nFrameRate = para.m_StreamPara0.m_nFrameRate;
		m_StreamPara0.m_nKeyFrameIntervals = para.m_StreamPara0.m_nKeyFrameIntervals;
		m_StreamPara0.m_nLevel = para.m_StreamPara0.m_nLevel;
		m_StreamPara0.m_nMaxBPS = para.m_StreamPara0.m_nMaxBPS;
		m_StreamPara0.m_vbr = para.m_StreamPara0.m_vbr;
		m_StreamPara0.m_nPicFormat = para.m_StreamPara0.m_nPicFormat;
		m_StreamPara0.m_sSupportFormat = para.m_StreamPara0.m_sSupportFormat;

		m_StreamPara1.m_nFrameRate = para.m_StreamPara1.m_nFrameRate;
		m_StreamPara1.m_nKeyFrameIntervals = para.m_StreamPara1.m_nKeyFrameIntervals;
		m_StreamPara1.m_nLevel = para.m_StreamPara1.m_nLevel;
		m_StreamPara1.m_nMaxBPS = para.m_StreamPara1.m_nMaxBPS;
		m_StreamPara1.m_vbr = para.m_StreamPara1.m_vbr;
		m_StreamPara1.m_nPicFormat = para.m_StreamPara1.m_nPicFormat;
		m_StreamPara1.m_sSupportFormat = para.m_StreamPara1.m_sSupportFormat;

		return *this;
	}

public:

	StreamPara m_StreamPara0;
	StreamPara m_StreamPara1;
};

}
