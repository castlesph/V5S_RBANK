/*=========================================*       
 *             I N C L U D E S             *     
 *=========================================*/ 
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "../Includes/POSTypedef.h"
#include "../Includes/myEZLib.h"
#include "../Includes/wub_lib.h"


#include "../FileModule/myFileFunc.h"
#include "../DataBase/DataBaseFunc.h"

#include "../Debug/Debug.h"
#include "../Includes/TLESec.h"

#include "../Includes/POSTypedef.h"
#include "../Includes/V5IsoFunc.h"


//BYTE   szToDescryptPacket[ISO_REC_SIZE + 1];
//int    inToDescryptPacketLen;

int inEncryptTLESec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult)
{
	BYTE   szDataKey[16];
	BYTE   szMACKey[16];
	BYTE   szMasterKey[16];
	BYTE   szEncryptedKey[16];
	BYTE   szOutKey[16];
	BYTE   szPacketASC[ISO_REC_SIZE + 1];
	BYTE   szISOPacket[ISO_REC_SIZE + 1];
	BYTE   szEncISOPacket[ISO_REC_SIZE + 1];
	BYTE   szEncryptedPacket[ISO_REC_SIZE + 1];
	BYTE   szDataBuf[255];
	BYTE   szPadResult[8 + 1];
	BYTE   szTID[TERMINAL_ID_BYTES+1];
	BYTE   szTIDPadded[32 + 1];
	BYTE   szTIDPadded1[32 + 1];
	BYTE   szTIDPadded2[32 + 1];
	BYTE   szPart[16 + 1];
	BYTE   szDivisible[16 + 1];
	int    inDataCnt;
	int    inPaddedLen = 0;
	BYTE   szMAC[8];
	BYTE   szTPDU[5 + 1];
	BYTE   szDesData[16+1];
	BYTE   szOutEncData[16+1];
	int    inIndex;
	int    inLen;
	int    i = 0;
	int    usCnt = 0;
	int    usIndex = 0;
	int    inISOMACLen = 0;
	BYTE   szISOMACLen[2 + 1];
	BYTE   szASCLen[2 + 1];
	BYTE   szEncData[ISO_REC_SIZE+1];
	char   szAscBuf[4 + 1], szBcdBuf[2 + 1];
	BYTE   szTIDDummy[TERMINAL_ID_BYTES+1];
	BYTE   szDefaultPadded[TERMINAL_ID_BYTES+1];
	BYTE   szData[ISO_REC_SIZE+1];
	BYTE   szDataForCompute[ISO_REC_SIZE + 1];
	int inMACSize = 8;
	int inTPDUSize = 5;

	vdDebug_LogPrintf("--inEncryptTLESec--");
	vdDebug_LogPrintf("strHDT.inHostIndex=[%d], strMMT[0].HDTid=[%d]", strHDT.inHostIndex, strMMT[0].HDTid);
	inTLESecRead(strHDT.inHostIndex);

	vdDebug_LogPrintf("--TLESEC READ TABLE--");
	vdDebug_LogPrintf("inHostindex=[%d]", strTLESEC.inHostindex);
	vdDebug_LogPrintf("szHostLabel=[%s]", strTLESEC.szHostLabel);
	DebugAddHEX("szTPDU", strTLESEC.szTPDU, 5);
	DebugAddHEX("szNII", strTLESEC.szNII, 2);
	DebugAddHEX("szTID", strTLESEC.szTID, 8);
	
	//inLen = 87;
	//inMsgLen = inLen;
	//memcpy(szMsg, "\x60\x12\x34\x67\x89\x04\x00\x20\x20\x01\x00\x00\xC0\x10\x03\x93\x00\x00\x00\x00\x01\x05\x55\x34\x30\x30\x30\x30\x30\x30\x31\x34\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x6D\xA6\xDF\xCC\xAE\xCD\xB0\xCA\x00\x31\x33\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53", inLen);

	vdDebug_LogPrintf("Original Size=[%d]", inMsgLen);	
	DebugAddHEX("*szMsg", szMsg, inMsgLen);
	
	inLen=inMsgLen-5; /*subtract 5 for TPDU*/
    memcpy(szData, szMsg+5, inLen); /*copy data exclude TPDU*/

	vdDebug_LogPrintf("Remove TPDU Size=[%d]", inLen);
	DebugAddHEX("szData", szData, inLen); // without TPDU

	// -----------------------------------------------------------------------
	// Get TPDU
	// -----------------------------------------------------------------------
	memset(szTPDU, 0x00, sizeof(szTPDU));	
	memcpy(szTPDU, strTLESEC.szTPDU, 5 + 1);
	//memcpy(szTPDU, szMsg, 5);
	DebugAddHEX("AAA >> szTPDU", szTPDU, 5);
	
	//memset(szDataBuf, 0x00, sizeof(szDataBuf));
	
	//memset(szMACKey, 0x00, sizeof(szMACKey));
	//memcpy(szMACKey, "\x18\x29\x82\x3D\xB3\xFD\xF2\x4A\xDC\x1E\x07\xCF\xF2\x8E\x2C\xB8", 16);

	//memset(szPacketASC, 0x00, sizeof(szPacketASC));
	//memcpy(szPacketASC, "\x04\x00\x20\x20\x01\x00\x00\xC0\x10\x03\x93\x00\x00\x00\x00\x01\x05\x55\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x6D\xA6\xDF\xCC\xAE\xCD\xB0\xCA\x00\x31\x33\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53\x80\x00\x00\x00\x00\x00", 88);
	//memcpy(szPacketASC, "\x04\x00\x20\x20\x01\x00\x00\xC0\x10\x03\x93\x00\x00\x00\x00\x01\x05\x55\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x6D\xA6\xDF\xCC\xAE\xCD\xB0\xCA\x00\x31\x33\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53", 82);
	//iDES_CBC(d_ENCRYPTION, szPacketASC, 88, szDataBuf, szMACKey); 

	//memset(szMasterKey, 0x00, sizeof(szMasterKey));
	//memcpy(szMasterKey, "\x11\x11\x11\x11\x22\x22\x22\x22\x33\x33\x33\x33\x44\x44\x44\x44", 16);
	
	//memset(szSessionKey, 0x00, sizeof(szSessionKey));
	//memcpy(szSessionKey, "\x31\x32\x33\x34\x35\x36\x37\x38\x99\x88\x00\x00\x00\x00\x00\x00", 16);

	//memset(szTIDDummy, 0x00, sizeof(szTIDDummy));
	//memcpy(szTIDDummy, "\x99\x88\x00\x00\x00\x00\x00\x00", TERMINAL_ID_BYTES);	
	//memset(szTIDPadded1, 0x00, sizeof(szTIDPadded1));

	// -----------------------------------------------------------------------
	// Get TID
	// -----------------------------------------------------------------------
	memset(szTID, 0x00, sizeof(szTID));
	//inMMTReadRecord(MCC_HOST,1);
	//strcpy(srTransRec.szTID, strMMT[0].szTID);	
	memcpy(szTID, srTransRec.szTID, TERMINAL_ID_BYTES);

	// Default Pad
	memset(szDefaultPadded, 0x00, sizeof(szDefaultPadded));
	//memcpy(szDefaultPadded, "\x99\x88\x00\x00\x00\x00\x00\x00", TERMINAL_ID_BYTES);
	memcpy(szDefaultPadded, strTLESEC.szTID, TERMINAL_ID_BYTES);
	
	// -----------------------------------------------------------------------
	// Get Data Key
	// -----------------------------------------------------------------------	
	memset(szDataKey, 0x00, sizeof(szDataKey));
	inGetDataKey(szTID, szDefaultPadded, szDataKey);

	// -----------------------------------------------------------------------
	// Get MAC Key
	// -----------------------------------------------------------------------
	memset(szMACKey, 0x00, sizeof(szMACKey));
	inGetMACKey(szTID, szDefaultPadded, szMACKey);

	// -----------------------------------------------------------------------
	// Get TID Padded
	// -----------------------------------------------------------------------
	memset(szTIDPadded, 0x00, sizeof(szTIDPadded));
	memcpy((char *)&szTIDPadded[0], szDefaultPadded, TERMINAL_ID_BYTES);
	memcpy((char *)&szTIDPadded[8], szTID, TERMINAL_ID_BYTES);
	DebugAddHEX("szTIDPadded:", szTIDPadded, 16);
	
	// -----------------------------------------------------------------------
	// Get Multiple By 8
	// -----------------------------------------------------------------------
	memset(szPadResult, 0x00, sizeof(szPadResult));
	inPaddedLen = inGetMultiplePadded(inLen, szPadResult);
	vdDebug_LogPrintf("szPadResult 1, inPaddedLen[%d]", inPaddedLen);
	DebugAddHEX("szPadResult 1:", szPadResult, inPaddedLen);

	// -----------------------------------------------------------------------
	// Get MAC
	// -----------------------------------------------------------------------
	vdDebug_LogPrintf("Before Get MAC, inLen=[%d], inPaddedLen[%d]", inLen, inPaddedLen);
	memset(szMAC, 0x00, sizeof(szMAC));
	memset(szDataForCompute, 0x00, sizeof(szDataForCompute));
	memcpy(szDataForCompute, szData, inLen);
	memcpy(&szDataForCompute[inLen], szPadResult, inPaddedLen);
	inGetMAC(szDataForCompute, inLen+inPaddedLen, szMACKey, szMAC);

	// ----------------------------------------------------------------------
	// Original ISO with MAC for Encryption
	// ----------------------------------------------------------------------	
	//inLen = 0;
	//inPaddedLen = 0;
	//memset(szISOPacket, 0x00, sizeof(szISOPacket));
	//memset(szPacketASC, 0x00, sizeof(szPacketASC));
	//memset(szDataForCompute, 0x00, sizeof(szDataForCompute));
	//memcpy(szDataForCompute, szMsg, inMsgLen); inLen+=inMsgLen;
	//memcpy(szDataForCompute, "\x04\x00\x20\x20\x01\x00\x00\xC0\x10\x03\x93\x00\x00\x00\x00\x01\x05\x55\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x6D\xA6\xDF\xCC\xAE\xCD\xB0\xCA\x00\x31\x33\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53\x3A\x50\x50\x30\x30\x2E\x30\x31\x4D\x53\x41\x4E\x44\x45\x53", 82);

	//memcpy((char *)&szISOPacket[inLen], szPacketASC, 82); inLen+=82;
	//memcpy((char *)&szISOPacket[inLen], szMAC, 8); inLen+=8;

	memset(szPadResult, 0x00, sizeof(szPadResult));
	memset(szISOPacket, 0x00, sizeof(szISOPacket));
	//inPaddedLen = inGetMultiplePadded(inLen+inPaddedLen, szPadResult);
	inPaddedLen = inGetMultiplePadded(inLen+inTPDUSize+inMACSize, szPadResult);
	vdDebug_LogPrintf("szPadResult 2, inPaddedLen[%d]", inPaddedLen);
	DebugAddHEX("szPadResult 2:", szPadResult, inPaddedLen);

	inLen = 0;
	if (inPaddedLen > 0)
	{
		memcpy(szISOPacket, szMsg, inMsgLen);inLen+=inMsgLen;
		memcpy(&szISOPacket[inLen], szMAC, 8);inLen+=8;
		memcpy(&szISOPacket[inLen], szPadResult, inPaddedLen);inLen+=inPaddedLen;
	}
	else
	{
		memcpy(szISOPacket, szMsg, inMsgLen);inLen+=inMsgLen;
		memcpy(&szISOPacket[inLen], szMAC, 8);inLen+=8;
	}

	inISOMACLen = inLen - inPaddedLen;
	vdDebug_LogPrintf("inLen=[%d], inISOMACLen=[%d]", inLen, inISOMACLen);
	DebugAddHEX("szISOPacket:", szISOPacket, inLen);

	memset(szISOMACLen, 0x00, sizeof(szISOMACLen));
	inGetHexLength(inISOMACLen, szISOMACLen);
	DebugAddHEX("szISOMACLen:", szISOMACLen, 2);
	
	// ----------------------------------------------------------------------
	// Original ISO with MAC + Pad (Encrypt)
	// ----------------------------------------------------------------------	
	for(usIndex=0; usIndex<((inLen)/8); usIndex++)
	{
		memset(szDesData, 0, sizeof(szDesData));
		memcpy(szDesData, szISOPacket+i, 8);
		DebugAddHEX("szDesData", szDesData, 8);
		
		memset(szOutEncData, 0x00, sizeof(szOutEncData));
		Encrypt3Des(szDesData, szDataKey, szOutEncData);
		memcpy(&szEncryptedPacket[i], szOutEncData, 8);
		
		i+=8;
	}

	DebugAddHEX("Encrypt Data:", szEncryptedPacket, inLen);
	//memset(szToDescryptPacket, 0x00, sizeof(szToDescryptPacket));
	//inToDescryptPacketLen = inLen;
	//memcpy(szToDescryptPacket, szEncryptedPacket, inToDescryptPacketLen);
	
	// ----------------------------------------------------------------------
	// Encrypted ISO sent from terminal with two bytes header len in hex
	// ----------------------------------------------------------------------
	DebugAddHEX("AAA szTPDU", szTPDU, 5);
	DebugAddHEX("AAA strTLESEC.szTPDU", strTLESEC.szTPDU, 5);
	i = 0;
	memset(szEncISOPacket, 0x00, sizeof(szEncISOPacket));
	//memcpy(szEncISOPacket, szTPDU, 5); i+=5;
	memcpy(szEncISOPacket, strTLESEC.szTPDU, 5 + 1); i+=5;
	memcpy(&szEncISOPacket[i], szTIDPadded, 16); i+=16;
	memcpy(&szEncISOPacket[i], szISOMACLen, 2); i+=2;
	memcpy(&szEncISOPacket[i], szEncryptedPacket, inLen); i+=inLen;

	memcpy(ptrResult, szEncISOPacket, i);
	DebugAddHEX("Send szEncISOPacket:", szEncISOPacket, i);
	vdDebug_LogPrintf("szEncISOPacket, inLen[%d]", i);

	inLen = i;
	
	return inLen;
}


