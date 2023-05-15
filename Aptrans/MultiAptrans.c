
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "MultiAptrans.h"
#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSSettlement.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\Dmenu.h"
#include "..\Includes\CTOSInput.h"


#include "..\Print\Print.h"
#include "..\UI\Display.h"
#include "..\Debug\Debug.h"
#include "..\FileModule\myFileFunc.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\powrfail\POSPOWRFAIL.h"
#include "..\Ctls\POSWave.h"


#include "..\TMS\TMS.h"

// patrick add code 20141205 start
int inSendECRResponseFlag = 0;
char inSendECRResponseCommandID[2+1];
//int inIPC_KEY_SEND_Parent = 0;
extern BOOL fECRApproved;

extern int inSettlementHost;

// patrick this function need provide to application level and response code
/*================================================================
  Handle IPC function.
  [Return] IPC process status return code
  ================================================================ */
USHORT inMultiAP_HandleIPC(BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int inRet = d_NO;
	USHORT len;
    int inSelectTrans=0;
	len = 0;

    //ECR txn no need this, make it fast
    if(0 == fGetECRTransactionFlg())
    {
        //if main APP change the parameter, sub need read again
    	inTCTRead(1);
    	inCPTRead(MCC_HOST);
    	inPCTRead(1);
    	inCSTRead(1);
    	inTCPRead(1);
    }

	// patrick add code 20141205 start
	inSendECRResponseCommandID[0] = inbuf[0];

	/* Process IPC command code action */
    switch (inbuf[0])
    {
        case d_IPC_CMD_ONLINES_SALE:
        case d_IPC_CMD_WAVE_SALE:
			vduiKeyboardBackLight(TRUE);
			if(fGetECRTransactionFlg())
			{
				inTCTRead(1);
//                inSelectTrans=inSelectECRTrans();
                inSelectTrans = 4;
                if(inSelectTrans == 1) /*CREDIT SALE*/
				    inRet = inCTOS_WAVE_SALE();
				else if(inSelectTrans == 2) /*DEBIT SALE*/
					inRet = inCTOS_DebitSale();
				else if(inSelectTrans == 3) /*DINERS*/
					inRet = inCTOS_DinersSALE();
			    else if(inSelectTrans == 4) /*BANCNET*/
					inRet = inCTOS_BancnetSale();
			}
			else
                inRet = inCTOS_WAVE_SALE();
            outbuf[len++] = d_IPC_CMD_ONLINES_SALE;
            if (inRet == d_OK)
            outbuf[len++] = d_SUCCESS;
            else
            outbuf[len++] = d_FAIL;
            outbuf[len] = 0x00;
        break;

	case d_IPC_CMD_WAVE_REFUND:
		inRet = inCTOS_WAVE_REFUND();
		outbuf[len++] = d_IPC_CMD_WAVE_REFUND;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;

	case d_IPC_CMD_OFFLINE_SALE:
		inRet = inCTOS_SALE_OFFLINE();
		outbuf[len++] = d_IPC_CMD_OFFLINE_SALE;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_PRE_AUTH:
		inRet = inCTOS_PREAUTH();
		outbuf[len++] = d_IPC_CMD_PRE_AUTH;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;
		
	case d_IPC_CMD_PRE_COMP:
		inRet = inCTOS_COMP();
		outbuf[len++] = d_IPC_CMD_PRE_COMP;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;
		
	case d_IPC_CMD_CASH_ADV:
		inRet = inCTOS_CASH();
		outbuf[len++] = d_IPC_CMD_CASH_ADV;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;
	
	case d_IPC_CMD_REFUND:
		inRet = inCTOS_REFUND();
		outbuf[len++] = d_IPC_CMD_REFUND;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_POWER_FAIL:
		inRet = inCTLOS_Getpowrfail();
		outbuf[len++] = d_IPC_CMD_POWER_FAIL;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;
		
	case d_IPC_CMD_GET_PID:
		outbuf[len++] = d_IPC_CMD_GET_PID;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_VOID_SALE:
        vduiKeyboardBackLight(TRUE);
		inRet = inCTOS_VOID();
		outbuf[len++] = d_IPC_CMD_VOID_SALE;
		
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;

	case d_IPC_CMD_TIP_ADJUST:
		inRet = inCTOS_TIPADJUST();
		outbuf[len++] = d_IPC_CMD_TIP_ADJUST;
		
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;

	case d_IPC_CMD_SETTLEMENT:
		inCTOS_SETTLEMENT();
		outbuf[len++] = d_IPC_CMD_SETTLEMENT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SETTLE_ALL:
		inRet = inCTOS_SETTLE_ALL();
		outbuf[len++] = d_IPC_CMD_SETTLE_ALL;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_CHK_BATCH_EMPTY:
		inRet = inCTOSS_TMSChkBatchEmpty();
		outbuf[len++] = d_IPC_CMD_CHK_BATCH_EMPTY;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BATCH_REVIEW:
		inCTOS_BATCH_REVIEW();
		outbuf[len++] = d_IPC_CMD_BATCH_REVIEW;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BATCH_TOTAL:
		inCTOS_BATCH_TOTAL();
		outbuf[len++] = d_IPC_CMD_BATCH_TOTAL;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_REPRINT_LAST:
		inRet = inCTOS_REPRINT_LAST();
		outbuf[len++] = d_IPC_CMD_REPRINT_LAST;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_DELETE_BATCH:
		vdCTOS_DeleteBatch();
		outbuf[len++] = d_IPC_CMD_DELETE_BATCH;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
    case d_IPC_CMD_DELETE_REVERSAL:
		vdCTOS_DeleteReversal();
		outbuf[len++] = d_IPC_CMD_DELETE_REVERSAL;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_REPRINT_ANY:
		inRet = inCTOS_REPRINT_ANY();
		outbuf[len++] = d_IPC_CMD_REPRINT_ANY;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_REPRINT_LAST_SETT:
		inCTOS_REPRINTF_LAST_SETTLEMENT();
		outbuf[len++] = d_IPC_CMD_REPRINT_LAST_SETT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SUMMARY_REPORT:
		inCTOS_PRINTF_SUMMARY();
		outbuf[len++] = d_IPC_CMD_SUMMARY_REPORT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_DETAIL_REPORT:
		inCTOS_PRINTF_DETAIL();
		outbuf[len++] = d_IPC_CMD_DETAIL_REPORT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_CHECK_DUP_INV:
		inCTOSS_BatchCheckDuplicateInvoice();
		outbuf[len++] = d_IPC_CMD_CHECK_DUP_INV;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_FUN_KEY1:
		inF1KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY1;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY2:
		inF2KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY2;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY3:
		inF3KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY3;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY4:
		inF4KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY4;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BAL_INQ:
		inCTOS_DebitBalInq();
		outbuf[len++] = d_IPC_CMD_BAL_INQ;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_LOGON:
		inCTOS_DebitLogon();
		outbuf[len++] = d_IPC_CMD_LOGON;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_LOY_BAL_INQ:
		inCTOS_LoyaltyBalanceInquiry();
		outbuf[len++] = d_IPC_CMD_LOY_BAL_INQ;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_LOY_REDEEM_5050:
		inCTOS_Redeem5050();
		outbuf[len++] = d_IPC_CMD_LOY_REDEEM_5050;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_LOY_REDEEM_VARIABLE:
		inCTOS_RedeemVariable();
		outbuf[len++] = d_IPC_CMD_LOY_REDEEM_VARIABLE;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_INSTALLMENT:
		inCTOS_INSTALLMENT();
		outbuf[len++] = d_IPC_CMD_INSTALLMENT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_DEBIT_SALE:
        inCTOS_DebitSale();
        outbuf[len++] = d_IPC_CMD_DEBIT_SALE;
        outbuf[len++] = d_SUCCESS;
        outbuf[len] = 0x00;
        break;

    case d_IPC_CMD_CARD_VERIFICATION:
        inCTOS_DinersCardVer();
        outbuf[len++] = d_IPC_CMD_DEBIT_SALE;
        outbuf[len++] = d_SUCCESS;
        outbuf[len] = 0x00;
    break;

    case d_IPC_CMD_EDC_SETTINGS:
         vdCTOS_ModifyEdcSetting();
         outbuf[len++] = d_IPC_CMD_DEBIT_SALE;
         outbuf[len++] = d_SUCCESS;
         outbuf[len] = 0x00;
    break;

    case d_IPC_CMD_PRINT_CRC:
         vdCTOS_PrintCRC();
         outbuf[len++] = d_IPC_CMD_PRINT_CRC;
         outbuf[len++] = d_SUCCESS;
         outbuf[len] = 0x00;
    break;

	case d_IPC_CMD_PUT_ENV_CRC:
         vdPutEnvCRC();
         outbuf[len++] = d_IPC_CMD_PUT_ENV_CRC;
         //outbuf[len++] = d_SUCCESS;
         outbuf[len] = 0x00;
    break;
	
    case d_IPC_CMD_PRINT_EMV_TAGS:
         vdPrintEMVTags();
         outbuf[len++] = d_IPC_CMD_PRINT_EMV_TAGS;
         outbuf[len++] = d_SUCCESS;
         outbuf[len] = 0x00;
    break;

    case d_IPC_CMD_PRINT_HOST_INFO:
        vdCTOS_HostInfo();
        outbuf[len++] = d_IPC_CMD_PRINT_HOST_INFO;
        outbuf[len++] = d_SUCCESS;
        outbuf[len] = 0x00;
    break;
		
    case d_IPC_CMD_CHANGE_DEF_APP:
        vdCTOSS_ChangeDefaultApplication();
        outbuf[len++] = d_IPC_CMD_CHANGE_DEF_APP;
        outbuf[len++] = d_SUCCESS;
        outbuf[len] = 0x00;
    break;

    case d_IPC_CMD_FORCE_SETTLE:
		inRet = inCTOSS_ForceSettle();
		outbuf[len++] = d_IPC_CMD_FORCE_SETTLE;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SETFONT:
		inSetTextModeEx(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_SETFONT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	
	default:
		outbuf[len++] = 0xFF;
		break;	
	}

	inTCTSave(1);
	
	*outlen = len;
	return len;
}


#if 0
int inMultiAP_RunIPCCmdTypes(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	pid_t wpid;	
	BYTE processID[100];
	pid_t pid = -1;
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];

	strcpy(szAppname, Appname);

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes =[%d][%s]",pid, szAppname);
	if (pid <= 0)
	{
		inCTOS_ReForkSubAP(szAppname);
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
		//here need delay, wait sub AP initialization 
		if (pid > 0)
			CTOS_Delay(3000);
		
		vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes22 =[%d][%s]",pid, szAppname);
	}

	if (pid > 0)
	{
//		inIPC_KEY_SEND_Parent = pid;
		IPC_IN_LEN = 0;
		memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
		memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
		
		IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
		if (inlen > 0)
		{
			memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
			IPC_IN_LEN = IPC_IN_LEN + inlen;
		}
		
		do
		{
			if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			{
				vdDebug_LogPrintf("Multi UnForkAPName=[%s] ....", szAppname);
				break;
			}
			
			memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
			inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);
			IPC_OUT_LEN = inMultiAP_IPCCmdParent(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
			wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
			
			//vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_IN_LEN);

			if (IPC_OUT_BUF[0] == d_SUCCESS)
			{
				break;
			}

			insendloop++;
			if (insendloop > 2)
			{
			/*
			//cannot refork SubAP, if Sub AP update configExpress will timeout.
				if (insendloop > 3)
					return d_NO;
				
				inCTOS_ReForkSubAP(szAppname);
				memset(processID,0x00,sizeof(processID));
				vdMultiAP_getPID(szAppname,processID);
				pid = atoi(processID);
				vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes22 =[%d][%s]",pid, szAppname);
				//here need delay, wait sub AP initialization 
				if (pid > 0)
					CTOS_Delay(3000);
				else
				*/
				return d_NO;
			}
		} while (1);

		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
		while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
		{	
			memset(processID,0x00,sizeof(processID));
			vdMultiAP_getPID(szAppname,processID);
			if (atoi(processID) <= 0)
			{
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
				{
					vdDebug_LogPrintf("Multi UnForkAPName11=[%s] ....", szAppname);
					return d_OK;
				}
				inCTOS_ReForkSubAP(szAppname);
				return d_NO;
			}

			CTOS_Delay(strTCT.inWaitTime);

			ipc_len = d_MAX_IPC_BUFFER;
			// patrick need hendle check status if transaction fail or success.
			if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
			{
				ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
				*outlen = ipc_len;
				
				return d_OK;
			}
		}
	}
	else
	{
		return d_NO;
	}
}
#else
	int inMultiAP_RunIPCCmdTypes(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
	{
		int insendloop = 0;
		int status;
		int ipc_len;
		pid_t wpid; 
		BYTE processID[100];
		pid_t pid = -1;
		USHORT IPC_IN_LEN, IPC_OUT_LEN;
		BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
		BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
		
		
		char szAppname[100+1];
		int begin;
		int timeoutSecond = 10;
	
		strcpy(szAppname, Appname);
	
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
	
		vdDebug_LogPrintf("saturn credit inMultiAP_RunIPCCmdTypes =[%d][%s],[%x]",pid, szAppname,IPC_EVENT_ID);
		if (pid <= 0)
		{
			inCTOS_ReForkSubAP(szAppname);
			memset(processID,0x00,sizeof(processID));
			vdMultiAP_getPID(szAppname,processID);
			pid = atoi(processID);
			//here need delay, wait sub AP initialization 
			if (pid > 0 && d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
				CTOS_Delay(3000);
			
			vdDebug_LogPrintf("saturn credit inMultiAP_RunIPCCmdTypes22 =[%d][%s]",pid, szAppname);
		}
	
		if (pid > 0)
		{
	//		inIPC_KEY_SEND_Parent = pid;
			IPC_IN_LEN = 0;
			memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
			memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
			memset(IPC_OUT_BUF,0x00,sizeof(IPC_OUT_BUF));
			
			IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
			if (inlen > 0)
			{
				memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
				IPC_IN_LEN = IPC_IN_LEN + inlen;
			}
			do
			{
				//beacuse the unforkAP must have IPC EVENT ID.
				//if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
				//{
				//	vdDebug_LogPrintf("Multi UnForkAPName=[%s] ....", szAppname);
				//	break;
				//}
				
				memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
				inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);

				vdDebug_LogPrintf("saturn credit inMultiAP_IPCCmdParent 1 %x %x %x %x %x %x  %d", IPC_IN_BUF_STR[0], IPC_IN_BUF_STR[1],IPC_IN_BUF_STR[2], IPC_IN_BUF_STR[3], IPC_IN_BUF_STR[4], IPC_IN_BUF_STR[5],IPC_IN_LEN);
				
				IPC_OUT_LEN = inMultiAP_IPCCmdParent(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
				wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
				
				vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_OUT_BUF[0]);

				vdDebug_LogPrintf("saturn credit return 1 %x", IPC_OUT_BUF[0]);
	
				if (IPC_OUT_BUF[0] == d_SUCCESS)
				{
                    vdDebug_LogPrintf("saturn IPC_OUT_BUF[0] == d_SUCCESS, inMultiAP_RunIPCCmdTypes");
					break;
				}

				insendloop++;
				if (insendloop > 1)//now it's blocking mode, no need wait too long.
				{
					vdDebug_LogPrintf("inMultiAP_IPCCmdParent TIMEOUT.!!!");
#if 0
	//no need add below sample code, already fixed in inCTOS_ReForkSubAP,
	//have not check ResumeAPName in inCTOS_ReForkSubAP
					vdCTOSS_ReForkSubAP(szAppname);
					memset(processID,0x00,sizeof(processID));
					vdMultiAP_getPID(szAppname,processID);
					pid = atoi(processID);
					vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes333 =[%d][%s]",pid, szAppname);
					//here need delay, wait sub AP initialization 
					if (pid > 0)
						CTOS_Delay(3000);
					else
						return d_NO;
	
					vdDebug_LogPrintf("insendloop =[%d]",insendloop);
					if (insendloop == 3)
#endif					
					{
					if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
						|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
					{
						vdDebug_LogPrintf("inCTOS_Kill unfork SubAP = [%s]",szAppname);
						inCTOS_KillASubAP(szAppname);
					}
					return d_NO;
					}
				}
			} while (1);

			vdDebug_LogPrintf("saturn credit inWaitTime =[%d]",strTCT.inWaitTime);
			if (strTCT.inWaitTime <= 0)
				strTCT.inWaitTime = 100;
			
			begin=time(NULL);
			memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
			while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
			{	
#if 0
				memset(processID,0x00,sizeof(processID));
				vdMultiAP_getPID(szAppname,processID);
				if (atoi(processID) <= 0)
				{
					if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
						|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
					{
						vdDebug_LogPrintf("Multi UnForkAPName11=[%s] ....", szAppname);
						return d_OK;
					}
					
					inCTOS_ReForkSubAP(szAppname);
                    vdDebug_LogPrintf("inCTOS_ReForkSubAP");
					return d_NO;
				}
#endif
	
				if (d_IPC_CMD_EMV_WGET == IPC_EVENT_ID)
				{
					if (time(NULL)-begin > timeoutSecond)
					{
						inCTOS_ReForkSubAP(szAppname);
						return d_NO;
					}
				}
				
               // vdDebug_LogPrintf("TINE: CTOS_Delay(strTCT.inWaitTime) - START -");
				//CTOS_Delay(strTCT.inWaitTime);
               // vdDebug_LogPrintf("TINE: CTOS_Delay(strTCT.inWaitTime) - END -");
				ipc_len = d_MAX_IPC_BUFFER;
				// patrick need hendle check status if transaction fail or success.

//				vdDebug_LogPrintf("saturn credit before get parent");
				if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
				{
				    vdDebug_LogPrintf("saturn credit get parent success");
					ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
					*outlen = ipc_len;
	
					if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
						|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
					{
						inCTOS_KillASubAP(szAppname);
					}
                    vdDebug_LogPrintf("TINE: inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK");
					return d_OK;
				}
			}
		}
		else
		{
            vdDebug_LogPrintf("TINE: pid < 0");
			return d_NO;
		}
	}

#endif

int inMultiAP_RunIPCCmdTypesEx(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	pid_t wpid;	
	BYTE processID[100];
	pid_t pid = -1;
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];

	strcpy(szAppname, Appname);

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("saturn inMultiAP_RunIPCCmdTypesEx =[%d][%s],[%x]",pid, szAppname,IPC_EVENT_ID);
	if (pid <= 0)
	{
		inCTOS_ReForkSubAP(szAppname);
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
		//here need delay, wait sub AP initialization 
		if (pid > 0 && d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			CTOS_Delay(3000);
		
		vdDebug_LogPrintf("saturn inMultiAP_RunIPCCmdTypesEx22 =[%d][%s]",pid, szAppname);
	}

	if (pid > 0)
	{
//		inIPC_KEY_SEND_Parent = pid;
		IPC_IN_LEN = 0;
		memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
		memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
		
		IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
		if (inlen > 0)
		{
			memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
			IPC_IN_LEN = IPC_IN_LEN + inlen;
		}
		
		do
		{
			memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
			inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);
			IPC_OUT_LEN = inMultiAP_IPCCmdParentEx(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
			wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
			
			//vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_IN_LEN);

			if (IPC_OUT_BUF[0] == d_SUCCESS)
			{
				break;
			}

			insendloop++;
			if (insendloop > 0)
			{
				vdDebug_LogPrintf("1.inMultiAP_IPCCmdParentEx TIMEOUT.!!!");
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					vdDebug_LogPrintf("inCTOS_Kill unfork SubAP = [%s]",szAppname);
					inCTOS_KillASubAP(szAppname);
				}
            	return d_OK;
			}
		} while (1);

		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		
		CTOS_TimeOutSet (TIMER_ID_1 , 200);
		memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
		while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
		{
			CTOS_Delay(strTCT.inWaitTime);

			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
			{
				vdDebug_LogPrintf("2.inMultiAP_RunIPCCmdTypesEx TIMEOUT.!!!");
            	return d_OK;
			}

			ipc_len = d_MAX_IPC_BUFFER;
			// patrick need hendle check status if transaction fail or success.
			if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
			{
				ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
				*outlen = ipc_len;

				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					inCTOS_KillASubAP(szAppname);
				}
				
				return d_OK;
			}
		}
	}
	else
	{
		return d_NO;
	}
}

