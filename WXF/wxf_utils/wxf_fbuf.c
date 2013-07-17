#include <wxf_comm.h>
#include "wxf_utils.h"
#ifndef _wxf_fbuf_unit_
#define _wxf_fbuf_unit_
wxf_fbuf *wxf_create_fbuf()
{
	wxf_fbuf *ret;
	int temp = sizeof(wxf_fbuf);
	ret = (wxf_fbuf *)wxf_malloc(temp);
	wxf_memset(ret,0,temp);
	return ret;
}
void wxf_clear_fbuf(wxf_fbuf *fbuf)
{
	fbuf->size = 0;
	fbuf->pread = fbuf->pwrite;
	if (fbuf->fbem != EM_FB_EMPTY)
	{
		wxf_lock_ipc(fbuf->hread);
		fbuf->fbem = EM_FB_EMPTY;
	}
	wxf_unlock_ipc(fbuf->hwrite);	
}
void wxf_uninit_fbuf(wxf_fbuf *buf)
{
	if (buf)
	{
		if (buf->pdata)
		{
			wxf_free(buf->pdata);
		}
		wxf_uninit_ipc(buf->hread);
		wxf_uninit_ipc(buf->hwrite);
		wxf_free(buf);
	}
}
wxf_fbuf *wxf_init_fbuf(int cap)
{
	wxf_fbuf *temp;
	temp = wxf_create_fbuf();
	if (temp == NULL) return NULL;

	temp->hread = wxf_init_ipc(0,1);
	temp->hwrite = wxf_init_ipc(1,1);
	temp->pdata = (char *)wxf_malloc(cap);
	temp->fbem = EM_FB_EMPTY;
	temp->cap = cap;
	temp->size = 0;
	temp->pread = 0;
	temp->pwrite = 0;
	
	return temp;
}
void wxf_bread_fbuf(wxf_fbuf *fbuf,void *pDst,wxf_uint32 num)
{
	wxf_uint32 pread,temp;
	char *psrc = NULL;
	char *pdst = (char *)pDst;

	pread = fbuf->pread % fbuf->cap;
	psrc = fbuf->pdata + pread;
	if (pread + num > fbuf->cap)
	{
		temp = fbuf->cap - pread;
		wxf_memcpy(pdst,psrc,temp);
		wxf_memcpy(pdst+temp,fbuf->pdata,num-temp);
	}
	else
	{
		wxf_memcpy(pdst,psrc,num);
	}
}
void wxf_bwrite_fbuf(wxf_fbuf *fbuf,void *pSrc,wxf_uint32 num) 
{
	wxf_uint32 pwrite,temp;
	char *psrc = (char *)pSrc;
	char *pdst = NULL;

	pwrite = fbuf->pwrite % fbuf->cap;
	pdst = fbuf->pdata + pwrite;
	if (pwrite + num > fbuf->cap)
	{
		temp = fbuf->cap - pwrite;
		wxf_memcpy(pdst,psrc,temp);
		wxf_memcpy(fbuf->pdata,psrc+temp,num-temp);
	}
	else
	{
		wxf_memcpy(pdst,psrc,num);
	}
}
int wxf_fread_fbuf(wxf_fbuf *buf,void *pDst,wxf_uint32 num)
{
	uint len,temp;
	wxf_lock_ipc(buf->hread);
	temp = buf->size;
	len = wxf_min(temp,num);
	wxf_bread_fbuf(buf,pDst,len);
	buf->size -= len;
	buf->pread += len;
	wxf_unlock_ipc(buf->hread);
	if (buf->fbem == EM_FB_UNINIT)
	{
		wxf_unlock_ipc(buf->hwrite);
		return len;
	}
	if (buf->fbem == EM_FB_FULL)
	{
		buf->fbem = EM_FB_UNFULL;
		wxf_unlock_ipc(buf->hwrite);
	}
	if (buf->size == 0)
	{
		buf->fbem = EM_FB_EMPTY;
		wxf_lock_ipc(buf->hread);
	}
	Sleep(1);
	return len;
	
}
int wxf_fwrite_fbuf(wxf_fbuf *buf,void *pSrc,wxf_uint32 num) 
{
	uint len,temp;
	wxf_lock_ipc(buf->hwrite);
	temp = buf->cap - buf->size;
	len = wxf_min(temp,num);
	wxf_bwrite_fbuf(buf,pSrc,len);
	buf->size += len;
	buf->pwrite += len;
	wxf_unlock_ipc(buf->hwrite);
	if (buf->fbem == EM_FB_UNINIT)
	{
		wxf_unlock_ipc(buf->hread);
		return num;
	}
	if (buf->fbem == EM_FB_EMPTY)
	{
		buf->fbem = EM_FB_UNFULL;
		wxf_unlock_ipc(buf->hread);
	}
	if (buf->size == buf->cap)
	{
		buf->fbem = EM_FB_FULL;
		wxf_lock_ipc(buf->hwrite);
	}
	Sleep(1);
	return len;
	
}
int wxf_read_fbuf(wxf_fbuf *buf,void *pDst,wxf_uint32 num)
{
	uint len = 0;
	char *pcur = (char *)pDst;
	while (len < num)
	{
		len += wxf_fread_fbuf(buf,pcur+len,num-len);
	}
	return wxf_succ;
}
int wxf_write_fbuf(wxf_fbuf *buf,void *pSrc,wxf_uint32 num) 
{
	uint len = 0;
	char *pcur = (char *)pSrc;
	while (len < num)
	{
		len += wxf_fwrite_fbuf(buf,pcur+len,num-len);
	}
	return wxf_succ;
}

