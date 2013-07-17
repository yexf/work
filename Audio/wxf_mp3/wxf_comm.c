#include "wxf_comm.h"
#define wxf_get_entry_name				wxf_getname
#define wxf_entryisdir					wxf_isdir
#define wxf_app_free					wxf_free
#define wxf_app_malloc					wxf_malloc
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

#ifdef _WIN32
/********************************************************************************************* 
** Function name  : win_opendir
** Arguments      : dirname - 
** Return         : wxf_dir_t 
** Date&Time      : 2012-09-05  10:05:14
** Description    : 
*********************************************************************************************/ 
wxf_dir_t win_opendir(const char* dirname)
{
	wxf_dir_t ret;

	if(_chdir(dirname))
	{
		printf("Unable to locate the directory: %s\n", dirname);
		return NULL;
	}
	else 
	{
		ret = (wxf_dir_t)malloc(sizeof(wxf_dir));
		/* Find first in the current directory */
		ret->dir =  _findfirst("*.*", &(ret->dirent)); 
		return ret;
	}
}

/********************************************************************************************* 
** Function name  : win_closedir
** Arguments      : dir - 
** Return         : int 
** Date&Time      : 2012-09-05  10:05:17
** Description    : 
*********************************************************************************************/ 
int win_closedir(wxf_dir_t dir)
{
	_findclose(dir->dir); 
	free(dir);
	return 0;
}

/********************************************************************************************* 
** Function name  : win_readdir
** Arguments      : dir - 
** Return         : wxf_dirent_t 
** Date&Time      : 2012-09-05  10:05:18
** Description    : 
*********************************************************************************************/ 
wxf_dirent_t win_readdir(wxf_dir_t dir)
{	
	if (_findnext(dir->dir, &(dir->dirent)) == 0)
		return &(dir->dirent);
	else
		return NULL;
}

/********************************************************************************************* 
** Function name  : win_entry_is_dir
** Arguments      : dirent - 
** Return         : int 
** Date&Time      : 2012-09-05  10:05:20
** Description    : 
*********************************************************************************************/ 
int win_entry_is_dir(wxf_dirent_t dirent)
{
	struct _finddata_t* entry = (struct _finddata_t*)dirent;

	if (entry->attrib & _A_SUBDIR)
		return true;
	else
		return false;
}

/********************************************************************************************* 
** Function name  : win_get_entry_name
** Arguments      : dirent - 
** Return         : char* 
** Date&Time      : 2012-09-05  10:05:21
** Description    : 
*********************************************************************************************/ 
char* win_get_entry_name(void *dirent)
{
#ifdef _WIN32
	struct _finddata_t* entry = (struct _finddata_t*)dirent;
	return entry->name;
#else
	return dirent->d_name;
#endif
}

/********************************************************************************************* 
** Function name  : win_fsize
** Arguments      : fp - 
** Return         : unsigned int 
** Date&Time      : 2012-09-05  10:05:23
** Description    : 
*********************************************************************************************/ 
unsigned int win_fsize(wxf_file_t fp)
{
	unsigned int size,temp;
	unsigned int pos = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	temp = ftell(fp); 
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return size - temp;
}

int wxf_fcopy(const char* dest, const char* src, int bufsz, fileCopyCallback_t proc)
{
	wxf_file_t fp;
	wxf_file_t fpd;
	char* tmpbuf;
	int ret = 0;
	int fsize;
	int remainsz;

	if ((fp = wxf_fopen(src, WXF_OM_READ)) == WXF_ERR_FILE)
	{
		return 1;
	}
	else
	{
		if ((fpd = wxf_fopen(dest, WXF_OM_CREATE)) == WXF_ERR_FILE)
		{
			wxf_fclose(fp);
			return 1;
		}

		fsize = wxf_fsize(fp);
		bufsz = wxf_min(bufsz, fsize);
		tmpbuf = (char*)wxf_malloc(bufsz);
		WXF_ASSERT(tmpbuf != NULL);
		remainsz = fsize;
		while (remainsz)
		{
			bufsz = wxf_min(bufsz, remainsz);
			if (!wxf_fread_succ(fp, tmpbuf, bufsz))
			{
				ret++;
				break;
			}
			if (!wxf_fwrite_succ(fpd, tmpbuf, bufsz))
			{
				ret++;
				break;
			}
			remainsz -= bufsz;
			if (proc != NULL)
				proc(((fsize-remainsz)*100)/fsize, (char*)dest);
		}
		wxf_free(tmpbuf);
		wxf_fclose(fp);
		wxf_fclose(fpd);
	}

	return ret;
}
int wxf_xcopy(const char* dest, const char* src, int bufsz, fileCopyCallback_t proc)
{
	wxf_dir_t dp;
	wxf_dir_t dpd;
	wxf_dirent_t dirent;
	int ret = 0;
	char* tmpsrc;
	char* tmpdest;

	if ((dp = wxf_opendir(src)) == WXF_ERR_DIR)
	{
		return wxf_fcopy(dest, src, bufsz, proc);
	}
	else
	{
		if ((dpd = wxf_opendir(dest)) == WXF_ERR_DIR)
		{
			if (!wxf_mkdir_succ(dest))
			{
				ret++;
				wxf_closedir(dp);
				return ret;
			}
		}
		else
		{
			wxf_closedir(dpd);
		}
		tmpsrc = (char*)wxf_app_malloc(512);
		tmpdest = (char*)wxf_app_malloc(512);
		WXF_ASSERT(tmpsrc!=NULL && tmpdest!=NULL);
		dirent = wxf_readdir(dp);
		while (dirent != NULL)
		{
			if (wxf_strcmp(wxf_get_entry_name(dirent), "..") != 0 && wxf_strcmp(wxf_get_entry_name(dirent), ".") != 0)
			{
				wxf_sprintf(tmpsrc, "%s\\%s", src, wxf_get_entry_name(dirent));
				wxf_sprintf(tmpdest, "%s\\%s", dest, wxf_get_entry_name(dirent));
				if (wxf_entryisdir(dirent))
				{
					ret += wxf_xcopy(tmpdest, tmpsrc, bufsz, proc);
				}
				else
				{
					ret += wxf_fcopy(tmpdest, tmpsrc, bufsz, proc);
				}
			}
			dirent = wxf_readdir(dp);
		}
		wxf_closedir(dp);
		wxf_app_free(tmpsrc);
		wxf_app_free(tmpdest);
	}

	return ret;
}
#endif

