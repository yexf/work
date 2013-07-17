#ifndef _WXF_COMM_H_
#define _WXF_COMM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <conio.h>
#include <ctype.h> 
#endif

#ifdef _MSC_VER
#define MSVCPP_VER10_0							1600
#define MSVCPP_VER9_0							1500
#define MSVCPP_VER8_0							1400
#define MSVCPP_VER7_1							1310
#define MSVCPP_VER7_0							1300
#define MSVCPP_VER6_0							1200
#define MSVCPP_VER5_0							1100

#pragma warning(disable:4996)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4133)
#endif

#ifdef _MSC_VER
#define WXF_IMPORT								__declspec(dllimport)
#define WXF_EXPORT								__declspec(dllexport)
#define WXF_STCB								__stdcall		//标准回调
#define WXF_STCD								__cdecl			//标准C调用	
#else
#define WXF_IMPORT					
#define WXF_EXPORT
#define WXF_STCB						
#define WXF_STCD						
#endif

typedef int (*CMP_CALL_BACK)(void *,void *);
typedef CMP_CALL_BACK							ccb;

typedef unsigned char							uchar;
typedef unsigned long							ulong;
typedef unsigned int							uint;
typedef unsigned short							ushort;
typedef char *									pchar;
typedef const char								cchar;
typedef const char*								pcstr;	
typedef void *									pvoid;

typedef unsigned char							wxf_uint8;
typedef unsigned long							wxf_uint32;
typedef unsigned short							wxf_uint16;
typedef unsigned long long						wxf_uint64;
typedef char									wxf_int8;
typedef long									wxf_int32;
typedef short									wxf_int16;
typedef long long								wxf_int64;



#ifdef _WIN32

typedef struct _finddata_t						win_dirent;
typedef struct {
	win_dirent dirent;
	intptr_t dir;
}wxf_dir;

/* TODO: FILE/DIR/DIRENT TYPE DEFINE */
typedef FILE									wxf_file;
typedef win_dirent								wxf_dirent;
typedef wxf_file*								wxf_file_t;
typedef wxf_dir*								wxf_dir_t;
typedef wxf_dirent*								wxf_dirent_t;
/* TODO: END */

/* TODO: SEEK PARAMETER */
#define WXF_SEEK_SET							SEEK_SET
#define WXF_SEEK_END							SEEK_END
#define WXF_SEEK_CUR							SEEK_CUR
/* TODO: END */

/* TODO: FILE OPEN MODE */
#define WXF_OM_READ								"rb"
#define WXF_OM_READWRITE						"rb+"
#define WXF_OM_WRITE							"wb"
#define WXF_OM_CREATE							"wb+"
/* TODO: END */

/* TODO: ERROR RETURN CODE OF FILE OPEN AND DIR OPEN  */
#define WXF_ERR_FILE							(wxf_file_t)(NULL)
#define WXF_ERR_DIR								(wxf_dir_t)(NULL)
#define WXF_ERR_DIRENT							(wxf_dirent_t)(NULL)
#define WXF_EOF									EOF
/* TODO: END */


#define WXF_FRAME_RATE							30


#define WXF_FCOPY_BUF							(64*1024)
#define wxf_printf(...)							printf(__VA_ARGS__)
#define WXF_ASSERT(ab) 							if (!(ab)){ \
												wxf_printf("assert: file \"%s\", line %d\n", __FILE__, __LINE__); \
												exit(-1);}
#define __inf(...)								wxf_printf(__VA_ARGS__)
#define __msg(...)								(wxf_printf("[MSG]:L%d(%s):", __LINE__, __FILE__),  \
												 wxf_printf(__VA_ARGS__))
#define __wrn(...)								(wxf_printf("[WRN]:L%d(%s):", __LINE__, __FILE__),  \
												 wxf_printf(__VA_ARGS__))
#define __err(...)								(wxf_printf("[ERR]:L%d(%s):", __LINE__, __FILE__),  \
												 wxf_printf(__VA_ARGS__),system("pause"))

#ifndef _DEBUG_LEVEL
#define _DEBUG_LEVEL							1
#endif

#ifdef _DEBUG_LEVEL_ALL 
#undef _DEBUG_LEVEL
#define _DEBUG_LEVEL							4
#endif

#if    _DEBUG_LEVEL == 4
#define WXF_INF									__inf
#define WXF_MSG									__msg
#define WXF_WRN									__wrn
#define WXF_ERR									__err
#elif  _DEBUG_LEVEL == 3
#define WXF_INF	
#define WXF_MSG									__msg
#define WXF_WRN									__wrn
#define WXF_ERR									__err
#elif  _DEBUG_LEVEL == 2
#define WXF_INF						
#define WXF_MSG	
#define WXF_WRN									__wrn
#define WXF_ERR									__err
#elif  _DEBUG_LEVEL == 1
#define WXF_INF						
#define WXF_MSG	
#define WXF_WRN						
#define WXF_ERR									__err
#elif  _DEBUG_LEVEL == 0
#define WXF_INF						
#define WXF_WRN						
#define WXF_MSG
#define WXF_ERR
#endif

#define wxf_cat(i,j)							i##j
#define wxf_cat3(i,j,k)							i##j##k

#define wxf_max(a,b)							((a)>(b)?(a):(b))
#define wxf_min(a,b)							((a)<(b)?(a):(b))
#define wxf_max3(a,b,c)							wxf_max(wxf_max(a,b),c)
#define wxf_min3(a,b,c)							wxf_min(wxf_min(a,b),c)

