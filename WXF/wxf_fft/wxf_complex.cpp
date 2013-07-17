#include "wxf_complex.h"
#include "wxf_comm.h"
char wxf_complex::m_str[256] = {0};
wxf_complex::wxf_complex(double raA,int aan,int aaN)
{ 
	double temp = 2*pi*aan/aaN;
	m_re = cos(temp)*raA;
	m_im = sin(temp)*raA;
}
const wxf_complex& wxf_complex::operator=(const wxf_complex& op)
{
	m_re = op.m_re;
	m_im = op.m_im;
	return *this;
}
double wxf_complex::ra() const
{
	return sqrt(m_re*m_re + m_im*m_im);
}
double wxf_complex::aa() const
{
	return atan(m_re/m_im);
}
char *wxf_complex::str(void) const
{
	if (m_im > eps)
	{
		sprintf(comp::m_str,"\t<%g + %gj>",m_re,m_im);
	}
	else if (m_im < -eps)
	{
		sprintf(comp::m_str,"\t<%g - %gj>",m_re,-m_im);
	}
	else
	{
		sprintf(comp::m_str,"\t<%g>",m_re);
	}
	return comp::m_str;
}

/********************************************************************************************* 
** Function name  : operator+
** Arguments      : left - 
**                  right - 
** Return         : const wxf_complex 
** Date&Time      : 2013/06/01  16:25:13
** Description    : 
*********************************************************************************************/ 
const wxf_complex operator+(const wxf_complex& left,const wxf_complex& right)
{
	double re = left.m_re + right.m_re;
	double im = left.m_im + right.m_im;
	return wxf_complex(re,im);
}
const wxf_complex operator-(const wxf_complex& left,const wxf_complex& right)
{
	double re = left.m_re - right.m_re;
	double im =  left.m_im - right.m_im;
	return wxf_complex(re,im);
}
const wxf_complex operator*(const wxf_complex& left,const wxf_complex& right)
{
	double re = left.m_re*right.m_re - left.m_im*right.m_im;
	double im = left.m_re*right.m_im + left.m_im*right.m_re;
	return wxf_complex(re,im);
}
const wxf_complex operator/(const wxf_complex& left,const wxf_complex& right)
{
	double temp = right.m_re*right.m_re + right.m_im*right.m_im;
	double re = left.m_re*right.m_re + left.m_im*right.m_im;
	double im = left.m_im*right.m_re - left.m_re*right.m_im;
	return wxf_complex(re/temp,im/temp);
}
