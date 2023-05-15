#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <vwdleapi.h>
#include <sqlite3.h>


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
#include "..\Includes\Wub_lib.h"
#include "..\Database\DatabaseFunc.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\CardUtil.h"
#include "..\Includes\POSSetting.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"


#include "POSCtls.h"
#include "POSWave.h"
#include "../Includes/LocalAptrans.h"
#include "../Erm/PosErm.h"
#include "../Includes/myEZLib.h"
#include "../Includes/POSDiag.h"
#include "../Includes/POSTrans.h"
#include "../Includes/POSTypedef.h"
#include "../Includes/POSSale.h"
#include "../Debug/debug.h"
#include "../Includes/POSSetting.h"
#include "../Includes/POSHost.h"
#include "../Ui/Display.h"
#include "../Accum/accum.h"
#include "../Comm/V5Comm.h"

sqlite3 * dbWave;
sqlite3_stmt *stmtWave;

int inWaveTransType = 0;
BOOL fECRApproved=FALSE;
extern int inSaleGroupIndex;
extern BOOL fEntryCardfromIDLE;
extern BOOL ErmTrans_Approved;
//extern BOOL gblDCCTrans;


void vdCTOSS_SetWaveTransType(int type)
{
	inWaveTransType = type;
}

int inCTOSS_GetWaveTransType(void)
{
	return inWaveTransType;
}

int inCTOSS_CLMSetCustomEMVTags(void)
{
	//sqlite3 * dbWave;
	//sqlite3_stmt *stmtWave;

	int inStmtSeqWave = 0;
	BYTE baBuf[16+1];

	BYTE buf[100];
	UINT len;
	ULONG rtn;
 	char setbuf[100];
	int inlen=0;

	char szName[100+1];
	char szTag[10+1];
	char szValue[200+1];
	char szTagHex[10+1];
	char szValueHex[200+1];
	char inbuf[1024];
	int inBufLen = 0;	
	int inTaglen = 0;
	int inValuelen = 0;

	int result;
	int inResult = d_NO;
	char *sql = "SELECT szName, szTag, szValue FROM TAGS";

	vdDebug_LogPrintf("inCTOSS_CLMSetCustomEMVTags"); 
	inCTOSS_CLMOpenAndGetVersion();
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		rtn = MsessionAuthenAdmin();
		vdDebug_LogPrintf("rtn =[%x]........",rtn);
		if (rtn != d_NO_ERROR) 
		{
			vdDisplayErrorMsg(1, 8, "Authen Fail");
			return ST_ERROR;
		}
		
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_ADMIN,baBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,baBuf);
	}

	/* open the database */
	result = sqlite3_open(DB_WAVE,&dbWave);
	if (result != SQLITE_OK) {
		sqlite3_close(dbWave);
		return 1;
	}
	
	sqlite3_exec( dbWave, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(dbWave, sql, -1, &stmtWave, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(dbWave);
		return 2;
	}
	
	inStmtSeqWave = 0;

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmtWave);
		
		//vdDebug_LogPrintf("strAIDT.result[%d]AIDid[%d]",result,inSeekCnt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeqWave = 0;
			
			/* szName*/
			strcpy((char*)szName, (char *)sqlite3_column_text(stmtWave,inStmtSeqWave));
			/* szTag*/
			strcpy((char*)szTag, (char *)sqlite3_column_text(stmtWave,inStmtSeqWave +=1 ));
			/* szValue*/
			strcpy((char*)szValue, (char *)sqlite3_column_text(stmtWave,inStmtSeqWave +=1 ));

			vdDebug_LogPrintf("szName=[%s].szTag=[%s].szValue=[%s]",szName, szTag, szValue);

			wub_str_2_hex((unsigned char *)szTag, (unsigned char *)szTagHex, strlen((char *)szTag));
			wub_str_2_hex((unsigned char *)szValue, (unsigned char *)szValueHex, strlen((char *)szValue));

			inTaglen = strlen((char *)szTag)/2;
			inValuelen = strlen((char *)szValue)/2;
			
			DebugAddHEX("szTagHex", szTagHex, inTaglen);
			DebugAddHEX("szValueHex", szValueHex, inValuelen);

			inBufLen = 0;
			memset(inbuf, 0x00, sizeof(inbuf));

			inbuf[inBufLen++] = 0x01;

			if (inTaglen == 1)
            {
                inbuf[inBufLen++] = szTag[0];                            
            }
            else
            {
				memcpy(&inbuf[inBufLen], szTagHex, inTaglen);
				inBufLen = inBufLen + inTaglen;
            }

			inbuf[inBufLen++] = inValuelen;
			
            if (inValuelen == 1)
            {
                    inbuf[inBufLen++] = szValueHex[0];                            
                    inlen =  inBufLen;
            }
            else
            {
                    memcpy(&inbuf[inBufLen], szValueHex, inValuelen);                        
                    inlen =  inBufLen + inValuelen;
            }

			DebugAddHEX("Set Tag Data1", inbuf, inlen);
			
			if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
				rtn = EMVCL_SetEMVTags(inbuf, inlen, buf, &len);
			if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
				rtn = VW_SetEMVTags(inbuf, inlen, buf, &len);
		
			DebugAddHEX("Set Tag Data2", inbuf, inlen);
			vdDebug_LogPrintf("rtn =[%x].inlen=[%d].len=[%d]...",rtn,inlen,len);
			if(rtn != d_EMVCL_NO_ERROR)
			{
				return rtn;
			}
			
 			if(inlen != len)
			{
				return 0xD0000001;
			}
			DebugAddHEX("Set Tag Data Resopnse", buf, len);
		//	if(memcmp(d_SET_TAG, buf, len) != 0)
			if(memcmp(inbuf, buf, len) != 0)
			{
				return 0xD0000002;
			}
		}
	} while (result == SQLITE_ROW);    

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_NORMAL,baBuf);
	vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,baBuf);
	
	vdCTOSS_CLMClose();
	
	sqlite3_exec(dbWave,"commit;",NULL,NULL,NULL);
	//�ر�sqlite����
	sqlite3_finalize(stmtWave);
	sqlite3_close(dbWave);

	return(inResult);
}

