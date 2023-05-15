#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

#include "../Database/DatabaseFunc.h"
#include "pinpad.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"
#include "..\Debug\Debug.h"
#include "../Ui/Display.h"


#define PIN_POSITION_X	8
#define PIN_POSITION_Y	7

int gblinNoKeys = 0;

int inInitializePinPad(void)
{		
		CTOS_KMS2Init();
		//TEST_Write3DES_Plaintext();		
		return d_OK;
}
//For testing and development only, hardcode the key
void TEST_Write3DES_Plaintext(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];
	//Debit TMK key set and index
	KeySet = 0xC000;
	KeyIndex = 0x0001;
	memcpy(KeyData, "\x11\x11\x11\x11\x22\x22\x22\x22\x33\x33\x33\x33\x44\x44\x44\x44", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
//	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN | SBKMS2_KEYATTRIBUTE_MAC;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;

	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	ret = CTOS_KMS2KeyWrite(&para);
vdDebug_LogPrintf("--CTOS_KMS2KeyWrite:(%d)", ret);
    #if 0
	strcpy(srTransRec.szPAN,"1234567890123456");
	srTransRec.HDTid = 6;
	srTransRec.byTransType = SALE;
	strcpy(strCST.szCurSymbol,"SGD");
	strcpy(srTransRec.szTotalAmount,"1");
	inGetIPPPin();
	memset(szDataIn, 'A', 100);
	memset(szInitialVector, 0, 8);
	memset(szMAC, 0, 8);
	inIPPGetMAC(szDataIn, 8, szInitialVector, szMAC);
	#endif
}
	
void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine)
{
	char szDisplayBuf[30];
	BYTE baAmount[20];
    BYTE szStr[20];
	
	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);

	memset(szStr, 0x00, sizeof(szStr));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn",baAmount, szStr);
	sprintf(szDisplayBuf, "%s%s", strCST.szCurSymbol, szStr);
	vdDebug_LogPrintf("AAA - inCTOS_DisplayCurrencyAmount szDisplayBuf:%s",szDisplayBuf);
	setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szDisplayBuf);
}

void OnGetPINDigit(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
	
   DebugAddINT("OnGetPINDigit", NoDigits);

}

void OnGetPINCancel(void)
{
   DebugAddINT("OnGetPINCancel", 1);

}

void OnGetPINBackspace(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
   DebugAddINT("OnGetPINBackspace", NoDigits);
}

void vdDisplayErrorMessage(USHORT errorID)
{
	char szErr[50+1];

	memset(szErr,0x00,sizeof(szErr));
	switch(errorID)
	{
		case d_KMS2_GET_PIN_TIMEOUT:
			strcpy(szErr,"TIME OUT");
			break;
		case d_KMS2_GET_PIN_ABORT:
			strcpy(szErr,"GET PIN ABORT");
			break;
		default:
			strcpy(szErr,"GET PIN ABORT");
			break;
	}
	vdDisplayErrorMsg(1, 8, szErr);
}

int inGetIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_7 stPinGetPara;
    int inRet;
	
	BYTE str[40],key;
	USHORT ret;


	
	vdDebug_LogPrintf("saturn --inGetIPPPin[%d]--",srTransRec.HDTid);
	//inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);

    /*check for keys if injected -- sidumili*/
    //inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
    //if (inRet != d_OK)
    //    return(inRet);
    /*check for keys if injected -- sidumili*/

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);

	if(srTransRec.byTransType != BAL_INQ)
	    inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_7));
	stPinGetPara.Version = 0x07;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;

	//test
	stPinGetPara.Protection.CipherKeyIndex = 0xC0;
	stPinGetPara.Protection.CipherKeySet = 0x01;

	//test
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	stPinGetPara.Control.Timeout = 0;
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	vdDebug_LogPrintf("saturn before ");

	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
	if(ret != d_OK)
	{
		sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}
	return d_OK;
}

