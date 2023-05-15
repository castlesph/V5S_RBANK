/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <emv_cl.h>
#include <vwdleapi.h>

#include <sys/stat.h>   
#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\display.h"

#include "..\Debug\Debug.h"

#include "..\Includes\CTOSInput.h"

#include "..\comm\v5Comm.h"
#include "..\Accum\Accum.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Comm\V5Comm.h"
#include "..\debug\debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\CardUtil.h"
#include "..\Debug\Debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Ctls\POSCtls.h"
#include "..\Ctls\PosWave.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\print\print.h"
#include "..\Includes\POSDiag.h"
#include <sqlite3.h>


extern BOOL ErmTrans_Approved;
extern BOOL fECRApproved;
extern int inInstallmentGroup;
extern BOOL inDebitSaleGroupIndex;

int inCTOS_WAVE_SALE_DIAG(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    vdCTOS_SetTransType(SALE);
	
#if 0
	inCTOSS_GetRAMMemorySize("UPLOAD PENDING ERM RECEIPT START");
	inRet = inCTOSS_ERM_CheckSlipImage();
	inCTOSS_GetRAMMemorySize("UPLOAD PENDING ERM RECEIPT END");
	if(d_OK != inRet)
		return inRet;	
#endif 
 
    inRet = inCTOS_SaleFlowProcess_Diag();    

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    inMultiAP_ECRSendSuccessResponse();
	
    inCTOS_inDisconnect();

	//if (isCheckTerminalMP200() == d_OK)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	if (get_env_int("CONNECTED") == 0)
			{			
				inRet = inCTOS_PreConnect();
				if (inRet != d_OK)
				{
					vdCTOS_TransEndReset();
					return inRet;
				}
			}

			inCTOSS_GetRAMMemorySize("UPLOAD ERM RECEIPT START");
			inCTOSS_UploadReceipt(FALSE,FALSE);
			inCTOSS_GetRAMMemorySize("UPLOAD ERM RECEIPT END");
	    }
	}
            
    vdCTOS_TransEndReset();
    
    return inRet;
}

