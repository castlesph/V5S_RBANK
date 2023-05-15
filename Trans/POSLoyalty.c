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
#include "..\Includes\CTOSInput.h"
#include "..\Includes\POSLoyalty.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

extern BOOL ErmTrans_Approved;

struct LOYALTY_DATA stLoyData;

int inCTOS_Redeem5050FlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    //CTOS_PrinterPutString("inCTOS_Redeem5050FlowProcess");

    srTransRec.fLoyalty=TRUE;
	
    vdCTOS_SetTransType(LOY_REDEEM_5050);
    
    //display title
    vdDispTransTitle(LOY_REDEEM_5050);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
    
    //inRet = inCTOS_GetCardFieldsNoEMV();
    //if(d_OK != inRet)
    //    return inRet;

        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;
	
    
	//strCDT.HDTid=strCDT.inLoyaltyGroup;
	strCDT.HDTid=19; // FOR NEW HOST

     vdCheckForDualCard();

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOY_REDEEM_5050);
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
	
    //inRet = inCTOS_GetTxnTipAmount();
    //if(d_OK != inRet)
        //return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;

    inRet = inEnterProductCode();
    if(d_OK != inRet)
        return inRet;

    inRet = inEnterCustomerNumber();
    if(d_OK != inRet)
        return inRet;
	 
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

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inProcessAdviceTCTrail(&srTransRec);
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return d_OK;
}

int inCTOS_Redeem5050(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_Redeem5050FlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_RedeemVariableFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    //CTOS_PrinterPutString("inCTOS_RedeemVariableFlowProcess");

    srTransRec.fLoyalty=TRUE;

    vdCTOS_SetTransType(LOY_REDEEM_VARIABLE);
    
    //display title
    vdDispTransTitle(LOY_REDEEM_VARIABLE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
    
    //inRet = inCTOS_GetCardFieldsNoEMV();
    //if(d_OK != inRet)
    //    return inRet;

        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;
	

//    strCDT.HDTid=strCDT.inLoyaltyGroup;
	strCDT.HDTid=19; // FOR NEW HOST

     vdCheckForDualCard();
	
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOY_REDEEM_VARIABLE);
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
	
    //inRet = inCTOS_GetTxnTipAmount();
    //if(d_OK != inRet)
        //return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inEnterRedeemAmount();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inEnterProductCode();
    if(d_OK != inRet)
        return inRet;

    inRet = inEnterCustomerNumber();
    if(d_OK != inRet)
        return inRet;
	
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

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;
	
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

int inCTOS_RedeemVariable(void)
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
	
    inRet = inCTOS_RedeemVariableFlowProcess();

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

int inCTOS_LoyaltyBalanceInquiryFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

//    CTOS_PrinterPutString("inCTOS_LoyaltyBalanceInquiryFlowProcess");

    srTransRec.fLoyalty=TRUE;

	vdCTOS_SetTransType(LOY_BAL_INQ);
	
	//display title
	vdDispTransTitle(LOY_BAL_INQ);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	//inRet = inCTOS_GetCardFieldsNoEMV();
	//if(d_OK != inRet)
	//	return inRet;

        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;

	//strCDT.HDTid=strCDT.inLoyaltyGroup;
	strCDT.HDTid = 19; // FOR NEW HOST

	vdCheckForDualCard();
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOY_BAL_INQ);
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

	/*
	inRet = inCTOS_PreConnectAndInit();
    if(d_OK != inRet)
        return inRet;
	*/
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;

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

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;

    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	return d_OK;
}

int inCTOS_LoyaltyBalanceInquiry(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	
    inRet = inCTOS_LoyaltyBalanceInquiryFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inEnterProductCode(void)
{
    int inRet=0,inPANLen;
    BYTE strOut[16+1];//, szPAN[4+1];
    short shMaxLen=16, shMinLen=1;

    if(strTCT.fProductCode!= TRUE)
        return d_OK;	  	

    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);
	   
    while(1)
    {	
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "PRODUCT CODE:");
        memset(strOut,0x00, sizeof(strOut));
        inRet = InputString(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            return d_NO;
        else if(inRet == 0)
            return d_NO;
        else if(inRet == 0xFF)
        {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
        else if(inRet>=1)
        {
			memset(srTransRec.szProductCode, 0x00, sizeof(srTransRec.szProductCode));
			strcpy(srTransRec.szProductCode, strOut); 
			break;
        }	
    }	
    
    return d_OK;
}

int inEnterCustomerNumber(void)
{
    int inRet=0,inPANLen;
    BYTE strOut[16+1];//, szPAN[4+1];
    short shMaxLen=16, shMinLen=1;

    if(strTCT.fCustomerNo!= TRUE)
        return d_OK;	  	

    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);
	   
    while(1)
    {	
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "CUSTOMER NUMBER:");
        memset(strOut,0x00, sizeof(strOut));
        inRet = InputString(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            return d_NO;
        else if(inRet == 0)
            return d_NO;
        else if(inRet == 0xFF)
        {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
        else if(inRet>=1)
        {
			memset(srTransRec.szCustomerNo, 0x00, sizeof(srTransRec.szCustomerNo));
			strcpy(srTransRec.szCustomerNo, strOut); 
			break;
        }	
    }	
    
    return d_OK;
}

int inEnterRedeemAmount(void)
{
    int inRet=0,inPANLen;
    BYTE strOut[12+1];//, szPAN[4+1];
    short shMaxLen=16, shMinLen=1;

    if(strTCT.fProductCode!= TRUE)
        return d_OK;	  	

    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);
	   
    while(1)
    {	
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "REDEEM AMOUNT:");
        memset(strOut,0x00, sizeof(strOut));
        inRet = InputString(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            return d_NO;
        else if(inRet == 0)
            return d_NO;
        else if(inRet == 0xFF)
        {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
        else if(inRet>=1)
        {
			memset(srTransRec.szRedeemAmount, 0x00, sizeof(srTransRec.szRedeemAmount));
			strcpy(srTransRec.szRedeemAmount, strOut); 
			break;
        }	
    }	
    
    return d_OK;
}

void vdExtractLoyaltyData(struct LOYALTY_DATA *stLoyData, char *ptrLoyData)
{ 
    int inIndex=0;
		
    memcpy(stLoyData->szSaleAmount, ptrLoyData+inIndex, 12); inIndex+=12;
    memcpy(stLoyData->szRedeemAmountAmount, ptrLoyData+inIndex, 12); inIndex+=12;
    memcpy(stLoyData->szNetSalesAmount, ptrLoyData+inIndex, 12); inIndex+=12;
    memcpy(stLoyData->szRedeemPoints, ptrLoyData+inIndex, 11); inIndex+=11;
    memcpy(stLoyData->szAwardPoints, ptrLoyData+inIndex, 11); inIndex+=11;
    memcpy(stLoyData->szPointBalance, ptrLoyData+inIndex, 11); inIndex+=11;
    memcpy(stLoyData->szProductCode, ptrLoyData+inIndex, 2); inIndex+=2;
    memcpy(stLoyData->szBeginPointBalance, ptrLoyData+inIndex, 10); inIndex+=10;
    memcpy(stLoyData->szBeginEPurseBalance, ptrLoyData+inIndex, 11); inIndex+=11;
    memcpy(stLoyData->szEPurseBalance, ptrLoyData+inIndex, 11); inIndex+=11;
    memcpy(stLoyData->szTransType, ptrLoyData+inIndex, 2); inIndex+=2;
}
