/** 
**  A Template for developing new terminal shared application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

/** These two files are necessary for calling CTOS API **/
#include "../Includes/myEZLib.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "../Debug/Debug.h"
#include "../DataBase/DataBaseFunc.h"

#define SHOW_SCRREN 1
#define d_LINE_DOT 12

BOOL fLogByUSB;
BOOL fLogByRS232;
BOOL fZMKByRS232;
BOOL ONUSB = FALSE; 

#define d_LCD_Width 16+1

extern BOOL fERMTransaction; // Enhancement for printing speed


void vdMyEZLib_ComPortInit()
{
    int inResult;
    USHORT usVendorID=0x0CA6, usProductID=0xA050, usRtn;
    
    fLogByUSB   = FALSE;
    fLogByRS232 = FALSE; 
    
    CTOS_RS232Close(DBG_PORT);
    if(ONUSB==TRUE)
    {
    
        CTOS_USBHostOpen(usVendorID, usProductID); 
        fLogByUSB = TRUE;   
    }
    else
    {            
        if((inResult=CTOS_RS232Open(DBG_PORT, 115200, 'N', 8, 1)) == d_OK)
        {
            fLogByRS232 = TRUE;    
        }        
    }
}



void vdMyEZLib_Printf(const char* fmt, ...)
{
    char printBuf[2048];
    char msg[2048];
    int inSendLen;
    va_list marker;
    BYTE baTemp[PAPER_X_SIZE * 64];   
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

	//vdDebug_LogPrintf("vdMyEZLib_Printf | fERMTransaction[%d]", fERMTransaction);
	
    memset(msg, 0x00, sizeof(msg));
    
    va_start( marker, fmt );
    vsprintf( msg, fmt, marker );
    va_end( marker );
    
    memset(printBuf, 0x00, sizeof(printBuf));
    strcat(printBuf, msg);
    strcat(printBuf ,"\n" );
    
    inSendLen = strlen(printBuf);
    
    //memset (baTemp, 0x00, sizeof(baTemp));      
    stFONT_ATTRIB.FontSize = d_FONT_16x16;      // Font Size = 12x24
    stFONT_ATTRIB.X_Zoom = 1;       // The width magnifies X_Zoom diameters
    stFONT_ATTRIB.Y_Zoom = 2;       // The height magnifies Y_Zoom diameters
    stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font
    stFONT_ATTRIB.Y_Space = 0;      // The Height of the space between the font with next font        
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, printBuf, &stFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 
    inCCTOS_PrinterBufferOutput(printBuf,&stFONT_ATTRIB, fERMTransaction);
}

void vdMyEZLib_Printf2(const char* fmt, ...)
{
    char printBuf[2048];
    char msg[2048];
    int inSendLen;
    va_list marker;
    BYTE baTemp[PAPER_X_SIZE * 64];   
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    memset(msg, 0x00, sizeof(msg));
    
    va_start( marker, fmt );
    vsprintf( msg, fmt, marker );
    va_end( marker );
    
    memset(printBuf, 0x00, sizeof(printBuf));
    strcpy(printBuf, msg);
    strcat(printBuf ,"\n" );
    
    inSendLen = strlen(printBuf);
    
    memset (baTemp, 0x00, sizeof(baTemp));      
    stFONT_ATTRIB.FontSize = d_FONT_16x16;      // Font Size = 12x24
    stFONT_ATTRIB.X_Zoom = 1;       // The width magnifies X_Zoom diameters
    stFONT_ATTRIB.Y_Zoom = 2;       // The height magnifies Y_Zoom diameters
    stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font
    stFONT_ATTRIB.Y_Space = 0;      // The Height of the space between the font with next font        
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, printBuf, &stFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 
    //inCCTOS_PrinterBufferOutput(printBuf,&stFONT_ATTRIB,1);
}


/*==========================================================================*
 *Name:vdMyEZLib_LogPrintf                                                  *                           
 *input: as Like vdMyEZLib_LogPrintf("%s", test);                           *           
 *output: void                                                              *
 *Description:                                                              *
 *  Caller can ouput that any data type wanted to check to console by RS232 *
 *==========================================================================*/
