/* MPEG AUDIO LAYER 3 DECODER */
/* Bjorn Wesen 1997           */

/*#define USE_DATA */

#undef ROCKFORD

#ifndef DSP
#define DECVERBOSE
#endif

#include "mp3dec.h"

#ifdef DSPSIMUL
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#else
#ifdef DSP
#include "c3x_start.h"
#include "c3x_control.h"
#endif
#endif

#ifdef INT_MATH

mpfloat
operator+(mpfloat a, mpfloat b) {
    return mpfloat(a._val + b._val);
}

mpfloat
operator-(mpfloat a, mpfloat b) {
    return mpfloat(a._val - b._val);
}

mpfloat
operator-(mpfloat a) {
    return mpfloat(-a._val);
}

mpfloat
operator*(mpfloat a, mpfloat b) {
    return mpfloat(a._val * b._val);
}

mpfloat operator/(mpfloat a, mpfloat b) {
    return mpfloat(a._val / b._val);
}

#endif


static float Frame_sampfreqs[4] = { 44.1, 48, 32, 0 };
static int Frame_bitrates[15] = { 0, 32, 40, 48, 56, 64, 80, 96, 
				  112, 128, 160, 192, 224, 256, 320 };

/* scalefac_compress indexes into this table to find the sizes of the 
   scalefactor bitfields */

static int Frame_slen[2][16] = { { 0, 0, 0, 0, 3, 1, 1, 1,
				   2, 2, 2, 3, 3, 3, 4, 4 },
				 { 0, 1, 2, 3, 0, 1, 2, 3,
				   1, 2, 3, 1, 2, 3, 2, 3 } };

/* for each samplerate there is a special division of the 576 frequency
   lines into bands */

struct Frame_band Frame_bands[3] = {
    
    { { 0,4,8,12,16,20,24,30,36,44,52,
	62,74,90,110,134,162,196,238,288,342,418,576 },
      { 0,4,8,12,16,22,30,40,52,66,84,106,136,192 }},

    { { 0,4,8,12,16,20,24,30,36,42,50,
	60,72,88,106,128,156,190,230,276,330,384,576 },
      { 0,4,8,12,16,22,28,38,50,64,80,100,126,192 }},

    { { 0,4,8,12,16,20,24,30,36,44,54,
	66,82,102,126,156,194,240,296,364,448,550,576 },
      { 0,4,8,12,16,22,30,42,58,78,104,138,180,192 }}
};


static Bitstream Frame_bitbuf;   /* used for keeping the main data */


#ifdef DECVERBOSE

void dump_floats(mpfloat *f, int num)
{
    int i;
    printf("\n");
    for(i = 0; i < num; i++) {
	if(i && !(i % 5))
	    printf("\n");
	printf("%10f ", f[i]);
    }
    printf("\n");
}


void dump_ints(int *f, int num)
{
    int i;
    printf("\n");
    for(i = 0; i < num; i++) {
	if(i && !(i % 5))
	    printf("\n");
	printf("%8d ", f[i]);
    }
    printf("\n");
}
#endif

int frameNum = 0;
#ifdef UNIX
int
main(int argc, char **argv)
{
    FILE *fp; 
    int i = 0;
    char nameb[256];
    PCMSample samples[576*4];
    int numframes = 1024;
    Bitstream my_bs;
    Frame my_frame;

    my_bs.buffer32 = (unsigned long *)malloc(0x2000 * 4);
    
    if(argc < 2)
	exit(0);
    
    if(argc == 3)
	numframes = atoi(argv[2]);

    Frame_init();
    HuffmanTable_init();
    Granule_init();
    sprintf(nameb, "%s.mp3", argv[1]);
    Bitstream_open(&my_bs, nameb);
    sprintf(nameb, "%s.raw", argv[1]);
    fp = fopen(nameb, "wb");
    printf("Decoding...\n");
    do {
	fprintf(stderr, "\r[%6d]", i++);
	fflush(stderr);
	frameNum++;
	Frame_load(&my_frame, &my_bs, samples);
	fwrite(samples, 2, 576*4, fp);
/*    Frame_dump(&my_frame); */
    } while(!Bitstream32_fillbuffer(&my_bs) && i < numframes);
    printf("\n");
    Bitstream_close(&my_bs);
    fclose(fp);
    
    exit(0);
}

#endif

#ifdef DSP
 
#include "testmp.h"
//static unsigned char inbuffer[20000];

main()
{
    int i, jayjay;
#ifdef DSP_LOFI
    PCMSample samples[288*40];
#else
    PCMSample samples[576*4];
#endif
    int numframes = 40;
    Bitstream my_bs;
    Frame my_frame;
    static unsigned char bit_reservoir[BITSTREAM_BUFSIZE + 4];
    static unsigned char buffel[BITSTREAM_BUFSIZE + 4];

#ifdef DSPSIMUL
    FILE *output;
#else
#ifndef ROCKFORD
    init_da();
#endif
#endif

#ifdef ROCKFORD
    for(jayjay = 0; jayjay < 10;) {
#endif
    i = 0;
    Frame_init();
    Frame_bitbuf.buffer = bit_reservoir;

    HuffmanTable_init();
    Granule_init();

    Bitstream_open(&my_bs, 0);
    my_bs.buffer = buffel;
    Bitstream_fillbuffer(&my_bs);
#ifdef DSPSIMUL
    output = fopen("outfil.raw", "wb");
#endif
    do {
	frameNum++;
	Frame_load(&my_frame, &my_bs, samples + 288*i);
#ifdef DSPSIMUL
	fprintf(stderr, ".");
	fflush(stderr);
	fwrite(samples, 1, 576*4, output);
#else
	/*play_buffer((void *)(samples + 288*i));*/
#endif
	i++;
	Bitstream_fillbuffer(&my_bs);
    } while(i < numframes);
#ifdef ROCKFORD
    }
#else
    play_buffer((void *)samples);
#endif
#ifdef DSPSIMUL
    fprintf(stderr, "\n");
    fclose(output);
    exit(0);
#endif
    while(1);

}
#endif

static int Frame_bitbuf_framestart = 0;

void
Frame_init()
{
    Bitstream_open(&Frame_bitbuf, 0);
}


