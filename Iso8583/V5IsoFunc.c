#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <emv_cl.h>

#include "../Includes/wub_lib.h"
#include "../Includes/Encryption.h"
#include "../Includes/msg.h"
#include "../Includes/myEZLib.h"

#include "../Includes/V5IsoFunc.h"
#include "../Includes/POSTypedef.h"
#include "../Comm/V5Comm.h"
#include "../FileModule/myFileFunc.h"
#include "../UI/Display.h"
#include "../Includes/Trans.h"
#include "../UI/Display.h"
#include "../Accum/Accum.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "../DataBase/DataBaseFunc.h"
#include "../Includes/POSTrans.h"
#include "..\Debug\Debug.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\ISOEnginee.h"
#include "..\Includes\EFTSec.h"

#include "../ctls/POSCtls.h"

#include <emvaplib.h>

#include "..\Includes\TLESec.h"
#include "../Print/Print.h"
#include "../Debug/debug.h"


int inDataCnt;
BYTE TempKey;
int inFinalSend;
BYTE szDataForMAC[512];
int  inMacMsgLen;


BYTE byField_02_ON;
BYTE byField_14_ON;
BYTE byField_35_ON;
BYTE byField_45_ON;
BYTE byField_48_ON;

int inLenTermSerialNo=0;

//int inGlobalCount = 0;
VS_BOOL inGlobalCount = 0;  // fix for 1. Performed sale transaction 2. Prints DE 63 first: Parsing not in order

extern BOOL fPrinterStart;
BOOL fISOLog = 0;

//extern BOOL gblDCCTrans;

BYTE gblszForeignAmountfld63[12+1];
BYTE gblszPrintExchangeRate[12+1];



ISO_FUNC_TABLE srIsoFuncTable[] =
{
    {
        inPackMessageIdData,/*inPackMTI*/
		inPackPCodeData,/*inPackPCode*/
        vdModifyBitMapFunc,/*vdModifyBitMap*/
        inCheckIsoHeaderData,/*inCheckISOHeader*/
        inAnalyseReceiveData,/*inTransAnalyse*/
        inAnalyseAdviceData  /*inAdviceAnalyse*/
    },
};

static TRANS_DATA_TABLE *ptsrISOEngTransData = NULL;

void vdSetISOEngTransDataAddress(TRANS_DATA_TABLE *srTransPara)
{
    ptsrISOEngTransData= srTransPara;
}

TRANS_DATA_TABLE* srGetISOEngTransDataAddress(void)
{
    return ptsrISOEngTransData;
}



void vdDispTextMsg(char *szTempMsg)
{    
//    CTOS_LCDUClearDisplay();
//    CTOS_LCDTPutchXY(1,8,"TEST");
//    CTOS_KBDGet(&TempKey);
}

void vdDecideWhetherConnection(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**vdDecideWhetherConnection START**");
#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "[%d]:[%d]:[%d]", srTransPara->byTransType, srTransPara->byOrgTransType, srTransPara->byUploaded);
    	CTOS_PrinterPutString(szTemp);
	
}
#endif    
	
    
    vdMyEZLib_LogPrintf(". BefTrnsType(%d) UpLoad(%d) Offline (%d)",srTransPara->byTransType,
                                                              srTransPara->byUploaded,
                                                              srTransPara->byOffline);
    
    if(srTransPara->byTransType != VOID)
    {    
        if(srTransPara->byTransType == SALE_TIP || srTransPara->byTransType == SALE_ADJUST)
        {
            srTransPara->byOffline = CN_TRUE;
           
        }
        else if(srTransPara->byTransType == SALE_OFFLINE)// || srTransPara->byTransType == PRE_COMP)
        {
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
        }
        else if(srTransPara->byTransType == SALE &&
            srTransPara->byEntryMode == CARD_ENTRY_ICC &&
            srTransPara->shTransResult == TRANS_AUTHORIZED &&
            0 == memcmp(&srTransPara->szAuthCode[0], "Y1", 2) && 
            0x40 == srTransPara->stEMVinfo.T9F27)
        {
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
        }
        else if(srTransPara->byTransType == SALE &&
            srTransPara->byEntryMode != CARD_ENTRY_ICC &&
            srTransPara->shTransResult == TRANS_AUTHORIZED &&
            srTransPara->byOffline == CN_TRUE &&
            fAmountLessThanFloorLimit() == d_OK )
        {
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
        }
        else
        {
            srTransPara->byUploaded = CN_TRUE;
            srTransPara->byOffline = CN_FALSE;
        }
    }
    else
    {
        //Start Should be Online void the Intial SALE amount.
        if(srTransPara->byOrgTransType == SALE)
        {
            if(srTransPara->byUploaded == CN_FALSE) //Y1 or below floor limit, and not upload yet
                srTransPara->byOffline = CN_TRUE;
            else
                srTransPara->byOffline = CN_FALSE;
        }
        else if(srTransPara->byOrgTransType == SALE_OFFLINE) // || srTransPara->byOrgTransType == PRE_COMP)
        {
            if(srTransPara->byUploaded == CN_FALSE)
                srTransPara->byOffline = CN_TRUE;
            else
                srTransPara->byOffline = CN_FALSE; //piggy back and upload to host already
        }
        else if(srTransPara->byOrgTransType == SALE_TIP)
        {
            if(srTransPara->byUploaded == CN_FALSE) //Y1 or below floor limit, and not upload yet
                srTransPara->byOffline = CN_TRUE;
            else
                srTransPara->byOffline = CN_FALSE;
        }
        //End Should be Online void the Intial SALE amount.
    }
    vdDebug_LogPrintf(". AftTrnsType(%d) srTransPara->byOrgTransType[%d]byEntryMode[%d] shTransResult[%d] szAuthCode[%s] 9F27[%02X] UpLoad(%d) Offline (%d)",
                                                              srTransPara->byTransType,
                                                              srTransPara->byOrgTransType,
                                                              srTransPara->byEntryMode,
                                                              srTransPara->shTransResult,
                                                              srTransPara->szAuthCode,
                                                              srTransPara->stEMVinfo.T9F27,
                                                              srTransPara->byUploaded,
                                                              srTransPara->byOffline);
    
    vdMyEZLib_LogPrintf("**vdDecideWhetherConnection END**");
    return;
}


/************************************************************************
Function Name: inBuildAndSendIsoData()
Description:
    To generate iso data and send to host
Parameters:
    [IN] srTransPara

Return: ST_SUCCESS  
        ST_ERROR
        TRANS_COMM_ERROR
        ST_SEND_DATA_ERR
        ST_UNPACK_DATA_ERR
    
************************************************************************/
int inBuildAndSendIsoData(void)
{
    int inResult,i;
    char szErrorMessage[30+1];
    char szBcd[INVOICE_BCD_SIZE+1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;
    TRANS_DATA_TABLE srTransTemp;

	
	BYTE szDisMsg[50];
	BYTE szTitle[25+1];


	vdDebug_LogPrintf("saturn inBuildAndSendIsoData");

    /*albert - start - fix stan issue*/ 
    //memset(szBcd, 0x00, sizeof(szBcd));
	//memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
	//inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
	/*albert - end - fix stan issue*/

	//inGlobalCount = 0; // fix for 1. Performed sale transaction 2. Prints DE 63 first: Parsing not in order
//    CTOS_PrinterPutString("inBuildAndSendIsoData");
	
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
	// fix issue: DE 60 = 0200 STAN of Orignal and Orignal RRN #1
    //if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
        //srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;
    inHDTSave(strHDT.inHostIndex);
    
    srTransPara = &srTransRec;

	vdDebug_LogPrintf("srTransRec.ulTraceNum: %d", srTransRec.ulTraceNum);
	vdDebug_LogPrintf("srTransPara->ulTraceNum: %d", srTransPara->ulTraceNum);
	vdDebug_LogPrintf("strHDT.inHostIndex: %d", strHDT.inHostIndex);

	vdDebug_LogPrintf("**inBuildAndSendIsoData V02**");
	inCTLOS_Updatepowrfail(PFR_BEGIN_SEND_ISO);

    strHDT.fSignOn = CN_TRUE; //notsupport SignON
        
    if(strHDT.fSignOn == CN_FALSE)
    {
        if( (srTransRec.byTransType == SALE)    ||  // SALE
            (srTransRec.byTransType == REFUND)  ||  // REFUND
            (srTransRec.byTransType == PRE_AUTH)||  // PREAUTH
            (srTransRec.byTransType == CASH_ADV)||
            (srTransRec.byTransType == LOY_REDEEM_VARIABLE)||
            (srTransRec.byTransType == SETTLE)    // SETTLE
          )
        {
            i = srTransPara->HDTid;
            inHDTRead(srTransPara->HDTid);
            memcpy(&srTransParaTmp, srTransPara, sizeof(TRANS_DATA_TABLE));
            memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            CTOS_LCDTClearDisplay();
			
			DebugAddSTR("Sign on false","---",12);  
            //SignOnTrans(i);            
            if(strHDT.fSignOn == CN_FALSE)
            {
                srTransRec.shTransResult = TRANS_TERMINATE;
				vdSetErrorMessage("TRANS TERMINATE");
                inCTOS_inDisconnect();
                return ST_ERROR;
            }
            memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            CTOS_LCDTClearDisplay();
            memcpy(srTransPara, &srTransParaTmp, sizeof(TRANS_DATA_TABLE));
            inHDTRead(srTransPara->HDTid);
            srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
        }
    }
    //CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

	#if 0
	if ((strTCT.byTerminalType % 2) == 0)
		setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing...        ");
	else	
    	setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing...        ");  
	#else
		
    vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		
	#endif
	
    vdDecideWhetherConnection(srTransPara);
    
    if (CN_FALSE == srTransPara->byOffline)
    {        
        ULONG ulTraceNum; 		

		if(srTransPara->byTransType == SETTLE)
		{
			strMMT[0].fMustSettFlag = CN_TRUE;
			inMMTSave(strMMT[0].MMTid);
		}

		/*save and restore transaction after doing reversal*/
        memset(&srTransTemp, 0x00, sizeof(TRANS_DATA_TABLE));
        memcpy(&srTransTemp, &srTransRec, sizeof(TRANS_DATA_TABLE));			
		
        if(VS_TRUE == strTCT.fDemo)
        {
            vdDebug_LogPrintf("DEMO Call Connect!!");
        }
        else
        {
            vdDebug_LogPrintf("Call Connect!!sharecom[%d]",strTCT.fShareComEnable);

			if(srTransPara->fLogon == TRUE)
                srTransPara->fLogon=FALSE;
			else
			{
                if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
                {
                    vdDebug_LogPrintf("AAA-srTransPara->byTransType[%d] fGetECRTransactionFlg[%d]", srTransPara->byTransType, fGetECRTransactionFlg());
					if ((srTransPara->byTransType==VOID) && (fGetECRTransactionFlg()==TRUE))
                    {
                         memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
                         memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
                         strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
                         strcpy(srTransRec.szResponseText, ECR_COMM_ERROR_RESP);
                    }
                    if(srTransPara->shTransResult == 0)
                        srTransPara->shTransResult = TRANS_COMM_ERROR;
                    inCTOS_inDisconnect();
                    vdSetErrorMessage("CONNECT FAILED");
                    if(strCPT.fCommBackUpMode == CN_TRUE && strCPT.inCommunicationMode != DIAL_UP_MODE)
                    {
                        if(inCTOS_CommsFallback(strHDT.inHostIndex) != d_OK)
                            return ST_ERROR;
                        
                        if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
                        {
                            if(srTransPara->shTransResult == 0)
                                srTransPara->shTransResult = TRANS_COMM_ERROR;
							
							CTOS_Beep();
							CTOS_Delay(1500);

                            inCTOS_inDisconnect();
                            vdSetErrorMessage("CONNECT FAILED"); 
							return ST_ERROR;
                        }						
                    }
					else
                        return ST_ERROR;    
                }		
			}
        }

		vdDebug_LogPrintf("connect success");
        if (inProcessReversal(srTransPara) != ST_SUCCESS)            
        {
            inCTOS_inDisconnect();
			//vdSetErrorMessage("Reversal Error");
			vdSetErrorMessage("NO RESPONSE FROM HOST");
            return ST_ERROR;
        }
        
        if(srTransPara->byTransType == SETTLE)
        {
 // no need to send tc upload on new host - mfl       
#if 0        
            if ((inResult = inProcessEMVTCUpload(srTransPara, -1)) != ST_SUCCESS)
            {
                vdDebug_LogPrintf(". inProcessAdviceTrans(%d) ADV_ERROR!!", inResult);
                inCTOS_inDisconnect();
				vdSetErrorMessage("TC Upload Error");
                return ST_ERROR;
            }
#endif            
            if ((inResult = inProcessAdviceTrans(srTransPara, -1)) != ST_SUCCESS)
            {
                vdDebug_LogPrintf(". inProcessAdviceTrans(%d) ADV_ERROR!!", inResult);
                inCTOS_inDisconnect();
				vdSetErrorMessage("Advice Error");
                return ST_ERROR;
            }
        }

		/*save and restore transaction after doing reversal*/
		ulTraceNum=srTransPara->ulTraceNum;
        memcpy(srTransPara, &srTransTemp, sizeof(TRANS_DATA_TABLE));
		srTransPara->ulTraceNum=ulTraceNum;
    }
    else
    {
        CTOS_RTC SetRTC;
        BYTE szCurrentTime[20];
    
        CTOS_RTCGet(&SetRTC);
	    sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,srTransPara->szDate,DATE_ASC_SIZE);
        sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        wub_str_2_hex(szCurrentTime,srTransPara->szTime,TIME_ASC_SIZE);
    }
    
    if(srTransPara->byTransType == SETTLE)
    {
        strMMT[0].fMustSettFlag = CN_TRUE;
        inMMTSave(strMMT[0].MMTid);
    }
    else
    {
        /*for TLE field 57*/
        byField_02_ON = 0;
        byField_14_ON = 0;
        byField_35_ON = 0;
        byField_45_ON = 0;
        byField_48_ON = 0;

	// fix issue: DE 60 = 0200 STAN of Orignal and Orignal RRN #2
	if ((VOID != srTransPara->byTransType) && (SALE_TIP != srTransPara->byTransType))
            srTransPara->ulOrgTraceNum = srTransPara->ulTraceNum;		
    }

    srTransPara->byContinueTrans = CN_FALSE;
    do
    {
        if (CN_FALSE == srTransPara->byOffline)
        {
            if(srTransPara->byTransType == SETTLE)
            {
                vdCTOS_SetDateTime();
                memcpy(srTransPara->szDate, srTransRec.szDate, DATE_BCD_SIZE);
                memcpy(srTransPara->szTime, srTransRec.szTime, TIME_BCD_SIZE);
            }
			
            if ((inResult = inBuildOnlineMsg(srTransPara)) != ST_SUCCESS)
            {
            	if (ST_RESP_MATCH_ERR == inResult)
            	{
            		vdDebug_LogPrintf("inBuildOnlineMsg %d",inResult);
                    return inResult;
            	}
                if ((srTransPara->byEntryMode == CARD_ENTRY_ICC) &&
                    (srTransPara->byTransType == SALE || srTransPara->byTransType == PRE_AUTH))
                { 
                    if((inResult == ST_SEND_DATA_ERR) ||
                       (inResult == ST_UNPACK_DATA_ERR))
                    {
                        srTransPara->shTransResult = TRANS_COMM_ERROR;
		     #if 0 
			   return inResult;
		     #else //AAA USE #IF ON PRODUCTION THIS CERTIFSETUP IS FOR JCB CERTIFICATION ONLY!!! PLEASE DELETE
		      if(get_env_int("CERTIFSETUP")!=1)
		           return inResult;
		      else
			  strcpy(srTransPara->szAuthCode,"Y3");
		      #endif
                    }
                    vdDebug_LogPrintf("CARD_ENTRY_ICC Flow");
                } 
                else
                {
                    vdDebug_LogPrintf("inBuildOnlineMsg %d",inResult);
                    return inResult;
                }
            }
        }
        else
        {
            /* If the transaction is completed, 
               offline transaction do not need to analyze any information*/
            if (inProcessOfflineTrans(srTransPara) != ST_SUCCESS)
            {
                vdDebug_LogPrintf("inProcessOfflineTrans Err");
                inCTOS_inDisconnect();
                return ST_ERROR;
            }
            else
                break;
        }


        if (inAnalyseIsoData(srTransPara) != ST_SUCCESS)
        {
            vdDebug_LogPrintf("inAnalyseIsoData Err byTransType[%d]shTransResult[%d]srTransPara->szRespCode[%s]", srTransPara->byTransType, srTransRec.shTransResult, srTransPara->szRespCode);
            if((srTransPara->byTransType == SETTLE) && (!memcmp(srTransPara->szRespCode,"95",2)))
            {
	            // Minimize the white screend display -- sidumili
				if (isCheckTerminalMP200() == d_OK)
					vdCTOS_DispStatusMessage("PROCESSING...");
				
                inResult = inPorcessTransUpLoad(srTransPara);
                if(inResult != ST_SUCCESS)
                {
                    srTransRec.shTransResult = TRANS_COMM_ERROR;
                    inCTOS_inDisconnect();

					memset(szTitle, 0x00, sizeof(szTitle));
					szGetTransTitle(srTransRec.byTransType, szTitle);
                    /* MCC: Display the correct error message if batch upload error or settlement recon err -- jzg */
                    if(inResult == ST_CLS_BATCH_ERR){
                        //vdSetErrorMessage("SETTLE FAILED");
                        
						strcpy(szDisMsg, szTitle);
						strcat(szDisMsg, "|");
						strcat(szDisMsg, "SETTLE FAILED");
						//usCTOSS_LCDDisplay(szDisMsg);
						vdDisplayMessageBox(1, 8, "", "SETTLE FAILED", "", MSG_TYPE_ERROR);
						CTOS_Beep();
						CTOS_Delay(1500);
                    }else{
                        //vdSetErrorMessage("BATCH UPLOAD ERROR");
                        
						strcpy(szDisMsg, szTitle);
						strcat(szDisMsg, "|");
						strcat(szDisMsg, "BATCH UPLOAD ERROR");
						//usCTOSS_LCDDisplay(szDisMsg);
						vdDisplayMessageBox(1, 8, "", "BATCH UPLOAD ERROR", "", MSG_TYPE_ERROR);
						CTOS_Beep();
						CTOS_Delay(1500);
                    }
                    
                    return ST_ERROR;
                }		
            }	
	        else
            {
                 if((strHDT.fReversalEnable == CN_TRUE) && (srTransPara->byTransType != SETTLE) && (srTransRec.shTransResult == TRANS_REJECTED || srTransRec.shTransResult == TRANS_CALL_BANK))
                 {
                     inMyFile_ReversalDelete();
                 }
                 
                 inCTOS_inDisconnect();
                 
                 if((inGetErrorMessage(szErrorMessage) > 0) || (srTransRec.shTransResult == TRANS_REJECTED))
                 {
                     vdDebug_LogPrintf("2nd AC failed or Host reject");
                 }
                 return ST_ERROR;
            }            
        }
        else
        {
            if(srTransPara->byTransType != VOID)
			   vdCTOS_SyncHostDateTime();        	
			
            if((strHDT.fReversalEnable == CN_TRUE) && (srTransPara->byTransType != SETTLE))
            {
                inMyFile_ReversalDelete();
            }
        }
    
    } while (srTransPara->byContinueTrans);

	DebugAddSTR("inBuildAndSendIsoData","end",20);

	//tine/sidumili: android - set ui thread to display nothing
	DisplayStatusLine(" ");

    return ST_SUCCESS;
    
}

int inProcessLogon(void)
{
    int inResult,i;
    char szErrorMessage[30+1];
    char szBcd[INVOICE_BCD_SIZE+1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;
	
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
        srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;
    inHDTSave(strHDT.inHostIndex);

    srTransPara = &srTransRec;

	if ((strTCT.byTerminalType % 2) == 0)
		setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "PROCESSING...        ");
	else	
    	setLCDPrint(8, DISPLAY_POSITION_LEFT, "PROCESSING...        ");  
	
	srTransPara->byOffline = CN_FALSE;
	
    if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
    {
        if(srTransPara->shTransResult == 0)
            srTransPara->shTransResult = TRANS_COMM_ERROR;
        inCTOS_inDisconnect();
        vdSetErrorMessage("CONNECT FAILED");
        return ST_ERROR;
    }	

    vdIncSTAN(srTransPara);
	
	if ((inResult = inBuildOnlineMsg(srTransPara)) != ST_SUCCESS)
	{
		inTCTSave(1);
		if (ST_RESP_MATCH_ERR == inResult)
		{
			vdDebug_LogPrintf("inBuildOnlineMsg %d",inResult);
			return inResult;
		}
		memset(strTCT.szLogonDate, 0x00, sizeof(strTCT.szLogonDate));
		return inResult;
	}
    else
    {
        if ((strTCT.byTerminalType % 2) == 0)
            setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "LOGON SUCCESSFUL");
        else	
            setLCDPrint(8, DISPLAY_POSITION_LEFT, "LOGON SUCCESSFUL");  
		CTOS_Delay(1000);
    }
	
    return ST_SUCCESS;
}

int inProcessRegistration(void)
{
    int inResult,i;
    char szErrorMessage[30+1];
    char szBcd[INVOICE_BCD_SIZE+1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;
     int inRet = d_NO;
     int inNumOfMit = 0,inNum;
     char szErrMsg[30+1];
     char szDisplayMsg[40];
     int inLine=8;
     
     BYTE szTitle[25+1];
     BYTE szDisMsg[100];


	vdDebug_LogPrintf("saturn inProcessRegistration");
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
        srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;
    inHDTSave(strHDT.inHostIndex);

    srTransPara = &srTransRec;

	if ((strTCT.byTerminalType % 2) == 0)
	    inLine=V3_ERROR_LINE_ROW;

#if 1	    
	//memset(szErrMsg,0x00,sizeof(szErrMsg));
	//sprintf(szErrMsg,"%s",strHDT.szHostLabel);
   	//setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szErrMsg);  		
	inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
	vdDebug_LogPrintf("inNumOfMit=[%d]-----",inNumOfMit);
          CTOS_Delay(250);
          vduiClearBelow(2);
#endif
       
    vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	srTransPara->byOffline = CN_FALSE;

    if(VS_TRUE == strTCT.fDemo)
    {
        vdDebug_LogPrintf("DEMO Call Connect!!");
    }
    else if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
    {
        if(srTransPara->shTransResult == 0)
            srTransPara->shTransResult = TRANS_COMM_ERROR;
        inCTOS_inDisconnect();
        vdSetErrorMessage("CONNECT FAILED");
        return ST_ERROR;
    }	

    for(inNum =0 ;inNum < inNumOfMit; inNum++)
    {
        memcpy(&strMMT[0],&strMMT[inNum],sizeof(STRUCT_MMT));
        srTransRec.MITid = strMMT[0].MITid;
        strcpy(srTransRec.szTID, strMMT[0].szTID);
        strcpy(srTransRec.szMID, strMMT[0].szMID);
        memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        
        vdCTOS_SetTransType(TERM_REG);
        
        vdIncSTAN(srTransPara);

        memset(szDisplayMsg,0x00,sizeof(szDisplayMsg));
        sprintf(szDisplayMsg,"%s",strMMT[0].szMerchantName);
        //CTOS_LCDTPrintXY(1, inLine, "                   ");
        //clearLine(inLine);
        //CTOS_LCDTPrintXY(1, inLine, szDisplayMsg);		
		CTOS_Delay(250);
		//vduiClearBelow(2);

        if ((inResult = inBuildOnlineMsg(srTransPara)) != ST_SUCCESS)
        {
            inTCTSave(1);
            if (ST_RESP_MATCH_ERR == inResult)
                return inResult;
            return inResult;
        }

        if(srTransPara->shTransResult != TRANS_AUTHORIZED)
            return ST_ERROR; 		
    }
	
    return ST_SUCCESS;
}

void vdDecSTAN(TRANS_DATA_TABLE *srTransPara)
{
    char szStr[46 + 1];
	
    memset(szStr, 0, sizeof(szStr));
    srTransPara->ulTraceNum--;
    sprintf(szStr, "%06ld", srTransPara->ulTraceNum);
    inAscii2Bcd(szStr, strHDT.szTraceNo, 3);
    inHDTSave(srTransPara->HDTid);
}	

void vdIncSTAN(TRANS_DATA_TABLE *srTransPara)
{
    char szStr[46 + 1];
	
    memset(szStr, 0, sizeof(szStr));
    srTransPara->ulTraceNum++;
    sprintf(szStr, "%06ld", srTransPara->ulTraceNum);
    inAscii2Bcd(szStr, strHDT.szTraceNo, 3);
    inHDTSave(srTransPara->HDTid);
}	

/************************************************************************
Function Name: inSnedReversalToHost()
Description:
    Send Reversal Data To Host
Parameters:
    [IN] srTransPara
         inTransCode

Return: ST_SUCCESS  
        ST_ERROR
        TRANS_COMM_ERROR
        ST_SEND_DATA_ERR
        ST_UNPACK_DATA_ERR
************************************************************************/
int inSnedReversalToHost(TRANS_DATA_TABLE *srTransPara, int inTransCode)
{
    int inResult;
    int inSendLen, inReceLen;
    BYTE uszSendData[ISO_SEND_SIZE + 1], uszReceData[ISO_REC_SIZE + 1];
    CHAR szFileName[d_BUFF_SIZE];

//    CTOS_PrinterPutString("inSnedReversalToHost.1");
    
    memset(szFileName,0,sizeof(szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
                        , strHDT.szHostLabel
                        , strHDT.inHostIndex
                        , srTransRec.MITid);
    
	DebugAddSTR("inSnedReversalToHost",szFileName,12);  
    
    vdDebug_LogPrintf("Rever Name %s",szFileName);
    
    if((inResult = inMyFile_CheckFileExist(szFileName)) < 0)
    {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        return ST_SUCCESS;
    }



    inSendLen = inResult;
    
    vdDebug_LogPrintf("inMyFile_ReversalRead(%d)",inResult);
    if((inResult = inMyFile_ReversalRead(&uszSendData[0],sizeof(uszSendData))) == ST_SUCCESS)
    {        
        DebugAddHEX("Reversal orig", uszSendData, inSendLen);
        inCTOSS_ISOEngCheckEncrypt(srTransPara->HDTid, uszSendData, &inSendLen);
        DebugAddHEX("Reversal Encrypt", uszSendData, inSendLen);

		/*
//gcitra
//#00102
        //no reversal iso log printed on receipt
        if((strTCT.fPrintISOMessage == VS_TRUE) && (strTCT.fDemo == VS_FALSE))
        {
            //inPrintISOPacket(VS_TRUE , uszSendData, inSendLen);
            vdMyEZLib_HexDump("Send ISO Packet", uszSendData, inSendLen);
            //vdPrintParseISO(uszSendData+5);
            vdPrintISOOption("TX",uszSendData, inSendLen);
        }
        //gcitra
        */
        
        if ((inReceLen = inSendAndReceiveFormComm(srTransPara,
                             (unsigned char *)uszSendData,
                             inSendLen,
                             (unsigned char *)uszReceData)) <= ST_SUCCESS)
        {
            vdDebug_LogPrintf("inSnedReversalToHost Send Err");
            srTransRec.shTransResult = TRANS_COMM_ERROR;
            return ST_SEND_DATA_ERR;
        }


		/*
//gcitra
        //no reversal iso log printed on receipt
        if ((strTCT.fPrintISOMessage == VS_TRUE) && (strTCT.fDemo == VS_FALSE))
        {
            //inPrintISOPacket(VS_TRUE , uszReceData, inSendLen);
            vdMyEZLib_HexDump("Receive ISO Packet", uszReceData, inReceLen);
            //vdPrintParseISO(uszReceData+5);
            vdPrintISOOption("RX",uszReceData, inReceLen);
        }
        //gcitra
        */
		
        vdSetISOEngTransDataAddress(srTransPara);
        inResult = inCTOSS_UnPackIsodata(srTransPara->HDTid,
                                     (unsigned char *)uszSendData,
                                     inSendLen,
                                     (unsigned char *)uszReceData,
                                     inReceLen);
        if (inResult != ST_SUCCESS)
        {
            vdDebug_LogPrintf("inSnedReversalToHost inCTOSS_UnPackIsodata Err");
            return ST_UNPACK_DATA_ERR;
        }else
        {   
            if (memcmp(srTransPara->szRespCode, "00", 2))
    		{
    		    vdDebug_LogPrintf(". inSnedReversalToHost Resp Err %02x%02x",srTransPara->szRespCode[0],srTransPara->szRespCode[1]);
                inCTOS_inDisconnect();

                return ST_ERROR;
    		}
    		else
    		{
                inResult = CTOS_FileDelete(szFileName);
                if (inResult != d_OK)
    			{
    			    vdDebug_LogPrintf(". inSnedReversalToHost %04x",inResult);
                    inCTOS_inDisconnect();
                    return ST_ERROR;
    			} 
                else
                    vdDebug_LogPrintf("rev. file deelted succesfully after send rev to host");
                
    		}
        }
    }
    
    vdDebug_LogPrintf("**inSnedReversalToHost END**");
    
    return ST_SUCCESS;
}

/************************************************************************
Function Name: inSaveReversalFile()
Description:
    Save Reversal Data into file
Parameters:
    [IN] srTransPara
         inTransCode

Return: ST_SUCCESS  
        ST_ERROR
        ST_BUILD_DATD_ERR
************************************************************************/
int inSaveReversalFile(TRANS_DATA_TABLE *srTransPara, int inTransCode)
{
    int inResult = ST_SUCCESS;
    int inSendLen, inReceLen;
    unsigned char uszSendData[ISO_SEND_SIZE + 1], uszReceData[ISO_REC_SIZE + 1];

#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "inSaveReversalFile:[%d]", inTransCode);
    	CTOS_PrinterPutString(szTemp);
}
#endif
    
    vdDebug_LogPrintf("**inSaveReversalFile START TxnType[%d]Orig[%d]**", srTransPara->byTransType, srTransPara->byOrgTransType);

    if(REFUND == srTransPara->byTransType)
        srTransPara->byPackType = REFUND_REVERSAL;
    else if(VOID == srTransPara->byTransType && REFUND == srTransPara->byOrgTransType)
        srTransPara->byPackType = VOIDREFUND_REVERSAL;
    else if(VOID == srTransPara->byTransType)
        srTransPara->byPackType = VOID_REVERSAL;        
    else if(PRE_AUTH == srTransPara->byTransType)
        srTransPara->byPackType = PREAUTH_REVERSAL;
    else if(CASH_ADV == srTransPara->byTransType)
        srTransPara->byPackType = CASHADV_REVERSAL;	
    else if(srTransPara->byTransType == LOY_REDEEM_5050 || srTransPara->byTransType == LOY_REDEEM_VARIABLE)
        srTransPara->byPackType = LOY_REDEEM_REVERSAL;		
    else if(VOID == srTransPara->byTransType && LOY_REDEEM_5050 == srTransPara->byOrgTransType || LOY_REDEEM_VARIABLE == srTransPara->byOrgTransType)
		srTransPara->byPackType = VOIDREDEEM_REVERSAL;
    else if(srTransPara->byTransType == PREAUTH_VER)
        srTransPara->byPackType = PREAUTH_VER_REVERSAL;
    else if(srTransPara->byTransType == PREAUTH_VER)
        srTransPara->byPackType = PREAUTH_VER_REVERSAL;
    else if(srTransPara->byTransType == PREAUTH_VOID)
        srTransPara->byPackType = PREAUTH_VOID_REVERSAL;
    else
        srTransPara->byPackType = REVERSAL;

    vdDebug_LogPrintf(" byPackType(%d)",srTransPara->byPackType);   

    vdSetISOEngTransDataAddress(srTransPara);
	if ((inSendLen = inCTOSS_PackIsoDataNoEncryp(srTransPara->HDTid, strHDT.szTPDU, uszSendData, srTransPara->byPackType)) <= ST_SUCCESS)
    {  
		inCTOS_inDisconnect();
        vdDebug_LogPrintf(" inSave_inSendLen %d",inSendLen);
        vdDebug_LogPrintf("**inSaveReversalFile ST_BUILD_DATD_ERR**");
        srTransPara->byPackType = inTransCode;
        inResult = ST_BUILD_DATD_ERR;
    }
    
    vdDebug_LogPrintf(". inSaveReversalFile Send(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)",uszSendData[0],uszSendData[1],uszSendData[2],uszSendData[3],uszSendData[4],uszSendData[5],uszSendData[6],uszSendData[7],uszSendData[8],uszSendData[9]);

    vdDebug_LogPrintf(". inSaveReversalFile Send Len(%d)",inSendLen);

    if((inResult = inMyFile_ReversalSave(&uszSendData[0], inSendLen)) != ST_SUCCESS)
    {
        vdDebug_LogPrintf(". inSave_inMyFile_ReversalSave(%04x)",inResult);
        inCTOS_inDisconnect();
        inResult = ST_ERROR;
    }
    
    srTransPara->byPackType = inTransCode;
    
    return inResult;
}


/************************************************************************
Function Name: inProcessReversal()
Description:
    Processing Reversal the transaction flow
Parameters:
    [IN] srTransPara

Return: ST_SUCCESS  
        ST_ERROR
************************************************************************/

int inProcessReversal(TRANS_DATA_TABLE *srTransPara)
{    
    int inResult,inTransCode;
    
    /*for TLE field 57*/
    byField_02_ON = 0;
    byField_14_ON = 0;
    byField_35_ON = 0;
    byField_45_ON = 0;
    byField_48_ON = 0;
    
    vdDebug_LogPrintf("**inProcessReversal START**");

//    CTOS_PrinterPutString("inProcessReversal");
    
    inResult = ST_SUCCESS;    
    inTransCode = srTransPara->byTransType;    
    srTransPara->byPackType = REVERSAL;
    vdDebug_LogPrintf(". transtype %d",srTransPara->byTransType);
    vdDebug_LogPrintf(". pack type %d",srTransPara->byPackType);

// for testing
#if 0
	//inResult = inCTOS_BatchSearch();
	//if(d_OK != inResult)
	//	return inResult;
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);
	
#endif    


    /* Check reversal flag, If flag is true then open reversal file and send data to host */    
    if ((inResult = inSnedReversalToHost(srTransPara,inTransCode)) != ST_SUCCESS)
    {
        vdDebug_LogPrintf(". Process_inSnedReversalToHost %d",inResult);
		if(inExceedMaxTimes_ReversalDelete() != ST_SUCCESS)
		{
			vdDisplayErrorMsg(1, 8, "Delete REV Fail...");
		}
    }

//    CTOS_PrinterPutString("inProcessReversal.1");
    
    if(inResult == ST_SUCCESS)
    {
//    CTOS_PrinterPutString("inProcessReversal.2");
    
        inMyFile_ReversalDelete();
        /*albert - start - fix stan issue*/        		
		vdIncSTAN(srTransPara);
        /*albert - end - fix stan issue*/
 		
        /* Check reversal flag, If flag is true then create reversal file */
        /*if ((strHDT.fReversalEnable == CN_TRUE) && (srTransRec.byTransType != SETTLE) && (srTransRec.byTransType != SIGN_ON)
			&& (srTransRec.byTransType != PRE_AUTH) && (srTransRec.byTransType != BAL_INQ) && (srTransRec.byTransType != LOY_BAL_INQ)
			&& (srTransRec.byTransType != CARD_VERIFY) && (srTransRec.byTransType != TERM_REG))*/

//    CTOS_PrinterPutString("inProcessReversal.3");
			
			// REMOVED PRE_AUTH TO fix issue Pre-auth should have reversal
	if ((strHDT.fReversalEnable == CN_TRUE) && (srTransRec.byTransType != SETTLE) && (srTransRec.byTransType != SIGN_ON)
			&& (srTransRec.byTransType != BAL_INQ) && (srTransRec.byTransType != LOY_BAL_INQ)
			&& (srTransRec.byTransType != CARD_VERIFY) && (srTransRec.byTransType != TERM_REG))			
        {
//    CTOS_PrinterPutString("inProcessReversal.4");
        
            vdDebug_LogPrintf("inSaveReversalFile START");
            if ((inResult = inSaveReversalFile(srTransPara,inTransCode)) != ST_SUCCESS)
            {
//    CTOS_PrinterPutString("inProcessReversal.5");
            
                vdDebug_LogPrintf(". Process_inSaveReversalFile %04x",inResult);
            }
        }
    }

#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "inTransCode.1:[%d]",  inTransCode);
        CTOS_PrinterPutString(szTemp);
	
}
#endif	
    
    srTransPara->byTransType = inTransCode; 
    
    vdDebug_LogPrintf("**inProcessReversal TYPE(%d) Rest(%d)END**",srTransPara->byTransType,
                                                                     inResult);

    strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
	
//    CTOS_PrinterPutString("inProcessReversal.end");
    
    return inResult;
}

int inProcessAdviceTrans(TRANS_DATA_TABLE *srTransPara, int inAdvCnt)
{
    int inResult,inUpDateAdviceIndex;
    int inCnt;
    TRANS_DATA_TABLE srAdvTransTable;
    ISO_FUNC_TABLE srAdviceFunc;
    STRUCT_ADVICE strAdvice;

    //add to do not do any advice for Y1, advice will be sent on the next online transaction
    if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_APPROVAL) {
        return d_OK;
    }

    memset((char *)&srAdvTransTable, 0, sizeof(TRANS_DATA_TABLE));
    memset((char *)&strAdvice, 0, sizeof(strAdvice));
    
    memcpy((char *)&srAdvTransTable, (char *)srTransPara, sizeof(TRANS_DATA_TABLE));
    memcpy((char *)&srAdviceFunc, (char *)&srIsoFuncTable[0], sizeof(ISO_FUNC_TABLE));
    
    inResult = ST_SUCCESS;

    vdDebug_LogPrintf("inProcessAdviceTrans byPackType(%d)byTransType(%d)", srAdvTransTable.byPackType, strAdvice.byTransType);
    while(1)
    {
        inResult = inMyFile_AdviceRead(&inUpDateAdviceIndex,&strAdvice,&srAdvTransTable);
        
        vdDebug_LogPrintf("ADVICE inUpDateAdviceIndex[%d] inMyFile_AdviceRead Rult(%d)(%d)(%d)(%d)", inUpDateAdviceIndex, inResult, srAdvTransTable.ulTraceNum, srAdvTransTable.byPackType, strAdvice.byTransType);
        
        if(inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA)
        {
            inResult = ST_SUCCESS;
            break;
        }

        if(inResult == ST_SUCCESS)
        {
            vdDebug_LogPrintf("srTransPara->HDTid = [%d] CVV2[%s]Tip[%02X%02X%02X%02X%02X%02X]", srTransPara->HDTid,srAdvTransTable.szCVV2, srAdvTransTable.szTipAmount[0], srAdvTransTable.szTipAmount[1], srAdvTransTable.szTipAmount[2], srAdvTransTable.szTipAmount[3], srAdvTransTable.szTipAmount[4], srAdvTransTable.szTipAmount[5]);
            //advice need add traceNum
            //srAdvTransTable.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
            //inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
            //inHDTRead(srTransPara->HDTid);
            
            /*albert - start - fix stan issue*/
            //srAdvTransTable.ulTraceNum=srTransPara->ulTraceNum;
            //	// fix issue: DE 60 = 0200 STAN of Orignal and Orignal RRN #2
            srAdvTransTable.ulTraceNum=srAdvTransTable.ulOrgTraceNum=srTransPara->ulTraceNum;
            /*albert - end - fix stan issue*/

			//srAdvTransTable.byUploaded=FALSE;

            inResult = inPackSendAndUnPackData(&srAdvTransTable, strAdvice.byTransType);

            /*albert - start - fix stan issue*/
            vdIncSTAN(srTransPara);
            srAdvTransTable.ulTraceNum=srTransPara->ulTraceNum;
            /*albert - end - fix stan issue*/
			
            vdDebug_LogPrintf(". inProcessAdviceTrans Rult(%d)srAdvTransTable.byTCFailUpCnt[%d]", inResult, srAdvTransTable.byTCFailUpCnt);
            if (memcmp(srAdvTransTable.szRespCode, "00", 2))
                inResult = ST_ERROR;
                
            if ((inResult == ST_SUCCESS))
            {
                if ((srAdviceFunc.inAdviceAnalyse != 0x00))
                {
                    vdSetISOEngTransDataAddress(&srAdvTransTable);
                    inResult = srAdviceFunc.inAdviceAnalyse(CN_FALSE);
                }
                
                if (inResult == ST_SUCCESS)
                {
                    vdDebug_LogPrintf(". inAdviceAnalyse Rult(%d)", inResult);

                    srAdvTransTable.byUploaded = CN_TRUE;
                    srAdvTransTable.fTipUploaded = CN_TRUE;
                    //Should be Online void the Intial SALE amount.
                    //use szStoreID to store how much amount fill up in DE4 for VOID
                    memcpy(srAdvTransTable.szStoreID, srAdvTransTable.szTotalAmount, 6);
					
		// Transfer to function vdUpdatePreCompTotal, failed or not sending of 0220 must update precomp total -- sidumili
		/*			
                    if(srAdvTransTable.byTransType == PREAUTH_VER)
                    {
						srAdvTransTable.byTransType=PREAUTH_COMP;
                        inCTOS_UpdateAccumPreCompTotal(&srAdvTransTable);						
                    }
		*/			
		
                    if((inResult = inMyFile_BatchSave(&srAdvTransTable,DF_BATCH_UPDATE)) == ST_SUCCESS)
                    {
                        inResult = inMyFile_AdviceUpdate(inUpDateAdviceIndex);
                        vdDebug_LogPrintf(". inProcessAdviceTrans Update Rult(%d)**", inResult);

                    }

                    if (inResult != ST_SUCCESS)
                    {
                        vdDebug_LogPrintf(". inProcessAdviceTrans Err(%d)**", inResult);
                        break;
                    }
                }
            }
        }
        
        if(inResult != ST_SUCCESS)
        {
            if(srTransPara->byTransType == SETTLE)
            {
                srTransRec.shTransResult = TRANS_COMM_ERROR;
                inCTOS_inDisconnect();
                return ST_ERROR;
            }
            else
                return ST_SUCCESS;
        }

        
        if(inAdvCnt != -1)
        {
            inAdvCnt --;
            if(inAdvCnt == 0)
                break;
        }
    }
    
    vdDebug_LogPrintf("**inProcessAdviceTrans(%d) END**", inResult);
	return (inResult);
}

int inProcessAdviceTCTrail(TRANS_DATA_TABLE *srTransPara)
{
	int inRet=d_OK, inUpDateAdviceIndex;
	TRANS_DATA_TABLE srAdvTransTable;
	STRUCT_ADVICE strAdvice;


	//Not uploading offline advise - #00188
	inHDTRead(srTransPara->HDTid);

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

    inUpDateAdviceIndex=0;
    memset((char *)&srAdvTransTable, 0, sizeof(TRANS_DATA_TABLE));
    memset((char *)&strAdvice, 0, sizeof(strAdvice));	



    if(srTransPara->byOffline == CN_FALSE)
    {
        if(inMyFile_AdviceRead(&inUpDateAdviceIndex,&strAdvice,&srAdvTransTable) == d_OK)
            vdIncSTAN(srTransPara);
	    
        inRet=inProcessAdviceTrans(srTransPara, strHDT.inNumAdv);
		vdDebug_LogPrintf("inProcessAdviceTrans[%d]",inRet);
        if(d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
    }	

    inUpDateAdviceIndex=0;
    memset((char *)&srAdvTransTable, 0, sizeof(TRANS_DATA_TABLE));
    memset((char *)&strAdvice, 0, sizeof(strAdvice));	

	return inRet;
}

int inProcessEMVTCUpload(TRANS_DATA_TABLE *srTransPara, int inAdvCnt)
{
    int inResult,inUpDateAdviceIndex;
    int inCnt;
    TRANS_DATA_TABLE srOrigTransFromBatch;
    TRANS_DATA_TABLE srAdvTransTable;
    ISO_FUNC_TABLE srAdviceFunc;
    STRUCT_ADVICE strAdvice;

    memset((char *)&srAdvTransTable, 0, sizeof(TRANS_DATA_TABLE));
    memset((char *)&strAdvice, 0, sizeof(strAdvice));
    
    memcpy((char *)&srAdvTransTable, (char *)srTransPara, sizeof(TRANS_DATA_TABLE));
    memcpy((char *)&srAdviceFunc, (char *)&srIsoFuncTable[0], sizeof(ISO_FUNC_TABLE));
    
    inResult = ST_SUCCESS;
        
//    vdDebug_LogPrintf("inProcessEMVTCUpload byPackType(%d)byTransType(%d)", srAdvTransTable.byPackType, strAdvice.byTransType);
    while(1)
    {
        inResult = inMyFile_TCUploadFileRead(&inUpDateAdviceIndex,&strAdvice,&srAdvTransTable);
        
        if(strAdvice.byTransType == TC_UPLOAD)
            srAdvTransTable.byPackType = TC_UPLOAD;
        
        if(inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA)
        {
            inResult = ST_SUCCESS;
            break;
        }

        memcpy(&srOrigTransFromBatch, &srAdvTransTable, sizeof(TRANS_DATA_TABLE));
        if(inResult == ST_SUCCESS)
        {
            vdDebug_LogPrintf("srTransPara->HDTid = [%d] ", srTransPara->HDTid);
            inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
            inHDTRead(srTransPara->HDTid);
            
            inResult = inPackSendAndUnPackData(&srAdvTransTable, strAdvice.byTransType);
            
//            vdDebug_LogPrintf(". inProcessEMVTCUpload Rult(%d)srAdvTransTable.byTCFailUpCnt[%d]srTransPara->szRespCode[%s]", inResult, srAdvTransTable.byTCFailUpCnt,srAdvTransTable.szRespCode);

  
            if(srAdvTransTable.byTCFailUpCnt >= 2)
            {
              srAdvTransTable.byTCuploaded = CN_TRUE;
              srAdvTransTable.byUploaded = CN_TRUE;
              inResult = inMyFile_TCUploadFileUpdate(inUpDateAdviceIndex);
//              vdDebug_LogPrintf(". inProcessEMVTCUpload Update Rult(%d)**", inResult);
                      
              if (inResult != ST_SUCCESS)
              {
//                  vdDebug_LogPrintf(". inProcessEMVTCUpload Err(%d)**", inResult);
                  break;
              }
            }
            if (inResult == ST_SUCCESS)
            {
                if(memcmp(srAdvTransTable.szRespCode,"00",2) != 0)
                {
                    vdDebug_LogPrintf(". resp not succ(%s)**srTransPara->byPackType[%d]strAdvice.byTransType[%d]", srAdvTransTable.szRespCode,srTransPara->byPackType,strAdvice.byTransType);
                    if( strAdvice.byTransType == TC_UPLOAD )
                    {
                        srOrigTransFromBatch.byTCFailUpCnt++;                    
                        inMyFile_BatchSave(&srOrigTransFromBatch,DF_BATCH_UPDATE);
                        inCTOS_inDisconnect();
                        return ST_ERROR;
                    }
                }
                else
                {
                    srAdvTransTable.byTCuploaded = CN_TRUE;
                    srAdvTransTable.byUploaded = CN_TRUE;
                    inResult = inMyFile_TCUploadFileUpdate(inUpDateAdviceIndex);
//                    vdDebug_LogPrintf(". inProcessEMVTCUpload Update Rult(%d)**", inResult);
                          
                    if (inResult != ST_SUCCESS)
                    {
//                      vdDebug_LogPrintf(". inProcessEMVTCUpload Err(%d)**", inResult);
                      break;
                    }
                }    
            }
            else
            {
                if(strAdvice.byTransType == TC_UPLOAD)
                {
                    srOrigTransFromBatch.byTCFailUpCnt++; 
                    inMyFile_BatchSave(&srOrigTransFromBatch,DF_BATCH_UPDATE);
                }
            }
            
        }
        
        if(inResult != ST_SUCCESS)
        {
            if(srTransPara->byTransType == SETTLE)
            {
                srTransRec.shTransResult = TRANS_COMM_ERROR;
                inCTOS_inDisconnect();
                return ST_ERROR;
            }
            else
                return ST_SUCCESS;
        }

        
        if(inAdvCnt != -1)
        {
            inAdvCnt --;
            if(inAdvCnt == 0)
                break;
        }
    }
    
//    vdDebug_LogPrintf("**inProcessEMVTCUpload(%d) END**", inResult);
	return (inResult);
}


/************************************************************************
Function Name: inPackSendAndUnPackData()
Description:
    Composed upload host information.
    Analysis of host return message.
Parameters:
    [IN] srTransPara
         inTransCode
Return: ST_SUCCESS  
        ST_ERROR
        ST_BUILD_DATD_ERR
        ST_SEND_DATA_ERR
        ST_UNPACK_DATA_ERR
************************************************************************/

int inPackSendAndUnPackData(TRANS_DATA_TABLE *srTransPara, int inTransCode)
{
    int inResult;
    int inSendLen, inReceLen;
    unsigned char uszSendData[ISO_SEND_SIZE + 1], uszReceData[ISO_REC_SIZE + 1];
    
    vdDebug_LogPrintf("**inPackSendAndUnPackData START**");
	inCTOSS_GetRAMMemorySize("PACK&UNPACK START");
	
//	CTOS_PrinterPutString("inPackSendAndUnPackData");
   // 	CTOS_PrinterPutString(srTransRec.szTerms);
    
    memset(uszSendData, 0x00, sizeof(uszSendData));
    memset(uszReceData, 0x00, sizeof(uszReceData));

    inResult = ST_SUCCESS;

    vdSetISOEngTransDataAddress(srTransPara);


//	CTOS_PrinterPutString("inPackSendAndUnPackData-1");
#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "inTransCode:[%d]:[%d]:[%d]", srTransPara->HDTid, inTransCode, strHDT.szTPDU);
    	CTOS_PrinterPutString(szTemp);
	
}
#endif    

if((srTransRec.byTransType!=DCC_RATE_REQUEST) && (srTransRec.byTransType!=DCC_MERCH_RATE_REQ))
{

     if ((inSendLen = inCTOSS_PackIsoData(srTransPara->HDTid, strHDT.szTPDU, uszSendData, inTransCode)) <= ST_SUCCESS)
     {
          inCTOS_inDisconnect();    
          vdDebug_LogPrintf(" inSendLen %d",inSendLen);
          vdDebug_LogPrintf("**inPackSendAndUnPackData ST_BUILD_DATD_ERR**");
          return ST_BUILD_DATD_ERR;
     }
}
else
{
     inFXTRead(1);

     if ((inSendLen = inCTOSS_PackIsoDataEx(srTransPara->HDTid, strFXT.szFXTTPDU, uszSendData, inTransCode)) <= ST_SUCCESS)
     {
          inCTOS_inDisconnect(); 	 
          vdDebug_LogPrintf("DCC_RATE_REQUEST inSendLen %d",inSendLen);
          vdDebug_LogPrintf("**inPackSendAndUnPackData DCC_RATE_REQUEST ST_BUILD_DATD_ERR**");
          return ST_BUILD_DATD_ERR;
     }

}

    if(srTransPara->byPackType == VOID && srTransPara->byOrgTransType == SALE_OFFLINE && srTransPara->byUploaded == TRUE)
        uszSendData[6]=0x20;
        
//	CTOS_PrinterPutString("inPackSendAndUnPackData-2");

	inResult = inCTOSS_CheckBitmapSetBit(5);
    vdDebug_LogPrintf("inCTOSS_CheckSetBit 5 [%ld]", inResult);
	inResult = inCTOSS_CheckBitmapSetBit(11);
    vdDebug_LogPrintf("inCTOSS_CheckSetBit 11 [%ld]", inResult);
	
    vdDebug_LogPrintf(". inPackData Send Len(%d)",inSendLen);

//	CTOS_PrinterPutString("inPackSendAndUnPackData-2.1");

	/*
    //gcitra
    if ((strTCT.fPrintISOMessage == VS_TRUE) && (strTCT.fDemo == VS_FALSE))
    {
        //inPrintISOPacket(VS_TRUE , uszSendData, inSendLen);
        vdMyEZLib_HexDump("Send ISO Packet", uszSendData, inSendLen);
        //vdPrintParseISO(uszSendData+5);
        vdPrintISOOption("TX",uszSendData, inSendLen);
    }
	//gcitra
	*/

//	CTOS_PrinterPutString("inPackSendAndUnPackData-3");
    
    if ((inReceLen = inSendAndReceiveFormComm(srTransPara,
                         (unsigned char *)uszSendData,
                         inSendLen,
                         (unsigned char *)uszReceData)) <= 0)
    {
        vdDebug_LogPrintf(". inPackData ST_SEND_DATA_ERR");
        return ST_SEND_DATA_ERR;
    }

    vdDebug_LogPrintf("**@@@@@@@@@@@@@@@@@**");    
    vdDebug_LogPrintf("uszReceData:inPackData Rec Len(%s):(%d)",uszReceData, inReceLen);

	/*
    //gcitra
    if ((strTCT.fPrintISOMessage == VS_TRUE) && (strTCT.fDemo == VS_FALSE))
    {
        //inPrintISOPacket(VS_FALSE , uszReceData, inReceLen);
        vdMyEZLib_HexDump("Receive ISO Packet", uszReceData, inReceLen);
        //vdPrintParseISO(uszReceData+5);
        vdPrintISOOption("RX",uszReceData, inReceLen);
    }
    //gcitra
	*/

    vdSetISOEngTransDataAddress(srTransPara);
    inResult = inCTOSS_UnPackIsodata(srTransPara->HDTid,
                                 (unsigned char *)uszSendData,
                                 inSendLen,
                                 (unsigned char *)uszReceData,
                                 inReceLen);
    vdDebug_LogPrintf("**inPackSendAndUnPackData inResult=[%d]srTransPara->szRespCode[%s]",inResult,srTransPara->szRespCode);

    if (inResult != ST_SUCCESS)
    {
        vdSetErrorMessage("INVALID RESPONSE");
        inResult = ST_UNPACK_DATA_ERR;
		inCTOS_inDisconnect();
    }

#if 0
    if(srTransPara->byPackType == LOG_ON)
    {
        if(inCTOSS_CheckBitmapSetBit(53) == 0)
        {
            vdSetErrorMessage("LOGON FAILED");
            inResult = ST_UNPACK_DATA_ERR;
            inCTOS_inDisconnect();
        }
    }
#endif	

	inCTOSS_GetRAMMemorySize("PACK&UNPACK END");
    vdDebug_LogPrintf("**saturn inPackSendAndUnPackData END**");
//	CTOS_PrinterPutString("inPackSendAndUnPackData.end");
   // 	CTOS_PrinterPutString(srTransRec.szTerms);
	
    return inResult;
}





/************************************************************************
Function Name: inBuildOnlineMsg()
Description:
   To handle online messages and check the response code and authorization code
Parameters:
    [IN] srTransPara

Return: ST_SUCCESS  
        ST_ERROR
        ST_UNPACK_DATA_ERR
        ST_SEND_DATA_ERR
************************************************************************/
int inBuildOnlineMsg(TRANS_DATA_TABLE *srTransPara)
{
    	int inResult;
	TRANS_DATA_TABLE srTransParaTmp;
	char szTemp[30+1];
	
    
    vdDebug_LogPrintf("**inBuildOnlineMsg START TxnType[%d]Orig[%d]**", srTransPara->byTransType, srTransPara->byOrgTransType);


//	CTOS_PrinterPutString("inBuildOnlineMsg");
   // 	CTOS_PrinterPutString(srTransRec.szTerms);


    if(VOID == srTransPara->byTransType && REFUND == srTransPara->byOrgTransType)
        srTransPara->byPackType = VOID_REFUND;
    else if(VOID == srTransPara->byTransType && LOY_REDEEM_VARIABLE == srTransPara->byOrgTransType || LOY_REDEEM_5050 == srTransPara->byOrgTransType)
        srTransPara->byPackType = VOID_REDEEM;	
    else if(VOID == srTransPara->byTransType && PRE_COMP == srTransPara->byOrgTransType) // CHANGE request from Jaja dated 07092015 "Processing code for Void Sales Completion should be:  19 a0 0x"
        srTransPara->byPackType = VOID_PRECOMP;			
        else
        srTransPara->byPackType = srTransPara->byTransType;



	memset(&srTransParaTmp, 0x00, sizeof(TRANS_DATA_TABLE));
	memcpy(&srTransParaTmp, srTransPara, sizeof(TRANS_DATA_TABLE));
    vdDebug_LogPrintf(" byPackType(%d)",srTransPara->byPackType);    

    inResult = inPackSendAndUnPackData(srTransPara, srTransPara->byPackType);


//	CTOS_PrinterPutString("inBuildOnlineMsg-1");
   // 	CTOS_PrinterPutString(srTransRec.szTerms);
   
    if (inResult == ST_BUILD_DATD_ERR)
    {  
//	CTOS_PrinterPutString("inBuildOnlineMsg-2");
    
        vdDebug_LogPrintf("**inBuildOnlineMsg TRANS_COMM_ERROR**");
        srTransPara->shTransResult = TRANS_COMM_ERROR;
        vdSetErrorMessage("BUILD DATA ERR");
        inCTOS_inDisconnect();
        return ST_ERROR;
    }
    else if (inResult == ST_SEND_DATA_ERR || inResult == ST_UNPACK_DATA_ERR)
    {

	//DE 12 and DE 13 has zero value (0) on reversal, advice and void transactions
	// #00164
    	//memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    	//memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);

//	memcpy(srTransPara->szOrgDate, srTransRec.szDate, 2);
//	memcpy(srTransPara->szOrgTime, srTransRec.szTime, 2);
	
//	CTOS_PrinterPutString("inBuildOnlineMsg-3");
         if((srTransPara->byTransType==DCC_RATE_REQUEST) ||(srTransPara->byTransType==DCC_MERCH_RATE_REQ))
         	{
         	      srTransPara->fDCCOptin = FALSE; //dcc transaction by default field 63 dcc value is 2 (opted local currency)
         	      vdDisplayErrorMsg2(1, 8,"NO RESPONSE FROM HOST", " ", MSG_TYPE_ERROR);
         	}
                
    
        srTransRec.shTransResult = TRANS_COMM_ERROR;
        vdDebug_LogPrintf("**inBuildOnlineMsg ST_SEND_DATA_ERR**");
        return inResult;
    }else
    {

//	CTOS_PrinterPutString("inBuildOnlineMsg-4");

        if((srTransPara->byTransType==DCC_RATE_REQUEST) ||(srTransPara->byTransType==DCC_MERCH_RATE_REQ))
       {
            srTransPara->shTransResult = inCheckHostFEXCORespCode(srTransPara);	

	    if (srTransPara->shTransResult != ST_SUCCESS)
	    	{
	    	     vdDebug_LogPrintf("AAA -  srTransPara->shTransResult[%d]", srTransPara->shTransResult);
		      srTransPara->fDCCOptin = FALSE;
	    	      return ST_ERROR;     
	    	}
                
       }
       else
          srTransPara->shTransResult = inCheckHostRespCode(srTransPara);
	   
        vdDebug_LogPrintf(". shTransResult %d",srTransPara->shTransResult);

        if (srTransPara->shTransResult == TRANS_AUTHORIZED)
        {
            if (ST_SUCCESS != inBaseRespValidation(&srTransParaTmp,srTransPara))
				return ST_RESP_MATCH_ERR;
        }
        else if (srTransPara->shTransResult == ST_UNPACK_DATA_ERR)
        {
            vdDebug_LogPrintf("**inBuildOnlineMsg shTransResult UNPACK_ERR**");
            srTransPara->shTransResult = TRANS_COMM_ERROR;// for not delete reversal file
            
            vdSetErrorMessage("RESP ERROR");
            return ST_UNPACK_DATA_ERR;
        }
    }

#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "inBuildOnlineMsg:[%d]",  srTransPara->byPackType);
        CTOS_PrinterPutString(szTemp);
	
}
	//if(srTransRec.byOrgTransType == SALE_OFFLINE && srTransRec.fCredit == FALSE && srTransRec.byTransType == VOID)
	//	srTransRec.fLoyalty = FALSE;

#endif	
   
    vdDebug_LogPrintf("**inBuildOnlineMsg END**");
    return ST_SUCCESS;
}

/************************************************************************
Function Name: inSetBitMapCode()
Description:
    Use the transaction code to generate the corresponding bitmap code
Parameters:
    [IN] srTransPara
         srPackFunc
         inTempBitMapCode
Return: ST_SUCCESS  
        inBitMapArrayIndex
************************************************************************/

int inSetBitMapCode(TRANS_DATA_TABLE *srTransPara, int inTransCode)
{
    int    inBitMapIndex = -1;

    inBitMapIndex = inTransCode;
    
    return inBitMapIndex;
}

/************************************************************************
Function Name: inPackMessageIdData()
Description:
    Pack message id data
Parameters:
    [IN] srTransPara
         inTransCode
         uszPackData
Return: inPackLen
************************************************************************/
int inPackMessageIdData(int inTransCode, unsigned char *uszPackData, char *szMTI)
{
    int    inPackLen;
    TRANS_DATA_TABLE *srTransPara;
    
    inPackLen = 0;
    srTransPara = srGetISOEngTransDataAddress();

    wub_str_2_hex(szMTI, (char *)&uszPackData[inPackLen], MTI_ASC_SIZE);

    if(srTransPara->byPackType != BATCH_UPLOAD && srTransPara->byPackType != TC_UPLOAD)
    {   
       wub_str_2_hex(szMTI, (char *)srTransPara->szMassageType, MTI_ASC_SIZE);
    }

    inPackLen += MTI_BCD_SIZE;
    
    return inPackLen;
}

/************************************************************************
Function Name: inPackPCodeData()
Description:
    Pack message id data
Parameters:
    [IN] srTransPara
         inTransCode
         uszPackData
Return: inPackLen
************************************************************************/
int inPackPCodeData(int inTransCode, unsigned char *uszPackData, char *szPCode)
{
    char szTempFile03[10];
    int    inPackLen;
    TRANS_DATA_TABLE *srTransPara;
	
    inPackLen = 0;
    srTransPara = srGetISOEngTransDataAddress();



	vdDebug_LogPrintf(". inPackPCodeData [%02X] %s", inTransCode, szPCode);

	if(srTransPara->byPackType == BATCH_UPLOAD)
	{
		memset(szTempFile03,0x00,sizeof(szTempFile03));
		memcpy(szTempFile03,srTransPara->szIsoField03,PRO_CODE_BCD_SIZE);
	}

	wub_str_2_hex(szPCode, srTransPara->szIsoField03, PRO_CODE_ASC_SIZE);

   //vdMyEZLib_HexDump("inPackPCodeData", szPCode, 6);

    if (srTransPara->byTransType == SALE || srTransPara->byTransType == BAL_INQ)
    {
        if (srTransPara->inAccountType == SAVINGS_ACCOUNT)
            srTransPara->szIsoField03[1] += 0x10;
        else if (srTransPara->inAccountType == CURRENT_ACCOUNT)
            srTransPara->szIsoField03[1] += 0x20;
    }

	if(srTransPara->byPackType == BATCH_UPLOAD)
	{
		memcpy(srTransPara->szIsoField03,szTempFile03,PRO_CODE_BCD_SIZE);
		
		if(inFinalSend == CN_TRUE)
			srTransPara->szIsoField03[2] = 0x00;
	}

    inPackLen += PRO_CODE_BCD_SIZE;

	vdDebug_LogPrintf(". szPCode %s [%02X%02X%02X]", szPCode, srTransPara->szIsoField03[0], srTransPara->szIsoField03[1], srTransPara->szIsoField03[2]);

   //vdMyEZLib_HexDump("inPackPCodeData1", szPCode, 6);

	return inPackLen;
}

/************************************************************************
Function Name: vdModifyBitMapFunc()
Description:
    Modify bitmap array
Parameters:
    [IN] srTransPara
         inTransCode
         inBitMap
Return: 
************************************************************************/
void vdModifyBitMapFunc(int inTransCode, int *inBitMap)
{
    BYTE szTipAmount[20];
    TRANS_DATA_TABLE *srTransPara;
	int inRet;

	vdDebug_LogPrintf("--vdModifyBitMapFunc--[start]");
	
    srTransPara = srGetISOEngTransDataAddress();


#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "####:[%d]", inTransCode);
    	CTOS_PrinterPutString(szTemp);
}
#endif
    
    if(inTransCode == SIGN_ON)
    {
        return;
    }

    vdDebug_LogPrintf(". vdModifyBitMapFunc inTransCode[%d]byEntryMode[%d]byTransType[%d]szCVV2[%s]", inTransCode, srTransPara->byEntryMode, srTransPara->byTransType, srTransPara->szCVV2);

	vdDebug_LogPrintf("[1.]strTCT.fEMVOnlinePIN[%d] | srTransPara->fDebit[%d] | srTransPara->fCUPPINEntry[%d] srTransPara->fEMVPIN[%d]srTransPara->fInstallment[%d] srTransRec.fInstallment[%d], srTransPara->fEMVPINEntered[%d]", strTCT.fEMVOnlinePIN, srTransPara->fDebit, srTransPara->fCUPPINEntry, srTransPara->fEMVPIN, srTransPara->fInstallment, srTransRec.fInstallment, srTransPara->fEMVPINEntered);

    if((inTransCode != BATCH_UPLOAD) && (inTransCode != REVERSAL) && (inTransCode != SETTLE) && (inTransCode != CLS_BATCH))
    {
    	//CTOS_PrinterPutString("vdModifyBitMapFunc-1");
    
        if(srTransPara->byEntryMode == CARD_ENTRY_MSR ||
           srTransPara->byEntryMode == CARD_ENTRY_ICC)
        {
            {
                vdMyEZLib_LogPrintf(". usTrack1Len>0");
                vdCTOSS_SetBitMapOff(inBitMap, 45);
            }
		if(inTransCode == LOY_REDEEM_5050 || inTransCode == LOY_REDEEM_VARIABLE || inTransCode == LOY_BAL_INQ)
			                vdCTOSS_SetBitMapOn(inBitMap, 35);  // rtr 05252015 issue

        }
        // fix for No DE 23 and DE 55 on Chip Sale Completion request message.. ADDED PRE_AUTH and PRE_COMP on condition
        if ((srTransPara->byEntryMode == CARD_ENTRY_ICC || 
			(srTransPara->byEntryMode == CARD_ENTRY_WAVE &&
					(srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_2 ||
			(srTransPara->bWaveSID == 0x63) ||
			(srTransPara->bWaveSID == 0x65) ||
			 srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC ||
			 srTransPara->bWaveSID == d_VW_SID_AE_EMV ||
			 srTransPara->bWaveSID == d_VW_SID_CUP_EMV ||
			 srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP))) && 
            (  srTransPara->byTransType == SALE
            || srTransPara->byTransType == SALE_OFFLINE
            || srTransPara->byTransType == PRE_AUTH
            || srTransPara->byTransType == PRE_COMP
            /*|| srTransPara->byTransType == REFUND*/
            || srTransPara->byTransType == SALE_TIP
            || srTransPara->byTransType == CASH_ADV
            || srTransPara->byTransType == VOID
            || srTransPara->byTransType == LOY_REDEEM_5050 //// rtr 05252015 issue
            || srTransPara->byTransType == LOY_REDEEM_VARIABLE
            || srTransPara->byTransType == LOY_BAL_INQ
            || srTransPara->byTransType == PREAUTH_VER
            || srTransPara->byTransType == PREAUTH_COMP)) // rtr 05252015 issue
        {

            vdDebug_LogPrintf("Transcation set DE55 T5F34_len[%d]", srTransPara->stEMVinfo.T5F34_len);
            vdCTOSS_SetBitMapOn(inBitMap, 55);
            
            if(srTransPara->stEMVinfo.T5F34_len > 0)
                vdCTOSS_SetBitMapOn(inBitMap, 23); 
        }
    
        if (srTransPara->byEntryMode == CARD_ENTRY_MANUAL)
        {
            vdMyEZLib_LogPrintf(". byEntryMode CN_TRUE");
            vdCTOSS_SetBitMapOn(inBitMap, 2);
            vdCTOSS_SetBitMapOn(inBitMap, 14);

            vdCTOSS_SetBitMapOff(inBitMap, 35);
        }


        memset(szTipAmount, 0x00, sizeof(szTipAmount));
        wub_hex_2_str(srTransPara->szTipAmount, szTipAmount, 6);
        DebugAddSTR("Tip", szTipAmount, 12);
        if(atol(szTipAmount) > 0)
        {
            vdCTOSS_SetBitMapOn(inBitMap, 54);

           //Should be Online void the Intial SALE amount.
            //use szStoreID to store how much amount fill up in DE4 for VOID
            if(srTransPara->byTransType == VOID && 0 == memcmp(srTransPara->szStoreID, srTransPara->szBaseAmount, 6))
            {
                vdCTOSS_SetBitMapOff(inBitMap, 54);
            }
        }
#if 0
        if(strlen(srTransPara->szCVV2) > 0)
        {
            vdCTOSS_SetBitMapOn(inBitMap, 48);
        }
#endif

       //for compliance - add originator reason code
       //if (srTransPara->IITid == VISA_ISSUER){

		 //set to 1 if disable
         //if(inCTOSS_CheckBitmapSetBit(63) == 0)
	   	//	vdCTOSS_SetBitMapOn(inBitMap, 63);
       //}
        
    }
    else if(inTransCode == BATCH_UPLOAD || inTransCode == REVERSAL || inTransCode == VOID_REVERSAL || inTransCode == PREAUTH_REVERSAL
		|| inTransCode == CASHADV_REVERSAL)
    {

	// for testing - 05262015
        if (srTransPara->byEntryMode == CARD_ENTRY_MANUAL)
        {
            vdCTOSS_SetBitMapOn(inBitMap, 2);
            vdCTOSS_SetBitMapOn(inBitMap, 14);
        }

	if(inTransCode == BATCH_UPLOAD && srTransPara->byEntryMode == CARD_ENTRY_MANUAL)
       		vdCTOSS_SetBitMapOff(inBitMap, 35);
		

    	//CTOS_PrinterPutString("vdModifyBitMapFunc-2");
    	if(inTransCode == REVERSAL) // 
       		vdCTOSS_SetBitMapOff(inBitMap, 37);

        if(srTransPara->byEntryMode == CARD_ENTRY_ICC || 
        (srTransPara->byEntryMode == CARD_ENTRY_WAVE &&
        (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_2 ||
        srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC ||
        (srTransPara->bWaveSID == 0x63) ||
        (srTransPara->bWaveSID == 0x65) ||
        srTransPara->bWaveSID == d_VW_SID_AE_EMV ||
        srTransPara->bWaveSID == d_VW_SID_CUP_EMV ||
        srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP)))
        {
            vdDebug_LogPrintf(("BATCH_UPLOAD set DE55"));
            vdCTOSS_SetBitMapOn(inBitMap, 55);
            
            if(srTransPara->stEMVinfo.T5F34_len > 0)
                vdCTOSS_SetBitMapOn(inBitMap, 23); 
        }

        memset(szTipAmount, 0x00, sizeof(szTipAmount));
        wub_hex_2_str(srTransPara->szTipAmount, szTipAmount, 6);
        DebugAddSTR("Tip", szTipAmount, 12);
        if(atol(szTipAmount) > 0)
        {
            vdCTOSS_SetBitMapOn(inBitMap, 54);
        }
#if 0
        if(strlen(srTransPara->szCVV2) > 0)
        {
            vdCTOSS_SetBitMapOn(inBitMap, 48);
        }
#endif		
    }
    else if((inTransCode == SETTLE) && (inTransCode == CLS_BATCH))
    {
        vdDebug_LogPrintf(("Settlement modify field"));
    }

	vdDebug_LogPrintf("[2.]strTCT.fEMVOnlinePIN[%d] | srTransPara->fDebit[%d] | srTransPara->fCUPPINEntry[%d]", strTCT.fEMVOnlinePIN, srTransPara->fDebit, srTransPara->fCUPPINEntry);
	
	if(srTransPara->fDebit== TRUE)
	{
		if(srTransPara->byTransType == SALE || srTransPara->byTransType == BAL_INQ)
		{
            vdCTOSS_SetBitMapOn(inBitMap, 52);
			vdCTOSS_SetBitMapOn(inBitMap, 26);
		}
	}

    //EMV: Online PIN enchancement - start -- jzg
    if(strTCT.fEMVOnlinePIN == TRUE)
    {
        if(srTransPara->fEMVPIN == TRUE) 
        {
            vdCTOSS_SetBitMapOn(inBitMap, 52);
            vdCTOSS_SetBitMapOn(inBitMap, 63);
			vdCTOSS_SetBitMapOn(inBitMap, 26);
        }
    }
    //EMV: Online PIN enchancement - end -- jzg

    if(srTransPara->fCUPPINEntry == TRUE) /*cup card with PIN*/
    {
        vdCTOSS_SetBitMapOn(inBitMap, 52);		
		vdCTOSS_SetBitMapOn(inBitMap, 26);
    }

    /*start - do not send de55 and de23 on refund*/
    if(srTransPara->byTransType == REFUND || srTransPara->byOrgTransType == REFUND)
    {
       vdCTOSS_SetBitMapOff(inBitMap, 55);
       vdCTOSS_SetBitMapOff(inBitMap, 23);
    }
    /* end - do not send de55 and de23 on refund*/	

    // FIX ISSUE: Since offline sale is not uploaded to the host, the advice packet to be sent should be offline
    if(srTransPara->byTransType == SALE_TIP && srTransPara->byOrgTransType == SALE_OFFLINE && srTransPara->byUploaded == FALSE)
        vdCTOSS_SetBitMapOff(inBitMap, 54);
	
    if(memcmp(srTransPara->stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0 && srTransPara->byTransType == VOID)    
    {
        vdCTOSS_SetBitMapOff(inBitMap, 55);
    }

	// For CUP -- sidumili
	if ((srTransPara->byTransType == SALE) && (srTransRec.IITid == 8))
	{
		//vdCTOSS_SetBitMapOff(inBitMap, 25);
		vdCTOSS_SetBitMapOn(inBitMap, 25);
	}
	
	// For CUP / JCB -- sidumili
	if ((srTransPara->byTransType == VOID) && (srTransRec.IITid == 8 || srTransRec.IITid == 4))
	{
		vdCTOSS_SetBitMapOn(inBitMap, 25);
		
		if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
		{
			vdCTOSS_SetBitMapOff(inBitMap, 52);
		}
		else
		{
			vdCTOSS_SetBitMapOff(inBitMap, 23);
			//vdCTOSS_SetBitMapOn(inBitMap, 52);
		}

		vdCTOSS_SetBitMapOff(inBitMap, 52);
		vdCTOSS_SetBitMapOff(inBitMap, 26);
	}
	
	if ((srTransPara->byEntryMode == CARD_ENTRY_ICC) && (srTransPara->byTransType == SALE_OFFLINE || srTransPara->byTransType == SALE_TIP))
	{
		vdCTOSS_SetBitMapOff(inBitMap, 2);
		vdCTOSS_SetBitMapOff(inBitMap, 14);
		vdCTOSS_SetBitMapOn(inBitMap, 35);
	}

	if(srTransPara->byPackType == VOID && srTransPara->byOrgTransType == SALE_OFFLINE && srTransPara->byUploaded == TRUE)
	{
		vdCTOSS_SetBitMapOn(inBitMap, 39);
		vdCTOSS_SetBitMapOn(inBitMap, 60);

		vdCTOSS_SetBitMapOff(inBitMap, 26);
		vdCTOSS_SetBitMapOff(inBitMap, 52);
		
		vdCTOSS_SetBitMapOff(inBitMap, 23);
		vdCTOSS_SetBitMapOff(inBitMap, 55);
	}	

	vdDebug_LogPrintf("byCeillingAmt[%d]", srTransPara->byCeillingAmt);

	if (srTransPara->byCeillingAmt == VS_TRUE)
	{
		vdCTOSS_SetBitMapOff(inBitMap, 23);
		vdCTOSS_SetBitMapOff(inBitMap, 55);

		vdCTOSS_SetBitMapOff(inBitMap, 26);
		vdCTOSS_SetBitMapOff(inBitMap, 52);
	}

	if ((srTransPara->byTransType == DCC_RATE_REQUEST) || (srTransPara->byTransType == DCC_MERCH_RATE_REQ)) 
		{
		     vdCTOSS_SetBitMapOff(inBitMap, 14);
		     vdCTOSS_SetBitMapOff(inBitMap, 54);
		}

	 /*if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.IITid == 8)
 	{
 	     vdCTOSS_SetBitMapOff(inBitMap, 26);
	      vdCTOSS_SetBitMapOff(inBitMap, 52);
 	}*/

	 if (srTransPara->fEMVPIN == TRUE && srTransPara->IITid != 8 && srTransPara->fInstallment == FALSE)
	{
		vdCTOSS_SetBitMapOn(inBitMap, 26);
		vdCTOSS_SetBitMapOn(inBitMap, 52);
	}
    if((strIIT.fBit45ONSwipe==TRUE) && (srTransPara->byEntryMode == CARD_ENTRY_MSR || srTransPara->byEntryMode == CARD_ENTRY_FALLBACK))
    {
          srTransPara->fBit45ONSwipe = TRUE;
         vdCTOSS_SetBitMapOn(inBitMap, 45);
    }

	if (inTransCode == BATCH_UPLOAD)
    {
         if (srTransPara->fBit45ONSwipe == TRUE)
              vdCTOSS_SetBitMapOn(inBitMap, 45);
    }


	vdDebug_LogPrintf("--vdModifyBitMapFunc--[end]");
}

/************************************************************************
Function Name: inSendAndReceiveFormComm()
Description:
    Call function to send and receive data
Parameters:
    [IN] srTransPara
         uszSendData
         inSendLen
         uszReceData
Return: inResult --> Receive data len;
        ST_ERROR
************************************************************************/

int inSendAndReceiveFormComm(TRANS_DATA_TABLE* srTransPara,
                             unsigned char* uszSendData,
                             int inSendLen,
                             unsigned char* uszReceData)
{
    int inResult, inRetval;
    BYTE key;
	char uszSendEcnryptedData[1024+1];
	char uszRcvDecnryptedData[ISO_REC_SIZE+1];
	  int inOrgSendLen = inSendLen;
	  
	BYTE szMTI[MTI_BCD_SIZE+1];
    BYTE szBitMap[8+1];
	char uszTempSendData[ISO_SEND_SIZE+1];
	int inPackLen = 0;

    vdDebug_LogPrintf("**inSendAndReceiveFormComm START**");
	vdDebug_LogPrintf("fTPSec[%d]", strTCT.fTPSec);
	vdDebug_LogPrintf("fTLESec[%d]", strTCT.fTLESec);
	vdDebug_LogPrintf("inHostIndex[%d] . szHostLabel[%s] . fHostTPSec[%d]", strHDT.inHostIndex, strHDT.szHostLabel, strHDT.fHostTPSec);
	vdDebug_LogPrintf("fGBLvDCCDev=[%d]", strGBLVar.fGBLvDCCDev);
	
    //vdMyEZLib_LogPrintff(uszSendData,inSendLen);
    DebugAddHEX("send data",uszSendData,inSendLen);

    if(VS_TRUE == strTCT.fDemo)
    {
        vdDebug_LogPrintf("DEMO Call inSendData!!");
        inResult = d_OK;
    }
    else
    {
		 if(strTCT.fTPSec == TRUE && strHDT.fHostTPSec == TRUE)
		 {
		 	if(inCheckTPSecKey() != d_OK)
				return ST_SEND_DATA_ERR;

			memset(uszSendEcnryptedData, 0, sizeof(uszSendEcnryptedData));
		    inSendLen=inEncryptTPSec(uszSendData, inSendLen, &uszSendEcnryptedData);
            DebugAddHEX("uszSendEcnryptedData",uszSendEcnryptedData,inSendLen); 			
			inResult = srCommFuncPoint.inSendData(srTransPara,uszSendEcnryptedData,inSendLen);

			// Parse ISO Send
			 if(inResult == d_OK && strTCT.fPrintISOMessage == VS_TRUE)
			 {	
				// Revise for printer start mp200 -- sidumili
				vdCTOSS_PrinterStart(100);
				fPrinterStart = TRUE;

				inPrint("*CLEAR PACKET*");
	            vdPrintISOOption("TX",uszSendData, inSendLen);

				inPrint("*ENCRYPTED PACKET*");
	            vdPrintISOOption("TX",uszSendEcnryptedData, inSendLen);
			 }

			 if(inResult == d_OK && fISOLog)
			 {
			 	inPrint("*CLEAR PACKET*");
				vdLogISOOption("TX",uszSendData, inSendLen);

				inPrint("*ENCRYPTED PACKET*");
				vdLogISOOption("TX",uszSendEcnryptedData, inSendLen);
			 }
						
		 }
		 else if(strTCT.fTLESec== TRUE && strHDT.fHostTLESec== TRUE && (srTransPara->byTransType != DCC_RATE_REQUEST && srTransPara->byTransType != DCC_MERCH_RATE_REQ))
		 {
		 	if(inCheckTLESecKey() != d_OK)
				return ST_SEND_DATA_ERR;

			inOrgSendLen = inSendLen;
			memset(uszSendEcnryptedData, 0x00, sizeof(uszSendEcnryptedData));
			inSendLen=inEncryptTLESec(uszSendData, inSendLen, uszSendEcnryptedData);
            DebugAddHEX("uszSendEcnryptedData",uszSendEcnryptedData,inSendLen);
	
			if(strCPT.inCommunicationMode != DIAL_UP_MODE)
				inSendLen-=2;
			
			inResult = srCommFuncPoint.inSendData(srTransPara,uszSendEcnryptedData,inSendLen);	
			vdDebug_LogPrintf("TLE Send, inResult[%d]", inResult);
			
			// Parse ISO Send
			 if(inResult == d_OK && strTCT.fPrintISOMessage == VS_TRUE)
			 {	
				// Revise for printer start mp200 -- sidumili
				vdCTOSS_PrinterStart(100);
				fPrinterStart = TRUE;

				inPrint("*CLEAR PACKET*");
	            vdPrintISOOption("TX", uszSendData, inOrgSendLen);

				inPrint("*TLE-ENCRYPTED PACKET*");
				inPrintISOPacket("TX" , uszSendEcnryptedData, inSendLen);
			 }

			 if(inResult == d_OK && fISOLog)
			 {
			 	inPrint("*CLEAR PACKET*");
				vdLogISOOption("TX",uszSendData, inOrgSendLen);

				inPrint("*TLE-ENCRYPTED PACKET*");
				inLogISOPacket("TX" , uszSendEcnryptedData, inSendLen);
			 }
			 
			  if((strTCT.inISOLogger == TRUE) && (inResult == d_OK)){
			     inSaveISOLog(VS_TRUE, uszSendData, inOrgSendLen);
			  	}
			  
		 }
		 else
		 {
		 	 // sidumili: For DCC which host simulator does not support MTI = 0904. Force to set MTI = 0800
		 	 if (strGBLVar.fGBLvDCCDev && (srTransPara->byTransType == DCC_RATE_REQUEST || srTransPara->byTransType == DCC_MERCH_RATE_REQ))
		 	 {
		 	    inPackLen = 0;
		 	 	memset(szBitMap, 0x00, sizeof(szBitMap));
				memset(szMTI, 0x00, sizeof(szMTI));
				memset(uszTempSendData, 0x00, sizeof(uszTempSendData));

				memcpy(szBitMap, &uszSendData[0], 8);
				inPackLen += TPDU_BCD_SIZE;
				memcpy(szMTI, "\x08\x00", MTI_BCD_SIZE);
				inPackLen += MTI_BCD_SIZE;
				memcpy(uszTempSendData, &uszSendData[inPackLen], inSendLen - inPackLen);

				inPackLen = 0;
				memset(uszSendData, 0x00, sizeof(uszSendData));
				memcpy(&uszSendData[0], szBitMap, TPDU_BCD_SIZE);
				inPackLen += TPDU_BCD_SIZE;
				memcpy(&uszSendData[inPackLen], szMTI, MTI_BCD_SIZE);
				inPackLen += MTI_BCD_SIZE;
				memcpy(&uszSendData[inPackLen], uszTempSendData, inSendLen - inPackLen);

				DebugAddHEX("DCCDEV:::send data",uszSendData,inSendLen);
		 	 }
			 
             inResult = srCommFuncPoint.inSendData(srTransPara,uszSendData,inSendLen);

              vdDebug_LogPrintf("saturn before print ISO %d", inResult);
			 // Parse ISO Send
			 if(inResult == d_OK && strTCT.fPrintISOMessage == VS_TRUE)
			 {	
				// Revise for printer start mp200 -- sidumili
				vdCTOSS_PrinterStart(100);
				fPrinterStart = TRUE;
			
	            vdPrintISOOption("TX",uszSendData, inSendLen);
			 }

			 if(inResult == d_OK && fISOLog)			 
				vdLogISOOption("TX",uszSendData, inSendLen);	

			 if((strTCT.inISOLogger == TRUE) && (inResult == d_OK)){
			 	inSaveISOLog(VS_TRUE, uszSendData, inSendLen);
		}
		 }
    }


    vdDebug_LogPrintf("%%%%%%:[%d]",inResult);

    vdDebug_LogPrintf("**inSendAndReceiveFormComm [%d]",inResult);
    if (inResult != d_OK)
    {
       if( srTransPara->byPackType != TC_UPLOAD )
       	{
             if ((srTransPara->byTransType == DCC_MERCH_RATE_REQ) || (srTransPara->byTransType == DCC_RATE_REQUEST))
             {
                  srTransPara->fDCCOptin = FALSE; //dcc transaction by default field 63 dcc value is 2 (opted local currency)
             }
             vdSetErrorMessage("NO RESPONSE FROM HOST");
       	}
       inSetTextMode();				
       inCTOS_inDisconnect();
       return ST_ERROR;
    }
    else
    {
        //vdDisplayAnimateBmp(0,0, "Comms1.bmp", "Comms2.bmp", "Comms3.bmp", "Comms4.bmp", NULL);
        if(VS_TRUE == strTCT.fDemo)
        {
            vdDebug_LogPrintf("DEMO Call inRecData!!");
            inResult = inCTOS_PackDemoResonse(srTransPara,uszReceData);
        }
        else
        {          
            inResult = srCommFuncPoint.inRecData(srTransPara,uszReceData);
        }
        
        vdDebug_LogPrintf("$$$$$$$$$$$$$$:[%d]",inResult);
        //vdDebug_LogPrintf("inSendAndReceiveFormComm:szRespCode [%s]",srTransPara->szRespCode);
		
        if (inResult > 0)
        {   
            //vdMyEZLib_LogPrintff(uszReceData,inResult);
            DebugAddHEX("recv data",uszReceData,inResult);
			
			if(strTCT.fTPSec == TRUE && strHDT.fHostTPSec == TRUE)
			{								
				if(strCPT.inCommunicationMode == ETHERNET_MODE)
				    inResult-=2;

				memset(uszRcvDecnryptedData, 0x00, sizeof(uszRcvDecnryptedData));
				memcpy(uszRcvDecnryptedData, uszReceData, inResult); // Holds receive encrypted packet
				
				inResult=inDecryptTPSec(uszReceData, inResult, uszReceData);
				DebugAddHEX("decrypted recv data",uszReceData,inResult);

				// Parse ISO Receive
				if(strTCT.fPrintISOMessage == VS_TRUE)
				{
					
					if (strCPT.inCommunicationMode == DIAL_UP_MODE)
					{
						inPrint("*ENCRYPTED PACKET*");
						vdPrintISOOption("RX",uszRcvDecnryptedData, inResult);
						
						inPrint("*CLEAR PACKET*");
						vdPrintISOOption("RX",uszReceData, inResult);						
					}						
					else
					{
						inPrint("*ENCRYPTED PACKET*");
						vdPrintISOOption("RX",uszRcvDecnryptedData, inResult);

						inPrint("*CLEAR PACKET*");
						vdPrintISOOption("RX",uszReceData, inResult);
					}						
				}

				if(fISOLog)
				{
				  inPrint("*ENCRYPTED PACKET*");	
				  vdLogISOOption("RX",uszRcvDecnryptedData, inResult);

				  inPrint("*CLEAR PACKET*");	
				  vdLogISOOption("RX",uszReceData, inResult);				  
				}

                if(inResult > 0)
                {
                     vdDebug_LogPrintf("AAA - inSendAndReceieveFormComm start");
                     if(strTCT.inISOLogger == TRUE){
                          if (strCPT.inCommunicationMode == DIAL_UP_MODE)
                               inSaveISOLog(FALSE, uszReceData, inResult);
                          else
                               inSaveISOLog(FALSE, uszReceData, inResult - 2);
						 
                     }
                }
			}
			else if (strTCT.fTLESec == TRUE && strHDT.fHostTLESec == TRUE && (srTransPara->byTransType != DCC_RATE_REQUEST && srTransPara->byTransType != DCC_MERCH_RATE_REQ))
			{				
				//if(strCPT.inCommunicationMode == ETHERNET_MODE)
				//    inResult-=2;

				memset(uszRcvDecnryptedData, 0x00, sizeof(uszRcvDecnryptedData));
				memcpy(uszRcvDecnryptedData, uszReceData, inResult); // Holds receive encrypted packet
				
				inResult=inDecryptTLESec(uszReceData, inResult, uszReceData);
				DebugAddHEX("TLE decrypted recv data",uszReceData,inResult);

				// Parse ISO Receive
				if(strTCT.fPrintISOMessage == VS_TRUE)
				{
					
					if (strCPT.inCommunicationMode == DIAL_UP_MODE)
					{
						inPrint("*TLE-ENCRYPTED PACKET*");
						inPrintISOPacket("RX", uszRcvDecnryptedData, inResult-2);
						
						inPrint("*CLEAR PACKET*");
						vdPrintISOOption("RX", uszReceData, inResult);						
					}						
					else
					{
						inPrint("*TLE-ENCRYPTED PACKET*");
						inPrintISOPacket("RX", uszRcvDecnryptedData, inResult);

						inPrint("*CLEAR PACKET*");
						vdPrintISOOption("RX", uszReceData, inResult);
					}						
				}

				if(fISOLog)
				{
				  inPrint("*TLE-ENCRYPTED PACKET*");	
				  inLogISOPacket("RX" , uszRcvDecnryptedData, inResult);

				  inPrint("*CLEAR PACKET*");	
				  vdLogISOOption("RX",uszReceData, inResult);				  
				}

                if(strTCT.inISOLogger == TRUE)
                {
                     if (strCPT.inCommunicationMode == DIAL_UP_MODE)
                          inSaveISOLog(FALSE, uszReceData, inResult);
                     else
                          inSaveISOLog(FALSE, uszReceData, inResult - 2);
                }

				// Check respons
				if (inCheckTLEResponseCode(uszReceData, inResult) != d_OK)
					return ST_ERROR;
			}
			else
			{
				// Parse ISO Receive
				if(strTCT.fPrintISOMessage == VS_TRUE)
				{
					if (strCPT.inCommunicationMode == DIAL_UP_MODE)
						vdPrintISOOption("RX",uszReceData, inResult);
					else
						vdPrintISOOption("RX",uszReceData, inResult);
				}

				if(fISOLog)				
				  vdLogISOOption("RX",uszReceData, inResult);		

				if(strTCT.inISOLogger == TRUE){
					 	 vdDebug_LogPrintf("AAA - inSendAndReceieveFormComm inISOLogger true start");
                          if (strCPT.inCommunicationMode == DIAL_UP_MODE)
                               inSaveISOLog(FALSE, uszReceData, inResult);
                          else
                               inSaveISOLog(FALSE, uszReceData, inResult - 2);
                     }
			}			
        }
        else
        {   
            if(strHDT.inDeleteREV == 0) /*albert - auto delete reversal*/
            {
                if(srTransPara->byPackType == REVERSAL) 
                    return ST_ERROR;				
            }
        
            inCTOS_inDisconnect();
        
            if( srTransPara->byPackType != TC_UPLOAD )
            {
                vdDebug_LogPrintf("inSendAndReceiveFormComm[%d]",inResult);
                //vdSetErrorMessage("NO RESP.FRM HOST"); //#00226 - any transaction ensure that there is no response from host._InitModem
                vdSetErrorMessage("NO RESPONSE FROM HOST");
                if (fGetECRTransactionFlg())
                {
                     memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
                     memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
                     strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
                     strcpy(srTransRec.szResponseText, ECR_COMM_ERROR_RESP);
                }
                return ST_ERROR;
            }
        }
    }
    vdDebug_LogPrintf("**inSendAndReceiveFormComm END**");
    return inResult;
}

/************************************************************************
Function Name: inCheckIsoHeaderData()
Description:
    Check message id value
Parameters:
    [IN] srTransPara
         szSendISOHeader
         szReceISOHeader

Return: ST_SUCCESS
        ST_ERROR
************************************************************************/
int inCheckIsoHeaderData(char *szSendISOHeader, char *szReceISOHeader)
{
    int    inCnt = 0;
    TRANS_DATA_TABLE *srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

    inCnt += TPDU_BCD_SIZE;

    szSendISOHeader[inCnt + 1] += 0x10;
    if (memcmp(&szSendISOHeader[inCnt], &szReceISOHeader[inCnt], MTI_BCD_SIZE))
    {
        if(VS_TRUE == strTCT.fDemo)
        {
            return ST_SUCCESS;
        }
        
        vdMyEZLib_LogPrintf("**ISO header data Error**");
        inCTOS_inDisconnect();
        return ST_ERROR;
    }

    return ST_SUCCESS;
}


/************************************************************************
Function Name: inProcessOfflineTrans()
Description:
    Setup and save the file offline transactions need
Parameters:
    [IN] srTransPara
         szSendISOHeader
         szReceISOHeader

Return: ST_SUCCESS
        ST_ERROR
************************************************************************/
int inProcessOfflineTrans(TRANS_DATA_TABLE *srTransPara)
{
    int inResult;
    
    if(srTransPara->byTransType == SALE_TIP && srTransPara->byOrgTransType == SALE_OFFLINE
    && srTransPara->byUploaded == FALSE)
        srTransPara->byPackType = SALE_OFFLINE;
    else
        srTransPara->byPackType = srTransPara->byTransType;
    
    // srTransPara->byPackType = srTransPara->byTransType;
    if(!memcmp(srTransRec.szAuthCode, "Y1", 2))// for save trans as Y1 TC UPLOAD format
        srTransPara->byPackType = SEND_ADVICE;	
    return ST_SUCCESS;
}    



/************************************************************************
Function Name: inAnalyseIsoData()
Description:
    Analysis of the host to send back information
Parameters:
    [IN] srTransPara
         
Return: ST_SUCCESS
        ST_ERROR
************************************************************************/
int inAnalyseIsoData(TRANS_DATA_TABLE *srTransPara)
{
    int inResult;
    ISO_FUNC_TABLE srPackFunc;
        
    inResult = ST_SUCCESS;
    
    if (srTransPara->byTransType == SALE &&
        srTransPara->byEntryMode == CARD_ENTRY_ICC &&
        srTransPara->shTransResult == TRANS_AUTHORIZED &&
        !memcmp(&srTransPara->szAuthCode[0], "Y1", 2))
    {
        return ST_SUCCESS;
    }

    if (srTransPara->byOffline == CN_TRUE)
    {
        return inResult;
    }

    memset((char *)&srPackFunc, 0x00, sizeof(srPackFunc));
    memcpy((char *)&srPackFunc, (char *)&srIsoFuncTable[0], sizeof(srPackFunc));

    if (srPackFunc.inTransAnalyse != 0x00)
    {
        vdSetISOEngTransDataAddress(srTransPara);
        inResult = srPackFunc.inTransAnalyse();
    }
	vdDebug_LogPrintf("inAnalyseIsoData[%d]", inResult);

	if(VS_TRUE == strTCT.fDemo)
		CTOS_LCDTPrintXY(1, 8, "APPROVE        ");

    return inResult;
}

/************************************************************************
Function Name: inCheckHostRespCode()
Description:
    Check the host response code
Parameters:
    [IN] srTransPara
         
Return: TRANS_AUTHORIZED
        TRANS_COMM_ERROR
        TRANS_AUTHORIZED
        TRANS_CALL_BANK
        TRANS_CANCELLED
        ST_UNPACK_DATA_ERR
************************************************************************/
int inCheckHostRespCode(TRANS_DATA_TABLE *srTransPara)
{
    int    inResult = TRANS_COMM_ERROR;
    int inMsgid=0;

    srTransRec.szRespCode[2]=0;
	
    vdDebug_LogPrintf("inCheckHostRespCode %s",srTransPara->szRespCode);

    CTOS_LCDTClearDisplay();	
	
    if (!memcmp(srTransPara->szRespCode, "00", 2)
    || !memcmp(srTransPara->szRespCode, "08", 2)
    || !memcmp(srTransPara->szRespCode, "10", 2)
    || !memcmp(srTransPara->szRespCode, "11", 2)
    || !memcmp(srTransPara->szRespCode, "16", 2))
    {
        	inResult = TRANS_AUTHORIZED;
		srTransPara->shTransResult = TRANS_AUTHORIZED;
		strcpy(srTransPara->szRespCode, "00");
		DebugAddSTR("txn approval",srTransPara->szAuthCode ,6);  
    }
    else
    {
        if((srTransPara->szRespCode[0] >= '0' && srTransPara->szRespCode[0] <= '9') &&
           (srTransPara->szRespCode[1] >= '0' && srTransPara->szRespCode[1] <= '9'))
        {
            memset(srTransRec.szResponseText, 0x00, sizeof(srTransRec.szResponseText));
            inResult = TRANS_REJECTED;
            if((srTransRec.byTransType == SETTLE) && (memcmp(srTransPara->szRespCode,"95",2)))
            {
                // fix for issue 00028
                vdDisplayErrorMsg(1, 8, "SETTLE FAILED");
				strcpy(srTransRec.szResponseText, "SETTLE FAILED");
                
            }
            else
            {
                    inCTOS_DisplayResponse();
            }
        }
        else
        {
            char szTemp[30+1];
            memset(szTemp,0x00,sizeof(szTemp));
            strcpy(szTemp, "RC: ");   	   
            strcat(szTemp, srTransRec.szRespCode);
            vdDisplayErrorMsg2(1, 8, "CALL MCC HELP DESK", szTemp, MSG_TYPE_ERROR);
			strcpy(srTransRec.szResponseText, "CALL MCC HELP DESK");
            inResult = TRANS_REJECTED;           
        }
		ushCTOS_printErrorReceipt();
    }

    vdDebug_LogPrintf("<><><><><><><><><>[%d]", inResult);

    return (inResult);
}

int inCheckHostFEXCORespCode(TRANS_DATA_TABLE *srTransPara)
{
     int inMsgid = atoi(srTransRec.szRespCode);
    int inHostIndex = srTransRec.HDTid;
    int inResult = 0;
    char szResponseCode[40], szResponseCode2[40];
	
    CTOS_LCDTClearDisplay();	//#00235 - Unexpected response code errors, should display other than 00 - 99

    srTransRec.szRespCode[2]=0;
	
    vdDebug_LogPrintf("inCheckHostFEXCORespCode %s",srTransPara->szRespCode);

    CTOS_LCDTClearDisplay();	
	
    memset(szResponseCode, 0x00, sizeof(szResponseCode2));	
    memset(szResponseCode2, 0x00, sizeof(szResponseCode));
    //vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]strHDT.szAPName[%s]", inMsgid, inHostIndex, szResponseCode, strHDT.szAPName);
    inMSG2ResponseCodeReadByHostName(szResponseCode, szResponseCode2, inMsgid, 1);  

     if (inMsgid!=ST_SUCCESS)
     {
          if(0 == strlen(szResponseCode) )
          {
              // inMSGResponseCodeRead(szResponseCode, szResponseCode2, inMsgid, 1);
               if(0 != strlen(szResponseCode) )
               {					
					vdDisplayMessageBox(1, 8, szResponseCode, szResponseCode2, "", MSG_TYPE_ERROR);					
					CTOS_Beep();
					CTOS_Delay(1500);
					CTOS_Beep();
					//vdDisplayErrorMsg2(1, 8, szResponseCode, szResponseCode2);
               }
          }
          else
          {
                vdDisplayMessageBox(1, 8, szResponseCode, szResponseCode2, "", MSG_TYPE_ERROR);
				CTOS_Beep();
				CTOS_Delay(1500);
				CTOS_Beep();
        		//vdDisplayErrorMsg2(1, 8, szResponseCode, szResponseCode2);
          }
               memset(srTransRec.szResponseText, 0x00, sizeof(srTransRec.szResponseText));
               sprintf(srTransRec.szResponseText, "%s %s", szResponseCode2, szResponseCode);
          
               vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
          
               ushCTOS_printErrorReceipt();
	     srTransPara->shTransResult = inMsgid;

	     usCTOSS_LCDDisplay(" ");
     }
    //else
		//srTransPara->shTransResult = TRANS_AUTHORIZED;

       
  
    return (srTransPara->shTransResult);
}


int inBaseRespValidation(TRANS_DATA_TABLE *srOrgTransPara,TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("inBaseRespValidation ulTraceNum=[%ld][%ld]",srOrgTransPara->ulTraceNum,srTransPara->ulTraceNum);
	if (srOrgTransPara->ulTraceNum != srTransPara->ulTraceNum)
	{
		vdSetErrorMessage("TID Not Match");
		return ST_RESP_MATCH_ERR;
	}

	vdDebug_LogPrintf("inBaseRespValidation szTID=[%s][%s]",srOrgTransPara->szTID,srTransPara->szTID);
	if (memcmp(srOrgTransPara->szTID,srTransPara->szTID,TERMINAL_ID_BYTES) != 0)
	{
		vdSetErrorMessage("STAN Not Match");
		return ST_RESP_MATCH_ERR;
	}

	return ST_SUCCESS;
}

/************************************************************************
Function Name: inCheckTransAuthCode()
Description:
    Check the host authorization code
Parameters:
    [IN] srTransPara
         
Return: ST_SUCCESS
        ST_ERROR
      
************************************************************************/

int inCheckTransAuthCode(TRANS_DATA_TABLE *srTransPara)
{
    int inResult = ST_SUCCESS;

    if(srTransPara->byTransType != SETTLE && srTransPara->byTransType != CLS_BATCH)
    {
        if (!memcmp(&srTransPara->szAuthCode[0], "000000", 6) || 
            !memcmp(&srTransPara->szAuthCode[0], "      ", 6))
        {
            if(srTransPara->byTransType != VOID) //Synergy host does not return Auth.code for void sale
                inResult = ST_ERROR;
        }
    }

    return (inResult);
}


int inAnalyseChipData(TRANS_DATA_TABLE *srTransPara)
{
    int	inResult;
    USHORT inlen=0;
    unsigned char stScript[512];
    
//    vduiClearBelow(8); // patrick fix code 20140421
    
	memset(stScript,0,sizeof(stScript));

    vdDebug_LogPrintf("tag71[%d] tag72[%d]", srTransPara->stEMVinfo.T71Len, srTransPara->stEMVinfo.T72Len);  
    if( srTransPara->stEMVinfo.T71Len>0)
    {
    	memcpy(&stScript[inlen], srTransPara->stEMVinfo.T71, srTransPara->stEMVinfo.T71Len );
    	inlen=srTransPara->stEMVinfo.T71Len;    	
    }
    if( srTransPara->stEMVinfo.T72Len>0)
    {
    	memcpy(&stScript[inlen], srTransPara->stEMVinfo.T72, srTransPara->stEMVinfo.T72Len );
      inlen= inlen + 	srTransPara->stEMVinfo.T72Len;      
    }	
    DebugAddHEX("inAnalyseChipData ", stScript, inlen);
	inResult = shCTOS_EMVSecondGenAC(stScript, inlen);

	vdDebug_LogPrintf("inAnalyseChipData[%d] srTransPara->shTransResult[%d] srTransPara->byOffline[%d]", inResult, srTransPara->shTransResult, srTransPara->byOffline);

    if(inResult == PP_OK)
    {
        vdDisplayTxnFinishUI();
        //if(0 != memcmp(srTransRec.szAuthCode, "Y3", 2))
        {
            inMyFile_ReversalDelete();

            #if 0
            if (srTransPara->byOffline == CN_FALSE)
    		{		
    		    if(strHDT.inNumAdv > 0)
                {
                    inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);
    			    inProcessAdviceTrans(srTransPara, strHDT.inNumAdv);
                }
    		}
			#endif
        }
        
        
        inResult = ST_SUCCESS;
        
    }
    else
    {
        if (srTransPara->shTransResult == TRANS_AUTHORIZED || srTransPara->shTransResult == TRANS_COMM_ERROR)
        {
        }
        else
        {
       
            vdDebug_LogPrintf(". Resp Err");
        }
        inResult = ST_ERROR;
        
    }
    
    return inResult;
}

int inAnalyseNonChipData(TRANS_DATA_TABLE *srTransPara)
{
	int	inResult = ST_SUCCESS;

	vdDebug_LogPrintf("**inAnalyseNonChipData(TxnResult = %d) [%d] byTransType[%d] START**", srTransPara->shTransResult, srTransPara->byPackType, srTransPara->byTransType);
    if(srTransPara->byPackType == SETTLE || srTransPara->byPackType == CLS_BATCH)
    {
        if(srTransPara->shTransResult != TRANS_AUTHORIZED)
            inResult = ST_ERROR;
        
    }
    else if (srTransPara->shTransResult == TRANS_AUTHORIZED)
	{
	    vdDisplayTxnFinishUI();

        //Should be Online void the Intial SALE amount.
        if(srTransPara->byTransType == VOID)
        {
            inCTOSS_DeleteAdviceByINV(srTransPara->szInvoiceNo);
        }
        #if 0
		if(srTransPara->byOffline == CN_FALSE)
		{		
		    if(strHDT.inNumAdv > 0)
            {     
                inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);
			    inProcessAdviceTrans(srTransPara, strHDT.inNumAdv);
            }
		}
		#endif
	}
	else if (srTransPara->shTransResult == TRANS_CANCELLED)
	{		
		inResult = ST_ERROR;
	}
	else
	{		
		inResult = ST_ERROR;
	}

	vdDebug_LogPrintf("**inAnalyseNonChipData(%d) END**", inResult);
	return inResult;
}

int inAnalyseReceiveData(void)
{
	int	inResult;
    TRANS_DATA_TABLE* srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

	if ((srTransPara->byEntryMode == CARD_ENTRY_ICC) && 
            ((srTransPara->byTransType == SALE) || (srTransPara->byTransType == PRE_AUTH))
           )
	{
		// Minimize the white screend display -- sidumili
		if (isCheckTerminalMP200() == d_OK)
			vdCTOS_DispStatusMessage("PROCESSING...");
		
		inResult = inAnalyseChipData(srTransPara);
	}
	else
	{
		inResult = inAnalyseNonChipData(srTransPara);
	}

	return inResult;
}


int inAnalyseAdviceData(int inPackType)
{   
    TRANS_DATA_TABLE *srTransPara;
    
    srTransPara = srGetISOEngTransDataAddress();
    
    if(srTransPara->byTransType == SALE_OFFLINE) // || srTransPara->byTransType == PRE_COMP)
    {
        srTransPara->byUploaded = CN_FALSE;
        
    }else if(srTransPara->byTransType == SALE_TIP || srTransPara->byTransType == SALE_ADJUST)
    {
           if(srTransPara->byOffline == CN_TRUE)
            srTransPara->byUploaded = CN_FALSE;
    }
    
    return ST_SUCCESS;
}



int inPorcessTransUpLoad(TRANS_DATA_TABLE *srTransPara)
{
    int inSendCount,inTotalCnt,inFileMaxLen;
    int inResult;
    int inBatchRecordNum = 0;
    int i;
    TRANS_DATA_TABLE srUploadTransRec,srTransParaTmp;
    STRUCT_FILE_SETTING strFile;
    int  *pinTransDataid = NULL;
    int  *pinTransDataidSend = NULL;

    inResult = ST_SUCCESS;
    
    memset(&srUploadTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));

    inBatchRecordNum = inBatchNumRecord();

    vdDebug_LogPrintf("BatchUpload totaltxn[%d]",inBatchRecordNum);
    if(inBatchRecordNum > 0)
    {
        pinTransDataid = (int*)malloc(inBatchRecordNum * sizeof(int));
        pinTransDataidSend = (int*)malloc(inBatchRecordNum * sizeof(int));

	    inBatchByMerchandHost(inBatchRecordNum, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

        inTotalCnt = 0;
        for(i=0; i<inBatchRecordNum; i++)
        {
            inDatabase_BatchReadByTransId(&srUploadTransRec, pinTransDataid[i]);

            vdDebug_LogPrintf("BatchUpload curren[%d] Void?[%d]",pinTransDataid[i], srUploadTransRec.byVoided);
            if((srUploadTransRec.byVoided != TRUE) && (srUploadTransRec.byTransType != PRE_AUTH))
            {
                pinTransDataidSend[inTotalCnt] = pinTransDataid[i];
                inTotalCnt ++;
            }
        }

        //in case all is void
        inResult = ST_SUCCESS; 
        inFinalSend = CN_TRUE;

        vdDebug_LogPrintf("BatchUpload total None void txn[%d]",inTotalCnt);
        for (inSendCount = 0; inSendCount < inTotalCnt; inSendCount ++)
        {
            if(((inSendCount + 1) == inTotalCnt))
                inFinalSend = CN_TRUE;
            else
                inFinalSend = CN_FALSE;

            vdDebug_LogPrintf("Before HDTid[%d]MITid[%d]AMT[%02X%02X%02X%02X%02X%02X]", srTransPara->HDTid, srTransPara->MITid, srTransPara->szTotalAmount[0]
                                                                                                                            , srTransPara->szTotalAmount[1]
                                                                                                                            , srTransPara->szTotalAmount[2]
                                                                                                                            , srTransPara->szTotalAmount[3]
                                                                                                                            , srTransPara->szTotalAmount[4]
                                                                                                                            , srTransPara->szTotalAmount[5]);
            inDatabase_BatchReadByTransId(&srUploadTransRec, pinTransDataidSend[inSendCount]);
            
            vdDebug_LogPrintf("After HDTid[%d]MITid[%d]AMT[%02X%02X%02X%02X%02X%02X]", srUploadTransRec.HDTid, srUploadTransRec.MITid, srUploadTransRec.szTotalAmount[0]
                                                                                                                            , srUploadTransRec.szTotalAmount[1]
                                                                                                                            , srUploadTransRec.szTotalAmount[2]
                                                                                                                            , srUploadTransRec.szTotalAmount[3]
                                                                                                                            , srUploadTransRec.szTotalAmount[4]
                                                                                                                            , srUploadTransRec.szTotalAmount[5]);

            if((srUploadTransRec.byTransType == PRE_AUTH)/* || (srUploadTransRec.byTransType == PREAUTH_VER)*/
            || (srUploadTransRec.byTransType == PREAUTH_VOID))
            {
                continue;
            }
            
            srUploadTransRec.byPackType =  BATCH_UPLOAD;
            
            vdDebug_LogPrintf(". Bef Add szTraceNo = %02x%02x%02x",strHDT.szTraceNo[0],
                                                                strHDT.szTraceNo[1],
                                                                strHDT.szTraceNo[2]);
            
            inMyFile_HDTTraceNoAdd(srUploadTransRec.HDTid);
            
            vdDebug_LogPrintf(". Aft Add szTraceNo = %02x%02x%02x",strHDT.szTraceNo[0],
                                                                strHDT.szTraceNo[1],
                                                                strHDT.szTraceNo[2]);
            
            srUploadTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
            vdDebug_LogPrintf(". UploadSendTracNum(%d) [%s]",srUploadTransRec.ulTraceNum,srUploadTransRec.szTID);
			memcpy(&srTransParaTmp,&srUploadTransRec,sizeof(TRANS_DATA_TABLE));
            
            if ((inResult = inPackSendAndUnPackData(&srUploadTransRec, srUploadTransRec.byPackType) != ST_SUCCESS))
            {
                vdDebug_LogPrintf(". inPorcessTransUpLoad(%d)_Err",inResult);
                vdDebug_LogPrintf(". byTransType %d",srUploadTransRec.byTransType);
                inResult = ST_UNPACK_DATA_ERR;
                break;
            }
            else
            {
                if (memcmp(srUploadTransRec.szRespCode, "00", 2))
                {
                    vdDebug_LogPrintf(". inPorcessTransUpLoad(%s) BatchUpload Fail", srUploadTransRec.szRespCode);
                    inResult = ST_ERROR;
                    break;
                }
				else
				{
					if (ST_SUCCESS != inBaseRespValidation(&srTransParaTmp,&srUploadTransRec))
					{
						inResult = ST_UNPACK_DATA_ERR;
						break;
					}
				}
            
                vdDebug_LogPrintf(". inPorcessTransUpLoad(%d)BatchUpload OK", inResult);
            }
        }

        free(pinTransDataid);
        free(pinTransDataidSend);
    
    }
    else
    {
        inResult = ST_ERROR;
        vdDebug_LogPrintf("No bath record found");
    }

    /*albert - fix stan issue*/
    //inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
    //srUploadTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    //vdMyEZLib_LogPrintf(". Aft Upload TraceNum(%d)",srUploadTransRec.ulTraceNum);
    
    if(inResult == ST_SUCCESS && inFinalSend == CN_TRUE)
    {
        srTransPara->byPackType = CLS_BATCH; /*albert - fix stan issue*/
        inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
        srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
        vdMyEZLib_LogPrintf(". Aft CLS_BATCH TraceNum(%d)",srTransPara->ulTraceNum);
        if ((inResult = inPackSendAndUnPackData(srTransPara, srTransPara->byPackType) != ST_SUCCESS))
        {
            vdMyEZLib_LogPrintf(". FinalSettle(%d)_Err",inResult);
            vdMyEZLib_LogPrintf(". byTransType %d",srTransPara->byTransType);
            inResult = ST_UNPACK_DATA_ERR;
        }

		/* MCC: Check settlement reconciliation response code - start -- jzg */
		if(memcmp(srTransPara->szRespCode, "00", 2) != 0)
		{
			vdDebug_LogPrintf("JEFF::ST_CLS_BATCH_ERR");
			inResult = ST_CLS_BATCH_ERR;
		}
		/* MCC: Check settlement reconciliation response code - end -- jzg */
        //inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
        //srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
        //vdMyEZLib_LogPrintf(". Aft CLS_BATCH TraceNum(%d)",srTransPara->ulTraceNum);
    }

    return inResult;
}


int inPackIsoFunc02(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    int inPANLen;
    char szTempPan[PAN_SIZE+1];

    vdDebug_LogPrintf("**inPackIsoFunc02 STARTo**");
    
    inDataCnt = 0;
    inPANLen = 0;

    memset(szTempPan, 0x00, sizeof(szTempPan));   

    if ((srTransPara->byTransType == DCC_MERCH_RATE_REQ) || (srTransPara->byTransType == DCC_RATE_REQUEST))
    {
          if((srTransPara->IITid == VISA_ISSUER) || (srTransPara->IITid == MASTERCARD_ISSUER))
          {
                inPANLen = 11;
                memcpy(szTempPan,srTransPara->szPAN,11);
          }
		  else
          {
               inPANLen = 6;
               memcpy(szTempPan,srTransPara->szPAN,6);
          }
    }
    else
    {
         inPANLen = strlen(srTransPara->szPAN);
         memcpy(szTempPan,srTransPara->szPAN,inPANLen);
    }  
    uszSendData[inDataCnt ++] = (inPANLen / 10 * 16) + (inPANLen % 10);
        
    if (inPANLen % 2)
        szTempPan[inPANLen ++] = 'F';
    
#ifdef TLE
    memset(&uszSendData[inDataCnt], 0x00, (inPANLen+1) / 2);
    byField_02_ON = TRUE;
#else
    wub_str_2_hex(szTempPan, (char *)&uszSendData[inDataCnt], inPANLen);
#endif    
    inDataCnt += (inPANLen / 2);
    
    vdDebug_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc02 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(2) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
    vdMyEZLib_HexDump("FIELD 02:", uszSendData, inDataCnt);
        	}
    #endif
    return (inDataCnt);

}

int inPackIsoFunc03(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    inDataCnt = 0;
	char szTemp[20+1];

    
    vdDebug_LogPrintf("**inPackIsoFunc03 START**");

    
    memcpy(&uszSendData[inDataCnt], &srTransPara->szIsoField03[0], PRO_CODE_BCD_SIZE);
    inDataCnt += PRO_CODE_BCD_SIZE;

 
    if((BATCH_UPLOAD == srTransPara->byPackType))
    {
        //fix Tip adjust Sale , batch upload process code is 02000x, should be 00000x
        // fix for issue incorrect processing code on Cash Advance batch upload message(0320)
        //uszSendData[0] &= 0xF0;
        
        if(inFinalSend != CN_TRUE)
            uszSendData[2] |= 0x01;
    }

#if 0
	if(srTransPara->byPackType != TC_UPLOAD)
	{
		if(srTransPara->byEntryMode == CARD_ENTRY_ICC && srTransPara->byTransType == SALE)
			uszSendData[2] |= 0x01;
	}
#endif

#if 0
        if(inCTOSS_CheckBitmapSetBit(3) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
    vdMyEZLib_HexDump("FIELD 03:", uszSendData, inDataCnt);
}
        	}
#endif
		
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc03 END**");
    return (inDataCnt);
}

int inPackIsoFunc04(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    inDataCnt = 0;

    vdDebug_LogPrintf("inPackIsoFunc04 srTransPara->byPackType:%d, srTransPara->byTransType: %d, srTransPara->byUploaded: %d", srTransPara->byPackType, srTransPara->byTransType, srTransPara->byUploaded);
    //Should be Online void the Intial SALE amount.
    if(srTransPara->byTransType == VOID)
    {
        //use szStoreID to store how much amount fill up in DE4 for VOID
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szStoreID, 6);
        if(srTransPara->byPackType == VOID && srTransPara->byOrgTransType == SALE_OFFLINE && srTransPara->byUploaded == TRUE)
            memcpy((char *)&uszSendData[inDataCnt], "\x00\x00\x00\x00\x00\x00", 6);	
    }
    else
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szTotalAmount, 6);
    
    inDataCnt += 6;

#if 0
        if(inCTOSS_CheckBitmapSetBit(4) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
    vdMyEZLib_HexDump("FIELD 04:", uszSendData, inDataCnt);
}
        	}
#endif	
    return (inDataCnt);
}

int inPackIsoFunc05(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    inDataCnt = 0;

    vdDebug_LogPrintf("inPackIsoFunc05 srTransPara->byPackType:%d, srTransPara->byTransType: %d, srTransPara->byUploaded: %d", srTransPara->byPackType, srTransPara->byTransType, srTransPara->byUploaded);
    //Should be Online void the Intial SALE amount.
    if(srTransPara->byTransType == VOID)
    {
        //use szStoreID to store how much amount fill up in DE4 for VOID
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szStoreID, 6);
        if(srTransPara->byPackType == VOID && srTransPara->byOrgTransType == SALE_OFFLINE && srTransPara->byUploaded == TRUE)
            memcpy((char *)&uszSendData[inDataCnt], "\x00\x00\x00\x00\x00\x00", 6);	
    }
    else
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szTotalAmount, 6);
    
    inDataCnt += 6;

#if 0
        if(inCTOSS_CheckBitmapSetBit(4) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
    vdMyEZLib_HexDump("FIELD 04:", uszSendData, inDataCnt);
}
        	}
#endif	
    return (inDataCnt);
}

int inPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szSTAN[6 + 1];
    
    inDataCnt = 0;    
    memset(szSTAN, 0x00, sizeof(szSTAN));
    
    
    if(srTransPara->byTransType == SETTLE)// 20121204
    {
        sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
        wub_str_2_hex(&szSTAN[0], (char *)&uszSendData[inDataCnt], 6);
        vdDebug_LogPrintf("sys trace %d",uszSendData[0], uszSendData[1],uszSendData[2]);        
    }
     else                  
    {
        if((srTransPara->byPackType == TC_UPLOAD) || srTransPara->byTransType == CLS_BATCH)
        {
            vdDebug_LogPrintf("**inPackIsoFunc11 %d**", srTransPara->ulTraceNum);
            sprintf(szSTAN, "%06ld", (srTransPara->ulTraceNum+1));
        }
        else
            sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
        
        wub_str_2_hex(&szSTAN[0], (char *)&uszSendData[inDataCnt], 6);
    }
    
    inDataCnt += 3;

#if 0
        if(inCTOSS_CheckBitmapSetBit(11) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
	vdMyEZLib_HexDump("FIELD 11:", uszSendData, inDataCnt);
}
        	}
#endif

    vdDebug_LogPrintf("  PACK_LEN[%d] %d [%02X%02X%02X] inFinalSend[%d]byPackType[%d]",inDataCnt, srTransPara->byPackType, uszSendData[0], uszSendData[1], uszSendData[2], inFinalSend, srTransPara->byPackType);
    return (inDataCnt);
}

void vdGetTimeDate(TRANS_DATA_TABLE *srTransPara)
{
	if(srTransPara->byPackType == SALE || srTransPara->byPackType == PRE_AUTH 
			|| srTransPara->byPackType == LOG_ON 
			||srTransPara->byPackType == BAL_INQ ||srTransPara->byPackType == CASH_ADV
			||srTransPara->byPackType == PRE_COMP ||srTransPara->byPackType == REVERSAL)
	{
		CTOS_RTC SetRTC;
		BYTE szCurrentTime[20];
			
		CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
		sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
		wub_str_2_hex(szCurrentTime,srTransPara->szDate,DATE_ASC_SIZE);

		memset(szCurrentTime, 0, sizeof(szCurrentTime));
		sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
		wub_str_2_hex(szCurrentTime,srTransPara->szTime,TIME_ASC_SIZE);
	}

}


int  inPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTempTime[6];

    vdGetTimeDate(srTransPara);
		
    vdMyEZLib_LogPrintf("**inPackIsoFunc12 START**");
    inDataCnt = 0;
    
    if(srTransPara->byPackType == SALE_TIP || srTransPara->byPackType == SALE_ADJUST || srTransPara->byPackType == SALE_TIP)// || srTransPara->byPackType == REVERSAL)
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgTime, 2);
    else
       memcpy((char *)&uszSendData[inDataCnt], srTransPara->szTime, 3);

	
    inDataCnt += 3;
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc12 END**");

    return (inDataCnt);
}

int inPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTempDate[6];
    
    vdMyEZLib_LogPrintf("**inPackIsoFunc13 START**");

#if 0
{
	char szTerms[30+1];
	memset(szTerms,0x00,sizeof(szTerms));
	sprintf(szTerms, "inPackIsoFunc13::[%d]", srTransPara->byPackType);
	CTOS_PrinterPutString(szTerms);
	
}
#endif	
//	CTOS_PrinterPutString("inPackIsoFunc13");

    //memcpy(srTransPara->szOrgDate, srTransRec.szDate, 2);
	
    inDataCnt = 0;
    
    if(srTransPara->byPackType == SALE_TIP || srTransPara->byPackType == SALE_ADJUST || srTransPara->byPackType == SALE_TIP)// ||	srTransPara->byPackType == REVERSAL)
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgDate, 2);
    else
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szDate, 2);
    inDataCnt += 2;
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc13 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(13) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 13:", inDataCnt);
        	}
	#endif
    return (inDataCnt);
}

int inPackIsoFunc14(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTempExpDate[6];
    vdMyEZLib_LogPrintf("**inPackIsoFunc14 START**");
    inDataCnt = 0;

#ifdef TLE 
    memcpy((char *)&uszSendData[inDataCnt], "\x00\x00", 2);
    byField_14_ON = TRUE;
#else
    memcpy((char *)&uszSendData[inDataCnt], srTransPara->szExpireDate, 2);
#endif
    inDataCnt += 2;

#if 0
        if(inCTOSS_CheckBitmapSetBit(14) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 14:", uszSendData, inDataCnt);
        	}
#endif
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc14 END**");
    return (inDataCnt);
}

int inPackIsoFunc18(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTempExpDate[6];
    vdMyEZLib_LogPrintf("**inPackIsoFunc18 START**");
    inDataCnt = 0;

#ifdef TLE 
    memcpy((char *)&uszSendData[inDataCnt], "\x00\x00", 2);
    byField_14_ON = TRUE;
#else
    memcpy((char *)&uszSendData[inDataCnt], srTransPara->szExpireDate, 2);
#endif
    inDataCnt += 2;

#if 0
        if(inCTOSS_CheckBitmapSetBit(14) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 14:", uszSendData, inDataCnt);
        	}
#endif
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc18 END**");
    return (inDataCnt);
}

int inPackIsoFunc22(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTempEnterMode[5];
	int inPINCapability = 0;

	vdMyEZLib_LogPrintf("**inPackIsoFunc22 START**");
   
    inDataCnt = 0;
       
    memset(szTempEnterMode,0,sizeof(szTempEnterMode));
    
    if (srTransPara->byEntryMode == CARD_ENTRY_ICC)
    {
        //sprintf(szTempEnterMode,"005%d",srTransPara->byPINEntryCapability);
        //strcpy(szTempEnterMode,"0051");	
		if((srTransPara->fEMVPIN == TRUE || srTransPara->fEMVPINEntered== TRUE || srTransPara->stEMVinfo.T9F34[0] == 0x44 || ((srTransPara->stEMVinfo.T9F34[0] & 0x0F) == 0x01)) && (srTransRec.fEMVPINBYPASS==FALSE && srTransRec.fInstallment==FALSE))
            strcpy(szTempEnterMode,"0051");
		else
			strcpy(szTempEnterMode,"0052");
    }
    else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
    {
        //sprintf(szTempEnterMode,"080%d",srTransPara->byPINEntryCapability);
        //strcpy(szTempEnterMode,"0802");
        if(srTransPara->fCUPPINEntry == TRUE)
            strcpy(szTempEnterMode,"0801");
		else
			strcpy(szTempEnterMode,"0802");
    }
     else if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
    {
        //sprintf(szTempEnterMode,"002%d",srTransPara->byPINEntryCapability);
        if(srTransPara->fCUPPINEntry == TRUE)
            strcpy(szTempEnterMode,"0021");
        else
            strcpy(szTempEnterMode,"0022");
    }
    else if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL)
    {        
        if(srTransPara->fCUPPINEntry == TRUE)
            strcpy(szTempEnterMode,"0011");
        else
            strcpy(szTempEnterMode,"0012");
    }
	else if (srTransPara->byEntryMode  == CARD_ENTRY_WAVE)
    {   
        if(srTransPara->fEMVPIN == TRUE || ((srTransPara->stEMVinfo.T9F34[0] & 0x0F) == 0x01))
            strcpy(szTempEnterMode,"0071");
		else
            strcpy(szTempEnterMode,"0072");
		

		if ((srTransPara->bWaveSID == d_VW_SID_PAYPASS_MAG_STRIPE) ||
        (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_MSD) ||
        (srTransPara->bWaveSID == d_VW_SID_AE_MAG_STRIPE) ||
        (srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_2) ||
        (srTransPara->bWaveSID == 0x64))
        {
			if(srTransPara->fEMVPIN == TRUE || ((srTransPara->stEMVinfo.T9F34[0] & 0x0F) == 0x01))
				strcpy(szTempEnterMode,"0911");
			else
				strcpy(szTempEnterMode,"0912");
        }
    }
    else if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL2) // #00143 - Fallback to MKE should be implemented
    {        
         //sprintf(szTempEnterMode,"001%d",srTransPara->byPINEntryCapability);
         strcpy(szTempEnterMode,"0792");
    }

	if(srTransPara->byTransType == VOID && srTransPara->IITid == 8) /*issuer 8: CUP*/
	{
		memset(szTempEnterMode,0,sizeof(szTempEnterMode));
        if(srTransPara->fEMVPIN == TRUE || srTransPara->fCUPPINEntry == TRUE || ((srTransPara->stEMVinfo.T9F34[0] & 0x0F) == 0x01))
			strcpy(szTempEnterMode,"0011");
		else
			strcpy(szTempEnterMode,"0012");
	}
		
    wub_str_2_hex(szTempEnterMode, (char *)&uszSendData[inDataCnt], 4);
    
    inDataCnt +=2 ; //+= 3; 

#if 0
        if(inCTOSS_CheckBitmapSetBit(22) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
	vdMyEZLib_HexDump("FIELD 22:", uszSendData, inDataCnt);
}
        	}
	#endif


	vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc22 END**");
    return (inDataCnt);
}

int inPackIsoFunc23(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc23 START**");
    inDataCnt = 0;
    
    uszSendData[inDataCnt] = 0x00;
    uszSendData[inDataCnt+1] = srTransPara->stEMVinfo.T5F34;
    
    inDataCnt += 2;

#if 0
        if(inCTOSS_CheckBitmapSetBit(23) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 23:", uszSendData, inDataCnt);
        	}
        #endif
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc23 END**");
    return (inDataCnt);
}


int inPackIsoFunc24(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    inDataCnt = 0;

    DebugAddHEX("inPackIsoFunc24", strHDT.szNII, 2);
    
    memcpy((char *)&uszSendData[inDataCnt], strHDT.szNII, 2);
    
    inDataCnt += 2;

#if 0
        if(inCTOSS_CheckBitmapSetBit(24) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 24:", uszSendData, inDataCnt);
        	}
            #endif
    return (inDataCnt);
}

int inPackIsoFunc25(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc25 START**");
    inDataCnt = 0;

#if 0
{
	char szTerms[30+1];
	memset(szTerms,0x00,sizeof(szTerms));
	sprintf(szTerms, "inPackIsoFunc25::[%d]:[%d]", srTransPara->byPackType, srTransPara->byOrgTransType);
	CTOS_PrinterPutString(szTerms);
	
}
#endif	
	

//fix for: Incorrect DE 25 value on Sale Completion
// POS Condition code for Sale completion and void sale completion should be 06
//#00169
        vdDebug_LogPrintf("AAA - srTransPara->byOrgTransType[%d], srTransPara->byPackType[%d] srTransPara->byTransType[%d] srTransPara->szMassageType[%02X] srTransPara->szMassageType[%02X]", srTransPara->byOrgTransType, srTransPara->byPackType, srTransPara->byTransType, srTransPara->szMassageType[0], srTransPara->szMassageType[1]);
	if(srTransPara->byPackType == PRE_COMP || srTransPara->byPackType == VOID && srTransPara->byOrgTransType == PRE_COMP) 
	{
	     wub_str_2_hex("06", (char *)&uszSendData[inDataCnt], 2);
	    inDataCnt += 1;
	    return (inDataCnt);
	}
	else if (srTransPara->byPackType == PREAUTH_VER && srTransPara->szMassageType[0] == 0x02 && srTransPara->szMassageType[1] == 0x20) 
	{
	     wub_str_2_hex("06", (char *)&uszSendData[inDataCnt], 2);
	    inDataCnt += 1;
	    return (inDataCnt);
	}

	if ((srTransPara->IITid == 3) && (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL)) //Amex Manual Entry de25 value
	{
	     wub_str_2_hex("05", (char *)&uszSendData[inDataCnt], 2);
	    inDataCnt += 1;
	    return (inDataCnt);
	}

    wub_str_2_hex("00", (char *)&uszSendData[inDataCnt], 2);
    
    inDataCnt += 1;

#if 0
        if(inCTOSS_CheckBitmapSetBit(25) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 25:", uszSendData, inDataCnt);
        	}
	#endif
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc25 END**");
    return (inDataCnt);
}


int inPackIsoFunc26(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc26 START**");
    inDataCnt = 0;

	//temp remove
    //uszSendData[inDataCnt] = 0x08;
    uszSendData[inDataCnt] = 0x06;
    
    inDataCnt += 1;
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc26 END**");
    return (inDataCnt);
}


int inPackIsoFunc35(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTrack2Data[100+1];//[40]; - fix system error for tap to go txn - mfl
    int inLen;
    int i;

    
    vdMyEZLib_LogPrintf("**inPackIsoFunc35 START**");
    		//CTOS_PrinterPutString("inPackIsoFunc35");

	
    inDataCnt = 0;
    inLen = 0;
    
    memset(szTrack2Data,0x00,sizeof(szTrack2Data));   
    strcpy(szTrack2Data, srTransPara->szTrack2Data);

    DebugAddHEX("inPackIsoFunc35-start", szTrack2Data, 100);

    for (i = 0; i < strlen(szTrack2Data); i ++)
    {
        if (szTrack2Data[i] == '=')
            szTrack2Data[i] = 'D';
			
		if (szTrack2Data[i] == '?' || szTrack2Data[i] == 'F')
            szTrack2Data[i] = 0x00;
    }

    inLen = strlen(szTrack2Data);
    
    /* Data Length */
			
    uszSendData[inDataCnt ++] = (inLen / 10 * 16) + (inLen % 10);

    if (inLen % 2)
    {
        inLen ++;
        strcat(szTrack2Data, "F");
    }

    		//CTOS_PrinterPutString("inPackIsoFunc35.1");
	
#ifdef TLE
    memset((char *)&uszSendData[inDataCnt], 0x00, (inLen/2)+1);
    byField_35_ON = TRUE;
#else
    wub_str_2_hex(&szTrack2Data[0], (char *)&uszSendData[inDataCnt], inLen);
#endif
    inDataCnt += (inLen / 2);
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc35 END**");
	
    DebugAddHEX("inPackIsoFunc35-end", szTrack2Data, 100);

    		//CTOS_PrinterPutString("inPackIsoFunc35.2");
#if 0
        if(inCTOSS_CheckBitmapSetBit(35) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 35:", uszSendData, inDataCnt);
        	}
#endif	
    return (inDataCnt);
}

int inPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc37 START**");
     unsigned char szDateandTime[12+1];
    inDataCnt = 0;

    if((srTransRec.byTransType!=DCC_RATE_REQUEST) && (srTransRec.byTransType!=DCC_MERCH_RATE_REQ))
         memcpy((char *)&uszSendData[inDataCnt], srTransPara->szRRN, 12);
    else
    {
         GetDateAndTime(szDateandTime);
         memcpy((char *)&uszSendData[inDataCnt], szDateandTime, 12);
    }

    inDataCnt += 12;
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc37 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(37) == 1){
    	CTOS_PrinterPutString("inPackIsoFunc37");

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 37:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}

int inPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc38 START**");

    inDataCnt = 0;
   if (strlen(srTransPara->szAuthCode) > 0)	
	    memcpy((char *)&uszSendData[inDataCnt], srTransPara->szAuthCode, 6);
	else
		memcpy((char *)&uszSendData[inDataCnt], "\x20\x20\x20\x20\x20\x20", 6);	

    inDataCnt += 6;
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc38 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(38) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 38:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}

int inPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc39 START**");
    inDataCnt = 0;
    memcpy((char *)&uszSendData[inDataCnt], srTransPara->szRespCode, 2);
    
    inDataCnt += 2;
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc39 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(39) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 39:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}

int inPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    BYTE szTID[TERMINAL_ID_BYTES+1];

    inDataCnt = 0;    

    memset(szTID, 0x00, sizeof(szTID));
    memset(szTID, 0x20, TERMINAL_ID_BYTES);
    memcpy(szTID, srTransPara->szTID, strlen(srTransPara->szTID));
    memcpy((char *)&uszSendData[inDataCnt], szTID, TERMINAL_ID_BYTES);
    inDataCnt += TERMINAL_ID_BYTES;
    vdDebug_LogPrintf(" TID[%s] PACK_LEN[%d]",szTID, inDataCnt);

#if 0
        if(inCTOSS_CheckBitmapSetBit(41) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 41:", uszSendData, inDataCnt);
        	}
#endif
    return inDataCnt;
}

int inPackIsoFunc42(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    BYTE szMID[MERCHANT_ID_BYTES+1];
    
    inDataCnt = 0;

    memset(szMID, 0x00, sizeof(szMID));
    memset(szMID, 0x20, MERCHANT_ID_BYTES);
    memcpy(szMID, srTransPara->szMID, strlen(srTransPara->szMID));
    memcpy((char *)&uszSendData[inDataCnt], szMID, MERCHANT_ID_BYTES);
    inDataCnt += MERCHANT_ID_BYTES;
    vdDebug_LogPrintf(" MID[%s] PACK_LEN[%d]",szMID, inDataCnt);

#if 0
        if(inCTOSS_CheckBitmapSetBit(42) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 42:", uszSendData, inDataCnt);
        	}
#endif
    return inDataCnt;
}

int inPackIsoFunc45(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{

    vdMyEZLib_LogPrintf("**inPackIsoFunc45 START**");
    inDataCnt = 0;
    uszSendData[inDataCnt++] = (srTransPara->usTrack1Len%100)/10*16+
                               (srTransPara->usTrack1Len%100)%10;
    vdMyEZLib_LogPrintf("  45Len%02x",uszSendData[0]);
#ifdef TLE
    memset((char *)&uszSendData[inDataCnt], 0x00, srTransPara->usTrack1Len);
    byField_45_ON = TRUE;
#else
    memcpy((char *)&uszSendData[inDataCnt], srTransPara->szTrack1Data, srTransPara->usTrack1Len);
#endif
    inDataCnt += srTransPara->usTrack1Len;
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc45 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(45) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 45:", uszSendData, inDataCnt);
        	}
#endif
    return inDataCnt;
}

int inPackIsoFunc48(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    int inCVV2Len;
    BYTE szBuf[20];
    vdMyEZLib_LogPrintf("**inPackIsoFunc48 START**");
    inDataCnt = 0;
        
#ifdef TLE
    memset(&uszSendData[inDataCnt], 0x00, 2);
    byField_48_ON = TRUE;
    inDataCnt += 2;
#else
    DebugAddSTR("inPackIsoFunc48", srTransPara->szCVV2, 4);

if ((srTransRec.byTransType!=DCC_RATE_REQUEST) && (srTransRec.byTransType!=DCC_MERCH_RATE_REQ))
{
     inCVV2Len = strlen(srTransPara->szCVV2);
     
     DebugAddINT("LEN", inCVV2Len);
     memset(szBuf, 0x00, sizeof(szBuf));
     sprintf(szBuf, "%04d", inCVV2Len);
     DebugAddSTR("inPackIsoFunc48", szBuf, 4);
     wub_str_2_hex(szBuf, (char *)&uszSendData[inDataCnt], 4);
     inDataCnt += 2;
     memcpy((char *)&uszSendData[inDataCnt], srTransPara->szCVV2, inCVV2Len);
     inDataCnt += inCVV2Len;
}
	
else
{
     inCVV2Len = 3;
     memset(szBuf, 0x00, sizeof(szBuf));
     sprintf(szBuf, "%04d", inCVV2Len);
     DebugAddSTR("inPackIsoFunc48", szBuf, 4);
     wub_str_2_hex(szBuf, (char *)&uszSendData[inDataCnt], 4);
     inDataCnt += 2;
     memcpy((char *)&uszSendData[inDataCnt], "\x4d\x43\x43", inCVV2Len);
     inDataCnt += inCVV2Len;
   vdDebug_LogPrintf("AAA uszSendData[%02x %02x %02x %02x %02x]", (char*)uszSendData[0],(char*)uszSendData[1],(char*)uszSendData[2],(char*)uszSendData[3],(char*)uszSendData[4]);
}
    
#endif

#if 0
        if(inCTOSS_CheckBitmapSetBit(48) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 48:", uszSendData, inDataCnt);
        	}
#endif
     return inDataCnt;
}

// for all RTR transaction. send de49 curr code - 05262015
int inPackIsoFunc49(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    int inCVV2Len;
    BYTE szBuf[20];
	char szCatgCode[4+1];
	
    vdMyEZLib_LogPrintf("**inPackIsoFunc48 START**");
    inDataCnt = 0;

        inTCTRead(1);	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

	memset(szCatgCode,0x00,sizeof(szCatgCode));
	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode)); // with format "0608"

        //memcpy((char *)&uszSendData[inDataCnt], strCST.szCurCode, 3);
        //wub_str_2_hex("0608", (char *)&uszSendData[inDataCnt], 4); -- working
        wub_str_2_hex(szCatgCode, (char *)&uszSendData[inDataCnt], 4);

    inDataCnt += 2;

#if 0
        if(inCTOSS_CheckBitmapSetBit(49) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 49:", uszSendData, inDataCnt);
        	}
#endif
     return inDataCnt;
}



int inPackIsoFunc50(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
     int inCVV2Len;
     BYTE szBuf[20+1];
     unsigned char szCurrencyCode[4+1];
	 BYTE szCurrencyCode2[4+1];
     int inCurrencyCode;
     
     vdMyEZLib_LogPrintf("**AAA inPackIsoFunc50 START**");

	 vdDebug_LogPrintf("AAA srTransRec.stEMVinfo.9F42[%02x %02x]", srTransRec.stEMVinfo.T9F42[0], srTransRec.stEMVinfo.T9F42[1]);
     inDataCnt = 0;
	 sprintf(szCurrencyCode,"%02x%02x",srTransRec.stEMVinfo.T9F42[0],srTransRec.stEMVinfo.T9F42[1]);
	
	 wub_hex_2_str(srTransRec.stEMVinfo.T9F42, szBuf, 4);
	
	 vdPCIDebug_HexPrintf("AAA szBuf",szBuf,4);
	 memcpy(szCurrencyCode2, &szBuf[1], 3);
     memcpy((char *)uszSendData, szCurrencyCode2, 3);
     inDataCnt += 3;
     
     return inDataCnt;
}





int inPackIsoFunc52(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    char szTempPin[17];
	CTOS_KMS2PINGET_PARA stPinGetPara;
	
    vdMyEZLib_LogPrintf("**inPackIsoFunc52 START**");
	vdDebug_LogPrintf("**inPackIsoFunc52 | szPINBlock[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", (unsigned char)srTransRec.szPINBlock[0], (unsigned char)srTransRec.szPINBlock[1], (unsigned char)srTransRec.szPINBlock[2], (unsigned char)srTransRec.szPINBlock[3], (unsigned char)srTransRec.szPINBlock[4], (unsigned char)srTransRec.szPINBlock[5], (unsigned char)srTransRec.szPINBlock[6], (unsigned char)srTransRec.szPINBlock[7]);
    inDataCnt = 0;
    
    //wub_str_2_hex((BYTE *)&uszSendData[inDataCnt], szTempPin, 16);
    memcpy((char *)uszSendData, srTransPara->szPINBlock, 8);

    
    inDataCnt += 8;
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc52 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(52) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 52:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}



int inPackIsoFunc54(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    int inPacketCnt = 0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[20 + 1];
    char szBaseAmt[20 + 1];
    char szVoidTotalAmt[20 + 1];

    inDataCnt = 0;
    DebugAddHEX("inPackIsoFunc54",srTransPara->szTipAmount,6);
    
    memset(szPacket, 0x00, sizeof(szPacket));
    
     //Should be Online void the Intial SALE amount.
    if(srTransPara->byTransType == VOID)
    {
        //use szStoreID to store how much amount fill up in DE4 for VOID
        memset(szBaseAmt, 0x00, sizeof(szBaseAmt));
        memset(szVoidTotalAmt, 0x00, sizeof(szVoidTotalAmt));
        
        wub_hex_2_str(srTransPara->szBaseAmount, szBaseAmt, 6);
        wub_hex_2_str(srTransPara->szStoreID, szVoidTotalAmt, 6);
        sprintf(szPacket, "%012ld", atol(szVoidTotalAmt) - atol(szBaseAmt));
    }
    else
    {
        wub_hex_2_str(srTransPara->szTipAmount, szPacket, 6);
    }

    inPacketCnt = strlen(szPacket);
    memset(szAscBuf, 0x00, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
	inDataCnt += 2;

    DebugAddHEX("inPackIsoFunc54", szBcdBuf, 2);
    
	/* Packet Data */
	memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
	inDataCnt += inPacketCnt;

    DebugAddSTR("inPackIsoFunc54", szPacket, 12);
	
	vdMyEZLib_LogPrintf("**inPackIsoFunc54 START**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(54) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 54:", uszSendData, inDataCnt);
        	}
	#endif
	return inDataCnt;
}


int inPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc55 START**");

	vdDebug_LogPrintf("srTransPara->bWaveSID[%d]", srTransPara->bWaveSID);
	
    inDataCnt = 0;
    
    if (srTransPara->byEntryMode == CARD_ENTRY_ICC)
        inDataCnt = inPackISOEMVData(srTransPara, uszSendData);
    if (srTransPara->byEntryMode == CARD_ENTRY_WAVE)
    {
        if (srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP)
            inDataCnt = inPackISOPayPassData(srTransPara, uszSendData);
        
        if (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_2 ||
        srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC )
            inDataCnt = inPackISOPayWaveData(srTransPara, uszSendData);
        
        //if ((srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_2) ||
        //(srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_QVSDC))	
        
        if ((srTransPara->bWaveSID == 0x63) ||
        (srTransPara->bWaveSID == 0x65))
            inDataCnt = inPackISOJCBCtlsData(srTransPara, uszSendData);
        
        /* CTLS: AMEX ExpressPay 3.0 requirement - start -- jzg */
        if (srTransPara->bWaveSID == d_VW_SID_AE_EMV)
            inDataCnt = inPackISOExpressPayData(srTransPara, uszSendData);
        /* CTLS: AMEX ExpressPay 3.0 requirement - end -- jzg */
        
        if (srTransPara->bWaveSID == d_VW_SID_CUP_EMV)
            inDataCnt = inPackISOQuickpassData(srTransPara, uszSendData);
    }

    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc55 END**");
#if 0
    if(inCTOSS_CheckBitmapSetBit(55) == 1)
    {
        if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
             vdMyEZLib_HexDump("FIELD 55:", uszSendData, inDataCnt);
    }
#endif
    return inDataCnt;
}

int inPackIsoFunc56(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoFunc56 START**");
    inDataCnt = 0;
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc56 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(56) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 56:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}

int inPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    int	           in_FieldLen = 0, inPacketCnt = 0, inPacketLen=0;
    BYTE           szAscBuf[4 + 1], szBcdBuf[2 + 1];
    BYTE           szPacket[512 + 1];
    BYTE           szPacketASC[512 + 1];
    unsigned short usLen, usRetVal;
    BYTE           btTrack2[20];
    int            inBitMapIndex;
    BYTE           szDataBuf[255], szTrack2Data[40], szTempPan[19];
    short   i,in_Field35Len=0, in_Field02Len=0;
    char szAscBuf1[4 + 1];
    
        
    inTLERead(1);
    vdMyEZLib_LogPrintf("**inPackIsoFunc57 START**");
    memset(szPacket, 0, sizeof(szPacket));
    inDataCnt = 0; 

 		if(srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)
 		{
    		inPacketCnt=18;    		   	    
   	    sprintf(szAscBuf, "%04d", inPacketCnt);
		    memset(szBcdBuf, 0, sizeof(szBcdBuf));
		    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);		    
		    inDataCnt += 2;
		    memcpy(&uszSendData[inDataCnt], stTLE.szVERSION, 2);
    		inDataCnt += 2;    		
    		
    		memset(szPacketASC, 0x30, 16);    
    		vdTripleDES_CBC(d_ENCRYPTION, szPacketASC, 16, szDataBuf);;     		
			  memcpy((char *)&uszSendData[inDataCnt], &szDataBuf[0], 16);
		    inDataCnt += 16;
				
				vdMyEZLib_LogPrintff(uszSendData,inDataCnt );
				    
 			  return (inDataCnt);
 		} 	        
    else if(srTransPara->byTransType == SIGN_ON)
    {
    		memcpy(&szPacket[inPacketCnt], stTLE.szVERSION, 2);
    		inPacketCnt = inPacketCnt + 2; 
    
        memcpy(&szPacket[inPacketCnt], stTLE.szTMKRefNum, 8);
        inPacketCnt = inPacketCnt + 8;
        /* Packet Data Length */
		    memset(szAscBuf, 0, sizeof(szAscBuf));
		    sprintf(szAscBuf, "%04d", inPacketCnt);
		    memset(szBcdBuf, 0, sizeof(szBcdBuf));
		    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
		    inDataCnt += 2;
			  memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
		    inDataCnt += inPacketCnt;
    
        return (inDataCnt);
    }
    else
    {
        inBitMapIndex = inPacketCnt;
    
        inPacketCnt = inPacketCnt + 8; // skip bit map
        if (byField_02_ON == CN_TRUE)
        {
	          szPacket[inBitMapIndex+0] = 0x40;
	
						strcpy(szTempPan, srTransPara->szPAN);
						in_Field02Len = strlen(srTransPara->szPAN);
						if (in_Field02Len % 2)
						{	
							in_FieldLen= in_Field02Len + 1;
        			szTempPan[in_FieldLen] = '0';        			
            }
            else            		          
             in_FieldLen= in_Field02Len;
    				
    				vdisoTwoOne(srTransPara->szPAN, in_FieldLen, &szPacket[inPacketCnt]);
            
            vdMyEZLib_LogPrintf("**PAN**");
            vdMyEZLib_LogPrintff(&szPacket[inPacketCnt], in_FieldLen/2);
            inPacketCnt += in_FieldLen/2;
        }

    
        if (byField_14_ON == CN_TRUE)
        {
            szPacket[inBitMapIndex + 1] = szPacket[inBitMapIndex + 1] | 0x04;
        
            memcpy((char *)&szPacket[inPacketCnt], srTransPara->szExpireDate, 2);
            vdMyEZLib_LogPrintf("**Expired**");
            vdMyEZLib_LogPrintff(&szPacket[inPacketCnt], 2);
            inPacketCnt += 2;
        }
    
        if (byField_35_ON == CN_TRUE && srTransPara->byPackType != TC_UPLOAD)
        {
            szPacket[inBitMapIndex + 4] = szPacket[inBitMapIndex + 4] | 0x20;
                        
            strcpy(szTrack2Data, srTransPara->szTrack2Data);

				    for (i = 0; i < strlen(szTrack2Data); i ++)
				    {
				        if (szTrack2Data[i] == '=')
				            szTrack2Data[i] = 'D';
				    }
            in_Field35Len = strlen(szTrack2Data);
    				if (in_Field35Len % 2)
    				{
        				in_FieldLen= in_Field35Len + 1;
        				strcat(szTrack2Data, "0");
    				} 
    				else
    					in_FieldLen=  in_Field35Len; 					       
         		vdisoTwoOne(szTrack2Data, in_FieldLen, &szPacket[inPacketCnt]);
            vdMyEZLib_LogPrintf("**Track 2**");
            vdMyEZLib_LogPrintff(&szPacket[inPacketCnt], in_FieldLen/2);
            inPacketCnt += (in_FieldLen/2);
        }

        if (byField_45_ON == CN_TRUE && srTransPara->byPackType != TC_UPLOAD)
        {
            szPacket[inBitMapIndex + 5] = szPacket[inBitMapIndex + 5] | 0x08;
	
            in_FieldLen = strlen(srTransPara->szTrack1Data);	
        
            memcpy((char *)&szPacket[inPacketCnt], srTransPara->szTrack1Data, in_FieldLen);
            inPacketCnt += in_FieldLen;
        }

        if(strTCT.fCVVEnable)
        {
        if (byField_48_ON == CN_TRUE)
        {
            szPacket[inBitMapIndex + 5] = szPacket[inBitMapIndex + 5] | 0x01;
	    
                        sprintf(szAscBuf1,"%s","%04s");
        		sprintf(szAscBuf,szAscBuf1,(char *)srTransPara->szCVV2);
                        
         		vdisoTwoOne(szAscBuf, 4, szBcdBuf);
        		
        		vdMyEZLib_LogPrintf("**CVV**  %s",srTransPara->szCVV2);        
        		vdMyEZLib_LogPrintff(szBcdBuf,2);
        		
            memcpy((char *)&szPacket[inPacketCnt], szBcdBuf, 2);
            inPacketCnt += 2;
        }
        }
    }
    
    memset(szPacketASC, 0x30, sizeof(szPacketASC));
    vdMyEZLib_LogPrintf("**before**  %d",inPacketCnt);
		vdMyEZLib_LogPrintff(szPacket,inPacketCnt );
		
    vdisoOneTwo(szPacket, szPacketASC, inPacketCnt);        
    inPacketCnt = inPacketCnt*2;    
    
    vdMyEZLib_LogPrintf("**before**  %d",inPacketCnt);
		vdMyEZLib_LogPrintf(szPacketASC,inPacketCnt );
		
    if(inPacketCnt%8!=0)
    {
        szPacketASC[inPacketCnt] = 0x38;
        inPacketCnt = inPacketCnt + (8-(inPacketCnt%8));
    }
    
    memset(szDataBuf, 0, sizeof(szDataBuf));
    vdTripleDES_CBC(d_ENCRYPTION, szPacketASC, inPacketCnt, szDataBuf);
    
	/* Packet Data Length */
	  inPacketLen= inPacketCnt+2;  // add 2 for stTLE.szVERSION
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketLen);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    memcpy(&uszSendData[inDataCnt], stTLE.szVERSION, 2);
    inDataCnt += 2;
    
	/* Packet Data */
    memcpy((char *)&uszSendData[inDataCnt], &szDataBuf[0], inPacketCnt);
    inDataCnt += inPacketCnt;
	
	  vdMyEZLib_LogPrintf("**57 data**");
		vdMyEZLib_LogPrintff(uszSendData,inDataCnt );
    vdMyEZLib_LogPrintf("**inPackIsoFunc57 START**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(57) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 57:", uszSendData, inDataCnt);
        	}
#endif
    return (inDataCnt);
}

int inPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    int inPacketCnt = 0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[100 + 1];
    vdDebug_LogPrintf("**inPackIsoFunc60 byPackType[%d]ulOrgTraceNum[%ld]szMassageType[%02X%02X]**", srTransPara->byPackType, srTransPara->ulOrgTraceNum, srTransPara->szMassageType[0], srTransPara->szMassageType[1]);
    inDataCnt = 0;
    
    memset(szPacket, 0x00, sizeof(szPacket));


   
    if((srTransPara->byPackType == BATCH_UPLOAD) || (srTransPara->byPackType == TC_UPLOAD))
    {   
        /* Load the Original Data Message in field 60 */
        /* Field 60 contains 4 digits of MID, 6 digits of STAN
           and 12 digits of Reserved space set to spaces.
           */
        wub_hex_2_str(srTransPara->szMassageType,szPacket,2);
        inPacketCnt += 4;

        sprintf(&szPacket[inPacketCnt], "%06ld", srTransPara->ulOrgTraceNum);
        inPacketCnt += 6;
        
        memcpy(&szPacket[inPacketCnt],srTransPara->szRRN,RRN_BYTES);
        inPacketCnt += RRN_BYTES;  
    }
    else if(srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)
    {
        //wub_hex_2_str(srTransRec.szBatchNo,szPacket,3);
        wub_hex_2_str(srTransPara->szBatchNo,szPacket,3);
        
        inPacketCnt += 6;
    }
    else
    {
        if(srTransPara->byTransType == SALE_TIP)//#00202 - Incorrect DE 60 amount on tip adjust "2".
        {
            if(srTransPara->byUploaded == FALSE)
                wub_hex_2_str(srTransPara->szOrgAmount,szPacket,6);
	    else
                wub_hex_2_str(srTransPara->szBaseAmount,szPacket,6);				
        }
        else
        {
            if(srTransPara->byUploaded == TRUE)
                wub_hex_2_str(srTransPara->szOrgAmount,szPacket,6);
            else			
                wub_hex_2_str(srTransPara->szBaseAmount,szPacket,6);

			if(srTransPara->byPackType == VOID && srTransPara->byOrgTransType == SALE_OFFLINE && srTransPara->byUploaded == TRUE)
			{
				wub_hex_2_str(srTransPara->szBaseAmount,szPacket,6);
			}
        }
        inPacketCnt += 12;
    }
   
    /* Packet Data Length */
	memset(szAscBuf, 0, sizeof(szAscBuf));
	sprintf(szAscBuf, "%04d", inPacketCnt);
	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
	inDataCnt += 2;
	/* Packet Data */
	memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
	inDataCnt += inPacketCnt;
	
	vdDebug_LogPrintf(". Pack Len(%d)",inDataCnt);
	vdDebug_LogPrintf("**inPackIsoFunc60 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(60) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 60:", uszSendData, inDataCnt);
        	}
	#endif
	return inDataCnt;

}


int inPackIsoFunc61(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
	int inPacketCnt = 0;
    int inResult;
    int inTranCardType;
    ACCUM_REC srAccumRec;
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szTemplate[100], szPacket[128 + 1];
	
#if 1
    vdMyEZLib_LogPrintf("**inPackIsoFunc61 START**");
    inDataCnt = 0;
    uszSendData[inDataCnt++] = 0x00;
    uszSendData[inDataCnt++] = 0x06;
    sprintf((char *)&uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szInvoiceNo,3));
    inDataCnt += 6;
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc61 END**");
#else
	
if(srTransPara->fLoyalty == TRUE)
{
            if(srTransPara->byTransType == VOID || srTransPara->byPackType == BATCH_UPLOAD)
            { 
                memcpy(&szPacket[inPacketCnt], srTransPara->szLoyaltyData, 105); inPacketCnt+=105;
            }
            else if(srTransPara->byTransType == LOY_BAL_INQ)
            {
                memcpy(&szPacket[inPacketCnt],"000000000000", 12); inPacketCnt+=12; //Sale amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"000000000000", 12); inPacketCnt+=12; //Redeem amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"000000000000", 12); inPacketCnt+=12; //Net Sales amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Redeem Points - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Award Points - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Point Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00", 2); inPacketCnt+=2;//Product Code - 2 bytes
                memcpy(&szPacket[inPacketCnt],"0000000000", 10); inPacketCnt+=10; //Beginning Point Balance - 10 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Beginning E-Purse Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11);  inPacketCnt+=11; //E-Purse Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"  ", 2);  inPacketCnt+=2; //Card Block Code, Transaction Type indicators (default both to 0x20)
            }
            else if(srTransPara->byTransType == LOY_REDEEM_5050)
            {
                char szSaleAmount[12+1];
                char szTmp1[12+1];
                long lnTotalAmount=0L;
                
                memset(szTmp1, 0, sizeof(szTmp1));
                wub_hex_2_str(srTransPara->szTotalAmount, szTmp1, 6);
                lnTotalAmount=atol(szTmp1);
                memset(szSaleAmount, 0, sizeof(szSaleAmount));		
                sprintf(szSaleAmount, "%012ld", lnTotalAmount);
                memcpy(&szPacket[inPacketCnt],szSaleAmount, 12); inPacketCnt+=12; //Sale amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"000000000000", 12); inPacketCnt+=12; //Redeem amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"000000000000", 12); inPacketCnt+=12; //Net Sales amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Redeem Points - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Award Points - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Point Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00", 2); inPacketCnt+=2;//Product Code - 2 bytes
                memcpy(&szPacket[inPacketCnt],"0000000000", 10); inPacketCnt+=10; //Beginning Point Balance - 10 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Beginning E-Purse Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11);  inPacketCnt+=11; //E-Purse Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt]," R", 2);  inPacketCnt+=2; //Card Block Code, Transaction Type indicators (default both to 0x20)
            }
            else if(srTransPara->byTransType == LOY_REDEEM_VARIABLE)
            {
                char szTempAmount[12+1];
                char szTmp1[12+1];
                long lnTotalAmount=0L;
                
                memset(szTmp1, 0, sizeof(szTmp1));
                wub_hex_2_str(srTransPara->szTotalAmount, szTmp1, 6);
                lnTotalAmount=atol(szTmp1);
                memset(szTempAmount, 0, sizeof(szTempAmount));		
                sprintf(szTempAmount, "%012ld", lnTotalAmount);
                memcpy(&szPacket[inPacketCnt],szTempAmount, 12); inPacketCnt+=12; //Sale amount - 12 bytes
                lnTotalAmount=atol(srTransPara->szRedeemAmount);
                memset(szTempAmount, 0, sizeof(szTempAmount));		
                sprintf(szTempAmount, "%012ld", lnTotalAmount);
                memcpy(&szPacket[inPacketCnt],szTempAmount, 12); inPacketCnt+=12; //Redeem amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"000000000000", 12); inPacketCnt+=12; //Net Sales amount - 12 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Redeem Points - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Award Points - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Point Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00", 2); inPacketCnt+=2;//Product Code - 2 bytes
                memcpy(&szPacket[inPacketCnt],"0000000000", 10); inPacketCnt+=10; //Beginning Point Balance - 10 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11); inPacketCnt+=11; //Beginning E-Purse Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt],"00000000000", 11);  inPacketCnt+=11; //E-Purse Balance - 11 bytes
                memcpy(&szPacket[inPacketCnt]," R", 2);  inPacketCnt+=2; //Card Block Code, Transaction Type indicators (default both to 0x20)
        	}
}

	/* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
	
    vdDebug_LogPrintf(". Pack Len(%d)",inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc63 END**");
#endif	

#if 0
        if(inCTOSS_CheckBitmapSetBit(61) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 61:", uszSendData, inDataCnt);
        	}
#endif
    return inDataCnt;
}

int inPackIsoInstallment61(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    vdMyEZLib_LogPrintf("**inPackIsoInstallment61 START**");
    char szPacket[100 + 1];
    char szTemp[12+1], szTemp2[12+1];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    int inPacketCnt = 0;
    	
    inDataCnt = 0;

    if(srTransPara->fInstallment == TRUE)
    {
        memset(szPacket, 0, sizeof(szPacket));
        if(srTransPara->byTransType == VOID || srTransPara->byPackType == BATCH_UPLOAD)
        {
    		memcpy(szPacket, srTransPara->szInstallmentData, 81);
            inPacketCnt=81;
        }
        else if(srTransPara->byTransType == SALE)
        {
            /*installment plan*/
            memset(szTemp, 0, sizeof(szTemp));
            sprintf(szTemp, "%03d", atol(srTransPara->szPromo));
            memcpy(szPacket, szTemp, 3); inPacketCnt+=3;
            
            /*installment term*/		
            memset(szTemp, 0, sizeof(szTemp));
            sprintf(szTemp, "%02d", atol(srTransPara->szTerms));
            memcpy(&szPacket[inPacketCnt], szTemp, 2); inPacketCnt+=2;
            
            /*compute method*/
            memcpy(&szPacket[inPacketCnt], "0", 1); inPacketCnt+=1;
            
            /*interest rate - 6 bytes*/ 
            memcpy(&szPacket[inPacketCnt], "000000", 6); inPacketCnt+=6;
            
            /*interest free months - 2 bytes*/ 
            memcpy(&szPacket[inPacketCnt], "00", 2); inPacketCnt+=2;
            
            /*first pay - 9 bytes*/ 
            memcpy(&szPacket[inPacketCnt], "000000000", 9); inPacketCnt+=9;
            
            /*last pay - 9 bytes*/ 
            memcpy(&szPacket[inPacketCnt], "000000000", 9); inPacketCnt+=9;
            
            /*monthly installment amount - 9 bytes*/
            memcpy(&szPacket[inPacketCnt], "000000000", 9); inPacketCnt+=9;
            
            /*total installment amount - 9 bytes*/
            memcpy(&szPacket[inPacketCnt], "000000000", 9); inPacketCnt+=9;
            
            /*gross amount*/ 
            memset(szTemp, 0, sizeof(szTemp));
            memset(szTemp2, 0, sizeof(szTemp2));
            wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
            sprintf(szTemp2, "%09ld", atol(szTemp));
            memcpy(&szPacket[inPacketCnt], szTemp2, 9); inPacketCnt+=9;
            
            /*out interest - 9 bytes*/
            memcpy(&szPacket[inPacketCnt], "000000000", 9); inPacketCnt+=9;
            
            /*handling fee - 9 bytes*/
            memcpy(&szPacket[inPacketCnt], "000000000", 9); inPacketCnt+=9;
            
            /*wave fr mos - 2 bytes*/
            memcpy(&szPacket[inPacketCnt], "00", 2); inPacketCnt+=2;
            
            /*wave fr mos - 2 bytes*/
            memcpy(&szPacket[inPacketCnt], "00", 2); inPacketCnt+=2;
        }
    }
	
	/* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
	
    vdDebug_LogPrintf(". Pack Len(%d)",inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc61 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(61) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 61:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}


int inPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
	  char szPacket[7];

      DebugAddHEX("inPackIsoFunc62", srTransPara->szInvoiceNo, 3);

    inDataCnt = 0;
    uszSendData[inDataCnt++] = 0x00;
    uszSendData[inDataCnt++] = 0x06;
    if(srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)
    {
    	memcpy((char *)&uszSendData[inDataCnt],"000000",6);
    }
    else
   	{   		
   		sprintf((char *)&uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szInvoiceNo,3));   		
   	}
    inDataCnt += 6;
    
    vdMyEZLib_LogPrintf("  PACK_LEN%d",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc62 END**");

#if 0
        if(inCTOSS_CheckBitmapSetBit(62) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0)
	vdMyEZLib_HexDump("FIELD 62:", uszSendData, inDataCnt);
        	}
	#endif
    return inDataCnt;
}

#if 1
int inPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszSendData)
{
     
	int inPacketCnt = 0;
	int inResult;
	int inTranCardType;
	ACCUM_REC srAccumRec;
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szTemplate[100], szPacket[128 + 1];
	char szTermSerialNum[25+1];
	char szTempTermSerialNum[25+1];
	char szTemplate1[100];
	char szPacket1[4+1];
	char szBcdBuf1[4 + 1];	
	int inLen=0;		
	char szTemp[4+1], szTemp2[12+1];
	char szCatgCode[3+1];
	BYTE szKSN[25+1];
	int inKSNPadLen=0, inKSNLen=20;

	BYTE szForeignAmountfld63[12+1];
	BYTE szExchangeRate[12+1];
	BYTE szSendExchangeRate[12+1];
	char * ptr;
	char szdecimalplace[1+1];
	int indecimalplace;
	int indel;
	BOOL fOnlinePINSupport = FALSE;
	
    	//USHORT usTotalSaleCount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount;
    	//ULONG  ulTotalSaleAmount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount;

	
	memset(szPacket, 0, sizeof(szPacket));
	memset(szKSN, 0x00, sizeof(szKSN));
	memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
	memset(szTempTermSerialNum, 0x00, sizeof(szTempTermSerialNum));
	
	inDataCnt = 0;

	vdDebug_LogPrintf("--inPackIsoFunc63--");

// fix for Wrong implementation of USD Currency
//USD currency prompts upon voiding & settlement
//Incorrect Currecy code sent on 0500 DE 63 using USD Profile
        inTCTRead(1);	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

	vdDebug_LogPrintf("srTransPara->byTransType=[%d]", srTransPara->byTransType);
	vdDebug_LogPrintf("srTransPara->fDebit=[%d]", srTransPara->fDebit);
	vdDebug_LogPrintf("srTransPara->fEMVPIN=[%d]", srTransPara->fEMVPIN);
	vdDebug_LogPrintf("srTransPara->fCUPPINEntry=[%d]", srTransPara->fCUPPINEntry);
	vdDebug_LogPrintf("srTransPara->fEMVPINEntered=[%d]", srTransPara->fEMVPINEntered);
	vdDebug_LogPrintf("strTCT.fTSNEnable=[%d]", strTCT.fTSNEnable);
	vdDebug_LogPrintf("srTransPara->fInstallment=[%d]", srTransPara->fInstallment);
	vdDebug_LogPrintf("srTransPara->fCash2Go=[%d]", srTransPara->fCash2Go);
	vdDebug_LogPrintf("strTCT.fDCCTerminal=[%d]", strTCT.fDCCTerminal);
	vdDebug_LogPrintf("strTCT.fRegUSD=[%d]", strTCT.fRegUSD);
	vdDebug_LogPrintf("strCDT.fDCCEnable=[%d]", strCDT.fDCCEnable);
	vdDebug_LogPrintf("srTransPara->fDCCOptin=[%d]", srTransPara->fDCCOptin);
	vdDebug_LogPrintf("srTransPara->IITid=[%d], srTransPara->HDTid=[%d]", srTransPara->IITid, srTransPara->HDTid);
	vdDebug_LogPrintf("srTransPara->byEntryMode=[%d]", srTransPara->byEntryMode);
	vdDebug_LogPrintf("srTransPara->byOffline [%d]", srTransPara->byOffline);
	vdDebug_LogPrintf("srTransPara->byOrgTransType [%d]", srTransPara->byOrgTransType);
	vdDebug_LogPrintf("srTransPara->byPackType [%d]", srTransPara->byPackType);
	vdDebug_LogPrintf("srTransPara->byUploaded [%d]", srTransPara->byUploaded);
	vdDebug_LogPrintf("srTransPara->fDCCEnable [%d]", srTransPara->fDCCEnable);
	

	if ((srTransPara->fDebit == TRUE) || (srTransPara->fEMVPIN == TRUE) || (srTransPara->fCUPPINEntry == TRUE))
	{
		fOnlinePINSupport = TRUE;
	}
	
	vdDebug_LogPrintf("fOnlinePINSupport=[%d]", fOnlinePINSupport);
																					//fix issue:Installment data should be present on DE 63 on 0320 message
    if ((srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH) && srTransPara->byPackType != BATCH_UPLOAD)  // || srTransPara->byPackType == BATCH_UPLOAD)
    {
		        memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
		        if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
		        {
		            vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		            return ST_ERROR;	
		        }		
		        vdDebug_LogPrintf("Test2222");
		        //0 is for Credit type, 1 is for debit type
		        inTranCardType = 0;
		        vdDebug_LogPrintf("**inPackIsoFunc63 START**byTransType[%d]Sale[%d]Refund[%d]", srTransPara->byTransType,srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);

				// =======================================================
				// Count // Amount
				// =======================================================
				vdDebug_LogPrintf("usSaleCount[%d]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
				vdDebug_LogPrintf("usVoidSaleCount[%d]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount);
				vdDebug_LogPrintf("usDCCCount[%d]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usDCCCount);
				vdDebug_LogPrintf("usVoidSaleDCCCount[%d]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleDCCCount);

				vdDebug_LogPrintf("ulSaleTotalAmount[%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
				vdDebug_LogPrintf("ulVoidSaleTotalAmount[%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);
				vdDebug_LogPrintf("ulSaleDCCTotalAmount[%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleDCCTotalAmount);
				vdDebug_LogPrintf("ulVoidSaleTotalDCCAmount[%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalDCCAmount);				
					
		        if((srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount) == 0 
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount) == 0					
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount) == 0										
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount) == 0)
					
		        {

		//	CTOS_PrinterPutString("inPackIsoFunc63");
		        
		            //strcpy(szPacket, "000000000000000");
		            //strcat(szPacket, "000000000000000");

		                memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 1st set			
		                memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 2nd set
		                memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 3rd set
		                memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 4th set
		               
		                
				sprintf(szCatgCode, "%03d", atoi(strCST.szCurCode));
		                memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code1 set
		                memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code2 set
					
		            //strcat(szPacket, "000");
		            //inPacketCnt += 30;
		            
			
        		}
			else
        		{    	
				//	CTOS_PrinterPutString("inPackIsoFunc63.1");

				//============================
				// Send data required for installment de63
				//============================
#if 0	
				inLen = 0;		
				inLen+=126;

				/*message lenght*/	
				memset(szCatgCode, 0, sizeof(szCatgCode)); // curr code

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4);
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; 
#endif	   
					/* SALE - Credit, Redeem, Installment sale*/
					memset(szTemplate, 0x00, sizeof(szTemplate));
					sprintf(szTemplate, "%03d", 
					srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount				
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount								
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount
					+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount);
					memcpy(&szPacket[inPacketCnt], szTemplate, 3); inPacketCnt+=3; 

				sprintf(szTemplate, "%012ld", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount				
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount								
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount);  
				memcpy(&szPacket[inPacketCnt], szTemplate, 12); inPacketCnt += 12;

				/* REFUND */
				memset(szTemplate, 0x00, sizeof(szTemplate));
				sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
				memcpy(&szPacket[inPacketCnt], szTemplate, 3); inPacketCnt += 3;			

				sprintf(szTemplate, "%012ld", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount));
				memcpy(&szPacket[inPacketCnt], szTemplate, 12); inPacketCnt += 12;            


				// memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 2nd set
				// memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 3rd set
				// memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 4th set

				memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 2nd set
				memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 3rd set
				memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 4th set


				sprintf(szCatgCode, "%03d", atoi(strCST.szCurCode));
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code1 set
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code2 set 
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code3 set 		fix for issue: DE 63 reconciliation total missing fourth currency code (settlement 0500)		
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code3 set 		fix for issue: DE 63 reconciliation total missing fourth currency code (settlement 0500)		
				
        		}

    }
    else
    {
    	vdDebug_LogPrintf("DE63: SETTLE / CLS_BATCH");
		
    	if (srTransPara->fInstallment == TRUE || srTransPara->fCash2Go == TRUE)
    	{    				
            if (srTransPara->byTransType == SALE || srTransPara->byTransType == VOID || srTransPara->byPackType == BATCH_UPLOAD)
			{
				inLenTermSerialNo = 0;
				inLen=0;
				inLen+=36;

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4);

				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*message lenght*/	
				memcpy(&szPacket[inPacketCnt], "90", 2); inPacketCnt+=2; /*Installment - table id - "90"*/  

				//memset(szTemp, 0, sizeof(szTemp));
				//sprintf(szTemp, "%04d", atol(srTransPara->szTerms));
				sprintf(szTemp, "%04d", atol(srTransPara->szTerms1));                
				memcpy(&szPacket[inPacketCnt], szTemp, 4); inPacketCnt+=4; /*Loan Period n4 - 4*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szOffsetPeriodEX, 4); inPacketCnt+=4; /*Offset Period an4 - 4*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szFundIndicator, 1); inPacketCnt+=1; /*Funding Indicator an1 - 1*/ /*I: Issuer funded (default) A: Acquirer funded*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szPlanID1, 10); inPacketCnt+=10; /*Plan ID an10 - 10*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szPlanGroup, 10); inPacketCnt+=10; /*Plan Group an10 - 10*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szPlanGroupIDX, 5); inPacketCnt+=5;/*Plan Group Index n5 - 5*/

				inLen=0;			        
	        	memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
				memset(szTempTermSerialNum, 0x00, sizeof(szTempTermSerialNum)); 

				//inCTOSS_GetEnvDB("SERNUM", szTempTermSerialNum);
				inGetSERNUM(szTempTermSerialNum);
				vdDebug_LogPrintf("2.Len[%d],szTempTermSerialNum[%s]", strlen(szTempTermSerialNum), szTempTermSerialNum);
				
				if (strlen(szTempTermSerialNum) > 0)
				{
					strcpy(szTermSerialNum, szTempTermSerialNum);
					inLenTermSerialNo=strlen(szTermSerialNum);
				}
				else
				{
	                                     //CTOS_GetFactorySN(szTermSerialNum);
	                                      usGetSerialNumber(szTermSerialNum); 
										 inLenTermSerialNo=strlen(szTermSerialNum);
					//inLenTermSerialNo=strlen(szTermSerialNum)-1;
				}

				vdDebug_LogPrintf("2.Len[%d],szTermSerialNum[%s]", strlen(szTermSerialNum), szTermSerialNum);
				
				//szTermSerialNum[15]=0;					        	
				vdDebug_LogPrintf("2.inLenTermSerialNo[%d]", inLenTermSerialNo);
	        
	        	inLen+=inLenTermSerialNo+2;
	        	//if(srTransRec.fRegUSD == 1) // append USD on terminal serial number registration.
	        	if(srTransPara->fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.	        	
		        	inLen+=3;
	        
	        	memset(szAscBuf, 0, sizeof(szAscBuf));
	        	sprintf(szAscBuf, "%04d", inLen);
	        	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	        	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	        
	        	memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
				inPacketCnt+=2; /*Terminal Serial Number Length*/  
		        	memcpy(&szPacket[inPacketCnt], "18", 2); 
				inPacketCnt+=2; /*Terminal Serial Number - table id - "18"*/  
	        
	        	if(srTransPara->fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.
	        	{
		        	memcpy(&szPacket[inPacketCnt], "USD", 3); 
		        	inPacketCnt+=3;
	        	}
	        
	        	memcpy(&szPacket[inPacketCnt], szTermSerialNum, inLenTermSerialNo); 
				inPacketCnt+=inLenTermSerialNo; /*Terminal Serial Number*/
				
                 #if 1         
                 vdDebug_LogPrintf("+++++++++++++++++++++:[%d]:[%d]",srTransRec.CDTid2, strCDT.inCVV_II);
                 
                 //if(strlen(srTransPara->szCVV2) > 0)
                 if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_MANUAL)
                 {
                 
                      //CTOS_PrinterPutString("@@@@@@");
                      
                      vdDebug_LogPrintf("AAA - Manual endtry and cvv2 manual");
                      
                      // send szCVV2					
                      inLen=0;		
                      inLen+=8; // szCVV2 message lenght
                      
                      //memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);
                      
                      memset(szAscBuf, 0, sizeof(szAscBuf));
                      sprintf(szAscBuf, "%04d", inLen);
                      memset(szBcdBuf, 0, sizeof(szBcdBuf));
                      wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
                      
                      
                      if(strlen(srTransPara->szCVV2) < 4)
                      {
                           BYTE szAscBuf1[4 + 1];
                           BYTE szPos[12+1];
                           
                           memset(szAscBuf1, 0, sizeof(szAscBuf1));
                           memset(szPos, 0X00, sizeof(szPos));
                           
                           // right - justify and space fill the entry.
                           // ex: 123 - "31 32 33 20 20 20"
                           //sprintf(szAscBuf1, "%d%20x", atol(srTransRec.szCVV2)); 
                           
                           
                           // #00234 - Incorrect CVV format DE 63
                           
                           strcpy(szPos, "\x31\x30");				
                           vdCTOS_Pad_String(srTransPara->szCVV2, 4, ' ', POSITION_LEFT);	// ex: 123 - "20 31 32 33"
                           strcat(szPos, srTransPara->szCVV2);
                           
                           memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/	
                           memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
                           //memcpy(&szPacket[inPacketCnt], szAscBuf1, 6); inPacketCnt+=6; /*szCVV2*/				
                           memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
                      
                      }
                      else  //already tested working - send 31 30 31 32 33 34
                      {
                           //char szAscBuf1[4 + 1];
                           BYTE szPos[12+1];
                           
                           //memset(szAscBuf1, 0, sizeof(szAscBuf1));
                           memset(szPos, 0X00, sizeof(szPos));
                           
                           strcpy(szPos, "\x31\x30");
                           //sprintf(szAscBuf1, "%d", srTransPara->szCVV2);				
                           strcat(szPos, srTransPara->szCVV2);
                           
                           memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/	
                           memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/				
                           //memcpy(&szPacket[inPacketCnt], srTransRec.szCVV2, 6); inPacketCnt+=6; /*szCVV2*/
                           memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
                      }
                 
                 }
                 
                 if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_NONE)
                 {
                 BYTE szAscBuf1[4 + 1];
                 BYTE szPos[12+1];
                 
                 // send szCVV2					
                 inLen=0;		
                 inLen+=8; // szCVV2 message lenght
                 
                 //memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);
                 
                 //CTOS_PrinterPutString("&&&&&&&&&&&&&&&&&&&&");
                 
                 
                 memset(szAscBuf, 0, sizeof(szAscBuf));
                 sprintf(szAscBuf, "%04d", inLen);
                 memset(szBcdBuf, 0, sizeof(szBcdBuf));
                 wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
                 
                 memset(szAscBuf1, 0, sizeof(szAscBuf1));
                 memset(szPos, 0X00, sizeof(szPos));
                 
                 // #00234 - Incorrect CVV format DE 63
                 
                 strcpy(szPos, "\x30\x30\x20\x20\x20\x20");				
                 memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/	
                 memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
                 memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
                 }
                 #endif



				 //add originators reason code (for visa only)
#if 0
				 if (srTransPara->byTransType != SETTLE && srTransPara->byTransType != CLS_BATCH && srTransPara->IITid == VISA_ISSUER) {
				 
					 memset(szAscBuf, 0, sizeof(szAscBuf));
					 sprintf(szAscBuf, "%04d", 4);
					 memset(szBcdBuf, 0, sizeof(szBcdBuf));
					 wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
					 
					 memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; 
					 
					 memcpy(&szPacket[inPacketCnt], "20", 2); inPacketCnt+=2; //table id - "20" 
					 memcpy(&szPacket[inPacketCnt], "11", 2); inPacketCnt+=2;
				 }
#endif

				/* Packet Data Length */
			    memset(szAscBuf, 0, sizeof(szAscBuf));
			    sprintf(szAscBuf, "%04d", inPacketCnt);
			    memset(szBcdBuf, 0, sizeof(szBcdBuf));
			    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
			    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
			    inDataCnt += 2;
			    /* Packet Data */
			    memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
			    inDataCnt += inPacketCnt;

				return inDataCnt;
			}
            //if(srTransPara->byPackType == BATCH_UPLOAD) //            //
            if(srTransPara->byTransType == VOID || srTransPara->byPackType == BATCH_UPLOAD)
			{
				inLen=0;
				inLen+=36;

				//compliance
				//add table 20 size	
				//if (srTransPara->IITid == VISA_ISSUER) 
				//	inLen=+4;

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4);

				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*message lenght*/	
				memcpy(&szPacket[inPacketCnt], "90", 2); inPacketCnt+=2; /*Installment - table id - "90"*/  

				//memset(szTemp, 0, sizeof(szTemp));
				sprintf(szTemp, "%04d", atol(srTransPara->szTerms1));
				memcpy(&szPacket[inPacketCnt], szTemp, 4); inPacketCnt+=4; /*Loan Period n4 - 4*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szOffsetPeriodEX, 4); inPacketCnt+=4; /*Offset Period an4 - 4*/				
				memcpy(&szPacket[inPacketCnt], srTransPara->szFundIndicator, 1); inPacketCnt+=1; /*Funding Indicator an1 - 1*/ /*I: Issuer funded (default) A: Acquirer funded*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szPlanID1, 10); inPacketCnt+=10; /*Plan ID an10 - 10*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szPlanGroup, 10); inPacketCnt+=10; /*Plan Group an10 - 10*/
				memcpy(&szPacket[inPacketCnt], srTransPara->szPlanGroupIDX, 5); inPacketCnt+=5;/*Plan Group Index n5 - 5*/

				//add originators reason code (for visa only)
#if 0
				 if (srTransPara->IITid == VISA_ISSUER) {
				 
					 memset(szAscBuf, 0, sizeof(szAscBuf));
					 sprintf(szAscBuf, "%04d", 4);
					 memset(szBcdBuf, 0, sizeof(szBcdBuf));
					 wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
					 
					 memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; 
					 
					 memcpy(&szPacket[inPacketCnt], "20", 2); inPacketCnt+=2; //table id - "20" 
					 memcpy(&szPacket[inPacketCnt], "11", 2); inPacketCnt+=2;
				 }
#endif

			}
				
	        
    	}
		else
		{
			inLenTermSerialNo = 0;
			inPacketCnt = 0;
			if (strTCT.fTSNEnable == TRUE)
			{
				// Send TSN (18)
				inLen=0;		

				memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
				memset(szTempTermSerialNum, 0x00, sizeof(szTempTermSerialNum)); 

				//inCTOSS_GetEnvDB("SERNUM", szTempTermSerialNum);
				inGetSERNUM(szTempTermSerialNum);
				
				vdDebug_LogPrintf("2.Len[%d],szTempTermSerialNum[%s]", strlen(szTempTermSerialNum), szTempTermSerialNum);

				if (strlen(szTempTermSerialNum) > 0)
				{
					strcpy(szTermSerialNum, szTempTermSerialNum);
					inLenTermSerialNo=strlen(szTermSerialNum);
				}
				else
				{
					//CTOS_GetFactorySN(szTermSerialNum);
					usGetSerialNumber(szTermSerialNum); 
					//inLenTermSerialNo=strlen(szTermSerialNum)-1;
					inLenTermSerialNo=strlen(szTermSerialNum);
				}

				vdDebug_LogPrintf("2.Len[%d],szTermSerialNum[%s]", strlen(szTermSerialNum), szTermSerialNum);

				//szTermSerialNum[15]=0;					        	
				vdDebug_LogPrintf("2.inLenTermSerialNo[%d]", inLenTermSerialNo);

				inLen+=inLenTermSerialNo+2;
				//if(srTransRec.fRegUSD == 1) // append USD on terminal serial number registration.
				if(srTransRec.fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.	        	
				inLen+=3;

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4);

				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
				inPacketCnt+=2; /*Terminal Serial Number Length*/  
				memcpy(&szPacket[inPacketCnt], "18", 2); 
				inPacketCnt+=2; /*Terminal Serial Number - table id - "18"*/  

				if(srTransRec.fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.
				{
					memcpy(&szPacket[inPacketCnt], "USD", 3); 
					inPacketCnt+=3;
				}

				memcpy(&szPacket[inPacketCnt], szTermSerialNum, inLenTermSerialNo); 
				inPacketCnt+=inLenTermSerialNo; /*Terminal Serial Number*/ 

				//(srTransPara->IITid == VISA_ISSUER || srTransPara->IITid == MASTERCARD_ISSUER || srTransPara->IITid == AMEX_ISSUER 
					//|| srTransPara->IITid == JCB_ISSUER || srTransPara->IITid == UPI_ISSUER))

				if((srTransPara->byPackType == BATCH_UPLOAD || srTransPara->byPackType == SEND_ADVICE))
				{
                    if(srTransPara->byEntryMode == CARD_ENTRY_ICC || srTransPara->byEntryMode == CARD_ENTRY_WAVE)
                    {
						inIITRead(srTransPara->IITid);
						vdDebug_LogPrintf("strIIT.fTag20[%d]", strIIT.fTag20);
						if(strIIT.fTag20 == VS_TRUE)
						{
                            memset(szAscBuf, 0, sizeof(szAscBuf));
                            sprintf(szAscBuf, "%04d", 4);
                            memset(szBcdBuf, 0, sizeof(szBcdBuf));
                            wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set length
                            memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; 
        					 
                            memcpy(&szPacket[inPacketCnt], "20", 2); 
                            inPacketCnt+=2;
                            memcpy(&szPacket[inPacketCnt], "11", 2); 
                            inPacketCnt+=2;
						}
                    }
				}
				
				// Send KSN(33)
				if (fOnlinePINSupport == TRUE)
				{					
					memset(szKSN, 0x00, sizeof(szKSN));					
					//wub_hex_2_str(srTransRec.szKSN, szKSN, 20); // convert to ascii
					
					vdDebug_LogPrintf("srTransPara->szKSN=(%s)", srTransPara->szKSN);
					strcpy(szKSN, srTransPara->szKSN);
					
					if (srTransPara->byTransType == VOID && srTransPara->IITid == 8)
					{
						memset(szKSN, 0x00, sizeof(szKSN));							
					}
										
					if (strlen(szKSN) > 0)
					{
						inKSNPadLen = 0;
			            if(szKSN[0] == 'F') 
			            {
			                vdDebug_LogPrintf("szKSN=(%s)", szKSN);
			                
			                while(szKSN[inKSNPadLen] == 'F')
			                    inKSNPadLen++;
								
			                vdDebug_LogPrintf("inKSNPadLen=(%d)", inKSNPadLen);
			            }

						inKSNLen=inKSNLen-inKSNPadLen;
			            vdDebug_LogPrintf("inLen=(%d)", inLen);
			    				
			            // send KSN					
			            inLen=0;		
			            inLen+=2+inKSNLen;

						vdDebug_LogPrintf("inLen=(%d)", inLen);
						vdDebug_LogPrintf("Send::inKSNLen=[%d], inKSNPadLen=[%d], szKSN=[%s]", inKSNLen, inKSNPadLen, szKSN);
						
			            memset(szAscBuf, 0, sizeof(szAscBuf));
			            sprintf(szAscBuf, "%04d", inLen);
			            memset(szBcdBuf, 0, sizeof(szBcdBuf));
			            wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
			         
			            memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*Key Serial Number Length*/  
			            memcpy(&szPacket[inPacketCnt], "33", 2); inPacketCnt+=2; /*key Serial Number - table id - "33"*/            	
			            memcpy(&szPacket[inPacketCnt], szKSN+inKSNPadLen, inKSNLen); inPacketCnt+=inKSNLen; /*KSN*/
					}	
	            }				
			}
			

			if (strTCT.fDCCTerminal == TRUE)
			{
				if (srTransPara->fDCCOptin == TRUE)
				{
					// send dcc				
					inLen=0;		
					inLen+=26; // dcc message lenght

					memset(szAscBuf, 0, sizeof(szAscBuf));
					sprintf(szAscBuf, "%04d", inLen);
					memset(szBcdBuf, 0, sizeof(szBcdBuf));
					wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

					memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
					inPacketCnt+=2; /*DCC Number Length*/  

					memcpy(&szPacket[inPacketCnt], "19", 2);  
					inPacketCnt+=2; 

					vdDebug_LogPrintf("AAA - srTransPara->byTransType[%d]", srTransPara->byTransType);
					vdDebug_LogPrintf("AAA - srTransPara->szExchangeRate [%s]", srTransPara->szExchangeRate);
					vdDebug_LogPrintf("AAA - srTransPara->byOffline [%d]", srTransPara->byOffline);
					vdDebug_LogPrintf("AAA - srTransPara->szForeignAmount [%s]", srTransPara->szForeignAmount);

					memset(szExchangeRate,0x00,sizeof(szExchangeRate));
					memset(szSendExchangeRate,0x00,sizeof(szSendExchangeRate));

					memcpy(&szExchangeRate, srTransPara->szExchangeRate, srTransPara->inExchangeRate_len);

					ptr = strchr(szExchangeRate,'.');

					indecimalplace=strlen(ptr);

					indel = srTransPara->inExchangeRate_len-indecimalplace;

					indecimalplace--;

					int x=1, inPadzero = 0;

					memmove(&szExchangeRate[indel], &szExchangeRate[indel + 1], srTransPara->inExchangeRate_len - indel);
					sprintf(szdecimalplace, "%d", indecimalplace);
					memcpy(&szSendExchangeRate[0], szdecimalplace, strlen(szdecimalplace));
					if (srTransPara->inExchangeRate_len < 8)
		            {
		                 inPadzero = 8 - srTransPara->inExchangeRate_len; //8 is fixed length for foreign exchange rate
		                 do
		                 {			 	
		                      memcpy(&szSendExchangeRate[x], "0", 1);
		                      x++;
		                 }while (x == inPadzero);
		            }
					memcpy(&szSendExchangeRate[x], szExchangeRate, strlen(szExchangeRate));

					vdDebug_LogPrintf("AAA szExchangeRate[%s] indecimalplace[%d] szSendExchangeRate[%s] inPadzero[%d]", szExchangeRate, indecimalplace, szSendExchangeRate, inPadzero);

					//memcpy(&szPacket[inPacketCnt],gblszForeignAmountfld63, 12); inPacketCnt+=12; 
					memcpy(&szPacket[inPacketCnt],srTransPara->szForeignAmount, 12); inPacketCnt+=12; 
					memcpy(&szPacket[inPacketCnt],srTransPara->szCurrencyCode, 3); inPacketCnt+=3; 
					memcpy(&szPacket[inPacketCnt],szSendExchangeRate, 8); inPacketCnt+=8;
					//memcpy(&szPacket[inPacketCnt],srTransPara->szExchangeRate, srTransPara->inExchangeRate_len); inPacketCnt+=srTransPara->inExchangeRate_len;
					//memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30", 8); inPacketCnt+=8; //aaa hardcode temporary 
					memcpy(&szPacket[inPacketCnt],"\x31", 1); inPacketCnt+=1; 
				}
				else
				{
					inLen=0;		
					inLen+=26; // dcc message lenght
					memset(szAscBuf, 0, sizeof(szAscBuf));
					sprintf(szAscBuf, "%04d", inLen);
					memset(szBcdBuf, 0, sizeof(szBcdBuf));
					wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
					memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
					inPacketCnt+=2; /*DCC Number Length*/  
					memcpy(&szPacket[inPacketCnt], "19", 2);  
					inPacketCnt+=2; 
						
					if ((srTransPara->fDCCOptin== FALSE) && (srTransPara->fDCCEnable== TRUE))
					{
						memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency
					}					
					else
					{
						memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30", 24); inPacketCnt+=24; // 0 - dcc not offiered
					}					
				}
			}

			
#if 1         
				vdDebug_LogPrintf("+++++++++++++++++++++:[%d]:[%d]",srTransRec.CDTid2, strCDT.inCVV_II);
			
						//if(strlen(srTransPara->szCVV2) > 0)
					if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_MANUAL)
					{
			
					//CTOS_PrinterPutString("@@@@@@");
			
						vdDebug_LogPrintf("AAA - Manual endtry and cvv2 manual");
			
						// send szCVV2					
						inLen=0;		
						inLen+=8; // szCVV2 message lenght
			
								//memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);
			
						memset(szAscBuf, 0, sizeof(szAscBuf));
						sprintf(szAscBuf, "%04d", inLen);
						memset(szBcdBuf, 0, sizeof(szBcdBuf));
						wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
			
						
						if(strlen(srTransPara->szCVV2) < 4)
						{
							BYTE szAscBuf1[4 + 1];
							BYTE szPos[12+1];
							
							memset(szAscBuf1, 0, sizeof(szAscBuf1));
							memset(szPos, 0X00, sizeof(szPos));
			
							// right - justify and space fill the entry.
							// ex: 123 - "31 32 33 20 20 20"
							//sprintf(szAscBuf1, "%d%20x", atol(srTransRec.szCVV2)); 
			
			
							// #00234 - Incorrect CVV format DE 63
										
							strcpy(szPos, "\x31\x30");				
							vdCTOS_Pad_String(srTransPara->szCVV2, 4, ' ', POSITION_LEFT);	// ex: 123 - "20 31 32 33"
							strcat(szPos, srTransPara->szCVV2);
							
							memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/	
							memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
							//memcpy(&szPacket[inPacketCnt], szAscBuf1, 6); inPacketCnt+=6; /*szCVV2*/				
							memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
							
						}
						else  //already tested working - send 31 30 31 32 33 34
						{
							//char szAscBuf1[4 + 1];
							BYTE szPos[12+1];
							
							//memset(szAscBuf1, 0, sizeof(szAscBuf1));
							memset(szPos, 0X00, sizeof(szPos));
			
							strcpy(szPos, "\x31\x30");
							//sprintf(szAscBuf1, "%d", srTransPara->szCVV2);				
							strcat(szPos, srTransPara->szCVV2);
					
							memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/	
							memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/				
							//memcpy(&szPacket[inPacketCnt], srTransRec.szCVV2, 6); inPacketCnt+=6; /*szCVV2*/
							memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
						}
			
					}
			
					if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_NONE)
					{
						BYTE szAscBuf1[4 + 1];
						BYTE szPos[12+1];
			
						// send szCVV2					
						inLen=0;		
						inLen+=8; // szCVV2 message lenght
			
								//memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);
			
					//CTOS_PrinterPutString("&&&&&&&&&&&&&&&&&&&&");
			
			
						memset(szAscBuf, 0, sizeof(szAscBuf));
						sprintf(szAscBuf, "%04d", inLen);
						memset(szBcdBuf, 0, sizeof(szBcdBuf));
						wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
						
						memset(szAscBuf1, 0, sizeof(szAscBuf1));
						memset(szPos, 0X00, sizeof(szPos));
			
						// #00234 - Incorrect CVV format DE 63
									
						strcpy(szPos, "\x30\x30\x20\x20\x20\x20");				
						memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/	
						memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
						memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
					}
 #endif

#if 0
            //add originators reason code (for visa only)
            if (srTransPara->byTransType != SETTLE && srTransPara->byTransType != CLS_BATCH && srTransPara->IITid == VISA_ISSUER) {

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", 4);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
				
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; 
				
 				memcpy(&szPacket[inPacketCnt], "20", 2); inPacketCnt+=2; //table id - "20" 
 				memcpy(&szPacket[inPacketCnt], "11", 2); inPacketCnt+=2;
			}
#endif

         
			/* Packet Data Length */
			memset(szAscBuf, 0, sizeof(szAscBuf));
			sprintf(szAscBuf, "%04d", inPacketCnt);
			memset(szBcdBuf, 0, sizeof(szBcdBuf));
			wub_str_2_hex(szAscBuf, szBcdBuf, 4);
			memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
			inDataCnt += 2;

			/* Packet Data */
			memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
			inDataCnt += inPacketCnt;

			return inDataCnt;  
			
		}
        
       
        
}

/*
Given:
CVV2: 221
 
00 08 31 36 31 30 20 32 32 31
  
00 08               | LENGTH
31 36               | TAG
31                    | 1 � CVV2 Value is Present - fixed value.
30                    | 0 � Only the normal Response Code should be returned - fixed value
20                    | Space
32 32 31          | CVV2
*/
//#00138
// CVV Entry not implemented this should be parameterized and to be sent on DE 63 for Manual Entry only
#if 1         
    vdDebug_LogPrintf("+++++++++++++++++++++:[%d]:[%d]",srTransRec.CDTid2, strCDT.inCVV_II);

        	//if(strlen(srTransPara->szCVV2) > 0)
		if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_MANUAL)
		{

    	//CTOS_PrinterPutString("@@@@@@");

		    vdDebug_LogPrintf("AAA - Manual endtry and cvv2 manual");

			// send szCVV2					
			inLen=0;		
			inLen+=8; // szCVV2 message lenght

	            	//memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);

			memset(szAscBuf, 0, sizeof(szAscBuf));
			sprintf(szAscBuf, "%04d", inLen);
			memset(szBcdBuf, 0, sizeof(szBcdBuf));
			wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

			
			if(strlen(srTransPara->szCVV2) < 4)
			{
				BYTE szAscBuf1[4 + 1];
				BYTE szPos[12+1];
				
				memset(szAscBuf1, 0, sizeof(szAscBuf1));
				memset(szPos, 0X00, sizeof(szPos));

				// right - justify and space fill the entry.
				// ex: 123 - "31 32 33 20 20 20"
				//sprintf(szAscBuf1, "%d%20x", atol(srTransRec.szCVV2)); 


				// #00234 - Incorrect CVV format DE 63
							
				strcpy(szPos, "\x31\x30"); 				
				vdCTOS_Pad_String(srTransPara->szCVV2, 4, ' ', POSITION_LEFT);  // ex: 123 - "20 31 32 33"
				strcat(szPos, srTransPara->szCVV2);
				
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/  
				memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
				//memcpy(&szPacket[inPacketCnt], szAscBuf1, 6); inPacketCnt+=6; /*szCVV2*/				
				memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
				
			}
			else  //already tested working - send 31 30 31 32 33 34
			{
				//char szAscBuf1[4 + 1];
				BYTE szPos[12+1];
				
				//memset(szAscBuf1, 0, sizeof(szAscBuf1));
				memset(szPos, 0X00, sizeof(szPos));

				strcpy(szPos, "\x31\x30");
				//sprintf(szAscBuf1, "%d", srTransPara->szCVV2); 				
				strcat(szPos, srTransPara->szCVV2);
		
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/  
				memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/            	
				//memcpy(&szPacket[inPacketCnt], srTransRec.szCVV2, 6); inPacketCnt+=6; /*szCVV2*/
				memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
			}

		}

		if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_NONE)
		{
			BYTE szAscBuf1[4 + 1];
			BYTE szPos[12+1];

			// send szCVV2					
			inLen=0;		
			inLen+=8; // szCVV2 message lenght

	            	//memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);

    	//CTOS_PrinterPutString("&&&&&&&&&&&&&&&&&&&&");


			memset(szAscBuf, 0, sizeof(szAscBuf));
			sprintf(szAscBuf, "%04d", inLen);
			memset(szBcdBuf, 0, sizeof(szBcdBuf));
			wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
			
			memset(szAscBuf1, 0, sizeof(szAscBuf1));
			memset(szPos, 0X00, sizeof(szPos));

			// #00234 - Incorrect CVV format DE 63
						
			strcpy(szPos, "\x30\x30\x20\x20\x20\x20"); 				
			memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/  
			memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
			memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
		}
 #endif
		if(strTCT.fTSNEnable == 1)
		{
			// send TSN on every transaction - start		
	            	inLen=0;		
	        
	        	memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
				memset(szTempTermSerialNum, 0x00, sizeof(szTempTermSerialNum)); 

				//inCTOSS_GetEnvDB("SERNUM", szTempTermSerialNum);
				inGetSERNUM(szTempTermSerialNum);
				vdDebug_LogPrintf("2.Len[%d],szTempTermSerialNum[%s]", strlen(szTempTermSerialNum), szTempTermSerialNum);
				
				if (strlen(szTempTermSerialNum) > 0)
				{
					strcpy(szTermSerialNum, szTempTermSerialNum);
					inLenTermSerialNo=strlen(szTermSerialNum);
				}
				else
				{
                                               //CTOS_GetFactorySN(szTermSerialNum);
                                               usGetSerialNumber(szTermSerialNum);
					//inLenTermSerialNo=strlen(szTermSerialNum)-1;
					inLenTermSerialNo=strlen(szTermSerialNum);
				}

				vdDebug_LogPrintf("2.Len[%d],szTermSerialNum[%s]", strlen(szTermSerialNum), szTermSerialNum);
				
				//szTermSerialNum[15]=0;					        	
				vdDebug_LogPrintf("2.inLenTermSerialNo[%d]", inLenTermSerialNo);
	        
	        	inLen+=inLenTermSerialNo+2;
	        	//if(srTransRec.fRegUSD == 1) // append USD on terminal serial number registration.
	        	if(srTransRec.fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.	        	
	        	inLen+=3;
	        
	        	memset(szAscBuf, 0, sizeof(szAscBuf));
	        	sprintf(szAscBuf, "%04d", inLen);
	        	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	        	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	        
	        	memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
			inPacketCnt+=2; /*Terminal Serial Number Length*/  
	        	memcpy(&szPacket[inPacketCnt], "18", 2); 
			inPacketCnt+=2; /*Terminal Serial Number - table id - "18"*/  
	        
	        	if(srTransRec.fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.
	        	{
		        	memcpy(&szPacket[inPacketCnt], "USD", 3); 
		        	inPacketCnt+=3;
	        	}
	        
	        	memcpy(&szPacket[inPacketCnt], szTermSerialNum, inLenTermSerialNo); 
			inPacketCnt+=inLenTermSerialNo; /*Terminal Serial Number*/ 
	        	
		}
		// send TSN on every transaction - end

     #if 0
	 //if  ((strTCT.fDCCTerminal == FALSE) && (strCDT.fDCCEnable == TRUE) && ( (srTransPara->byTransType==SALE) ||(srTransPara->byTransType==VOID) ||(srTransPara->byTransType==PRE_AUTH) 
          if  ((strTCT.fDCCTerminal == TRUE) && /*(strCDT.fDCCEnable == FALSE) &&*/ ( (srTransPara->byTransType==SALE) ||(srTransPara->byTransType==VOID) ||(srTransPara->byTransType==PRE_AUTH)
	 ||(srTransPara->byTransType==PRE_COMP)||(srTransPara->byTransType==VOID_PRECOMP)||(srTransPara->byTransType==REFUND)
	 ||(srTransPara->byTransType==SALE_OFFLINE)||(srTransPara->byTransType==SALE_TIP) || (srTransPara->byTransType==PREAUTH_VER) 
	 || (srTransPara->byTransType==PREAUTH_VOID)|| (srTransPara->byTransType==REVERSAL)|| (srTransPara->byTransType==PREAUTH_REVERSAL)
	 || (srTransPara->byTransType==PREAUTH_VOID_REVERSAL)|| (srTransPara->byTransType==PRECOMP_REVERSAL) || (srTransPara->byTransType==VOID_REVERSAL)
	 || (srTransPara->byTransType==REFUND_REVERSAL)|| (srTransPara->byTransType==VOIDREFUND_REVERSAL) || (srTransPara->byTransType==PREAUTH_COMP) ))
	 	{
				// send dcc				
				inLen=0;		
				inLen+=26; // dcc message lenght

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
				inPacketCnt+=2; /*DCC Number Length*/  

				memcpy(&szPacket[inPacketCnt], "19", 2);  
				inPacketCnt+=2; 
				memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency
                            
	 	}
	#endif
	
	 if ((strTCT.fDCCTerminal == TRUE) && /*(strCDT.fDCCEnable == TRUE) &&*/ ( (srTransPara->byTransType==SALE) ||(srTransPara->byTransType==VOID) ||(srTransPara->byTransType==PRE_AUTH) 
	 ||(srTransPara->byTransType==PRE_COMP)||(srTransPara->byTransType==VOID_PRECOMP)||(srTransPara->byTransType==REFUND)
	 ||(srTransPara->byTransType==SALE_OFFLINE)||(srTransPara->byTransType==SALE_TIP) || (srTransPara->byTransType==PREAUTH_VER) 
	 || (srTransPara->byTransType==PREAUTH_VOID)|| (srTransPara->byTransType==REVERSAL)|| (srTransPara->byTransType==PREAUTH_REVERSAL)
	 || (srTransPara->byTransType==PREAUTH_VOID_REVERSAL)|| (srTransPara->byTransType==PRECOMP_REVERSAL) || (srTransPara->byTransType==VOID_REVERSAL)
	 || (srTransPara->byTransType==REFUND_REVERSAL)|| (srTransPara->byTransType==VOIDREFUND_REVERSAL) || (srTransPara->byTransType==PREAUTH_COMP) ))
      {
           //if (gblDCCTrans==FALSE)
           vdDebug_LogPrintf("AAA - srTransRec.fDCCOptin[%d] srTransPara->byTransType[%d] srTransPara.fDCCOptin[%d]",  srTransRec.fDCCOptin, srTransPara->byTransType, srTransPara->fDCCOptin);
           if (srTransPara->fDCCOptin==FALSE)
           {
                // send dcc				
                inLen=0;		
                inLen+=26; // dcc message lenght
                                    
                memset(szAscBuf, 0, sizeof(szAscBuf));
                sprintf(szAscBuf, "%04d", inLen);
                memset(szBcdBuf, 0, sizeof(szBcdBuf));
                wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
                
                memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
                inPacketCnt+=2; /*DCC Number Length*/  
                
                
                memcpy(&szPacket[inPacketCnt], "19", 2);  
                inPacketCnt+=2; 
                memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency
           }
           else
           {
              
				// send dcc				
				inLen=0;		
				inLen+=26; // dcc message lenght

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
				inPacketCnt+=2; /*DCC Number Length*/  

				memcpy(&szPacket[inPacketCnt], "19", 2);  
				inPacketCnt+=2; 

				// memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x38\x38\x30\x30\x34\x35\x38\x33\x30\x30\x30\x30\x30\x38\x30\x31", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency

				vdDebug_LogPrintf("AAA - srTransPara->byTransType[%d]", srTransPara->byTransType);
				vdDebug_LogPrintf("AAA - srTransPara->szExchangeRate [%s]", srTransPara->szExchangeRate);
				vdDebug_LogPrintf("AAA - srTransPara->byOffline [%d]", srTransPara->byOffline);
				vdDebug_LogPrintf("AAA - srTransPara->szForeignAmount [%s]", srTransPara->szForeignAmount);

				memset(szExchangeRate,0x00,sizeof(szExchangeRate));
				memset(szSendExchangeRate,0x00,sizeof(szSendExchangeRate));

				memcpy(&szExchangeRate, srTransPara->szExchangeRate, srTransPara->inExchangeRate_len);

				ptr = strchr(szExchangeRate,'.');

				indecimalplace=strlen(ptr);

				indel = srTransPara->inExchangeRate_len-indecimalplace;

				indecimalplace--;

				int x=1, inPadzero = 0;

				memmove(&szExchangeRate[indel], &szExchangeRate[indel + 1], srTransPara->inExchangeRate_len - indel);
				sprintf(szdecimalplace, "%d", indecimalplace);
				memcpy(&szSendExchangeRate[0], szdecimalplace, strlen(szdecimalplace));
				if (srTransPara->inExchangeRate_len < 8)
	            {
	                 inPadzero = 8 - srTransPara->inExchangeRate_len; //8 is fixed length for foreign exchange rate
	                 do
	                 {			 	
	                      memcpy(&szSendExchangeRate[x], "0", 1);
	                      x++;
	                 }while (x == inPadzero);
	            }
				memcpy(&szSendExchangeRate[x], szExchangeRate, strlen(szExchangeRate));

				vdDebug_LogPrintf("AAA szExchangeRate[%s] indecimalplace[%d] szSendExchangeRate[%s] inPadzero[%d]", szExchangeRate, indecimalplace, szSendExchangeRate, inPadzero);

				//memcpy(&szPacket[inPacketCnt],gblszForeignAmountfld63, 12); inPacketCnt+=12; 
				memcpy(&szPacket[inPacketCnt],srTransPara->szForeignAmount, 12); inPacketCnt+=12; 
				memcpy(&szPacket[inPacketCnt],srTransPara->szCurrencyCode, 3); inPacketCnt+=3; 
				memcpy(&szPacket[inPacketCnt],szSendExchangeRate, 8); inPacketCnt+=8;
				//memcpy(&szPacket[inPacketCnt],srTransPara->szExchangeRate, srTransPara->inExchangeRate_len); inPacketCnt+=srTransPara->inExchangeRate_len;
				//memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30", 8); inPacketCnt+=8; //aaa hardcode temporary 
				memcpy(&szPacket[inPacketCnt],"\x31", 1); inPacketCnt+=1; 
	
           }
      }
       

//	CTOS_PrinterPutString("inPackIsoFunc63-3");
//	CTOS_PrinterPutString(srTransRec.szTerms);

	/* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
	
   // vdDebug_LogPrintf(". Pack Len(%d)",inDataCnt);
    //vdDebug_LogPrintf("**inPackIsoFunc63 END**");

//	CTOS_PrinterPutString("inPackIsoFunc63-4");
//	CTOS_PrinterPutString(srTransRec.szTerms);


//inGlobalCount ++; // wont print packet again.
/*{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "inGlobalCount::[%d]", inGlobalCount);
    	CTOS_PrinterPutString(szTemp);
   	}*/
   
#if 0
       // if(inCTOSS_CheckBitmapSetBit(63) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
	inGlobalCount = 1;	
    vdMyEZLib_HexDump("FIELD 63:", uszSendData, inDataCnt);
}
//}
#endif


//	CTOS_PrinterPutString("inPackIsoFunc63-end");
//	CTOS_PrinterPutString(srTransRec.szTerms);

    return inDataCnt;
	
}

#else
int inPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszSendData)
{
     
	int inPacketCnt = 0;
	int inResult;
	int inTranCardType;
	ACCUM_REC srAccumRec;
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szTemplate[100], szPacket[128 + 1];
	char szTermSerialNum[25+1];
	char szTempTermSerialNum[25+1];
	char szTemplate1[100];
	char szPacket1[4+1];
	char szBcdBuf1[4 + 1];	
	int inLen=0;		
	char szTemp[4+1], szTemp2[12+1];
	char szCatgCode[3+1];
	BYTE szKSN[20+1];
	int inKSNPadLen=0, inKSNLen=20;

	BYTE szForeignAmountfld63[12+1];
         BYTE szExchangeRate[12+1];
         BYTE szSendExchangeRate[12+1];
         char * ptr;
         char szdecimalplace[1+1];
         int indecimalplace;
         int indel;
	
    	//USHORT usTotalSaleCount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount;
    	//ULONG  ulTotalSaleAmount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount;

	
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

// fix for Wrong implementation of USD Currency
//USD currency prompts upon voiding & settlement
//Incorrect Currecy code sent on 0500 DE 63 using USD Profile
        inTCTRead(1);	
	if(strTCT.fRegUSD == 1) 
		inCSTRead(2);
	else
		inCSTRead(1);

	
#if 0
{
	char szTerms[30+1];
	memset(szTerms,0x00,sizeof(szTerms));
	sprintf(szTerms, "byTransType::%d::%s", srTransPara->byTransType, srTransRec.szPromoLabel);
	CTOS_PrinterPutString(szTerms);
	
}
#endif	

//	CTOS_PrinterPutString("inPackIsoFunc63-start!!");

	vdDebug_LogPrintf("saturn field 63");																	//fix issue:Installment data should be present on DE 63 on 0320 message
    if(srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)// || srTransPara->byPackType == BATCH_UPLOAD)
    {
		        memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
		        if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
		        {
		            vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		            return ST_ERROR;	
		        }		
		        vdDebug_LogPrintf("Test2222");
		        //0 is for Credit type, 1 is for debit type
		        inTranCardType = 0;
		        vdDebug_LogPrintf("**inPackIsoFunc63 START**byTransType[%d]Sale[%d]Refund[%d]", srTransPara->byTransType,srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
				
		        if((srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount) == 0 
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount) == 0					
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount) == 0										
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount) == 0
					&& (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount) == 0)
					
		        {

		//	CTOS_PrinterPutString("inPackIsoFunc63");
		        
		            //strcpy(szPacket, "000000000000000");
		            //strcat(szPacket, "000000000000000");

		                memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 1st set			
		                memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 2nd set
		                memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 3rd set
		                memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 4th set
		               
		                
				sprintf(szCatgCode, "%03d", atoi(strCST.szCurCode));
		                memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code1 set
		                memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code2 set
					
		            //strcat(szPacket, "000");
		            //inPacketCnt += 30;
		            
			
        		}
			else
        		{    	
				//	CTOS_PrinterPutString("inPackIsoFunc63.1");

				//============================
				// Send data required for installment de63
				//============================
#if 0	
				inLen = 0;		
				inLen+=126;

				/*message lenght*/	
				memset(szCatgCode, 0, sizeof(szCatgCode)); // curr code

				memset(szAscBuf, 0, sizeof(szAscBuf));
				sprintf(szAscBuf, "%04d", inLen);
				memset(szBcdBuf, 0, sizeof(szBcdBuf));
				wub_str_2_hex(szAscBuf, szBcdBuf, 4);
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; 
#endif	   
				/* SALE - Credit, Redeem, Installment sale*/
				memset(szTemplate, 0x00, sizeof(szTemplate));
				sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount				
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount								
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount);
				memcpy(&szPacket[inPacketCnt], szTemplate, 3); inPacketCnt+=3; 

				sprintf(szTemplate, "%012ld", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount				
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount								
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount
				+srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount);  
				memcpy(&szPacket[inPacketCnt], szTemplate, 12); inPacketCnt += 12;

				/* REFUND */
				memset(szTemplate, 0x00, sizeof(szTemplate));
				sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
				memcpy(&szPacket[inPacketCnt], szTemplate, 3); inPacketCnt += 3;			

				sprintf(szTemplate, "%012ld", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount));
				memcpy(&szPacket[inPacketCnt], szTemplate, 12); inPacketCnt += 12;            


				// memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 2nd set
				// memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 3rd set
				// memcpy(&szPacket[inPacketCnt],"000000000000000000000000000000", 30); inPacketCnt+=30; // 4th set

				memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 2nd set
				memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 3rd set
				memcpy(&szPacket[inPacketCnt],"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 30); inPacketCnt+=30; // 4th set


				sprintf(szCatgCode, "%03d", atoi(strCST.szCurCode));
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code1 set
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code2 set 
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code3 set 		fix for issue: DE 63 reconciliation total missing fourth currency code (settlement 0500)		
				memcpy(&szPacket[inPacketCnt],szCatgCode, 3); inPacketCnt+=3; // curr code3 set 		fix for issue: DE 63 reconciliation total missing fourth currency code (settlement 0500)		
				
        		}

    }
    else
    {
        if((srTransPara->fDebit == TRUE) || (srTransPara->fEMVPIN == TRUE) || (srTransPara->fCUPPINEntry == TRUE))        
        {
            if(strTCT.fTSNEnable == 1)
            {
                // sending TSN(18) after KSN(33)	
                inLen=0;		
                
                memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
				memset(szTempTermSerialNum, 0x00, sizeof(szTempTermSerialNum));

				inCTOSS_GetEnvDB("SERNUM", szTempTermSerialNum);
				vdDebug_LogPrintf("saturn 1.Len[%d],szTempTermSerialNum[%s]", strlen(szTempTermSerialNum), szTempTermSerialNum);
				if (strlen(szTempTermSerialNum) > 0)
				{
					strcpy(szTermSerialNum, szTempTermSerialNum);
					inLenTermSerialNo=strlen(szTermSerialNum);
				}
				else
				{
	                CTOS_GetFactorySN(szTermSerialNum);
					inLenTermSerialNo=strlen(szTermSerialNum)-1;
				}

				vdDebug_LogPrintf("saturn 1.Len[%d],szTermSerialNum[%s]", strlen(szTermSerialNum), szTermSerialNum);
				
                //szTermSerialNum[15]=0;
                
				vdDebug_LogPrintf("saturn 1.inLenTermSerialNo[%d]", inLenTermSerialNo);
                
                inLen+=inLenTermSerialNo+2;
                if(srTransRec.fRegUSD == 1) // append USD on terminal serial number registration.
                    inLen+=3;
                
                memset(szAscBuf, 0, sizeof(szAscBuf));
                sprintf(szAscBuf, "%04d", inLen);
                memset(szBcdBuf, 0, sizeof(szBcdBuf));
                wub_str_2_hex(szAscBuf, szBcdBuf, 4); // TSN message length
                
                memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
                inPacketCnt+=2; /*Terminal Serial Number Length*/  
                memcpy(&szPacket[inPacketCnt], "18", 2); 
                inPacketCnt+=2; /*Terminal Serial Number - table id - "18"*/  
                
                if(srTransRec.fRegUSD == 1) // append USD on terminal serial number registration.
                {
                    memcpy(&szPacket[inPacketCnt], "USD", 3); 
                    inPacketCnt+=3;
                }
                
                memcpy(&szPacket[inPacketCnt], szTermSerialNum, inLenTermSerialNo); 
                inPacketCnt+=inLenTermSerialNo; /*Terminal Serial Number*/ 
            }

            
			vdDebug_LogPrintf("saturn get ksn");																	
			wub_hex_2_str(srTransRec.szKSN, szKSN, 20); // convert to ascii

			vdDebug_LogPrintf("AAA - szKSN[%s] len[%d]", szKSN, strlen(szKSN));
			
            if(szKSN[0] == 'F')
            {
                vdDebug_LogPrintf("szKSN=(%s)", szKSN);
                
                while(szKSN[inKSNPadLen] == 'F')  
                    inKSNPadLen++;
					
                vdDebug_LogPrintf("inKSNPadLen=(%d)", inKSNPadLen);
            }

			inKSNLen=inKSNLen-inKSNPadLen;
            vdDebug_LogPrintf("inLen=(%d)", inLen);
    				
            // send KSN					
            inLen=0;		
            //inLen+=22; // KSN message lenght
            inLen+=2+inKSNLen;

			vdDebug_LogPrintf("inLen=(%d)", inLen);
			
            memset(szAscBuf, 0, sizeof(szAscBuf));
            sprintf(szAscBuf, "%04d", inLen);
            memset(szBcdBuf, 0, sizeof(szBcdBuf));
            wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
            
            
            
            //CTOS_PrinterPutString("inPackIsoFunc63");
            //CTOS_PrinterPutString(szKSN);
            vdDebug_LogPrintf("");
            
            
            memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*Key Serial Number Length*/  
            memcpy(&szPacket[inPacketCnt], "33", 2); inPacketCnt+=2; /*key Serial Number - table id - "33"*/            	
            memcpy(&szPacket[inPacketCnt], szKSN+inKSNPadLen, inKSNLen); inPacketCnt+=inKSNLen; /*KSN*/
            //memcpy(&szPacket[inPacketCnt], srTransRec.szKSN, 10); inPacketCnt+=10; /*KSN*/
            
            
            /* Packet Data Length */
            memset(szAscBuf, 0, sizeof(szAscBuf));
            sprintf(szAscBuf, "%04d", inPacketCnt);
            memset(szBcdBuf, 0, sizeof(szBcdBuf));
            wub_str_2_hex(szAscBuf, szBcdBuf, 4);
            memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
            inDataCnt += 2;
            
            /* Packet Data */
            memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
            inDataCnt += inPacketCnt;
            
            return inDataCnt;          	
        }
       
        if(srTransPara->fInstallment == TRUE || srTransPara->fCash2Go == TRUE)
        {

//	CTOS_PrinterPutString("inPackIsoFunc63-1");
//	CTOS_PrinterPutString(srTransPara->szTerms);
		
		//#62 Installment data should be present on DE 63 on 0200 message & 
		//#63 Installment DATA not sent on DE 63 value Void Reversal 0400 message
		//Ensure that there is CASH2Go installment transaction stored  in the batch 2. Settle transaction with batch uploadCheck 0320 message of istallment transaction"
		#if 0
		/*installment data start*/
            //if(srTransPara->byTransType == VOID || srTransPara->byPackType == BATCH_UPLOAD)
            if(srTransPara->byPackType == BATCH_UPLOAD) // removed original data for void condition - 05262015
            {

		inLen=0;
		inLen+=36;
			
		memset(szAscBuf, 0, sizeof(szAscBuf));
		sprintf(szAscBuf, "%04d", inLen);
		memset(szBcdBuf, 0, sizeof(szBcdBuf));
		wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		
		memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*message lenght*/	
		memcpy(&szPacket[inPacketCnt], "90", 2); inPacketCnt+=2; /*Installment - table id - "90"*/  
			
                memcpy(&szPacket[inPacketCnt], srTransPara->szInstallmentData, 36);
                inPacketCnt+=36;
            }
            else if(srTransPara->byTransType == SALE)
		#endif

/*{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "@@@@::[%d]:[%d]", srTransPara->byTransType, srTransPara->byPackType);
    	CTOS_PrinterPutString(szTemp);
   	}*/
		
            if(srTransPara->byTransType == SALE && srTransPara->byPackType != BATCH_UPLOAD)
            {
		inLen=0;
		inLen+=36;

//	CTOS_PrinterPutString("SALE!!!");
		
		memset(szAscBuf, 0, sizeof(szAscBuf));
		sprintf(szAscBuf, "%04d", inLen);
		memset(szBcdBuf, 0, sizeof(szBcdBuf));
		wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		
		memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*message lenght*/	
		memcpy(&szPacket[inPacketCnt], "90", 2); inPacketCnt+=2; /*Installment - table id - "90"*/  
            
                //memset(szTemp, 0, sizeof(szTemp));
                //sprintf(szTemp, "%04d", atol(srTransPara->szTerms));
		sprintf(szTemp, "%04d", atol(srTransRec.szTerms1));                
                memcpy(&szPacket[inPacketCnt], szTemp, 4); inPacketCnt+=4; /*Loan Period n4 - 4*/
		//memcpy(&szPacket[inPacketCnt], srTransPara->szOffsetPeriod, 4); inPacketCnt+=4; /*Offset Period an4 - 4*/				
		memcpy(&szPacket[inPacketCnt], srTransRec.szOffsetPeriodEX, 4); inPacketCnt+=4; /*Offset Period an4 - 4*/
		memcpy(&szPacket[inPacketCnt], srTransRec.szFundIndicator, 1); inPacketCnt+=1; /*Funding Indicator an1 - 1*/ /*I: Issuer funded (default) A: Acquirer funded*/
		memcpy(&szPacket[inPacketCnt], srTransRec.szPlanID1, 10); inPacketCnt+=10; /*Plan ID an10 - 10*/
		memcpy(&szPacket[inPacketCnt], srTransRec.szPlanGroup, 10); inPacketCnt+=10; /*Plan Group an10 - 10*/
		memcpy(&szPacket[inPacketCnt], srTransRec.szPlanGroupIDX, 5); inPacketCnt+=5;/*Plan Group Index n5 - 5*/


	            memcpy(srTransPara->szTerms, srTransRec.szTerms1, 4);
	            memcpy(srTransPara->szOffsetPeriodEX, srTransRec.szOffsetPeriodEX, 4);
	            memcpy(srTransPara->szFundIndicator, srTransRec.szFundIndicator, 1);
	            memcpy(srTransPara->szPlanID1, srTransRec.szPlanID1, 10);
	            memcpy(srTransPara->szPlanGroup, srTransRec.szPlanGroup, 10);
	            memcpy(srTransPara->szPlanGroupIDX, srTransRec.szPlanGroupIDX, 5);

		

            }
            //if(srTransPara->byPackType == BATCH_UPLOAD) //            //
            if(srTransPara->byTransType == VOID || srTransPara->byPackType == BATCH_UPLOAD)
            {
		inLen=0;
		inLen+=36;

#if 0
	CTOS_PrinterPutString("BATCH_UPLOAD!!!");

	CTOS_PrinterPutString(srTransPara->szTerms);
	CTOS_PrinterPutString(srTransPara->szOffsetPeriod);
	CTOS_PrinterPutString(srTransPara->szFundIndicator);
	CTOS_PrinterPutString(srTransPara->szPlanID1);
	CTOS_PrinterPutString(srTransPara->szPlanGroup);
	CTOS_PrinterPutString(srTransPara->szPlanGroupIDX);
#endif

		memset(szAscBuf, 0, sizeof(szAscBuf));
		sprintf(szAscBuf, "%04d", inLen);
		memset(szBcdBuf, 0, sizeof(szBcdBuf));
		wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		
		memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*message lenght*/	
		memcpy(&szPacket[inPacketCnt], "90", 2); inPacketCnt+=2; /*Installment - table id - "90"*/  
            
                //memset(szTemp, 0, sizeof(szTemp));
                sprintf(szTemp, "%04d", atol(srTransPara->szTerms1));
                memcpy(&szPacket[inPacketCnt], szTemp, 4); inPacketCnt+=4; /*Loan Period n4 - 4*/
		memcpy(&szPacket[inPacketCnt], srTransPara->szOffsetPeriodEX, 4); inPacketCnt+=4; /*Offset Period an4 - 4*/				
		memcpy(&szPacket[inPacketCnt], srTransPara->szFundIndicator, 1); inPacketCnt+=1; /*Funding Indicator an1 - 1*/ /*I: Issuer funded (default) A: Acquirer funded*/
		memcpy(&szPacket[inPacketCnt], srTransPara->szPlanID1, 10); inPacketCnt+=10; /*Plan ID an10 - 10*/
		memcpy(&szPacket[inPacketCnt], srTransPara->szPlanGroup, 10); inPacketCnt+=10; /*Plan Group an10 - 10*/
		memcpy(&szPacket[inPacketCnt], srTransPara->szPlanGroupIDX, 5); inPacketCnt+=5;/*Plan Group Index n5 - 5*/

            }
			
        }
}

//	CTOS_PrinterPutString("inPackIsoFunc63-2");
//	CTOS_PrinterPutString(srTransRec.szTerms);
#if 0
{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "[%d]:[%d]:[%d]:[%d]", srTransPara->byTransType, srTransPara->byPackType, srTransRec.CDTid2, strCDT.inCVV_II);
    	CTOS_PrinterPutString(szTemp);
    	//CTOS_PrinterPutString(srTransPara->szCVV2);
		
}
#endif
/*
Given:
CVV2: 221
 
00 08 31 36 31 30 20 32 32 31
  
00 08               | LENGTH
31 36               | TAG
31                    | 1 � CVV2 Value is Present - fixed value.
30                    | 0 � Only the normal Response Code should be returned - fixed value
20                    | Space
32 32 31          | CVV2
*/
//#00138
// CVV Entry not implemented this should be parameterized and to be sent on DE 63 for Manual Entry only
#if 1         
    vdDebug_LogPrintf("+++++++++++++++++++++:[%d]:[%d]",srTransRec.CDTid2, strCDT.inCVV_II);

        	//if(strlen(srTransPara->szCVV2) > 0)
		if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_MANUAL)
		{

    	//CTOS_PrinterPutString("@@@@@@");

			// send szCVV2					
			inLen=0;		
			inLen+=8; // szCVV2 message lenght

	            	//memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);

			memset(szAscBuf, 0, sizeof(szAscBuf));
			sprintf(szAscBuf, "%04d", inLen);
			memset(szBcdBuf, 0, sizeof(szBcdBuf));
			wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

			
			if(strlen(srTransPara->szCVV2) < 4)
			{
				BYTE szAscBuf1[4 + 1];
				BYTE szPos[12+1];
				
				memset(szAscBuf1, 0, sizeof(szAscBuf1));
				memset(szPos, 0X00, sizeof(szPos));

				// right - justify and space fill the entry.
				// ex: 123 - "31 32 33 20 20 20"
				//sprintf(szAscBuf1, "%d%20x", atol(srTransRec.szCVV2)); 


				// #00234 - Incorrect CVV format DE 63
							
				strcpy(szPos, "\x31\x30"); 				
				vdCTOS_Pad_String(srTransPara->szCVV2, 4, ' ', POSITION_LEFT);  // ex: 123 - "20 31 32 33"
				strcat(szPos, srTransPara->szCVV2);
				
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/  
				memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
				//memcpy(&szPacket[inPacketCnt], szAscBuf1, 6); inPacketCnt+=6; /*szCVV2*/				
				memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
				
			}
			else  //already tested working - send 31 30 31 32 33 34
			{
				//char szAscBuf1[4 + 1];
				BYTE szPos[12+1];
				
				//memset(szAscBuf1, 0, sizeof(szAscBuf1));
				memset(szPos, 0X00, sizeof(szPos));

				strcpy(szPos, "\x31\x30");
				//sprintf(szAscBuf1, "%d", srTransPara->szCVV2); 				
				strcat(szPos, srTransPara->szCVV2);
		
				memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/  
				memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/            	
				//memcpy(&szPacket[inPacketCnt], srTransRec.szCVV2, 6); inPacketCnt+=6; /*szCVV2*/
				memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
			}

		}

		if (srTransPara->byEntryMode  == CARD_ENTRY_MANUAL && strCDT.inCVV_II == CVV2_NONE)
		{
			BYTE szAscBuf1[4 + 1];
			BYTE szPos[12+1];

			// send szCVV2					
			inLen=0;		
			inLen+=8; // szCVV2 message lenght

	            	//memcpy(srTransPara->szCVV2, srTransRec.szCVV2, 6);

    	//CTOS_PrinterPutString("&&&&&&&&&&&&&&&&&&&&");


			memset(szAscBuf, 0, sizeof(szAscBuf));
			sprintf(szAscBuf, "%04d", inLen);
			memset(szBcdBuf, 0, sizeof(szBcdBuf));
			wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
			
			memset(szAscBuf1, 0, sizeof(szAscBuf1));
			memset(szPos, 0X00, sizeof(szPos));

			// #00234 - Incorrect CVV format DE 63
						
			strcpy(szPos, "\x30\x30\x20\x20\x20\x20"); 				
			memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); inPacketCnt+=2; /*szCVV2 Length*/  
			memcpy(&szPacket[inPacketCnt], "16", 2); inPacketCnt+=2; /*szCVV2 - table id - "16"*/  
			memcpy(&szPacket[inPacketCnt], szPos, 6); inPacketCnt+=6; /*szCVV2*/				
		}
 #endif

        vdDebug_LogPrintf("saturn tsn enable %d", strTCT.fTSNEnable);
		if(strTCT.fTSNEnable == 1)
		{
			// send TSN on every transaction - start		
	            	inLen=0;		
	        
	        	memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
				memset(szTempTermSerialNum, 0x00, sizeof(szTempTermSerialNum)); 

				inCTOSS_GetEnvDB("SERNUM", szTempTermSerialNum);
				vdDebug_LogPrintf("saturn sernum 2.Len[%d],szTempTermSerialNum[%s]", strlen(szTempTermSerialNum), szTempTermSerialNum);
				
				if (strlen(szTempTermSerialNum) > 0)
				{  
                    vdDebug_LogPrintf("saturn sernum > 0");
					strcpy(szTermSerialNum, szTempTermSerialNum);
					inLenTermSerialNo=strlen(szTermSerialNum);
				}
				else
				{
				    vdDebug_LogPrintf("saturn get fatory sn");
	                CTOS_GetFactorySN(szTermSerialNum);
 					inLenTermSerialNo=strlen(szTermSerialNum)-1;

					vdDebug_LogPrintf("saturn getfactory serial number %s",szTermSerialNum );
				}

				vdDebug_LogPrintf("saturn 2.Len[%d],szTermSerialNum[%s]", strlen(szTermSerialNum), szTermSerialNum);
				
				//szTermSerialNum[15]=0;					        	
				vdDebug_LogPrintf("saturn 2.inLenTermSerialNo[%d]", inLenTermSerialNo);
	        
	        	inLen+=inLenTermSerialNo+2;
	        	//if(srTransRec.fRegUSD == 1) // append USD on terminal serial number registration.
	        	if(srTransRec.fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.	        	
	        	inLen+=3;
	        
	        	memset(szAscBuf, 0, sizeof(szAscBuf));
	        	sprintf(szAscBuf, "%04d", inLen);
	        	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	        	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	        
	        	memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
			inPacketCnt+=2; /*Terminal Serial Number Length*/  
	        	memcpy(&szPacket[inPacketCnt], "18", 2); 
			inPacketCnt+=2; /*Terminal Serial Number - table id - "18"*/  
	        
	        	if(srTransRec.fRegUSD == 1 || strTCT.fRegUSD == 1) // 1. append USD on terminal serial number registration. 2. send usd currency to host.
	        	{
		        	memcpy(&szPacket[inPacketCnt], "USD", 3); 
		        	inPacketCnt+=3;
	        	}
	        
	        	memcpy(&szPacket[inPacketCnt], szTermSerialNum, inLenTermSerialNo); 
			inPacketCnt+=inLenTermSerialNo; /*Terminal Serial Number*/ 
	        	
		}
		// send TSN on every transaction - end

	 //if  ((strTCT.fDCCTerminal == FALSE) && (strCDT.fDCCEnable == TRUE) && ( (srTransPara->byTransType==SALE) ||(srTransPara->byTransType==VOID) ||(srTransPara->byTransType==PRE_AUTH) 
          if  ((strTCT.fDCCTerminal == TRUE) && (strCDT.fDCCEnable == FALSE) && ( (srTransPara->byTransType==SALE) ||(srTransPara->byTransType==VOID) ||(srTransPara->byTransType==PRE_AUTH)
	 ||(srTransPara->byTransType==PRE_COMP)||(srTransPara->byTransType==VOID_PRECOMP)||(srTransPara->byTransType==REFUND)
	 ||(srTransPara->byTransType==SALE_OFFLINE)||(srTransPara->byTransType==SALE_TIP) || (srTransPara->byTransType==PREAUTH_VER) 
	 || (srTransPara->byTransType==PREAUTH_VOID)|| (srTransPara->byTransType==REVERSAL)|| (srTransPara->byTransType==PREAUTH_REVERSAL)
	 || (srTransPara->byTransType==PREAUTH_VOID_REVERSAL)|| (srTransPara->byTransType==PRECOMP_REVERSAL) || (srTransPara->byTransType==VOID_REVERSAL)
	 || (srTransPara->byTransType==REFUND_REVERSAL)|| (srTransPara->byTransType==VOIDREFUND_REVERSAL) || (srTransPara->byTransType==PREAUTH_COMP) ))
	 	{
	 	         // send dcc				
                            inLen=0;		
                            inLen+=26; // dcc message lenght
                                                        
                            memset(szAscBuf, 0, sizeof(szAscBuf));
                            sprintf(szAscBuf, "%04d", inLen);
                            memset(szBcdBuf, 0, sizeof(szBcdBuf));
                            wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

			memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
			inPacketCnt+=2; /*DCC Number Length*/  

                            memcpy(&szPacket[inPacketCnt], "19", 2);  
                            inPacketCnt+=2; 
                            memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency
                            
	 	}
	 if ((strTCT.fDCCTerminal == TRUE) && (strCDT.fDCCEnable == TRUE) && ( (srTransPara->byTransType==SALE) ||(srTransPara->byTransType==VOID) ||(srTransPara->byTransType==PRE_AUTH) 
	 ||(srTransPara->byTransType==PRE_COMP)||(srTransPara->byTransType==VOID_PRECOMP)||(srTransPara->byTransType==REFUND)
	 ||(srTransPara->byTransType==SALE_OFFLINE)||(srTransPara->byTransType==SALE_TIP) || (srTransPara->byTransType==PREAUTH_VER) 
	 || (srTransPara->byTransType==PREAUTH_VOID)|| (srTransPara->byTransType==REVERSAL)|| (srTransPara->byTransType==PREAUTH_REVERSAL)
	 || (srTransPara->byTransType==PREAUTH_VOID_REVERSAL)|| (srTransPara->byTransType==PRECOMP_REVERSAL) || (srTransPara->byTransType==VOID_REVERSAL)
	 || (srTransPara->byTransType==REFUND_REVERSAL)|| (srTransPara->byTransType==VOIDREFUND_REVERSAL) || (srTransPara->byTransType==PREAUTH_COMP) ))
          {
               //if (gblDCCTrans==FALSE)
               if (srTransRec.fDCCOptin==FALSE)
               {
                    // send dcc				
                    inLen=0;		
                    inLen+=26; // dcc message lenght
                                        
                    memset(szAscBuf, 0, sizeof(szAscBuf));
                    sprintf(szAscBuf, "%04d", inLen);
                    memset(szBcdBuf, 0, sizeof(szBcdBuf));
                    wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght
                    
                    memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
                    inPacketCnt+=2; /*DCC Number Length*/  
                    
                    
                    memcpy(&szPacket[inPacketCnt], "19", 2);  
                    inPacketCnt+=2; 
                    memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x32", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency
               }
               else
               {
                  
                     // send dcc				
                            inLen=0;		
                            inLen+=26; // dcc message lenght
                                                        
                            memset(szAscBuf, 0, sizeof(szAscBuf));
                            sprintf(szAscBuf, "%04d", inLen);
                            memset(szBcdBuf, 0, sizeof(szBcdBuf));
                            wub_str_2_hex(szAscBuf, szBcdBuf, 4); // set lenght

			memcpy(&szPacket[inPacketCnt], szBcdBuf, 2); 
			inPacketCnt+=2; /*DCC Number Length*/  
                    
                    memcpy(&szPacket[inPacketCnt], "19", 2);  
                    inPacketCnt+=2; 

		// memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30\x38\x38\x30\x30\x34\x35\x38\x33\x30\x30\x30\x30\x30\x38\x30\x31", 24); inPacketCnt+=24; // 2 - dcc offered but choose local currency

                   vdDebug_LogPrintf("AAA - srTransPara->byTransType[%d]", srTransPara->byTransType);
                   vdDebug_LogPrintf("AAA - srTransPara->szExchangeRate [%s]", srTransPara->szExchangeRate);

		memset(szExchangeRate,0x00,sizeof(szExchangeRate));
		memset(szSendExchangeRate,0x00,sizeof(szSendExchangeRate));

                   
		memcpy(&szExchangeRate, srTransPara->szExchangeRate, srTransPara->inExchangeRate_len);
	
		ptr = strchr(szExchangeRate,'.');

		indecimalplace=strlen(ptr);

		 indel = srTransPara->inExchangeRate_len-indecimalplace;

		 indecimalplace--;

		 int x=1, inPadzero = 0;
		 
		memmove(&szExchangeRate[indel], &szExchangeRate[indel + 1], srTransPara->inExchangeRate_len - indel);
		sprintf(szdecimalplace, "%d", indecimalplace);
		memcpy(&szSendExchangeRate[0], szdecimalplace, strlen(szdecimalplace));
		if (srTransPara->inExchangeRate_len < 8)
                  {
                     inPadzero = 8 - srTransPara->inExchangeRate_len; //8 is fixed length for foreign exchange rate
                     do
                     {			 	
                          memcpy(&szSendExchangeRate[x], "0", 1);
                          x++;
                     }while (x == inPadzero);
                  }
		memcpy(&szSendExchangeRate[x], szExchangeRate, strlen(szExchangeRate));

		vdDebug_LogPrintf("AAA szExchangeRate[%s] indecimalplace[%d] szSendExchangeRate[%s] inPadzero[%d]", szExchangeRate, indecimalplace, szSendExchangeRate, inPadzero);

                    memcpy(&szPacket[inPacketCnt],gblszForeignAmountfld63, 12); inPacketCnt+=12; 
                    memcpy(&szPacket[inPacketCnt],srTransPara->szCurrencyCode, 3); inPacketCnt+=3; 
		 memcpy(&szPacket[inPacketCnt],szSendExchangeRate, 8); inPacketCnt+=8;
		//memcpy(&szPacket[inPacketCnt],srTransPara->szExchangeRate, srTransPara->inExchangeRate_len); inPacketCnt+=srTransPara->inExchangeRate_len;
                    //memcpy(&szPacket[inPacketCnt],"\x30\x30\x30\x30\x30\x30\x30\x30", 8); inPacketCnt+=8; //aaa hardcode temporary 
                    memcpy(&szPacket[inPacketCnt],"\x31", 1); inPacketCnt+=1; 
		
               }
          }
                 
                
               


//	CTOS_PrinterPutString("inPackIsoFunc63-3");
//	CTOS_PrinterPutString(srTransRec.szTerms);

	/* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
	
   // vdDebug_LogPrintf(". Pack Len(%d)",inDataCnt);
    //vdDebug_LogPrintf("**inPackIsoFunc63 END**");

//	CTOS_PrinterPutString("inPackIsoFunc63-4");
//	CTOS_PrinterPutString(srTransRec.szTerms);


//inGlobalCount ++; // wont print packet again.
/*{
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "inGlobalCount::[%d]", inGlobalCount);
    	CTOS_PrinterPutString(szTemp);
   	}*/
   
#if 0
       // if(inCTOSS_CheckBitmapSetBit(63) == 1){

if(strTCT.fPrintISOMessage == TRUE && inGlobalCount == 0){
	inGlobalCount = 1;	
    vdMyEZLib_HexDump("FIELD 63:", uszSendData, inDataCnt);
}
//}
#endif


//	CTOS_PrinterPutString("inPackIsoFunc63-end");
//	CTOS_PrinterPutString(srTransRec.szTerms);

    return inDataCnt;
	
}
#endif

int inPackIsoFunc64(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData)
{
    BYTE           szSHABinTemp[20];
    BYTE           szSHAAscTemp[50];
    BYTE           szTempMacResutl[16];
    SHA_CTX        stSHA;
    BYTE           szInitVictor[8];
    BYTE szClearTMK[33], szClearTAK[33], temp[17], ClearTMK[17];   
           
    inTLERead(1);
     
    vdMyEZLib_LogPrintf("**inPackIsoFunc64 START**");
    
    GET_KEY( szClearTMK , szClearTAK);

   
    vdMyEZLib_LogPrintf("**MAC DATA**");
    vdMyEZLib_LogPrintff(szDataForMAC,inMacMsgLen);
    
    CTOS_SHA1Init(&stSHA);   //Perform the SHA1 algorithm with the input data //                                                     
    CTOS_SHA1Update(&stSHA, szDataForMAC, inMacMsgLen);     //Finalize the SHA1 operation and retrun the result //                                                   
    CTOS_SHA1Final(szSHABinTemp,&stSHA);     //vdSHA1Generate(szDataForMAC, inMacMsgLen, szSHABinTemp); //wrong code          
    vdMyEZLib_LogPrintff(szSHABinTemp,20);   vdMyEZLib_LogPrintf("**SHA Generate**");    
    wub_hex_2_str(szSHABinTemp, szSHAAscTemp, 20);    
    memset(szInitVictor, 0x00, sizeof(szInitVictor));
    memset(szTempMacResutl, 0x00, sizeof(szTempMacResutl));
    
    if(srTransPara->byTransType == SIGN_ON)
    {
    	vdMyEZLib_LogPrintf("**SIGN ON CLEAR TAK**");
    	CTOS_MAC (szClearTAK, 16, szInitVictor, szSHAAscTemp, 40, szTempMacResutl);
    }    
    else 
    {
    	   //hard code key
	    memset( temp, 0x00, 17);	   
			vdMyEZLib_LogPrintf("**MAC KEY");	
		 	memcpy(szClearTAK,stTLE.szMACKey,16);	
    	CTOS_MAC (szClearTAK, 16, szInitVictor, szSHAAscTemp, 40, szTempMacResutl);
    }
    inDataCnt = 0;
    
    memcpy((char *)&uszSendData[inDataCnt], szTempMacResutl, 8);
    inDataCnt += 8;
    
    vdMyEZLib_LogPrintf("**inPackIsoFunc64 END**");
    return inDataCnt;
}

int inUnPackIsoFunc04(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     unsigned char szForeignAmount[12 + 1];
     if(srTransPara->byTransType == BAL_INQ)
     {
          memset(srTransPara->szTotalAmount, 0x00, sizeof(srTransPara->szTotalAmount));
          memcpy(srTransPara->szTotalAmount,(char *)uszUnPackBuf,12);
     }
    #if 0 
    else
    {
         wub_hex_2_str(uszUnPackBuf, szForeignAmount, 6);
         srTransPara->ulForeignAmount = atol(szForeignAmount);
    }
	vdDebug_LogPrintf("inUnPackIsoFunc05(%s) [%d]]", szForeignAmount,srTransPara->ulForeignAmount);
   #endif	
    return ST_SUCCESS;	
}

int inUnPackIsoFunc05(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	
       unsigned char szForeignAmount[12 + 1];
    
    memset(szForeignAmount, 0x00, sizeof(szForeignAmount));
    memset(gblszForeignAmountfld63,0,sizeof(gblszForeignAmountfld63));
  
	wub_hex_2_str(uszUnPackBuf, szForeignAmount, 6);
	
          //srTransPara->ulForeignAmount = atol(szForeignAmount);
          memcpy(srTransPara->szForeignAmount,szForeignAmount,12);

	  memcpy(gblszForeignAmountfld63,szForeignAmount,sizeof(gblszForeignAmountfld63));
	
	vdDebug_LogPrintf("inUnPackIsoFunc05(%s) [%s] [%s]]", szForeignAmount,srTransPara->szForeignAmount, gblszForeignAmountfld63);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
	int inLen;
	inLen = sizeof(szSTAN);	
    vdMyEZLib_HexDump("FIELD 11:", szSTAN, inLen);
}
	#endif
    return ST_SUCCESS;
}


int inUnPackIsoUnknown(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{	
    return ST_SUCCESS;	
}

int inUnPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	unsigned char szSTAN[6 + 1];
    
    memset(szSTAN, 0x00, sizeof(szSTAN));

	wub_hex_2_str(uszUnPackBuf, szSTAN, 3);
	
    srTransPara->ulTraceNum = atol(szSTAN);
	
	vdDebug_LogPrintf("inUnPackIsoFunc11(%s) [%d]", szSTAN,srTransPara->ulTraceNum);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
	int inLen;
	inLen = sizeof(szSTAN);	
    vdMyEZLib_HexDump("FIELD 11:", szSTAN, inLen);
}
	#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    memcpy(srTransPara->szTime,(char *)uszUnPackBuf,TIME_BCD_SIZE);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
	
    vdMyEZLib_HexDump("FIELD 12:", srTransPara->szTime, TIME_BCD_SIZE);
}
	#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    memcpy(srTransPara->szDate,(char *)uszUnPackBuf,DATE_BCD_SIZE);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
    vdMyEZLib_HexDump("FIELD 13:", srTransPara->szDate, DATE_BCD_SIZE);
}
	#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    if(srTransPara->byPackType == REVERSAL || srTransPara->byPackType == PREAUTH_VER || srTransPara->byPackType == PREAUTH_COMP || srTransPara->byTransType == DCC_MERCH_RATE_REQ || srTransPara->byTransType == DCC_RATE_REQUEST)
        return ST_SUCCESS;
    memcpy(srTransPara->szRRN, (char *)uszUnPackBuf, 12);

    vdDebug_LogPrintf("AAA inUnPackIsoFunc37 srTransPara->szRRN[%s]", srTransPara->szRRN);
    
    return ST_SUCCESS;
}

int inUnPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    if(srTransPara->byPackType == PREAUTH_VER || srTransPara->byPackType == PREAUTH_COMP)
        return ST_SUCCESS;
    
    memcpy(srTransPara->szAuthCode, (char *)uszUnPackBuf, AUTH_CODE_DIGITS);
    
    return ST_SUCCESS;
}

int inUnPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    memcpy(srTransPara->szRespCode, (char *)uszUnPackBuf, RESP_CODE_SIZE);
	memcpy(srTransPara->szECRRespCode, (char *)uszUnPackBuf, RESP_CODE_SIZE);
	vdDebug_LogPrintf("AAA - inUnPackIsoFunc39(%s)", srTransPara->szRespCode);
  
    //test
    //memcpy(srTransPara->szRespCode, "00", RESP_CODE_SIZE);
    #if 0
    if(strTCT.fPrintISOMessage == TRUE){
        vdMyEZLib_HexDump("FIELD 39:", srTransPara->szRespCode, 2);
    }
    #endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    memcpy(srTransPara->szTID, (char *)uszUnPackBuf, TERMINAL_ID_BYTES);
	vdDebug_LogPrintf("inUnPackIsoFunc42(%s)", srTransPara->szTID);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
    vdMyEZLib_HexDump("FIELD 41:", srTransPara->szTID, TERMINAL_ID_BYTES);
}
	#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc44(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
      unsigned char szUnPack44data[10];
     unsigned char szField44len[2+1];
     int inField44len;
#if 1

     vdDebug_LogPrintf("AAA - inUnPackIsoFunc44[%s]", (char *)uszUnPackBuf);
      strcpy(szUnPack44data, (char *)uszUnPackBuf);

      vdDebug_LogPrintf("szUnPack44data[%02x]", szUnPack44data[0]);
      sprintf(szField44len,"%02x",szUnPack44data[0]);

	inField44len=atoi(szField44len);

	memcpy(srTransPara->szDCCOffered,&szUnPack44data[1],inField44len);
	
	vdDebug_LogPrintf("inUnPackIsoFunc44(%s)", srTransPara->szDCCOffered);



#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc50(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     BYTE szForeignCurrencyCode[10];

	   vdPCIDebug_HexPrintf("AAA >>> inUnPackIsoFunc50 uszUnPackBuf",uszUnPackBuf,3);

      //memcpy(&szForeignCurrencyCode, uszUnPackBuf, 5);
      memcpy(srTransPara->szCurrencyCode, (char*)uszUnPackBuf, 3);  
      //vdDebug_LogPrintf("AAA - uszUnPackBuf[szForeignCurrencyCode:%02x%02x%02x]]", szForeignCurrencyCode[0], szForeignCurrencyCode[1], szForeignCurrencyCode[2]);
      //memcpy(srTransPara->szCurrencyCode, &szForeignCurrencyCode[0], 3);  
      vdDebug_LogPrintf("AAA - srTransPara->szCurrencyCode[%s]", srTransPara->szCurrencyCode);
      inCSTReadCurrencySymbol(srTransPara->szCurrencyCode, srTransPara->szDCCCurrencySymbol, srTransPara->szAmountFormat);

     vdDebug_LogPrintf("strCST.inCurrencyIndex[%d]", strCST.inCurrencyIndex);
     
     srTransPara->inCSTid = strCST.inCurrencyIndex;

      vdDebug_LogPrintf("AAA - currency symbol[%s]", srTransPara->szDCCCurrencySymbol);
	   vdDebug_LogPrintf("AAA ETO- srTransPara->inCSTid[%d]", srTransPara->inCSTid);
	  
	//wub_hex_2_str(uszUnPackBuf,srTransPara->szCurrencyCode,2);
     //memcpy(srTransPara->szCurrencyCode, uszUnPackBuf, 3);
     //vdDebug_LogPrintf("inUnPackIsoFunc50(%s)", srTransPara->szCurrencyCode);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
    vdMyEZLib_HexDump("FIELD 41:", srTransPara->szTID, TERMINAL_ID_BYTES);
}
	#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc53(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	if(srTransPara->byTransType == LOG_ON)
	{		
		unsigned char szPINKey[16 + 1];
		int inIndex;
        CTOS_RTC SetRTC;
        BYTE szCurrentTime[20];
        
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        memset(szPINKey, 0x00, sizeof(szPINKey));
        memcpy(szPINKey, uszUnPackBuf, 16);
	
        vdDebug_LogPrintf("inUnPackIsoFunc53(%s)", szPINKey);
        vdDebug_LogPrintf("srTransPara->HDTid(%d)", srTransPara->HDTid);
         
        for(inIndex = 0; inIndex < 16; inIndex++)
        {
            switch(szPINKey[inIndex])
            {
                case ':' :  
                    szPINKey[inIndex] = 'A'; 
                    break;
                case ';' :  
                    szPINKey[inIndex] = 'B'; 
                    break;
                case '<' :  
                    szPINKey[inIndex] = 'C'; 
                    break;
                case '=' :  
                    szPINKey[inIndex] = 'D'; 
                    break;
                case '>' :  
                    szPINKey[inIndex] = 'E'; 
                    break;
                case '?' :  
                    szPINKey[inIndex] = 'F'; 
                    break;
                default:   
                    break;			
            }
        }

        //inDCTRead(srTransPara->HDTid);
        
		inDCTRead(srTransPara->HDTid,srTransPara->MITid);
        memcpy(strDCT.szPINKey, szPINKey, 16);
        //inDCTSave(srTransPara->HDTid);
		inDCTSave(srTransPara->HDTid,srTransPara->MITid);		 
		inTCTRead(1);
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,strTCT.szLogonDate,DATE_ASC_SIZE);
		
        inTCTSave(1);
	}
		
    return ST_SUCCESS;
}

int inUnPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    int inLen, inTotalLen, inTagLen;
	unsigned short usTag;
	int inNumber = 0, inTempNumber = 0;
	float flForeignCurrencyRate = 0;
	BYTE szTempForeignRateBuff[12+1], szTempForeignRateptr[12+1];
	char *pch;
	int inLenDecimalPoint = 0;


	vdPCIDebug_HexPrintf("AAA - inUnPackIsoFunc55 uszUnPackBuf",uszUnPackBuf,13);

	vdDebug_LogPrintf("saturn inUnPackIsoFunc55 %d %d",inLen,srTransRec.byTransType);

    inLen =((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) *100;
    inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;


    //if (inLen > 0)
    if ((inLen > 0) && (srTransRec.byTransType!=DCC_RATE_REQUEST) && (srTransRec.byTransType!=DCC_MERCH_RATE_REQ))
    {
         
         //inLen =((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) *100;
         //inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

         vdDebug_LogPrintf("**inEDC_EMV_UnPackData55(%d) START** uszUnPackBuf[%02X %02X]", inLen, uszUnPackBuf[0], uszUnPackBuf[1]);
         DebugAddHEX("DE 55", uszUnPackBuf, inLen+2);
	
    	memset(srTransPara->stEMVinfo.T8A, 0x00, sizeof(srTransPara->stEMVinfo.T8A));
    	memcpy(srTransPara->stEMVinfo.T8A, srTransPara->szRespCode, strlen(srTransPara->szRespCode));

        for (inTotalLen = 2; inTotalLen < inLen;)
        {
            usTag = (unsigned short)uszUnPackBuf[inTotalLen] * 256;
    
    	    if ((uszUnPackBuf[inTotalLen ++] & 0x1F) == 0x1F)
                usTag += ((unsigned short)uszUnPackBuf[inTotalLen ++]);

            vdDebug_LogPrintf("usTag[%X]", usTag);
            switch (usTag)
            {
                case 0x9100 :
                    memset(srTransPara->stEMVinfo.T91, 0x00, sizeof(srTransPara->stEMVinfo.T91));
                    srTransPara->stEMVinfo.T91Len = (unsigned short)uszUnPackBuf[inTotalLen ++];
                    memcpy(srTransPara->stEMVinfo.T91, (char *)&uszUnPackBuf[inTotalLen], srTransPara->stEMVinfo.T91Len);
                    inTotalLen += srTransPara->stEMVinfo.T91Len;
                    vdDebug_LogPrintf(". 91Len(%d)",srTransPara->stEMVinfo.T91Len);
                    DebugAddHEX("Tag 91", srTransPara->stEMVinfo.T91, srTransPara->stEMVinfo.T91Len);
                    ushCTOS_EMV_NewTxnDataSet(TAG_91_ARPC, srTransPara->stEMVinfo.T91Len, srTransPara->stEMVinfo.T91);
                    break;
                case 0x7100 :
                    memset(srTransPara->stEMVinfo.T71, 0x00, sizeof(srTransPara->stEMVinfo.T71));
                    srTransPara->stEMVinfo.T71Len = (unsigned short)uszUnPackBuf[inTotalLen ++];
                    srTransPara->stEMVinfo.T71Len+=2;
                    memcpy(&srTransPara->stEMVinfo.T71[0], (char *)&uszUnPackBuf[inTotalLen-2], srTransPara->stEMVinfo.T71Len);
                    inTotalLen += srTransPara->stEMVinfo.T71Len-2;
                    vdDebug_LogPrintf(". 71Len(%d)",srTransPara->stEMVinfo.T71Len);
                    DebugAddHEX("Tag 71", srTransPara->stEMVinfo.T71, srTransPara->stEMVinfo.T71Len);
                    ushCTOS_EMV_NewTxnDataSet(TAG_71, srTransPara->stEMVinfo.T71Len, srTransPara->stEMVinfo.T71);
                    break;
                case 0x7200 :
                    memset(srTransPara->stEMVinfo.T72, 0x00, sizeof(srTransPara->stEMVinfo.T72));
                    srTransPara->stEMVinfo.T72Len = (unsigned short)uszUnPackBuf[inTotalLen ++];
                    srTransPara->stEMVinfo.T72Len+=2;
                    memcpy(&srTransPara->stEMVinfo.T72[0], (char *)&uszUnPackBuf[inTotalLen-2], srTransPara->stEMVinfo.T72Len);
                    inTotalLen += srTransPara->stEMVinfo.T72Len-2;
                    vdDebug_LogPrintf(". 72Len(%d)",srTransPara->stEMVinfo.T72Len);
                    DebugAddHEX("Tag 72", srTransPara->stEMVinfo.T72, srTransPara->stEMVinfo.T72Len);
                    ushCTOS_EMV_NewTxnDataSet(TAG_72, srTransPara->stEMVinfo.T72Len, srTransPara->stEMVinfo.T72);
                    break;
                 default :
                    vdDebug_LogPrintf("**inEDC_EMV_UnPackData55(%X) Err**", usTag);
                    inTagLen = (unsigned short)uszUnPackBuf[inTotalLen ++];
                    inTotalLen += inTagLen;
                    vdDebug_LogPrintf("**inTagLen(%d) inTotalLen[%d] Err**", inTagLen, inTotalLen);
                    break;
    	    }
    	}    
    }
	else if  ((srTransRec.byTransType==DCC_RATE_REQUEST) || (srTransRec.byTransType==DCC_MERCH_RATE_REQ))
         {
         
              inLen =((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) *100;
              vdDebug_LogPrintf("AAA - inLen1[%d]",inLen );
              inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
              vdDebug_LogPrintf("AAA - inLen2[%d]",inLen );
              srTransPara->inExchangeRate_len = inLen;

			  memcpy(srTransPara->szExchangeRate, &uszUnPackBuf[2], inLen);
              
              vdDebug_LogPrintf("**AAA_UnPackData55(%d) START** uszUnPackBuf[%02X %02X]", inLen, uszUnPackBuf[0], uszUnPackBuf[1]);
              DebugAddHEX("DE 55", uszUnPackBuf, inLen+2);
             if((srTransPara->IITid == VISA_ISSUER) || (srTransPara->IITid == MASTERCARD_ISSUER))
             {
                   flForeignCurrencyRate = (1 / atof(&uszUnPackBuf[2]));
				   //memset(szTempForeignRate,0, sizeof(szTempForeignRate));
				   memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate)); 
				   memset(szTempForeignRateBuff,'\0', sizeof(gblszPrintExchangeRate)); 
				   memset(szTempForeignRateptr,'\0', sizeof(szTempForeignRateptr)); 
				   sprintf(srTransPara->szForeignRate,"%0.4f", flForeignCurrencyRate);
				   strcpy(szTempForeignRateBuff, srTransPara->szForeignRate);
				   strcpy(szTempForeignRateptr, szTempForeignRateBuff);
				   vdDebug_LogPrintf("srTransPara->szForeignRate[%s] szTempForeignRateBuff[%s]", srTransPara->szForeignRate, szTempForeignRateBuff);
				   /*memset(szTempForeignRate,'\0',sizeof(szTempForeignRate));
				      memset(szTempForeignRateBuff,'\0',sizeof(szTempForeignRateBuff));
				      strcpy(szTempForeignRate, "9999.905");
                                  strcpy(szTempForeignRateBuff, "9999.905");*/
                                  
                   #if 1 //4 //4 decimal places
                   strcpy(gblszPrintExchangeRate, srTransPara->szForeignRate);
                   vdDebug_LogPrintf("AAA - gblszPrintExchangeRate[%s]", gblszPrintExchangeRate);
				   #else //rounding off
				   pch = strtok(szTempForeignRateptr,".");
				   inLenDecimalPoint = strlen(pch);

				   switch(inLenDecimalPoint)
                   {
                       case 1:
					   	      memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 4);
							  inNumber = 3;
							  if (szTempForeignRateBuff[inNumber + 1] >= '5')
							  	{
                                   if((szTempForeignRateBuff[inNumber - 3] == '9') && (szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                       memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, strlen(szTempForeignRateBuff));
                                   }
								   else if((szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                        szTempForeignRateBuff[inNumber - 3] = szTempForeignRateBuff[inNumber - 3] + 1;
                                        szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] =  '0';
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 4);
                                   }
								   else if(szTempForeignRateBuff[inNumber] == '9')
                                   {
                                        szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] + 1;
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 4);
                                   }
							  	   else
							            gblszPrintExchangeRate[inNumber] = gblszPrintExchangeRate[inNumber] + 1;
							  	}
							  vdDebug_LogPrintf("case 1: gblszPrintExchangeRate[%s], srTransPara->szForeignRate[%s]", gblszPrintExchangeRate, srTransPara->szForeignRate);
                              break;
					   case 2:
					   	      memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 5);
							  inNumber = 4;
							  if (szTempForeignRateBuff[inNumber + 1] >= '5')
							  	{
                                   if((szTempForeignRateBuff[inNumber - 4] == '9') && (szTempForeignRateBuff[inNumber - 3] == '9') && (szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                       memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, strlen(szTempForeignRateBuff));
                                   }
								   else if((szTempForeignRateBuff[inNumber - 3] == '9') && (szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                        szTempForeignRateBuff[inNumber - 4] = szTempForeignRateBuff[inNumber - 4] + 1;
                                        szTempForeignRateBuff[inNumber - 3] = szTempForeignRateBuff[inNumber - 3] =  '0';
										szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] =  '0';
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 5);
                                   }
								   else if((szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                        szTempForeignRateBuff[inNumber - 3] = szTempForeignRateBuff[inNumber - 3] + 1;
                                        szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber] =  '0';
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 5);
                                   }
							  	   else if(szTempForeignRateBuff[inNumber] == '9')
                                   {
                                        szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] + 1;
                                        szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 5);
                                   }
							  	   else
							            gblszPrintExchangeRate[inNumber] = gblszPrintExchangeRate[inNumber] + 1;
							  	}
							  vdDebug_LogPrintf("case 2: gblszPrintExchangeRate[%s], srTransPara->szForeignRate[%s]", gblszPrintExchangeRate, srTransPara->szForeignRate);
					   	      break;
						case 3:
					   	      memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 6);
							  inNumber = 5;
							  if (szTempForeignRateBuff[inNumber + 1] >= '5')
							  	{
                                   if((szTempForeignRateBuff[inNumber - 5] == '9') && (szTempForeignRateBuff[inNumber - 4] == '9') && (szTempForeignRateBuff[inNumber - 3] == '9') && (szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                       memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, strlen(szTempForeignRateBuff));
                                   }
								   else if((szTempForeignRateBuff[inNumber - 4] == '9') && (szTempForeignRateBuff[inNumber - 3] == '9') && (szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                        szTempForeignRateBuff[inNumber - 5] = szTempForeignRateBuff[inNumber - 5] + 1;
                                        szTempForeignRateBuff[inNumber - 4] = szTempForeignRateBuff[inNumber - 4] =  '0';
                                        szTempForeignRateBuff[inNumber - 3] = szTempForeignRateBuff[inNumber - 3] =  '0';
										szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] =  '0';
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 6);
                                   }
								   else if((szTempForeignRateBuff[inNumber - 3] == '9') && (szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                        szTempForeignRateBuff[inNumber - 4] = szTempForeignRateBuff[inNumber - 4] + 1;
                                        szTempForeignRateBuff[inNumber - 3] = szTempForeignRateBuff[inNumber - 3] =  '0';
										szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] =  '0';
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 6);
                                   }
								   else if((szTempForeignRateBuff[inNumber - 1] == '9') && (szTempForeignRateBuff[inNumber] == '9'))
                                   {
                                        szTempForeignRateBuff[inNumber - 3] = szTempForeignRateBuff[inNumber - 3] + 1;
                                        szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber] =  '0';
										szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 6);
                                   }
							  	   else if(szTempForeignRateBuff[inNumber] == '9')
                                   {
                                        szTempForeignRateBuff[inNumber - 1] = szTempForeignRateBuff[inNumber - 1] + 1;
                                        szTempForeignRateBuff[inNumber] = szTempForeignRateBuff[inNumber] =  '0';
                                        memset(gblszPrintExchangeRate,'\0', sizeof(gblszPrintExchangeRate));
                                        memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, 6);
                                   }
							  	   else
							            gblszPrintExchangeRate[inNumber] = gblszPrintExchangeRate[inNumber] + 1;
							  	}
							  vdDebug_LogPrintf("case 3: gblszPrintExchangeRate, srTransPara->szForeignRate[%s]", gblszPrintExchangeRate, srTransPara->szForeignRate);
					   	      break;
					     default:
						 	  memcpy(gblszPrintExchangeRate, szTempForeignRateBuff, strlen(szTempForeignRateBuff));
							  vdDebug_LogPrintf("Default: srTransPara->szExchangeRate[%s], szTempForeignRate[%s]", gblszPrintExchangeRate, srTransPara->szForeignRate);
							  break;
						
                   }
	   			   #endif   	   
             }
              //else
                   //memcpy(srTransPara->szExchangeRate, &uszUnPackBuf[2], inLen);

              vdDebug_LogPrintf("inUnPackIsoFunc55(%s)", srTransPara->szExchangeRate);
              //vdDebug_LogPrintf("AAA -DCC_RATE_REQUEST transaction ");
              //memcpy(srTransPara->szExchangeRate,"7.7438",6);
         }
    else
    {
        inCTOS_inDisconnect();
        return (ST_ERROR);
    }

#if 0
if(strTCT.fPrintISOMessage == TRUE){
    vdMyEZLib_HexDump("FIELD 55:", uszUnPackBuf, inLen+2);
}
#endif
    return ST_SUCCESS;
}


int inUnPackIsoFunc56(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	unsigned char szFxName[100+1];
          int        inLen;
		  
	 inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
          inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

		  
		  vdPCIDebug_HexPrintf("AAA >>> inUnPackIsoFunc56 uszUnPackBuf",uszUnPackBuf,27);

	memcpy(srTransPara->szFXSourceName, &uszUnPackBuf[2], inLen);

	vdDebug_LogPrintf("AAA - inUnPackIsoFunc56[%s][%d]", srTransPara->szFXSourceName, inLen);

         //vdDebug_LogPrintf("AAA - inUnPackIsoFunc56szFxName[%s][%d]", (char*)szFxName,strlen(szFxName));
	//memcpy(srTransPara->szFXSourceName,szFxName,strlen(szFxName));
	//vdDebug_LogPrintf("srTransPara->szFXSourceName[%s]", srTransPara->szFXSourceName);
#if 0
if(strTCT.fPrintISOMessage == TRUE){
	vdMyEZLib_HexDump("FIELD 41:", srTransPara->szTID, TERMINAL_ID_BYTES);
}
#endif
	return ST_SUCCESS;
}

int inUnPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    int        inIndex = 0;
    BYTE       szTmp[32];
    int        inLen;
		BYTE szClearTMK[17] , szClearTAK[17];
   vdDebug_LogPrintf("AAA - inUnPackIsoFunc57");
if ((srTransRec.byTransType!=DCC_RATE_REQUEST) && (srTransRec.byTransType!=DCC_MERCH_RATE_REQ))
{
    if(srTransPara->byTransType != SIGN_ON)
    {
        return ST_SUCCESS;
    }
    
    inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
    inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
    vdMyEZLib_LogPrintf("**inEDC_EMV_UnPackData57(%d) START**", inLen);
    inIndex = 2;
    
    inIndex += 2;//skip version
    inIndex += 8;//skip tmk refercence
		
	inTLERead(1);
		
    if (inLen > 10)
    {
        memset(szTmp, 0x00, sizeof(szTmp));
        GET_KEY( szClearTMK , szClearTAK);        
        
        Decrypt3Des(&uszUnPackBuf[inIndex], szClearTMK, szTmp);
        memcpy(stTLE.szTermPinEncryuptKey, szTmp, 8);
        Decrypt3Des(&uszUnPackBuf[inIndex+8], szClearTMK, szTmp);
        memcpy(&stTLE.szTermPinEncryuptKey[8], szTmp, 8);
        inIndex = inIndex +16;//TPK
        
        memset(szTmp, 0x00, sizeof(szTmp));
                
        vdMyEZLib_LogPrintff(&uszUnPackBuf[inIndex],16);
                
        Decrypt3Des(&uszUnPackBuf[inIndex], szClearTMK, szTmp);
        memcpy(stTLE.szMACKey, szTmp, 8);
        Decrypt3Des(&uszUnPackBuf[inIndex+8], szClearTMK, szTmp); 
        memcpy(&stTLE.szMACKey[8], szTmp, 8);       
        inIndex = inIndex +16;//TPK
        
        vdMyEZLib_LogPrintf("szCLEar MAC**");
	
       
        vdMyEZLib_LogPrintff(&uszUnPackBuf[inIndex],16); 
        memset(szTmp, 0x00, sizeof(szTmp));
        Decrypt3Des(&uszUnPackBuf[inIndex], szClearTMK, szTmp);
        memcpy(stTLE.szLineEncryptKey, szTmp, 8);
        Decrypt3Des(&uszUnPackBuf[inIndex+8], szClearTMK, szTmp);  
        memcpy(&stTLE.szLineEncryptKey[8], szTmp, 8);
        inIndex = inIndex +16;//TPK
        
        vdMyEZLib_LogPrintf("szCLEar TPK**");
    }
    else
    {
        inCTOS_inDisconnect();
        return (ST_ERROR);
    }
    
    inTLESave(1);
    vdMyEZLib_LogPrintf("**inEDC_EMV_UnPackData57(%d) END**", inLen);
}
else
{
      inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
      inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

      memcpy(srTransPara->szExchangeRateSourceStamp, &uszUnPackBuf[2], inLen);

     vdDebug_LogPrintf("AAA - inUnPackIsoFunc57[%s]", srTransPara->szExchangeRateSourceStamp);
	  
     //vdDebug_LogPrintf("inUnpackIsoFunc57[%s]", uszUnPackBuf);
    // memcpy(srTransPara->szExchangeRateSourceStamp, "20040219 23:10",15);
    // vdDebug_LogPrintf("inUnpackIsoFunc57[%s]", srTransPara->szExchangeRateSourceStamp);
}
	
    return ST_SUCCESS;
}


int inUnPackIsoFunc58(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
      int        inLen;
        vdDebug_LogPrintf("AAA - inUnPackIsoFunc58");
      inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
      inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

       memcpy(srTransPara->szMarginRatePercentage, &uszUnPackBuf[2], inLen); //aaa temp hardcoded
       //memcpy(srTransPara->szMarginRatePercentage, "\x30\x32\x2E\x35\x30\x30\x30", 7);
       vdDebug_LogPrintf("AAA - inUnPackIsoFunc58[%s]", srTransPara->szMarginRatePercentage);
	  
	//memcpy(srTransPara->szMarginRatePercentage,"02.5000", 8);
	//vdDebug_LogPrintf("inUnPackIsoFunc58(%s)", srTransPara->szMarginRatePercentage);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
	vdMyEZLib_HexDump("FIELD 41:", srTransPara->szTID, TERMINAL_ID_BYTES);
}
#endif
	return ST_SUCCESS;
}

int inUnPackIsoFunc59(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
       int        inLen;

      vdDebug_LogPrintf("AAA - inUnPackIsoFunc59");
      inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
      inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

      memcpy(srTransPara->szCommisionPercentage, &uszUnPackBuf[2], inLen);
      vdDebug_LogPrintf("AAA - inUnPackIsoFunc59[%s]", srTransPara->szCommisionPercentage);
       //memcpy(srTransPara->szMarginRatePercentage, &uszUnPackBuf[2], inLen);
	//memcpy(srTransPara->szCommisionPercentage, "00.0000", 8);
	//vdDebug_LogPrintf("inUnPackIsoFunc59(%s)", srTransPara->szCommisionPercentage);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
	vdMyEZLib_HexDump("FIELD 41:", srTransPara->szTID, TERMINAL_ID_BYTES);
}
#endif
	return ST_SUCCESS;
}

int inUnPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	int 	   inLen;

        vdDebug_LogPrintf("AAA - inUnPackIsoFunc60");
        inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
        inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
		
	//memcpy(srTransPara->szValidHours, &uszUnPackBuf[2], inLen);
	 //vdDebug_LogPrintf("AAA - inUnPackIsoFunc60[%s]", srTransPara->szValidHours);
	//memcpy(srTransPara->szValidHours, "8", 2);
	//vdDebug_LogPrintf("inUnPackIsoFunc60(%s)", srTransPara->szValidHours);

#if 0
if(strTCT.fPrintISOMessage == TRUE){
	vdMyEZLib_HexDump("FIELD 41:", srTransPara->szTID, TERMINAL_ID_BYTES);
}
#endif
	return ST_SUCCESS;
}



int inUnPackIsoInstallment61(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    return ST_SUCCESS;
}

int inUnPackIsoFunc61(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
    int inIndex=2;
#if 1
    if(srTransPara->fLoyalty == TRUE)
    {
		/*need to adjust the length later...*/
        if(srTransPara->byTransType == LOY_BAL_INQ || srTransPara->byTransType == LOY_REDEEM_5050 || srTransPara->byTransType == LOY_REDEEM_VARIABLE)
        {
           memset(srTransPara->szLoyaltyData, 0, sizeof(srTransPara->szLoyaltyData));
           memcpy(srTransPara->szLoyaltyData, uszUnPackBuf+inIndex, 105);
        }
    }
#else
    //if(srTransPara->fLoyalty == TRUE)
    //{
		
        if(srTransPara->byTransType == LOY_BAL_INQ || srTransPara->byTransType == LOY_REDEEM_5050 || srTransPara->byTransType == LOY_REDEEM_VARIABLE)
        {
        inIndex+=2; /*skip len of loyalty data*/
        inIndex+=2; /*skip table id "90" - loyalty data*/
        
            memset(srTransPara->szLoyaltyData, 0, sizeof(srTransPara->szLoyaltyData));
            memcpy(srTransPara->szLoyaltyData, uszUnPackBuf+inIndex, 129);
            
            memcpy(srTransPara->szBeginPointBalance, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Points Balance*/         
            memcpy(srTransPara->szBeginEPurseBalance, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Peso value of points*/

        }
    //}

#endif

#if 0
if(strTCT.fPrintISOMessage == TRUE){
    //vdMyEZLib_HexDump("FIELD 61:", srTransPara->szLoyaltyData, 105);
vdMyEZLib_HexDump("FIELD 61:", uszUnPackBuf, 105);
	
}
#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     int inIndex=2;
     int inFld63Len;
     BYTE szMinorUnit[1+1];

     BYTE       szTmp[32];
     int        inLen;

    //CTOS_PrinterPutString("inUnPackIsoFunc63");
	

	//inFld63Len = atoi(uszUnPackBuf); // for testing
	
	inIndex+=2; /*skip len of Terminal serial no*/
	if(memcmp(uszUnPackBuf+inIndex, "18", 2) == 0)
	{
		inIndex+=2; /*skip table id "18" - Terminal serial no*/
		inIndex+=inLenTermSerialNo; /*skip Terminal serial no*/
	}
	else 
		inIndex-=2;

#if 0
{
	char szTerms[30+1];
	memset(szTerms,0x00,sizeof(szTerms));
	sprintf(szTerms, "[%d]:[%d]", srTransRec.byTransType, srTransRec.byOrgTransType);
	CTOS_PrinterPutString(szTerms);
	
}
#endif	

    if(srTransPara->fInstallment == TRUE || srTransPara->fCash2Go == TRUE)
    {
    
        inIndex+=2; /*skip len of installment data*/
        inIndex+=2; /*skip table id "90" - installment data*/
		
        if(srTransPara->byTransType == SALE)
        {
            memset(srTransPara->szInstallmentData, 0, sizeof(srTransPara->szInstallmentData));
            memcpy(srTransPara->szInstallmentData, uszUnPackBuf+inIndex, 129);
            
            memcpy(srTransPara->szTerms, (char *)uszUnPackBuf+inIndex, 4); inIndex+=4; /*Loan Period*/         
            memcpy(srTransPara->szFactorRate, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Interest Rate*/
            memcpy(srTransPara->szMoAmort, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Repayment Amount*/
            memcpy(srTransPara->szTotalInstAmnt, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Full amount including int, fees and taxes*/
            memcpy(srTransPara->szPlanID, (char *)uszUnPackBuf+inIndex, 10); inIndex+=10; /*Plan ID*/
            memcpy(srTransPara->szPlanType, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; /*Plan Type*/
            memcpy(srTransPara->szOffsetPeriod, (char *)uszUnPackBuf+inIndex, 4); inIndex+=4; /*Offset period*/
            memcpy(srTransPara->szOffsetType, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; /*Offset type*/
            memcpy(srTransPara->szAdjustedAmount, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Adjusted Amount*/
            memcpy(srTransPara->szInterestAmount, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Interest Amount*/
            memcpy(srTransPara->szFeeAmount, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Fee Amount*/
            memcpy(srTransPara->szTaxAmount, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Tax Amount*/
            memcpy(srTransPara->szIntFeeAmount, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Interest on Fee Amount*/
            memcpy(srTransPara->szAPR, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; /*Estimated APR*/
            memcpy(srTransPara->szWaveFromPeriod, (char *)uszUnPackBuf+inIndex, 4); inIndex+=4; /*Wave from period*/
            memcpy(srTransPara->szWaveToPeriod, (char *)uszUnPackBuf+inIndex, 4); inIndex+=4; /*Wave to Period*/			


#if 0
    	    CTOS_PrinterPutString(srTransPara->szTerms);
    	    CTOS_PrinterPutString(srTransPara->szFactorRate);
    	    CTOS_PrinterPutString(srTransPara->szMoAmort);
    	    CTOS_PrinterPutString(srTransPara->szTotalInstAmnt);
    	    CTOS_PrinterPutString(srTransPara->szPlanID);
    	    CTOS_PrinterPutString(srTransPara->szPlanType);			
    	    CTOS_PrinterPutString(srTransPara->szOffsetPeriod);
    	    CTOS_PrinterPutString(srTransPara->szOffsetType);
    	    CTOS_PrinterPutString(srTransPara->szAdjustedAmount);
    	    CTOS_PrinterPutString(srTransPara->szInterestAmount);
    	    CTOS_PrinterPutString(srTransPara->szFeeAmount);
    	    CTOS_PrinterPutString(srTransPara->szTaxAmount);			
    	    CTOS_PrinterPutString(srTransPara->szIntFeeAmount);						
    	    CTOS_PrinterPutString(srTransPara->szAPR);
    	    CTOS_PrinterPutString(srTransPara->szWaveFromPeriod);
    	    CTOS_PrinterPutString(srTransPara->szWaveToPeriod);
#endif			
			
        }
    }
    else if(srTransPara->fDebit == TRUE)
    {
    	if(srTransPara->byTransType == BAL_INQ)
	{
            memset(srTransPara->szDebitData, 0, sizeof(srTransPara->szDebitData));
            memcpy(srTransPara->szDebitData, uszUnPackBuf+inIndex, 17);    

	    memcpy(srTransPara->szBalCurrency, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // balance currency
            memcpy(srTransPara->szSpace, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Space
            memcpy(srTransPara->szBalSign, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // balance sign
            memcpy(srTransPara->szBalAmount, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // balance amount
    	}

#if 0
    	    CTOS_PrinterPutString(srTransPara->szBalCurrency);
    	    CTOS_PrinterPutString(srTransPara->szSpace);
    	    CTOS_PrinterPutString(srTransPara->szBalSign);
    	    CTOS_PrinterPutString(srTransPara->szBalAmount);
#endif			
			
	    		
    }	
    else if(srTransPara->fLoyalty == TRUE)
    {

    //CTOS_PrinterPutString("inUnPackIsoFunc63-loyalty");
        if(srTransPara->byTransType == LOY_BAL_INQ)			
        {
        inIndex+=2; /*skip len of loyalty data*/
        inIndex+=2; /*skip len of loyalty data*/
        
            memset(srTransPara->szLoyaltyData, 0, sizeof(srTransPara->szLoyaltyData));
            memcpy(srTransPara->szLoyaltyData, uszUnPackBuf+inIndex, 60);

    	    //Account 1 Type - Points Balance
	    memcpy(srTransPara->szAccount1, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // fix issue #00036  // Account type 1
            memcpy(srTransPara->szAmount1Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 1  
            memcpy(srTransPara->szAmount1type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 1 type
            memcpy(srTransPara->szDrCrFlag1, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount1, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Points Balance
            memcpy(srTransPara->szFiller1, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces            

	   //Account 2 Type - ePurse Balance
	    memcpy(srTransPara->szAccount2, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 2
            memcpy(srTransPara->szAmount2Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 2  
            memcpy(srTransPara->szAmount2type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 2 type
            memcpy(srTransPara->szDrCrFlag2, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount2, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // ePurse Balance
            memcpy(srTransPara->szFiller2, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     
			
        
        }			
        else if(srTransPara->byTransType == LOY_REDEEM_5050 || srTransPara->byTransType == LOY_REDEEM_VARIABLE)// || srTransPara->byPackType == VOID_REDEEM || srTransPara->byTransType == VOID)
        {
        inIndex+=2; /*skip len of loyalty data*/
        inIndex+=2; /*skip table id "90" - loyalty data*/

    		//CTOS_PrinterPutString("inUnPackIsoFunc63.sale");
        
            memset(srTransPara->szLoyaltyData, 0, sizeof(srTransPara->szLoyaltyData));
            memcpy(srTransPara->szLoyaltyData, uszUnPackBuf+inIndex, 180);
//jyanisu.05.28.2015
#if 0
    	    //Account 1 Type - Beginning Points Balance
	    memcpy(srTransPara->szAccount1, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // fix issue #00035 // Account type 1
            memcpy(srTransPara->szAmount1Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 1  
            memcpy(srTransPara->szAmount1type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 1 type
            memcpy(srTransPara->szDrCrFlag1, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount1, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Beginning Points Balance
            memcpy(srTransPara->szFiller1, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces            

	   //Account 2 Type - Redeemed Points
	    memcpy(srTransPara->szAccount2, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 2
            memcpy(srTransPara->szAmount2Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 2  
            memcpy(srTransPara->szAmount2type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 2 type
            memcpy(srTransPara->szDrCrFlag2, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount2, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Redeemed Points
            memcpy(srTransPara->szFiller2, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     


	   //Account 3 Type - Points Balance
	    memcpy(srTransPara->szAccount3, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 3
            memcpy(srTransPara->szAmount3Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 3
            memcpy(srTransPara->szAmount3type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 3 type
            memcpy(srTransPara->szDrCrFlag3, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount3, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Points Balance
            memcpy(srTransPara->szFiller3, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //Account 4 Type - Sales Amount
	    memcpy(srTransPara->szAccount4, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 4
            memcpy(srTransPara->szAmount4Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 4
            memcpy(srTransPara->szAmount4type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 4 type
            memcpy(srTransPara->szDrCrFlag4, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount4, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Sales Amount
            memcpy(srTransPara->szFiller4, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //Account 5 Type - Redeem Amount
	    memcpy(srTransPara->szAccount5, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 5
            memcpy(srTransPara->szAmount5Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 5
            memcpy(srTransPara->szAmount5type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 5 type
            memcpy(srTransPara->szDrCrFlag5, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount5, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Redeem Amount
            memcpy(srTransPara->szFiller5, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //Account 6 Type - Net Sales Amount
	    memcpy(srTransPara->szAccount6, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 6
            memcpy(srTransPara->szAmount6Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 6
            memcpy(srTransPara->szAmount6type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 6 type
            memcpy(srTransPara->szDrCrFlag6, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount6, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Net Sales Amount 
            memcpy(srTransPara->szFiller6, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     
#else
	    // adjust as per specs 05282015
    	    //Account 4 Type - Beginning Points Balance
	    memcpy(srTransPara->szAccount4, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // fix issue #00035 // Account type 1
            memcpy(srTransPara->szAmount4Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 1  
            memcpy(srTransPara->szAmount4type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 1 type
            memcpy(srTransPara->szDrCrFlag4, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount4, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Beginning Points Balance
            memcpy(srTransPara->szFiller4, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces            

	   //Account 5 Type - Redeemed Points
	    memcpy(srTransPara->szAccount5, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 2
            memcpy(srTransPara->szAmount5Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 2  
            memcpy(srTransPara->szAmount5type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 2 type
            memcpy(srTransPara->szDrCrFlag5, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount5, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Redeemed Points
            memcpy(srTransPara->szFiller5, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     


	   //Account 6 Type - Points Balance
	    memcpy(srTransPara->szAccount6, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 3
            memcpy(srTransPara->szAmount6Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 3
            memcpy(srTransPara->szAmount6type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 3 type
            memcpy(srTransPara->szDrCrFlag6, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount6, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Points Balance
            memcpy(srTransPara->szFiller6, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //Account 1 Type - Sales Amount
	    memcpy(srTransPara->szAccount1, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 4
            memcpy(srTransPara->szAmount1Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 4
            memcpy(srTransPara->szAmount1type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 4 type
            memcpy(srTransPara->szDrCrFlag1, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount1, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Sales Amount
            memcpy(srTransPara->szFiller1, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //Account 2 Type - Redeem Amount
	    memcpy(srTransPara->szAccount2, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 5
            memcpy(srTransPara->szAmount2Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 5
            memcpy(srTransPara->szAmount2type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 5 type
            memcpy(srTransPara->szDrCrFlag2, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount2, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Redeem Amount
            memcpy(srTransPara->szFiller2, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //Account 3 Type - Net Sales Amount
	    memcpy(srTransPara->szAccount3, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 6
            memcpy(srTransPara->szAmount3Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 6
            memcpy(srTransPara->szAmount3type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 6 type
            memcpy(srTransPara->szDrCrFlag3, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount3, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Net Sales Amount 
            memcpy(srTransPara->szFiller3, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

#endif            

//srTransPara->byPackType, srTransPara->byTransType);
        }
    }

	//if(srTransRec.byTransType == VOID && srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE || srTransRec.byOrgTransType == LOY_REDEEM_5050)
	if(srTransPara->byPackType == VOID_REDEEM)// || srTransPara->byTransType == VOID)	
	{
	        inIndex+=2; /*skip len of loyalty data*/
	        inIndex+=2; /*skip table id "90" - loyalty data*/

    		//CTOS_PrinterPutString("inUnPackIsoFunc63.Void");

#if 1
            memset(srTransPara->szAccount1, 0, sizeof(srTransPara->szAccount1));
            memset(srTransPara->szAmount1Curr, 0, sizeof(srTransPara->szAmount1Curr));
            memset(srTransPara->szAmount1type, 0, sizeof(srTransPara->szAmount1type));
            memset(srTransPara->szDrCrFlag1, 0, sizeof(srTransPara->szDrCrFlag1));
            memset(srTransPara->szAmount1, 0, sizeof(srTransPara->szAmount1));
            memset(srTransPara->szFiller1, 0, sizeof(srTransPara->szFiller1));
			
			
            memset(srTransPara->szAccount2, 0, sizeof(srTransPara->szAccount2));
            memset(srTransPara->szAmount2Curr, 0, sizeof(srTransPara->szAmount2Curr));
            memset(srTransPara->szAmount2type, 0, sizeof(srTransPara->szAmount2type));
            memset(srTransPara->szDrCrFlag2, 0, sizeof(srTransPara->szDrCrFlag2));
            memset(srTransPara->szAmount2, 0, sizeof(srTransPara->szAmount2));
            memset(srTransPara->szFiller2, 0, sizeof(srTransPara->szFiller2));
			

            memset(srTransPara->szAccount3, 0, sizeof(srTransPara->szAccount3));
            memset(srTransPara->szAmount3Curr, 0, sizeof(srTransPara->szAmount3Curr));
            memset(srTransPara->szAmount3type, 0, sizeof(srTransPara->szAmount3type));
            memset(srTransPara->szDrCrFlag3, 0, sizeof(srTransPara->szDrCrFlag3));
            memset(srTransPara->szAmount3, 0, sizeof(srTransPara->szAmount3));
            memset(srTransPara->szFiller3, 0, sizeof(srTransPara->szFiller3));

            memset(srTransPara->szAccount4, 0, sizeof(srTransPara->szAccount4));
            memset(srTransPara->szAmount4Curr, 0, sizeof(srTransPara->szAmount4Curr));
            memset(srTransPara->szAmount4type, 0, sizeof(srTransPara->szAmount4type));
            memset(srTransPara->szDrCrFlag4, 0, sizeof(srTransPara->szDrCrFlag4));
            memset(srTransPara->szAmount4, 0, sizeof(srTransPara->szAmount4));
            memset(srTransPara->szFiller4, 0, sizeof(srTransPara->szFiller4));
			
			
            memset(srTransPara->szAccount5, 0, sizeof(srTransPara->szAccount5));
            memset(srTransPara->szAmount5Curr, 0, sizeof(srTransPara->szAmount5Curr));
            memset(srTransPara->szAmount5type, 0, sizeof(srTransPara->szAmount5type));
            memset(srTransPara->szDrCrFlag5, 0, sizeof(srTransPara->szDrCrFlag5));
            memset(srTransPara->szAmount5, 0, sizeof(srTransPara->szAmount5));
            memset(srTransPara->szFiller5, 0, sizeof(srTransPara->szFiller5));
			

            memset(srTransPara->szAccount6, 0, sizeof(srTransPara->szAccount6));
            memset(srTransPara->szAmount6Curr, 0, sizeof(srTransPara->szAmount6Curr));
            memset(srTransPara->szAmount6type, 0, sizeof(srTransPara->szAmount6type));
            memset(srTransPara->szDrCrFlag6, 0, sizeof(srTransPara->szDrCrFlag6));
            memset(srTransPara->szAmount6, 0, sizeof(srTransPara->szAmount6));
            memset(srTransPara->szFiller6, 0, sizeof(srTransPara->szFiller6));
			
#endif

	   //BEGINNING BALANCE POINTS
	    memcpy(srTransPara->szAccount1, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 4
            memcpy(srTransPara->szAmount1Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 4
            memcpy(srTransPara->szAmount1type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 4 type
            memcpy(srTransPara->szDrCrFlag1, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount1, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Sales Amount
            memcpy(srTransPara->szFiller1, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //POINTS REVERSED
	    memcpy(srTransPara->szAccount2, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 5
            memcpy(srTransPara->szAmount2Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 5
            memcpy(srTransPara->szAmount2type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 5 type
            memcpy(srTransPara->szDrCrFlag2, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount2, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Redeem Amount
            memcpy(srTransPara->szFiller2, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

	   //ENDING BALANCE: POINTS
	    memcpy(srTransPara->szAccount3, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 6
            memcpy(srTransPara->szAmount3Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 6
            memcpy(srTransPara->szAmount3type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 6 type
            memcpy(srTransPara->szDrCrFlag3, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount3, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Net Sales Amount 
            memcpy(srTransPara->szFiller3, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     

    	    //Account 4 Type - AMOUNT REVERSED
	    memcpy(srTransPara->szAccount4, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // fix issue #00035 // Account type 1
            memcpy(srTransPara->szAmount4Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 1  
            memcpy(srTransPara->szAmount4type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 1 type
            memcpy(srTransPara->szDrCrFlag4, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount4, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Beginning Points Balance
            memcpy(srTransPara->szFiller4, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces            

	   //Account 5 Type - AMOUNT REVERSED pts
	    memcpy(srTransPara->szAccount5, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 2
            memcpy(srTransPara->szAmount5Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 2  
            memcpy(srTransPara->szAmount5type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 2 type
            memcpy(srTransPara->szDrCrFlag5, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount5, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Redeemed Points
            memcpy(srTransPara->szFiller5, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     


	   //Account 6 Type - Amount reversed to card
	    memcpy(srTransPara->szAccount6, (char *)uszUnPackBuf+inIndex, 2); inIndex+=2; // Account type 3
            memcpy(srTransPara->szAmount6Curr, (char *)uszUnPackBuf+inIndex, 3); inIndex+=3; // Amount Currency 3
            memcpy(srTransPara->szAmount6type, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Amount 3 type
            memcpy(srTransPara->szDrCrFlag6, (char *)uszUnPackBuf+inIndex, 1); inIndex+=1; // Debit/Credit Flag
            memcpy(srTransPara->szAmount6, (char *)uszUnPackBuf+inIndex, 12); inIndex+=12; // Points Balance
            memcpy(srTransPara->szFiller6, (char *)uszUnPackBuf+inIndex, 11); inIndex+=11; // spaces     
			
			
	
	}

#if 0
    	    CTOS_PrinterPutString(srTransPara->szAccount1);
    	    CTOS_PrinterPutString(srTransPara->szAmount1Curr);
    	    CTOS_PrinterPutString(srTransPara->szAmount1type);
    	    CTOS_PrinterPutString(srTransPara->szDrCrFlag1);
    	    CTOS_PrinterPutString(srTransPara->szAmount1);
    	    CTOS_PrinterPutString(srTransPara->szFiller1);
			
    	    CTOS_PrinterPutString(srTransPara->szAccount2);
    	    CTOS_PrinterPutString(srTransPara->szAmount2Curr);
    	    CTOS_PrinterPutString(srTransPara->szAmount2type);
    	    CTOS_PrinterPutString(srTransPara->szDrCrFlag2);
    	    CTOS_PrinterPutString(srTransPara->szAmount2);
    	    CTOS_PrinterPutString(srTransPara->szFiller2);
			
    	    CTOS_PrinterPutString(srTransPara->szAccount3);
    	    CTOS_PrinterPutString(srTransPara->szAmount3Curr);
    	    CTOS_PrinterPutString(srTransPara->szAmount3type);
    	    CTOS_PrinterPutString(srTransPara->szDrCrFlag3);
    	    CTOS_PrinterPutString(srTransPara->szAmount3);
    	    CTOS_PrinterPutString(srTransPara->szFiller3);
			
    	    CTOS_PrinterPutString(srTransPara->szAccount4);
    	    CTOS_PrinterPutString(srTransPara->szAmount4Curr);
    	    CTOS_PrinterPutString(srTransPara->szAmount4type);
    	    CTOS_PrinterPutString(srTransPara->szDrCrFlag4);
    	    CTOS_PrinterPutString(srTransPara->szAmount4);
    	    CTOS_PrinterPutString(srTransPara->szFiller4);

    	    CTOS_PrinterPutString(srTransPara->szAccount5);
    	    CTOS_PrinterPutString(srTransPara->szAmount5Curr);
    	    CTOS_PrinterPutString(srTransPara->szAmount5type);
    	    CTOS_PrinterPutString(srTransPara->szDrCrFlag5);
    	    CTOS_PrinterPutString(srTransPara->szAmount5);
    	    CTOS_PrinterPutString(srTransPara->szFiller5);

    	    CTOS_PrinterPutString(srTransPara->szAccount6);
    	    CTOS_PrinterPutString(srTransPara->szAmount6Curr);
    	    CTOS_PrinterPutString(srTransPara->szAmount6type);
    	    CTOS_PrinterPutString(srTransPara->szDrCrFlag6);
    	    CTOS_PrinterPutString(srTransPara->szAmount6);
    	    CTOS_PrinterPutString(srTransPara->szFiller6);
#endif			

	#if 0
	if(strTCT.fPrintISOMessage == TRUE)
	    vdMyEZLib_HexDump("FIELD 63:", uszUnPackBuf, inIndex);
#endif

    if ((srTransRec.byTransType==DCC_RATE_REQUEST) || (srTransRec.byTransType==DCC_MERCH_RATE_REQ))
    {
         vdDebug_LogPrintf("AAA - inUnPackIsoFunc63");
         inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
         inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
         
         memcpy(szMinorUnit, &uszUnPackBuf[2], inLen); 
         srTransPara->inMinorUnit=atoi(szMinorUnit);
        //srTransPara->inMinorUnit=atoi("\x32"); //aaa temp hardcoded
        
         vdDebug_LogPrintf("AAA - inUnPackIsoFunc63[][%d]inLen[%d]", srTransPara->inMinorUnit, inLen);
    }
    return ST_SUCCESS;
}

int inPackISOEMVData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
    char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;

    
	DebugAddSTR("load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOEMVData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

    {	
	   
        szPacket[inPacketCnt ++] = 0x5F;
        szPacket[inPacketCnt ++] = 0x2A;
        szPacket[inPacketCnt ++] = 2;
        if (strTCT.fRegUSD == 0)
        {
             memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
			 inPacketCnt += 2;
        }
        else // FOR USD CURRENCY
        {
             szPacket[inPacketCnt ++] = 0x08; 
             szPacket[inPacketCnt ++] = 0x40;
        }
        

        DebugAddINT("5F34 Len",srTransPara->stEMVinfo.T5F34_len );
        if(srTransPara->stEMVinfo.T5F34_len > 0)
        {
	        szPacket[inPacketCnt ++] = 0x5F;
	        szPacket[inPacketCnt ++] = 0x34;
	        szPacket[inPacketCnt ++] = 1;
	        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T5F34;
			DebugAddSTR("EMV tag","5f34--finish--",2);
        }

        
        szPacket[inPacketCnt ++] = 0x82;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

        
        szPacket[inPacketCnt ++] = 0x84;		
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;

       
        szPacket[inPacketCnt ++] = 0x95;
        szPacket[inPacketCnt ++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        
        szPacket[inPacketCnt ++] = 0x9A;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        
        szPacket[inPacketCnt ++] = 0x9C;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;  // SL check again  //spec said 2 byte

       
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x02;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x03;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x09;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x10;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;
        
        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x1A;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;
        
        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x1E;
        szPacket[inPacketCnt ++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x26;
        szPacket[inPacketCnt ++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;

        DebugAddHEX("srTransPara->.stEMVinfo.T9F26",srTransPara->stEMVinfo.T9F26, 3);
		
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x27;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x33;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x34;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;

        DebugAddHEX("srTransPara->.stEMVinfo.T9F34",srTransPara->stEMVinfo.T9F34, 3);
		
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x35;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x36;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x37;
        szPacket[inPacketCnt ++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

        
        //Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - start -- jzg
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x41;
        if(strTCT.fEMVOnlinePIN == FALSE)
        {
            szPacket[inPacketCnt ++] = 3;
            memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3);// get chip transaction counter
            inPacketCnt += 3;		 
        }
        else
        {
            szPacket[inPacketCnt ++] = 4;
            memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 4);// get chip transaction counter
            inPacketCnt += 4;		
			DebugAddHEX("srTransPara->.stEMVinfo.T9F41",srTransPara->stEMVinfo.T9F41,4);
        }
        //Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - end -- jzg					
    }
	
    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
    
    vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOEMVData END**");
    return (inDataCnt);
}


int inPackISOPayWaveData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
    char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;

    
	DebugAddSTR("load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOPayWaveData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

    {	
	    szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x02;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x03;
        szPacket[inPacketCnt ++] = 6;
        //memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
		memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
        inPacketCnt += 6;

		szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x26;
        szPacket[inPacketCnt ++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;
		
        szPacket[inPacketCnt ++] = 0x82;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;
	
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x36;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;
		
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x10;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;
		
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x33;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;
		
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x1A;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt ++] = 0x95;
        szPacket[inPacketCnt ++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        szPacket[inPacketCnt ++] = 0x5F;
        szPacket[inPacketCnt ++] = 0x2A;
        szPacket[inPacketCnt ++] = 2;
        if (strTCT.fRegUSD == 0)
        {
             memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
			 inPacketCnt += 2;
        }
        else // FOR USD CURRENCY
        {
             szPacket[inPacketCnt ++] = 0x08; 
             szPacket[inPacketCnt ++] = 0x40;
        }

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x27;
		szPacket[inPacketCnt ++] = 1;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x34;
		szPacket[inPacketCnt ++] = 3;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
		inPacketCnt += 3;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x35;
		szPacket[inPacketCnt ++] = 1;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;

		szPacket[inPacketCnt ++] = 0x84;		
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
		inPacketCnt += srTransPara->stEMVinfo.T84_len;


        szPacket[inPacketCnt ++] = 0x9A;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt ++] = 0x9C;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;  // SL check again  //spec said 2 byte

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x37;
        szPacket[inPacketCnt ++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

        //visa form factor indicator
        if (srTransPara->stEMVinfo.T9F6E_len > 0)
        {
            szPacket[inPacketCnt ++] = 0x9F;		
            szPacket[inPacketCnt ++] = 0x6E;		
            szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F6E_len;
            memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F6E, srTransPara->stEMVinfo.T9F6E_len);
            inPacketCnt += srTransPara->stEMVinfo.T9F6E_len;
        }
        //visa form factor indicator
    }
	
    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
    
    vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOPayWaveData END**");
    return (inDataCnt);
}


int inPackISOPayPassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{ 
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
	char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;

	
	DebugAddSTR("load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOPayWaveData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

	{
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x26;
		szPacket[inPacketCnt ++] = 8;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
		inPacketCnt += 8;
	
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x27;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x10;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
		inPacketCnt += srTransPara->stEMVinfo.T9F10_len;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x34;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x35;
		szPacket[inPacketCnt ++] = 1;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;
	
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x37;
		szPacket[inPacketCnt ++] = 4;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
		inPacketCnt += 4;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x36;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

		szPacket[inPacketCnt ++] = 0x84;		
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
		inPacketCnt += srTransPara->stEMVinfo.T84_len;


		szPacket[inPacketCnt ++] = 0x95;
		szPacket[inPacketCnt ++] = 5;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
		inPacketCnt += 5;

		szPacket[inPacketCnt ++] = 0x9A;
		szPacket[inPacketCnt ++] = 3;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
		inPacketCnt += 3;

		szPacket[inPacketCnt ++] = 0x9C;
		szPacket[inPacketCnt ++] = 1;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;	// SL check again  //spec said 2 byte

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x02;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

		szPacket[inPacketCnt ++] = 0x5F;
		szPacket[inPacketCnt ++] = 0x2A;
		szPacket[inPacketCnt ++] = 2;
		if (strTCT.fRegUSD == 0)
        {
             memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
			 inPacketCnt += 2;
        }
        else // FOR USD CURRENCY
        {
             szPacket[inPacketCnt ++] = 0x08; 
             szPacket[inPacketCnt ++] = 0x40;
        }

		szPacket[inPacketCnt ++] = 0x82;
		szPacket[inPacketCnt ++] = 2;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
		inPacketCnt += 2;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x1A;
		szPacket[inPacketCnt ++] = 2;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
		inPacketCnt += 2;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x33;
		szPacket[inPacketCnt ++] = 3;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
		inPacketCnt += 3;
		
		
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x03;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
        inPacketCnt += 6;
	}
	
	/* Packet Data Length */
	memset(szAscBuf, 0, sizeof(szAscBuf));
	sprintf(szAscBuf, "%04d", inPacketCnt);
	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
	inDataCnt += 2;
	/* Packet Data */
	memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
	inDataCnt += inPacketCnt;
	
	vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
	vdMyEZLib_LogPrintf("**inPackISOPayWaveData END**");
	return (inDataCnt);
}



int inCTOS_PackDemoResonse(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
    int inPackLen;
    BYTE szSTAN[6+1];
    BYTE szTID[TERMINAL_ID_BYTES+1];
    BYTE szMTI[MTI_BCD_SIZE+1];
    BYTE szBitMap[8+1];
    BYTE szCurrentTime[20];
    CTOS_RTC SetRTC;

    //default response turn on 3, 11, 12, 13, 24, 37, 38, 39, 41
    memset(szBitMap, 0x00, sizeof(szBitMap));
    memcpy(szBitMap, "\x20\x38\x01\x00\x0E\x80\x00\x00", 8);

    inPackLen = 0;
    
    memcpy(&uszRecData[inPackLen], "\x60\x00\x01\x00\x00",TPDU_BCD_SIZE);
    inPackLen += TPDU_BCD_SIZE;

    memcpy(szMTI, "\x02\x10", MTI_BCD_SIZE);
    szMTI[1] |= 0x10;
    memcpy(&uszRecData[inPackLen], szMTI, MTI_BCD_SIZE);
    inPackLen += MTI_BCD_SIZE;

    memcpy(&uszRecData[inPackLen], szBitMap, 8);
    inPackLen += 8;

    //DE 3
    memcpy(&uszRecData[inPackLen], "\x00\x00\x00", PRO_CODE_BCD_SIZE);
    inPackLen += PRO_CODE_BCD_SIZE;

    //DE 11
    sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
    wub_str_2_hex(&szSTAN[0], (char *)&uszRecData[inPackLen], 6);
    inPackLen += 3;

    //DE 12
    CTOS_RTCGet(&SetRTC);
    memset(szCurrentTime, 0x00, sizeof(szCurrentTime));
    sprintf(szCurrentTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
    wub_str_2_hex(&szCurrentTime[0], (char *)&uszRecData[inPackLen], 6);
    inPackLen += 3;

    //DE 13
    memset(szCurrentTime, 0x00, sizeof(szCurrentTime));
    sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth,SetRTC.bDay);
    wub_str_2_hex(&szCurrentTime[0], (char *)&uszRecData[inPackLen], 4);
    inPackLen += 2;

    //DE 24
    memcpy((char *)&uszRecData[inPackLen], strHDT.szNII, 2);
    inPackLen += 2;

    //DE 37
    memcpy((char *)&uszRecData[inPackLen], "111111111111", 12);
    inPackLen += 12;

    //DE 38
    if(0 == strlen(srTransPara->szAuthCode))
        memcpy((char *)&uszRecData[inPackLen], "123456", 6);
    else
        memcpy((char *)&uszRecData[inPackLen], srTransPara->szAuthCode, 6);
    inPackLen += 6;

    //DE 39
    memcpy((char *)&uszRecData[inPackLen], "00", 2);
    inPackLen += 2;

    //DE 41
    memset(szTID, 0x00, sizeof(szTID));
    memset(szTID, 0x20, TERMINAL_ID_BYTES);
    memcpy(szTID, srTransPara->szTID, strlen(srTransPara->szTID));
    memcpy((char *)&uszRecData[inPackLen], szTID, TERMINAL_ID_BYTES);
    inPackLen += TERMINAL_ID_BYTES;
    
    return inPackLen;
}

void vdInitialISOFunction(ISO_FUNC_TABLE *srPackFunc)
{
	/* Choose ISO_FUNC_TABLE Array */
	memcpy((char *)srPackFunc, (char *)&srIsoFuncTable[0], sizeof(ISO_FUNC_TABLE));
}

int inPackISOExpressPayData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
	char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;


	DebugAddSTR("load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOExpressPayData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

	{	
		vdDebug_LogPrintf("5F34 Len [%d]", srTransPara->stEMVinfo.T5F34_len);
		if(srTransPara->stEMVinfo.T5F34_len > 0)
		{
			szPacket[inPacketCnt ++] = 0x5F;
			szPacket[inPacketCnt ++] = 0x34;
			szPacket[inPacketCnt ++] = 1;
			szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T5F34;
			vdDebug_LogPrintf("srTransPara->stEMVinfo.T5F34 [%02X]", srTransPara->stEMVinfo.T5F34);
		}
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x02;
		szPacket[inPacketCnt ++] = 6;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
		inPacketCnt += 6;


		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x03;
		szPacket[inPacketCnt ++] = 6;
		//memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
		memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
		inPacketCnt += 6;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x26;
		szPacket[inPacketCnt ++] = 8;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
		inPacketCnt += 8;

		szPacket[inPacketCnt ++] = 0x82;
		szPacket[inPacketCnt ++] = 2;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
		inPacketCnt += 2;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x36;
		szPacket[inPacketCnt ++] = 2;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
		inPacketCnt += 2;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x10;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
		inPacketCnt += srTransPara->stEMVinfo.T9F10_len;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x33;
		szPacket[inPacketCnt ++] = 3;
		//memcpy(&szPacket[inPacketCnt], "\xE0\xB0\xC8", 3);
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
		inPacketCnt += 3;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x1A;
		szPacket[inPacketCnt ++] = 2;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
		inPacketCnt += 2;

		szPacket[inPacketCnt ++] = 0x95;
		szPacket[inPacketCnt ++] = 5;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
		inPacketCnt += 5;

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x27;
		szPacket[inPacketCnt ++] = 1;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x34;
		szPacket[inPacketCnt ++] = 3;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
		inPacketCnt += 3;
		
		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x35;
		szPacket[inPacketCnt ++] = 1;
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;
				
		szPacket[inPacketCnt ++] = 0x84;		
		szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
		inPacketCnt += srTransPara->stEMVinfo.T84_len;
		
		szPacket[inPacketCnt ++] = 0x5F;
		szPacket[inPacketCnt ++] = 0x2A;
		szPacket[inPacketCnt ++] = 2;
		if (strTCT.fRegUSD == 0)
        {
             memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
			 inPacketCnt += 2;
        }
        else // FOR USD CURRENCY
        {
             szPacket[inPacketCnt ++] = 0x08; 
             szPacket[inPacketCnt ++] = 0x40;
        }

		szPacket[inPacketCnt ++] = 0x9A;
		szPacket[inPacketCnt ++] = 3;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
		inPacketCnt += 3;

		szPacket[inPacketCnt ++] = 0x9C;
		szPacket[inPacketCnt ++] = 1;
		/* MCC: Quasi should be parametrized per issuer - start -- jzg */
		//if((strIIT.fQuasiCash)	&& 
			//((srTransPara->byTransType == SALE) || (srTransPara->byTransType == SALE_OFFLINE)))
			//szPacket[inPacketCnt ++] = 0x11;
		/* MCC: Quasi should be parametrized per issuer - end -- jzg */
		//else
			szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;	// SL check again  //spec said 2 byte
		//Issue# 000141 - end -- jzg	

		szPacket[inPacketCnt ++] = 0x9F;
		szPacket[inPacketCnt ++] = 0x37;
		szPacket[inPacketCnt ++] = 4;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
		inPacketCnt += 4;
	}

	/* Packet Data Length */
	memset(szAscBuf, 0, sizeof(szAscBuf));
	sprintf(szAscBuf, "%04d", inPacketCnt);
	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
	inDataCnt += 2;
	
	/* Packet Data */
	memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
	inDataCnt += inPacketCnt;

	vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
	vdMyEZLib_LogPrintf("**inPackISOExpressPayData END**");
	return (inDataCnt);
}


int inPackISOQuickpassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
    char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;

    
	DebugAddSTR("load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOQuickpassData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

  {

//remove later --jzg
vdDebug_LogPrintf("inPackISOEMVData:: txn type = [%d]", srTransPara->byTransType);
vdDebug_LogPrintf("inPackISOEMVData:: pack type = [%d]", srTransPara->byPackType);
		
		if(srTransPara->byPackType != REVERSAL)
		{
        szPacket[inPacketCnt ++] = 0x5F;
        szPacket[inPacketCnt ++] = 0x2A;
        szPacket[inPacketCnt ++] = 2;
        if (strTCT.fRegUSD == 0)
        {
             memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
			 inPacketCnt += 2;
        }
        else // FOR USD CURRENCY
        {
             szPacket[inPacketCnt ++] = 0x08; 
             szPacket[inPacketCnt ++] = 0x40;
        }
   
#if 0
        DebugAddINT("5F34 Len",srTransPara->stEMVinfo.T5F34_len );
        if(srTransPara->stEMVinfo.T5F34_len > 0)
        {
	        szPacket[inPacketCnt ++] = 0x5F;
	        szPacket[inPacketCnt ++] = 0x34;
	        szPacket[inPacketCnt ++] = 1;
	        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T5F34;
			DebugAddSTR("EMV tag","5f34--finish--",2);
        }
#endif
        
        szPacket[inPacketCnt ++] = 0x82;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

        
        szPacket[inPacketCnt ++] = 0x84;		
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;
		}
		
    szPacket[inPacketCnt ++] = 0x95;
    szPacket[inPacketCnt ++] = 5;
    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
    inPacketCnt += 5;

		if(srTransPara->byPackType != REVERSAL)
		{
        szPacket[inPacketCnt ++] = 0x9A;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        
        szPacket[inPacketCnt ++] = 0x9C;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;  // SL check again  //spec said 2 byte

       
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x02;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x03;
        szPacket[inPacketCnt ++] = 6;
        //memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x09;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;
  	}
    
    szPacket[inPacketCnt ++] = 0x9F;
    szPacket[inPacketCnt ++] = 0x10;
    szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
    inPacketCnt += srTransPara->stEMVinfo.T9F10_len;
    
		if(srTransPara->byPackType != REVERSAL)
		{
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x1A;
      szPacket[inPacketCnt ++] = 2;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
      inPacketCnt += 2;
      
      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x1E;
      szPacket[inPacketCnt ++] = 8;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
      inPacketCnt += 8;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x26;
      szPacket[inPacketCnt ++] = 8;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
      inPacketCnt += 8;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x27;
      szPacket[inPacketCnt ++] = 1;
      szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x33;
      szPacket[inPacketCnt ++] = 3;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
      inPacketCnt += 3;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x34;
      szPacket[inPacketCnt ++] = 3;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
      inPacketCnt += 3;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x35;
      szPacket[inPacketCnt ++] = 1;
      szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;
		}
		
    szPacket[inPacketCnt ++] = 0x9F;
    szPacket[inPacketCnt ++] = 0x36;
    szPacket[inPacketCnt ++] = 2;
    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
    inPacketCnt += 2;

		if(srTransPara->byPackType != REVERSAL)
		{
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x37;
      szPacket[inPacketCnt ++] = 4;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
      inPacketCnt += 4;
      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x41;
      szPacket[inPacketCnt ++] = 3;

      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3);// get chip transaction counter
      inPacketCnt += 3;        
		}
	}
	
  /* Packet Data Length */
  memset(szAscBuf, 0, sizeof(szAscBuf));
  sprintf(szAscBuf, "%04d", inPacketCnt);
  memset(szBcdBuf, 0, sizeof(szBcdBuf));
  wub_str_2_hex(szAscBuf, szBcdBuf, 4);
  memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
  inDataCnt += 2;
  /* Packet Data */
  memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
  inDataCnt += inPacketCnt;
  
  vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
  vdMyEZLib_LogPrintf("**inPackISOEMVData END**");
  return (inDataCnt);
}


int inPackISOJCBCtlsData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
     
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
    char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;
    
	DebugAddSTR("load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOJCBCtlsData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;

    {	

        szPacket[inPacketCnt ++] = 0x5F;
        szPacket[inPacketCnt ++] = 0x2A;
        szPacket[inPacketCnt ++] = 2;
        if (strTCT.fRegUSD == 0)
        {
             memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
			 inPacketCnt += 2;
        }
        else // FOR USD CURRENCY
        {
             szPacket[inPacketCnt ++] = 0x08; 
             szPacket[inPacketCnt ++] = 0x40;
        }


        DebugAddINT("5F34 Len",srTransPara->stEMVinfo.T5F34_len );
        if(srTransPara->stEMVinfo.T5F34_len > 0)
        {
	        szPacket[inPacketCnt ++] = 0x5F;
	        szPacket[inPacketCnt ++] = 0x34;
	        szPacket[inPacketCnt ++] = 1;
	        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T5F34;
			DebugAddSTR("EMV tag","5f34--finish--",2);
        }
		szPacket[inPacketCnt ++] = 0x82;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt ++] = 0x84;		
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;

        szPacket[inPacketCnt ++] = 0x95;
        szPacket[inPacketCnt ++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        szPacket[inPacketCnt ++] = 0x9A;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt ++] = 0x9C;
        szPacket[inPacketCnt ++] = 1;

				/* MCC: Quasi should be parametrized per issuer - start -- jzg */
				//if((strIIT.fQuasiCash)	&& 
				//	((srTransPara->byTransType == SALE) || (srTransPara->byTransType == SALE_OFFLINE)))
				//	szPacket[inPacketCnt ++] = 0x11;
				/* MCC: Quasi should be parametrized per issuer - end -- jzg */
				
				//Issue# 000143 - start -- jzg
				if(srTransPara->byTransType == PRE_AUTH)
					szPacket[inPacketCnt ++] = 0x00;
				//Issue# 000143 - end -- jzg
				else
					szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;	// SL check again  //spec said 2 byte
				//Issue# 000141 - end -- jzg	

#if 0
        szPacket[inPacketCnt ++] = 0x5F;
        szPacket[inPacketCnt ++] = 0x2A;
        szPacket[inPacketCnt ++] = 2;
        //memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);//test
        memcpy(&szPacket[inPacketCnt], "\x06\x08", 2);//test
        inPacketCnt += 2;
#endif
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x02;
        szPacket[inPacketCnt ++] = 6;
		
        //memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x03;
        szPacket[inPacketCnt ++] = 6;
        //memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x09;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;
		 //temp remove

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x10;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;
      
	  	szPacket[inPacketCnt ++] = 0x9F;
	  	szPacket[inPacketCnt ++] = 0x1A;
	  	szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
	  	inPacketCnt += 2;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x1E;
        szPacket[inPacketCnt ++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x26;
        szPacket[inPacketCnt ++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x27;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x33;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x34;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x35;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x36;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x37;
        szPacket[inPacketCnt ++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;


        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x41;
        szPacket[inPacketCnt ++] = 3;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3);// get chip transaction counter
        inPacketCnt += 3;        
		
		//gcitra

        /*
        DebugAddINT("5F34 Len",srTransPara->stEMVinfo.T5F34_len );
        if(srTransPara->stEMVinfo.T5F34_len > 0)
        {
	        szPacket[inPacketCnt ++] = 0x5F;
	        szPacket[inPacketCnt ++] = 0x34;
	        szPacket[inPacketCnt ++] = 1;
	        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T5F34;
			DebugAddSTR("EMV tag","5f34--finish--",2);
        }
        */

       
    }
	
    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof(szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof(szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;
    
    vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOEMVData END**");
    return (inDataCnt);
}

