#include "wxf_comm.h"
#define wxf_get_entry_name				wxf_getname
#define wxf_entryisdir					wxf_isdir
#define wxf_app_free					wxf_free
#define wxf_app_malloc					wxf_malloc

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

