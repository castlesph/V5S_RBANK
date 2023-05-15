
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

#include "..\Includes\CTOSInput.h"

#include "..\ui\Display.h"
#include "..\print\print.h"
#include "..\Debug\Debug.h"
#include "..\Includes\DMenu.h"
#include "..\Ctls\POSWave.h"

#include "POSFunctionKey.h"

#include "..\FileModule\myFileFunc.h"
#include "..\Database\DatabaseFunc.h"

#include "../Comm/V5Comm.h"

/*display host name, nii, tid, mid*/
void vdDisplayHostDetails(void)    
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
	char szStrNII[4+1];
    BYTE key;
	    
    //by host and merchant
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
	
    strHDT.inHostIndex = shHostIndex;
            
    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("TERMINAL CONFIG");

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%s", strHDT.szHostLabel);
	CTOS_LCDTPrintXY(1, 3, szStr);


	memset(szStrNII, 0, sizeof(szStrNII));
	wub_hex_2_str(strHDT.szNII, szStrNII, 2);
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "NII : %s", szStrNII);
	CTOS_LCDTPrintXY(1, 5, szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "TID : %s", strMMT[0].szTID);
	CTOS_LCDTPrintXY(1, 6, szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "MID : %s", strMMT[0].szMID);
	CTOS_LCDTPrintXY(1, 7, szStr);

    CTOS_KBDGet(&key);
	
    return; 
}

void vdTraceNo(BOOL fGetTraceNo)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;

    if(fGetTraceNo)
    {
        if(inCheckAllBatchEmtpy() > 0)
        {
            vduiClearBelow(2);
			vdDisplayErrorMsg(1, 8, "BATCH NOT EMPTY.");
            
            return;	
        }
    }
	
	inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("TRACE NO.");

	CTOS_LCDTPrintXY(1, 3, "TRACE NO.");

	ulTraceNo=wub_bcd_2_long(strTCT.szInvoiceNo,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%06ld", ulTraceNo);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fGetTraceNo == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
                if(ulTraceNo >= atoi(strOut))
                {
					vdDisplayErrorMsg2(1, 8, "INVALID INPUT, VALUE", "MUST BE GREATER!", MSG_TYPE_ERROR);
					vduiClearBelow(7);
                }
				else
				{
                    ulTraceNo=atoi(strOut);
                     memset(szStr, 0, sizeof(szStr));
                    sprintf(szStr, "%06ld", ulTraceNo);
                    inAscii2Bcd(szStr, strTCT.szInvoiceNo, 3);
                    inTCTSave(1);
                    break;
				}
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdSTANNo(BOOL fGetSTABNo)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
    //by host and merchant
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    if(fGetSTABNo)
    {
        if(inCheckBatchEmtpyPerHost(strHDT.inHostIndex) > 0)
        {
            vduiClearBelow(2);
            vdDisplayErrorMsg(1, 8, "BATCH NOT EMPTY.");

            return;	
        }
    }	
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("STAN NO.");

	CTOS_LCDTPrintXY(1, 3, "STAN NO.");

	ulTraceNo=wub_bcd_2_long(strHDT.szTraceNo,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%06ld", ulTraceNo);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fGetSTABNo == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
                ulTraceNo=atoi(strOut);
				memset(szStr, 0, sizeof(szStr));
				sprintf(szStr, "%06ld", ulTraceNo);
				inAscii2Bcd(szStr, strHDT.szTraceNo, 3);
				inHDTSave(shHostIndex);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdBatchNo(BOOL fGetBatchNo)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
    //by host and merchant
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    strHDT.inHostIndex = shHostIndex;

    if(inCheckBatchEmtpyPerHost(strHDT.inHostIndex) > 0)
    {
		vduiClearBelow(2);
        vdDisplayErrorMsg(1, 8, "BATCH NOT EMPTY.");

        return;	
    }
	
    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("BATCH NO.");

	CTOS_LCDTPrintXY(1, 3, "BATCH NO.");

	ulTraceNo=wub_bcd_2_long(strMMT[0].szBatchNo,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%06ld", ulTraceNo);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fGetBatchNo == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
                ulTraceNo=atoi(strOut);
				memset(szStr, 0, sizeof(szStr));
				sprintf(szStr, "%06ld", ulTraceNo);
				wub_str_2_hex(szStr,strMMT[0].szBatchNo,6);
				inMMTSave(strMMT[0].MMTid);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }
	}
}