int inCTOS_SaleFlowProcess_Diag(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	char   buf[50+1];
	fECRApproved=FALSE;
		
	srTransRec.fCredit=TRUE;
		
	vdCTOS_SetTransType(SALE);
	
	//display title
	vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	vdDisplayECRAmount(); /*display amount for ECR only*/
#if 0	
	inRet = inCTOS_GetCardFields();   
	if(d_OK != inRet)
		return inRet;
#else
	//memset(srTransRec.szPAN,0x00,PAN_SIZE+1);
	//inCTOSS_GetEnvDB ("MCCTEST", srTransRec.szPAN);

	inRet = inTestCardRead(&srTransRec, MCC_CARD_ID);
	vdDebug_LogPrintf("inTestCardRead[%d]",inRet);
	vdDebug_LogPrintf("byPanLen[%d]",srTransRec.byPanLen);
	DebugAddHEX("szExpireDate",srTransRec.szExpireDate,EXPIRY_DATE_BCD_SIZE);
	vdDebug_LogPrintf("szCardholderName[%s]", srTransRec.szCardholderName);
	vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
	vdDebug_LogPrintf("szCardLable[%s]", srTransRec.szCardLable);
	vdDebug_LogPrintf("usTrack1Len[%d]", srTransRec.usTrack1Len);
	vdDebug_LogPrintf("usTrack2Len[%d]", srTransRec.usTrack2Len);
	vdDebug_LogPrintf("usTrack3Len[%d]", srTransRec.usTrack3Len);
	vdDebug_LogPrintf("szTrack1Data[%s]", srTransRec.szTrack1Data);
	vdDebug_LogPrintf("szTrack2Data[%s]", srTransRec.szTrack2Data);
	vdDebug_LogPrintf("szTrack3Data[%s]", srTransRec.szTrack3Data);

	srTransRec.byEntryMode = CARD_ENTRY_FALLBACK;
    if (d_OK != inCTOS_LoadCDTIndex())
    {
        CTOS_KBDBufFlush();
        return USER_ABORT;
    }
#endif

		
	strCDT.HDTid=19; //Catch-all to New host

	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

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
#if 0
	inRet = inCTOS_FraudControl();
	if(d_OK != inRet)
		return inRet;
#endif
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
#if 0
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;
#else
	inRet = inCTOS_GetTxnBaseAmount_Diag();
	if(d_OK != inRet)
		return inRet;
#endif
	
	inRet = inCTOS_PreConnectAndInit();
	if(d_OK != inRet)
		return inRet;
#if 0	
	inRet = inCTOS_GetTxnTipAmount();
	if(d_OK != inRet)
		return inRet;
#endif
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
#if 0
	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
		return inRet;
#endif
	srTransRec.fEMVPINEntered = FALSE;
 
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
			
			if(d_OK != inRet)
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
	inMultiAP_ECRSendSuccessResponse();

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
	
	inRet = ushCTOS_printReceipt_Diag();
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


int inCTOS_GetTxnBaseAmount_Diag(void)
{
    char szDisplayBuf[30];
	char szBaseAmount[50+1];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	//gcitra-0728
	BYTE bFirstKey;
	//gcitra-0728

	//inDatabase_TerminalOpenDatabase(DB_TERMINAL);
	//inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);
	inDatabase_TerminalOpenDatabase();
	
    inTCTRead(1);	
    if(strTCT.fRegUSD == 1) 
        inCSTRead(2);
    else
        inCSTRead(1);

	inDatabase_TerminalCloseDatabase();

	if(srTransRec.fInstallment == TRUE)
		inCTOSS_GetEnvDB("TESTINSTAMT", baAmount);
	else
		inCTOSS_GetEnvDB("TESTTXNAMT", baAmount);
		
	memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%012.0f", atof(baAmount));
    wub_str_2_hex(szTemp,srTransRec.szBaseAmount,12);

	vdDebug_LogPrintf("srTransRec.szBaseAmount[%s]",srTransRec.szBaseAmount);
	DebugAddHEX("BASE AMOUNT",szTemp,6);
	
    return d_OK;
}


int inCTOS_INSTALLMENT_DIAG(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
#if 0
	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
		return inRet;
#endif	
//	vdCTOSS_GetAmt();
	inInstallmentGroup=0;

    inRet = inCTOS_InstallmentFlowProcess_Diag();

    inCTOS_inDisconnect();

    //if (isCheckTerminalMP200() == d_OK)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	if (get_env_int("CONNECTED") == 0)
			{			
				inRet = inCTOS_PreConnect();
				if (inRet != d_OK)
				{
					vdCTOS_TransEndReset();
					return inRet;
				}
			}

			inCTOSS_UploadReceipt(FALSE,FALSE);
	    }
	}

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_InstallmentFlowProcess_Diag(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

	srTransRec.fInstallment=TRUE;

    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
#if 0
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
#endif

	inRet = inTestCardRead(&srTransRec, INST_CARD_ID);
	vdDebug_LogPrintf("inTestCardRead[%d]",inRet);

	srTransRec.byEntryMode = CARD_ENTRY_FALLBACK;
    if (d_OK != inCTOS_LoadCDTIndex())
    {
        CTOS_KBDBufFlush();
        return USER_ABORT;
    }

	strCDT.inInstGroup = 1;
	
	/*Installment: Promo selection*/
    inRet = inCTOS_SelectInstallmentPromo_Diag(strCDT.inInstGroup);
    if(d_OK != inRet)
        return inRet;

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
#if 0
   	inRet = inCTOS_FraudControl();
    if(d_OK != inRet)
        return inRet;
#endif
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

    inRet = inCTOS_GetTxnBaseAmount_Diag();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_PreConnectAndInit();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_SelectInstallmentTerm_Diag();
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
#if 0
    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
 #endif
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
	
    inRet = ushCTOS_printReceipt_Diag();
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

int inCTOS_SelectInstallmentPromo_Diag(int inGroup)
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
#if 0
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
#endif
////////////////////////////////////////////////////////
	srTransRec.fInstallment=TRUE;

	inPRMReadbyinInstGroup(inGroup, &inNumRecs);
	
    //Select Instalment Promo type
    memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szPromoLabelList, 0x00,sizeof(szPromoLabelList));
    strcpy(szHeaderString,"SELECT PROMO");

	if(inNumRecs == 0)
	{
    		CTOS_LCDTClearDisplay();
            	vdDisplayErrorMsg(1, 8, "PROMOS UNAVAILABLE"); //# 00015 - No error message if there is no Promo selection / if promo is disabled
		return(ST_ERROR);
	}

	strCDT.HDTid = strMultiPRM[0].HDTid;		//Choose first available promo
	memcpy(srTransRec.szPromo, strMultiPRM[0].szPromoCode, 2);	
   	strcpy(srTransRec.szPromoLabel, strMultiPRM[0].szPromoLabel);
	vdDebug_LogPrintf("srTransRec.szPromo:(%s)\nkey:(%d)", srTransRec.szPromo, key);
	key = d_OK;
