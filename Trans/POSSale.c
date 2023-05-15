/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "../Debug/debug.h"

int inSaleGroupIndex=0;
extern BOOL fECRApproved;

extern BOOL ErmTrans_Approved;
extern int inSelectedIdleAppsHost;

int inCTOS_SaleFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    char szAscii[INVOICE_ASC_SIZE];
	int iOrientation = get_env_int("#ORIENTATION");
	char szDCCTransactionType[20];
	
	vdDebug_LogPrintf("saturn --inCTOS_SaleFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
	vdDebug_LogPrintf("saturn inSaleGroupINdex %d", inSaleGroupIndex);
    fECRApproved=FALSE;
		
    srTransRec.fCredit=TRUE;
		
    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    vdDisplayECRAmount(); /*display amount for ECR only*/
	
    if(inSaleGroupIndex == 6)
    {
        inRet = inCTOS_GetCardFieldsNoEMV();   
        if(d_OK != inRet)
            return inRet;
		
        if(inCheckValidApp(DINERS_HOST) != d_OK)
            return INVALID_CARD;
    }
	else
	{
		inRet = inCTOS_GetCardFields();   
		if(d_OK != inRet)
		    return inRet;

		vdDebug_LogPrintf("saturn byIdleSaleType[%d]", strTCT.byIdleSaleType);
		vdDebug_LogPrintf("saturn inSelectedIdleAppsHost[%d]", inSelectedIdleAppsHost);
		vdDebug_LogPrintf("saturn strCDT.HDTid[%d]", strCDT.HDTid);
		vdDebug_LogPrintf("saturn strCDT.IITid[%d]", strCDT.IITid);
		vdDebug_LogPrintf("saturn srTransRec.IITid[%d]", srTransRec.IITid);
		vdDebug_LogPrintf("AAA - strTCT.byIdleSaleType[%d] inSelectedIdleAppsHost[%d]", strTCT.byIdleSaleType, inSelectedIdleAppsHost);
		
		if ((strTCT.byIdleSaleType == IDLE_CREDIT_AND_BANCNET) && (inSelectedIdleAppsHost ==BANCNET_HOST))		
			strCDT.HDTid = strCDT.HDTid;		
		else
			strCDT.HDTid=19; //Catch-all to New host
 	}

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

    vdCheckForDualCard();
	
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
            
            inRet = inCTOS_MultiAPReloadTable();
            if(d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if(d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_FraudControl();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

	/*
	inRet = inCTOS_PreConnectAndInit();
    if(d_OK != inRet)
        return inRet;
	*/
	
    inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    if(srTransRec.IITid == 8) /*CUP*/
    {
         if ((inCTOSS_CheckCVMAmount()== d_NO) || (srTransRec.byEntryMode==CARD_ENTRY_FALLBACK))
         {
              inRet = inCTOS_ByPassPIN(); /*for CUP*/
              if(d_OK != inRet)
              return inRet;
              
              if(srTransRec.fCUPPINEntry == TRUE) /*for CUP*/
              {
                   inRet = GetPIN_With_3DESDUKPTEx();	
                   if (inRet == d_KMS2_GET_PIN_NULL_PIN)
                   {
                        srTransRec.fCUPPINEntry=FALSE;
                        inRet= d_OK;
                   }
                   if(d_OK != inRet)
                   return inRet;	
              }
              
              vdDispTransTitle(srTransRec.byTransType);
              vdClearBelowLine(2);
         }
    }
	
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

     srTransRec.fEMVPINEntered = FALSE;
     
  if((strCDT.fDCCEnable==TRUE) && (strTCT.fDCCTerminal==TRUE))
    {
         
               inCTOS_LocalAPSaveData();
               if(d_OK != inRet)
               return inRet;
			   
               #if 0
               if ( (srTransRec.byEntryMode==CARD_ENTRY_MSR) || (srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
               || (srTransRec.byEntryMode==CARD_ENTRY_MANUAL2)  || (srTransRec.byEntryMode==CARD_ENTRY_FALLBACK))
               {
                    vdCTOS_SetTransType(DCC_RATE_REQUEST);						   
                    vdDispTransTitle(DCC_RATE_REQUEST);
               }
	     #if 0
             if (srTransRec.byEntryMode==CARD_ENTRY_ICC) 
             {
                  vdCTOS_SetTransType(DCC_MERCH_RATE_REQ);						   
                  vdDispTransTitle(DCC_MERCH_RATE_REQ);
             }
	     #else
              if ((srTransRec.byEntryMode==CARD_ENTRY_ICC) && (srTransRec.stEMVinfo.T9F42[0]!=0x00) && (srTransRec.stEMVinfo.T9F42[1]!=0x00))
              {
                   vdCTOS_SetTransType(DCC_MERCH_RATE_REQ);						   
                   vdDispTransTitle(DCC_MERCH_RATE_REQ);
              }
              else
              {
                   vdCTOS_SetTransType(DCC_RATE_REQUEST);						   
                   vdDispTransTitle(DCC_RATE_REQUEST);
              }
	     #endif
	    #else		
             vdCTOS_SetTransType(DCC_RATE_REQUEST);						   
             vdDispTransTitle(DCC_RATE_REQUEST);
	    #endif
		
               inRet = inCTOS_PreConnect();
               if(d_OK != inRet)
                    return inRet;
		
               inRet =inDCCRateRequest();
              if ( RATE_REQUEST_CONN_FAIL==inRet) 
              {
                   return inRet;
              }
              else if (d_OK != inRet)
              {
                   inCTOS_inDisconnect();
                   //vdCTOS_SetTransType(SALE);
                   //gblDCCTrans = FALSE;
  				 
                  goto DisplayTotal;
              }
			   DisplayFXRate:
               inRet=inDCCDisplayFxRate("STRAIGHT SALE");
               if (USER_ABORT== inRet)
               {
                   return inRet;
               }
               if (DCC_PRINT_OPT == inRet)
               {
                    vdPrintDCCChooseCurrency("STRAIGHT SALE");
                    goto DisplayFXRate;
               }
              DisplayTotal:   
              vdCTOS_SetTransType(SALE);
              inRet = inCTOS_CustComputeAndDispTotal();
              if(d_OK != inRet)
              return inRet;
	
    }
    else
    {
         inRet = inCTOS_CustComputeAndDispTotal();
         if(d_OK != inRet)
         return inRet;
    }

	 inRet = inCTOS_EMVProcessing();
     if(d_OK != inRet)
     return inRet;   
     
     //EMV: Online PIN enchancement - start -- jzg
     if(strTCT.fEMVOnlinePIN == TRUE && srTransRec.fEMVPINEntered == FALSE)
     {
          srTransRec.fEMVPIN = FALSE;
          vdDebug_LogPrintf("JEFF::inCTOS_SaleFlowProcess 9F34 = [%02X %02X %02X]", srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
          if((srTransRec.stEMVinfo.T9F34[0] & 0x0F) == 0x02)
          {
               inRet = GetPIN_With_3DESDUKPTEx(); 
               if (inRet == d_KMS2_GET_PIN_NULL_PIN)
               {
                    srTransRec.fEMVPIN=FALSE;			 
                    inRet=d_OK;
               }
               else if(inRet != d_OK)
                    return inRet;
               else
                    srTransRec.fEMVPIN = TRUE;
          
          }
     }
	//EMV: Online PIN enchancement - end -- jzg

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

    fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

    inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;

	}

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
   else
   {
        if (fFSRMode() == TRUE)
        {  
             inBcd2Ascii(srTransRec.szInvoiceNo, szAscii, INVOICE_BCD_SIZE);
             vdOrientationInvoiceNumbers(szAscii);
             inCTOSS_UploadReceipt(FALSE, FALSE);	
        }
   }	

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
		
    return d_OK;
}

int inCTOS_SALE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();

	if(isCheckTerminalMP200() == d_OK)
	{
		inRet = inCTOSS_ERM_CheckSlipImage();
		if(d_OK != inRet)
			return inRet;
	}
		
    inSaleGroupIndex=0;
    
    inRet = inCTOS_SaleFlowProcess();

	vdSetECRTransactionFlg(0);
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

	if (isCheckTerminalMP200() == d_OK)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}
	inMultiAP_ECRSendSuccessResponse(); 
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DinersSALE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inSaleGroupIndex=6;

    inRet = inCTOS_SaleFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_SelectSaleSwipeMenu(void)
{
    int inRet;
    char szDebug[40 + 1]={0};
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0; 
	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    strcpy(szHeaderString, "SALE MENU");
    strcat(szChoiceMsg,"CREDIT SALE \n");
    strcat(szChoiceMsg,"DEBIT SALE \n");
    strcat(szChoiceMsg,"INSTALLMENT");
    
    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
    if (key > 0)
    {
        if(key == 1)
            inRet=CREDIT_SALE;
        else if(key == 2)
            inRet=DEBIT_SALE;
        else if(key == 3)
            inRet=INST_SALE;
        else if(key == d_KBD_CANCEL){
  	   		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//#00238 Card No not cleared once cancel after expiry date
			
            inRet=-1;
        	}
    }	
	return inRet;
}

int inCTOS_SelectSaleChipMenu(void)
{
    int inRet;
    char szDebug[40 + 1]={0};
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0; 

	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    strcpy(szHeaderString, "SALE MENU");
    strcat(szChoiceMsg,"CREDIT SALE \n");
    strcat(szChoiceMsg,"INSTALLMENT");
	
    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
    if (key > 0)
    {
        if(key == 1)
            inRet=CREDIT_SALE;
        else if(key == 2)
            inRet=INST_SALE;
        else if(key == d_KBD_CANCEL){
  	   		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//#00238 Card No not cleared once cancel after expiry date
			
            inRet=-1;
        }
    }	
	return inRet;
}

int inDisplayMsg1(void)
{
     int i = 1;

	  for(i=1;i<=16;i++)
	  {
	      CTOS_LCDTPrintXY(1, i, "测试");
	  }

	  CTOS_Delay(5*1000);

	 
	 

   // CTOS_LCDTPrintXY(1, 2, "开始");
   //CTOS_LCDTPrintXY(10, 8, "结束");

/*    CTOS_LCDTPrintAligned(1, "First Line", d_LCD_ALIGNCENTER);
    CTOS_LCDTPrintAligned(2, "Second Line",d_LCD_ALIGNRIGHT);
    CTOS_LCDTPrintAligned(3, "Third Line", d_LCD_ALIGNLEFT);
    CTOS_LCDTPrintAligned(4, "Fourth Line",d_LCD_ALIGNCENTER);
	CTOS_LCDTPrintAligned(5, "Fifth Line", d_LCD_ALIGNRIGHT);
	CTOS_LCDTPrintAligned(6, "Six Line",   d_LCD_ALIGNLEFT);*/
	CTOS_LCDTPrintAligned(1, "First Line", d_LCD_ALIGNCENTER);
	CTOS_LCDTPrintAligned(2, "Second Line",   d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintAligned(3, "Third Line",   d_LCD_ALIGNRIGHT);
	CTOS_LCDTPrintAligned(16, "Finish",	 d_LCD_ALIGNRIGHT);

	 CTOS_Delay(10*1000);

	 CTOS_LCDTClearDisplay();

	

    return d_OK;
}

int inDisplayImage(void)
{
   char szFileName[50+1];
   memset(szFileName, 0x00, sizeof(szFileName));
 
   sprintf(szFileName, "%s" , "p1");
   CTOS_LCDGShowBMPPic(0, 800, szFileName);

    return d_OK;
}