int inMultiAP_SendChild(BYTE *inbuf, USHORT inlen)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	int out_len = 0;
	memset(outbuf,0x00,sizeof(outbuf));
	
	out_len = wub_hex_2_str(inbuf ,outbuf ,inlen);

	bret = inMultiAP_IPCSendChild(outbuf , out_len);
	
	//vdDebug_LogPrintf("inMultiAP_SendChild=[%s] [%ld] [%d],[%d]",outbuf, out_len,inlen,bret);

	return bret;
}

#if 1
int inMultiAP_ECRSendSuccessResponse(void)
{
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;

	BYTE szTemp4[d_LINE_SIZE + 1] = {0};
	BYTE szTemp5[d_LINE_SIZE + 1] = {0};
	BYTE szTemp6[d_LINE_SIZE + 1] = {0};
	char szBuffer[100+1];
	char szHeader[23+1];
	char szMsg1[40], szMsg2[40], szMsg3[40];
	char szErrMsg[30+1] = {0};
	char szErrMsg1[30+1] = {0};
	int inMsgid;
	int inECRType = get_env_int("ECRTYPE"); /*0 - MERCURY / 1 - WILCON*/ 


	//inSendECRResponseFlag = get_env_int("ECRRESP");

	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fECR=[%d]", strTCT.fECR);
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse:::szRespCode[%s]", srTransRec.szRespCode);
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fGetECRTransactionFlg=[%d]", fGetECRTransactionFlg());
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,inSendECRResponseFlag[%d]", inSendECRResponseFlag);
	//vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fTimeOutFlag=[%d]", fTimeOutFlag);
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,srTransRec.MITid=[%d]", srTransRec.MITid);
	vdDebug_LogPrintf("AAA<<>>> inMultiAP_ECRSendSuccessResponse,srTransRec.HDTid=[%d]", srTransRec.HDTid);

	/*BDO: For ECR response code to ECR_TIMEOUT_ERR -- sidumili*/
	//if ((fTimeOutFlag) && strlen(srTransRec.szRespCode) <= 0)
		//strcpy(srTransRec.szECRRespCode, ECR_TIMEOUT_ERR);

	/*BDO: Send data to ECR for OFFLINE trxn -- sidumili*/
	/*
	if ((strlen(srTransRec.szRespCode) > 0) && (memcmp(srTransRec.szRespCode,"00",2) == 0)){

		if ((srTransRec.byTransType == SALE_OFFLINE) || (srTransRec.byOrgTransType == SALE_OFFLINE)){
			strcpy(srTransRec.szRespCode, "00");
			vdCTOS_Pad_String(srTransRec.szRRN, 12, IS_SPACE, POSITION_RIGHT);
			vdCTOS_Pad_String(srTransRec.szAuthCode, 6, IS_SPACE, POSITION_RIGHT);
		}	
	}
*/
	//if (inSendECRResponseFlag == 1)
		//return(d_OK);

	//check is ECR taansaction
	if (fGetECRTransactionFlg() != TRUE)
		return(d_OK);
	
	vdDebug_LogPrintf("AAA<<<>>inMultiAP_ECRSendSuccessResponse::RUN EXECUTE SHALRS!!!");	
	
	if (strTCT.fECR)
	{
         if ((srTransRec.HDTid==MCC_HOST) || (srTransRec.HDTid==0))
         {
              bret= inMultiAP_Database_BatchDelete();
              vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
              if(d_OK != bret)
              {
                   vdSetErrorMessage("MultiAP BatchDelete ERR");
              }
         }
         inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid); 
	            	

    if(inECRType >=1) //wilcon ecr
    {
         //----------------------------------------------------------------------------------------------------------------------
         // SAVE TO MULTIAP.S3DB -- sidumili
         //----------------------------------------------------------------------------------------------------------------------
         // For PAN Format
         memset(szTemp4, 0x00, sizeof(szTemp4));
		 
         if((strlen(srTransRec.szPAN) > 0) && (strTCT.fECRFormatPan == TRUE))
         {
              vdFormatPANForECR(srTransRec.szPAN, szTemp4);
              strcpy(srTransRec.szECRPANFormatted, szTemp4);
         }
         else
              strcpy(srTransRec.szECRPANFormatted, srTransRec.szPAN);
         /*if(srTransRec.HDTid == SM_CREDIT_CARD || srTransRec.HDTid == SMGUARANTOR_HDT_INDEX ||
              (srTransRec.HDTid == SMAC_HDT_INDEX && srTransRec.byEntryMode == CARD_ENTRY_WAVE 
              && srTransRec.byTransType == PTS_AWARDING && srTransRec.fOffline_PtsAwarding == TRUE))//Do not mask since terminal should send unmasked card number to ECR)
              strcpy(srTransRec.szECRPANFormatted, srTransRec.szPAN);
              else 
                  strcpy(srTransRec.szECRPANFormatted, szTemp4);*/
         
         // For ResponseText
         memset(szMsg1, 0x00, sizeof(szMsg1));
         memset(szMsg2, 0x00, sizeof(szMsg2));
         memset(szMsg3, 0x00, sizeof(szMsg3));
         
         vdDebug_LogPrintf("szRespCode  [%s]   szECRRespCode [%s]", srTransRec.szRespCode, srTransRec.szECRRespCode );
         
         if ((strlen(srTransRec.szRespCode) > 0) && (memcmp(srTransRec.szECRRespCode, "00", 2)== 0))
         	{
         	      strcpy(szMsg1, ECR_APPROVED_RESP);
                  strcpy(srTransRec.szECRRespCode, ECR_APPROVED);
         	}
         else if ((memcmp(srTransRec.szECRRespCode, "08", 2)== 0) || (memcmp(srTransRec.szECRRespCode, "10", 2)== 0)
         || (memcmp(srTransRec.szECRRespCode, "11", 2)== 0) || (memcmp(srTransRec.szECRRespCode, "16", 2)== 0))
         {
              inMSGResponseCodeRead(szMsg1, szMsg2, atoi(srTransRec.szECRRespCode), 1);
         }
		 else if (memcmp(srTransRec.szAuthCode, "Y1", 2) == 0)
         {
              strcpy(szMsg1, ECR_APPROVED_RESP);
              strcpy(srTransRec.szECRRespCode, ECR_APPROVED);
         }
		 else
         {
              if (((strlen(srTransRec.szRespCode) > 0) && (memcmp(srTransRec.szECRRespCode, "00", 2)!= 0) 
			  && (memcmp(srTransRec.szECRRespCode, ECR_COMMS_ERR, 2)!= 0) && (memcmp(srTransRec.szECRRespCode, ECR_TIMEOUT_ERR, 2)!= 0)
              && (memcmp(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR, 2)!= 0) && (memcmp(srTransRec.szECRRespCode, ECR_DECLINED_ERR, 2)!= 0))
              && ((memcmp(srTransRec.szECRRespCode, "08", 2)!= 0) || (memcmp(srTransRec.szECRRespCode, "10", 2)!= 0)
              || (memcmp(srTransRec.szECRRespCode, "11", 2)!= 0) || (memcmp(srTransRec.szECRRespCode, "16", 2)!= 0)))
              {		   
                   inMSGResponseCodeRead(szMsg1, szMsg2, atoi(srTransRec.szRespCode), 1);
                   //strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);
              }
              else
              {
                   memset(szMsg1, 0x00, sizeof(szMsg1));
                   if (memcmp(srTransRec.szECRRespCode, ECR_TIMEOUT_ERR, 2) == 0) /*TIMEOUT*/
                   {
                        strcpy(szMsg1, ECR_TIMEOUT_RESP);
                        //strcpy(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR);
                   }
                   else if (memcmp(srTransRec.szECRRespCode, ECR_COMMS_ERR, 2) == 0) /*COMM ERROR*/
                   {
                        strcpy(szMsg1, ECR_COMM_ERROR_RESP);
                        //strcpy(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR);
                   }
                   else if (memcmp(srTransRec.szECRRespCode, ECR_DECLINED_ERR, 2) == 0)
                   {		
                        if(strcmp(srTransRec.szResponseText,ECR_EXPIRED_CARD_RESP)==0)
                        {
                             strcpy(szMsg1, ECR_EXPIRED_CARD_RESP);
                        }
						else if (strcmp(srTransRec.szResponseText,ECR_RECORD_VOIDED)==0)
                        {
                             strcpy(szMsg1, ECR_RECORD_VOIDED);
                        }
						else if (strcmp(srTransRec.szResponseText,ECR_AMOUNT_ERR_RESP)==0)
                        {
                             strcpy(szMsg1, ECR_AMOUNT_ERR_RESP);
                        }
						else
						     strcpy(szMsg1, "INVALID RESPONSE CODE");
                   }
                   else
                   {
                        if (inGetErrorMessages(szErrMsg,szErrMsg1) > 0)//Copy only the first line of message. For GPRS/Wifi/LAN problem ECR message.
                             strcpy(szMsg1, szErrMsg);
                        else if (inGetErrorMessage(szErrMsg) > 0)//For Must Settle and other transaction errors.
                             strcpy(szMsg1, szErrMsg);
                        else
                             strcpy(szMsg1, ECR_OPER_CANCEL_RESP);  /*OPERATOR CANCEL*/
                        
                        strcpy(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR);            
                   }									
              }
              
              //if (0 == srTransRec.MITid ) //set MITid to 1
              //srTransRec.MITid = 1;
         }
         
         vdCTOS_Pad_String(szMsg1, 40, IS_SPACE, POSITION_RIGHT);
         strcpy(srTransRec.szResponseText, szMsg1);
         
         
         //inNMTReadRecord(srTransRec.MITid);
         
         
         // For Merchant Name
         memset(srTransRec.szECRMerchantName, 0, sizeof(srTransRec.szECRMerchantName));
         memset(szBuffer, 0, sizeof(szBuffer));
         memset(szHeader, 0, sizeof(szHeader));
         memcpy(szHeader, strMMT[0].szRctHdr1,23);
         if(strlen(szHeader)<23)
              vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);

		 strncpy(szBuffer, szHeader, 23);
         
         memset(szHeader, 0, sizeof(szHeader));
         memcpy(szHeader, strMMT[0].szRctHdr2,23);
         if(strlen(szHeader)<23)
              vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
		 
         strncpy(&szBuffer[23], szHeader, 23);
         
         memset(szHeader, 0, sizeof(szHeader));
         memcpy(szHeader, strMMT[0].szRctHdr3,23);
         if(strlen(szHeader)<23)
              vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
		 
         strncpy(&szBuffer[46], szHeader, 23);
         strcpy(srTransRec.szECRMerchantName, szBuffer);

               /*if(srTransRec.fVirtualCard == TRUE)
               {
               if(srTransRec.HDTid == SMAC_HDT_INDEX)
               srTransRec.byEntryMode = CARD_ENTRY_ICC;
               else
               srTransRec.byEntryMode = CARD_ENTRY_MANUAL;
               } */
    }
    else //mercury ecr
    {
         if(strMMT[0].MITid)
         {
              memset(srTransRec.szMerchantName,0x00, sizeof(srTransRec.szMerchantName));
              
              memset(outbuf,0x00, sizeof(outbuf));
              strcpy(outbuf,strMMT[0].szMerchantName);
              vdCTOS_Pad_String(outbuf, 23, 0x20, POSITION_RIGHT);
              memcpy(srTransRec.szMerchantName, outbuf, 23);
              
              memset(outbuf,0x00, sizeof(outbuf));
              strcpy(outbuf,strMMT[0].szRctHdr1);
              vdCTOS_Pad_String(outbuf, 23, 0x20, POSITION_RIGHT);
              memcpy(srTransRec.szMerchantName+23, outbuf, 23);
              
              memset(outbuf,0x00, sizeof(outbuf));
              strcpy(outbuf,strMMT[0].szRctHdr2);
              vdCTOS_Pad_String(outbuf, 23, 0x20, POSITION_RIGHT);
              memcpy(srTransRec.szMerchantName+46, outbuf, 23);   
              srTransRec.szMerchantName[69]=0x00;
         }
         
         vdDebug_LogPrintf("Resp Merch Name:%s, len:%d",srTransRec.szMerchantName, strlen(srTransRec.szMerchantName));
         
         vdDebug_LogPrintf("--inMultiAP_ECRSendSuccessResponse--");
         
         if(fECRApproved == TRUE)
         {
              strcpy(srTransRec.szResponseText,"APPROVED");
              strcpy(srTransRec.szRespCode,"00");
         }
         else
         {
              if(strlen(srTransRec.szResponseText) <= 0)
                   strcpy(srTransRec.szResponseText,"OPERATOR CANCEL");
         }
         vdCTOS_Pad_String(srTransRec.szResponseText, 40, 0x20, POSITION_RIGHT);
         srTransRec.szResponseText[40]=0x00;    
    }
