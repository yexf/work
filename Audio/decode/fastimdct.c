#include "mp3dec.h"

/* An Inverse Modified Discrete Cosinus Transform is applied to 18 values
   at a time. The 18 frequency values generate 36 time domain values, 
   where half of these are overlapped with half of the previously generated
   values to produce 18 output values. These can later be fed into the
   subband synthesis.

   The multiply-add loop here is very time critical. It uses a total of
   684*32=21888 MAC operations per imdct, 4 times per frame. With 38
   frames per second, thats 3.33 million MAC's.

   Using some small trigonometric tricks, we can do this in 360*32*4*38=
   1.75 million MAC's per second instead.

*/

extern mpfloat Granule_imdct_previous[2][576];  /* used for overlapping */
extern mpfloat Granule_9x9_idct[72];
extern mpfloat Granule_imdct_win[4][36];

mpfloat Granule_twiddles_short[23] = {
    0.866025403f, 0.5f,
    1.931851653f, 0.707106781f, 0.517638090f,

    0.504314480f, 0.541196100f, 0.630236207f,
    0.821339815f, 1.306562965f, 3.830648788f,
    0.793353340f, 0.608761429f, 0.923879532f,
    0.382683432f, 0.991444861f, 0.130526192f, 
    0.382683432f, 0.608761429f, 0.793353340f,
    0.923879532f, 0.991444861f, 0.130526192f
};

mpfloat Granule_twiddles_normal[] = {
    5.736856623f, 1.931851653f, 1.183100792f,
    0.871723397f, 0.707106781f, 0.610387294f, 
    0.551688959f, 0.517638090f, 0.501909918f,

    -0.500476342f, -0.504314480f, -0.512139757f,
    -0.524264562f, -0.541196100f, -0.563690973f, 
    -0.592844523f, -0.630236207f, -0.678170852f, 
    -0.740093616f, -0.821339815f, -0.930579498f,
    -1.082840285f, -1.306562965f, -1.662754762f, 
    -2.310113158f, -3.830648788f, -11.46279281f
};

