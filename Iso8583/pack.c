#if 0
#include "stdafx.h"
#endif

#include <stdio.h>
#include <string.h>
//#include <shared.h>
//#include <structs.h>

#include <stdlib.h>
#include <ctosapi.h>
#include "pack.h"

#include "..\Debug\Debug.h"
#include "..\Print\Print.h"

struct BITD bittab[] = {
	{ 2  ,	LLVAR     ,	NUMERIC  , 20 },
	{ 3  ,	0         ,	NUMERIC  , 6  },
	{ 4  ,	0         ,	NUMERIC  , 12 },
	{ 5  ,	0         ,	NUMERIC  , 12 },
	{ 7  ,	MMDDhhmmss,	NUMERIC  , 10 },
	{ 11 ,	0         ,	NUMERIC  , 6  },
	{ 12 ,	hhmmss    ,	NUMERIC  , 6  },
	{ 13 ,	MMDD      ,	NUMERIC  , 4  },
	{ 14 ,	YYMM      ,	NUMERIC  , 4  },
	{ 15 ,	MMDD      ,	NUMERIC  , 4  },
	{ 22 ,	0         ,	NUMERIC  , 3  },
	{ 23 ,	0         ,	NUMERIC  , 3  },
	{ 24 ,	0         ,	NUMERIC  , 3  },
	{ 25 ,	0         ,	NUMERIC  , 2  },
	{ 26 ,	0		  , NUMERIC  , 2  },		
	{ 35 ,	LLVAR     , TRACKX   , 37 },
	{ 37 ,	0         ,	ALPHANUM , 12 },
	{ 38 ,	0         ,	ALPHANUM , 6  },
	{ 39 ,	0         ,	ALPHANUM , 2  },
	{ 41 ,	0         ,	ALPHANUMS, 8  },
	{ 42 ,	0         ,	ALPHANUMS, 15 },
	{ 43 ,	0         ,	ALPHANUMS, 40 },
	{ 44 ,	LLVAR     ,	ALPHANUMS, 0  },
	{ 45 ,	LLVAR     , ALPHANUMS, 76 },
	{ 48 ,	LLLVAR    ,	ALPHANUMS, 0  },
	{ 52 ,	0         ,	BITS     , 64 },
	{ 54 ,	LLLVAR    ,	ALPHANUM , 0  },
	{ 55 ,	LLLVAR    ,	ALPHANUM , 0  },
	{ 56 ,	LLLVAR    ,	ALPHANUM , 0  },
	{ 57  ,	0         ,	NUMERIC  , 12 },
	{ 58  ,	0         ,	NUMERIC  , 12 },
	{ 59  ,	0         ,	NUMERIC  , 12 },
	{ 60 ,	LLLVAR    ,	ALPHANUMS, 0  },
	{ 61 ,	LLLVAR    ,	ALPHANUMS, 0  },
	{ 62 ,	LLLVAR    ,	ALPHANUMS, 0  },
	{ 63 ,	LLLVAR    ,	ALPHANUMS, 0  },
	{ 64 ,	0         ,	BITS     , 64 },
	{ 255,	0         ,	0        , 0, }
};

static unsigned char *bptr;
static unsigned char *bitmap;
unsigned char *bmap;
unsigned char *bitp[65];
static  int bptrlen=0;

#define NUMERIC_			0x01
#define TRACKX		  	0x02
#define ALPHANUM_	  	0x03
#define ALPHANUMS	  	0x04
#define BITS		    	  	0x05

#define LLVAR	      		0x01
#define LLLVAR		  	0x02
#define MMDDhhmmss	0x03
#define MMDD		    	0x04
#define YYMM		    	0x05
#define hhmmss		  	0x06

void init_tpdu(unsigned char *buf, unsigned int nii)
{
	unsigned char temp[20];
	memset(temp,0x00,sizeof(temp));
	sprintf((char *)temp,"%04d",nii);

  	buf[0] = 0x60;
    bin_to_bcd(temp, &buf[1], 4);
    buf[3] = 0x30;
    buf[4] = 0x00;
}

void init_pack(unsigned int msg_type, unsigned char *buf)
{
	unsigned char temp[20];
	memset(temp,0x00,sizeof(temp));

	sprintf((char *)temp,"%04d",msg_type);
	bin_to_bcd(temp, buf, 4);
	//printf("%02x %02x",buf[0],buf[1]);

	bitmap = buf+2;
	memset(bitmap, 0x00, 8);
	bptr = buf+10;
}

