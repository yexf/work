/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_mp3st.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/05
**
** Description:
**************************************************************************************/ 
#include "wxf_mp3st.h"
void wxf_mf_deinit(wxf_mfst *pmfst)
{
	if (pmfst != NULL)
	{
		wxf_free(pmfst->buf);
		wxf_free(pmfst);
	}
}
wxf_mfst *wxf_mf_parse(const char *filename)
{
	wxf_mfst *ret = NULL;
	wxf_file_t fp;
	int temp;
	wxf_uint32 size;

	fp = wxf_fopen(filename,WXF_OM_READ);
	if (fp == WXF_ERR_FILE)
	{
		printf("Can't Open File :%s\n",filename);
	}
	size = wxf_fsize(fp);
	temp = sizeof(wxf_mfst);
	ret = (wxf_mfst *)wxf_malloc(temp);
	WXF_ASSERT(ret);
	wxf_memset(ret,0,temp);

	ret->fd_len = size;
	ret->buf = (wxf_uint8 *)wxf_malloc(size);
	WXF_ASSERT(ret->buf);
	wxf_fread(ret->buf,1,size,fp);
	wxf_fclose(fp);

	ret->mih2 = (wxf_mih2 *)ret->buf;
	if (wxf_memcmp(ret->mih2->Tag,"ID3",3))
	{
		ret->mih2 = NULL;
		size = 0;
	}
	else
	{
		size = (ret->mih2->Size[0] & 0x7f) << 21;
		size += (ret->mih2->Size[1] & 0x7f) << 14;
		size += (ret->mih2->Size[2] & 0x7f) << 7;
		size += (ret->mih2->Size[3] & 0x7f);
		ret->id3v2_len = size;
	}
	ret->mfh = (wxf_mfh *)(ret->buf + size - 10);

	return ret;
}