void
Frame_load(Frame *f, Bitstream *bs, PCMSample *samps)
{
  int ch, scfsi_band, gr, i, previous_end, alignbits,
      trash, part2_start;

  f->samples = samps;

  /* seek the sync word */
  if(!Bitstream_seek_sync(bs)) {
#ifndef DSP
    printf("Couldn't find sync.\n");
#endif
    return;
  }

  /* check that the ID is 1, describing ISO/IEC 11172-3 audio */
  if(!Bitstream32_get1(bs)) {
#ifndef DSP
    printf("Incorrect frame ID = 0!\n");
#endif
    return;
  }

  /* check that its a layer 3 frame */
  if(Bitstream32_get(bs, 2) != LAYER3) {
#ifndef DSP
      printf("Not a layer-3 frame!\n");
#endif
      return;
  }

  /* read the header */

  f->CRC_enable = !Bitstream32_get1(bs);  /* meaning of this bit is inverted */
  f->bitrate_index = Bitstream32_get(bs, 4);
  f->sampling_frequency = Bitstream32_get(bs, 2); 
  f->padding_bit = Bitstream32_get1(bs);
  (void)Bitstream32_get1(bs);  /* skip the private bit, we dont use it */
  f->mode = Bitstream32_get(bs, 2);
  f->channels = (f->mode == MODE_SINGLE_CHANNEL) ? 1 : 2;
  f->mode_extension = Bitstream32_get(bs, 2);
  f->copyright = Bitstream32_get1(bs);
  f->originality = Bitstream32_get1(bs);
  f->emphasis = Bitstream32_get(bs, 2);

  /* if this frame has a CRC, read it */
  
  if(f->CRC_enable)
    f->CRC = Bitstream32_get(bs, 16);

  /* now read the Layer-3 audio_data part of the frame */

  /* the following data constitutes the side information stream -
       - main_data_begin pointer
       - side info for both granules (scfsi)
       - side info granule 1
       - side info granule 2
  */

  /* read the starting offset of the main data */
  
  f->main_data_begin = Bitstream32_get(bs, 9);

  /* read private parts */
  
  if(f->channels == 1)
    (void)Bitstream32_get(bs, 5);   /* 5 private bits for single_channel */
  else
    (void)Bitstream32_get(bs, 3);   /* 3 private bits for other modes */

  /* read scalefactor selection information, 4 bits per channel */
  
  for(ch = 0; ch < f->channels; ch++)
    for(scfsi_band = 0; scfsi_band < 4; scfsi_band++)
      f->scfsi[ch][scfsi_band] = Bitstream32_get1(bs);
  
  /* read the side info for the channels for each granule */
  
  for(gr = 0; gr < 2; gr++)  
    for(ch = 0; ch < f->channels; ch++)
	Granule_decode_info(&f->gr[gr][ch], bs);
    
  /* calculate the size of this header in bytes */

  f->header_size = 4 + 2 * f->CRC_enable + (f->channels == 1 ? 17 : 32);

  /* calculate the size of the main_data block of this frame */

  /* the distance between two consequitive syncwords is determined from
     the formula: N = 144 * bitrate / sampling_frequency + padding */

  /* then we find the size of the main data by taking that number and
     subtracting the size of the header and padding */

  f->main_data_size = (int)(((144 * Frame_bitrates[f->bitrate_index]) /
    Frame_sampfreqs[f->sampling_frequency])) + f->padding_bit - f->header_size;

  /* extract all main data of this frame and insert it into the bitbuffer */
  /* ## TODO check if main data is bytealigned, if so, we can copy it much
     faster */
  
  for(i = f->main_data_size; i > 0; i--)
    Bitstream_putbyte(&Frame_bitbuf, Bitstream32_get(bs, 8));

/*   
  printf("Inserted %d bytes of main data into buffer.\n", f->main_data_size);
  */

  /* get the end (in bytes) of the last used main data */
  
  previous_end = Bitstream_tell(&Frame_bitbuf) / 8;  
/*
  printf("previous_end %d\n", previous_end);
  */
 
  /* align the bit buffer to a byte boundary */
  
  if((alignbits = Bitstream_tell(&Frame_bitbuf) % 8)) {
    Bitstream_get(&Frame_bitbuf, 8 - alignbits);
    previous_end++;
  }

  /* see if there is a gap between the end of the last framedata and
     the start of the new framedata, and if so, discard it. If trash 
     is negative, it means there was an error and there isn't enough
     data to decode this frame */
 
  trash = Frame_bitbuf_framestart - previous_end - f->main_data_begin;

  if(previous_end > BITSTREAM_BUFSIZE) {
    Frame_bitbuf_framestart -= BITSTREAM_BUFSIZE;
    Bitstream_rewindbytes(&Frame_bitbuf, BITSTREAM_BUFSIZE);
  }

  if(trash < 0) {
#ifndef DSP
    printf("Too little data to decode frame.\n");
#endif
    return;
  } else if(trash > 0) {
#ifndef DSP
    printf("%d bytes of trash\n", trash);
#endif
    Bitstream_get(&Frame_bitbuf, 8 * trash);
  }

  /* duplicate the end to reduce the need for modulo calcs in huffman */
  
  Frame_bitbuf.buffer[BITSTREAM_BUFSIZE] = Frame_bitbuf.buffer[0];
  Frame_bitbuf.buffer[BITSTREAM_BUFSIZE + 1] = Frame_bitbuf.buffer[1];
  Frame_bitbuf.buffer[BITSTREAM_BUFSIZE + 2] = Frame_bitbuf.buffer[2];
  Frame_bitbuf.buffer[BITSTREAM_BUFSIZE + 3] = Frame_bitbuf.buffer[3];

  /* remember the start of the next frame */

  Frame_bitbuf_framestart += f->main_data_size;

  /* the Frame_bitbuf stream is now positioned at the start of the
     main_data for this frame, and we know there is enough data to
     decode it, so start reading the granules now. */

  /* the following data constitute the main data stream -
       - scalefactors and Huffman code data granule 1
       - scalefactors and Huffman code data granule 2
       - ancillary data
  */

  /* Bands, scalefactors etc:
       The spectrum of 576 frequency lines, is divided into bands 
       differently depending on sampling frequency and block type.
       Each band has its own scalefactor, which is read below.
       Each band also has its own huffman coded energies, read further
       below.
  */

  /* read the main data stream for both granules */

  for(gr = 0; gr < 2; gr++) {
      
      /* read the main data stream for all channels in the granule */

      Granule_floatfreqs xr[2];     /* 2*576 = 1152 */
      Granule_floatfreqs lr[2];     /* 2*576 = 1152 */

      for(ch = 0; ch < f->channels; ch++) {
	  Granule_intfreqs is;  /* for storing the de-huffmanized energies */

	  part2_start = Bitstream_tell(&Frame_bitbuf);
	  
	  /* decode the scalefactors */

	  Granule_decode_scalefactors(&f->gr[gr][ch], &Frame_bitbuf,
				      ch, gr, f);
	  
	  /* now comes the Huffman coded bits */
	  PROFSTART;

	  Granule_decode_huffman(&f->gr[gr][ch], &Frame_bitbuf, 
				 part2_start, is, f);
	  PROFSTOP;
/*
	  if(frameNum == 85)
	      dump_ints(is, 576);
	      */      
	  Granule_requantize(&f->gr[gr][ch], is, xr[ch], f);

      } /* end of channel loop */

      /* we have the decoded and requantized spectrum for both channels,
	 so we can apply stereo processing */
      /* xr -> lr */

      Granule_process_stereo(&f->gr[gr][0], &f->gr[gr][1], xr, lr, f);
      
      /* now we can, for each channel, synthesize the PCM output */

      for(ch = 0; ch < f->channels; ch++) {
	  
	  /* first we have to reorder short block data */
	  /* lr -> xr */
	  Granule_reorder(&f->gr[gr][ch], lr[ch], xr[ch], f);
/*
	  if(frameNum == 10) {
	      printf("Granule %d\n", gr);
	      dump_floats(lr2[ch], 576);
	  }
	  */
	  /* now we do the alias reduction */
	  /* xr -> xr */
	  Granule_antialias(&f->gr[gr][ch], xr[ch]);
	      
	  /* apply the IMDCT transform */
	  /* xr -> xr */
	  Granule_imdct(&f->gr[gr][ch], ch, xr[ch]);
	  /* frequency inversion to compensate for the polyphase filterbank */
	  /* xr -> xr */
	  
	  Granule_freqinverse(&f->gr[gr][ch], xr[ch]);

	  /* finally generate the PCM samples from the filterbank */
/*
	  if(frameNum == 10) {
	      int z;
	      for(z = 0; z < 576; z++) {
		  if(z && !(z % 5))
		      printf("\n");
		  printf("%6d ", f->samples[gr][z][0]);
	      }
	  }
	  */
      }

#ifdef WIN32
#define FABS(x) (x > 0.0 ? x : -x)
	  /* the windows player has a nice spectrum analyser.. */
	  {
	      int z, y;
	      for(z = 0; z < 32; z++) {
		  f->spectrum[z] = 0.0f;
		  for(y = 0; y < 18; y++)
		      f->spectrum[z] +=
			  FABS(xr[0][z*18+y]) +
			  FABS(xr[1][z*18+y]);
	      }
	  }
#endif

      /* for stereo, use an optimized subband synthesis routine that
	 processes both channels at once */
      /* xr -> samples */

#ifdef DSP_LOFI
      /* do lo-fidelity processing, mono in 11khz *shiver* */
      Granule_subband_synthesis(&f->gr[gr][0], 0, xr[0],
				&f->samples[gr*144]);
#else

      if(f->channels == 2)
	  Granule_subband_synthesis2(&f->gr[gr][ch], xr[0],
				     xr[1],
				     &f->samples[gr*576*2]);
      else
	  Granule_subband_synthesis(&f->gr[gr][0], 0, xr[0],
				    &f->samples[gr*576*2]);
#endif  

  } /* end of granule loop */

}

#ifdef MAKE_DATA

void
make_data_file(const char *fname, const char *datname, mpfloat *nbrs, int len)
{
    int i;
    FILE *f = fopen(fname, "w");
    fprintf(f, "static mpfloat %s[%d] = {\n", datname, len);
    for(i = 0; i < len; i++) {
	if(i && !(i % 4))
	    fprintf(f, "\n");
	fprintf(f, "%.12g, ", nbrs[i]);
    }
    fprintf(f, "\n};\n\n");

    fclose(f);
}

void
make_data_file_2d(const char *fname,
		  const char *datname, mpfloat *nbrs, int len1, int len2)
{
    int i, j;
    FILE *f = fopen(fname, "w");
    fprintf(f, "static mpfloat %s[%d][%d] = {\n", datname, len1, len2);
    for(j = 0; j < len1; j++) {
	fprintf(f, "{\n");
	for(i = 0; i < len2; i++) {
	    if(i && !(i % 4))
		fprintf(f, "\n");
	    fprintf(f, "%.12g, ", nbrs[i + j * len2]);
	}
	fprintf(f, "\n},\n");
    }
    fprintf(f, "\n};\n\n");

    fclose(f);
}

