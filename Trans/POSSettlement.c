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


#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\print\Print.h"
#include "..\Includes\POSHost.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Database\DatabaseFunc.h"
#include "../Debug/Debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\TMS\TMS.h"

extern char szGlobalAPName[25];
extern BOOL ErmTrans_Approved;
BOOL fFailedSettleAll = FALSE;
extern BOOL fMerchantOriented;

int inCTOS_SettlementFlowProcess(void)
{
    int inRet = d_NO;
	BYTE szTitle[25+1];
	BYTE szDisMsg[50];
	
	vdDebug_LogPrintf("saturn --inCTOS_SettlementFlowProcess--");

	ErmTrans_Approved = FALSE;
	
    vdCTOS_SetTransType(SETTLE);

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;
   
    //display title
   // vdDispTransTitle(SETTLE);
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("saturn --inCTOS_SettlementFlowProcess-- test 1");

    inRet = inCTOS_SettlementSelectAndLoadHost();
    if(d_OK != inRet)
        return inRet;

	
	vdDebug_LogPrintf("saturn --inCTOS_SettlementFlowProcess-- test 2");

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLEMENT);
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

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }


	vdDebug_LogPrintf("saturn --inCTOS_SettlementFlowProcess-- test 3");

    vdDebug_LogPrintf("saturn inCTOS_CheckAndSelectMutipleMID");

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

    vdDebug_LogPrintf("saturn inCTOS_ChkBatchEmpty");

	inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

    vdDebug_LogPrintf("saturn inCTOS_DisplaySettleBatchTotal");

	inRet=inCTOS_DisplaySettleBatchTotal(SETTLE, TRUE);
    if(d_OK != inRet)
        return inRet;
	
	//strcpy(szDisMsg, szTitle);
	//strcat(szDisMsg, "|");
	//strcat(szDisMsg, "PROCESSING...");
	//usCTOSS_LCDDisplay(szDisMsg);
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	//CTOS_Delay(1000);

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplaySettleSuccess();
	}

	if (isCheckTerminalMP200() == d_OK)
	{
		inRet = inCTOS_PrintSettleReport(FALSE, TRUE);
	    if(d_OK != inRet)
        return inRet;
	}
	else
	{
		// Capture erm receipt -- sidumili
		if (fFSRMode() == TRUE)
		{
			inRet = inCTOS_PrintSettleReport(FALSE, TRUE);
		    if(d_OK != inRet)
	        return inRet;
		}

		// Print settlement receipt -- sidumili
		inRet = inCTOS_PrintSettleReport(FALSE, FALSE);
		if(d_OK != inRet)
		return inRet;
	}

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;
	
    inRet = inCTOS_SettlementClearBathAndAccum(FALSE);
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
	
    return ST_SUCCESS;
}

int inCTOS_SETTLEMENT(void)
{
     int inRet = d_NO;

	 vdDebug_LogPrintf("--inCTOS_SETTLEMENT--");
     
     CTOS_LCDTClearDisplay();    

	 if (get_env_int("FSRMODE") == 3)
	 {
	 	vdDisplayMultiLineMsgAlign("PLEASE DO", "SETTLE ALL", "", DISPLAY_POSITION_CENTER);
		return d_NO;
	 }
	 
     if (isERMMode() == TRUE)
	 {
	 	inCTOSS_ERM_CheckSlipImage();      
	 }
     
     vdCTOS_TxnsBeginInit();
     
     //vdCTOS_SetTransType(SETTLE); // For ERM Display
 	 if ((isCheckTerminalMP200() != d_OK) && (fFSRMode() == TRUE))
	 {
	   inCTOSS_UploadReceipt(FALSE, TRUE);
	 }
     
     inRet = inCTOS_SettlementFlowProcess();
     
     if (isCheckTerminalMP200() == d_OK || fFSRMode() == TRUE)
     {
          if (inRet == d_OK || ErmTrans_Approved)
          {
               inRet = inCTOS_PreConnect();
               if (inRet != d_OK)
               {
                    vdCTOS_TransEndReset();
                    return inRet;
               }
               
               vdCTOS_SetTransType(SETTLE); // For ERM Display
               inCTOSS_UploadReceipt(TRUE, FALSE);			
          }
     }
     
     vdDebug_LogPrintf("Call TMS %d", inRet);
     if(d_OK == inRet)
     {
          inCTOSS_TMSDownloadRequestAfterSettle();
     }
     
     vdCTOS_TransEndReset();
     
     return inRet;
}



