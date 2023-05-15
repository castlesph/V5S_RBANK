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
#include <epad.h>
#include "..\Includes\POSSetting.h"

extern sqlite3 * db;
extern sqlite3_stmt *stmt;
extern int inStmtSeq;
extern BOOL fPrinterStart;
extern BOOL fERMTransaction; // Enhancement for printing speed
int inNotifyADEnable = 0;

BOOL fMerchantOriented = FALSE;
BOOL fERMUploadFailed = FALSE;
int inAnswer[26+1];
BOOL fEducationalCheckList = FALSE;

int gblinWitnessSign=0;
extern BOOL fTimeOutFlag;
BOOL fCaptureCopy = FALSE;

BYTE InputStringLetters(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    char szTemp[24+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[24+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	int x=0;
	int inLastKey=255;
	int inKey=0;
	int inKey2=0;
    int inMax=*usStrLen;
	
	char KeyPad[10][3]={{' ', '.', '-'},
	               		{'Q', 'Z', '0'},
	               		{'A', 'B', 'C'},
	   					{'D', 'E', 'F'},
	   					{'G', 'H', 'I'},
	   					{'J', 'K', 'L'},
	   					{'M', 'N', 'O'},
	   					{'P', 'R', 'S'},
	   					{'T', 'U', 'V'},
	   					{'W', 'X', 'Y'}};
	
	memset(szAmount, 0x00, sizeof(szAmount));
	
	CTOS_TimeOutSet(TIMER_ID_1, 100);
	
	while(1)
	{		
		memset(szTemp, 0x00, sizeof(szTemp));  
         
        clearLine(usY);
        //CTOS_LCDTPrintXY(40-(strlen(szAmount)*2), usY, szAmount);
		CTOS_LCDTPrintXY(1, usY, szAmount);
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
            if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                inLastKey=255;
			
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}

		if(inRet >= 48 && inRet <= 57)
		{
			char szTemp2[24+1];
			
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%c", inRet);
			inKey=atoi(szTemp);
			
			CTOS_TimeOutSet(TIMER_ID_1, 100);
			if((inCtr < inMax) || (inLastKey == inKey))
			{
                vdDebug_LogPrintf("1. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                if(inLastKey == inKey)
                {
					inKey2++;
					if(inKey2 > 3)
                       inKey2=0;
                }
                else
					inKey2=0;
				                
                if(inLastKey == inKey)
                    inCtr--;
                    
				szAmount[inCtr]=KeyPad[inKey][inKey2];
				inCtr++;

                vdDebug_LogPrintf("2. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                inLastKey=inKey;

                if(inKey == 0)
				    inLastKey=255;	
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_KBD_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{
			if(strlen(szAmount) > 0)
			{
				memcpy(pbaStr, szAmount, strlen(szAmount));
				*usStrLen=strlen(szAmount);
				return d_KBD_ENTER;
			}
			
		}
		else if(inRet == 82) /*clear key*/
		{		
			inCtr--;
			if(inCtr <= 0)
                inCtr=0;
            szAmount[inCtr]=0x00;
			inKey2=0;
			inLastKey=255;
		}
	}
}

#if 0
void vdCITASServices(void)
{
	int inRet;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "CITAS SERVICES";
    char szHostMenu[50]={"CTMS DOWNLOAD\nJOB TYPE"};
	srTransRec.byTransType = CITAS_SERVICES;

         
         inRet = inCTOS_GetTxnPassword();
         if(d_OK != inRet)
         return;

	CTOS_LCDTClearDisplay();


	//inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return FAIL;
        
		if(key == 1)
			inCTOSS_TMSDownloadRequest();		
		else if(key == 2)
			vdJobType();		
		/*else if(key == 3)
			vdDiagnostics();*/			

		return;
    }
	

}
#else
void vdCITASServices(void)
{
     int inRet;
     BYTE key=0;
     BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
     BYTE  x = 1;
     char szHeaderString[50] = "CITAS SERVICES";
     char szHostMenu[70+1]={"INSTALLATION\nPULL-OUT\nREPROGRAMMING\nNEGATIVE\nSERVICE\nPREVENTIVE"};
     char szTermSerialNum[15+1];
  	 int iFSR = get_env_int("FSRMODE");
	 
     srTransRec.byTransType = CITAS_SERVICES;

	 if (iFSR == 3)
	 {	 	
		if (inCheckBatcheNotEmtpy() > 0)
		 	vdDisplayMultiLineMsgAlign("PLEASE DO", "SETTLEMENT", "ORIENTATION", DISPLAY_POSITION_CENTER);
		else
			vdDisplayMultiLineMsgAlign("PLEASE DO", "ORIENTATION", "", DISPLAY_POSITION_CENTER);
		
		return;
	 }
	
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
    return;

	 
     memset(&strService,0x00,sizeof(SERVICE_INFO_TABLE));
     memset(&strHWDiag,0x00,sizeof(HW_DIAG_TABLE));
     memset(&strSWDiag,0x00,sizeof(SW_DIAG_TABLE));
     
     inTCTRead(1);
     strcpy(strService.szUnitModel,strTCT.szUnitModel);
     
     memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
     CTOS_GetFactorySN(szTermSerialNum); 	
    //usGetSerialNumber(szTermSerialNum);
     szTermSerialNum[15]=0;
     strcpy(strService.szUnitSN,szTermSerialNum);
     
     CTOS_LCDTClearDisplay();
     
     //inSetColorMenuMode();
     key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
     inSetTextMode();
     
     if (key == 0xFF) 
     {
          CTOS_LCDTClearDisplay();
          //vduiWarningSound();
          return;  
     }
     
     if(key > 0)
     {
          if (get_env_int("INVALIDKEY") == 1)
          return;

          if(key == 1)
               inCTOS_vdInstallation();		
          else if(key == 2)
               vdPullOut();		
          else if(key == 3)
               vdReprogramming();			
          else if(key == 4)
               vdNegative();
          else if(key == 5)
               vdService();
          else if(key == 6)
               vdPreventive_Maintenance();

		  vdSetErrorMessage("");	
          return;
     }

	 vdSetErrorMessage("");
}

#endif
int vdMerchantServices(void)
{
     int inRet;
     BOOL isKey;
     BYTE key=0;
     BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
     BYTE  x = 1;
     BYTE g_szPhoneNo[30];
     BYTE szCurrentTime[20];
     char szHeaderString[50] = "MERCHANT SERVICES";
     char szHostMenu[50]={"HARDWARE DIAG\nSOFTWARE DIAG"};
     unsigned char c;	
     CTOS_RTC SetRTC;
     char szTermSerialNum[15+1];
	 int iFSR = get_env_int("FSRMODE");	

     srTransRec.byTransType = MERCHANT_SERVICES;

	 if (iFSR == 3)
	 {
	 	if (inCheckBatcheNotEmtpy() > 0)
		 	vdDisplayMultiLineMsgAlign("PLEASE DO", "SETTLEMENT", "ORIENTATION", DISPLAY_POSITION_CENTER);
		else
			vdDisplayMultiLineMsgAlign("PLEASE DO", "ORIENTATION", "", DISPLAY_POSITION_CENTER);
		
		return d_NO;
	 }
	 
     inRet = inCTOS_GetTxnPassword();
     if(d_OK != inRet)
     return ST_ERROR;

     memset(&strService,0x00,sizeof(SERVICE_INFO_TABLE));
     memset(&strHWDiag,0x00,sizeof(HW_DIAG_TABLE));
     memset(&strSWDiag,0x00,sizeof(SW_DIAG_TABLE));
     
     inTCTRead(1);
     strcpy(strService.szUnitModel,strTCT.szUnitModel);
     
     memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
     CTOS_GetFactorySN(szTermSerialNum); 
     //usGetSerialNumber(szTermSerialNum);
     szTermSerialNum[15]=0;
     strcpy(strService.szUnitSN,szTermSerialNum);

    CTOS_LCDTClearDisplay();
     
     key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
     inSetTextMode();
     
     if (key == 0xFF) 
     {
          CTOS_LCDTClearDisplay();
          //vduiWarningSound();
          return d_NO;  
     }
     
     if(key > 0)
     {
          if (get_env_int("INVALIDKEY") == 1)
               return FAIL;		 

		 // Check ERM Connection
		 fERMUploadFailed = FALSE;
		 inRet = inCTOS_ERMPreConnect(TRUE);
		 if (inRet != d_OK)
			fERMUploadFailed = TRUE;
	
          switch(key)
          {
               case 1:
               vdHWDiagnostics();
               break;
               
               case 2:
               vdSWDiagnostics();
               
               vdDisplaySummary(SOFTWARE_TEST);
               
               //vdDisplaySWDetailReport();
               
               inInputReceiverName();
               
               memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
               inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
               strcpy(strService.szCustPhoneNo,g_szPhoneNo);
               
               ushCTOS_ePadSignatureEx();

               vdFSRPrintMCCHostInfo();
               
               srTransRec.byTransType = MERCHANT_SERVICES;
                             
			   // Capture merchant copy for upload -- sidumili
			   fCaptureCopy = TRUE;
			   ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, SOFTWARE_TEST);

			   // Print merchant copy
			   fCaptureCopy = FALSE;
			   ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, SOFTWARE_TEST);
					 
               inRet = inCTOSS_UploadReceipt(FALSE, FALSE);
               break;
               
               default:
               break;	
          }
     }	
     //WaitKey(10);
     return d_OK;

}




void vdJobType(void)
{
	int inRet;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "JOB TYPE";
    char szHostMenu[70+1]={"INSTALLATION\nPULL-OUT\nREPROGRAMMING\nNEGATIVE\nSERVICE\nPREVENTIVE"};
	char szTermSerialNum[15+1];

	memset(&strService,0x00,sizeof(SERVICE_INFO_TABLE));
	memset(&strHWDiag,0x00,sizeof(HW_DIAG_TABLE));
	memset(&strSWDiag,0x00,sizeof(SW_DIAG_TABLE));
	
	inTCTRead(1);
	strcpy(strService.szUnitModel,strTCT.szUnitModel);

	memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
    CTOS_GetFactorySN(szTermSerialNum); 	
    //usGetSerialNumber(szTermSerialNum);
    szTermSerialNum[15]=0;
    strcpy(strService.szUnitSN,szTermSerialNum);
	
	CTOS_LCDTClearDisplay();

	//inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return;
        
		if(key == 1)
			inCTOS_vdInstallation();		
		else if(key == 2)
			vdPullOut();		
		else if(key == 3)
			vdReprogramming();			
		else if(key == 4)
			vdNegative();
		else if(key == 5)
			vdService();
		else if(key == 6)
			vdPreventive_Maintenance();

		return;
    }
}

int vdService(void)
{
	int inRet;
	BOOL isKey;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
	BYTE g_szPhoneNo[30];
	BYTE szCurrentTime[20];
    char szHeaderString[50] = "SERVICE";
    char szHostMenu[50]={"HARDWARE DIAG\nSOFTWARE DIAG"};
	unsigned char c;	
	CTOS_RTC SetRTC;

	vdDebug_LogPrintf("--vdService--");
	
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return d_NO;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return FAIL;
		
		switch(key)
		{
			case 1:
				vdHWDiagnostics();
				break;
				
			case 2:
				vdSWDiagnostics();

				vdDisplaySummary(SOFTWARE_TEST);
					
				//vdDisplaySWDetailReport();
				
				inInputReceiverName();
				
				memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
				inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
				strcpy(strService.szCustPhoneNo,g_szPhoneNo);
				
				ushCTOS_ePadSignatureEx();
                                     vdDispTitleString("SOFTWARE DIAG");
				vdEnterTerminalDetails(SOFTWARE_TEST,"SOFTWARE DIAG");
				vdEnterFEDetails("SOFTWARE DIAG");
				vdConfirmFEDetails(SOFTWARE_TEST,"SOFTWARE DIAG");
				return d_OK;
#if 0
				vdDispTitleString("SOFTWARE DIAG");
				vduiClearBelow(2);
			    vduiDisplayStringCenter(3,"ENGINEER");
			    vduiDisplayStringCenter(4,"PRESENT?");

			    //CTOS_LCDTPrintXY(1,7," NO[X]  YES[OK] ");
			    vduiDisplayStringCenter(7,"NO[X]   YES[OK]");
			
			        
			    while(1)
			    {
			        while(1)
				    {
				        CTOS_KBDInKey(&isKey);
				        if (isKey){ //If isKey is TRUE, represent key be pressed //
				            vduiLightOn();
				            //Get a key from keyboard //
				            CTOS_KBDGet(&c);
							break;
				        }	        
				    }

					if (c==d_KBD_ENTER)
			        {
						vdEnterTerminalDetails(SOFTWARE_TEST,"SOFTWARE DIAG");
						vdEnterFEDetails("SOFTWARE DIAG");
						vdConfirmFEDetails(SOFTWARE_TEST,"SOFTWARE DIAG");
						return;
						
					}
			        else if (c==d_KBD_CANCEL)
			        {
						CTOS_RTCGet(&SetRTC);
						memset(szCurrentTime,0x00,20);
						sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
						wub_str_2_hex(szCurrentTime,strSWDiag.szSWEndTime,TIME_ASC_SIZE);
						vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

						vdFSRPrintMCCHostInfo();
						
			                            ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, SOFTWARE_TEST);
						inRet = inCTOSS_UploadReceipt(FALSE, FALSE);
						return;
			        }
			    }
			#endif	
				break;

			default:
			break;	
		}
    }	
		//WaitKey(10);
		return d_OK;
		
}

void vdSWDiagnostics(void)
{
	int inRet = -1;
	BOOL fCreditVoidSuccess = 0;
	BOOL fInstVoidSuccess = 0;
	BYTE szCurrentTime[20];
	CTOS_RTC SetRTC;
	
	vdCTOS_TransEndReset();
	put_env_int("BANCNETOK",0);
	//memset(&strSWDiag,0x00,sizeof(SW_DIAG_TABLE));

//Get SW Diagnostics Start time
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strSWDiag.szSWStartTime,TIME_ASC_SIZE);
	
	strSWDiag.inCredit_Status = FAIL;
	strSWDiag.inInst_Status = FAIL;
	strSWDiag.inBancnet_Status = FAIL;
#if 1	
	inRet = inCTOS_WAVE_SALE_DIAG();
	if(inRet == d_OK)
	{
		if(inTCTRead(1) == d_OK);
		{
			inCPTRead(19);
			vdDebug_LogPrintf("inCommunicationMode[%d]",strCPT.inCommunicationMode);
			if(strCPT.inCommunicationMode == WIFI_MODE)
				vdSetInit_Connect(FALSE);
//			if (inCTOS_InitComm(strCPT.inCommunicationMode) != d_OK) 
//		    {
//				vdDispErrMsg("COMM INIT ERR");
//		    }
//			else
//			{
				inRet = inCTOS_VOID_DIAG();
				if(inRet == d_OK)
					strSWDiag.inCredit_Status = SUCCESS;
				else
					strSWDiag.inCredit_Status = VOID_FAILED;
//			}
		}
	}
if(get_env_int("TEST_INSTALLMENT") ==1)
{
	inRet = inCTOS_INSTALLMENT_DIAG();
	if(inRet == d_OK)
	{
		if(inTCTRead(1) == d_OK)
		{
			inCPTRead(19);
			vdDebug_LogPrintf("inCommunicationMode[%d]",strCPT.inCommunicationMode);
			if(strCPT.inCommunicationMode == WIFI_MODE)
				vdSetInit_Connect(FALSE);
			
			inRet = inCTOS_VOID_DIAG();
			if(inRet == d_OK)
				strSWDiag.inInst_Status = SUCCESS;
			else
				strSWDiag.inInst_Status = VOID_FAILED;
		}
	}
}
#endif
if(get_env_int("TEST_BANCNET") ==1)
{
	inRet = inCTOS_BancnetSale_Diag();
	vdDebug_LogPrintf("inCTOS_BancnetSale_Diag[%d]",inRet);
	if(get_env_int("BANCNETOK") == TRUE)
		strSWDiag.inBancnet_Status = SUCCESS;
	
	if(strSWDiag.inCredit_Status == SUCCESS && strSWDiag.inInst_Status == SUCCESS && strSWDiag.inBancnet_Status == SUCCESS)
	{
		if(strCPT.inCommunicationMode == WIFI_MODE)
			vdSetInit_Connect(FALSE);

		inCTOS_SettleAllOperation_Diag();
	}	
}
//Get SW Diagnostics End time
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strSWDiag.szSWEndTime,TIME_ASC_SIZE);

}

void vdHWDiagnostics(void)
{
	int inRet;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "HARDWARE DIAG";
    char szHostMenu[50]={"AUTO\nMANUAL"};
	BOOL isKey;
	BYTE g_szPhoneNo[30];
	BYTE szCurrentTime[20];
	unsigned char c;	
	CTOS_RTC SetRTC;

	vdDebug_LogPrintf("--vdHWDiagnostics--");
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	
	CTOS_LCDTClearDisplay();

	//inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return;

		switch(key)
		{
			case 1:
				vdDebug_LogPrintf("--AUTO--");
				vdAutoHWDiagnostics("HARDWARE DIAG");
				vdDisplaySummary(HARDWARE_TEST);
				inInputReceiverName();
				
				memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
				inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
				strcpy(strService.szCustPhoneNo,g_szPhoneNo);
				
				ushCTOS_ePadSignatureEx();
                if (srTransRec.byTransType==CITAS_SERVICES)
				{
				    vdDispTitleString("HARDWARE DIAG");
				    vdEnterTerminalDetails(HARDWARE_TEST,"HARDWARE DIAG");
				    vdEnterFEDetails("HARDWARE DIAG");
				    vdConfirmFEDetails(HARDWARE_TEST,"HARDWARE DIAG");
				    return;
				    #if 0
				    vdDispTitleString("HARDWARE DIAG");
				    vduiClearBelow(2);
				    vduiDisplayStringCenter(3,"ENGINEER");
				    vduiDisplayStringCenter(4,"PRESENT?");
				    vduiDisplayStringCenter(7,"NO[X]   YES[OK]");
				    
				    while(1)
				    {
				         while(1)
				         {
				              CTOS_KBDInKey(&isKey);
				              if (isKey){ //If isKey is TRUE, represent key be pressed //
				              vduiLightOn();
				              //Get a key from keyboard //
				              CTOS_KBDGet(&c);
				              break;
				              }	        
				         }
				         
				         if (c==d_KBD_ENTER)
				         {
				              vdEnterTerminalDetails(HARDWARE_TEST,"HARDWARE DIAG");
				              vdEnterFEDetails("HARDWARE DIAG");
				              vdConfirmFEDetails(HARDWARE_TEST,"HARDWARE DIAG");
				              return;
				         }
				         else if (c==d_KBD_CANCEL)
				         {
				              CTOS_RTCGet(&SetRTC);
				              memset(szCurrentTime,0x00,20);
				              sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
				              wub_str_2_hex(szCurrentTime,strHWDiag.szHWEndTime,TIME_ASC_SIZE);
				              vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
				              
				              vdFSRPrintMCCHostInfo();
				              
				              ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, HARDWARE_TEST);
				              inRet = inCTOSS_UploadReceipt(FALSE,FALSE);
				              return;
				         }
				    }
				    #endif
				}
				else
                {
                     vdFSRPrintMCCHostInfo();

					 // Capture merchant copy for upload -- sidumili
					 fCaptureCopy = TRUE;
                     ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, HARDWARE_TEST);

					 // Print merchant copy
					 fCaptureCopy = FALSE;
                     ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, HARDWARE_TEST);
					 
                     inRet = inCTOSS_UploadReceipt(FALSE,FALSE);
                     return;
                }
				break;
			case 2:
				vdDebug_LogPrintf("--MANUAL--");
				vdManualHWDiagnostics();	
				break;

			default:
			break;	
		}
		
		return;
    }
}
void vdManualHWDiagnostics(void)	
{
	int inRet;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "DIAGNOSTICS";
    char szHostMenu[50]={"LCD\nPRINTER\nCOMMS\nCLTS\nMAGSTRIPE\nICC"};
	
	CTOS_LCDTClearDisplay();

	//inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return;

		switch(key)
		{
			case 1:
				vdDispTitleString("LCD");
				setLCDPrint(4,DISPLAY_POSITION_LEFT,"LCD               OK");	
				break;
			case 2:
				vdDispTitleString("PRINTER");
				vdCheckPrinter(4);	
				break;
			case 3:
				vdDispTitleString("COMMS");
				inCheckCommsHW(4);
				break;
			case 4:
				vdDispTitleString("CONTACTLESS");
				vdCheckCTLSReader(4);
				break;
			case 5:
				vdDispTitleString("MAG READER");
				vdCheckMSRReader(4);
				break;
			case 6:
				vdDispTitleString("ICC READER");
				vdCheckICCReader(4);
				break;

			default:
			break;	
		}
		
		setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO CONFIRM");	

		while(1)
		{
			key = struiGetchWithTimeOut();
			if (key==d_KBD_ENTER)
			    break;
			else
			    vduiWarningSound();
		}
	
		return;
    }
	
}

int inCTOS_vdInstallation(void)
{
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
	char szHeaderString[50] = "INSTALLATION";
    char szHostMenu[50]={"AUTO\nMANUAL"};

	vdDebug_LogPrintf("--inCTOS_vdInstallation--");
	
	CTOS_LCDTClearDisplay();

	//inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return d_NO;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return FAIL;
		
		if(key == 1)
		{
			vdDebug_LogPrintf("--AUTO--");
			inCTOS_vdAutoInstallation();
		}
		else if(key == 2)
		{	
			vdDebug_LogPrintf("--MANUAL--");
			fEducationalCheckList = TRUE;
			inCTOS_vdManualInstallation();
			fEducationalCheckList = FALSE;
		}
		/*else if(key == 3)
			vdDiagnostics();*/			

		return d_OK;
    }

}
void inCTOS_vdAutoInstallation(void)
{
	int inRet = 0;
	unsigned char key;
	//char szCustName[21+1] = {0};
	BYTE g_szPhoneNo[30];
	BYTE szCurrentTime[20];
	CTOS_RTC SetRTC;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("INSTALLATION");
	setLCDPrint(3,DISPLAY_POSITION_CENTER,"FULL TESTING WILL");
	setLCDPrint(4,DISPLAY_POSITION_CENTER,"TAKE A FEW MINUTES");	

	setLCDPrint(6,DISPLAY_POSITION_CENTER,"CONTINUE?");	
	setLCDPrint(7,DISPLAY_POSITION_CENTER," NO[X]  YES[OK] ");	

        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else if (key==d_KBD_CANCEL)
            return;
        else
            vduiWarningSound();
    }
	
	CTOS_LCDTClearDisplay();
#if 1
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strService.szSvcStartTime,TIME_ASC_SIZE);

DIAG_AGAIN:
	
	vdAutoHWDiagnostics("INSTALLATION");
	
	vdSWDiagnostics();

	if(inDisplaySummarywithRetry(SOFTWARE_HARDWARE_TEST) == FAIL)
		goto DIAG_AGAIN;
		
	vdDisplayHWDetailReport();

	vdDisplaySWDetailReport();
	
	inInputReceiverName();

	memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
	inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
	strcpy(strService.szCustPhoneNo,g_szPhoneNo);
		
	ushCTOS_ePadSignatureEx();
#endif
	vdEnterTerminalDetails(INSTALLATION,"INSTALLATION");

	vdEnterFEDetails("INSTALLATION");

	vdDebug_LogPrintf("strService.szCustName[%s]",strService.szCustName);
	vdDebug_LogPrintf("strService.szCustPhoneNo[%s]",strService.szCustPhoneNo);
	vdDebug_LogPrintf("strService.szUnitModel[%s]",strService.szUnitModel);
	vdDebug_LogPrintf("strService.szUnitSN[%s]",strService.szUnitSN);
	vdDebug_LogPrintf("strService.szPowerSN[%s]",strService.szPowerSN);
	vdDebug_LogPrintf("strService.szSIMSN[%s]",strService.szSIMSN);
	vdDebug_LogPrintf("strService.fOriented[%d]",strService.fOriented);
	vdDebug_LogPrintf("strService.szFEName[%s]",strService.szFEName);
	vdDebug_LogPrintf("strService.szOrigin[%s]",strService.szOrigin);
	vdDebug_LogPrintf("strService.szDestination[%s]",strService.szDestination);
	vdDebug_LogPrintf("strService.szFare[%s]",strService.szFare);
	vdDebug_LogPrintf("strService.szAccomodation[%s]",strService.szAccomodation);

	vdConfirmFEDetails(INSTALLATION,"INSTALLATION");
	vdDebug_LogPrintf("strService.szTotalSvcExpense[%s]",strService.szTotalSvcExpense);

	vdCTOS_TransEndReset();
}	

