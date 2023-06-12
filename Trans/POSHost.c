/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <CTOSInput.h>


#include "..\Includes\Wub_lib.h"

#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\Display.h"

#include "..\Includes\V5IsoFunc.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\Trans.h"   


#include "..\Includes\CTOSInput.h"


#include "..\debug\debug.h"
#include "..\Accum\Accum.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\accum\accum.h"
#include "..\Includes\POSSetting.h"
#include "..\Debug\Debug.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\CTOSInput.h"
#include "..\Ctls\PosWave.h"


#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "../Includes/POSTypedef.h"
#include "../Ui/Display.h"
#include "../Debug/debug.h"
#include "../Database/DatabaseFunc.h"
#include "../Includes/POSTrans.h"
#include "../Includes/wub_lib.h"
#include "../Print/Print.h"
//#include "../UIapi.h"


#define d_NOT_RECORD            102

#define d_GETPIN_TIMEOUT        6000
#define d_DUPLICATE_INVOICE     0x0080

extern int gblinMinorUnit;
extern BOOL fAutoSettle;

extern unsigned char gblszMarginRatePercentage[10+1], gblszExchangeRate[12+1], gblszDCCCurrencySymbol[3+1],  gblszCurrencyCode[3+1], gblszForeignAmount[15+1],  gblszLocalAmount[15+1], gblszCurrForeignAmount[15+1], gblszDCCCurrBaseAmount[15+1], gblszCurrDCCTipAmount[15+1];


#if 1
int inCTOS_GetTxnBaseAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	// mfl
	if (inMultiAP_CheckSubAPStatus() == d_OK)
	   return d_OK;

	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	// mfl
	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0){

		// sidumili: Issue#:000076 [check transaction maximum amount]
		if (inCTOS_ValidateTrxnAmount()!= d_OK){
			return(d_NO);
		}
				
		return d_OK;
	}
	
#if 1
	if(1 == fGetECRTransactionFlg()) // exit if ecr txn - mfl
	{
		if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) == 0){
			vdSetErrorMessage("OUT OF RANGE1");
			return(ST_ERROR);  
		}

		return d_OK;
	}
#endif

    while(1)
    {
        CTOS_LCDTClearDisplay();
        vduiLightOn();
        
        vdDispTransTitle(srTransRec.byTransType);
    
        if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
        {
            setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
            strcpy(bDisplayStr, srTransRec.szPAN);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");
    
            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
            memset(bBuf, 0x00, sizeof(bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
            memcpy(bDisplayStr, bBuf, 2);
            bDisplayStr[2] = '/';
            memset(bBuf, 0x00, sizeof(bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
            memcpy(bDisplayStr+3, bBuf, 2);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
        }
        else
        {
    		if (1 != inCTOSS_GetWaveTransType())
    		{
    			inCTOS_DisplayCardTitle(5, 6, TRUE);
    		}
        }
	  
        CTOS_KBDHit(&key);//clear key buffer
        memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
        sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
        CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    
        memset(baAmount, 0x00, sizeof(baAmount));
        key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    
        if(d_OK == key)
        {
            sprintf(szTemp, "%012ld", atol(baAmount));
            wub_str_2_hex(szTemp,srTransRec.szBaseAmount,12);
    
            if(srTransRec.byTransType == SALE && srTransRec.fInstallment == TRUE)
            {
                long amt1=0, amt2=0;	
               
                inTCTRead(1);
                memset(szTemp, 0x00, sizeof(szTemp));
                wub_hex_2_str(strHDT.szMinInstAmt, szTemp,6);
    
    			amt1=atol(baAmount);
    			amt2=atol(szTemp);
    
    			if(amt1 < amt2)
    			{
    				char szMinInstAmt[12+1];
    				
    				memset(szMinInstAmt, 0x00, sizeof(szMinInstAmt));
    				vdFormatAmount(szMinInstAmt, "", strHDT.szMinInstAmt,FALSE);
    				memset(szTemp, 0x00, sizeof(szTemp));
    				sprintf(szTemp,"MIN AMOUNT %s", szMinInstAmt);
    
                    CTOS_LCDTClearDisplay();
    				vdDisplayErrorMsg(1, 8, szTemp);
    				vduiWarningSound();
    				continue;
    			}
				else
                    break;  					
            }	
			else 
                break;				
        }
		else
            break;			
    	
        if(0xFF == key)
        {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
    }
	
    return key;
}
#else //mfl
int inCTOS_GetTxnBaseAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20], szTemp1[12+1]={0}, szTemp2[12+1]={0};
    BYTE baAmount[20]={0};
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;
    BYTE szTitle[25+1];
    BYTE szDisplay[128];

    BYTE szDisMsg[128];

	//gcitra-0728
	BYTE bFirstKey;
	//gcitra-0728
	
	long amt1=0, amt2=0, amt3=0,  amt4=0;
	
	char szMinInstAmt[13] = {0};
	
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

	vdDebug_LogPrintf("saturn --inCTOS_GetTxnBaseAmount--");
	vdDebug_LogPrintf("fInstallment[%d]", srTransRec.fInstallment);
	vdDebug_LogPrintf("fCash2Go[%d]", srTransRec.fCash2Go);
	vdDebug_LogPrintf("fDebit[%d]", srTransRec.fDebit);

    inDatabase_TerminalOpenDatabase();
    inTCTReadEx(1);	
    if(strTCT.fRegUSD == 1) 
        inCSTReadEx(2);
    else
        inCSTReadEx(1);
	
    inDatabase_TerminalCloseDatabase();
    if (inMultiAP_CheckSubAPStatus() == d_OK)
    return d_OK;

	
	vdDebug_LogPrintf("saturn after check sub-app");

    if (1 == inCTOSS_GetWaveTransType())
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;

	 vdDebug_LogPrintf("AAA >-- fECRTxnFlg=[%d]", fGetECRTransactionFlg());
    DebugAddHEX("AAA >-- debug: ecr: srTransRec.szBaseAmount",srTransRec.szBaseAmount,6);

    if(fGetECRTransactionFlg() == TRUE)
    {
         wub_hex_2_str(srTransRec.szBaseAmount,szTemp1,6);
         amt3 = atoi(szTemp1);
         amt4 = atoi(strTCT.szMinTrxnAmt);

		 if(amt3 < amt4)
         {
             vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szMinTrxnAmt, baAmount);
             
             sprintf(szTemp,"MIN AMOUNT %s", baAmount);
             
             vdDisplayErrorMsg(1, 8, szTemp);
             vduiWarningSound();
 
            usCTOSS_LCDDisplay(" ");
			memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
            memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
            strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);
            strcpy(srTransRec.szResponseText, ECR_AMOUNT_ERR_RESP);
            return ST_ERROR;
         }

          amt4 = 0;
		  amt4 = atoi(strTCT.szMaxTrxnAmt);
		  
		  if(amt3 >= amt4)
          {
             vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szMaxTrxnAmt, baAmount); 
             sprintf(szTemp,"MAX AMOUNT %s", baAmount);
             vdDisplayErrorMsg(1, 8, szTemp);
             vduiWarningSound();
 
            usCTOSS_LCDDisplay(" ");
			memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
            memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
            strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);
            strcpy(srTransRec.szResponseText, ECR_AMOUNT_ERR_RESP);
            return ST_ERROR;
          }

    }
	
    vdDebug_LogPrintf("AAA >--amt3[%d] strTCT.szMinTrxnAmt[%s] amt4[%d]", amt3, strTCT.szMinTrxnAmt, amt4);
    if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    {
        if (inCTOS_ValidateTrxnAmount()!= d_OK)
        {
            return(d_NO);
        }
        return d_OK;
    }

	 
    if(fGetECRTransactionFlg() == TRUE)
    {
        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) == 0)
        {
            vdSetErrorMessage("OUT OF RANGE");
            return(ST_ERROR);  
        }
    }

// #00088 fix for issue Will go back to idle screen when entered amount less than 3,000 (Cash3GO / Sale Installment)
// #00037 - Header disappears when entered amount less than minimum amount
SWIPE_AGAIN:	

	if (srTransRec.fInstallment)
	{
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(SALE);
	}

/*	
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE || srTransRec.byEntryMode == CARD_ENTRY_MSR || srTransRec.byEntryMode == CARD_ENTRY_MANUAL || srTransRec.byEntryMode == CARD_ENTRY_FALLBACK)
    {
        vdClearBelowLine(2);
        inCTOS_DisplayCardTitle(5, 6, TRUE);
    }	
    else
    {		
		inCTOS_DisplayCardTitle(5, 6, FALSE);
		vdClearBelowLine(7);
    }
    //CTOS_LCDTClearDisplay();
    //vduiLightOn();
    
    //vdDispTransTitle(srTransRec.byTransType);
    
    
    //CTOS_KBDHit(&key);//clear key buffer
    //memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    //CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "AMOUNT:");
//gcitra-0728
	if(d_OK == inCTOS_ValidFirstIdleKey())
		bFirstKey = chGetFirstIdleKey();
	else
		bFirstKey = 0x00;
//gcitra-0728
*/

    memset(baAmount, 0x00, sizeof(baAmount));
    //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    //key = InputAmount2(1, 8, szDisplayBuf, 2, bFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);

	memset(szDisplay, 0x00, sizeof(szDisplay));
	strcpy(szDisplay, szTitle);
	strcat(szDisplay, "|");
    strcat(szDisplay, "Please Enter Amount");
	strcat(szDisplay, "|");
	strcat(szDisplay, strCST.szCurSymbol);

    vdDebug_LogPrintf("saturn nputAmountEx szDisplay[%s]", szDisplay);
    
    key = InputAmountEx(1, 8, szDisplay, 2, bFirstKey, baAmount, &ulAmount, 30, 0);


	
	vdDebug_LogPrintf("saturn --after InputAmountEx--");
    vdSetFirstIdleKey(0x00);
    if(d_OK == key)
    {
        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp, "%012.0f", atof(baAmount));
        wub_str_2_hex(szTemp,srTransRec.szBaseAmount,12);

        if(srTransRec.byTransType == SALE && (srTransRec.fInstallment==TRUE || srTransRec.fCash2Go==TRUE))
        {
            long amt1=0, amt2=0;	
            inTCTRead(1);
            
            amt1 = wub_str_2_long(baAmount);
            amt2 = wub_str_2_long(strTCT.szMinInstAmt);
            
            if(amt1 < amt2)
            {
                //char szMinInstAmt[13] = {0};
                
                memset(szMinInstAmt, 0x00, sizeof(szMinInstAmt));
                memset(baAmount, 0x00, sizeof(baAmount));
                
                //vdFormatAmount(szMinInstAmt, "", strTCT.szMinInstAmt, FALSE);
                //memset(szTemp, 0x00, sizeof(szTemp));
                vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szMinInstAmt, baAmount);
                
                sprintf(szTemp,"MIN AMOUNT %s", baAmount);
                
                //CTOS_LCDTClearDisplay();
                //vdClearBelowLine8();
                vdDisplayErrorMsg(1, 8, szTemp);
                vduiWarningSound();
               // clearLine(14); // clear error msg.
               usCTOSS_LCDDisplay(" ");
                goto SWIPE_AGAIN;
            }
        }
		else 
		{
			// Check for minimum amount -- sidumili
			amt1=0;
			amt2=0;
			amt1 = wub_str_2_long(baAmount);
			amt2 = wub_str_2_long(strTCT.szMinTrxnAmt);
			
			vdDebug_LogPrintf("MIN:amt1[%ld]", amt1);
			vdDebug_LogPrintf("MIN:amt2[%ld]", amt2);
			
			if(amt1 < amt2)
			{								 
				memset(szMinInstAmt, 0x00, sizeof(szMinInstAmt));
				memset(baAmount, 0x00, sizeof(baAmount));
				
				vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szMinTrxnAmt, baAmount);
				
				sprintf(szTemp,"MIN AMOUNT %s", baAmount);
				
				vdDisplayErrorMsg(1, 8, szTemp);
				//vduiWarningSound();
				CTOS_Beep();
				CTOS_Delay(300);
				//clearLine(14); // clear error msg.
				usCTOSS_LCDDisplay(" ");
				goto SWIPE_AGAIN;
			}
			
			// Check for maximum amount
			amt1=0;
			amt2=0;
			amt1 = wub_str_2_long(baAmount);
			amt2 = wub_str_2_long(strTCT.szMaxTrxnAmt);
			
			vdDebug_LogPrintf("MAX:amt1[%ld]", amt1);
			vdDebug_LogPrintf("MAX:amt2[%ld]", amt2);
			
			if(amt1 > amt2)
			{								 
				memset(szMinInstAmt, 0x00, sizeof(szMinInstAmt));
				memset(baAmount, 0x00, sizeof(baAmount));
				
				vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szMaxTrxnAmt, baAmount);
				
				//sprintf(szTemp, "%s", "MAX AMOUNT EXCEED");				
				vdDisplayMessageBox(1, 8, "", "MAX AMOUNT EXCEED", "", MSG_TYPE_WARNING);
				//vduiWarningSound();
				CTOS_Beep();
				CTOS_Delay(1000);	
				clearLine(14); // clear error msg.
				goto SWIPE_AGAIN;
			}
		}

        // sidumili: Issue#:000076 [check transaction maximum amount]
        //if (inCTOS_ValidateTrxnAmount()!= d_OK)
        //{
        //    vdDisplayErrorMsg(1, 8, "OUT OF RANGE");
        //    vduiWarningSound();
        //    clearLine(14); // clear error msg.
        //    goto SWIPE_AGAIN;			//#00108 Must go back to amount entry prompt	
        //}

    }

	//if(key == 0xFF || key == d_USER_CANCEL) /*timeout and key cancel*/
        if (key == d_USER_CANCEL) /*timeout and key cancel*/
        {
             //vdSetErrorMessage("Amt entry cancelled");
             
             //memset(szTitle, 0x00, sizeof(szTitle));
             //szGetTransTitle(srTransRec.byTransType, szTitle);
             //strcpy(szDisMsg, szTitle);
            // strcat(szDisMsg, "|");
             //strcat(szDisMsg, "AMOUNT ENTRY CANCELLED");
             //usCTOSS_LCDDisplay(szDisMsg);
			 vdDisplayMessageBox(1, 8, "", "AMOUNT ENTRY CANCELLED", "", MSG_TYPE_WARNING);
             CTOS_Beep();
             CTOS_Delay(1000);
             CTOS_Beep();
             return d_NO;
        }
        else if (key == 0xFF )
        {
             //vdSetErrorMessage("Amt entry cancelled");
             
             //memset(szTitle, 0x00, sizeof(szTitle));
             //szGetTransTitle(srTransRec.byTransType, szTitle);
             //strcpy(szDisMsg, szTitle);
             //strcat(szDisMsg, "|");
             //strcat(szDisMsg, "TIME OUT");
             //usCTOSS_LCDDisplay(szDisMsg);
			 vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
             CTOS_Beep();
             CTOS_Delay(2000);
             CTOS_Beep();
             return d_NO;
        }
	
    return key;
}

