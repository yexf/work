/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : main.cpp
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include "wxf_test.h"
const char *str_tsf = "=>>";

int put_tsf(char *pdst)
{
	int len,num = -1; 
	wxf_printf("\n%s",str_tsf);
	wxf_fgets(pdst,256,stdin);
	len = wxf_strlen(pdst);
	while (len == 1)
	{
		wxf_printf("%s",str_tsf);
		wxf_fgets(pdst,256,stdin);
		len = wxf_strlen(pdst);
	}
	pdst[len-1] = '\0';
	if (isdigit(pdst[0]))
	{
		num = atoi(pdst);	
	}
	return num;
}
void main(int argn,char **args)
{
	char str_buf[256];
	void *input_para = NULL;
	int temp,ret = wxf_succ;

	wxf_test(0,NULL);
	while(wxf_err != ret)
	{
		temp = put_tsf(str_buf);
		ret = wxf_test(temp,str_buf);
	}
}