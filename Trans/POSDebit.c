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
#include "..\Includes\POSDebit.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Pinpad\Pinpad.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

int inDebitSaleGroupIndex=0;
extern BOOL fECRApproved;
extern int inSendECRResponseFlag;
extern BOOL ErmTrans_Approved;
BOOL fBancNetTrans=FALSE;
extern int inSelectedIdleAppsHost;

int inCTOS_DebitSaleFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("SATURN --inCTOS_DebitSaleFlowProcess--");
	vdDebug_LogPrintf("SATURN iOrientation[%d]", iOrientation);

	fECRApproved=FALSE;
	fBancNetTrans=TRUE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char szAscii[INVOICE_ASC_SIZE];

	srTransRec.fDebit=TRUE;
		
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
	
    if(inDebitSaleGroupIndex)
    {
		vdDebug_LogPrintf("SATURN inDebitSaleGroupIndex = [%d]", inDebitSaleGroupIndex);
		if(strTCT.fDebitEMVCapable == TRUE)
        {
            inRet = inCTOS_GetCardFields();
            if(d_OK != inRet)
                return inRet;
            
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
        vdDebug_LogPrintf("saturn call inCTOS_GetCardFieldsNoEMV");
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }

          vdCheckForDualCard();

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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

	
	vdDebug_LogPrintf("strTCT.fAutoLogon:(%02x)", strTCT.fAutoLogon);
	
	inRet = inCTOS_SelectAccountType();
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
	
	inInitializePinPad();
	
	inRet = GetPIN_With_3DESDUKPTEx();	
	if(d_OK != inRet)
		return inRet;			
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
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


int inCTOS_DebitSale(void)
{
    int inRet = d_NO;
     char szAscii[INVOICE_ASC_SIZE];
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	if(isCheckTerminalMP200() == d_OK)
	{
		inRet = inCTOSS_ERM_CheckSlipImage();
		if(d_OK != inRet)
			return inRet;
	}
	
	inDebitSaleGroupIndex=0;
	fBancNetTrans=FALSE;
	
    inRet = inCTOS_DebitSaleFlowProcess();

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

int inCTOS_BancnetSale(void)
{
    int inRet = d_NO;
    inSendECRResponseFlag = get_env_int("ECRRESP"); 
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
	
	inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;
	
    inRet = inCTOS_DebitSaleFlowProcess();

   //if(inSendECRResponseFlag == 1)
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

int inCTOS_DebitBalanceInquiryFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;

	vdCTOS_SetTransType(BAL_INQ);
	
	vdDispTransTitle(BAL_INQ);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
    if(inDebitSaleGroupIndex)
    {
        if(strTCT.fDebitEMVCapable == TRUE)
        {
            inRet = inCTOS_GetCardFields();
            if(d_OK != inRet)
                return inRet;
            
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BAL_INQ);
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

	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	/*
	inRet = inCTOS_PreConnectAndInit();
    if(d_OK != inRet)
        return inRet;
	*/
	
	inInitializePinPad();
	
	//inRet = inGetIPPPin();
	inRet = GetPIN_With_3DESDUKPTEx();
	if(d_OK != inRet)
		return inRet;			

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

	inRet = InDisplayBalance();
	if(d_OK != inRet)
		return inRet;

    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	return d_OK;
}

int inCTOS_DebitBalInq(void)
{
    int inRet = d_NO;
    vdDebug_LogPrintf("---- inCTOS_DebitBalInq ----");
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

    inDebitSaleGroupIndex=0;
	fBancNetTrans=FALSE;
	
    inRet = inCTOS_DebitBalanceInquiryFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_BancnetBalInq(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
	
	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;

    inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;
	
    inRet = inCTOS_DebitBalanceInquiryFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitLogonFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	vdDebug_LogPrintf("inCTOS_DebitLogonFlowProcess");
    	//CTOS_PrinterPutString("inCTOS_DebitLogonFlowProcess");

	vdCTOS_SetTransType(LOG_ON);
	
	//display title
	//vdDispTransTitle(LOG_ON);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	inTCTRead(1);
	
	strCDT.HDTid=strTCT.inLogonHostIndex;
	vdDebug_LogPrintf("strTCT.inLogonHostIndex=[%d]", strTCT.inLogonHostIndex);

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOGON);
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

	/*
	inRet = inCTOS_PreConnectAndInit();
    if(d_OK != inRet)
        return inRet;
	*/
	
	inRet = inCTOS_PreConnect();
	if(d_OK != inRet)
		return inRet;

	//inRet = inCTOS_CheckIssuerEnable();
	//if(d_OK != inRet)
		//return inRet;

	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	inRet = inProcessLogon();
	if(d_OK != inRet)
		return inRet;

	return d_OK;
}

int inCTOS_DebitLogon(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_DebitLogonFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitSelection(void)
{
	char szChoiceMsg[30 + 1];
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04, key=0; 

	memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	strcpy(szHeaderString, "SELECT TRANSACTION");
	strcat(szChoiceMsg,"SALE \n");
	strcat(szChoiceMsg,"BALANCE INQUIRY \n");
	strcat(szChoiceMsg,"LOGON");	

    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
    if (key > 0)
    {
        if (key == 1)
        {
			inCTOS_DebitSale();
        }
        else if(key == 2)
        {
            inCTOS_DebitBalInq();			
        }
        else if(key == 3)
        {
            inCTOS_DebitLogon();
        }
    }
	return d_OK;	
}

int inCTOS_SelectAccountType(void)
{
	//BYTE key;
	int inRet;
	char szDebug[40 + 1]={0};
	char szChoiceMsg[30 + 1];
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04, key=0; 
	
	vdDebug_LogPrintf("inCTOS_SelectAccountType[START]");

	vdDebug_LogPrintf("-->>SELECT ACNT byTransType[%d]", srTransRec.byTransType);

    memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	//if (2 == strTCT.byTerminalType)
	{
		strcpy(szHeaderString, "SELECT ACCOUNT");
		strcat(szChoiceMsg,"SAVINGS \n");
		strcat(szChoiceMsg,"CURRENT");
		key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);

		
		if (key > 0)
		{
			if (key == 1)
			{
				srTransRec.inAccountType =SAVINGS_ACCOUNT;
			}
			if (key == 2)
			{
				srTransRec.inAccountType =CURRENT_ACCOUNT;
			}
			
			if (key == d_KBD_CANCEL)
			{
				return -1;
			}

			return d_OK;
			
		}
	}
	
	return inRet;
}

int inCTOS_CheckSelectDebit(void)
{
    int key;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
	
    if(strCDT.inType == DEBIT_CARD)
		key=d_OK;
	else
	{
		vdSetErrorMessage("DEBIT CARD ONLY");
		key=ST_ERROR;
	}
	return key;
}

int InDisplayBalance(void)
{
    char szAmount[12+1];
    char szStr[12+1];
	char szSign[13+1];
    //TRANS_DATA_TABLE srTransRec;    
//TRANS_DATA_TABLE *srTransPara;

// fix for Wrong implementation of USD Currency
//USD currency prompts upon voiding & settlement
        inTCTRead(1);	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 4, "BALANCE:");

    //CTOS_PrinterPutString(srTransRec.szBalCurrency);
    //CTOS_PrinterPutString(srTransRec.szBalSign);
    //CTOS_PrinterPutString(srTransRec.szBalAmount);
    
    //wub_hex_2_str(srTransRec.szTotalAmount, szAmount, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));
    memset(szSign, 0x00, sizeof(szSign));
	
    //vdCTOS_FormatAmount("NNN,NNN,NNn.nn",szAmount, szStr);

    vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szBalAmount, szStr);	
    setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);

// fix for issue:On balance inqury negative sign not process	
    strcat(szSign, srTransRec.szBalSign);	
    strcat(szSign, szStr);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szStr)+1)*2,  6, szStr);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szSign)+1)*2,  6, szSign);
    
    WaitKey(30);

    return d_OK;
}