void vdMyEZLib_LogPrintff(unsigned char *fmt, short inlen)
{

    char printBuf[2048];
    char msg[2048];
    short i=0;
    
        
    memset(msg, 0x00, sizeof(msg));
    memset(printBuf, 0x00, sizeof(printBuf));
    memset(printBuf, 0x30, inlen*2);
    for ( i=0 ;i<inlen; i++)
    {
        sprintf(&printBuf[i*2], "%02X", fmt[i]);
    }

    vdMyEZLib_LogPrintf(printBuf);
      
}


void vdMyEZLib_Rpad(char *pad, char *space)
{
    short inlen;
    
    for(inlen=0; inlen<(79-strlen(pad)); inlen ++)
        space[inlen]=0x20;
}


void vdMyEZLib_LogPrintf(const char* fmt, ...)
{
    char printBuf[2048];
    char msg[2048];
    char space[100];
    int inSendLen;
    va_list marker;
    int j = 0;
               
    if(fLogByUSB == FALSE && fLogByRS232 == FALSE)
        return;

    memset(msg, 0x00, sizeof(msg));
    memset(printBuf, 0x00, sizeof(printBuf));
    memset(space, 0x00, sizeof(space));
    
    va_start( marker, fmt );
    vsprintf( msg, fmt, marker );
    va_end( marker );
    
    memset(printBuf, 0x00, sizeof(printBuf));       
    strcat(printBuf, msg);
    strcat(printBuf, space);
    strcat(printBuf ,"\n" );
    
    inSendLen = strlen(printBuf);

    
    j = (int)strTCT.byRS232DebugPort;        
    if(j == 1 ) //Remark this line to view log
    {
           
        if(fLogByUSB==TRUE)
        {       
            if (CTOS_USBTxReady() == d_OK)
                CTOS_USBTxData(printBuf, inSendLen);
        }
        else
        {        
            while(CTOS_RS232TxReady(DBG_PORT) != d_OK);
            CTOS_RS232TxData(DBG_PORT, printBuf, inSendLen);    
         
        }
    }
    return;        
}

int inAscii2Bcd(BYTE *szAscii, BYTE *szBcd, int inBcdLen)
{
    int inAsciiLen = inBcdLen*2;
    int inBcdIdx;
    int inAsciiIdx;
    BYTE szAscii2[inBcdLen*2+1];
    BYTE byLo;
    BYTE byHi;
        
    memset(szAscii2, 0x00, sizeof(szAscii2));
    /*pad zero for ascii buffer*/
    if((inAsciiLen = strlen(szAscii)) <= (inBcdLen*2))
    {
        memset(szAscii2, 0x30, inBcdLen*2);
        memcpy(&szAscii2[(inBcdLen*2)-inAsciiLen], szAscii, inAsciiLen);        
        vdMyEZLib_LogPrintf("[inAscii2Bcd]---szAscii2[%s], szAscii[%s]", szAscii2, szAscii);
    }
    else
    {       
        memcpy(szAscii2, szAscii, inBcdLen*2);
    }   
        
    
    inAsciiIdx=0;
    inBcdIdx=0;
    while(inBcdIdx < inBcdLen)
    {
        //for Hi Bit
        if(szAscii2[inAsciiIdx] >= 'A' && szAscii2[inAsciiIdx] <= 'F')
            byHi = (szAscii2[inAsciiIdx]-0x37) << 4; // 0x61 - 0x37 = 0x0A  
        else if(szAscii2[inAsciiIdx] >= 'a' && szAscii2[inAsciiIdx] <= 'f')
            byHi = (szAscii2[inAsciiIdx]-0x57) << 4;        
        else
            byHi = szAscii2[inAsciiIdx] << 4;   
        
        
        //for Low Bit
        if(szAscii2[inAsciiIdx+1] >= 'A' && szAscii2[inAsciiIdx+1] <= 'F')
            byLo = (szAscii2[inAsciiIdx+1]-0x37) & 0x0F; // 0x61('A') - 0x37 = 0x0A 
        else if(szAscii2[inAsciiIdx+1] >= 'a' && szAscii2[inAsciiIdx+1] <= 'f')
            byLo = (szAscii2[inAsciiIdx+1]-0x57) & 0x0F;        
        else
            byLo = szAscii2[inAsciiIdx+1] & 0x0F;   
                
        
        //byLo = szAscii2[inAsciiIdx+1] & 0x0F;
                                        
        szBcd[inBcdIdx] = byHi | byLo;
               
        
        inBcdIdx++;
        inAsciiIdx = inBcdIdx *2;
                
    }
    
    
    return TRUE;        
    
}