void vdPullOut(void)
{
	BYTE g_szPhoneNo[30+1];
	BYTE szCurrentTime[20];
	CTOS_RTC SetRTC;
	int inCount = 0;

	inCount = inCheckBatcheNotEmtpy();
	vdDebug_LogPrintf("HOSTS WITH BATCH[%d]",inCount);
	
	if(inCount > 0)
    {	
		vdDisplayErrorMsg3(1,8,"PLEASE SETTLE","FIRST","AND TRY AGAIN");
        return;
    }
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("PULL-OUT");
	
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strService.szSvcStartTime,TIME_ASC_SIZE);
	
	inInputReceiverName();

	memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
	inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
	strcpy(strService.szCustPhoneNo,g_szPhoneNo);

	ushCTOS_ePadSignatureEx();

	vdEnterTerminalDetails(PULL_OUT,"PULL-OUT");

	vdEnterFEDetails("PULL-OUT");
		
	vdConfirmFEDetails(PULL_OUT,"PULL-OUT");

	vdCTOS_TransEndReset();
}

void vdReprogramming(void)
{
	BYTE key=0;
	BYTE key2=0;
	BYTE key3=0;
	BYTE g_szPhoneNo[30+1];
	BYTE szCurrentTime[20];
	CTOS_RTC SetRTC;
	int inCount = 0, inRet = -1;

	inCount = inCheckBatcheNotEmtpy();
	vdDebug_LogPrintf("HOSTS WITH BATCH[%d]",inCount);
	
	if(inCount > 0)
    {	
		vdDisplayErrorMsg3(1,8,"PLEASE SETTLE","FIRST","AND TRY AGAIN");
        return;
    }
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("REPROGRAMMING");

	setLCDPrint(3,DISPLAY_POSITION_CENTER,"PERFORM TERMINAL");
	setLCDPrint(4,DISPLAY_POSITION_CENTER,"UPDATE?");	
	
	setLCDPrint(7,DISPLAY_POSITION_CENTER," NO[X]  YES[OK] ");	

        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            inCTOSS_TMSDownloadviaReprogramming();
        else if (key==d_KBD_CANCEL)
        {
            //CTOS_LCDTClearDisplay();
		
			vduiClearBelow(2);
			setLCDPrint(3,DISPLAY_POSITION_CENTER,"FILL OUT FIELD");
			setLCDPrint(4,DISPLAY_POSITION_CENTER,"SERVICE REPORT?");	
			setLCDPrint(7,DISPLAY_POSITION_CENTER," NO[X]  YES[OK] ");	
			while(1)
			{
		        key2 = struiGetchWithTimeOut();
		        if (key2==d_KBD_ENTER)
		       	{	
					CTOS_RTCGet(&SetRTC);
					memset(szCurrentTime,0x00,20);
					sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
					wub_str_2_hex(szCurrentTime,strService.szSvcStartTime,TIME_ASC_SIZE);
					
					inInputReceiverName();

					memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
					inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
					strcpy(strService.szCustPhoneNo,g_szPhoneNo);

					ushCTOS_ePadSignatureEx();

					vdDispTitleString("REPROGRAMMING");
                                              vdEnterTerminalDetails(REPROGRAMMING,"REPROGRAMMING");
                                              
                                              vdEnterFEDetails("REPROGRAMMING");
                                              
                                              vdConfirmFEDetails(REPROGRAMMING,"REPROGRAMMING");
					#if 0
					vduiClearBelow(2);
					setLCDPrint(3,DISPLAY_POSITION_CENTER,"ENGINEER");
					setLCDPrint(4,DISPLAY_POSITION_CENTER,"PRESENT?");	
					setLCDPrint(7,DISPLAY_POSITION_CENTER," NO[X]  YES[OK] ");	

					while(1)
					{
						key3 = struiGetchWithTimeOut();
						if (key3 == d_KBD_ENTER)
				       	{
							vdEnterTerminalDetails(REPROGRAMMING,"REPROGRAMMING");

							vdEnterFEDetails("REPROGRAMMING");
							
							vdConfirmFEDetails(REPROGRAMMING,"REPROGRAMMING");

							break;
						}
						else if (key3==d_KBD_CANCEL)
						{
							CTOS_RTCGet(&SetRTC);
							memset(szCurrentTime,0x00,20);
							sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
								
							wub_str_2_hex(szCurrentTime,strService.szSvcEndTime,TIME_ASC_SIZE);							
							
							vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

							vdFSRPrintMCCHostInfo();

							ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, REPROGRAMMING);

							if(strTCT.byERMMode != 0)
							{
								inRet = inCTOSS_UploadReceipt(FALSE,FALSE);								

								if(inRet == d_OK)
									vdDisplayMultiLineMsgAlign("REPROGRAMMING ", "REPORT UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);			
								else
									vdDisplayMultiLineMsgAlign("REPROGRAMMING ", "REPORT FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);	
							}
							
							
							break;
						}
							
					}
					#endif
					vdCTOS_TransEndReset();

					return;
		        }
		        else if (key2==d_KBD_CANCEL)
					return;				
		        else
		            vduiWarningSound();
		    }


        }	
        else
            vduiWarningSound();
    }	
	
}

void vdNegative(void)
{
	BYTE g_szPhoneNo[30+1];
	BYTE szCurrentTime[20];
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
	BYTE key=0;
    char szHeaderString[50] = "NEGATIVE";
    char szHostMenu[60+1]={"INSTALLATION\nPULL-OUT\nREPROGRAMMING"};
	CTOS_RTC SetRTC;

	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strService.szSvcStartTime,TIME_ASC_SIZE);

	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		if (get_env_int("INVALIDKEY") == 1)
			return;
		
		if(key == 1)
			 strcpy(strService.szNegativeTitle, "NEGATIVE INSTALLATION") ;		
		else if(key == 2)
			strcpy(strService.szNegativeTitle, "NEGATIVE PULL-OUT") ;	
		else if(key == 3)
			strcpy(strService.szNegativeTitle, "NEGATIVE REPROGRAMMING") ;	

    }

	if(key != 1)
	{
		if(inCheckBatcheNotEmtpy() > 0)
	    {	
			vdDisplayErrorMsg3(1,8,"PLEASE SETTLE","FIRST","AND TRY AGAIN");
	        return;
	    }
	}
	
	CTOS_LCDTClearDisplay();

         if (strcmp(strService.szNegativeTitle,"NEGATIVE INSTALLATION")==0)
         	{
           	if (inChooseMerchantorWitness()!=0)
		     return;
         	}
	else
		inInputReceiverName();

	memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
	inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
	strcpy(strService.szCustPhoneNo,g_szPhoneNo);
		
	ushCTOS_ePadSignatureEx();
	
	vdEnterTerminalDetails(NEGATIVE,"NEGATIVE");

	vdEnterFEDetails("NEGATIVE");
	
	vdConfirmFEDetails(NEGATIVE,"NEGATIVE");

	vdCTOS_TransEndReset();
}

void vdAutoHWDiagnostics(char* szTitle)
{
	BOOL fDeviceOk = TRUE;
	USHORT rtn = -1;
	int inRet = -1;
	ULONG ulAPRtn;
	BYTE bySC_status, byMSR_status, key;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1];
	BYTE szCurrentTime[20];
	char szTotalAmount[AMT_ASC_SIZE+1];
	CTOS_RTC SetRTC;
    
	//memset(&strHWDiag,0x00,sizeof(HW_DIAG_TABLE));

//Get HW Diagnostics Start time
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strHWDiag.szHWStartTime,TIME_ASC_SIZE);

	vduiClearBelow(2);
	vdDispTitleString(szTitle);
	setLCDPrint(3,DISPLAY_POSITION_CENTER,"HARDWARE");
	
	
	setLCDPrint(5,DISPLAY_POSITION_LEFT,"LCD               OK");
	strHWDiag.inLCD_Status = SUCCESS;
	
//Check PRINTER
	vdCheckPrinter(6);
	
//Check Comms	
	inRet = inCheckCommsHW(7);
	vdDebug_LogPrintf("inCheckCommsHW[%d]", inRet);

//Check CTLS
	vdCheckCTLSReader(8);

//Check MAG READER

	vdCheckMSRReader(9);

//Check CHIP CARD READER
	vdCheckICCReader(10);

//get HW Diagnostics End time
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strHWDiag.szHWEndTime,TIME_ASC_SIZE);

	//WaitKey(10);
	CTOS_Delay(3000);

	return;

}

int inCheckCommsHW(int inLine)
{
	int inResult = d_OK;
	int inCommMode;
	
	inCPTRead(19); //MCC HOST
	
	inCommMode = strCPT.inCommunicationMode;
	inResult = inCTOS_CheckHardware(inCommMode);
	if(inResult != d_OK)
	{
	    if(inResult == d_ERR_MODEM)
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"DIAL           ERROR");
	    else if(inResult == d_ERR_ETHERNET)
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"ETH            ERROR");
	    else if(inResult == d_ERR_GSM) /*gprs*/
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"GPRS           ERROR");
	    else if(inResult == d_ERR_WIFI)
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"WIFI           ERROR");
	    else
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"DEVICE ERROR");

		strHWDiag.inCOMMS_Status = FAIL;
	}
	else
	{
	    if(inCommMode == DIAL_UP_MODE)
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"DIAL              OK");
	    else if(inCommMode == ETHERNET_MODE)
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"ETH               OK");
	    else if(inCommMode == GPRS_MODE) /*gprs*/
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"GPRS              OK");
	    else if(inCommMode == WIFI_MODE)
	        setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"WIFI              OK");

		strHWDiag.inCOMMS_Status = SUCCESS;
	}

	return(inResult);
	
}

void vdCheckPrinter(int inLine)
{
	int inRet;
	int inPrinterError = FAIL;
	inRet = CTOS_PrinterStatus();
	
	if (inRet == d_OK)
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PRINTER           OK");
		inPrinterError = SUCCESS;
	}
	else if(inRet == d_PRINTER_PAPER_OUT)
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PRINTER     NO PAPER");
	else if(inRet == d_PRINTER_HEAD_OVERHEAT)
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PRINTER    OVER HEAT");
	else 
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PRINTER        ERROR");

	strHWDiag.inPrinter_Status= inPrinterError;
	
	return;
}
void vdCheckMSRReader(int inLine)
{
	BYTE byMSR_status;
	USHORT usTk1Len, usTk2Len, usTk3Len;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[50], szTk3Buf[TRACK_III_BYTES];

	if ((strTCT.byTerminalType % 2) == 0)
		setLCDPrint(14,DISPLAY_POSITION_CENTER," --- SWIPE CARD --- ");
	else
		setLCDPrint(8,DISPLAY_POSITION_CENTER," --- SWIPE CARD --- ");
	
	CTOS_TimeOutSet(TIMER_ID_1, 3000);
	while(1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
			break;		

		CTOS_Delay(300);
		//Read magnetic stripe card data //
		byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
		vdDebug_LogPrintf("CTOS_MSRRead[%d]",byMSR_status);
		if(byMSR_status != 1)
			break;		
	}
	
	if(byMSR_status != 1)
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"MSR               OK");
		strHWDiag.inMSReader_Status = SUCCESS;
	}
	else
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"MSR            ERROR");
		strHWDiag.inMSReader_Status = FAIL;
	}

	if ((strTCT.byTerminalType % 2) == 0)
		clearLine(14);
	else
		clearLine(8);

	vdResetAllCardData();
	
	return;

}	
void vdCheckICCReader(int inLine)
{
	BOOL fDeviceOk = TRUE;
	USHORT rtn = -1;
	USHORT usTk1Len, usTk2Len, usTk3Len;
	BYTE bySC_status;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[50], szTk3Buf[TRACK_III_BYTES];
		
	if ((strTCT.byTerminalType % 2) == 0)
		setLCDPrint(14,DISPLAY_POSITION_CENTER," --- INSERT CARD --- ");
	else
		setLCDPrint(8,DISPLAY_POSITION_CENTER," --- INSERT CARD --- ");
	
	CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
	CTOS_TimeOutSet(TIMER_ID_1, 3000);
	do
	{
		rtn = CTOS_SCStatus(d_SC_USER,&bySC_status);
		vdDebug_LogPrintf("CTOS_SCStatus[%d] :: bySC_status [%d]",rtn,bySC_status);
#if 1
		if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		{
			vdDebug_LogPrintf("ICC CHECK TIMEOUT");
			fDeviceOk = FALSE;
			break;
		}
#endif	
	}while(!(bySC_status & d_MK_SC_PRESENT));

	if(bySC_status == 1)
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"ICC               OK");
		strHWDiag.inICCReader_Status = SUCCESS;
	}
	else
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"ICC            ERROR");
		strHWDiag.inICCReader_Status = FAIL;
	}

	if ((strTCT.byTerminalType % 2) == 0)
		clearLine(14);
	else
		clearLine(8);

	vdResetAllCardData();

	// Remove Card -- sidumili
	WaitKey(1);
	vdDiagRemoveCard();
		
	return;
}

void vdCheckCTLSReader(int inLine)
{
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1];
	ULONG ulAPRtn;
	char szTotalAmount[AMT_ASC_SIZE+1];
#if 0
	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));

	strTCT.byCtlsMode = CTLS_V3_SHARECTLS;
	
	strcpy(szTotalAmount,"000000000100");
	ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,"0608");
#else
	ulAPRtn = EMVCL_Open();
	vdDebug_LogPrintf("EMVCL_Open[%d]",ulAPRtn);
#endif

	if(ulAPRtn != d_EMVCL_NO_ERROR)
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"CTLS           ERROR");

		strHWDiag.inCTLSReader_Status = FAIL;
	}
	else
	{
		setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"CTLS              OK");
		strHWDiag.inCTLSReader_Status = SUCCESS;
	}

	//inCTOSS_CLMCancelTransaction();
	vdResetAllCardData();
	return;
}

void vdResetAllCardData(void)
{
	inCTOSS_CLMCancelTransaction();
	vdCTOS_ResetMagstripCardData();
	memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
	memset(&strEMVT,0x00, sizeof(STRUCT_EMVT));
	srTransRec.bWaveSID = 0x00;
	srTransRec.usChipDataLen = 0;
	srTransRec.usAdditionalDataLen = 0;
	memset(srTransRec.baChipData,0x00,CHIP_DATA_LEN+1);
	memset(srTransRec.baAdditionalData,0x00,ADD_DATA_LEN+1);

}

int inSetCardForInstallation(void)
{
	int inRet = -1;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	BYTE key;
	char szHeaderString[50] = "SET CARD";
    char szHostMenu[50]={"CREDIT\nINSTALLMENT\nBANCNET"};
	char buf[50+1];

	srTransRec.byTransType = SET_CARD;
	CTOS_LCDTClearDisplay();
	
	memset(buf,0x00,sizeof(buf));
#if 0
	inCTOSS_GetEnvDB ("MCCTEST", buf);
	vdDebug_LogPrintf("getenv[%s]",buf);
	
	inCTOSS_GetEnvDB ("INSTTEST", buf);
	vdDebug_LogPrintf("getenv[%s]",buf);

	inCTOSS_GetEnvDB ("BANCNETTEST", buf);
	vdDebug_LogPrintf("getenv[%s]",buf);
#endif	
	//inSetColorMenuMode();

	while(1)
	{
	    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
	    inSetTextMode();

		if (key == 0xFF) 
	    {
	        CTOS_LCDTClearDisplay();
	       //vduiWarningSound();
	        return FAIL;  
	    }

	    if(key > 0)
	    {
			if (get_env_int("INVALIDKEY") == 1)
				return FAIL;
	        
			if(key == 1)
			{
				inRet = inCTOS_GetCardFields();
				if(inRet != d_OK)
					return FAIL;
				
				if(srTransRec.HDTid != 19)
				{
					vdDisplayErrorMsg2(1, 4,  "CARD NOT SUPPORTED", "", MSG_TYPE_ERROR);
					return FAIL;
				}
				
				if(get_env_int("MCCTEST") != 1)
				{
					inRet = inDatabase_TestCardInsert(&srTransRec, MCC_CARD_ID);
					vdDebug_LogPrintf("inDatabase_TestCardInsert[%d]",inRet);
				}
				else
				{
					inRet = inDatabase_TestCardUpdate(&srTransRec, MCC_CARD_ID);
					vdDebug_LogPrintf("inDatabase_TestCardUpdate[%d]",inRet);
				}

				if(inRet == ST_SUCCESS)
					put_env_int("MCCTEST",1);

			}
			else if(key == 2)
			{
				inRet = inCTOS_GetCardFields();
				if(inRet != d_OK)
					return FAIL;
				
				if(strCDT.inInstGroup == 1 && strCDT.fInstallmentEnable == TRUE)
				{}
				else
				{
					vdDisplayErrorMsg2(1, 4,  "CARD NOT SUPPORTED", "", MSG_TYPE_ERROR);
					return FAIL;
				}

				if(get_env_int("INSTTEST") != 1)
				{
					inRet = inDatabase_TestCardInsert(&srTransRec, INST_CARD_ID);
					vdDebug_LogPrintf("inDatabase_TestCardInsert[%d]",inRet);
				}
				else
				{
					inRet = inDatabase_TestCardUpdate(&srTransRec, INST_CARD_ID);
					vdDebug_LogPrintf("inDatabase_TestCardUpdate[%d]",inRet);
				}

				if(inRet == ST_SUCCESS)
					put_env_int("INSTTEST",1);

			}
			else if(key == 3)
			{
				inRet = inCTOS_GetCardFields();
				if(inRet != d_OK)
					return FAIL;
				
				if(srTransRec.HDTid != 12)
				{
					vdDisplayErrorMsg2(1, 4,  "CARD NOT SUPPORTED", "", MSG_TYPE_ERROR);
					return FAIL;
				}

				if(get_env_int("BANCNETTEST") != 1)
				{
					inRet = inDatabase_TestCardInsert(&srTransRec, BANCNET_CARD_ID);
					vdDebug_LogPrintf("inDatabase_TestCardInsert[%d]",inRet);
				}
				else
				{
					inRet = inDatabase_TestCardUpdate(&srTransRec, BANCNET_CARD_ID);
					vdDebug_LogPrintf("inDatabase_TestCardUpdate[%d]",inRet);
				}

				if(inRet == ST_SUCCESS)
					put_env_int("BANCNETTEST",1);
				
			}
			
			vdCTOS_TransEndReset();
			
			
			//return d_OK;
	    }
	}

	return d_OK;
}

void vdDisplaySummary(int inPurpose)
{
	int inHWErrorCtr = 0;
	int inSWErrorCtr = 0;
	int inLineCtr = 0;
	unsigned char key;

	strHWDiag.inHWErrorCount = 0;
	strSWDiag.inSWErrorCount = 0;
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("REPORT");

	if(inPurpose != SOFTWARE_TEST)
	{
		setLCDPrint(2,DISPLAY_POSITION_LEFT,"HARDWARE:");

		if(strHWDiag.inLCD_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"LCD ERROR");
		}

		if(strHWDiag.inPrinter_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"PRINTER ERROR");
		}
		
		if(strHWDiag.inCTLSReader_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"CTLS ERROR");
		}	

		if(strHWDiag.inMSReader_Status== FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"MSR ERROR");
		}	

		if(strHWDiag.inICCReader_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"ICC ERROR");
		}

		if(strHWDiag.inCOMMS_Status == FAIL)
		{
			inHWErrorCtr++;
			switch(strCPT.inCommunicationMode)
			{
				case ETHERNET_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"ETH ERROR");
					break;
				
				case DIAL_UP_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"DIAL ERROR");
					break;

				case GPRS_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"GPRS ERROR");
					break;

				case WIFI_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"WIFI ERROR");
					break;
				default:
		        break;
		    }

		}
		
		if(inHWErrorCtr == 0)
			setLCDPrint(inHWErrorCtr+3,DISPLAY_POSITION_LEFT,"NO ERRORS");

		strHWDiag.inHWErrorCount = inLineCtr = inHWErrorCtr ;
	}

	if(inPurpose != HARDWARE_TEST)
	{
		setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"SOFTWARE");
		
		if(strSWDiag.inCredit_Status == FAIL)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"CREDIT ERROR");
		}
		else if(strSWDiag.inCredit_Status == VOID_FAILED)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"CREDIT VOID FAILED");
		}

		if(strSWDiag.inInst_Status == FAIL)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"INSTALLMENT ERROR");
		}
		else if(strSWDiag.inInst_Status == VOID_FAILED)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"INST VOID FAILED");
		}
#if 1		
		if(strSWDiag.inBancnet_Status == FAIL)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"BANCNET ERROR");
		}
#endif

		if(inSWErrorCtr == 0)
			setLCDPrint(inLineCtr+6,DISPLAY_POSITION_LEFT,"NO ERRORS");

		strSWDiag.inSWErrorCount = inSWErrorCtr;
	}
		
	setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");	

        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else
            vduiWarningSound();
    }
	
}
	
int inDatabase_TestCardInsert(TRANS_DATA_TABLE *transData, int inCardDataID)
{
	int result;
	char *sql = "INSERT INTO TestCardData (CardDataid,byPanLen,szExpireDate,szCardholderName,szPAN,szCardLable,usTrack1Len,usTrack2Len,usTrack3Len,szTrack1Data,szTrack2Data,szTrack3Data) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inCardDataID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inDatabase_TestCardUpdate(TRANS_DATA_TABLE *transData, int inCardDataID)
{
	int result;

	
	/* Issue# 000096: BIN VER Checking -- jzg*/
	char *sql = "UPDATE TestCardData SET byPanLen = ?, szExpireDate = ?, szCardholderName = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ? WHERE CardDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inCardDataID);
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inTestCardRead(TRANS_DATA_TABLE *transData, int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT byPanLen, szExpireDate, szCardholderName, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data FROM TestCardData WHERE CardDataid = ?";


	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		vdDebug_LogPrintf("sqlite3_step");
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("sqlite3_step [%d]",result);
		if (result == SQLITE_ROW) 
		{ /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq);
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

void vdDisplayHWDetailReport(void)
{
	unsigned char key;
	char szBuff[20+1];
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("DETAIL REPORT");
	setLCDPrint(2,DISPLAY_POSITION_CENTER,"HARDWARE:");


	memset(szBuff,0x00,sizeof(szBuff));
	if(strHWDiag.inLCD_Status == SUCCESS)
		strcpy(szBuff,"OK");
	else
		strcpy(szBuff,"ERROR");
	
	inDisplayLeftRight(4, "LCD",szBuff,20);

	memset(szBuff,0x00,sizeof(szBuff));
	if(strHWDiag.inPrinter_Status == SUCCESS)
		strcpy(szBuff,"OK");
	else
		strcpy(szBuff,"ERROR");
	
	inDisplayLeftRight(5, "PRINTER",szBuff,20);

	memset(szBuff,0x00,sizeof(szBuff));
	if(strHWDiag.inCOMMS_Status== SUCCESS)
		strcpy(szBuff,"OK");
	else
		strcpy(szBuff,"ERROR");

	switch(strCPT.inCommunicationMode)
	{
		case ETHERNET_MODE:
			inDisplayLeftRight(6, "ETH",szBuff,20);;
			break;
		
		case DIAL_UP_MODE:
			inDisplayLeftRight(6, "DIAL",szBuff,20);;
			break;

		case GPRS_MODE:
			inDisplayLeftRight(6, "DIAL",szBuff,20);;
			break;

		case WIFI_MODE:
			inDisplayLeftRight(6, "WIFI",szBuff,20);;
			break;
		default:
        break;
    }

	memset(szBuff,0x00,sizeof(szBuff));
	if(strHWDiag.inCTLSReader_Status== SUCCESS)
		strcpy(szBuff,"OK");
	else
		strcpy(szBuff,"ERROR");
	
	inDisplayLeftRight(7, "CTLS",szBuff,20);

	memset(szBuff,0x00,sizeof(szBuff));
	if(strHWDiag.inMSReader_Status== SUCCESS)
		strcpy(szBuff,"OK");
	else
		strcpy(szBuff,"ERROR");
	
	inDisplayLeftRight(8, "MSR",szBuff,20);

	memset(szBuff,0x00,sizeof(szBuff));
	if(strHWDiag.inICCReader_Status== SUCCESS)
		strcpy(szBuff,"OK");
	else
		strcpy(szBuff,"ERROR");
	
	inDisplayLeftRight(9, "ICC",szBuff,20);


	setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");	
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else
            vduiWarningSound();
    }
	
}

void vdDisplaySWDetailReport(void)
{
	unsigned char key;
	char szBuff[20+1];
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("DETAIL REPORT");
	setLCDPrint(2,DISPLAY_POSITION_CENTER,"SOFTWARE:");


	memset(szBuff,0x00,sizeof(szBuff));
	if(strSWDiag.inCredit_Status == SUCCESS)
		strcpy(szBuff,"OK");
	else if(strSWDiag.inCredit_Status == VOID_FAILED)
		strcpy(szBuff,"VOID FAILED");
	else
		strcpy(szBuff,"ERROR");
	
	inDisplayLeftRight(4, "CREDIT",szBuff,20);

	if(get_env_int("TEST_INSTALLMENT") ==1)
         {
              memset(szBuff,0x00,sizeof(szBuff));
              if(strSWDiag.inInst_Status == SUCCESS)
                   strcpy(szBuff,"OK");
              else if(strSWDiag.inInst_Status == VOID_FAILED)
                   strcpy(szBuff,"VOID FAILED");
              else
                   strcpy(szBuff,"ERROR");
              
              inDisplayLeftRight(5, "INST",szBuff,20);
         }
	if(get_env_int("TEST_BANCNET") ==1)
        {
             memset(szBuff,0x00,sizeof(szBuff));
             if(strSWDiag.inBancnet_Status == SUCCESS)
                  strcpy(szBuff,"OK");
             else
                  strcpy(szBuff,"ERROR");
             
             inDisplayLeftRight(6, "BANCNET",szBuff,20);
        }

	setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");	
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else
            vduiWarningSound();
    }

	return;
}


int inInputReceiverName(void)
{
    BYTE bRet;
	USHORT szCustNameLen = 20;
	BYTE szCustName[20+1];
	CTOS_LCDTClearDisplay();

	setLCDPrint(1,DISPLAY_POSITION_LEFT,"ENTER MERCHANT");
	setLCDPrint(2,DISPLAY_POSITION_LEFT,"REPRESENTATIVE NAME:");
	setLCDPrint(4,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(6, DISPLAY_POSITION_LEFT, "ENTER CONTACT NO:");
	setLCDPrint(8,DISPLAY_POSITION_LEFT,"___________ ");
	memset(szCustName,0x00,sizeof(szCustName));
	memset(strService.szCustName,0x00,sizeof(strService.szCustName));
	while(TRUE)
    {
		bRet = InputStringLetters(1, 4, 0x00, 0x02, szCustName, &szCustNameLen, 1, d_INPUT_TIMEOUT);

		if(bRet == d_KBD_ENTER)
		{
			if(strlen(szCustName) > 0)
			{
				break;
			}
		}
				
    }
	
	memcpy(strService.szCustName,szCustName,strlen(szCustName));
		
	return d_OK;	
}

int inCTOSS_EnterNumber(BYTE* szBuff, int inMaxChar, int inMinChar, int inLine)
{
	unsigned char key;
	BYTE sztmpPhoneNo[100];
	USHORT usLen;
	int inRetVal;

	
	while(1)
	{
		CTOS_KBDBufFlush();
		vdDebug_LogPrintf("inCTOSS_EnterNumber...");
		
		usLen = 50;
		memset(sztmpPhoneNo,0x00,sizeof(sztmpPhoneNo));
		key = shCTOS_GetNum(inLine, 0x02,  sztmpPhoneNo, &usLen, inMinChar, inMaxChar, 0, d_INPUT_TIMEOUT);
		vdDebug_LogPrintf("sztmpPhoneNo=[%s]",sztmpPhoneNo);

		strcpy(szBuff,sztmpPhoneNo);

		if  (key == d_KBD_CANCEL){
			vduiClearBelow(10);
			continue;
		}
		
	    else if(key>=1)
		{

			return d_OK;
        }
	}
	
	return d_OK;
}


int ushCTOS_ePadSignatureEx(void)
{
	char szNewFileName[24+1];
	int fNSRflag = 0;
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    //int inRequestPhone = get_env_int("ERMREQNUM");

    DISPLAY_REC szDisplayRec;

	vdDebug_LogPrintf("--ushCTOS_ePadSignatureEx--");	

	vdDebug_LogPrintf("--yushCTOS_ePadSignature--1");
	
     
    memset(szStr, 0x00, sizeof(szStr));	

    memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));

    strcpy(szDisplayRec.szDisplay1," PLEASE SIGN HERE ");
	
	vdDebug_LogPrintf("--yushCTOS_ePadSignature--2");
	
	ePad_SignatureCaptureLibEex(&szDisplayRec);

	vdDebug_LogPrintf("--yushCTOS_ePadSignature--3");
	
	return d_OK;
}

