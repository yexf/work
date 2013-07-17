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
	uint nFileLen;				// RIFF chunk �����ݴ�С
	char cWaveFlag[4];			//'W','A','V','E'
	char cFmtFlag[4];			//'f','m','t','\0'
	uint cnTransition;			//������� 16 �� 18	 
	ushort nFormatTag;			//WAVE_FORMAT_PCM
	ushort nChannels;			//ͨ����
	uint nSamplesPerSec;		//����Ƶ�ʣ�ÿ����������
	uint nAvgBytesperSec;		//ÿ������������ֵΪͨ����������Ƶ�ʡ�ÿ����������λ����8
	ushort nBlockAlign;			//���ݿ�ĵ����������ֽ���ģ�����ֵΪͨ������ÿ����������λֵ��8
	ushort nBitNumPerSample;	//������� 16 �� 18
	char cDataFlag[4];			//'d','a','t','a'
	uint nAudioLength;		//���ݴ�С
	char pData[4];				//������
}wxf_wft;

#ifdef __cplusplus
extern "C" {
#endif
	wxf_wft *wxf_wft_open(const char *filename);
#ifdef __cplusplus
}
#endif

#endif /*_wxf_wav_h_*/
