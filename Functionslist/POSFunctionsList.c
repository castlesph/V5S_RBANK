
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

#include "POSFunctionsList.h"
#include "..\Includes\POSTypedef.h"

#include <ctosapi.h>
#include <semaphore.h>
#include <pthread.h>

#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSConfig.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSBatch.h"
#include "..\Includes\POSSettlement.h"
#include "..\Includes\POSInstallment.h"
#include "..\Includes\POSLoyalty.h"
#include "..\Includes\POSDebit.h"
#include "..\Includes\PosTrans.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSCardVer.h"
#include "..\Includes\POSVerComp.h"

#include "..\Includes\CTOSInput.h"

#include "..\ui\Display.h"
#include "..\print\print.h"
#include "..\Debug\Debug.h"
#include "..\Includes\DMenu.h"
#include "..\Ctls\POSWave.h"

#include "..\Includes\Posreg.h"

#include "..\Includes\PosComp.h"
#include "..\Includes\PosCash.h"

#include "..\tms\tms.h"

#include "../FileModule/myFileFunc.h"
#include "..\Aptrans\MultiAptrans.h"


extern TRANS_DATA_TABLE* srGetISOEngTransDataAddress(void);

#define MAXFUNCTIONS 		2048
extern void vdCTOS_FunctionKeyEx(void);
// can declare any functions type and link with string.
Func_vdFunc stFunctionList_vdFunc[] = {
	{"inCTOS_SALE_OFFLINE", inCTOS_SALE_OFFLINE},
	{"inCTOS_WAVE_SALE", inCTOS_WAVE_SALE},
	{"inCTOS_WAVE_REFUND", inCTOS_WAVE_REFUND},
	{"vdCTOS_InitWaveData", (DMENU_FUN)vdCTOS_InitWaveData},
	{"inCTOS_VOID", inCTOS_VOID},
	{"inCTOS_SETTLEMENT", inCTOS_SETTLEMENT},
	{"inCTOS_SETTLE_ALL", inCTOS_SETTLE_ALL},
	{"inCTOS_SettleAllHosts", inCTOS_SettleAllHosts},
	{"inCTOS_SettleBancnet", inCTOS_SettleBancnet},
	{"inCTOS_SettleDigitalWallet", inCTOS_SettleDigitalWallet},
	{"inCTOS_PREAUTH", inCTOS_PREAUTH},
	{"inCTOS_REFUND", inCTOS_REFUND},
	{"inCTOS_TIPADJUST", inCTOS_TIPADJUST},
	{"inCTOS_BATCH_REVIEW", inCTOS_BATCH_REVIEW},
	{"inCTOS_BATCH_TOTAL", inCTOS_BATCH_TOTAL},
	{"inCTOS_REPRINT_LAST", inCTOS_REPRINT_LAST},
	{"inCTOS_REPRINT_ANY", inCTOS_REPRINT_ANY},
	{"inCTOS_REPRINTF_LAST_SETTLEMENT", inCTOS_REPRINTF_LAST_SETTLEMENT},
	{"inCTOS_PRINTF_SUMMARY", inCTOS_PRINTF_SUMMARY},
	{"inCTOS_PRINTF_DETAIL", inCTOS_PRINTF_DETAIL},
	{"inCTOS_PRINTF_SUMMARY", inCTOS_PRINTF_SUMMARY},
	{"inCTOS_PRINTF_DETAIL", inCTOS_PRINTF_DETAIL},
	{"inCTOS_REPRINT_LAST", inCTOS_REPRINT_LAST},
	{"inCTOS_REPRINT_ANY", inCTOS_REPRINT_ANY},
	{"inCTOS_REPRINTF_LAST_SETTLEMENT", inCTOS_REPRINTF_LAST_SETTLEMENT},
	{"vdCTOS_uiPowerOff", (DMENU_FUN)vdCTOS_uiPowerOff},
	{"vdCTOS_IPConfig", (DMENU_FUN)vdCTOS_IPConfig},
//	{"vdCTOS_IPConfig", vdCTOS_ChangeComm},
	{"vdCTOS_DialConfig", (DMENU_FUN)vdCTOS_DialConfig},
	{"vdCTOS_ModifyEdcSetting", (DMENU_FUN)vdCTOS_ModifyEdcSetting},	
	
	{"vdCTOS_TMSRangeSetting", (DMENU_FUN)vdCTOS_TMSRangeSetting},	
	
	{"vdEditSTAN", (DMENU_FUN)vdCTOSS_ModifyStanNumber}, //@@IBR ADD 20170202
	{"vdCTOS_GPRSSetting", (DMENU_FUN)vdCTOS_GPRSSetting},
	{"vdCTOS_DeleteBatch", (DMENU_FUN)vdCTOS_DeleteBatch},
	{"vdCTOS_PrintEMVTerminalConfig", (DMENU_FUN)vdCTOS_PrintEMVTerminalConfig},
	{"vdCTOSS_PrintTerminalConfig", (DMENU_FUN)vdCTOSS_PrintTerminalConfig},
	{"inSelectTelcoSetting", (DMENU_FUN)inSelectTelcoSetting},
	{"vdCTOS_TipAllowd", (DMENU_FUN)vdCTOS_TipAllowd},
	{"vdCTOS_Debugmode", (DMENU_FUN)vdCTOS_Debugmode},
	{"vdCTOSS_SelectPinpadType", (DMENU_FUN)vdCTOSS_SelectPinpadType},
	{"vdCTOSS_InjectMKKey", (DMENU_FUN)vdCTOSS_InjectMKKey},
	{"vdCTOSS_CtlsMode", (DMENU_FUN)vdCTOSS_CtlsMode},
	{"vdCTOS_DemoMode", (DMENU_FUN)vdCTOS_DemoMode},	
	{"vdCTOSS_DownloadMode", (DMENU_FUN)vdCTOSS_DownloadMode},	
	{"vdCTOSS_CheckMemory", (DMENU_FUN)vdCTOSS_CheckMemory},	
	{"CTOSS_SetRTC", (DMENU_FUN)CTOSS_SetRTC},	
	{"inCTOS_SALE", (DMENU_FUN)inCTOS_SALE},	
    {"vdCTOS_PrintCRC", (DMENU_FUN)vdCTOS_PrintCRC},
	{"vdCTOS_HostInfo", (DMENU_FUN)vdCTOS_HostInfo},
//    {"vdCTOS_FunctionKey", (DMENU_FUN)vdCTOS_FunctionKey},
        {"vdCTOS_FunctionKey", (DMENU_FUN)vdCTOS_FunctionKeyEx},
	{"inCTOS_INSTALLMENT", (DMENU_FUN)inCTOS_INSTALLMENT},	
	{"inCTOS_CASH2GO", (DMENU_FUN)inCTOS_CASH2GO},		
    {"inCTOS_DebitSelection", (DMENU_FUN)inCTOS_DebitSelection},	
    {"inCTOS_DebitSale", (DMENU_FUN)inCTOS_DebitSale},	
    {"inCTOS_DebitBalInq", (DMENU_FUN)inCTOS_DebitBalInq},	
    {"inCTOS_DebitLogon", (DMENU_FUN)inCTOS_DebitLogon},	
	{"inCTOS_PromptPassword", inCTOS_PromptPassword},
	{"vdCTOS_DeleteReversal", vdCTOS_DeleteReversal},	
    {"inCTOS_Redeem5050", inCTOS_Redeem5050},	
    {"inCTOS_RedeemVariable", inCTOS_RedeemVariable},	
    {"inCTOS_LoyaltyBalanceInquiry", inCTOS_LoyaltyBalanceInquiry},			
	{"inCTOS_DinersINST", inCTOS_DinersINST}, 
    {"inCTOS_DinersSALE", inCTOS_DinersSALE}, 
    {"inCTOS_Reg", inCTOS_Reg},			
    {"inCTOS_RegUSD", inCTOS_RegUSD},			
    {"inCTOS_DinersCardVer", inCTOS_DinersCardVer},		
	{"inCTOS_DinersSaleOffline", inCTOS_DinersSaleOffline},
	{"inCTOS_DinersRefund", inCTOS_DinersRefund},
	{"inCTOS_StoreCardINST", inCTOS_StoreCardINST},
	{"inCTOS_COMP", inCTOS_COMP},	
	{"inCTOS_CASH", inCTOS_CASH},	
//	{"vdCTOS_TerminalConfig", (DMENU_FUN)vdCTOS_TerminalConfig},
    {"inCTOSS_TMSDownloadRequest", (DMENU_FUN)inCTOSS_TMSDownloadRequest},
    {"vdCTOS_TMSSetting", (DMENU_FUN)vdCTOS_TMSSetting},
    {"vdCTOSS_ChangeDefaultApplication", vdCTOSS_ChangeDefaultApplication},
    {"inCTOS_POSVERCOMP", inCTOS_POSVERCOMP},		
    {"inCTOS_POSVERCOMPVOID", inCTOS_POSVERCOMPVOID},
    {"inCTOS_BancnetSale", inCTOS_BancnetSale},
    {"inCTOS_BancnetBalInq", inCTOS_BancnetBalInq},
    {"inCTOS_BancnetCashOut", inCTOS_BancnetCashOut},
//    {"vdCTOS_ERMConfig", vdCTOS_ERMConfig},
    {"vdCTOS_ECRConfig", vdCTOS_ECRConfig},
    {"vdCTOSS_EditTable", vdCTOSS_EditTable},
	//{"vdCTOSS_EditTable", inEditDatabase},	
    {"inCTOS_WAVE_CUP_SALE", inCTOS_WAVE_CUP_SALE},
	{"vdCITASServices", vdCITASServices},
	{"vdMerchantServices", vdMerchantServices},
	{"CTOSS_SetRTCEx", CTOSS_SetRTCEx},	
//	{"inCTOS_PromptReprintPassword", inCTOS_PromptReprintPassword},	
	{"inQRPAY", inQRPAY},
	{"inCTOS_Retrieve",inCTOS_Retrieve},
 	{"inEditDatabase",inEditDatabase},
	{"vdPrintEMVTags",vdPrintEMVTags},
	{"vdCTOS_PrintIP",vdCTOS_PrintIP},
//	{"vdCTOS_ChangeTelco",vdCTOS_ChangeTelco},
	{"vdCTOS_EditHost", vdCTOS_EditHost},
	{"vdCTOS_EditIP", vdCTOS_EditIP},
	{"vdCTOS_EditTerminal", vdCTOS_EditTerminal},
	{"vdCTOS_EditPrinter", vdCTOS_EditPrinter},
	{"vdCTOS_DeleteISOLog", vdCTOS_DeleteISOLog},
	{"vdCTOS_UserManual", vdCTOS_UserManual},
	{"vdCTOS_ReceiptSelect", vdCTOS_ReceiptSelect},
	{"vdCTOS_CTMSUPDATE", vdCTOS_CTMSUPDATE},
	{"inPHQRSale", inPHQRSale},
	{"inCTOS_PHQRRetreive", inCTOS_PHQRRetreive},
	{"inCTOS_SettlePHQR", inCTOS_SettlePHQR},
	{"vdCTOS_ECRSettings", vdCTOS_ECRSettings},
	{"vdSetECRConfig", vdSetECRConfig},
	
	{"inLogout", inLogout},
	{"inCTOS_SetupMenu", inCTOS_SetupMenu},
	{"inCTOS_ReportMenu", inCTOS_ReportMenu},
	{"inCTOS_CashIn", inCTOS_CashIn},
	{"inCTOS_CashOut", inCTOS_CashOut},
	{"inCTOS_CardlessBanking", inCTOS_CardlessBanking},
	{"inCTOS_Payment", inCTOS_Payment},
	{"inCTOS_FundTransfer", inCTOS_FundTransfer},
    {"inCTOS_RequestTerminalKey", inCTOS_RequestTerminalKey},
    {"vdCTOS_EditBiller", vdCTOS_EditBiller},
    {"vdCTOS_EditUser", vdCTOS_EditUser},
    {"vdCTOS_EditEnvParam", (DMENU_FUN)vdCTOS_EditEnvParamDB},
    {"inPing", inPing},
 	{"", (DFUNCTION_LIST)NULL},
};

