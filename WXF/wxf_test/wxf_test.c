#include <wxf_comm.h>
#include "wxf_test.h"
#include "wxf_config.h"
int wxf_test_none(void *para)
{
	int i,num = wxf_anum(tsf_ent);
	wxf_printf("wxf_test help\n");
	for (i = 0;i < num;i++)
	{
		wxf_printf("%d:%s\n",tsf_ent[i].type,tsf_ent[i].str);
	}
	return wxf_succ;
}
int wxf_test_do(int type,char *para)
{
	wxf_tsf_ent *pte;
	pte = (wxf_tsf_ent*)tsf_ent + type;
	if (pte->hdl)
		return pte->hdl(para);
	else
	{
		wxf_printf("Don't Binding %s's Handle",pte->str);
		return wxf_succ;
	}
}
int wxf_test_match(char *para)
{
	wxf_tsf_ent *pte = (wxf_tsf_ent*)tsf_ent;
	int i,num = wxf_anum(tsf_ent);
	for (i = 0;i < num;i++)
	{
		if (strstr(para,pte->str))
		{
			return wxf_test_do(pte->type,para);
		}
		pte++;
	}
	wxf_printf("Do not define \"%s\"",para);
	return wxf_succ;
}
int wxf_test_default(char *para)
{
	int ret = wxf_succ;
	char *pstr = (char *)para;
	
	if (strcmp(para,"exit") == 0)
	{
		wxf_printf("exit wxf_test\n");
		return wxf_err;
	}
	else if (strcmp(para,"help") == 0)
	{
		return wxf_test_none(para);
	}
	else
	{
		ret = wxf_test_match(para);
		return ret;
	}
}

int wxf_test(int type,char *para)
{
	int ret = wxf_succ;
	
	if (type == EM_TEST_NONE)
	{
		return wxf_test_none(para);
	}
	if (type >= EM_TEST_CONUT || type < EM_TEST_NONE)
	{
		return wxf_test_default(para);
	}
	else 
	{
		return wxf_test_do(type,NULL);
	}
}