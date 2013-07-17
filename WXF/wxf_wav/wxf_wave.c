#include <windows.h>     
#include <mmsystem.h> 
#include <wxf_comm.h>
#include "wxf_wave.h"
#pragma comment(lib, "winmm.lib")
#define WXF_WPPARA_INIT					0
#define WXF_WPPARA_PLAY					1
#define WXF_WPPARA_PAUSE				2
#define WXF_WPPARA_STOP					3
#define WXF_WPPARA_UNINIT				(-1)
typedef struct wxf_wave_proc_param_st
{
	WAVEFORMATEX wavform;			//波形参数
	HWAVEOUT hWaveOut;
	int status;						//状态 0 初始化 1 播放 2 暂停 3 停止
	WAVEHDR wavhdr;
	char wavbuf[FRAGMENT_SIZE*2];	//双缓存
	wxf_uint64 wavcount;
	wxf_wpf wavproc;
	wxf_wpplayer *player;
	wxf_thread wavthread;
	wxf_uint32 uMsg;
}wxf_wppara;


char *wxf_get_cur_para(wxf_wppara *para)
{
	int temp = para->wavcount%2;
	temp *= FRAGMENT_SIZE;
	return para->wavbuf + temp;
}
int wxf_get_buf_para(wxf_wppara *para)
{
	int temp;
	char *buf;

	para->wavcount++;
	buf = wxf_get_cur_para(para);
	
	temp = para->player->wavbuf->size;
	if (temp < FRAGMENT_SIZE)
	{
		memset(buf,0,FRAGMENT_SIZE);
		return wxf_err;
	}
	else
	{
		wxf_read_fbuf(para->player->wavbuf,buf,FRAGMENT_SIZE);
		return wxf_succ;
	}
}
unsigned int wxf_stdcall wxf_wav_proc_thread(void *arg);
static void CALLBACK waveOutProc(HWAVEOUT hwo,     
						  UINT uMsg,           
						  DWORD_PTR dwInstance,     
						  DWORD_PTR dwParam1,      
						  DWORD_PTR dwParam2     )  
{  
	wxf_wppara *para;
	para = (wxf_wppara *)dwInstance;
	para->uMsg = uMsg;
	wxf_unlock_thread(para->wavthread);
}
/********************************************************************************************* 
** Function name  : wxf_mem_match
** Arguments      : Dst - 
**                  size - 
**                  num - 
**                  Src - 
** Return         : int 
** Date&Time      : 2013/06/15  14:13:50
** Description    : 在序列中匹配指定的内容，匹配返回匹配的个数，否则返回-1
*********************************************************************************************/ 
int wxf_mem_match(const void *Dst,int size,int num,const void *Src)
{
	int i;
	char *temp;

	temp = (char *)Dst;
	for (i = 0;i < num;i++)
	{
		if (memcmp(temp,Src,size) == 0)
		{
			return i;
		}
		else
		{
			temp += size;
		}
	}
	return wxf_err;
}
int wxf_set_form(WAVEFORMATEX *wavform,char nchannels,char wbps,int sps)
{
	int csps[4] = {8000,11025,22050,44100};
	
	if (nchannels != 1 && nchannels != 2)
	{
		return wxf_err;
	}
	if (wxf_mem_match(csps,4,4,&sps) == wxf_err)
	{
		return wxf_err;
	}
	wavform->wFormatTag = WAVE_FORMAT_PCM;  
	wavform->nChannels = nchannels;							//通道数 
	wavform->nSamplesPerSec = sps;							//采样率
	wavform->wBitsPerSample = wbps;							//采样深度
	wavform->nBlockAlign = wbps*nchannels/8;				//以字节为单位设置块对齐
	wavform->nAvgBytesPerSec = sps*wavform->nBlockAlign;		//设置请求的平均数据传输率	
	wavform->cbSize = 0;  

	return wxf_succ;
}
int wxf_wpplayer_set_status(wxf_wpplayer *player,int statuse)
{
	wxf_wppara *para;
	para = (wxf_wppara *)player->para;
	para->status = statuse;
	return para->status;
}
int wxf_wpplayer_get_status(wxf_wpplayer *player)
{
	wxf_wppara *para;
	para = (wxf_wppara *)player->para;
	return para->status;
}
void wxf_wpplayer_uninit(wxf_wpplayer *player)
{
	if (player)
	{
		int temp;

		temp = wxf_wpplayer_get_status(player);
		wxf_uninit_fbuf(player->wavbuf);
		if (temp != WXF_WPPARA_INIT)
		{
			wxf_wpplayer_set_status(player,WXF_WPPARA_UNINIT);
			player->para = NULL;
		}
		else
		{
			wxf_free(player->para);
		}
		wxf_free(player);
	}
}
wxf_wpplayer *wxf_wpplayer_init()
{
	wxf_wpplayer *player;
	wxf_wppara *para;
	int temp;
	temp = sizeof(wxf_wppara);
	para = (wxf_wpplayer *)wxf_malloc(temp);
	wxf_memset(para,0,temp);

	temp = sizeof(wxf_wpplayer);
	player = (wxf_wpplayer *)wxf_malloc(temp);
	wxf_memset(player,0,temp);
	para->player = player;
	player->para = para;
	para->wavthread = wxf_create_thread(wxf_wav_proc_thread,para,wxf_create_suspended,NULL);
	player->wavbuf = wxf_init_fbuf(NULL,BUFFER_SIZE);
	
	return player;
}