int inDecryptTLESec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult)
{
	BYTE szEncISOData[ISO_REC_SIZE + 1] = {0};
	BYTE szPart1[32 + 1] = {0};
	//BYTE szDataKey[32 + 1] = {0};
	BYTE szEncData[32 + 1] = {0};
	BYTE szClearPacket [ISO_REC_SIZE + 1] = {0};
	BYTE szDesData[32 + 1] = {0};
	BYTE szOutClearData[32 + 1] = {0};
	int usIndex = 0;
	int usCnt = 0;
	int inLen = 0;
	int i = 0;
	int x = 0;
	BYTE szTPDU[10];
	BYTE szData[ISO_REC_SIZE+1];
	BYTE szEncryptedRandomKey[16+1];
	int iCount = 0;
	
	vdDebug_LogPrintf("--inDecryptTLESec--");

	memset(szEncISOData, 0x00, sizeof(szEncISOData));
	memset(szPart1, 0x00, sizeof(szPart1));
	memset(szData, 0x00, sizeof(szData));
	
	//memset(szDataKey, 0x00, sizeof(szDataKey));

	DebugAddHEX("Encrypted Data:", szMsg, inMsgLen);
	
	inLen = inMsgLen;
	vdDebug_LogPrintf("inLen=[%d]", inLen);

	//memcpy(szEncISOData, szMsg, inLen);
	
	//inLen = inToDescryptPacketLen;
	//memcpy(szEncISOData, szToDescryptPacket, inLen);
	//memcpy(szDataKey, "\xDC\x1E\x07\xCF\xF2\x8E\x2C\xB8\x18\x29\x82\x3D\xB3\xFD\xF2\x4A", 16);

	//vdDebug_LogPrintf("Original Size=[%d]", inMsgLen);	
	//DebugAddHEX("*szMsg", szMsg, inMsgLen);
	
	//inLen=inMsgLen-5; /*subtract 5 for TPDU*/
    //memcpy(szData, szMsg+5, inLen); /*copy data exclude TPDU*/

	//vdDebug_LogPrintf("Remove TPDU Size=[%d]", inLen);
	//DebugAddHEX("szData", szData, inLen); // without TPDU

	//memcpy(szEncISOData, szData, inLen);

	//memcpy(szEncISOData, szMsg, inMsgLen);
	
	// -----------------------------------------------------------------------
	// Get TPDU
	// -----------------------------------------------------------------------
	//memset(szTPDU, 0x00, sizeof(szTPDU));
	//memcpy(szTPDU, szMsg, 5);
	//DebugAddHEX("szTPDU", szTPDU, 5);

	inLen = inLen - 23;
	memcpy(szEncISOData, szMsg, inLen);
	memcpy(szEncISOData, &szMsg[23], inLen);

	DebugAddHEX("Data Key:", strTLESEC.szDataKey, 16);
	DebugAddHEX("Receive Encrypted Data:", szEncISOData, inLen);
	
	for(usIndex=0; usIndex<((inLen)/8); usIndex++)
	{
		memset(szDesData, 0, sizeof(szDesData));
		memcpy(szDesData, szEncISOData+i, 8);
		DebugAddHEX("szDesData", szDesData, 8);
		
		memset(szOutClearData, 0x00, sizeof(szOutClearData));
		Decrypt3Des(szDesData, strTLESEC.szDataKey, szOutClearData);

		DebugAddHEX("Out Clear Data ->>", szOutClearData, 8);
		
		memcpy(&szClearPacket[i], szOutClearData, 8);
		
		i+=8;
	}

	DebugAddHEX("Clear Data:", szClearPacket, inLen);

	// Remove pad (80 00 ....)
	vdDebug_LogPrintf("Remove pad (80 00 ...)");
	x = inLen;
	for (i = 0; i < 8; i++) // TLE sample message padded in 8 bytes
	{
		x--;
		
		if (memcmp(&szClearPacket[x], "\x00", 1) == 0 || memcmp(&szClearPacket[x], "\x80", 1) == 0)
		{
			iCount++;
		}

		if (memcmp(&szClearPacket[x], "\x80", 1) == 0)
		{
			break;
		}
	
	}
	
	vdDebug_LogPrintf("iCount=[%d]", iCount);

	if (iCount > 0)
		inLen-=iCount;
	
	memcpy(ptrResult, szClearPacket, inLen);
	
	return inLen;
}