void vdModifyPassword(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "CHANGE PASSWORD";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inRet;
	BYTE strOut[30];
	USHORT usLen=0;
    USHORT shMaxLen, shMinLen;
	
    memset(szHostMenu, 0, sizeof(szHostMenu));
	
    strcpy((char *)szHostMenu, (char *)"SUPER PASSWORD \n"); 
	strcat((char *)szHostMenu, (char *)"SYSTEM PASSOWRD\n");
	strcat((char *)szHostMenu, (char *)"ENGINEER PASSWORD\n");
	strcat((char *)szHostMenu, (char *)"MERCHANT PASSWORD");
	
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		UCHAR szPassword[12+1];
		
        if(d_KBD_CANCEL == key)
            return;

		CTOS_LCDTClearDisplay();
		vdDispTitleString("MODIFY PASSWORD");

        inTCTRead(1);

		memset(szPassword, 0, sizeof(szPassword));
        if(key == 1)
            strcpy(szPassword, strTCT.szSuperPW);
		else if(key == 2)
            strcpy(szPassword, strTCT.szSystemPW);
		else if(key == 3)
            strcpy(szPassword, strTCT.szEngineerPW);
		else if(key == 4)
            strcpy(szPassword, strTCT.szMerchantPW);

        while(1)
        {   
        	shMinLen = PWD_MIN_LEN;
		    shMaxLen = PWD_MAX_LEN;
		
			vduiClearBelow(8);
            CTOS_LCDTPrintXY(1, 7, "Old:");
            memset(strOut,0x00, sizeof(strOut));
            inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                return;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				if(memcmp(strOut, szPassword, strlen(szPassword)) == 0) 	
                    break;
				else
					vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
            }   
        }
		
        while(1)
        {   
        	shMinLen = PWD_MIN_LEN;
	     	shMaxLen = PWD_MAX_LEN;
			
			if ((strTCT.byTerminalType % 2) == 0)
				clearLine(V3_ERROR_LINE_ROW);
			else	
				clearLine(8);
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                return;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
		if(key == 1)
			strcpy(strTCT.szSuperPW, strOut);
		else if(key == 2)
			strcpy(strTCT.szSystemPW, strOut);
		else if(key == 3)
			strcpy(strTCT.szEngineerPW, strOut);
		else if(key == 4)
			strcpy(strTCT.szMerchantPW, strOut);
		inTCTSave(1);
                break;    
            }   
        }
		
    }
}

void vdSetDebugISO(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("ISO CONFIG");

	// PrintISO
	CTOS_LCDTPrintXY(1, 3, "PRINT ISO");
    inTCTRead(1);

	if(strTCT.fPrintISOMessage == TRUE)
	  CTOS_LCDTPrintXY(1, 4, "1");  	
	else
	  CTOS_LCDTPrintXY(1, 4, "0"); 	

    CTOS_LCDTPrintXY(1, 5, "0-DISABLE      1-ENABLE");     

    while(1)
    {   
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fPrintISOMessage=1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fPrintISOMessage=0;
                 }
                 inTCTSave(1);
				 break;
             }
             else
             {
			     vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
             }
        }   
        if(inRet == d_KBD_CANCEL)
            break;
    }

	// PrintISOOption
	CTOS_LCDTClearDisplay();
	vdDispTitleString("ISO CONFIG");
	
	CTOS_LCDTPrintXY(1, 3, "ISO OPTION");
    inTCTRead(1);	

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.inPrintISOOption);
	CTOS_LCDTPrintXY(1, 4, szStr);

    while(1)
    {   
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
            strTCT.inPrintISOOption=atoi(strOut);
			inTCTSave(1);
			break;
        }   
        if(inRet == d_KBD_CANCEL)
            break;
    }

	//ISOLOG
	CTOS_LCDTClearDisplay();
	vdDispTitleString("ISO CONFIG");
	
	CTOS_LCDTPrintXY(1, 3, "ISO LOG");
    inTCTRead(1);	

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", get_env_int("ISOLOG"));
	CTOS_LCDTPrintXY(1, 4, szStr);

	while(1)
    {   
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
			put_env_int("ISOLOG", atoi(strOut));
			break;
        }   
        if(inRet == d_KBD_CANCEL)
            break;
    }
	
}

void vdShareCom(BOOL fGetShareCom)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
    ULONG ulTraceNo=0L;
    BYTE strOut[30];
    USHORT usLen=0;
    
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("SHARE COM");
    
    CTOS_LCDTPrintXY(1, 3, "SHARE COM");
    
    inTCTRead(1);
    
    if(strTCT.fShareComEnable== TRUE)
        CTOS_LCDTPrintXY(1, 4, "1");		
    else
        CTOS_LCDTPrintXY(1, 4, "0");	
    
    CTOS_LCDTPrintXY(1, 5, "0-DISABLE     1-ENABLE");	   
    
    while(1)
    {	
        if (strTCT.fShareComEnable == 2)
            clearLine(V3_ERROR_LINE_ROW);
        else	
            clearLine(8);
		
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                if(strOut[0] == 0x31)
                {
                    strTCT.fShareComEnable=1;
                }
                if(strOut[0] == 0x30)
                {
                    strTCT.fShareComEnable=0;
                }
                inTCTSave(1);
                break;
            }
            else
            {
                vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
            }
        }	
		
        if(inRet == d_KBD_CANCEL)
            break;
    }
}
//fix for Function Keys accepting 3 passwords Note: Occurs on all Function Keys
int inFunctionKeyPassword(unsigned char *ptrTitle, int inPasswordLevel)
{
    //char szPassword[6+1]={0}; 
	BYTE strOut[6+1]={0};
	int inRet;
	USHORT shMaxLen, shMinLen;
	
    inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString(ptrTitle);

    while(1)
    {	
    	shMinLen = PWD_MIN_LEN;
		shMaxLen = PWD_MAX_LEN;
		
        if ((strTCT.byTerminalType % 2) == 0)
            clearLine(V3_ERROR_LINE_ROW);
        else	
            clearLine(8);
		
        CTOS_LCDTPrintXY(1, 7, "PASSWORD:");
        memset(strOut,0x00, sizeof(strOut));
        inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            return d_NO;
        else if(0 == inRet )
            return d_NO;
        else if(inRet == 0xFF)
        {
			vdDisplayErrorMsg(1, 8, "TIME OUT");
            return d_NO;	
        }
        else if(inRet>=1)
        {
           if(inPasswordLevel == SUPER_PW)
           {
               if(strcmp(strOut,strTCT.szSuperPW) == 0)
                   return d_OK;					
           }
           else if(inPasswordLevel == ENGINEER_PW)
           {
               if(strcmp(strOut,strTCT.szEngineerPW) == 0)
                   return d_OK;					
           }
           else if(inPasswordLevel == SUPER_ENGINEER_PW)
           {
               if((strcmp(strOut,strTCT.szEngineerPW) == 0) || (strcmp(strOut,strTCT.szSuperPW) == 0))
                   return d_OK;					
           }			
           vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
		   clearLine(8);
       }	
    }
}