#endif


#ifdef USE_DATA
#include "pow2table1.h"
#include "pow2table2.h"
#include "powerscales.h"
#include "imdct_win.h"
#include "globgaintable.h"
#else
static mpfloat Granule_pow2table_m2[32]; /* (2^(-2))^i */
static mpfloat Granule_pow2table_m05[32]; /* (2^(-0.5))^i */
static mpfloat Granule_powerscaling[200]; /* i^(4/3) */
mpfloat Granule_imdct_win[4][36];
static mpfloat Granule_globgaintable[256];
#endif

static float Granule_alias_c[8] = { -0.6f, -0.535f, -0.33f, -0.185f,
				    -0.095f, -0.041f, -0.0142f, -0.0037f };

mpfloat Granule_alias_cs[8], Granule_alias_ca[8];

#ifdef LEE_IMDCT
#ifdef USE_DATA
#include "idct_9x9.h"
#else
mpfloat Granule_9x9_idct[72];
#endif
#else
static mpfloat Granule_imdct_bigCOS[18*18];
static mpfloat Granule_imdct_bigCOS2[12*6];
#endif

mpfloat Granule_imdct_previous[2][576];  /* used for overlapping */

int Granule_sbsynth_Vptr[2] = { 64, 64 };
#include "mp3dec_D.h"

void
Granule_init()
{
    int i, j, k, m, p, x1, x2;
	int odd_i, two_odd_i, four_odd_i, eight_odd_i;
    float f;
    static int inited = 0;

    if(inited)
	return;

    inited = 1;

    /* initialize some powertables used in Granule_requantize */
    /* ## alternative is to make a .h file with these calculated
       directly in the source */
#ifndef USE_DATA
    for(i = 0; i < 32; i++) {
	Granule_pow2table_m2[i] = (mpfloat)pow(2.0, -2.0 * i);
	Granule_pow2table_m05[i] = (mpfloat)pow(2.0, -0.5 * i);
    }
#endif

#ifdef MAKE_DATA
    make_data_file("pow2table1.h", "Granule_pow2table_m2", 
		   Granule_pow2table_m2, 32);
    make_data_file("pow2table2.h", "Granule_pow2table_m05", 
		   Granule_pow2table_m05, 32);
#endif

#ifndef USE_DATA
    for(i = 0; i < 200; i++)
	Granule_powerscaling[i] = (mpfloat)pow( i, (4.0 / 3.0) );
#endif

#ifdef MAKE_DATA
    make_data_file("powerscales.h", "Granule_powerscaling", 
		   Granule_powerscaling, 200);
#endif

    /* calculate the anti-aliasing butterfly coefficients */

    for(i = 0; i < 8; i++) {
	f = sqrt(1.0 + Granule_alias_c[i] * Granule_alias_c[i]);
	Granule_alias_cs[i] = (mpfloat)(1.0f / f);
	Granule_alias_ca[i] = (mpfloat)(Granule_alias_c[i] / f);
    }

#ifndef USE_DATA
    /* calculate some window shapes and cos tables for the IMDCT */

    /* block_type 0 (normal window) */

    for(i = 0; i < 36; i++)
	Granule_imdct_win[0][i] = (mpfloat)sin(PI/36 * (i + 0.5));
    
    /* block_type 1 (start block) */

    for(i = 0; i < 18; i++)
	Granule_imdct_win[1][i] = (mpfloat)sin(PI/36 * (i + 0.5));

    for(i = 18; i < 24; i++)
	Granule_imdct_win[1][i] = (mpfloat)1.0f;

    for(i = 24; i < 30; i++)
	Granule_imdct_win[1][i] = (mpfloat)sin(PI/12 * (i - 18 + 0.5));

    for(i = 30; i < 36; i++)
	Granule_imdct_win[1][i] = (mpfloat)0.0f;
    
    /* block_type 3 (stop block) */

    for(i = 0; i < 6; i++)
	Granule_imdct_win[3][i] = (mpfloat)0.0f;

    for(i = 6; i < 12; i++)
	Granule_imdct_win[3][i] = (mpfloat)sin(PI/12 * (i - 6 + 0.5));

    for(i = 12; i < 18; i++)
	Granule_imdct_win[3][i] = (mpfloat)1.0f;

    for(i = 18; i < 36; i++)
	Granule_imdct_win[3][i] = (mpfloat)sin(PI/36 * (i + 0.5));
    
    /* block_type 2 (short block) */

    for(i = 0; i < 12; i++)
	Granule_imdct_win[2][i] = (mpfloat)sin(PI/12 * (i + 0.5));

    for(i = 12; i < 36; i++)
	Granule_imdct_win[2][i] = (mpfloat)0.0 ;

#endif

#ifdef MAKE_DATA
    make_data_file_2d("imdct_win.h", "Granule_imdct_win", 
		   &Granule_imdct_win[0][0], 4, 36);
#endif
          
#ifdef LEE_IMDCT
#ifndef USE_DATA
    j = 0;
    for(i = 0; i < 9; i++) {
	odd_i = (i << 1) + 1;
	two_odd_i = odd_i << 1;
	four_odd_i = odd_i << 2;
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * odd_i);
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * two_odd_i);
	eight_odd_i = two_odd_i << 2;
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * (four_odd_i - odd_i));
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * four_odd_i);
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * (four_odd_i + odd_i));
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * (four_odd_i + two_odd_i));
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * (eight_odd_i - odd_i));
	Granule_9x9_idct[j++] = (mpfloat)cos(PI/18 * eight_odd_i);
    }
#endif
#ifdef MAKE_DATA
    make_data_file("idct_9x9.h", "Granule_9x9_idct", 
		   Granule_9x9_idct, 72);
#endif

#else
    /* make the costable for the short blocks */
    for(i = 0, p = 0; i < 6; i++)
	for(j = 0; j < 6; j++)
	    Granule_imdct_bigCOS2[p++] = (mpfloat)-cos((2*(i-3)+7)*
						       (2*j+1)*PI/24);

    /* make a linear cos factor table, so the IMDCT can be calculated
       from it without messing with any modulos, weird index additions
       etc */

    for(i = 0, p = 0; i < 18; i++) {
	for(j = 0; j < 18; j++)
	    Granule_imdct_bigCOS[p++] = (mpfloat)-cos((2*(i-9)+19)*
						      (2*j+1)*PI/72); 
    }
#endif

    /* clear the previous data table used for overlapping in IMDCT */
    
    for(p = 0; p < 2; p++)
	for(i = 0; i < 576; i++)
	    Granule_imdct_previous[p][i] = (mpfloat)0.0f;

#ifndef USE_DATA
    fast_idct_init();
#endif

#ifndef INT_MATH
    /* rescale the D table to include the 32768 factor */
    for(i = 0; i < 512; i++)
#ifdef DSP_LOFI
	Granule_sbsynth_D[i] *= 8192;
#else
	Granule_sbsynth_D[i] *= 32768;
#endif
#endif
#if 0
    for(i = 0; i < 512; i++)
	printf("D[%d] = 0x%8x, ", i, Granule_sbsynth_D[i].rint());
#endif

    windowing_init();

#ifndef USE_DATA
    for(i = 0; i < 256; i++)
	Granule_globgaintable[i] = (mpfloat)pow(2.0 , 
						(0.25 * (i -
							 210.0)));
#endif
#ifdef MAKE_DATA
    make_data_file("globgaintable.h", "Granule_globgaintable", 
		   Granule_globgaintable, 256);
#endif

}

void
Granule_decode_info(Granule *gr, Bitstream *bs)
{
    int region, window;

    gr->part2_3_length = Bitstream32_get(bs, 12);
    gr->big_values = Bitstream32_get(bs, 9);
    gr->global_gain = Bitstream32_get(bs, 8);
    gr->scalefac_compress = Bitstream32_get(bs, 4);
      
    if((gr->window_switching_flag = Bitstream32_get1(bs))) {
	
	/* the block_type indicates the window type for the granule */
	
	gr->block_type = Bitstream32_get(bs, 2);

	gr->mixed_block_flag = Bitstream32_get1(bs);

	/* table selectors select different Huffman tables for each
	   frequency region */

	for(region = 0; region < 2; region++)
	    gr->table_select[region] = Bitstream32_get(bs, 5);

	/* subblock_gain multiplied by 4 indicates the gain offset for
	   each window, compared to the global_gain of the granule */
	
	for(window = 0; window < 3; window++)
	  gr->subblock_gain[window] = Bitstream32_get(bs, 3);

	/* region0count and region1count are set to default values */

	if(gr->block_type == BLOCKTYPE_3WIN &&
	   !gr->mixed_block_flag)
	    gr->region0_count = 8;
	else
	    gr->region0_count = 7;
	
	gr->region1_count = 20 - gr->region0_count;

    } else {
	
	gr->block_type = 0;
	
	for(region = 0; region < 3; region++)
	    gr->table_select[region] = Bitstream32_get(bs, 5);
	
	gr->region0_count = Bitstream32_get(bs, 4);
	gr->region1_count = Bitstream32_get(bs, 3);
    }

    gr->preflag = Bitstream32_get1(bs);
    gr->scalefac_scale = Bitstream32_get1(bs);
    gr->count1table_select = Bitstream32_get1(bs);   
}