int inCTOS_GetTerminalDetails(BYTE* szBuff, int inColumn, int inRow, USHORT inMax, int inMin)
{
    USHORT usX =1, usY = 6;
    BYTE bShowAttr = 0x02; 
    USHORT inMinLen=inMin;
	//USHORT szInputStrLen=20;
    BYTE szInputStr[20+1];
    BYTE bRet;

    memset(szInputStr, 0x00, sizeof(szInputStr));

    while(TRUE)
    {
        //inAuthCodeLen = 6;
        bRet = InputStringAlpha2Ex(inColumn, inRow, 0x00, 0x02, szInputStr, &inMax, inMinLen, d_INPUT_TIMEOUT);
        if (bRet == d_KBD_CANCEL )
        {
            continue;
        }
        else if (bRet == d_KBD_ENTER ) 
        {
            if(strlen(szInputStr) >= inMinLen)
            {
                strcpy(szBuff, szInputStr);
                break;
            }
            else
            {
                continue;      
            }
        }
        else
        {
            memset(szInputStr, 0x00, sizeof(szInputStr));           
            clearLine(inRow);                
            vdDisplayErrorMsg(inColumn, 8, "INVALID INPUT");	
            clearLine(inRow);
        }
    }

    return ST_SUCCESS;
}


void vdEnterTerminalDetails(int inPurpose, char* szTitle)
{
	BYTE szBuff[20+1];
	unsigned char key;
	char szTermSerialNum[15+1];
	BYTE bShowAttr = 0x02; 
    USHORT inReasonLen=30, inMinLen=3;
	BYTE bRet;
	BYTE szReason[30 + 1] = {0};
	
    CTOS_LCDTClearDisplay();
	vdDispTitleString(szTitle);		
	
    setLCDPrint(2,DISPLAY_POSITION_LEFT,"UNIT MODEL:");
	setLCDPrint(3,DISPLAY_POSITION_LEFT,strService.szUnitModel);
	setLCDPrint(4,DISPLAY_POSITION_LEFT,"UNIT SN:");
	setLCDPrint(5,DISPLAY_POSITION_LEFT,strService.szUnitSN);
	setLCDPrint(6,DISPLAY_POSITION_LEFT,"PWR SN:");
	setLCDPrint(7,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(8,DISPLAY_POSITION_LEFT,"SIM SN:");
	setLCDPrint(9,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(10,DISPLAY_POSITION_LEFT,"DOCK SN:");
	setLCDPrint(11,DISPLAY_POSITION_LEFT,"__________________ ");
#if 0	
	memset(szBuff,0x00,sizeof(szBuff));	
	inCTOS_GetTerminalDetails(szBuff,1,3,20,3);
	strcpy(strService.szUnitModel,szBuff);

	memset(szBuff,0x00,sizeof(szBuff));	
	inCTOS_GetTerminalDetails(szBuff,1,5,20,7);
	strcpy(strService.szUnitSN,szBuff);
#endif
	memset(szBuff,0x00,sizeof(szBuff));	
	inCTOS_GetTerminalDetails(szBuff,1,7,20,0);
	strcpy(strService.szPowerSN,szBuff);

	memset(szBuff,0x00,sizeof(szBuff));	
	inCTOS_GetTerminalDetails(szBuff,1,9,20,0);
	strcpy(strService.szSIMSN,szBuff);
	
	memset(szBuff,0x00,sizeof(szBuff));		
	inCTOS_GetTerminalDetails(szBuff,1,11,20,0);
	strcpy(strService.szDockSN,szBuff);

	if(inPurpose == INSTALLATION)
	{
		setLCDPrint(13,DISPLAY_POSITION_LEFT,"ORIENTED[Y/N]: ");
		while(1)
		{
			memset(szBuff,0x00,sizeof(szBuff)); 
			inCTOS_GetTerminalDetails(szBuff,16,14,1,1);
			if(szBuff[0] != 'Y' && szBuff[0] != 'N')
				continue;
			else
			{
				if(szBuff[0] == 'Y')
					strService.fOriented = TRUE;
				else
					strService.fOriented = FALSE;

				break;
			}
		}
	}
	else if(inPurpose == PULL_OUT || inPurpose == REPROGRAMMING || inPurpose == NEGATIVE)
	{
		setLCDPrint(13,DISPLAY_POSITION_LEFT,"REASON: ");
		memset(szBuff,0x00,sizeof(szBuff));	
		//inInputName(szBuff,1,14);

		while (TRUE)
		{
			bRet = InputStringAlphaEx2(1, 14, 0x00, bShowAttr, szReason, &inReasonLen, inMinLen, d_INPUT_TIMEOUT);

			if(bRet == d_KBD_ENTER)
			{
				if(strlen(szReason) > 0)
				{
					break;
				}
			}
		}
		
		strcpy(strService.szReason,szReason);
	}
	
	setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");
       
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else
            vduiWarningSound();
    }
	
}


void vdEnterFEDetails(char* szTitle)
{
	BYTE szBuff[20+1];
	unsigned char key;
	char szDisplayBuf[30];
	BYTE baAmount[20];
	ULONG ulAmount = 0L;
	BYTE bFirstKey;
	BYTE szCurrentTime[20];
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString(szTitle);		

	setLCDPrint(2,DISPLAY_POSITION_LEFT,"FIELD ENGINEER:");
	setLCDPrint(3,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(4,DISPLAY_POSITION_LEFT,"ARRIVAL:");
	setLCDPrint(5,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(6,DISPLAY_POSITION_LEFT,"ORIGIN:");
	setLCDPrint(7,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(8,DISPLAY_POSITION_LEFT,"DESTINATION:");
	setLCDPrint(9,DISPLAY_POSITION_LEFT,"__________________ ");
	setLCDPrint(10,DISPLAY_POSITION_LEFT,"FARE:");
	setLCDPrint(11,DISPLAY_POSITION_LEFT,"___________ ");
	setLCDPrint(12,DISPLAY_POSITION_LEFT,"ACCOM:");
	setLCDPrint(13,DISPLAY_POSITION_LEFT,"___________ ");
	
	memset(szBuff,0x00,sizeof(szBuff));	
	inInputName(szBuff,1,3);
	strcpy(strService.szFEName,szBuff);

	memset(szBuff,0x00,sizeof(szBuff));	
	clearLine(5);
	inCTOSS_EnterTime(szBuff,6,6,5);
	strcpy(szCurrentTime,szBuff);
	wub_str_2_hex(szCurrentTime,strService.szArrivalTime,TIME_ASC_SIZE);

	memset(szBuff,0x00,sizeof(szBuff));	
	inInputName(szBuff,1,7);
	strcpy(strService.szOrigin,szBuff);

	memset(szBuff,0x00,sizeof(szBuff));	
	inInputName(szBuff,1,9);
	strcpy(strService.szDestination,szBuff);
	
	memset(szBuff,0x00,sizeof(szBuff));	
    key = InputAmount3(1, 11, szDisplayBuf, 2, 0x00, szBuff, &ulAmount, d_INPUT_TIMEOUT, 0);
	strcpy(strService.szFare,szBuff);
	vdDebug_LogPrintf("strService.szFare[%s]",strService.szFare);
	
	memset(szBuff,0x00,sizeof(szBuff));		
	key = InputAmount3(1, 13, szDisplayBuf, 2, 0x00, szBuff, &ulAmount, d_INPUT_TIMEOUT, 1);
	strcpy(strService.szAccomodation,szBuff);
	vdDebug_LogPrintf("strService.szAccomodation[%s]",strService.szAccomodation);

	
	setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");	
       
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else
            vduiWarningSound();
    }
}

void vdConfirmFEDetails(int inPurpose, char* szTitle)
{
	int inRet = -1;
	BYTE szBuff[20+1];
	BYTE szBuff2[20+1];
	BYTE szCurrentTime[20];
	unsigned char key;
	char baAmount[24+1];
	CTOS_RTC SetRTC;

	vdDebug_LogPrintf("--vdConfirmFEDetails--");
	vdDebug_LogPrintf("inPurpose[%d].byERMMode[%d].szTitle[%d]", inPurpose, strTCT.byERMMode, szTitle);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString(szTitle);		

	memset(szBuff,0x00,20);
	sprintf(szBuff,"NAME: %s",strService.szFEName);
	setLCDPrint(2,DISPLAY_POSITION_LEFT,szBuff);

	memset(szBuff,0x00,20);
	memset(szBuff2,0x00,20);
	memset(szCurrentTime,0x00,20);
	wub_hex_2_str(strService.szArrivalTime,szCurrentTime,TIME_BCD_SIZE);
	vdFormatDisplayTime(szCurrentTime,szBuff2);
	sprintf(szBuff,"ARRIVAL: %s",szBuff2);
	setLCDPrint(3,DISPLAY_POSITION_LEFT,szBuff);

	memset(szBuff,0x00,20);
	sprintf(szBuff,"ORIG: %s",strService.szOrigin);
	setLCDPrint(4,DISPLAY_POSITION_LEFT,szBuff);

	memset(szBuff,0x00,20);
	sprintf(szBuff,"DEST: %s",strService.szDestination);
	setLCDPrint(5,DISPLAY_POSITION_LEFT,szBuff);

	memset(szBuff,0x00,20);
	memset(baAmount,0x00,sizeof(baAmount));
	vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strService.szFare, baAmount);
	sprintf(szBuff,"FARE: %s",baAmount);
	setLCDPrint(6,DISPLAY_POSITION_LEFT,szBuff);

	memset(szBuff,0x00,20);
	memset(baAmount,0x00,sizeof(baAmount));
	vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strService.szAccomodation, baAmount);
	sprintf(szBuff,"ACCOM: %s",baAmount);
	setLCDPrint(7,DISPLAY_POSITION_LEFT,szBuff);	

	memset(szBuff,0x00,20);
	memset(baAmount,0x00,sizeof(baAmount));
	sprintf(strService.szTotalSvcExpense, "%012.0f", atof(strService.szFare) + atof(strService.szAccomodation));
	vdDebug_LogPrintf("szTotalSvcExpense[%s]",strService.szTotalSvcExpense);
	vdTrimSpaces(strService.szTotalSvcExpense);
	vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strService.szTotalSvcExpense, baAmount);
	sprintf(szBuff,"TOTAL: %s",baAmount);
	setLCDPrint(8,DISPLAY_POSITION_LEFT,szBuff);
	
	setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");	

	// Check ERM Connection
	fERMUploadFailed = FALSE;
	inRet = inCTOS_ERMPreConnect(FALSE);	
	if (inRet != d_OK)
		fERMUploadFailed = TRUE;
		
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            break;
        else
            vduiWarningSound();
    }

	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
		
	if(inPurpose == SOFTWARE_TEST)
		wub_str_2_hex(szCurrentTime,strSWDiag.szSWEndTime,TIME_ASC_SIZE);
	else if(inPurpose == HARDWARE_TEST)
		wub_str_2_hex(szCurrentTime,strHWDiag.szHWEndTime,TIME_ASC_SIZE);	
	else
		wub_str_2_hex(szCurrentTime,strService.szSvcEndTime,TIME_ASC_SIZE);
	
	//if(inPurpose != NEGATIVE)
	{
		vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

		vdFSRPrintMCCHostInfo();

		// Capture merchant copy for upload -- sidumili
		fCaptureCopy = TRUE;		
		ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, inPurpose);

		// Print merchant copy
		fCaptureCopy = FALSE;
		ushCTOS_PrintServiceBody(MERCHANT_COPY_RECEIPT, inPurpose);
		
		//aaronnino Additional Bank Copy of FSR for Pullout and Negative installation start
	    if((inPurpose == PULL_OUT) || (inPurpose==NEGATIVE))
        {
            while(1)
            { 				
                 
                 setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT BANK COPY?");
                 setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");
                 
                 vduiWarningSound();
                 
                 CTOS_KBDHit(&key);
                 if(key == d_KBD_ENTER)
                 {
                      ushCTOS_PrintServiceBody(BANK_COPY_RECEIPT, inPurpose);
                      CTOS_KBDBufFlush();//cleare key buffer
                      break;
                 }
                 else if((key == d_KBD_CANCEL))
                 {
                      //CTOS_LCDTClearDisplay();
      				  CTOS_KBDBufFlush();//cleare key buffer
                      break;
                 }

            }
        }
		//aaronnino Additional Bank Copy of FSR for Pullout and Negative installation end	

		if(strTCT.byERMMode != 0)
		{
			inRet = inCTOSS_UploadReceipt(FALSE,FALSE);
			

			if(inRet == d_OK)
			{
				if(inPurpose == INSTALLATION)
					vdDisplayMultiLineMsgAlign("FULL TESTING", "REPORT UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);
				else if(inPurpose == PULL_OUT)
					vdDisplayMultiLineMsgAlign("PULL-OUT REPORT", "UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);
				else if(inPurpose == REPROGRAMMING)
					vdDisplayMultiLineMsgAlign("REPROGRAMMING ", "REPORT UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);
				else if(inPurpose == NEGATIVE)
					vdDisplayMultiLineMsgAlign("NEGATIVE ", "REPORT UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);
			}
			else
			{
				if(inPurpose == INSTALLATION)
					vdDisplayMultiLineMsgAlign("FULL TESTING", "REPORT FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);
				else if(inPurpose == PULL_OUT)
					vdDisplayMultiLineMsgAlign("PULL-OUT REPORT", "FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);
				else if(inPurpose == REPROGRAMMING)
					vdDisplayMultiLineMsgAlign("REPROGRAMMING ", "REPORT FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);			
				else if(inPurpose == NEGATIVE)
					vdDisplayMultiLineMsgAlign("NEGATIVE ", "REPORT FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);
				fERMUploadFailed = TRUE;
			}
		}
	}
	
	ushCTOS_ePadSignatureEx();

	vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	ushCTOS_PrintServiceBody(ENGINEER_COPY_RECEIPT, inPurpose);
#if 0
	if(inPurpose == NEGATIVE)
	{
		if(strTCT.byERMMode =! 0)
		{
			inRet = inCTOSS_UploadReceipt(TRUE,FALSE);
			if(inRet == d_OK)
				vdDisplayMultiLineMsgAlign("NEGATIVE ", "REPORT UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);
			else
				vdDisplayMultiLineMsgAlign("NEGATIVE ", "REPORT FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);
		}
	}
#endif
}

void vdTrimSpaces(char* pchString) {
    int inIndex;

    while (1) {
        inIndex = strlen(pchString);
        if (inIndex) {
            if (pchString[inIndex - 1] == IS_SPACE) {
                pchString[inIndex - 1] = IS_NULL_CH;
                continue;
            }
        }
        break;
    }
}

int inInputName(BYTE* szBuff, USHORT usColumn, USHORT usRow)
{
    BYTE bRet;
	USHORT szCustNameLen = 20;
	BYTE szCustName[20+1];
	//CTOS_LCDTClearDisplay();

	memset(szCustName,0x00,sizeof(szCustName));
	//
	while(TRUE)
    {
		bRet = InputStringLetters(usColumn, usRow, 0x00, 0x02, szCustName, &szCustNameLen, 3, d_INPUT_TIMEOUT);

		if(bRet == d_KBD_ENTER)
		{
			if(strlen(szCustName) > 0)
			{
				break;
			}
		}
				
    }
	
	memcpy(szBuff,szCustName,strlen(szCustName));
		
	return d_OK;	
}

int inCheckBatchEmtpy(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TransData where byTransType <> 102";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_INVOICE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

void inCTOS_vdManualInstallation(void)
{
	unsigned char key;
	//char szCustName[21+1] = {0};
	BYTE g_szPhoneNo[30];
	BYTE szCurrentTime[20];
	CTOS_RTC SetRTC;
	char szvalue[1+1];
	 int  inFileSize; 
	 FILE *hHandle;
	 char szFile[100];

	//memset(&strService,0x00,sizeof(SERVICE_INFO_TABLE));

	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strService.szSvcStartTime,TIME_ASC_SIZE);
	
	if(fMerchantOriented == FALSE)
	{
		CTOS_LCDTClearDisplay();
		vdDispTitleString("INSTALLATION");
		setLCDPrint(3,DISPLAY_POSITION_CENTER,"PRESS ENTER TO");
		setLCDPrint(4,DISPLAY_POSITION_CENTER,"ORIENT MERCHANT");	

		//setLCDPrint(6,DISPLAY_POSITION_CENTER,"CONTINUE?");	
		//setLCDPrint(7,DISPLAY_POSITION_CENTER," NO[X]  YES[OK] ");	
	        
	    while(1)
	    {
	        key = struiGetchWithTimeOut();
	        if (key==d_KBD_ENTER)
                 {
                      fMerchantOriented = TRUE;
                      if ( inCTOSS_GetEnvDB ("#ORIENTATION", szvalue) != d_OK )
                      {
                           inCTOSS_PutEnvDB("#ORIENTATION", "1");
		         		   inCTOSS_PutEnvDB("#ORIENTATIONTRANS", "0");
                      }
                      else
                      {
                            put_env_char("#ORIENTATION","1");
							put_env_char("#ORIENTATIONTRANS", "0");
                      }
					  
						strcpy(szFile, "/home/ap/pub/fsrtrans.dat");
						hHandle = fopen(szFile, "w");
						fclose(hHandle);

						vdSetFSRMode(MERCHANT_ORIENT);
						
                     	return;
                      }
		}
	}
	
	CTOS_LCDTClearDisplay();
#if 1
	vdEnterManualSWDiagStatus();

	DIAG_AGAIN:
	
	vdAutoHWDiagnostics("INSTALLATION");

	//vdDisplaySummary(SOFTWARE_HARDWARE_TEST);
	if(inDisplaySummarywithRetry(SOFTWARE_HARDWARE_TEST) == FAIL)
		goto DIAG_AGAIN;

	//vdDisplayHWDetailReport();

	//vdDisplaySWDetailReport();

	inMerchantCheckList();
	
	inInputReceiverName();

	memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
	inCTOSS_EnterNumber(g_szPhoneNo, 11, 7, 8);
	strcpy(strService.szCustPhoneNo,g_szPhoneNo);
		
	ushCTOS_ePadSignatureEx();
#endif
	vdEnterTerminalDetails(INSTALLATION,"INSTALLATION");

	vdEnterFEDetails("INSTALLATION");

	vdDebug_LogPrintf("strService.szCustName[%s]",strService.szCustName);
	vdDebug_LogPrintf("strService.szCustPhoneNo[%s]",strService.szCustPhoneNo);
	vdDebug_LogPrintf("strService.szUnitModel[%s]",strService.szUnitModel);
	vdDebug_LogPrintf("strService.szUnitSN[%s]",strService.szUnitSN);
	vdDebug_LogPrintf("strService.szPowerSN[%s]",strService.szPowerSN);
	vdDebug_LogPrintf("strService.szSIMSN[%s]",strService.szSIMSN);
	vdDebug_LogPrintf("strService.fOriented[%d]",strService.fOriented);
	vdDebug_LogPrintf("strService.szFEName[%s]",strService.szFEName);
	vdDebug_LogPrintf("strService.szOrigin[%s]",strService.szOrigin);
	vdDebug_LogPrintf("strService.szDestination[%s]",strService.szDestination);
	vdDebug_LogPrintf("strService.szFare[%s]",strService.szFare);
	vdDebug_LogPrintf("strService.szAccomodation[%s]",strService.szAccomodation);

	vdConfirmFEDetails(INSTALLATION,"INSTALLATION");
	vdDebug_LogPrintf("strService.szTotalSvcExpense[%s]",strService.szTotalSvcExpense);

	vdPrintOrientationInvoiceNumbers();

	put_env_char("#ORIENTATION","0");
	put_env_char("#ORIENTATIONTRANS", "0");
	
	fMerchantOriented = FALSE; 
	
	vdCTOS_TransEndReset();
}

void vdEnterManualSWDiagStatus(void)
{
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("INSTALLATION");

	strSWDiag.inCredit_Status = inSWDiagAnswer("CREDIT");
	if(get_env_int("TEST_INSTALLMENT") ==1)
	strSWDiag.inInst_Status = inSWDiagAnswer("INSTALLMENT");
	if(get_env_int("TEST_BANCNET") ==1)
	strSWDiag.inBancnet_Status = inSWDiagAnswer("BANCNET");
	
}

int inSWDiagAnswer(char *szApplication)
{
	BOOL isKey;
	unsigned char key;
	
    vduiClearBelow(2);
    vduiDisplayStringCenter(3,szApplication);
    vduiDisplayStringCenter(4,"SUCCESSFUL?");
    //vduiDisplayStringCenter(5,"CONTINUE?");
    vduiDisplayStringCenter(5,"NO[X]   YES[OK]");    
	
    while(1)
    {
        while(1)
	    {
	        CTOS_KBDInKey(&isKey);
	        if (isKey){ //If isKey is TRUE, represent key be pressed //
	            vduiLightOn();
	            //Get a key from keyboard //
	            CTOS_KBDGet(&key);
				break;
	        }	        
	    }

		if (key==d_KBD_ENTER)
            return SUCCESS;
        else if (key==d_KBD_CANCEL)
            return FAIL;
    }
}

int inCTOSS_EnterTime(BYTE* szBuff, int inMaxChar, int inMinChar, int inLine)
{
	unsigned char key;
	BYTE szTime[100];
	BYTE sztmpPhoneNo[100];
	USHORT usLen;
	int inRetVal;

	
	while(1)
	{
		CTOS_KBDBufFlush();
		vdDebug_LogPrintf("inCTOSS_EnterTime...");
		
		usLen = 50;
		memset(szTime,0x00,sizeof(szTime));
		key = shCTOS_GetTime(inLine, 0x02,  szTime, &usLen, inMinChar, inMaxChar, 0, d_INPUT_TIMEOUT);
		//key = shCTOS_GetNum(inLine, 0x02,  sztmpPhoneNo, &usLen, inMinChar, inMaxChar, 0, d_INPUT_TIMEOUT);
		vdDebug_LogPrintf("szTime=[%s]",szTime);

		strcpy(szBuff,szTime);

		if  (key == d_KBD_CANCEL){
			vduiClearBelow(10);
			continue;
		}
		
	    else if(key>=1)
		{

			return d_OK;
        }
	}
	
	return d_OK;
}

USHORT shCTOS_GetTime(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{
    
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[128];
    USHORT  usInputStrLen;
	BYTE    bOutputStrData[128];
    USHORT  usOutputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));
    
    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    vdDebug_LogPrintf("start [%d] data[%s]", strlen(baBuf), baBuf);
    if(strlen(baBuf) > 0 )
    {
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bDisplayStr, 0x20, usMaxLen*2);
        usInputStrLen = strlen(baBuf);
        strcpy(bInputStrData, baBuf);
		vdFormatDisplayTime(bInputStrData,bOutputStrData);
		vdDebug_LogPrintf("bInputStrData[%s] :: bOutputStrData[%s]",bInputStrData,bOutputStrData);
        if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bOutputStrData))*2], bOutputStrData);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bOutputStrData, strlen(bOutputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
    }
	else
	{
		strcpy(bInputStrData, "000000");
		vdFormatDisplayTime(bInputStrData,bOutputStrData);
		if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bOutputStrData))*2], bOutputStrData);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bOutputStrData, strlen(bOutputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
	}
    
    while(1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        }
        
        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
        case d_KBD_DOT:
            break;
        case d_KBD_CLEAR:
            if (usInputStrLen)
            {
                usInputStrLen--;
                bInputStrData[usInputStrLen] = '0';
                vdFormatDisplayTime(bInputStrData,bOutputStrData);
                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bOutputStrData))*2], bOutputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bOutputStrData, strlen(bOutputStrData));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_CANCEL:
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            if (usInputStrLen < usMaxLen )
            {
                bInputStrData[usInputStrLen++] = bKey;
				vdFormatDisplayTime(bInputStrData,bOutputStrData);
                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bOutputStrData))*2], bOutputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bOutputStrData, strlen(bOutputStrData));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_ENTER:
            if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
            {   
                *usStrLen = usInputStrLen;
                strcpy(baBuf, bInputStrData);
                return *usStrLen;
            }
            else if(usByPassAllow && 0 == usInputStrLen)
            {   
                *usStrLen = usInputStrLen;
                baBuf[0] = 0x00;
                return *usStrLen;
            }
            break;
        default :
            break;
        }
    }

    return 0;
}


