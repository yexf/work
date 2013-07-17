/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_ipc.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/28
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_ipc_h_
#define _wxf_ipc_h_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>
#include <WindowsX.h>
#include <process.h> 
#include <objbase.h>
#endif


/* TODO: ANSI C MULTITHREAD */
typedef int (wxf_thread_handle)(void *para);
typedef wxf_thread_handle *						wxf_thdl;

typedef	wxf_handle								wxf_thread;
typedef GUID									wxf_guid;
typedef wxf_handle								wxf_ipc;

#define wxf_create_suspended					CREATE_SUSPENDED
#define wxf_create_activte						(0)
#define wxf_lock_flag							1
#define wxf_unlock_flag							0
#define wxf_no_timeout_flag						INFINITE

#define wxf_create_thread(cb,arg,flag,rid)		(HANDLE)CreateThread(NULL,0,cb,arg,flag,rid)
#define wxf_close_thread(val)					CloseHandle(val)
#define wxf_lock_thread(thd)					SuspendThread(thd)
#define wxf_unlock_thread(thd)					ResumeThread(thd)
#define wxf_set_thread(thd,n)					SetThreadPriority(thd,n);
#define wxf_exit_thread(dwExitCode)				ExitThread(dwExitCode)
#define wxf_kill_thread(thd,dwExitCode)			TerminateThread(thd,dwExitCode)
/* TODO: END */

#define wxf_wait_single(obj,ntime)				WaitForSingleObject(obj,ntime)
#define wxf_wait_multi(num,obj,flag,ntime)		WaitForMultipleObjects(num,obj,flag,ntime)
#define wxf_wait_obj(obj)						wxf_wait_single(obj,wxf_no_timeout_flag);
#define wxf_close_obj(obj)						CloseHandle(obj)
#define wxf_lock(obj)							wxf_wait_obj(obj)

#define wxf_create_xhl(name,num,cap)			CreateSemaphore(NULL,num,cap,name)
#define wxf_unlock_semaphore(xhl,num,pint)		ReleaseSemaphore(xhl,num,pint)
#define wxf_open_xhl(name)						OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,name)
#define wxf_unlock_xhl(xhl)						ReleaseSemaphore(xhl,1,NULL)

//#define wxf_create_mutex(name,lock)				CreateMutex(NULL,lock,name)
//#define wxf_open_mutex(name)					OpenMutex(MUTEX_ALL_ACCESS,FALSE,name)
//#define wxf_unlock_mutex(mutex)					ReleaseMutex(mutex);

/* TODO: END */

#ifdef __cplusplus
extern "C" {
#endif
	wxf_thread wxf_new_thread(wxf_thdl proc,const void *para,int isrun);
	char *wxf_guid2str(wxf_guid guid);
	void wxf_guid_uninit(wxf_guid *guid);
	int wxf_guid_update(wxf_guid *guid);
	wxf_guid *wxf_guid_init();

	wxf_ipc wxf_init_ipc(int num,int cap);
	void wxf_uninit_ipc(wxf_ipc ipc);
	void wxf_lock_ipc(wxf_ipc ipc);
	void wxf_unlock_ipc(wxf_ipc ipc);
	int wxf_locked_num(wxf_ipc ipc);
	void wxf_unlock_num_ipc(wxf_ipc ipc,int num);

	void *wxf_init_csec(void);
	void wxf_uninit_csec(void *cs);
	void wxf_ent_csec(void *cs);
	void wxf_lev_csec(void *cs);


	void wxf_thread_test(void *para);

	
#ifdef __cplusplus
}
#endif

#endif /*_wxf_ipc_h_*/