int inCTOS_SettleAMerchant(void)
{
	int inRet = d_NO;
	char szDisplayMsg[50];
	
	vdDebug_LogPrintf("saturn --inCTOS_SettleAMerchant--");

/*
	memset(szDisplayMsg,0x00,sizeof(szDisplayMsg));
	sprintf(szDisplayMsg,"%s",strMMT[0].szMerchantName);
	CTOS_LCDTPrintXY(1, 8, "                   ");
	CTOS_LCDTPrintXY(1, 8, szDisplayMsg);
	CTOS_Delay(800);
	vdDebug_LogPrintf("inCTOS_SettleAMerchant--hostname=[%s]-merchant=[%s]--",strHDT.szHostLabel,strMMT[0].szMerchantName);
*/
	inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

	inRet=inCTOS_DisplaySettleBatchTotal(SETTLE, TRUE);
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

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplaySettleSuccess();
	}

	if (isCheckTerminalMP200() == d_OK)
	{
		inRet = inCTOS_PrintSettleReport(FALSE, TRUE);
	    if(d_OK != inRet)
        return inRet;
	}
	else
	{
		// Capture erm receipt -- sidumili
		if (fFSRMode() == TRUE)
		{
			inRet = inCTOS_PrintSettleReport(FALSE, TRUE);
		    if(d_OK != inRet)
	        return inRet;
		}

		// Print settlement receipt -- sidumili
		inRet = inCTOS_PrintSettleReport(FALSE, FALSE);
		if(d_OK != inRet)
		return inRet;
	} 
		
	if (strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;
	
    inRet = inCTOS_SettlementClearBathAndAccum(FALSE);
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	
	if (isCheckTerminalMP200() == d_OK || fFSRMode == TRUE || ErmTrans_Approved == TRUE)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}

			vdCTOS_SetTransType(SETTLE); // For ERM Display
			inCTOSS_UploadReceipt(TRUE, FALSE);
	    }
	}

	return ST_SUCCESS;
}



int inCTOS_SettleAHost(void)
{
	int inRet = d_NO;
	int inNumOfMit = 0,inNum;
	char szErrMsg[30+1];

	vdDebug_LogPrintf("--inCTOS_SettleAHost--");

	
	//memset(szErrMsg,0x00,sizeof(szErrMsg));
	//sprintf(szErrMsg,"SETTLE %s",strHDT.szHostLabel);
	//CTOS_LCDTPrintXY(1, 8, "                   ");
	//CTOS_LCDTPrintXY(1, 8, szErrMsg);
	inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
	vdDebug_LogPrintf("inNumOfMit=[%d]-----",inNumOfMit);
	fFailedSettleAll = FALSE;
	for(inNum =0 ;inNum < inNumOfMit; inNum++)
	{
		memcpy(&strMMT[0],&strMMT[inNum],sizeof(STRUCT_MMT));
		srTransRec.MITid = strMMT[0].MITid;
	    strcpy(srTransRec.szTID, strMMT[0].szTID);
	    strcpy(srTransRec.szMID, strMMT[0].szMID);
	    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
	    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

        vdCTOS_SetTransType(SETTLE);

	    inRet = inCTOS_SettleAMerchant();			
		if (d_OK != inRet)
		{
			fFailedSettleAll = TRUE;
			memset(szErrMsg,0x00,sizeof(szErrMsg));
		    if (inGetErrorMessage(szErrMsg) > 0)
		    {
		        vdDisplayErrorMsg(1, 8, szErrMsg);
		    }
			vdSetErrorMessage("");
		}			
    }

	return ST_SUCCESS;
}

