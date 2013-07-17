/* MPEG AUDIO LAYER 3 DECODER */
/* Bjorn Wesen 1997           */

#ifndef MP3DEC_H
#define MP3DEC_H

/* enable the assembler version of the IDCT in the subband synth */
#ifdef WIN32
#define USE_INLINE_ASM
#endif

#ifndef INT_MATH
/* enable the fast implementation of the IMDCT */
#define LEE_IMDCT
#endif

#ifndef WIN32
#undef MAKE_DATA
#define USE_DATA
#else
#undef USE_DATA
#undef MAKE_DATA
#endif

#ifdef DSP
/* in wait of a real math runtime... */
float tan(float f);
float sin(float f);
float cos(float f);
float sqrt(float f);
#else
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* enable clipping in the sample output */
#define OVERFLOW_CHECKING
#endif

#ifndef PI
#define PI 3.1415926535897932385
#endif

#define MAXCH 2           /* number of channels - 2 is stereo */
#define NUM_SUBBANDS 32   /* number of frequency subbands */
#define NUM_DCTBANDS 18   /* number of DCT lines */

#ifdef DSP
#define BITSTREAM_BUFSIZE 1024
#else
#define BITSTREAM_BUFSIZE 131072
#endif

/* ISO-MPEG layers */
#define LAYER1 3
#define LAYER2 2
#define LAYER3 1
#define LAYERRS 0

/* for the sampling_frequency field in a Frame */
#define FREQ_44 0    /* 44.1kHz sampling rate */
#define FREQ_48 1    /* 48kHz sampling rate */
#define FREQ_32 2    /* 32kHz sampling rate */
#define FREQ_RS 3    /* reserved */

/* stereo-mode of a Frame */
#define MODE_STEREO 0
#define MODE_JOINT_STEREO 1
#define MODE_DUAL_CHANNEL 2
#define MODE_SINGLE_CHANNEL 3

/* mode-extensions of joint_stereo in layer 3 */
/* maybe recode these to be two on/off fields instead! */
#define EXT_INT_OFF_MS_OFF 0
#define EXT_INT_ON_MS_OFF 1
#define EXT_INT_OFF_MS_ON 2
#define EXT_INT_ON_MS_ON 3

/* types of de-emphasis */
#define EMPH_NONE 0      /* no de-emphasis */
#define EMPH_5015 1      /* 50/15 microseconds */
#define EMPH_RS 2        /* reserved */
#define EMPH_CCITT 3     /* CCITT J.17 */


typedef int ibool;
#ifndef INT_MATH
#ifdef MAKE_DATA
typedef double mpfloat;   /* use extra precision while doing tables */
#else
typedef float mpfloat;   /* used for audio data calculations */
#endif
#define ISCALE(x)
#else

#define INTFACT 32767
#define ISCALE(x) (x).scale()

class mpfloat {
private:
    mpfloat(int i) { _val = i; }
public:
    mpfloat() { _val = 0; }
//    mpfloat(float f) { _val = (int)(f*INTFACT); } 
    mpfloat(double d) { _val = (int)(d*INTFACT); }

    void scale(int i = 16) {
//	printf("Scaling from 0x%8x.\n", _val);
	_val >>= i;
    }

    int operator!=(mpfloat a) {
	return a._val != _val;
    }

    int operator==(mpfloat a) {
	return a._val == _val;
    }

    mpfloat& operator=(mpfloat a) { 
	_val = a._val;
	return *this;
    }

    mpfloat& operator=(float a) { 
	_val = (int)(a*INTFACT);
	return *this;
    }

    mpfloat& operator=(double a) { 
	_val = (int)(a*INTFACT);
	return *this;
    }

    mpfloat& operator+=(mpfloat a) { 
	_val += a._val;
	return *this;
    }

    mpfloat& operator-=(mpfloat a) { 
	_val -= a._val;
	return *this;
    }

    mpfloat& operator*=(mpfloat a) { 
	_val *= a._val;
	return *this;
    }

    mpfloat& operator/=(mpfloat a) { 
	_val /= a._val;
	return *this;
    }

    friend mpfloat operator+(mpfloat, mpfloat);
    friend mpfloat operator-(mpfloat, mpfloat);
    friend mpfloat operator-(mpfloat);
    friend mpfloat operator*(mpfloat, mpfloat);
    friend mpfloat operator/(mpfloat, mpfloat);

    operator int () const { return _val / INTFACT; }
    operator float () const { return (float)_val / INTFACT; }
//    operator double () const { return (double)_val / INTFACT; }

