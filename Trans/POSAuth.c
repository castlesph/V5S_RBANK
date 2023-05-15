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
#include "..\comm\V5comm.h"
#include "..\print\print.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"

#include "../FileModule/myFileFunc.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"

extern BOOL ErmTrans_Approved;
//extern BOOL gblDCCTrans;


int inCTOS_PreAuthFlowProcess(void)
{
    int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_PreAuthFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);
	
    srTransRec.fCredit=TRUE;

    vdCTOS_SetTransType(PRE_AUTH);
    
    //vdDispTransTitle(PRE_AUTH);

    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;

     strCDT.HDTid=19; //Catch-all to New host  
     
     vdCheckForDualCard();

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH);
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
	
    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

//#00248 - NO CVV Entry on Pre-Auth transaction
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

   // inRet = inCTOS_CustComputeAndDispTotal();
    //if(d_OK != inRet)
        //return inRet;

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
               //vdCTOS_SetTransType(PRE_AUTH);
               //gblDCCTrans = FALSE;
               goto DisplayTotal;			  
          
          }
          DisplayFXRate:
          inRet=inDCCDisplayFxRate("PREAUTHORIZATION");
          if (USER_ABORT== inRet)
          {
               return inRet;
          }
          if (DCC_PRINT_OPT == inRet)
          {
               vdPrintDCCChooseCurrency("PREAUTHORIZATION");
               goto DisplayFXRate;
          }
          DisplayTotal:
          vdCTOS_SetTransType(PRE_AUTH);

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

	inRet = inCTOS_PreConnect();
		if(d_OK != inRet)
			return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    if(inNSRFlag() != 1)
    {
         inRet = ushCTOS_ePadSignature();
         if(d_OK != inRet)
              return inRet;
    }
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
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }         
    }
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

int inCTOS_PREAUTH(void)
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
	
    inRet = inCTOS_PreAuthFlowProcess();

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