// *********************************************************************
// BANCNET CASH_OUT -- sidumili
// *********************************************************************
int inCTOS_BancnetCashOut(void)
{
    int inRet = d_NO;

	vdDebug_LogPrintf("--inCTOS_BancnetCashOut--");
	
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	if(isCheckTerminalMP200() == d_OK)
	{
		inRet = inCTOSS_ERM_CheckSlipImage();
		if(d_OK != inRet)
			return inRet;
	}
	
	inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;
	
    inRet = inCTOS_DebitCashOutFlowProcess();

    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

    if (isCheckTerminalMP200() == d_OK)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	// Preconnect transferred to inCTOSS_UploadReceipt
	    	/*
	    	inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}
			*/

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}
    inMultiAP_ECRSendSuccessResponse();
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitCashOutFlowProcess(void)
{
	int inRet = d_NO;
	//int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_DebitCashOutFlowProcess--");
	//vdDebug_LogPrintf("iOrientation[%d]", iOrientation);

	fECRApproved=FALSE;
	fBancNetTrans=TRUE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char szAscii[INVOICE_ASC_SIZE];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CASH_OUT);
	
	//display title
	vdDispTransTitle(CASH_OUT);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

    vdDisplayECRAmount(); /*display amount for ECR only*/
	
    if(inDebitSaleGroupIndex)
    {
        if(strTCT.fDebitEMVCapable == TRUE)
        {
            inRet = inCTOS_GetCardFields();
            if(d_OK != inRet)
                return inRet;
            
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }


	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_DEBIT_CASH_OUT);
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

	
	vdDebug_LogPrintf("strTCT.fAutoLogon:(%02x)", strTCT.fAutoLogon);
	
	inRet = inCTOS_SelectAccountType();
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
	
	inInitializePinPad();
	
	inRet = GetPIN_With_3DESDUKPT();	
	if(d_OK != inRet)
		return inRet;			
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
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

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;

	}

    fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();	
	
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

