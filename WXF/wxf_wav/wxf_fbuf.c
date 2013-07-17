#include <wxf_comm.h>
#include "wxf_fbuf.h"

wxf_fbuf *wxf_create_fbuf()
{
	wxf_fbuf *ret;
	int temp = sizeof(wxf_fbuf);
	ret = (wxf_fbuf *)wxf_malloc(temp);
	wxf_memset(ret,0,temp);
	return ret;
}
void wxf_uninit_fbuf(wxf_fbuf *buf)
{
	if (buf)
	{
		if (buf->pdata)
		{
			wxf_free(buf->pdata);
		}
		wxf_free(buf);
	}
}
wxf_fbuf *wxf_init_fbuf(wxf_fbuf *buf,int cap)
{
	wxf_fbuf *temp = buf;
	if (temp == NULL)
		temp = wxf_create_fbuf();
	if (temp == NULL) return NULL;

	if (temp->cap != cap)
	{
		if (temp->pdata != NULL)
		{
			wxf_free(temp);
		}
		temp->pdata = wxf_malloc(cap);
		temp->cap = cap;
	}
	temp->size = 0;
	temp->pcur = temp->pdata;
	temp->pread = 0;
	temp->pwrite = 0;
	temp->status = WXF_FB_EMPTY;
	return temp;
}
int wxf_read_fbuf(wxf_fbuf *buf,void *pDst,wxf_uint32 num)
{
	int ret,len;
	char *pdst = (char *)pDst;

	if (buf->status == WXF_FB_EMPTY
		|| buf->status == WXF_FB_UNINIT)
	{
		return wxf_err;
	}
	if (buf->size < num)
	{
		len = buf->size;
		ret = len;
		buf->status = WXF_FB_EMPTY;
	}
	else if(buf->size == num)
	{
		len = num;
		ret = wxf_succ;	
		buf->status = WXF_FB_EMPTY;
	}
	else if (buf->size > num)
	{
		len = num;
		ret = wxf_succ;
		buf->status = WXF_FB_UNFULL;
	}
	buf->size -= len;
	buf->pcur = buf->pdata + buf->pread;

	if (buf->pread + len > buf->cap)
	{
		int temp = buf->cap - buf->pread;
		buf->pread = len - temp;
		wxf_memcpy(pdst,buf->pcur,temp);
		wxf_memcpy(pdst+temp,buf->pdata,buf->pread);		
	}
	else
	{
		buf->pread += len;
		wxf_memcpy(pdst,buf->pcur,len);
	}
	return ret;
}
int wxf_write_fbuf(wxf_fbuf *buf,void *pSrc,wxf_uint32 num) 
{
	int ret,len;
	char *psrc = (char *)pSrc;
	if (buf->status == WXF_FB_FULL
		|| buf->status == WXF_FB_UNINIT)
	{
		return wxf_err;
	}

	if (buf->size + num > buf->cap)
	{
		len = buf->cap - buf->size;
		ret = len;
		buf->status = WXF_FB_FULL;
	}
	else if (buf->size + num == buf->cap)
	{
		len = num;
		ret = wxf_succ;
		buf->status = WXF_FB_FULL;
	}
	else if (buf->size + num < buf->cap)
	{
		len = num;
		ret = wxf_succ;
		buf->status = WXF_FB_UNFULL;
	}

	buf->size += len;
	buf->pcur = buf->pdata + buf->pwrite;

	if (buf->pwrite + len > buf->cap)
	{
		int temp = buf->cap - buf->pwrite;
		buf->pwrite = len - temp;
		wxf_memcpy(buf->pcur,psrc,temp);
		wxf_memcpy(buf->pdata,psrc+temp,buf->pwrite);		
	}
	else
	{
		buf->pwrite += len;
		wxf_memcpy(buf->pcur,psrc,len);
	}
	return ret;
}