void
Granule_decode_scalefactors(Granule *gr, Bitstream *bs, 
			    int channel, int grnum, Frame *f)
{
    int slen1, slen2, i, sfb, window;

    slen1 = Frame_slen[0][gr->scalefac_compress];
    slen2 = Frame_slen[1][gr->scalefac_compress];

    /* read the scalefactors - these are read in two different ways
       depending on the block type */

    if(gr->window_switching_flag &&
       gr->block_type == BLOCKTYPE_3WIN) {

	if(gr->mixed_block_flag) {
	    
	    /* mixed blocks - a mixed block uses
	       both long scalefactors (0-7) and short scalefactors (3-11)
	       which together makes up the whole spectrum 
	       */
	    
	    /* block_type 2 and mixed_block_flag 1, 
	       slen1: length of scalefactors for bands 0 to 7 of the
	       long sf and length of sf's 3 to 5
	       of the short scalefactor band 
	       slen2: length of scalefactors for bands 6 to 11 of the
	       short scalefactor band
	       */
	    
	    for(sfb = 0; sfb < 8; sfb++) 
		gr->scalefac_l[sfb] =
		    Bitstream_get(bs, slen1);
	    
	    for(sfb = 3; sfb < 6; sfb++)
		for(window = 0; window < 3; window++)
		    gr->scalefac_s[sfb][window] =
			Bitstream_get(bs, slen1);
	    
	    for(sfb = 6; sfb < 12; sfb++)
		for(window = 0; window < 3; window++)
		    gr->scalefac_s[sfb][window] =
			Bitstream_get(bs, slen2);
	    
	    for(window = 0; window < 3; window++)
		    gr->scalefac_s[sfb][window] = 0;
		
	} else {
	    
	    /* short block using only short scalefactors
	       slen1: length of scalefactors for short sf bands 0 to 5
	       slen2: length of scalefactors for short sf bands 6 to 11
	       */
	    
	    for(sfb = 0; sfb < 6; sfb++)
		for(window = 0; window < 3; window++)
		    gr->scalefac_s[sfb][window] =
			Bitstream_get(bs, slen1);
	    
	    for(sfb = 6; sfb < 12; sfb++)
		for(window = 0; window < 3; window++)
		    gr->scalefac_s[sfb][window] =
			Bitstream_get(bs, slen2);

	    for(window = 0; window < 3; window++)
		    gr->scalefac_s[sfb][window] = 0;
	    
	}  
	
    } else {
	
	/* long block
	   slen1: length of scalefactors for long sf bands 0 to 10
	   slen2: length of scalefactors for long sf bands 11 to 20
	   */
	
	/* the sf bands are divided in 4 parts, 0-5, 6-10, 11-15, 16-20
	   the standard seems to disagree with itself here, but apparently
	   it works this way:
	   */
	
	static int sfb_bound[5] = { 0, 6, 11, 16, 21 };
	
	for(i = 0; i < 4; i++)
	    if(f->scfsi[channel][i] == 0 || grnum == 0)
		for(sfb = sfb_bound[i]; sfb < sfb_bound[i + 1]; sfb++) 
		    gr->scalefac_l[sfb] =
			Bitstream_get(bs, sfb < 11 ? slen1 : slen2);
	
	gr->scalefac_l[21] = 0;
	gr->scalefac_l[22] = 0;

    }    
}


/* The so-called Requantizer takes the de-huffmanized energies of each
   frequency and applies the scalefactors to them to rescale them to 
   their former glory. 
 */

void
Granule_requantize(Granule *gr, Granule_intfreqs is,
		   Granule_floatfreqs xr, Frame *f)
{
    /* 
       We have to go through all frequency lines in all bands and apply
       a large formula to the is[freq] value. All bands have their own
       scalefactors that are incorporated in this formula, so we have to
       extract these per band before requantizing a band.
     
       The formula for short blocks is:

       xr[i] = sign(is[i]) * |is[i]|^(4/3) * 2 ^ 0.25 * (global_gain - 210 -
               8 * subblock_gain[window]) * 2 ^ -(scalefac_multiplier *
	       scalefac_s[band][window])

       and for long blocks:

       xr[i] = sign(is[i]) * | is[i] |^(4/3) * 2 ^ 0.25 * (global_gain - 210) *
               2 ^ -(scalefac_multiplier * (scalefac_l[band] + preflag *
	       pretab[band]))

       The preflag when set applies a predefined pre-emphasis value from
       the pretab table. 

       The 2 ^ operation is very costly, but fortunately, the range of the
       possible exponents is fairly small so we can do a table lookup and
       avoid all 2 ^ operations.

    */

    static int pretab[22] = { 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2,0 };
    int i, band, bandend, window, windowlength = 0, j = 0, t, sign, needc;
    mpfloat myxr = (mpfloat)0.0f;

    /* get the correct band-boundaries table */

    struct Frame_band *bands = &Frame_bands[f->sampling_frequency];

    /* get the global scaling factor - ## see if this can be table-ized */
/*     globscale = (mpfloat)pow(2.0 , 
		       	      (0.25 * (gr->global_gain -
			      		       210.0)));
					       */

    mpfloat globscale = Granule_globgaintable[gr->global_gain];
	
    /* In mixed blocks, the lowest two polyphase subbands are transformed
       using normal block bands, and the rest according to block_type.
       
       Remember that band boundaries in short blocks should be multiplied
       by 3 (since there are 3 windows in each band).

    */

    if(gr->window_switching_flag && gr->block_type == BLOCKTYPE_3WIN &&
       !gr->mixed_block_flag) {
	bandend = bands->s[1] * 3;  /* short block */
	j = windowlength = bands->s[1];
    } else
	bandend = bands->l[1];      /* normal block or mixed short block */

    band = window = 0;
    needc = 1;
    
    for(i = 0; i < 576; i++) {
	
	if(i == bandend) {

	    needc = 1;
	    /* advance to the next band */
	    
	    if(gr->window_switching_flag && gr->block_type == BLOCKTYPE_3WIN) {

		window = 0;   /* reset window number */

		if(gr->mixed_block_flag) {

		    /* a short, mixed block is coded as a long block for
		     the first two polyphase subbands, so we have 3 
		     situations here - either we are in the long part, we are
		     on the boundary, or we are in the short part 

		     bands->l[8] == bands->s[3] == 36 for all frequencies

		     */

		    if(i == bands->l[8]) {
			/* on the boundary */
			bandend = bands->s[4] * 3;
			band = 3;
			windowlength = 4;  /* s[4]-s[3] is same for all freq */
		    } else {
			if(i < bands->l[8]) 
			    /* in the long part */
			    bandend = bands->l[++band + 1];
			else {
			    /* in the short part */
			    bandend = bands->s[++band + 1] * 3;
			    windowlength = bands->s[band + 1] - bands->s[band];
			}
		    }
		} else {
		    /* pure short block */
		    bandend = bands->s[++band + 1] * 3;
		    windowlength = bands->s[band + 1] - bands->s[band];
		}

		j = windowlength;

		/* For short blocks, i.e blocks with 3 windows in each band,
		   the windowlength gives how many values are in each band
		   in each window. We put this value in j, and decrease j
		   for each value. When j hits 0, we restore j and go to the
		   next window. 
		 */

	    } else
		/* long block */
		bandend = bands->l[++band + 1];
	}
	
	if(i == 36)
	    needc = 1;

/* only recalculate the xr formula when we pass between bands and
	windows
*/
	if(needc) {
	    needc = 0;
	    
	    /* start with the global part (same for all values in the block) */

	    myxr = globscale;

	    /* apply the formula according to if its a short or long block */
	    /* remember that mixed blocks are long in the lowest 2 subbands */

	    if (gr->window_switching_flag && gr->block_type == BLOCKTYPE_3WIN &&
		(!gr->mixed_block_flag || (gr->mixed_block_flag &&
					   i >= 2 * NUM_DCTBANDS))) {

		/* its a short block, use this formula:

		   xr[i] = sign(is[i])*|is[i]|^(4/3)*2^0.25*(global_gain - 210 -
		   8 * subblock_gain[window]) * 2^-(scalefac_multiplier *
		   scalefac_s[band][window])

		   the global gain part is already in *xr
	
		   we are clever enough to use tables for the powers-of-2

		   scalefac_multiplier is 0.5 for scalefac_scale == 0 and 1
		   for scalefac_scale == 1
	       
		   */
		
		t = gr->subblock_gain[window];

		if(t >= 32) { /* when dividing by 2^32, you don't have much left */
		    myxr = (mpfloat)0.0f;
		}
		else
		    myxr *= Granule_pow2table_m2[t];
		ISCALE(myxr);
   		
		t = (1 + gr->scalefac_scale) * gr->scalefac_s[band][window];
	    
		if(t >= 32) { /* Only 32 values in the table */
		    myxr = (mpfloat)0.0f;
		}
		else
		    myxr *= Granule_pow2table_m05[t];
		ISCALE(myxr);

	    } else {

		/* 
		   Long block, use this formula:

		   xr[i] = sign(is[i])*|is[i]|^(4/3)*2^0.25*(global_gain - 210) *
		   2 ^ -(scalefac_multiplier * (scalefac_l[band] + preflag *
		   pretab[band]))

		   global_gain is already in *xr

		   The preflag when set applies a pre-emphasis value from
		   the pretab table. 

		   */

		/* xr[sb][ss] *= pow(2.0, -0.5 * (1.0+gr_info->scalefac_scale)
		 * ((*scalefac)[ch].l[cb]
		 + gr_info->preflag * pretab[cb]));
		 */

		t = (1 + gr->scalefac_scale) * (gr->scalefac_l[band]
						+ gr->preflag * pretab[band]);

		if(t >= 32) { /* Only 32 values in the table */
		    myxr = (mpfloat)0.0f;
		} else
		    myxr *= Granule_pow2table_m05[t];
		ISCALE(myxr);
	    }
	}

	/* apply the sign(is[i]) * |is[i]| ^ (4/3) formula */
	
	sign = (*is < 0) ? 1 : 0; 
	t = sign ? -*is : *is;       /* t = abs(is) */
	if(t >= 200)
	    t = 199;
	
	*xr = myxr * Granule_powerscaling[t];

/*	    *xr = myxr * (mpfloat)pow((float)t, (4.0f / 3.0f)); */

	ISCALE(*xr);
	/* restore the sign */

	if(sign)
	    *xr = -*xr;

	/* advance input and output */
	
	xr++;
	is++;

	/* check if we pass into the next window in short blocks */
	if(j && !--j) {
	    j = windowlength;
	    window++;
		needc = 1;
	}
    }    
}

