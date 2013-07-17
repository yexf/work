/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_test.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_test_h_
#define _wxf_test_h_

typedef int (wxf_test_handle)(char *para);
typedef wxf_test_handle *wxf_th;
typedef struct
{
	int type;
	char *str;
	wxf_th hdl;
}wxf_tsf_ent;

#ifdef __cplusplus
extern "C" {
#endif
	int wxf_test(int type,char *para);
#ifdef __cplusplus
}
#endif

#endif /*_wxf_test_h_*/