    int rint() const { return _val; }

private:
    int _val;

};

#endif

typedef short PCMSample;
#define MIN(a,b) (a < b ? a : b)

/* Bitstream handles the streaming of a file into bits */

struct Bitstream {
#ifndef DSP
    FILE *f;
#endif
    int offset;      /* offset into file f - next byte to be read */
    ibool eof_found;   /* true if the file has hit eof */
    
    int write_index; /* load pointer in buffer */
    
#ifdef DSP
    unsigned char *buffer;  /* circular bytebuffer */
#else
    unsigned char buffer[BITSTREAM_BUFSIZE + 4];  /* circular bytebuffer */
    unsigned long *buffer32;  /* inbuffer, not huffbuff */
#endif
    int bit_index;  /* next bit to read */
#if 0
    int byte_index; /* byte to read - mapped into buffer by %BITSTREAM_BUFSIZE */
#endif
    int pos;        /* bit position in the stream */

};

typedef struct Bitstream Bitstream;


/* gets numbits bits from the bitstream bs */
#ifdef WIN32
unsigned int __inline Bitstream_get(Bitstream *bs, int numbits);
unsigned int __inline Bitstream_get1(Bitstream *bs);
#else
inline unsigned int Bitstream_get(Bitstream *bs, int numbits);
inline unsigned int Bitstream_get1(Bitstream *bs);
#endif

unsigned int Bitstream_viewbits(Bitstream *bs, int n);
void Bitstream_flushbits(Bitstream *bs, int n);

/* write a byte to the stream */
void Bitstream_putbyte(Bitstream *bs, unsigned int val);

/* rewind the read pointer n bits */
void Bitstream_rewindbits(Bitstream *bs, int n);

/* rewind the read pointer n bytes */
void Bitstream_rewindbytes(Bitstream *bs, int n);

/* returns true if the bitstream is at EOF */
ibool Bitstream_eof(Bitstream *bs);


/* seek synchronization word - return true if found */
ibool Bitstream_seek_sync(Bitstream *bs);
#if 0
typedef unsigned char Hufnode[2];

struct HuffmanTable {
    int number;                 /* number of the table, 0-33 in mpeg layer 3 */
    int xlen, ylen;             /* size */
    Hufnode *code;              /* decoder data tree */
    int codelen;                /* length of the decoder tree */
    int linbits;                /* number of linbits */
};

typedef struct HuffmanTable HuffmanTable;
extern HuffmanTable Huffman_h[34];
#endif

void HuffmanTable_boot(Bitstream *bs);
int HuffmanTable_bitsused(Bitstream *bs);
int HuffmanTable_decode(int htable, Bitstream *bs,
			int *x, int *y, int *v, int *w);


/* used in Granule.block_type */
#define BLOCKTYPE_NORMAL 0
#define BLOCKTYPE_START 1
#define BLOCKTYPE_3WIN 2
#define BLOCKTYPE_STOP 3

/* structure describing side information for one granule */

struct Granule {
  int part2_3_length;     /* nbr of bits used for sfactors and Huffman data */
  int big_values;         /* some weird value for huffman energies */
  int global_gain;        /* quantizer step size information for the granule */
  int scalefac_compress;  /* selects bitsize for transmission of sfactors */
  ibool window_switching_flag; /* signals that a special block type is used */

  /* these are only used if window_switching_flag is true */
  int block_type;         /* window type for this granule */
  ibool mixed_block_flag;  /* true if lower frequencies are special */
  int table_select[3];    /* selects Huffman tables for each region */
  int subblock_gain[3];   /* gain offsets for each subblock */
  int region0_count;      /* nbr of scalefactor bands in region 0 minus 1 */
  int region1_count;      /* nbr of scalefactor bands in region 1 minus 1 */
  ibool preflag;           /* true for high frequency amplification */
  ibool scalefac_scale;    /* scalefactor multiplier, false = 0.5, true = 1 */
  ibool count1table_select; /* selects huffman tables for some regions */

  int scalefac_l[23];     /* actual scalefactors for this granule */
  int scalefac_s[13][3];  /* scalefactors used in short windows */

    
};

typedef struct Granule Granule;

typedef int Granule_intfreqs[NUM_SUBBANDS * NUM_DCTBANDS + 10];
typedef mpfloat Granule_floatfreqs[NUM_SUBBANDS * NUM_DCTBANDS + 10];