#if 0
				// For Merchant Name
				memset(srTransRec.szECRMerchantName, 0, sizeof(srTransRec.szECRMerchantName));
				memset(szBuffer, 0, sizeof(szBuffer));
				memset(szHeader, 0, sizeof(szHeader));
				strcpy(szHeader, strMMT[0].szRctHdr1);
				vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(szBuffer, szHeader, 23);

				memset(szHeader, 0, sizeof(szHeader));
				strcpy(szHeader, strMMT[0].szRctHdr2);				  
				vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(&szBuffer[23], szHeader, 23);

				memset(szHeader, 0, sizeof(szHeader));
				strcpy(szHeader, strMMT[0].szRctHdr3);
				vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(&szBuffer[46], szHeader, 23);
				strcpy(srTransRec.szECRMerchantName, szBuffer);
#endif		


				//----------------------------------------------------------------------------------------------------------------------
				// SAVE TO MULTIAP.S3DB -- sidumili
				//----------------------------------------------------------------------------------------------------------------------
				
				vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
				vdDebug_LogPrintf("RX ECR::LEN[%d] PAN FORMAT[%s]",strlen(srTransRec.szECRPANFormatted), srTransRec.szECRPANFormatted);
				vdDebug_LogPrintf("RX ECR::LEN[%d] MERCHANT NAME[%s]",strlen(srTransRec.szECRMerchantName), srTransRec.szECRMerchantName);
				vdDebug_LogPrintf("RX ECR::LEN[%d] RESPONSE CODE[%s]|inMsgid[%d]",strlen(srTransRec.szECRRespCode), srTransRec.szECRRespCode, inMsgid);
				vdDebug_LogPrintf("RX ECR::LEN[%d] RESPONSE TEXT[%s]",strlen(srTransRec.szECRRespText), srTransRec.szECRRespText);
				vdDebug_LogPrintf("RX ECR::LEN[%d] PAN[%s]", strlen(srTransRec.szPAN), srTransRec.szPAN);
				vdDebug_LogPrintf("RX ECR::LEN[%d] DE39[%s]",strlen(srTransRec.szRespCode), srTransRec.szRespCode);
				vdDebug_LogPrintf("RX ECR::LEN[%d] RREF[%s]",strlen(srTransRec.szRRN), srTransRec.szRRN);
				vdDebug_LogPrintf("RX ECR::ExpDate[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
				vdDebug_LogPrintf("RX ECR::LEN[%d] TID[%s]",strlen(srTransRec.szTID), srTransRec.szTID);
				vdDebug_LogPrintf("RX ECR::LEN[%d] MID[%s]",strlen(srTransRec.szMID), srTransRec.szMID);
				vdDebug_LogPrintf("RX ECR::HDTID:[%02d]",srTransRec.HDTid);
				vdDebug_LogPrintf("RX ECR::IITID:[%02d]",srTransRec.IITid);
				vdDebug_LogPrintf("RX ECR::Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
				vdDebug_LogPrintf("RX ECR::szBatchNo:[%02X%02X%02X]",srTransRec.szBatchNo[0], srTransRec.szBatchNo[1], srTransRec.szBatchNo[2]);
				//vdDebug_LogPrintf("RX ECR::bySMACPay_CardSeqNo:[%02X%02X]",srTransRec.bySMACPay_CardSeqNo[0], srTransRec.bySMACPay_CardSeqNo[1]);

			#if 0
				strcpy(srTransRec.baChipData,strMMT[0].szRctHdr1);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr2);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr3);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr4);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr5);
				vdDebug_LogPrintf("RX ECR::LEN[%d] szRctHdr1:%s",strlen(srTransRec.baChipData), srTransRec.baChipData);
			#endif
				
				vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
                if ((srTransRec.HDTid==MCC_HOST) || (srTransRec.HDTid==0))
                {
                     bret = inMultiAP_Database_BatchInsert(&srTransRec);
                     vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
                     if(d_OK != bret)
                     {
                          vdSetErrorMessage("MultiAP BatchInsert ERR");
                     } 
                }
						//vdSetECRTransactionFlg(0);
						//memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

				//outbuf[out_len++] = inSendECRResponseCommandID[0];//d_IPC_CMD_WAVE_SALE;
				//outbuf[out_len++] = d_SUCCESS;

				//inSendECRResponseFlag = 1;
				//put_env_int("ECRRESP",1);
				
				//bret = inMultiAP_SendChild(outbuf,out_len);
				//hubing enhance ECR
				bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
				vdSetECRTransactionFlg(0);