int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC)
{
	CTOS_KMS2MAC_PARA para;
	USHORT ret;
	BYTE key,str[40];
	
	CTOS_LCDTClearDisplay();
	DebugAddSTR("inGetIPPMAC","Processing...",20);    
	
	memset(&para, 0x00, sizeof(CTOS_KMS2MAC_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = strDCT.usKeySet;
	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	para.Protection.CipherMethod = KMS2_MACMETHOD_CBC;
	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	para.Protection.pSK = strDCT.szMACKey;
	para.ICV.Length = 8;
	para.ICV.pData = szInitialVector;
	para.Input.Length = inLengthIn;
	para.Input.pData = szDataIn;
	para.Output.pData = szMAC;
	
	ret = CTOS_KMS2MAC(&para);
	if(ret != d_OK)
		return ret;
	return d_OK;		
}

int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC)
{
	BYTE szInitialVector[8];

	inIPPGetMAC(szDataIn, inLengthIn,  szInitialVector, szMAC);
}
	
int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex)
{
    USHORT rtn;
    
    vdDebug_LogPrintf("SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);
    
    rtn = CTOS_KMS2KeyCheck(ushKeySet, ushKeyIndex);
    if (rtn != d_OK){
//   				CTOS_PrinterPutString("PLEASE INJECT KEY");
		
        vdDisplayErrorMsg(1, 8, "PLEASE INJECT KEY");
    	}
    
    return rtn;
}


int GetPIN_With_3DESDUKPT(void)
{
	//CTOS_KMS2PINGET_PARA stPinGetPara;
	CTOS_KMS2PINGET_PARA_VERSION_7 stPinGetPara;
	USHORT ret;
	BYTE str[17];
	BYTE key;
	BYTE PINBlock[16];
	BYTE *pCipherKey;
	BYTE CipherKeyLength;
	BYTE DecipherPINBlock[16];
	BYTE ExpectPINBlock[16];
	BYTE ksn[20];
	BYTE ksn_Len;
	int inRet;

	vdDebug_LogPrintf("--GetPIN_With_3DESDUKPT--");
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	
//Retry://#00087 No terminal response upon pressing enter button without PIN
	DebugAddSTR("GetPIN_With_3DESDUKPT","Processing...",20);
	//inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);


	
	vdDebug_LogPrintf("saturn after getpiUI");

    /*check for keys if injected -- sidumili*/
    //inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
    //if (inRet != d_OK)
    //    return(inRet);
    /*check for keys if injected -- sidumili*/

#if 0
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);

	if(srTransRec.byTransType != BAL_INQ)
	    inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
#endif

	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_7));
	stPinGetPara.Version = 0x07;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	//stPinGetPara.PIN_Info.PINDigitMinLength = 4;
	//stPinGetPara.PIN_Info.PINDigitMaxLength = 12;
	//stPinGetPara.Protection.CipherKeySet = 0x1010;
	//stPinGetPara.Protection.CipherKeyIndex = 0x0001;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;

	//test
	stPinGetPara.Protection.CipherKeyIndex = 0xC0;
	stPinGetPara.Protection.CipherKeySet = 01;
	
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.Protection.SK_Length = 0;
	//stPinGetPara.AdditionalData.InLength = 16;
	//stPinGetPara.AdditionalData.pInData = (BYTE*)TestPAN;
	
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
	stPinGetPara.DUKPT_PARA.IsUseCurrentKey = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;//PINBlock; -- fix for issue DE 52 has no value
	stPinGetPara.Control.Timeout = 20;
	//stPinGetPara.Control.AsteriskPositionX = 2;
	//stPinGetPara.Control.AsteriskPositionY = 7;
	stPinGetPara.Control.NULLPIN = TRUE;//FALSE; -to accept null value for a pin - #00087 No terminal response upon pressing enter button without PIN
	stPinGetPara.Control.piTestCancel = NULL;


	// SHOULD be KSN 1st before Pin Entry - fix for decline response
	
	vdDebug_LogPrintf("saturn before ksn");
    ksn_Len = 20;
    ret = CTOS_KMS2DUKPTGetKSN(stPinGetPara.Protection.CipherKeySet, stPinGetPara.Protection.CipherKeyIndex, ksn, &ksn_Len);

    memcpy(srTransRec.szKSN, ksn, 20);

	
	//CTOS_LCDSelectMode(d_LCD_TEXT_320x240_MODE); //-- if enable will remove all display title.
	// #00085 - PIN still displays upon clearing entered PIN (half asterisk only)
        CTOS_LCDFontSelectMode(d_FONT_FNT_MODE); 

	vdDebug_LogPrintf("saturn before CTOS_KMS2PINGet");
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
#if 1
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BYPASSED");
            CTOS_Delay(300);
            //memset(card_holder_pin,0,sizeof(card_holder_pin));
            //srTransRec.fCUPPINEntry = FALSE;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else if (ret == 10511)
        {
            vdSetErrorMessage("USER CANCEL");
            return(d_NO);
        }
        else if (ret == 10512) /*Timeout --sidumili*/ 
        {
            //fTimeOutFlag = TRUE; /*MCC: Flag for timeout --sidumili*/
            vdDisplayErrorMsg(1, 8, "PIN TIMEOUT");
            return(d_NO);
        }
        else
        {
            /* MCCCLG: Added message if user cancelled during pin entry - start -- jzg*/
            if(ret == d_KMS2_GET_PIN_ABORT)
            {
                //vdDisplayErrorMsg(1, 8, "USER CANCEL");
            }
            else
            {
                sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
                vdDisplayErrorMsg(1, 8, str);
            }
            /* MCCCLG: Added message if user cancelled during pin entry - end -- jzg*/
            return ret;
        }
    }

