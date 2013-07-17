/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_utils.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_utils_h_
#define _wxf_utils_h_

#define WXF_MAX_NAME							256

typedef enum wxf_common_status_em
{
	EM_INIT = 0,
	EM_READY,
	EM_START,
	EM_RUN,
	EM_PAUSE,
	EM_STOP,
	EM_UNINIT,
}wxf_comms_em;

#ifdef __cplusplus
extern "C" {
#endif

	int Low2Up(unsigned char *str_in,unsigned char *str_out,unsigned int len);
	unsigned StrLen( const char * str );
	int TypeCmp(unsigned char *pFileName, unsigned char *pExt);
	unsigned int GetN_LSB(unsigned int nNumber,unsigned char *address);
	void R_memset (void *output,int value,unsigned int len);
	void R_memcpy (void *output,void *input,unsigned int len);
	void MSB2LSB(unsigned char *p_src,unsigned char *p_dst,unsigned long len);
	void LSB2MSB(unsigned char *p_src,unsigned char *p_dst,unsigned long len);

	int DeleteSameItem(void *p_info,int num,int size,ccb pf);
	int SaveSameItem(void *src,int src_num,void *dst,int dst_num,int size,ccb pf);
	int MergeSameItem(void *src,int src_num,void *dst,int dst_num,int size,ccb pf);
#ifdef WXF_TEST
	int wxf_utils_test(char *para);
#endif
#ifdef __cplusplus
}
#endif



#ifndef WXF_LIB
#pragma comment(lib,"wxf_utils.lib")
#endif //_DEBUG

#include "wxf_ipc.h"
#include "wxf_gxmem.h"
#include "wxf_fq.h"
#include "wxf_fbuf.h"


#endif /*_wxf_utils_h_*/