// can declare any functions type and link with string.
Func_inISOPack stFunctionList_inISOPack[] = {
	{"inPackIsoFunc02", inPackIsoFunc02},
	{"inPackIsoFunc03", inPackIsoFunc03},
	{"inPackIsoFunc04", inPackIsoFunc04},
	{"inPackIsoFunc05", inPackIsoFunc05}, /*cash mo*/
	{"inPackIsoFunc11", inPackIsoFunc11},
	{"inPackIsoFunc12", inPackIsoFunc12},
	{"inPackIsoFunc13", inPackIsoFunc13},
	{"inPackIsoFunc14", inPackIsoFunc14},
	{"inPackIsoFunc18", inPackIsoFunc18},
	{"inPackIsoFunc22", inPackIsoFunc22},
	{"inPackIsoFunc23", inPackIsoFunc23},
	{"inPackIsoFunc24", inPackIsoFunc24},
	{"inPackIsoFunc25", inPackIsoFunc25},
	{"inPackIsoFunc26", inPackIsoFunc26},
	{"inPackIsoFunc35", inPackIsoFunc35},
	{"inPackIsoFunc37", inPackIsoFunc37},
	{"inPackIsoFunc38", inPackIsoFunc38},
	{"inPackIsoFunc39", inPackIsoFunc39},
	{"inPackIsoFunc41", inPackIsoFunc41},
	{"inPackIsoFunc42", inPackIsoFunc42},
	{"inPackIsoFunc45", inPackIsoFunc45},
	{"inPackIsoFunc48", inPackIsoFunc48},
	{"inPackIsoFunc49", inPackIsoFunc49},	
	{"inPackIsoFunc50", inPackIsoFunc50},
	{"inPackIsoFunc52", inPackIsoFunc52},
	{"inPackIsoFunc54", inPackIsoFunc54},
	{"inPackIsoFunc55", inPackIsoFunc55},
	{"inPackIsoFunc56", inPackIsoFunc56},
	{"inPackIsoFunc57", inPackIsoFunc57},
	{"inPackIsoFunc60", inPackIsoFunc60},
	{"inPackIsoFunc61", inPackIsoFunc61},
	{"inPackIsoFunc62", inPackIsoFunc62},
	{"inPackIsoFunc63", inPackIsoFunc63},
	{"inPackIsoFunc64", inPackIsoFunc64},
	{"", (DFUNCTION_inISOPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOUnPack stFunctionList_inISOUnPack[] = {
	{"inUnPackIsoFunc04", inUnPackIsoFunc04},
	{"inUnPackIsoFunc05", inUnPackIsoFunc05},	
	{"inUnPackIsoFunc11", inUnPackIsoFunc11},
	{"inUnPackIsoFunc12", inUnPackIsoFunc12},
	{"inUnPackIsoFunc13", inUnPackIsoFunc13},
	{"inUnPackIsoFunc37", inUnPackIsoFunc37},
	{"inUnPackIsoFunc38", inUnPackIsoFunc38},
	{"inUnPackIsoFunc39", inUnPackIsoFunc39},
	{"inUnPackIsoFunc41", inUnPackIsoFunc41},
	{"inUnPackIsoFunc44", inUnPackIsoFunc44},
	{"inUnPackIsoFunc50", inUnPackIsoFunc50},
	{"inUnPackIsoFunc55", inUnPackIsoFunc55},
	{"inUnPackIsoFunc56", inUnPackIsoFunc56},
	{"inUnPackIsoFunc57", inUnPackIsoFunc57},
    {"inUnPackIsoFunc53", inUnPackIsoFunc53},
    {"inUnPackIsoFunc58", inUnPackIsoFunc58},
    {"inUnPackIsoFunc59", inUnPackIsoFunc59},
	{"inUnPackIsoFunc60", inUnPackIsoFunc60},    
	{"inUnPackIsoFunc61", inUnPackIsoFunc61},    
	{"inUnPackIsoFunc63", inUnPackIsoFunc63},
	{"inUnPackIsoUnknown", inUnPackIsoUnknown},	
	{"", (DFUNCTION_inISOUnPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOCheck stFunctionList_inISOCheck[] = {
	{"", (DFUNCTION_inISOCheck)NULL},
};

int inPOSFunctionList(void)
{		
}

int inCTOSS_ExeFunction(char *INuszFunctionName)
{
	int inDex, inRetVal = -1;

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
			if (stFunctionList_vdFunc[inDex].uszFunctionName[0]==0x00)
			{
		        vduiWarningSound();
				vduiDisplayStringCenter(7,INuszFunctionName);
				vduiDisplayStringCenter(8,"FUNCTION INVALID");
				break;
			}
			
			if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_vdFunc[inDex].uszFunctionName))
			{
			   vdDebug_LogPrintf("%s", INuszFunctionName); 	  			
			   inRetVal = stFunctionList_vdFunc[inDex].d_FunctionP();
			   break;
			}
	 }
	 return(inRetVal);
}

int inExeFunction_PackISO(char *INuszFunctionName, unsigned char *uszSendData)
{
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;

    	//CTOS_PrinterPutString("inExeFunction_PackISO");

    srTransPara = srGetISOEngTransDataAddress();

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
	 
		  if (stFunctionList_inISOPack[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOPack[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		 
			   inRetVal = stFunctionList_inISOPack[inDex].d_FunctionP(srTransPara, uszSendData);
			   break;
		  }
	 }

	 		 
	 return(inRetVal);
}

int inExeFunction_UnPackISO(char *INuszFunctionName, unsigned char *uszReceiveData)
{	
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

	if (INuszFunctionName[0] == 0x00)
		return inRetVal;
		
	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOUnPack[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOUnPack[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		  
			   inRetVal = stFunctionList_inISOUnPack[inDex].d_FunctionP(srTransPara, uszReceiveData);
			   break;
		  }
	 }
	 return(inRetVal);
}

int inExeFunction_CheckISO(char *INuszFunctionName, unsigned char *uszSendData, unsigned char *uszReceiveData)
{
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;
    
    srTransPara = srGetISOEngTransDataAddress();

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOCheck[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOCheck[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		 		  
			   inRetVal = stFunctionList_inISOCheck[inDex].d_FunctionP(srTransPara, uszSendData, uszReceiveData);
			   break;
		  }
	 }
	 return(inRetVal);
}

int inQRPAY(void)
{
   	int inRet = 0;
	char szAppName[30];



	vdDebug_LogPrintf("SATURN inQRPAY");

   	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
        return inRet;

   memset(szAppName, 0x00, sizeof(szAppName));
   strcpy(szAppName, "com.Source.S1_MCCPAY.MCCPAY");
   


   vdDebug_LogPrintf("SATURN inReserveApp1 APPNAME %s", szAppName);

   inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_QRPAY);
   
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("saturn inQRPAY app FAIL");
	  return inRet;
   }


    return d_OK;
}

int inCTOS_Retrieve(void)
{
   	int inRet = 0;
	char szAppName[30];

	vdDebug_LogPrintf("SATURN inCTOS_Retrieve");

   	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
        return inRet;

   memset(szAppName, 0x00, sizeof(szAppName));
   strcpy(szAppName, "com.Source.S1_MCCPAY.MCCPAY");


   vdDebug_LogPrintf("inReserveApp1 APPNAME %s", szAppName);

   inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_RETRIEVE);
   
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("saturn inCTOS_Retrieve app FAIL");
	  return inRet;
   }


    return d_OK;
}