void
Granule_imdct(Granule *gr, int ch, Granule_floatfreqs X)
{
    int sb, i, j, k, l, window;
    mpfloat save, sum, sum2, pp1;
    mpfloat s;
    mpfloat x[36], t[18];
    mpfloat *v, *prev, *z, *twid;

    prev = Granule_imdct_previous[ch];

    /* process each subband */

    for(sb = 0; sb < NUM_SUBBANDS; sb++) {

	for(i = 0; i < 36; i++)
	    x[i] = (mpfloat)0.0f;
	
	/* decode the block_type - it's in block_type, but we have to think
	   about the mixed blocks lower 2 subbands */
	
	if(gr->block_type == BLOCKTYPE_3WIN &&
	   !(gr->window_switching_flag && 
	     gr->mixed_block_flag && sb < 2)) {

	    /* process the 3 windows separately, each window has 12 values */

	    for(window = 0; window < 3; window++) {

#ifdef LEE_IMDCT
		/* 30*3=90 adds, 25*3=75 muls */

		X[15+window] += X[12+window];
		X[12+window] += X[9+window];
		X[9+window] += X[6+window];
		X[6+window] += X[3+window];
		X[3+window] += X[window];

		X[15+window] += X[9+window];
		X[9+window] += X[3+window];

		twid = Granule_twiddles_short;

		/* do a 3x3 IDCT on the even part */

		pp1 = X[6+window] * twid[0];
		sum = X[window] + X[12+window] * twid[1];
		t[1] = X[window] - X[12+window];
		t[0] = sum + pp1;
		t[2] = sum - pp1;

		/* 3x3 IDCT for odd part */

		pp1 = X[9+window] * twid[0];
		sum = X[3+window] + X[15+window] * twid[1];
		t[4] = X[3+window] - X[15+window];
		t[5] = sum + pp1;
		t[3] = sum - pp1;

		/* scale the odd part */

		t[3] *= twid[2];
		t[4] *= twid[3];
		t[5] *= twid[4];

		save = t[0];
		t[0] += t[5];
		t[5] = save - t[5];

		save = t[1];
		t[1] += t[4];
		t[4] = save - t[4];

		save = t[2];
		t[2] += t[3];
		t[3] = save - t[3];

		t[0]  *= twid[5];
		t[1]  *= twid[6];
		t[2]  *= twid[7];
		t[3]  *= twid[8];
		t[4]  *= twid[9];
		t[5]  *= twid[10];

		t[6]  = -t[2] * twid[15];
		t[7]  = -t[1] * twid[13];
		t[8]  = -t[0] * twid[11];
		t[9]  = -t[0] * twid[12];
		t[10] = -t[1] * twid[14];
		t[11] = -t[2] * twid[16];
		
		t[0]  =  t[3];
		t[1]  =  t[4] * twid[17];
		t[2]  =  t[5] * twid[18];
		
		t[3]  = -t[5] * twid[19];
		t[4]  = -t[4] * twid[20];
		t[5]  = -t[0] * twid[21];
		
		t[0] *= twid[22];

		z = &x[window * 6 + 6];
		z[0]  += t[0];
		z[1]  += t[1];
		z[2]  += t[2];
		z[3]  += t[3];
		z[4]  += t[4];
		z[5]  += t[5];
		z[6]  += t[6];
		z[7]  += t[7];
		z[8]  += t[8];
		z[9]  += t[9];
		z[10]  += t[10];
		z[11]  += t[11];

#else
		/* uses 3*6*6+3*12=144 MAC's */
		v = Granule_imdct_bigCOS2;
		for(i = 0; i < 6; i++) {
		    s = X[window] * *(v++); 
		    s += X[window + 3] * *(v++);
		    s += X[window + 6] * *(v++);
		    s += X[window + 9] * *(v++);
		    s += X[window + 12] * *(v++);
		    s += X[window + 15] * *(v++);
		    ISCALE(s);
		    t[i] = s;
		}

		/* periodic expansion */
		z = &x[window * 6 + 6];
		z[0] -= t[3] * Granule_imdct_win[2][0];
		z[1] -= t[4] * Granule_imdct_win[2][1];
		z[2] -= t[5] * Granule_imdct_win[2][2];
		z[3] += t[5] * Granule_imdct_win[2][3];
		z[4] += t[4] * Granule_imdct_win[2][4];
		z[5] += t[3] * Granule_imdct_win[2][5];
		z[6] += t[2] * Granule_imdct_win[2][6];
		z[7] += t[1] * Granule_imdct_win[2][7];
		z[8] += t[0] * Granule_imdct_win[2][8];
		z[9] += t[0] * Granule_imdct_win[2][9];
		z[10] += t[1] * Granule_imdct_win[2][10];
		z[11] += t[2] * Granule_imdct_win[2][11];
#endif
	    }

#ifdef INT_MATH
	    for(i = 0; i < 36; i++)
		ISCALE(x[i]);
#endif

	} else {

#ifdef LEE_IMDCT
	    
	    /* uses 207 muls, 189 adds */
		 
	    X[17] += X[16];
	    X[16] += X[15];
	    X[15] += X[14];
	    X[14] += X[13];
	    X[13] += X[12];
	    X[12] += X[11];
	    X[11] += X[10];
	    X[10] += X[9];
	    X[9] += X[8];
	    X[8] += X[7];
	    X[7] += X[6];
	    X[6] += X[5];
	    X[5] += X[4];
	    X[4] += X[3];
	    X[3] += X[2];
	    X[2] += X[1];
	    X[1] += X[0];

	    X[17] += X[15];
	    X[15] += X[13];
	    X[13] += X[11];
	    X[11] += X[9];
	    X[9] += X[7];
	    X[7] += X[5];
	    X[5] += X[3];
	    X[3] += X[1];

	    for(i = 0, j = 0; i < 9; i++, j += 8) {
#ifdef USE_INLINE_ASM
__asm {
	mov eax, X
	mov ecx, j
	lea ebx, [Granule_9x9_idct + ecx*4]
	fld dword ptr [eax]
	fld dword ptr [eax + 4]

	fld dword ptr [ebx]
	fld st(0)
	fmul dword ptr [eax + 8]
	faddp st(3), st
	fmul dword ptr [eax + 12]
	faddp st(1), st

	fld dword ptr [ebx + 4]
	fld st(0)
	fmul dword ptr [eax + 16]
	faddp st(3), st
	fmul dword ptr [eax + 20]
	faddp st(1), st

	fld dword ptr [ebx + 8]
	fld st(0)
	fmul dword ptr [eax + 24]
	faddp st(3), st
	fmul dword ptr [eax + 28]
	faddp st(1), st

	fld dword ptr [ebx + 12]
	fld st(0)
	fmul dword ptr [eax + 32]
	faddp st(3), st
	fmul dword ptr [eax + 36]
	faddp st(1), st

	fld dword ptr [ebx + 16]
	fld st(0)
	fmul dword ptr [eax + 40]
	faddp st(3), st
	fmul dword ptr [eax + 44]
	faddp st(1), st

	fld dword ptr [ebx + 20]
	fld st(0)
	fmul dword ptr [eax + 48]
	faddp st(3), st
	fmul dword ptr [eax + 52]
	faddp st(1), st

	fld dword ptr [ebx + 24]
	fld st(0)
	fmul dword ptr [eax + 56]
	faddp st(3), st
	fmul dword ptr [eax + 60]
	faddp st(1), st

	fld dword ptr [ebx + 28]
	fld st(0)
	fmul dword ptr [eax + 64]
	faddp st(3), st
	fmul dword ptr [eax + 68]
	faddp st(1), st

	fstp dword ptr [sum2]
	fstp dword ptr [sum]
}
#else
                        sum = X[0];
			sum2 = X[1];
			for(l = 0, k = 0; l < 16; l += 2, k++) {
			    s = Granule_9x9_idct[j+k];
			    sum += X[2+l] * s;
			    sum2 += X[3+l] * s;
			}
#endif
			t[i] = sum;
			t[17-i] = sum2;
	    }

	    twid = Granule_twiddles_normal;
	    
	    for(i = 0; i < 9; i++)
		t[9+i] *= twid[i];

	    for(i = 0; i < 9; i++) {
		save = t[i];
		t[i] += t[17-i];
		t[17-i] = save - t[17-i];
	    }

	    for(i = 0; i < 18; i++)
		t[i] *= twid[9+i];

#else
	    /* this loop uses 19*36=684 MAC operations */

	    /* do a 18x18 IMDCT */
	    /* 324 muls and 324 adds */

	    v = Granule_imdct_bigCOS;
	    for(i = 0; i < 18; i++) {
		s = (mpfloat)0.0f;
		k = 0;
		do {
		    s += X[k] * v[k];
		    s += X[k+1] * v[k+1];
		    s += X[k+2] * v[k+2];
		    s += X[k+3] * v[k+3];
		    s += X[k+4] * v[k+4];
		    s += X[k+5] * v[k+5];
		    k += 6;
		} while(k < 18);
		v += 18;
		ISCALE(s);
		t[i] = s;
	    }
#endif
	    /* correct the transform into the 18x36 IMDCT we need */
	    /* 36 muls */

	    for(i = 0; i < 9; i++) {
		x[i] = t[i+9] * Granule_imdct_win[gr->block_type][i];
		ISCALE(x[i]);
		x[i+9] = t[17-i] * Granule_imdct_win[gr->block_type][i+9];
		ISCALE(x[i+9]);
		x[i+18] = t[8-i] * Granule_imdct_win[gr->block_type][i+18];
		ISCALE(x[i+18]);
		x[i+27] = t[i] * Granule_imdct_win[gr->block_type][i+27];
		ISCALE(x[i+27]);
	    }

	}
	
	/* Overlap and add with previous block -
	   The first half of the 36 values is overlapped with the second half
	   of the previous block. The second half of the actual block is stored
	   to be used in the next block.
	   */
	
	for(i = 0; i < 18; i++) {
	    *(X++) = x[i] + *prev;  /* produce an output value */
	    *(prev++) = x[i+18]; 
	}
    }	
    
}