int inEncryptTLE(BYTE *szMsg, int inMsgLen, BYTE *ptrResult)
{
    BYTE szEncData[ISO_REC_SIZE+1], szData[ISO_REC_SIZE+1], szDesData[16+1];
    BYTE szRandomKey[16+1], szLen[2+1];
    int inLen=0, inIndex=0, inCnt=0, inPad;
    int inKeySet=TLESEC_KEY_SET, inKeyIndex=TLESEC_KEY_INDEX;
    CTOS_KMS2DATAENCRYPT_PARA para;
    BYTE plaindata[16+1], szEncryptedRandomKey[16+1];
    USHORT ret;
	BYTE iv[100];
	BYTE cipherdata[100];

	vdDebug_LogPrintf("--inEncryptTLE--");
	
	memset(szEncData, 0, sizeof(szEncData));
	memset(szData, 0, sizeof(szData));
    memset(szRandomKey, 0, sizeof(szRandomKey));
	memset(szLen, 0, sizeof(szLen));
	
    DebugAddHEX("*szMsg", szMsg, inMsgLen);
		
   	memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = inKeySet;
	para.Protection.CipherKeyIndex = inKeyIndex;
	para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_ECB;
	para.Protection.SK_Length = 0;

	memset(plaindata, 0x00, sizeof(plaindata));
    memset(szEncryptedRandomKey, 0x00, sizeof(szEncryptedRandomKey));
    memcpy(plaindata, szMsg, 16);
	para.Input.Length = 16;
	para.Input.pData = plaindata;
	para.Output.pData = szEncryptedRandomKey;

	ret = CTOS_KMS2DataEncrypt(&para);

	vdDebug_LogPrintf("CTOS_KMS2DataEncrypt, ret=[%d]", ret);
	
    if(ret != d_OK)
        return ret;

	
    DebugAddHEX("inEncryptTLE-------1",szEncryptedRandomKey, 16);
	memcpy(ptrResult, szEncryptedRandomKey, 16);
	
	return d_OK;
}