int inBcd2Ascii(BYTE *szBcd, BYTE *szAscii, int inBcdLen)
{
    int inBcdIdx;
    int inAsciiIdx;
    BYTE szAscii2[3];
    BYTE byLo;
    BYTE byHi;
    
    inBcdIdx = 0;
    inAsciiIdx = 0;
    while(inBcdIdx < inBcdLen)
    {
        sprintf(szAscii2, "%02x", szBcd[inBcdIdx]);
        memcpy(&szAscii[inAsciiIdx], szAscii2, 2);
        inBcdIdx ++;
        inAsciiIdx = inBcdIdx*2;        
    }

    return TRUE;    
    
}

int inBcd2Bin(BYTE *szBcd, int inBcdLen)
{
    char szAscii[inBcdLen*2+1];
    int inBin;
    
    memset(szAscii, 0x00, sizeof(szAscii));

    if(inBcd2Ascii(szBcd, szAscii, inBcdLen))
    {
        inBin = atoi(szAscii);
        return inBin;
    }                        
    
    return ST_ERROR;
}

int inBin2Bcd(int inBin, BYTE *szBcd, int inBcdLen)
{
    char szAscii[inBcdLen*2+1];
    memset(szAscii, 0x00, sizeof(szAscii));

    sprintf(szAscii, "%d", inBin);
    vdMyEZLib_LogPrintf("[inBin2Bcd]---Bin2Ascii[%s]",szAscii);
    
    
    return(inAscii2Bcd(szAscii, szBcd, inBcdLen));
}

int inBcdAddOne(BYTE *szInBcd, BYTE *szOutBcd, int inBcdLen)
{
    int inBin;
    int inResult;
    char szAscii[inBcdLen*2+1];
    
    
    memset(szAscii, 0x00, sizeof(szAscii));
        
    /*BCD to Bin*/
    inBin = inBcd2Bin(szInBcd, inBcdLen);
    vdMyEZLib_LogPrintf("[inBcdAddOne]---BCD 2 Bin[%d]", inBin);
    
    /*Bin++*/
    inBin++;
    
    /*Bin to BCD*/
    inResult = inBin2Bcd(inBin, szOutBcd, inBcdLen);
        
    return inResult;
    
}

#if 0
void vdMyEZLib_HexDump(unsigned char *msg, unsigned char *fmt, short inlen)
{
    char printBuf[128+1];
    short i=0;
    short index=0;
	BOOL inLabelFlag=TRUE;

    //vdMyEZLib_Printf2("%s Len: (%d)\n",msg, inlen);    
    memset(printBuf, 0x00, sizeof(printBuf));
    for ( i=0 ;i<inlen; i++)
    {
        sprintf(&printBuf[index*3], "%02x ", fmt[i]);
        
        if(index == 15)
        {
            vdMyEZLib_Printf2(printBuf);
            memset(printBuf, 0x00, sizeof(printBuf));
            index=0;
        }
        else
            index++; 
    }
    vdMyEZLib_Printf2(printBuf);
}
#else
void vdMyEZLib_HexDump(unsigned char *msg, unsigned char *fmt, short inlen)
{
    char printBuf[128+1];
    short i=0;
    short index=0;
	BOOL inLabelFlag=TRUE;	
   
    //vdMyEZLib_Printf2("%s Len: (%d)\n",msg, inlen);    
    memset(printBuf, 0x00, sizeof(printBuf));
    for ( i=0 ;i<inlen; i++)
    {
        sprintf(&printBuf[index*3], "%02x ", fmt[i]);
        
        if(index == 13)
        {
            //vdMyEZLib_Printf2(printBuf);
            if(i>13)
				inLabelFlag=FALSE;
            vdMyEZLib_Printf3(inLabelFlag,msg,printBuf);
            memset(printBuf, 0x00, sizeof(printBuf));
            index=0;
        }
        else
            index++; 
    }
	
	if(i>13)
		inLabelFlag=FALSE;
		
    vdMyEZLib_Printf3(inLabelFlag,msg,printBuf);
}

