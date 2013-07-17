/************************************************************************************** 
** Copyright (C) 2011 Multak,Inc. All rights reserved
**
** Filename : wxf_utils.c
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/22
**
** Description:
**************************************************************************************/ 
#include "wxf_comm.h"
#include "wxf_utils.h"
#ifndef _wxf_old_mk_8167_
#define _wxf_old_mk_8167_
/********************************************************************************************* 
** Function name  : MergeUnsameItem
** Arguments      : src - 
**                  src_num - 
**                  dst - 
**                  dst_num - 
**                  size - 
**                  pf - 
** Return         : int 
** Date&Time      : 2012-09-06  13:13:42
** Description    : dst = (src ∩ dst) ∈ dst
*********************************************************************************************/ 
int SaveSameItem(void *src,int src_num,void *dst,int dst_num,int size,ccb pf)
{
	int *table;
	char *tmp,*p_dst;
	char *src_info;
	char *dst_info;
	int i,j,temp;
	int ret_cnt = 0;
	int flag = 1;

	src_info = (char *)src;
	dst_info = (char *)dst;
	p_dst = dst_info;
	temp = sizeof(int) * dst_num;
	table = (int *)wxf_malloc(temp);
	WXF_ASSERT(table);

	for (i = 0;i < dst_num;i++)
	{			
		for(j = 0;j < src_num;j++)
		{
			tmp = src_info + j * size;
			if (pf(tmp,p_dst) == 0)
			{
				flag = 0;
				break;
			}
		}
		if (flag == 0)
		{
			table[ret_cnt] = i;
			ret_cnt++;
		}
		p_dst += size;
		flag = 1;
	}
	for (i = 0;i < ret_cnt;i++)
	{
		tmp = dst_info + table[i] * size;
		wxf_memcpy(dst_info+i*size,tmp,size);
	}
	wxf_free(table);
	return ret_cnt;
}
/********************************************************************************************* 
** Function name  : MergeSameItem
** Arguments      : src - 
**                  src_num - 
**                  dst - 
**                  dst_num - 
**                  size - 
**                  pf - 
** Return         : int 
** Date&Time      : 2012-09-06  10:06:20
** Description    : dst = dst - (src ∩ dst)
*********************************************************************************************/ 
int MergeSameItem(void *src,int src_num,void *dst,int dst_num,int size,ccb pf)
{
	int *table;
	char *tmp,*p_dst;
	char *src_info;
	char *dst_info;
	int i,j,temp;
	int ret_cnt = 0;
	int flag = 1;

	src_info = (char *)src;
	dst_info = (char *)dst;
	p_dst = dst_info;
	temp = sizeof(int) * dst_num;
	table = (int *)wxf_malloc(temp);
	WXF_ASSERT(table);

	for (i = 0;i < dst_num;i++)
	{			
		for(j = 0;j < src_num;j++)
		{
			tmp = src_info + j * size;
			if (pf(tmp,p_dst) == 0)
			{
				flag = 0;
				break;
			}
		}
		if (flag != 0)
		{
			table[ret_cnt] = i;
			ret_cnt++;
		}
		p_dst += size;
		flag = 1;
	}
	for (i = 0;i < ret_cnt;i++)
	{
		tmp = dst_info + table[i] * size;
		wxf_memcpy(dst_info+i*size,tmp,size);
	}
	wxf_free(table);
	return ret_cnt;
}
/********************************************************************************************* 
** Function name  : DeleteSameItem
** Arguments      : p_info - 
**                  num - 
**                  size - 
**                  pf - 
** Return         : int 
** Date&Time      : 2012-09-06  10:06:19
** Description    : 
*********************************************************************************************/ 
int DeleteSameItem(void *p_info,int num,int size,ccb pf)
{
	int *table;
	char *info,*tmp;
	int i,j,flag,temp;
	int ret_cnt = 0;

	info = (char *)p_info;
	temp = sizeof(int) * num;
	table = (int *)wxf_malloc(temp);
	WXF_ASSERT(table);

	for (i = 0;i < num;i++)
	{
		flag = 1;			
		for(j = 0;j < ret_cnt;j++)
		{
			tmp = info + table[j]*size;
			if (pf(info+i*size,tmp) == 0)
			{
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			table[ret_cnt] = i;
			ret_cnt++;
		}
	}
	for (i = 0;i < ret_cnt;i++)
	{
		tmp = info+table[i]*size;
		wxf_memcpy(info+i*size,tmp,size);
	}
	wxf_free(table);
	return ret_cnt;
}
/******************************************************************
* Function	: MSB2LSB
* Description	: msb to lsb
******************************************************************/
void MSB2LSB(unsigned char *p_src, unsigned char *p_dst, unsigned long len)
{
	unsigned long i;

	p_src += len;
	for (i = 0; i < len; i++)
	{
		*p_dst++ = *--p_src;
	}
}

/******************************************************************
* Function	: LSB2MSB
* Description	: lsb to msb
******************************************************************/
void LSB2MSB(unsigned char *p_src, unsigned char *p_dst, unsigned long len)
{
	MSB2LSB(p_src, p_dst, len);
}

/********************************************************************************************* 
** Function name  : Low2Up
** Arguments      : str_in - 
**                  str_out - 
**                  len - 
** Return         : int 
** Description    : 小写转换成大写
*********************************************************************************************/ 
int Low2Up(unsigned char *str_in, unsigned char *str_out, unsigned int len)
{
	if(0 == len || NULL == str_in || NULL == str_out)
		return FALSE;

	while(len --)
	{
		if((*str_in >= 'a')&&(*str_in <= 'z'))
			(*str_out) = (*str_in) - ('a' - 'A');
		else
			*str_out = *str_in;
		str_out++;
		str_in++;
	}

	return TRUE;
}
/********************************************************************************************* 
** Function name  : StrLen
** Arguments      : str - 
** Return         : unsigned int 
** Description    : 字符串的长度
*********************************************************************************************/ 
unsigned int StrLen( const char * str ) 
{
	unsigned int len;

	len = 0;

	while( *str++ )
	{
		len++;
		if (len > 1000)
			return 0;
	}
	return len;	
}
/********************************************************************************************* 
** Function name  : TypeCmp
** Arguments      : pFileName - 
**                  pExt - 
** Return         : int 
** Description    : 
*********************************************************************************************/ 
int TypeCmp(unsigned char *pFileName, unsigned char *pExt)
{
	unsigned int ext_len, file_name_len;

	ext_len = StrLen((const char *)pExt);
	file_name_len = StrLen((const char *)pFileName);

	if(ext_len > 4 || file_name_len > 64)
		return FALSE;

	return !(wxf_memcmp(pFileName + file_name_len - ext_len, pExt, ext_len));
}
/********************************************************************************************* 
** Function name  : GetN_LSB
** Arguments      : nNumber - 数据的长度
**                  address - 数据的首地址
** Return         : unsigned int 
** Description    : 以小端方式取数
*********************************************************************************************/ 
unsigned int GetN_LSB(unsigned int nNumber,unsigned char *address)
{
	int ret,i;

	ret = 0;

	for ( i = nNumber-1; i >= 0; i -- )
		ret = ret * 256 + address[i];

	return ret;
}
/********************************************************************************************* 
** Function name  : 
** Arguments      : output - 
**                  value - 
**                  len - 
** Return         : void R_memset 
** Description    : memset
*********************************************************************************************/ 
void R_memset (void *output,int value,unsigned int len)										 /* length of block */
{
	if (len) wxf_memset(output, value, len);
}
/********************************************************************************************* 
** Function name  : 
** Arguments      : output - 
**                  input - 
**                  len - 
** Return         : void R_memcpy 
** Description    : memcpy
*********************************************************************************************/ 
void R_memcpy (void *output,void *input,unsigned int len)                                       /* length of blocks */
{
	if (len) wxf_memcpy (output, input, len);
}
#endif //_wxf_old_mk_8167_

int wxf_utils_test(char *para)
{
	char sw[256];
	sscanf(para,"wxf_utils %s",sw);
	if (strcmp(sw,"fqst") == 0)
	{
		wxf_fqst_test();
	}
	else if (strcmp(sw,"gxmem") == 0)
	{
		wxf_gxmem_test(para);
	}
	else if (strcmp(sw,"thread") == 0)
	{
		wxf_thread_test(para);
	}
	else if (strcmp(sw,"fbuf") == 0)
	{
		wxf_fbuf_test();
	}
	else 
	{
		wxf_printf("<Don't define \"%s\">\n",para);
	}
	return wxf_succ;
}