void iDES_CBC(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *DESResult, BYTE *szMKey)
{
    USHORT       usIndex ;
    USHORT       usCnt ;
    BYTE         temp[17], strDatakey[17];
    BYTE         szDesData[17];
	BYTE szEncData[ISO_REC_SIZE+1];
	BYTE szOutData[32 + 1] = {0};
	int i = 0;
	int iLoop = 0;

	vdDebug_LogPrintf("--iDES_CBC--");
	
	vdDebug_LogPrintf("inMsgLen=[%d]", inMsgLen);
	DebugAddHEX("szMsg:", szMsg, inMsgLen);
	DebugAddHEX("szMKey:", szMKey, 16);

	memset(szEncData, 0x00, sizeof(szEncData));
	memset(DESResult, 0x00, sizeof(DESResult));
	
    memcpy(strDatakey, szMKey,16 );
    for(usIndex = 0; usIndex < inMsgLen; usIndex = usIndex + 8)
    {
        for(usCnt = 0; usCnt < 8; usCnt++)
        {
            if(usIndex == 0)
                szDesData[usCnt] = szMsg[usIndex + usCnt] ^ 0x00;
            else
                szDesData[usCnt] = szMsg[usIndex + usCnt] ^ szEncData[(usIndex - 8) + usCnt];

			//DebugAddHEX("11szDesData:", &szDesData[usCnt], 8);
        }

		//DebugAddHEX("22szDesData:", szDesData, 8);
		
        Encrypt3Des(szDesData, strDatakey, &szEncData[usIndex]);

		//vdDebug_LogPrintf("usIndex=[%d]", usIndex);
		//DebugAddHEX("iDES_CBC Result:", &szEncData[usIndex], 8);	

		memcpy(DESResult, &szEncData[usIndex], 8);
		vdDebug_LogPrintf("iLoop=[%d]", iLoop);
    }

	/*
	vdDebug_LogPrintf("Data By 8 bytes...");
	for(usIndex=0; usIndex<((inMsgLen)/8); usIndex++)
	{
		memset(szDesData, 0, sizeof(szDesData));
		memcpy(szDesData, szMsg+i, 8);
		DebugAddHEX("szDesData", szDesData, 8);
		
		memset(szOutData, 0x00, sizeof(szOutData));
		Encrypt3Des(szDesData, strDatakey, szOutData);

		DebugAddHEX("Out Data ->>", szOutData, 8);
				
		i+=8;
	}

	DebugAddHEX("Last szOutData:", szOutData, 8);
	*/
	
	DebugAddHEX("iDES_CBC::DESResult", DESResult, 8);
	
}

