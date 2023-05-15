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
#include "..\Includes\POSInstallment.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Database\DatabaseFunc.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

int inInstallmentGroup=0;
int inCash2Go = 0;

extern BOOL ErmTrans_Approved;

int inCTOS_InstallmentFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    char szAscii[INVOICE_ASC_SIZE];
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_InstallmentFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);
	
	srTransRec.fInstallment=TRUE;

    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if(inInstallmentGroup == 1)
	{
        inRet = inCTOS_GetCardFieldsNoEMV();	
        if(d_OK != inRet)
            return inRet;

        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if((strCDT.inInstGroup != 2) && (strCDT.inInstGroup != 3)) /*diners and sb mc installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	else if(inInstallmentGroup == 2)
	{
        inRet = inCTOS_GetCardFieldsNoEMV();	
        if(d_OK != inRet)
            return inRet;

        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if(strCDT.inInstGroup != 4) /*store card installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	else
	{
        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;
        
        strCDT.inInstGroup=1;
		strCDT.HDTid=19; // FOR NEW HOST 
	}
	
	/*Installment: Promo selection*/
    inRet = inCTOS_SelectInstallmentPromo(strCDT.inInstGroup);
    if(d_OK != inRet)
        return inRet;

     vdCheckForDualCard();

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
    
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_INSTALLMENT);
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
	
    inRet = inCTOS_SelectInstallmentTerm();
    if(d_OK != inRet)
        return inRet;

    inRet =  inCTOS_SelectInstallmentPromo2(strCDT.inInstGroup);
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
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


int inCTOS_CASH2GOFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess");

	//srTransRec.fInstallment=TRUE;
	srTransRec.fCash2Go=TRUE;
	inCash2Go = 1;

    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if(inInstallmentGroup == 1)
	{
        inRet = inCTOS_GetCardFieldsNoEMV();	
        if(d_OK != inRet)
            return inRet;

        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if((strCDT.inInstGroup != 2) && (strCDT.inInstGroup != 3)) /*diners and sb mc installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	else if(inInstallmentGroup == 2)
	{
//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-2");
	
        inRet = inCTOS_GetCardFieldsNoEMV();	
        if(d_OK != inRet)
            return inRet;

        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if(strCDT.inInstGroup != 4) /*store card installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	else
	{
        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;
        
        //if (inMultiAP_CheckSubAPStatus() != d_OK)
        //{
            //if(strCDT.inInstGroup != 1) /*mcc installment*/
            //{
                //vdSetErrorMessage("CARD NOT SUPPORTED");
                //return INVALID_CARD;	
            //}
        //}        
        strCDT.inInstGroup=1;
		strCDT.HDTid=19; // FOR NEW HOST 
	}
	
	/*Installment: Promo selection*/
    inRet = inCTOS_SelectInstallmentPromo(strCDT.inInstGroup);
    if(d_OK != inRet)
        return inRet;

   vdCheckForDualCard();

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-4");
    
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_INSTALLMENT);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-5");
    
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

//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess");

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

	/*
	inRet = inCTOS_PreConnectAndInit();
    if(d_OK != inRet)
        return inRet;
	*/
	
//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-1");

    //inRet = inCTOS_GetTxnTipAmount();
    //if(d_OK != inRet)
        //return inRet;

    inRet = inCTOS_SelectInstallmentTerm();
    if(d_OK != inRet)
        return inRet;


inRet =  inCTOS_SelectInstallmentPromo2(strCDT.inInstGroup);
    if(d_OK != inRet)
        return inRet;


//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-2");

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;

//     CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-3");

    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;   

//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-4");

	inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-5");

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

//    CTOS_PrinterPutString("inCTOS_InstallmentFlowProcess-6");

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

int inCTOS_CASH2GO(void)
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
	
//	vdCTOSS_GetAmt();
	inInstallmentGroup=0;
	inCash2Go = 0;
	
    inRet = inCTOS_CASH2GOFlowProcess();

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

int inCTOS_INSTALLMENT(void)
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
	
//	vdCTOSS_GetAmt();
	inInstallmentGroup=0;

	inCash2Go = 0;
	
    inRet = inCTOS_InstallmentFlowProcess();

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

int inCTOS_DinersINST(void)
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
	