void vdSetConnectiontype(void)
{
	int inResult;
	short shHostIndex;
    BYTE strOut[21+1], strtemp[21+1];
    USHORT usLen=1;
	BOOL BolDetachLANChange=FALSE,BolDetachDIALChangeSetting=FALSE,BolDetachGPRSChangeSetting=FALSE;
	int inCPTNumRecs, i;
	int inCommMode = -1;
	USHORT ret = 0;
	int inIPHeader = -1;
	BOOL fValid = TRUE;

	// Set communication mode to all host -- sidumili
	/*
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
                
    inResult = inCPTRead(shHostIndex);
    if(inResult != d_OK)
        return;
	*/

	// Set current communication mode -- sidumili
	CTOS_LCDTClearDisplay();
    vdDispTitleString("SELECT COMMS");
	
	inCPTRead(MCC_HOST);
	setLCDPrint(2, DISPLAY_POSITION_LEFT, "CURRENT COMMS:");
	switch(strCPT.inCommunicationMode)
	{
		case 0:
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "DIALUP");
			break;
		case 1:
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "LAN");
			break;
		case 2:
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS");
			break;
		case 4:
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "WIFI");
			break;
	}

	if (isCheckTerminalMP200() == d_OK)
	{
		CTOS_LCDTPrintXY(1, 5, "1-GPRS");
		CTOS_LCDTPrintXY(1, 6, "2-WIFI");
	}
	else
	{
		CTOS_LCDTPrintXY(1, 5, "1-DIAL-UP    2-LAN");
		CTOS_LCDTPrintXY(1, 6, "3-GPRS          4-WIFI");
	}
	
	CTOS_LCDTPrintXY(1, 7, "New:");

	do
	{
		fValid = TRUE;
		CTOS_LCDTPrintXY(1, 8, "                    ");
		memset(strOut,0x00, sizeof(strOut));
		ret = shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);

		if (ret == d_KBD_CANCEL || ret == 0)
		{
			return;
		}
		else
		{
			if (isCheckTerminalMP200() == d_OK)
			{
				switch(strOut[0])
				{
					case 0x31:
						inCommMode = GPRS_MODE;
					break;
					case 0x32:
						inCommMode = WIFI_MODE;
					break;
					default:
						fValid = FALSE;
					break;
				}
			}
			else
			{
				switch(strOut[0])
				{
					case 0x31:
						inCommMode = DIAL_UP_MODE;
					break;
					case 0x32:
						inCommMode = ETHERNET_MODE;
					break;
					case 0x33:
						inCommMode = GPRS_MODE;
					break;
					case 0x34:
						inCommMode = WIFI_MODE;
					break;
					default:
						fValid = FALSE;
					break;
				}			
			}
		}

		// Check for valid input - sidumili
		if (!fValid) 
			vdDisplayErrorMsg(1, 8, "INVALID INPUT");
		else
			break; // exit while loop
		
	}
	
	while (1);

	vdDebug_LogPrintf("strOut[0]=[%d] . inCommMode=[%d]", strOut[0], inCommMode);
	
	// Check hardware
	inResult = inCheckPromptHW(inCommMode);
	if(inResult != d_OK)
	return;
	
	vdDebug_LogPrintf("JEFF::COMMS [%d]", inCommMode);

	//inCPTNumRecs = inCPTNumRecord();
	//vdDebug_LogPrintf("JEFF::Comm Count [%d]", inCPTNumRecs);
	
	vduiClearBelow(2);
	CTOS_LCDTPrintXY(1, 7, "Configuring Comms");
	CTOS_LCDTPrintXY(1, 8, "Please wait...");	

		/*
		if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE))
			inIPHeader = 1;
		else
			inIPHeader = 0;
		*/
		
		strCPT.inCommunicationMode = inCommMode;
		//strCPT.inIPHeader = inIPHeader;
			
		inCPT_Update(inCommMode, inIPHeader);
		
	inTCTRead(1);
	
	if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE) /*|| (inCommMode == DIAL_UP_MODE)*/)
		strTCT.fShareComEnable = 1;
	else
		strTCT.fShareComEnable = 0;

	strTCT.inTMSComMode = inCommMode;
	
	inTCTSave(1);

	#if 0
	/*MCC: Update fDHCPEnable for GPRS/WIFI*/
	inTCPRead(1);
	if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE))
		strTCP.fDHCPEnable = IPCONFIG_DHCP;
	else	
		strTCP.fDHCPEnable = IPCONFIG_STATIC;

	inTCPSave(1);
	/*MCC: Update fDHCPEnable for GPRS/WIFI*/
	#endif

	// Update inCommMode for ERM -- sidumili
	if (strTCT.byERMMode != 0)
	{
		inERMTCT_Update(inCommMode, 1);
	}
		
    if((BolDetachLANChange==TRUE) || (BolDetachDIALChangeSetting==TRUE) || (BolDetachGPRSChangeSetting==TRUE))
    {
        srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
        vduiClearBelow(3);
        CTOS_LCDTPrintXY (1,7, "Please Wait 	"); 	  
        if(strCPT.inCommunicationMode == ETHERNET_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init LAN... 	");
        else if(strCPT.inCommunicationMode == GPRS_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init GPRS...	 ");
		else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init Modem...	 ");
		else if(strCPT.inCommunicationMode == WIFI_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init WIFI...	 ");
		
        if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
        {        
            vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
            return;
        }
        
        srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
        srCommFuncPoint.inGetCommConfig(&srTransRec);
        srCommFuncPoint.inSetCommConfig(&srTransRec);		
    }

    if(inCommMode == WIFI_MODE)
        inCTOSS_COMMWIFISCAN();

	vduiClearBelow(2);
	CTOS_LCDTPrintXY(1, 7, "Configuring Comms");
	CTOS_LCDTPrintXY(1, 8, "Done!            ");
	CTOS_Beep();
    CTOS_Delay(500);
	
}		   
//fix for Function Keys accepting 3 passwords Note: Occurs on all Function Keys
void vdCTOS_FunctionKeyEx(void)
{
    int ret;
    BYTE strOut[2], szErrMsg[21+1];
    USHORT usLen=2;
	int inRet;
	
    CTOS_LCDTClearDisplay();
    vdDispTitleString("FUNCTION MENU");
    
    CTOS_LCDTPrintXY(1, 7, "FUNCTION?");
	
    memset(strOut, 0, sizeof(strOut));
    ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);

	vdDebug_LogPrintf("vdCTOS_FunctionKey | Key[%d]", atoi(strOut));
	
    if(ret==d_KBD_ENTER)
    {
		switch(atoi(strOut))
		{ 				
            case 0: /*Print terminal config*/

			// Check if erm function available -- sidumili
			if (fERM_ChkFuncAvailable(TRUE) == FALSE)
				return;
			
                vdCTOS_PrintEMVTerminalConfig();				
            break;
            
            case 1: /*Print terminal/host info*/
                if(isCheckTerminalMP200() == d_OK)
                {    
                	 vdPutEnvCRC();
                     vdDisplayHostInforMP200();
                     break;
                }
				
                vdCTOS_HostInfo();				
            break;

            case 2:// app switch module 06052015 - MCC app switch to new host. from 40 to 2 as requested by MCC.
                //if(inFunctionKeyPassword("SWITCH APP", SUPER_ENGINEER_PW) != d_OK)
                    //return;		
				//vdCTOSS_ChangeDefaultApplication();
				vdCTOSS_GetMemoryStatus("MEMORY");
            break;		

			case 3:
			if(isCheckTerminalMP200() == d_OK)
                {               	 
                     vdDisplayIPInforMP200();
                     break;
                }
				
             vdCTOS_PrintIP();	
				
			break;
			
            case 4:
                vdDisplayHostDetails();				
            break;
            
            case 5:
				inCTOS_BATCH_TOTAL_Process();
            break;
            
            case 6: /*get and display trace no.*/
                vdTraceNo(FALSE);				
            break;
            
            case 7: /*set trace no.*/
                if(inFunctionKeyPassword("TRACE NO.", ENGINEER_PW) != d_OK)
                    return;					
                vdTraceNo(TRUE);				
            break;
            
            case 8: /*get and display batch no.*/
                if(inFunctionKeyPassword("BATCH NO.", ENGINEER_PW) != d_OK)
                    return;					
                vdBatchNo(TRUE);
            break;
            
            case 9: /*set batch no.*/
                if(inFunctionKeyPassword("CONNECTION TYPE", ENGINEER_PW) != d_OK)
                    return;					
				vdSetConnectiontype();
            break;
            
            case 10:
				if(inFunctionKeyPassword("EDIT EMV PARAM", ENGINEER_PW) != d_OK)
                    return;
				vdCTOS_EditEnvParamDB();
            break;

			case 11:
				if(inFunctionKeyPassword("SET GSM AUTH TYPE", ENGINEER_PW) != d_OK)
                    return;
//				vdCTOS_AuthenticationType();
			break;
			

			case 12:
				
                // Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(FALSE) == FALSE)
					return;
				
                if(inFunctionKeyPassword("CLEAR ERM BATCH", ENGINEER_PW) != d_OK)
                    return;

				// Check if ERM Batch Empty -- sidumili
				if (fERM_ChkBatchEmpty(TRUE) == TRUE)
					return;
				
                vdClearERMBatch(TRUE);
			break;
			
            case 13:
				
                // Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(FALSE) == FALSE)
					return;

                //if(inFunctionKeyPassword("ERM INIT", ENGINEER_PW) != d_OK)
                //    return;	
				
                inCTOSS_ERMInitAllHost();
			break;

			case 14:
				
                // Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(FALSE) == FALSE)
					return;
                
                //if(inFunctionKeyPassword("ERM DE-INIT", ENGINEER_PW) != d_OK)
                //     return;
                
                inCTOS_ERMAllHosts_DeInitialization();
            break;

			case 15:
				
                // Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(FALSE) == FALSE)
					return;
                
                if(inFunctionKeyPassword("UPLOAD ERM RECEIPT", SUPER_PW) != d_OK)
                    return;

				// Check if ERM Batch Empty -- sidumili
				if (fERM_ChkBatchEmpty(TRUE) == TRUE)
					return;
				
				if (usCTOSS_Erm_GetReceiptTotal() > 0)
				{
	                inRet = inCTOSS_UploadReceipt(FALSE, TRUE);

					if (inRet == d_OK && get_env_int("FAILEDERMTRXN") == 2)
					{
						put_env_int("FAILEDERMTRXN", 0);
						vdSetFSRMode(0);
					}
				}
				
				else
					vdDisplayMultiLineMsgAlign("", "NO PENDING", "ERM RECEIPT", DISPLAY_POSITION_CENTER);
			break;	

			case 16:
                if(inFunctionKeyPassword("SHARE COM", ENGINEER_PW) != d_OK)
                    return;					
                vdShareCom(TRUE);
            break;				

			case 17:
				if(inFunctionKeyPassword("SET ERM NOTIFICATION", ENGINEER_PW) != d_OK)
	              return;	
				
				vdCTOS_ERMNotification();
			break;
								
            case 18:
                vdSTANNo(FALSE);				
            break;
            
            case 19:
                if(inFunctionKeyPassword("STAN NO.", ENGINEER_PW) != d_OK)
                    return;									
                vdSTANNo(TRUE);  				
            break;

			case 20:
				if(inFunctionKeyPassword("DIALUP PABX", ENGINEER_PW) != d_OK)
                    return;	
				vdSetPabx();
			break;

			case 21:
				if(inFunctionKeyPassword("CTMS DOWNLOAD", SUPER_PW) != d_OK)
				    return;

				inCTOSS_TMSDownloadRequest();				
			break;

			case 22:
                if(inFunctionKeyPassword("DEV MENU", ENGINEER_PW) != d_OK)
                    return;									
				vdCTOS_DevFunctionKey();
			break;

			case 23:
				vdDebug_LogPrintf("1. inCTOSS_COMMWIFISCAN");
                if(strCPT.inCommunicationMode == WIFI_MODE)
                {
					vdDebug_LogPrintf("2. inCTOSS_COMMWIFISCAN");
                    inCTOSS_COMMWIFISCAN();		
                }
            break;	        			
			
            case 50:
                if(inFunctionKeyPassword("BATCH REVIEW", SUPER_ENGINEER_PW) != d_OK)
                    return;					
                inCTOS_BATCH_REVIEW_Process();			  	
            break;

            case 51:								
	            //if(inFunctionKeyPassword("FSR ERM INIT", ENGINEER_PW) != d_OK)
	                //return;	
					
	            //inCTOSS_FSR_ERMInitAllHost();
	            vdCITASServices();
			break;
#if 0
			case 52:				
                if(inFunctionKeyPassword("FSR ERM DE-INIT", ENGINEER_PW) != d_OK)
                     return;
                inCTOS_FSR_ERMAllHosts_DeInitialization();
            break;
#endif
			case 55:
				if(inFunctionKeyPassword("SELECT TELCO", ENGINEER_PW) != d_OK)
					return;

				inCTOS_SelectTelco();
				break;
            
            case 71: /*Reprint any transaction receipt*/
				
                // Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(TRUE) == FALSE)
					return;
				
                if(inFunctionKeyPassword("REPRINT ANY", SUPER_ENGINEER_PW) != d_OK)
                    return;					
                inCTOS_REPRINT_ANY();				
            break;
            
            case 72: /*Reprint last transaction receipt*/	

				// Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(TRUE) == FALSE)
					return;
			
                if(inFunctionKeyPassword("REPRINT LAST", SUPER_ENGINEER_PW) != d_OK)
                    return;					
                inCTOS_REPRINT_LAST();				
            break;
            
            case 73: /*Reprint last settement receipt*/

				// Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(TRUE) == FALSE)
					return;
			
                if(inFunctionKeyPassword("REPRINT LAST SETTLE", SUPER_ENGINEER_PW) != d_OK)
                    return;					
                inCTOS_REPRINTF_LAST_SETTLEMENT();				
            break;

	 		case 74: /*Reprint last fsr receipt*/	
	 						
                //inPrintLastFSRReport();				
            break;
            
            case 75: /*Print Detail Report*/

				// Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(TRUE) == FALSE)
					return;
			
			    inCTOS_PRINTF_DETAIL();	
            break;
            
            case 76: /*Print Summary Report*/

				// Check if erm function available -- sidumili
				if (fERM_ChkFuncAvailable(TRUE) == FALSE)
					return;
			
                inCTOS_PRINTF_SUMMARY();				
            break;
            
            case 81: /*CRC Report*/                
                if(inFunctionKeyPassword("CRC REPORT", ENGINEER_PW) != d_OK)
                    return;	

				if(isCheckTerminalMP200() == d_OK)
                {      
                	 //vdPutEnvCRC();
                	 vdDisplayCRCForMP200();
                     break;
                }	
				
                vdCTOS_PrintCRC();
				
            break;
            
            case 90:
                vdModifyPassword();				
            break;
            
            case 91:				
            break;
            
            case 93:
                if(inFunctionKeyPassword("ISO CONFIG", ENGINEER_PW) != d_OK)
                    return;					
                vdSetDebugISO();				
            break;
            
            case 97: /*Clear Reversal*/
                if(inFunctionKeyPassword("CLEAR REVERSAL", ENGINEER_PW) != d_OK)
                    return;					
                vdCTOS_DeleteReversal();				
            break;
            
            case 98: /*Delete Batch*/
                if(inFunctionKeyPassword("DELETE BATCH", ENGINEER_PW) != d_OK)
                    return;					
                vdCTOS_DeleteBatch(); 				
            break;
            
            case 99: /*restart terminal*/
//                vdCTOS_uiRestart();				
            break;	

			default:
			break;	
		}

		inCLearTablesStructure();
		
		memset(szErrMsg,0x00,sizeof(szErrMsg));
		if (inGetErrorMessage(szErrMsg) > 0)
		{
			vdDisplayErrorMsg(1, 8, szErrMsg);
			vdSetErrorMessage("");
		}    
    }
}

void vdTerminalType(BOOL fTerminalType)
{
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulValue=0L;
	BYTE strOut[30];
	USHORT usLen=0;

	// TERMINAL TYPE
	inTCTRead(1);

	CTOS_LCDTClearDisplay();
	vdDispTitleString("TERMINAL TYPE/MODEL");

	CTOS_LCDTPrintXY(1, 3, "TERMINAL TYPE");

	//ulValue=wub_bcd_2_long(strTCT.byTerminalType,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.byTerminalType);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fTerminalType == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				strTCT.byTerminalType=(unsigned char)strOut[0] & 0x0F;
				inTCTMenuSave(1);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}

	// TERMINAL MODEL
	CTOS_LCDTClearDisplay();
	vdDispTitleString("TERMINAL TYPE/MODEL");

	CTOS_LCDTPrintXY(1, 3, "TERMINAL MODEL");

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.byTerminalModel);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fTerminalType == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				strTCT.byTerminalModel=(unsigned char)strOut[0] & 0x0F;

				if (strTCT.byTerminalModel == 1 || strTCT.byTerminalModel == 2) // Desktop(1), Mobile(2)
				{
					inTCTMenuSave(1);
					break;
				}
				else
				{
					vdDisplayErrorMsg(1, 8, "INVALID INPUT");
				}				                
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdHostEnable(int inHostIndex, BOOL fEnable)
{
	inHDTRead(inHostIndex);
	strHDT.fHostEnable=fEnable;
	inHDTSave(inHostIndex);
}

