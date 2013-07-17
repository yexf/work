/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : main.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/05
**
** Description:
**************************************************************************************/ 
#include "wxf_mp3st.h"

void Test(char *para)
{
	wxf_mfst *temp;
	temp = wxf_mf_parse(para);
	wxf_mf_deinit(temp);
}

void main(int argc,char **argv)
{
	Test(argv[1]);
}