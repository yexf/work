/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_gxmem.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include "wxf_utils.h"
#include "wxf_gxmem.h"
#ifndef _wxf_gxmem_unit_
#define _wxf_gxmem_unit_
void wxf_gxmem_close(wxf_gxmem gxmem)
{
	if (gxmem)
	{
		CloseHandle(gxmem);
	}
}
wxf_gxmem wxf_gxmem_create(const char *name,int mem_size)
{
	HANDLE hShare;

	if (mem_size <= 0) return NULL;
	hShare = CreateFileMapping((HANDLE)-1,0,PAGE_READWRITE,0,mem_size,name); 
	/*if (hShare == NULL) 
		wxf_printf("create filemapping %s err!!\n",name);*/
	if (GetLastError() != ERROR_ALREADY_EXISTS)  
	{
		void *pmem = wxf_gxmem_map(hShare,WXF_GXMEM_RW);
		ZeroMemory(pmem,mem_size);
		wxf_gxmem_unmap(pmem);
	}
	return hShare;
}
wxf_gxmem wxf_gxmem_open(const char *name,int flag)
{
	HANDLE hShare;
	hShare = OpenFileMapping(flag,FALSE,name);
	/*if (hShare == NULL) 
		wxf_printf("open filemapping %s err!!\n",name);*/
	return hShare;
}
void *wxf_gxmem_map(wxf_gxmem gxmem,int flag)
{
	void *pShare;
	pShare = MapViewOfFile(gxmem,flag,0,0,0);
	return pShare;
}
void wxf_gxmem_unmap(void *pshare)
{
	if (pshare)
	{
		UnmapViewOfFile(pshare);
	}
}
#endif
#ifndef _wxf_smem_unit_
#define _wxf_smem_unit_
void wxf_smem_deinit(wxf_smem *psmem)
{
	if (psmem)
	{
		if (psmem->pshare)
		{
			UnmapViewOfFile(psmem->pshare);
		}
		if (psmem->hanlde)
		{
			CloseHandle((HANDLE)psmem->hanlde);
		}
		wxf_free(psmem);
	}
}
wxf_smem *wxf_smem_init(const char *name)
{
	int itemp;
	wxf_smem *pret;
	
	itemp = sizeof(wxf_smem);
	pret = (wxf_smem *)malloc(itemp);
	if (pret == NULL) return NULL;
	memset(pret,0,itemp);

	itemp = strlen(name);
	if (itemp >= WXF_MAX_NAME)
	{
		itemp = WXF_MAX_NAME - 1;
	}
	memcpy(pret->name,name,itemp);
	return pret;
	
}

wxf_smem *wxf_smem_create(const char *name,int mem_size)
{
	char *pShare;
	HANDLE hShare;
	wxf_smem *pret;
	
	if (mem_size <= 0) return NULL;
	pret = wxf_smem_init(name);
	if (pret == NULL) return NULL;

	hShare = CreateFileMapping((HANDLE)-1,0,PAGE_READWRITE,0,mem_size+sizeof(int),pret->name); 
	if (hShare == NULL)
	{
		wxf_smem_deinit(pret);
		return NULL;
	}
	pShare = (char *)MapViewOfFile(hShare,FILE_MAP_ALL_ACCESS,0,0,0);
	
	pret->mem_size = mem_size;
	pret->pshare = pShare;
	pret->pmem = pShare + sizeof(int);
	pret->hanlde = hShare;

	if (GetLastError() != ERROR_ALREADY_EXISTS)  
	{
		ZeroMemory(pret->pmem,pret->mem_size);
		memcpy(pShare,&mem_size,sizeof(int));  // 共享区初始化
		return pret;
	}
	else
	{
		wxf_smem_deinit(pret);
		return NULL;
	}
}
wxf_smem *wxf_smem_open(const char *name,int access)
{
	int itemp;
	HANDLE hShare;
	char *pShare;
	wxf_smem *pret;

	pret = wxf_smem_init(name);
	hShare = OpenFileMapping(access,FALSE,pret->name);
	if (hShare == NULL)
	{
		wxf_smem_deinit(pret);
		return NULL;
	}
	pShare = (char *)MapViewOfFile(hShare,access,0,0,0);
	memcpy(&itemp,pShare,sizeof(int));

	pret->mem_size = itemp;
	pret->pshare = pShare;
	pret->pmem = pShare + sizeof(int);
	pret->hanlde = hShare;

	return pret;
}
#endif
void wxf_gxmem_test(char *para)
{
	char name[256];
	static wxf_gxmem sm = NULL;
	static wxf_gxmem ot = NULL;
	char op[256];
	sscanf(para,"wxf_utils gxmem %s %s",op,name);
	if (strcmp(op,"open") == 0)
	{
		if (ot != NULL)
		{
			wxf_close_obj(ot);
			ot = NULL;
		}
		ot = wxf_gxmem_open(name,WXF_GXMEM_RO);
		if (ot != NULL)	wxf_printf("open %s gxmem succ\n",name);
		else wxf_printf("open %s gxmem err\n",name);
	}
	else if (strcmp(op,"new") == 0)
	{
		if (sm != NULL)
		{
			wxf_close_obj(sm);
			sm = NULL;
		}
		sm = wxf_gxmem_create(name,4096);
		if (sm != NULL)	wxf_printf("new %s gxmem succ\n",name);
		else wxf_printf("new %s gxmem err\n",name);
	}
	else if (strcmp(op,"close") == 0)
	{
		if (sm != NULL)
		{
			wxf_close_obj(sm);
			sm = NULL;
		}
		if (ot != NULL)
		{
			wxf_close_obj(ot);
			ot = NULL;
		}
	}
	else if (strcmp(op,"read") == 0)
	{
		if (ot != NULL)
		{
			void *pmem;
			pmem = wxf_gxmem_map(ot,WXF_GXMEM_RO);
			puts((char *)pmem);
			wxf_gxmem_unmap(pmem);
		}
	}
	else if (strcmp(op,"write") == 0)
	{
		if (sm != NULL)
		{
			void *pmem;
			pmem = wxf_gxmem_map(sm,WXF_GXMEM_RW);
			gets((char*)pmem);
			wxf_gxmem_unmap(pmem);
		}
	}
}
