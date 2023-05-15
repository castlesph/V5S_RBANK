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
#include "..\Accum\Accum.h"
#include "..\Comm\V5Comm.h"
#include "..\print\Print.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

int inRefundGroupIndex=0;

extern BOOL ErmTrans_Approved;
//extern BOOL gblDCCTrans;


int inCTOS_RefundFlowProcess(void)
{
    int inRet = d_NO;


   //CTOS_PrinterPutString("inCTOS_RefundFlowProcess");

    srTransRec.fCredit=TRUE;

    vdCTOS_SetTransType(REFUND);
    
    //display title
    vdDispTransTitle(REFUND);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    if(inRefundGroupIndex)
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
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_REFUND);
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

   	//no last 4 digits on refund when card was swiped - #00178
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
	
    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

   /* inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
       return inRet;   */

   
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
                //vdCTOS_SetTransType(REFUND);
                //gblDCCTrans = FALSE;
           goto DisplayTotal;			 
                         
           }
		  DisplayFXRate:
          inRet=inDCCDisplayFxRate("REFUND");
          if (USER_ABORT== inRet)
          {
               return inRet;
          }
          if (DCC_PRINT_OPT == inRet)
          {
               vdPrintDCCChooseCurrency("REFUND");
               goto DisplayFXRate;
          }
          DisplayTotal:
		  vdCTOS_SetTransType(REFUND);

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
        vdSetErrorMessage("");

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

int inCTOS_REFUND(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
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
	
	inRefundGroupIndex=0;
		
    inRet = inCTOS_RefundFlowProcess();

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

int inCTOS_DinersRefund(void)
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
	
	inRefundGroupIndex=6;
		
    inRet = inCTOS_RefundFlowProcess();

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