//	vdCTOSS_GetAmt();
	inInstallmentGroup=1;
	inCash2Go = 0;
	
    inRet = inCTOS_InstallmentFlowProcess();

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

int inCTOS_StoreCardINST(void)
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
	
//	vdCTOSS_GetAmt();
	inInstallmentGroup=2;
	
    inRet = inCTOS_InstallmentFlowProcess();

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

int inCTOS_SelectInstallmentPromo(int inGroup)
{
    int inNumRecs = 0,
    key = 0,
    i,
    bHeaderAttr = 0x01+0x04; 
    char szHeaderString[21];
    char szPromoLabelList[100] = {0};
    char szTransactionList[100] = {0};
    int inIndex=0;
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (strCDT.fInstallmentEnable == 0 || strCDT.inType == DEBIT_CARD)
    {
		vdSetErrorMessage("INSTALLMENT NOT ALLWD");
		return(ST_ERROR);
    }

	if(inCash2Go || srTransRec.fCash2Go == TRUE) // no need for promo selection on cash2go transaction - 05282015
	{
		srTransRec.fCash2Go=TRUE;
		strcpy(srTransRec.szPromoLabel, "CASH2GO");
		return d_OK;
	}
	else
		srTransRec.fInstallment=TRUE;
	
    //Select Instalment Promo type
    memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szPromoLabelList, 0x00,sizeof(szPromoLabelList));
    strcpy(szHeaderString,"SELECT PROMO");

    memset(strMultiPRM, 0, sizeof(strMultiPRM));

	vdDebug_LogPrintf("sizeof(strMultiPRM):(%d)", sizeof(strMultiPRM));


    if(inGroup == 2 || inGroup == 3 || inGroup == 4)
	    inPRMReadbyinInstGroupOldHost(inGroup, &inNumRecs);
	else	
	    inPRMReadbyinInstGroup(inGroup, &inNumRecs);

	vdDebug_LogPrintf("inNumRecs(%d)", inNumRecs);

	if(inNumRecs == 0)
	{
    		CTOS_LCDTClearDisplay();
            	vdDisplayErrorMsg(1, 8, "PROMOS UNAVAILABLE"); //# 00015 - No error message if there is no Promo selection / if promo is disabled
		return(ST_ERROR);
	}
	
	for (i=0; i<inNumRecs; i++)
	{
		if (strMultiPRM[i].szPromoLabel[0] != 0)
		{
			inIndex=i;
			strcat((char *)szPromoLabelList, strMultiPRM[i].szPromoLabel);

			
			if (strMultiPRM[i+1].szPromoLabel[0] != 0)
				strcat((char *)szPromoLabelList, (char *)" \n");			
		}
	}

    if(inNumRecs > 1)
        key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szPromoLabelList, TRUE);
    else
        key = inIndex+1;                		

	if (key > 0)
	{
		if (key == d_KBD_CANCEL)
		{
		         //usCTOSS_LCDDisplay( "|PROCESSING...");
		         vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			return -1;
		}
        else if(key == 255)
        {   
             vdDebug_LogPrintf("saturn cancel promo selection");         
             return d_NO;  
        }  

		strCDT.HDTid = strMultiPRM[key-1].HDTid;
		memcpy(srTransRec.szPromo, strMultiPRM[key-1].szPromoCode, 2);	
        strcpy(srTransRec.szPromoLabel, strMultiPRM[key-1].szPromoLabel);
		srTransRec.INSHDTid= strMultiPRM[key-1].INSHDTid;
		//memcpy(srTransRec.szOffsetPeriod, strMultiPRM[key-1].szOffsetPeriod, 4);	
		//memcpy(srTransRec.szFundIndicator, strMultiPRM[key-1].szFundIndicator, 1);	
		//memcpy(srTransRec.szPlanID1, strMultiPRM[key-1].szPlanID1, 10);	
		//memcpy(srTransRec.szPlanGroup, strMultiPRM[key-1].szPlanGroup, 10);	
		//memcpy(srTransRec.szPlanGroupIDX, strMultiPRM[key-1].szPlanGroupIDX, 5);	
		
		vdDebug_LogPrintf("srTransRec.szPromo:(%s)\nkey:(%d)", srTransRec.szPromo, key);
		key = d_OK;
	}