//phqr
int inPHQRSale(void)
{
   	int inRet = 0;
	char szAppName[30];



	vdDebug_LogPrintf("SATURN inPHQRSale");

   	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
        return inRet;

   memset(szAppName, 0x00, sizeof(szAppName));
   strcpy(szAppName, "com.Source.S1_MCCPAY.MCCPAY");
   


   vdDebug_LogPrintf("SATURN inReserveApp1 APPNAME %s", szAppName);

   inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_PHQR_SALE);
   
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("saturn inPHQRSale app FAIL");
	  return inRet;
   }


    return d_OK;
}

int inCTOS_PHQRRetreive(void)
{
   	int inRet = 0;
	char szAppName[30];



	vdDebug_LogPrintf("SATURN inCTOS_PHQRRetreive");

   	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
        return inRet;

   memset(szAppName, 0x00, sizeof(szAppName));
   strcpy(szAppName, "com.Source.S1_MCCPAY.MCCPAY");
   


   vdDebug_LogPrintf("SATURN inReserveApp1 APPNAME %s", szAppName);

   inRet = inCTOS_MultiSwitchApp(szAppName, d_IPC_CMD_PHQR_RETRIEVE);
   
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("saturn inPHQRSale app FAIL");
	  return inRet;
   }


    return d_OK;
}