#else
    if(ret != d_OK)
    {

	if(ret == 10511)// return cancel value
	{
    		inSetTextMode(); // 1. #00085 - PIN still displays upon clearing entered PIN (half asterisk only) 2. to allign displays again.	
	   	return ret;
	}
	
    	// removed to fix issue #84 amd #86
        //sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
        //CTOS_LCDTPrintXY(1, 8, str);
        //CTOS_KBDGet(&key);
    	inSetTextMode(); // #00085 - PIN still displays upon clearing entered PIN (half asterisk only) to allign displays again.
    	if(ret == 10512) // TIME OUT UPON PIN ENTRY - #00032 Incorrect terminal display on Enter PIN prompt
	{
    		CTOS_LCDTPrintXY(1, 6, "TIME OUT"); //#00087 No terminal response upon pressing enter button without PIN
    		CTOS_LCDTPrintXY(1, 8, "PLS ENTER PIN"); //#00087 No terminal response upon pressing enter button without PIN	
    	}
	else
    		CTOS_LCDTPrintXY(1, 8, "INVALID ENTRY"); //#00087 No terminal response upon pressing enter button without PIN
    	
    	vduiWarningSound();
        goto Retry;
        //return ret;
    }
#endif
		
    return d_OK;
}




int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}


int inCTOS_KMS2PINGetExDukpt(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow)
{
	BYTE szIndex[2]={0};
	USHORT inRet = 0;

         vdDebug_LogPrintf("inGetIPPPinEx Start KeySet[%d] KeyIndex[%d]", KeySet, KeyIndex);
         vdDebug_LogPrintf("pInData[%s] fPinBypassAllow[%d]", pInData, pinBypassAllow);
         DebugAddHEX("inCTOS_KMS2PINGetExDukpt szKSN*", szKSN, 10);
//         inCallJAVA_inGetIPPPinEx(KeySet, KeyIndex, pInData, szPINBlock, szKSN,  szIndex, pinBypassAllow);
         vdDebug_LogPrintf("szIndex[%c]", szIndex[0]);
         vdDebug_LogPrintf("inGetIPPPinEx End szIndex[%c]", szIndex[0]);
         DebugAddHEX("szIndex*", szIndex, 2);
         DebugAddHEX("szPINBlock*", szPINBlock, 8);

	inRet = atoi(szIndex);

	if(inRet == 0)
	{
		return d_OK;
	}
	else
	{
        inRet = htoi(szKSN);	
		return inRet;
    }
}