//    CTOS_PrinterPutString(srTransRec.szPromoLabel);		
   // CTOS_PrinterPutString(srTransRec.szOffsetPeriod);
    //CTOS_PrinterPutString(srTransRec.szFundIndicator);
    //CTOS_PrinterPutString(srTransRec.szPlanID1);
    //CTOS_PrinterPutString(srTransRec.szPlanGroup);
    //CTOS_PrinterPutString(srTransRec.szPlanGroupIDX);

	CTOS_LCDTClearDisplay();
	
	return(key);	
}

// get planid / plan group/ plan group idx from PRM2 table.
int inCTOS_SelectInstallmentPromo2(int inGroup)
{
    int inNumRecs = 0,
    key = 0,
    i,
    bHeaderAttr = 0x01+0x04; 
    char szHeaderString[21];
    char szPromoLabelList[100] = {0};
    char szTransactionList[100] = {0};
    int inIndex=0;
    char szTerms[4+1];
    char szOffsetPeriod2[4+1];
    char szOffsetPeriod3[4+1];
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (strCDT.fInstallmentEnable == 0 || strCDT.inType == DEBIT_CARD)
    {
		vdSetErrorMessage("INSTALLMENT NOT ALLWD");
		return(ST_ERROR);
    }


	if(inCash2Go)
		srTransRec.fCash2Go=TRUE;
	else
		srTransRec.fInstallment=TRUE;

    	//CTOS_PrinterPutString("inCTOS_SelectInstallmentPromo2");
    	//CTOS_PrinterPutString(srTransRec.szPromoLabel);

	//Select Instalment Promo type
	memset(szHeaderString, 0x00,sizeof(szHeaderString));
	memset(szPromoLabelList, 0x00,sizeof(szPromoLabelList));

	memset(szTerms, 0, sizeof(szTerms));
	memset(szOffsetPeriod2, 0, sizeof(szOffsetPeriod2));
	memset(szOffsetPeriod3, 0, sizeof(szOffsetPeriod3));

	//CTOS_PrinterPutString("inCTOS_SelectInstallmentPromo2");	
	sprintf(szTerms, "%04d", atol(srTransRec.szTerms));
	//CTOS_PrinterPutString(szTerms);


	memset(strMultiPRM2, 0, sizeof(strMultiPRM2));

	inPRMReadbyinInstGroup2(inGroup, &inNumRecs);
	
	for (i=0; i<inNumRecs; i++)
	{
			//strcpy((char *)szPromoLabelList, strMultiPRM2[i].szPromoLabel2);
			//strcpy((char *)szOffsetPeriod2, strMultiPRM2[i].szOffsetPeriod2);
			//memcpy(srTransRec.szOffsetPeriod, &strMultiPRM2[i].szOffsetPeriod2[0], 4); // get offset perion
			memcpy(srTransRec.szTermsEX, &strMultiPRM2[i].szTermsEX2[0], 4); // get offset perion

    		//CTOS_PrinterPutString(szPromoLabelList);
    		//CTOS_PrinterPutString(szOffsetPeriod3);
	
		if((strcmp(strMultiPRM2[i].szPromoLabel2,  srTransRec.szPromoLabel) == 0) && (strcmp(srTransRec.szTermsEX, szTerms) == 0))
		{
	//CTOS_PrinterPutString("PUMASOK DITOO!!!");

				memcpy(srTransRec.szOffsetPeriodEX, &strMultiPRM2[i].szOffsetPeriod2[0], 4); // get offset perion				
				memcpy(srTransRec.szFundIndicator, &strMultiPRM2[i].szFundIndicator2[0], 1);	 // get fund indicator
				memcpy(srTransRec.szPlanID1, &strMultiPRM2[i].szPlanID12[0], 10);			// get plan id
				memcpy(srTransRec.szPlanGroup, &strMultiPRM2[i].szPlanGroup2[0], 10);		//get plan group
				memcpy(srTransRec.szPlanGroupIDX, &strMultiPRM2[i].szPlanGroupIDX2[0], 5);	 // get plan group idx


		break;	
			
		}
		if((strncmp(strMultiPRM2[i].szPromoLabel2,  srTransRec.szPromoLabel, 3) == 0) && (strcmp(srTransRec.szTermsEX, szTerms) == 0))
		{

				memcpy(srTransRec.szOffsetPeriodEX, &strMultiPRM2[i].szOffsetPeriod2[0], 4); // get offset perion				
				memcpy(srTransRec.szFundIndicator, &strMultiPRM2[i].szFundIndicator2[0], 1);	 // get fund indicator
				memcpy(srTransRec.szPlanID1, &strMultiPRM2[i].szPlanID12[0], 10);			// get plan id
				memcpy(srTransRec.szPlanGroup, &strMultiPRM2[i].szPlanGroup2[0], 10);		//get plan group
				memcpy(srTransRec.szPlanGroupIDX, &strMultiPRM2[i].szPlanGroupIDX2[0], 5);	 // get plan group idx


		break;	
			
		}
	}
#if 0
    CTOS_PrinterPutString(srTransRec.szOffsetPeriod);
    CTOS_PrinterPutString(srTransRec.szFundIndicator);
    CTOS_PrinterPutString(srTransRec.szPlanID1);
    CTOS_PrinterPutString(srTransRec.szPlanGroup);
    CTOS_PrinterPutString(srTransRec.szPlanGroupIDX);
#endif
	//return(key);	

	CTOS_LCDTClearDisplay();

	return(ST_SUCCESS);
}