void iDES_ECB(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *DESResult, BYTE *szMKey)
{
    USHORT       usIndex ;
    USHORT       usCnt ;
    BYTE         temp[17], strDatakey[17];
    BYTE         szDesData[17];
	BYTE szEncData[ISO_REC_SIZE+1];

	vdDebug_LogPrintf("--iDES_CBC--");

	vdDebug_LogPrintf("inMsgLen=[%d]", inMsgLen);
	DebugAddHEX("szMsg:", szMsg, inMsgLen);
	DebugAddHEX("szMKey:", szMKey, 16);

	memset(szEncData, 0x00, sizeof(szEncData));
	memset(DESResult, 0x00, sizeof(DESResult));
	
    memcpy(strDatakey, szMKey,16 );	
    for(usIndex = 0; usIndex < inMsgLen; usIndex = usIndex + 8)
    {
        for(usCnt = 0; usCnt < 8; usCnt++)
        {
            if(usIndex == 0)
                szDesData[usCnt] = szMsg[usIndex + usCnt] ^ 0x00;
            else
                szDesData[usCnt] = szMsg[usIndex + usCnt] ^ szEncData[(usIndex - 8) + usCnt];
        }

		DebugAddHEX("szDesData:", szDesData, 8);
		
        Decrypt3Des(szDesData, strDatakey, &szEncData[usIndex]);

		vdDebug_LogPrintf("usIndex=[%d]", usIndex);
		DebugAddHEX("iDES_ECB Result:", &szEncData[usIndex], 8);

		memcpy(DESResult, &szEncData[usIndex], 8);
    }	

	DebugAddHEX("iDES_ECB::DESResult", DESResult, 8);
}

