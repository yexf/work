/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_player.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/28
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_player_h_
#define _wxf_player_h_

#define Player(obj)								((wxf_player*)obj)

typedef const void *wxf_pplayer;							//播放器对象
typedef struct wxf_player_tag wxf_player;					//播放器结构
typedef struct wxf_common_player_tag wxf_cplayer;			//通用播放器结构

typedef int (wxf_wave_player_handle)(wxf_player *player);
typedef wxf_wave_player_handle *wxf_wph;
typedef int (wxf_wave_player_method)(wxf_player *player,const void *para,int type);
typedef wxf_wave_player_method *wxf_wpm;

typedef enum wxf_player_mode_em
{
	EM_PMEM_NONE = 0,
	EM_PMEM_AUDIO,
	EM_PMEM_VEDIO,
	EM_PMEM_PICTRUE,
	EM_PMEM_COUNT,
}wxf_pmem;
typedef enum wxf_player_para_em
{
	EM_PPEM_NONE = 0,
	EM_PPEM_STATUS,

	EM_PPEN_COUNT,
}wxf_ppem;
typedef enum wxf_play_type_em
{
	EM_PTEM_NONE = 0,
	EM_PTEM_PCM,				
	EM_PTEM_FILE,
	EM_PTEM_WAV_FILE,
	EM_PTEM_MP3_FILE,
	EM_PTEM_WMV_FILE,
	EM_PTEN_COUNT,
}wxf_ptem;

struct wxf_player_tag
{
	wxf_comms_em status;		//播放运行状态
	wxf_pmem player_mode;		//播放器类型
	wxf_ipc proc_ipc;			//控制信号量
	wxf_pplayer para;			//播放器参数

	wxf_wph play_hdl;			//播放动作
	wxf_wph stop_hdl;			//停止动作
	wxf_wph pause_hdl;			//暂停动作
	wxf_wph init_hdl;			//初始化动作
	wxf_wph uninit_hdl;			//释放动作
	wxf_wpm set_mtd;			//设置方法
	wxf_wpm get_mtd;			//获取方法
	wxf_wpm load_mtd;			//装载方法
	
	int play_para;				//保留参数
	void *play_para0;			//保留参数
	void *play_para1;			//保留参数
	void *play_para2;			//保留参数
	void *play_para3;			//保留参数
};
struct wxf_common_player_tag	//通用播放器
{
	wxf_player player;			//播放器
	wxf_comms_em status;		//播放运行状态
	wxf_pmem player_mode;		//播放器类型
	wxf_thread proc_thd;		//回调线程
	wxf_wph play_proc;			//回调函数
};

#ifdef __cplusplus
extern "C" {
#endif
	int wxf_get_player(wxf_player *plyer,const void *pSrc,int type);
	int wxf_set_player(wxf_player *plyer,const void *pSrc,int type);
	int wxf_load_player(wxf_player *plyer,const void *pSrc,int type);
	int wxf_start_player(wxf_player *plyer);
	int wxf_stop_player(wxf_player *plyer);
	int wxf_pause_player(wxf_player *plyer);
	int wxf_uninit_plyaer(wxf_player *plyer);
	wxf_player *wxf_init_player(wxf_pmem player_mode,wxf_wph play_proc);
#ifdef __cplusplus
}
#endif

#endif /*_wxf_player_h_*/
