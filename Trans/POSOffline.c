/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\includes\POSbatch.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"


int inSaleOffGroupIndex=0;

extern BOOL ErmTrans_Approved;
//extern BOOL gblDCCTrans;


int inCTOS_SaleOffLineFlowProcess(void)
{
    int inRet = d_NO;
    BOOL fGoOnline=VS_FALSE;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_SaleOffLineFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);
	
    srTransRec.fCredit=TRUE;

    vdCTOS_SetTransType(SALE_OFFLINE);
    
    //display title
    //vdDispTransTitle(SALE_OFFLINE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    if(inSaleOffGroupIndex)
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

        strCDT.HDTid=19; //Catch-all to New host
	}

    vdCheckForDualCard();
	
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_OFFLINE_SALE);
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

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet=inCTOS_CheckOfflineCeilingAmt();
	if(inRet != d_OK) /*if Offline Sale celing ammount is met it will force to be Sale Online*/
	{
        vdCTOS_SetTransType(SALE);
        vdDispTransTitle(SALE);	
		fGoOnline=VS_TRUE;
	}
	else
	{
        inRet = inCTOS_GetOffApproveNO();
        if(d_OK != inRet)
            return inRet;
	}
	
    //inRet = inCTOS_CustComputeAndDispTotal();
   // if(d_OK != inRet)
      //  return inRet;

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
                   if(fGoOnline == VS_TRUE)
                   {
                        inCTOS_inDisconnect();
                        vdCTOS_SetTransType(SALE);
                   }
				   else
                        vdCTOS_SetTransType(SALE_OFFLINE);
				   
                   goto DisplayTotal;
               }
			   DisplayFXRate:
               if(fGoOnline == VS_TRUE)	   
                    inRet=inDCCDisplayFxRate("STRAIGHT SALE");
               else
                    inRet=inDCCDisplayFxRate("OFFLINE SALE");
               if (USER_ABORT== inRet)
               {
                    return inRet;
               }
			   if (DCC_PRINT_OPT == inRet)
               {
                    if(fGoOnline == VS_TRUE)	 
                         vdPrintDCCChooseCurrency("STRAIGHT SALE");
					else
						 vdPrintDCCChooseCurrency("OFFLINE SALE");
                    goto DisplayFXRate;
               }

     DisplayTotal:
	    if(fGoOnline == VS_TRUE)
                    vdCTOS_SetTransType(SALE);
	    else
		vdCTOS_SetTransType(SALE_OFFLINE);

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

	if (fGoOnline == VS_TRUE)
	{
	    inRet = inCTOS_EMVProcessing();
	    if(d_OK != inRet)
	        return inRet;   
	}
	 
    if(fGoOnline == VS_TRUE)
    {
        inRet = inCTOS_PreConnect();
        if(d_OK != inRet)
            return inRet;
    }

	//vdDisplayTxnFinishUI();
			
    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

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
	
    if(fGoOnline == VS_TRUE)
    {
       inRet = ushCTOS_printReceipt();
       if(d_OK != inRet)
           return inRet; 
	   else
	    {
	         if (fFSRMode() == TRUE)
	         {  
	              inCTOSS_UploadReceipt(FALSE, FALSE);	 
	         }	         
	    }

		vdSetErrorMessage("");
		
       inRet = inProcessAdviceTCTrail(&srTransRec);
       if(d_OK != inRet)
           return inRet;
       else
           vdSetErrorMessage("");
    }
    else
    {
        inRet = ushCTOS_printReceipt();
        if(d_OK != inRet)
            return inRet;
        else
	    {
	         if (fFSRMode() == TRUE)
	         {  
	              inCTOSS_UploadReceipt(FALSE, FALSE);	 
	         }	         
	    }
		vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;
		
    return d_OK;
}

int inCTOS_SALE_OFFLINE(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;

	if(isCheckTerminalMP200() == d_OK)
	{
		inRet = inCTOSS_ERM_CheckSlipImage();
		if(d_OK != inRet)
			return inRet;
	}
	
	inSaleOffGroupIndex=0;
	
    inRet = inCTOS_SaleOffLineFlowProcess();

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
        	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DinersSaleOffline(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inSaleOffGroupIndex=6;
	
    inRet = inCTOS_SaleOffLineFlowProcess();

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
	
    vdCTOS_TransEndReset();

    return inRet;
}

