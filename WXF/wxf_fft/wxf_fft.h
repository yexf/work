/************************************************************************************** 
** Copyright (C) 2013 XJSH,Inc. All rights reserved
**
** Filename : wxf_fft.h
** Revision : 1.00
**
** Author :  yexf
** Date :  2013/06/01
**
** Description:
**************************************************************************************/ 
#ifndef _wxf_fft_h_
#define _wxf_fft_h_

#ifdef __cplusplus
	void wxf_dft(comp *in_op,comp *out_op,int num);
	void wxf_fft(comp* in_op,comp* out_op,unsigned int num);
#endif

#ifdef __cplusplus
extern "C" {
#endif
	/********************************************************************************************* 
	** Function name  : wxf_fft 
	** Arguments      : in_re - 输入参数的实数部分
	**                  in_im - 输入参数的虚数部分
	**                  out_re - 输出参数的实数部分 
	**                  out_im - 输出参数的虚数部分 
	**                  num - 变换的点的个数，调用者保证其为2的整数次幂
	** Return         : int 计算的点的个数 
	** Date&Time      : 2013/06/03 
	** Description    : 快速傅立叶变换
	*********************************************************************************************/
	int wxf_fft(double *in_re,double *in_im,double *out_re,double *out_im,unsigned int num);
#ifdef __cplusplus
}
#endif

#ifndef WXF_LIB
#pragma comment(lib,"wxf_fft.lib")
#endif

#endif /*_wxf_fft_h_*/