int inCTOS_CashIn(void)
{
    int inRet = d_NO;
    inSendECRResponseFlag = get_env_int("ECRRESP"); 
	char szTemp[24+1];
	vdDebug_LogPrintf("---- inCTOS_CashIn ----");
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
	
	inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;

    vdDebug_LogPrintf("saturn with CTLS");
    vdCTOSS_SetWaveTransType(1);
    inRet = inCTOSS_CLMOpenAndGetVersion();
			
    inRet = inCTOS_CashInFlowProcess();

   //if(inSendECRResponseFlag == 1)
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
	
    #ifdef ERM_MODE
	inCTOSS_UploadReceipt(FALSE, FALSE);
	#endif
	
    inMultiAP_ECRSendSuccessResponse();
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CashInFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("SATURN --inCTOS_CashInFlowProcess--");
	vdDebug_LogPrintf("SATURN iOrientation[%d]", iOrientation);

	fECRApproved=FALSE;
	fBancNetTrans=TRUE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char szAscii[INVOICE_ASC_SIZE];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CASH_IN);
	
	//display title
	vdDispTransTitle(CASH_IN);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

    vdDisplayECRAmount(); /*display amount for ECR only*/

	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

    if(inDebitSaleGroupIndex)
    {
        	if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
		vdDebug_LogPrintf("SATURN inDebitSaleGroupIndex = [%d]", inDebitSaleGroupIndex);
		if(strTCT.fDebitEMVCapable == TRUE)
        {
            //inRet = inCTOS_GetCardFields();
            inRet=inCTOS_GetCardFieldsCtls();
            if(d_OK != inRet)
                return inRet;
            
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
        vdDebug_LogPrintf("saturn call inCTOS_GetCardFieldsNoEMV");
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }

          vdCheckForDualCard();

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_CASH_IN);
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

	
	vdDebug_LogPrintf("strTCT.fAutoLogon:(%02x)", strTCT.fAutoLogon);
	
	inRet = inCTOS_SelectAccountType();
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
	
	inInitializePinPad();
	
	inRet = GetPIN_With_3DESDUKPTEx();	
	if(d_OK != inRet)
		return inRet;			
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
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

int inCTOS_CashOut(void)
{
    int inRet = d_NO;
    inSendECRResponseFlag = get_env_int("ECRRESP"); 
    CTOS_LCDTClearDisplay();
    vdDebug_LogPrintf("---- inCTOS_CashOut ----");
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
	
	inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;
	
    inRet = inCTOS_CashOutFlowProcess();

   //if(inSendECRResponseFlag == 1)
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

int inCTOS_CashOutFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("SATURN --inCTOS_CashInFlowProcess--");
	vdDebug_LogPrintf("SATURN iOrientation[%d]", iOrientation);

	fECRApproved=FALSE;
	fBancNetTrans=TRUE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char szAscii[INVOICE_ASC_SIZE];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CASH_OUT);
	
	//display title
	vdDispTransTitle(CASH_OUT);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

    vdDisplayECRAmount(); /*display amount for ECR only*/

	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;
	
    if(inDebitSaleGroupIndex)
    {
		vdDebug_LogPrintf("SATURN inDebitSaleGroupIndex = [%d]", inDebitSaleGroupIndex);
		if(strTCT.fDebitEMVCapable == TRUE)
        {
            //inRet = inCTOS_GetCardFields();
			inRet=inCTOS_GetCardFieldsCtls();
            if(d_OK != inRet)
                return inRet;
            
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
        vdDebug_LogPrintf("saturn call inCTOS_GetCardFieldsNoEMV");
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }

          vdCheckForDualCard();

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_CASH_OUT);
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

	
	vdDebug_LogPrintf("strTCT.fAutoLogon:(%02x)", strTCT.fAutoLogon);
	
	inRet = inCTOS_SelectAccountType();
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
	
	inInitializePinPad();
	
	inRet = GetPIN_With_3DESDUKPTEx();	
	if(d_OK != inRet)
		return inRet;			
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
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