//				inCallJAVA_SendEcrResponse();
				/*if (bret != d_OK)
				{
					return d_OK;
				}*/
			}

	return d_OK;		
}

#else
// patrick add code 20141205 start
int inMultiAP_ECRSendSuccessResponse(void)
{
    BYTE bret = d_NO;
    BYTE outbuf[d_MAX_IPC_BUFFER]={0};
    USHORT out_len = 0;

    inSendECRResponseFlag = get_env_int("ECRRESP");

	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fGetECRTransactionFlg=[%d]", fGetECRTransactionFlg());
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,inSendECRResponseFlag[%d]", inSendECRResponseFlag);

    //if(inSendECRResponseFlag == 1) /*no need to resent*/
        //return 1;
	
    if (strTCT.fECR)
    {		
		if(fGetECRTransactionFlg() != TRUE)
			return d_OK;		

        bret= inMultiAP_Database_BatchDelete();
        vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
        if(d_OK != bret)
        {
            vdSetErrorMessage("MultiAP BatchDelete ERR");
        }

		vdDebug_LogPrintf("--inMultiAP_ECRSendSuccessResponse--");
		vdDebug_LogPrintf("Resp MITid:%d",strMMT[0].MITid);
		vdDebug_LogPrintf("Resp Len[%d] | szMerchantName:%s",strlen(strMMT[0].szMerchantName), strMMT[0].szMerchantName);
		vdDebug_LogPrintf("Resp Len[%d] | szRctHdr1:%s",strlen(strMMT[0].szRctHdr1), strMMT[0].szRctHdr1);
		vdDebug_LogPrintf("Resp Len[%d] | szRctHdr2:%s",strlen(strMMT[0].szRctHdr2), strMMT[0].szRctHdr2);
		vdDebug_LogPrintf("Resp Len[%d] | szRctHdr3:%s",strlen(strMMT[0].szRctHdr3), strMMT[0].szRctHdr3);
        vdDebug_LogPrintf("Resp Len[%d] | PAN:%s",strlen(srTransRec.szPAN), srTransRec.szPAN);
        vdDebug_LogPrintf("Resp Len[%d] | DE39:%s",strlen(srTransRec.szRespCode), srTransRec.szRespCode);
		vdDebug_LogPrintf("DE39:[%02X%02X]",srTransRec.szRespCode[0], srTransRec.szRespCode[1]);
        vdDebug_LogPrintf("Resp Len[%d] | RREF:%s",strlen(srTransRec.szRRN), srTransRec.szRRN);
        vdDebug_LogPrintf("Rex. sp ExpDate:[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
        vdDebug_LogPrintf("Resp Len[%d] | MID:%s",strlen(srTransRec.szTID), srTransRec.szTID);
        vdDebug_LogPrintf("Resp Len[%d] | TID:%s",strlen(srTransRec.szMID), srTransRec.szMID);
        vdDebug_LogPrintf("Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
        vdDebug_LogPrintf("Resp Response Text:%s, len:%d",srTransRec.szResponseText, strlen(srTransRec.szResponseText));
		

        if(strMMT[0].MITid)
        {
            memset(srTransRec.szMerchantName,0x00, sizeof(srTransRec.szMerchantName));

            memset(outbuf,0x00, sizeof(outbuf));
            strcpy(outbuf,strMMT[0].szMerchantName);
            vdCTOS_Pad_String(outbuf, 23, 0x20, POSITION_RIGHT);
            memcpy(srTransRec.szMerchantName, outbuf, 23);
            
            memset(outbuf,0x00, sizeof(outbuf));
            strcpy(outbuf,strMMT[0].szRctHdr1);
            vdCTOS_Pad_String(outbuf, 23, 0x20, POSITION_RIGHT);
            memcpy(srTransRec.szMerchantName+23, outbuf, 23);
            
            memset(outbuf,0x00, sizeof(outbuf));
            strcpy(outbuf,strMMT[0].szRctHdr2);
            vdCTOS_Pad_String(outbuf, 23, 0x20, POSITION_RIGHT);
            memcpy(srTransRec.szMerchantName+46, outbuf, 23);	
            srTransRec.szMerchantName[69]=0x00;
        }
		
        vdDebug_LogPrintf("Resp Merch Name:%s, len:%d",srTransRec.szMerchantName, strlen(srTransRec.szMerchantName));

		vdDebug_LogPrintf("--inMultiAP_ECRSendSuccessResponse--");
		
        if(fECRApproved == TRUE)
        {
            strcpy(srTransRec.szResponseText,"APPROVED");
			strcpy(srTransRec.szRespCode,"00");
        }
        else
        {
			if(strlen(srTransRec.szResponseText) <= 0)
                strcpy(srTransRec.szResponseText,"OPERATOR CANCEL");
        }
        vdCTOS_Pad_String(srTransRec.szResponseText, 40, 0x20, POSITION_RIGHT);
        srTransRec.szResponseText[40]=0x00;
			
        bret = inMultiAP_Database_BatchInsert(&srTransRec);
        vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
        if(d_OK != bret)
        {
            vdSetErrorMessage("MultiAP BatchInsert ERR");
        }
        
        vdSetECRTransactionFlg(0);
        
        outbuf[out_len++] = inSendECRResponseCommandID[0]; //d_IPC_CMD_WAVE_SALE; // 
        outbuf[out_len++] = d_SUCCESS;
        
        inSendECRResponseFlag = 1;		
		put_env_int("ECRRESP",1);//hubing enhance ECR
        //bret = inMultiAP_SendChild(outbuf,out_len);
        bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
        vdDebug_LogPrintf("call inCallJAVA_SendEcrResponse");
//        inCallJAVA_SendEcrResponse();
        if (bret != d_OK)
            return d_OK;
        
    }
    return d_OK;		
}
#endif

// patrick move to postrans?
int inMultiAP_ECRGetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
	BYTE szResponseCode[40], szResponseCode2[40];
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));

