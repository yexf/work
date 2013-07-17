/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_wav.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/27
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_wav_h_
#define _wxf_wav_h_

typedef struct wxf_wave_format_tag
{ 
	char cRiffFlag[4];			//'R','I','F','F'
	uint nFileLen;				// RIFF chunk 的数据大小
	char cWaveFlag[4];			//'W','A','V','E'
	char cFmtFlag[4];			//'f','m','t','\0'
	uint cnTransition;			//采样深度 16 或 18	 
	ushort nFormatTag;			//WAVE_FORMAT_PCM
	ushort nChannels;			//通道数
	uint nSamplesPerSec;		//采样频率（每秒样本数）
	uint nAvgBytesperSec;		//每秒数据量；其值为通道数×采样频率×每样本的数据位数／8
	ushort nBlockAlign;			//数据块的调整数（按字节算的），其值为通道数×每样本的数据位值／8
	ushort nBitNumPerSample;	//采样深度 16 或 18
	char cDataFlag[4];			//'d','a','t','a'
	uint nAudioLength;		//数据大小
	char pData[4];				//数据区
}wxf_wft;

#ifdef __cplusplus
extern "C" {
#endif
	wxf_wft *wxf_wft_open(const char *filename);
#ifdef __cplusplus
}
#endif

#endif /*_wxf_wav_h_*/
