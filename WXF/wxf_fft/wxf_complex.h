#pragma once

const double pi = 3.14159265;				//��
const double eps = 1e-10;					//���徫��
typedef class wxf_complex comp;

class wxf_complex
{
public:
	wxf_complex(void):m_re(0.0),m_im(0.0){}
	wxf_complex(double real):m_re(real),m_im(0.0){}
	wxf_complex(double real,double img):m_re(real),m_im(img){}
	wxf_complex(const wxf_complex& op):m_re(op.re()),m_im(op.im()){}

	wxf_complex(double raA,int aan,int aaN = 360);	//�Է�ֵ��ʽ��ʼ��
public:
	~wxf_complex(void){}
private:
	double m_re;									//ʵ��
	double m_im;									//�鲿
	static char m_str[256];							//������ӡ���ַ���
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
	double re(void) const{return m_re;}							//ʵ��
	double im(void) const{return m_im;}							//�鲿
	void set_re(double real){m_re = real;}
	void set_im(double img){m_im = img;}
public:
	double ra(void) const;										//����
	double aa(void) const;										//����
	char *str(void) const;
};	