//for improve transaction speed
	if (inMultiAP_IPCGetChildEx(str, &ipc_len) == d_OK)
	{
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;

        vdSetECRTransactionFlg(1);
        
		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);

        // patrick add code 20141205 start		
        inSendECRResponseFlag = 0;
        put_env_int("ECRRESP",0);//hubing enhance ECR
        ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request

		vdClearMSRAndRemoveCard();
		
        // patrick add code 20141205 start
        inSendECRResponseFlag = get_env_int("ECRRESP");//hubing enhance ECR
        if (inSendECRResponseFlag == 1)	
        {
        	vdSetECRTransactionFlg(0);
            return d_OK;
        }
		// patrick add code 20141205 end

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}

		inTCTSave(1);
		inDatabase_TerminalOpenDatabaseEx(DB_MULTIAP);
		bret= inMultiAP_Database_BatchDeleteEx();
		vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchDelete ERR");
		}
		
                  vdDebug_LogPrintf("Resp Len[%d] | PAN:%s",strlen(srTransRec.szPAN), srTransRec.szPAN);
                  vdDebug_LogPrintf("Resp Len[%d] | DE39:%s",strlen(srTransRec.szRespCode), srTransRec.szRespCode);
                  vdDebug_LogPrintf("Resp Len[%d] | RREF:%s",strlen(srTransRec.szRRN), srTransRec.szRRN);
                  vdDebug_LogPrintf("Resp ExpDate:[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
                  vdDebug_LogPrintf("Resp Len[%d] | MID:%s",strlen(srTransRec.szTID), srTransRec.szTID);
                  vdDebug_LogPrintf("Resp Len[%d] | TID:%s",strlen(srTransRec.szMID), srTransRec.szMID);
                  vdDebug_LogPrintf("Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
        		
		bret = inMultiAP_Database_BatchInsert(&srTransRec);
		vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchInsert ERR");
		}
        inDatabase_TerminalCloseDatabase();
        vdSetECRTransactionFlg(0);
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		out_len = 0;
		outbuf[out_len++] = inSendECRResponseCommandID[0];//d_IPC_CMD_WAVE_SALE;
		outbuf[out_len++] = d_SUCCESS;

		//inSendECRResponseFlag = 1;
		//put_env_int("ECRRESP",1);
		
		//bret = inMultiAP_SendChild(outbuf,out_len);
		//hubing enhance ECR
		//bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
		bret = inMultiAP_SendAP_IPCCmd("com.persistent.app", outbuf,out_len);
//		bret = d_NO;
		if (bret != d_OK)
		{
			// patrick add code 20141205 start		
			//exit(1);
			/*
			my_pid = getpid();
			memset(outbuf,0x00,sizeof(outbuf));
			sprintf(outbuf,"kill -9 %d",my_pid);
			vdDebug_LogPrintf("kill,[%d][%s]",my_pid,outbuf);
			system(outbuf);
			*/
			return d_OK;
		}
		// patrick add code 20141205 start
		return d_OK;
	}
}