#endif
   
void vdComputeCRC(unsigned long lnCRC, char *ptrCRC)
{
    unsigned char szCRC[8+1];

    memset(szCRC, 0, sizeof(szCRC));
    sprintf(szCRC, "%08ld",lnCRC);
    sprintf(ptrCRC,"%02x%02x%02x%02x",(unsigned char)((szCRC[0])*(szCRC[4])),
				      (unsigned char)((szCRC[1])*(szCRC[5])),
				      (unsigned char)((szCRC[2])*(szCRC[6])),
				      (unsigned char)((szCRC[3])*(szCRC[7])));

}

void vdFormatAmount(char *str, char *strCur, char *strAmt, BOOL fVoid)
{
	char amountstr[32], moredigits[7], tempstr[15], szTemp[AMT_BCD_SIZE+1];

    wub_hex_2_str(strAmt, szTemp, AMT_BCD_SIZE);

	sprintf(amountstr, "%lu.%02lu", atol(szTemp)/100, atol(szTemp)%100);

	while ( atol(amountstr) > 999 ) {
		sprintf(moredigits, "%ld", (long)(atol(amountstr)/1000));
		sprintf(tempstr, "%s,%s", moredigits, &amountstr[strlen(moredigits)]);
		strcpy(amountstr, tempstr);
	}
	if(fVoid == TRUE)
		sprintf(str, "-%s%s", strCur, amountstr);
	else	
	    sprintf(str, "%s%s", strCur, amountstr);
}

void TrimTrail(unsigned char *strTrim)
{
    USHORT shLen;
    char szStr[64 + 1];
    
    shLen = strlen(strTrim);

    if(shLen > 0)
    {
        memset(szStr, 0x00, 64);
        strcpy(szStr, strTrim);

        while(0x20 == szStr[--shLen] && shLen >= 1)
            szStr[shLen] = 0x00;
    }
	strTrim[shLen+1] = 0x00;
}

void vdMyEZLib_Printf3(BOOL inLabelFlag, unsigned char *DELabel, const char* fmt, ...)
{
    char printBuf[2048];
    char msg[2048];
    int inSendLen;
    va_list marker;
    BYTE baTemp[PAPER_X_SIZE * 64];   
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    memset(msg, 0x00, sizeof(msg));
    
    va_start( marker, fmt );
    vsprintf( msg, fmt, marker );
    va_end( marker );
    
    memset(printBuf, 0x00, sizeof(printBuf));
    //strcpy(printBuf, msg);
    if(inLabelFlag)
		strcpy(printBuf, DELabel);
	else
		strcpy(printBuf, "   ");

	strcat(printBuf, msg);
	//strcat(printBuf ,"\n" );

	/*
    inSendLen = strlen(printBuf);
    memset (baTemp, 0x00, sizeof(baTemp));      
    stFONT_ATTRIB.FontSize = d_FONT_16x16;      // Font Size = 12x24
    stFONT_ATTRIB.X_Zoom = 1;       // The width magnifies X_Zoom diameters
    stFONT_ATTRIB.Y_Zoom = 2;       // The height magnifies Y_Zoom diameters
    stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font
    stFONT_ATTRIB.Y_Space = 0;      // The Height of the space between the font with next font        
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, printBuf, &stFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 
    inCCTOS_PrinterBufferOutput(printBuf,&stFONT_ATTRIB,1);
    */

	vdMyEZLib_Printf2(printBuf);
}