/* apply the selected stereo processing to the 2 xr channels, producing
   a left and right output in lr.
*/

void
Granule_process_stereo(Granule *gr, Granule *grch1, 
		       Granule_floatfreqs xr[2],
		       Granule_floatfreqs lr[2], Frame *f)
{
    int sfb;
    int i, j, sb, ch;

    /* extract some stuff from the frame header */

    int stereo = f->channels;

    /* get the correct band-boundaries table */

    struct Frame_band *bands = &Frame_bands[f->sampling_frequency];

    /* decode what stereo mode we should process */

    int i_stereo = (f->mode == MODE_JOINT_STEREO) &&
	(f->mode_extension & 0x1);
    int ms_stereo = (f->mode == MODE_JOINT_STEREO) &&
	(f->mode_extension & 0x2); 

    /* ration and positions for stereo processing */
    int is_pos[576]; 
    mpfloat is_ratio[576];

    /* the following HUGE chunk of code decodes the stereo positions and
       rations in case of special stereo modes that don't transmit using
       normal left-right signals */

    /* clear the position table */
    for(i = 0; i < 576; i++)
	is_pos[i] = 7;
    
   if ((stereo == 2) && i_stereo) {

       /* Intensity stereo mode */

       if (gr->window_switching_flag && (gr->block_type == BLOCKTYPE_3WIN)) {
	   if(gr->mixed_block_flag) {
	       int max_sfb = 0;

	       /* mixed block */

	       /* in the short block part of the spectrum, find the highest
		  non-zero frequency - process each window separately */

	       for (j = 0; j < 3; j++) {
		   int sfbcnt;
		   sfbcnt = 2;

		   for(sfb = 12; sfb >= 3; sfb--) {
		       int lines;
		       
		       /* find the last frequency line with a non-zero value */
		       
		       lines = bands->s[sfb+1] - bands->s[sfb];
		       i = 3 * bands->s[sfb] + (j + 1) * lines - 1;
		       while(lines > 0) {
			   if(xr[1][i] != 0.0f ) {
			       sfbcnt = sfb;
			       sfb = -10;
			       lines = -10;
			   }
			   lines--;
			   i--;
		       }
		   }

		   sfb = sfbcnt + 1;
		   
		   if (sfb > max_sfb)
		       max_sfb = sfb;

		   while(sfb < 12) {
		       sb = bands->s[sfb+1] - bands->s[sfb];
		       i = 3 * bands->s[sfb] + j * sb;
		       for (; sb > 0; sb--) {
			   is_pos[i] = grch1->scalefac_s[sfb][j];
			   if (is_pos[i] != 7)
			       /* ## tabelize this tan */
			       is_ratio[i] = tan(is_pos[i] * (PI / 12));
			   i++;
		       }
		       sfb++;
		   }
		   sb = bands->s[11] - bands->s[10];
		   sfb = 3 * bands->s[10] + j * sb;
		   sb = bands->s[12] - bands->s[11];
		   i = 3 * bands->s[11] + j * sb;
		   for (; sb > 0; sb--) {
		       is_pos[i] = is_pos[sfb];
		       is_ratio[i] = is_ratio[sfb];
		       i++;
		   }
	       }
	       if (max_sfb <= 3) {
		   /* find the last non-zero frequency line in the lowest 
		      3 subbands */
		   i = 2 * 18 + 17;
		   sb = -1;
		   while (i >= 0) {
		       if (xr[1][i] != (mpfloat)0.0f) {
			   sb = i;
			   break;
		       }
		       i--;
		   }
		   i = 0;
		   while(bands->l[i] <= sb)
		       i++;
		   sfb = i;
		   i = bands->l[i];
		   for (; sfb < 8; sfb++) {
		       sb = bands->l[sfb + 1] - bands->l[sfb];
		       for (; sb > 0; sb--) {
			   is_pos[i] = grch1->scalefac_l[sfb];
			   if(is_pos[i] != 7)
			       /* ## tabelize this */
			       is_ratio[i] = tan(is_pos[i] * (PI / 12));
			   i++;
		       }
		   }
	       }

	   } else {

	       /* this is a pure short block */

	       for(j = 0; j < 3; j++) {
		   int sfbcnt;
		   sfbcnt = -1;
		   for(sfb = 12; sfb >= 0; sfb--) {
		       int lines;
		       lines = bands->s[sfb + 1] - bands->s[sfb];
		       i = 3 * bands->s[sfb] + (j + 1) * lines - 1;
		       while (lines > 0) {
			   if(xr[1][i] != 0.0 ) {
			       sfbcnt = sfb;
			       sfb = -10;
			       lines = -10;
			   }
			   lines--;
			   i--;
		       }
		   }
		   sfb = sfbcnt + 1;
		   while(sfb < 12) {
		       sb = bands->s[sfb + 1] - bands->s[sfb];
		       i = 3 * bands->s[sfb] + j * sb;
		       for (; sb > 0; sb--) {
			   is_pos[i] = grch1->scalefac_s[sfb][j];
			   if (is_pos[i] != 7)
			       /* ## tabelize this */
			       is_ratio[i] = tan(is_pos[i] * (PI / 12));
			   i++;
		       }
		       sfb++;
		   }
		   
		   sb = bands->s[11] - bands->s[10];
		   sfb = 3 * bands->s[10] + j * sb;
		   sb = bands->s[12] - bands->s[11];
		   i = 3 * bands->s[11] + j * sb;
		   for (; sb > 0; sb--) {
		       is_pos[i] = is_pos[sfb];
		       is_ratio[i] = is_ratio[sfb];
		       i++;
		   }
	       }
	   }
       } else {
	   /* long block */
	   /* find the highest non-zero frequency */
	   i = 31 * 18 + 17;
	   sb = 0;
	   while (i >= 0) {
	       if(xr[1][i] != 0.0) {
		   sb = i;
		   break;
	       }
	       i--;
	   }
	   i = 0;
	   while(bands->l[i] <= sb)
	       i++;
	   sfb = i;
	   i = bands->l[i];
	   for (; sfb < 21; sfb++) {
	       sb = bands->l[sfb + 1] - bands->l[sfb];
	       for (; sb > 0; sb--) {
		   is_pos[i] = grch1->scalefac_l[sfb];
		   if (is_pos[i] != 7)
		       /* ## tabelize this */
		       is_ratio[i] = tan(is_pos[i] * (PI / 12));
		   i++;
	       }
	   }
	   sfb = bands->l[20];
	   for (sb = 576 - bands->l[21]; sb > 0; sb--) {
	       is_pos[i] = is_pos[sfb];
	       is_ratio[i] = is_ratio[sfb];
	       i++;
	   }
       }
   }
   
   /* prepare for making output */

   for(ch = 0;ch < 2; ch++)
       for(i = 0; i < 576; i++)
	   lr[ch][i] = (mpfloat)0.0f;

   /* do the stereo matrixing */

   if(stereo == 2) 
       for(i = 0; i < 576; i++) {
	   if( is_pos[i] == 7 ) {
               if(ms_stereo) {
		   lr[0][i] = (xr[0][i] + xr[1][i]) / 1.41421356;
		   lr[1][i] = (xr[0][i] - xr[1][i]) / 1.41421356;
               } else {
		   /* dual-channel mode, the most common, just copy */
		   lr[0][i] = xr[0][i];
		   lr[1][i] = xr[1][i];
               }
	   } else if (i_stereo) {
               lr[0][i] = xr[0][i] * (mpfloat)(is_ratio[i] / (1.0f + is_ratio[i]));
               lr[1][i] = xr[0][i] * (mpfloat)(1.0f / (1.0f + is_ratio[i])); 
	   ISCALE(lr[0][i]);
	   ISCALE(lr[1][i]);
	   } else {
#ifndef DSP
                printf("Illegal stereo mode!\n");
#endif
	   }
       } else
	   /* single channel mode */
	   for(i = 0; i < 576; i++)
	       lr[0][i] = xr[0][i];
}

