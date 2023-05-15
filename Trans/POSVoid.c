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
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"

extern BOOL fECRApproved;
extern BOOL ErmTrans_Approved;

int inCTOS_VoidFlowProcess(void)
{
    int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_VoidFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);

    fECRApproved=FALSE;
	
    vdCTOS_SetTransType(VOID);
    
    //display title
    //vdDispTransTitle(VOID);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    if(fGetECRTransactionFlg() != TRUE)
    {
        inRet = inCTOS_GeneralGetInvoice();
        if(d_OK != inRet)
            return inRet;
    }
	
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        
		vdDebug_LogPrintf("saturn inCTOS_MultiAPBatchSearch");
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
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

	vdDebug_LogPrintf("saturn before check void");

    inRet = inCTOS_CheckVOID();
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

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet)
        return inRet;

	
	vdDebug_LogPrintf("saturn after confirm invoice amount");

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;       

   vdDebug_LogPrintf("inCTOS_VoidFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
   inRet = inMultiAP_ECRSendSuccessResponse();

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

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();
	
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
    {
         if (fFSRMode() == TRUE)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }
	if(strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

    return d_OK;
}

int inCTOS_VOID(void)
{
    int inRet = d_NO;
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = {0};
    char szMenu[1024] = {0};	
	
    vdCTOS_TxnsBeginInit();

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;

    if(isCheckTerminalMP200() == d_OK)
    {
         inRet = inCTOSS_ERM_CheckSlipImage();
         if(d_OK != inRet)
              return inRet;
         
         CTOS_LCDTClearDisplay();
         memset(szMenu, 0x00, sizeof(szMenu));
         strcpy(szHeaderString, "VOID");
         strcpy((char*)szMenu, "BATCH REVIEW \nENTER TRACE NO.");
         key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMenu, TRUE);
         
         if(key > 0)
         {
              if(key == d_KBD_CANCEL)
                   return d_OK;
              
              if(key == 1)
              {
                   inCTOS_BATCH_REVIEW_Process();
                   return d_OK;
              }
         }
    }
	
    inRet = inCTOS_VoidFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili

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