void vdFormatDisplayTime(BYTE* bInputStrData, BYTE* bOutputStrData)
{
	memcpy(bOutputStrData,bInputStrData,2);
	bOutputStrData[2] = ':';
	memcpy(&bOutputStrData[3],&bInputStrData[2],2);
	bOutputStrData[5] = ':';
	memcpy(&bOutputStrData[6],&bInputStrData[4],2);
}


int inADiagStart1;
int inADiagEnd1;
int inADiagStart2;
int inADiagEnd2;
int inADiagStart3;
int inADiagEnd3;
int inADiagTimeRangeUsed;
int inADiagLimit;
int inMaxTime;
int inNotifyStart;
int inNotifyEnd;

//ADC
int inADLStart1;
int inADLEnd1;
int inADLStart2;
int inADLEnd2;
int inADLStart3;
int inADLEnd3;
int inADLTimeRangeUsed;
int inADLLimit;
int inMaxTime;
//ADC



int inCTOS_AutoDiag_ParamCheck(void){

    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, Menukey=0;
	int inRet;
	int key;
	USHORT shRet = 0,
		shLen = 0,
		iInitX = 0,
		shMinLen = 4,
		shMaxLen = 6;
	BYTE szTitleDisplay[MAX_CHAR_PER_LINE + 1] = {0},
		szTitle[MAX_CHAR_PER_LINE + 1] = {0};

  	CTOS_RTC rtcClock;
  	CTOS_RTC SetRTC;
  	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;

	int inAutoDiagStart;	
	int inAutoDiagEnd;
	int inADiagPerformed;
	
	int inADiagTries;
  	BYTE szCurrDate[8] = {0};	
  	BYTE szADiagDate[8] = {0};

	if (get_env_int("ADTYPE") == 0 || get_env_int("ADTYPE") == -1)	
		return d_OK;	

	vdDebug_LogPrintf("inNotifyADEnable[%d]",inNotifyADEnable);
	if(inNotifyADEnable	== TRUE)
		inCTOSS_Notify_DayBefore_Scheduled_Diagnostics();
		
	memset(szCurrDate, 0x00, sizeof(szCurrDate));
	CTOS_RTCGet(&SetRTC);
	sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

	//inAutoDLTime = get_env_int("AUTODLTIME");
	
    memset(szADiagDate, 0x00, sizeof(szADiagDate));
	inCTOSS_GetEnvDB("ADCURRDATE", szADiagDate);

	vdDebug_LogPrintf("DATE %s %s", szADiagDate, szCurrDate);

	if (strcmp(szADiagDate,szCurrDate)!=0) {
		put_env_int("AD",0);
		put_env_char("ADCURRDATE",szCurrDate);
		put_env_int("ADTRY1",0);	
		//put_env_int("ADTRY2",0);
		//put_env_int("ADTRY3",0);	
		
	}
	
	inADiagPerformed = get_env_int("AD");
	
	
	if (inADiagPerformed == 1) 
		return SUCCESS;


	vdDebug_LogPrintf("ADC  %d %d",  inADiagStart1,inADiagEnd1 );
	//vdDebug_LogPrintf("ADC  %d %d",  inADiagStart2,inADiagEnd2 );
	//vdDebug_LogPrintf("ADC  %d %d",  inADiagStart3,inADiagEnd3 );

   
    memset(szCurrTime, 0x00, sizeof(szCurrTime));
    CTOS_RTCGet(&rtcClock);
    sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
    inCurrTime = wub_str_2_long(szCurrTime);


	if (inCurrTime >= inADiagStart1 && inCurrTime <= inADiagEnd1){
		inAutoDiagStart = inADiagStart1;
		inAutoDiagEnd = inADiagEnd1;
		inADiagTimeRangeUsed = 1;
		vdDebug_LogPrintf("TIME1	%d %d",  inAutoDiagStart,inAutoDiagEnd );	
	}
#if 1
	else if (inCurrTime >= inADiagStart2 && inCurrTime <= inADiagEnd2){
		inAutoDiagStart = inADiagStart2;
		inAutoDiagEnd = inADiagEnd2;
		inADiagTimeRangeUsed = 2;	
		vdDebug_LogPrintf("TIME2	%d %d",  inAutoDiagStart,inAutoDiagEnd );
	}else if (inCurrTime >= inADiagStart3 && inCurrTime <= inADiagEnd3){
		inAutoDiagStart = inADiagStart3;
		inAutoDiagEnd = inADiagEnd3;
		inADiagTimeRangeUsed = 3;	
		vdDebug_LogPrintf("TIME3	%d %d",  inAutoDiagStart,inAutoDiagEnd );
	}else
#endif
	{
		return SUCCESS;
	}


	
	if (inADiagTimeRangeUsed == 1){
		if (get_env_int("ADTRY1") == 1)
			return SUCCESS;
	}
#if 1
	else if (inADiagTimeRangeUsed == 2){
		if (get_env_int("ADTRY2") == 1)
			return SUCCESS;
	}else if (inADiagTimeRangeUsed == 3){
		if (get_env_int("ADTRY3") == 1)
			return SUCCESS;
	}
#endif			
	vdDebug_LogPrintf("ADC %d %d %d", inCurrTime, inAutoDiagStart,inAutoDiagEnd );

	if (inCurrTime >= inAutoDiagStart &&  inCurrTime <= inAutoDiagEnd){

	    if ((inADiagTimeRangeUsed+1 > inADiagLimit)	|| (inCurrTime > inMaxTime))
			put_env_int("AD",1);

        inCTOSS_Scheduled_Diagnostics();
	
	}
  	return SUCCESS;


}

int inCTOSS_Scheduled_Diagnostics(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
	BOOL isKey;
	unsigned char key;
    int inYear, inMonth, inDate,inDateGap,inAutoDiagGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;
	char buf[6+1];
	char szYear[2+1];
	char szMonth[2+1];
	char szDate[2+1];
		
	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    
    vdDebug_LogPrintf("inCTOSS_Scheduled_Diagnostics");
   
	inCTOSS_GetEnvDB("LASTAD", buf);
	memcpy(szYear,buf,2);
	memcpy(szMonth,&buf[2],2);
	memcpy(szDate,&buf[4],2);
	
	if (inADiagTimeRangeUsed == 1)
		put_env_int("ADTRY1",1);	
#if 1
	else if (inADiagTimeRangeUsed == 2)
		put_env_int("ADTRY2",1);	
	else if (inADiagTimeRangeUsed == 3)
		put_env_int("ADTRY3",1);	
#endif

	inYear = atoi(szYear);
    inMonth = atoi(szMonth);
    inDate = atoi(szDate);

	vdDebug_LogPrintf("inYear[%d] :: inMonth[%d] :: inDate[%d]",inYear,inMonth,inDate);
    CTOS_RTCGet(&SetRTC);

    inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
	 

	inCTOSS_GetEnvDB("ADGAP", buf);
	inAutoDiagGap = atoi(buf);
	vdDebug_LogPrintf("inDateGap=[%d],strTCT.inAutoDiagGap=[%d]",inDateGap,inAutoDiagGap);
	
	if(inDateGap < inAutoDiagGap)
        return d_NO;
	
	
	CTOS_LCDTClearDisplay();

    vdScheduledTerminalDiagnostics();

	put_env_int("AD",1);
	put_env_int("NOTIFYAD",0);
	
	sprintf(buf,"%02d%02d%02d",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay);

	vdDebug_LogPrintf("LASTAD UPDATE[%s]",buf);
	put_env_char("LASTAD",buf);
	CTOS_LCDTClearDisplay();
	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}


void vdSetADiagParams(void){

  	CTOS_RTC rtcClock;
  	CTOS_RTC SetRTC;
  	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;

  	BYTE szADiagDate[8] = {0};
  	BYTE szCurrDate[8] = {0};

	int inStart1;
	int inEnd1;
	int inStart2;
	int inEnd2;
	int inStart3;
	int inEnd3;


	memset(szCurrDate, 0x00, sizeof(szCurrDate));
	CTOS_RTCGet(&SetRTC);
	sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

	inCTOSS_GetEnvDB("ADCURRDATE", szADiagDate);

	
	if ((strcmp(szADiagDate,"")==0) || (strcmp(szADiagDate,"000000")==0)){
		put_env_char("ADCURRDATE",szCurrDate);

	}

	inADiagStart1 = get_env_int("AD1START");
	inADiagEnd1 = get_env_int("AD1END");

	
	inADiagStart2= get_env_int("AD2START");
	inADiagEnd2= get_env_int("AD2END");

	inMaxTime = 0;
	
	inADiagStart3= get_env_int("AD3START");
	inADiagEnd3= get_env_int("AD3END");

	if (inADiagStart1 > 0 && inADiagEnd1 > 0){
		inADiagLimit= inADiagLimit+1;
		inMaxTime = inADiagEnd1;
	}

	if (inADiagStart2 > 0 && inADiagEnd2 > 0){
		inADiagLimit= inADiagLimit+1;
		inMaxTime = inADiagEnd2;
	}
	
	if (inADiagStart3 > 0 && inADiagEnd3 > 0){
		inADiagLimit= inADiagLimit+1;
		inMaxTime = inADiagEnd3;
	}

	inNotifyStart = get_env_int("NOTIFYSTART");
	inNotifyEnd = get_env_int("NOTIFYEND");
	

}
#if 0
void put_env_char(char *tag, char *value)
{
	int     ret = -1;
	char    buf[12];

    memset (buf, 0, sizeof (buf));
    //int2str (buf, value);
    sprintf(buf, "%s", value);
    ret = inCTOSS_PutEnvDB (tag, buf);

	vdDebug_LogPrintf("put_env_char [%s]=[%s] ret[%d]", tag, value, ret);
}
#endif
void vdScheduledHWDiagnostics(char* szTitle)
{
	BOOL fDeviceOk = TRUE;
	USHORT rtn = -1;
	int inRet = -1;
	ULONG ulAPRtn;
	BYTE bySC_status, byMSR_status, key;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1];
	BYTE szCurrentTime[20];
	char szTotalAmount[AMT_ASC_SIZE+1];
	CTOS_RTC SetRTC;
    
	//memset(&strHWDiag,0x00,sizeof(HW_DIAG_TABLE));

//START - Get HW Diagnostics Start time
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strHWDiag.szHWStartTime,TIME_ASC_SIZE);
//END - Get HW Diagnostics Start time

	vduiClearBelow(2);
	vdDispTitleString(szTitle);
	
	setLCDPrint(3,DISPLAY_POSITION_CENTER,"HARDWARE");
		
	setLCDPrint(5,DISPLAY_POSITION_LEFT,"LCD               OK");
	strHWDiag.inLCD_Status = SUCCESS;
	
//Check PRINTER
	vdCheckPrinter(6);
	
//Check Comms	
	inRet = inCheckCommsHW(7);
	vdDebug_LogPrintf("inCheckCommsHW[%d]", inRet);

//Check CTLS
	vdCheckCTLSReader(8);

// START get HW Diagnostics End time
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime,0x00,20);
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,strHWDiag.szHWEndTime,TIME_ASC_SIZE);
// END get HW Diagnostics End time

	//WaitKey(10);
	CTOS_Delay(5000);

	return;

}

void vdScheduledTerminalDiagnostics(void)
{
	int inRet = -1;

	vdDebug_LogPrintf("--vdScheduledTerminalDiagnostics--");
	
	fERMUploadFailed = FALSE;
	memset(&strHWDiag,0x00,sizeof(HW_DIAG_TABLE));
	memset(&strSWDiag,0x00,sizeof(SW_DIAG_TABLE));

	if(get_env_int("ADTYPE") != SCHEDULED_SOFTWARE)
		vdScheduledHWDiagnostics("HARDWARE DIAG");

	if(get_env_int("ADTYPE") != SCHEDULED_HARDWARE)
		vdSWDiagnostics();

	vdDisplaySummary(SOFTWARE_HARDWARE_TEST);

	inRet = inCTOS_ERMPreConnect(TRUE);	
	if (inRet != d_OK)	
		fERMUploadFailed = TRUE;
	
	ushCTOS_PrintScheduledDiagBody();
	
	if(strTCT.byERMMode != 0 && !fERMUploadFailed)
	{

		inHDTRead(19); //mcc host 
		inRet = inCTOSS_UploadReceipt(FALSE,FALSE);
	
		if(inRet == d_OK)
		{
			vdDisplayMultiLineMsgAlign("SCHEDULED DIAG", "REPORT UPLOAD", "SUCCESSFUL", DISPLAY_POSITION_CENTER);
		}
		else
		{
			vdDisplayMultiLineMsgAlign("SCHEDULED DIAG ", "UPLOAD FAILED", "CHECK CONNECTIVITY", DISPLAY_POSITION_CENTER);
		}

	}		
}