int inGetMultiplePadded(int inMsgLen, BYTE *ptrResult)
{
	int inLen = 0;
	int i = 0;
	BYTE szPad[8 + 1] = {0};
	int iMod = 0;

	vdDebug_LogPrintf("--inGetMultiplePadded--");
	vdDebug_LogPrintf("inMsgLen=[%d]", inMsgLen);
	
	memset(szPad, 0x00, sizeof(szPad));
	iMod = inMsgLen % 8;

	/*
	inLen = 8 - iMod;		
	if (inLen > 0)
	{
		do
		{
			szPad[i] = 0x00;
			i++;
		}
		while (i < inLen);

		szPad[0] = 0x80;
		memcpy(ptrResult, szPad, inLen);
	}
	*/	
	
	if (iMod > 0)
	{
		inLen = 8 - iMod;
		
		if (inLen > 0)
		{
			do
			{
				szPad[i] = 0x00;
				i++;
			}
			while (i < inLen);

			szPad[0] = 0x80;
			memcpy(ptrResult, szPad, inLen);
		}
	}
	else
	{
		memset(ptrResult, 0x00, sizeof(ptrResult));
		inLen = 0;
	}
	

	vdDebug_LogPrintf("inLen=[%d], iMod=[%d]", inLen, iMod);	
	DebugAddHEX("ptrResult:", ptrResult, inLen);

	return inLen;
}

