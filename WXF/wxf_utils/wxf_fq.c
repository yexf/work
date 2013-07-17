/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_fq.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/23
**
** Description:
**************************************************************************************/ 
#include <wxf_comm.h>
#include "wxf_utils.h"
#include "wxf_gxmem.h"
#include "wxf_fq.h"

static wxf_fqst *wxf_fqst_create()
{
	wxf_fqst *pret;
	int temp = sizeof(wxf_fqst);
	pret = (wxf_fqst *)wxf_malloc(temp);
	wxf_memset(pret,0,temp);
	return pret;
}
int wxf_fqst_read(wxf_fqst *fqst,void *pdata,wxf_uint32 num)
{
	wxf_uint32 len,size,cap;
	char *pstr,*pcur;
	wxf_fqid *fqid;
	wxf_fqsx *fqsx;
	wxf_fqmem *fqmem;

	fqid = &fqst->fqts->fqid;
	fqsx = &fqst->fqts->fqsx;
	fqmem = &fqst->fqmem;
	size = fqsx->size;
	cap = fqsx->cap;

	len = wxf_min(num,size);
	pstr = (char *)fqmem->pdata;
	pcur = pstr + (fqsx->pread%cap)*size;

	wxf_lock(fqid->rxhl);
	wxf_memcpy(pdata,pcur,len);
	fqsx->pread++;
	fqsx->num--;
	wxf_unlock_xhl(fqid->wxhl);
	return wxf_succ;
}
int wxf_fqst_write(wxf_fqst *fqst,void *pdata,wxf_uint32 num)
{
	wxf_uint32 len,size,cap;
	char *pstr,*pcur;
	wxf_fqid *fqid;
	wxf_fqsx *fqsx;
	wxf_fqmem *fqmem;

	fqid = &fqst->fqts->fqid;
	fqsx = &fqst->fqts->fqsx;
	fqmem = &fqst->fqmem;
	size = fqsx->size;
	cap = fqsx->cap;

	len = wxf_min(num,size);
	pstr = (char *)fqmem->pdata;
	pcur = pstr + (fqsx->pwrite%cap)*size;

	wxf_lock(fqid->wxhl);
	wxf_memcpy(pcur,pdata,len);
	fqsx->pwrite++;
	fqsx->num++;
	wxf_unlock_xhl(fqid->rxhl);
	return wxf_succ;
}
void wxf_fqst_uninit(wxf_fqst *fqst)
{
	wxf_fqmem *fqmem;
	if (fqst == NULL) return;
	fqmem = &fqst->fqmem;

	wxf_gxmem_unmap(fqmem->pmem);
	wxf_close_obj(fqmem->gxmem);
	wxf_free(fqst);
}
static wxf_fqts *wxf_fqts_init(wxf_fqts *fqts,wxf_uint32 cap,wxf_uint32 size,wxf_fqm_em mode)
{
	wxf_uint32 temp;
	char *pstr,buf[256];
	void *pmem;
	wxf_fqid *fqid;
	wxf_fqsx *fqsx;

	fqid = &fqts->fqid;
	fqsx = &fqts->fqsx;
	temp = sizeof(wxf_fqts);

	fqsx->cap = cap;
	fqsx->size = size;
	fqsx->num = 0;
	fqsx->pread = 0;
	fqsx->pwrite = 0;

	fqts->bsize = cap * size;
	fqts->bcap = fqts->bsize + temp;
	fqts->mode = mode;

	wxf_guid_update(&fqid->guid);
	pstr = wxf_guid2str(fqid->guid);
	fqid->gxmem = wxf_gxmem_create(pstr,fqts->bcap);
	
	wxf_sprintf(buf,"%s_rxhl",pstr);
	fqid->rxhl = wxf_create_xhl(buf,0,cap);
	wxf_sprintf(buf,"%s_wxhl",pstr);
	fqid->wxhl = wxf_create_xhl(buf,cap,cap);

	pmem = wxf_gxmem_map(fqid->gxmem,WXF_GXMEM_RW);
	wxf_memcpy(pmem,fqts,temp);
	wxf_gxmem_unmap(pmem);

	return fqts;
}
wxf_fqst *wxf_fqst_open(wxf_guid guid)
{
	char *pstr;
	wxf_fqst *fqst;
	wxf_fqmem *fqmem;

	fqst = wxf_fqst_create();
	pstr = wxf_guid2str(guid);
	fqmem = &fqst->fqmem;
	fqmem->gxmem = wxf_gxmem_open(pstr,WXF_GXMEM_RW);
	fqmem->pmem = wxf_gxmem_map(fqmem->gxmem,WXF_GXMEM_RW);
	fqmem->pdata = (char *)fqmem->pmem + sizeof(wxf_fqts);
	fqst->fqts = (wxf_fqts *)fqmem->pmem;
	fqst->fqread = wxf_fqst_read;
	fqst->fqwrite = wxf_fqst_write;
	fqst->fqstatus = EM_INIT;
	return fqst;
}
wxf_fqst *wxf_fqst_init(wxf_uint32 cap,wxf_uint32 size,wxf_fqm_em mode)
{
	wxf_fqts fqts;
	wxf_fqts_init(&fqts,cap,size,mode);
	return wxf_fqst_open(fqts.fqid.guid);
}