/* TODO: FILE SYSTEM API */
#define wxf_fopen(fn, om)						fopen((const char *)fn, om)
#define wxf_fcreate(fn)							fopen((const char *)fn, WXF_OM_CREATE)
#define wxf_fclose(fp)							fclose(fp)
#define wxf_fread								fread
#define wxf_fread_succ(fp, buf, size)			(fread(buf, 1, size, (fp)) == size)
#define wxf_fwrite								fwrite
#define wxf_fwrite_succ(fp, buf, size)			(fwrite(buf, 1, size, (fp)) == size)
#define wxf_fseek(fp, offset, opr)				fseek(fp, offset, opr)
#define wxf_ftell(fp)							ftell(fp)
#define wxf_feof(fp)							feof(fp)
#define wxf_opendir(dirname)					win_opendir((const char *)dirname)
#define wxf_closedir(dir)						win_closedir(dir)
#define wxf_readdir(dir)						win_readdir(dir)
#define wxf_isdir(dirent)						win_entry_is_dir(dirent)
#define wxf_getname(dirent)						win_get_entry_name(dirent)
#define wxf_fsize(fp)							win_fsize(fp)

#define wxf_mkdir(dir)							_mkdir(dir)
#define wxf_mkdir_succ(dir)						(_mkdir(dir) == 0)
#define wxf_rmdir(dir)							_rmdir(dir)
#define wxf_rmdir_succ(dir)						(_rmdir(dir) == 0)
#define wxf_fremove(fn)							remove(fn)
#define wxf_fremove_succ(fn)					(remove(fn) == 0)
#define wxf_frename(on, nn)						rename(on, nn)
#define wxf_frename_succ(on, nn)				(rename(on, nn) == 0)


#define wxf_fputc(ch,fp)						fputc((int)ch,fp)
#define wxf_fgetc(fp)							fgetc(fp)
#define wxf_fgets(buf, size, fp)				fgets(buf, size, fp)
#define wxf_fputs(buf, fp)						fputs(buf, fp)
#define wxf_fscanf								fscanf
#define wxf_fprintf								fprintf
#define wxf_sprintf								sprintf
/* TODO: END */


/* TODO: ANSI C API */
#define wxf_strlen(a)							strlen(a)
#define wxf_strchr(a,b)							strchr(a,b)
#define wxf_strcpy(a,b)							strcpy(a,b)
#define wxf_strncpy(a,b,c)						strncpy(a,b,c)
#define wxf_stricmp(a,b)						_stricmp(a,b)
#define wxf_strncmp(a,b,c)						strncmp(a,b,c)
#define wxf_strcmp(a,b)							strcmp(a,b)
#define wxf_strstr(a,b)							strstr(a,b)
#define wxf_strcat(a,b)							strcat(a,b)
#define wxf_sprintf								sprintf

#define wxf_rand(a)								(a * rand() / RAND_MAX)
#define wxf_abs(a)								abs(a)
#define wxf_memset(a,b,c)						memset(a,b,c)
#define wxf_memcpy(a,b,c)						memcpy(a,b,c)
#define wxf_memmove(a,b,c)						memmove(a,b,c)
#define wxf_memcmp(a,b,c)						memcmp(a,b,c)
#define wxf_malloc(a)							malloc(a)
#define wxf_realloc(a,b)						realloc(a,b)
#define wxf_free(a)								free(a)
#define wxf_atoi(a)								atoi(a)
#define wxf_itoa(i,a,f)							itoa(i,a,f)
#define wxf_qsort(b,n,s,f)						qsort(buf,num,size,pf)
/* TODO: END */

typedef void (*fileCopyCallback_t)(int, void*);

#ifdef __cplusplus
extern "C" {
#endif

int Low2Up(unsigned char *str_in,unsigned char *str_out,unsigned int len);
unsigned StrLen( const char * str );
int TypeCmp(unsigned char *pFileName, unsigned char *pExt);
unsigned int GetN_LSB(unsigned int nNumber,unsigned char *address);
void R_memset (void *output,int value,unsigned int len);
void R_memcpy (void *output,void *input,unsigned int len);
void MSB2LSB(unsigned char *p_src,unsigned char *p_dst,unsigned long len);
void LSB2MSB(unsigned char *p_src,unsigned char *p_dst,unsigned long len);

int DeleteSameItem(void *p_info,int num,int size,ccb pf);
int SaveSameItem(void *src,int src_num,void *dst,int dst_num,int size,ccb pf);
int MergeSameItem(void *src,int src_num,void *dst,int dst_num,int size,ccb pf);

wxf_dir_t win_opendir(const char* dirname);
int win_closedir(wxf_dir_t dir);
wxf_dirent_t win_readdir(wxf_dir_t dir);
int win_entry_is_dir(wxf_dirent_t dirent);
char* win_get_entry_name(void *dirent);
unsigned int win_fsize(wxf_file_t fp);

int wxf_fcopy(const char* dest, const char* src, int bufsz, fileCopyCallback_t proc);
int wxf_xcopy(const char* dest, const char* src, int bufsz, fileCopyCallback_t proc);

#ifdef  __cplusplus
}
#endif


#undef TRUE
#define TRUE									(1)
#undef FALSE
#define FALSE									(0)

#ifndef true
#define true									TRUE
#endif
#ifndef false
#define false									FALSE
#endif


#endif	//_WIN32

#ifdef _DEBUG

#endif //_DEBUG

#ifdef __VLD_DEBUG
#include "vld/vld.h"
#pragma comment(lib,"vld.lib")
#endif

#endif //_WXF_COMM_H_