#endif //_wxf_fbuf_unit_

int wxf_fbuf_test_td(void *para)
{
	wxf_fbuf *fbuf;
	char buf[256];
	int temp;
	fbuf = (wxf_fbuf *)para;
	wxf_printf("\t<start test thread>\n");
	while(fbuf->fbem != EM_FB_UNINIT)
	{
		wxf_printf("\t<read fbuf begin>\n");
		temp = wxf_fread_fbuf(fbuf,buf,256);
		buf[temp] = '\0';
		wxf_printf("\t<read fbuf end>\n");
		wxf_printf("\t<printf fbuf>:%s\n",buf);
	}
	wxf_printf("\t<stop test thread>\n");
	return 0;
}
void wxf_fbuf_test()
{
	char pstr[256];
	char op[256];
	char op1[256];
	char op2[256];
	int ret = wxf_succ;
	wxf_thread td;
	wxf_fbuf *fbuf;
	wxf_print("start fbuf\n");
	while(ret == wxf_succ)
	{
		wxf_printf("%s",WXF_TSF);
		wxf_fgets(pstr,256,stdin);
		sscanf(pstr,"%s %s %s",op,op1,op2); 
		if (!strcmp(op,"init"))
		{
			fbuf = wxf_init_fbuf(atoi(op1));
			if (fbuf != NULL) 
			{
				wxf_print("wxf_fbuf cap:%s init succ!\n",op1);
				td = wxf_new_thread(wxf_fbuf_test_td,fbuf,FALSE);
				if (td != NULL)
				{
					wxf_print("wxf_fbuf thread init succ!\n");
				}
				else 
				{
					wxf_print("wxf_fbuf thread init err!\n");
				}
				wxf_unlock_thread(td);
				Sleep(50);
			}
			else 
			{
				wxf_print("wxf_fbuf init err!\n");
			}
		}
		else if (!strcmp(op,"write"))
		{
			wxf_printf("<write fbuf begin>\n");
			if (atoi(op2) == 0)
			{				
				wxf_write_fbuf(fbuf,op1,strlen(op1)+1);							
			}
			else
			{
				int i,temp = atoi(op2);
				for (i = 0;i < temp;i++)
				{
					wxf_sprintf(pstr,"%s %d",op1,i);
					wxf_write_fbuf(fbuf,pstr,strlen(pstr)+1);
				}		
			}
			Sleep(5);	
			wxf_printf("<write fbuf end>\n");
		}
		else if (!strcmp(op,"uninit"))
		{
			fbuf->fbem = EM_FB_UNINIT;
			wxf_write_fbuf(fbuf,"delete",7);
			wxf_lock(td);\
			Sleep(5);
			wxf_close_obj(td);
			td = NULL;
			wxf_uninit_fbuf(fbuf);
			fbuf = NULL;
		}
		else if (!strcmp(op,"sx"))
		{
			wxf_print("cap:%d\n",fbuf->cap);
			wxf_print("size:%d\n",fbuf->size);
			wxf_print("free num:%d\n",fbuf->cap - fbuf->size);
			wxf_print("read count:%d\n",fbuf->pread);
			wxf_print("write count:%d\n",fbuf->pwrite);
		}
		else if (!strcmp(op,"exit"))
		{
			ret = wxf_err;
		}
		else if (!strcmp(op,"help"))
		{
			/*wxf_printf("<1:[new] CAP SIZE>\n");
			wxf_printf("<2:[sx] look for attribute>\n");
			wxf_printf("<3:[write] CONTENT[< CAP] [num]>\n");
			wxf_printf("<4:[post] CONTENT [num]>\n");
			wxf_printf("<5:[delete]>\n");
			wxf_printf("<6:[exit]>\n");*/

		}
		else
		{
			wxf_printf("<Don't define \"%s\">\n",op);
		}
		wxf_memset(op,0,256);
		wxf_memset(op1,0,256);
		wxf_memset(op2,0,256);
	}
	wxf_print("stop fqst\n");
}