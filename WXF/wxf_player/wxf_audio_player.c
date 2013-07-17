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
	wxf_fbuf *player_fbuf;		//����fbuf
	wxf_thread proc_thd;		//�ص��߳�
	wxf_thread play_thd;		//�����߳�
	wxf_ipc proc_ipc;			//�ص��ź���
	wxf_wph play_proc;			//�ص�����

	int buf_size;				//�����С
	void *hHalf_buf;			//��λ����
	void *lHalf_buf;			//��λ����
	wxf_bool isloop;			//�Ƿ�ѭ��
	wxf_bool loop_mode;			//ѭ����ʽ

	WAVEFORMATEX wavform;		//���β���
	HWAVEOUT hWaveOut;			//������
	WAVEHDR wavHhdr;			//���ξ��1
	WAVEHDR wavDhdr;			//���ξ��2
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