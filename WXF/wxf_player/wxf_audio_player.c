/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_audio_player.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/28
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include <wxf_utils.h>
#include <mmsystem.h> 
#pragma comment(lib, "winmm.lib")
#include "wxf_player.h"
typedef struct wxf_audio_player_para
{
	wxf_fbuf *player_fbuf;		//播放fbuf
	wxf_thread proc_thd;		//回调线程
	wxf_thread play_thd;		//播放线程
	wxf_ipc proc_ipc;			//回调信号量
	wxf_wph play_proc;			//回调函数

	int buf_size;				//缓存大小
	void *hHalf_buf;			//高位缓存
	void *lHalf_buf;			//低位缓存
	wxf_bool isloop;			//是否循环
	wxf_bool loop_mode;			//循环方式

	WAVEFORMATEX wavform;		//波形参数
	HWAVEOUT hWaveOut;			//输出句柄
	WAVEHDR wavHhdr;			//波形句柄1
	WAVEHDR wavDhdr;			//波形句柄2
}wxf_appara;
#ifndef _wxf_audio_player_unit_
#define _wxf_audio_player_unit_

int wxf_init_audio_player(wxf_player *player)
{
	int temp,ret = wxf_succ;

	temp = sizeof(wxf_player);
	wxf_memset(player,0,temp);



	return ret;
}
#endif