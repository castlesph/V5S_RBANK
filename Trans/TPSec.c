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
#include "../Includes/TPSec.h"

#if 0
void vdTripleDES_CBC(BYTE Mode, BYTE *szMsg, BYTE szMsgLen, BYTE *DESResult)
{
    USHORT       usIndex ;
    USHORT       usCnt ;
    BYTE         temp[17], strDatakey[17];
    BYTE         szDesData[17];

    
    inTLERead(1);
    
    vdMyEZLib_LogPrintff(szMsg, szMsgLen);  
        
    memcpy(strDatakey, stTLE.szLineEncryptKey,16 );
    for(usIndex = 0; usIndex < szMsgLen; usIndex = usIndex + 8)
    {
        for(usCnt = 0; usCnt < 8; usCnt++)
        {
            if(usIndex == 0)
                szDesData[usCnt] = szMsg[usIndex + usCnt] ^ 0x00;
            else
                szDesData[usCnt] = szMsg[usIndex + usCnt] ^ DESResult[(usIndex - 8) + usCnt];
        }

        Encrypt3Des(szDesData, strDatakey, &DESResult[usIndex]);
    }
}
#endif

//#define TEST_KEYS

int inEncryptTPSec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult)
{
    BYTE szEncData[512+1], szData[512+1], szDesData[16+1];
    BYTE szRandomKey[16+1], szLen[2+1];
    int inLen=0, inIndex=0, inCnt=0, inPad;
    int inKeySet=TPSEC_KEY_SET, inKeyIndex=TPSEC_KEY_INDEX;
    CTOS_KMS2DATAENCRYPT_PARA para;
    BYTE plaindata[16+1], szEncryptedRandomKey[16+1];
    USHORT ret;

	memset(szEncData, 0, sizeof(szEncData));
	memset(szData, 0, sizeof(szData));
    memset(szRandomKey, 0, sizeof(szRandomKey));
	memset(szLen, 0, sizeof(szLen));

    DebugAddHEX("*szMsg", szMsg, inMsgLen);
	
	inLen=inMsgLen-5; /*subtract 5 for TPDU*/
    memcpy(szData, szMsg+5, inLen); /*copy data exclude TPDU*/

	vdGenerateRandomKey(15, szRandomKey+1); 

    inPad=inLen%8;
	if(inPad > 0)
	{
		inPad=8-inPad;
		#ifdef TEST_KEYS
		memcpy(szData+inLen, "\x9E\x2D", inPad);
		#else
		memcpy(szData+inLen, szRandomKey+1, inPad);
		#endif
		inLen+=inPad;
	}
	
    sprintf(szLen, "%02X", inPad);
    vdDebug_LogPrintf("inPad: %s", szLen);
    wub_str_2_hex(szLen, szRandomKey, 2);
    /*generate radom key*/
    DebugAddHEX("*szRandomKey", szRandomKey, 16);

	#ifdef TEST_KEYS
	memset(szRandomKey, 0, sizeof(szRandomKey));
	memcpy(szRandomKey, "\x02\x3F\x70\x1D\x8E\x25\x77\xE3\x01\xAC\xD6\x20\x71\x8B\xF9\xD4", 16);
	#endif
	
    //for(inCnt=0; inCnt<(inMsgLen/8); inCnt++)
    for(inCnt=0; inCnt<(inLen/8); inCnt++)
    {
		memset(szDesData, 0, sizeof(szDesData));
        memcpy(szDesData, szData+inIndex, 8);
		//DebugAddHEX("szDesData", szDesData, 8);
		
        Encrypt3Des(szDesData, szRandomKey, &szEncData[inIndex]);
		DebugAddHEX("szEncData", &szEncData[inIndex], 8);
		inIndex+=8;
    }
	
	inLen=0;
	/*copy TPDU*/
	memcpy(ptrResult, szMsg, 5); inLen+=5;
	/*copy encrypted packet*/
	memcpy(ptrResult+inLen, szEncData, inIndex); inLen+=inIndex;
	
	/*encrypt random key using TPSec Key*/
    memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
	para.Version = CTOS_KMS2_VERSION;
	para.Protection.CipherKeySet = inKeySet;
	para.Protection.CipherKeyIndex = inKeyIndex;
	para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_ECB;
	para.Protection.SK_Length = 0;

	memset(plaindata, 0x00, sizeof(plaindata));
    memset(szEncryptedRandomKey, 0x00, sizeof(szEncryptedRandomKey));
    memcpy(plaindata, szRandomKey, 16);
	para.Input.Length = 16;
	para.Input.pData = plaindata;
	para.Output.pData = szEncryptedRandomKey;

	ret = CTOS_KMS2DataEncrypt(&para);
    
    //vdDebug_LogPrintf("KMS encry KeySet[%d] KeyIndex[%d] ret[%d]", para.Protection.CipherKeySet, para.Protection.CipherKeyIndex, ret);
    if(ret != d_OK)
        return ret;

    //memcpy(szDataout, EncrydataOut, inDataLen);
    DebugAddHEX("inCTOSS_MayBankTLE_KMS3DESEncryptData-------",szEncryptedRandomKey, 16);

    /*copy encrypted random key*/
    memcpy(ptrResult+inLen, szEncryptedRandomKey, inIndex); inLen+=16;
	
	return inLen;
}

