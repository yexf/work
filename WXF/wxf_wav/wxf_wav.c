/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_wav.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/27
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include "wxf_wav.h"

wxf_wft *wxf_wft_open(const char *filename)
{
	wxf_file_t fp;
	wxf_wft *ret;
	uint len;

	fp = wxf_fopen(filename,WXF_OM_READ);
	if (fp == WXF_ERR_FILE) 
		return NULL;
	len = wxf_fsize(fp);

	ret = (wxf_wft *)wxf_malloc(len);
	if (ret == NULL) 
		goto _err_ret;
	if (!wxf_fread_succ(fp,ret,len))
		goto _err_ret;
	if (memcmp(ret->cRiffFlag,"RIFF",4)
		|| memcmp(ret->cWaveFlag,"WAVE",4)
		|| memcmp(ret->cFmtFlag,"fmt ",4)
		|| memcmp(ret->cDataFlag,"data",4))
		goto _err_ret;
	wxf_fclose(fp);
	return ret;
_err_ret:
	wxf_fclose(fp);
	if (ret) wxf_free(ret);
	return NULL;
}
