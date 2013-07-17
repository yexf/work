/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_gxmem.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_gxmem_h_
#define _wxf_gxmem_h_

#define WXF_GXMEM_RW						FILE_MAP_ALL_ACCESS
#define WXF_GXMEM_RO						FILE_MAP_READ


typedef wxf_handle							wxf_gxmem;
typedef struct wxf_share_mem_t				wxf_smem;


struct wxf_share_mem_t
{
	char name[WXF_MAX_NAME];						//名字
	void *pshare;									//映射内存，用来释放
	char *pmem;										//数据指针
	int mem_size;									//数据区大小，实际多4个字节
	wxf_handle hanlde;								//对象句柄，用来释放资源
};
#ifdef __cplusplus
extern "C" {
#endif
	void wxf_gxmem_close(wxf_gxmem gxmem);
	wxf_gxmem wxf_gxmem_create(const char *name,int mem_size);
	wxf_gxmem wxf_gxmem_open(const char *name,int flag);
	void *wxf_gxmem_map(wxf_gxmem pmap,int flag);
	void wxf_gxmem_unmap(void *pshare);

	void wxf_smem_deinit(wxf_smem *psmem);
	wxf_smem *wxf_smem_create(const char *name,int mem_size);
	wxf_smem *wxf_smem_open(const char *name,int access);


	void wxf_gxmem_test(char *para);

#ifdef __cplusplus
}
#endif
#endif /*_wxf_gxmem_h_*/