#endif
int inCTOS_GetTxnTipAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	BYTE szTitle[25+1];
    BYTE szDisplay[128];
    BYTE szDisMsg[128];
	BYTE szStr[45];
    BYTE szStr1[45];

    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

    DebugAddSTR("inCTOS_GetTxnTipAmount","Processing...",20);
 //mfl
    if(d_OK != fGetECRTransactionFlg()) // exit if ecr triggered txn - mffl
		return d_OK;

    if (d_OK != inCTOS_CheckTipAllowd())
        return d_OK;

	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    
    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    //wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    //sprintf(szDisplayBuf, "%s %s", strCST.szCurSymbol, baBaseAmount);

	wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));
	memset(szStr1, 0x00, sizeof(szStr1));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", baBaseAmount, szStr);
	sprintf(szStr1, "%s %s", strCST.szCurSymbol,szStr);
        
    do
    {
        vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
        //usCTOSS_LCDDisplay(" ");
        //CTOS_LCDTClearDisplay();
        //vdDispTransTitle(srTransRec.byTransType);
        //vdClearBelowLine(7);
		
        if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
        {
			#if 0
            setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
            strcpy(bDisplayStr, srTransRec.szPAN);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
            memset(bBuf, 0x00, sizeof(bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
            memcpy(bDisplayStr, bBuf, 2);
            bDisplayStr[2] = '/';
            memset(bBuf, 0x00, sizeof(bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
            memcpy(bDisplayStr+3, bBuf, 2);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
			#endif
        }
        else
        {
        	if (1 != inCTOSS_GetWaveTransType())
        	{
				inCTOS_DisplayCardTitle(5, 6, FALSE);
        	}
        }
        
        CTOS_KBDHit(&key);//clear key buffer
        
        //CTOS_LCDTPrintXY(1, 7, "TIP:");

		memset(szDisplay, 0x00, sizeof(szDisplay));
		//strcpy(szDisplay, "BASE AMOUNT: ");
		//strcat(szDisplay, szStr1);
		strcpy(szDisplay, szTitle);
		strcat(szDisplay, "|");
	    strcat(szDisplay, "Tip Amount");
		strcat(szDisplay, "|");
		strcat(szDisplay, strCST.szCurSymbol);

        memset(baAmount, 0x00, sizeof(baAmount));
        //key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
		key = InputAmountEx(1, 8, szDisplay, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if(d_OK == key)
        {
			ULONG ulTipPercent=0;
			
            if(0 == strlen(baAmount)) //by pass TIP
                return d_OK;

            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));

                        
			ulTipPercent=(atol(baBaseAmount) * strHDT.ulTipPercent)/100;
			//vdDebug_LogPrintf("atol(szTemp): %ld", atol(szTemp));
			//vdDebug_LogPrintf("strHDT.ulTipPercent: %ld", strHDT.ulTipPercent);
			//vdDebug_LogPrintf("ulTipPercent: %ld", ulTipPercent);

			// MCC Change Request -- Any tip amount
			if(atol(szTemp) > ulTipPercent)
			{
                //clearLine(7);
				//clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                //clearLine(8);
                continue;				
			}

			/*
			if(strcmp(szTemp, baBaseAmount) > 0)
            {
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;
            }
		*/
            
            wub_str_2_hex(szTemp,srTransRec.szTipAmount,12);
            
            break;
        }
        else if(d_USER_CANCEL == key)
        {   
            //clearLine(8);
            //vdSetErrorMessage("USER CANCEL");
            //clearLine(8);
            vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_WARNING);
            CTOS_Beep();
            CTOS_Delay(1000);
            return d_NO;
        }
        else if(0xFF == key)
        {   
            //clearLine(8);
            //vdSetErrorMessage("TIME OUT");
            vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
            CTOS_Delay(1500);
            //clearLine(8);
            return d_NO;
        }
        
        
    }while(1);
    
    return key;
}

#if 1
int inCTOS_GetTipAfjustAmount(void)
{
	char szDisplayBuf[30];
	BYTE key;
	BYTE szTemp[20];
	BYTE baAmount[20];
	BYTE baBaseAmount[20];
	BYTE bBuf[4+1];
	BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
	ULONG ulAmount = 0L;

	BYTE szTitle[25+1];
	BYTE szDisplay[128];
	BYTE szDisMsg[128];
	BYTE szStr[45];
	BYTE szStr1[45];

	int inRet=d_NO;

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	DebugAddSTR("inCTOS_GetTxnTipAmount","Processing...",20);
 //mfl
	if(d_OK != fGetECRTransactionFlg()) // exit if ecr triggered txn - mffl
		return d_OK;

	if (d_OK != inCTOS_CheckTipAllowd())
		return d_OK;

	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
		return d_OK;
	
	memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
	//wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	//sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
	//sprintf(szDisplayBuf, "%s %s", strCST.szCurSymbol, baBaseAmount);

	wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, AMT_BCD_SIZE); 	 
	memset(szStr, 0x00, sizeof(szStr));
	memset(szStr1, 0x00, sizeof(szStr1));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", baBaseAmount, szStr);
	sprintf(szStr1, "%s %s", strCST.szCurSymbol,szStr);
		
	do
	{
	    usCTOSS_LCDDisplay(" ");
		//CTOS_LCDTClearDisplay();
		//vdDispTransTitle(srTransRec.byTransType);
		//vdClearBelowLine(7);
		
		if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
		{
		#if 0
			setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
			memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
			strcpy(bDisplayStr, srTransRec.szPAN);
			CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
			setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

			memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
			memset(bBuf, 0x00, sizeof(bBuf));
			wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
			memcpy(bDisplayStr, bBuf, 2);
			bDisplayStr[2] = '/';
			memset(bBuf, 0x00, sizeof(bBuf));
			wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
			memcpy(bDisplayStr+3, bBuf, 2);
			CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
		#endif
		}
		else
		{
			if (1 != inCTOSS_GetWaveTransType())
			{
				inCTOS_DisplayCardTitle(5, 6, FALSE);
			}
		}
		
		CTOS_KBDHit(&key);//clear key buffer
		
		//CTOS_LCDTPrintXY(1, 7, "TIP:");

		memset(szDisplay, 0x00, sizeof(szDisplay));
		//strcpy(szDisplay, "BASE AMOUNT: ");
		//strcat(szDisplay, szStr1);
		strcpy(szDisplay, szTitle);
		strcat(szDisplay, "|");
		strcat(szDisplay, "Tip Amount");
		strcat(szDisplay, "|");
		strcat(szDisplay, strCST.szCurSymbol);

		memset(baAmount, 0x00, sizeof(baAmount));
		//key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
		key = InputAmountEx(1, 8, szDisplay, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

		if(d_OK == key)
		{
			ULONG ulTipPercent=0;
			
			if(0 == strlen(baAmount)) //by pass TIP
            {
				memset(srTransRec.szTipAmount, 0, sizeof(srTransRec.szTipAmount));
              	return d_OK;
            }
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%012ld", atol(baAmount));

						
			ulTipPercent=(atol(baBaseAmount) * strHDT.ulTipPercent)/100;
			//vdDebug_LogPrintf("atol(szTemp): %ld", atol(szTemp));
			//vdDebug_LogPrintf("strHDT.ulTipPercent: %ld", strHDT.ulTipPercent);
			//vdDebug_LogPrintf("ulTipPercent: %ld", ulTipPercent);

			// MCC Change Request -- Any tip amount
			if(atol(szTemp) > ulTipPercent)
			{
				//clearLine(7);
				//clearLine(8);
				vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
				//clearLine(8);
				continue;				
			}

			/*
			if(strcmp(szTemp, baBaseAmount) > 0)
			{
				clearLine(8);
				vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
				clearLine(8);
				continue;
			}
		*/
			
			wub_str_2_hex(szTemp,srTransRec.szTipAmount,12);
			
			break;
		}
		else if(d_USER_CANCEL == key)
		{	
			//clearLine(8);
			vdSetErrorMessage("USER CANCEL");
			//clearLine(8);
			return d_NO;
		}
		else if(0xFF == key)
		{	
			//clearLine(8);
			vdSetErrorMessage("TIME OUT");
			//clearLine(8);
			return d_NO;
		}
		
		
	}while(1);
	
	return key;
}



#else
int inCTOS_GetTipAfjustAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    BYTE szAmtTmp2[16+1];

    DebugAddSTR("inCTOS_GetTipAfjustAmount","Processing...",20);
    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(baTipAmount, 0x00, sizeof(baTipAmount));
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    wub_hex_2_str(srTransRec.szTipAmount, baTipAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4, FALSE);
    
    
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    sprintf(szAmtTmp2, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ORG TIP:");
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(42-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2);
    
    CTOS_LCDTPrintXY(1, 7, "TIP:");
             
    do
    {
        CTOS_KBDHit(&key);//clear key buffer

        memset(baAmount, 0x00, sizeof(baAmount));
        key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if(key == d_OK && strlen(baAmount) == 0){//by pass TIP - #00216 - Unable to accept zero amount on TIP
		memset(srTransRec.szTipAmount, 0, sizeof(srTransRec.szTipAmount)); //#00216 Terminal able to accept zero amt on tip adjust but the printour still print PHP 1.00 on tip adjust amt. It should be PHP 0.00
	return d_OK;
        }
		
        if(strlen(baAmount) > 0)
        {	
            ULONG ulTipPercent=0;			
			
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));

			ulTipPercent=(atol(baBaseAmount) * strHDT.ulTipPercent)/100;
			//vdDebug_LogPrintf("atol(szTemp): %ld", atol(szTemp));
			//vdDebug_LogPrintf("strHDT.ulTipPercent: %ld", strHDT.ulTipPercent);
			//vdDebug_LogPrintf("ulTipPercent: %ld", ulTipPercent);

			// MCC Change request -- sidumili
			#if 1
			if(atol(szTemp) > ulTipPercent)
			{
                clearLine(14);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(14);
                continue;				
			}
			/*
            if(strcmp(szTemp, baBaseAmount) > 0)
            {
                clearLine(14);//vduiClearBelow(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(14);//vduiClearBelow(8);
                continue;
            }			
            */
			#endif
            
            wub_str_2_hex(szTemp,srTransRec.szTipAmount,12);
            
            break;
        }
        else if(d_USER_CANCEL == key)
        {   
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        }
        else if(0xFF == key)
        {   
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
    }while(1);
    
    return key;
}
#endif


int inCTOS_UpdateTxnTotalAmount(void)
{
    BYTE szBaseAmount[20];
    BYTE szTipAmount[20];
    BYTE szTotalAmount[20];
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    BYTE  byDataTmp1[32];
    BYTE  byDataTmp2[32];
    BYTE  bPackSendBuf[256];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;

    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));

    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);

    sprintf(szTotalAmount, "%012ld", atol(szBaseAmount) + atol(szTipAmount));
    wub_str_2_hex(szTotalAmount,srTransRec.szTotalAmount,12);
    
    DebugAddSTR(szTotalAmount,szTipAmount,12); 
    DebugAddSTR("Total amount","Tip amount",12); 
    
    DebugAddHEX("Tip amount",srTransRec.szTipAmount,6); 
    DebugAddHEX("Total amount",srTransRec.szTotalAmount,6); 

	vdDebug_LogPrintf("srTransRec.byEntryMode: %d", srTransRec.byEntryMode);
	
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE)
    {
		inCTLOS_Updatepowrfail(PFR_EMV_PROCESSING);
        memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
        memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
        wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
        lnTmp = atol(byDataTmp1);
        wub_long_2_array(lnTmp, byDataTmp2);

        memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
        bPackSendBuf[usPackSendLen++] = 0x04;
        memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
        usPackSendLen += 4;
        
        memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);
        
        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
        usPackSendLen += 6;

        if(atol(szTipAmount) > 0)
        {
            memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
        }
        else
        {
            memset(szTipAmount, 0x00, sizeof(szTipAmount));
            memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
        }

        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
        usPackSendLen += 6;

        usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);
    }
    
    return d_OK;
}

int inCTOS_GetOffApproveNO(void)
{
    USHORT usX =1, usY = 6;
    BYTE bShowAttr = 0x02; 
    USHORT inAuthCodeLen=6,inMinLen=6;
    BYTE baPIN[6 + 1];
    BYTE   szAuthCode[6+1];
    BYTE bRet;
    BYTE szDisplay[100];
    BYTE szTitle[25+1];
    BYTE szMsg[100];

    memset(szAuthCode, 0x00, sizeof(szAuthCode));
    memset(szDisplay, 0x00, sizeof(szDisplay));
    memset(szTitle, 0x00, sizeof(szTitle));

    szGetTransTitle(srTransRec.byTransType, szTitle);

    strcpy(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter Approval Code:");

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "ENTER APPROVAL CODE:");

    while(TRUE)
    {
        inAuthCodeLen = 6;
        //bRet = InputStringAlpha2(1, 8, 0x00, bShowAttr, szAuthCode, &inAuthCodeLen, inMinLen, d_INPUT_TIMEOUT);
		//bRet = InputAlphaUI(0x03, 0x02, szAuthCode, &inAuthCodeLen, inMinLen, d_INPUT_TIMEOUT, szDisplay);
		bRet = InputQWERTY(0x03, 0x02, szAuthCode, &inAuthCodeLen, inMinLen, d_INPUT_TIMEOUT, szDisplay);
        if (bRet == d_KBD_CANCEL )
        {
            //CTOS_LCDTClearDisplay();
            //vdDisplayErrorMsg(1, 8, "USER CANCEL");
           /* memset(szMsg, 0x00, sizeof(szMsg));
            strcpy(szMsg, szTitle);
            strcat(szMsg, "|");
            strcat(szMsg, "PROCESSING...");
            usCTOSS_LCDDisplay(szMsg);*/
	  		//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
            return (d_EDM_USER_CANCEL);
        }
        else if (bRet == d_KBD_ENTER ) 
        {
            if(strlen(szAuthCode) >= 6)
            {
                strcpy(srTransRec.szAuthCode, szAuthCode);
                break;
            }
            else
            {
                memset(szAuthCode, 0x00, sizeof(szAuthCode));
                clearLine(14);                
                vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
                clearLine(14);
                clearLine(8);         // clear input line        
            }
        }
        else
        {
            memset(szAuthCode, 0x00, sizeof(szAuthCode));           
            clearLine(14);                
            vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
            clearLine(14);
        }
    }
    //usCTOSS_LCDDisplay( "|PROCESSING...");
    //vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
    return ST_SUCCESS;
}

int inCTOS_GetOffRefNoEntry(void)
{
    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 12;
    BYTE bShowAttr = 0x02; 
     USHORT inRefNoLen = 12, inMinLen = 12;
    BYTE baPIN[6 + 1];
    BYTE   szRefNo[12+1];
    BYTE   szInvNoBcd[6];
    BYTE bRet;
    int iLens = 6;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    BYTE szTitle[25+1];
    BYTE szDisplay[100];
    BYTE szDisMsg[100];


    memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

    if (inMultiAP_CheckSubAPStatus() == d_OK)
    {
        return d_OK;
    }

    memset(szRefNo, 0x00, sizeof(szRefNo));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));
    
    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
    //vdClearNonTitleLines();
    //setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO.: ");
    //CTOS_LCDTPrintXY(1, 5, "TRACE NO.: ");

    strcpy(szDisplay, "1");
	strcat(szDisplay, "|");
    strcat(szDisplay, "12");
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter RRN:");
	
    while(TRUE)
    {
         inRefNoLen = 12;
         vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         //bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
         //bRet = InputString2(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
         
         //bRet = InputStringUI(0x01, 0x02, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT,"", "ENTER INVOICE NO.");
         bRet = InputStringUI(0x01, 0x02, szRefNo, &inRefNoLen, inMinLen, 0, szDisplay);
         
         vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         if (bRet == d_KBD_CANCEL )
         {
              //memset(szDisMsg, 0x00, sizeof(szDisMsg));
              //strcpy(szDisMsg, szTitle);
             // strcat(szDisMsg, "|");
              //strcat(szDisMsg, "USER CANCEL");
             // usCTOSS_LCDDisplay(szDisMsg);
             // CTOS_Beep();
                //strcpy(szDisMsg, "|PROCESSING...");
                //usCTOSS_LCDDisplay(szDisMsg);
	      vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
              //CTOS_Delay(1500);
              return (d_EDM_USER_CANCEL);
         }
         else if (bRet == d_KBD_ENTER ) 
         {
              if(strlen(szRefNo) == 12)
              {
                   strcpy(srTransRec.szRRN, szRefNo);
				   vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
                   break;
              }
              else
              {
              	   vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
                   CTOS_Beep();
                   CTOS_Delay(1500);
                   CTOS_Beep();
              }
         }
         else
         {
         	  vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
              CTOS_Beep();
              CTOS_Delay(1500);
              CTOS_Beep();
         }
    }
	
   //  strcpy(szDisMsg, "|PROCESSING...");
     //usCTOSS_LCDDisplay(szDisMsg);
     //vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS); // sidumili: remove to fix crash on preauth void after entry of RRN
     return ST_SUCCESS;
}

