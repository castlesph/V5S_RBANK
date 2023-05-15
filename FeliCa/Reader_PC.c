/**
\file	Reader_PC.c
\brief	IC Card Reader Access functions
\date	2013/11/6
\author	Copyright 2013 Sony Corporation*/

//#pragma comment(lib, "winscard.lib")

//#include <winscard.h>
//#include <stdio.h>
//#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include <ctos_clapi.h>
#include <Vwdleapi.h>
#include "emv_cl.h"

#include "Reader.h"
#include "Util.h"
#include "..\Debug\Debug.h"
#include "..\Aptrans\MultiAptrans.h"



// PC/SC connection const
static const unsigned char PASORI_PCSC_NO_ERROR_HEADER[5] = {0xC0, 0x03, 0x00, 0x90, 0x00};

// Reader name of PaSoRi
static unsigned char  CARD_IF_PASORI[] = "Sony FeliCa Port/PaSoRi 3.0 0";

// Reader name of Contact Card reader (depends on driver version)
static unsigned char SAM_IF_GEM[] = "Gemalto USB Smart Card Reader 0";
//static wchar_t SAM_IF_GEM[] = L"Gemplus USB Smart Card Reader 0";
//static wchar_t SAM_IF_GEM[] = L"Gemplus USB SmartCard Reader 0"; 

// PC/SC context
//SCARDCONTEXT	hContext;
//SCARDHANDLE		hCardFeliCa;
//SCARDHANDLE		hCardSAM;

// Buffer to store PC/SC Reader Names
//LPTSTR  mszReaders;

#define SCARD_S_SUCCESS 0

extern USHORT usSecTimeOut;

//---------------------------------------------
USHORT usCal_SECUtimeout(UCHAR *bParameter)
{
	USHORT msec;
	
	// unit ( 0.1 msec)
	msec = bParameter[1] * 256 + bParameter[2];
	// transfer into 0.302ms unit
	msec /= 3;
	return msec;  
}


long InitializeReader(void)
{
	#if 0
	long			_ret =0;
	unsigned long	pcchReaders = SCARD_AUTOALLOCATE;
	unsigned long	dwActProtocolSAM;
	LPTSTR			pReader;

	printf("Initialize Reader\n");

	//Establish Context
	printf("Establish Context\n");
	_ret = SCardEstablishContext( SCARD_SCOPE_USER,
		NULL,
		NULL,
		&hContext );
	if(_ret != SCARD_S_SUCCESS) {
		printf(" -> Error\n");
		return APP_ERROR;
	}

	// List All Readers
	printf("List All Readers\n");
	_ret = SCardListReaders(	hContext, NULL, (LPTSTR)&mszReaders, &pcchReaders);
	if(_ret != SCARD_S_SUCCESS) {
		printf(" -> Error\n");
		return APP_ERROR;
	}
	pReader = mszReaders;
	while ( '\0' != *pReader ) {
		printf(" %S\n", pReader );
		pReader = pReader + wcslen((wchar_t *)pReader) + 1;
	}

	SCardFreeMemory(hContext, mszReaders );

	// Connect to SAM interface
	printf("Connect SAM\n");
	_ret = SCardConnect(hContext,
							SAM_IF_GEM,
							SCARD_SHARE_SHARED,
							SCARD_PROTOCOL_T1,
							&hCardSAM,
							&dwActProtocolSAM
							);
	if(_ret != SCARD_S_SUCCESS){
		printf(" -> Error\n");
		return APP_ERROR;
	}
	#endif
	CTOS_CLPowerOn();
	
	return APP_SUCCESS;
}

BYTE	bSAMSlot;
void DisconnectReader(void) 
{

	//SCardDisconnect(hCardFeliCa, SCARD_UNPOWER_CARD);
	//SCardDisconnect(hCardSAM, SCARD_UNPOWER_CARD);
	//SCardReleaseContext(hContext);

	// power off SAM
	CTOS_SCPowerOff(bSAMSlot);

	// power off RF??
	CTOS_CLPowerOff();
}

