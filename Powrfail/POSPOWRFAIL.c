
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>


#include "POSPOWRFAIL.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\myEZLib.h"
#include "..\ui\Display.h"
#include "..\debug\debug.h"
#include "..\Includes\POSHost.h"
#include "..\Comm\V5Comm.h"
#include "..\print\Print.h"
#include "..\includes\POSbatch.h"
#include "..\accum\accum.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\DataBase\DataBaseFunc.h"
#include <epad.h>

extern BOOL	fRePrintFlag; 
extern BOOL fRePrintPwrFailFlag;

int inCTOS_Readpowrfail(STRUCT_FILE_SETTING *strFile)
{

    return (inMyFile_RecRead(strFile));    
}

int inCTOS_Savepowrfail(STRUCT_FILE_SETTING *strFile)
{
    return (inMyFile_RecSave(strFile));    
}


int inCTOS_Clearpowrfail(char *szFileName)
{
	int inResult;

	
	if((inResult = CTOS_FileDelete(szFileName)) != d_OK)
    {
        return ST_ERROR;
    }     
	
    return ST_SUCCESS;
	
}


int inCTLOS_Updatepowrfail(short inPowrfailtag)
{
	STRUCT_FILE_SETTING strFile;
	POWRFAIL_REC strTotal;
	int inResult;

	memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(&strTotal, 0x00, sizeof(POWRFAIL_REC));

	strTotal.inPowrfailtag = inPowrfailtag;
	strTotal.srTransPowerRec = srTransRec;

	strcpy(strFile.szFileName, PFR_FIME_NAME);
	
	strFile.bSeekType           = d_SEEK_FROM_EOF;
    strFile.bStorageType        = d_STORAGE_FLASH ;
    strFile.fCloseFileNow       = TRUE;
    strFile.ulRecSize           = sizeof(POWRFAIL_REC);
    strFile.ptrRec              = &strTotal;
    strFile.inSeekCnt           = 0;//fseek 0 from end of file   

	if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) >= 0)
    {
        if (ST_SUCCESS != inCTOS_Clearpowrfail(strFile.szFileName))
			return ST_ERROR;
    }

	if (inPowrfailtag == PFR_IDLE_STATE)
		return ST_SUCCESS;
	
	inResult = inCTOS_Savepowrfail(&strFile);

	vdDebug_LogPrintf("inCTLOS_Updatepowrfail =[%d],[%d]", inResult,inPowrfailtag);

	return inResult;
}

int inCTLOS_Getpowrfail(void)
{
	STRUCT_FILE_SETTING strFile;
	POWRFAIL_REC strTotal;
	int inResult;
	char szFileName[d_BUFF_SIZE];
    int  inFileSize;
	int inRet;
 
    memset(szFileName, 0x00, sizeof(szFileName));
	strcpy(szFileName, PFR_FIME_NAME);
	vdDebug_LogPrintf("SATURN inCTLOS_Getpowrfail =[%s]", szFileName);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPGetpowrfail();
	}

	if((inResult = inMyFile_CheckFileExist(szFileName)) < 0)
    {
        return d_NOT_RECORD;
    }

	memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(&strTotal, 0x00, sizeof(POWRFAIL_REC));
    strcpy(strFile.szFileName, PFR_FIME_NAME);    
    strFile.bSeekType           = d_SEEK_FROM_BEGINNING;
    strFile.bStorageType        = d_STORAGE_FLASH ;
    strFile.fCloseFileNow       = TRUE;
    strFile.ulRecSize           = sizeof(POWRFAIL_REC);
    strFile.ptrRec              = &strTotal;
    strFile.inSeekCnt           = 0;//fseek 0 from end of file 

	inResult = inCTOS_Readpowrfail(&strFile);

vdDebug_LogPrintf("SATURN inPowrfailtag =[%d]", strTotal.inPowrfailtag);
	

	if (strTotal.inPowrfailtag == PFR_IDLE_STATE)
		return d_NOT_RECORD;

    vdCTOSS_DelBackupS3DB();
		
    fRePrintFlag = TRUE;
    fRePrintPwrFailFlag = TRUE;
	
	if (strTotal.inPowrfailtag == PFR_BEGIN_SEND_ISO)
	{
		//before update batch after send ISO, print error reciep and disp error message
		//CTOS_LCDTClearDisplay();
		//CTOS_LCDTPrintXY(1, 7, "TRANS NOT SUC.");
		//vdDisplayErrorMsg(1, 8, "PLS TRY AGAIN");	

		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

        srTransRec= strTotal.srTransPowerRec;
        inRet = inCTOS_LoadCDTandIIT();
        inRet = inCTOS_VoidSelectHost();
		
		//inRet = inCTOS_inDisconnect();
		//inRet = ushCTOS_printErrorReceipt();

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

	}

 	if (strTotal.inPowrfailtag == PFR_BEGIN_BATCH_UPDATE)
	{
		//befor update batch, update bach & accum, print the lastrecipt
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		srTransRec= strTotal.srTransPowerRec;

		inRet = inCTOS_LoadCDTandIIT();

		inRet = inCTOS_VoidSelectHost();
	
		inRet = inCTOS_SaveBatchTxn();

    	inRet = inCTOS_UpdateAccumTotal();

		inRet = inCTOS_inDisconnect();
        
		inRet = ushCTOS_printReceipt();

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	}
	
	if (strTotal.inPowrfailtag == PFR_BATCH_UPDATE_COMPLETE)
	{
		//after update batch, print the lastrecipt
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		srTransRec= strTotal.srTransPowerRec;

		inRet = inCTOS_LoadCDTandIIT();

		inRet = inCTOS_VoidSelectHost();

		inRet = ushCTOS_printReceipt();

		inRet = inCTOS_inDisconnect();

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	}
 	
	if (strTotal.inPowrfailtag == PFR_BATCH_SETTLEMENT_PRINT)
	{
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
        srTransRec= strTotal.srTransPowerRec;
        inRet = inCTOS_VoidSelectHost();
        inRet = inCTOS_PrintSettleReport(strTotal.srTransPowerRec.fManualSettle, FALSE);
        inRet = inCTOS_SettlementClearBathAndAccum(strTotal.srTransPowerRec.fManualSettle);
        //inRet = inCTOS_inDisconnect();
        inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	}

	if (strTotal.inPowrfailtag == PFR_EMV_PROCESSING)
	{
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
        srTransRec= strTotal.srTransPowerRec;
        inRet = inCTOS_LoadCDTandIIT();
		
        inRet = inCTOS_VoidSelectHost();
		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));	

        CTOS_LCDTClearDisplay();
		
        CTOS_LCDTPrintXY(1, 7, "RECLAIMING FLASH"); 
        CTOS_LCDTPrintXY(1, 8, "RESTARTING...");

		CTOS_SystemReset();
	}

	vdSetErrorMessage("");
	
    fRePrintFlag = FALSE;
    fRePrintPwrFailFlag = FALSE;
		
    CTOS_LCDTClearDisplay();
	
	return ST_SUCCESS;
}