int inCTOS_GetInvoice(void)
{

    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02; 
    USHORT usInvoiceLen = 6;
    BYTE baPIN[6 + 1];
    BYTE   szInvNoAsc[6+1];
    BYTE   szInvNoBcd[3];
    BYTE bRet;
    int iLens = 6;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];
    
    TRANS_DATA_TABLE srTransRecTemp;
    
    DebugAddSTR("inCTOS_GetInvoice","Processing...",20);
	
    if (!strTCT.fManualEntryInv)
    {
        memset(szBcd, 0x00, sizeof(szBcd));
        memcpy(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);    
        inBcdAddOne(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);
		
        if((inResult = inTCTSave(1)) != ST_SUCCESS)
        {
            vdSetErrorMessage("LOAD TCT ERR");
            return ST_ERROR;
        }

        memcpy(srTransRec.szInvoiceNo,strTCT.szInvoiceNo, 3);
    }
    else
    {
        memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
        memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));
        
        memset((char*)&srTransRecTemp, 0x00, sizeof(TRANS_DATA_TABLE));
        memcpy(&srTransRecTemp, &srTransRec, sizeof(TRANS_DATA_TABLE));
        
        while(TRUE)
        {
            if (strTCT.fECR) // tct
            {
                //If ECR send INV
                if (memcmp(srTransRec.szInvoiceNo, "\x00\x00\x00", 3) != 0)
                {                    
                    inResult = inCTOSS_BatchCheckDuplicateInvoice();
                    vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice[%d]", inResult);
                    
                    memcpy(&srTransRec, &srTransRecTemp, sizeof(TRANS_DATA_TABLE));
                    if (d_OK == inResult)
                    {
                        CTOS_LCDTClearDisplay();    
                        vdDisplayErrorMsg(1, 8, "DUPLICATE INVOICE");

                        return (d_NO);
                    }
                    else
                    {
                        return (d_OK);
                    }
                }
            }
            
            CTOS_LCDTClearDisplay();
            vdDispTransTitle(srTransRecTemp.byTransType);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO.: ");

            bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
            if (bRet == d_KBD_CANCEL )
            {
                CTOS_LCDTClearDisplay();    
                vdSetErrorMessage("USER CANCEL");
                memcpy(&srTransRec, &srTransRecTemp, sizeof(TRANS_DATA_TABLE));
                return (d_EDM_USER_CANCEL);
            }
            
            
            if(atoi(szInvNoAsc) != 0)
            {
                inAscii2Bcd(szInvNoAsc, szInvNoBcd, INVOICE_BCD_SIZE);
        
                memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);
                inResult = inCTOSS_BatchCheckDuplicateInvoice();
                vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice[%d]", inResult);
                if (d_OK == inResult)
                {
                    CTOS_LCDTClearDisplay();    
                    vdDisplayErrorMsg(1, 8, "DUPLICATE INVOICE");
                    memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
                    usInvoiceLen = 6;

                    memset(srTransRec.szInvoiceNo, 0x00, sizeof(srTransRec.szInvoiceNo));
                    continue;
                }
                else
                {
                    break;
                }
            }       
        }   

        memcpy(&srTransRec, &srTransRecTemp, sizeof(TRANS_DATA_TABLE));
        memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);
    }
    
    return (d_OK);
}

VS_BOOL fAmountLessThanFloorLimit(void) 
{
    long lnTotalAmount = 0;
    BYTE   szTotalAmt[12+1];
    
    /* If the transaction amount is less than the floor limit,
        Set the transaction type to OFFLINE */

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    lnTotalAmount = atol(szTotalAmt);
    
    if (lnTotalAmount < strCDT.InFloorLimitAmount)
        return(d_OK);

    return(d_NO);
}

int inCTOS_CustComputeAndDispTotal(void)
{
    CTOS_RTC SetRTC;
    BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];
    BYTE    szStr1[45];
	
	BYTE szDisMsg[1024];
    BYTE szPan[30+1];
	char szLineBuf[40];
	char szCardLabel[50 + 1]; 
	int ret;
    BYTE szTemp1[30+1];
   	BYTE szTitle[25+1];
    BYTE szExpYear[3];
    BYTE szExpMonth[3];
    BYTE szTemp[5];
    int inUTF8 = 1;
	BYTE szAmtMsg[40 + 1];

     memset(szStr, 0x00, sizeof(szStr));
     memset(szStr1, 0x00, sizeof(szStr1));

	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

    if(srTransRec.fDCCOptin == TRUE)
    {
         memset(szStr1, 0x00, sizeof(szStr1));
         sprintf(szStr1, "%s", gblszCurrForeignAmount);
    }
    else
    {
          wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
          vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
          sprintf(szStr1, "%s %s", strCST.szCurSymbol,szStr);
    }

	 memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
     sprintf(szAmtMsg, "TOTAL AMOUNT\n%s", szStr1);
		  
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
	//sprintf(szStr1, "%s %s", "TOTAL", strCST.szCurSymbol);
	//setLCDPrint(7, DISPLAY_POSITION_LEFT, szStr1);
	//setLCDPrint(7, DISPLAY_POSITION_RIGHT, szStr);	

	/*
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        return (d_OK);
    }

    if(srTransRec.byTransType != SALE)
    {
        return (d_OK);
    }
    */
    
    if (fAmountLessThanFloorLimit() == d_OK)
    {
        srTransRec.shTransResult = TRANS_AUTHORIZED;
        srTransRec.byOffline = CN_TRUE;

        //Read the date and the time //
        CTOS_RTCGet(&SetRTC);  
        /* Set Month & Day*/
        memset(szTempBuf,0,sizeof(szTempBuf));
        sprintf(szTempBuf,"%02d%02d",SetRTC.bMonth,SetRTC.bDay);
        wub_str_2_hex(szTempBuf,srTransRec.szDate,4);
        sprintf(szTempBuf1,"%02d",SetRTC.bYear);
        memcpy(srTransRec.szYear,szTempBuf1,2);
        memset(szTempBuf,0,sizeof(szTempBuf));
        sprintf(szTempBuf,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        wub_str_2_hex(szTempBuf,srTransRec.szTime,6);
    }

    //TINE:  display ExpiryDate
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szExpYear, 0x00, sizeof(szExpYear));
    memset(szExpMonth, 0x00, sizeof(szExpMonth));

    wub_hex_2_str(srTransRec.szExpireDate, szTemp, 2);
    memcpy(szExpYear,&szTemp[0],2);
    memcpy(szExpMonth,&szTemp[2],2);


    memset(szTemp1,0,sizeof(szTemp1));
	vdCTOS_FormatPANEx(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);

	    //if(fDisplayLogo == TRUE)
	    //{
		//    vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
		//	displayAppbmpDataEx(140, 35, strIIT.szIssuerLogo, TRUE);
	    //}

    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

    if (srTransRec.szCardholderName[0] != 0x20)
    {
         if(strIIT.inIssuerNumber != 8)
         TrimTrail(srTransRec.szCardholderName); // Trim Name -- sidumili	
         inUTF8 = is_utf8(srTransRec.szCardholderName);
         vdDebug_LogPrintf("AAA - srTransRec.szCardholderName[%s]", srTransRec.szCardholderName);
         if (inUTF8 == FALSE) //Cardholder name has special characters
         {
              memset(srTransRec.szCardholderName,0, sizeof(srTransRec.szCardholderName));
              strcpy(srTransRec.szCardholderName, " ");
         }
    }
    else
    {
         memset(srTransRec.szCardholderName,0, sizeof(srTransRec.szCardholderName));
         strcpy(srTransRec.szCardholderName, " ");
    }
    
          vdDebug_LogPrintf("usCTOSS_ConfirmMenu");
          strcpy(szDisMsg, szTitle);
          strcat(szDisMsg, "|");
          strcat(szDisMsg, szTemp1);	//PAN
          strcat(szDisMsg, "|");
          strcat(szDisMsg, szExpMonth);
          strcat(szDisMsg, "/");
          strcat(szDisMsg, szExpYear);
          strcat(szDisMsg, "|");
          strcat(szDisMsg, srTransRec.szCardholderName);	
          strcat(szDisMsg, "|");
          //strcat(szDisMsg, "Total Amount: ");
          strcat(szDisMsg, szAmtMsg);
          strcat(szDisMsg, "|");
          strcat(szDisMsg, strIIT.szIssuerLogo);

	ret = usCTOSS_Confirm(szDisMsg);
//	ret = ConfirmCardDetails(szDisMsg);
     
	vdDebug_LogPrintf("usCTOSS_ConfirmMenu szDisMsg[%s]", szDisMsg);
	
	
	

	vdDebug_LogPrintf("saturn confirm PAN %d", ret);

	if (ret == d_USER_CANCEL) /*key cancel*/
	{
		 //vdSetErrorMessage("Amt entry cancelled");
		 
		 //DisplayStatusLine("USER CANCEL");
		 vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		 CTOS_Beep();
		 CTOS_Delay(1500);
		 vdSetErrorMessage("");
		 return d_NO;
	}
	else if (ret == 0xFF) /*timeout*/
	{
		//DisplayStatusLine("TIME OUT");
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(1500);
		return d_NO;
	}
	
	if (ret != d_OK)
		return d_NO;

	// Confirmation
	//if (inConfirmPAN() != d_OK)
	//	return d_NO;
    vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
    return(d_OK);
}


int inCTOS_DisplayResponse(void)
{
    int inMsgid = atoi(srTransRec.szRespCode);
    int inHostIndex = srTransRec.HDTid;
    int inResult = 0;
    char szResponseCode[40], szResponseCode2[40];
	int inMsgType = 0;

    CTOS_LCDTClearDisplay();	//#00235 - Unexpected response code errors, should display other than 00 - 99

    memset(szResponseCode, 0x00, sizeof(szResponseCode2));	
    memset(szResponseCode2, 0x00, sizeof(szResponseCode));
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    inMSGResponseCodeReadByHostName(szResponseCode, szResponseCode2, inMsgid, strHDT.szAPName);

	if (srTransRec.byTransType == SETTLE && memcmp(srTransRec.szRespCode, "95", 2) == 0)            
		inMsgType = MSG_TYPE_INFO;
	else
		inMsgType = MSG_TYPE_ERROR;

	vdDebug_LogPrintf("inMsgid=[%d]",inMsgid);
	vdDebug_LogPrintf("byTransType=[%d]",srTransRec.byTransType);
	vdDebug_LogPrintf("inMsgType=[%d]",inMsgType);
			
    if(0 == strlen(szResponseCode) )
    {
        inMSGResponseCodeRead(szResponseCode, szResponseCode2, inMsgid, 1);
        if(0 != strlen(szResponseCode) )
        {   
			vdDisplayMessageBox(1, 8, szResponseCode, szResponseCode2, "", inMsgType);
			CTOS_Beep();
		    CTOS_Delay(1500);
		    CTOS_Beep();
			//vdDisplayErrorMsg2(1, 8, szResponseCode, szResponseCode2);
        }            
    }
    else
    {			
			vdDisplayMessageBox(1, 8, szResponseCode, szResponseCode2, "", inMsgType);
			CTOS_Beep();
			CTOS_Delay(1500);
			CTOS_Beep();
        //vdDisplayErrorMsg2(1, 8, szResponseCode, szResponseCode2);
    }

    //if(fGetECRTransactionFlg() == TRUE)
    //{
        //memset(srTransRec.szResponseText, 0x00, sizeof(srTransRec.szResponseText));
        //memcpy(srTransRec.szResponseText, szResponseCode, strlen(szResponseCode));
    //}

	// Mercury Requiremenrs -- sidumili
	memset(srTransRec.szResponseText, 0x00, sizeof(srTransRec.szResponseText));
	sprintf(srTransRec.szResponseText, "%s %s", szResponseCode2, szResponseCode);
	//memcpy(srTransRec.szResponseText, szResponseCode, strlen(szResponseCode));

    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    return(d_OK);  
}



int inCTOS_GeneralGetInvoice(void)
{
    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02; 
    USHORT usInvoiceLen = 6;
    BYTE baPIN[6 + 1];
    BYTE   szInvNoAsc[6+1];
    BYTE   szInvNoBcd[3];
    BYTE bRet;
    int iLens = 6;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    if (inMultiAP_CheckSubAPStatus() == d_OK)
    {
        return d_OK;
    }

    memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    //vdClearNonTitleLines();
    //setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO.: ");
    CTOS_LCDTPrintXY(1, 5, "TRACE NO.: ");
	
    while(TRUE)
    {
        usInvoiceLen = 6;
        vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szInvNoAsc),usInvoiceLen);
        //bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
        bRet = InputString2(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
        vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szInvNoAsc),usInvoiceLen);
        if (bRet == d_KBD_CANCEL )
        {
            CTOS_LCDTClearDisplay();    
            vdSetErrorMessage("USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }
        
        
        if(atoi(szInvNoAsc) != 0)
        {
//    	CTOS_PrinterPutString("PASOK DITO!!");
        
            inAscii2Bcd(szInvNoAsc, szInvNoBcd, INVOICE_BCD_SIZE);
            DebugAddSTR("INV NUM:",szInvNoAsc,12);  
            memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);
            break;
        }       
    }


    return ST_SUCCESS;
}

int inCTOS_BatchSearch(void)
{
    int inResult = d_NO;

TRANS_DATA_TABLE *srTransPara;

//	CTOS_PrinterPutString("inCTOS_BatchSearch");

    inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);

    DebugAddSTR("inCTOS_BatchSearch","Processing...",20);
    
    DebugAddINT("inCTOS_BatchSearch", inResult);

    if(inResult != d_OK)
    {   
        if (inMultiAP_CheckSubAPStatus() != d_OK)
            vdSetErrorMessage("NO RECORD FOUND");
        return d_NOT_RECORD;
    }
    
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);

#if 0
	CTOS_PrinterPutString(srTransRec.szDate);
	CTOS_PrinterPutString(srTransRec.szTime);
	CTOS_PrinterPutString("##HERE##");
	
    memcpy(srTransPara->szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransPara->szOrgTime, srTransRec.szTime, 2);
	CTOS_PrinterPutString(srTransPara->szOrgDate);
	CTOS_PrinterPutString(srTransPara->szOrgTime);
	CTOS_PrinterPutString("$$DITO$$");
	#endif
    
    return inResult;
}

int inCTOS_LoadCDTandIIT(void)
{
	if ( inCDTRead(srTransRec.CDTid) != d_OK)
	{
        vdDebug_LogPrintf("inCTOS_LoadCDTandIIT LOAD CDT ERROR [%d]", srTransRec.CDTid);
		vdSetErrorMessage("LOAD CDT ERROR");
		return(d_NO);
    }
	
    //inGetIssuerRecord(strCDT.IITid);
    if (inIITRead(srTransRec.IITid) != d_OK)
	{
        vdDebug_LogPrintf("inCTOS_LoadCDTandIIT LOAD IIT ERROR [%d]", srTransRec.IITid);
		vdSetErrorMessage("LOAD IIT ERROR");
		return(d_NO);
    }

    return ST_SUCCESS;
}