USHORT inCTOS_KMS2PINGetEx3Des(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow)
{

    BYTE szIndex[2]={0};
    USHORT inRet = 0;

    vdDebug_LogPrintf("inGetPIN_With_3DESDUKPTEx Start KeySet[%d] KeyIndex[%d]", KeySet, KeyIndex);
    vdDebug_LogPrintf("pInData[%s]", pInData);
//    inCallJAVA_inGetPIN_With_3DESDUKPTEx(KeySet, KeyIndex, pInData, szPINBlock, szKSN,  szIndex, pinBypassAllow);
    vdDebug_LogPrintf("szIndex[%c]", szIndex[0]);
    vdDebug_LogPrintf("inGetPIN_With_3DESDUKPTEx End szIndex[%c]", szIndex[0]);

    inRet = atoi(szIndex);

    if(inRet == 0)
    {
        return d_OK;
    }
    else
    {
        inRet = htoi(szKSN);
        return inRet;
    }
}



int inGetIPPPinEx(void)
{
    int inRet;
	
	BYTE str[40],key;
	USHORT ret;
	BYTE PINBlockStr[20];
    BYTE ksn[20];
    BYTE ksnStr[20];;

		
	vdDebug_LogPrintf("saturn --inGetIPPPinEx");
	//inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);

	vdDebug_LogPrintf("CTOS_KMS2Init");
	CTOS_KMS2Init();


	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);

	if(srTransRec.byTransType != BAL_INQ)
	    inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	

	vdDebug_LogPrintf("saturn before ");

     
	
	memset(PINBlockStr, 0x00, sizeof(PINBlockStr));
	memset(ksnStr, 0x00, sizeof(ksnStr));
	
	ret = inCTOS_KMS2PINGetEx3Des(strDCT.usKeySet, strDCT.usKeyIndex, srTransRec.szPAN, PINBlockStr, ksnStr, 0);
	
	wub_str_2_hex(PINBlockStr, srTransRec.szPINBlock, strlen(PINBlockStr));

	if(ret != d_OK)
	{
		sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}
	return d_OK;
}

int CTOSS_Load_3DES_TMK_PlaintextNew(char *szKeyData)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	
	CTOS_LCDTClearDisplay();
            
	CTOS_LCDTPrintXY(1, 1, "Write 3DES PT");
	
	//---------------------------------------------------------------------
	// Write 3DES Key in plaintext	
	
    //inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);
	KeySet = strDCT.usKeySet;
	KeyIndex = strDCT.usKeyIndex;

	KeySet = 0xC001;
    KeyIndex = 0x01;
	
	vdDebug_LogPrintf("**CTOSS_Load_3DES_TMK_Plaintext*,strDCT.usKeySet[%x]strDCT.usKeyIndex[%x]*",strDCT.usKeySet,strDCT.usKeyIndex);
        
	sprintf(str, "KeySet = %04X", KeySet);
	CTOS_LCDTPrintXY(1, 3, str);
	sprintf(str, "KeyIndex = %04X", KeyIndex);
	CTOS_LCDTPrintXY(1, 4, str);
	
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
#ifdef ANDROID
	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN;
#else
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
#endif
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = szKeyData;
	para.Value.KeyLength = 16;

	ret = CTOS_KMS2KeyWrite(&para);
    
	vdDebug_LogPrintf("**TMK CTOS_KMS2KeyWrite*ret[%d]*",ret );
	if(ret != d_OK)
	{
		sprintf(str, "ret = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
        return d_NO;
	}
    else
        return d_OK;
    

	CTOS_LCDTPrintXY(1, 7, "Write Key Done");
__Leave:
	CTOS_KBDGet(&key);
}