#if 0
void vdCTOS_InitWaveData(void)
{

//    	CTOS_PrinterPutString("inCTOS_WaveFlowProcess");

	vdCTOSS_CLMSetALLCAPK();
	vdCTOSS_CLMSetAIDAndEMVTag();	
	inCTOSS_CLMSetCustomEMVTags();
	
}
#endif

void vdCTOSS_GetALLWaveSetting(void)
{
	BYTE OutRBuf[2048];
	UINT outLen,rtn;
	EMVCL_SCHEME_DATA stScheme;
	EMVCL_CA_PUBLIC_KEY stCAPubKey;

	inCTOSS_CLMOpenAndGetVersion();

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		rtn = MsessionAuthenAdmin();
		vdDebug_LogPrintf("rtn =[%x]........",rtn);
        if (rtn != d_NO_ERROR) 
		{
			vdDisplayErrorMsg(1, 8, "Authen Fail");
			return ;
        }
		
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_ADMIN,OutRBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,OutRBuf);
	}
		
	inCTOSS_CLMGetEMVTag(OutRBuf,&outLen);
	inCTOSS_CLMVisaGetCapability(&stScheme);

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_NORMAL,OutRBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,OutRBuf);
		
	vdCTOSS_CLMClose();
		
	inCTOSS_CLMListAllCAPKID(OutRBuf,&outLen);

	inCTOSS_CLMGetACAPK("\xA0\x00\x00\x00\x03",0x08,&stCAPubKey);
}



int inCTOS_MultiAPReloadWaveData(void)
{
	if (srTransRec.usChipDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baChipData, srTransRec.usChipDataLen);
	}
		
	if (srTransRec.usAdditionalDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baAdditionalData, srTransRec.usAdditionalDataLen);
	}
	
	return (d_OK);
}

