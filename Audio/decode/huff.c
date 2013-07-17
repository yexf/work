/* MPEG Audio Layer-3 decoder             */
/* Bjorn Wesen 1997                       */
/*                                        */
/* Huffman decoder                        */

#undef ROCKFORD

#include "mp3dec.h"
#include "huffman.h"

/* lots of tables */

/* code implementing the Huffman decoder for layer-3 */
/* decode the Huffman coded energies of a Granule */

#define BITS_USED ((((datapos - (bs->pos >> 3)) * 8) % BITSTREAM_BUFSIZE) - \
	dataword_len - (bs->pos & 7))

static char t_slen1[16] = {0,0,0,0,3,1,1,1,2,2,2,3,3,3,4,4};
static char t_slen2[16] = {0,1,2,3,0,1,2,3,1,2,3,1,2,3,2,3};
static int t_linbits[32] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,6,8,
			     10,13,4,5,6,7,8,9,11,13 };

static unsigned int dataword;
int dataword_len;
int datapos;

void
HuffmanTable_init()
{
}

void
Granule_decode_huffman(Granule *gr, Bitstream *bs, int part2_start,
		       Granule_intfreqs dec, Frame *f)
{
    int reg1, reg2, i, v, w,
	part2_end = part2_start + gr->part2_3_length;
    int h, used;

    /* The spectrum is partitioned in 3 regions, and every regions energies
       are coded using its own huffman table, in table_select[region].
       
       region0_count specifies the number of bands - 1 in region 0.
       For short blocks, each band is counted 3 times due to the 3 windows. 
       
       region1_count specifies 1 less than the number of bands in region 1.
       
       If its a BLOCKTYPE_3WIN and mixed_block_flag==0, the granule 
       contains 12*3=36 bands total. If mixed_block_flag==1, its 8+9*3=35
       and for other block types its 21 bands in total.
       
       */
    
    /* first find out the region boundaries - we have 3 regions, the first
       one ranging from 1 to reg1, then one from reg1 to reg2, and the
       last one from reg2 to 576 
     */
    
    if(gr->window_switching_flag &&
       gr->block_type == BLOCKTYPE_3WIN) {
	
	/* short block regions */
	
	reg1 = 36;  
	reg2 = 576;
	
    } else {

	/* long block regions */

	reg1 = Frame_bands[f->sampling_frequency].l[gr->region0_count + 1];
	reg2 = Frame_bands[f->sampling_frequency].l[gr->region0_count + 
						    gr->region1_count + 2];
    }

    /* There is a subdivision of the frequency band into two parts, the
       first is called the 'big values' part and extends from 1 to 
       big_values * 2. The second one extends to big_values*2+count1*4.
       We don't know the value of count1 explicetely.
       */

    /* fill up dataword, end at a bytealign in the buffer */
    {
	int bitpos = bs->pos & 7;
	datapos = (bs->pos >> 3) & (BITSTREAM_BUFSIZE - 1);
	/* the first read might not be bytealigned so shift it in place */
	/* we have duplicated the last 4 bytes in the stream so it doesnt
	   matter that datapos+2 etc dont pass through modulo */
	dataword = bs->buffer[datapos] << 24 |
	    bs->buffer[datapos + 1] << 16 | 
	    bs->buffer[datapos + 2] << 8 |
	    bs->buffer[datapos + 3];
	datapos = (datapos + 4) % BITSTREAM_BUFSIZE;
	dataword <<= bitpos;  /* strip the misaligned leading bits */
	dataword_len = 32 - bitpos;  /* number of valid bits in dataword */
	/* now dataword is loaded with at least 24 bits of data */
    }

    part2_end -= bs->pos;

    /* read the big values, they come in pairs */

    h = gr->table_select[0];  /* start with the first table */
    for(i = 0; i < gr->big_values * 2; i += 2) {
	unsigned int *h_tab;
	unsigned int lead;
	int l, len, x, y;

	if(i == reg1)
	    h = gr->table_select[1];
	if(i == reg2)
	    h = gr->table_select[2];
  
	h_tab = h_tables[h];

	/* dataword is always leftaligned, not rightaligned */

	lead = dataword >> (32 - NC_O); /* 19 - NC_O */
	h_tab += h_cue[h][lead];  
	
	len = (*h_tab >> 8) & 0x1f;
	
	/* check for an immediate hit, so we can decode short codes very fast
	 */
	if ((*h_tab >> (32 - len)) != dataword >> (32 - len)) {
	    int dir_flag;
	    int lag;
	    unsigned int chunk = (dataword & 0xffffe000) | 0x1ff; /* WHY */
	    lag = h_cue[h][lead + 1] - h_cue[h][lead];
	    dir_flag = 0;
	    while(lag > 1) {
		if (!dir_flag)
		    h_tab += lag >> 1;
		else
		    h_tab -= lag >> 1;
		if(*h_tab > chunk) {
		    if(!dir_flag)
			lag >>= 1;
		    else
			lag -= lag >> 1;
		    dir_flag =- 1;
		} else {
		    if (!dir_flag)
			lag -= lag >> 1;
		    else
			lag >>= 1;
		    dir_flag = 0;
		}
	    }
	    len = (*h_tab >> 8) & 0x1f;
	    if((*h_tab >> (32 - len)) != (chunk >> (32 - len))) {
		if (!dir_flag)
		    h_tab++;
		else
		    h_tab--;
		len = (*h_tab >> 8) & 0x1f;
	    }
	}
	
	dataword_len -= len;
	dataword <<= len;  /* flush away the used bits */
	
	/* fill up dataword */
	while(dataword_len <= 24) {
	    dataword |= bs->buffer[datapos++] << (24 - dataword_len);
	    dataword_len += 8;
	}
	datapos = datapos % BITSTREAM_BUFSIZE;
	
	/* dataword now contains at least 24 bits */
	
	x = (*h_tab >> 4) & 0xf;
	y = *h_tab & 0xf;

	l = t_linbits[h];

	/* linbits are used when values larger than 15 has to be encoded */

	/* l is 13 or less, and we know we have at least 24 bits of data
	   in the dataword, so use that directly */
	
	if(x == 15 && l > 0) {
	    x += dataword >> (32 - l);
	    dataword <<= l;
	    dataword_len -= l;
	    /* better refill since we might have used 13 bits */
	    while(dataword_len <= 24) {
		dataword |= bs->buffer[datapos++] << (24 - dataword_len);
		dataword_len += 8;
	    }
	    datapos = datapos % BITSTREAM_BUFSIZE;
	}

	/* read sign-bit for x */
	if(x) {
	    if(dataword & 0x80000000)
		x = - x;
	    dataword <<= 1;
	    dataword_len--;
	}

	if(y == 15 && l > 0) {
	    y += dataword >> (32 - l);
	    dataword <<= l;
	    dataword_len -= l;
	    /* better refill since we might have used 13 bits */
	    while(dataword_len <= 24) {
		dataword |= bs->buffer[datapos++] << (24 - dataword_len);
		dataword_len += 8;
	    }
	    datapos = datapos % BITSTREAM_BUFSIZE;
	}

	/* read sign-bit for y */
	if(y) {
	    if(dataword & 0x80000000)
		y = - y;
	    dataword <<= 1;
	    dataword_len--;
	}

	*dec++ = x;
	*dec++ = y;

	/* there are enough bits in dataword for the next run */
	
    }
    /* Now read the count1 values, they come in quadruples, and we
       don't know explicetely how many there are but we are supposed to
       keep decoding until all huffman bits have been decoded or until
       all frequency lines have been assigned (i == 576). 
    */

    h = gr->count1table_select + 32;  /* choose count1 table */
    while(BITS_USED < part2_end && i < 576) {
	(void)HuffmanTable_decode(h, bs, dec + 2, dec + 3, dec, dec + 1);
	dec += 4;
	i += 4;
    }

    used = BITS_USED;

    bs->pos += used;

    /* rewind if we got a bit too far, and discard if we got too short  */

    if(used > part2_end) {
	i -= 4;
	Bitstream_rewindbits(bs, used - part2_end);
    } else if(used < part2_end)
	Bitstream_flushbits(bs, part2_end - used);
    
    /* the rest of the frequency lines are zero */

    for(; i < 576; i++)
	*dec++ = 0;

}