int inCTOS_CheckTipadjust()
{
    inCDTRead(srTransRec.CDTid);
    if(srTransRec.byTransType == PRE_AUTH || strCDT.inType == DEBIT_CARD
    || srTransRec.fInstallment == TRUE || srTransRec.fDebit == TRUE  || srTransRec.fLoyalty == TRUE || srTransRec.fCash2Go == TRUE)
    {
        vdSetErrorMessage("TRANS NOT ALLWD");
        return d_NO;
    }
	
    if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_TIP) || (srTransRec.byTransType == SALE_OFFLINE))
    {
		if(srTransRec.inTipAdjCount >= strTCT.inTipAdjCount)
		{
			vdSetErrorMessage("MAX TIP ADJ");
			return d_NO;
		}
			
        if((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_OFFLINE))
        {
            srTransRec.byOrgTransType = srTransRec.byTransType;
            memcpy(srTransRec.szOrgAmount, srTransRec.szTotalAmount, 6);
        }		
		else if(srTransRec.byTransType == SALE_TIP && srTransRec.fTipUploaded == TRUE)
		{
			memcpy(srTransRec.szOrgAmount, srTransRec.szTotalAmount, 6);
		}
        memcpy(srTransRec.szOrgTipAmount, srTransRec.szTipAmount, 6);
    }
    else
    {
        vdSetErrorMessage("TIP NOT ALLWD");
        return d_NO;
    }

    srTransRec.byTransType = SALE_TIP;

    return d_OK;
}

int inCTOS_CheckVOID()
{

	BYTE szTitle[25+1];
	BYTE szDisMsg[100];

	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

    if(srTransRec.byVoided == TRUE)
    {
        //vdSetErrorMessage("RECORD VOIDED");
        
		//strcpy(szDisMsg, szTitle);
		//strcat(szDisMsg, "|");
		//strcat(szDisMsg, "RECORD VOIDED");
		//usCTOSS_LCDDisplay(szDisMsg);
		if(fGetECRTransactionFlg()==TRUE)
        {
             memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
             memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
             strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);
             strcpy(srTransRec.szResponseText, ECR_RECORD_VOIDED);
        }
		vdDisplayMessageBox(1, 8, "", "RECORD VOIDED", "", MSG_TYPE_ERROR);
		CTOS_Beep();
		CTOS_Delay(1500);
		
        return d_NO;
    }


	//vdDebug_LogPrintf("testlang17 host %s",srTransRec.HDTid  );

	inCDTRead(srTransRec.CDTid);
    //if(srTransRec.byTransType == PRE_AUTH || strCDT.inType == DEBIT_CARD || srTransRec.byTransType == PREAUTH_VER
    vdDebug_LogPrintf("AAA - srTransRec.byTransType[%d]", srTransRec.byTransType);
    if(srTransRec.byTransType == PRE_AUTH || srTransRec.fDebit == TRUE || srTransRec.byTransType == PREAUTH_VER
    || srTransRec.byTransType == PREAUTH_COMP || srTransRec.HDTid == WECHAT_HOST ||srTransRec.HDTid == ALIPAY_HOST ||srTransRec.HDTid == GCASH_HOST ||srTransRec.HDTid == GRAB_HOST ||srTransRec.HDTid == UPI_HOST || srTransRec.HDTid == PHQR_HOST)
    {
        //vdSetErrorMessage("TRANS NOT ALLWD");
       // strcpy(szDisMsg, szTitle);
		//strcat(szDisMsg, "|");
		//strcat(szDisMsg, "TRANS NOT ALLOWED");
		//usCTOSS_LCDDisplay(szDisMsg);
		vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
		CTOS_Beep();
		CTOS_Delay(1500);
 
        return d_NO;
    }
	vdDebug_LogPrintf("AAA - srTransRec.byTransType[%d] 2", srTransRec.byTransType);	
    //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - START
    if(srTransRec.byTransType == SALE_TIP)
    {
        szOriginTipTrType = srTransRec.byOrgTransType;
        srTransRec.byOrgTransType = srTransRec.byTransType;
    }
    else //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - END
        srTransRec.byOrgTransType = srTransRec.byTransType;

    srTransRec.byTransType = VOID;

    return d_OK;
}


int inCTOS_VoidSelectHost(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;

	vdDebug_LogPrintf("--inCTOS_VoidSelectHost--");

    inDatabase_TerminalOpenDatabase();
	
    inHostIndex = (short) srTransRec.HDTid;

	vdDebug_LogPrintf("inHostIndex[%d]", inHostIndex);
    
     DebugAddSTR("inCTOS_VoidSelectHost","Processing...",20);

	
    if ( inHDTReadEx(inHostIndex) != d_OK)
    {
         inDatabase_TerminalCloseDatabase();
         vdSetErrorMessage("HOST SELECTION ERR");
         return(d_NO);
    } 
    else 
    {
        
        srTransRec.HDTid = inHostIndex;

		vdDebug_LogPrintf("srTransRec.HDTid[%d]", srTransRec.HDTid);
		vdDebug_LogPrintf("srTransRec.MITid[%d]", srTransRec.MITid);		

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTReadEx(inCurrencyIdx) != d_OK) {

        inDatabase_TerminalCloseDatabase();
        vdSetErrorMessage("LOAD CST ERR");
        return(d_NO);
        }

		vdDebug_LogPrintf("inMMTReadRecord inHostIndex[%d]", inHostIndex);
		vdDebug_LogPrintf("inMMTReadRecord HDTid[%d]", srTransRec.HDTid);
		vdDebug_LogPrintf("inMMTReadRecord MITid[%d]", srTransRec.MITid);
		
        if ( inMMTReadRecordEx(inHostIndex,srTransRec.MITid) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }

		vdDebug_LogPrintf("inCPTRead inHostIndex[%d]", inHostIndex);
        if ( inCPTReadEx(inHostIndex) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

		vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);
		
        inCTOS_PreConnect();

        //return (d_OK);
    }
	
	inDatabase_TerminalCloseDatabase();
	return (d_OK);

}

int inCTOS_ChkBatchEmpty()
{
    int         inResult;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	BYTE szTitle[30+1];

	vdDebug_LogPrintf("saturn --inCTOS_ChkBatchEmpty--");
	
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
    memset(&srAccumRec,0,sizeof(srAccumRec));
    memset(&strFile,0,sizeof(strFile));
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    memset(&strFile,0,sizeof(strFile));
    vdCTOS_GetAccumName(&strFile, &srAccumRec);

	inResult = inMyFile_CheckFileExist(strFile.szFileName);
	vdDebug_LogPrintf("inResult[%d]", inResult);
	vdDebug_LogPrintf("strMMT[0].fMustSettFlag[%d]", strMMT[0].fMustSettFlag);
	
    if(inResult < 0)
    {
        if(CN_TRUE == strMMT[0].fMustSettFlag)
        {
            strMMT[0].fMustSettFlag = CN_FALSE;
            inMMTSave(strMMT[0].MMTid);
        }
        
        //CTOS_LCDTClearDisplay();
        //vdSetErrorMessage("BATCH EMPTY");
        //vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
        #if 0
		memset(szDisMsg, 0x00, sizeof(szDisMsg));
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "BATCH EMPTY");
		vdDebug_LogPrintf("szDisMsg = [%s]", szDisMsg);
		usCTOSS_LCDDisplay(szDisMsg);
		#else
		vdDebug_LogPrintf("saturn batch empty hostnum = %d", srTransRec.HDTid);		
		vdDisplayMessageBox(1, 8, "", "BATCH EMPTY", "", MSG_TYPE_ERROR);
		#endif
		CTOS_Beep();
		CTOS_Delay(1500);
        return (d_NO);
    }
    
    return (d_OK);
}

int inCTOS_ConfirmInvAmt()
{
    BYTE szAmtTmp1[16+1];
    BYTE szAmtTmp2[16+1];
    BYTE szAmtTmp3[16+1];
	BYTE szAmtTmp4[16+1];
	BYTE szAmtTmp5[16+1];
    unsigned long inDCCAmount;
    BYTE szDCCAmt[12+1];
    char szDisplayBuf[512];
    BYTE   key;
    USHORT result;
	
    //BYTE szTransTitle[20+1];
	BYTE szTemp1[30+1];

	
   BYTE szTransTitle[100+1];
   BYTE szOrgTransTitle[100+1];
   BYTE szDisMsg[30];
   char szTempDate[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp2[d_LINE_SIZE + 1];
   char szTemp3[d_LINE_SIZE + 1];
   char szTemp4[d_LINE_SIZE + 1];
   char szTemp5[d_LINE_SIZE + 1];

    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    int inUTF8 = 1;

   BYTE szDisplayName[20+1];


    memset(szTransTitle, 0x00, sizeof(szTransTitle));
    memset(szOrgTransTitle, 0x00, sizeof(szOrgTransTitle));
    //CTOS_LCDTClearDisplay();
    //vduiLightOn();
    //vdDispTransTitle(srTransRec.byTransType);

    //vdClearNonTitleLines();
    //vdClearBelowLine(2);
    //inCTOS_DisplayCardTitle(3, 4, FALSE);

// fix for Wrong implementation of USD Currency
//USD currency prompts upon voiding & settlement
        inTCTRead(1);	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtTmp1, 0x00, sizeof(szAmtTmp1));
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2)); 
    if(srTransRec.fDCCOptin==TRUE)
    	{
                  sprintf(szDisplayBuf, "%s", srTransRec.szDCCCurrencySymbol);
                  
                  inDCCAmount=atol((char *)srTransRec.szForeignAmount);
                  sprintf(szDCCAmt,"%ld",inDCCAmount);    
                  
                  inCSTReadCurrencySymbol(srTransRec.szCurrencyCode, srTransRec.szDCCCurrencySymbol, srTransRec.szAmountFormat);
	        
                  gblinMinorUnit=strCST.inMinorUnit;
                  
                 // vdDCCModifyAmount(szDCCAmt,&szAmtTmp2);
                 vdCTOS_FormatAmount(srTransRec.szAmountFormat, szDCCAmt, &szAmtTmp2);
                 
    	}
    else
        {
             sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
             wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
             //sprintf(szAmtTmp2, "%lu.%02lu", atol(szAmtTmp1)/100, atol(szAmtTmp1)%100);
             vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp2);
        }
   
    
    //CTOS_LCDTPrintXY(1, 5, "AMOUNT");
    //setLCDPrint(5, DISPLAY_POSITION_LEFT, "AMOUNT");

    if(srTransRec.byTransType == SALE_TIP)
    {
          DebugAddSTR("inCTOS_GetTipAfjustAmount","Processing...",20);

	memset(szTemp1,0,sizeof(szTemp1));
	vdCTOS_FormatPANEx(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);

	if(strIIT.inIssuerNumber != 8)
         TrimTrail(srTransRec.szCardholderName); // Trim Name -- sidumili	

    inUTF8 = is_utf8(srTransRec.szCardholderName);
    
    vdDebug_LogPrintf("AAA - srTransRec.szCardholderName[%s]", srTransRec.szCardholderName);
    if (inUTF8 == FALSE) //Cardholder name has special characters
    {
         memset(srTransRec.szCardholderName,0, sizeof(srTransRec.szCardholderName));
         strcpy(srTransRec.szCardholderName, " ");
    }

    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, AMT_BCD_SIZE);
	memset(szAmtTmp4, 0x00, sizeof(szAmtTmp4));
	memset(szAmtTmp5, 0x00, sizeof(szAmtTmp5));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", baBaseAmount, szAmtTmp4);
    sprintf(szAmtTmp5, "%s %s", strCST.szCurSymbol,szAmtTmp4);
          
    memset(baTipAmount, 0x00, sizeof(baTipAmount));
    wub_hex_2_str(srTransRec.szTipAmount, baTipAmount, 6);

	memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
	memset(szAmtTmp3, 0x00, sizeof(szAmtTmp3));
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", baTipAmount, szAmtTmp2);
    sprintf(szAmtTmp3, "%s %s", strCST.szCurSymbol,szAmtTmp2);

    memset(szDisplayName,0,sizeof(szDisplayName));
    strncpy (szDisplayName, srTransRec.szCardholderName, 19);
          
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    szGetTransTitle(srTransRec.byTransType,szTransTitle);
    strcpy(szDisplayBuf,szTransTitle);		
    strcat(szDisplayBuf,"|");
	strcat(szDisplayBuf,szTemp1);
	strcat(szDisplayBuf,"|");
    strcat(szDisplayBuf,szDisplayName);
	strcat(szDisplayBuf,"|SALE AMOUNT:|");
    strcat(szDisplayBuf,szAmtTmp5);
    strcat(szDisplayBuf,"|ORIG TIP AMOUNT:|");
    strcat(szDisplayBuf,szAmtTmp3);
	strcat(szDisplayBuf,"|CONFIRM TIP ADJUST?");
	  
//    key = usCTOSS_ConfirmTip(szDisplayBuf);    

    if(key == d_OK)
    {
		/*strcpy(szDisMsg, szTransTitle);
        strcat(szDisMsg, "|");
        strcat(szDisMsg, "PROCESSING...");
        usCTOSS_LCDDisplay(szDisMsg);*/
        vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
        return d_OK;
    }

	if (key == d_USER_CANCEL) /*timeout and key cancel*/
    {
       	//memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
       	//strcpy(szDisplayBuf, szTransTitle);
       	//strcat(szDisplayBuf, "|");
       	//strcat(szDisplayBuf, "USER CANCEL");
       	//usCTOSS_LCDDisplay(szDisplayBuf);

		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
        CTOS_Beep();
        CTOS_Delay(1000);
        return d_NO;
    }

	// sidumili: timeout display message
	if (key == d_TIMEOUT)
	{
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
        CTOS_Delay(2000);
		return d_NO;
	}
        
        if (key != d_OK)
             return d_NO;
				
	return d_OK;
    }
    if(srTransRec.byTransType == VOID)
    {
		memset(szTemp1,0,sizeof(szTemp1));
		vdCTOS_FormatPANEx(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);
		
		szGetTransTitle(srTransRec.byTransType,szTransTitle);
        szGetTransTitle(srTransRec.byOrgTransType,szOrgTransTitle);
		memset(szDisplayBuf,0x00,sizeof(szDisplayBuf));
		memset(szAmtTmp1, 0x00, sizeof(szAmtTmp1));

          vdDebug_LogPrintf("AAA - szAmtTmp2[%s]", szAmtTmp2);
		  
       if (srTransRec.inMinorUnit ==0 )
             purgeleadingchar('0',szAmtTmp2,&szAmtTmp3[0]);
       else
	   memcpy(&szAmtTmp3[0],szAmtTmp2,sizeof(szAmtTmp3));
       	

	   vdDebug_LogPrintf("AAA - szAmtTmp3[%s]", szAmtTmp3);
	   
       if(srTransRec.fDCCOptin == TRUE)
	   	sprintf(szAmtTmp1,"%s %s",srTransRec.szDCCCurrencySymbol,szAmtTmp3); 
       else
		sprintf(szAmtTmp1,"%s %s",strCST.szCurSymbol,szAmtTmp3);

        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
        wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);

        vdDebug_LogPrintf("szTemp = [%s]", szTemp);
        vdDebug_LogPrintf("szTemp2 = [%s]", szTemp2);

        memset(szTempDate, 0x00, sizeof(szTempDate));

		//Date
		vdDebug_LogPrintf("Get date");
        vdDebug_LogPrintf("szTranYear[%s]", srTransRec.szTranYear);
		if (strlen(srTransRec.szTranYear) > 0)
        {
            sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szTranYear), atol(szTemp)/100, atol(szTemp)%100);
        }
		else
		    sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
		vdDebug_LogPrintf("szTempDate: [%s]", szTempDate);

		vdCTOS_FormatDate(szTempDate);
		//sprintf(szTemp3, "DATE: %s", szTempDate);
        //vdDebug_LogPrintf("szTemp3: [%s]", szTemp3);
        vdDebug_LogPrintf("szTempDate: [%s]", szTempDate);
		
		//Time
		sprintf(szTemp4, "%02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
		vdDebug_LogPrintf("TIME: [%s]", szTemp4);

		vdDebug_LogPrintf("srTransRec.byOrgTransType: %d", srTransRec.byOrgTransType);
        vdDebug_LogPrintf("srTransRec.szAuthCode: %s", srTransRec.szAuthCode);
        vdDebug_LogPrintf("srTransRec.szRRN: %s", srTransRec.szRRN);

        wub_hex_2_str(srTransRec.szInvoiceNo, szTemp5, INVOICE_BCD_SIZE);

        vdDebug_LogPrintf("srTransRec.szRRN: %s", srTransRec.szInvoiceNo);

                
		
		strcpy(szDisplayBuf,szTransTitle);		
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,szOrgTransTitle);
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,szAmtTmp1);
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,szTemp1);		//CardNumber
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,srTransRec.szAuthCode);
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,srTransRec.szRRN);
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,szTemp5);
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,szTempDate);
		strcat(szDisplayBuf,"|");
		strcat(szDisplayBuf,szTemp4);


		vdDebug_LogPrintf("saturn usCTOSS_ConfirmMenu szDisMsg[%s]", szDisplayBuf);
		