int inCTOS_WaveFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    int inEVENT_ID = 0;
    char szAscii[INVOICE_ASC_SIZE];
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_WaveFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);
    fECRApproved=FALSE;
		
	memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));

    srTransRec.fCredit=TRUE;

    vdCTOS_SetTransType(SALE);
    //vdDispTransTitle(SALE);

    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
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
	inHDTReadEx2(19); /*load hdt for new host to get the if tip is enabled*/

    inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    
    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    //vdDisplayECRAmount(); /*display amount for ECR only*/
    inRet=inCTOS_GetCardFieldsCtls();
    //inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;
	
	//usCTOSS_BackToProgress("");
	//DisplayStatusLine("PROCESSING...");
	//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
    if(fGetECRTransactionFlg() == TRUE)
        srTransRec.fECRTxnFlg=TRUE;

    //remove for testing
      strCDT.HDTid=19; //Catch-all to New host

    vdCheckForDualCard();

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        
		vdDebug_LogPrintf("saturn GO TO SUB-AP");
        if (srTransRec.byTransType == REFUND)
            inEVENT_ID = d_IPC_CMD_REFUND;
        else
            inEVENT_ID = d_IPC_CMD_ONLINES_SALE;
        inRet = inCTOS_MultiAPSaveData(inEVENT_ID);
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
            
            inRet = inCTOS_MultiAPReloadWaveData();
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

	//vdDispTransTitle(SALE);
	
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
                   //inRet = inGetIPPPin();
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
	
    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    //remove - put before card entry
    //inRet = inCTOS_UpdateTxnTotalAmount();
    //if(d_OK != inRet)
    //    return inRet;

    srTransRec.fEMVPINEntered = FALSE;
	
	vdDebug_LogPrintf("saturn after online PIN cdtid %d", srTransRec.CDTid);
    //EMV: Online PIN enchancement - end -- jzg
 char szTotalAmount[12+1];
  wub_hex_2_str(srTransRec.szTotalAmount,szTotalAmount,6);
  vdDebug_LogPrintf("AAA - srTransRec.szTotalAmount CTLS[%ld] szCTLSDCCMinAmt[%ld]", atol(szTotalAmount), atol(strTCT.szCTLSDCCMinAmt));

  if ((srTransRec.byEntryMode==CARD_ENTRY_WAVE)  &&  atol(szTotalAmount) < atol(strTCT.szCTLSDCCMinAmt)) 
  {
         inRet = inCTOS_CustComputeAndDispTotal();
         if(d_OK != inRet)
         return inRet;
  }
else
  {
      if((strCDT.fDCCEnable==TRUE) && (strTCT.fDCCTerminal==TRUE))
      {
          inRet = inCTOS_LocalAPSaveData();
          if(d_OK != inRet)
              return inRet;

#if 0
          if ( (srTransRec.byEntryMode==CARD_ENTRY_MSR) || (srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
          || (srTransRec.byEntryMode==CARD_ENTRY_MANUAL2) || (srTransRec.byEntryMode==CARD_ENTRY_FALLBACK) ||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
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
#endif

          vdCTOS_SetTransType(DCC_RATE_REQUEST);
          vdDispTransTitle(DCC_RATE_REQUEST);


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
             // vdCTOS_SetTransType(SALE);
              //gblDCCTrans = FALSE;
              goto DisplayTotal;
          }
          DisplayFXRate:
          inRet=inDCCDisplayFxRate("STRAIGHT SALE");
          if (USER_ABORT== inRet)
          {
              if (fGetECRTransactionFlg())
                   vdSetECRResponse(ECR_UNKNOWN_ERR);
			  
              return inRet;
          }
		  if (DCC_TIMEOUT == inRet)
          {
               if (fGetECRTransactionFlg())
                    vdSetECRResponse(ECR_TIMEOUT_ERR);
			   
               return inRet;
          }
          if (DCC_PRINT_OPT == inRet)
          {
               vdPrintDCCChooseCurrency("STRAIGHT SALE");
               goto DisplayFXRate;
          }
      }

  DisplayTotal:
   vdCTOS_SetTransType(SALE);
   inRet = inCTOS_CustComputeAndDispTotal();
     if(d_OK != inRet)
          return inRet;     

  }

  inRet = inCTOS_EMVProcessing();
  if(d_OK != inRet)
  return inRet;	 
  
  //EMV: Online PIN enchancement - start -- jzg
  if(strTCT.fEMVOnlinePIN == TRUE && srTransRec.fEMVPINEntered == FALSE)
  {
       srTransRec.fEMVPIN = FALSE;
       vdDebug_LogPrintf("JEFF::inCTOS_WaveFlowProcess 9F34 = [%02X %02X %02X]", srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
       //if(((srTransRec.stEMVinfo.T9F34[0] & 0x0F) == 0x02) || (fEMVCheckCUPAID() == TRUE)/*CUP CTLS always ask for PIN*/)
       if((srTransRec.stEMVinfo.T9F34[0] & 0x0F) == 0x02 && inCTOSS_CheckCVMAmount()== d_NO)
       {
            //test
            
            inInitializePinPad();
            
            inRet = GetPIN_With_3DESDUKPTEx();	 
            if (inRet == d_KMS2_GET_PIN_NULL_PIN)
            {
                 srTransRec.fEMVPIN=FALSE;			 
                 inRet=d_OK;
            }
            else if(inRet != d_OK)
                 return inRet;
            else
                 srTransRec.fEMVPIN = TRUE;
       }
  }

     
     vdDebug_LogPrintf("saturn before pre-connect");

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;
	
	inCTOSS_GetRAMMemorySize("BUILD&SEND START");
    inRet = inBuildAndSendIsoData();
	inCTOSS_GetRAMMemorySize("BUILD&SEND END");
    if(d_OK != inRet)
    {
         if (strlen(srTransRec.szRespCode) <= 0)
              strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);

	     return inRet;
			  
    }
    inRet = inCTOS_SaveBatchTxn();
	vdDebug_LogPrintf("TINE: inCTOS_SaveBatchTxn -- END, inret=[%d]", inRet);
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

    //fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();
	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
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
	 
	 vdDebug_LogPrintf("saturn after print receipt");
		
	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
    if(d_OK != inRet)
        return inRet;
    else
         vdSetErrorMessage("");

	
	vdDebug_LogPrintf("saturn wave process flow return");
	fECRApproved=TRUE;
	//vdDebug_LogPrintf("AAA<<<>> 1");
	//inMultiAP_ECRSendSuccessResponse();
	
    return d_OK;
}

