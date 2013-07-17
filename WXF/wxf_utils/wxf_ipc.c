/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_ipc.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/28
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include "wxf_utils.h"
#ifndef _wxf_new_thread_
#define _wxf_new_thread_
typedef unsigned int (wxf_stdcall thread_callback) (void *);
typedef thread_callback	*						thread_cb;
typedef struct
{
	wxf_thdl proc;
	void *para;
	int isrun;
	wxf_thread itis;
}_wxf_thread_para;
static unsigned int wxf_stdcall _wxf_thread_proc(void *para)
{
	unsigned int ret = 0;
	_wxf_thread_para *psp;
	psp = (_wxf_thread_para *)para;

	if (!psp->isrun)
	{
		wxf_lock_thread(psp->itis);
	}
	if(psp->proc)
	{
		ret = psp->proc(psp->para);
	}	
	wxf_free(psp);
	return ret;
}
/********************************************************************************************* 
** Function name  : wxf_new_thread
** Arguments      : proc - 
**                  para - 
**                  flag - 
** Return         : wxf_thread 
** Date&Time      : 2013/06/22  23:19:19
** Description    : 创建一个新线程
*********************************************************************************************/ 
wxf_thread wxf_new_thread(wxf_thdl proc,const void *para,int isrun)
{
	wxf_thread thd;
	_wxf_thread_para *sp;
	sp = (_wxf_thread_para *)wxf_malloc(sizeof(_wxf_thread_para));
	thd = wxf_create_thread(_wxf_thread_proc,sp,wxf_create_suspended,NULL);
	sp->proc = proc;
	sp->para = (void*)para;
	sp->isrun = isrun;
	sp->itis = thd;
	wxf_unlock_thread(thd);
	return thd;
}

#endif //_wxf_new_thread_

#ifndef _wxf_guid_unit_
#define _wxf_guid_unit_
char *wxf_guid2str(wxf_guid Guid)
{
	wxf_guid *guid = &Guid;
	static char buf[64] = {0};
	const char *fmt = "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"; 
	if (guid == NULL) return NULL;

	_snprintf(buf,sizeof(buf),fmt,guid->Data1,guid->Data2,guid->Data3,
		guid->Data4[0],guid->Data4[1],guid->Data4[2],guid->Data4[3],
		guid->Data4[4],guid->Data4[5],guid->Data4[6],guid->Data4[7]);
	return buf;
}
void wxf_guid_uninit(wxf_guid *guid)
{
	if (guid) wxf_free(guid);
}
int wxf_guid_update(wxf_guid *guid)
{
	int ret = 0;
	if (guid == NULL) return 0;
	CoInitialize(NULL);
	if (S_OK == CoCreateGuid(guid)) ret = 1;
	CoUninitialize();
	return ret;
}
wxf_guid *wxf_guid_init()
{
	wxf_guid *guid;
	guid = (wxf_guid *)wxf_malloc(sizeof(wxf_guid));
	WXF_ASSERT(guid);
	wxf_memset(guid,0,sizeof(guid));
	CoInitialize(NULL);
	if (S_OK == CoCreateGuid(guid))
	{
		return guid;
	} 
	else
	{
		wxf_guid_uninit(guid);
		return NULL;
	}
	CoUninitialize();
}

#endif //_wxf_guid_unit_

#ifndef _wxf_ipc_unit_
#define _wxf_ipc_unit_
wxf_ipc wxf_init_ipc(int num,int cap)
{
	wxf_guid guid;
	wxf_ipc ret;
	char *pstr;
	wxf_guid_update(&guid);
	pstr = wxf_guid2str(guid);
	ret = CreateSemaphore(NULL,num,cap,pstr);
	Sleep(5);
	return ret;
}
void wxf_uninit_ipc(wxf_ipc ipc)
{
	if (ipc)
	{
		CloseHandle(ipc);
	}
}
void wxf_lock_ipc(wxf_ipc ipc)
{
	WaitForSingleObject(ipc,INFINITE);
}
void wxf_unlock_ipc(wxf_ipc ipc)
{
	long temp;
	ReleaseSemaphore(ipc,1,&temp);
}
void wxf_unlock_num_ipc(wxf_ipc ipc,int num)
{
	ReleaseSemaphore(ipc,num,NULL);
}
int wxf_locked_num(wxf_ipc ipc)
{
	long temp;
	ReleaseSemaphore(ipc,0,&temp);
	return temp;
} 
void *wxf_init_csec(void)
{
	CRITICAL_SECTION *cs; // 定义临界区对象，如果程序是OOP的，可以定义为类non-static成员  
	// 在进入多线程环境之前，初始化临界区  
	cs = (CRITICAL_SECTION *)wxf_malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(cs);
	return cs;
}
void wxf_uninit_csec(void *cs)
{
	// 释放临界区资源，当不再使用临界区时调用该函数  
	DeleteCriticalSection((CRITICAL_SECTION *)cs); 
}
void wxf_ent_csec(void *cs)
{
	EnterCriticalSection((CRITICAL_SECTION *)cs);// 进入临界区，其它线程则无法进入  
}
void wxf_lev_csec(void *cs)  
{      
	// 安全访问该区域  
	LeaveCriticalSection((CRITICAL_SECTION *)cs);  // 离开临界区，其它线程可以进入  
	wxf_free(cs);
}  
#endif	//_wxf_ipc_unit_