// patrick move to postrans?
int inMultiAP_GetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));

	if (inMultiAP_IPCGetChild(str, &ipc_len) == d_OK)
	{
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;

		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);
		ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}
		
		bret = inMultiAP_SendChild(outbuf,out_len);
//		bret = d_NO;
		if (bret != d_OK)
		{
			exit(1);
			/*
			my_pid = getpid();
			memset(outbuf,0x00,sizeof(outbuf));
			sprintf(outbuf,"kill -9 %d",my_pid);
			vdDebug_LogPrintf("kill,[%d][%s]",my_pid,outbuf);
			system(outbuf);
			*/
			return d_OK;
		}
	}
}

int inCTOS_MultiAPALLAppEventID(int IPC_EVENT_ID)
{
	int inResult = d_NO, inRetVal = d_NO;
//	int inLoop = 0;
	USHORT inLoop = 0;
	CTOS_stCAPInfo stinfo;
	BYTE byStr[25];
	USHORT Outlen; 
    char szAPName[125];
	int inAPPID;
	BYTE bret;
	BYTE szTempDebug [100+1];


	vdDebug_LogPrintf("saturn inCTOS_MultiAPALLAppEventID %d", IPC_EVENT_ID);
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
	  
	   vdDebug_LogPrintf("saturn inCTOS_MultiAPALLAppEventID for loop");
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		memset(byStr, 0x00, sizeof(byStr));
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
			memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
			CTOS_APGet(inLoop, &stinfo);
		}

//		CTOS_Delay(50);
//		sprintf(szTempDebug, "inLoop=[%d", inLoop);
//		CTOS_LCDTPrintXY(1, 6, szTempDebug);

//		vdDebug_LogPrintf("baName=[%s],inLoop=[%d] ", stinfo.baName, inLoop);

		if (strcmp(szAPName, stinfo.baName)!=0)
		{	
			if (stinfo.baName[0] != 0x00)
			{
				if (memcmp(stinfo.baName, "SHARLS_", 7)==0)
					continue;

				if (d_OK == inCTOSS_MultiCheckUnForkAPName(stinfo.baName))
				{
					vdDebug_LogPrintf("saturn Multi UnForkAPName22=[%s] ....", stinfo.baName);
					continue;
				}

//				CTOS_LCDTPrintXY(1, 7, stinfo.baName);

				//inTCTSave(1); /*for investigation*/

   				vdDebug_LogPrintf("saturn before batch delete");
				bret= inMultiAP_Database_BatchDelete();
				vdDebug_LogPrintf("saturn inMultiAP_Database_BatchDelete,bret=[%d]", bret);
				if(d_OK != bret)
			    	return bret;

//				CTOS_LCDTPrintXY(1, 3, "BatchDelete");

				bret = inMultiAP_Database_BatchInsert(&srTransRec);
				vdDebug_LogPrintf("saturn inMultiAP_Database_BatchInsert=[%d]", bret);
				if(d_OK != bret)
			    	return bret;

//				CTOS_LCDTPrintXY(1, 4, "BatchInsert");

				vdDebug_LogPrintf("saturn before inMultiAP_RunIPCCmdTypes");

				inRetVal = inMultiAP_RunIPCCmdTypes(stinfo.baName, IPC_EVENT_ID, srTransRec.szInvoiceNo,INVOICE_BCD_SIZE,byStr,&Outlen);    
//				CTOS_LCDTPrintXY(1, 8, "PFR");

				inTCTRead(1);

				vdDebug_LogPrintf("baName[%x][%d] ",byStr[1],byStr[1]);
				if ((byStr[1] == d_SUCCESS)||(byStr[1] == d_FAIL))
					return byStr[1];
				else
					continue;
			}
		}
	}
	return d_NOT_RECORD;
}