#ifdef HOST_ONE_BY_ONE
int inCTOS_SettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];


    vdCTOS_SetTransType(SETTLE);
    
    //display title
    vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
	{
	    inNumOfHost = inHDTNumRecord();
		vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
		for(inNum =1 ;inNum <= inNumOfHost; inNum++)
		{
			if(inHDTRead(inNum) == d_OK)
			{
				inCPTRead(inNum);
				srTransRec.HDTid = inNum;
        		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
				memset(szBcd, 0x00, sizeof(szBcd));
			    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
			    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
				
				if (inMultiAP_CheckMainAPStatus() == d_OK)
				{
					//multi AP
			        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL);
			        if (d_OK == inRet)//Current AP
					{
						inRet = inCTOS_SettleAHost();	
					}
					memset(szErrMsg,0x00,sizeof(szErrMsg));
				    if (inGetErrorMessage(szErrMsg) > 0)
				    {
				        vdDisplayErrorMsg(1, 8, szErrMsg);
				    }
					vdSetErrorMessage("");
					
				}
				else
				{
					// only one AP
					inRet = inCTOS_SettleAHost();
		                return inRet;
				}
			}
		}
	}
	else// Sub APP
    {
        inRet = inCTOS_MultiAPGetData();
        if(d_OK != inRet)
            return inRet;

        inRet = inCTOS_MultiAPReloadHost();
        if(d_OK != inRet)
            return inRet;

		inRet = inCTOS_SettleAHost();
            return inRet;
    }

    return ST_SUCCESS;
}
#endif

int inCTOS_SettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;

	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord();
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{

			if (strHDT.inHostIndex == MCC_HOST)
				inDisplayHostToSettle((char*)strHDT.szHostLabel); // sidumili: added to display host to be settled

			if (strcmp(szAPName, strHDT.szAPName)!=0)
				continue;

			//if((inNum==1) || (inNum==2) || (inNum==3) || (inNum==4)) /*do not settle old host apps*/
                //continue;				
				
			inCPTRead(inNum);
			srTransRec.HDTid = inNum;
    		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			memset(szBcd, 0x00, sizeof(szBcd));
		    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
		    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
		    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);

			vdCTOS_SetDateTime();
			
			vdCTOS_SetTransType(SETTLE);
			inCTOS_SettleAHost();
			
		}
	}


	vdDebug_LogPrintf("saturn end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}


int inCTOS_SettleBancnet(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;
	
	inNum = 2;
	if(inHDTRead(inNum) == d_OK)
	{
		vdDebug_LogPrintf("saturn szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);

		inDisplayHostToSettle((char*)strHDT.szHostLabel); // sidumili: added to display host to be settled
			
		inCPTRead(inNum);
		srTransRec.HDTid = inNum;
    	strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		memset(szBcd, 0x00, sizeof(szBcd));
		memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
		inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
		srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);

		vdCTOS_SetDateTime();
			
		vdCTOS_SetTransType(SETTLE);
		//CTOS_SettleAHost();
		
        //inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLEMENT);
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL);
        if(d_OK != inRet)
            return inRet;
			
	}

	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}


int inCTOS_SettleAllOperation(void)
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
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{        
		inCTOS_SettleAllHosts();
		//inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);
		inCTOS_SettleBancnet(); // Bancnet
		//inCTOS_SettleDigitalWallet(); // Wechat/Alipay
		//inCTOS_SettleDigitalWalletAndPHQR();//settle wallet - including phqr
		
	}
	else
	{
		inCTOS_SettleAllHosts();
	}

    return ST_SUCCESS;
}



