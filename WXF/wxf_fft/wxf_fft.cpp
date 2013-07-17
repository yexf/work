#include "wxf_comm.h"
#include "wxf_complex.h"
#include "wxf_fft.h"
void wxf_dft(comp *in_op,comp *out_op,int num)
{
	int i,k;
	int n = abs(num);
	comp ep,sum;
	for (k = 0;k < n;k++)
	{
		sum = comp(0.0,0.0);
		for (i = 0;i < n;i++)
		{
			ep = comp(1.0,i*k,num);
			sum = sum+in_op[i]*ep;
		}
		out_op[k]=sum;
	}
}
int wxf_log2(unsigned int num)
{
	if (num == 0) return -1;
	if (num == 1) return 0;
	return 1 + wxf_log2(num>>1);
}
int wxf_mr(int *mr,unsigned int n)
{
	int r,i,j,m,temp;
	r = wxf_log2(n);
	mr[0] = 0;
	for (i = 0;i < r;i++)
	{
		m = 1 << i;
		temp = 1 << (r-1-i);
		for (j = 0;j < m;j++)
		{
			mr[m+j] = mr[j] + temp;
		}
	}
	return r;
}
void wxf_fftn(comp* op,int r)
{
	int i,j,k,l,m,h,n=1<<r;
	comp *temp,temp1,temp2;
	for (i = 0;i < r;i++)
	{
		l = 1 << (i+1);							//fft长度
		m = n / l;								//fft次数
		h = l / 2;
		for (j = 0;j < m;j++)
		{
			temp = op + j*l;
			for (k = 0;k < h;k++)
			{
				temp1 = temp[k];
				temp2 = temp[h+k]*comp(1.0,k,l);
				temp[k] = temp1 + temp2;
				temp[h+k] = temp1 - temp2;
			}
		}
	}
}
void wxf_fft(comp* in_op,comp* out_op,unsigned int num)
{
	int r,i,*mr = new int[num];
	comp *buf = new comp[num];
	r = wxf_mr(mr,num);
	for (i = 0;i < num;i++)
	{
		buf[i] = in_op[mr[i]];					//移位
	}
	for (i = 0;i < num;i++)
	{
		out_op[i] = buf[i];
	}
	wxf_fftn(out_op,r);
	delete mr;
	delete[] buf;
}
/********************************************************************************************* 
** Function name  : wxf_fft 
** Arguments      : in_re - 输入参数的实数部分
**                  in_im - 输入参数的虚数部分
**                  out_re - 输出参数的实数部分 
**                  out_im - 输出参数的虚数部分 
**                  num - 变换的点的个数，调用者保证其为2的整数次幂
** Return         : int 计算的点的个数 
** Date&Time      : 2013/06/03  13:21:57
** Description    : 快速傅立叶变换
*********************************************************************************************/ 
int wxf_fft(double *in_re,double *in_im,double *out_re,double *out_im,unsigned int num)
{
	int r,i,*mr = new int[num];
	comp *buf = new comp[num];
	r = wxf_mr(mr,num);
	for (i = 0;i < num;i++)
	{
		buf[i].set_re(in_re[mr[i]]);
		buf[i].set_im(in_im[mr[i]]);
	}
	wxf_fftn(buf,r);
	for (i = 0;i < num;i++)
	{
		out_re[i] = buf[i].re();
		out_im[i] = buf[i].im();
	}
	delete mr;
	delete[] buf;
	return 1 << r;
}