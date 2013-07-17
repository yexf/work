#include"wxf_comm.h"
#include "wxf_complex.h"
#include "wxf_fft.h"

int wxf_mr(int *mr,unsigned int n);
#ifdef WXF_MAIN
void test_dft()
{
	/*wxf_complex c,c1(3.0,4.0),c2(c1);
	double temp;
	c = c1*c2;
	printf("%s\n",c1.str());
	printf("%s\n",c2.str());
	printf("%s\n",c.str());
	c = c/c2;
	printf("%s\n",c.str());
	printf("%g\n",c.im());
	temp = c2.ra();
	printf("%g\n",temp);
	printf("%g\n",exp(3.0));*/
	comp in[32];
	comp out[32];
	double ret[32];
	int i;
	for (i = 0;i < 32;i++)
	{
		in[i] = comp(i,0.0);
	}
	wxf_dft(in,out,32);
	for (i = 0;i < 32;i++)
	{
		out[i] = ~out[i];
	}
	wxf_dft(out,in,32);
	for (i = 0;i < 32;i++)
	{
		ret[i] = in[i].re() / 32;
	}
}
void wxf_put2(unsigned num,int n)
{
	char i,str[32] = {0};
	for (i = 0;i < n;i++)
	{
		//str[n-1-i] = num%2+0x30;
		str[i] = num%2+0x30;
		num >>= 1;
	}
	printf("%s\n",str);
}
void wxf_putcp(comp *cp,int n)
{
	int i;
	for (i = 0;i < n;i++)
	{
		printf("%d:%s\n",i,cp[i].str());
	}
}

void test_mr()
{
	int mr[32],i,r;
	r = wxf_mr(mr,32);
	for (i = 0;i < 32;i++)
	{
		wxf_put2(mr[i],16);
	}
	printf("\n");
}
void test_fft()
{
	int i,n,r = 5;
	n = 1 << r;
	comp *cp;

	cp = new comp[n];
	for (i = 0;i < n;i++)
	{
		cp[i] = comp(i*0.25-n/8,0.0);
	}
	wxf_putcp(cp,n);
	wxf_fft(cp,cp,n);
	wxf_putcp(cp,n);
	for (i = 0;i < n;i++)
	{
		cp[i] = ~cp[i];
	}
	wxf_fft(cp,cp,n);

	for (i = 0;i < n;i++)
	{
		cp[i] = cp[i] / n;
	}
	wxf_putcp(cp,n);
	delete[] cp;
}
void test_fft1()
{
	int i,n,temp,r = 5;
	double *re,*im;
	n = 1 << r;
	temp = sizeof(double)*n;
	re = (double *)wxf_malloc(temp);
	im = (double *)wxf_malloc(temp);
	for (i = 0;i < n;i++)
	{
		re[i] = i*0.25-n/8;
		im[i] = 0;
	}
	
	wxf_fft(re,im,re,im,n);
	for (i = 0;i < n;i++)
	{
		im[i] = -im[i];
	}
	wxf_fft(re,im,re,im,n);
	for (i = 0;i < n;i++)
	{
		re[i] = re[i] / n;
		printf("%d:real = %g\n",i,re[i]);
	}
	wxf_free(re);
	wxf_free(im);
}
void Test()
{
	//test_dft();
	//test_mr();
	//wxf_put2(3,3);
	test_fft1();
}

#endif