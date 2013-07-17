#pragma once

const double pi = 3.14159265;				//π
const double eps = 1e-10;					//定义精度
typedef class wxf_complex comp;

class wxf_complex
{
public:
	wxf_complex(void):m_re(0.0),m_im(0.0){}
	wxf_complex(double real):m_re(real),m_im(0.0){}
	wxf_complex(double real,double img):m_re(real),m_im(img){}
	wxf_complex(const wxf_complex& op):m_re(op.re()),m_im(op.im()){}

	wxf_complex(double raA,int aan,int aaN = 360);	//以幅值方式初始化
public:
	~wxf_complex(void){}
private:
	double m_re;									//实部
	double m_im;									//虚部
	static char m_str[256];							//用来打印的字符串
public:
	friend const wxf_complex
		operator+(const wxf_complex& left,
		const wxf_complex& right);
	friend const wxf_complex
		operator-(const wxf_complex& left,
		const wxf_complex& right);
	friend const wxf_complex
		operator*(const wxf_complex& left,
		const wxf_complex& right);
	friend const wxf_complex
		operator/(const wxf_complex& left,
		const wxf_complex& right);
		
	const wxf_complex&						
		operator=(const wxf_complex& op);

	const wxf_complex& 
		operator~(void) {m_im = -m_im;return *this;}
public:
	double re(void) const{return m_re;}							//实部
	double im(void) const{return m_im;}							//虚部
	void set_re(double real){m_re = real;}
	void set_im(double img){m_im = img;}
public:
	double ra(void) const;										//幅度
	double aa(void) const;										//幅角
	char *str(void) const;
};	