////////////////////////////////////////////////////////
#if 0
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
			return -1;
		}

		strCDT.HDTid = strMultiPRM[key-1].HDTid;
		memcpy(srTransRec.szPromo, strMultiPRM[key-1].szPromoCode, 2);	
        	strcpy(srTransRec.szPromoLabel, strMultiPRM[key-1].szPromoLabel);
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
#endif	
	return(key);	
}

int inCTOS_SelectInstallmentTerm_Diag(void)
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
	vdDebug_LogPrintf("inCTOS_SelectInstallmentTerm");
	inINSRead(strHDT.inHostIndex);
	// to print the correct installment terms
	strcpy(srTransRec.szTerms,strINS.szTerm1);  //Read first term
	strcpy(srTransRec.szTerms1, srTransRec.szTerms);
	
#if 0	
	vdDebug_LogPrintf("inINSRead[%d], strHDT.inHostIndex[%d]", inINSRead(strHDT.inHostIndex), strHDT.inHostIndex);
	vdDebug_LogPrintf("strINS.szTerm1[%s]", strINS.szTerm1);

		
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
#endif

	CTOS_LCDTClearDisplay();
	
    return key;
}


int inCTOS_VOID_DIAG(void)
{
    int inRet = d_NO;
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = {0};
    char szMenu[1024] = {0};

	if(isCheckTerminalMP200() == d_OK)
	{
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
	
    vdCTOS_TxnsBeginInit();
#if 0
	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
		return inRet;
#endif	
    inRet = inCTOS_VoidFlowProcess_Diag();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    inMultiAP_ECRSendSuccessResponse();
    
    inCTOS_inDisconnect();

    if (inRet == d_OK || ErmTrans_Approved)
    {
    	if (get_env_int("CONNECTED") == 0)
		{			
			inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}
		}

		inCTOSS_UploadReceipt(FALSE,FALSE);
    }
        	
    vdCTOS_TransEndReset();
    
    return inRet;
}