void bin_to_bcd(unsigned char *src, unsigned char *des, unsigned int len)
{
	unsigned int in_index;
	unsigned char temp[50+1];

	memset(temp,0x00,sizeof(temp));

	for(in_index=0; in_index<len; in_index++)
	{
		temp[in_index]=(*src << 4);// + ((*src)+1 & 0x0F);
		src++;
		temp[in_index]=temp[in_index] + (*src & 0x0f);
		src++;
#ifdef TEST_DEBUG
		printf("%02x ",temp[in_index]);
#endif
	}
	memcpy(des,temp,len);
}

/*convert bcd to long*/
unsigned long bcd_to_long(unsigned char *src, unsigned int len)
{
	unsigned int in_index;
	unsigned char temp[50+1];

	memset(temp,0x00,sizeof(temp));

	for(in_index=0; in_index<len; in_index++)
	{
		temp[in_index]=((*src >> 4) & 0x0f) + 0x30;
#ifdef TEST_DEBUG
		printf("%c ",temp[in_index]);
#endif
		in_index++;
		temp[in_index]= (*src & 0x0f) + 0x30;
#ifdef TEST_DEBUG
		printf("%c \n",temp[in_index]);
#endif
		src++;
	}
	//memcpy(des,temp,len);
	return atol((const char *)temp);
}
#if 0

void pack_to_bcd(unsigned char bitn, void *buf, unsigned int len)
{
	unsigned char temp[20];
	
	memset(temp,0,sizeof(temp));
	bin_to_bcd(buf, temp, len);
	pack(bitn, temp, len);	
}

unsigned char *pack(unsigned char bitn, void *buf, unsigned int len)
{
  unsigned char i;
  unsigned char temp[10+1];

  	/*convert len to string*/
	memset(temp,0x00,sizeof(temp));
	sprintf((char *)temp,"%04d",len);

  for(i=0; bittab[i].bitn != bitn && bittab[i].bitn != 255; i++);

  if( bittab[i].bitn == 255 )
     return(NULL);

  bitset(bitmap, bitn);

	switch(bittab[i].attrib)
	{
  	case NUMERIC :
		  if( bittab[i].format == LLLVAR )
		  {
		  	bin_to_bcd(temp, bptr, 4);
		  	bptr += 2;
		  }

		  else if( bittab[i].format == LLVAR )
		  {
		  	bin_to_bcd(temp, bptr, 2);
		  	++bptr;
		  }

		  memcpy(bptr, buf, (len+1)/2);
		  bptr += (len+1)/2;

		  if( bitn == 2 && (len % 2))
		    *(bptr-1) |= 0x0f;

		  break;

	  case TRACKX :
	    bin_to_bcd(temp, bptr, 2);
  /*		*bptr = ((len/10) << 4) | len % 10;*/
	  	++bptr;
		  memcpy(bptr, buf, (len+1)/2);
		  bptr += (len+1)/2;

#if 0						/* Fuck hypercom */
  		if ((*(bptr-1)&0x0f)==0x0f) *(bptr-1)&=0xf0;
#endif

		break;

	  case ALPHANUM :
	  case ALPHANUMS :
	  	if (bittab[i].format == LLLVAR)
	  	{
	  		bin_to_bcd(temp, bptr, 4);
	  		bptr += 2;
	  	}
	  	else if (bittab[i].format == LLVAR)
	  	{
	  		bin_to_bcd(temp, bptr, 2);
	  		++bptr;
	  	}
	  	memcpy(bptr, buf, len);
	  	bptr += len;
	  	break;

	  case BITS :
	  	memcpy(bptr, buf, 8);
	  	bptr += 8;
	  	break;
  }
  return(bptr);
}
#endif