//		key = usCTOSS_ConfirmInvAndAmt(szDisplayBuf);

		if (key == d_USER_CANCEL)
		{
            if (fGetECRTransactionFlg())
            {
                 memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
                 memcpy(srTransRec.szECRRespCode,ECR_UNKNOWN_ERR,2);
            }
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			CTOS_Delay(1000);
			return d_NO;
		}

		// sidumili: timeout display message
		if (key == d_TIMEOUT)
		{
		    if (fGetECRTransactionFlg())
            {
                 memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
                 memcpy(srTransRec.szECRRespCode,ECR_TIMEOUT_ERR,2);
            }
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
	        CTOS_Delay(2000);
			return d_NO;
		}
    }
		else
			 setLCDPrint(8, DISPLAY_POSITION_CENTER,"NO[X] YES[OK]");
    
		CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
    vduiWarningSound();

    CTOS_KBDBufFlush();//cleare key buffer
    
    while(1)
    { 
        
        //CTOS_KBDHit(&key);
        if(key == d_OK)
        {
            result = d_OK;
			//vdClearBelowLine(2);
			//vdCTOS_DispStatusMessage("PROCESSING...");  
    		/*strcpy(szDisMsg, szTransTitle);
    		strcat(szDisMsg, "|");
    		strcat(szDisMsg, "PROCESSING...");
    		usCTOSS_LCDDisplay(szDisMsg);*/
    		vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
            break;
        }
        else if((key == d_NO))
        {
            result = d_NO;
            //vdSetErrorMessage("USER CANCEL");
            //strcpy(szDisMsg, szTransTitle);
    		//strcat(szDisMsg, "|");
    		//strcat(szDisMsg, "USER CANCEL");
    		//usCTOSS_LCDDisplay(szDisMsg);
			CTOS_Beep();		
			CTOS_Delay(1500);
			usCTOSS_LCDDisplay(" ");
            break;
        }
        if(key == d_TIMER_INVALID_PARA)
        {
            result = d_NO;
            //vdSetErrorMessage("TIME OUT");
            
            result=d_NO;
            /*strcpy(szDisMsg, szTransTitle);
    		strcat(szDisMsg, "|");
    		strcat(szDisMsg, "TIME OUT");
    		usCTOSS_LCDDisplay(szDisMsg);*/
           vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
           CTOS_Beep();		
           CTOS_Delay(1500);
            break;
        }       
    }

    
    return result;
}

int inCTOS_ConfirmTipAdj(void)
{
	BYTE szAmtTmp1[16+1];
    BYTE szAmtTmp2[16+1];
    BYTE szAmtTmp3[16+1];
	BYTE szAmtTmp4[16+1];
	BYTE szAmtTmp5[16+1];
    unsigned long inDCCAmount;
    BYTE szDCCAmt[12+1];
    char szDisplayBuf[1024];
    BYTE   key;
    USHORT result;
	
    //BYTE szTransTitle[20+1];
    BYTE szTemp1[30+1];
    BYTE szDisplayName[20+1];

	
   BYTE szTransTitle[100+1];
   BYTE szOrgTransTitle[100+1];
   BYTE szDisMsg[30];
   char szTempDate[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp2[d_LINE_SIZE + 1];
   char szTemp3[d_LINE_SIZE + 1];
   char szTemp4[d_LINE_SIZE + 1];
   char szTemp5[d_LINE_SIZE + 1];

    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    int inUTF8 = 1;
#if 0
	memset(szTransTitle, 0x00, sizeof(szTransTitle));
    memset(szOrgTransTitle, 0x00, sizeof(szOrgTransTitle));

	inTCTRead(1);	
	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtTmp1, 0x00, sizeof(szAmtTmp1));
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2)); 
    if(srTransRec.fDCCOptin==TRUE)
    	{
			sprintf(szDisplayBuf, "%s", srTransRec.szDCCCurrencySymbol);
                  
            inDCCAmount=atol((char *)srTransRec.szForeignAmount);
            sprintf(szDCCAmt,"%ld",inDCCAmount);    
                  
            inCSTReadCurrencySymbol(srTransRec.szCurrencyCode, srTransRec.szDCCCurrencySymbol, srTransRec.szAmountFormat);
	        
            gblinMinorUnit=strCST.inMinorUnit;
                  
            // vdDCCModifyAmount(szDCCAmt,&szAmtTmp2);
            vdCTOS_FormatAmount(srTransRec.szAmountFormat, szDCCAmt, &szAmtTmp4);
			sprintf(szAmtTmp2, "%s %s", strCST.szCurSymbol,szAmtTmp4);
                 
    	}
    else
        {
             sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
             wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
             //sprintf(szAmtTmp2, "%lu.%02lu", atol(szAmtTmp1)/100, atol(szAmtTmp1)%100);
             vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp4);
			 sprintf(szAmtTmp2, "%s %s", strCST.szCurSymbol,szAmtTmp4);
        }

	memset(szTemp1,0,sizeof(szTemp1));
	vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);
	
	if(strIIT.inIssuerNumber != 8)
	     TrimTrail(srTransRec.szCardholderName); // Trim Name -- sidumili	

     inUTF8 = is_utf8(srTransRec.szCardholderName);
     
     vdDebug_LogPrintf("saturn - srTransRec.szCardholderName[%s]", srTransRec.szCardholderName);
     if (inUTF8 == FALSE) //Cardholder name has special characters
     {
          memset(srTransRec.szCardholderName,0, sizeof(srTransRec.szCardholderName));
          strcpy(srTransRec.szCardholderName, " ");
     }
    	
    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, AMT_BCD_SIZE);
	memset(szAmtTmp4, 0x00, sizeof(szAmtTmp4));
	memset(szAmtTmp5, 0x00, sizeof(szAmtTmp5));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", baBaseAmount, szAmtTmp4);
    sprintf(szAmtTmp5, "%s %s", strCST.szCurSymbol,szAmtTmp4);
          
    memset(baTipAmount, 0x00, sizeof(baTipAmount));
    wub_hex_2_str(srTransRec.szTipAmount, baTipAmount, 6);

	memset(szAmtTmp3, 0x00, sizeof(szAmtTmp3));
	memset(szAmtTmp4, 0x00, sizeof(szAmtTmp4));
    //sprintf(szAmtTmp3, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", baTipAmount, szAmtTmp4);
    sprintf(szAmtTmp3, "%s %s", strCST.szCurSymbol,szAmtTmp4);
          
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    szGetTransTitle(srTransRec.byTransType,szTransTitle);

   memset(szDisplayName, 0x00, sizeof(szDisplayName));
   strncpy (szDisplayName, srTransRec.szCardholderName, 19);

   strcpy(szDisplayBuf,szTransTitle);		
   strcat(szDisplayBuf,"|");
   strcat(szDisplayBuf,szTemp1);
   strcat(szDisplayBuf,"|");
   strcat(szDisplayBuf,szDisplayName);
   strcat(szDisplayBuf,"|SALE AMOUNT:|");
   strcat(szDisplayBuf,szAmtTmp5);
   strcat(szDisplayBuf,"|TIP ADJUST:|");
   strcat(szDisplayBuf,szAmtTmp3);
   strcat(szDisplayBuf,"|");
   strcat(szDisplayBuf,szAmtTmp2);
   strcat(szDisplayBuf,"|TOTAL AMOUNT");
	  
    key = usCTOSS_ConfirmTip(szDisplayBuf);    

    if(key == d_OK)
    {
		/*strcpy(szDisMsg, szTransTitle);
        strcat(szDisMsg, "|");
        strcat(szDisMsg, "PROCESSING...");
        usCTOSS_LCDDisplay(szDisMsg);*/
        vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
        return d_OK;
    }

	if (key == d_USER_CANCEL) /*timeout and key cancel*/
    {
       	//memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
       	//strcpy(szDisplayBuf, szTransTitle);
       	//strcat(szDisplayBuf, "|");
       	//strcat(szDisplayBuf, "USER CANCEL");
       	//usCTOSS_LCDDisplay(szDisplayBuf);
       	memcpy(srTransRec.szTotalAmount, srTransRec.szOrgAmount, 6);
        memcpy(srTransRec.szTipAmount, srTransRec.szOrgTipAmount, 6);

		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
        CTOS_Beep();
        CTOS_Delay(1000);
        return d_NO;
    }

	// sidumili: timeout display message
	if (key == d_TIMEOUT)
	{
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
        CTOS_Delay(2000);
		return d_NO;
	}
        
        if (key != d_OK)
             return d_NO;
#endif				
	return d_OK;

}


int inCTOS_SettlementSelectAndLoadHost(void)
{
    int key;
    char szBcd[INVOICE_BCD_SIZE+1];
	BYTE szTitle[25+1];
	BYTE szDisMsg[100];

	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

#if 1
    key = inCTOS_SelectHostSetting();
    if (key == -1)
    {
		/*strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "USER CANCEL");
		usCTOSS_LCDDisplay(szDisMsg);*/
		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_WARNING);
		CTOS_Beep();
		CTOS_Delay(1500);
		return d_NO;
        //return key;
    }
#else
	key = inCTOS_AutoSelectHostSetting();

#endif

	
    vdDebug_LogPrintf("saturn inCTOS_SettlementSelectAndLoadHost strHDT.inHostIndex[%d]", strHDT.inHostIndex);

    memset(szBcd, 0x00, sizeof(szBcd));
    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    inHDTSave(strHDT.inHostIndex);

    return d_OK;
}

int inCTOS_SettlementClearBathAndAccum(BOOL fManualSettle)
{
    int         inResult;
    BYTE    szBcd[INVOICE_BCD_SIZE+1];
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    vdDebug_LogPrintf("inCTOS_SettlementClearBathAndAccum");

	// Minimize the white screend display -- sidumili
	if (isCheckTerminalMP200() == d_OK && !fManualSettle)	
		vdCTOS_DispStatusMessage("PROCESSING...");
	
    memset(szBcd, 0x00, sizeof(szBcd));
    memcpy(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);    
    inBcdAddOne(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);
  
    strMMT[0].fMustSettFlag = CN_FALSE;
    inMMTSave(strMMT[0].MMTid);

	strMMT[0].fBatchNotEmpty = 0;	
	inMMTBatchNotEmptySave(strMMT[0].MMTid);       
    
    inDatabase_BatchDelete();

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    inCTOS_ReadAccumTotal(&srAccumRec);
    strcpy(srAccumRec.szTID, srTransRec.szTID);
    strcpy(srAccumRec.szMID, srTransRec.szMID);
    memcpy(srAccumRec.szYear, srTransRec.szYear, DATE_BCD_SIZE);
    memcpy(srAccumRec.szDate, srTransRec.szDate, DATE_BCD_SIZE);
    memcpy(srAccumRec.szTime, srTransRec.szTime, TIME_BCD_SIZE);
    memcpy(srAccumRec.szBatchNo, srTransRec.szBatchNo, BATCH_NO_BCD_SIZE);

	vdDebug_LogPrintf("inCTOS_SettlementClearBathAndAccum|time[%02x:%02x:%02x]date[%02x][%02x]",srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);
	
	srAccumRec.fManualSettle=fManualSettle;
    inCTOS_SaveAccumTotal(&srAccumRec);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    memset(&strFile,0,sizeof(strFile));
    vdCTOS_GetAccumName(&strFile, &srAccumRec);
    vdDebug_LogPrintf("[strFile.szFileName[%s]", strFile.szFileName);
    vdCTOS_SetBackupAccumFile(strFile.szFileName);
    if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
    }

    inDatabase_InvoiceNumDelete(srTransRec.HDTid, srTransRec.MITid);
		
    inMyFile_ReversalDelete();

    inMyFile_AdviceDelete();

    inMyFile_TCUploadDelete();

    vdLinuxCommandClearERMBitmap(); /*albert - ERM*/
	
    return d_OK;
}

#if 0
int inCTOS_DisplayBatchTotal(void)
{
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    ULONG ulSaleTotalAmount;
    USHORT usRefundCount;
    ULONG  ulRefundTotalAmount;
    ACCUM_REC srAccumRec;

	
    BYTE szDisplayBuf1[40];
	BYTE szDisplayBuf2[40];
	BYTE szDisplayBuf3[40];
	
	int key=d_NO;
	BYTE szDispUI[128];

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(BATCH_TOTAL);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;    
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

#if 0
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    strcpy(szDisplayBuf, "TYPE  CNT  AMT");
    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
#endif
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulSaleTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    sprintf(szDisplayBuf, "SALE  %03d  %s", usSaleCount, szAmtBuf);
    //CTOS_LCDTPrintXY(1, 5, szDisplayBuf);

	/*
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulRefundTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    sprintf(szDisplayBuf, "RFND  %03d  %s", usRefundCount, szAmtBuf);
    CTOS_LCDTPrintXY(1, 6, szDisplayBuf);
    */

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount-ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    memset(szBuf, 0x00, sizeof(szBuf));
    if(ulSaleTotalAmount > ulRefundTotalAmount)
        szBuf[0] = ' ';
    else
        szBuf[0] = '-';
    sprintf(szDisplayBuf, "TOTL  %03d %s%s", (usSaleCount + usRefundCount), szBuf, szAmtBuf);
    //CTOS_LCDTPrintXY(1, 8, szDisplayBuf);

    //WaitKey(30);
    
	strcpy(szDispUI, "BATCH TOTAL|SALE:|");
	strcat(szDispUI, szDisplayBuf1);
	strcat(szDispUI, "|VOID:|");
	strcat(szDispUI, szDisplayBuf2);
	strcat(szDispUI, "|TOTAL:|");
	strcat(szDispUI, szDisplayBuf3);

	key = usCTOSS_ConfirmOK(szDispUI);

    return d_OK;
}
#endif