void vdSelectPhase(void)
{
    //BYTE key;
    int inRet;
    char szDebug[40 + 1]={0};
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0; 
        
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    strcpy(szHeaderString, "SELECT PHASE");
    strcat(szChoiceMsg,"FLEET \n");
    strcat(szChoiceMsg,"FLEET/DINERS");
    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
    if (key > 0)
        {
        if (key == 1)
        {
            vdHostEnable(1, TRUE);
            vdHostEnable(2, TRUE);
            vdHostEnable(3, TRUE);
            vdHostEnable(4, TRUE);
            vdHostEnable(5, FALSE);
            vdHostEnable(6, FALSE);
            vdHostEnable(7, FALSE);
            vdHostEnable(8, FALSE);
            vdHostEnable(9, FALSE);
            vdHostEnable(10, FALSE);
            vdHostEnable(11, FALSE);
            vdHostEnable(12, TRUE);
            vdHostEnable(13, FALSE);	
            vdHostEnable(14, FALSE);	
            vdHostEnable(15, FALSE);	
            vdHostEnable(16, FALSE);	
            vdHostEnable(17, FALSE);	
            vdHostEnable(18, FALSE);	
            
            inTCTRead(1);       
            strTCT.inMenuid=2;
            inTCTMenuSave(1);
        }
        else if (key == 2)
        {
            //vdHostEnable(1, TRUE);
            //vdHostEnable(2, TRUE);
            //vdHostEnable(3, TRUE);
            //vdHostEnable(4, TRUE);
            //vdHostEnable(5, FALSE);
            vdHostEnable(6, TRUE);
            vdHostEnable(7, TRUE);
            vdHostEnable(8, TRUE);
            vdHostEnable(9, TRUE);
            vdHostEnable(10, TRUE);
            vdHostEnable(11, TRUE);
            vdHostEnable(12, TRUE);
            //vdHostEnable(13, FALSE);	
            vdHostEnable(14, TRUE);	
            vdHostEnable(15, TRUE);	
            vdHostEnable(16, TRUE);	
            vdHostEnable(17, TRUE);	
            vdHostEnable(18, TRUE);	
            
            inTCTRead(1);		
            strTCT.inMenuid=3;
            inTCTMenuSave(1);
        }
        
        if (key == d_KBD_CANCEL)
        {
            return;
        }        
        return;
    }

return;
}