int inCTOS_WAVE_START(void)
{
    int inRet = d_OK;
    vdDebug_LogPrintf("SATURN inCTOS_WAVE_START.............");

    return inRet;
}

int inCTOS_WAVE_SALE(void)
{
    int inRet = d_NO;


	
    vdDebug_LogPrintf("SATURN inCTOS_WAVE_SALE............. %d", inSaleGroupIndex);

    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    vdCTOS_SetTransType(SALE);


	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;

	if(isCheckTerminalMP200() == d_OK)
	{
		inCTOSS_GetRAMMemorySize("UPLOAD PENDING ERM RECEIPT START");
		inRet = inCTOSS_ERM_CheckSlipImage();
		inCTOSS_GetRAMMemorySize("UPLOAD PENDING ERM RECEIPT END");
		if(d_OK != inRet)
			return inRet;
	}
	

	
    if (inCTOSS_GetCtlsMode() == NO_CTLS)
    {
        vdDebug_LogPrintf("saturn no CTLS");
        inSaleGroupIndex=0;
        inRet = inCTOS_SaleFlowProcess();
    }
    else
    {
		if(fEntryCardfromIDLE == TRUE && (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_MSR))
		{
			vdDebug_LogPrintf("saturn swipe from idle");
			inSaleGroupIndex=0;
			inRet = inCTOS_SaleFlowProcess();
		}
		else
		{

			vdDebug_LogPrintf("saturn with CTLS");
            vdCTOSS_SetWaveTransType(1);
            inRet = inCTOSS_CLMOpenAndGetVersion();
            if(d_OK != inRet)
                return inRet;
			inSaleGroupIndex=0;
            inRet = inCTOS_WaveFlowProcess();
            vdCTOSS_CLMClose();
		}
    }

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
	//vdDebug_LogPrintf("AAA<<<>> 2");
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

	vdDebug_LogPrintf("saturn wave sale - call transendreset");
    inMultiAP_ECRSendSuccessResponse();        
    vdCTOS_TransEndReset();
    return inRet;

}



int inCTOS_WAVE_REFUND(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	vdCTOS_SetTransType(REFUND);
	vdCTOSS_SetWaveTransType(1);

	vdCTOSS_GetAmt();

	inRet = inCTOSS_CLMOpenAndGetVersion();
	if(d_OK != inRet)
        return inRet;
	//display title
    vdDispTransTitle(REFUND);

    inRet = inCTOS_WaveFlowProcess();

	vdCTOSS_CLMClose();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}


void vdCTOS_InitWaveData(void)
{
	BYTE bInBuf[250];
	BYTE bOutBuf[250];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	int inCTLSmode = 0;
	
	inCTLSmode = inCTOSS_GetCtlsMode();
	if (NO_CTLS == inCTLSmode)
		return;

	//Open the Back Light in the LCD Display //
	//vduiKeyboardBackLight(VS_TRUE);
	//vduiLightOn();
	//CTOS_LCDTClearDisplay();
    vdDispTitleString((BYTE *)"CONTACTLESS");
	setLCDPrint(5, DISPLAY_POSITION_LEFT, "CONFIGURE READER...");
//    usCTOSS_DisplayStatusBox("CONTACTLESS|CONFIGURE READER...|wait");
	
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	vdDebug_LogPrintf("d_IPC_CMD_EMV_WAVESETTAGS usInLen[%d],inCTLSmode=[%d],GetCtlsMode=[%d] ",usInLen,inCTLSmode,inCTOSS_GetCtlsMode());

	if (CTLS_INTERNAL == inCTLSmode || CTLS_EXTERNAL == inCTLSmode)
	{
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_WAVESETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
	}

	if (CTLS_V3_SHARECTLS == inCTLSmode || CTLS_V3_INT_SHARECTLS == inCTLSmode)
	{
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_CTLS.SHARLS_CTLS", d_IPC_CMD_EMV_WAVESETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
	}	

}