void vdGenerateRandomKey(int inLen, BYTE *ptrResult)
{
	int i;
	time_t t;
	BYTE szRandomKey[64+1];
    BYTE sKey[2+1];
	int inRandonNum=0;

    vdDebug_LogPrintf("vdGenerateRandomKey");
	vdDebug_LogPrintf("Len:%d", inLen);
	
	memset(szRandomKey, 0, sizeof(szRandomKey));
	
	/* Intializes random number generator */
	srand((unsigned) time(&t));
	
	/* get 16 random numbers from 0 to 49 */
	for( i = 0 ; i < inLen ; i++ ) {
	   inRandonNum=rand() % 255;
	   //vdDebug_LogPrintf("b=%02X", (int)inRandonNum);
	   memset(sKey, 0, sizeof(sKey));
	   sprintf(sKey, "%02X", inRandonNum);
	   memcpy(&szRandomKey[i*2], sKey, 2);
	}
	//vdDebug_LogPrintf("*szRandomKey", szRandomKey);
	vdDebug_LogPrintf("szRandomKey:%s", szRandomKey);
	
	wub_str_2_hex(szRandomKey, ptrResult, 32);
}

/*
6001000027 D7252DA6170B6DF5 C337CC752D6F87F7 79391D720D0D573B
233AE3ED3417DDCB F8711A5B65CE7AD2 7DF3A6CA9C2E7848
1FB6131FAB375252 A453AC8FF267CF43
len=64+5

\x60\x01\x00\x00\x27\xD7\x25\x2D\xA6\x17\x0B\x6D\xF5\xC3\x37\xCC\x75\x2D\x6F\x87\xF7\x79\x39\x1D\x72\x0D\x0D\x57\x3B
\x23\x3A\xE3\xED\x34\x17\xDD\xCB\xF8\x71\x1A\x5B\x65\xCE\x7A\xD2\x7D\xF3\xA6\xCA\x9C\x2E\x78\x48
\x1F\xB6\x13\x1F\xAB\x37\x52\x52\xA4\x53\xAC\x8F\xF2\x67\xCF\x43

*/

int inDecryptTPSec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult)
{
    BYTE szDecryptData[512+1], szData[512+1], szDesData[16+1];
    BYTE szRandomKey[16+1], szLen[2+1];
    int inLen=0, inIndex=0, inCnt=0, inPad;
    int inKeySet=TPSEC_KEY_SET+1, inKeyIndex=TPSEC_KEY_INDEX;
    CTOS_KMS2DATAENCRYPT_PARA para;
    BYTE plaindata[16+1], szEncryptedRandomKey[16+1];
    USHORT ret;

    memset(szData, 0, sizeof(szData));
	memset(szEncryptedRandomKey, 0, sizeof(szEncryptedRandomKey));
	
	inLen=inMsgLen-5; /*subtract 5 for TPDU*/
    memcpy(szData, szMsg+5, inLen-16); /*copy data exclude TPDU and Encrypted Random Key*/
    DebugAddHEX("szData-------",szData, inLen-16);

	memcpy(szEncryptedRandomKey, szMsg+(inMsgLen-16), 16);
	DebugAddHEX("szEncryptedRandomKey-------",szEncryptedRandomKey, 16);
	
	/*decrypt random key using TPSec Key*/
    memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
    para.Version = CTOS_KMS2_VERSION;
    para.Protection.CipherKeySet = inKeySet;
    para.Protection.CipherKeyIndex = inKeyIndex;
    para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_ECB;
    para.Protection.SK_Length = 0;
    
    memset(plaindata, 0x00, sizeof(plaindata));
    memset(szRandomKey, 0x00, sizeof(szRandomKey));
    memcpy(plaindata, szEncryptedRandomKey, 16);
    para.Input.Length = 16;
    para.Input.pData = plaindata;
    para.Output.pData = szRandomKey;
    
    ret = CTOS_KMS2DataEncrypt(&para);
    
    vdDebug_LogPrintf("KMS encry KeySet[%d] KeyIndex[%d] ret[%d]", para.Protection.CipherKeySet, para.Protection.CipherKeyIndex, ret);
    if(ret != d_OK)
        return ret;
    
    //memcpy(szDataout, EncrydataOut, inDataLen);
    DebugAddHEX("szRandomKey-------",szRandomKey, 16);

	memset(szDecryptData, 0, sizeof(szDecryptData));
	
	for(inCnt=0; inCnt<((inMsgLen-16)/8); inCnt++) /*subtract the encrypted random key*/
    {
		memset(szDesData, 0, sizeof(szDesData));
        memcpy(szDesData, szData+inIndex, 8);
		//DebugAddHEX("szDesData", szDesData, 8);
		
        Decrypt3Des(szDesData, szRandomKey, &szDecryptData[inIndex]);
		DebugAddHEX("szDecryptData", &szDecryptData[inIndex], 8);
		inIndex+=8;
    }

	inLen=0;
	
	/*copy TPDU*/
	memcpy(ptrResult, szMsg, 5); inLen+=5;
	/*copy decrypted packet*/
	memcpy(ptrResult+inLen, szDecryptData, inIndex); inLen+=inIndex;
	/*remove 2 padded random data*/
	memset(plaindata, 0x00, sizeof(plaindata));
	wub_hex_2_str(szRandomKey, plaindata, 16);
	plaindata[2]=0x00;
	inLen-=atoi(plaindata);
	
    return inLen;
}

int inCheckTPSecKey(void)
{
    int inRet=0;

	if(strTCT.fTPSec != TRUE)
		return d_OK;

    inRet = CTOS_KMS2KeyCheck(TPSEC_KEY_SET, TPSEC_KEY_INDEX);
    if(inRet != d_OK)
    {
        vdDisplayErrorMsg2(1, 8, "PLEASE INJECT", "TPSEC KEY", MSG_TYPE_ERROR);
        return(inRet);
    }
	
    return d_OK;
}