int inCTOS_SelectInstallmentTerm(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT TERM";
	char szTermMenu[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;
    
    memset(szTermMenu, 0x00, sizeof(szTermMenu));
    memset(szTerm, 0x00, sizeof(szTerm));

//    CTOS_PrinterPutString("inCTOS_SelectInstallmentTerm");

    memset(&strINS, 0, sizeof(strINS));
	vdDebug_LogPrintf("xinCTOS_SelectInstallmentTerm");

    vdDebug_LogPrintf("xsrTransRec.INSHDTid:%d", srTransRec.INSHDTid);
	
	if(srTransRec.INSHDTid > 0)
	{
	   vdDebug_LogPrintf("srTransRec.INSHDTid > 0");
	   inINSRead(srTransRec.INSHDTid);
	}
	else	
	   inINSRead(strHDT.inHostIndex);
		
    if(strINS.fTerm1Enable == TRUE)   
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm1); 
		inTermIndex++;
    }

    if(strINS.fTerm2Enable == TRUE)     
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm2); 
		inTermIndex++;
    }

    if(strINS.fTerm3Enable == TRUE) 
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm3); 
		inTermIndex++;
    }
	
    
    if(strINS.fTerm4Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm4); 
		inTermIndex++;
    }
    
    if(strINS.fTerm5Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm5); 
		inTermIndex++;
    }
    
    if(strINS.fTerm6Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm6); 
		inTermIndex++;
    }
    
    if(strINS.fTerm7Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm7); 
		inTermIndex++;
    }
     
    if(strINS.fTerm8Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm8); 
		inTermIndex++;
    }
    
    if(strINS.fTerm9Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm9); 
		inTermIndex++;
    }
    
    if(strINS.fTerm10Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm10); 
		inTermIndex++;
    }
    
    if(strINS.fTerm11Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm11); 
		inTermIndex++;
    }
    
    if(strINS.fTerm12Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm12); 
		inTermIndex++;
    }	

    for (inLoop = 0; inLoop < 50; inLoop++)
    {
        if (szTerm[inLoop][0]!= 0)
        {
			inIndex=inLoop;
            strcat((char *)szTermMenu, szTerm[inLoop]);
			strcat((char *)szTermMenu, (char *)" MONTHS"); // fix for Add months on payment terms 			
            if (szTerm[inLoop+1][0]!= 0)
                strcat((char *)szTermMenu, (char *)" \n"); 
			vdDebug_LogPrintf("inLoop:(%d), szTerm[%s]", inLoop, szTerm[inLoop]);
        }
        else
            break;
    }

    vdDebug_LogPrintf("all szTermMenu[%s]", szTermMenu);

	if(inTermIndex > 1)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szTermMenu, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

        strcpy(srTransRec.szTerms,szTerm[key-1]);  
		key=d_OK;
    }

	// to print the correct installment terms
	strcpy(srTransRec.szTerms1, srTransRec.szTerms);

	CTOS_LCDTClearDisplay();
	
    return key;
}