void vdMyEZLib_HexDump_ERM(unsigned char *msg, unsigned char *fmt, short inlen)
{
    char printBuf[128+1];
    short i=0;
    short index=0;
	BOOL inLabelFlag=TRUE;	
   
    //vdMyEZLib_Printf2("%s Len: (%d)\n",msg, inlen);    
    memset(printBuf, 0x00, sizeof(printBuf));
    for ( i=0 ;i<inlen; i++)
    {
        sprintf(&printBuf[index*3], "%02x ", fmt[i]);
        
        if(index == 13)
        {
            //vdMyEZLib_Printf2(printBuf);
            if(i>13)
				inLabelFlag=FALSE;
            vdMyEZLib_Printf3_ERM(inLabelFlag,msg,printBuf);
            memset(printBuf, 0x00, sizeof(printBuf));
            index=0;
        }
        else
            index++; 
    }
	
	if(i>13)
		inLabelFlag=FALSE;
		
    vdMyEZLib_Printf3_ERM(inLabelFlag,msg,printBuf);
}

void vdMyEZLib_Printf3_ERM(BOOL inLabelFlag, unsigned char *DELabel, const char* fmt, ...)
{
    char printBuf[2048];
    char msg[2048];
    int inSendLen;
    va_list marker;
    BYTE baTemp[PAPER_X_SIZE * 64];   
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    memset(msg, 0x00, sizeof(msg));
    
    va_start( marker, fmt );
    vsprintf( msg, fmt, marker );
    va_end( marker );
    
    memset(printBuf, 0x00, sizeof(printBuf));
    if(inLabelFlag)
		strcpy(printBuf, DELabel);
	else
		strcpy(printBuf, "   ");

	strcat(printBuf, msg);

    inSendLen = strlen(printBuf);

	//vdDebug_LogPrintf2(printBuf);


}

/* Delimited string -- sidumili */
int GetDelimitedString(char *buf, int fldnum, unsigned char sep, char *dest)
{
	unsigned char *ptr;
	int i;

	ptr = (unsigned char *) buf;
	for (i=1; i < fldnum; i++) 
	{
		if ((ptr = (unsigned char *) strchr((char *) ptr, sep))) ptr++;
		else return(0);
	}

	for (i=0;*ptr != sep && *ptr && i<254;i++) *dest++ = *ptr++;
	*dest = '\0';

	if (*ptr != sep) return(0);

	return(i);
}