int inCTOS_DisplaySettleBatchTotal(BYTE byTransType, BOOL fConfirm)
{
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    ULONG ulSaleTotalAmount;
    USHORT usRefundCount;
    ULONG  ulRefundTotalAmount;
    ACCUM_REC srAccumRec;
    BYTE byKeyBuf; 
    USHORT usVoidCount;
    ULONG ulVoidTotalAmount;
    int inRow=2;
	
	//RTR
    USHORT usRedeemCount;
    ULONG ulRedeemTotalAmount;
	
	//Regular
    USHORT usRegularCount;
    ULONG ulRegularTotalAmount;
	
	//REduce
    USHORT usReduceCount;
    ULONG ulReduceTotalAmount;
	
	//Zero
    USHORT usZeroCount;
    ULONG ulZeroTotalAmount;

	//BNPL
    USHORT usBNPLCount;
    ULONG ulBNPLTotalAmount;

	//BNPL
    USHORT us2GOCount;
    ULONG ul2GOTotalAmount;
	
	
	//CASH
    USHORT usCashCount;
    ULONG ulCashTotalAmount;
	
//sale completion
    USHORT usCompCount;
    ULONG ulCompTotalAmount;

	USHORT usTotalCount;
    ULONG ulTotalAmount;

     int key=d_NO;
     BYTE szDispUI[400];

     BYTE szDisplayBuf0[20+1];
     BYTE szDisplayBuf1[40];
     BYTE szDisplayBuf2[40];
     BYTE szDisplayBuf3[40];
     BYTE szDisplayBuf4[40];
     BYTE szDisplayBuf5[40];
	
	BYTE szTitle[25+1];

	int inConfirmTimeOut = 30;

	BYTE szCount[40];
	BYTE szTemp[40];


	vdDebug_LogPrintf("SATURN inCTOS_DisplaySettleBatchTotal");

     if (srTransRec.fAutoSettleFlag == TRUE)
	{
		fConfirm = FALSE;
		inConfirmTimeOut = 1;
	}
	

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(byTransType);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;    
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

    usVoidCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount);
    ulVoidTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);

    usRedeemCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount);
    ulRedeemTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount);

    usRegularCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount);
    ulRegularTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount);

    usReduceCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount);
    ulReduceTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount);

    usZeroCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount);
    ulZeroTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount);

    usBNPLCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount);
    ulBNPLTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount);

    us2GOCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount);
    ul2GOTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount);
	
    usCashCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount);
    ulCashTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount);

// sale completion
    usCompCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount);
    ulCompTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount);

	
// fix for Wrong implementation of USD Currency
//USD currency prompts upon voiding & settlement
        inTCTRead(1);	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

#if 1
//RTR START
if(usRedeemCount > 0){
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulRedeemTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
	{
	    sprintf(szDisplayBuf, "%s   %s", strCST.szCurSymbol, szAmtBuf);	    
		inDisplayLeftRight(inRow++, "RTR SALES",szDisplayBuf,20);		
	}
	else
	{
	    sprintf(szDisplayBuf, "%s %s", strCST.szCurSymbol, szAmtBuf);
	    //CTOS_LCDTPrintXY(1, inRow++, "RTR SALES"); 
	    //CTOS_LCDTPrintXY(1, inRow++, szDisplayBuf);
	}
}
//RTR END	
#endif
		
    //memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //strcpy(szDisplayBuf, "Type  Cnt  Amt");
    //CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
#if 0
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulSaleTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    sprintf(szDisplayBuf, "   %s   %s", strCST.szCurSymbol, szAmtBuf);
	CTOS_LCDTPrintXY(1, 4, "SALES");
    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
#else
    memset(szDisplayBuf1, 0x00, sizeof(szDisplayBuf1));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    usTotalCount = 0;
    ulTotalAmount = 0;

    usTotalCount = usSaleCount + usRedeemCount + usRegularCount + usReduceCount + usZeroCount + usBNPLCount +us2GOCount+ usCashCount; // for RTR total inclusion. 
    ulTotalAmount = ulSaleTotalAmount +  ulRedeemTotalAmount + ulRegularTotalAmount + ulReduceTotalAmount + ulZeroTotalAmount + ulBNPLTotalAmount +ul2GOTotalAmount+ ulCashTotalAmount; // for RTR total inclusion. 
    sprintf(szBuf, "%ld", ulTotalAmount); // credit + RTR + installment  sale
   //sprintf(szBuf, "%ld", ulSaleTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount); // credit + RTR + installment  sale
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
	{
	    sprintf(szDisplayBuf, "%s   %s", strCST.szCurSymbol, szAmtBuf);    
		inDisplayLeftRight(inRow++, "SALES",szDisplayBuf,20);
	}
	else
	{
	    
		vdDebug_LogPrintf("SATURN inCTOS_DisplaySettleBatchTotal get sales count");
	    //sprintf(szDisplayBuf, "   %s   %s", strCST.szCurSymbol, szAmtBuf);
		sprintf(szDisplayBuf1,"%s %s",strCST.szCurSymbol,szAmtBuf);
		//CTOS_LCDTPrintXY(1, inRow++, "SALES");
	    //CTOS_LCDTPrintXY(1, inRow++, szDisplayBuf);
	}

#endif
    //memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    //memset(szBuf, 0x00, sizeof(szBuf));
    //sprintf(szBuf, "%ld", ulVoidTotalAmount);
    //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    //sprintf(szDisplayBuf, "   %s-  %s", strCST.szCurSymbol, szAmtBuf);
	//CTOS_LCDTPrintXY(1, 3, "TOTAL VOID");	
    //CTOS_LCDTPrintXY(1, 4, szDisplayBuf);

#if 1
//CASH ADV START
if(usCashCount > 0){
    memset(szDisplayBuf2, 0x00, sizeof(szDisplayBuf2));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulCashTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
	{
	    //sprintf(szDisplayBuf, "%s   %s", strCST.szCurSymbol, szAmtBuf);    
		//inDisplayLeftRight(inRow++, "CASH ADV",szDisplayBuf,20);
	}
	else
	{
	    sprintf(szDisplayBuf2, "%s %s", strCST.szCurSymbol, szAmtBuf);
	    //CTOS_LCDTPrintXY(1, inRow++, "CASH ADV"); inRow++;
	    //CTOS_LCDTPrintXY(1, inRow++, szDisplayBuf);
	}
}
//CASH ADV END	
#endif
if(usRefundCount > 0){
    memset(szDisplayBuf3, 0x00, sizeof(szDisplayBuf3));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulRefundTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
	{
	    //sprintf(szDisplayBuf, "%s   %s", strCST.szCurSymbol, szAmtBuf);    
		//inDisplayLeftRight(inRow++, "REFUND",szDisplayBuf,20);
	}
	else
	{
	    sprintf(szDisplayBuf3, "%s- %s", strCST.szCurSymbol, szAmtBuf);
		//CTOS_LCDTPrintXY(1, inRow++, "REFUND");
	    //CTOS_LCDTPrintXY(1, inRow++, szDisplayBuf);
	}
}
#if 1
//SALE COMP START
if(usCompCount > 0){
    memset(szDisplayBuf4, 0x00, sizeof(szDisplayBuf4));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulCompTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
	{
	    //sprintf(szDisplayBuf4, "%s   %s", strCST.szCurSymbol, szAmtBuf);    
		//inDisplayLeftRight(inRow++, "PRE COMP",szDisplayBuf,20);
	}
	else
	{
	    sprintf(szDisplayBuf4, "%s %s", strCST.szCurSymbol, szAmtBuf);
	    //CTOS_LCDTPrintXY(1, inRow++, "PRE COMP");
	    //CTOS_LCDTPrintXY(1, inRow++, szDisplayBuf);
	}
}
//SALE COMP END	
#endif

	#if 0
    memset(szDisplayBuf5, 0x00, sizeof(szDisplayBuf5));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
	
	// Total Amount
    ulSaleTotalAmount = ulSaleTotalAmount +  ulRedeemTotalAmount + ulRegularTotalAmount + ulReduceTotalAmount + ulZeroTotalAmount + ulBNPLTotalAmount +ul2GOTotalAmount+ ulCashTotalAmount + ulCompTotalAmount; // for RTR total inclusion. 
    
    sprintf(szBuf, "%ld", (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount-ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
	
    memset(szBuf, 0x00, sizeof(szBuf));
    if(ulSaleTotalAmount > ulRefundTotalAmount)
        szBuf[0] = ' ';
    else
        szBuf[0] = '-';
	szBuf[1]=0x00;


	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
	{		
	    //sprintf(szDisplayBuf, "%s%s  %s", strCST.szCurSymbol, szBuf, szAmtBuf);
		//inDisplayLeftRight(7, "TOTALS",szDisplayBuf,20);
	}
	else
	{
	    //sprintf(szDisplayBuf, "   %s%s  %s", strCST.szCurSymbol, szBuf, szAmtBuf);
		//CTOS_LCDTPrintXY(1, 12, "TOTALS");
	    //CTOS_LCDTPrintXY(1, 13, szDisplayBuf);
	    sprintf(szDisplayBuf5, "%s%s%s", strCST.szCurSymbol, szBuf, szAmtBuf);
	}
	#endif

#if 0
	if (!fConfirm)
	{
                       //WaitKey(30);
                        vdDebug_LogPrintf("SATURN  inCTOS_DisplaySettleBatchTotal confirm");
                        strcpy(szDispUI, "BATCH TOTAL2|SALES:|");
                        strcat(szDispUI, szDisplayBuf1);
                        strcat(szDispUI, "| ");
                        strcat(szDispUI, "|TOTAL:|");
                        strcat(szDispUI, szDisplayBuf3);
                        
                        vdDebug_LogPrintf("SATURN  inCTOS_DisplaySettleBatchTotal %s", szDispUI);
                        if (inConfirmTimeOut == 1)
                        {
                             usCTOSS_DisplayUI(szDispUI);
                             CTOS_Delay(10000);
                             return d_OK;
                        }
		     else
                       {
                            key = usCTOSS_ConfirmOK(szDispUI);
                            return d_OK;
                       }                   
                  
	}
	else
	{
		if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
		{		
			vduiDisplayStringCenter(8,"NO[X] YES[OK]");
		}
		else
		{
		    CTOS_LCDTPrintXY(1, 15, "NO[X] YES[OK]");
		}
	
		vduiWarningSound();
		
		CTOS_TimeOutSet(TIMER_ID_1, 6000);/* 6000 means 60 sec  */
	    while (1)
	    {        
	        CTOS_KBDHit(&byKeyBuf);
	        if(byKeyBuf == d_KBD_CANCEL)
	        {
	            CTOS_KBDBufFlush ();
	            return d_NO;
	        }
			else if(byKeyBuf == d_KBD_ENTER)
			{
				
	            CTOS_KBDBufFlush();
	            return d_OK;
			}
			
			if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
	            return d_NO;               			
	    }
	}
#else

      //TEST
      //inConfirmTimeOut = 1;

      vdDebug_LogPrintf("SATURN  inCTOS_DisplaySettleBatchTotal confirm");

	  memset(szDispUI, 0x00, sizeof(szDispUI));		  
      if (srTransRec.byTransType == SETTLE)
      {	      				
      	//strcpy(szDispUI, "SETTLEMENT|SALE:|");
              memset(szCount, 0x00, sizeof(szCount));
              memset(szTemp, 0x00, sizeof(szTemp));	
			  
              //sprintf(szCount, "%03d", usSaleCount);
              sprintf(szCount, "%03d", usTotalCount);
              sprintf(szTemp, "SETTLEMENT|SALE:|%s|", szCount);
              strcpy(szDispUI, szTemp);
		
      }
        
      else
      {
             memset(szCount, 0x00, sizeof(szCount));
             memset(szTemp, 0x00, sizeof(szTemp));	
		
            // sprintf(szCount, "%03d", usSaleCount);
            sprintf(szCount, "%03d", usTotalCount);
            //sprintf(szTemp, "BATCH TOTAL|SALE:|%s|", szCount);
            sprintf(szTemp, "BATCH TOTAL|SALE:|%s|", szCount);
            strcpy(szDispUI, szTemp);
      }
       
	   strcat(szDispUI, szDisplayBuf1);
	   strcat(szDispUI, "|");

	   // For Refund
	   if (usRefundCount > 0)
            {
                  memset(szCount, 0x00, sizeof(szCount));
	         memset(szTemp, 0x00, sizeof(szTemp));	
			 
                  sprintf(szCount, "%03d", usRefundCount);
                  sprintf(szTemp, "REFUND:|%s|", szCount);	   
                  strcat(szDispUI, szTemp);
                  strcat(szDispUI, (strlen(szDisplayBuf3) > 0 ? szDisplayBuf3 : "PHP 0.00"));
                  strcat(szDispUI, "|");
            }
	    else if (usRefundCount <= 0)
            {
                  strcat(szDispUI,  "| | |");
            }		 
       // For CompCount
            if (usCompCount > 0)
           {
                memset(szCount, 0x00, sizeof(szCount));
	       memset(szTemp, 0x00, sizeof(szTemp));	
		  
                sprintf(szCount, "%03d", usCompCount);
                sprintf(szTemp, "PRE COMP:|%s|", szCount);
                strcat(szDispUI, szTemp);
                strcat(szDispUI, (strlen(szDisplayBuf4) > 0 ? szDisplayBuf4 : "PHP 0.00"));
                strcat(szDispUI, "|");
           }	
	 else if (usCompCount <= 0)
            {
               strcat(szDispUI,  "| | | ");
            }
	   // Total Count / Amount
            usTotalCount = 0;
	   ulTotalAmount = 0;
            usTotalCount = usSaleCount + usRedeemCount + usRegularCount + usReduceCount + usZeroCount + usBNPLCount + us2GOCount + usCashCount + usCompCount + usRefundCount; // for RTR total inclusion. 
             ulTotalAmount = ulSaleTotalAmount +  ulRedeemTotalAmount + ulRegularTotalAmount + ulReduceTotalAmount + ulZeroTotalAmount + ulBNPLTotalAmount +ul2GOTotalAmount+ ulCashTotalAmount + ulCompTotalAmount; // for RTR total inclusion. 
            sprintf(szBuf, "%ld", (ulTotalAmount > ulRefundTotalAmount) ? (ulTotalAmount-ulRefundTotalAmount) : (ulRefundTotalAmount - ulTotalAmount));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	   memset(szBuf, 0x00, sizeof(szBuf));
	   if(ulTotalAmount > ulRefundTotalAmount)
	        szBuf[0] = ' ';
	   else
	        szBuf[0] = '-';
	   szBuf[1]=0x00;

	   sprintf(szDisplayBuf5, "%s%s%s", strCST.szCurSymbol, szBuf, szAmtBuf);

	   // For Total
	   memset(szCount, 0x00, sizeof(szCount));
	   memset(szTemp, 0x00, sizeof(szTemp));		
	   
	   sprintf(szCount, "%03d", usTotalCount);
	   sprintf(szTemp, "TOTALS:|%s|", szCount);
	   strcat(szDispUI, szTemp);			
	   strcat(szDispUI, szDisplayBuf5);

	   //merchant name
	    memset(szDisplayBuf0,0x00,sizeof(szDisplayBuf0));
	    memset(szTemp, 0x00, sizeof(szTemp));		
			  
              strncpy(szDisplayBuf0, strMMT[0].szMerchantName, 20);
              sprintf(szTemp, "|%s| | ", szDisplayBuf0);
              strcat(szDispUI, szTemp);
		   
	   vdDebug_LogPrintf("SATURN  inCTOS_DisplaySettleBatchTotal %s inConfirmTimeOut %d", szDispUI, inConfirmTimeOut);

	   
	   if (inConfirmTimeOut == 1)
	   {
			//usCTOSS_DisplayUI(szDispUI);
			key = usCTOSS_Confirm2(szDispUI);
			CTOS_Delay(3000);
			return d_OK;
	   }else{
	                   if(srTransRec,byTransType==SETTLE)
			     key = usCTOSS_Confirm(szDispUI);
			else
			     key = usCTOSS_Confirm3(szDispUI);
			
			if (key == d_USER_CANCEL) /*timeout and key cancel*/
			{
					 
				memset(szTitle, 0x00, sizeof(szTitle));
				szGetTransTitle(srTransRec.byTransType, szTitle);

				//memset(szDispUI, 0x00, sizeof(szDispUI));
				//strcpy(szDispUI, szTitle);
				//strcat(szDispUI, "|");
				//strcat(szDispUI, "USER CANCEL");
				//usCTOSS_LCDDisplay(szDispUI);

				vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
				CTOS_Beep();
				CTOS_Delay(1000);
				return d_NO;
			}

			// sidumili: timeout display message
			if (key == d_TIMEOUT)
			{
				vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
				CTOS_Beep();
		        CTOS_Delay(2000);
				return d_NO;
			}
				
			if (key != d_OK)
				return d_NO;
				
			return d_OK;
	   } 				  


#endif
	
    return d_OK;
}

int inCTOS_DisplayBatchRecordDetail(int inType, int inNavRecType)
{
    BYTE szTotalAmount[12+1];
    BYTE szAmtOut[12+1];
    BYTE szINV[6+1];
    BYTE szDisplayBuf[100+1];
    BYTE szTitle[25+1];

	BYTE szINVBuf[40+1];
	BYTE szPANBuf[40+1];
	BYTE szDisplayAmountBuf[40+1];
	BYTE inKey = d_NO;
	BYTE szDisplayBuf2[40+1];
	BYTE szDCCFormatTotalAmount[20+1] = {0};
	BYTE szAmtBuf2[20+1]={0};
    BYTE szDisplayInvNo[40+1];

	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	BYTE szDisplayDateTime[40+1];
	BYTE szDisplayIssuer[40+1];
	CTOS_RTC SetRTC;
	BYTE szNavRecType[2+1]={0};
  
    //CTOS_LCDTClearDisplay(); 

	vdDebug_LogPrintf("inNavRecType=[%d]", inNavRecType);

	memset(szNavRecType, 0x00, sizeof(szNavRecType));
	sprintf(szNavRecType, "%d", inNavRecType);

    vdDispTransTitle(inType);

    inIITRead(srTransRec.IITid);
    inCSTRead(1);
	
    memset(szTitle, 0x00, sizeof(szTitle));
	if(srTransRec.byTransType == PREAUTH_VOID)
		strcpy(szTitle, "PRE VOID");
	else	
        szGetTransTitle(srTransRec.byTransType, szTitle);
    memset(szDisplayBuf2, 0x00, sizeof(szDisplayBuf2));
    sprintf(szDisplayBuf2, "%s   %s", szTitle, strIIT.szIssuerLabel);
    //CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
    
	vdDebug_LogPrintf("SATURN inCTOS_BatchReviewFlow szDisplayBuf2 %s", szDisplayBuf2);

    memset(szDisplayBuf,0,sizeof(szDisplayBuf));
    vdCTOS_FormatPANEx(strIIT.szPANFormat, srTransRec.szPAN, szDisplayBuf, strIIT.fMaskPan);
    //CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
    memset(szPANBuf,0,sizeof(szPANBuf));
	strcpy(szPANBuf, szDisplayBuf);


    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    vdDebug_LogPrintf("szTotalAmount[%s]", szTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmount,szAmtOut);
    vdDebug_LogPrintf("szTotalAmount[%s]", szDisplayBuf);
    memset(szDisplayAmountBuf,0,sizeof(szDisplayAmountBuf));

	if (srTransRec.fDCCOptin == TRUE)
	{
		memset(szDCCFormatTotalAmount, 0x00, sizeof(szDCCFormatTotalAmount));
		memset(szAmtBuf2, 0x00, sizeof(szAmtBuf2));

		vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount); 	
		if (srTransRec.inMinorUnit == 0)
		{
			purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
			sprintf(szDisplayAmountBuf,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
		}
		else
		{
			sprintf(szDisplayAmountBuf, "%s%s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
		}
	}
	else
	{
		sprintf(szDisplayAmountBuf, "%s %s", strCST.szCurSymbol, szAmtOut);
	}
    
    //CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
    

   // Invoice No.
	memset(szINV, 0x00, sizeof(szINV));
	wub_hex_2_str(srTransRec.szInvoiceNo, szINV, 3);
	memset(szDisplayInvNo, 0x00, sizeof(szDisplayInvNo));
	sprintf(szDisplayInvNo, "%06ld", atol(szINV));
    //CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

    //if ((strTCT.byTerminalType%2) == 0)
    //{
    //    CTOS_LCDTPrintAligned(8, "UP->UP", d_LCD_ALIGNLEFT);
    //    CTOS_LCDTPrintAligned(8, "DOT->DOWN", d_LCD_ALIGNRIGHT);
    //}
    //else
    //    vdCTOS_LCDGShowUpDown(1,1);

	// Date Time	
	CTOS_RTCGet(&SetRTC);
	memset(szYear, 0x00, sizeof(szYear));
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
	memset(szTempDate, 0x00, sizeof(szTempDate));
	sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	vdCTOS_FormatDate(szTempDate);
	sprintf(szTemp3, "%s", szTempDate);
	sprintf(szTemp4, "%02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	memset(szDisplayDateTime, 0x00, sizeof(szDisplayDateTime));
	sprintf(szDisplayDateTime, "%s / %s", szTemp3, szTemp4);

	// Issuer
	memset(szDisplayIssuer,0x00,sizeof(szDisplayIssuer));
	strcpy(szDisplayIssuer, strIIT.szIssuerLabel);
    
	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	strcpy(szDisplayBuf, "BATCH REVIEW");	
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szTitle);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayIssuer);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayDateTime);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szPANBuf);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayAmountBuf);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayInvNo);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szNavRecType);
	
	vdDebug_LogPrintf("inCTOS_DisplayBatchRecordDetail, szDisplayBuf[%s]", szDisplayBuf);
	
	inKey=usCTOSS_BatchReviewUI(szDisplayBuf);
	
    return inKey;
}

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo)
{
    int inResult,inUpDateAdviceIndex;
    TRANS_DATA_TABLE srAdvTransTable;
    STRUCT_ADVICE strAdvice;
    
    memset((char *)&srAdvTransTable, 0, sizeof(TRANS_DATA_TABLE));
    memset((char *)&strAdvice, 0, sizeof(strAdvice));
    
    memcpy((char *)&srAdvTransTable, (char *)&srTransRec, sizeof(TRANS_DATA_TABLE));
    
    inResult = ST_SUCCESS;
    inUpDateAdviceIndex = 0;
    while(1)
    {
        inResult = inMyFile_AdviceReadByIndex(inUpDateAdviceIndex,&strAdvice,&srAdvTransTable);
        
        vdDebug_LogPrintf("ADVICE inUpDateAdviceIndex[%d] inMyFile_AdviceRead Rult(%d)(%d)(%d)(%d)", inUpDateAdviceIndex, inResult, srAdvTransTable.ulTraceNum, srAdvTransTable.byPackType, strAdvice.byTransType);
        
        if(inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA)
        {
            inResult = ST_SUCCESS;
            break;
        }
        
        vdDebug_LogPrintf("ulnInvoiceNo[%02X %02X %02X] strAdvice->szInvoiceNo [%02X %02X %02X]", szInvoiceNo[0], szInvoiceNo[1], szInvoiceNo[2], strAdvice.szInvoiceNo[0], strAdvice.szInvoiceNo[1], strAdvice.szInvoiceNo[2]);
        if(0 != memcmp(szInvoiceNo, strAdvice.szInvoiceNo, INVOICE_BCD_SIZE))
        {
            inUpDateAdviceIndex ++;
            continue;
        }
        else
        {
            srAdvTransTable.byUploaded = CN_TRUE;
            inResult = inMyFile_AdviceUpdate(inUpDateAdviceIndex);
            break;
        }
        
    }

    return ST_SUCCESS;
}

