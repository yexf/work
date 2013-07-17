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
	EM_FQMEM_GXMEM,					//默认模式
	EM_FQMEM_GXMEM_SYNC,			//同步模式
	EM_FQMEM_GXMEM_ASYNC,			//异步模式
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
	wxf_uint32 id;							//id号 可缺省为0
	wxf_guid guid;							//guid号
	wxf_gxmem gxmem;						//gxmem区的创建句柄
	wxf_handle rxhl;						//读的信号量
	wxf_handle wxhl;						//写的信号量
	
};
struct wxf_fifo_queue_mem
{
	wxf_gxmem gxmem;						//gxmem区的创建句柄
	void *pmem;								//共享区
	void *pdata;							//数据区
};
struct wxf_fifo_queue_sx					//队列的属性
{								
	wxf_uint32 cap;							//容量
	wxf_uint32 num;							//已写的个数
	wxf_uint32 size;						//元素的大小
	wxf_uint64 pread;						//读指针
	wxf_uint64 pwrite;						//写指针
};
struct wxf_fifo_queue_ts
{
	wxf_comms_em fqsend_status;				//发送线程状态
	wxf_comms_em fqjs_status;					//监视线程状态
	wxf_thread fqsend;						//发送线程
	wxf_thread fqjs;						//监视线程

	wxf_fqm_em mode;
	wxf_fqid fqid;
	wxf_fqsx fqsx;

	wxf_uint32 bcap;						//共享区的大小
	wxf_uint32 bsize;						//数据区的大小
};
struct wxf_fifo_queue_st
{
	wxf_fqts *fqts;							//状态和属性 在共享区
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