//ISO Logger 
int inSaveISOLog(BOOL fSendPacket, unsigned char *pucMessage, int inLen)
{
     char ucLineBuffer[1000] = {0};
     unsigned char *pucBuff;
     int inBuffPtr = 0;
     FILE *ISOFile;
     char szBuf[100] = {0};
     int inRecCnt = 0;
     int inResult = 0;
     CTOS_RTC SetRTC;
     BYTE szCurrentTime[50] = {0};
     
     if((inLen > 0) && (strTCT.inISOLogger == TRUE))
     {
          memset(szCurrentTime, 0x00, sizeof(szCurrentTime));
          CTOS_RTCGet(&SetRTC);
          sprintf(szCurrentTime, "%02d/%02d/%02d %02d:%02d\n", SetRTC.bMonth, SetRTC.bDay, SetRTC.bYear, SetRTC.bHour, SetRTC.bMinute);
          strcpy((char*)srTransISOLog.szISOLogDateTime,szCurrentTime);
		  
          memset(szBuf, 0, sizeof(szBuf));
          
          switch(srTransRec.byPackType)
          {
               case SALE:
               strcpy(szBuf, "SALE ");
               break;
               case PRE_AUTH:
               strcpy(szBuf, "PRE_AUTH ");
               break;
               case TC_UPLOAD:
               strcpy(szBuf, "TC_UPLOAD ");
               break;
               case SEND_ADVICE:
               strcpy(szBuf, "SEND_ADVICE ");
               break;
               case BAL_INQ:
               strcpy(szBuf, "BALANCE INQUIRY ");
               break;
               case LOG_ON:
               strcpy(szBuf, "LOG-ON ");
               break;
               case REVERSAL:
               strcpy(szBuf, "REVERSAL ");
               break;				
               case REFUND:
               strcpy(szBuf, "REFUND ");
               break;
               case VOID:
               strcpy(szBuf, "VOID ");
               break;
               case SALE_TIP:
               strcpy(szBuf, "SALE_TIP ");
               break;
               case SALE_OFFLINE:
               strcpy(szBuf, "TSALE_OFFLINE ");
               break;
               case SETTLE:
               strcpy(szBuf, "SETTLE ");
               break;
               case CLS_BATCH:
               strcpy(szBuf, "CLS_BATCH ");
               break;
               case BATCH_UPLOAD:
               strcpy(szBuf, "BATCH_UPLOAD ");
               break;
               case SALE_ADJUST:
               strcpy(szBuf, "SALE_ADJUST ");
               break;
               case EPP_SALE:
               strcpy(szBuf, "EPP_SALE ");
               break;
               case VOID_REFUND:
               strcpy(szBuf, "VOID_REFUND ");
               break;
               case OFFLINE_VOID:
               strcpy(szBuf, "OFFLINE_VOID ");
               break;
               case OFFLINE_REFUND:
               strcpy(szBuf, "OFFLINE_REFUND ");
               break;
               case VOID_REVERSAL:
               strcpy(szBuf, "VOID_REVERSAL ");
               break;
               case LOY_BAL_INQ:
               strcpy(szBuf, "LOY_BAL_INQ ");
               break;
               case LOY_REDEEM_5050:
               strcpy(szBuf, "LOY_BAL_INQ ");
               break;
               case LOY_REDEEM_VARIABLE:
               strcpy(szBuf, "LOY_REDEEM_VARIABLE ");
               break;
               case LOY_REDEEM_REVERSAL:
               strcpy(szBuf, "LOY_REDEEM_REVERSAL ");
               break;
               case INSTALLMENT_SALE:
               strcpy(szBuf, "INSTALLMENT_SALE ");
               break;
               case TERM_REG:
               strcpy(szBuf, "TERM_REG ");
               break;
               case CARD_VERIFY:
               strcpy(szBuf, "CARD_VERIFY ");
               break;
               case PRECOMP_REVERSAL:
               strcpy(szBuf, "PRECOMP_REVERSAL ");
               break;
               case PRE_COMP:
               strcpy(szBuf, "PRE_COMP ");
               break;
               case CASH_ADV:
               strcpy(szBuf, "CASH_ADV ");
               break;
               case CASHADV_REVERSAL:
               strcpy(szBuf, "CASHADV_REVERSAL ");
               break;
               case VOID_REDEEM:
               strcpy(szBuf, "VOID_REDEEM ");
               break;
               case VOIDREDEEM_REVERSAL:
               strcpy(szBuf, "VOIDREDEEM_REVERSAL ");
               break;
               case VOID_PRECOMP:
               strcpy(szBuf, "VOID_PRECOMP ");
               break;
               case PREAUTH_VER:
               strcpy(szBuf, "PREAUTH_VER ");
               break;
               case PREAUTH_COMP:
               strcpy(szBuf, "PREAUTH_COMP ");
               break;
               case PREAUTH_VER_REVERSAL:
               strcpy(szBuf, "PREAUTH_VER_REVERSAL ");
               break;
               case PREAUTH_VOID:
               strcpy(szBuf, "PREAUTH_VOID ");
               break;
               case PREAUTH_VOID_REVERSAL:
               strcpy(szBuf, "PREAUTH_VOID_REVERSAL ");
               break;
               case DCC_RATE_REQUEST:
               strcpy(szBuf, "DCC_RATE_REQUEST ");
               break;
               case DCC_MERCH_RATE_REQ:
               strcpy(szBuf, "DCC_MERCH_RATE_REQ ");
               break;
			   default:
                 strcpy(szBuf, "UNDEFINED TRANS TYPE");
                 break;
          
          }
          
          if (fSendPacket)
          strcat(szBuf, "REQUEST");
          else
          strcat(szBuf, "RESPONSE");

		  strcpy((char*)srTransISOLog.szISOLogTransType, szBuf);
          
          wub_hex_2_str(pucMessage,ucLineBuffer,inLen);
          strcpy((char*)srTransISOLog.szISOLogData,ucLineBuffer);
          inDatabase_ISOLogInsert(&srTransISOLog);
          strTCT.inISOTxnLogCnt++;
          inRecCnt = strTCT.inISOTxnLogLimit - strTCT.inISOTxnLogCnt;
          if(inRecCnt == 0)
          {
               inDatabase_DeleteISOLogRec();
               strTCT.inISOTxnLogCnt--; 
          }
          inTCTSave(1);
     }
     return SUCCESS;
}