int wxf_wpplayer_start(wxf_wpplayer *player)
{
	wxf_wppara *para;

	para = (wxf_wppara *)player->para;
	wxf_set_form(&para->wavform,1,16,44100);
	waveOutOpen(&para->hWaveOut, WAVE_MAPPER, &para->wavform, (DWORD_PTR)waveOutProc,(DWORD_PTR)para,CALLBACK_FUNCTION);  
	
	return para->status;
}
int wxf_wpplayer_stop(wxf_wpplayer *player)
{
	wxf_wppara *para;
	para = (wxf_wppara *)player->para;
	para->status = WXF_WPPARA_STOP;
	return para->status;
}
int wxf_wpplayer_play(wxf_wpplayer *player,void *pdata,int len)
{
	int ret;
	char *pcm = (char *)pdata;
	if (player->wavbuf->status == WXF_FB_FULL)
	{
		Sleep(50);
		return wxf_wpplayer_play(player,pcm,len);
	}

	ret = wxf_write_fbuf(player->wavbuf,pcm,len);
	if (ret == wxf_err)
		return wxf_err;
	else if (ret == wxf_succ) 
		return wxf_succ;
	else
	{
		Sleep(50);
		return wxf_wpplayer_play(player,pcm+ret,len-ret);
	}
}
void wxf_wav_proc(wxf_wppara *para,wxf_uint32 uMsg)
{
	if (WOM_OPEN == uMsg)
	{
		wxf_get_buf_para(para);
		para->wavhdr.lpData = wxf_get_cur_para(para);  
		para->wavhdr.dwBufferLength = FRAGMENT_SIZE;  
		para->wavhdr.dwFlags = 0;  
		para->wavhdr.dwLoops = 0; 
		para->status = WXF_WPPARA_PLAY;
		waveOutPrepareHeader(para->hWaveOut,&para->wavhdr, sizeof(WAVEHDR));
		waveOutWrite(para->hWaveOut,&para->wavhdr, sizeof(WAVEHDR));
	}
	else if (WOM_CLOSE == uMsg)
	{

	}
	else if (WOM_DONE == uMsg)  
	{  
		//LPWAVEHDR pWaveHeader  = (LPWAVEHDR)dwParam1;
		LPWAVEHDR pWaveHeader = &para->wavhdr;
		waveOutUnprepareHeader(para->hWaveOut, pWaveHeader, sizeof(WAVEHDR)); // 清空设备缓存 

		if (para->status == WXF_WPPARA_PLAY)
		{
			pWaveHeader->lpData = wxf_get_cur_para(para);
			pWaveHeader->dwBufferLength = FRAGMENT_SIZE;
			waveOutPrepareHeader(para->hWaveOut, pWaveHeader, sizeof(WAVEHDR)); 
			waveOutWrite(para->hWaveOut, pWaveHeader, sizeof(WAVEHDR)); 
			wxf_get_buf_para(para);	
		}
		else if (para->status == WXF_WPPARA_STOP)
		{
			wxf_wpplayer *player = para->player;
			wxf_wpf wavproc = para->wavproc;
			waveOutReset(para->hWaveOut);  
			waveOutUnprepareHeader(para->hWaveOut, &para->wavhdr, sizeof(WAVEHDR));  
			waveOutClose(para->hWaveOut);
			wxf_memset(para,0,sizeof(wxf_wppara));
			para->player = player;
			para->wavproc = wavproc;
		}
		else if (para->status == WXF_WPPARA_UNINIT)
		{
			waveOutReset(para->hWaveOut);  
			waveOutUnprepareHeader(para->hWaveOut, &para->wavhdr, sizeof(WAVEHDR));  
			waveOutClose(para->hWaveOut);
			wxf_free(para);
		}
	}  
}
unsigned int wxf_stdcall wxf_wav_proc_thread(void *arg)
{
	wxf_wppara *para;
	wxf_thread hThis;
	para = (wxf_wppara *)arg;
	hThis = para->wavthread;
	while (1)
	{	
		wxf_wav_proc(para,para->uMsg);
		if (para->uMsg == WXF_WPPARA_UNINIT)
		{
			break;
		}
		else wxf_lock_thread(hThis);
	}
	wxf_close_thread(hThis);
	return 0;
}