#if 1
int inCTOS_MultiAPBatchSearch(int IPC_EVENT_ID)
{
	int inResult = d_NO;
	BYTE szTitle[25+1];
	BYTE szDisMsg[100];
	BYTE byHoldTransType;

	byHoldTransType = srTransRec.byTransType;
	vdDebug_LogPrintf("--inCTOS_MultiAPBatchSearch--");
	vdDebug_LogPrintf("inSettlementHost=[%d], byHoldTransType[%d]", inSettlementHost, byHoldTransType);
	
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	//CTOS_LCDTClearDisplay();
    //vdClearNonTitleLines();
    //vdClearBelowLine(2);
	//vdDisplayStatusLine(DISPLAY_POSITION_LEFT, "PROCESSING...");
	//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS); // sidumili: remove to fix crash on tip adjust
	//CTOS_Delay(1000);
	
    inResult = inDatabase_InvoiceNumSearch(&srTransRec, srTransRec.szInvoiceNo);

	vdDebug_LogPrintf("inDatabase_InvoiceNumSearch=inResult[%d], srTransRec.HDTid[%d] inSettlementHost[%d], byHoldTransType[%d]", inResult, srTransRec.HDTid, inSettlementHost, byHoldTransType);
    if(inResult == d_OK)
    {
    	// sidumili: selected credit app and record is wechat/alipay
    	//if (inSettlementHost == 1 && byHoldTransType == VOID && (srTransRec.HDTid == WECHAT_HOST || srTransRec.HDTid == ALIPAY_HOST || srTransRec.HDTid == GCASH_HOST || srTransRec.HDTid == GRAB_HOST || srTransRec.HDTid == UPI_HOST || srTransRec.HDTid == PHQR_HOST))
        //if (byHoldTransType == VOID && (srTransRec.HDTid == WECHAT_HOST || srTransRec.HDTid == ALIPAY_HOST 
			//|| srTransRec.HDTid == GCASH_HOST || srTransRec.HDTid == GRAB_HOST || srTransRec.HDTid == UPI_HOST 
			//|| srTransRec.HDTid == PHQR_HOST))
		if (byHoldTransType == VOID && srTransRec.HDTid == PHQR_HOST) //only PHQR not allowed.	
		{
    		vdDisplayMessageBox(1, 8, "", "VOID NOT ALLOWED", "", MSG_TYPE_ERROR);
			CTOS_Beep();
	    	CTOS_Delay(1500);
			usCTOSS_LCDDisplay(" ");

			if((srTransRec.HDTid == PHQR_HOST) && (fGetECRTransactionFlg()==TRUE))
            {
                 vdSetECRResponse(ECR_UNKNOWN_ERR);
            }
	        return d_NO;
    	}
		
#if 0
		// sidumili: selected mccpay app and record is credit
		//if (inSettlementHost == 3 && byHoldTransType == VOID && (srTransRec.HDTid == MCC_HOST || srTransRec.HDTid == PHQR_HOST))
        if (byHoldTransType == VOID && (srTransRec.HDTid == MCC_HOST || srTransRec.HDTid == PHQR_HOST))  
		{
    		vdDisplayMessageBox(1, 8, "", "VOID NOT ALLOWED", "", MSG_TYPE_ERROR);
			CTOS_Beep();
	    	CTOS_Delay(1500);
			usCTOSS_LCDDisplay(" ");
			if((srTransRec.HDTid == PHQR_HOST) && (fGetECRTransactionFlg()==TRUE))
            {
                 vdSetECRResponse(ECR_UNKNOWN_ERR);
            }
			
	        return d_NO;
    	}
#endif

		inHDTRead(srTransRec.HDTid);
		//if(srTransRec.HDTid == 1)
		
		if(strcmp(strHDT.szAPName,"com.Source.S1_RBANK.RBANK")==0)
		{
			vdDebug_LogPrintf("saturn database batch search");
            inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);
            DebugAddSTR("inCTOS_BatchSearch","Processing...       ",20);
            DebugAddINT("inCTOS_BatchSearch", inResult);
			if(inResult != d_OK)
			{
                vdSetErrorMessage("NO RECORD FOUND");  
				return d_NO;
			}
		}
		else
		{
            inResult = inCTOS_MultiAPSaveData(IPC_EVENT_ID);
            if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
                return d_NO;            
        }
    }
    else
    {
        //vdSetErrorMessage("NO RECORD FOUND");
		
		//strcpy(szDisMsg, szTitle);
		//strcat(szDisMsg, "|");
		//strcat(szDisMsg, "NO RECORD FOUND");
		//usCTOSS_LCDDisplay(szDisMsg);					 //Tine:  25Apr2019		
		vdDisplayMessageBox(1, 8, "", "NO RECORD FOUND", "", MSG_TYPE_ERROR);
		CTOS_Beep();
    	CTOS_Delay(1500);
		usCTOSS_LCDDisplay(" ");
        return d_NOT_RECORD;
        return d_NO;
    }
	
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);
	
	return inResult;
}

#else
int inCTOS_MultiAPBatchSearch(int IPC_EVENT_ID)
{
	int inResult = d_NO;

	inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);

	DebugAddSTR("inCTOS_BatchSearch","Processing...",20);
	
	DebugAddINT("inCTOS_BatchSearch", inResult);

	if(inResult != d_OK)
    {
		inResult = inCTOS_MultiAPALLAppEventID(IPC_EVENT_ID);
		if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
			return d_NO;
			
		vdSetErrorMessage("NO RECORD FOUND");
		return d_NO;
    }
    
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);
	
	return inResult;
}
#endif
int inCTOS_MultiAPSaveData(int IPC_EVENT_ID)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[125];
	int inAPPID;
	
	memset(outbuf,0x00,sizeof(outbuf));
	memset(szAPName,0x00,sizeof(szAPName));

	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	//TEST
	//strcpy(strHDT.szAPName,"BANCNET");

	vdDebug_LogPrintf("saturn szAPName[%s]=[%s]", strHDT.szAPName,szAPName);

	if (strlen(strHDT.szAPName) == 0)
		return d_NO;

	//test
	if (strcmp (strHDT.szAPName,szAPName) == 0)
		return d_OK;

	//inTCTSave(1);

	bret= inMultiAP_Database_BatchDelete();
	vdDebug_LogPrintf("saturn inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("MultiAP BatchDelete ERR");
    	return bret;
	}
	
	bret = inMultiAP_Database_BatchInsert(&srTransRec);
	vdDebug_LogPrintf("saturn test 1=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("saturn MultiAP BatchInsert ERR");
    	return bret;
	}


	vdDebug_LogPrintf("V3 Run IPC test 2  %s[%d]", strHDT.szAPName,bret);
	
	inMultiAP_RunIPCCmdTypes(strHDT.szAPName,IPC_EVENT_ID,"",0, outbuf,&out_len);
	//inMultiAP_RunIPCCmdTypes("com.Source.S1_BANCNET.BANCNET",IPC_EVENT_ID,"",0, outbuf,&out_len);

	inTCTRead(1);
   
    //to remove processing display after going back from sub-app
   	usCTOSS_BackToProgress(" ");
	
	vdDebug_LogPrintf("saturn before call ipc event");
	if (outbuf[0] != IPC_EVENT_ID)
	{
		//vdSetErrorMessage("MultiAP Type ERR");
		return d_NO;
	}

	if (outbuf[1] != d_SUCCESS)
	{
		//vdSetErrorMessage("MultiAP ERR");
		return d_NO;
	}
	
	return d_ON;
}

int inCTOS_MultiAPCheckAllowd(void)
{
	char szAPName[125];
	int inAPPID;
	
	memset(szAPName,0x00,sizeof(szAPName));

	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("szAPName[%s]=[%s]", strHDT.szAPName,szAPName);

	if (strcmp (strHDT.szAPName,szAPName) == 0)
		return d_OK;

	vdDisplayErrorMsg(1, 8, "TRANS NOT ALLWD");
	return d_ON;
}

int inCTOS_MultiAPGetData(void)
{
	BYTE bret;
	
	vdDebug_LogPrintf("AAA >>-inCTOS_MultiAPGetData-----");
	//CTOS_LCDTPrintXY(1, 8, "LOADING TABLE... ");
	vdDebug_LogPrintf("AAA >>-inCTOS_MultiAPGetData----- 2");
	bret = inMultiAP_Database_BatchRead(&srTransRec);
	vdDebug_LogPrintf("AAA >>-inMultiAP_Database_BatchReadbret=[%d]", bret);
    
    DebugAddHEX("ECR AMOUNT", srTransRec.szBaseAmount, 6);
	if(d_OK != bret)
	{
		//vdSetErrorMessage("MultiAP BatchRead ERR");
		//vdDisplayErrorMsg(1, 8, "MultiAP BatchRead ERR");  
    	return bret;
	}

	bret= inMultiAP_Database_BatchDelete();
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		//vdSetErrorMessage("MultiAP Batch Delete ERR");
    	return bret;
	}

	return d_OK;
	
}

