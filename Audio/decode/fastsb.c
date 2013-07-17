/* Fast Inverse DCT implemented using Lee's algorithm */
/* Bjorn Wesen 1997 */

#include "mp3dec.h"

/*

  The DCT matrix for N values is defined as:

  D(i,j) = cos((2*j+1)*i*PI/(2*N))

  Lee's fast-DCT algorithm, as used here, needs an 8-value DCT
  and an 16-value DCT matrix.

*/

#ifdef USE_DATA
#include "fastsb_A8.h"
#include "fastsb_B8.h"
#include "fastsb_B16.h"
#else

static mpfloat A16[16][16], A8[8][8];       /* DCT matrix         */
static mpfloat G16[16][16], G8[8][8];       /* Output butterfly   */
static mpfloat H16[16][16], H8[8][8];       /* Scaling            */

static mpfloat B16[16][16], B8[8][8];       /* B = G * DCT * H    */

#if 0
static mpfloat A32[32][32];
#endif

void matrix_mul16(mpfloat in1[16][16],
		  mpfloat in2[16][16],
		  mpfloat out[16][16]);

void matrix_mul8(mpfloat in1[8][8],
		 mpfloat in2[8][8],
		 mpfloat out[8][8]);

void
fast_idct_init()
{
    int i,j;
    mpfloat t16[16][16], t8[8][8];

#if 0
    for(i = 0; i < 32; i++)
	for(j = 0; j < 32; j++)
	    A32[i][j] = cos((2*j+1)*i*PI/64);
#endif

    /* create the 16 matrixes */

    for(i = 0; i < 16; i++) {
	for(j = 0; j < 16; j++) {
	    A16[i][j] = cos((2*j+1)*i*PI/32);
	    if(i == j || j == (i + 1))
		G16[i][j] = 1.0f;
	    else
		G16[i][j] = 0.0f;
	    if(i == j)
		H16[i][j] = 1.0f/(2*cos((2*i+1)*PI/64));
	    else
		H16[i][j] = 0.0;
	}
    }

    /* create the 8 matrixes */

    for(i = 0; i < 8; i++) {
	for(j = 0; j < 8; j++) {
	    A8[i][j] = cos((2*j+1)*i*PI/16);
	    if(i == j || j == (i + 1))
		G8[i][j] = 1.0f;
	    else
		G8[i][j] = 0.0f;
	    if(i == j)
		H8[i][j] = 1.0f/(2*cos((2*i+1)*PI/32));
	    else
		H8[i][j] = 0.0f;
	}
    }

    /* generate the B matrixes */

    matrix_mul16(A16, H16, t16);
    matrix_mul16(G16, t16, B16);

    matrix_mul8(A8, H8, t8);
    matrix_mul8(G8, t8, B8);

#ifdef MAKE_DATA
    make_data_file_2d("fastsb_A8.h", "A8", &A8[0][0], 8, 8);
    make_data_file_2d("fastsb_B8.h", "B8", &B8[0][0], 8, 8);
    make_data_file_2d("fastsb_B16.h", "B16", &B16[0][0], 16, 16);
#endif
}

#endif

/* This is a two-level implementation of Lee's fast-DCT algorithm */
/* 
   The 32 input values are split in two 16-value vectors using an
   even butterfly and an odd butterfly. The odd values are taken
   through Lee's odd path using a 16x16 DCT matrix (A16) and appropriate
   scaling (G16*A16*H16). The even values are further split into
   two 8-value vectors using even and odd butterflies into ee and eo.
   The ee values are fed through an 8x8 DCT matrix (A8) while the eo
   values are fed through the odd path using G8*A8*H8.

   This two-level configuration uses 384 muls and 432 adds, compared
   to the direct 32x32 DCT which uses 1024 muls and 992 adds.
*/

#ifndef USE_C3X_ASM
void
fast_idct(mpfloat *in, mpfloat *out)
{
    mpfloat even[16], odd[16], ee[8], eo[8];
    mpfloat s1, s2;
    mpfloat t[32];
    int i, j;

#if 0
    /* direct 32x32 idct */

    for(i = 0; i < 32; i++) {
	s1 = 0.0;
	for(j = 0; j < 32; j++)
	    s1 += in[j] * A32[i][j];
	t[i] = s1;
    }
#endif

    /* input butterflies - level 1 */
    /* 32 adds */

    for(i = 0; i < 16; i++) {
	even[i] = in[i] + in[31-i];
	odd[i] = in[i] - in[31-i];
    }

    /* input butterflies - level 2 */
    /* 16 adds */

    for(i = 0; i < 8; i++) {
	ee[i] = even[i] + even[15-i];
	eo[i] = even[i] - even[15-i];
    }

    /* multiply the even_even vector (ee) with the ee matrix (A8) */
    /* multiply the even_odd vector (eo) with the eo matrix (B8) */
    /* 128 muls, 128 adds */

    for(i = 0; i < 8; i++) {
	s1 = 0.0;
	s2 = 0.0;
	for(j = 0; j < 8; j += 2) {
	    s1 += A8[i][j] * ee[j] +
		A8[i][j+1] * ee[j+1];
	    s2 += B8[i][j] * eo[j] +
		B8[i][j+1] * eo[j+1];
	}
	ISCALE(s1);
	t[i*4] = s1;
	ISCALE(s2);
	t[i*4+2] = s2;
    }

#if 0
    /* multiply the even vector (even) with the even matrix (A16) */
    /* JUST FOR TESTING if we only want to use a 1-level Lee */

    for(i = 0; i < 16; i++) {
	s1 = 0.0;
	for(j = 0; j < 16; j++) {
	    s1 += A16[i][j] * even[j];
	}
	ISCALE(s1);
	t[i*2] = s1;
    }
#endif

    /* multiply the odd vector (odd) with the odd matrix (B16) */
    /* 256 muls, 256 adds */

    for(i = 0; i < 16; i++) {
	s1 = 0.0;
	for(j = 0; j < 16; j += 4) {
	    s1 += B16[i][j] * odd[j] +
		B16[i][j+1] * odd[j+1] +
		B16[i][j+2] * odd[j+2] +
		B16[i][j+3] * odd[j+3];
	}
	ISCALE(s1);
	t[i*2+1] = s1;
    }

    /* the output vector t now is expanded to 64 values using the
       symmetric property of the cosinus function */

    for(i = 0; i < 16; i++) {
	out[i] = t[i+16];
	out[i+17] = -t[31-i];
	out[i+32] = -t[16-i];
	out[i+48] = -t[i];
    }
    out[16] = 0.0;
}
#endif

#ifndef USE_DATA
void matrix_mul16(mpfloat in1[16][16],
		  mpfloat in2[16][16],
		  mpfloat out[16][16])
{
    int i,j,z;

    for(i = 0; i < 16; i++) {
	for(j = 0; j < 16; j++) {
	    out[i][j] = 0.0;
	    for(z = 0; z < 16; z++)
		out[i][j] += in1[i][z] * in2[z][j];
	    ISCALE(out[i][j]);
	}
    }
}

void matrix_mul8(mpfloat in1[8][8],
		 mpfloat in2[8][8],
		 mpfloat out[8][8])
{
    int i,j,z;

    for(i = 0; i < 8; i++) {
	for(j = 0; j < 8; j++) {
	    out[i][j] = 0.0;
	    for(z = 0; z < 8; z++)
		out[i][j] += in1[i][z] * in2[z][j];
	    ISCALE(out[i][j]);
	}
    }
}
#endif