#if 1
unsigned char *unpack(unsigned char *buf)
{
	unsigned char i, j;
	unsigned int len;

	bitmap = buf+2;
	bptr = buf+10;

  for (i=1; i<65; i++)
  {

  	if (bittest(bitmap, i))
  	{
	  	for (j=0; bittab[j].bitn != 255; j++)
	  	  if (bittab[j].bitn == i)
	  	    break;

		  if (bittab[j].bitn == 255)
		  {
		    return(NULL);
		  }

	    bitp[i] = bptr;
	  	switch(bittab[j].attrib)
		  {
				case NUMERIC :
			  	if (bittab[j].format == LLLVAR)
				{
				    len = (unsigned int)bcd_to_long(bptr, 4);
  					bptr += 2;
	  			}
		  		else if (bittab[j].format == LLVAR)
			  	{
				  	len = (unsigned int)bcd_to_long(bptr, 2);
					  ++bptr;
  				}
	  			else
					len = bittab[j].max;
#if 0
				if(i == 3)
				{
					char temp3[40+1];

					memset(temp3,0x00,sizeof(temp3));
					bcd_to_bin((unsigned char *)bitp[3],(unsigned char *)temp3,len);
					printf("prcode=%s\n",temp3);
				}
				if(i == 4)
				{
					char temp3[40+1];
					memset(temp3,0x00,sizeof(temp3));
					bcd_to_bin((unsigned char *)bitp[4],(unsigned char *)temp3,len);
					printf("amount=%s\n",temp3);
				}

				if(i == 11)
				{
					char temp3[40+1];
					memset(temp3,0x00,sizeof(temp3));
					bcd_to_bin((unsigned char *)bitp[11],(unsigned char *)temp3,len);
					printf("stan=%s\n",temp3);
				}
#endif
		  		bptr += (len+1)/2;
			  	break;

			  case TRACKX :
				  len = (unsigned int)bcd_to_long(bptr, 2);
				  ++bptr;
				  bptr += (len+1)/2;
				  break;

			  case ALPHANUM :
			  case ALPHANUMS :
				if (bittab[j].format == LLLVAR)
  				{
	  				len = (unsigned int)bcd_to_long(bptr, 4);
					printf("len=%d\n",len);
		  			bptr += 2;
			  	}
				else if (bittab[j].format == LLVAR)
				{
					  len = (unsigned int)bcd_to_long(bptr, 2);
					  ++bptr;
				}
				else
					len = bittab[j].max;
#if 0
				  if(i == 41)
					  printf("##########41=%08s\n\n",bitp[41]);
				  if(i == 42)
					  printf("##########42=%015s\n",bitp[42]);
				  if(i == 61)
				  {
					  printf("##########61=%s\n",&bitp[61][2]);
				  }
#endif
				  bptr += len;
				  break;

			  case BITS :
				  bptr += 8;
				  break;
		  }
	  }
	//else
	//	bitp[i] = NULL;
  }
  return(bptr);
}
#endif

