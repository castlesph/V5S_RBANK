#define NUMERIC		  0x01
#define TRACKX		  0x02
#define ALPHANUM	  0x03
#define ALPHANUMS	  0x04
#define BITS		    0x05

#define LLVAR	      0x01
#define LLLVAR		  0x02
#define MMDDhhmmss	0x03
#define MMDD		    0x04
#define YYMM		    0x05
#define hhmmss		  0x06

struct BITD {
	unsigned char bitn;				/* bit number */
	unsigned char format;			/* bit format */
	unsigned char attrib;			/* attribute */
	unsigned char max;				/* max width */
};

//static unsigned char bit_set_bits[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

static unsigned char bit_set_bits[] = {128, 64, 32, 16, 8, 4, 2, 1 };

unsigned char bittest(unsigned char *bitmap, unsigned char pbitno);
void bitset(unsigned char *bmap, unsigned char bitnum);
void bin_to_bcd(unsigned char *src, unsigned char *des, unsigned int len);
unsigned long bcd_to_long(unsigned char *src, unsigned int len);
void vdPrintParseISO(unsigned char *szTitle, unsigned char *pucTitle, unsigned char *buf);
void vdLogParseISO(unsigned char * byTransType, unsigned char *pucTitle,unsigned char *buf);
int insert_field( char *src,  char bitnum,  char *buf, char *dst/*,  int len*/);
int pack(char *dst);
long bcd_long(char *bcd, int len);
void long_bcd( char *buf,  long val, int len);
char *Unpack( char *buf);