/* This struct is the main object used when decoding a frame of audio. 
   A frame produces, in layer 3, 1152 samples per channel.
   */
typedef PCMSample SampleBlock[2][576][2];         /* this frames decoded PCM samples */

struct Frame {
    
    /* general header information for this frame */
    
    ibool CRC_enable;             /* true if CRC is enabled */
    int bitrate_index;           /* indicates the bitrate */
    int sampling_frequency;      /* indicates the sampling frequency */
    ibool padding_bit;            /* true if there is a padding slot */
    int mode;                    /* indicates the stereomode of this frame */
    int mode_extension;          /* indicates intensity and ms stereo on/off */
    ibool copyright;              /* true if the bitstream is copyrighted */
    ibool originality;            /* true if this bitstream is an original */
    int emphasis;                /* type of de-emphasis */
    
    int CRC;                     /* 16-bit CRC if error protection is on */
    
    int channels;                 /* number of channels, 2 for stereo */
    
    int main_data_begin;         /* bit offset to where the main data lives */
    int header_size;             /* size of header in bytes */
    int main_data_size;          /* number of bytes of main_data */
    
    ibool scfsi[MAXCH][4];        /* scalefactor selection information */
    
    Granule gr[2][MAXCH];        /* two granules per channel */
    
    PCMSample *samples;
    
#ifdef WIN32
    float *spectrum;
#endif

};

typedef struct Frame Frame;


#if defined(__cplusplus)
extern "C"
{
#endif
/* opens a binary file on disk for reading bits */
int Bitstream_open(Bitstream *bs, char *filename);

/* closes the file */
void Bitstream_close(Bitstream *bs);

/* fills the bitstreams internal buffer */
int Bitstream_fillbuffer(Bitstream *bs);
/* returns position of the read pointer, in bits */
int Bitstream_tell(Bitstream *bs);

void HuffmanTable_init();

void Frame_init();
void Frame_load(Frame *, Bitstream *, PCMSample *); 
void Granule_init();
extern int frameNum;

/* Function declarations */

#if defined(__cplusplus)
}
#endif 

/* load and decode a frame from the given bitstream */


#ifdef DECVERBOSE
void Frame_dump(Frame *);  /* dump verbose info about a frame */
#endif

/* debug code */
void dump_floats(mpfloat *f, int num);
void dump_ints(int *f, int num);


/* Granule operations - these constitute most of the decoding process */

void Granule_decode_info(Granule *gr, Bitstream *bs);
void Granule_decode_scalefactors(Granule *gr, Bitstream *bs,
				 int channel, int grnum, Frame *f);
void Granule_decode_huffman(Granule *gr, Bitstream *bs, 
			    int part2_start, Granule_intfreqs is, Frame *f);

void Granule_requantize(Granule *gr, Granule_intfreqs is,
			Granule_floatfreqs xr, Frame *f);

void Granule_process_stereo(Granule *grch0, Granule *grch1,
			    Granule_floatfreqs xr[2], 
			    Granule_floatfreqs lr[2], Frame *f);

void Granule_reorder(Granule *gr, Granule_floatfreqs xr,
		     Granule_floatfreqs xr2, Frame *f);

void Granule_antialias(Granule *gr, Granule_floatfreqs xr);

void Granule_imdct(Granule *gr, int ch, Granule_floatfreqs X);

void Granule_freqinverse(Granule *gr, Granule_floatfreqs x);

void Granule_subband_synthesis(Granule *gr, int ch, Granule_floatfreqs s, 
			       PCMSample *S);

void Granule_subband_synthesis2(Granule *gr, Granule_floatfreqs s1,
				Granule_floatfreqs s2,
				PCMSample *S);



void fast_idct_init();
void fast_idct(mpfloat *in, mpfloat *out);
PCMSample *windowing(int ch, PCMSample *S);
void windowing_init();
extern mpfloat *Granule_sbsynth_V[2];

struct Frame_band {
    int l[23];
    int s[14];
};

extern struct Frame_band Frame_bands[3];

#ifdef ROCKFORD
void prof_start();
void prof_end();
#define PROFSTART prof_start()
#define PROFSTOP prof_stop()
#else
#define PROFSTART
#define PROFSTOP
#endif

#ifdef MAKE_DATA
void
make_data_file(const char *fname, const char *datname, mpfloat *nbrs, int len);
void
make_data_file_2d(const char *fname, 
		  const char *datname, mpfloat *nbrs, int len1, int len2);
#endif


#endif