int CTOSS_Load_3DES_TMK_PlaintextNewTest(char *szKeyData)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	
	CTOS_LCDTClearDisplay();
            
	CTOS_LCDTPrintXY(1, 1, "Write 3DES PT");
	
	//---------------------------------------------------------------------
	// Write 3DES Key in plaintext	
	//KeySet = 0xC001;
    //KeyIndex = 0x01;
	KeySet = 0xC000;
    KeyIndex = 0x04;
	
	vdDebug_LogPrintf("**CTOSS_Load_3DES_TMK_Plaintext*,KeySet[%x]KeyIndex[%x]*", KeySet, KeyIndex);
        
	sprintf(str, "KeySet = %04X", KeySet);
	CTOS_LCDTPrintXY(1, 3, str);
	sprintf(str, "KeyIndex = %04X", KeyIndex);
	CTOS_LCDTPrintXY(1, 4, str);
	
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
#ifdef ANDROID
	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN;
#else
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
#endif
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = szKeyData;
	para.Value.KeyLength = 16;

	ret = CTOS_KMS2KeyWrite(&para);
    
	vdDebug_LogPrintf("**TMK CTOS_KMS2KeyWrite*ret[%d]*",ret );
	if(ret != d_OK)
	{
		sprintf(str, "ret = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
        return d_NO;
	}
    else
        return d_OK;
    

	CTOS_LCDTPrintXY(1, 7, "Write Key Done");
__Leave:
	CTOS_KBDGet(&key);
}

void vdCTOSS_GetKSN(void)
{
	BYTE baTmlKSN[10+1];
	BYTE baExtPPKSN[10+1];
	BYTE ksn_len = 10;
	USHORT ushRet = d_OK;

   /*Get Tml KSN*/
   memset(baTmlKSN, 0x00, sizeof(baTmlKSN));
   //ushRet = CTOS_KMS2DUKPTGetKSN(0xC000, 0x0004, baTmlKSN, &ksn_len);
   ushRet = CTOS_KMS2DUKPTGetKSN(strDCT.usKeySet, strDCT.usKeyIndex, baTmlKSN, &ksn_len);
   vdDebug_LogPrintf("CTOS_KMS2DUKPTGetKSN[%04X] ksn_len[%d]", ushRet, ksn_len);
   DebugAddHEX("TML KSN:", baTmlKSN, 10);
   memset(srTransRec.szKSN, 0x00, sizeof(srTransRec.szKSN));		
   wub_hex_2_str(baTmlKSN, srTransRec.szKSN, 10); 
   //memcpy(srTransRec.szKSN, baTmlKSN, 10);
   DebugAddHEX("srTransRec.szKSN", srTransRec.szKSN, 10);

}
void vdCTOSS_EFT_TestHardcodeKey(void)
{
    USHORT ret;
    BYTE msg[16];
    BYTE basekey[16];
    BYTE ksn[10];
    CTOS_KMS2KEYWRITE_PARA params;
    int i;
    BYTE testksn[10];
    BYTE testksnlen = 10;
	int inKeySet; 
	int inKeyIndex;
	
	//BYTE  IPEK[16] = {0xB4, 0x60, 0xA5, 0x1C, 0x2D, 0x45, 0x88, 0x54, 0x5C, 0x58, 0x94, 0xAD, 0x3F, 0x97, 0xB0, 0x6D};
					//IPEK:      B460A51C2D4588545C5894AD3F97B06D
	//MCC android
	BYTE  IPEK[16] = {0xB7, 0xF0, 0xD0, 0xEB, 0x98, 0x87, 0x32, 0x75, 0xE7, 0x1F, 0x9A, 0xE3, 0x2D, 0xB4, 0x96, 0xA0};
					//IPEK:      B7F0D0EB98873275E71F9AE32DB496A0
	//BYTE  InitiallyLoadedKeySerialNumber[10] = {0x00, 0x11, 0x00, 0x10, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x00};
	BYTE  InitiallyLoadedKeySerialNumber[10] = {0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0x00};

    //CTOS_KMS2Erase();


    
    memset(&params, 0, sizeof(params));

	//inKeySet = 49152;//0xC000;
	//inKeyIndex = 4; //0x0004;

	//inKeySet = 0xC000;
	//inKeyIndex = 0x0004;
	inKeySet = 0xC001;
	inKeyIndex = 0x0001;

	//CTOS_KMS2DUKPTGetKSN[0]inKeySet[49152]inKeyIndex[4]
    //vdEFT_Debug_LogPrintf("IPEK[%02x %02x %02x %02x %02x]", IPEK[0], IPEK[1], IPEK[2], IPEK[3], IPEK[4]);
    //vdEFT_Debug_LogPrintf("KSN[%02x %02x %02x %02x %02x]", InitiallyLoadedKeySerialNumber[0], InitiallyLoadedKeySerialNumber[1], InitiallyLoadedKeySerialNumber[2], InitiallyLoadedKeySerialNumber[3], InitiallyLoadedKeySerialNumber[4]);
	DebugAddHEX("IPEK",(char *)IPEK,16);
	DebugAddHEX("KSN",(char *)InitiallyLoadedKeySerialNumber,10);
    memcpy(basekey, IPEK, 16);
    memcpy(ksn, InitiallyLoadedKeySerialNumber, 10);        
    params.Version = 0x01;
    params.Info.KeySet = inKeySet;
    params.Info.KeyIndex = inKeyIndex;
    params.Info.KeyType = KMS2_KEYTYPE_3DES_DUKPT;
    params.Info.KeyVersion = 0x07;
//    params.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN | SBKMS2_KEYATTRIBUTE_ENCRYPT | SBKMS2_KEYATTRIBUTE_MAC | SBKMS2_KEYATTRIBUTE_KPK;
    params.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;        
    params.Value.KeyLength = 16;
    params.Value.pKeyData = basekey;
    params.DUKPT_DATA.KSNLength = 10;
    params.DUKPT_DATA.pKSN = ksn;        

    ret = CTOS_KMS2KeyWrite(&params);    

    vdDebug_LogPrintf("vdCTOSS_EFT_TestHardcodeKey[%d]", ret);

}