void
Granule_reorder(Granule *gr, Granule_floatfreqs xr, 
		Granule_floatfreqs xr2, Frame *f)
{
    /* get the correct band-boundaries table */
    
    struct Frame_band *bands = &Frame_bands[f->sampling_frequency];

    int sfb, bandstart, bandsize;
    int window, i, src, dst;

    /* check the block type - we only reorder short and mixed blocks */

    if(gr->window_switching_flag && gr->block_type == BLOCKTYPE_3WIN) {
	for(i = 0; i < 576; i++)
	    xr2[i] = (mpfloat)0.0f;
	if(gr->mixed_block_flag) {
	    
	    /* lowest 2 subbands are long blocks - pass through */
	    
	    for(i = 0; i < 2 * 18; i++)
		xr2[i] = xr[i];

	    /* reorder all 3-window bands */
	    
	    bandstart = bands->s[3];
	    bandsize = bands->s[4] - bandstart;
	    for(sfb = 3; sfb < 13; sfb++) {
		src = bandstart * 3;
		for(window = 0; window < 3; window++)
		    dst = bandstart * 3 + window;
		    for(i = 0; i < bandsize; i++) {
			xr2[dst] = xr[src++];
			dst += 3;
		    }
		bandstart = bands->s[sfb];
		bandsize = bands->s[sfb + 1] - bandstart;
	    }
	} else {

	    /* short block, reorder everything  */

	    bandstart = 0;
	    bandsize = bands->s[1];
	    for(sfb = 0; sfb < 13; sfb++) {
		src = bandstart * 3;
		for(window = 0; window < 3; window++) {
		    dst = bandstart * 3 + window;
		    for(i = 0; i < bandsize; i++) {
			xr2[dst] = xr[src++];
			dst += 3;
		    }
		}
		bandstart = bands->s[sfb + 1];
		bandsize = bands->s[sfb + 2] - bandstart;
	    }
	}
    } else
	/* long block, pass through */
	for(i = 0; i < 576; i++)
	    xr2[i] = xr[i];
}

/* make some alias reduction */
#ifndef DSP
void
Granule_antialias(Granule *gr, Granule_floatfreqs xr)
{
    int i, topband, subband, upper, lower;
    mpfloat u, l;

    if(gr->window_switching_flag && gr->block_type == BLOCKTYPE_3WIN)
	if(!gr->mixed_block_flag)
	    return;       /* dont antialias short blocks */
	else
	    topband = 2-1;  /* only antialias the long part of a mixed block */
    else
	topband = NUM_SUBBANDS - 1;

    /* for each subband-pair, do 8 alias-reduction butterflies */

    upper = 17;
    lower = 18;

    for(subband = 0; subband < topband; subband++) {
	for(i = 0; i < 8; i++) {
	    u = xr[upper];
	    l = xr[lower];
	    xr[upper] = u * Granule_alias_cs[i] -
		l * Granule_alias_ca[i];
	    ISCALE(xr[upper]);
	    xr[lower] = l * Granule_alias_cs[i] +
		u * Granule_alias_ca[i];
	    ISCALE(xr[lower]);
	    upper--;
	    lower++;
	}
	upper += 26;
	lower += 10;
    }
}
#endif

/* Every odd time sample of every odd subband is multiplied by -1 to 
   compensate for the frequency inversion of the polyphase synthesis
 */

void
Granule_freqinverse(Granule *gr, Granule_floatfreqs x)
{
    int sb, dct;
    x = &x[NUM_DCTBANDS];
    for(sb = 1; sb < NUM_SUBBANDS; sb += 2) {
	for(dct = 1; dct < NUM_DCTBANDS; dct += 2)
	    x[dct] = -x[dct];
	x = &x[2 * NUM_DCTBANDS];
    }
}


void
Granule_subband_synthesis(Granule *gr, int ch, Granule_floatfreqs s,
			  PCMSample *S)
{
    int i, j, t, k;
    mpfloat band[32];
    mpfloat *v, sum, *a, *b;

    if(ch)
	S = &S[1];

    /* We have 18 time-vectors of 32 subband magnitudes each. For every
       vector of 32 magnitudes, the subband synthesis generates 32
       PCM samples, so the result of 18 of these is 18*32=576 samples.
     */

    /* go through each time window */

    for(t = 0; t < 18; t++) {

	/* extract the subband strengths */

	v = &s[t];
	for(i = 0; i < 32; i++) {
	    band[i] = *v;
	    v = &v[18];
	}

	/* advance the buffer position */

	Granule_sbsynth_Vptr[ch] = (Granule_sbsynth_Vptr[ch] - 64) & 0x3ff;
	v = &Granule_sbsynth_V[ch][Granule_sbsynth_Vptr[ch]];

	fast_idct(band, v);
  
	/* 32*16=512 mac's */
	
	S = windowing(ch, S);
	
    }
}

/* 18 * (4096+1024) = 92160 MAC's per call, with 2 calls per frame and
   38 frames per second this is 7 million MAC's per second.

   18 * (384 * 2 + 1024) = 32256 MAC's per call using Lee's fast DCT! That
   is just 2.4 million MAC's per second!

	We need a buffer of 1024 floats per channel.

   */