int inCTOSS_BatchCheckDuplicateInvoice(void)
{
    int inRet = d_NO;
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_CHECK_DUP_INV);

        vdSetErrorMessage("");
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetVoid();
            if(d_OK != inRet)
                return inRet;
        }       
        inRet = inCTOS_BatchSearch();
        
        vdSetErrorMessage("");
        if(d_OK != inRet)
            return inRet;
    }

    return inRet;
}

BYTE InputAmount2(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    char szTemp[24+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[24+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;;
	
	memset(szAmount, 0x00, sizeof(szAmount));

    if(first_key != 0x00)
    {
        szAmount[0]=first_key;
		inCtr=1;
    }
	
	while(1)
	{
		memset(szTemp, 0x00, sizeof(szTemp));         	 	
		if(strlen(szAmount) > 0)
		{
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmount, szTemp);
		}
		else
		{
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", "0", szTemp);
		}

 
        clearLine(usY);
        CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
        int x=0;
        int len, index;
        len=strlen(szTemp);
        for(index=0; index < len; index++)
        {
            if(szTemp[index] == '.')
                x+=1;
            else
                x+=2;
        }
        
        CTOS_LCDTPrintXY(36-x, usY, szTemp);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}

		if(inRet >= 48 && inRet <= 57)
		{
			if(inCtr < 9)
			{
			    memset(szTemp, 0x00, sizeof(szTemp));
			    sprintf(szTemp, "%c", inRet);
                strcat(szAmount, szTemp);			
                inCtr++; 		
				if(inCtr == 1 && szAmount[0] == 48)
				{
					memset(szAmount, 0x00, sizeof(szAmount));
					inCtr=0;
				}
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_USER_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{
			if(strlen(szAmount) > 0)
			{
				memcpy(baAmount, szAmount, strlen(szAmount));
				return d_OK;
			}
			else
			{
				if(bIgnoreEnter == 1)
					return d_OK;			
			}
		}
		else if(inRet == 82) /*clear key*/
		{		
			if(inCtr > 0)
                inCtr--;
            szAmount[inCtr]=0x00;
		}
	}
}



/***********************************************************/
//sidumili: added function
//check for transaction amount entry base on szMaxTrxnAmt limit
/***********************************************************/
int inCTOS_ValidateTrxnAmount(void)
{
	double dbAmt1 = 0.00, dbAmt2 = 0.00;
	BYTE szBaseAmt[30+1] = {0};
	char *strPTR;

	//inTCTRead(1);
	
	memset(szBaseAmt, 0x00, sizeof(szBaseAmt));
	wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmt, AMT_BCD_SIZE);
	dbAmt1 = strtod(szBaseAmt, &strPTR);
	dbAmt2 = strtod(strTCT.szMaxTrxnAmt, &strPTR);

	vdDebug_LogPrintf("JEFF::STR TXN AMT: [%s]", szBaseAmt);
	vdDebug_LogPrintf("JEFF::STR MAX AMT: [%s]", strTCT.szMaxTrxnAmt);
	vdDebug_LogPrintf("JEFF::TXN AMT: [%.0f]", dbAmt1);
	vdDebug_LogPrintf("JEFF::MAX AMT: [%.0f]", dbAmt2);
#if 0
	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 6, szBaseAmt);
	//CTOS_LCDTPrintXY(1, 8, strTCT.szMaxTrxnAmt);
    	CTOS_Delay(3000);
#endif
	if((dbAmt1 >= dbAmt2) || (dbAmt1 <= 0))
	{
		vdSetErrorMessage("OUT OF RANGE");
		return(d_NO);
	}

	return(d_OK);
}