void wxf_fqst_close(wxf_fqst *fqst)
{
	wxf_fqts *fqts;

	if (fqst == NULL) return;
	fqts = fqst->fqts;

	if (fqts->fqsend != NULL)
	{
		fqts->fqsend_status = EM_UNINIT;
		wxf_lock(fqts->fqsend);
		wxf_close_obj(fqts->fqsend);
	}
	if (fqts->fqjs != NULL)
	{
		fqts->fqjs_status = EM_UNINIT;
		wxf_lock(fqts->fqjs);
		wxf_close_obj(fqts->fqjs);
	}
	wxf_close_obj(fqts->fqid.gxmem);
	wxf_close_obj(fqts->fqid.rxhl);
	wxf_close_obj(fqts->fqid.wxhl);
	wxf_fqst_uninit(fqst);
}
int wxf_fqst_test_td(void *para)
{
	wxf_fqst *fqst;
	char buf[256];
	fqst = (wxf_fqst *)para;
	wxf_printf("\t<start test thread>\n");
	while(fqst->fqstatus != EM_UNINIT)
	{
		wxf_printf("\t<read fqst begin>\n");
		wxf_fqst_read(fqst,buf,256);
		wxf_printf("\t<read fqst end>\n");
		wxf_printf("\t<printf read>:%s\n",buf);
	}
	wxf_printf("\t<stop test thread>\n");
	return 0;
}
void wxf_fqst_test()
{
	char pstr[256];
	char op[256];
	char psrc[256];
	char pdst[256];
	int ret = wxf_succ;
	wxf_thread td;
	wxf_fqst *fqst;
	wxf_print("start fqst\n");
	while(ret == wxf_succ)
	{
		wxf_printf("%s",WXF_TSF);
		wxf_fgets(pstr,256,stdin);
		sscanf(pstr,"%s %s %s",op,psrc,pdst); 
		if (!strcmp(op,"new"))
		{
			fqst = wxf_fqst_init(atoi(psrc),atoi(pdst),EM_FQMEM_GXMEM);
			if (fqst != NULL) 
			{
				wxf_print("wxf_fqst cap:%s,size:%s init succ!\n",psrc,pdst);
				td = wxf_new_thread(wxf_fqst_test_td,fqst,FALSE);
				if (td != NULL)
				{
					wxf_print("wxf_fqst thread init succ!\n");
				}
				else 
				{
					wxf_print("wxf_fqst thread init err!\n");
				}
				wxf_unlock_thread(td);
				Sleep(50);
			}
			else 
			{
				wxf_print("wxf_fqst init err!\n");
			}
		}
		else if (!strcmp(op,"write"))
		{
			wxf_printf("<write fqst begin>\n");
			if (atoi(pdst) == 0)
			{				
				wxf_fqst_write(fqst,psrc,strlen(psrc)+1);							
			}
			else
			{
				int i,temp = atoi(pdst);
				wxf_lock_thread(td);
				for (i = 0;i < temp;i++)
				{
					wxf_sprintf(pstr,"%s %d",psrc,i);
					wxf_fqst_write(fqst,pstr,strlen(pstr)+1);
				}
				wxf_unlock_thread(td);
			}
			Sleep(50);	
			wxf_printf("<write fqst end>\n");
		}
		else if (!strcmp(op,"post"))
		{
			wxf_printf("<post fqst begin>\n");
			if (atoi(pdst) == 0)
			{
				wxf_fqst_write(fqst,psrc,strlen(psrc)+1);
			}
			else
			{
				int i,temp = atoi(pdst);
				for (i = 0;i < temp;i++)
				{
					wxf_sprintf(pstr,"%s %d",psrc,i);
					wxf_fqst_write(fqst,pstr,strlen(pstr)+1);
				}				
			}
			Sleep(500);
			wxf_printf("<post fqst end>\n");
		}
		else if (!strcmp(op,"delete"))
		{
			fqst->fqstatus = EM_UNINIT;
			wxf_fqst_write(fqst,"delete",7);
			wxf_lock(td);
			wxf_close_obj(td);
			td = NULL;
			wxf_fqst_uninit(fqst);
			fqst = NULL;
		}
		else if (!strcmp(op,"sx"))
		{
			wxf_print("num:%d\n",WXF_FQST_NUM(fqst));
			wxf_print("cap:%d\n",WXF_FQST_CAP(fqst));
			wxf_print("size:%d\n",WXF_FQST_SIZE(fqst));
			wxf_print("free num:%d\n",WXF_FQST_FREENUM(fqst));
			wxf_print("read count:%d\n",WXF_FQST_READCOUNT(fqst));
			wxf_print("write count:%d\n",WXF_FQST_WRITECOUNT(fqst));
		}
		else if (!strcmp(op,"exit"))
		{
			ret = wxf_err;
		}
		else if (!strcmp(op,"help"))
		{
			wxf_printf("<1:[new] CAP SIZE>\n");
			wxf_printf("<2:[sx] look for attribute>\n");
			wxf_printf("<3:[write] CONTENT[< CAP] [num]>\n");
			wxf_printf("<4:[post] CONTENT [num]>\n");
			wxf_printf("<5:[delete]>\n");
			wxf_printf("<6:[exit]>\n");

		}
		else
		{
			wxf_printf("<Don't define \"%s\">\n",op);
		}
		wxf_memset(op,0,256);
		wxf_memset(psrc,0,256);
		wxf_memset(pdst,0,256);
	}
	wxf_print("stop fqst\n");
}