int inCTOS_SETTLE_ALL(void)
{
    int inRet = d_NO;
	int inYear, inMonth, inDate,inDateGap;
	CTOS_RTC SetRTC;
	char buf[6+1]={0};
	char szYear[2+1]={0};
	char szMonth[2+1]={0};
	char szDate[2+1]={0};
	BYTE szCurrDate[8] = {0};
	int inReturn = 0;

	vdDebug_LogPrintf("saturn --inCTOS_SETTLE_ALL--");
	 
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	if (isERMMode() == TRUE)
	{
		inCTOSS_ERM_CheckSlipImage();       
	}
	
    inRet = inCTOS_SettleAllOperation();
    if(d_OK != inRet)
         return inRet;

	vdDebug_LogPrintf("saturn fFailedSettleAll[%d]", fFailedSettleAll);

	if (!fFailedSettleAll && fFSRMode() == TRUE)
	{
		if (get_env_int("FSRMODE") == 3)
			fMerchantOriented = TRUE;		
		
		put_env_int("FAILEDERMTRXN", 0);
		vdSetFSRMode(0);
	}

    //if(d_OK == inRet)
    //    inCTOSS_TMSDownloadRequestAfterSettle();


	vdDebug_LogPrintf("saturn before ctms update %d", strTCT.byCTMSCallAfterSettle );
	if(strTCT.byCTMSCallAfterSettle == 1){

		inReturn = inCheckAllBatchEmtpy();
		vdDebug_LogPrintf("AAA - inCheckAllBatchEmtpy[%d]", inReturn);
        if(inReturn > 0)
          return d_NO;

		inCTOSS_GetEnvDB("AUTODLDATE", buf);
		vdDebug_LogPrintf("AAA>> AUTODLDATE[%s]", buf);

		if(strcmp(buf,"000000")==0)
        {
            CTOS_RTCGet(&SetRTC);
            memset(szCurrDate, 0x00, sizeof(szCurrDate));
            sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
            vdDebug_LogPrintf("AAA>> inCTOS_CTMSUPDATE szCurrDate[%s]", szCurrDate);
            put_env_char("AUTODLDATE",szCurrDate);

            memset(buf,'\0',sizeof(buf));
			inCTOSS_GetEnvDB("AUTODLDATE", buf);
        }
		
        memcpy(szYear,buf,2);
        memcpy(szMonth,&buf[2],2);
        memcpy(szDate,&buf[4],2);
        
        inYear = atoi(szYear);
        inMonth = atoi(szMonth);
        inDate = atoi(szDate);
		vdDebug_LogPrintf("inYear[%d], inMonth[%d], inDate[%d]", inYear, inMonth, inDate);
        
        CTOS_RTCGet(&SetRTC);
       
       //inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);
       inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
       vdDebug_LogPrintf("AAA>> inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	
        inCTOS_CTMSUPDATE();//after settle call CTMS update, inside fun will check batch status
    }
    vdCTOS_TransEndReset();
    return inRet;
}


int inCTOS_BATCH_TOTAL_Process(void)
{
    int inRet;

	
	vdDebug_LogPrintf("SATURN inCTOS_BATCH_TOTAL_Process");
	
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
       return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return d_NO;

   vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
    CTOS_Delay(1000);

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BATCH_TOTAL);
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

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }


    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return d_NO;

    inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_DisplayBatchTotal();
    inRet = inCTOS_DisplaySettleBatchTotal(BATCH_TOTAL, FALSE);
    if(d_OK != inRet)
        return inRet;

    vdSetErrorMessage("");

    return d_OK;
    
}

int inCTOS_BATCH_REVIEW_Process(void)
{
    int inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
       return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BATCH_TOTAL);
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

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }


    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return;

    inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_DisplayBatchTotal();
    inRet = inCTOS_DisplaySettleBatchTotal(BATCH_TOTAL, FALSE);
    if(d_OK != inRet)
        return inRet;

    vdSetErrorMessage("");

    return d_OK;
    
}

int inCTOS_BATCH_TOTAL(void)
{
    int inRet;


	vdDebug_LogPrintf("SATURN BATCH TOTAL");
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit(); 

    vdDispTransTitle(BATCH_TOTAL);

    inCTOS_BATCH_TOTAL_Process();
    
    vdCTOS_TransEndReset();

    return d_OK;
    
}