#if 0
BYTE InputStringAlpha2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    char szTemp[24+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[24+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	int x=0;
	int inLastKey=255;
	int inKey=0;
	int inKey2=0;
    int inMax=*usStrLen;
	
	char KeyPad[10][4]={{'0', '0', '0', '0'},
		               {'1', 'Q', 'Z', '1'},
		               {'2', 'A', 'B', 'C'},
					   {'3', 'D', 'E', 'F'},
					   {'4', 'G', 'H', 'I'},
					   {'5', 'J', 'K', 'L'},
					   {'6', 'M', 'N', '0'},
					   {'7', 'P', 'R', 'S'},
					   {'8', 'T', 'U', 'V'},
					   {'9', 'W', 'X', 'Y'}};
	
	memset(szAmount, 0x00, sizeof(szAmount));
	
	CTOS_TimeOutSet(TIMER_ID_1, 100);
	
	while(1)
	{		
		memset(szTemp, 0x00, sizeof(szTemp));  
         
        clearLine(usY);
        CTOS_LCDTPrintXY(40-(strlen(szAmount)*2), usY, szAmount);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
            if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                inLastKey=255;
			
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}

		if(inRet >= 48 && inRet <= 57)
		{
			char szTemp2[24+1];
			
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%c", inRet);
			inKey=atoi(szTemp);
			
			CTOS_TimeOutSet(TIMER_ID_1, 100);
			if((inCtr < inMax) || (inLastKey == inKey))
			{
                vdDebug_LogPrintf("1. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                if(inLastKey == inKey)
                {
					inKey2++;
					if(inKey2 > 3)
                       inKey2=0;
                }
                else
					inKey2=0;
				                
                if(inLastKey == inKey)
                    inCtr--;
                    
				szAmount[inCtr]=KeyPad[inKey][inKey2];
				inCtr++;

                vdDebug_LogPrintf("2. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                inLastKey=inKey;

                if(inKey == 0)
				    inLastKey=255;	
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_KBD_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{
			if(strlen(szAmount) > 0)
			{
				memcpy(pbaStr, szAmount, strlen(szAmount));
				*usStrLen=strlen(szAmount);
				return d_KBD_ENTER;
			}
			
		}
		else if(inRet == 82) /*clear key*/
		{		
			inCtr--;
			if(inCtr <= 0)
                inCtr=0;
            szAmount[inCtr]=0x00;
			inKey2=0;
			inLastKey=255;
		}
	}
}
#endif

BYTE InputString2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    char szTemp[24+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[24+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	int x=0;
	int inKey=0;
    int inMax=*usStrLen;
	
	memset(szAmount, 0x00, sizeof(szAmount));
	
	while(1)
	{		
		memset(szTemp, 0x00, sizeof(szTemp));  
         
        clearLine(usY);
        CTOS_LCDTPrintXY(40-(strlen(szAmount)*2), usY, szAmount);
        
        //clearLine(usY);
		//setLCDPrint(usY, DISPLAY_POSITION_RIGHT, szAmount);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{		
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}

		if(inRet >= 48 && inRet <= 57)
		{
			if(inCtr < inMax)
			{
			    memset(szTemp, 0x00, sizeof(szTemp));
			    sprintf(szTemp, "%c", inRet);
                strcat(szAmount, szTemp);			
                inCtr++; 	
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_KBD_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{
			if(strlen(szAmount) > 0)
			{
				memcpy(pbaStr, szAmount, strlen(szAmount));
				*usStrLen=strlen(szAmount);
				return d_KBD_ENTER;
			}
		}
		else if(inRet == 82) /*clear key*/
		{		
			inCtr--;
			if(inCtr <= 0)
                inCtr=0;
            szAmount[inCtr]=0x00;
		}
	}
}

BYTE InputStringAlpha2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    char szTemp[40+1], szTemp2[40+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[40+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	int x=0;
	int inLastKey=255;
	int inKey=0;
	int inKey2=0;
    int inMax=*usStrLen;
	int inAtCtr=0;
	char KeyPad[10][4]={{'0', '0', '0', '0'},
		               {'1', 'Q', 'Z', '1'},
		               {'2', 'A', 'B', 'C'},
					   {'3', 'D', 'E', 'F'},
					   {'4', 'G', 'H', 'I'},
					   {'5', 'J', 'K', 'L'},
					   {'6', 'M', 'N', 'O'},
					   {'7', 'P', 'R', 'S'},
					   {'8', 'T', 'U', 'V'},
					   {'9', 'W', 'X', 'Y'}};

    char KeyPad2[10][10]={{'0', '~', '!', '@', '#', '$', '%', '^', '&', '.'},
					   {'1', 'Q', 'Z', '=', 'q', 'z', '+', '[', ']', '1'},
					   {'2', 'A', 'B', 'C', 'a', 'b', 'c', '(', ')', '2'},
					   {'3', 'D', 'E', 'F', 'd', 'e', 'f', '{', '}', '3'},
					   {'4', 'G', 'H', 'I', 'g', 'h', 'i', '<', '>', '4'},
					   {'5', 'J', 'K', 'L', 'j', 'k', 'l', '|', '5', '5'},
					   {'6', 'M', 'N', 'O', 'm', 'n', 'o', '6', '6', '6'},
					   {'7', 'P', 'R', 'S', 'p', 'r', 's', '7', '7', '7'},
					   {'8', 'T', 'U', 'V', 't', 'u', 'v', '?', '8', '8'},
					   {'9', 'W', 'X', 'Y', 'w', 'x', 'y', '9', '9', '9'}};

	char KeyPad3[10][4]={{'0', '@', '.', '_'},
		               {'q', 'z', '1', 'q'},
		               {'a', 'b', 'c', '2'},
					   {'d', 'e', 'f', '3'},
					   {'g', 'h', 'i', '4'},
					   {'j', 'k', 'l', '5'},
					   {'m', 'n', 'o', '6'},
					   {'p', 'r', 's', '7'},
					   {'t', 'u', 'v', '8'},
					   {'w', 'x', 'y', '9'}};

	memset(szAmount, 0x00, sizeof(szAmount));
	
	CTOS_TimeOutSet(TIMER_ID_1, 100);
	
	while(1)
	{	
		memset(szTemp, 0x00, sizeof(szTemp));  
         
        clearLine(usY);
		clearLine(usY+1);
        //CTOS_LCDTPrintXY(40-((strlen(szAmount)*2)+inAtCtr), usY, szAmount);
		
		if(strlen(szAmount)>20)
		{
			memset(szTemp, 0x00, sizeof(szTemp));
			memset(szTemp2, 0x00, sizeof(szTemp2));
			memcpy(szTemp, szAmount, 20);
			strcpy(szTemp2, szAmount+20);
		    setLCDPrint(usY, DISPLAY_POSITION_RIGHT, szTemp);	
		    setLCDPrint(usY+1, DISPLAY_POSITION_RIGHT, szTemp2);
		}
		else
			setLCDPrint(usY, DISPLAY_POSITION_RIGHT, szAmount);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
            if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                inLastKey=255;
			
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}
        vdDebug_LogPrintf("key: (%c)", inRet);
		if(inRet >= 48 && inRet <= 57)
		{
			char szTemp2[24+1];
			
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%c", inRet);
			inKey=atoi(szTemp);
			
			CTOS_TimeOutSet(TIMER_ID_1, 100);
			if((inCtr < inMax) || (inLastKey == inKey))
			{
                vdDebug_LogPrintf("1. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                if(inLastKey == inKey)
                {
					inKey2++;
					if(bInputMode == 0x04)
					{
					    if(inKey2 > 9)
                            inKey2=0;
					}
                    else
                    {
                        if(inKey2 > 3)
                        inKey2=0;
                    }
                }
                else
					inKey2=0;
				                
                if(inLastKey == inKey)
                    inCtr--;

				if(bInputMode == 0x04)
				{
					szAmount[inCtr]=KeyPad2[inKey][inKey2];
					if(szAmount[inCtr] == '@')
						inAtCtr++;
				}
				else if(bInputMode == 0x05)
				{
					szAmount[inCtr]=KeyPad3[inKey][inKey2];
					if(szAmount[inCtr] == '@') /*@ symbol is occupying 2 spaces*/
						inAtCtr++;
				}
				else
					szAmount[inCtr]=KeyPad[inKey][inKey2];
				inCtr++;

                vdDebug_LogPrintf("2. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
                vdDebug_LogPrintf("szAmount: (%s)", szAmount);
                				
                inLastKey=inKey;
                if(bInputMode != 0x04 && bInputMode != 0x05)
                {
                    if(inKey == 0)
				        inLastKey=255;
                }
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_KBD_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{			
			if(strlen(szAmount) > 0)
			{
				memcpy(pbaStr, szAmount, strlen(szAmount));
				*usStrLen=strlen(szAmount);
				return d_KBD_ENTER;
			}
			
			if(usMinLen == 0)
                return d_KBD_ENTER;			
		}
		else if(inRet == 81) /*dot key*/
		{	
			if(bInputMode == 0x04 || bInputMode == 0x05)
			{
                szAmount[inCtr]='.';
			    inCtr++;
			    inKey2=0;
			    inLastKey=255;
			}
		}
		else if(inRet == 82) /*clear key*/
		{	
            if(bInputMode == 0x04 || bInputMode == 0x05)
            {
                if(szAmount[inCtr] == '@')
                    inAtCtr--;			
            }
			
			inCtr--;
			if(inCtr <= 0)
                inCtr=0;
            szAmount[inCtr]=0x00;
			inKey2=0;
			inLastKey=255;
		}
	}
}

int inCTOS_CheckOfflineCeilingAmt(void)
{
	char szTotalAmt[12+1], szCeilingAmt[12+1];
	long lnTotalAmt=0L, lnCeilingAmt=0L;
    int inResult=0;
	
	//inResult=inTCTRead(1);
    //vdDebug_LogPrintf("inTCTRead inResult:(%d)", inResult);

	memset(szTotalAmt, 0x00, sizeof(szTotalAmt)); 
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
    
	memset(szCeilingAmt, 0x00, sizeof(szCeilingAmt));
    wub_hex_2_str(strTCT.szOfflineCeilingAmt, szCeilingAmt,6);
    
    lnTotalAmt=atol(szTotalAmt); 
    lnCeilingAmt=atol(szCeilingAmt);

	if (lnTotalAmt <= lnCeilingAmt)
		srTransRec.byCeillingAmt = VS_TRUE;	
	else
		srTransRec.byCeillingAmt = VS_FALSE;	
	
		
    vdDebug_LogPrintf("inCTOS_CheckOfflineCeilingAmt");
    vdDebug_LogPrintf("lnTotalAmt: (%ld), lnCeilingAmt: (%ld)  :byCeillingAmt: (%ld)", lnTotalAmt, lnCeilingAmt, srTransRec.byCeillingAmt);
	
	
    if(lnCeilingAmt <= 0) /*if offline ceiling amount is 0 - no need to check*/
    {
	    srTransRec.byCeillingAmt = VS_FALSE;
        return d_OK;
    }
    
    if(lnTotalAmt > lnCeilingAmt)
        return ST_ERROR;
	
	return d_OK;
}

int inNSRFlag(void)
{
    BYTE    szTotalAmt[12+1];
    int fNSRflag = 0;

	// 0 - Signature Line
	// 1 - No Signature Required
	
	vdDebug_LogPrintf("--inNSRFlag--");
	
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);    
    vdDebug_LogPrintf("strTCT.fNSR=%d, strIIT.fNSR[%d], atol(strIIT.szNSRLimit)=%ld, atol(szTotalAmt)=%ld, srTransRec.byTransType=%d, atol(strIIT.szCVMLimit)=%ld", strIIT.fNSR, strTCT.fNSR, atol(strIIT.szNSRLimit), atol(szTotalAmt), srTransRec.byTransType, atol(strIIT.szCVMLimit));
	vdDebug_LogPrintf("fInstallment[%d], fCash2Go[%d], fDebit[%d]", srTransRec.fInstallment, srTransRec.fCash2Go, srTransRec.fDebit);
	vdDebug_LogPrintf("T95 [%02X%02X%02X%02X%02X]",srTransRec.stEMVinfo.T95[0], srTransRec.stEMVinfo.T95[1], srTransRec.stEMVinfo.T95[2], srTransRec.stEMVinfo.T95[3], srTransRec.stEMVinfo.T95[4]);
	vdDebug_LogPrintf("T9F34 [%02X%02X%02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
	vdDebug_LogPrintf("T9F33 [%02X%02X%02X]",srTransRec.stEMVinfo.T9F33[0], srTransRec.stEMVinfo.T9F33[1], srTransRec.stEMVinfo.T9F33[2]);
	vdDebug_LogPrintf("AAA - srTransRec.fEMVPIN[%d] srTransRec.fEMVPINEntered[%d]", srTransRec.fEMVPIN, srTransRec.fEMVPINEntered);

	if ((srTransRec.byTransType == SALE_TIP) || (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP))
	{
	    fNSRflag=0;
		return fNSRflag;
	}
	
    if( (srTransRec.byTransType == SALE) || ((srTransRec.byTransType == SALE_OFFLINE) && (memcmp(srTransRec.szAuthCode,"Y1",2) == 0))|| 
		(srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE) /*|| (srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE_OFFLINE)*/) 
    {
        if (srTransRec.byEntryMode == CARD_ENTRY_FALLBACK || srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
            fNSRflag=0;
        else
        {
            if(strIIT.fNSR == 1 && (atol(strIIT.szNSRLimit) >= atol(szTotalAmt))&& (atol(strIIT.szCVMLimit)<=0) && srTransRec.fDCCOptin==FALSE)
           	{
           		fNSRflag = 1;
				vdDebug_LogPrintf("AAA fNSRflag is 1 (1)");
           	}                
        }
    }

	if (srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == PREAUTH_VOID || srTransRec.byTransType == PRE_COMP)
	{
		fNSRflag = 0;
		vdDebug_LogPrintf("AAA fNSRflag is 0 (1)");
	}

	if (srTransRec.stEMVinfo.T9F34[0] == 0x41) //pin entered
	 	fNSRflag = 1;

	// Check TVR for bypassed PIN -- sidumili
	if (srTransRec.stEMVinfo.T95[2] == 0x08 && srTransRec.byEntryMode == CARD_ENTRY_ICC  && (atol(strIIT.szNSRLimit) < atol(szTotalAmt))&& (atol(strIIT.szCVMLimit)> 0) && srTransRec.fDCCOptin==FALSE)
	{
		srTransRec.fEMVPINBYPASS = TRUE;
		srTransRec.fEMVPIN = FALSE;
		vdDebug_LogPrintf("AAA fNSRflag is 0 (2)");
		fNSRflag = 0;
	}
		
	//if ((srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered == TRUE) && (srTransRec.fEMVPINBYPASS == FALSE) && (atol(strIIT.szCVMLimit)> 0))
	 	     //fNSRflag = 1;

	if ((srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered == TRUE) && (srTransRec.fEMVPINBYPASS == FALSE)) //&& (atol(strIIT.szCVMLimit)<= 0 && atol(strIIT.szNSRLimit) < atol(szTotalAmt)))
		{
             vdDebug_LogPrintf("AAA fNSRflag is 1 (2)");
             fNSRflag = 1;
		}
						
	vdDebug_LogPrintf("fEMVPINBYPASS[%d], fEMVPINEntered[%d], fEMVPIN[%d]", srTransRec.fEMVPINBYPASS, srTransRec.fEMVPINEntered, srTransRec.fEMVPIN);
		
	/*if(srTransRec.fDCCOptin == TRUE)
	{
		if ((srTransRec.fEMVPINBYPASS == TRUE || srTransRec.byEntryMode == CARD_ENTRY_MSR) && (srTransRec.fEMVPIN == FALSE || srTransRec.fCUPPINEntry == FALSE))
			fNSRflag = 0;
	}*/

	if(srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE)
	{
        fNSRflag = 0;
	}	

	if ((srTransRec.fDCCOptin == TRUE) && (srTransRec.fEMVPIN == FALSE) && (srTransRec.fEMVPINEntered == FALSE) &&(srTransRec.fCUPPINEntry==FALSE))
         {
              fNSRflag = 0;
			 vdDebug_LogPrintf("AAA fNSRflag is 0 (3)");
         }

	// sidumili: always has a signature line
	if (srTransRec.byTransType == SALE_TIP)
	{
		fNSRflag = 0;
	}

	if (srTransRec.stEMVinfo.T9F34[0] == 0x44) //pin entered encrypted pin
	 	fNSRflag = 1;

	vdDebug_LogPrintf("fNSRflag[%d]", fNSRflag);
	
    return fNSRflag;
}


//s1-ctms
int inCTOS_ChkBatchEmpty_AllHosts(void)
{
	int inNumOfHost = 0, inNum1;
	int inNumOfMit = 0, inNum2;
    char szBcd[INVOICE_BCD_SIZE + 1];
    char szAPName[50];
    int inAPPID;
	int inRet = d_NO;

    memset(szAPName, 0x00, sizeof (szAPName));
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord(); 
    vdDebug_LogPrintf("inNumOfHost=[%d]-----", inNumOfHost);

	for (inNum1 = 1; inNum1 <= inNumOfHost; inNum1++) {
        if (inHDTRead(inNum1) == d_OK) {
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----", szAPName, strHDT.szAPName);
            if (strcmp(szAPName, strHDT.szAPName) != 0) {
                continue;
            }

            inCPTRead(inNum1);
            srTransRec.HDTid = inNum1;
            strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
            memset(szBcd, 0x00, sizeof (szBcd));
            memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

			inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfMit);
		    vdDebug_LogPrintf("inNumOfMit=[%d]-----", inNumOfMit);
		    for (inNum2 = 0; inNum2 < inNumOfMit; inNum2++) {
		        memcpy(&strMMT[0], &strMMT[inNum2], sizeof (STRUCT_MMT));
		        srTransRec.MITid = strMMT[0].MITid;
		        strcpy(srTransRec.szTID, strMMT[0].szTID);
		        strcpy(srTransRec.szMID, strMMT[0].szMID);
		        memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
		        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

		        inRet = inCTOS_ChkBatchEmpty();
			    if (d_NO != inRet)
			        return d_NO;
			}

        }
    }

	return d_OK;

}

int inPing(void)
{
    USHORT usX =1, usY = 6;
    BYTE bShowAttr = 0x02; 
    USHORT inMaxLen=48,inMinLen=9;
    BYTE baPIN[6 + 1];
    BYTE szIPAddress[48+1];
    BYTE bRet;
    BYTE szDisplay[100];
    BYTE szTitle[25+1];
    BYTE szMsg[100];

    memset(szIPAddress, 0x00, sizeof(szIPAddress));
    memset(szDisplay, 0x00, sizeof(szDisplay));
    memset(szTitle, 0x00, sizeof(szTitle));

    szGetTransTitle(srTransRec.byTransType, szTitle);

    strcpy(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter IP Address:");

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "ENTER APPROVAL CODE:");

    while(TRUE)
    {
        inMaxLen = 48;
        //bRet = InputStringAlpha2(1, 8, 0x00, bShowAttr, szAuthCode, &inAuthCodeLen, inMinLen, d_INPUT_TIMEOUT);
		//bRet = InputAlphaUI(0x03, 0x02, szAuthCode, &inAuthCodeLen, inMinLen, d_INPUT_TIMEOUT, szDisplay);
		bRet = InputQWERTY(0x03, 0x02, szIPAddress, inMaxLen, inMinLen, d_INPUT_TIMEOUT, szDisplay);
        if (bRet == d_KBD_CANCEL )
        {
            return (d_EDM_USER_CANCEL);
        }
        else if (bRet == d_KBD_ENTER ) 
        {
            if(strlen(szIPAddress) >= 9)
            {
				vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
                inCallJAVA_Ping(&szIPAddress);
				if(memcmp(szIPAddress, "OK", 2) == 0)
				{
					vdDisplayMessageBox(1, 8, "Host is reachable", "", "", MSG_TYPE_SUCCESS);
				}
				else
				{
					vdDisplayErrorMsg(1, 8, "Host is NOT reachable");
				}
				CTOS_Beep();
				CTOS_Delay(1000);
                break;
            }
            else
            {
                //memset(szAuthCode, 0x00, sizeof(szAuthCode));
                //clearLine(14);                
                vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
                //clearLine(14);
                //clearLine(8);         // clear input line        
            }
        }
        else
        {
            //memset(szAuthCode, 0x00, sizeof(szAuthCode));           
            //clearLine(14);                
            vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
            //clearLine(14);
        }
    }
    //usCTOSS_LCDDisplay( "|PROCESSING...");
    //vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
    return ST_SUCCESS;
}

