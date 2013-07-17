/* 1997 Bjorn Wesen */

#include "mp3dec.h"

extern int Granule_sbsynth_Vptr[2];
mpfloat *Granule_sbsynth_V[2];
extern mpfloat Granule_sbsynth_D[512];

PCMSample *
windowing(int ch, PCMSample *S)
{
    int i, j, k;
    int is;
    mpfloat sum, *a, *b;
    /* calculate 32 samples. each sample is the sum of 16 terms */
    
    /*     15          */
    /* Sj = E W(j+32i) */
    /*    i=0          */
#ifdef DSP_LOFI
    /* lo-fidelity: only calculate every 4th sample -> 11khz */
    for (j = 0; j < 32; j+=4) {
#else
    for (j = 0; j < 32; j++) {
#endif
	a = &Granule_sbsynth_D[j];
	k = j + Granule_sbsynth_Vptr[ch];
	b = &Granule_sbsynth_V[ch][0];
	sum = (mpfloat)0.0f;
	for(i = 0; i < 8; i++) {
	    sum += a[0] * b[k];
	    k = (k + 96) & 0x3ff;
	    sum += a[32] * b[k];
	    k = (k + 32) & 0x3ff; 
	    a = &a[64];
	}
	
	ISCALE(sum);
	
	/* convert to integer, and clip the output */
	
	is = (int)sum;
	
#ifdef OVERFLOW_CHECKING	    
	if(is >= 32768) {
	    is = 32767;
	} else if(is < -32768) {
	    is = -32768;
	}
#endif
#ifdef DSP_LOFI
	*S++ = is << 2;
#else
	*S = is;
	S += 2;
#endif
    }
    return S;
}

#ifndef DSP
static mpfloat vbuf1[1024];
static mpfloat vbuf2[1024];
#endif


void
windowing_init()
{
    int i, k;

#ifdef DSP
    /* use fixed RAM locations for these 