#if 0
void vdPrintParseISO(unsigned char *szTitle, unsigned char *pucTitle, unsigned char *buf)
{
	unsigned char i, j;
	unsigned int len;
    unsigned char szDE[12+1];
	unsigned char szDEData[256+1];
	int inDumpLen=0;
	int inAddLen=0;

	char szStr[44 + 4];
	
	bitmap = buf+2;
	bptr = buf+10;

	sprintf(szStr,"[%s: %s] \n", szTitle, pucTitle);
	vdMyEZLib_Printf2("%s", szStr);
	
    for (i=1; i<65; i++)
    {

        if (bittest(bitmap, i))
        {
            for (j=0; bittab[j].bitn != 255; j++)
                if (bittab[j].bitn == i)
                    break;
            
            if (bittab[j].bitn == 255)
            {
                return(NULL);
            }
            
            bitp[i] = bptr;
			memset(szDE, 0, sizeof(szDE));
			sprintf(szDE, "DE: %03d", i);
			memset(szDEData, 0x00, sizeof(szDEData));
			inAddLen=0;
            switch(bittab[j].attrib)
            {
                case NUMERIC :
                    if (bittab[j].format == LLLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 4);
                        bptr += 2;
						inAddLen=2;
                    }
                    else if (bittab[j].format == LLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 2);
                        ++bptr;
						inAddLen=1;
                    }
                    else
                        len = bittab[j].max;
                     bptr += (len+1)/2;
					vdMyEZLib_HexDump(szDE, bitp[i], ((len+1)/2)+inAddLen);
                break;
                
                case TRACKX :
                    len = (unsigned int)bcd_to_long(bptr, 2);
                    ++bptr;
                    bptr += (len+1)/2;
					inAddLen=1;
					vdMyEZLib_HexDump(szDE, bitp[i], ((len+1)/2)+inAddLen);	
                break;
                
                case ALPHANUM:
                case ALPHANUMS:
                    if (bittab[j].format == LLLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 4);
                        printf("len=%d\n",len);
                        bptr += 2;
						inAddLen=2;
                    }
                    else if (bittab[j].format == LLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 2);
                        ++bptr;
						inAddLen=1;
                    }
                    else
                        len = bittab[j].max;
                    bptr += len;	
                    vdMyEZLib_HexDump(szDE, bitp[i], len+inAddLen);
                break;
                
                case BITS :
                    bptr += 8;
					vdMyEZLib_HexDump(szDE, bitp[i], 8);
                break;
            }
        }
    }
}
#else
void vdPrintParseISO(unsigned char *szTitle, unsigned char *pucTitle, unsigned char *buf)
{
	unsigned char i, j;
	unsigned int len;
    unsigned char szDE[12+1];
	unsigned char szDEData[256+1];
	int inDumpLen=0;
	int inAddLen=0;
	char szStr[44 + 4];
	
	bitmap = buf+2;
	bptr = buf+10;

	sprintf(szStr,"[%s: %s] \n", szTitle, pucTitle);
		
	vdMyEZLib_Printf2("%s", szStr);
		
    for (i=1; i<65; i++)
    {

        if (bittest(bitmap, i))
        {
            for (j=0; bittab[j].bitn != 255; j++)
                if (bittab[j].bitn == i)
                    break;
            
            if (bittab[j].bitn == 255)
            {
                //android-removed
                //return(NULL);
                bittab[j].bitn = 0x00;
            }
            
            bitp[i] = bptr;
			memset(szDE, 0, sizeof(szDE));
			//sprintf(szDE, "DE: %03d", i);
			sprintf(szDE, "%02d:", i);
			memset(szDEData, 0x00, sizeof(szDEData));
			inAddLen=0;
            switch(bittab[j].attrib)
            {
                case NUMERIC_:
                    if (bittab[j].format == LLLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 4);
                        bptr += 2;
						inAddLen=2;
                    }
                    else if (bittab[j].format == LLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 2);
                        ++bptr;
						inAddLen=1;
                    }
                    else
                        len = bittab[j].max;
                     bptr += (len+1)/2;
					vdMyEZLib_HexDump(szDE, bitp[i], ((len+1)/2)+inAddLen);
                break;
                
                case TRACKX :
                    len = (unsigned int)bcd_to_long(bptr, 2);
                    ++bptr;
                    bptr += (len+1)/2;
					inAddLen=1;
					vdMyEZLib_HexDump(szDE, bitp[i], ((len+1)/2)+inAddLen);	
                break;
                
                case ALPHANUM_:
                case ALPHANUMS:
                    if (bittab[j].format == LLLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 4);
                        printf("len=%d\n",len);
                        bptr += 2;
						inAddLen=2;
                    }
                    else if (bittab[j].format == LLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 2);
                        ++bptr;
						inAddLen=1;
                    }
                    else
                        len = bittab[j].max;
                    bptr += len;	
                    vdMyEZLib_HexDump(szDE, bitp[i], len+inAddLen);
                break;
                
                case BITS :
                    bptr += 8;
					vdMyEZLib_HexDump(szDE, bitp[i], 8);
                break;
            }
        }
    }

	vdMyEZLib_Printf2(" "); // New Line

}

#endif

