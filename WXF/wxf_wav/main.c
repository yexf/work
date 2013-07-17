/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : main.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/15
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include <windows.h>     
#include <mmsystem.h> 
#pragma comment(lib, "winmm.lib")
#include "wxf_wave.h"
#include "wxf_wav.h"
wxf_handle hLock = NULL;
HWAVEOUT hWaveOut = NULL;		//输出接品句柄
static void CALLBACK waveOutProc(HWAVEOUT hwo,     
						  UINT uMsg,           
						  DWORD_PTR dwInstance,     
						  DWORD_PTR dwParam1,      
						  DWORD_PTR dwParam2     )  
{  
	if (WOM_DONE == uMsg)  
	{  
		wxf_unlock_xhl(hLock);
	}  
}
void test01(void)
{
	wxf_wpplayer *player;
	wxf_uint16 temp = 0;
	int i,count = 100;
	wxf_uint16 pcm[1024];
	player = wxf_wpplayer_init();
	wxf_wpplayer_start(player);
	while(count--)
	{
		for (i = 0;i < 1024;i++)
		{
			pcm[i] = temp++;
		}
		wxf_wpplayer_play(player,pcm,1024*2);
	}
	wxf_wpplayer_uninit(player);
}
wxf_wft *test02(const char *src_file)
{
	wxf_wft *wft;
	
	WAVEFORMATEX waveform; //采集音频的格式，结构体  
	//BYTE *pBuffer1,*pBuffer2;//采集音频时的数据缓存  
	WAVEHDR m_WaveHdr1; //采集音频时包含数据缓存的结构体  

	wft = wxf_wft_open(src_file);
	printf("采样率：%d Hz\n",wft->nSamplesPerSec);
	printf("每秒比特率：%g Mb/s\n",8.0*wft->nAvgBytesperSec/1000000);
	
	waveform.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM  
	waveform.nSamplesPerSec = 44100;//采样率，16000次/秒  
	waveform.wBitsPerSample = 16;//采样比特，16bits/次  
	waveform.nChannels = 2;//采样声道数，2声道  
	waveform.nAvgBytesPerSec = 44100 * 4;//每秒的数据率，就是每秒能采集多少字节的数据  
	waveform.nBlockAlign = 4;//一个块的大小，采样bit的字节数乘以声道数  
	waveform.cbSize = 0;//一般为0 

	m_WaveHdr1.lpData           = (LPTSTR)wft->pData; 
	m_WaveHdr1.dwBufferLength  = wft->nAudioLength; 
	m_WaveHdr1.dwBytesRecorded = 0; 
	m_WaveHdr1.dwUser          = 0; 
	m_WaveHdr1.dwFlags         = WHDR_BEGINLOOP | WHDR_ENDLOOP; 
	m_WaveHdr1.dwLoops         = 1; 
	m_WaveHdr1.lpNext          = NULL; 
	m_WaveHdr1.reserved        = 0; 
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD_PTR)NULL,(DWORD_PTR)NULL,CALLBACK_FUNCTION);  

	waveOutPrepareHeader(hWaveOut,&m_WaveHdr1, sizeof(WAVEHDR)); 
	waveOutWrite(hWaveOut,&m_WaveHdr1, sizeof(WAVEHDR)); 
	hLock = wxf_create_xhl("yexf_lock",0,1);
	Sleep(5);
	return wft;
}
void main()
{
	char *input_file = "E:\\Projects\\WXF\\data\\1874-320kbps.wav";
	wxf_wft *wft = test02(input_file);
	wxf_lock(hLock);
	wxf_close_obj(hLock);
	wxf_free(wft);
	CloseHandle(hWaveOut);
}