BYTE InputStringAlpha2Ex(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    char szTemp[40+1], szTemp2[40+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[40+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	int x=0;
	int inLastKey=255;
	int inKey=0;
	int inKey2=0;
    int inMax=*usStrLen;
	int inAtCtr=0;
	char KeyPad[10][4]={{'0', '0', '0', '0'},
		               {'1', 'Q', 'Z', ' '},
		               {'2', 'A', 'B', 'C'},
					   {'3', 'D', 'E', 'F'},
					   {'4', 'G', 'H', 'I'},
					   {'5', 'J', 'K', 'L'},
					   {'6', 'M', 'N', 'O'},
					   {'7', 'P', 'R', 'S'},
					   {'8', 'T', 'U', 'V'},
					   {'9', 'W', 'X', 'Y'}};

    char KeyPad2[10][10]={{'0', '~', '!', '@', '#', '$', '%', '^', '&', '.'},
					   {'1', 'Q', 'Z', '=', 'q', 'z', '+', '[', ']', '1'},
					   {'2', 'A', 'B', 'C', 'a', 'b', 'c', '(', ')', '2'},
					   {'3', 'D', 'E', 'F', 'd', 'e', 'f', '{', '}', '3'},
					   {'4', 'G', 'H', 'I', 'g', 'h', 'i', '<', '>', '4'},
					   {'5', 'J', 'K', 'L', 'j', 'k', 'l', '|', '5', '5'},
					   {'6', 'M', 'N', 'O', 'm', 'n', 'o', '6', '6', '6'},
					   {'7', 'P', 'R', 'S', 'p', 'r', 's', '7', '7', '7'},
					   {'8', 'T', 'U', 'V', 't', 'u', 'v', '?', '8', '8'},
					   {'9', 'W', 'X', 'Y', 'w', 'x', 'y', '9', '9', '9'}};

	char KeyPad3[10][4]={{'0', '@', '.', '_'},
		               {'q', 'z', '1', 'q'},
		               {'a', 'b', 'c', '2'},
					   {'d', 'e', 'f', '3'},
					   {'g', 'h', 'i', '4'},
					   {'j', 'k', 'l', '5'},
					   {'m', 'n', 'o', '6'},
					   {'p', 'r', 's', '7'},
					   {'t', 'u', 'v', '8'},
					   {'w', 'x', 'y', '9'}};

	memset(szAmount, 0x00, sizeof(szAmount));
	
	CTOS_TimeOutSet(TIMER_ID_1, 100);
	
	while(1)
	{	
		memset(szTemp, 0x00, sizeof(szTemp));  
         
        clearLine(usY);
		//clearLine(usY+1);
        //CTOS_LCDTPrintXY(40-((strlen(szAmount)*2)+inAtCtr), usY, szAmount);
		
		if(strlen(szAmount)>20)
		{
			memset(szTemp, 0x00, sizeof(szTemp));
			memset(szTemp2, 0x00, sizeof(szTemp2));
			memcpy(szTemp, szAmount, 20);
			strcpy(szTemp2, szAmount+20);
		    setLCDPrint(usY, DISPLAY_POSITION_LEFT, szTemp);	
		    setLCDPrint(usY+1, DISPLAY_POSITION_LEFT, szTemp2);
		}
		else
			setLCDPrint(usY, DISPLAY_POSITION_LEFT, szAmount);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
            if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                inLastKey=255;
			
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}
        vdDebug_LogPrintf("key: (%c)", inRet);
		if(inRet >= 48 && inRet <= 57)
		{
			char szTemp2[24+1];
			
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%c", inRet);
			inKey=atoi(szTemp);
			
			CTOS_TimeOutSet(TIMER_ID_1, 100);
			if((inCtr < inMax) || (inLastKey == inKey))
			{
                vdDebug_LogPrintf("1. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                if(inLastKey == inKey)
                {
					inKey2++;
					if(bInputMode == 0x04)
					{
					    if(inKey2 > 9)
                            inKey2=0;
					}
                    else
                    {
                        if(inKey2 > 3)
                        inKey2=0;
                    }
                }
                else
					inKey2=0;
				                
                if(inLastKey == inKey)
                    inCtr--;

				if(bInputMode == 0x04)
				{
					szAmount[inCtr]=KeyPad2[inKey][inKey2];
					if(szAmount[inCtr] == '@')
						inAtCtr++;
				}
				else if(bInputMode == 0x05)
				{
					szAmount[inCtr]=KeyPad3[inKey][inKey2];
					if(szAmount[inCtr] == '@') /*@ symbol is occupying 2 spaces*/
						inAtCtr++;
				}
				else
					szAmount[inCtr]=KeyPad[inKey][inKey2];
				inCtr++;

                vdDebug_LogPrintf("2. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
                vdDebug_LogPrintf("szAmount: (%s)", szAmount);
                				
                inLastKey=inKey;
                if(bInputMode != 0x04 && bInputMode != 0x05)
                {
                    if(inKey == 0)
				        inLastKey=255;
                }
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_KBD_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{			
			if(strlen(szAmount) >= usMinLen)
			{
				memcpy(pbaStr, szAmount, strlen(szAmount));
				*usStrLen=strlen(szAmount);
				return d_KBD_ENTER;
			}
			
			if(usMinLen == 0)
                return d_KBD_ENTER;			
		}
		else if(inRet == 81) /*dot key*/
		{	
			if(bInputMode == 0x04 || bInputMode == 0x05)
			{
                szAmount[inCtr]='.';
			    inCtr++;
			    inKey2=0;
			    inLastKey=255;
			}
		}
		else if(inRet == 82) /*clear key*/
		{	
            if(bInputMode == 0x04 || bInputMode == 0x05)
            {
                if(szAmount[inCtr] == '@')
                    inAtCtr--;			
            }
			
			inCtr--;
			if(inCtr <= 0)
                inCtr=0;
            szAmount[inCtr]=0x00;
			inKey2=0;
			inLastKey=255;
		}
	}
}

BYTE InputAmount3(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    char szTemp[24+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[24+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	
	memset(szAmount, 0x00, sizeof(szAmount));

    if(first_key != 0x00)
    {
        szAmount[0]=first_key;
		inCtr=1;
    }
	
	while(1)
	{
		memset(szTemp, 0x00, sizeof(szTemp));         	 	
		if(strlen(szAmount) > 0)
		{
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmount, szTemp);
		}
		else
		{
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", "0", szTemp);
		}

 
        clearLine(usY);
        //CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
        int x=0;
        int len, index;
        len=strlen(szTemp);
        for(index=0; index < len; index++)
        {
            if(szTemp[index] == '.')
                x+=1;
            else
                x+=2;
        }
        
        CTOS_LCDTPrintXY(usX, usY, szTemp);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				vdDebug_LogPrintf("INSIDE isKey");
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			/*
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}*/
		}

		if(inRet >= 48 && inRet <= 57)
		{
			vdDebug_LogPrintf("INSIDE 48 and 57");
			if(inCtr < 9)
			{
			    memset(szTemp, 0x00, sizeof(szTemp));
			    sprintf(szTemp, "%c", inRet);
                strcat(szAmount, szTemp);			
                inCtr++; 		
				if(inCtr == 1 && szAmount[0] == 48)
				{
					memset(szAmount, 0x00, sizeof(szAmount));
					inCtr=0;
				}
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_USER_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{
			if(strlen(szAmount) > 0)
			{
				memcpy(baAmount, szAmount, strlen(szAmount));
				return d_OK;
			}
			else
			{
				if(bIgnoreEnter == 1)
					return d_OK;			
			}
		}
		else if(inRet == 82) /*clear key*/
		{		
			if(inCtr > 0)
                inCtr--;
            szAmount[inCtr]=0x00;
		}
	}
}

USHORT ushCTOS_printReceipt_Diag(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	ULONG ulFileSize;

	vdDebug_LogPrintf("--ushCTOS_printReceipt_Diag--");
	
    if( printCheckPaper()==-1)
    	return -1;

	if (isCheckTerminalMP200() == d_OK)
	{
		if (srTransRec.byTransType == LOY_BAL_INQ)
		return (d_OK);
	}

	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);

	vdCTOS_PrintReceiptCopies(FALSE, FALSE, TRUE, TRUE);
	
	inCTOSS_ERM_Form_Receipt(0);
	
    return (d_OK);
}



USHORT ushCTOS_PrintServiceBody(int page, int inPurpose)
{	
    char szStr[d_LINE_SIZE + 1];
    char szStr2[d_LINE_SIZE + 1];	
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
	CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];
	BOOL fERMChangeFlag = FALSE;
	int inHost1 = 91;
	char szTID1[10] = "00055555";
	char szMID1[20] = "000000000005555";
	int inHost2 = 92;
	char szTID2[10] = "00022222";
	char szMID2[20] = "000000000002222";
	int inHost3 = 93;
	char szTID3[10] = "00033333";
	char szMID3[20] = "000000000003333";
	int inHost4 = 94;
	char szTID4[10] = "00044444";
	char szMID4[20] = "000000000004444";
	char szTID5[10] = "00077777";
	char szMID5[20] = "000000000007777";

	vdDebug_LogPrintf("--ushCTOS_PrintServiceBody--");
	vdDebug_LogPrintf("page[%d].inPurpose[%d].fCaptureCopy[%d]", page, inPurpose, fCaptureCopy);
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	
	if( printCheckPaper()==-1)
        return d_NO;
	
	if(inPurpose != NEGATIVE)
	{
		if(fERMTransaction == TRUE)//DIsable sending of receipt to ERM for Engineer's copy.
		{
			if(page == ENGINEER_COPY_RECEIPT)
			{
				fERMChangeFlag = TRUE;
				fERMTransaction = FALSE;
			}
		}

	}
	
	srTransRec.HDTid = strHDT.inHostIndex = 19;

	inMMTReadRecord(srTransRec.HDTid,1);
	strcpy(srTransRec.szTID, strMMT[0].szTID);
	strcpy(srTransRec.szMID, strMMT[0].szMID);
	memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
	DebugAddHEX("szBatchNo",srTransRec.szBatchNo,4);

	
	vdCTOSS_PrinterStart(200);
    CTOS_PrinterSetHeatLevel(4);
	
	ushCTOS_PrintHeader(page);
	vdCTOS_PrinterFline(1);	
	
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    	
	memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
	memset(szTemp2, ' ', d_LINE_SIZE);
    memset(szTemp3, ' ', d_LINE_SIZE);
	
    inResult = printTIDMID(); 
	
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
	wub_str_2_hex(szCurrentTime,srTransRec.szDate,DATE_ASC_SIZE);
	
	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,srTransRec.szTime,TIME_ASC_SIZE);

    printDateTime(FALSE);

	vdCTOS_PrinterFline(1);	
	if(inPurpose == INSTALLATION)
		vdPrintTitleCenter("INSTALLATION REPORT");
	else if(inPurpose == PULL_OUT)
		vdPrintTitleCenter("PULL-OUT REPORT");
	else if(inPurpose == REPROGRAMMING)
		vdPrintTitleCenter("REPROGRAMMING REPORT");
	else if(inPurpose == NEGATIVE)
		vdPrintTitleCenter(strService.szNegativeTitle);
	else if(inPurpose == HARDWARE_TEST)
		vdPrintTitleCenter("HARDWARE DIAG REPORT");
	else if(inPurpose == SOFTWARE_TEST)
		vdPrintTitleCenter("SOFTWARE DIAG REPORT");
	
	vdCTOS_PrinterFline(1);	
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);


	if(inPurpose == INSTALLATION && fEducationalCheckList == TRUE)
	{
		
		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[0],szStr2);
		sprintf(szStr,"I. PROPER CARD ACCEPTANCE                 %s ",szStr2);
		inPrint(szStr);
		inPrint("   PROCEDURE (5 STEPS)");

		vdCTOS_PrinterFline(1);

		inPrint("II. FRAUD ORIENTATION");
                   inPrint("(DETECTION PROCEDURE)");

		vdSetMerchantResponseToTxt(inAnswer[1],szStr2);
		sprintf(szStr,"     A. FRAUD TYPES                       %s ",szStr2);
		inPrint(szStr);
		inPrint("        (LOST/SKIMMING/ATO/NRI)");

		vdSetMerchantResponseToTxt(inAnswer[2],szStr2);
		sprintf(szStr,"     B. SUSPICIOUS BEHAVIOR               %s ",szStr2);
		inPrint(szStr);

		vdSetMerchantResponseToTxt(inAnswer[3],szStr2);
		sprintf(szStr,"     C. SECURITY FEATURES OF              %s ",szStr2);
		inPrint(szStr);
		inPrint("        VISA/MASTERCARD");

		vdSetMerchantResponseToTxt(inAnswer[4],szStr2);
		sprintf(szStr,"     D. CODE 10 AUTHORIZATION             %s ",szStr2);
		inPrint(szStr);	

                  vdCTOS_PrinterFline(1);
				   
		inPrint("II. POS FEATURES AND PROCEDURES");
		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		
		vdSetMerchantResponseToTxt(inAnswer[5],szStr2);
		sprintf(szStr,"     A. HOW TO VOID SALE                  %s ",szStr2);
		inPrint(szStr);
		inPrint("        (REVERSAL/CANCELLATION)");

		//vdCTOS_PrinterFline(1);	

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[6],szStr2);
		sprintf(szStr,"     B. HOW TO REPRINT TRANSACTIONS       %s ",szStr2);
		inPrint(szStr);
		inPrint("        AND SETTLEMENTS");

		//vdCTOS_PrinterFline(1);	

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[7],szStr2);
		sprintf(szStr,"     C. HOW TO DO SETTLEMENT              %s ",szStr2);
		inPrint(szStr);

		//vdCTOS_PrinterFline(1);	

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[8],szStr2);
		sprintf(szStr,"     D. PRE-AUTHORIZATION                 %s ",szStr2);
		inPrint(szStr);

		//vdCTOS_PrinterFline(1);	

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[9],szStr2);
		sprintf(szStr,"     E. HOW TO DO OFFLINE SALES           %s ",szStr2);
		inPrint(szStr);

		//vdCTOS_PrinterFline(1);	

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[10],szStr2);
		sprintf(szStr,"     F. INSTALLMENT TRANSACTIONS          %s ",szStr2);
		inPrint(szStr);

		vdCTOS_PrinterFline(1);

		inPrint("IV. TRANSACTIONS /PAYMENTS");

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[11],szStr2);
		sprintf(szStr,"     A. UNDERPAYMENT/OVERPAYMENT          %s ",szStr2);
		inPrint(szStr);
                 
                  memset(szStr,0x00,sizeof(szStr));
                  memset(szStr2,0x00,sizeof(szStr2));
                  vdSetMerchantResponseToTxt(inAnswer[12],szStr2);
                  sprintf(szStr,"     B. PAYMENT VIA MBTC DEPOSIT          %s ",szStr2);
                  inPrint(szStr);
	         inPrint("        OR CHECK DELIVERY");

		memset(szStr,0x00,sizeof(szStr));
                  memset(szStr2,0x00,sizeof(szStr2));
                  vdSetMerchantResponseToTxt(inAnswer[13],szStr2);
                  sprintf(szStr,"     C. CHARGEBACK                        %s ",szStr2);
                  inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
                  memset(szStr2,0x00,sizeof(szStr2));
                  vdSetMerchantResponseToTxt(inAnswer[14],szStr2);
                  sprintf(szStr,"     D. SALES SLIP RETRIEVAL              %s ",szStr2);
                  inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
                  memset(szStr2,0x00,sizeof(szStr2));
                  vdSetMerchantResponseToTxt(inAnswer[15],szStr2);
                  sprintf(szStr,"     E. HOW TO PROCESS TIPS               %s ",szStr2);
                  inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
                  memset(szStr2,0x00,sizeof(szStr2));
                  vdSetMerchantResponseToTxt(inAnswer[16],szStr2);
                  sprintf(szStr,"     F. HOW TO PROCESS MANUAL             %s ",szStr2);
                  inPrint(szStr);
		inPrint("        TRANSACTIONS");

		vdCTOS_PrinterFline(1);	

		inPrint("V. TELEPHONE NUMBERS TO CALL");
		
		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[17],szStr2);
		sprintf(szStr,"     A. POS PROBLEMS/COMPLAINS            %s ",szStr2);
		inPrint(szStr);

		//vdCTOS_PrinterFline(1);	

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[18],szStr2);
		sprintf(szStr,"     B. SUPPLIES - THERMAL ROLLS          %s ",szStr2);
		inPrint(szStr);
		inPrint("        SALES SLIPS, ETC.");

		vdCTOS_PrinterFline(1);	

		inPrint("VI. BIN VERIFICATION");

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[19],szStr2);
		sprintf(szStr,"     A. BINVER PROCEDURE                  %s ",szStr2);
		inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[20],szStr2);
		sprintf(szStr,"     B. BINVER THRESHOLDS /AMOUNTS        %s ",szStr2);
		inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[21],szStr2);
		sprintf(szStr,"     C. BINVER AGREEMENT                  %s ",szStr2);
		inPrint(szStr);

		vdCTOS_PrinterFline(1);	

		inPrint("VII. OTHERS");

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[22],szStr2);
		sprintf(szStr,"     A. SPLIT SALES                       %s ",szStr2);
		inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[23],szStr2);
		sprintf(szStr,"     B. DOUBLE BILLING                    %s ",szStr2);
		inPrint(szStr);

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[24],szStr2);
		sprintf(szStr,"     C. SALES DRAFT LAUNDERING            %s ",szStr2);
		inPrint(szStr);
                   inPrint("        /MULTIPLE SWIPING");

		memset(szStr,0x00,sizeof(szStr));
		memset(szStr2,0x00,sizeof(szStr2));
		vdSetMerchantResponseToTxt(inAnswer[25],szStr2);
		sprintf(szStr,"     D. REFER                             %s ",szStr2);
		inPrint(szStr);
		
	}

	vdCTOS_PrinterFline(2);	
	
	if(inPurpose == INSTALLATION || inPurpose == HARDWARE_TEST || inPurpose == SOFTWARE_TEST)
	{
		if(inPurpose != SOFTWARE_TEST)
		{
			strcpy(szStr,"HARDWARE DIAGNOSTICS");
			
			if(strHWDiag.inHWErrorCount > 0)
			{
				sprintf(szTemp,"%d %s",strHWDiag.inHWErrorCount,strHWDiag.inHWErrorCount>1?"ERRORS":"ERROR");
				inPrintLeftRight(szStr,szTemp,46);

				if(strHWDiag.inLCD_Status == FAIL)
					inPrint("  LCD");
				
				if(strHWDiag.inPrinter_Status == FAIL)
					inPrint("  PRINTER");
				
				if(strHWDiag.inCTLSReader_Status == FAIL)
					inPrint("  CTLS READER");

				if(strHWDiag.inMSReader_Status== FAIL)
					inPrint("  MSR");

				if(strHWDiag.inICCReader_Status == FAIL)
					inPrint("  ICC READER");

				if(strHWDiag.inCOMMS_Status == FAIL)
				{
					switch(strCPT.inCommunicationMode)
					{
						case ETHERNET_MODE:
							inPrint("  ETH");
							break;
						
						case DIAL_UP_MODE:
							inPrint("  DIAL");
							break;

						case GPRS_MODE:
							inPrint("  GPRS");
							break;

						case WIFI_MODE:
							inPrint("  WIFI");
							break;
						default:
				        break;
				    }

				}
			}
			else
				inPrintLeftRight(szStr,"COMPLETE",46);
		}

		if(inPurpose != HARDWARE_TEST)
		{
			memset(szStr, ' ', d_LINE_SIZE);
		    memset(szTemp, ' ', d_LINE_SIZE);

			strcpy(szStr,"SOFTWARE DIAGNOSTICS");
			
			if(strSWDiag.inSWErrorCount > 0)
			{
				sprintf(szTemp,"%d %s",strSWDiag.inSWErrorCount,strSWDiag.inSWErrorCount>1?"ERRORS":"ERROR");
				inPrintLeftRight(szStr,szTemp,46);

			if(strSWDiag.inCredit_Status != SUCCESS)
			inPrint("  CREDIT");
	
			if(get_env_int("TEST_INSTALLMENT") ==1)
			{
				if(strSWDiag.inInst_Status != SUCCESS)
					inPrint("  INSTALLMENT");
			}
			if(get_env_int("TEST_BANCNET") ==1)
			{
				if(strSWDiag.inBancnet_Status != SUCCESS)
					inPrint("  BANCNET");
			}

			}
			else
				inPrintLeftRight(szStr,"COMPLETE",46);
		}
		vdCTOS_PrinterFline(2);	
	}
	
	memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	memset(szTemp2, ' ', d_LINE_SIZE);
	memset(szTemp3, ' ', d_LINE_SIZE);
	
	sprintf(szStr,  "UNIT MODEL     : %s",strService.szUnitModel);
	inPrint(szStr);

	sprintf(szTemp, "UNIT SN        : %s",strService.szUnitSN);
	inPrint(szTemp);
	
         if( srTransRec.byTransType != MERCHANT_SERVICES)
         {
              sprintf(szTemp1,"PWR SN         : %s",strService.szPowerSN);
              inPrint(szTemp1);
              
              sprintf(szTemp2,"SIM SN         : %s",strService.szSIMSN);
              inPrint(szTemp2);
              
              sprintf(szTemp2,"DOCK SN        : %s",strService.szDockSN);
              inPrint(szTemp2);
         }

	if(inPurpose == INSTALLATION)
	{
		sprintf(szTemp3,"ORIENTED       : %s",strService.fOriented?"YES":"N0");
		inPrint(szTemp3);
	}
	else if(inPurpose == PULL_OUT || inPurpose == REPROGRAMMING || inPurpose == NEGATIVE)
	{
		sprintf(szTemp3,"REASON         : %s",strService.szReason);
		inPrint(szTemp3);
	}
	
	vdCTOS_PrinterFline(1);	

	if(fERMUploadFailed == TRUE && (page == ENGINEER_COPY_RECEIPT || MERCHANT_COPY_RECEIPT))
	{
		fERMTransaction = FALSE;
		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		vdPrintCenter("** NO CONNECTIVITY **");
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		fERMTransaction = TRUE;
	}

	vdCTOS_PrinterFline(1);	
	
	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	
	if(inPurpose == SOFTWARE_TEST)
	{
		wub_hex_2_str(strSWDiag.szSWStartTime, szTemp,TIME_BCD_SIZE);
		wub_hex_2_str(strSWDiag.szSWEndTime, szTemp2,TIME_BCD_SIZE);		
	}
	else if(inPurpose == HARDWARE_TEST)
	{
		wub_hex_2_str(strHWDiag.szHWStartTime, szTemp,TIME_BCD_SIZE);
		wub_hex_2_str(strHWDiag.szHWEndTime, szTemp2,TIME_BCD_SIZE);
	}
	else
	{
		wub_hex_2_str(strService.szSvcStartTime, szTemp,TIME_BCD_SIZE);
		wub_hex_2_str(strService.szSvcEndTime, szTemp2,TIME_BCD_SIZE);		
	}

	wub_hex_2_str(strService.szArrivalTime,szTemp3,TIME_BCD_SIZE);
	
	sprintf(szTemp3,"TIME ARRIVED  : %02lu:%02lu:%02lu", atol(szTemp3)/10000,atol(szTemp3)%10000/100, atol(szTemp3)%100);
	sprintf(szTemp, "TIME STARTED  : %02lu:%02lu:%02lu", atol(szTemp)/10000,atol(szTemp)%10000/100, atol(szTemp)%100);
	sprintf(szTemp2,"TIME COMPLETED: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);

	if(strlen(strService.szArrivalTime) > 0) //Only print TIME ARRIVED if engineer visits the merchant site.
		inPrint(szTemp3);

	inPrint(szTemp);
	inPrint(szTemp2);

	vdCTOS_PrinterFline(1);	
/*	
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szTemp, "MERCHANT NAME : %s", strService.szCustName);
	sprintf(szTemp1,"CONTACT NO    : %s", strService.szCustPhoneNo);
	inPrint(szTemp);
	inPrint(szTemp1);
*/
	//vdCTOS_PrinterFline(1);	
	
	//vdCTOS_PrinterFline(2);	
	memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	memset(szTemp2, ' ', d_LINE_SIZE);
	memset(szTemp3, ' ', d_LINE_SIZE);
	memset(szTemp4, ' ', d_LINE_SIZE);
	memset(szTemp5, ' ', d_LINE_SIZE);

	if(strlen(strService.szFEName) > 0)
	{
		sprintf(szTemp3,"FIELD ENGINEER: %s", strService.szFEName);
		inPrint(szTemp3);
	}
	else
		inPrint("FIELD ENGINEER: N/A");
	
	if(page == ENGINEER_COPY_RECEIPT)
	{
		sprintf(szStr,  "LOC ORIG      : %s", strService.szOrigin);
		inPrint(szStr);
		
		sprintf(szTemp, "LOC DEST      : %s", strService.szDestination);
		inPrint(szTemp);

		memset(szTemp1,0x00,sizeof(szTemp1));
		vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strService.szFare, szTemp1);
		sprintf(szTemp2,"FARE          : %s", szTemp1);
		inPrint(szTemp2);
		
		memset(szTemp3,0x00,sizeof(szTemp3));
		vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strService.szAccomodation, szTemp3);
		sprintf(szTemp4,"ACCOM         : %s", szTemp3);
						
		inPrint(szTemp4);
		
		memset(szStr, ' ', d_LINE_SIZE);
		
		memset(szTemp5,0x00,sizeof(szTemp5));
		vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strService.szTotalSvcExpense, szTemp5); 
		sprintf(szStr,  "TOTAL         : %s", szTemp5);
		inPrint(szStr);
	}

	vdCTOS_PrinterFline(1);	

	if(ushCTOS_ePadPrintSignature() != d_OK)
	{
		vdCTOS_PrinterFline(2);
	}

	//vdCTOS_PrinterFline(1); 

	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	
	if(page == ENGINEER_COPY_RECEIPT)
	{
		inPrint("FE SIGN:__________________________________");
		vdCTOS_PrinterFline(1);
		#if 0
		sprintf(szTemp,"FE NAME: %s",strService.szFEName);
		inPrint(szTemp);

		sprintf(szTemp1,"CONTACT NO: %s", strService.szCustPhoneNo);
		inPrint(szTemp1);
	         #endif
		vdCTOS_PrinterFline(1);
		vdPrintCenter("***** ENGINEER COPY *****");	
		vdCTOS_PrinterFline(1);
	}	
	else if(page == MERCHANT_COPY_RECEIPT || page == BANK_COPY_RECEIPT)
	{
	         if (gblinWitnessSign != 1)
		     inPrint("MERCHANT SIGN:____________________________");
		else
		      inPrint("WITNESS SIGN:____________________________");
		vdCTOS_PrinterFline(1);
		if (gblinWitnessSign != 1)
		     sprintf(szTemp,"MERCHANT NAME: %s",strService.szCustName);
		else
		     sprintf(szTemp,"WITNESS NAME: %s",strService.szCustName);
		inPrint(szTemp);

		sprintf(szTemp1,"CONTACT NO: %s", strService.szCustPhoneNo);
		inPrint(szTemp1);
		
		vdCTOS_PrinterFline(1);

		if(page == MERCHANT_COPY_RECEIPT)
			vdPrintCenter("***** MERCHANT COPY *****");
		else
			vdPrintCenter("***** BANK COPY *****");
		
		vdCTOS_PrinterFline(1);
	}
	else
	{
		vdPrintCenter(strService.szCustName);
		vdCTOS_PrinterFline(1);
		vdPrintCenter("***** BANK COPY *****");
		vdCTOS_PrinterFline(1);
	}
	
	vdCTOS_PrinterFline(2);	

	// Capture copy for receipt upload -- sidumili
	if (fCaptureCopy && page == MERCHANT_COPY_RECEIPT)
	{	
		vdDebug_LogPrintf("Capturing erm receipt...");
		
		if (srTransRec.byTransType == CITAS_SERVICES)
			vdSetFSRMode(CITAS_SERVICES);

		if (srTransRec.byTransType == MERCHANT_SERVICES)
			vdSetFSRMode(MERCHANT_SERVICES);
		
		vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");

		vdDebug_LogPrintf("BEFORE inCTOSS_ERM_Form_Receipt");
		if(strTCT.byERMMode != 0 && page != ENGINEER_COPY_RECEIPT && page != BANK_COPY_RECEIPT)
		{
			
			srTransRec.MITid=1;
			if(inPurpose == INSTALLATION)
			{
				strcpy(strService.szServiceID,szMID1);
			}
			else if(inPurpose == PULL_OUT)
			{
				strcpy(strService.szServiceID,szMID2);
			}
			else if(inPurpose == REPROGRAMMING)
			{
				strcpy(strService.szServiceID,szMID3);
			}
			else if(inPurpose == NEGATIVE)
			{
				strcpy(strService.szServiceID,szMID4);
			}
			else
			{
				strcpy(strService.szServiceID,szMID5);
			}
			
			inCTOSS_ERM_Form_Receipt_Diag(FALSE,inPurpose);
		}
	}
	else
	{
		vdCTOSS_PrinterEnd();
	}
		
	if(fERMChangeFlag  == TRUE)
		fERMTransaction = TRUE;
		
    return d_OK;	
}

USHORT ushCTOS_PrintScheduledDiagBody(void)
{	
    char szStr[d_LINE_SIZE + 1];
    char szStr2[d_LINE_SIZE + 1];	
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
	CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];
	BOOL fERMChangeFlag = FALSE;
	int inHost1 = 91;
	char szTID1[10] = "00011111";
	char szMID1[20] = "000000000001111";
	int inHost2 = 92;
	char szTID2[10] = "00022222";
	char szMID2[20] = "000000000002222";
	int inHost3 = 93;
	char szTID3[10] = "00033333";
	char szMID3[20] = "000000000003333";
	int inHost4 = 94;
	char szTID4[10] = "00044444";
	char szMID4[20] = "000000000004444";
	char szTID5[10] = "00077777";
	char szMID5[20] = "000000000007777";
	
	srTransRec.HDTid = strHDT.inHostIndex = 19;

	inMMTReadRecord(srTransRec.HDTid,1);
	strcpy(srTransRec.szTID, strMMT[0].szTID);
	strcpy(srTransRec.szMID, strMMT[0].szMID);
	memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
	DebugAddHEX("szBatchNo",srTransRec.szBatchNo,4);

	
	vdCTOSS_PrinterStart(200);
    CTOS_PrinterSetHeatLevel(4);
	
	ushCTOS_PrintHeader(0);
	vdCTOS_PrinterFline(1);	
	
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    	
	memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
	memset(szTemp2, ' ', d_LINE_SIZE);
    memset(szTemp3, ' ', d_LINE_SIZE);
	
    inResult = printTIDMID(); 
	
	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
	wub_str_2_hex(szCurrentTime,srTransRec.szDate,DATE_ASC_SIZE);
	
	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,srTransRec.szTime,TIME_ASC_SIZE);

    printDateTime(FALSE);

	vdCTOS_PrinterFline(1);	

	vdPrintTitleCenter("SCHEDULED DIAGNOSTICS");
		
	vdCTOS_PrinterFline(1);	
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	
	strcpy(szStr,"HARDWARE DIAGNOSTICS");
	
	if(strHWDiag.inHWErrorCount > 0)
	{
		sprintf(szTemp,"%d %s",strHWDiag.inHWErrorCount,strHWDiag.inHWErrorCount>1?"ERRORS":"ERROR");
		inPrintLeftRight(szStr,szTemp,46);

		if(strHWDiag.inLCD_Status == FAIL)
			inPrint("  LCD");
		
		if(strHWDiag.inPrinter_Status == FAIL)
			inPrint("  PRINTER");
		
		if(strHWDiag.inCTLSReader_Status == FAIL)
			inPrint("  CTLS READER");
#if 0
		if(strHWDiag.inMSReader_Status== FAIL)
			inPrint("  MSR");

		if(strHWDiag.inICCReader_Status == FAIL)
			inPrint("  ICC READER");
#endif
		if(strHWDiag.inCOMMS_Status == FAIL)
		{
			switch(strCPT.inCommunicationMode)
			{
				case ETHERNET_MODE:
					inPrint("  ETH");
					break;
				
				case DIAL_UP_MODE:
					inPrint("  DIAL");
					break;

				case GPRS_MODE:
					inPrint("  GPRS");
					break;

				case WIFI_MODE:
					inPrint("  WIFI");
					break;
				default:
		        break;
		    }

		}
	}
	else
		inPrintLeftRight(szStr,"COMPLETE",46);



	memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);

	strcpy(szStr,"SOFTWARE DIAGNOSTICS");
	
	if(strSWDiag.inSWErrorCount > 0)
	{
		sprintf(szTemp,"%d %s",strSWDiag.inSWErrorCount,strSWDiag.inSWErrorCount>1?"ERRORS":"ERROR");
		inPrintLeftRight(szStr,szTemp,46);

		if(strSWDiag.inCredit_Status != SUCCESS)
			inPrint("  CREDIT");

		if(get_env_int("TEST_INSTALLMENT") ==1)
                  {
                       if(strSWDiag.inInst_Status != SUCCESS)
                            inPrint("  INSTALLMENT");
                  }
		
		if(get_env_int("TEST_BANCNET") ==1)
                  {
                       if(strSWDiag.inBancnet_Status != SUCCESS)
                            inPrint("  BANCNET");
                  }

	}
	else
		inPrintLeftRight(szStr,"COMPLETE",46);
	

	vdCTOS_PrinterFline(1);	
	
	if(fERMUploadFailed == TRUE)
	{	
		fERMTransaction = FALSE;
		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		vdPrintCenter("** NO CONNECTIVITY **");
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		vdCTOS_PrinterFline(1);	
		fERMTransaction = TRUE;
	}
	
	memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	memset(szTemp2, ' ', d_LINE_SIZE);
	memset(szTemp3, ' ', d_LINE_SIZE);

	inTCTRead(1);
	strcpy(strService.szUnitModel,strTCT.szUnitModel);

	memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum)); 
    CTOS_GetFactorySN(szTermSerialNum); 	
    //usGetSerialNumber(szTermSerialNum);
    szTermSerialNum[15]=0;
    strcpy(strService.szUnitSN,szTermSerialNum);
	
	sprintf(szStr,  "UNIT MODEL     : %s",strService.szUnitModel);
	inPrint(szStr);

	sprintf(szTemp, "UNIT SN        : %s",strService.szUnitSN);
	inPrint(szTemp);

	vdCTOS_PrinterFline(2);	
	
	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	
	wub_hex_2_str(strHWDiag.szHWStartTime, szTemp,TIME_BCD_SIZE);
	wub_hex_2_str(strSWDiag.szSWEndTime, szTemp2,TIME_BCD_SIZE);	

	sprintf(szTemp, "TIME STARTED  : %02lu:%02lu:%02lu", atol(szTemp)/10000,atol(szTemp)%10000/100, atol(szTemp)%100);
	sprintf(szTemp2,"TIME COMPLETED: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);

	inPrint(szTemp);
	inPrint(szTemp2);


	vdCTOS_PrinterFline(2);	

	vdPrintCenter("***** MERCHANT COPY *****");

	vdCTOS_PrinterFline(5);	

	 vdCTOSS_PrinterEnd();
	 
	if(strTCT.byERMMode != 0)
	{
#if 0		
		srTransRec.MITid=1;
		if(inPurpose == INSTALLATION)
		{
			strcpy(srTransRec.szTID,szTID1);
			strcpy(srTransRec.szMID,szMID1);
			strHDT.inHostIndex = inHost1;
		}
		else if(inPurpose == PULL_OUT)
		{
			strcpy(srTransRec.szTID,szTID2);
			strcpy(srTransRec.szMID,szMID2);
			strHDT.inHostIndex = inHost2;
		}
		else if(inPurpose == REPROGRAMMING)
		{
			strcpy(srTransRec.szTID,szTID3);
			strcpy(srTransRec.szMID,szMID3);
			strHDT.inHostIndex = inHost3;
		}
		else if(inPurpose == NEGATIVE)
		{
			strcpy(srTransRec.szTID,szTID4);
			strcpy(srTransRec.szMID,szMID4);
			strHDT.inHostIndex = inHost4;
		}
		else
		{
			strcpy(srTransRec.szTID,"10000001");
			strcpy(srTransRec.szMID,"100000000000001");
			strHDT.inHostIndex = 19;
		}
#else
		srTransRec.MITid=1;
		strcpy(strService.szServiceID,szMID5);
#endif		

		inCTOSS_ERM_Form_Receipt_Diag(FALSE,SOFTWARE_HARDWARE_TEST);
	}
		
	if(fERMChangeFlag  == TRUE)
		fERMTransaction = TRUE;
		
    return d_OK;	
}