int
HuffmanTable_decode(int tbl, 
		    Bitstream *bs,
		    int *x, int *y, int *v, int *w)
{
    unsigned int *h_tab;
    unsigned int lead;
    int len;
  
    h_tab = h_tables[tbl];

    /* dataword is always leftaligned, not rightaligned */

    lead = dataword >> (32 - NC_O); /* 19 - NC_O */
    h_tab += h_cue[tbl][lead];  
    
    len = (*h_tab >> 8) & 0x1f;
    
    /* check for an immediate hit, so we can decode short codes very fast
     */
    if ((*h_tab >> (32 - len)) != dataword >> (32 - len)) {
	int dir_flag;
	int lag;
	unsigned int chunk = (dataword & 0xffffe000) | 0x1ff; /* WHY */
	lag = h_cue[tbl][lead + 1] - h_cue[tbl][lead];
	dir_flag = 0;
	while(lag > 1) {
	    if (!dir_flag)
		h_tab += lag >> 1;
	    else
		h_tab -= lag >> 1;
	    if(*h_tab > chunk) {
		if(!dir_flag)
		    lag >>= 1;
		else
		    lag -= lag >> 1;
		dir_flag =- 1;
	    } else {
		if (!dir_flag)
		    lag -= lag >> 1;
		else
		    lag >>= 1;
		dir_flag = 0;
	    }
	}
	len = (*h_tab >> 8) & 0x1f;
	if((*h_tab >> (32 - len)) != (chunk >> (32 - len))) {
	    if (!dir_flag)
		h_tab++;
	    else
		h_tab--;
	    len = (*h_tab >> 8) & 0x1f;
	}
    }

    dataword_len -= len;
    dataword <<= len;  /* flush away the used bits */

    /* fill up dataword */
    while(dataword_len <= 24) {
	dataword |= bs->buffer[datapos++] << (24 - dataword_len);
	dataword_len += 8;
    }
    datapos = datapos % BITSTREAM_BUFSIZE;

    /* dataword now contains at least 24 bits */

    *x = (*h_tab >> 4) & 0xf;
    *y = *h_tab & 0xf;
  	
    *v = (*x >> 3) & 1;
    *w = (*x >> 2) & 1;
    *y = *x & 1;
    *x = (*x >> 1) & 1;
    
    /* read and process the sign bits */
    
    if(*v) {
	if(dataword & 0x80000000)
	    *v = - *v;
	dataword <<= 1;
	dataword_len--;
    }
    if(*w) {
	if(dataword & 0x80000000)
	    *w = - *w;
	dataword <<= 1;
	dataword_len--;
    }
    if(*x) {
	if(dataword & 0x80000000)
	    *x = - *x;
	dataword <<= 1;
	dataword_len--;
    }
    if(*y) {
	if(dataword & 0x80000000)
	    *y = - *y;
	dataword <<= 1;
	dataword_len--;
    }
    
    /* we have used up to 4 bits, so there are at least 20 bits
       left in dataword, which is enough for the next run */
    
    return datapos;

}