int inCTOS_CardlessCashOut(void)
{
   	int inRet = 0;
	char szAppName[30];

	vdDebug_LogPrintf("inCTOS_CardlessCashOut");

   	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
        return inRet;

   memset(szAppName, 0x00, sizeof(szAppName));
   strcpy(szAppName, "com.Source.S1_BANCNET.BANCNET");
   
   vdDebug_LogPrintf("SATURN inReserveApp1 APPNAME %s", szAppName);

   inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_CARLESS_CASH_OUT);
   
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("saturn inCTOS_CardlessCashOut app FAIL");
	  return inRet;
   }

    return d_OK;
}

int inCTOS_CardlessBalInq(void)
{
	 int inRet = 0;
	 char szAppName[30];
	
	 vdDebug_LogPrintf("inCTOS_CardlessCashOut");
	
	 inRet = inCTOSS_CheckMemoryStatus();
	 if(d_OK != inRet)
		 return inRet;
	
	memset(szAppName, 0x00, sizeof(szAppName));
	strcpy(szAppName, "com.Source.S1_BANCNET.BANCNET");
	
	vdDebug_LogPrintf("SATURN inReserveApp1 APPNAME %s", szAppName);
	
	inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_CARLESS_BAL_INQ);
	
	if(d_OK != inRet)
	{
	   vdDebug_LogPrintf("saturn inCTOS_CardlessCashOut app FAIL");
	   return inRet;
	}
	
	 return d_OK;

}

int inCTOS_CardlessBanking(void)
{
    //BYTE key;
    int inRet;
    char szDebug[40 + 1]={0};
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0; 
    
    vdDebug_LogPrintf("inCTOS_SelectAccountType[START]");
    
    vdDebug_LogPrintf("-->>SELECT ACNT byTransType[%d]", srTransRec.byTransType);
    
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    //if (2 == strTCT.byTerminalType)
    {
        strcpy(szHeaderString, "CARDLESS BANKING");
        strcat(szChoiceMsg,"CARDLESS CASHOUT \n");
        strcat(szChoiceMsg,"CARDLESS BAL INQ");
        key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
        if (key > 0)
        {
            if (key == 1)
            {
                inCTOS_CardlessCashOut();
            }
            if (key == 2)
            {
                inCTOS_CardlessBalInq();
            }
            
            if (key == d_KBD_CANCEL)
            {
                return -1;
            }
            return d_OK;
        }
    }

    return inRet;
}