int inGetHexLength(int inLen, BYTE *ptrResult)
{
	BYTE   szHexLen[2 + 1];

	vdDebug_LogPrintf("--inGetHexLength--");
	
	memset(szHexLen, 0x00, sizeof(szHexLen));
	memset(ptrResult, 0x00, sizeof(ptrResult));
	
	szHexLen[0] = ((inLen & 0x0000FF00) >> 8);
	szHexLen[1] = (inLen & 0x000000FF);
	
	DebugAddHEX("szHexLen:", szHexLen, 2);
	memcpy(ptrResult, szHexLen, 2);
	
	return d_OK;
}

int inGetDataKey(BYTE *szTID, BYTE *szDefaultPadded, BYTE *ptrResult)
{
	BYTE szKey[16];
	BYTE szTIDPadded[32 + 1];

	vdDebug_LogPrintf("--inGetDataKey--");
	
	memset(szKey, 0x00, sizeof(szKey));
	memset(szTIDPadded, 0x00, sizeof(szTIDPadded));
	memcpy((char *)&szTIDPadded[0], szTID, TERMINAL_ID_BYTES);
	memcpy((char *)&szTIDPadded[8], szDefaultPadded, TERMINAL_ID_BYTES);
	DebugAddHEX("szTIDPadded:", szTIDPadded, 16);
	inEncryptTLE(szTIDPadded, 16, szKey);

	memset(ptrResult, 0x00, sizeof(ptrResult));
	memcpy(ptrResult, szKey, 16);
	
	DebugAddHEX("Data Key:", szKey, 16);

	memset(strTLESEC.szDataKey, 0x00, sizeof(strTLESEC.szDataKey));
	memcpy(strTLESEC.szDataKey, szKey, 16);
	
	
	return d_OK;
}

