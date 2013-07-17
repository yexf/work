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

typedef enum
{
	EM_FB_INIT = 0,
	EM_FB_EMPTY,
	EM_FB_UNFULL,
	EM_FB_FULL,
	EM_FB_UNINIT,
}wxf_fbem;
struct wxf_fifo_buffer_st								//FIFO������
{
	wxf_fbem fbem;										//������״̬
	char *pdata;										//������������
	wxf_uint32 cap;										//����������
	wxf_uint32 size;									//��������С
	wxf_uint64 pread;									//������дָ��
	wxf_uint64 pwrite;									//��������ָ��
	wxf_ipc hread;
	wxf_ipc hwrite;
};

#ifdef __cplusplus
extern "C" {
#endif
	void wxf_uninit_fbuf(wxf_fbuf *buf);
	wxf_fbuf *wxf_init_fbuf(int cap);
	void wxf_clear_fbuf(wxf_fbuf *fbuf);
	int wxf_read_fbuf(wxf_fbuf *buf,void *pDst,wxf_uint32 num);
	int wxf_write_fbuf(wxf_fbuf *buf,void *pSrc,wxf_uint32 len);
	int wxf_fread_fbuf(wxf_fbuf *buf,void *pDst,wxf_uint32 num);
	int wxf_fwrite_fbuf(wxf_fbuf *buf,void *pSrc,wxf_uint32 len);

	void wxf_fbuf_test(void);

#ifdef __cplusplus
}
#endif

#endif /*_wxf_fbuf_h_*/