int inCTOSS_FSR_ERMInitAllHost(void)
{
	int inResult = d_NO;

	vdDebug_LogPrintf("--inCTOSS_FSR_ERMInitAllHost--");
	vdDebug_LogPrintf("byERMMode[%d]..byERMInit[%d]",strTCT.byERMMode, strTCT.byERMInit);
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;
	
	vdDebug_LogPrintf("inCTOSS_ERMInit..byERMInit=[%d]",strTCT.byERMInit);
	if(strTCT.byERMInit == 1)
		return ST_SUCCESS;

	CTOS_LCDTClearDisplay();
    vdDispTitleString("ERM INIT");	
							
	inResult = inCTOS_FSR_ERMAllHosts_Initialization(TRUE);

	if (inResult != d_OK)
		vdDisplayMultiLineMsgAlign("ALL HOST", "INITIALIZE ERM ", "FAILED", DISPLAY_POSITION_CENTER);
	else
		vdDisplayMultiLineMsgAlign("ALL HOST", "INITIALIZE ERM ", "COMPLETE", DISPLAY_POSITION_CENTER);
	
	return ST_SUCCESS;
}

int inCTOS_FSR_ERMAllHosts_Initialization(BOOL fAllHost)
{
	int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	int inNumOfMit = 0,inMitNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;
	BYTE szParaName[100];
	USHORT inExeAPIndex = 0;
	BOOL fInit = TRUE;
	int inHost1 = 91;
	char szTID1[10] = "00011111";
	char szMID1[20] = "000000000001111";
	char szHostLabel1[20] = "INSTALLATION";
	int inHost2 = 92;
	char szTID2[10] = "00022222";
	char szMID2[20] = "000000000002222";
	char szHostLabel2[20] = "PULLOUT";
	int inHost3 = 93;
	char szTID3[10] = "00033333";
	char szMID3[20] = "000000000003333";
	char szHostLabel3[20] = "REPROGRAMMING";
	int inHost4 = 94;
	char szTID4[10] = "00044444";
	char szMID4[20] = "000000000004444";
	char szHostLabel4[20] = "NEGATIVE";
	char szBuff[20];
	
	vdDebug_LogPrintf("--inCTOS_ERMAllHosts_Initialization--");
	vdDebug_LogPrintf("fAllHost[%d]", fAllHost);
	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	//inNumOfHost = inFSRNumRecord();
	inNumOfHost = 4; //FSR ERM hosts starts at 101.
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		{
			//vdDisplayProcessing();

			memset(strHDT.szHostLabel,0x00,sizeof(strHDT.szHostLabel));
			memset(strMMT[0].szTID,0x00,sizeof(strMMT[0].szTID));
			memset(strMMT[0].szMID,0x00,sizeof(strMMT[0].szMID));
			strMMT[0].MITid = 1;
			strcpy(strHDT.szHeaderLogoName,"logo.bmp");
			
			if(inNum == 1)
			{
				strcpy(strHDT.szHostLabel,szHostLabel1);
				strcpy(strMMT[0].szHostName,szHostLabel1);
				strcpy(strMMT[0].szTID,szTID1);
				strcpy(strMMT[0].szMID,szMID1);
				strHDT.inHostIndex = inHost1;
			}
			else if(inNum == 2)
			{
				strcpy(strHDT.szHostLabel,szHostLabel2);
				strcpy(strMMT[0].szHostName,szHostLabel2);
				strcpy(strMMT[0].szTID,szTID2);
				strcpy(strMMT[0].szMID,szMID2);
				strHDT.inHostIndex = inHost2;
			}
			else if(inNum == 3)
			{
				strcpy(strHDT.szHostLabel,szHostLabel3);
				strcpy(strMMT[0].szHostName,szHostLabel3);
				strcpy(strMMT[0].szTID,szTID3);
				strcpy(strMMT[0].szMID,szMID3);
				strHDT.inHostIndex = inHost3;
			}
			else if(inNum == 4)
			{
				strcpy(strHDT.szHostLabel,szHostLabel4);
				strcpy(strMMT[0].szHostName,szHostLabel4);
				strcpy(strMMT[0].szTID,szTID4);
				strcpy(strMMT[0].szMID,szMID4);
				strHDT.inHostIndex = inHost4;
			}
			
			vdDebug_LogPrintf("szHostLabel[%s]",strHDT.szHostLabel);
			
			vdHostToInitialize(FALSE, FALSE);

			setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
			
			vdDebug_LogPrintf("szAPName=[%s]-[%s]-inHostIndex=[%d]---",szAPName,strHDT.szAPName,strHDT.inHostIndex);

			if (!fAllHost)
			{
				if (strcmp(szAPName, strHDT.szAPName)!=0)
				{
					continue;
				}
			}
			
			if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
			{
				continue;
			}

			//inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
			inNumOfMit = 1; //only 1 MIT for FSR per host
			vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
			vdDebug_LogPrintf("MMT inNum=[%d]-----",inNum);
			for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
			{
                //vdDisplayProcessing();
                vdHostToInitialize(FALSE, FALSE);

				setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
				
				//memcpy(&strMMT[0],&strMMT[inMitNum],sizeof(STRUCT_MMT));
				//strcpy(strMMT[0].szTID,strFSR.szTID);
				//strcpy(strMMT[0].szMID,strFSR.szMID);

				vdDebug_LogPrintf("HostName[%s]..MerchantName[%s]..szTID[%s]..szMID=[%s]..", strMMT[0].szHostName, strMMT[0].szMerchantName, strMMT[0].szTID,strMMT[0].szMID);
				
				inCTOSS_ERM_Initialization();

				// Check initialize result
				if (fAllHost)
				{
					memset(szParaName, 0x00, sizeof(szParaName));
					inExeAPIndex = strHDT.inHostIndex;
					sprintf(szParaName,"%s%02d%02d","ERM",inExeAPIndex,strMMT[0].MITid);
					vdDebug_LogPrintf("szParaName =[%s]",szParaName);
					
					if (get_env_int(szParaName) > 0)
						vdHostToInitialize(TRUE, TRUE);
					else
					{
						fInit = FALSE;
						vdHostToInitialize(TRUE, FALSE);
					}
				}
			}
		}		
	}

	vdDebug_LogPrintf("end inCTOS_ERMAllHosts_Initialization-----");

	if (!fAllHost)
		return ST_SUCCESS;
	else
	{
		if (!fInit)
			return d_NO;
		else
			return ST_SUCCESS;
	}
}


int inCTOS_FSR_ERMAllHosts_DeInitialization(void)
{
	int inNumOfHost = 0,inNum;
	int inNumOfMit = 0,inMitNum;
	char szAPName[50+1];
	char szParaName[12+1];
	int inAPPID;
	char szBuff[20];
	int inHost1 = 91;
	char szTID1[10] = "00011111";
	char szMID1[20] = "000000000001111";
	char szHostLabel1[20] = "INSTALLATION";
	int inHost2 = 92;
	char szTID2[10] = "00022222";
	char szMID2[20] = "000000000002222";
	char szHostLabel2[20] = "PULLOUT";
	int inHost3 = 93;
	char szTID3[10] = "00033333";
	char szMID3[20] = "000000000003333";
	char szHostLabel3[20] = "REPROGRAMMING";
	int inHost4 = 94;
	char szTID4[10] = "00044444";
	char szMID4[20] = "000000000004444";
	char szHostLabel4[20] = "NEGATIVE";
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	vdDebug_LogPrintf("--inCTOS_ERMAllHosts_DeInitialization--");
	vdDebug_LogPrintf("byERMMode[%d]",strTCT.byERMMode);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("ERM DE-INIT");
		
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	//inNumOfHost = inFSRNumRecord();
	inNumOfHost = 43;
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		//if(inFSRRead(inNum) == d_OK)
		{
			//memset(strHDT.szHostLabel,0x00,sizeof(strHDT.szHostLabel));
			//strcpy(strHDT.szHostLabel,strFSR.szHostLabel);
			//strHDT.inHostIndex = strFSR.inHostIndex;

			memset(strHDT.szHostLabel,0x00,sizeof(strHDT.szHostLabel));
			memset(strMMT[0].szTID,0x00,sizeof(strMMT[0].szTID));
			memset(strMMT[0].szMID,0x00,sizeof(strMMT[0].szMID));
			strMMT[0].MITid = 1;
			
			if(inNum == 1)
			{
				strcpy(strHDT.szHostLabel,szHostLabel1);
				strcpy(strMMT[0].szHostName,szHostLabel1);
				strcpy(strMMT[0].szTID,szTID1);
				strcpy(strMMT[0].szMID,szMID1);
				strHDT.inHostIndex = 91;
			}
			else if(inNum == 2)
			{
				strcpy(strHDT.szHostLabel,szHostLabel2);
				strcpy(strMMT[0].szHostName,szHostLabel2);
				strcpy(strMMT[0].szTID,szTID2);
				strcpy(strMMT[0].szMID,szMID2);
				strHDT.inHostIndex = 92;
			}
			else if(inNum == 3)
			{
				strcpy(strHDT.szHostLabel,szHostLabel3);
				strcpy(strMMT[0].szHostName,szHostLabel3);
				strcpy(strMMT[0].szTID,szTID3);
				strcpy(strMMT[0].szMID,szMID3);
				strHDT.inHostIndex = 93;
			}
			else if(inNum == 4)
			{
				strcpy(strHDT.szHostLabel,szHostLabel4);
				strcpy(strMMT[0].szHostName,szHostLabel4);
				strcpy(strMMT[0].szTID,szTID4);
				strcpy(strMMT[0].szMID,szMID4);
				strHDT.inHostIndex = 94;
			}
			
			
			vdHostToDeInitialize();

			setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
			
			vdDebug_LogPrintf("szAPName=[%s]-[%s]-inHostIndex=[%d]---",szAPName,strHDT.szAPName,strHDT.inHostIndex);
			if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
			{
				continue;
			}

			inNumOfMit = 1;
			vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
			vdDebug_LogPrintf("MMT inNum=[%d]-----",inNum);
			for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
			{
				vdHostToDeInitialize();

				setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
			
				vdDebug_LogPrintf("HostName[%s]..MerchantName[%s]..szTID[%s]..szMID=[%s]..", strMMT[0].szHostName, strMMT[0].szMerchantName, strMMT[0].szTID,strMMT[0].szMID);
				
				memset(szParaName,0x00,sizeof(szParaName));
				sprintf(szParaName,"%s%02d%02d","ERM",strHDT.inHostIndex,strMMT[0].MITid);
				vdDebug_LogPrintf("szParaName =[%s]",szParaName);
				inCTOSS_PutEnvDB(szParaName,"0");
			}
		}
	}

	inTCTRead(1);
	strTCT.byERMInit=0;
	inTCTSave(1);

	vdDisplayMultiLineMsgAlign("", "DEINITIALIZE ERM", "COMPLETE", DISPLAY_POSITION_CENTER);
	
	vdDebug_LogPrintf("end inCTOS_ERMAllHosts_DeInitialization-----");	
		
	return ST_SUCCESS;
}