int inGetMACKey(BYTE *szTID, BYTE *szDefaultPadded, BYTE *ptrResult)
{
	BYTE szKey[16];
	BYTE szTIDPadded[32 + 1];

	vdDebug_LogPrintf("--inGetMACKey--");
	
	memset(szKey, 0x00, sizeof(szKey));
	memset(szTIDPadded, 0x00, sizeof(szTIDPadded));
	memcpy((char *)&szTIDPadded[0], szDefaultPadded, TERMINAL_ID_BYTES);
	memcpy((char *)&szTIDPadded[8], szTID, TERMINAL_ID_BYTES);
	DebugAddHEX("szTIDPadded:", szTIDPadded, 16);
	inEncryptTLE(szTIDPadded, 16, szKey);

	memset(ptrResult, 0x00, sizeof(ptrResult));
	memcpy(ptrResult, szKey, 16);
	
	DebugAddHEX("MAC Key:", szKey, 16);

	memset(strTLESEC.szMACKey, 0x00, sizeof(strTLESEC.szMACKey));
	memcpy(strTLESEC.szMACKey, szKey, 16);
}

int inGetMAC(BYTE *szMsg, int inMsgLen, BYTE *szKey, BYTE *ptrResult)
{
	BYTE szMAC[8];
	
	vdDebug_LogPrintf("--inGetMAC--");

	vdDebug_LogPrintf("inMsgLen=[%d]", inMsgLen);
	DebugAddHEX("szMsg:", szMsg, inMsgLen);
	
	memset(szMAC, 0x00, sizeof(szMAC));
	iDES_CBC(d_ENCRYPTION, szMsg, inMsgLen, szMAC, szKey);

	memset(ptrResult, 0x00, sizeof(ptrResult));
	memcpy(ptrResult, szMAC, 8);

	DebugAddHEX("MAC:", szMAC, 8);
	
	return d_OK;
}

int inCheckTLESecKey(void)
{
    int inRet=0;

	vdDebug_LogPrintf("--inCheckTLESecKey--");
	vdDebug_LogPrintf("fTLESec[%d]", strTCT.fTLESec);
	
	if(strTCT.fTLESec != TRUE)
		return d_OK;

    inRet = CTOS_KMS2KeyCheck(TLESEC_KEY_SET, TLESEC_KEY_INDEX);
    if(inRet != d_OK)
    {
    	CTOS_LCDTClearDisplay();
        vdDisplayErrorMsg2(1, 8, "PLEASE INJECT", "TLE KEY (ENCRYPT)", MSG_TYPE_ERROR);
        return(inRet);

    }
	
    return d_OK;
}

int inCheckTLEResponseCode(BYTE *szMsg, int inMsgLen)
{
	int inRet = d_OK;
	BYTE szRespCode[RESP_CODE_SIZE+1];
	char szErrMsg[40+1];
	char szErrorDisplay[40+1];
	
	vdDebug_LogPrintf("--inCheckTLEResponseCode--");

	vdDebug_LogPrintf("inMsgLen=[%d]", inMsgLen);
	DebugAddHEX("szMsg:", szMsg, inMsgLen);

	memset(szRespCode, 0x00, sizeof(szRespCode));
	memset(szErrorDisplay, 0x00, sizeof(szErrorDisplay));
	memcpy(szRespCode, (char *)&szMsg[21], RESP_CODE_SIZE);
	vdDebug_LogPrintf("Length(%d), szRespCode(%s)", strlen(szRespCode), szRespCode);
	
	if (memcmp(szRespCode, "98", 2) == 0)
	{
		vdSetErrorMessage("TLE INVALID MAC");
		inRet = d_NO;
	}

	else if (memcmp(szRespCode, "97", 2) == 0)
	{
		vdSetErrorMessage("TLE DECRYPTION ERROR");
		inRet = d_NO;
	}

	else if (memcmp(szRespCode, "74", 2) == 0)
	{
		vdSetErrorMessage("TLE CONNECT FAILED");
		inRet = d_NO;
	}
	else
	{
		vdSetErrorMessage("");
	}
	
	memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
    	CTOS_LCDTClearDisplay();
        vdDisplayErrorMsg(1, 8, szErrMsg);
    }

	vdSetErrorMessage("");
	
	return inRet;
}