int inCTOS_VoidFlowProcess_Diag(void)
{
    int inRet = d_NO;

    fECRApproved=FALSE;
	
    vdCTOS_SetTransType(VOID);
    
    //display title
    vdDispTransTitle(VOID);
#if 0
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
#endif	
    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;
#if 0
    if(fGetECRTransactionFlg() != TRUE)
    {
        inRet = inCTOS_GeneralGetInvoice();
        if(d_OK != inRet)
            return inRet;
    }
#else
	memcpy(srTransRec.szInvoiceNo,strTCT.szLastInvoiceNo, INVOICE_BCD_SIZE);
	DebugAddHEX("szInvoiceNo",srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
#endif
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
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

    inRet = inCTOS_CheckVOID();
    if(d_OK != inRet)
        return inRet;

	if(inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) == d_OK)
	{
#if 0		
		if(inGetMerchantPassword() != d_OK)
            return d_NO;
#endif
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
#if 0 
    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet)
        return inRet;
#endif
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
	inMultiAP_ECRSendSuccessResponse();
	
    inRet = ushCTOS_printReceipt_Diag();
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

int inCTOS_SettleAllOperation_Diag(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;

    vdCTOS_SetTransType(SETTLE);
    
    //display title
    vdDispTransTitle(SETTLE);

	//#00245 - No Password Prompt on Settle ALL
    //inRet = inCTOS_GetTxnPassword();
    //if(d_OK != inRet)
    //   return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inCTOS_SettleAllHosts();
		inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);
	}
	else
	{
		inCTOS_SettleAllHosts();
	}

    return ST_SUCCESS;
}

int inCTOS_BancnetSale_Diag(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
#if 0
	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
		return inRet;
#endif	
	inDebitSaleGroupIndex=1;
	
    inRet = inCTOS_DebitSaleFlowProcess_Diag();
	vdDebug_LogPrintf("inCTOS_DebitSaleFlowProcess_Diag[%d]",inRet);
    inMultiAP_ECRSendSuccessResponse();

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


int inCTOS_DebitSaleFlowProcess_Diag(void)
{
	int inRet = d_NO;

	fECRApproved=FALSE;
		
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	int    inSavings=0;
	
	srTransRec.fDebit=TRUE;
	vdDebug_LogPrintf("*** inCTOS_DebitSaleFlowProcess_Diag ***");
	vdCTOS_SetTransType(SALE);
	
	//display title
	vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
#if 0	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
#endif
    vdDisplayECRAmount(); /*display amount for ECR only*/
	
#if 0
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
#endif
	inRet = inTestCardRead(&srTransRec, BANCNET_CARD_ID);
	vdDebug_LogPrintf("inTestCardRead[%d]",inRet);
	vdDebug_LogPrintf("byPanLen[%d]",srTransRec.byPanLen);
	DebugAddHEX("szExpireDate",srTransRec.szExpireDate,EXPIRY_DATE_BCD_SIZE);
	vdDebug_LogPrintf("szCardholderName[%s]", srTransRec.szCardholderName);
	vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
	vdDebug_LogPrintf("szCardLable[%s]", srTransRec.szCardLable);
	vdDebug_LogPrintf("usTrack1Len[%d]", srTransRec.usTrack1Len);
	vdDebug_LogPrintf("usTrack2Len[%d]", srTransRec.usTrack2Len);
	vdDebug_LogPrintf("usTrack3Len[%d]", srTransRec.usTrack3Len);
	vdDebug_LogPrintf("szTrack1Data[%s]", srTransRec.szTrack1Data);
	vdDebug_LogPrintf("szTrack2Data[%s]", srTransRec.szTrack2Data);
	vdDebug_LogPrintf("szTrack3Data[%s]", srTransRec.szTrack3Data);

	srTransRec.byEntryMode = CARD_ENTRY_FALLBACK;
    if (d_OK != inCTOS_LoadCDTIndex())
    {
        CTOS_KBDBufFlush();
        return USER_ABORT;
    }
	
	if(fGetECRTransactionFlg() == TRUE)
		srTransRec.fECRTxnFlg=TRUE;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE_DIAG);
        vdDebug_LogPrintf("inCTOS_MultiAPSaveData[%d]",inRet);
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
#if 0	
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;
#else
	inSavings = get_env_int("SAVINGS");

	if(inSavings == TRUE)
		srTransRec.inAccountType = SAVINGS_ACCOUNT;
	else
		srTransRec.inAccountType = CURRENT_ACCOUNT;
#endif

	inRet = inCTOS_GetTxnBaseAmount_Diag();
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
#if 0	
	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
		return inRet;
#endif	
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
	inMultiAP_ECRSendSuccessResponse();	
	
	inRet = ushCTOS_printReceipt_Diag();
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

