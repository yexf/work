/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_fq.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/23
**
** Description:
******************************************************ww********************************/ 
#ifndef _wxf_fq_h_
#define _wxf_fq_h_

#define WXF_FQST_NUM(fqst)					(fqst->fqts->fqsx.num)
#define WXF_FQST_CAP(fqst)					(fqst->fqts->fqsx.cap)
#define WXF_FQST_SIZE(fqst)					(fqst->fqts->fqsx.size)
#define WXF_FQST_READCOUNT(fqst)			(fqst->fqts->fqsx.pread)
#define WXF_FQST_WRITECOUNT(fqst)			(fqst->fqts->fqsx.pwrite)
#define WXF_FQST_FREENUM(fqst)				(WXF_FQST_CAP(fqst)-WXF_FQST_NUM(fqst))

typedef enum wxf_fifo_queue_mode_em
{
	EM_FQMEM_MALLOC = 0,
	EM_FQMEM_GXMEM,					//Ĭ��ģʽ
	EM_FQMEM_GXMEM_SYNC,			//ͬ��ģʽ
	EM_FQMEM_GXMEM_ASYNC,			//�첽ģʽ
	EM_FQMEM_COUNT,
}wxf_fqm_em;

typedef struct wxf_fifo_queue_mem wxf_fqmem;
typedef struct wxf_fifo_queue_ts wxf_fqts;
typedef struct wxf_fifo_queue_sx wxf_fqsx;
typedef struct wxf_fifo_queue_st wxf_fqst;
typedef struct wxf_fifo_queue_id wxf_fqid;

typedef int (wxf_fqst_handle)(wxf_fqst *fqst,void *pdata,wxf_uint32 len);
typedef wxf_fqst_handle *					wxf_fh;

struct wxf_fifo_queue_id
{
	wxf_uint32 id;							//id�� ��ȱʡΪ0
	wxf_guid guid;							//guid��
	wxf_gxmem gxmem;						//gxmem���Ĵ������
	wxf_handle rxhl;						//�����ź���
	wxf_handle wxhl;						//д���ź���
	
};
struct wxf_fifo_queue_mem
{
	wxf_gxmem gxmem;						//gxmem���Ĵ������
	void *pmem;								//������
	void *pdata;							//������
};
struct wxf_fifo_queue_sx					//���е�����
{								
	wxf_uint32 cap;							//����
	wxf_uint32 num;							//��д�ĸ���
	wxf_uint32 size;						//Ԫ�صĴ�С
	wxf_uint64 pread;						//��ָ��
	wxf_uint64 pwrite;						//дָ��
};
struct wxf_fifo_queue_ts
{
	wxf_comms_em fqsend_status;				//�����߳�״̬
	wxf_comms_em fqjs_status;					//�����߳�״̬
	wxf_thread fqsend;						//�����߳�
	wxf_thread fqjs;						//�����߳�

	wxf_fqm_em mode;
	wxf_fqid fqid;
	wxf_fqsx fqsx;

	wxf_uint32 bcap;						//�������Ĵ�С
	wxf_uint32 bsize;						//�������Ĵ�С
};
struct wxf_fifo_queue_st
{
	wxf_fqts *fqts;							//״̬������ �ڹ�����
	wxf_fqmem fqmem;
	wxf_fh fqread;
	wxf_fh fqwrite;
	wxf_comms_em fqstatus;
	void *para1;
	void *para2;
	void *para3;
};

#ifdef __cplusplus
extern "C" {
#endif
	int wxf_fqst_read(wxf_fqst *fqst,void *pdata,wxf_uint32 num);
	int wxf_fqst_write(wxf_fqst *fqst,void *pdata,wxf_uint32 num);
	void wxf_fqst_uninit(wxf_fqst *fqst);
	wxf_fqst *wxf_fqst_open(wxf_guid guid);
	wxf_fqst *wxf_fqst_init(wxf_uint32 cap,wxf_uint32 size,wxf_fqm_em mode);
	void wxf_fqst_close(wxf_fqst *fqst);
	int wxf_fqst_test_td(void *para);


	void wxf_fqst_test(void);

#ifdef __cplusplus
}
#endif

#endif /*_wxf_fq_h_*/