void vdLogParseISO(unsigned char *byTransType, unsigned char *pucTitle,unsigned char *buf)
{
	unsigned char i, j;
	unsigned int len;
    unsigned char szDE[12+1];
	unsigned char szDEData[256+1];
	int inDumpLen=0;
	int inAddLen=0;

	//char *bptr;
	//char *bitmap;
	//char *bitp[65];
	char szStr[44 + 4];
	BYTE baTemp[PAPER_X_SIZE * 64];   
	CTOS_FONT_ATTRIB stFONT_ATTRIB;
	BYTE szTitle[25+1];
	
	bitmap = buf+2;
	bptr = buf+10;

	szGetTransTitle(byTransType, szTitle);
	
	sprintf(szStr,"[%s: %s] \n", szTitle, pucTitle);

	//vdDebug_LogPrintf2((szStr));
	
     vdMyEZLib_HexDump_ERM("MSG ID:", buf, 2);
 
    vdMyEZLib_HexDump_ERM("BITMAP:", bitmap, 8);

   //CTOS_PrinterFline(d_LINE_DOT); 
	//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
	//vdCTOS_PrinterFline(1);
	
    for (i=1; i<65; i++)
    {

        if (bittest(bitmap, i))
        {
            for (j=0; bittab[j].bitn != 255; j++)
                if (bittab[j].bitn == i)
                    break;
            
            if (bittab[j].bitn == 255)
            {
                //android-removed
                //return(NULL);
                bittab[j].bitn = 0x00;
            }
            
            bitp[i] = bptr;
			memset(szDE, 0, sizeof(szDE));
			//sprintf(szDE, "DE: %03d", i);
			sprintf(szDE, "%02d:", i);
			memset(szDEData, 0x00, sizeof(szDEData));
			inAddLen=0;
            switch(bittab[j].attrib)
            {
                case NUMERIC_:
                    if (bittab[j].format == LLLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 4);
                        bptr += 2;
						inAddLen=2;
                    }
                    else if (bittab[j].format == LLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 2);
                        ++bptr;
						inAddLen=1;
                    }
                    else
                        len = bittab[j].max;
                     bptr += (len+1)/2;
					vdMyEZLib_HexDump_ERM(szDE, bitp[i], ((len+1)/2)+inAddLen);
                break;
                
                case TRACKX :
                    len = (unsigned int)bcd_to_long(bptr, 2);
                    ++bptr;
                    bptr += (len+1)/2;
					inAddLen=1;
					vdMyEZLib_HexDump_ERM(szDE, bitp[i], ((len+1)/2)+inAddLen);	
                break;
                
                case ALPHANUM_:
                case ALPHANUMS:
                    if (bittab[j].format == LLLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 4);
                        printf("len=%d\n",len);
                        bptr += 2;
						inAddLen=2;
                    }
                    else if (bittab[j].format == LLVAR)
                    {
                        len = (unsigned int)bcd_to_long(bptr, 2);
                        ++bptr;
						inAddLen=1;
                    }
                    else
                        len = bittab[j].max;
                    bptr += len;	
                    vdMyEZLib_HexDump_ERM(szDE, bitp[i], len+inAddLen);
                break;
                
                case BITS :
                    bptr += 8;
					vdMyEZLib_HexDump_ERM(szDE, bitp[i], 8);
                break;
            }
        }
    }

	//CTOS_PrinterFline(d_LINE_DOT*2); 
	//vdCTOS_PrinterFline(2);

	//if (isCheckTerminalMP200() != d_OK)
	//	vdCTOSS_PrinterEnd(); 
}

unsigned char bittest(unsigned char *bitmap, unsigned char pbitno)
{
  unsigned char val1, val2, val3;

  val1 = (unsigned char)((pbitno-1) / 8);
  val2 = (unsigned char)((pbitno-1)%8);
  val3 = bit_set_bits[val2];

  if(bitmap[val1] & val3)
	  return 1;
  else
	  return 0;
}

void bitset(unsigned char *bmap, unsigned char bitnum)
{
	unsigned char val1, val2, val3;

	val1 = (unsigned char)((bitnum - 1) / 8);
	val2 = (unsigned char)((bitnum - 1) % 8);
	val3 = bit_set_bits[val2];
	bmap[val1]|=val3;
}

#if 0
unsigned char *vdLoadField02(struct Transaction *t)
{
	return pack(2,(void *)"123",3);
}

unsigned char *vdLoadField03(struct Transaction *t)
{
	unsigned long prcode=10102L;
	unsigned char temp[6+1];
	unsigned char temp2[3+1];

	memset(temp,0x00,sizeof(temp));
	sprintf((char *)temp,"%06d",prcode);

	memset(temp2,0x00,sizeof(temp2));
	bin_to_bcd(temp,temp2,3);
	return pack(3,temp2,6);
}

unsigned char *vdLoadField04(struct Transaction *t)
{
	unsigned char temp[12+1];
	unsigned char temp2[6+1];

	memset(temp,0x00,sizeof(temp));
	sprintf((char *)temp,"%012d",(t->amount+t->tip_amount));

	memset(temp2,0x00,sizeof(temp2));
	bin_to_bcd(temp,temp2,6);

	return pack(4,temp2,12);
}

unsigned char *vdLoadField07(struct Transaction *t)
{
	return pack(7,(void *)"123",3);
}

