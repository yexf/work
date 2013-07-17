/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_wave.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/15
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_wave_h_
#define _wxf_wave_h_

#include "wxf_fbuf.h"
#include <wxf_utils.h>
#define DATA_SIZE_5M		(882)						//单通道10ms 44.1的数据字节数

#define BUFFER_SIZE			(100*2*DATA_SIZE_5M)		//缓存区大小  
#define FRAGMENT_SIZE		(5*2*DATA_SIZE_5M)			//帧缓存区大小

typedef struct wxf_wave_param_st wxf_wpara;				//pcm参数
typedef struct wxf_wave_pcm_player_st wxf_wpplayer;		//pcm播放器
typedef int (wxf_wave_proc_func)(wxf_wpplayer *player);
typedef wxf_wave_proc_func *wxf_wpf;

struct wxf_wave_param_st
{
	char nChannels;					//通道数 1为单声道 2为双声道
	char wBitsPerSample;			//采样深度 8bit 或者 16bit
    wxf_uint16 nSamplesPerSec;		//采样率
};
struct wxf_wave_pcm_player_st
{
	int sync_mode;					//同步模式 0 为异步 1 为同步
	void *para;						//回调参数
	wxf_fbuf *wavbuf;

};



#ifdef __cplusplus
extern "C" {
#endif
	void wxf_wpplayer_uninit(wxf_wpplayer *player);
	wxf_wpplayer *wxf_wpplayer_init();
	int wxf_wpplayer_start(wxf_wpplayer *player);
	int wxf_wpplayer_stop(wxf_wpplayer *player);
	int wxf_wpplayer_play(wxf_wpplayer *player,void *pdata,int len);
#ifdef __cplusplus
}
#endif

//void CALLBACK waveInProc(HWAVEIN hwi,        
//						 UINT uMsg,           
//						 DWORD_PTR dwInstance,    
//						 DWORD_PTR dwParam1,    
//						 DWORD_PTR dwParam2     );  
//void CALLBACK waveOutProc(  HWAVEOUT hwo,     
//						  UINT uMsg,           
//						  DWORD_PTR dwInstance,     
//						  DWORD_PTR dwParam1,      
//						  DWORD_PTR dwParam2     ); 
#endif /*_wxf_wave_h_*/