void vdMenuID(BOOL fTerminalType)
{
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulValue=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("MENU ID");

	CTOS_LCDTPrintXY(1, 3, "MENU ID");

	//ulValue=wub_bcd_2_long(strTCT.byTerminalType,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.inMenuid);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fTerminalType == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 2, 2, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
            			
				strTCT.inMenuid=atoi(strOut);

				#if 0
				if(strTCT.inMenuid != strTCT.inMenuidNewHost || strTCT.inMenuid != strTCT.inMenuidNHProfileMenu)
				{
						CTOS_LCDTClearDisplay();
						CTOS_LCDTPrintXY(1, 5, "INVALID INPUT");
						CTOS_LCDTPrintXY(1, 6, "PLS ENTER A");
						CTOS_LCDTPrintXY(1, 7, "VALID MENU ID.");
    						CTOS_Beep();
    						CTOS_Delay(2000);
				
						break;
				}
				#endif
				
				strTCT.fRegister=0; // #00065 - Terminal able to process sale transaction on idle menu when MENU ID 1 (registration) is activated
				inTCTMenuSave(1);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

//inMenuidNHProfileMenu
void vdMenuidNHProfileMenu(BOOL fTerminalType)
{
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulValue=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("MENU ID NH");

	CTOS_LCDTPrintXY(1, 3, "MENU ID NH");

	//ulValue=wub_bcd_2_long(strTCT.byTerminalType,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.inMenuidNHProfileMenu);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fTerminalType == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 2, 2, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
            			
				strTCT.inMenuidNHProfileMenu=atoi(strOut);

				#if 0
				if(strTCT.inMenuid != strTCT.inMenuidNewHost || strTCT.inMenuid != strTCT.inMenuidNHProfileMenu)
				{
						CTOS_LCDTClearDisplay();
						CTOS_LCDTPrintXY(1, 5, "INVALID INPUT");
						CTOS_LCDTPrintXY(1, 6, "PLS ENTER A");
						CTOS_LCDTPrintXY(1, 7, "VALID MENU ID.");
    						CTOS_Beep();
    						CTOS_Delay(2000);
				
						break;
				}
				#endif
				
				strTCT.fRegister=0; // #00065 - Terminal able to process sale transaction on idle menu when MENU ID 1 (registration) is activated
				inTCTMenuSave(1);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdCTOS_DevFunctionKey(void)
{
    int ret;
    BYTE strOut[2], szErrMsg[21+1];
    USHORT usLen=2;
	
    CTOS_LCDTClearDisplay();
    vdDispTitleString("DEV MENU");
    
    CTOS_LCDTPrintXY(1, 7, "FUNCTION?");
	
    memset(strOut, 0, sizeof(strOut));
    ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
    if(ret==d_KBD_ENTER)
    {
		switch(atoi(strOut))
		{
            case 1: /*set trace no.*/
                if(inFunctionKeyPassword("DOWNLOAD", ENGINEER_PW) != d_OK)
                    return; 												
                vdCTOSS_DownloadMode();
            break;
			
			case 2:
                if(inFunctionKeyPassword("TERMINAL TYPE/MODEL", ENGINEER_PW) != d_OK)
                    return;													
				vdTerminalType(TRUE);
			break;

            case 3:
                if(inFunctionKeyPassword("MENU ID/NH", ENGINEER_PW) != d_OK)
                    return;													
                vdMenuID(TRUE);
				vdMenuidNHProfileMenu(TRUE);
            break;

			case 4:
                if(inFunctionKeyPassword("MENU ID NH", ENGINEER_PW) != d_OK)
                    return;													
				vdMenuidNHProfileMenu(TRUE);
			break;	

            case 5:
                if(inFunctionKeyPassword("CONF CTLS", ENGINEER_PW) != d_OK)
                    return;													
	            vdCTOS_ManualConfigureCTLSReader();			
			break;

			case 6:
				if(inFunctionKeyPassword("PING IP ADDRESS", ENGINEER_PW) != d_OK)
                    return;		
				vdCTOS_PingIPAddress();
				
			break;	
			
			case 7:
				if(inFunctionKeyPassword("SET ERM MODE", ENGINEER_PW) != d_OK)
					return;		
				vdSetERMMode();
				
			break;
			
			case 8:
				if(inFunctionKeyPassword("DEBUG MODE", ENGINEER_PW) != d_OK)
				   return; 
				vdCTOS_Debugmode();
			break;
			
			case 9:
				if(inFunctionKeyPassword("EDIT DATABASE", ENGINEER_PW) != d_OK)
				   return;
				vdCTOSS_EditTable();
			break;

            case 10:
				inCTOSS_TMSDownloadRequest();
			break;

			case 11:
			if(inFunctionKeyPassword("SET CARD", ENGINEER_PW) != d_OK)
				return;		
			inSetCardForInstallation();
				
			break;
			
			default:
			break;	
		}
		
		memset(szErrMsg,0x00,sizeof(szErrMsg));
		if (inGetErrorMessage(szErrMsg) > 0)
		{
			vdDisplayErrorMsg(1, 8, szErrMsg);
			vdSetErrorMessage("");
		}    
    }
}

int inCTOS_CheckHardware(int inHW)
{
	USHORT usRtn=0;
	BOOL fPortable;
	BOOL fPCI;
	USHORT mkHWSupport;
	BYTE baBuf[17+1];

	vdDebug_LogPrintf("--inCTOS_CheckHardware--");
	
	usRtn = CTOS_HWSettingGet(&fPortable,&fPCI,&mkHWSupport);		
	
	if (usRtn != d_OK)
	{
	   return d_NO;
	}
	
	vdDebug_LogPrintf("mkHWSupport: (%04x)", mkHWSupport); 
	
	usRtn = CTOS_CradleAttached();
	vdDebug_LogPrintf(". CTOS_CradleAttached Ret=(%04x)",usRtn);
	if (usRtn == d_YES)
	{
		//BYTE baBuf[17];
		
		memset(baBuf,0x00,sizeof(baBuf));
		//ANDROID-REMOVED
		//usRtn = CTOS_GetSystemInfo(ID_CRADLE_MP,baBuf);
		//END
		//d_SYSTEM_INVALID_PARA  0201h
		//d_SYSTEM_HALT_FAULT  0202h
		//d_SYSTEM_SYS_PARA_ABSENT	0203h
		vdDebug_LogPrintf(". ID_CRADLE_MP shRet=[%04x],baBuf=(%s)",usRtn,baBuf);
		if (usRtn == d_OK)
		{
			mkHWSupport = mkHWSupport | d_MK_HW_MODEM;
			mkHWSupport = mkHWSupport | d_MK_HW_ETHERNET;
		}
		vdDebug_LogPrintf("mkHWSupport [%04x]", mkHWSupport);
	}				

	vdDebug_LogPrintf("usRtn[%x]::fPortable[%d]::fPCI[%d]::mkHWSupport[%04x]", usRtn, fPortable, fPCI, mkHWSupport);	
	vdDebug_LogPrintf("mkHWSupport & d_MK_HW_MODEM=[%04x]", mkHWSupport & d_MK_HW_MODEM);
	vdDebug_LogPrintf("mkHWSupport & d_MK_HW_ETHERNET=[%04x]", mkHWSupport & d_MK_HW_ETHERNET);
	vdDebug_LogPrintf("mkHWSupport & d_MK_HW_GPRS=[%04x]", mkHWSupport & d_MK_HW_GPRS);
	vdDebug_LogPrintf("mkHWSupport & d_MK_HW_WIFI=[%04x]", mkHWSupport & d_MK_HW_WIFI);
	
	if(inHW == 0) /*dialup*/
	{
		if ((mkHWSupport & d_MK_HW_MODEM) != d_MK_HW_MODEM)
			return d_ERR_MODEM;
	}
	else if(inHW == 1) /*lan*/
	{
		if ((mkHWSupport & d_MK_HW_ETHERNET) != d_MK_HW_ETHERNET)
			return d_ERR_ETHERNET;
	}
	else if(inHW == 2) /*gprs*/
	{
		if((mkHWSupport & d_MK_HW_GPRS) != d_MK_HW_GPRS)
			return d_ERR_GSM;
	}	
	else if(inHW == 4) /*wifi*/
	{
		if ((mkHWSupport & d_MK_HW_WIFI) != d_MK_HW_WIFI)
			return d_ERR_WIFI;
	}	

	return d_OK;
}

void vdCTOS_ManualConfigureCTLSReader(void)
{
	inTCTRead(1);
    vdCTOS_InitWaveData();
}

// Check / Prompt Hardware -- sidumili
int inCheckPromptHW(int inCommMode)
{
	int inResult = d_OK;
	
	inResult = inCTOS_CheckHardware(inCommMode);
	
	if(inResult != d_OK)
	{
	    if(inResult == d_ERR_MODEM)
	        vdDisplayErrorMsg(1, 8, "DIAL-UP NOT FOUND");	
	    else if(inResult == d_ERR_ETHERNET)
	        vdDisplayErrorMsg(1, 8, "LAN NOT FOUND");
	    else if(inResult == d_ERR_GSM) /*gprs*/
	        vdDisplayErrorMsg(1, 8, "GPRS NOT FOUND");
	    else if(inResult == d_ERR_WIFI)
	        vdDisplayErrorMsg(1, 8, "WIFI NOT FOUND");
	    else
	        vdDisplayErrorMsg(1, 8, "HARDWARE NOT FOUND");
	}

	return(inResult);
	
}

void vdSetERMMode(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("SET ERM MODE");

    inTCTRead(1);

	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr, "ERM MODE: %d", strTCT.byERMMode);
    CTOS_LCDTPrintXY(1, 5, szStr);     

    while(1)
    {   
        vduiClearBelow(8);
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33 || strOut[0]==0x34)
            {
                 strTCT.byERMMode = atoi(strOut);
				 
                 inTCTSave(1);
				 
				 break;
             }
             else
             {
			     vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
             }
        }   
        if(inRet == d_KBD_CANCEL)
            break;
    }
}

