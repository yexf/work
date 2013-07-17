/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_player.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/28
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include <wxf_utils.h>
#include "wxf_player.h"
#include "wxf_audio_player.h"

#ifndef _wxf_player_unit_
#define _wxf_player_unit_
int wxf_get_player(wxf_player *player,const void *pdata,int type)
{
	int ret = wxf_err;
	if (player->get_mtd)
	{
		ret = player->get_mtd(player,pdata,type);
	}
	return ret;
}
int wxf_set_player(wxf_player *player,const void *pdata,int type)
{
	int ret = wxf_err;
	if (player->set_mtd)
	{
		ret = player->set_mtd(player,pdata,type);
	}
	return ret;
}
int wxf_load_player(wxf_player *player,const void *pdata,int type)
{
	int ret = wxf_err;
	if (player->load_mtd)
	{
		ret = player->load_mtd(player,pdata,type);
	}
	return ret;
}
int wxf_start_player(wxf_player *player)
{
	int ret = wxf_err;
	if (player->play_hdl)
	{
		ret = player->play_hdl(player);
	}
	return ret;
}
int wxf_stop_player(wxf_player *player)
{
	int ret = wxf_err;
	if (player->stop_hdl)
	{
		ret = player->stop_hdl(player);
	}
	return ret;
}
int wxf_pause_player(wxf_player *player)
{
	int ret = wxf_err;
	if (player->pause_hdl)
	{
		ret = player->pause_hdl(player);
	}
	return ret;
}
int wxf_uninit_plyaer(wxf_player *player)
{
	int ret = wxf_succ;
	if (player->uninit_hdl)
	{
		ret = player->uninit_hdl(player);
	}
	return ret;
}
int wxf_player_proc_handle(void *para)
{
	wxf_cplayer *player = NULL;
	player = (wxf_cplayer *)para;

	while (player->status != EM_UNINIT)
	{
		wxf_lock_ipc(player->player.proc_ipc);
		player->status = player->player.status;
		if (player->play_proc)
		{
			player->play_proc(Player(player));
		}
	}
	return 0;
}
wxf_player *wxf_init_player(wxf_pmem player_mode,wxf_wph play_proc)
{
	int temp;
	wxf_cplayer *ret;

	temp = sizeof(wxf_cplayer);
	ret = (wxf_cplayer *)wxf_malloc(temp);
	wxf_memset(ret,0,temp);

	ret->play_proc = play_proc;
	ret->player_mode = player_mode;
	ret->status = EM_INIT;

	switch(player_mode)
	{
	case EM_PMEM_AUDIO:
		{
			temp = wxf_init_audio_player(Player(ret));
			break;
		}
	case EM_PMEM_VEDIO:
		{

		}
	case EM_PMEM_PICTRUE:
		{

		}
	default: temp = wxf_err;break;
	}

	if (temp == wxf_succ)
	{
		ret->proc_thd = wxf_new_thread(wxf_player_proc_handle,ret,TRUE);
		return Player(ret);
	}
	else 
	{
		wxf_free(ret);
		return NULL;
	}
}
#endif //_wxf_player_unit_