void vdCTOS_PartialInitWaveData(void)
{
	BYTE bInBuf[250];
	BYTE bOutBuf[250];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	int inCTLSmode = 0;
	
	inCTLSmode = inCTOSS_GetCtlsMode();
	if (NO_CTLS == inCTLSmode)
		return;

	inDatabase_WaveDelete("MCCS", "DF04");
	inDatabase_WaveUpdate("MCCS", "DFAF11", "888888888888");
	inDatabase_WaveUpdate("MCCS", "DFAF13", "555555555555");
	inDatabase_WaveUpdate("MCCS", "DFAF12", "000000000011");
	inDatabase_WaveInsert("MCCS", "DF05", "11");

	CTOS_LCDTClearDisplay();
    vdDispTitleString((BYTE *)"CONTACTLESS");
	setLCDPrint(5, DISPLAY_POSITION_LEFT, "CONFIGURE READER...");
	
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	vdDebug_LogPrintf("d_IPC_CMD_EMV_WAVESETTAGS usInLen[%d],inCTLSmode=[%d],GetCtlsMode=[%d] ",usInLen,inCTLSmode,inCTOSS_GetCtlsMode());
#if 0
	if(strTCT.byPinPadMode == 0)
	{
		if(strTCT.byPinPadType == 3 && CTLS_V3_SHARECTLS == inCTLSmode)
		{
			char szV3PSendBuf[1024+1];
			int inOffSet = 0;
			int status;
			char szRecvBuf[1024+1];
			int inRecvlen = 0;
			unsigned char *pszPtr;
			
			vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
			inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

			// send STX INJECT_KEY ETX LRC
			memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
			inOffSet = 0;
			szV3PSendBuf[inOffSet] = STX;
			inOffSet += 1;
			memcpy(&szV3PSendBuf[inOffSet], "INIT_WAVE", strlen("INIT_WAVE"));
			inOffSet += strlen("INIT_WAVE");
			szV3PSendBuf[inOffSet] = ETX;
			inOffSet += 1;			
			szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
			inOffSet += 1;
			inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);	
			status = inCTOSS_V3PRS232RecvACK(szRecvBuf, &inRecvlen, (d_CEPAS_READ_BALANCE_CARD_PRESENT_TIMEOUT));						

			//inCTOSS_USBHostCloseEx();
			inCTOSS_V3PRS232Close();
			return ;
		}
	}

	if (CTLS_INTERNAL == inCTLSmode || CTLS_EXTERNAL == inCTLSmode)
	{
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_WAVESETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
	}
#endif
	if (CTLS_V3_SHARECTLS == inCTLSmode || CTLS_V3_INT_SHARECTLS == inCTLSmode)
	{
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_CTLS.SHARLS_CTLS", d_IPC_CMD_CTLS_PARTIALSETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
	}	

}

BOOL fEMVCheckCUPAID(void)	
{
    if(memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0 && srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        return TRUE;
    
    return FALSE;
}

int inCTOS_WAVE_CUP_SALE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    vdCTOS_SetTransType(SALE);
	
    if (inCTOSS_GetCtlsMode() == NO_CTLS)
    {
        inSaleGroupIndex=8;
        inRet = inCTOS_SaleFlowProcess();
    }
    else
    {
		if(fEntryCardfromIDLE == TRUE && (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_MSR))
		{
			inSaleGroupIndex=8;
			inRet = inCTOS_SaleFlowProcess();			
		}
		else
		{
            vdCTOSS_SetWaveTransType(1);
            inRet = inCTOSS_CLMOpenAndGetVersion();
            if(d_OK != inRet)
                return inRet;

			inSaleGroupIndex=8;
            inRet = inCTOS_WaveFlowProcess();
            vdCTOSS_CLMClose();
		}
    }

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
	//vdDebug_LogPrintf("AAA<<<>> 3");
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