void CTOSS_SetRTCEx(void)
{
	//Declare Local Variable //
	CTOS_RTC SetRTC;
	USHORT i = 0;
	BYTE isSet = FALSE;
	BYTE baYear[4+1] = {0},
		baMonth[2+1] = {0},
		baDay[2+1] = {0},
		baHour[2+1] = {0},
		baMinute[2+1] = {0},
		baSecond[2+1]  = {0};
	BYTE key = 0;
	BYTE babuff[d_BUFF_SIZE] = {0};
	BYTE g_DeviceModel;

	CTOS_DeviceModelGet(&g_DeviceModel);
	vdDebug_LogPrintf("CTOS_DeviceModelGet =[%d]",g_DeviceModel);

/*	if (g_DeviceModel != d_MODEL_VEGA5000S)
	CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
*/
	CTOS_LCDTClearDisplay();
	CTOS_LCDTSetReverse(TRUE);
	CTOS_LCDTPrintXY(1, 1, "	 Set RTC	 ");
	CTOS_LCDTSetReverse(FALSE);

	//Read the date and the time //
	CTOS_RTCGet(&SetRTC);

	//Show on the LCD Display //
	CTOS_LCDTPrintXY(1, 2, "   Get	 Set");
	sprintf(babuff,"YY:%04d",SetRTC.bYear + 2000);
	CTOS_LCDTPrintXY(1, 3, babuff);
	sprintf(babuff,"MM:%02d",SetRTC.bMonth);
	CTOS_LCDTPrintXY(1, 4, babuff);
	sprintf(babuff,"DD:%02d",SetRTC.bDay);
	CTOS_LCDTPrintXY(1, 5, babuff);
	sprintf(babuff,"hh:%02d",SetRTC.bHour);
	CTOS_LCDTPrintXY(1, 6, babuff);
	sprintf(babuff,"mm:%02d",SetRTC.bMinute);
	CTOS_LCDTPrintXY(1, 7, babuff);
	sprintf(babuff,"ss:%02d",SetRTC.bSecond);
	CTOS_LCDTPrintXY(1, 8, babuff);
//	  sprintf(babuff,"%02d",SetRTC.MCCW);
//	  CTOS_LCDTPrintXY(15, 8, babuff);

	//Input data for the setting //
	//i = sizeof(baYear);
	i = 5;
	if (InputStringAlphaEx(10,3, 0x05, 0x00, baYear, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Year //
	SetRTC.bYear = (wub_str_2_long(baYear) - 2000);
	isSet = TRUE;
	}
	//i = sizeof(baMonth);
	i = 3;
	if (InputStringAlphaEx(10,4, 0x05, 0x00, baMonth, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){ //Input Month //
	SetRTC.bMonth = wub_str_2_long(baMonth);
	isSet = TRUE;
	}
	//i = sizeof(baDay);
	i = 3;
	if (InputStringAlphaEx(10,5, 0x05, 0x00, baDay, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){ //Input Day //
	SetRTC.bDay = wub_str_2_long(baDay);
	isSet = TRUE;
	}
	//i = sizeof(baHour);
	i = 3;
	if (InputStringAlphaEx(10,6, 0x05, 0x00, baHour, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Hour //
	SetRTC.bHour = wub_str_2_long(baHour);
	isSet = TRUE;
	}
	//i = sizeof(baMinute);
	i = 3;
	if (InputStringAlphaEx(10,7, 0x05, 0x00, baMinute, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Minute //
	SetRTC.bMinute = wub_str_2_long(baMinute);
	isSet = TRUE;
	}
	//i = sizeof(baSecond);
	i = 3;
	if (InputStringAlphaEx(10,8, 0x05, 0x00, baSecond, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Second //
	SetRTC.bSecond = wub_str_2_long(baSecond);
	isSet = TRUE;
	}

	if (isSet){
		//Set the date and time //
		if (CTOS_RTCSet(&SetRTC) == d_OK)
			CTOS_LCDTPrintXY(15, 2, "OK");
		else
			CTOS_LCDTPrintXY(13, 2, "Fail");
		isSet = FALSE;
	}

	WaitKey(10);
//	  CTOS_KBDGet ( &key );

	return;
}

void vdPreventive_Maintenance(void)
{
	int inRet;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	char szHeaderString[50] = "JOB TYPE";
	char szHostMenu[70+1]={"MAINTENANCE\nDAYS GAP"};
	char szTermSerialNum[15+1];

	CTOS_LCDTClearDisplay();

	//inSetColorMenuMode();
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
	inSetTextMode();

	if (key == 0xFF) 
	{
		CTOS_LCDTClearDisplay();
	   //vduiWarningSound();
		return;  
	}

	if(key > 0)
	{
		if (get_env_int("INVALIDKEY") == 1)
			return;
		
		if(key == 1)
			vdSetMaintenance();		
		else if(key == 2)
			vdSetDaysGap();		
		
		return;
	}
}

void vdSetMaintenance(void)
{
	int inInput = 0;
	BYTE strOut[30] = {0};
	USHORT usLen = 0, ret = 0;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("SET MAINTENANCE"); 
	
	switch(get_env_int("ADTYPE"))
	{
		case 0:
			setLCDPrint(2, DISPLAY_POSITION_LEFT, "DISABLED");
			break;
		case 1:
			setLCDPrint(2, DISPLAY_POSITION_LEFT, "HARDWARE");
			break;
		case 2:
			setLCDPrint(2, DISPLAY_POSITION_LEFT, "SOFTWARE");
			break;
		case 3:
			setLCDPrint(2, DISPLAY_POSITION_LEFT, "HW AND SW");
			break;
	}
	CTOS_LCDTPrintXY(1, 3, "0 - DISABLE");	
	CTOS_LCDTPrintXY(1, 4, "1 - HARDWARE");
	CTOS_LCDTPrintXY(1, 5, "2 - SOFTWARE");	 
	CTOS_LCDTPrintXY(1, 6, "3 - HW AND SW");	 
	CTOS_LCDTPrintXY(1, 7, "New:");
			
	do
	{
		memset(strOut,0x00, sizeof(strOut));
		ret = shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);

		if (ret == d_KBD_CANCEL)
			break;
		else if (ret == d_OK)
			break;
		else if (ret >= 1)
		{
			put_env_int("ADTYPE", atoi(strOut));
			break;
		}
	}
	while(inInput == 0);

	vdDebug_LogPrintf("ADTYPE [%d]", get_env_int("ADTYPE"));

	vduiClearBelow(2);
	CTOS_LCDTPrintXY(1, 8, "Please wait...");

	return;
				
}

void vdSetDaysGap(void)
{
	int inDaysGap = 0,
		inInput = 0;
		
	BYTE strOut[30] = {0};
	USHORT usLen = 0, ret = 0;
	char buf[6+1];
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("SET DAYS GAP"); 

	inCTOSS_GetEnvDB("ADGAP", buf);
	inDaysGap = atoi(buf);

	setLCDPrint(2, DISPLAY_POSITION_LEFT, "Value:");
	setLCDPrint(3, DISPLAY_POSITION_LEFT, buf);
		 
	CTOS_LCDTPrintXY(1, 4, "New:");

	do
	{
		//CTOS_LCDTPrintXY(1, 8, "					");
		memset(strOut,0x00, sizeof(strOut));
		ret = shCTOS_GetNum(5, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
		
		//if(ret == d_KBD_CANCEL)
		//	return d_NO;
		
		inDaysGap = atoi(strOut);
		put_env_int("ADGAP",inDaysGap);
		inInput = 1;		

	}
	while(inInput == 0);

	vdDebug_LogPrintf("ADGAP [%d]", get_env_int("ADGAP"));

	vduiClearBelow(2);
	CTOS_LCDTPrintXY(1, 8, "Please wait...");

	//return d_OK;
				
}

int inCTOSS_Notify_DayBefore_Scheduled_Diagnostics(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
	BOOL isKey;
	unsigned char key;
    int inYear, inMonth, inDate,inDateGap,inAutoDiagGap, inAutoDiagStart;
	int inCurrTime = 0;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;
	char buf[6+1];
	char szTimeStart[16+1];
	char szStr[16+1];
	char szYear[2+1];
	char szMonth[2+1];
	char szDate[2+1];
	BYTE szCurrTime[7] = {0};

	vdDebug_LogPrintf("inCTOSS_Notify_DayBefore_Scheduled_Diagnostics");

	if(get_env_int("NOTIFYAD") == TRUE || get_env_int("ADGAP") < 2)
	{
		vdDebug_LogPrintf("NOTIFY NOT PERFORMED");
		return d_OK;
	}

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    
    vdDebug_LogPrintf("inCTOSS_Notify_DayBefore_Scheduled_Diagnostics");
   
	inCTOSS_GetEnvDB("LASTAD", buf);
	memcpy(szYear,buf,2);
	memcpy(szMonth,&buf[2],2);
	memcpy(szDate,&buf[4],2);

	inYear = atoi(szYear);
    inMonth = atoi(szMonth);
    inDate = atoi(szDate);

	vdDebug_LogPrintf("Notify: inYear[%d] :: inMonth[%d] :: inDate[%d]",inYear,inMonth,inDate);
    CTOS_RTCGet(&SetRTC);

    inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
	 
	memset(buf,0x00,sizeof(buf));
	inCTOSS_GetEnvDB("ADGAP", buf);
	inAutoDiagGap = atoi(buf);
	vdDebug_LogPrintf("Notify: inDateGap=[%d],strTCT.inAutoDiagGap=[%d]",inDateGap,inAutoDiagGap);
	
	if(inDateGap < inAutoDiagGap - 1)
        return d_NO;
	
	memset(szCurrTime, 0x00, sizeof(szCurrTime));
    //CTOS_RTCGet(&SetRTC);
    sprintf(szCurrTime,"%02d%02d", SetRTC.bHour, SetRTC.bMinute);
    inCurrTime = wub_str_2_long(szCurrTime);

	vdDebug_LogPrintf("Notify: szCurrTime[%s] :: inCurrTime[%d]",szCurrTime,inCurrTime);	
	vdDebug_LogPrintf("Notify: inNotifyStart[%d] :: inNotifyEnd[%d]",inNotifyStart,inNotifyEnd);	

	if (inCurrTime >= inNotifyStart && inCurrTime <= inNotifyEnd)
	{
	
		CTOS_LCDTClearDisplay();

		vdDispTitleString("WARNING!!!");

		setLCDPrint(2,DISPLAY_POSITION_CENTER,"TERMINAL WILL RUN");
		setLCDPrint(3,DISPLAY_POSITION_CENTER,"PREVENTIVE");
		setLCDPrint(4,DISPLAY_POSITION_CENTER,"MAINTENANCE");
		setLCDPrint(5,DISPLAY_POSITION_CENTER,"TOMORROW AT");

		memset(buf,0x00,sizeof(buf));

		if(inADiagStart1 > 0)
			inCTOSS_GetEnvDB("AD1START", buf);
		else if(inADiagStart2 > 0)
			inCTOSS_GetEnvDB("AD2START", buf);
		else
			inCTOSS_GetEnvDB("AD3START", buf);
		
		memcpy(szTimeStart,buf,2);
		szTimeStart[2] = ':';
		memcpy(&szTimeStart[3],&buf[2],2);

		sprintf(szStr,"%s",szTimeStart);	

		setLCDPrint(6,DISPLAY_POSITION_CENTER,szStr);

		setLCDPrint(8,DISPLAY_POSITION_CENTER,"PRESS ENTER");	
		
	        
	    while(1)
	    {
	        key = struiGetchWithTimeOut();
	        if (key==d_KBD_ENTER)
	            break;
	        else
	            vduiWarningSound();
	    }
		
		put_env_int("NOTIFYAD",TRUE);
		
		CTOS_LCDTClearDisplay();
	}
	


	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}

int inCTOS_ADC_Download(void)
{

    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, Menukey=0;
	int inRet;
	int key;
	USHORT shRet = 0,
		shLen = 0,
		iInitX = 0,
		shMinLen = 4,
		shMaxLen = 6;
	BYTE szTitleDisplay[MAX_CHAR_PER_LINE + 1] = {0},
		szTitle[MAX_CHAR_PER_LINE + 1] = {0};

  	CTOS_RTC rtcClock;
  	CTOS_RTC SetRTC;
  	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;

	int inAutoDLStart;	
	int inAutoDLEnd;
	int inADLPerformed;
	
	int inADLTries;
  	BYTE szCurrDate[8] = {0};
	
  	BYTE szADLDate[8] = {0};


	
	if (get_env_int("ADLTYPE") != 2)	
		return d_OK;	

    if(inCheckBatcheNotEmtpy() > 0)
        return d_NO;	


	memset(szCurrDate, 0x00, sizeof(szCurrDate));
	CTOS_RTCGet(&SetRTC);
	sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

	//inAutoDLTime = get_env_int("AUTODLTIME");
	
    memset(szADLDate, 0x00, sizeof(szADLDate));
	inCTOSS_GetEnvDB("ADLCURRDATE", szADLDate);

	vdDebug_LogPrintf("DATE %s %s %d", szADLDate, szCurrDate);

	if (strcmp(szADLDate,szCurrDate)!=0) {
		put_env_int("ADL",0);
		put_env_char("ADLCURRDATE",szCurrDate);
		put_env_int("ADLTRY1",0);	
		put_env_int("ADLTRY2",0);
		put_env_int("ADLTRY3",0);	
		
	}
	
	inADLPerformed = get_env_int("ADL");
	
	
	if (inADLPerformed == 1) 
		return SUCCESS;


	vdDebug_LogPrintf("ADC  %d %d",  inADLStart1,inADLEnd1 );
	vdDebug_LogPrintf("ADC  %d %d",  inADLStart2,inADLEnd2 );
	vdDebug_LogPrintf("ADC  %d %d",  inADLStart3,inADLEnd3 );

   
    memset(szCurrTime, 0x00, sizeof(szCurrTime));
    CTOS_RTCGet(&rtcClock);
    sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
    inCurrTime = wub_str_2_long(szCurrTime);


	if (inCurrTime >= inADLStart1 && inCurrTime <= inADLEnd1){
		inAutoDLStart = inADLStart1;
		inAutoDLEnd = inADLEnd1;
		inADLTimeRangeUsed = 1;
		vdDebug_LogPrintf("TIME1	%d %d",  inAutoDLStart,inAutoDLEnd );	
	}else if (inCurrTime >= inADLStart2 && inCurrTime <= inADLEnd2){
		inAutoDLStart = inADLStart2;
		inAutoDLEnd = inADLEnd2;
		inADLTimeRangeUsed = 2;	
		vdDebug_LogPrintf("TIME2	%d %d",  inAutoDLStart,inAutoDLEnd );
	}else if (inCurrTime >= inADLStart3 && inCurrTime <= inADLEnd3){
		inAutoDLStart = inADLStart3;
		inAutoDLEnd = inADLEnd3;
		inADLTimeRangeUsed = 3;	
		vdDebug_LogPrintf("TIME3	%d %d",  inAutoDLStart,inAutoDLEnd );
	}else{
		return SUCCESS;
	}


	
	if (inADLTimeRangeUsed == 1){
		if (get_env_int("ADLTRY1") == 1)
			return SUCCESS;
	}else if (inADLTimeRangeUsed == 2){
		if (get_env_int("ADLTRY2") == 1)
			return SUCCESS;
	}else if (inADLTimeRangeUsed == 3){
		if (get_env_int("ADLTRY3") == 1)
			return SUCCESS;
	}
			
	vdDebug_LogPrintf("ADC %d %d %d", inCurrTime, inAutoDLStart,inAutoDLEnd );

	if (inCurrTime >= inAutoDLStart &&  inCurrTime <= inAutoDLEnd)
	{
	    if ((inADLTimeRangeUsed+1 > inADLLimit)	|| (inCurrTime > inMaxTime))
			put_env_int("ADL",1);

	        inCTOSS_ADLSettlementCheckTMSDownloadRequest();
			
	}
  	return SUCCESS;


}


void vdSetADLParams(void)
{

  	CTOS_RTC rtcClock;
  	CTOS_RTC SetRTC;
  	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;

  	BYTE szADLDate[8] = {0};
  	BYTE szCurrDate[8] = {0};

	int inStart1;
	int inEnd1;
	int inStart2;
	int inEnd2;
	int inStart3;
	int inEnd3;


	memset(szCurrDate, 0x00, sizeof(szCurrDate));
	CTOS_RTCGet(&SetRTC);
	sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

	inCTOSS_GetEnvDB("ADLCURRDATE", szADLDate);

	
	if ((strcmp(szADLDate,"")==0) || (strcmp(szADLDate,"000000")==0)){
		put_env_char("ADLCURRDATE",szCurrDate);

	}

	inADLStart1 = get_env_int("ADL1START");
	inADLEnd1 = get_env_int("ADL1END");

	
	inADLStart2= get_env_int("ADL2START");
	inADLEnd2= get_env_int("ADL2END");

	inMaxTime = 0;
	
	inADLStart3= get_env_int("ADL3START");
	inADLEnd3= get_env_int("ADL3END");

	if (inADLStart1 > 0 && inADLEnd1 > 0){
		inADLLimit= inADLLimit+1;
		inMaxTime = inADLEnd1;
	}

	if (inADLStart2 > 0 && inADLEnd2 > 0){
		inADLLimit= inADLLimit+1;
		inMaxTime = inADLEnd2;
	}
	
	if (inADLStart3 > 0 && inADLEnd3 > 0){
		inADLLimit= inADLLimit+1;
		inMaxTime = inADLEnd3;
	}

}

int inCTOSS_ADLSettlementCheckTMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;

	//if(DIAL_UP_MODE == strTCT.inTMSComMode)
		//return d_NO;
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

	if (inADLTimeRangeUsed == 1)
		put_env_int("ADLTRY1",1);	
	else if (inADLTimeRangeUsed == 2)
		put_env_int("ADLTRY2",1);	
	else if (inADLTimeRangeUsed == 3)
		put_env_int("ADLTRY3",1);	


	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    //inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);

		inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
		 

		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}


    //check if batch settle
    //should check all application?
    //vdDebug_LogPrintf("Check Batch Empty");
    //if(inCTOSS_TMSChkBatchEmpty() != d_OK)
        //return d_NO;
    //if(inCheckBatchEmtpy() > 0)
    //    return d_NO;	

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_NAC_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
	else if(WIFI_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_WIFI;
	
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
	CTOS_LCDTClearDisplay();
    usResult = inCTOSS_TMSPreConfig3(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}

int inCheckBatcheNotEmtpy(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM MMT where fBatchNotEmpty = 1";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("inCount[%d]",inCount);
	return(inCount);
}


int inCTOSS_TMSPreConfig3(int inComType)
{    
    CTMS_GPRSInfo stgprs;    
    CTMS_ModemInfo stmodem;    
    CTMS_EthernetInfo st;
    int inNumOfRecords = 0;
    BYTE szSerialNum[17+1], szInputBuf[21+1];
    BYTE count = 2,i;
	BYTE tmpbuf[16 + 1];
	int len;
    unsigned char ckey;
	USHORT usStatusLine=8, usRes=0;
	CTMS_WIFIInfo stWF;
	USHORT usRet;
	BYTE szTemp[4+1];

	inTCTRead(1);
	inTCPRead(1);
	inTMSRead(1);
    inHDTRead(1);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);

	memset(szSerialNum, 0x00, sizeof(szSerialNum));
	memset(tmpbuf, 0x00, sizeof(tmpbuf));

	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	if(strlen(szSerialNum) <= 0)
	    CTOS_GetFactorySN(szSerialNum); //usGetSerialNumber(szTermSerialNum); 
	for (i=0; i<strlen(szSerialNum); i++)
	{
		if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
		{
			szSerialNum[i] = 0;
			break;
		}
	}
	len = strlen(szSerialNum);
	strcpy(tmpbuf,"0000000000000000");
	memcpy(&tmpbuf[16-len],szSerialNum,len);
	vdDebug_LogPrintf("szSerialNum=[%s].tmpbuf=[%s]..",szSerialNum,tmpbuf);
	
    CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, tmpbuf);//if TID is 12345678, SN is 0000000012345678
    CTOS_CTMSSetConfig(d_CTMS_RECOUNT, &count);

	vdDebug_LogPrintf("usTMSGap[%d]", strTCT.usTMSGap);
	vdDebug_LogPrintf("inTMSComMode[%d]", strTCT.inTMSComMode);
	vdDebug_LogPrintf("szTMSRemoteIP[%s]", strTCT.szTMSRemoteIP);
	vdDebug_LogPrintf("usTMSRemotePort[%d]", strTCT.usTMSRemotePort);
	vdDebug_LogPrintf("fDHCPEnable[%d]", strTCP.fDHCPEnable);
	vdDebug_LogPrintf("szTerminalIP[%s]", strTCP.szTerminalIP);
	vdDebug_LogPrintf("szWifiSSID[%s]", strTCP.szWifiSSID);
	vdDebug_LogPrintf("szWifiPassword[%s]", strTCP.szWifiPassword);
	vdDebug_LogPrintf("szAPN[%s]", strTCP.szAPN);
	vdDebug_LogPrintf("szUserName[%s]", strTCP.szUserName);
	vdDebug_LogPrintf("szPassword[%s]", strTCP.szPassword);
	vdDebug_LogPrintf("inSIMSlot[%d]", strTCP.inSIMSlot);
	vdDebug_LogPrintf("szTMSNACDestAddr[%s]", strTMS.szTMSNACDestAddr);
	vdDebug_LogPrintf("szTMSNACProtocol[%s]", strTMS.szTMSNACProtocol);
	vdDebug_LogPrintf("szTMSNACSourceAddr[%d]", strTMS.szTMSNACSourceAddr);
	
    if(strTCP.fDHCPEnable)
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, "0.0.0.0");
    else
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, strTCP.szTerminalIP);// If it is DHCP, also need to configure any value, otherwise please put the value from database

    if(ETHERNET_MODE == strTCT.inTMSComMode)
    {
        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("ETHERNET SETTINGS");
	
    	vdDebug_LogPrintf("ETHERNET_MODE..");
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
        
        strcpy(st.strGateway, strTCP.szGetWay);
        strcpy(st.strMask, strTCP.szSubNetMask);
        st.bDHCP = strTCP.fDHCPEnable;

        strcpy(st.strRemoteIP, strTCT.szTMSRemoteIP);
        st.usRemotePort = strTCT.usTMSRemotePort;

        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    }
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
    {
			CTMS_NacInfo stNAC;
	
			vdDebug_LogPrintf("DIAL_UP_MODE..");

			memset(&stNAC, 0x00, sizeof (stNAC));
			CTOS_CTMSGetConfig(d_CTMS_NAC_CONFIG, &stNAC);
	
			DebugAddHEX("stNAC.baSourceAddr", stNAC.baSourceAddr, 2);
			DebugAddHEX("stNAC.baDestAddr", stNAC.baDestAddr, 2);
	
			vdDebug_LogPrintf("stNAC.baSourceAddr[%s]..",stNAC.baSourceAddr);
			vdDebug_LogPrintf("stNAC.baDestAddr[%s]..",stNAC.baDestAddr);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_str_2_hex(strTMS.szTMSNACProtocol, szTemp, 2);		  
			stNAC.bProtocol=szTemp[0];
			
			stNAC.usBlockSize=1024;
	
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_str_2_hex(strTMS.szTMSNACSourceAddr, szTemp, 4);		
			memcpy(stNAC.baSourceAddr, szTemp, 2);
	
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_str_2_hex(strTMS.szTMSNACDestAddr, szTemp, 4);				  
			memcpy(stNAC.baDestAddr, szTemp, 2);
	
			
			stNAC.bLenType=0;
			stNAC.bAddLenFlag=0;
					
			usRes=CTOS_CTMSSetConfig(d_CTMS_NAC_CONFIG, &stNAC);
			
			memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
			CTOS_CTMSGetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
			
			stmodem.bMode  = d_M_MODE_SDLC_FAST;
			stmodem.bHandShake = d_M_HANDSHAKE_V22_ONLY;
			stmodem.bCountryCode = d_M_COUNTRY_SINGAPORE;
	
			strcpy(stmodem.strRemotePhoneNum, strTCT.szTMSRemotePhone);
			vdDebug_LogPrintf("strRemotePhoneNum[%s]..",stmodem.strRemotePhoneNum);
			strcpy(stmodem.strID, strTCT.szTMSRemoteID);	
			vdDebug_LogPrintf("strID[%s]..",stmodem.strID);
			strcpy(stmodem.strPW, strTCT.szTMSRemotePW);
			vdDebug_LogPrintf("strPW[%s]..",stmodem.strPW);
	
			strcpy(stmodem.strRemoteIP, strTCT.szTMSRemoteIP);
			stmodem.usRemotePort = strTCT.usTMSRemotePort;
	
			stmodem.usPPPRetryCounter = 2;
			stmodem.ulPPPTimeout = 34463;
			stmodem.ulDialTimeout = 34463;
			vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].",stmodem.strRemoteIP,stmodem.usRemotePort);
			stmodem.bType = d_CTMS_MODEM; //d_CTMS_TCP_MODEM, d_CTMS_MODEM
			stmodem.ulBaudRate = 115200;
			stmodem.bParity = 'N'; 
			stmodem.bDataBits = 8;
			stmodem.bStopBits = 1;
			stmodem.usDialDuration = 30;				// Dialing maximum total duration in second. Range: 0~255
			stmodem.usDTMFOnTime = 95;				 // DTMF on time in ms.
			stmodem.usDTMFOffTime = 95; 			   // DTMF off time in ms.	  
			stmodem.bTxPowerLevel = 3;				   // Tx Power Level(refer to Modem Functions)
			stmodem.bRxPowerLevel = 3;
			
			usRes=CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
		}
    else if(GPRS_MODE == strTCT.inTMSComMode)
    {
		CTMS_GPRSInfo stgprs;

        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("GPRS SETTINGS");
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);
		
              vdDebug_LogPrintf("GPRS_MODE..");
        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        strcpy(stgprs.strAPN, strTCP.szAPN);
        strcpy(stgprs.strID, strTCP.szUserName);
        strcpy(stgprs.strPW, strTCP.szPassword);
 
        strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
        stgprs.usRemotePort = strTCT.usTMSRemotePort;
        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
        stgprs.ulTCPConnectTimeout = 10000;

		//test
		stgprs.ulTCPTxTimeout=10000;
		stgprs.ulTCPRxTimeout=10000;
		stgprs.bSIMSlot=1;

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }
    else if(WIFI_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("WIFI SETTINGS");
        
        vdDebug_LogPrintf("WIFI..");
        memset(&stWF, 0x00, sizeof (CTMS_WIFIInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_WIFI_CONFIG, &stWF);


		strcpy(stWF.strRemoteIP, strTCT.szTMSRemoteIP);
	    stWF.usRemotePort = strTCT.usTMSRemotePort;

		strcpy(stWF.baPassword, strTCP.szWifiPassword);
		strcpy(stWF.baSSid, strTCP.szWifiSSID);
		//stWF.bProtocal = 3;
		//stWF.bGroup = 2;
		//stWF.bPairwise = 2;
		stWF.bDHCP = strTCP.fDHCPEnable;
		stWF.bSCAN_Mode = 1;
		stWF.IsAutoConnect =48;
		stWF.IsHidden =48;

		memset(stWF.strLocalIP, 0x00, sizeof(stWF.strLocalIP));
		memset(stWF.strMask, 0x00, sizeof(stWF.strMask));	
		memset(stWF.strGateway, 0x00, sizeof(stWF.strGateway));

		if (strTCP.fDHCPEnable){
			strcpy(stWF.strLocalIP, "0.0.0.0");
			strcpy(stWF.strMask, "0.0.0.0");
			strcpy(stWF.strGateway, "0.0.0.0");
		}else{
			strcpy(stWF.strLocalIP, strTCP.szTerminalIP);
			strcpy(stWF.strMask, strTCP.szSubNetMask);
			strcpy(stWF.strGateway, strTCP.szGetWay);
		}
	
		
       CTOS_LCDTClearDisplay();
       vdDispTitleString("WIFI SETTINGS");
    
       memset(szSerialNum, 0x00, sizeof(szSerialNum));
       CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
       
       memset(szInputBuf, 0x00, sizeof(szInputBuf));
       sprintf(szInputBuf, "SN: %s", szSerialNum);         
       setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 
       
       memset(szInputBuf, 0x00, sizeof(szInputBuf));
       sprintf(szInputBuf, "IP: %s", stWF.strRemoteIP);         
       setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 
       
       memset(szInputBuf, 0x00, sizeof(szInputBuf));
       sprintf(szInputBuf, "PORT: %d", stWF.usRemotePort);         
       setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 


	   //bProtocal
	   if (strcmp(strTCP.szWifiProtocal, "WEP")  == 0)
	   {
		   stWF.bProtocal = d_WIFI_PROTOCOL_WEP;
	   }
	   else if (strcmp(strTCP.szWifiProtocal, "WPA")  == 0)
	   {
		   stWF.bProtocal = d_WIFI_PROTOCOL_WPA;
	   }
	   else if (strcmp(strTCP.szWifiProtocal, "WPA2")  == 0)
	   {
		   stWF.bProtocal = 3;
	   }
	   
	   //Pairwise
	   if (strcmp(strTCP.szWifiPairwise, "TKIP")  == 0)
	   {
		   stWF.bPairwise = d_WIFI_PAIRWISE_TKIP;
	   }
	   else if (strcmp(strTCP.szWifiPairwise, "CCMP")  == 0)
	   {
		   stWF.bPairwise = d_WIFI_PAIRWISE_CCMP;
	   }
	   else if (strcmp(strTCP.szWifiPairwise, "CCMPTKI")  == 0)
	   {
		   stWF.bPairwise = d_WIFI_PAIRWISE_TKIPCCMP;
	   }
	   
	   //Group
	   if (strcmp(strTCP.szWifiGroup, "TKIP")  == 0)
	   {
		   stWF.bGroup = d_WIFI_GROUP_TKIP;
	   }
	   else if (strcmp(strTCP.szWifiGroup, "CCMP") == 0)
	   {
		   stWF.bGroup = d_WIFI_GROUP_CCMP;
	   }
	   else if (strcmp(strTCP.szWifiGroup, "CCMPTKI")  == 0)
	   {
		  stWF.bGroup = d_WIFI_GROUP_TKIPCCMP;
	   }
       
       usRes=CTOS_CTMSSetConfig(d_CTMS_WIFI_CONFIG , &stWF);

	   	vdDebug_LogPrintf("stWF.IsAutoConnect - %d", stWF.IsAutoConnect);
		vdDebug_LogPrintf("stWF.IsHidden - %d", stWF.IsHidden);
		vdDebug_LogPrintf("stWF.baPassword - %s", stWF.baPassword);
		vdDebug_LogPrintf("stWF.baSSid - %s", stWF.baSSid);
		vdDebug_LogPrintf("stWF.bProtocal - %d", stWF.bProtocal);
		vdDebug_LogPrintf("stWF.bGroup - %d", stWF.bGroup);
		vdDebug_LogPrintf("stWF.bPairwise - %d", stWF.bPairwise);
		vdDebug_LogPrintf("stWF.strRemoteIP - %s", stWF.strRemoteIP);
		vdDebug_LogPrintf("stWF.usRemotePort - %d", stWF.usRemotePort);
		vdDebug_LogPrintf("stWF.bDHCP - %d", stWF.bDHCP);
		vdDebug_LogPrintf("stWF.strLocalIP - %s", stWF.strLocalIP);
		vdDebug_LogPrintf("stWF.strMask - %s", stWF.strMask);
		vdDebug_LogPrintf("stWF.strGateway - %s", stWF.strGateway);
		vdDebug_LogPrintf("stWF.bSCAN_Mode - %d", stWF.bSCAN_Mode);
    }

	return usRes;
}

int inCTOSS_TMSDownloadviaReprogramming(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;

	
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_NAC_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
	else if(WIFI_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_WIFI;
	
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
	CTOS_LCDTClearDisplay();
    usResult = inCTOSS_TMSPreConfig3(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

    CTOS_CTMSUtility(usComType);

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}



int inTMSRead(int inSeekCnt)
{
	int result;
	int len = 0;
	int inResult = -1;
	char *sql = "SELECT szTMSNACProtocol, szTMSNACSourceAddr, szTMSNACDestAddr FROM TMS WHERE TMSid = ?";
	
	memset(&strTMS,0x00,sizeof(STRUCT_TMS));
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	//vdSetJournalModeOff();

	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) 
		{ /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;			
    
	        /*szTMSNACProtocol*/
			strcpy((char*)strTMS.szTMSNACProtocol, (char *)sqlite3_column_text(stmt,inStmtSeq));

			/*szTMSNACSourceAddr*/
			strcpy((char*)strTMS.szTMSNACSourceAddr, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szTMSNACDestAddr*/
			strcpy((char*)strTMS.szTMSNACDestAddr, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
        }
	} while (result == SQLITE_ROW);	

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inRetryDiagnosticsPrompt(void)
{
	BOOL isKey;
	unsigned char c;
	int inRet = 0;
	
	//CTOS_LCDTClearDisplay();
    //vdDispTitleString("DIAGNOSTICS");
	
	//vduiClearBelow(2);
    //vduiDisplayStringCenter(3,"RETRY");
    //vduiDisplayStringCenter(4,"DIAGNOSTICS");

    setLCDPrint(16,DISPLAY_POSITION_LEFT,"RETRY[X] PROCEED[OK]");

	/*				
	while(1)
    {
        while(1)
	    {
	        CTOS_KBDInKey(&isKey);
	        if (isKey){ //If isKey is TRUE, represent key be pressed //
	            vduiLightOn();
	            //Get a key from keyboard //
	            CTOS_KBDGet(&c);
				break;
	        }	        
	    }

		if (c==d_KBD_ENTER)
        	return SUCCESS;			
		else if (c==d_KBD_CANCEL)
        	return FAIL;
        
    }
    */

	fTimeOutFlag = FALSE;
	while(TRUE)
	{
		c = struiGetchWithTimeOut();

		if (fTimeOutFlag == TRUE)
		{
			fTimeOutFlag = FALSE;
			continue;
		}
		
		if (c == d_KBD_ENTER)
		{
			inRet = SUCCESS;
			break;
		}

		if (c == d_KBD_CANCEL)
		{
			inRet = FAIL;
			break;
		}			
	}

	return inRet;
}

int inDisplaySummarywithRetry(int inPurpose)
{
	int inHWErrorCtr = 0;
	int inSWErrorCtr = 0;
	int inLineCtr = 0;
	unsigned char key;

	strHWDiag.inHWErrorCount = 0;
	strSWDiag.inSWErrorCount = 0;
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("REPORT");

	if(inPurpose != SOFTWARE_TEST)
	{
		setLCDPrint(2,DISPLAY_POSITION_LEFT,"HARDWARE:");

		if(strHWDiag.inLCD_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"LCD ERROR");
		}

		if(strHWDiag.inPrinter_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"PRINTER ERROR");
		}
		
		if(strHWDiag.inCTLSReader_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"CTLS ERROR");
		}	

		if(strHWDiag.inMSReader_Status== FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"MSR ERROR");
		}	

		if(strHWDiag.inICCReader_Status == FAIL)
		{
			inHWErrorCtr++;
			setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"ICC ERROR");
		}

		if(strHWDiag.inCOMMS_Status == FAIL)
		{
			inHWErrorCtr++;
			switch(strCPT.inCommunicationMode)
			{
				case ETHERNET_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"ETH ERROR");
					break;
				
				case DIAL_UP_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"DIAL ERROR");
					break;

				case GPRS_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"GPRS ERROR");
					break;

				case WIFI_MODE:
					setLCDPrint(inHWErrorCtr+2,DISPLAY_POSITION_LEFT,"WIFI ERROR");
					break;
				default:
		        break;
		    }

		}
		
		if(inHWErrorCtr == 0)
			setLCDPrint(inHWErrorCtr+3,DISPLAY_POSITION_LEFT,"NO ERRORS");

		strHWDiag.inHWErrorCount = inLineCtr = inHWErrorCtr ;
	}

	if(inPurpose != HARDWARE_TEST)
	{
		setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"SOFTWARE:");
		
		if(strSWDiag.inCredit_Status == FAIL)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"CREDIT ERROR");
		}
		else if(strSWDiag.inCredit_Status == VOID_FAILED)
		{
			inSWErrorCtr++;
			inLineCtr++;
			setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"CREDIT VOID FAILED");
		}
		if(get_env_int("TEST_INSTALLMENT") ==1)
                  {
                       if(strSWDiag.inInst_Status == FAIL)
                       {
                            inSWErrorCtr++;
                            inLineCtr++;
                            setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"INSTALLMENT ERROR");
                       }
                       else if(strSWDiag.inInst_Status == VOID_FAILED)
                       {
                            inSWErrorCtr++;
                            inLineCtr++;
                            setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"INST VOID FAILED");
                       }
                  }
		if(get_env_int("TEST_BANCNET") ==1)
                  {
                       if(strSWDiag.inBancnet_Status == FAIL)
                       {
                            inSWErrorCtr++;
                            inLineCtr++;
                            setLCDPrint(inLineCtr+5,DISPLAY_POSITION_LEFT,"BANCNET ERROR");
                       }
                  }

		if(inSWErrorCtr == 0)
			setLCDPrint(inLineCtr+6,DISPLAY_POSITION_LEFT,"NO ERRORS");

		strSWDiag.inSWErrorCount = inSWErrorCtr;
	}
		
	//setLCDPrint(16,DISPLAY_POSITION_CENTER,"ENTER TO PROCEED");	

        
    return inRetryDiagnosticsPrompt();

}