#if 0
long DetectFeliCaCard(void) 
{
	long			_ret;

	unsigned long	readerLen, ATRLen, dwState;
	unsigned char	ATRVal[262];
	unsigned long	dwActProtocol;

	unsigned char	_send_buf[262], receiveBuf[262];
	unsigned long	_send_len, receiveLen;

	printf("\nTap FeliCa Card\n");
	printf("<Press ESC Key to cancel>\n");

	while(1){
		// Hit Esc to cancel operation
		if((0 != kbhit()) && (SMPL_ESC_KEY == getch()))
		{
			printf("  -> Canceled\n");
			return APP_CANCEL;
		}
		_ret = SCardConnect(hContext,
			CARD_IF_PASORI,
			SCARD_SHARE_SHARED,
			SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1,
			&hCardFeliCa,
			&dwActProtocol
			);

		if(_ret != SCARD_S_SUCCESS)
		{
			continue;
		}

		//Get ATR value to check status
		readerLen = 100;
		ATRLen = 32;
		_ret = SCardStatus( hCardFeliCa,
			CARD_IF_PASORI,
			&readerLen,
			&dwState,
			&dwActProtocol,
			ATRVal,
			&ATRLen );

		if( _ret != SCARD_S_SUCCESS )
		{
			return APP_ERROR;
		}

		// Start Transparent Session of PC/SC on Pasori
		_send_buf[0] = 0xFF;
		_send_buf[1] = 0xC2;
		_send_buf[2] = 0x00;
		_send_buf[3] = 0x00;
		_send_buf[4] = 0x02;
		_send_buf[5] = 0x81;
		_send_buf[6] = 0x00;
		_send_len = 7;
		receiveLen = 7;
		_ret = SCardTransmit(hCardFeliCa,
			NULL,
			_send_buf,
			_send_len,
			NULL,  
			receiveBuf,
			&receiveLen);

		if(_ret != SCARD_S_SUCCESS){
			return APP_ERROR;
		}

		// Response of PC/SC Start Transparent Session with no error contains 7 bytes
		//  C0 03 00 90 00  90 00
		if(receiveLen != 7){
			return APP_ERROR;
		}
		if ( memcmp (receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
			return APP_ERROR;
		}

		break;
	}

	return APP_SUCCESS;
}


long DetectFeliCaCardEx(void) 
{
	long			_ret;

	unsigned long	readerLen, ATRLen, dwState;
	unsigned char	ATRVal[262];
	unsigned long	dwActProtocol;

	unsigned char	_send_buf[262], receiveBuf[262];
	unsigned long	_send_len, receiveLen;

	vdDebug_LogPrintf("\nTap FeliCa Card\n");
	vdDebug_LogPrintf("<Press ESC Key to cancel>\n");

	return 0;

	//while(1)
	{


		// Start Transparent Session of PC/SC on Pasori
		_send_buf[0] = 0xFF;
		_send_buf[1] = 0xC2;
		_send_buf[2] = 0x00;
		_send_buf[3] = 0x00;
		_send_buf[4] = 0x02;
		_send_buf[5] = 0x81;
		_send_buf[6] = 0x00;
		_send_len = 7;
		receiveLen = 7;
		#if 0
		_ret = SCardTransmit(hCardFeliCa,
			NULL,
			_send_buf,
			_send_len,
			NULL,  
			receiveBuf,
			&receiveLen);
		#endif	
		_ret = cepas_Sub_CLSendAPDU(_send_buf, _send_len, receiveBuf, &receiveLen);

		if(_ret != SCARD_S_SUCCESS){
			return APP_ERROR;
		}

		// Response of PC/SC Start Transparent Session with no error contains 7 bytes
		//  C0 03 00 90 00  90 00
		if(receiveLen != 7){
			return APP_ERROR;
		}
		if ( memcmp (receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
			return APP_ERROR;
		}

		//break;
	}

	return APP_SUCCESS;
}
#endif


long DisconnectFeliCaCard(void) 
{
	long	_ret;
	unsigned char	_send_buf[262];
	unsigned char	receiveBuf[262];
	unsigned long	_send_len, receiveLen;

	#if 0

	// Turn off RF Power
	_send_buf[0] = 0xFF;
	_send_buf[1] = 0xC2;
	_send_buf[2] = 0x00;
	_send_buf[3] = 0x00;
	_send_buf[4] = 0x02;
	_send_buf[5] = 0x83;
	_send_buf[6] = 0x00;
	_send_len = 7;
	receiveLen = 7;
	#if 0
	_ret = SCardTransmit(hCardFeliCa,
							NULL,
							_send_buf,
							_send_len,
							NULL,  
							receiveBuf,
							&receiveLen);
	#endif
	_ret = usFelicaTransmit(_send_buf,_send_len,receiveBuf,&receiveLen, usSecTimeOut);

	if(_ret != SCARD_S_SUCCESS){
		return APP_ERROR;
	}

	// End Transparent Session of PC/SC on Pasori
	_send_buf[0] = 0xFF;
	_send_buf[1] = 0xC2;
	_send_buf[2] = 0x00;
	_send_buf[3] = 0x00;
	_send_buf[4] = 0x02;
	_send_buf[5] = 0x82;
	_send_buf[6] = 0x00;
	_send_len = 7;
	receiveLen = 7;
	#if 0
	_ret = SCardTransmit(hCardFeliCa,
							NULL,
							_send_buf,
							_send_len,
							NULL,  
							receiveBuf,
							&receiveLen);
	#endif
	_ret = usFelicaTransmit(_send_buf,_send_len,receiveBuf,&receiveLen, usSecTimeOut);

	if(_ret != SCARD_S_SUCCESS){
		return APP_ERROR;
	}

	// Response of PC/SC Start Transparent Session with no error contains 7 bytes
	//  C0 03 00 90 00  90 00
	if(receiveLen != 7){
		return APP_ERROR;
	}
	if ( memcmp (receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
		return APP_ERROR;
	}

	#endif

	return APP_SUCCESS;
}

#if 0
long TransmitDataToSAM(unsigned char samCmdBuf[], unsigned long samCmdLen, unsigned char samResBuf[], unsigned long* samResLen)
{
	long _ret = 0;
	unsigned char _send_buf[262];
	unsigned long _send_len;
	unsigned char tmpBuf[262];
	unsigned long tmpBufLen;
	unsigned char sw1, sw2;
	unsigned int i;

	_send_buf[0] = 0xA0; //CLA
	_send_buf[1] = 0x00; //INS
	_send_buf[2] = 0x00; //P1
	_send_buf[3] = 0x00; //P2
	_send_buf[4] = (unsigned char)samCmdLen; //Lc
	for( i=0; i<samCmdLen; i++) {
		_send_buf[i+5] = samCmdBuf[i];
	}
	_send_buf[samCmdLen +5] = 0x00; //Le
	_send_len = samCmdLen + 6;
	tmpBufLen = *samResLen;

#ifdef DEBUG
	PrintHexArray("PC->SAM: ", _send_len, _send_buf);
#endif
#if 0
	_ret = SCardTransmit(hCardSAM,
		NULL,
		_send_buf,
		_send_len,
		NULL,  
		tmpBuf,
		&tmpBufLen);
#endif
	_ret = cepas_Sub_CLSendAPDU(_send_buf,_send_len,tmpBuf,&tmpBufLen);


	if( _ret != SCARD_S_SUCCESS )
	{
		printf("SCardTransmit Error\n");
		return APP_ERROR;
	}

#ifdef DEBUG
	PrintHexArray("SAM->PC: ", tmpBufLen, tmpBuf);
#endif
	sw1 = tmpBuf[tmpBufLen - 2];
	sw2 = tmpBuf[tmpBufLen - 1];

	if( (sw1 == 0x67) && (sw2 == 0x00) ){
		return RCS500_LCLE_ERROR;
	}
	if( (sw1 == 0x6A) && (sw2 == 0x86) ){
		return RCS500_P1P2_ERROR;
	}
	if( (sw1 == 0x6D) && (sw2 == 0x00) ){
		return RCS500_INS_ERROR;
	}
	if( (sw1 == 0x6E) && (sw2 == 0x00) ){
		return RCS500_CLA_ERROR;
	}

	// Remove SW1/SW2
	*samResLen = tmpBufLen-2;
	for(i=0; i<*samResLen; i++ ) {
		samResBuf[i] = tmpBuf[i];
	}

	if( (sw1 == 0x90) && (sw2 == 0x00) ){
		return SCARD_S_SUCCESS;
	}

	return UNKOWN_ERROR;
}
#endif

long TransmitDataToFeliCaCard(unsigned long felicaCmdLen, unsigned char felicaCmdBuf[], unsigned long* felicaResLen, unsigned char felicaResBuf[])
{
	long _ret = 0;
	unsigned char _send_buf[262];
	unsigned char receiveBuf[262];
	unsigned short _send_len, receiveLen;
	unsigned char tag, len;
	unsigned int i, j, k, l;
	extern USHORT usSecTimeOut;
	
	vdDebug_LogPrintf("======TransmitDataToFeliCaCard=====");

	#if 0
	// Wrap FeliCa Command with Transceive Exchange Command of PC/SC
	// See PC/SC Specification "Part 3. Supplemental Document 2 - Contactless ICCs"
	_send_buf[0] = 0xFF; // CLA
	_send_buf[1] = 0xC2; // INS
	_send_buf[2] = 0x00; // P1
	_send_buf[3] = 0x01; // P2
	if( felicaCmdLen < 128 )
	{
		_send_buf[4] = (unsigned char)felicaCmdLen + 3; // Lc
		_send_buf[5] = 0x95;			// Transceive Data Object
		_send_buf[6] = (unsigned char)felicaCmdLen + 1;	// Length
		k = 0;
	}else{
		_send_buf[4] = (unsigned char)felicaCmdLen + 4; // Lc
		_send_buf[5] = 0x95;			// Transceive Data Object
		_send_buf[6] = 0x81;	// Length
		_send_buf[7] = ((unsigned char)felicaCmdLen -128 + 1)|0x80;	// Length
		k = 1;
	}

	_send_buf[7+k] = (unsigned char)felicaCmdLen + 1;	// Length of FeliCa Command
	for(i=0; i<felicaCmdLen; i++ ) {
		_send_buf[8+i+k] = felicaCmdBuf[i];
	}

	_send_len = felicaCmdLen + 8 + k;
	#endif
	#if 1
	memcpy(_send_buf, felicaCmdBuf, felicaCmdLen);
	_send_len = felicaCmdLen;
	#endif
	receiveLen = 262;

#ifdef DEBUG
//	PrintHexArray("PC->CARD: ", _send_len, _send_buf);
#endif
#if 0
	_ret = SCardTransmit(hCardFeliCa,
		NULL,
		_send_buf,
		_send_len,
		NULL,  
		receiveBuf,
		&receiveLen);
#endif
	
	_ret = usFelicaTransmit(_send_buf,_send_len,receiveBuf,&receiveLen, usSecTimeOut);
	//_ret = CTOS_FelicaReadWrite( _send_buf, _send_len, receiveBuf, &receiveLen, usSecTimeOut);
	vdDebug_LogPrintf("CTOS_FelicaReadWrite ret[%04X] usSecTimeOut[%d]", _ret, usSecTimeOut);
	if(_ret != SCARD_S_SUCCESS){
		vdDebug_LogPrintf("SCardTransmit Error\n");
		return APP_ERROR;
	}

#ifdef DEBUG
//	PrintHexArray("CARD->PC: ", receiveLen, receiveBuf);
#endif

	// receive data of PC/SC Transceive Exchange contains at least 10 bytes
	//  C0 03 00 90 00 + Tag + Len + data + SW1 + SW2
	if(receiveLen < 10){
		//return APP_ERROR;
	}

	if ( memcmp (receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
		//return APP_ERROR;
	}

	memcpy(felicaResBuf, receiveBuf, receiveLen);
	*felicaResLen = receiveLen;
	#if 0
	j = 5;
	while(j < receiveLen) {
		l = 0;
		tag = receiveBuf[j];
		if (( receiveBuf[j+1] & 0x80) && ( receiveBuf[j+2] & 0x80) ){
			len = 128 + (receiveBuf[j+2] & 0x7F);
			l = 1;
		}else{
			len = receiveBuf[j+1];
		}
		if( tag == 0x97 ) {
			// in case of ICC Response
			for(i=0; i<(unsigned int)(len - 1); i++) {
				felicaResBuf[i] = receiveBuf[j + 3 + i + l];
			}
			*felicaResLen = len-1;
			break;
		} else {
			// skip data except ICC Response Object
			j += len + 2 + l;
		}
	}
	#endif
	
	vdDebug_LogPrintf("SCardTransmit APP_SUCCESS\n");

	return APP_SUCCESS;
}


BOOL cepas_Sub_CLCardInit(void)
{   
    UINT uiRtn;
        
#if 0
	vdDebug_LogPrintf(" == cepas_Sub_CLCardInit ==");
    
    uiRtn = CTOS_CLInitComm(38400);
    if(uiRtn != d_CL_OK)
    {
		vdDisplayErrorMsg(1, 8, "CLInitComm Err  ");
        return d_NO;
    }
#endif
    return d_OK;
}

long DetectFeliCaCard(void)
{
    USHORT ret;
    ULONG ulCardPresentTimeout;
	unsigned char ucKey = 0x00;

	BYTE baIDm[8];
	BYTE baPMm[8];

	long			_ret;

	unsigned long	readerLen, ATRLen, dwState;
	unsigned char	ATRVal[262];
	unsigned long	dwActProtocol;

	unsigned char	_send_buf[262], receiveBuf[262];
	unsigned short	_send_len, receiveLen;

	vdDebug_LogPrintf("\nTap FeliCa Card\n");
	vdDebug_LogPrintf("<Press ESC Key to cancel>\n");

	CTOS_LCDTClearDisplay();
	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	
	//CTOS_LCDTPrintXY(1, 1, "Tap FeliCa Card");  
	//CTOS_LCDTPrintXY(1, 2, "Cancel Key to exit");
	CTOS_LCDTPrintAligned(3, "Tap FeliCa Card", d_LCD_ALIGNCENTER);
	CTOS_LCDTPrintAligned(4, "Cancel Key to exit", d_LCD_ALIGNCENTER);

	memset(baIDm, 0x00, sizeof(baIDm));
	memset(baPMm, 0x00, sizeof(baPMm));
	
	//set timeout
    ulCardPresentTimeout = CTOS_TickGet() + 1000;
    
    while( CTOS_TickGet() < ulCardPresentTimeout )
    {
		ret = CTOS_FelicaPolling (baIDm, baPMm);
		if(ret == d_OK)
        {
        	vdDebug_LogPrintf("get CTOS_FelicaPolling");
			DebugAddHEX("baIDm", baIDm, 8);
			DebugAddHEX("baPMm", baPMm, 8);
            break;
        }

		/*check if user cancel*/
		CTOS_KBDInKey(&ucKey);
		if (ucKey)
		{
			CTOS_KBDGet(&ucKey);
			
			if (ucKey == d_KBD_CANCEL)
			{
				//vdSetErrorMessage("TXN CANCELLED");
        		return d_NO;
			}
		}
		
    }
    
    if( CTOS_TickGet() >= ulCardPresentTimeout )
    {
    	//vdDisplayErrorMsg(1, 8, "Time Out");
        return 0xDDDD;
    }

	#if 0
    if(isBeep)
    {
        CTOS_Sound(2000, 5);
    }
	#endif

	return d_OK;

	// Start Transparent Session of PC/SC on Pasori
	_send_buf[0] = 0xFF;
	_send_buf[1] = 0xC2;
	_send_buf[2] = 0x00;
	_send_buf[3] = 0x00;
	_send_buf[4] = 0x02;
	_send_buf[5] = 0x81;
	_send_buf[6] = 0x00;
	_send_len = 7;
	receiveLen = 7;

	#if 0
	_ret = SCardTransmit(hCardFeliCa,
			NULL,
			_send_buf,
			_send_len,
			NULL,  
			receiveBuf,
			&receiveLen);
	#endif

	usSecTimeOut = 60;
	_ret = usFelicaTransmit(_send_buf,_send_len,receiveBuf,&receiveLen, usSecTimeOut);

	if(_ret != SCARD_S_SUCCESS)
	{
		return APP_ERROR;
	}

	// Response of PC/SC Start Transparent Session with no error contains 7 bytes
	//  C0 03 00 90 00  90 00
	if(receiveLen != 7){
		return APP_ERROR;
		}
	
	if ( memcmp (receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
			return APP_ERROR;
		}
    
    return d_OK;
    
}

#if 0
USHORT cepas_Sub_CLCardDetect( IN BOOL isBeep )
{
    USHORT ret;
    BYTE baATQA1[20], bSAK1[20], baTCSN1[20];
    BYTE bTCSNLen1;
    BYTE bAutoBR = 0;
    BYTE baATS[30];
    USHORT bATSLen;
    BYTE baPUPI[256];
    ULONG ulCardPresentTimeout;

	unsigned char ucKey = 0x00;
    
    BYTE baRxData[256];
    BYTE bRxLen;
    BYTE bCMD;

	//return usDetectFelica(1);

    ulCardPresentTimeout = CTOS_TickGet() + ( 5000 /10 );
    
    while( CTOS_TickGet() < ulCardPresentTimeout )
    {
        bTCSNLen1 = sizeof(baTCSN1);
        ret = CTOS_CLTypeAActiveFromIdle(0, baATQA1, bSAK1, baTCSN1, &bTCSNLen1);
        if(ret == d_OK)
        {
            ret = CTOS_CLRATS(bAutoBR,baATS,&bATSLen);
            if(ret != d_OK)
            {
                return ret;
            }
			vdDebug_LogPrintf("TypeA.....");
            break;
        }

        ret = CTOS_CLTypeBActive( baPUPI );
        if(ret == d_OK)
        {
        	vdDebug_LogPrintf("TypeB.....");
            break;
        }

		/*check if user cancel*/
		CTOS_KBDInKey(&ucKey);
		if (ucKey)
		{
			CTOS_KBDGet(&ucKey);
			
			if (ucKey == d_KBD_CANCEL)
			{
				vdSetErrorMessage("TXN CANCELLED");
        		return d_NO;
			}
		}
		
    }
    
    if( CTOS_TickGet() >= ulCardPresentTimeout )
    {
    	vdDisplayErrorMsg(1, 8, "Card Error -5001");
        return 0xDDDD;
    }
    
    if(isBeep)
    {
        CTOS_Sound( 2000, 5 );
    }
    
    return d_OK;
    
}
#endif

USHORT usFelicaTransmit( IN BYTE *baTxBuf, IN USHORT usTxLen, OUT BYTE *baRxBuf, USHORT *usRxLen, USHORT usTimeOut)
{
    USHORT ret;

	BYTE baSendBuf[512];
	USHORT usSendLen = 0;
	BYTE baRecvBuf[512];
	USHORT usRecvLen = 0;
	
	int i;
	//extern USHORT usSecTimeOut;

    DebugAddHEX("RAW CMD", baTxBuf, usTxLen);

	memset(baSendBuf, 0x00, sizeof(baSendBuf));
	memset(baRecvBuf, 0x00, sizeof(baRecvBuf));
	//set command packet
	// ulTimeout is in T=256x16/fc basis (0.302ms)
	// TxBuf : Length + Payload
	// RxBuf : Length + Payload

	memset(baSendBuf, 0x00, sizeof(baSendBuf));
	usSendLen = usTxLen + 1;
	baSendBuf[0] = usSendLen;
	memcpy(&baSendBuf[1], baTxBuf, usTxLen);

	usRecvLen = *usRxLen+1;
	
	DebugAddHEX("CTOS Felica Send", baSendBuf, usSendLen);
    vdDebug_LogPrintf("EXP RESP LEN=[%d] usTimeOut[%d]", usRecvLen, usTimeOut);
    ret = CTOS_FelicaReadWrite( baSendBuf, baSendBuf[0], baRecvBuf, &usRecvLen, usTimeOut);
	vdDebug_LogPrintf("CTOS_FelicaReadWrite ret[%04X]", ret);
    if( ret != d_OK )
    {
        return ret;
    }
    DebugAddHEX("CTOS Felica Recv", baRecvBuf, usRecvLen);
	
	memcpy(baRxBuf, &baRecvBuf[1], usRecvLen-1);
	*usRxLen = usRecvLen-1;
	
    if( *usRxLen < 2 ) // 2 = SW12
    {
        return d_NO;
    }
    
    return d_OK;
}