void vdSetPabx(void)
{
	BYTE bRet,strOut[30],strtemp[17],key;
    int inResult;
    USHORT ret;
    USHORT usLen;
	
	inResult = inTCTRead(1);
    if (inResult != d_OK)
        return;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("DIALUP PABX");
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRIMARY PABX");
        if (wub_strlen(strTCT.szPabx)<=0)
            setLCDPrint(4,DISPLAY_POSITION_LEFT, "[DISABLED]");
        else
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szPabx);

        
        strcpy(strtemp,"New:") ; 
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        memset(strtemp, 0x00, sizeof(strtemp));
        usLen = 10;
        ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
                if(strlen(strOut)>0)
                {
                    memcpy(strTCT.szPabx,strOut,strlen(strOut));
                    strTCT.szPabx[strlen(strOut)]=',';
                    strTCT.szPabx[strlen(strOut)+1]=0;
                    break;
                }
                else
                {
                    vduiClearBelow(4);
                    vduiDisplayStringCenter(5,"NO PABX VALUE");
                    vduiDisplayStringCenter(6,"ENTERED,DISABLE");
                    vduiDisplayStringCenter(7,"PABX?");
                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                    key=struiGetchWithTimeOut();
                    if(key==d_KBD_ENTER)
                    {
                        memset(strTCT.szPabx,0,sizeof(strTCT.szPabx));
                                                break;
                    }
                }
        }
            if(ret == d_KBD_CANCEL)
                break;
        }

        inTCTSave(1);
}