void
Granule_subband_synthesis2(Granule *gr, Granule_floatfreqs s1,
			   Granule_floatfreqs s2,
			   PCMSample *S)
{
    int i, j, t, k, isum1, isum2;
    mpfloat band[64];
    mpfloat *v, *v2, sum, sum2, *a, *b;

    /* We have 18 time-vectors of 32 subband magnitudes each. For every
       vector of 32 magnitudes, the subband synthesis generates 32
       PCM samples, so the result of 18 of these is 18*32=576 samples.
     */

    /* go through each time window */

    for(t = 0; t < 18; t++) {

	/* extract the subband strengths */

	v = &s1[t];
	v2 = &s2[t];
	for(i = 0; i < 32; i++) {
	    band[i] = *v;
	    band[i+32] = *v2;
	    v = &v[18];
	    v2 = &v2[18];
	}

	/* advance the buffer position */

	Granule_sbsynth_Vptr[0] = (Granule_sbsynth_Vptr[0] - 64) & 0x3ff;
	v = &Granule_sbsynth_V[0][Granule_sbsynth_Vptr[0]];
	
	/* calculate 64 values for each channel and insert them into the 1024 wide buffer */

	fast_idct(band, v);
	fast_idct(&band[32], &v[1024]);
         
	/* 32*16*2=1024 mac's */
	
	/* windowing - calculate 32 samples. each sample is the sum of 16 terms */
	
	/*     15          */
	/* Sj = E W(j+32i) */
	/*    i=0          */
	
#ifdef USE_INLINE_ASM
__asm {
	push edi
	// grab the sample pointer
	mov edi, S

	xor ebx, ebx	// loop counter 
	ALIGN 4
window_loop:
	lea ecx, [Granule_sbsynth_D + ebx*4]	// ecx = &Granule_sbsynth_D[j]
	mov eax, ebx
	add eax, [Granule_sbsynth_Vptr]		// eax = j + Granule_sbsynth_Vptr[0]
	fldz	// sum1 = 0.0
	fldz	// sum2 = 0.0
	xor edx, edx	// filt_lp1 counter 

	ALIGN 4
filt_lp1:
	fld [ecx]	// get the window coefficient (D)
	fld st(0)	// duplicate it
	fmul dword ptr [Granule_sbsynth_V + eax*4]	// multiply by Granule_sbsynth_V[eax]
	faddp st(3), st
	fmul dword ptr [Granule_sbsynth_V + 4096 + eax*4]	// same for channel 2
	faddp st(1), st
	add eax, 96
	and eax, 0x3ff
	fld [128 + ecx]	// get the next D
	fld st(0)			// dup it
	fmul dword ptr [Granule_sbsynth_V + eax*4]
	faddp st(3), st
	fmul dword ptr [Granule_sbsynth_V + 4096 + eax*4]
	faddp st(1), st
	add eax, 32
	and eax, 0x3ff
	add ecx, 256
	inc edx
	cmp edx, 7
	jle filt_lp1

	// store accumulated sums as integers
	// scaling is not necessary, because we put the 32768 factor in the D coefficients
	fistp	isum2
	fistp	isum1

	// clipcheck and store sample 1
	mov edx, isum1
#ifdef OVERFLOW_CHECKING
	cmp edx, 32767
	jle no_isum1_of
	mov word ptr [edi], 0x7fff
	jmp done_isum1
no_isum1_of:
	cmp edx, -32768
	jge no_isum1_uf
	mov word ptr [edi], 0x8000
	jmp done_isum1
no_isum1_uf:
#endif
	mov [edi], dx
done_isum1:

	// clipcheck and store sample 2
	mov edx, isum2
#ifdef OVERFLOW_CHECKING
	cmp edx, 32767
	jle no_isum2_of
	mov word ptr [edi+2], 0x7fff
	jmp done_isum2
no_isum2_of:
	cmp edx, -32768
	jge no_isum2_uf
	mov word ptr [edi+2], 0x8000
	jmp done_isum2
no_isum2_uf:
#endif
	mov [edi+2], dx
done_isum2:
	add edi, 4

	// increase loop counter and loop

	inc ebx
	cmp ebx, 31
	jle window_loop

	// store the new sample counter
	mov S, edi
	pop edi
}

#else

	for (j = 0; j < 32; j++) {
	    
	    a = &Granule_sbsynth_D[j];
	    k = j + Granule_sbsynth_Vptr[0];
	    b = &Granule_sbsynth_V[0][0];
	    
	    sum = (mpfloat)0.0f;
	    sum2 = (mpfloat)0.0f;
	    for(i = 0; i < 8; i++) {
		sum += a[0] * b[k];
		sum2 += a[0] * b[k+1024];
		k = (k + 96) & 0x3ff;
		sum += a[32] * b[k];
		sum2 += a[32] * b[k+1024];
		k = (k + 32) & 0x3ff; 
		a = &a[64];
	    }
	   
#ifdef INT_MATH
	    i = sum.rint();
#else
	    /* convert to integer, and clip the output */
	    i = (int)sum;
#endif

#ifdef OVERFLOW_CHECKING
	    if(i >= 32768) {
		*S = 32767;	
	    } else if(i < -32768) {
		*S = -32768;
	    } else 
#endif
		*S = i;
	    
	    S++;

#ifdef INT_MATH
	    i = sum2.rint();
#else
	    /* convert to integer, and clip the output */
	    i = (int)sum2;
#endif

#ifdef OVERFLOW_CHECKING
	    if(i >= 32768) {
		*S = 32767;
	    } else if(i < -32768) {
		*S = -32768;
	    } else 
#endif
		*S = i;
	    
	    S++;

	}
#endif

    }
}



#ifdef DECVERBOSE
void
Frame_dump(Frame *f)
{
  printf("CRC %d, ", f->CRC_enable);
  printf("bitrate %d, ", f->bitrate_index);
  printf("sampfreq %d, ", f->sampling_frequency);
  printf("mode %d, ", f->mode);
  printf("mode_ext %d\n", f->mode_extension);
  printf("copyright %d, ", f->copyright);
  printf("originality %d, ", f->originality);
  printf("emphasis %d\n", f->emphasis);
  if(f->CRC_enable)
    printf("CRC 0x%4x\n", f->CRC);
  printf("main_data_begin %d, header_size %d, datasize %d\n",
	 f->main_data_begin,
	 f->header_size,
	 f->main_data_size);
 
}
#endif


/* Bitstream code */
/* ###TODO Its NOT optimal to have a hardcoded bufsize */

int
Bitstream_open(Bitstream *bs, char *fname)
{
#ifndef DSP
  if(fname) {
    /* open the file */
    
    if(!(bs->f = fopen(fname, "rb"))) {
      printf("Couldn't open file %s for reading.\n", fname);
      bs->eof_found = 1;
      return -1;
    }
  } else
    bs->f = 0;
#endif

  /* reset indexes to first bit in the file */

  bs->bit_index = 0;
#if 0
  bs->byte_index = 0;
#endif
  bs->offset = 0;
  bs->write_index = 0;
  bs->pos = 0;
  bs->eof_found = 0;

  /* fill it up */
#ifndef DSP
  if(bs->f)
    Bitstream_fillbuffer(bs);
#endif
  return 0;
}


/* close the bitstream */

void
Bitstream_close(Bitstream *bs)
{
#ifndef DSP
  if(bs->f) {
    fclose(bs->f);
    bs->f = 0;
  }
#endif
}

/* fill the buffer with as many new bytes as possible */
/* returns 0 if successful, -1 if it couldn't load any new bytes */

int
Bitstream_fillbuffer(Bitstream *bs)
{
  int to_load, loaded, i;

  /* Figure out how many bytes we can read. we have a circular buffer
     with a read (byte_index) and write (write_index) pointer.
     We can load new bytes from the write pointer up to but not including
     the read pointer. */
  
  to_load = ((bs->pos >> 3) - bs->write_index) % BITSTREAM_BUFSIZE;
  /* if the read and write pointers are equal, we assume we should read in
     the whole buffer */
  if(!to_load)
      to_load = BITSTREAM_BUFSIZE;
  
#ifndef DSP
  /* don't bother to fill if the buffer is more than half full */
  if(to_load < BITSTREAM_BUFSIZE/2)
      return 0;
#endif

#ifdef DSP
  loaded = MIN(20000 - bs->offset, MIN(to_load,
				       BITSTREAM_BUFSIZE - bs->write_index));
  for(i = 0; i < loaded; i++)
      bs->buffer[bs->write_index++] = inbuffer[bs->offset++];
  bs->write_index = bs->write_index % BITSTREAM_BUFSIZE;
  to_load -= loaded;

  if(to_load) {
      loaded = MIN(20000 - bs->offset, to_load);
      for(i = 0; i < loaded; i++)
	  bs->buffer[bs->write_index++] = inbuffer[bs->offset++];
      bs->write_index = bs->write_index % BITSTREAM_BUFSIZE;
  }
  
#else
  loaded = fread(&bs->buffer[bs->write_index], 1, 
		 MIN(to_load, BITSTREAM_BUFSIZE - bs->write_index),
		 bs->f);
  if(!loaded) {
      printf("End of file in Bitstream_fillbuffer.\n");
      return -1;
  }
  to_load -= loaded;
  bs->offset += loaded;
  bs->write_index = (bs->write_index + loaded) % BITSTREAM_BUFSIZE;
  
  if(to_load) {
    /* read the second chunk if any */
    loaded = fread(bs->buffer, 1, to_load, bs->f);
    if(loaded != to_load) {
      printf("End of file in Bitstream_fillbuffer.\n");
      return 0;
    }
    bs->offset += loaded;
    bs->write_index = (bs->write_index + loaded) % BITSTREAM_BUFSIZE;
  }
#endif

  /* duplicate the end to reduce the need for modulo calcs in viewbits */
  
  bs->buffer[BITSTREAM_BUFSIZE] = bs->buffer[0];
  bs->buffer[BITSTREAM_BUFSIZE + 1] = bs->buffer[1];
  bs->buffer[BITSTREAM_BUFSIZE + 2] = bs->buffer[2];
  bs->buffer[BITSTREAM_BUFSIZE + 3] = bs->buffer[3];

  return 0;
}