int inCTOS_BATCH_REVIEW(void)
{
    int inRet;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit(); 

    //vdDispTransTitle(BATCH_REVIEW);
    
    inCTOS_BATCH_REVIEW_Process();
    
    vdCTOS_TransEndReset();
    
    return d_OK;
    
}

int inCTOS_SettleDigitalWallet(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;
	int i = 0;

	vdDebug_LogPrintf("--inCTOS_SettleDigitalWallet--");
	
	//inNum = 20;
#if 0
	inNum = 24;
	for (i = 0; i < 5; i++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			vdDebug_LogPrintf("saturn szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);

			inDisplayHostToSettle((char*)strHDT.szHostLabel); // sidumili: added to display host to be settled
							
			inCPTRead(inNum);
			srTransRec.HDTid = inNum;
	    	strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			memset(szBcd, 0x00, sizeof(szBcd));
			memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
			inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);

			vdCTOS_SetDateTime();
				
			vdCTOS_SetTransType(SETTLE);
						
	        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLEMENT);
	       				
		}
		
		inNum--;
	}	
#else
	inNum = 24;
	for (i = 0; i < 5; i++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			//vdDebug_LogPrintf("saturn szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);

			//inDisplayHostToSettle((char*)strHDT.szHostLabel); // sidumili: added to display host to be settled
							
			inCPTRead(inNum);
			srTransRec.HDTid = inNum;
	    	strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			memset(szBcd, 0x00, sizeof(szBcd));
			memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
			inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);

			vdCTOS_SetDateTime();
				
			vdCTOS_SetTransType(SETTLE);
						
	        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL);
			break;
	       				
		}
		
		inNum--;
	}	


#endif

	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
	
    return ST_SUCCESS;
}

int inCTOS_SettlePHQR(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;
	int i = 0;

	vdDebug_LogPrintf("--inCTOS_SettleDigitalWallet--");
	
	inNum = 25;
	//for (i = 0; i < 5; i++)
	//{
	if(inHDTRead(inNum) == d_OK){
							
		inCPTRead(inNum);
		srTransRec.HDTid = inNum;
	    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		memset(szBcd, 0x00, sizeof(szBcd));
		memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
		inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
		srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
		vdCTOS_SetDateTime();
				
		vdCTOS_SetTransType(SETTLE);
						
	    inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PHQR_SETTLE);
			//break;
	       				
	}
		
		//inNum--;
	//}	



	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
	
    return ST_SUCCESS;
}


int inCTOS_SettleDigitalWalletAndPHQR(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;
	int i = 0;

	vdDebug_LogPrintf("--inCTOS_SettleDigitalWallet--");
	
	inNum = 25;
	for (i = 0; i < 5; i++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			//vdDebug_LogPrintf("saturn szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);

			//inDisplayHostToSettle((char*)strHDT.szHostLabel); // sidumili: added to display host to be settled
							
			inCPTRead(inNum);
			srTransRec.HDTid = inNum;
	    	strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			memset(szBcd, 0x00, sizeof(szBcd));
			memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
			inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);

			vdCTOS_SetDateTime();
				
			vdCTOS_SetTransType(SETTLE);
						
	        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL_WALLET);
			break;
	       				
		}
		
		inNum--;
	}	



	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
	
    return ST_SUCCESS;
}


// sidumili: added to display host to be settled during auto settlement
int inDisplayHostToSettle(char* szHostName)
{
	BYTE szTemp[30+1] = {0};

	vdDebug_LogPrintf("--vdDisplayHostToSettle--");
	vdDebug_LogPrintf("szHostName=[%s]", szHostName);
	
	memset(szTemp, 0x00, sizeof(szTemp));
	sprintf(szTemp, "SETTLE %s", strHDT.szHostLabel);
	vdDisplayMessageStatusBox(1, 8, szTemp, MSG_PLS_WAIT, MSG_TYPE_INFO);
	CTOS_Delay(1000);
	CTOS_Beep();

	return d_OK;
			
}