int inCTOS_MultiAPReloadHost(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
    inHostIndex = (short) srTransRec.HDTid;

	vdDebug_LogPrintf("inCTOS_MultiAPReloadTable,HDTid=[%d]",inHostIndex);
	
	if ( inHDTRead(inHostIndex) != d_OK)
	{
		vdSetErrorMessage("HOST SELECTION ERR");
		return(d_NO);
    } 
    else 
	{
		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

		inCurrencyIdx = strHDT.inCurrencyIdx;
		
		if (inCSTRead(inCurrencyIdx) != d_OK)
		{	
			vdSetErrorMessage("LOAD CST ERR");
	        return(d_NO);
        }

		if ( inCPTRead(inHostIndex) != d_OK)
		{
			vdSetErrorMessage("LOAD CST ERR");
			return(d_NO);
	    }
    }

	return (d_OK);
	
}


int inCTOS_MultiAPReloadTable(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
	vdDebug_LogPrintf("inCTOS_MultiAPReloadTable-CDTid[%d]--IITid[%d]--",srTransRec.CDTid,srTransRec.IITid);
	if (srTransRec.CDTid > 0)
	{
		if ( inCDTRead(srTransRec.CDTid) != d_OK)
		{
		    vdDebug_LogPrintf("LOAD CDT ERROR [%d]", srTransRec.CDTid);
			vdSetErrorMessage("LOAD CDT ERROR");
			return(d_NO);
	    }
	}

 	if (srTransRec.IITid > 0) 
    	inGetIssuerRecord(srTransRec.IITid);

    if(srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE) 
    {
	    strCDT.HDTid=srTransRec.HDTid;	
    }
    else if(srTransRec.fLoyalty == TRUE)
    {
        strCDT.HDTid=strCDT.inLoyaltyGroup=srTransRec.HDTid;  
    }
	else if(srTransRec.HDTid == 6)
	{
	    strCDT.HDTid=srTransRec.HDTid;	
	}

    inHostIndex = (short) strCDT.HDTid;
	
	vdDebug_LogPrintf("inCTOS_SelectHost =[%d],HDTid=[%d]",inHostIndex,strCDT.HDTid); 	
	vdDebug_LogPrintf("srTransRec.szPromo:(%s)", srTransRec.szPromo);
	vdDebug_LogPrintf("srTransRec.fInstallment:(%02x)", srTransRec.fInstallment);

	if ( inHDTRead(inHostIndex) != d_OK)
	{
		vdDisplayErrorMsg(1, 8, "HOST SELECTION ERR");
		return(d_NO);
    } 
    else 
	{
		srTransRec.HDTid = inHostIndex;
		inCurrencyIdx = strHDT.inCurrencyIdx;

		if (inCSTRead(inCurrencyIdx) != d_OK)
		{	
	        vdDisplayErrorMsg(1, 8, "CST ERR");  
	        return(d_NO);
        }

		if ( inCPTRead(inHostIndex) != d_OK)
		{
	        vdDisplayErrorMsg(1, 8, "CPT ERR");
			return(d_NO);
	    }
    }

	return (d_OK);
	
}

int inCTOS_MultiAPGetpowrfail(void)
{
	int inResult = d_NO;
	
	inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_POWER_FAIL);
	if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
			return d_NO;
}

int inCTOS_MultiAPGetVoid(void)
{
	BYTE bret;
	TRANS_DATA_TABLE srTransRectmp;
	
	vdDebug_LogPrintf("inCTOS_MultiAPGetVoid-----");
	
	bret = inMultiAP_Database_BatchRead(&srTransRectmp);
	vdDebug_LogPrintf("inMultiAP_Database_BatchReadbret=[%d]", bret);
	if(d_OK != bret)
    	return bret;

	bret= inMultiAP_Database_BatchDelete();
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
    	return bret;

	memcpy(srTransRec.szInvoiceNo,srTransRectmp.szInvoiceNo,3);
	vdDebug_LogPrintf("inCTOS_MultiAPGetVoid =[%02x%02x%02x]",srTransRec.szInvoiceNo[0],srTransRec.szInvoiceNo[1],srTransRec.szInvoiceNo[2]);

	return d_OK;
}

void vdCTOSS_DislplayMianAPMenu(int IPC_EVENT_ID)
{
	CTOS_LCDTClearDisplay();
		
	switch(IPC_EVENT_ID)
    {
        case d_IPC_CMD_FUN_KEY1:
            inF1KeyEvent();
            break;
        case d_IPC_CMD_FUN_KEY2:
            inF2KeyEvent();
            break;
        case d_IPC_CMD_FUN_KEY3:
            inF3KeyEvent();
            break;
		case d_IPC_CMD_FUN_KEY4:
            inF4KeyEvent();
            break;
		default:
        	break;
    }

	return;
}

int inCTOSS_MultiDislplayMenu(int IPC_EVENT_ID)
{
	CTOS_stCAPInfo stinfo;
	int inLoop = 0;
    char szHostMenu[1024];
	int inHostindex = 0;
    char szHostName[d_MAX_APP+1][25];
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[125];
	int inAPPID;

	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
	char szHeaderString[50] = "SELECT AP";

	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
	
	vdDebug_LogPrintf("begain inCTOSS_MultiDislplayMenu............");
	memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
	
  	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
 			CTOS_APGet(inLoop, &stinfo);
		}
		
 		vdDebug_LogPrintf("baName[%s] ", stinfo.baName);
 
		if (memcmp(stinfo.baName, "SHARLS_", 7) == 0)
			continue;

		if (stinfo.baName[0] != (char)0x00)
		{
			strncpy(szHostName[inHostindex++],stinfo.baName,25);
		}
	}

	for (inLoop = 0; inLoop < d_MAX_APP; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");      
        }
        else
            break;
    }

	vdDebug_LogPrintf("inHostindex=[%d]..........", inHostindex);
	if (inHostindex == 1)//only one app 
	{
		vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
		return d_OK;
	}
	
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
        vdDebug_LogPrintf("key=[%d],szHostName =[%s]", key, szHostName[key-1]);

		if (strcmp(szAPName, szHostName[key-1])==0)
		{
			vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
			return d_OK;
		}
		inTCTSave(1);
		
		inMultiAP_RunIPCCmdTypes(szHostName[key-1],IPC_EVENT_ID,"",0, outbuf,&out_len);

		inTCTRead(1);

		vdDebug_LogPrintf("end inCTOSS_MultiDislplayMenu............");
		
		if (outbuf[0] != IPC_EVENT_ID)
		{
			return d_NO;
		}

		if (outbuf[1] != d_SUCCESS)
		{
			return d_NO;
		}
	
	}
	
 	return d_OK;
}


int inCTOS_MultiSwitchApp(const char szAppName[25], int IPC_EVENT_ID)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[125];
	int inAPPID;
	int inRet = 0;

	vdDebug_LogPrintf("--inCTOS_MultiSwitchApp--");
	
	CTOS_LCDTClearDisplay();
	//vdCTOS_DispStatusMessage("PROCESSING...       ");
	//vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	//CTOS_Delay(500);
	
	memset(outbuf,0x00,sizeof(outbuf));
	memset(szAPName,0x00,sizeof(szAPName));

	vdDebug_LogPrintf("inCTOS_MultiAPSaveData szAppName[%s]=[%s]=%d",szAppName,szAPName, srTransRec.inCardType);

	if (strlen(szAppName) == 0)
		return d_NO;

	vdDebug_LogPrintf("inCTOS_MultiAPSaveData szAppName[%s]=[%s]=%d",szAppName,szAPName, srTransRec.inCardType);

	inRet = inMultiAP_RunIPCCmdTypes(szAppName,IPC_EVENT_ID,"",0, outbuf,&out_len);

	vdDebug_LogPrintf("inCTOS_MultiAPSaveData szAppName[%d]=[%d]",IPC_EVENT_ID,outbuf[0]);
	vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes:::inRet=[%d]", inRet);

	if(d_OK != inRet)
    {
	  vdDebug_LogPrintf("inCTOS_MultiSwitchApp FAILED!!!");
	  return inRet;
    }
	else
	{
		vdDebug_LogPrintf("inCTOS_MultiSwitchApp SUCCESS!!!");
		return d_OK;
	}
	
	/*
	if (outbuf[0] != IPC_EVENT_ID)
	{
		//vdSetErrorMessage("MultiAP Type ERR");
		return d_NO;
	}

	if (outbuf[1] != d_SUCCESS)
	{
		//vdSetErrorMessage("MultiAP ERR");
		return d_NO;
	}
	*/


	return d_ON;
}


