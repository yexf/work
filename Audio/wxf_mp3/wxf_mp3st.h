/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_mp3st.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/05
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_mp3st_h_
#define _wxf_mp3st_h_
#include "wxf_comm.h"
typedef struct wxf_mpeg_file_st	wxf_mfst,*wxf_pmfst;

typedef struct wxf_mpeg_frame_header wxf_mfh,*wxf_pmfh;
typedef struct wxf_mpeg_id3v2_header wxf_mih2,*wxf_pmih2;
typedef struct wxf_mpeg_id3v2_frame_header wxf_mid3v2fh,*wxf_pmid3v2fh;
typedef struct wxf_mpeg_id3v1_header wxf_mih1,*wxf_pmih1;

//////////////////////////////////////////////////////////////////////////

struct wxf_mpeg_frame_header							//mpeg数据帧结构
{
	unsigned int sync: 11;								//同步信息
	unsigned int version: 2;							//版本
	unsigned int layer: 2;								//层
	unsigned int error_protection: 1;					// CRC校验
	unsigned int bitrate_index: 4;						//位率
	unsigned int sampling_frequency: 2;					//采样频率
	unsigned int padding: 1;							//帧长调节
	unsigned int privated: 1;							//保留字
	unsigned int mode: 2;								//声道模式
	unsigned int mode_extension: 2;						//扩充模式
	unsigned int copyright: 1;							// 版权
	unsigned int original: 1;							//原版标志
	unsigned int emphasis: 2;							//强调模式
};

struct wxf_mpeg_id3v2_header							//ID3V2.3标签头结构
{
	char Tag[3];										/*必须为"ID3"否则认为标签不存在*/ 
	char Ver;											/*版本号 ID3V2.3 就记录 3*/ 
	char Revision;										/*副版本号此版本记录为 0*/ 
	char Flag;											/*存放标志的字节,这个版本只定义了三位,稍后详细解说*/ 
	char Size[4];										/*标签大小,包括标签头的 10 个字节和所有的标签帧的大小*/ 
};
struct wxf_mpeg_id3v2_frame_header
{
	char FrameID[4];									/*用四个字符标识一个帧,说明其内容,稍后有常用的标识对照表*/ 
	char Size[4];										/*帧内容的大小,不包括帧头,不得小于 1*/ 
	char Flags[2];										/*存放标志,只定义了 6 位,稍后详细解说*/ 
};

struct wxf_mpeg_id3v1_header
{
	char Tag[3];										//必须为"TAG"
	char SongName[30];									//歌名
	char Author[30];									//作者
	char SpecialName[30];								//专辑名
	int  Year;											//年份
	char Remark[30];									//附注
	unsigned char Type;									//音乐类型
};

struct wxf_mpeg_file_st
{
	wxf_uint8 *buf;
	wxf_mfh *mfh;
	wxf_mih2 *mih2;
	wxf_mih1 *mih1;
	wxf_uint32 file_len;	//文件长度
	wxf_uint32 fd_len;		//帧的总长度
	wxf_uint32 id3v2_len;	//id3v2长度
};
#ifdef __cplusplus
extern "C" {
#endif
	void wxf_mf_deinit(wxf_mfst *pmfst);
	wxf_mfst *wxf_mf_parse(const char *filename);
#ifdef __cplusplus
}
#endif

#endif /*_wxf_mp3st_h_*/
