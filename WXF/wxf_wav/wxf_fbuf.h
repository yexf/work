/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_fbuf.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/15
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_fbuf_h_
#define _wxf_fbuf_h_

typedef struct wxf_fifo_buffer_st wxf_fbuf;				//������

#define WXF_FB_UNINIT					0
#define WXF_FB_EMPTY					1
#define WXF_FB_FULL						2
#define WXF_FB_UNFULL					3

struct wxf_fifo_buffer_st								//FIFO������
{
	int status;											//������״̬
	char *pdata;										//������������
	char *pcur;											//��ǰָ��
	wxf_uint32 cap;										//����������
	wxf_uint32 size;									//��������С
	wxf_uint32 pread;									//������дָ��
	wxf_uint32 pwrite;									//��������ָ��
};

#ifdef __cplusplus
extern "C" {
#endif

	wxf_fbuf *wxf_create_fbuf();
	void wxf_uninit_fbuf(wxf_fbuf *buf);
	wxf_fbuf *wxf_init_fbuf(wxf_fbuf *buf,int cap);
	int wxf_read_fbuf(wxf_fbuf *buf,void *pDst,wxf_uint32 num);
	int wxf_write_fbuf(wxf_fbuf *buf,void *pSrc,wxf_uint32 len);
#ifdef __cplusplus
}
#endif

#endif /*_wxf_fbuf_h_*/