int inCTOS_Payment(void)
{
    int inRet = d_NO;
    inSendECRResponseFlag = get_env_int("ECRRESP"); 
    CTOS_LCDTClearDisplay();
    vdDebug_LogPrintf("---- inCTOS_Payment ----");
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
	
	inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;
	
    inRet = inCTOS_PaymentFlowProcess();

   //if(inSendECRResponseFlag == 1)
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

int inCTOS_PaymentFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("SATURN --inCTOS_CashInFlowProcess--");
	vdDebug_LogPrintf("SATURN iOrientation[%d]", iOrientation);

	fECRApproved=FALSE;
	fBancNetTrans=TRUE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char szAscii[INVOICE_ASC_SIZE];

	srTransRec.fDebit=TRUE;
	srTransRec.fBillsPaymentCash=VS_FALSE;
		
	vdCTOS_SetTransType(BILLS_PAYMENT);
	
	//display title
	vdDispTransTitle(BILLS_PAYMENT);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

    vdDisplayECRAmount(); /*display amount for ECR only*/

	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;
	
    if(inDebitSaleGroupIndex)
    {
		vdDebug_LogPrintf("SATURN inDebitSaleGroupIndex = [%d]", inDebitSaleGroupIndex);
		if(strTCT.fDebitEMVCapable == TRUE)
        {
            //inRet = inCTOS_GetCardFields();
            inRet = inCTOS_GetCardFieldsCtls();
            if(d_OK != inRet)
                return inRet;
			
            vdDebug_LogPrintf("srTransRec.fBillsPaymentCash: %d", srTransRec.fBillsPaymentCash);
			if(srTransRec.fBillsPaymentCash == VS_TRUE)
			{
				srTransRec.IITid=16; //Issuer Cash
			    srTransRec.CDTid=15;

                inRet = inEnterAdminPassword();
                if(d_OK != inRet)
                    return inRet;
			}
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
        vdDebug_LogPrintf("saturn call inCTOS_GetCardFieldsNoEMV");
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }

    vdCheckForDualCard();

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BILLS_PAYMENT);
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

	
	vdDebug_LogPrintf("strTCT.fAutoLogon:(%02x)", strTCT.fAutoLogon);
	
	inRet = inCTOS_SelectAccountType();
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
	
	inInitializePinPad();
	
	inRet = GetPIN_With_3DESDUKPTEx();	
	if(d_OK != inRet)
		return inRet;			
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
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

int inCTOS_FundTransfer(void)
{
    int inRet = d_NO;
    inSendECRResponseFlag = get_env_int("ECRRESP"); 
    CTOS_LCDTClearDisplay();
    vdDebug_LogPrintf("---- inCTOS_FundTransfer ----");
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
	
	inDebitSaleGroupIndex=1;
	inSelectedIdleAppsHost = BANCNET_HOST;
	fBancNetTrans=TRUE;
	
    inRet = inCTOS_FundTransferFlowProcess();

   //if(inSendECRResponseFlag == 1)
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

int inCTOS_FundTransferFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("SATURN --inCTOS_CashInFlowProcess--");
	vdDebug_LogPrintf("SATURN iOrientation[%d]", iOrientation);

	fECRApproved=FALSE;
	fBancNetTrans=TRUE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char szAscii[INVOICE_ASC_SIZE];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(FUND_TRANSFER);
	
	//display title
	vdDispTransTitle(FUND_TRANSFER);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

    vdDisplayECRAmount(); /*display amount for ECR only*/

	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;
	
    if(inDebitSaleGroupIndex)
    {
		vdDebug_LogPrintf("SATURN inDebitSaleGroupIndex = [%d]", inDebitSaleGroupIndex);
		if(strTCT.fDebitEMVCapable == TRUE)
        {
            //inRet = inCTOS_GetCardFields();
            inRet=inCTOS_GetCardFieldsCtls();
            if(d_OK != inRet)
                return inRet;
            
            strCDT.HDTid = BANCNET_HOST;  
        }
        else
        {
            inRet = inCTOS_GetCardFieldsNoEMV();
            if(d_OK != inRet)
                return inRet;
            
            if(inCheckValidApp(BANCNET_HOST) != d_OK)
                return INVALID_CARD;
        }
    }
    else
    {
        vdDebug_LogPrintf("saturn call inCTOS_GetCardFieldsNoEMV");
		inRet = inCTOS_GetCardFieldsNoEMV();
		if(d_OK != inRet)
			return inRet;
		
        strCDT.HDTid = 19;
    }

          vdCheckForDualCard();

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_FUND_TRANSFER);
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

	
	vdDebug_LogPrintf("strTCT.fAutoLogon:(%02x)", strTCT.fAutoLogon);
	
	inRet = inCTOS_SelectAccountType();
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
	
	inInitializePinPad();
	
	inRet = GetPIN_With_3DESDUKPTEx();	
	if(d_OK != inRet)
		return inRet;			
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
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

int inCTOS_RequestTerminalKey(void)
{
   	int inRet = 0;
	char szAppName[30];

	vdDebug_LogPrintf("inCTOS_RequestTerminalKey");

   	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
        return inRet;

   memset(szAppName, 0x00, sizeof(szAppName));
//   strcpy(szAppName, "com.Source.S1_BANCNET.BANCNET");
   strcpy(szAppName, "V5S_RBDEBIT");
   
   vdDebug_LogPrintf("SATURN inReserveApp1 APPNAME %s", szAppName);

   inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_REQUEST_TERMINAL_KEY);
   
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("saturn inCTOS_RequestTerminalKey app FAIL");
	  return inRet;
   }

    return d_OK;
}