unsigned char *vdLoadField011(struct Transaction *t)
{
	unsigned char temp[12+1];
	unsigned char temp2[3+1];

	memset(temp,0x00,sizeof(temp));
	sprintf((char *)temp,"%06d",t->trace_number);
	bin_to_bcd(temp,temp2,3);

	return pack(11,temp2,6);
}

unsigned char *vdLoadField012(struct Transaction *t)
{
	return pack(12,(void *)"123",3);
}
unsigned char *vdLoadField013(struct Transaction *t)
{
	return pack(13,(void *)"123",3);
}
unsigned char *vdLoadField014(struct Transaction *t)
{
	return pack(14,(void *)"123",3);
}
unsigned char *vdLoadField021(struct Transaction *t)
{
	return pack(21,(void *)"123",3);
}
unsigned char *vdLoadField022(struct Transaction *t)
{
	return pack(22,(void *)"123",3);
}
unsigned char *vdLoadField041(struct Transaction *t)
{

	return pack(41,(void *)"12345678",8);
}
unsigned char *vdLoadField042(struct Transaction *t)
{
	return pack(42,(void *)"123456789012345",15);
}

unsigned char *vdLoadField061(struct Transaction *t)
{
	return pack(61,(void *)"012000000001475",15);
}
#endif

int insert_field( char *src,  char bitnum,  char *buf, char *dst/*,  int len*/)
{	
	char *ptr;
	//char temp[512+1];
	char temp[2048+1];
	int insert_field_len=0,index=0;
	char tpdu[8+1];
	char msg_id[2+1];

	memset(temp,0,sizeof(temp));
	memset(tpdu,0,sizeof(tpdu));
	memset(msg_id,0,sizeof(msg_id));
	memset(&bitp,0,sizeof(bitp));
	//memset(bitmap,0,8);

	memcpy(tpdu,src,5); 
	memcpy(msg_id,src+5,2);
	
	Unpack(src+5);

	//print_hex("bitmap",bitmap,8);
	
	bitset(bitmap, bitnum);
	bitp[bitnum]=buf;

#ifdef PACK_DEBUG
	//print_hex("tpdu, msg_id, and bitmap",temp,index);
#endif

    index=0;
	memcpy(temp+index,tpdu,5); index+=5;
	memcpy(temp+index,msg_id,2); index+=2;
	memcpy(temp+index,bitmap,8); index+=8;

	insert_field_len=pack(&temp[index]); 
	insert_field_len+=index;

	memcpy(dst,temp,insert_field_len);

	return insert_field_len;
}

int pack(char *dst)
{
	 int i,j;
	 int len;
	char temp[512+1];
	
	//bptr=0;
	bptrlen=0;
	//temp=(char *)bptr;
	memset(temp,0,sizeof(temp));
	
#ifdef PACK_DEBUG
	printf("Pack Field(s) ");
#endif
	
	for (i=1; i<65; i++)
	{
		if (bittest(bitmap, i))
		{
	  		for (j=0; bittab[j].bitn != 255; j++)
			{
	  			if (bittab[j].bitn == i)
	  				break;
			}
	  	    
			if (bittab[j].bitn == 255)
				return(0);//return(NULL);
			
#ifdef PACK_DEBUG
	printf("%d ",i);
#endif
			
			switch(bittab[j].attrib)
			{
				case NUMERIC_ :
					if( bittab[j].format == LLLVAR )
					{
						len = ( int)bcd_long(bitp[i], 2);
						len+=2;
					}
					else if( bittab[j].format == LLVAR )
					{
						len = ( int)bcd_long(bitp[i], 1);
						len+=1;
					}
					else
					{
						len=( int)bittab[j].max;
					}

					len=(len+1)/2;
					memcpy(temp+bptrlen,bitp[i],len);
					//print_hex("NUMERIC_",(char *)temp+bptrlen,len);
					bptrlen+=len;
					//if( bitn == 2 && (len % 2)) /*TO DO*/
					//	*(bptr-1) |= 0x0f;
					//bptrlen+=(len+1)/2;
				break;

				case TRACKX :
					len = ( int)bcd_long(bitp[i], 1);
					len=(len+1)/2;
					len+=1;
					memcpy(temp+bptrlen,bitp[i],len);
					bptrlen+=len;
				break;

				case ALPHANUM_ :
				case ALPHANUMS :
					//printf("ALPHANUM_(S)\n");
					if (bittab[j].format == LLLVAR)
					{
						len = ( int)bcd_long(bitp[i], 2);
						len+=2;
						//bintobcd(len, bptr, 4);
						//bptr += 2;
					}
					else if (bittab[j].format == LLVAR)
					{
						len = ( int)bcd_long(bitp[i], 1);
						len+=1;
						//bintobcd(len, bptr, 2);
						//++bptr;
					}
					else
					{
						len=bittab[j].max;
					}

					//len=(len+1)/2;
					memcpy(temp+bptrlen,bitp[i],len);
					//print_hex("ALPHANUM_(S)",(char *)temp+bptrlen,len);
					bptrlen+=len;
					
					//bptr+bptrlen;
					
				break;

				case BITS :
					printf("BITS\n");
					//memcpy(bptr, bitp[i], 8);
					len=8;
					memcpy(temp+bptrlen,bitp[i],len);
					//bptr=bptr+bptrlen;
					//bptr=bitp[i];
					bptrlen+=8;
				break;
			}
		}
	}

#ifdef PACK_DEBUG
		printf("\n");
#endif
	
	memcpy(dst,temp,bptrlen);
	//print_hex("bptr1",(char *)temp,bptrlen);
	return bptrlen;
}

