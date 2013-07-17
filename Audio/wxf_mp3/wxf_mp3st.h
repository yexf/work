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

struct wxf_mpeg_frame_header							//mpeg����֡�ṹ
{
	unsigned int sync: 11;								//ͬ����Ϣ
	unsigned int version: 2;							//�汾
	unsigned int layer: 2;								//��
	unsigned int error_protection: 1;					// CRCУ��
	unsigned int bitrate_index: 4;						//λ��
	unsigned int sampling_frequency: 2;					//����Ƶ��
	unsigned int padding: 1;							//֡������
	unsigned int privated: 1;							//������
	unsigned int mode: 2;								//����ģʽ
	unsigned int mode_extension: 2;						//����ģʽ
	unsigned int copyright: 1;							// ��Ȩ
	unsigned int original: 1;							//ԭ���־
	unsigned int emphasis: 2;							//ǿ��ģʽ
};

struct wxf_mpeg_id3v2_header							//ID3V2.3��ǩͷ�ṹ
{
	char Tag[3];										/*����Ϊ"ID3"������Ϊ��ǩ������*/ 
	char Ver;											/*�汾�� ID3V2.3 �ͼ�¼ 3*/ 
	char Revision;										/*���汾�Ŵ˰汾��¼Ϊ 0*/ 
	char Flag;											/*��ű�־���ֽ�,����汾ֻ��������λ,�Ժ���ϸ��˵*/ 
	char Size[4];										/*��ǩ��С,������ǩͷ�� 10 ���ֽں����еı�ǩ֡�Ĵ�С*/ 
};
struct wxf_mpeg_id3v2_frame_header
{
	char FrameID[4];									/*���ĸ��ַ���ʶһ��֡,˵��������,�Ժ��г��õı�ʶ���ձ�*/ 
	char Size[4];										/*֡���ݵĴ�С,������֡ͷ,����С�� 1*/ 
	char Flags[2];										/*��ű�־,ֻ������ 6 λ,�Ժ���ϸ��˵*/ 
};

struct wxf_mpeg_id3v1_header
{
	char Tag[3];										//����Ϊ"TAG"
	char SongName[30];									//����
	char Author[30];									//����
	char SpecialName[30];								//ר����
	int  Year;											//���
	char Remark[30];									//��ע
	unsigned char Type;									//��������
};

struct wxf_mpeg_file_st
{
	wxf_uint8 *buf;
	wxf_mfh *mfh;
	wxf_mih2 *mih2;
	wxf_mih1 *mih1;
	wxf_uint32 file_len;	//�ļ�����
	wxf_uint32 fd_len;		//֡���ܳ���
	wxf_uint32 id3v2_len;	//id3v2����
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