#ifndef _wxf_thread_test_
#define _wxf_thread_test_
wxf_gxmem sm = NULL;
wxf_gxmem ot = NULL;
wxf_handle read_xhl = NULL;
wxf_handle write_xhl = NULL;
wxf_handle thread_hdl = NULL;
int wxf_test_thread(void *para)
{
	void *pmem;
	pmem = wxf_gxmem_map(ot,WXF_GXMEM_RO);
	while(thread_hdl != NULL)
	{
		wxf_lock(read_xhl);
		puts("<read thread start>");
		puts((char *)pmem);
		puts("<read thread end>");
		wxf_unlock_xhl(write_xhl);
	}
	wxf_gxmem_unmap(pmem);
	return 0;
}
void wxf_thread_test(void *para)
{
	char name[256] = {0};
	char op[256] = {0};
	sscanf(para,"wxf_utils thread %s %s",op,name);
	if (strcmp(op,"open") == 0)
	{
		char temp[256];
		if (name[0] == '\0')
		{
			wxf_printf("<wxf_utils thread [open] NAME>\n");
			return;
		}
		if (sm != NULL)
		{
			wxf_close_obj(sm);
			sm = NULL;
		}
		sm = wxf_gxmem_create(name,4096);
		if (sm != NULL)	wxf_printf("new %s gxmem succ\n",name);
		else wxf_printf("new %s gxmem err\n",name);
		if (ot != NULL)
		{
			wxf_close_obj(ot);
			ot = NULL;
		}
		ot = wxf_gxmem_open(name,WXF_GXMEM_RO);
		if (ot != NULL)	wxf_printf("open %s gxmem succ\n",name);
		else wxf_printf("open %s gxmem err\n",name);

		wxf_sprintf(temp,"%s_read",name);
		read_xhl = wxf_create_xhl(temp,0,1);
		if (read_xhl != NULL)	wxf_printf("create %s succ\n",temp);
		else wxf_printf("create %s err\n",temp);

		wxf_sprintf(temp,"%s_write",name);
		write_xhl = wxf_create_xhl(temp,1,1);
		if (write_xhl != NULL)	wxf_printf("create %s succ\n",temp);
		else wxf_printf("create %s err\n",temp);

		thread_hdl = wxf_new_thread(wxf_test_thread,NULL,TRUE);
		if (thread_hdl != NULL)	wxf_printf("create read thread succ\n");
		else wxf_printf("create read thread err\n");
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
		if (read_xhl != NULL)
		{
			wxf_close_obj(read_xhl);
			read_xhl = NULL;
		}
		if (write_xhl != NULL)
		{
			wxf_close_obj(write_xhl);
			write_xhl = NULL;
		}
		if (thread_hdl != NULL)
		{
			wxf_close_obj(thread_hdl);
			thread_hdl = NULL;
		}
	}
	else if (strcmp(op,"read") == 0)
	{
		if (ot != NULL)
		{
			void *pmem;
			pmem = wxf_gxmem_map(ot,WXF_GXMEM_RO);
			wxf_print("<read gxmem>:%s\n",(char *)pmem);
			wxf_gxmem_unmap(pmem);
		}
	}
	else if (strcmp(op,"write") == 0)
	{

		if (sm != NULL)
		{
			void *pmem;
			if (name[0] == '\0')
			{
				wxf_printf("<wxf_utils thread [write] CONTENT>\n");
			}
			pmem = wxf_gxmem_map(sm,WXF_GXMEM_RW);
			wxf_lock(write_xhl);
			wxf_memcpy(pmem,name,wxf_strlen(name)+1);
			wxf_unlock_xhl(read_xhl);
			wxf_gxmem_unmap(pmem);
		}
		Sleep(50);
	}
	else if (strcmp(op,"help") == 0)
	{
		wxf_printf("<wxf_utils thread [help]>\n");
		wxf_printf("<wxf_utils thread [open] NAME>\n");		
		wxf_printf("<wxf_utils thread [read]>\n");
		wxf_printf("<wxf_utils thread [write] CONTENT>\n");
		wxf_printf("<wxf_utils thread [close]>\n");
	}
	else 
	{
		wxf_printf("<Don't define \"%s\">\n",op);
	}
}
#endif //_wxf_thread_test_