long bcd_long(char *bcd, int len)
{
	char temp[8+1];
	unsigned int index,index2=0; 
	char chTemp;

	memset(temp,0,sizeof(temp));
	for(index=0; index < len; index++)
	{
		chTemp=bcd[index];
		//cout << "chTemp = " << chTemp << endl;
		temp[index2]=((chTemp & 0xf0) >> 4) | 0x30; index2++;
		//cout << "1. temp[index2] = " << temp[index2-1] << endl;
		temp[index2]=(chTemp & 0x0f) | 0x30; index2++;
		//cout << "2. temp[index2] = " << temp[index2-1] << endl;
	}

	return atol(temp);
}

void long_bcd( char *buf,  long val, int len)
{
	char temp[8+1];
	int index,index2=0;
	char chTemp;

	memset(temp,0,sizeof(temp));
	//sprintf_s(temp,"%04ld",val);
	sprintf(temp,"%04ld",val);
	
	for(index=0; index < len; index++)
	{

	}
}

char *Unpack( char *buf)
{
   char i, j;
   int len;

  bitmap = buf+2;
  bptr = buf+10;

  for (i=1; i<65; i++)
  {
  	if (bittest(bitmap, i))
  	{
		//printf("unpack field %d\n",i);
	  	for (j=0; bittab[j].bitn != 255; j++)
	  	  if (bittab[j].bitn == i) 
	  	    break;
	  	    
		  if (bittab[j].bitn == 255)
		    return(NULL);
		//LOG_PRINTF(("bitp[%d]",i));    
  		bitp[i] = bptr;
		switch(bittab[j].attrib)
		  {
				case NUMERIC_ :
			  	if (bittab[j].format == LLLVAR)
				  {
					  len = ( int)bcd_long(bptr, 2);
  					bptr += 2;
	  			}
		  		else if (bittab[j].format == LLVAR)
			  	{
				  	len = ( int)bcd_long(bptr, 1);
					  ++bptr;
  				}
	  			else
	  			{
					len = bittab[j].max;
	  			}
				//printf("NUMERIC_ len=%d\n",len);
		  		bptr += (len+1)/2;
			  	break;
			  	
			  case TRACKX :
				  len = ( int)bcd_long(bptr, 1);
				  ++bptr;
				  bptr += (len+1)/2;
				  break;
				  
			  case ALPHANUM_ :
			  case ALPHANUMS :
				  if (bittab[j].format == LLLVAR)
  				{
					//print_hex("len value",bptr,2);
	  				len = ( int)bcd_long(bptr, 2);
		  			bptr += 2;
			  	}
				  else
				  if (bittab[j].format == LLVAR)
				  {
					  len = ( int)bcd_long(bptr, 1);
					  ++bptr;
				  }
				  else
				  {
				  	len = bittab[j].max;
				  }
				 //printf("ALPHANUM_ len=%d\n",len);
				  bptr += len;
				  break;
				  
			  case BITS :
				  bptr += 8;
				  break;
		  }
		//print_hex("bitp[i]",bitp[i],len);
	  }else bitp[i] = NULL;
  }
  return(bptr);
}