int inMerchantCheckList(void)
{
	int inCount = 0;
	char szTemp[15+1];

	memset((char*)inAnswer, 0x00, sizeof(inAnswer));
	
	CTOS_LCDTClearDisplay();

	vdDispTitleString("CARD ACCEPTANCE");
	
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	
	CTOS_LCDTSetReverse(TRUE); 
	setLCDPrint(8,DISPLAY_POSITION_LEFT,"Proper Card Acceptance   ");
	setLCDPrint(9,DISPLAY_POSITION_LEFT,"procedure were performed?");
	setLCDPrint(10,DISPLAY_POSITION_CENTER,  "       (5 steps)         ");
	CTOS_LCDTSetReverse(FALSE); 
	inAnswer[inCount] = inGetAnswer(19);
	inCount++;

	CTOS_LCDTClearDisplay();

	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	vdDispTitleString("FRAUD ORIENTATION");
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	
	while(1)
	{
		vduiClearBelow(3);
			
		if(inCount == 1)
			CTOS_LCDTSetReverse(TRUE);		
		setLCDPrint(3,DISPLAY_POSITION_LEFT,"Fraud Types       ");
		setLCDPrint(4,DISPLAY_POSITION_LEFT,"(Lost/Skimming/ATO/NRI)  ");
		if(inCount == 1)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 2)
			CTOS_LCDTSetReverse(TRUE);      
		setLCDPrint(6,DISPLAY_POSITION_LEFT,"Suspicious behavior   ");
		if(inCount == 2)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount == 3)
			CTOS_LCDTSetReverse(TRUE);	    
		setLCDPrint(8,DISPLAY_POSITION_LEFT,"How to do settlement     ");
		setLCDPrint(9,DISPLAY_POSITION_LEFT,"Security features of Visa/Master");
		if(inCount == 3)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 4)
			CTOS_LCDTSetReverse(TRUE);		 	
		setLCDPrint(11,DISPLAY_POSITION_LEFT,"Code 10 authorization");
		if(inCount == 4)
			CTOS_LCDTSetReverse(FALSE);
		
		inAnswer[inCount] = inGetAnswer(19);

		vdDebug_LogPrintf("inAnswer[%d] = %d", inCount, inAnswer[inCount] );
		
		inCount++;
		
		if(inCount == 5)
			break;

		
	}

	CTOS_LCDTClearDisplay();
	
	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	vdDispTitleString("POS FEATURES");
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	
	while(1)
	{
		vduiClearBelow(3);
			
		if(inCount ==5)
			CTOS_LCDTSetReverse(TRUE);		
		setLCDPrint(3,DISPLAY_POSITION_LEFT,"How to void a Sale       ");
		setLCDPrint(4,DISPLAY_POSITION_LEFT,"(Reversal/Cancellation)  ");
		if(inCount == 5)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 6)
			CTOS_LCDTSetReverse(TRUE);      
		setLCDPrint(6,DISPLAY_POSITION_LEFT,"How to reprint txns      ");
		setLCDPrint(7,DISPLAY_POSITION_LEFT,"and settlements          ");
		if(inCount == 6)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount ==7)
			CTOS_LCDTSetReverse(TRUE);	    
		setLCDPrint(9,DISPLAY_POSITION_LEFT,"How to do settlement     ");
		if(inCount == 7)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 8)
			CTOS_LCDTSetReverse(TRUE);		 	
		setLCDPrint(11,DISPLAY_POSITION_LEFT,"Pre-Authorization        ");
		if(inCount == 8)
			CTOS_LCDTSetReverse(FALSE);
		



		if(inCount == 9)
			CTOS_LCDTSetReverse(TRUE);		 
		setLCDPrint(13,DISPLAY_POSITION_LEFT,"How to do offline sales  ");
		if(inCount == 9)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount == 10)
			CTOS_LCDTSetReverse(TRUE);		 
		setLCDPrint(15,DISPLAY_POSITION_LEFT,"Installment transactions ");
		if(inCount == 10)
			CTOS_LCDTSetReverse(FALSE);
		
		
		inAnswer[inCount] = inGetAnswer(19);

		vdDebug_LogPrintf("inAnswer[%d] = %d", inCount, inAnswer[inCount] );
		
		inCount++;
		
		if(inCount == 11)
			break;

		
	}
	
	CTOS_LCDTClearDisplay();

	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	vdDispTitleString("TRANSACTION&PAYMENTS");
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	
	while(1)
	{
		vduiClearBelow(3);
			
		if(inCount == 11)
			CTOS_LCDTSetReverse(TRUE);	
		setLCDPrint(3,DISPLAY_POSITION_LEFT,"Underpayment / ");
		setLCDPrint(4,DISPLAY_POSITION_LEFT,"Overpayment       ");
		if(inCount == 11)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 12)
			CTOS_LCDTSetReverse(TRUE);      
		setLCDPrint(6,DISPLAY_POSITION_LEFT,"Payment via MBTC deposit");
		setLCDPrint(7,DISPLAY_POSITION_LEFT,"or check delivery");
		if(inCount == 12)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount == 13)
			CTOS_LCDTSetReverse(TRUE);	    
		setLCDPrint(9,DISPLAY_POSITION_LEFT,"Chargeback     ");
		if(inCount == 13)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 14)
			CTOS_LCDTSetReverse(TRUE);		 	
		setLCDPrint(11,DISPLAY_POSITION_LEFT,"Sales slip retrieval");
		if(inCount == 14)
			CTOS_LCDTSetReverse(FALSE);
		



		if(inCount == 15)
			CTOS_LCDTSetReverse(TRUE);		 
		setLCDPrint(13,DISPLAY_POSITION_LEFT,"How to process tips  ");
		if(inCount == 15)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount == 16)
			CTOS_LCDTSetReverse(TRUE);		 
		setLCDPrint(15,DISPLAY_POSITION_LEFT,"How to process     ");
		setLCDPrint(16,DISPLAY_POSITION_LEFT,"manual transactions");
		if(inCount == 16)
			CTOS_LCDTSetReverse(FALSE);
		
		
		inAnswer[inCount] = inGetAnswer(19);

		vdDebug_LogPrintf("inAnswer[%d] = %d", inCount, inAnswer[inCount] );
		
		inCount++;
		
		if(inCount == 17)
			break;

		
	}

	CTOS_LCDTClearDisplay();
	
	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	vdDispTitleString("NUMBERS TO CALL");
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	
	while(1)
	{
			
		if(inCount == 17)
			CTOS_LCDTSetReverse(TRUE);		
		setLCDPrint(3,DISPLAY_POSITION_LEFT,"POS problem/complaints   ");
		if(inCount == 17)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount == 18)
			CTOS_LCDTSetReverse(TRUE);		
		setLCDPrint(5,DISPLAY_POSITION_LEFT,"Supplies - thermal rolls,");
		setLCDPrint(6,DISPLAY_POSITION_LEFT,"sales slips, etc         ");
		if(inCount == 18)
			CTOS_LCDTSetReverse(FALSE);

		inAnswer[inCount] = inGetAnswer(19);

		vdDebug_LogPrintf("inAnswer[%d] = %d", inCount, inAnswer[inCount] );

		inCount++;
		
		if(inCount == 19)
			break;

		
		
	}

	CTOS_LCDTClearDisplay();

	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	vdDispTitleString("BIN VERIFICATION");
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	
	while(1)
	{
		vduiClearBelow(3);
			
		if(inCount == 19)
			CTOS_LCDTSetReverse(TRUE);		
		setLCDPrint(3,DISPLAY_POSITION_LEFT,"BINVER Procedure       ");
		if(inCount == 19)
			CTOS_LCDTSetReverse(FALSE);
		

		
		if(inCount == 20)
			CTOS_LCDTSetReverse(TRUE);      
		setLCDPrint(5,DISPLAY_POSITION_LEFT,"BINVER Thresholds  /");
		setLCDPrint(6,DISPLAY_POSITION_LEFT,"Amounts   ");
		if(inCount == 20)
			CTOS_LCDTSetReverse(FALSE);
		


		if(inCount == 21)
			CTOS_LCDTSetReverse(TRUE);	    
		setLCDPrint(8,DISPLAY_POSITION_LEFT,"BINVER Agreement     ");
		if(inCount == 21)
			CTOS_LCDTSetReverse(FALSE);
		
		inAnswer[inCount] = inGetAnswer(19);

		vdDebug_LogPrintf("inAnswer[%d] = %d", inCount, inAnswer[inCount] );
		
		inCount++;
		
		if(inCount == 22)
			break;

		
	}

	
	CTOS_LCDTClearDisplay();
	
		CTOS_LCDTSelectFontSize(d_FONT_16x30);
		vdDispTitleString("OTHERS");
		CTOS_LCDTSelectFontSize(d_FONT_12x24);
		
		while(1)
		{
			vduiClearBelow(3);
				
			if(inCount == 22)
				CTOS_LCDTSetReverse(TRUE);		
			setLCDPrint(3,DISPLAY_POSITION_LEFT,"Split Sales          ");
			if(inCount == 22)
				CTOS_LCDTSetReverse(FALSE);
			
	
			
			if(inCount == 23)
				CTOS_LCDTSetReverse(TRUE);		
			setLCDPrint(5,DISPLAY_POSITION_LEFT,"Double billing    ");
			if(inCount == 23)
				CTOS_LCDTSetReverse(FALSE);
			
	
			if(inCount == 24)
				CTOS_LCDTSetReverse(TRUE);		
			setLCDPrint(7,DISPLAY_POSITION_LEFT,"Sales draft laundering /");
			setLCDPrint(8,DISPLAY_POSITION_LEFT,"multiple swiping     ");
			if(inCount == 24)
				CTOS_LCDTSetReverse(FALSE);

			if(inCount == 25)
				CTOS_LCDTSetReverse(TRUE);		
			setLCDPrint(10,DISPLAY_POSITION_LEFT,"Procedure on REFER");
			setLCDPrint(11,DISPLAY_POSITION_LEFT,"and DECLINE responses     ");
			if(inCount == 25)
				CTOS_LCDTSetReverse(FALSE);
			
			inAnswer[inCount] = inGetAnswer(19);
	
			vdDebug_LogPrintf("inAnswer[%d] = %d", inCount, inAnswer[inCount] );
			
			inCount++;
			
			if(inCount == 26)
				break;
				
		}
	

	
	
    CTOS_LCDTSelectFontSize(d_FONT_16x30);

}

int inGetAnswer(int inLine)
{
	BOOL isKey;
	unsigned char c;
	
	vduiDisplayStringCenter(inLine,"YES[OK] NO[X] NA[<-]");
	
	while(1)
	{
		
		while(1)
	    {
	        CTOS_KBDInKey(&isKey);
	        if (isKey){ 
	            vduiLightOn();

	            CTOS_KBDGet(&c);
				break;
	        }	        
	    }

		if (c==d_KBD_ENTER)
			return 0;
	    else if (c==d_KBD_CANCEL)
		    return -1;
		else if (c == d_KBD_CLEAR)
			return -2;
	}	
}

void vdSetMerchantResponseToTxt(int inAnswer, char* szResponse)
{

	if(inAnswer == 0)
		strcpy(szResponse,"YES");
	else if (inAnswer == -1)
		strcpy(szResponse,"NO");	
	else if (inAnswer == -2)
		strcpy(szResponse,"NA");

}

int inChooseMerchantorWitness (void)
{
	int inRet;
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	char szHeaderString[50] = "Choose";
	char szHostMenu[70+1]={"Merchant\nWitness"};
	char szTermSerialNum[15+1];

	CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("inChooseMerchantorWitness START");

	//inSetColorMenuMode();
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
	inSetTextMode();

	if (key == 0xFF) 
	{
		CTOS_LCDTClearDisplay();
	   //vduiWarningSound();
		return d_NO;  
	}

	if(key > 0)
	{
		if (get_env_int("INVALIDKEY") == 1)
			return ST_ERROR;
		
		if(key == 1)
		     inInputReceiverName();	
		else if(key == 2)
			inInputReceiverNameWitness();
		else if(key==d_KBD_CANCEL)
			return -1;
		
		return 0;
	}
}

int inInputReceiverNameWitness(void)
{
     BYTE bRet;
     USHORT szCustNameLen = 20;
     BYTE szCustName[20+1];
     CTOS_LCDTClearDisplay();
     
     setLCDPrint(1,DISPLAY_POSITION_LEFT,"ENTER WITNESS");
     setLCDPrint(2,DISPLAY_POSITION_LEFT,"REPRESENTATIVE NAME:");
     setLCDPrint(4,DISPLAY_POSITION_LEFT,"__________________ ");
     setLCDPrint(6, DISPLAY_POSITION_LEFT, "ENTER CONTACT NO:");
     setLCDPrint(8,DISPLAY_POSITION_LEFT,"___________ ");
     memset(szCustName,0x00,sizeof(szCustName));
     memset(strService.szCustName,0x00,sizeof(strService.szCustName));

      gblinWitnessSign = 1;

     while(TRUE)
     {
          bRet = InputStringLetters(1, 4, 0x00, 0x02, szCustName, &szCustNameLen, 1, d_INPUT_TIMEOUT);
          
          if(bRet == d_KBD_ENTER)
          {
               if(strlen(szCustName) > 0)
               {
                    break;
               }
          }
     }     
     memcpy(strService.szCustName,szCustName,strlen(szCustName));
     
     return d_OK;	
}

void vdOrientationInvoiceNumbers (char * InvoiceNumber)
{
	FILE *hHandle;
	BYTE szLineBuff[7];
	char szFile[100];
	BYTE szInvNoBcd[3];

	strcpy(szFile, "/home/ap/pub/fsrtrans.dat");

	hHandle = fopen(szFile, "a+");
	fprintf(hHandle,"%s\n",InvoiceNumber);
	fclose(hHandle);

}

void vdPrintOrientationInvoiceNumbers(void)
{
	FILE *hHandle;
	BYTE szLineBuff[7];
	char szFile[100];
	BYTE szInvNoBcd[3];
	int inFileSize;

	vdDebug_LogPrintf("--vdPrintOrientationInvoiceNumbers--");

	strcpy(szFile, "/home/ap/pub/fsrtrans.dat");
         	
               hHandle = fopen(szFile, "r");
	      while (fgets(szLineBuff,7,hHandle))
              {
                   char  *chbuff=szLineBuff;
                   if(strlen(chbuff) > 5)
                   {
                        if(atoi(chbuff) != 0)
                        {
                             inAscii2Bcd(chbuff, szInvNoBcd, INVOICE_BCD_SIZE);
                             memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);			
                             put_env_char("#ORIENTATIONTRANS", "1");
                             //inCTOS_REPRINT_ANY();	 
                        }
                   }
              
	  
         	}
         fclose(hHandle);

		 vdDebug_LogPrintf("szInvNoBcd[%s]", szInvNoBcd);
		 DebugAddHEX("srTransRec.szInvoiceNo", srTransRec.szInvoiceNo, 3);
		 
}

int inPrintLastFSRReport(void)
{

    char szStr[d_LINE_SIZE + 1];
    char szStr2[d_LINE_SIZE + 1];	
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
    CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];
    BOOL fERMChangeFlag = FALSE;
    int inHost1 = 91;
    char szTID1[10] = "00055555";
    char szMID1[20] = "000000000005555";
    int inHost2 = 92;
    char szTID2[10] = "00022222";
    char szMID2[20] = "000000000002222";
    int inHost3 = 93;
    char szTID3[10] = "00033333";
    char szMID3[20] = "000000000003333";
    int inHost4 = 94;
    char szTID4[10] = "00044444";
    char szMID4[20] = "000000000004444";
    char szTID5[10] = "00077777";
    char szMID5[20] = "000000000007777";
    char szFile[50];
    char szBuffer[100];

	
    int x =0;
   unsigned char chTid[10];
   unsigned char chMid[15];

      vdDebug_LogPrintf("AAA - inPrintLastFSRReport start");

      vduiClearBelow(2);
      vdDisplayErrorMsg(1, 8, "FUNC NOT YET AVAILABLE");
      return FAIL;
#if 0	  
      strcpy(szFile, "/home/ap/pub/fsrrpt.dat");

       if((inResult = inMyFile_CheckFileExist("fsrrpt.dat")) < 0)
       {
            vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
             vduiClearBelow(2);
             vdDisplayErrorMsg(1, 8, "FSR REPORT NOT  EXIST");
             return FAIL;
       }
      else
      {
           vdCTOSS_PrinterStart(200);
           CTOS_PrinterSetHeatLevel(4);

		   
            ushCTOS_PrintHeader(ENGINEER_COPY_RECEIPT);
            vdCTOS_PrinterFline(1);	
            
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
            
            memset(szStr, ' ', d_LINE_SIZE);
            memset(szTemp, ' ', d_LINE_SIZE);
            memset(szTemp1, ' ', d_LINE_SIZE);
            memset(szTemp2, ' ', d_LINE_SIZE);
            memset(szTemp3, ' ', d_LINE_SIZE);

	fERMTransaction = FALSE;
           
           FILE * fHandle = fopen(szFile, "r");
           while(fgets(szBuffer,100,fHandle))
           {
                char * chbuff = szBuffer;
	       x++;
	      vdDebug_LogPrintf("AAA - x:%d", x);
               if(x==1)
               {
                        strcpy (szStr, "TID:");
                        strcat(szStr, chbuff);
                        szStr[strlen(szStr)-1] = '\0';
                        vdDebug_LogPrintf("AAA 1 - szStr:%s x:%d", szStr, x);
               }
	     if(x==2)
              {
                      strcpy (szTemp, "MID:");
                      strcat(szTemp, chbuff);
                      szTemp[strlen(szTemp)-1] = '\0';
                      vdDebug_LogPrintf("AAA 2 - chTid:%s chMid:%s %d",chTid, szTemp, x);
                      inPrintLeftRight(szStr, szTemp, 46);
              }
	          
           }
          
           fclose(fHandle);
      }

	  vdCTOSS_PrinterEnd();
	  #endif
       return SUCCESS;
}

void vdFSRPrintMCCHostInfo(void){
    int shHostIndex = 1, inNumOfMerchant=0;
    int inResult,inRet;
    unsigned char szCRC[8+1];
    char ucLineBuffer[d_LINE_SIZE];
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inLoop = 0;
    char szBillerName[1024] = {0};
    char szBillerCode[1024] = {0};
    char szStr[d_LINE_SIZE + 1];
    BYTE szTempBuf[12+1];
    BYTE szBuf[50];

    vdDebug_LogPrintf("-->>vdCTOS_HostInfo[START]");
    
    vduiLightOn();

    //fix for issue #00092
    //Must display "Out of Paper..."
    if( printCheckPaper()==-1)
        return;

  
    srTransRec.HDTid = 19; 
    strHDT.inHostIndex = 19;
    inHDTRead(19);
    inCPTRead(19);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    CTOS_LCDTClearDisplay();
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRINT_HOST_INFO);
        if(d_OK != inRet)
            return ;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return ;
        
            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return ;
        }
    }	 


    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    	
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  
    inCTOS_DisplayPrintBMP();
    //vdSetPrintThreadStatus(1); //##00129
    
    //print Logo	
    if(strlen(strHDT.szHeaderLogoName) > 0)
        vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
    else
        vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");
    
    vdPrintTitleCenter("HOST INFO REPORT");
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    vdCTOS_PrinterFline(1);
	
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    
    vdCTOS_SetDateTime();
    
    printDateTime(FALSE);
    
    inPrint("---------------------------------------------");

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "HOST        : %s", (char *)strHDT.szHostLabel);
    inPrint(szStr);
    
    memset(szTempBuf, 0x00, sizeof(szTempBuf));
    wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
    sprintf(szStr, "TPDU        : %s", szTempBuf);
    inPrint(szStr);
    
    memset(szTempBuf, 0x00, sizeof(szTempBuf));
    wub_hex_2_str(strHDT.szNII,szTempBuf,2);
    sprintf(szStr, "NII         : %s", szTempBuf);
    inPrint(szStr);
    
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "PRI TEL NUM : %s", (char *)strCPT.szPriTxnPhoneNumber);
    inPrint(szStr);
    
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "SEC TEL NUM : %s", (char *)strCPT.szSecTxnPhoneNumber);
    inPrint(szStr);
    
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "PRI IP      : %s", (char *)strCPT.szPriTxnHostIP);
    inPrint(szStr);
    
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "PRI IP PORT : %04ld", strCPT.inPriTxnHostPortNum);
    inPrint(szStr);
    
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "SEC IP      : %s", (char *)strCPT.szSecTxnHostIP);
    inPrint(szStr);
    
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "SEC IP PORT : %04ld", strCPT.inSecTxnHostPortNum);
    inPrint(szStr);

	/*print Serial No*/
    {    
        char szTermSerialNumber[15]; // print terminal serial number on all txn receipt - mfl
        char szStr1[35+1]; // print terminal serial number on all txn receipt - mfl
        
        memset(szStr1, 0x00, sizeof(szStr1));
        memset (baTemp, 0x00, sizeof(baTemp));			
        memset(szTermSerialNumber,0x00,sizeof(szTermSerialNumber));
        
        CTOS_GetFactorySN(szTermSerialNumber);	
         //usGetSerialNumber(szTermSerialNumber);
        szTermSerialNumber[15]=0;        
        sprintf(szStr1, "SERIAL NO   : %s", szTermSerialNumber);
        inPrint(szStr1);    
    }

	// Print CRC
	vdPrintCRC();
	
    inPrint("---------------------------------------------");

    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);

    vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
    for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
    {
        if((inResult = inMMTReadRecord(strHDT.inHostIndex,inLoop)) !=d_OK)
        {
            vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", inLoop,strHDT.inHostIndex,inResult);
            continue;
            //break;
        }
        else 
        {
            if(strMMT[0].fMMTEnable)
            {	
                memset(szStr, 0x00, sizeof(szStr));
                sprintf(szStr, "  MERCHANT : %s", strMMT[0].szMerchantName); 											 
                inPrint(szStr);
                
                memset(szStr, 0x00, sizeof(szStr));
                sprintf(szStr, "  TID      : %s", strMMT[0].szTID); 											 
                inPrint(szStr);
                
                memset(szStr, 0x00, sizeof(szStr));
                sprintf(szStr, "  MID      : %s", strMMT[0].szMID); 											 
                inPrint(szStr);
                
                memset(szStr, 0x00, sizeof(szStr));
                memset(szBuf, 0x00, sizeof(szBuf));
                wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
                sprintf(szStr, "  BATCH NO : %s", szBuf); 											 
                inPrint(szStr);
                vdCTOS_PrinterFline(1);
            }										
        }
    }

    vdLineFeed(FALSE);
	
	vdCTOSS_PrinterEnd();
	
    vdDebug_LogPrintf("-->>vdCTOS_HostInfo[END]");
}

void vdDiagRemoveCard(void)
{
	BYTE bySC_status;
	
	while(1)
	{
		CTOS_SCStatus(d_SC_USER, &bySC_status);
		if(bySC_status & d_MK_SC_PRESENT)
		{
			if ((strTCT.byTerminalType % 2) == 0)
				setLCDPrint(14,DISPLAY_POSITION_CENTER," --- REMOVE CARD --- ");
			else
				setLCDPrint(8,DISPLAY_POSITION_CENTER," --- REMOVE CARD --- ");
	
			CTOS_Beep();
			CTOS_Delay(300);
			CTOS_Beep();
			continue;
		}	
		break;
	}
	
}