ibool
Bitstream_eof(Bitstream *bs)
{
  return bs->eof_found;
}
/*
static int Bitstream_getmask[9] = { 0, 0x1, 0x3, 0x7, 0xf,
				    0x1f, 0x3f, 0x7f, 0xff };

static int Bitstream_getmask2[9] = { 0, 0x1, 0x2, 0x4, 0x8,
				     0x10, 0x20, 0x40, 0x80 };
				     */

unsigned int Bitstream_msk[8] = {
    0xffffffff,0x7fffffff,0x3fffffff,0x1fffffff,
    0x0fffffff,0x07ffffff,0x03ffffff,0x01ffffff,
};

/* get numbits from the bitstream bs */
/* it is assumed that the buffer has enough fresh bits to satisfy the
   request
*/
unsigned int 
Bitstream_viewbits(Bitstream *bs, int n)
{
    int pos;
    unsigned int ret_value=0;
    if(!n)
	return 0;
    pos = (bs->pos >> 3) & (BITSTREAM_BUFSIZE - 1);
    ret_value = bs->buffer[pos] << 24 |
	bs->buffer[pos + 1] << 16 | 
	bs->buffer[pos + 2] << 8 |
	bs->buffer[pos + 3];
    ret_value &= Bitstream_msk[bs->pos & 7];
    ret_value >>= 32 - n - (bs->pos & 7);
    return ret_value;
}

void 
Bitstream_flushbits(Bitstream *bs, int n)
{
	bs->pos += n;
/*	bs->pos &= 8 * BITSTREAM_BUFSIZE - 1; */
}

unsigned int 
Bitstream_get(Bitstream *bs, int n)
{
    unsigned int ret_value;
    if(!n)
	return 0;
    ret_value = Bitstream_viewbits(bs, n);
    Bitstream_flushbits(bs, n);
    return ret_value;	
}	

unsigned int
Bitstream_get1(Bitstream *bs)
{
    return Bitstream_get(bs, 1);
}

void
Bitstream_putbyte(Bitstream *bs, unsigned int val)
{
    bs->buffer[bs->write_index] = val;
    bs->write_index = (bs->write_index + 1) % BITSTREAM_BUFSIZE;
}


void
Bitstream_rewindbits(Bitstream *bs, int n)
{
    bs->pos -= n;
/*
    bs->bit_index += n;
    bs->byte_index -= bs->bit_index / 8;
    bs->bit_index = bs->bit_index % 8;
    */
}

void
Bitstream_rewindbytes(Bitstream *bs, int n)
{
    bs->pos -= n*8;
/*
    bs->byte_index -= n;
    */
}

int
Bitstream_tell(Bitstream *bs)
{
    return bs->pos;
}

/* seek forward until we find a bytealigned bit sequence of 1111 1111 1111 */
/* this is the syncword in mpeg audio layer 3 */

ibool
Bitstream32_seek_sync(Bitstream *bs)
{
    int aligning;
    unsigned int val;
    
    aligning = bs->pos % 8;   /* sync words are byte aligned */
    
    if (aligning)
	Bitstream32_get(bs, 8 - aligning);
    
    /* preload val with 8 bits, then keep loading 4 bits at a time until we
       find the sync word */
    
    val = Bitstream32_get(bs, 8);
    while (((val & 0xfff) != 0xfff) && !Bitstream_eof(bs)) {
	val <<= 4;
	val |= Bitstream32_get(bs, 4);
    }
    
/*  printf("sync found, now at %d\n", bs->pos);
 */
    
    if (Bitstream_eof(bs))
	return(0);
    else
	return(1);
}

int
Bitstream32_open(Bitstream *bs, char *fname)
{
#ifndef DSP
  if(fname) {
    /* open the file */
    
    if(!(bs->f = fopen(fname, "rb"))) {
      printf("Couldn't open file %s for reading.\n", fname);
      bs->eof_found = 1;
      return -1;
    }
  } else
    bs->f = 0;
#endif

  /* reset indexes to first bit in the file */

  bs->bit_index = 0;
#if 0
  bs->byte_index = 0;
#endif
  bs->offset = 0;
  bs->write_index = 0;
  bs->pos = 0;
  bs->eof_found = 0;

  /* fill it up */
#ifndef DSP
  if(bs->f)
    Bitstream32_fillbuffer(bs);
#endif
  return 0;
}

/* fill the buffer with as many new bytes as possible */
/* returns 0 if successful, -1 if it couldn't load any new bytes */

int
Bitstream32_fillbuffer(Bitstream *bs)
{
  int to_load, loaded, i;

  /* Figure out how many bytes we can read. we have a circular buffer
     with a read (byte_index) and write (write_index) pointer.
     We can load new bytes from the write pointer up to but not including
     the read pointer. */
  
  to_load = ((bs->pos >> 5) - bs->write_index) & 0x1fff;
  /* if the read and write pointers are equal, we assume we should read in
     the whole buffer */
  if(!to_load)
      to_load = 0x2000;
  
#ifndef DSP
  /* don't bother to fill if the buffer is more than half full */
  if(to_load < 0x1000)
      return 0;
#endif

#ifdef DSP
  loaded = MIN(20000 - bs->offset, MIN(to_load,
				       BITSTREAM_BUFSIZE - bs->write_index));
  for(i = 0; i < loaded; i++)
      bs->buffer[bs->write_index++] = inbuffer[bs->offset++];
  bs->write_index = bs->write_index % BITSTREAM_BUFSIZE;
  to_load -= loaded;

  if(to_load) {
      loaded = MIN(20000 - bs->offset, to_load);
      for(i = 0; i < loaded; i++)
	  bs->buffer[bs->write_index++] = inbuffer[bs->offset++];
      bs->write_index = bs->write_index % BITSTREAM_BUFSIZE;
  }
  
#else
  for(i = 0; i < MIN(to_load, 0x2000 - bs->write_index); i++) {
      loaded = fread(&bs->buffer32[bs->write_index], 4, 1, bs->f);
      if(!loaded) {
	  printf("End of file in Bitstream_fillbuffer.\n");
	  return -1;
      }
      bs->buffer32[bs->write_index] = htonl(bs->buffer32[bs->write_index]);
      bs->write_index = (bs->write_index + loaded) & 0x1fff;
      to_load -= loaded;
      bs->offset += loaded;
  }
  
/* read the second chunk if any */  
  for(i = 0; i < to_load; i++) {
      loaded = fread(&bs->buffer32[bs->write_index], 4, 1, bs->f);
      if(!loaded) {
	  printf("End of file in Bitstream_fillbuffer.\n");
	  return 0;
      }
      bs->buffer32[bs->write_index] = htonl(bs->buffer32[bs->write_index]);
      bs->offset += loaded;
      bs->write_index = (bs->write_index + loaded) & 0x1fff;
  }
#endif

  return 0;
}

unsigned int Bitstream32_msk[32] = {
    0xffffffff,0x7fffffff,0x3fffffff,0x1fffffff,
    0x0fffffff,0x07ffffff,0x03ffffff,0x01ffffff,

    0x00ffffff,0x007fffff,0x003fffff,0x001fffff,
    0x000fffff,0x0007ffff,0x0003ffff,0x0001ffff,

    0x0000ffff,0x00007fff,0x00003fff,0x00001fff,
    0x00000fff,0x000007ff,0x000003ff,0x000001ff,

    0x000000ff,0x0000007f,0x0000003f,0x0000001f,
    0x0000000f,0x00000007,0x00000003,0x00000001
};

/* get numbits from the bitstream bs */
/* it is assumed that the buffer has enough fresh bits to satisfy the
   request
*/
unsigned int 
Bitstream32_viewbits(Bitstream *bs, int n)
{
    int pos;
    unsigned int ret_value=0;
    if(!n)
	return 0;
    pos = (bs->pos >> 5) & 0x1fff;  /* find index in buffer */
    
    ret_value = bs->buffer[pos] << 24 |
	bs->buffer[pos + 1] << 16 | 
	bs->buffer[pos + 2] << 8 |
	bs->buffer[pos + 3];
    ret_value &= Bitstream_msk[bs->pos & 7];
    ret_value >>= 32 - n - (bs->pos & 7);
    return ret_value;
}

unsigned int 
Bitstream32_get(Bitstream *bs, int n)
{
    unsigned int ret_value;
    if(!n)
	return 0;
    ret_value = Bitstream_viewbits(bs, n);
    Bitstream_flushbits(bs, n);
    return ret_value;	
}	

unsigned int
Bitstream32_get1(Bitstream *bs)
{
    return Bitstream_get(bs, 1);
}

