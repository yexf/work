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
	** Arguments      : in_re - ���������ʵ������
	**                  in_im - �����������������
	**                  out_re - ���������ʵ������ 
	**                  out_im - ����������������� 
	**                  num - �任�ĵ�ĸ����������߱�֤��Ϊ2����������
	** Return         : int ����ĵ�ĸ��� 
	** Date&Time      : 2013/06/03 
	** Description    : ���ٸ���Ҷ�任
	*********************************************************************************************/
	int wxf_fft(double *in_re,double *in_im,double *out_re,double *out_im,unsigned int num);
#ifdef __cplusplus
}
#endif

#ifndef WXF_LIB
#pragma comment(lib,"wxf_fft.lib")
#endif

#endif /*_wxf_fft_h_*/
