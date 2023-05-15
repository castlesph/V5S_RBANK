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
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSbatch.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\ui\Display.h"
#include "..\accum\accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Debug\Debug.h"


extern BOOL ErmTrans_Approved;

int inCTOS_TipAdjustFlowProcess(void)
{
    int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_TipAdjustFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);

    vdCTOS_SetTransType(SALE_TIP);
    
    //display title
    vdDispTransTitle(SALE_TIP);

    //inRet = inCTOS_CheckTipAllowd();
    //if(d_OK != inRet)
        //return inRet;
       
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GeneralGetInvoice();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_TIP_ADJUST);
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
        if(d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckTipadjust();
    if(d_OK != inRet)
        return inRet;

	if(inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) == d_OK)
	{
		if(inGetMerchantPassword() != d_OK)
            return d_NO;
	}

    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHost();
    if(d_OK != inRet)
        return inRet;

    if(strHDT.fTipAllowFlag == FALSE)
    {
        vdSetErrorMessage("TIP NOT ALLWD");
        return d_NO;
    }

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTipAfjustAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

	//tine: mcc req't
	inRet = inCTOS_ConfirmTipAdj();
    if(d_OK != inRet)
        return inRet;

	//vdDisplayTxnFinishUI();
	
    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    srTransRec.fTipUploaded=FALSE;
    srTransRec.inTipAdjCount++;
		
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

    inRet = ushCTOS_ePadSignature_Tip();
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
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }         
    }
	
	vdSetErrorMessage("");

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;
		
    return d_OK;
}

int inCTOS_TIPADJUST(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
		return inRet;
	
    inRet = inCTOS_TipAdjustFlowProcess();

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