int inCTOS_InjectKeyTest(void)
{
	BYTE	key;
	BYTE	KeyType;
	int i;
	int inRet = d_NO;
	char szKeyData[32+1] ={'\0'};
	USHORT ushLen = 16;
 char szResponseString[50],szResponseString2[50], szReResponseString[50];
	char szResponseStringHex[33],szResponseString2Hex[33];
 char szMsg[23];
	BYTE bRet;

	CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("inCTOS_InjectKeyTest");

	{
	     memcpy(szKeyData,"\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33",16);

		CTOS_KMS2Init();
		
		inRet = CTOSS_Load_3DES_TMK_PlaintextNewTest(szKeyData);

		CTOS_LCDTClearDisplay();
		vduiWarningSound();
		vduiDisplayStringCenter(7,"INJECT SUCCESS");
		CTOS_Delay(1500);
		vdSetErrorMessage("");
	}


	return d_OK;
}



int GetPIN_With_3DESDUKPTEx(void)
{
	USHORT ret;
	BYTE str[17];
	BYTE key;
	BYTE PINBlock[16];
	BYTE ksn_Len;
	int inRet;

	BYTE PINBlockStr[20];
    BYTE ksn[20];
    BYTE ksnStr[20];



	vdDebug_LogPrintf("saturn --GetPIN_With_3DESDUKPTEx--");

	// No pin entry on installment transaction -- sidumili
	if (srTransRec.fInstallment == TRUE)
	{
		return d_OK;
	}
	
	DebugAddSTR("GetPIN_With_3DESDUKPTEx","Processing...",20);

	
	vdDebug_LogPrintf("inCTOS_InjectKeyTest");

	
	vdDebug_LogPrintf("CTOS_KMS2Init");
	CTOS_KMS2Init();
	
	//inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);

	vdDebug_LogPrintf("saturn before ksn");
  
   // memcpy(srTransRec.szKSN, ksn, 20);

	
    CTOS_LCDFontSelectMode(d_FONT_FNT_MODE); 

    vdDebug_LogPrintf("saturn before inCTOS_KMS2PINGetExDukpt, inCTOSS_CheckCVMOnlinepin[%d], srTransRec.IITid[%d], srTransRec.byTransType[%d]", inCTOSS_CheckCVMOnlinepin(), srTransRec.IITid, srTransRec.byTransType);

     
    if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.IITid == 8 && srTransRec.byEntryMode == CARD_ENTRY_WAVE)
    {
         srTransRec.fCUPPINEntry = FALSE;
		 srTransRec.fEMVPIN = FALSE;
		 srTransRec.fEMVPINEntered = FALSE;
         return d_KMS2_GET_PIN_NULL_PIN;
    }
	//vdCTOSS_EFT_TestHardcodeKey();		//tine -- for test only - hardcode master session key for dukpt pin entry

    memset(PINBlockStr, 0x00, sizeof(PINBlockStr));
    memset(ksnStr, 0x00, sizeof(ksnStr));

   // strDCT.usKeySet = 0xC001;
  //  strDCT.usKeyIndex = 0x01;

    vdCTOSS_GetKSN();

   //wub_str_2_hex(srTransRec.szPINBlock, PINBlockStr, strlen(PINBlockStr));
    //wub_str_2_hex(srTransRec.szKSN, ksn, strlen(ksnStr));


   // ret = inCTOS_KMS2PINGetExDukpt(strDCT.usKeySet, strDCT.usKeyIndex, srTransRec.szPAN, PINBlockStr, ksnStr, 0);
   ret = inCTOS_KMS2PINGetExDukpt(strDCT.usKeySet, strDCT.usKeyIndex, srTransRec.szPAN, srTransRec.szPINBlock, ksnStr, 1); //ksnStr has no value

   
    //wub_str_2_hex(PINBlockStr, srTransRec.szPINBlock, strlen(PINBlockStr));
    //wub_str_2_hex(srTransRec.szKSN, ksn, strlen(ksnStr));	   
   
    vdDebug_LogPrintf("saturn inCTOS_KMS2PINGetExDukpt[%d]", ret);
    vdPCIDebug_HexPrintf("GetPIN_With_3DESDUKPTEx srTransRec.szPINBlock", srTransRec.szPINBlock, 8);
    vdPCIDebug_HexPrintf("GetPIN_With_3DESDUKPTEx srTransRec.szKSN", srTransRec.szKSN, 10);

	
    if(ret != d_OK)
    {
        srTransRec.fEMVPINBYPASS = FALSE;
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BYPASSED");
            CTOS_Delay(300);
	  		srTransRec.fEMVPINBYPASS = TRUE;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else if (ret == 10511)
        {
            vdSetErrorMessage("USER CANCEL");
            return(d_NO);
        }
        else if (ret == 10512) /*Timeout --sidumili*/ 
        {
            vdDisplayErrorMsg(1, 8, "PIN TIMEOUT");
            return(d_NO);
        }
        else if (ret == d_KMS2_KEY_NOT_EXIST)
        {
             gblinNoKeys = 1;
            vdDisplayErrorMsg(1, 8, "PLEASE INJECT KEY");
            return(d_NO);
        }
        else
        {
            /* MCCCLG: Added message if user cancelled during pin entry - start -- jzg*/
            if(ret == d_KMS2_GET_PIN_ABORT)
            {
                //vdDisplayErrorMsg(1, 8, "USER CANCEL");
            }
            else
            {
                sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
                vdDisplayErrorMsg(1, 8, str);
            }
            /* MCCCLG: Added message if user cancelled during pin entry - end -- jzg*/
            return ret;
        }
    }
		
    return d_OK;
}

void vdSetDummyMKey(void)
{
	vdDebug_LogPrintf("--vdSetDummyMKey--");

//	CTOS_KMS2Init();
//	TEST_Write3DES_Plaintext();
//	vdCTOSS_EFT_TestHardcodeKey(); // tle dummy master key
}

