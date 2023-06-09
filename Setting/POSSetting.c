/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>

#include "../Includes/myEZLib.h"
#include "../Includes/msg.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSSetting.h"


#include "..\Includes\CTOSInput.h"
#include "../Includes/POSTypedef.h"
#include "../FileModule/myFileFunc.h"

#include "../print/Print.h"
#include "../FileModule/myFileFunc.h"
#include "../Comm/V5Comm.h"


#include "../Accum/accum.h"
#include "../DataBase/DataBaseFunc.h"
#include "..\Includes\POSSetting.h"
#include "..\ui\Display.h"
#include "..\debug\debug.h"
#include "..\Includes\POSTrans.h"
#include "..\Accum\Accum.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"

//-- Inject Key
#include "..\Aptrans\MultiShareEMV.h"
#include "..\PCI100\PCI100.h"
#include "..\PCI100\COMMS.h"

/* BDO CLG: Revised menu functions - start -- jzg */
#include "..\Includes\DMenu.h"
extern int inReadDynamicMenu(void);
/* BDO CLG: Revised menu functions - end -- jzg */


//#include "..\Includes\POSFleet.h"
#include "..\Aptrans\MultiShareCOM.h"
//#include "..\Includes\POSDCC.h"


#define DISPLAY_POSITION_LEFT 0
#define DISPLAY_POSITION_CENTER 1
#define DISPLAY_POSITION_RIGHT 2
#define DISPLAY_LINE_SIZE 16


BOOL BolDetachGPRSChangeSetting=FALSE, BolDetachDIALChangeSetting=FALSE;
BOOL fManualSettle = FALSE; //aaronnino for BDOCLG ver 9.0 fix on issue #00102 Manual settle prints settlement closed instead of summary report 1 of 4

BOOL fAUTOManualSettle = FALSE;
BOOL fCommsFallback = FALSE;
int inCommsFallbackMode = -1;
//powersave
static int inSleepflag = 0;
//powersave



char szFuncTitleName [21 + 1]; //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 1 of  6
extern BOOL fRePrintFlag;
int inSelectedIdleAppsHost;
void vdCTOS_uiPowerOff(void)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;
        
    CTOS_LCDTClearDisplay();

    //vduiDisplayStringCenter(3,"ARE YOU SURE");
    //vduiDisplayStringCenter(4,"WANT TO POWER");
    //vduiDisplayStringCenter(5,"OFF TERMINAL");
    vduiDisplayStringCenter(4,"POWER OFF TERMINAL?");
	//gcitra-0728
    //CTOS_LCDTPrintXY(1,7,"NO[X]   YES[OK] ");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
	//gcitra-0728
    c=WaitKey(60);
    
    if(c!=d_KBD_ENTER)
    {
        return;
    }
    
    for(ya =1; ya<5; ya++)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,ya);
        CTOS_LCDTClear2EOL();
    }
    for(yb=8; yb>4; yb--)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,yb);
        CTOS_LCDTClear2EOL();
    }
    CTOS_LCDTPrintXY(1,4,"----------------");
    for(xa=1; xa<8; xa++)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xa,4," ");
    }
    for(xb=16; xb>7; xb--)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xb,4," ");
    }
            
    CTOS_LCDGShowPic(58, 6, block, 0, 6);
    CTOS_Delay(250);
    CTOS_LCDTGotoXY(7,4);
    CTOS_LCDTClear2EOL();
    CTOS_Delay(250);

    CTOS_PowerOff();
}

int inCTOS_SelectHostSetting(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[6144];
    char szHostName[200][400];
    int inCPTID[400];
    int inLoop = 0;
    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    

    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));

    inHDTReadHostName(szHostName, inCPTID);

    for (inLoop = 0; inLoop < 150; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");  
		
		vdDebug_LogPrintf("szHostMenu %s", szHostMenu);
        }
        else
            break;
    }


	vdDebug_LogPrintf("szHostMenu2 %s %d", szHostMenu, strlen(szHostMenu));

		inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();
		
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
        
        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = inCPTID[key-1];
        strHDT.inHostIndex = inCPTID[key-1];
        inHDTRead(inCPTID[key-1]);
        inCPTRead(inCPTID[key-1]);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		inCSTRead(strHDT.inCurrencyIdx); /*to load Currence and amount format*/

		vdDebug_LogPrintf("strHDT.szAPName = %s", strHDT.szAPName);
    }
    
	vdDebug_LogPrintf("hostid %d", inCPTID[key-1]);
    return inCPTID[key-1];
}

/*configuration functions*/


void vdCTOS_IPConfig(void)
{
		BYTE bRet, key;
		BYTE szInputBuf[5];
		int inResult;
		BYTE strOut[30],strtemp[17];
		USHORT ret;
		USHORT usLen;
		BOOL BolDetachLANChange=FALSE;
		int shHostIndex = 1;

//		if (strTCT.fSingleComms)	
//			shHostIndex = 1;
//		else
			shHostIndex = inCTOS_SelectHostSetting();

		if (shHostIndex == -1)
				return;
								
		inResult = inCPTRead(shHostIndex);
		if(inResult != d_OK)
				return;

		CTOS_LCDTClearDisplay();
		vdDispTitleString("HOST SETTING");
		while(1)
		{
				vduiClearBelow(3);
				setLCDPrint(3, DISPLAY_POSITION_LEFT, "Connection Type");
				if((strCPT.inCommunicationMode)== ETHERNET_MODE)
						setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");
				if((strCPT.inCommunicationMode)== DIAL_UP_MODE)
						setLCDPrint(4, DISPLAY_POSITION_LEFT, "0"); 			 
				if((strCPT.inCommunicationMode)== GPRS_MODE)
					    setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
//wifi-mod
				else if((strCPT.inCommunicationMode)== WIFI_MODE)
					setLCDPrint(4, DISPLAY_POSITION_LEFT, "3");
//wifi-mod							
				
				CTOS_LCDTPrintXY(1, 5, "0-DIAL-UP    1-LAN");
				CTOS_LCDTPrintXY(1, 6, "2-GPRS          3-WIFI");
	 
				strcpy(strtemp,"New:") ;
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
				if (ret == d_KBD_CANCEL )
						break;
				else if(0 == ret )
						break;
				else if(ret==1)
				{
						if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33)
						{
								 if(strOut[0] == 0x31)
								 {
												strCPT.inCommunicationMode = ETHERNET_MODE;
												BolDetachLANChange = TRUE;
								 }
								 if(strOut[0] == 0x30)
								 {
												strCPT.inCommunicationMode = DIAL_UP_MODE;
												BolDetachDIALChangeSetting = TRUE;
								 }
								 if(strOut[0] == 0x32)
								 {
												strCPT.inCommunicationMode = GPRS_MODE;
												BolDetachGPRSChangeSetting = TRUE;
								 }
								 //wifi-mod
								 if(strOut[0] == 0x33)
								 {
									   			strCPT.inCommunicationMode = WIFI_MODE;
												//BolDetachLANChange = TRUE;
								 }
								 //wifi-mod

								 break;
						 }
						 else
						 {
								vduiWarningSound();
								vduiClearBelow(6);
								vduiDisplayStringCenter(6,"PLEASE SELECT");
								vduiDisplayStringCenter(7,"A VALID");
								vduiDisplayStringCenter(8,"CONNECTION MODE");
								CTOS_Delay(2000); 			
						}
				}
		}
		inResult = inCPTSave(shHostIndex);
		inResult = inCPTRead(shHostIndex);

		if ((strCPT.inCommunicationMode == GPRS_MODE) || (strCPT.inCommunicationMode == WIFI_MODE))
			strTCT.fShareComEnable = 1;
		else
			strTCT.fShareComEnable = 0;
		inTCTSave(1);

		if(((strCPT.inCommunicationMode)== ETHERNET_MODE) || ((strCPT.inCommunicationMode)== GPRS_MODE) || ((strCPT.inCommunicationMode)== WIFI_MODE))
		{
				//-------------------------------------------------------------
				// inIPHeader
				inResult = inCPTRead(shHostIndex);
				if(inResult != ST_SUCCESS)
						return;

				while(1)
				{
						vduiClearBelow(3);
						setLCDPrint(3, DISPLAY_POSITION_LEFT, "IP/GPRS HEADER");
						memset(szInputBuf, 0x00, sizeof(szInputBuf));
						sprintf(szInputBuf, "%d", strCPT.inIPHeader);
						setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
						
						strcpy(strtemp,"New:") ;
						CTOS_LCDTPrintXY(1, 7, strtemp);
						memset(strOut,0x00, sizeof(strOut));
						ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
						if (ret == d_KBD_CANCEL )
								break;
						else if(0 == ret )
								break;
						else if(ret>=1)
						{
								strCPT.inIPHeader = atoi(strOut); 
								inResult = inCPTSave(shHostIndex);
								vdMyEZLib_LogPrintf("new inIPHeader %d",strCPT.inIPHeader);
								break;
						} 	
						if(ret == d_KBD_CANCEL)
								break;
				} 
				inResult = inCPTRead(shHostIndex);
				//-------------------------------------------------------------
		
				inResult = inTCPRead(1);
				if(inResult != ST_SUCCESS)
						return;

				while(1)
				{
						vduiClearBelow(3);
						setLCDPrint(3, DISPLAY_POSITION_LEFT, "IP Config");
						if (strTCP.fDHCPEnable == IPCONFIG_DHCP)
								setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");
						if (strTCP.fDHCPEnable == IPCONFIG_STATIC)	
								setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
						
						CTOS_LCDTPrintXY(1, 5, "0-STATIC     1-DHCP");
						CTOS_LCDTPrintXY(1, 6, "									 ");
						
						
						strcpy(strtemp,"New:") ;
						CTOS_LCDTPrintXY(1, 7, strtemp);
						memset(strOut,0x00, sizeof(strOut));
						ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
						if (ret == d_KBD_CANCEL )
								break;
						else if(0 == ret )
								break;
						else if(ret==1)
						{
								if (strOut[0]==0x30 || strOut[0]==0x31)
								{
										BolDetachLANChange=TRUE;
										
										if(strOut[0] == 0x30)  
														strTCP.fDHCPEnable = IPCONFIG_STATIC;
										if(strOut[0] == 0x31)
														strTCP.fDHCPEnable = IPCONFIG_DHCP;
										break;
								}
								else
								{
										vduiWarningSound();
										vduiClearBelow(6);
										vduiDisplayStringCenter(6,"PLEASE SELECT");
										vduiDisplayStringCenter(7,"A VALID");
										vduiDisplayStringCenter(8,"IP Config");
										CTOS_Delay(2000); 			
								}
						}
				} 							
				inResult = inTCPSave(1);
				inResult = inTCPRead(1);

				inResult = inCPTRead(shHostIndex);
				if(inResult != ST_SUCCESS)
						return;
                while(1)
                {
                    vduiClearBelow(3);
                    setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRI HOST IP");
                    setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriTxnHostIP);
                    
                    strcpy(strtemp,"New:") ;
                    CTOS_LCDTPrintXY(1, 7, strtemp);
                    memset(strtemp, 0x00, sizeof(strtemp));
                    memset(strOut,0x00, sizeof(strOut));
                    ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
                    if(ret==d_KBD_ENTER)
                    {
                        BolDetachLANChange=TRUE;
                        memcpy(strCPT.szPriTxnHostIP,strOut,strlen(strOut));
                        strCPT.szPriTxnHostIP[strlen(strOut)]=0;
//                        if(strTCT.fSMMode == TRUE)
//                        {
//                            memcpy(strCPT.szPriSettlementHostIP,strOut,strlen(strOut));
//                            strCPT.szPriSettlementHostIP[strlen(strOut)]=0;							 	
//                        }
                        vdMyEZLib_LogPrintf("new host ip %s",strCPT.szPriTxnHostIP);
                        break;
                    } 	
                    if(ret == d_KBD_CANCEL)
                        break;
                }
				inResult = inCPTSave(shHostIndex);
				inResult = inCPTRead(shHostIndex);
				if(inResult != ST_SUCCESS)
						return;
				
				while(1)
				{ 	
						vduiClearBelow(3);
						setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRI HOST PORT");
						memset(szInputBuf, 0x00, sizeof(szInputBuf));
						sprintf(szInputBuf, "%d", strCPT.inPriTxnHostPortNum);
						setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
						
						strcpy(strtemp,"New:") ;
						CTOS_LCDTPrintXY(1, 7, strtemp);
						memset(strOut,0x00, sizeof(strOut));
						ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
						if (ret == d_KBD_CANCEL )
								break;
						else if(0 == ret )
								break;
						else if(ret>=1)
						{
								BolDetachLANChange=TRUE;
								strCPT.inPriTxnHostPortNum = atoi(strOut);	
//								if(strTCT.fSMMode == TRUE)
//								    strCPT.inPriSettlementHostPort = atoi(strOut);
								vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriTxnHostPortNum);
								break;
						} 	
						if(ret == d_KBD_CANCEL)
								break;
				}
				inResult = inCPTSave(shHostIndex);

				
				inResult = inTCPRead(1);
				inResult = inCPTRead(shHostIndex);
				if(inResult != ST_SUCCESS)
						return;

				//-------------------------------------------------------------------------
                while(1)
                {
                    vduiClearBelow(3);
                    setLCDPrint(3, DISPLAY_POSITION_LEFT, "SEC HOST IP");
                    setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecTxnHostIP);
                    
                    strcpy(strtemp,"New:") ;
                    CTOS_LCDTPrintXY(1, 7, strtemp);
                    memset(strtemp, 0x00, sizeof(strtemp));
                    memset(strOut,0x00, sizeof(strOut));
                    ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
                    if(ret==d_KBD_ENTER)
                    {
                        BolDetachLANChange=TRUE;
                        memcpy(strCPT.szSecTxnHostIP,strOut,strlen(strOut));
                        strCPT.szSecTxnHostIP[strlen(strOut)]=0;
//                        if(strTCT.fSMMode == TRUE)
//                        {
//                            memcpy(strCPT.szSecSettlementHostIP,strOut,strlen(strOut));
//                            strCPT.szSecSettlementHostIP[strlen(strOut)]=0;							 	
//                        }
                        vdMyEZLib_LogPrintf("new host ip %s",strCPT.szSecTxnHostIP);
                        break;
                    } 	
                    if(ret == d_KBD_CANCEL)
                        break;
                }
				inResult = inCPTSave(shHostIndex);
				inResult = inCPTRead(shHostIndex);
				if(inResult != ST_SUCCESS)
						return;
				
				while(1)
				{ 	
						vduiClearBelow(3);
						setLCDPrint(3, DISPLAY_POSITION_LEFT, "SEC HOST PORT");
						memset(szInputBuf, 0x00, sizeof(szInputBuf));
						sprintf(szInputBuf, "%d", strCPT.inSecTxnHostPortNum);
						setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
						
						strcpy(strtemp,"New:") ;
						CTOS_LCDTPrintXY(1, 7, strtemp);
						memset(strOut,0x00, sizeof(strOut));
						ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
						if (ret == d_KBD_CANCEL )
								break;
						else if(0 == ret )
								break;
						else if(ret>=1)
						{
								BolDetachLANChange=TRUE;
								strCPT.inSecTxnHostPortNum = atoi(strOut);	
//                                if(strTCT.fSMMode == TRUE)
//                                    strCPT.inSecSettlementHostPort = atoi(strOut);
								vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecTxnHostPortNum);
								break;
						} 	
						if(ret == d_KBD_CANCEL)
								break;
				}
				inResult = inCPTSave(shHostIndex);

				
				inResult = inTCPRead(1);
				inResult = inCPTRead(shHostIndex);
				if(inResult != ST_SUCCESS)
						return;
				//-------------------------------------------------------------------------
//                if(strTCT.fSMMode == FALSE)
                    if(0)
                {
                    while(1)
                    {
                         vduiClearBelow(3);
                         setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRI SETTLE HOST IP");
                         setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriSettlementHostIP);
                         
                         strcpy(strtemp,"New:") ;
                         CTOS_LCDTPrintXY(1, 7, strtemp);
                         memset(strtemp, 0x00, sizeof(strtemp));
                         memset(strOut,0x00, sizeof(strOut));
                         ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
                         if(ret==d_KBD_ENTER)
                         {
                             BolDetachLANChange=TRUE;
                             memcpy(strCPT.szPriSettlementHostIP,strOut,strlen(strOut));
                             strCPT.szPriSettlementHostIP[strlen(strOut)]=0;
                             vdMyEZLib_LogPrintf("new host ip %s",strCPT.szPriSettlementHostIP);
                             break;
                         } 	
                         if(ret == d_KBD_CANCEL)
                              break;
                    }
                    inResult = inCPTSave(shHostIndex);
                    inResult = inCPTRead(shHostIndex);
                    if(inResult != ST_SUCCESS)
                        return;
                    
                    while(1)
                    { 	
                        vduiClearBelow(3);
                        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRI SETTLE HOST PORT");
                        memset(szInputBuf, 0x00, sizeof(szInputBuf));
                        sprintf(szInputBuf, "%d", strCPT.inPriSettlementHostPort);
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
                        
                        strcpy(strtemp,"New:") ;
                        CTOS_LCDTPrintXY(1, 7, strtemp);
                        memset(strOut,0x00, sizeof(strOut));
                        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
                        if (ret == d_KBD_CANCEL )
                            break;
                        else if(0 == ret )
                            break;
                        else if(ret>=1)
                        {
                            BolDetachLANChange=TRUE;
                            strCPT.inPriSettlementHostPort = atoi(strOut);			
                            vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriSettlementHostPort);
                            break;
                        } 	
                        if(ret == d_KBD_CANCEL)
                            break;
                    }
                    inResult = inCPTSave(shHostIndex);				
                    inResult = inCPTRead(shHostIndex);
                    
                    //-------------------------------------------------------------------------------
                    while(1)
                    {
                        vduiClearBelow(3);
                        setLCDPrint(3, DISPLAY_POSITION_LEFT, "SEC SETTLE HOST IP");
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecSettlementHostIP);
                        
                        strcpy(strtemp,"New:") ;
                        CTOS_LCDTPrintXY(1, 7, strtemp);
                        memset(strtemp, 0x00, sizeof(strtemp));
                        memset(strOut,0x00, sizeof(strOut));
                        ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
                        if(ret==d_KBD_ENTER)
                        {
                            BolDetachLANChange=TRUE;
                            memcpy(strCPT.szSecSettlementHostIP,strOut,strlen(strOut));
                            strCPT.szSecSettlementHostIP[strlen(strOut)]=0;
                            vdMyEZLib_LogPrintf("new host ip %s",strCPT.szSecSettlementHostIP);
                            break;
                        } 	
                        if(ret == d_KBD_CANCEL)
                            break;
                    }
                    inResult = inCPTSave(shHostIndex);
                    inResult = inCPTRead(shHostIndex);
                    if(inResult != ST_SUCCESS)
                    return;
                    
                    while(1)
                    { 	
                        vduiClearBelow(3);
                        setLCDPrint(3, DISPLAY_POSITION_LEFT, "SEC SETTLE HOST PORT");
                        memset(szInputBuf, 0x00, sizeof(szInputBuf));
                        sprintf(szInputBuf, "%d", strCPT.inSecSettlementHostPort);
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
                        
                        strcpy(strtemp,"New:") ;
                        CTOS_LCDTPrintXY(1, 7, strtemp);
                        memset(strOut,0x00, sizeof(strOut));
                        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
                        if (ret == d_KBD_CANCEL )
                            break;
                        else if(0 == ret )
                            break;
                        else if(ret>=1)
                        {
                            BolDetachLANChange=TRUE;
                            strCPT.inSecSettlementHostPort = atoi(strOut);			
                            vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecSettlementHostPort);
                            break;
                        } 	
                        if(ret == d_KBD_CANCEL)
                            break;
                    }
                    inResult = inCPTSave(shHostIndex);
                }
				
				inResult = inCPTRead(shHostIndex);
				//-------------------------------------------------------------------------------
				
				if(strTCP.fDHCPEnable == IPCONFIG_STATIC)
				{
						while(1)
						{
								vduiClearBelow(3);
								setLCDPrint(3, DISPLAY_POSITION_LEFT, "TERMINAL IP");
								setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szTerminalIP);
								
								strcpy(strtemp,"New:") ;		 
								CTOS_LCDTPrintXY(1, 7, strtemp);
								memset(strtemp, 0x00, sizeof(strtemp));
								memset(strOut,0x00, sizeof(strOut));
								ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
								if(ret==d_KBD_ENTER)
								{
										BolDetachLANChange=TRUE;
										memcpy(strTCP.szTerminalIP, strOut, strlen(strOut));
										strTCP.szTerminalIP[strlen(strOut)]=0;
										vdMyEZLib_LogPrintf("new Client ip %s",strTCP.szTerminalIP);
										break;
								} 	
								if(ret == d_KBD_CANCEL)
										break;
						}
						inResult = inTCPSave(1);
						inResult = inTCPRead(1);		
						
						while(1)
						{
								vduiClearBelow(3);
								setLCDPrint(3, DISPLAY_POSITION_LEFT, "Sub. Mask IP");
								setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szSubNetMask);
								
								strcpy(strtemp,"New:") ;	 
								CTOS_LCDTPrintXY(1, 7, strtemp);
								memset(strtemp, 0x00, sizeof(strtemp));
								memset(strOut,0x00, sizeof(strOut));
								ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
								if(ret==d_KBD_ENTER)
								{
										BolDetachLANChange=TRUE;
										memcpy(strTCP.szSubNetMask, strOut,strlen(strOut));
										strTCP.szSubNetMask[strlen(strOut)]=0;
										vdMyEZLib_LogPrintf("new Sub. Mask ip %s", strTCP.szSubNetMask);
										break;
								} 	
								if(ret == d_KBD_CANCEL)
										break;
						} 							
								inResult = inTCPSave(1);
								inResult = inTCPRead(1);								
								while(1)
								{
										vduiClearBelow(3);
										setLCDPrint(3, DISPLAY_POSITION_LEFT, "Gateway IP");
										setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szGetWay);
										
										strcpy(strtemp,"New:") ;	
										CTOS_LCDTPrintXY(1, 7, strtemp);
										memset(strtemp, 0x00, sizeof(strtemp));
										memset(strOut,0x00, sizeof(strOut));
										ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
										if(ret==d_KBD_ENTER)
										{
												BolDetachLANChange=TRUE;
												memcpy(strTCP.szGetWay, strOut,strlen(strOut));
												strTCP.szGetWay[strlen(strOut)]=0;
												vdMyEZLib_LogPrintf("new Gateway ip %s", strTCP.szGetWay);
												break;
										} 	
										if(ret == d_KBD_CANCEL)
												break;
								}
								inResult = inTCPSave(1);
								inResult = inTCPRead(1);
								
								while(1)
								{
										vduiClearBelow(3);
										setLCDPrint(3, DISPLAY_POSITION_LEFT, "DNS1 IP");
										if (wub_strlen(strTCP.szHostDNS1)<=0)
												setLCDPrint(4, DISPLAY_POSITION_LEFT, "[NOT SET]");
										else
												setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szHostDNS1);
										
										strcpy(strtemp,"New:"); 		
										CTOS_LCDTPrintXY(1, 7, strtemp);
										memset(strtemp, 0x00, sizeof(strtemp));
										memset(strOut,0x00, sizeof(strOut));
										ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 0, IP_LEN);
										if(ret==d_KBD_ENTER)
										{
												if(strlen(strOut)>0)
												{
														BolDetachLANChange=TRUE;
														memcpy(strTCP.szHostDNS1,strOut,strlen(strOut));
														strTCP.szHostDNS1[strlen(strOut)]=0;
														vdMyEZLib_LogPrintf("new DNS ip %s", strTCP.szHostDNS1);
														break;
												}
												else
												{
														vduiClearBelow(3);
														vduiDisplayStringCenter(4,"NO DNS IP");
														vduiDisplayStringCenter(5,"ENTERED,DISABLE");
														vduiDisplayStringCenter(6,"DNS IP?");
														vduiDisplayStringCenter(8,"YES[OK] NO[X]");
														key=struiGetchWithTimeOut();
														if(key==d_KBD_ENTER)
														{
																memset(strTCP.szHostDNS1, 0, sizeof(strTCP.szHostDNS1));
																break;
														}
												}
										} 											
										if(ret == d_KBD_CANCEL)
												break;
								}
								inResult = inTCPSave(1);
								inResult = inTCPRead(1);

								while(1)
								{
										vduiClearBelow(3);
										setLCDPrint(3, DISPLAY_POSITION_LEFT, "DNS2 IP");
										if (wub_strlen(strTCP.szHostDNS2)<=0)
												setLCDPrint(4, DISPLAY_POSITION_LEFT, "[NOT SET]");
										else
												setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szHostDNS2);
										
										strcpy(strtemp,"New:"); 		
										CTOS_LCDTPrintXY(1, 7, strtemp);
										memset(strtemp, 0x00, sizeof(strtemp));
										memset(strOut,0x00, sizeof(strOut));
										ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 0, IP_LEN);
										if(ret==d_KBD_ENTER)
										{
												if(strlen(strOut)>0)
												{
														BolDetachLANChange=TRUE;
														memcpy(strTCP.szHostDNS2,strOut,strlen(strOut));
														strTCP.szHostDNS2[strlen(strOut)]=0;
														vdMyEZLib_LogPrintf("new DNS ip %s", strTCP.szHostDNS2);
														break;
												}
												else
												{
														vduiClearBelow(3);
														vduiDisplayStringCenter(4,"NO DNS IP");
														vduiDisplayStringCenter(5,"ENTERED,DISABLE");
														vduiDisplayStringCenter(6,"DNS IP?");
														vduiDisplayStringCenter(8,"YES[OK] NO[X]");
														key=struiGetchWithTimeOut();
														if(key==d_KBD_ENTER)
														{
																memset(strTCP.szHostDNS2, 0, sizeof(strTCP.szHostDNS2));
																break;
														}
												}
										} 											
										if(ret == d_KBD_CANCEL)
												break;
								}
								inResult = inTCPSave(1);
								inResult = inTCPRead(1);
						}

						//------------------------------------------------------------------------------------
						// IP/GPRS REQUEST TIMEOUT
						while(1)
						{ 	
								vduiClearBelow(3);
								setLCDPrint(3, DISPLAY_POSITION_LEFT, "IP/GPRS REQ TIMEOUT");
								memset(szInputBuf, 0x00, sizeof(szInputBuf));
								sprintf(szInputBuf, "%d", strCPT.inTCPConnectTimeout);
								setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
								
								strcpy(strtemp,"New:") ;
								CTOS_LCDTPrintXY(1, 7, strtemp);
								memset(strOut,0x00, sizeof(strOut));
								ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
								if (ret == d_KBD_CANCEL )
										break;
								else if(0 == ret )
										break;
								else if(ret>=1)
								{
										strCPT.inTCPConnectTimeout = atoi(strOut);
										break;
								} 	
								if(ret == d_KBD_CANCEL)
										break;
						}
						
						inResult = inCPTSave(shHostIndex);
						inResult = inCPTRead(shHostIndex);
						//------------------------------------------------------------------------------------

						//------------------------------------------------------------------------------------
						// IP/GPRS RESPONSE TIMEOUT
						while(1)
						{ 	
								vduiClearBelow(3);
								setLCDPrint(3, DISPLAY_POSITION_LEFT, "IP/GPRS RESP TIMEOUT");
								memset(szInputBuf, 0x00, sizeof(szInputBuf));
								sprintf(szInputBuf, "%d", strCPT.inTCPResponseTimeout);
								setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
								
								strcpy(strtemp,"New:") ;
								CTOS_LCDTPrintXY(1, 7, strtemp);
								memset(strOut,0x00, sizeof(strOut));
								ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
								if (ret == d_KBD_CANCEL )
										break;
								else if(0 == ret )
										break;
								else if(ret>=1)
								{
										strCPT.inTCPResponseTimeout = atoi(strOut);
										break;
								} 	
								if(ret == d_KBD_CANCEL)
										break;
						}
						
						inResult = inCPTSave(shHostIndex);
						inResult = inCPTRead(shHostIndex);

//wifi-mod
#if 1
						if(strCPT.inCommunicationMode == WIFI_MODE)
						{
						//inCTOSS_COMMWIFISCAN();
								srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
								if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
								{
										//vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
										vdDisplayErrorMsgResp2("","Initialization","Error");
										return;
								}	
										BolDetachLANChange = FALSE;
						}
#endif
						//wifi-mod

						//------------------------------------------------------------------------------------
						if(BolDetachLANChange==TRUE)
						{
								srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
								vduiClearBelow(3);
								CTOS_LCDTPrintXY (1,7, "Please Wait 		"); 			
								if(strCPT.inCommunicationMode == ETHERNET_MODE)
										CTOS_LCDTPrintXY(1, 8, "Init LAN... 		");
								else if(strCPT.inCommunicationMode == GPRS_MODE)
										CTOS_LCDTPrintXY(1, 8, "Init GPRS...		 ");

																	
								if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
								{
									 
										//vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
										vdDisplayErrorMsgResp2("","Initialization","Error");
										return;
								}
								srCommFuncPoint.inInitComm(&srTransRec,srTransRec.usTerminalCommunicationMode);
								srCommFuncPoint.inGetCommConfig(&srTransRec);
								srCommFuncPoint.inSetCommConfig(&srTransRec); 			

						} 	
		}

		inResult = inCPTRead(shHostIndex);
		
		return ;
}


void vdCTOS_DialConfig(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    USHORT usLen;
    int shHostIndex = 1;

//
//	if (strTCT.fSingleComms)	
//    	shHostIndex = 1;
//	else
		shHostIndex = inCTOS_SelectHostSetting();;
		
    if (shHostIndex == -1)
        return;
    
    inResult = inCPTRead(shHostIndex);  
    if (inResult != d_OK)
        return;
    
    inResult = inTCTRead(1);
    if (inResult != d_OK)
        return;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("Dial Up Setting");
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Primary PABX");
        if (wub_strlen(strTCT.szPabx)<=0)
            setLCDPrint(4,DISPLAY_POSITION_LEFT, "[DISABLED]");
        else
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szPabx);

        
        strcpy(strtemp,"New:") ; 
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        memset(strtemp, 0x00, sizeof(strtemp));
        usLen = 10;
        ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
                BolDetachDIALChangeSetting = TRUE;
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
        
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Primary Line");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriTxnPhoneNumber);

            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szPriTxnPhoneNumber,strOut,strlen(strOut));
                strCPT.szPriTxnPhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
            
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Second Line");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecTxnPhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szSecTxnPhoneNumber,strOut,strlen(strOut));
                strCPT.szSecTxnPhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
            
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLEMENT PRI LINE");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriSettlePhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szPriSettlePhoneNumber,strOut,strlen(strOut));
                strCPT.szPriSettlePhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
            
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLEMENT SEC LINE");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecSettlePhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szSecSettlePhoneNumber,strOut,strlen(strOut));
                strCPT.szSecSettlePhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }


	while(1)
	{
	   vduiClearBelow(3);
	   setLCDPrint(3, DISPLAY_POSITION_LEFT, "COMMS BACKUP");
	   if(strCPT.fCommBackUpMode == 0)
		  setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
	   else
		  setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");	 
	   
	   CTOS_LCDTPrintXY(1, 5, "0-DISABLE  1-ENABLE");
	   
	   strcpy(strtemp,"New:") ;
	   CTOS_LCDTPrintXY(1, 7, strtemp);
	   memset(strOut,0x00, sizeof(strOut));
	   ret= shCTOS_GetNum(8, 0x01,	strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
	   if (ret == d_KBD_CANCEL)
		  break;
	   else if(ret == 0) 
		  break;
	   else if(ret == 1)
	   {
		  if (strOut[0]==0x30 || strOut[0]==0x31)
		  {
			 if(strOut[0]==0x30)  
				strCPT.fCommBackUpMode=FALSE;
			 else if(strOut[0]==0x31)
				strCPT.fCommBackUpMode=TRUE;
			 break;
		  }
		  else
		  {
			 vduiWarningSound();
			 setLCDPrint(6, DISPLAY_POSITION_LEFT, "INVALID SELECTION");
			 CTOS_Delay(2000);
		  }
	   }
	}

    inResult = inCPTSave(shHostIndex);      
    inResult = inCPTRead(shHostIndex);      


    /* BDOCLG-00079: BIN VER tel# should be configurable at terminal level - start -- jzg */
		inResult = inBVTRead(1);
		if (inResult != d_OK)
			return;

    while(1)
    {
      vduiClearBelow(3);
      setLCDPrint(3, DISPLAY_POSITION_LEFT, "BINVer Line 1");
//      setLCDPrint(4, DISPLAY_POSITION_LEFT, strBVT.szBINVerPriNum);
    
      strcpy(strtemp,"New:") ; 
      CTOS_LCDTPrintXY(1, 7, strtemp);
      memset(strOut,0x00, sizeof(strOut));
      usLen = 18;
      ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
      if(ret == d_KBD_CANCEL)
        break;
      else if(ret == 0)
        break;
      else if(ret >= 1)
      {
//        memcpy(strBVT.szBINVerPriNum, strOut, strlen(strOut));
//        strBVT.szBINVerPriNum[strlen(strOut)] = 0;
        break;
      }
      if(ret == d_KBD_CANCEL)
        break;
    }
    
    while(1)
    {
      vduiClearBelow(3);
      setLCDPrint(3, DISPLAY_POSITION_LEFT, "BINVer Line 2");
//      setLCDPrint(4, DISPLAY_POSITION_LEFT, strBVT.szBINVerSecNum);
    
      strcpy(strtemp,"New:") ; 
      CTOS_LCDTPrintXY(1, 7, strtemp);
      memset(strOut,0x00, sizeof(strOut));
      usLen = 18;
      ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
      if(ret == d_KBD_CANCEL)
        break;
      else if(ret == 0)
        break;
      else if(ret >= 1)
      {
//        memcpy(strBVT.szBINVerSecNum, strOut, strlen(strOut));
//        strBVT.szBINVerSecNum[strlen(strOut)] = 0;
        break;
      }
      if(ret == d_KBD_CANCEL)
        break;
    }
		
    inResult = inBVTSave(1);      
    /* BDOCLG-00079: BIN VER tel# should be configurable at terminal level - end -- jzg */


			
    if(BolDetachDIALChangeSetting==TRUE)
    {
        srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
        vduiClearBelow(3);
        CTOS_LCDTPrintXY (1,7, "Please Wait     ");       
        CTOS_LCDTPrintXY(1, 8, "Init Modem...     ");

        if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
        {
           
            //vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
            //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
            vdDisplayErrorMsgResp2("Modem","Initialization","Error");
            return;
        }
        
        srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE);
        srCommFuncPoint.inGetCommConfig(&srTransRec);
        srCommFuncPoint.inSetCommConfig(&srTransRec); 

    }
}

void vdCTOS_ModifyEdcSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;


	inResult = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inResult)
		return inResult;
    
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    strHDT.inHostIndex = shHostIndex;

		srTransRec.HDTid = shHostIndex;


    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {     
//    	inResult = inCTOS_MultiAPSaveData(d_IPC_CMD_EDC_SETTING);
      	if(d_OK != inResult)
        	return inResult;
    }
    else
    {
			if (inMultiAP_CheckSubAPStatus() == d_OK)
			{
					inResult = inCTOS_MultiAPGetData();
					if(d_OK != inResult)
							return d_NO;
			
					inResult = inCTOS_MultiAPReloadHost();
					if(d_OK != inResult)
							return d_NO;
			}
    }


	//remove prompt fro selection-Merchant selection is done by inCTOS_TEMPCheckAndSelectMutipleMID
	//	  inRet = inCTOS_CheckAndSelectMutipleMID();
	//	  if(d_OK != inRet)
	//		  return inRet;
	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("EDC SETTING");
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TERMINAL ID");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strMMT[0].szTID);
    
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strMMT[0].szTID, 0x00, sizeof(strMMT[0].szTID));
                memcpy(strMMT[0].szTID, strOut, TERMINAL_ID_BYTES);
                strMMT[0].szTID[TERMINAL_ID_BYTES]=0;
                inMMTSave(strMMT[0].MMTid);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
   if (ret == d_KBD_CANCEL )
        break ;
    }
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "MERCHANT ID");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strMMT[0].szMID);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 15, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strMMT[0].szMID, 0x00, sizeof(strMMT[0].szMID));
                memcpy(strMMT[0].szMID, strOut, MERCHANT_ID_BYTES);
                strMMT[0].szMID[MERCHANT_ID_BYTES]=0;
                inMMTSave(strMMT[0].MMTid);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
       if (ret == d_KBD_CANCEL )
            break ;
    }

    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TPDU");
        wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTempBuf);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 10, 10, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==10)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strHDT.szTPDU, 0x00, sizeof(strHDT.szTPDU)); 
                wub_str_2_hex(strOut,strtemp,TPDU_BYTES);
                memcpy(strHDT.szTPDU, strtemp, TPDU_BYTES/2);
//                inHDTSaveEDCSETTTING(strHDT.inHostIndex);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
       if (ret == d_KBD_CANCEL )
            break ;
    }
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "NII");
        wub_hex_2_str(strHDT.szNII,szTempBuf,2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTempBuf);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 4, 4, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==4)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strHDT.szNII, 0x00, sizeof(strHDT.szNII));
                wub_str_2_hex(strOut, strtemp, NII_BYTES);
                memcpy(strHDT.szNII, strtemp, NII_BYTES/2);
//                inHDTSaveEDCSETTTING(strHDT.inHostIndex);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
       if (ret == d_KBD_CANCEL )
            break ;
        }

    inTCTRead(1);    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "System PWD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szSystemPW);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szSystemPW,0x00,PASSWORD_LEN);
                memcpy(strTCT.szSystemPW,strOut, strlen(strOut));
                strTCT.szSystemPW[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"SYSTEM PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szSystemPW,0x00,PASSWORD_LEN);
                    inTCTSave(1);
                    break;
                }
            }
    }
    if (ret == d_KBD_CANCEL )
        break ;
    }
  
    inTCTRead(1);
    while(1)
    {    
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Engineer PWD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szEngineerPW);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szEngineerPW,0x00,PASSWORD_LEN);
                memcpy(strTCT.szEngineerPW,strOut, strlen(strOut));
                strTCT.szEngineerPW[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"ENGINEER PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szEngineerPW,0x00,PASSWORD_LEN);
                    inTCTSave(1);
                    break;
                }
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
        }

	inTCTRead(1);
    while(1)
	{    
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PM password");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szPMpassword);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szPMpassword,0x00,8);
                memcpy(strTCT.szPMpassword,strOut, strlen(strOut));
                strTCT.szPMpassword[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"ENGINEER PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szPMpassword,0x00,8);
                    inTCTSave(1);
                    break;
                }
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
	}

	inTCTRead(1);
    while(1)
	{    
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "FunKey PWD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szFunKeyPW);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szFunKeyPW,0x00,8);
                memcpy(strTCT.szFunKeyPW,strOut, strlen(strOut));
                strTCT.szFunKeyPW[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"ENGINEER PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szFunKeyPW,0x00,8);
                    inTCTSave(1);
                    break;
                }
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
	}


	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
	inTCTRead(1);
	while(1)
	{    
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "Max CTLS Amount");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szMaxCTLSAmount);

		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 13, 0, d_INPUT_TIMEOUT);
		if(ret == d_KBD_CANCEL)
			break;
		else if(ret == 0)
			break;
		else if(ret >= 1)
		{
			if(strlen(strOut) > 0)
			{
				memset(strTCT.szMaxCTLSAmount, 0, 12);
				memcpy(strTCT.szMaxCTLSAmount, strOut, strlen(strOut));
				strTCT.szMaxCTLSAmount[strlen(strOut)] = 0;
				inTCTSave(1);
				break;
			}
		}
		if (ret == d_KBD_CANCEL)
			break ;
	}
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


	//Issue# 000166 - start -- jzg
/*
	inTCTRead(1);
	vduiClearBelow(3);
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Inst TC Upload");
	if (strTCT.fInstTCUpload == TRUE)
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "ENABLED");
	else
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "DISABLED");

	setLCDPrint(6, DISPLAY_POSITION_RIGHT, "[O] - ENABLE ");
	setLCDPrint(7, DISPLAY_POSITION_RIGHT, "[X] - DISABLE");

	do
	{
		vduiClearBelow(8);

		key = WaitKey(60);
		switch (key)
		{
			case d_KBD_ENTER:
				strTCT.fInstTCUpload = 1;
				break;
			case d_KBD_CANCEL:
				strTCT.fInstTCUpload = 0;
				break;
		}
	}
	while ((key != d_KBD_ENTER) && (key != d_KBD_CANCEL));
	inTCTSave(1);
*/
	//Issue# 000166 - end -- jzg
#if 0
  inTCTRead(1);
  while(1)
  {
     vduiClearBelow(3);
     setLCDPrint(3, DISPLAY_POSITION_LEFT, "ISO Logging");
     if(strTCT.fISOLogging == FALSE)
           setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
     else
           setLCDPrint(4, DISPLAY_POSITION_LEFT, "1"); 		
     
     CTOS_LCDTPrintXY(1, 5, "0-DISABLE 	 1-ENABLE");
     
     strcpy(strtemp,"New:");
     CTOS_LCDTPrintXY(1, 7, strtemp);
     memset(strOut,0x00, sizeof(strOut));
     ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
     if(ret == d_KBD_CANCEL)
        break;
     else if(ret == 0)
        break;
     else if(ret == 1)
     {
        if (strOut[0] == 0x30 || strOut[0] == 0x31)
        {
           if(strOut[0] == 0x30) 	 
              strTCT.fISOLogging = FALSE;
           else if(strOut[0] == 0x31)
              strTCT.fISOLogging = TRUE;
           
           inTCTSave(1);
           break;
        } 
        else
        {
           vduiWarningSound();
           setLCDPrint(6, DISPLAY_POSITION_LEFT, "INVALID SELECTION");
           CTOS_Delay(2000);
        }
     }
     if(ret == d_KBD_CANCEL)
        break;
  }
  inTCTSave(1);
  
  inTCTRead(1);
  while(1)
  { 	
     vduiClearBelow(3);
     setLCDPrint(3, DISPLAY_POSITION_LEFT, "ISO LOG TRANS LIMIT");
     memset(szInputBuf, 0x00, sizeof(szInputBuf));
     sprintf(szInputBuf, "%d", strTCT.inTxnLogLimit);
     setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
     
     strcpy(strtemp,"New:") ;
     CTOS_LCDTPrintXY(1, 7, strtemp);
     memset(strOut,0x00, sizeof(strOut));
     ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
     if(ret == d_KBD_CANCEL )
        break;
     else if(ret == 0)
        break;
     else if(ret >= 1)
     {
        strTCT.inTxnLogLimit = atoi(strOut);
        break;
     } 	
     if(ret == d_KBD_CANCEL)
        break;
  } 
  inTCTSave(1);
#endif
            
	return d_OK;
}

void vdCTOS_DeleteBatch(void)
{
	int 		shHostIndex = 1;
	int 		inResult,inRet;
	ACCUM_REC srAccumRec;
	STRUCT_FILE_SETTING strFile;

	
	vduiLightOn();				  
	vdDebug_LogPrintf("vdCTOS_DeleteBatch");	

	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DELETE_BATCH);
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

	inResult = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inResult)
		return;
			
	inResult = vduiAskConfirmContinue(1);
	if(inResult == d_OK)
	{

		if(CN_TRUE == strMMT[0].fMustSettFlag)
		{
			strMMT[0].fMustSettFlag = CN_FALSE;
			inMMTSave(strMMT[0].MMTid);
		}
	
		// delete batch where hostid and mmtid is matcj
		inDatabase_BatchDelete();

		memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
		memset(&strFile,0,sizeof(strFile));
		vdCTOS_GetAccumName(&strFile, &srAccumRec);
	
		if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
		}
		//create the accum file
		memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
		inCTOS_ReadAccumTotal(&srAccumRec);
		
		inCTOS_DeleteBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);

		inMyFile_ReversalDelete();

		inMyFile_AdviceDelete();
		
		inMyFile_TCUploadDelete();
	
		CTOS_LCDTClearDisplay();
		setLCDPrint(5, DISPLAY_POSITION_CENTER, "CLEAR BATCH");
		setLCDPrint(6, DISPLAY_POSITION_CENTER, "RECORD DONE");
		CTOS_Delay(1000); 
	}				 
}


int vdCTOS_DeleteReversal(void)
{
    CHAR szFileName[d_BUFF_SIZE];
    int         shHostIndex = 1;
    int         inResult,inRet;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    
    vduiLightOn();                

	//inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	//if(d_OK != inRet)
		//return inRet;

    //shHostIndex = inCTOS_SelectHostSettingWithIndicator(2);	
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DELETE_REVERSAL);
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

	//remove prompt fro selection-Merchant selection is done by inCTOS_TEMPCheckAndSelectMutipleMID
	inRet = inCTOS_CheckAndSelectMutipleMID();
		  if(d_OK != inRet)
			  return inRet;
	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    memset(szFileName,0,sizeof(szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
                    , strHDT.szHostLabel
                    , strHDT.inHostIndex
                    , srTransRec.MITid);

    vdDebug_LogPrintf("Rever Name %s",szFileName);
    
    if((inResult = inMyFile_CheckFileExist(szFileName)) < 0)
    {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        vduiClearBelow(2);
        vdDisplayErrorMsg(1, 8, "EMPTY REVERSAL");
        return ;
    }
            
    inResult = vduiAskConfirmContinue(2);
    if(inResult == d_OK)
    {
        inMyFile_ReversalDelete();
    
        CTOS_LCDTClearDisplay();
        setLCDPrint(5, DISPLAY_POSITION_CENTER, "CLEAR REVERSAL");
        setLCDPrint(6, DISPLAY_POSITION_CENTER, "RECORD DONE");
        CTOS_Delay(1000); 
    }                
}

void vdCTOS_PrintEMVTerminalConfig(void)
{
    CTOS_LCDTClearDisplay();
    vdPrintEMVTags();
    
    return;
}

void vdCTOSS_PrintTerminalConfig(void)
{
    CTOS_LCDTClearDisplay();
    #if 0
    vdPrintTerminalConfig();
	#else
    vdPrintTerminalConfig();
	#endif
    return;
}


void vdCTOS_ThemesSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;
	// patrick test 20150115 start
	char szDatabaseName[100+1];
	strcpy(szDatabaseName, "DMENGTHAI.S3DB");

//    inRet = inTCTRead(1);  
//    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "THEMES SELECTION");
//        if(strTCT.byRS232DebugPort == 0)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
//        if(strTCT.byRS232DebugPort == 1)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
//        if(strTCT.byRS232DebugPort == 2)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
//        if(strTCT.byRS232DebugPort == 8)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "8");  
        
        CTOS_LCDTPrintXY(1, 5, "0-SPRING       1-SUMMER");
//		CTOS_LCDTPrintXY(1, 6, "2-AUTUMN       3-WINTER");

//        CTOS_LCDTPrintXY(1, 6, "2-COM2    8-USB");
        
//        strcpy(strtemp,"New:") ;
//        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x38)
            {
                 if(strOut[0] == 0x31)
                 {
					 // patrick test 20150115 start
                     CTOS_LCDTTFSelect("tahoma.ttf", 0);
					 CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

					 CTOS_PrinterTTFSelect("tahoma.ttf", 0);
					 inCTOSS_SetERMFontType("tahoma.ttf", 0);
					 CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

					 inSetLanguageDatabase(szDatabaseName);
					 if ((strTCT.byTerminalType%2) == 0)
					 {
						 vdCTOSS_CombineMenuBMP("BG_SUMMER.BMP");
						 vdBackUpWhiteBMP("SUMMER.BMP","WHITE.BMP");
						 vdBackUpWhiteBMP("SUMMERTOUCH.BMP","menutouch.bmp");
						 vdBackUpWhiteBMP("SUMMERITEM.BMP","menuitem.bmp");
					 }
					 CTOS_LCDTClearDisplay();
//					 inCTOSS_SetALLApFont("tahoma.ttf");
//					 strTCT.inThemesType = 1;
					 inTCTSave(1);
					 break;
					 // patrick test 20150115 end
                 }
                 if(strOut[0] == 0x30)
                 {
					 // patrick test 20150115 start
					 CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
					 CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);					 

					 CTOS_PrinterTTFSelect(d_FONT_DEFAULT_TTF, 0);
					 inCTOSS_SetERMFontType(d_FONT_DEFAULT_TTF, 0);
					 CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);

					 inSetLanguageDatabase("");
					 if ((strTCT.byTerminalType%2) == 0)
					 {
						 vdCTOSS_CombineMenuBMP("BG_SPRING.BMP");
						 vdBackUpWhiteBMP("SPRING.BMP","WHITE.BMP");
						 vdBackUpWhiteBMP("SPRINGTOUCH.BMP","menutouch.bmp");
						 vdBackUpWhiteBMP("SPRINGITEM.BMP","menuitem.bmp");
					 }
					 CTOS_LCDTClearDisplay();
//					 inCTOSS_SetALLApFont(d_FONT_DEFAULT_TTF);
//					 strTCT.inThemesType = 0;
					 inTCTSave(1);
					 break;
					 // patrick test 20150115 end
                 }
				 /*
	                 if(strOut[0] == 0x32)
	                 {
	                        strTCT.byRS232DebugPort = 2;
	                 }
	                 if(strOut[0] == 0x38)
	                 {
	                        strTCT.byRS232DebugPort = 8;
	                 }
	                
	                 inRet = inTCTSave(1);
	                 
	                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
				*/
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"THEMES");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

void vdCTOS_Debugmode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "DEBUG MODE");
        if(strTCT.byRS232DebugPort == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byRS232DebugPort == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        if(strTCT.byRS232DebugPort == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
        if(strTCT.byRS232DebugPort == 8)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "8");  
        
        CTOS_LCDTPrintXY(1, 5, "0-NO        1-COM1");
        CTOS_LCDTPrintXY(1, 6, "2-COM2    8-USB");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x38)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.byRS232DebugPort = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.byRS232DebugPort = 0;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strTCT.byRS232DebugPort = 2;
                 }
                 if(strOut[0] == 0x38)
                 {
                        strTCT.byRS232DebugPort = 8;
                 }
                
                 inRet = inTCTSave(1);
                 
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}


void vdCTOSS_SelectPinpadType(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PINPAD TYPE");
        if(strTCT.byPinPadType == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byPinPadType == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        if(strTCT.byPinPadType == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
		if(strTCT.byPinPadType == 3)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "3");
  
        
        CTOS_LCDTPrintXY(1, 5, "0-None	1-PCI100");
        CTOS_LCDTPrintXY(1, 6, "2-OTHER 3-V3P");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.byPinPadType = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.byPinPadType = 0;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strTCT.byPinPadType = 2;
                 }
				 if(strOut[0] == 0x33)
                 {
                        strTCT.byPinPadType = 3;
                 }
 
                
                 inRet = inTCTSave(1);
                 
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"PINPAD TYPE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

int inCTOSS_GetCtlsMode(void)
{
	//inTCTRead(1);

	return strTCT.byCtlsMode;
}

void vdCTOSS_CtlsMode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "CTLS MODE");
        if(strTCT.byCtlsMode == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byCtlsMode == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        if(strTCT.byCtlsMode == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
		if(strTCT.byCtlsMode == 3)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "3");
 
        
        CTOS_LCDTPrintXY(1, 5, "0-Disable 1-Internal");
        CTOS_LCDTPrintXY(1, 6, "2-External 3-V3 CTLS");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.byCtlsMode = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.byCtlsMode = 0;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strTCT.byCtlsMode = 2;
                 }
				 if(strOut[0] == 0x33)
                 {
                        strTCT.byCtlsMode = 3;
                 } 
                
                 inRet = inTCTSave(1);
                 
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}


void vdCTOS_DemoMode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    //int inResult = 0;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    inTCTRead(1);   
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "DEMO MODE");
        if(strTCT.fDemo == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.fDemo == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");     
        
        CTOS_LCDTPrintXY(1, 5, "0-DISABLE      1-ENABLE");
        
   
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            //check host num
            inNumOfHost = inHDTNumRecord();
            
            vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
            for(inNum =1 ;inNum <= inNumOfHost; inNum++)
            {
                if(inHDTRead(inNum) == d_OK)
                {
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
                        else    // delete batch where hostid and mmtid is match  
                        {
                            strMMT[0].HDTid = strHDT.inHostIndex;
                            strMMT[0].MITid = inLoop;
                            inDatabase_BatchDeleteHDTidMITid();
                            vdDebug_LogPrintf("[inDatabase_BatchDelete]-Mitid[%d]strHDT.inHostIndex[%d]", inLoop,strHDT.inHostIndex);
                            
                            memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
                            memset(&strFile,0,sizeof(strFile));
                            srTransRec.MITid = inLoop;
                            vdCTOS_GetAccumName(&strFile, &srAccumRec);
                            
                            if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
                            {
                                vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
                            }

                            inCTOS_DeleteBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);

                            inDatabase_InvoiceNumDelete(srTransRec.HDTid, srTransRec.MITid);
                            inMyFile_ReversalDelete();

                            inMyFile_AdviceDelete();
                            
                            inMyFile_TCUploadDelete();

//							inMyFile_TransLogDelete();

//							inDatabase_SMACFooterDeleteAll();

                        }
                    }
                }
                else
                    continue;

            }
            
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fDemo = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fDemo = 0;
                 }

                 inTCTSave(1);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}


void vdCTOS_TipAllowd(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
		int inNumOfHost=0,i=0;

    inTCTRead(1);    
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Tip Allowed");
        if(strTCT.fTipAllowFlag == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.fTipAllowFlag == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");     
        
        CTOS_LCDTPrintXY(1, 5, "0-DISABLE      1-ENABLE");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
             //0722
                 inNumOfHost = inHDTNumRecord();

                 inResult = -1;
						     for (i=0; i<= inNumOfHost; i++){
								 	inHDTRead(i);
									srTransRec.MITid = 1;//have to modify for multiple MID

									if (strcmp(strHDT.szAPName,"V5S_RBANK") != 0)
										continue;

//									inResult = inCTOS_ChkBatchEmpty2();

									if (inResult == d_OK)
										break;
									
						     }

								 if (inResult == d_OK){
										vduiWarningSound();
									 	vduiClearBelow(5);
									 	vduiDisplayStringCenter(6,"   BATCH NOT");
									 	vduiDisplayStringCenter(7,"   EMPTY");
									 	CTOS_Delay(2000);
										break;
								 	}

                 
						 //0722
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fTipAllowFlag = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fTipAllowFlag = 0;
                 }

                 inTCTSave(1);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}



void vdCTOS_GPRSSetting(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    short inRtn;
    USHORT shMaxLen = 30;
    
    inResult = inTCPRead(1);
    if (inResult != d_OK)
        return;
        
    CTOS_LCDTClearDisplay();
    vdDispTitleString("GPRS SETUP");
    
    while(1)
    {
		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szAPN);
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
 
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "APN");
		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szAPN);
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
		
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
		shMaxLen = 30;
        ret = InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
		if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szAPN, strOut,strlen(strOut));
            strTCP.szAPN[strlen(strOut)]=0;
            inResult = inTCPSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }               

    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS USER NAME");
		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szUserName);
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);

        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
		shMaxLen = 30;
        ret = InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szUserName, strOut,strlen(strOut));
            strTCP.szUserName[strlen(strOut)]=0;
            inResult = inTCPSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }               

    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS PASSWORD");
		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szPassword);
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
        

        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
		shMaxLen = 30;
        ret = InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szPassword, strOut,strlen(strOut));
            strTCP.szPassword[strlen(strOut)]=0;
            inResult = inTCPSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }   

    if(BolDetachGPRSChangeSetting==TRUE)
    {
            srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
            clearLine(3);
            clearLine(4);
            clearLine(5);
            clearLine(6);
            clearLine(7);
            clearLine(8);
            CTOS_LCDTPrintXY (1,7, "Please Wait     ");       
            CTOS_LCDTPrintXY(1, 8, "Init GPRS...     ");
            if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
            {
               
                //vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
                //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
                vdDisplayErrorMsgResp2("GPRS","Initialization","Error");
                return;
            }
                          
            srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE);
            srCommFuncPoint.inGetCommConfig(&srTransRec);
            srCommFuncPoint.inSetCommConfig(&srTransRec);

    }

    return ;
}

/* delete a Char in string */
void DelCharInStr(char *str, char c, int flag)
{
    int i,l;

    l=strlen(str);

    if (flag == STR_HEAD)
    {
        for (i=0;i<l&&str[i]==c;i++);
        if (i>0) strcpy(str,str+i);
    }

    if (flag == STR_BOTTOM)
    {
        for (i=l-1;i>=0&&str[i]==c;i--);
        str[i+1]='\0';
    }

    if (flag == STR_ALL)
    {
        for (i=0;i<l;i++)
            if (str[i]==c)
            {
                strcpy(str+i,str+i+1);
                i--;
            }
    }
}



void vdConfigEditAddHeader(void)
{
    USHORT inRet, inSelIndex=7;
    BYTE strBuff[50];
    BYTE strActualValue[50];
    USHORT inResult;
    
    STR *keyboardLayoutEnglish[]={" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
    "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>_",
    ".!@#$%^&*()-"};
    //numeric keyboard (0123456789) and radix point '.'
    STR *keyboardLayoutNumberWithRadixPoint[]={"0", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "", "."};
    //numeric keyboard (0123456789) without radix point
    STR *keyboardLayoutNumber[]={"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "", ""};
    
    //Parameter for CTOS_UIKeypad:
    //1. usX: 4 => Cursor horizontal position at 4
    //2. usY: 2 => Cursor vertical position at 2
    //3. *pcaKeyboardLayout[]: keyboardLayoutEnglish => use english keyboard
    //layout defined at the beginning of the program
    //4. ucCursorBlinkInterval: 40 => Cursor blink at 400ms interval
    //5. ucDelayToNextChar: 80 => Move Cursor to next character if user did not
    //press any key within 800ms
    //6. boEnableCursorMove: d_TRUE => Allow user to move cursor by F3 and F4 key
    //7. boOneRadixPointOnly: d_FALSE => Do not check radix point
    //8. ucDigitAfterRadixPoint: 0 => Do not limit digits after radix point
    //9. bPasswordMask: 0 => Do not mask user data.
    //10. caData[]: baBuff => Pointer to store the user data retrieved
    //11. ucDataLen: 9 => baBuff has 9 bytes. User may input 8 bytes max.
    
    inResult = inPCTRead(1);
     
//---------HEADER LINE 1---------------------//
    vduiClearBelow(2);  
    vduiDisplayStringCenter(1, "ADDRESS HEADER");
    CTOS_LCDTPrintXY(1, 2, "OLD HEADER LINE1:           ");
    CTOS_LCDTPrintXY(1, 3, strPCT.szRctHdr1);
    CTOS_LCDTPrintXY(1, 4, &strPCT.szRctHdr1[16]);
    CTOS_LCDTPrintXY(1, 5, &strPCT.szRctHdr1[32]);
    CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE1:");
    memset(strBuff,0x00, sizeof(strBuff));  
    memset(strActualValue,0x00, sizeof(strActualValue));
    
    inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);

    if(inRet==d_OK)
    {
        if (strlen(strBuff)>0)
        {
            memcpy(strActualValue,strBuff,strlen(strBuff));

            if (strlen(strBuff)>=15)
            {
                vduiClearBelow(2);  
                vduiDisplayStringCenter(4,"CONTINUE INPUT");
                vduiDisplayStringCenter(5,"MORE TEXT?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                while(1)
                {
                    inRet=struiGetchWithTimeOut();
                    if (inRet==d_KBD_ENTER)
                    {
                        vduiClearBelow(2);  
                        CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE1:");
                        memset(strBuff,0x00, sizeof(strBuff));
                        
                        vduiClearBelow(7);  
                        CTOS_LCDTPrintXY(1, 7, strActualValue);
                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                        if(inRet==d_OK)
                        {
                            if (strlen(strBuff)>0)
                            {
                                memcpy(&strActualValue[15],strBuff,strlen(strBuff));
                                if (strlen(strBuff)>=15)
                                {
                                    vduiClearBelow(2);  
                                    vduiDisplayStringCenter(4,"CONTINUE INPUT");
                                    vduiDisplayStringCenter(5,"MORE TEXT?");
                                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                                    inRet=struiGetchWithTimeOut();
                                    if (inRet==d_KBD_ENTER)
                                    {
                                        vduiClearBelow(2);
                                        CTOS_LCDTPrintXY(1, 5, "ENTR NEW LINE1:");
                                        memset(strBuff,0x00, sizeof(strBuff));                                                                  
                                        CTOS_LCDTPrintXY(1, 6, strActualValue);
                                        CTOS_LCDTPrintXY(1, 7, &strActualValue[16]);
                                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                                        if(inRet==d_OK)
                                        {    
                                           memcpy(&strActualValue[30],strBuff,strlen(strBuff));
                                        }
                                        break;
                                    }
                                    else if (inRet==d_KBD_CANCEL)
                                        break;
                                    else
                                            vduiWarningSound();
                                }
                            }
                            else
                                break;
                                                
                        }
                        break;
                    }
                    else if (inRet==d_KBD_CANCEL)
                        break;
                    else
                        vduiWarningSound();
                }
            }
            memset(strPCT.szRctHdr1,0x00, sizeof(strPCT.szRctHdr1));    
            memcpy(strPCT.szRctHdr1,strActualValue,strlen(strActualValue));
            
        }
        else
        {
            vduiClearBelow(3);  
            vduiDisplayStringCenter(4,"NO VALUE ENTERED");
            vduiDisplayStringCenter(5,"SAVE EMPTY?");
            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
            while(1)
            {
                inRet=struiGetchWithTimeOut();
                if (inRet==d_KBD_ENTER)
                {
                    memset(strPCT.szRctHdr1,0x00, sizeof(strPCT.szRctHdr1));    
                    break;
                }
                else if (inRet==d_KBD_CANCEL)
                    break;
                else
                    vduiWarningSound();
            }
        }
    }
        
   
    //---------HEADER LINE 2---------------------//    
    vduiClearBelow(2);  
    vduiDisplayStringCenter(1, "ADDRESS HEADER");
    CTOS_LCDTPrintXY(1, 2, "OLD HEADER LINE2:           ");
    CTOS_LCDTPrintXY(1, 3, strPCT.szRctHdr2);
    CTOS_LCDTPrintXY(1, 4, &strPCT.szRctHdr2[16]);
    CTOS_LCDTPrintXY(1, 5, &strPCT.szRctHdr2[32]);
    CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE2:");
    memset(strBuff,0x00, sizeof(strBuff));  
    memset(strActualValue,0x00, sizeof(strActualValue));
    
    inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);

    if(inRet==d_OK)
    {
        if (strlen(strBuff)>0)
        {
            memcpy(strActualValue,strBuff,strlen(strBuff));

            if (strlen(strBuff)>=15)
            {
                vduiClearBelow(2);  
                vduiDisplayStringCenter(4,"CONTINUE INPUT");
                vduiDisplayStringCenter(5,"MORE TEXT?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                while(1)
                {
                    inRet=struiGetchWithTimeOut();
                    if (inRet==d_KBD_ENTER)
                    {
                        vduiClearBelow(2);  
                        CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE2:");
                        memset(strBuff,0x00, sizeof(strBuff));
                        
                        vduiClearBelow(7);  
                        CTOS_LCDTPrintXY(1, 7, strActualValue);
                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                        if(inRet==d_OK)
                        {
                            if (strlen(strBuff)>0)
                            {
                                memcpy(&strActualValue[15],strBuff,strlen(strBuff));
                                if (strlen(strBuff)>=15)
                                {
                                    vduiClearBelow(2);  
                                    vduiDisplayStringCenter(4,"CONTINUE INPUT");
                                    vduiDisplayStringCenter(5,"MORE TEXT?");
                                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                                    inRet=struiGetchWithTimeOut();
                                    if (inRet==d_KBD_ENTER)
                                    {
                                        vduiClearBelow(2);
                                        CTOS_LCDTPrintXY(1, 5, "ENTR NEW LINE2:");
                                        memset(strBuff,0x00, sizeof(strBuff));                                                                  
                                        CTOS_LCDTPrintXY(1, 6, strActualValue);
                                        CTOS_LCDTPrintXY(1, 7, &strActualValue[16]);
                                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                                        if(inRet==d_OK)
                                        {    
                                           memcpy(&strActualValue[30],strBuff,strlen(strBuff));
                                        }
                                        break;
                                    }
                                    else if (inRet==d_KBD_CANCEL)
                                        break;
                                    else
                                        vduiWarningSound();
                                }
                            }
                            else
                                break;
                                                
                        }
                        break;
                    }
                    else if (inRet==d_KBD_CANCEL)
                        break;
                    else
                        vduiWarningSound();
                }
            }
            memset(strPCT.szRctHdr2,0x00, sizeof(strPCT.szRctHdr2));    
            memcpy(strPCT.szRctHdr2,strActualValue,strlen(strActualValue));
            
        }
        else
        {
            vduiClearBelow(3);  
            vduiDisplayStringCenter(4,"NO VALUE ENTERED");
            vduiDisplayStringCenter(5,"SAVE EMPTY?");
            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
            while(1)
            {
                inRet=struiGetchWithTimeOut();
                if (inRet==d_KBD_ENTER)
                {
                    memset(strPCT.szRctHdr2,0x00, sizeof(strPCT.szRctHdr2));    
                    break;
                }
                else if (inRet==d_KBD_CANCEL)
                    break;
                else
                    vduiWarningSound();
            }
        }
    }    
        
        
   //---------HEADER LINE 3---------------------//    
    vduiClearBelow(2);  
    vduiDisplayStringCenter(1, "ADDRESS HEADER");
    CTOS_LCDTPrintXY(1, 2, "OLD HEADER LINE3:           ");
    CTOS_LCDTPrintXY(1, 3, strPCT.szRctHdr3);
    CTOS_LCDTPrintXY(1, 4, &strPCT.szRctHdr3[16]);
    CTOS_LCDTPrintXY(1, 5, &strPCT.szRctHdr3[32]);
    CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE3:");
    memset(strBuff,0x00, sizeof(strBuff));  
    memset(strActualValue,0x00, sizeof(strActualValue));
    
    inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);

    if(inRet==d_OK)
    {
        if (strlen(strBuff)>0)
        {
            memcpy(strActualValue,strBuff,strlen(strBuff));

            if (strlen(strBuff)>=15)
            {
                vduiClearBelow(2);  
                vduiDisplayStringCenter(4,"CONTINUE INPUT");
                vduiDisplayStringCenter(5,"MORE TEXT?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                while(1)
                {
                    inRet=struiGetchWithTimeOut();
                    if (inRet==d_KBD_ENTER)
                    {
                        vduiClearBelow(2);  
                        CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE3:");
                        memset(strBuff,0x00, sizeof(strBuff));
                        
                        vduiClearBelow(7);  
                        CTOS_LCDTPrintXY(1, 7, strActualValue);
                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                        if(inRet==d_OK)
                        {
                            if (strlen(strBuff)>0)
                            {
                                memcpy(&strActualValue[15],strBuff,strlen(strBuff));
                                if (strlen(strBuff)>=15)
                                {
                                    vduiClearBelow(2);  
                                    vduiDisplayStringCenter(4,"CONTINUE INPUT");
                                    vduiDisplayStringCenter(5,"MORE TEXT?");
                                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                                    inRet=struiGetchWithTimeOut();
                                    if (inRet==d_KBD_ENTER)
                                    {
                                        vduiClearBelow(2);
                                        CTOS_LCDTPrintXY(1, 5, "ENTR NEW LINE3:");
                                        memset(strBuff,0x00, sizeof(strBuff));                                                                  
                                        CTOS_LCDTPrintXY(1, 6, strActualValue);
                                        CTOS_LCDTPrintXY(1, 7, &strActualValue[16]);
                                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                                        if(inRet==d_OK)
                                        {    
                                           memcpy(&strActualValue[30],strBuff,strlen(strBuff));
                                        }
                                        break;
                                    }
                                    else if (inRet==d_KBD_CANCEL)
                                        break;
                                    else
                                            vduiWarningSound();
                                }
                            }
                            else
                                break;
                                                
                        }
                        break;
                    }
                    else if (inRet==d_KBD_CANCEL)
                        break;
                    else
                        vduiWarningSound();
                }
            }
            memset(strPCT.szRctHdr3,0x00, sizeof(strPCT.szRctHdr3));    
            memcpy(strPCT.szRctHdr3,strActualValue,strlen(strActualValue));
            
        }
        else
        {
            vduiClearBelow(3);  
            vduiDisplayStringCenter(4,"NO VALUE ENTERED");
            vduiDisplayStringCenter(5,"SAVE EMPTY?");
            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
            while(1)
            {
                inRet=struiGetchWithTimeOut();
                if (inRet==d_KBD_ENTER)
                {
                    memset(strPCT.szRctHdr3,0x00, sizeof(strPCT.szRctHdr3));    
                    break;
                }
                else if (inRet==d_KBD_CANCEL)
                    break;
                else
                    vduiWarningSound();
            }
        }
    }         
        
      
        
    //---------HEADER LINE 4---------------------//    
    vduiClearBelow(2);  
    vduiDisplayStringCenter(1, "ADDRESS HEADER");
    CTOS_LCDTPrintXY(1, 2, "OLD HEADER LINE4:           ");
    CTOS_LCDTPrintXY(1, 3, strPCT.szRctHdr4);
    CTOS_LCDTPrintXY(1, 4, &strPCT.szRctHdr4[16]);
    CTOS_LCDTPrintXY(1, 5, &strPCT.szRctHdr4[32]);
    CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE4:");
    memset(strBuff,0x00, sizeof(strBuff));  
    memset(strActualValue,0x00, sizeof(strActualValue));
    
    inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);

    if(inRet==d_OK)
    {
        if (strlen(strBuff)>0)
        {
            memcpy(strActualValue,strBuff,strlen(strBuff));

            if (strlen(strBuff)>=15)
            {
                vduiClearBelow(2);  
                vduiDisplayStringCenter(4,"CONTINUE INPUT");
                vduiDisplayStringCenter(5,"MORE TEXT?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                while(1)
                {
                    inRet=struiGetchWithTimeOut();
                    if (inRet==d_KBD_ENTER)
                    {
                        vduiClearBelow(2);  
                        CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE4:");
                        memset(strBuff,0x00, sizeof(strBuff));
                        
                        vduiClearBelow(7);  
                        CTOS_LCDTPrintXY(1, 7, strActualValue);
                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                        if(inRet==d_OK)
                        {
                            if (strlen(strBuff)>0)
                            {
                                memcpy(&strActualValue[15],strBuff,strlen(strBuff));
                                if (strlen(strBuff)>=15)
                                {
                                    vduiClearBelow(2);  
                                    vduiDisplayStringCenter(4,"CONTINUE INPUT");
                                    vduiDisplayStringCenter(5,"MORE TEXT?");
                                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                                    inRet=struiGetchWithTimeOut();
                                    if (inRet==d_KBD_ENTER)
                                    {
                                        vduiClearBelow(2);
                                        CTOS_LCDTPrintXY(1, 5, "ENTR NEW LINE4:");
                                        memset(strBuff,0x00, sizeof(strBuff));                                                                  
                                        CTOS_LCDTPrintXY(1, 6, strActualValue);
                                        CTOS_LCDTPrintXY(1, 7, &strActualValue[16]);
                                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                                        if(inRet==d_OK)
                                        {    
                                           memcpy(&strActualValue[30],strBuff,strlen(strBuff));
                                        }
                                        break;
                                    }
                                    else if (inRet==d_KBD_CANCEL)
                                        break;
                                    else
                                            vduiWarningSound();
                                }
                            }
                            else
                                break;
                                                
                        }
                        break;
                    }
                    else if (inRet==d_KBD_CANCEL)
                        break;
                    else
                        vduiWarningSound();
                }
            }
            memset(strPCT.szRctHdr4,0x00, sizeof(strPCT.szRctHdr4));    
            memcpy(strPCT.szRctHdr4,strActualValue,strlen(strActualValue));
            
        }
        else
        {
            vduiClearBelow(3);  
            vduiDisplayStringCenter(4,"NO VALUE ENTERED");
            vduiDisplayStringCenter(5,"SAVE EMPTY?");
            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
            while(1)
            {
                inRet=struiGetchWithTimeOut();
                if (inRet==d_KBD_ENTER)
                {
                    memset(strPCT.szRctHdr4,0x00, sizeof(strPCT.szRctHdr4));    
                    break;
                }
                else if (inRet==d_KBD_CANCEL)
                    break;
                else
                    vduiWarningSound();
            }
        }
    }   
        
        
        
   //---------HEADER LINE 5---------------------//    
    vduiClearBelow(2);  
    vduiDisplayStringCenter(1, "ADDRESS HEADER");
    CTOS_LCDTPrintXY(1, 2, "OLD HEADER LINE5:           ");
    CTOS_LCDTPrintXY(1, 3, strPCT.szRctHdr5);
    CTOS_LCDTPrintXY(1, 4, &strPCT.szRctHdr5[16]);
    CTOS_LCDTPrintXY(1, 5, &strPCT.szRctHdr5[32]);
    CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE5:");
    memset(strBuff,0x00, sizeof(strBuff));  
    memset(strActualValue,0x00, sizeof(strActualValue));
    
    inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);

    if(inRet==d_OK)
    {
        if (strlen(strBuff)>0)
        {
            memcpy(strActualValue,strBuff,strlen(strBuff));

            if (strlen(strBuff)>=15)
            {
                vduiClearBelow(2);  
                vduiDisplayStringCenter(4,"CONTINUE INPUT");
                vduiDisplayStringCenter(5,"MORE TEXT?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                while(1)
                {
                    inRet=struiGetchWithTimeOut();
                    if (inRet==d_KBD_ENTER)
                    {
                        vduiClearBelow(2);  
                        CTOS_LCDTPrintXY(1, 6, "ENTR NEW LINE5:");
                        memset(strBuff,0x00, sizeof(strBuff));
                        
                        vduiClearBelow(7);  
                        CTOS_LCDTPrintXY(1, 7, strActualValue);
                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                        if(inRet==d_OK)
                        {
                            if (strlen(strBuff)>0)
                            {
                                memcpy(&strActualValue[15],strBuff,strlen(strBuff));
                                if (strlen(strBuff)>=15)
                                {
                                    vduiClearBelow(2);  
                                    vduiDisplayStringCenter(4,"CONTINUE INPUT");
                                    vduiDisplayStringCenter(5,"MORE TEXT?");
                                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                                    inRet=struiGetchWithTimeOut();
                                    if (inRet==d_KBD_ENTER)
                                    {
                                        vduiClearBelow(2);
                                        CTOS_LCDTPrintXY(1, 5, "ENTR NEW LINE5:");
                                        memset(strBuff,0x00, sizeof(strBuff));                                                                  
                                        CTOS_LCDTPrintXY(1, 6, strActualValue);
                                        CTOS_LCDTPrintXY(1, 7, &strActualValue[16]);
                                        inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,strBuff, 16);
                                        if(inRet==d_OK)
                                        {    
                                           memcpy(&strActualValue[30],strBuff,strlen(strBuff));
                                        }
                                        break;
                                    }
                                    else if (inRet==d_KBD_CANCEL)
                                        break;
                                    else
                                            vduiWarningSound();
                                }
                            }
                            else
                                break;
                                                
                        }
                        break;
                    }
                    else if (inRet==d_KBD_CANCEL)
                        break;
                    else
                        vduiWarningSound();
                }
            }
            memset(strPCT.szRctHdr5,0x00, sizeof(strPCT.szRctHdr5));    
            memcpy(strPCT.szRctHdr5,strActualValue,strlen(strActualValue));
            
        }
        else
        {
            vduiClearBelow(3);  
            vduiDisplayStringCenter(4,"NO VALUE ENTERED");
            vduiDisplayStringCenter(5,"SAVE EMPTY?");
            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
            while(1)
            {
                inRet=struiGetchWithTimeOut();
                if (inRet==d_KBD_ENTER)
                {
                    memset(strPCT.szRctHdr5,0x00, sizeof(strPCT.szRctHdr5));    
                    break;
                }
                else if (inRet==d_KBD_CANCEL)
                    break;
                else
                    vduiWarningSound();
            }
        }
    }         
        
    inPCTSave(1);
}

void vdCTOSS_DownloadMode(void)
{
	CTOS_LCDTClearDisplay();
	CTOS_EnterDownloadMode();

	inCTOS_KillALLSubAP();
	
	exit(0);
	return ;
}

void vdCTOSS_CheckMemory(void)
{
	vdCTOSS_GetMemoryStatus("MEMORY");
	return ;
}

/* ==========================================================================
 *
 * FUNCTION NAME: SetRTC
 *
 * DESCRIPTION:   Use this function to set the real-time clock's data and time.
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
//Host date & time synchronization fix - start -- jzg
void CTOSS_SetRTC(void)
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
//	  sprintf(babuff,"%02d",SetRTC.bDoW);
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

//Host date & time synchronization fix - end -- jzg

#if 0
//gcitra
int inCTOS_DialBackupConfig(int shHostIndex)
{
	int inResult;

    vdDebug_LogPrintf("vdCTOS_DialBackupConfig, shHostIndex: %d", shHostIndex);

	vduiClearBelow(3);
	CTOS_LCDTPrintXY (1,7, "Please Wait 	"); 	  
	CTOS_LCDTPrintXY(1, 8, "Init Modem...	  ");
	
    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationBackUpMode;	
	
	if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
	{
		vdDebug_LogPrintf("vdCTOS_DialBackupConfig, COMM INIT ERR");
		vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
		return d_NO;
	}

    inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

	vdDebug_LogPrintf("vdCTOS_DialBackupConfig, inCTOS_CheckInitComm");
	
    if (srTransRec.byOffline == CN_FALSE)
    {   
        inResult = srCommFuncPoint.inCheckComm(&srTransRec);        
    }
  
	vduiClearBelow(3);
	
    return d_OK;	
}
#else
//Comms fallback - start -- jzg
int inCTOS_CommsFallback(int shHostIndex)
{
	int inResult = 0;

	vdSetInit_Connect(1);
	put_env_int("CONNECTED",0);
						
	vdDebug_LogPrintf("inCTOS_CommsFallback, Host Index [%d]", shHostIndex);
	vdDebug_LogPrintf("srTransRec.usTerminalCommunicationMode [%d]", srTransRec.usTerminalCommunicationMode);
	vdDebug_LogPrintf("strCPT.inCommunicationMode [%d]", strCPT.inCommunicationMode);

	vduiClearBelow(3);
	CTOS_LCDTPrintXY (1,6, "Comms Fallback");
	CTOS_LCDTPrintXY (1,7, "Please Wait 	");

#if 0
	if ((srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE))
	{
		CTOS_LCDTPrintXY(1, 8, "Init Modem... 	");
		srTransRec.usTerminalCommunicationMode = DIAL_UP_MODE;
		strCPT.inCommunicationMode = DIAL_UP_MODE;
	}
	else
	{
		if (strTCT.fShareComEnable == 0)
		{
			CTOS_LCDTPrintXY(1, 8, "Init IP...     ");
			srTransRec.usTerminalCommunicationMode = ETHERNET_MODE;
			strCPT.inCommunicationMode = ETHERNET_MODE;
		}
		else
		{
			CTOS_LCDTPrintXY(1, 8, "Init GPRS...   ");
			srTransRec.usTerminalCommunicationMode = GPRS_MODE;
			strCPT.inCommunicationMode = GPRS_MODE;
		}
	}
#else
	if ((srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE))
	{
		CTOS_LCDTPrintXY(1, 8, "Init Modem... 	");
		srTransRec.usTerminalCommunicationMode = DIAL_UP_MODE;
		strCPT.inCommunicationMode = DIAL_UP_MODE;
		strCPT.inIPHeader = NO_HEADER_LEN;
	}
	else
	{
/*
		if(strCPT.inCommunicationBackUpMode == 0)
		{
			srTransRec.usTerminalCommunicationMode = DIAL_UP_MODE;
			strCPT.inCommunicationMode = DIAL_UP_MODE;
			strCPT.inIPHeader = NO_HEADER_LEN;
			
		}
		else*/ if(strCPT.inCommunicationBackUpMode == 1)
		{
			CTOS_LCDTPrintXY(1, 8, "Init IP...     ");
			srTransRec.usTerminalCommunicationMode = ETHERNET_MODE;
			strCPT.inCommunicationMode = ETHERNET_MODE;
			strCPT.inIPHeader = BCD_EXCLUDE_LEN;
		}
		else if(strCPT.inCommunicationBackUpMode == 2)
		{
			CTOS_LCDTPrintXY(1, 8, "Init GPRS...   ");
			srTransRec.usTerminalCommunicationMode = GPRS_MODE;
			strCPT.inCommunicationMode = GPRS_MODE;
			strCPT.inIPHeader = HEX_EXCLUDE_LEN;
		}
		else if(strCPT.inCommunicationBackUpMode == 3)
		{
			CTOS_LCDTPrintXY(1, 8, "Init WiFi...   ");
			srTransRec.usTerminalCommunicationMode = WIFI_MODE;
			strCPT.inCommunicationMode = WIFI_MODE;
			strCPT.inIPHeader = HEX_EXCLUDE_LEN;
		}
	}
#endif
	
	
	if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
	{
		vdDebug_LogPrintf("inCTOS_CommsFallback, COMM INIT ERR");
		//vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
		//vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
		vdDisplayErrorMsgResp2("","Initialization","Error");
		return d_NO;
	}

	inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

	vdDebug_LogPrintf("vdCTOS_DialBackupConfig, inCTOS_CheckInitComm");

	if (srTransRec.byOffline == CN_FALSE)
	{
		inResult = srCommFuncPoint.inCheckComm(&srTransRec);
	}

	vduiClearBelow(3);

	//vdSetInit_Connect(1);
	put_env_int("CONNECTED",1);
	
	fCommsFallback = TRUE;
	inCommsFallbackMode = strCPT.inCommunicationMode;
	return d_OK;	
}
//Comms fallback - end -- jzg

#endif




/*albert - start - August2014 - manual settlement*/
int inCTOS_ManualSettle(void)
{

}


void vdCTOS_PrintDetailReportForManualSettlement(int shHostIndex)
{

}

/*albert - end - August2014 - manual settlement*/


/*sidumili: [prompt for password]*/
int inCTOS_PromptPassword(void)
{
	int inRet = d_NO;

	CTOS_LCDTClearDisplay();
	vduiClearBelow(7);
	//vdCTOS_SetTransType(SETUP); //aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 1 of 9
	vdDispTransTitle(SETUP);

	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
	{
	    //clearLine(1);
	    
		CTOS_LCDTClearDisplay();
		inCTOS_IdleEventProcess();
		return inRet;
	}

	return d_OK;
}




//format amount 10+2
void vdCTOSS_DisplayAmount(USHORT usX, USHORT usY, char *szCurSymbol,char *szAmount)
{
	int x=0;
	int len, index;

	CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
	x=0;
	len=strlen(szAmount);
	for(index=0; index < len; index++)
	{
	   if(szAmount[index] == '.')
	       x+=1;
	   else
	       x+=2;
	}
	CTOS_LCDTPrintXY(36-x, usY, szAmount);
}

//-- Inject Key
void vdCTOSS_InjectMKKey(void)
{
	BYTE bInBuf[250];
	BYTE bOutBuf[250];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
  BYTE  x = 1;
  BYTE key;
	char szHeaderString[50] = "SELECT TO LOAD KEY";
	char szInjectKeyMenu[1024] = {0};


	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 1, "INJECT MK");

	/*********************************************/
	//sidumili: [Select Pinpad Type for Injection]

	memset(szInjectKeyMenu, 0x00, sizeof(szInjectKeyMenu));
	strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nCANCEL");
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szInjectKeyMenu, TRUE);

	if (key == 0xFF) 
	{	
		CTOS_LCDTClearDisplay();
		setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		vduiWarningSound();
		return;	
	}

	if (key > 0){
		
		if(d_KBD_CANCEL == key)
        return;

		memset(bOutBuf, 0x00, sizeof(bOutBuf));
		memset(bInBuf, 0x00, sizeof(bInBuf));

		// --->> IPP
		if (key == 1){
	
			// --- Inject to IPP
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "INJECT MK");
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_INJECTMK, bInBuf, usInLen, bOutBuf, &usOutLen);

			// -- sidumili: debugging of key injected
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT IN", bInBuf, usInLen);
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT OUT", bOutBuf, usOutLen);
			
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "Save MK error");
			else
				vdDisplayErrorMsg(1, 8, "Save MK OK");
	
			return ;
		}

		// --->> PCi100 / BOTH
		if ((key == 2) || (key == 3)){
			
			// --- Inject to IPP
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "INJECT MK");
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_INJECTMK, bInBuf, usInLen, bOutBuf, &usOutLen);

			// -- sidumili: debugging of key injected
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT IN", bInBuf, usInLen);
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT OUT", bOutBuf, usOutLen);
			
			if (usResult != d_OK)
			return ;

			// --- Inject to PCi100
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "INJECT MK");
			
			if (usOutLen == 8)
			usResult = inPCI100_SaveMKProcess(MK_DES_KEY,usOutLen,bOutBuf);
			if (usOutLen == 16)
			usResult = inPCI100_SaveMKProcess(MK_3DES_DOUBLE_KEY,usOutLen,bOutBuf);
			if (usOutLen == 24)
			usResult = inPCI100_SaveMKProcess(MK_3DES_TRIPLE_KEY,usOutLen,bOutBuf);

			if (usResult != d_OK)
			return ;

		}
		
		// --->> CANCEL
		if (key == 4){
			vdDisplayErrorMsg(1, 8, "Inject cancelled");
			return;
		}
	}
	/*********************************************/

	
	#if 0
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	vdDebug_LogPrintf("d_IPC_CMD_EMV_INJECTMK usInLen[%d] ",usInLen);
	
	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_INJECTMK, bInBuf, usInLen, bOutBuf, &usOutLen);

	if (usResult != d_OK)
		return ;

	CTOS_LCDTPrintXY(1, 1, "INJECT MK");
	vdDebug_LogPrintf("d_IPC_CMD_EMV_INJECTMK usOutLen[%d] ",usOutLen);
	DebugAddHEX("MK",bOutBuf,usOutLen);
	
	inTCTRead(1);
	vdDebug_LogPrintf("InjectMKKey..byPinPadType=[%d],byPinPadPort=[%d]",strTCT.byPinPadType,strTCT.byPinPadPort);
	if (1 == strTCT.byPinPadType)
	{
		if (usOutLen == 8)
			inPCI100_SaveMKProcess(MK_DES_KEY,usOutLen,bOutBuf);
		if (usOutLen == 16)
			inPCI100_SaveMKProcess(MK_3DES_DOUBLE_KEY,usOutLen,bOutBuf);
		if (usOutLen == 24)
			inPCI100_SaveMKProcess(MK_3DES_TRIPLE_KEY,usOutLen,bOutBuf);
	}
	#endif

	return;
}


//BDO UAT 0007: Change comms shortcut - start -- jzg
void vdChangeComms(void)
{
	int inCPTNumRecs = 0,
		i = 0,
		inInput = 0,
		inCommMode =-1;
	BYTE strOut[30] = {0};
	USHORT usLen = 0,
		ret = 0;

//	CTOS_LCDTClearDisplay();
//	vdDispTitleString(szFuncTitleName); 
	
	inCPTRead(1);
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
		//wifi-mod2 	
		case 4:
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "WIFI");
			break;
		//wifi-mod2

	}
	
	//wifi-mod2

	
	CTOS_LCDTPrintXY(1, 5, "0-DIAL-UP    1-LAN");
	CTOS_LCDTPrintXY(1, 6, "2-GPRS          3-WIFI");
	CTOS_LCDTPrintXY(1, 7, "New:");

	do
	{
		CTOS_LCDTPrintXY(1, 8, "                    ");
		memset(strOut,0x00, sizeof(strOut));
		ret = shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
		switch(ret)
		{
			case d_KBD_CANCEL:
				//inInput = 2;
				//break;
				return;
			case 1:
				inInput = 1;
				switch(strOut[0])
				{
					case 0x30:
						inCommMode = DIAL_UP_MODE;
						break;
					case 0x31:
						inCommMode = ETHERNET_MODE;
						break;
					case 0x32:
						inCommMode = GPRS_MODE;
						break;
//wifi-mod2
					case 0x33:
						inCommMode = WIFI_MODE;
						break;
//wifi-mod2

				}
				break;
		}
	}
	while(inCommMode == -1);

	vdDebug_LogPrintf("JEFF::COMMS [%d]", inCommMode);


//	inCPTNumRecs = inCPTNumRecord();
//	vdDebug_LogPrintf("JEFF::Comm Count [%d]", inCPTNumRecs);
	
	vduiClearBelow(2);
	CTOS_LCDTPrintXY(1, 7, "Configuring Comms");
	CTOS_LCDTPrintXY(1, 8, "Please wait...");
	
#if 0	
	for(i = 1 ;i <= inCPTNumRecs; i++)
	{
		inCPTRead(i);
		strCPT.inCommunicationMode = inCommMode;
		//wifi-mod2
		if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE))
			strCPT.inIPHeader = 1;
		else
			strCPT.inIPHeader = 0;

		
		//if ((inCommMode == WIFI_MODE) || (inCommMode == GPRS_MODE))
			//strCPT.fDialMode = 1;
		//else if (inCommMode == GPRS_MODE)
			//strCPT.fDialMode = 0;
		
		inCPTSave(i);
	}

	
	if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE))
		strCPT.inIPHeader = 1;
	else
		strCPT.inIPHeader = 0;

	inCPT_Update(inCommMode, strCPT.inIPHeader);

	inCPTRead(1);//Read CPT to display correct communication ICON on idle.
	


	inTCTRead(1);
	//wifi-mod2
	if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE) /*|| (inCommMode == DIAL_UP_MODE)*/)
	//wifi-mod2
		strTCT.fShareComEnable = 1;
	else
		strTCT.fShareComEnable = 0;
	inTCTSave(1);


	/*BDO: Update fDHCPEnable for GPRS/WIFI*/
	inTCPRead(1);
	if ((inCommMode == GPRS_MODE) || (inCommMode == WIFI_MODE))
		strTCP.fDHCPEnable = IPCONFIG_DHCP;
	else	
		strTCP.fDHCPEnable = IPCONFIG_STATIC;

	
	inTCPSave(1);
#endif

	inCPT_Update(inCommMode, strCPT.inIPHeader);
	inCPTRead(1);//Read CPT to display correct communication ICON on idle.
	/*BDO: Update fDHCPEnable for GPRS/WIFI*/
	
	//BDO: [RESTART] -- sidumili
	vduiClearBelow(7);
	vdDisplayErrorMsg(1, 8, "Change Comm..Done");
				
}
//BDO UAT 0007: Change comms shortcut -end -- jzg

//BDO: [Restart Function] -- sidumili
void vdCTOS_uiRestart(BOOL fConfirm)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;

    CTOS_LCDTClearDisplay();
    
    if (fConfirm == TRUE)
    {
        vduiDisplayStringCenter(4, "RESTART TERMINAL?");
        vduiDisplayStringCenter(7, "YES[OK]   NO[X]"); 
        
        c=WaitKey(60);
        
        if(c!=d_KBD_ENTER)
        {
            return;
        }		
    }
    else
    {
        vduiDisplayStringCenter(3, "TO TAKE EFFECT");
        vduiDisplayStringCenter(4, "TERMINAL WILL");
        vduiDisplayStringCenter(5, "REBOOT");
        vduiDisplayStringCenter(6, "PLEASE WAIT...");
    }
		
    
    for(ya =1; ya<5; ya++)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,ya);
        CTOS_LCDTClear2EOL();
    }
    for(yb=8; yb>4; yb--)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,yb);
        CTOS_LCDTClear2EOL();
    }
    CTOS_LCDTPrintXY(1,4,"----------------");
    for(xa=1; xa<8; xa++)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xa,4," ");
    }
    for(xb=16; xb>7; xb--)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xb,4," ");
    }
            
    CTOS_LCDGShowPic(58, 6, block, 0, 6);
    CTOS_Delay(250);
    CTOS_LCDTGotoXY(7,4);
    CTOS_LCDTClear2EOL();
    CTOS_Delay(250);

    CTOS_SystemReset();
}
//BDO: [Restart Function] -- sidumili

//BDO: [Select Telco Setting] -- sidumili
int inSelectTelcoSetting()
{
	BYTE bInBuf[250] = {0};
	BYTE bOutBuf[250] = {0};
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult = 0;

	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE	x = 1;
	BYTE key;
	char szHeaderString[50] = "SELECT TO TELCO";
	char szMenu[1024] = {0};
	int inRet = 0;
	int inResult = 0;
	int inCPTNumRecs = 0;
	int i = 0;

	//----------------------------
	BYTE	szAPN[50] = {0};
	BYTE	szUserName[30] = {0};
	BYTE	szPassword[30] = {0};
	BYTE	szPriTxnHostIP[30] = {0};
	BYTE	szSecTxnHostIP[30] = {0};
	BYTE	szPriSettlementHostIP[30] = {0};
	BYTE	szSecSettlementHostIP[30] = {0};
	int	inPriTxnHostPortNum = 0;
	int	inSecTxnHostPortNum = 0;
	int	inPriSettlementHostPort = 0;
	int	inSecSettlementHostPort = 0;
	//-----------------------------
	BOOL fSelectOK = FALSE;


	memset(szAPN, 0x00, sizeof(szAPN));
	memset(szUserName, 0x00, sizeof(szUserName));
	memset(szPassword, 0x00, sizeof(szPassword));

	vdCTOS_SetTransType(SETUP);
	//display title
	vdDispTransTitle(SETUP);

	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inTCPRead(1);
	if(d_OK != inRet)
		return inRet;	

	memset(szMenu, 0x00, sizeof(szMenu));
	strcpy((char*)szMenu, "SMART \nGLOBE \nOTHER \nCANCEL");
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMenu, TRUE);

	if (key == 0xFF) 
	{ 
		CTOS_LCDTClearDisplay();
		setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		vduiWarningSound();
		return(ST_SUCCESS); 
	}

	if (key > 0)
	{
		if(key == d_KBD_CANCEL)
			return(ST_SUCCESS);

		memset(bOutBuf, 0x00, sizeof(bOutBuf));
		memset(bInBuf, 0x00, sizeof(bInBuf));

		// --->> SMART
		if (key == 1)
		{
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "SMART TELCO");

			//-->> READ [GPT TABLE]
			inGPTRead(1);

			//------------------------------------------------------------------------
			//-->> UPDATE TCP TABLE -- START
			//------------------------------------------------------------------------
//			strcpy(strTCP.szAPN,strGPT.szGPTAPN);
//			strcpy(strTCP.szUserName,strGPT.szGPTUserName);
//			strcpy(strTCP.szPassword,strGPT.szGPTPassword);

			inTCPSave(1);
			//------------------------------------------------------------------------
			//-->> UPDATE TCP TABLE -- START
			//------------------------------------------------------------------------

			if (usResult != d_OK)		
				fSelectOK = FALSE;
			else
				fSelectOK = TRUE;
		}

		// --->> GLOBE
		if (key == 2)
		{
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "GLOBE TELCO");

			//-->> READ [GPT TABLE]
			inGPTRead(2);

			//------------------------------------------------------------------------
			//-->> UPDATE TCP TABLE -- START
			//------------------------------------------------------------------------
//			strcpy(strTCP.szAPN,strGPT.szGPTAPN);
//			strcpy(strTCP.szUserName,strGPT.szGPTUserName);
//			strcpy(strTCP.szPassword,strGPT.szGPTPassword);

			inTCPSave(1);
			//------------------------------------------------------------------------
			//-->> UPDATE TCP TABLE -- START
			//------------------------------------------------------------------------

			if (usResult != d_OK)		
				fSelectOK = FALSE;
			else
				fSelectOK = TRUE;
		}

		// --->> SG-SMART
		if (key == 3)
		{
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "OTHER TELCO");

			//-->> READ [GPT TABLE]
			inGPTRead(3);

			//------------------------------------------------------------------------
			//-->> UPDATE TCP TABLE -- START
			//------------------------------------------------------------------------
//			strcpy(strTCP.szAPN,strGPT.szGPTAPN);
//			strcpy(strTCP.szUserName,strGPT.szGPTUserName);
//			strcpy(strTCP.szPassword,strGPT.szGPTPassword);

			inTCPSave(1);
			//------------------------------------------------------------------------
			//-->> UPDATE TCP TABLE -- END
			//------------------------------------------------------------------------

			if (usResult != d_OK)		
				fSelectOK = FALSE;
			else
				fSelectOK = TRUE;
		}

		// --->> CANCEL
		if (key == 4)
		{
			vdDisplayErrorMsg(1, 8, "TELCO setting cancel");
			return(ST_SUCCESS);
		}

		//-------------------------------------------------------------
		// UPDATE CPT TABLE [START]
		//-------------------------------------------------------------
		vdCTOS_DispStatusMessage("Please wait...");
		
		inCPTNumRecs = inCPTNumRecord();
		vdDebug_LogPrintf("JEFF::Comm Count [%d]", inCPTNumRecs);
		for(i = 1 ;i <= inCPTNumRecs; i++)
		{
			if(i == 108 || i == 109 || i == 110 || i == 112 || i == 113 || i == 114)
				continue;
			
			inCPTRead(i);

//			strcpy(strCPT.szPriTxnHostIP,strGPT.szGPTPriTxnHostIP);
//			strcpy(strCPT.szSecTxnHostIP,strGPT.szGPTSecTxnHostIP);
//			strcpy(strCPT.szPriSettlementHostIP,strGPT.szGPTPriSettlementHostIP);
//			strcpy(strCPT.szSecSettlementHostIP,strGPT.szGPTSecSettlementHostIP);
//
//			strCPT.inPriTxnHostPortNum = strGPT.inGPTPriTxnHostPortNum;
//			strCPT.inSecTxnHostPortNum = strGPT.inGPTSecTxnHostPortNum;
//			strCPT.inPriSettlementHostPort = strGPT.inGPTPriSettlementHostPort;
//			strCPT.inSecSettlementHostPort = strGPT.inGPTSecSettlementHostPort;

			inCPTSave(i);
		}
		//-------------------------------------------------------------
		// UPDATE CPT TABLE [END]
		//-------------------------------------------------------------
		if (key != 4){
			CTOS_LCDTClearDisplay();
			if (fSelectOK){
				if (key == 1)
					vdDisplayErrorMsg(1, 8, "Save SMART..done");
				else if (key == 2)
					vdDisplayErrorMsg(1, 8, "Save GLOBE..done");
				else if (key == 3)
					vdDisplayErrorMsg(1, 8, "Save OTHER..error");
			}
			else{
				vdDisplayErrorMsg(1, 8, "Save TELCO Error");
			}
		}
	}

	return(ST_SUCCESS);
}



void vdCTOSS_ModifyStanNumber(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szStanNo[8+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;


    CTOS_LCDTClearDisplay();
    vdDispTitleString("Modify STAN");
    shHostIndex = inCTOS_SelectHostSetting();
	vdDebug_LogPrintf("shHostIndex=[%d]",shHostIndex);
    if (shHostIndex == -1)
        return;

	inHDTRead(shHostIndex);
	memset(szStanNo, 0x00, sizeof (szStanNo));
	wub_hex_2_str(strHDT.szTraceNo,szStanNo,3);
		
    while(1)
    {
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Original STAN ");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szStanNo);
    
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 6, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if(ret==6)
        {
        	vdDebug_LogPrintf("strOut=[%s]",strOut);
			wub_str_2_hex(strOut,strHDT.szTraceNo,6);
			//inHDTUpdateTraceNum();
			inHDTSave(shHostIndex);
			break;
       	}
   		if (ret == d_KBD_CANCEL)
        	break ;
    }
            
	return ;
}

/* BDO CLG: Revised menu functions - start -- jzg */
void vdDisplaySetup(void)
{
	int inMenuNumRecs = 0,
		inCtr = 0,
		inKey = 0,
		inHeaderAttr = 0x01+0x04;

	char szMenuItems[9000] = {0},
		szBuf[200] = {0},
		szHeader[10] = {0};


	inMenuNumRecs = inReadDynamicMenu();

	strcpy(szHeader, "SETUP");
	memset(szMenuItems, 0, sizeof(szMenuItems));
	for(inCtr = 0; inCtr < inMenuNumRecs; ++inCtr)
	{
		strcat(szMenuItems, strDynamicMenu[inCtr].szButtonItemLabel);
		if((strlen(strDynamicMenu[inCtr].szButtonItemLabel) > 0) &&
			(inCtr != inMenuNumRecs))
			strcat(szMenuItems, "\n");
	}

	inCtr = strlen(szMenuItems) - 1;
	szMenuItems[inCtr] = 0x00;

	inKey = MenuDisplay(szHeader, strlen(szHeader), inHeaderAttr, 1, 1, szMenuItems, TRUE);
	if(inKey == 0xFF)
	{
		CTOS_LCDTClearDisplay();
		setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		vduiWarningSound();
		return -1;
	}

	if(inKey > 0)
	{
		if(inKey == d_KBD_CANCEL)
			return -1;

		inCTOSS_ExeFunction(strDynamicMenu[inKey-1].szButtonItemFunc);
		if(strlen(strDynamicMenu[inKey-1].szSubMenuName) > 0)
			inCTOSS_ExeFunction(strDynamicMenu[inKey-1].szSubMenuName);

		inKey = d_OK;
	}
}


void vdCTOS_FunctionKey(void)
{
	int ret = 0,
		inRet = d_OK;
	BYTE strOut[2] = {0}, 
		szErrMsg[22] = {0};
	USHORT usLen = 2;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("FUNCTION MENU");

	CTOS_LCDTPrintXY(1, 7, "FUNCTION?");

	memset(strOut, 0, sizeof(strOut));
	//ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
	ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, 60 * 100);
	if(ret == d_KBD_ENTER)
	{

		memset(szFuncTitleName,0,sizeof(szFuncTitleName)); //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 2 of 6
	 
		switch(atoi(strOut))
		{
			case 1:
				memcpy(szFuncTitleName,"CLEAR",11);
				vdCTOS_SetTransType(SETUP);
//				vdFunctionOne();
				break;

			case 2:
				vdCTOS_HostInfo();
				break;

			case 3:
				memcpy(szFuncTitleName,"CHANGE COMMS",12);
				//vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 3 of 9
				//inCTOS_PromptPassword();
				//CTOS_LCDTClearDisplay();
				//vdDispTitleString("CHANGE COMMS");
//				if(inFunctionKeyPassword("CHANGE COMMS", SUPER_ENGINEER_PWD)==d_OK)
//				{	
//					CTOS_LCDTClearDisplay();
//					vdDispTitleString("CHANGE COMMS");
//			    	vdChangeComms();
//				}
				break;

			case 4:
				memcpy(szFuncTitleName,"SETUP",5);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 4 of 9
				vdCTOS_SetTransType(SETUP);
				inCTOS_PromptPassword();

				/* BDOCLG: change to BDO color scheme if in icon mode - start -- jzg */
				inTCTRead(1);
				if((strTCT.byTerminalType % 2) == 0)
				{
					CTOS_LCDForeGndColor(RGB(13, 43, 112));
					CTOS_LCDBackGndColor(RGB(255, 255, 255));
				}
				/* BDOCLG: change to BDO color scheme if in icon mode - end -- jzg */
				
				vdDisplaySetup();
				break;

			case 5:
//				if(strTCT.fHotelSetup)
					inCTOS_PREAUTH();
				break;

			case 6:
//				if(strTCT.fHotelSetup)
					inCTOS_SALE_OFFLINE();
				break;

			case 7:
				memset(&strPIT,0x00,sizeof(STRUCT_PIT));
//				inPITRead(DCC_MODE);

				if(inFunctionKeyPasswordEx("DCC MODES", strPIT.inPasswordLevel)!=d_OK)
				{
					memset(szFuncTitleName,0,sizeof(szFuncTitleName));
					return d_NO;
				}
				inCTOS_vdSetDCCMode();
				break;
				
			case 8:
				inTCTRead(1);
//				strTCT.fScreenLocked = TRUE;
				put_env_int("LOCKPROMPT", 1);
				inTCTSave(1);
				break;

			case 9:
//				WIFI_Scan();
				break;

			case 11:
				vdCTOS_PrintEMVTerminalConfig();
				break;
				
            case 13:
                inCTOSS_UploadReceipt();
            	break;	
            
            case 14:
                inCTOS_ERMAllHosts_DeInitialization();
            	break;		

      #if 1
			case 27: // from Function 7 to Function 50
				memcpy(szFuncTitleName,"PRINT ISO LOG",13); 
				vdCTOS_SetTransType(SETUP);
				inCTOS_PromptPassword(); //aaronnino for BDOCLG ver 9.0 fix on issue #000106 Input Password for Func 7 (Engineer Password)
				vdCTOS_ISOLogger();
				break;	
        #endif

			case 30:
//				memcpy(szFuncTitleName,"BIN ROUTING",11);
//				vdCTOS_SetTransType(BIN_ROUTING);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
//				inCTOS_PromptPassword();
//				vdCTOS_BINRouting(); 
				break;

			case 31:
			memcpy(szFuncTitleName,"BIN ROUTING NII",15);
			vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
//				if(inFunctionKeyPassword("BIN ROUTING NII", SUPER_ENGINEER_PWD)==d_OK)
//				    vdCTOS_BINRoutingNII();
				break;

			case 33: // from Function 7 to Function 50
				memcpy(szFuncTitleName,"PING IP ADDRESS",15); //aaronnino for BDOCLG ver 9.0 fix on issue #00181 Display on Function 7 was carrying over the display of the previous funtion used
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 5 of 9
				//inCTOS_PromptPassword(); ; //aaronnino for BDOCLG ver 9.0 fix on issue #000106 Input Password for Func 7 (Engineer Password)
				vdCTOS_PingIPAddress();
				break;	

			case 50: // from Function 7 to Function 50
				memcpy(szFuncTitleName,"MENU SETTINGS",13); //aaronnino for BDOCLG ver 9.0 fix on issue #00181 Display on Function 7 was carrying over the display of the previous funtion used
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 5 of 9
				inCTOS_PromptPassword(); ; //aaronnino for BDOCLG ver 9.0 fix on issue #000106 Input Password for Func 7 (Engineer Password)
				vdCTOS_ChangeMenuID();
				break;

			case 51: // from Function 9 to Function 51
        		memcpy(szFuncTitleName,"PRINT ISO",9);
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
				inCTOS_PromptPassword();
				vdCTOS_PrintISOMode(); 
				break;

			case 88:
				memcpy(szFuncTitleName,"TMS DOWNLOAD",12);
				if(inFunctionKeyPasswordEx("TMS DOWNLOAD", 2)==d_OK) //SYSTEM PASSWORD
				{	
					CTOS_LCDTClearDisplay();
					vdDispTitleString("TMS DOWNLOAD");
			    	inCTOSS_TMSDownloadRequest();
				}
				break;


			case 90:
				memcpy(szFuncTitleName,"Lock Screen PWD",15); //aaronnino for BDOCLG ver 9.0 fix on issue #00219 carrying  over the header display
				//inCTOS_PromptPassword(); //aaronnino for BDOCLG ver 9.0 fix on issue #00219 
				vdChangeLockPassword();
				break;

				case 94:
				vdCTOS_SetTransType(SETUP); 	
				memcpy(szFuncTitleName,"MODEM RECEIVING TIME",20);
				inCTOS_PromptPassword();
				vdCTOS_ModemReceivingTime(); //aaronnino for BDOCLG ver 9.0 fix on issue #0092 Make Function 1 shorcut key to manual settlement function instead of clear batch
				break;
//tms
			case 95:
				vdCTOS_TMSReSet();
				break;
//SIT
#if 0
      		case 96: 
					inCTOSS_TMSDownloadRequest();
			    break;
			case 97: 
					vdCTOS_TMSSetting();
			    break;

//tms       
			case 97:
				CTOS_KMS2KeyDelete(0xC001, 0x0001);
				vdDisplayErrorMsgResp2("","SMAC KEYS","ERASED");
				break;
#endif

			case 98:
				vdCTOS_uiRestart(TRUE);
				break; //BDO-00150: added break to make sure not to run vdCTOS_uiPowerOff() when cancel key is pressed -- jzg

			case 99: 
				vdCTOS_uiPowerOff();		
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


void vdCTOS_ChangeMenuID()
{
	BYTE szBuf[20] = {0};
	BYTE strOut[30] = {0};
	USHORT ret = 0;
	USHORT usLen = 0;
	int inRet = 0;
	int inTmpNum = 0;

	inRet = inTCTRead(1);  
	vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

	CTOS_LCDTClearDisplay();
	vdDispTitleString("SETTING");
	while(1)
	{
		clearLine(3);
		clearLine(4);
		clearLine(5);
		clearLine(6);
		clearLine(7);
		clearLine(8);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "MENU ID");
		sprintf(szBuf, "CURRENT: [%d]", strTCT.inMenuid);
		CTOS_LCDTPrintXY(1, 4, szBuf);
		
		//change menu items to 106
		//CTOS_LCDTPrintXY(1, 7, "PLEASE ENTER [1..10]");
		CTOS_LCDTPrintXY(1, 7, "PLEASE ENTER [1..108]");

		memset(strOut,0x00, sizeof(strOut));
		ret = shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 3, 0, d_INPUT_TIMEOUT);
		if(ret == d_KBD_CANCEL )
			break;
		else if(ret == 0)
			break;
		else if(inRet == 0xFF)
		{
			vdSetErrorMessage("TIME OUT");
			return d_NO;
		}
		else if(ret >= 1)
		{
			if(strlen(strOut) > 0)
			{
				inTmpNum = atoi(strOut);
				//change menu items to 41
				//if((inTmpNum >= 1) && (inTmpNum <= 10))
				if((inTmpNum >= 1) && (inTmpNum <= 108))
				{
					strTCT.inMenuid = inTmpNum;
					inTCTSave(1);
					vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
					break;
				}
				else
				{
					vduiWarningSound();
					vduiClearBelow(2);
					vduiDisplayStringCenter(6,"PLEASE SELECT");
					vduiDisplayStringCenter(7,"A VALID");
					vduiDisplayStringCenter(8,"MENU INDEX");
					CTOS_Delay(2000); 			
				}
			}
		}
		if (ret == d_KBD_CANCEL )
			break ;
	}
       
}
/* BDO CLG: Revised menu functions - end -- jzg */


/* BDO: Manual settlement prompt after failed settlement - start -- jzg */
int inBDOManualSettle()
{
	int shHostIndex = 1;
	int inResult = 0;
	char szErrMsg[31] = {0};
	ACCUM_REC srAccumRec;

	vdDebug_LogPrintf("MANUAL SETTLE AFTER FAILED SETTLEMENT");

	CTOS_LCDTClearDisplay();	
	//vdDispTitleString("CLEAR BATCH");

	inResult = inCTOS_PrintSettleReport(TRUE, FALSE);	
	if(inResult != d_OK)
	{
		if(inGetErrorMessage(szErrMsg) > 0)
		{
			CTOS_LCDTClearDisplay();
			vdDisplayErrorMsg(1, 8, szErrMsg);
			vdSetErrorMessage("");
		}
		return d_ERROR;
	}

//remove later -- jzg
vdDebug_LogPrintf("JEFF::inBDOManualSettle Host Idx = [%d]", strHDT.inHostIndex);

	shHostIndex = strHDT.inHostIndex;
	vdCTOS_PrintDetailReportForManualSettlement(shHostIndex);
//	if(srTransRec.fManualSettlement == TRUE)
//	     vdCTOS_PrintManualPosting();

	inResult = vduiAskConfirmContinue(1);
	if(inResult != d_OK)
		return inResult;

	inResult = inCTOS_SettlementClearBathAndAccum(TRUE);
	if(inResult == d_OK)
	{
	  #if 0
		CTOS_LCDTClearDisplay();
		CTOS_LCDTPrintXY(1, 8, "BATCH DELETED");
		CTOS_Beep();
		CTOS_Delay(2000);
		#else
		vdDisplayErrorMsgResp2(" ", " ","BATCH DELETED");
		#endif
		
		
		return inResult;
	}

	CTOS_KBDBufFlush();

	return d_OK;
}
/* BDO: Manual settlement prompt after failed settlement - end -- jzg */


int get_env(char *tag, char *value, int len)
{
	int inRet = 0;
	inRet = inCTOSS_GetEnvDB (tag, value);

	vdDebug_LogPrintf("get_env tag[%s] value[%s] Ret[%d]", tag, value, inRet);
	return inRet;
}

int put_env(char *tag, char *value, int len)
{
	int inRet = 0;
	
	inRet = inCTOSS_PutEnvDB (tag, value);

	vdDebug_LogPrintf("put_env tag[%s] value[%s] Ret[%d]", tag, value, inRet);
	return inRet;
}

//hubing enhance ECR
int get_env_int (char *tag)
{
	int     ret = -1;
	char    buf[6];

    memset (buf, 0, sizeof (buf));
    if ( inCTOSS_GetEnvDB (tag, buf) == d_OK )
    {
        ret = atoi (buf);
    }

	vdDebug_LogPrintf("get_env_int [%s]=[%d]", tag, ret);

    return ret;
}
//hubing enhance ECR
void put_env_int(char *tag, int value)
{
	int     ret = -1;
	char    buf[6];

    memset (buf, 0, sizeof (buf));
    //int2str (buf, value);
    sprintf(buf, "%d", value);
    ret = inCTOSS_PutEnvDB (tag, buf);

	vdDebug_LogPrintf("put_env_int [%s]=[%d] ret[%d]", tag, value, ret);
}



void vdCTOSS_EditTable(void)
{
	BYTE strOut[100];
	BYTE szdatabase[100];
	BYTE sztable[100];
    USHORT usLen;
	USHORT ret;
	char szDispay[50];
	BYTE g_DeviceModel;

	CTOS_DeviceModelGet(&g_DeviceModel);
	vdDebug_LogPrintf("CTOS_DeviceModelGet =[%d]",g_DeviceModel);
	
	memset(szDispay,0x00,sizeof(szDispay));
	memset(szdatabase,0x00,sizeof(szdatabase));
	memset(sztable,0x00,sizeof(sztable));
	sprintf(szDispay,"EDIT DATABASE");
	CTOS_LCDTClearDisplay();
	
    vdDispTitleString(szDispay);            
	CTOS_LCDTPrintXY(1, 3, "DATABASE NAME:");
	
	usLen = 20;
	if (g_DeviceModel != d_MODEL_VEGA5000S)
	CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
    ret = InputStringAlphaEx(1, 7, 0x00, 0x02, szdatabase, &usLen, 1, d_INPUT_TIMEOUT);
	if (g_DeviceModel != d_MODEL_VEGA5000S)
	{
//	if (strTCT.inThemesType == 1)
            if(0)
	{
		CTOS_LCDTTFSelect("tahoma.ttf", 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
	}
	else
	{
		CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);
	}
	}

	if (ret == d_KBD_CANCEL || 0 == ret )
	{
		return ;
	}
	if(ret>= 1)
    {
    	vduiClearBelow(2);
        vdDebug_LogPrintf("szdatabase[%s].usLen=[%d].",szdatabase,usLen);
		CTOS_LCDTPrintXY(1, 3, "TABLE NAME:");
	
		usLen = 20;
		if (g_DeviceModel != d_MODEL_VEGA5000S)
		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
	    ret = InputStringAlphaEx(1, 7, 0x00, 0x02, sztable, &usLen, 1, d_INPUT_TIMEOUT);
		if (g_DeviceModel != d_MODEL_VEGA5000S)
		{
//		if (strTCT.inThemesType == 1)
                    if (0)
		{
			CTOS_LCDTTFSelect("tahoma.ttf", 0);
			CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
		}
		else
		{
			CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
			CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);
		}
		}
		
		if (ret == d_KBD_CANCEL || 0 == ret )
		{
			return ;
		}
		if(ret>= 1)
	    {
	        vdDebug_LogPrintf("sztable[%s].usLen=[%d].szdatabase[%s]",sztable,usLen,szdatabase);
			inCTOSS_EditTalbe(sztable,szdatabase);
	    }
	
    }
	
}
//aaronnino for BDOCLG ver 9.0 fix on issue #0073 No terminal function for enable/disable on ISO packet 2 of 3 start
void vdCTOS_PrintISOMode(void)
{
    BYTE strOut[30],strtemp[17];
    USHORT ret;
    USHORT usLen;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("PRINT ISO");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRINT ISO MODE?");
        if(strTCT.fPrintISOMessage== 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byRS232DebugPort == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        
        CTOS_LCDTPrintXY(1, 5, "0-NO        1-YES");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
				
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x31) 
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fPrintISOMessage = 1;
                 }
                 inRet = inTCTSave(1);
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 return;
             }
						if (strOut[0]==0x30) 
            {
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fPrintISOMessage = 0;
                 }
                 inRet = inTCTSave(1);
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 return; 
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"WRONG INPUT");
                CTOS_Delay(2000);  
            }
						 
        }
      
    }
       
    return ;
}
//aaronnino for BDOCLG ver 9.0 fix on issue #0073 No terminal function for enable/disable on ISO packet 2 of 3 end


//aaronnino for remote download setup 10 of 12 start
void vdCTOS_TMSSetting(void)
{
	BYTE strOut[50],strtemp[17];
	BYTE szInputBuf[5];
    int inResult;
    int ret;
	USHORT usLen;
    BYTE szSerialNum[17 + 1];//, 

	//bSIMSlot=0x30;

	BYTE bSIMSlot = 0x32;

	vdCTOS_InputTMSSetting();
	return;
	
		vdDebug_LogPrintf("AAA - vdCTOS_TMSSetting start");
		
    CTOS_LCDTClearDisplay();
		vdDebug_LogPrintf("AAA - vdCTOS_TMSSetting 1");
    vdDispTitleString("TMS SETTINGS");
		vdDebug_LogPrintf("AAA - vdCTOS_TMSSetting 2");
		
    memset(szSerialNum, 0x00, sizeof(szSerialNum));
		vdDebug_LogPrintf("AAA - vdCTOS_TMSSetting 3");
	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	vdDebug_LogPrintf("AAA - vdCTOS_TMSSetting 4");
    while(1)
    {   
    vdDebug_LogPrintf("AAA - vdCTOS_TMSSetting 5");
        vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "SERIAL NO.");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szSerialNum);
		
		strcpy(strtemp,"New:") ;
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		usLen = 32;
		ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 16, d_INPUT_TIMEOUT);
		if(ret==d_KBD_ENTER)
		{
			//memcpy(strTCP.szAPN, strOut,strlen(strOut));
			//strTCP.szAPN[strlen(strOut)]=0;
			CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, strOut);
			break;
		}	
		if(ret == d_KBD_CANCEL)
			break;
    }	

    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;	
    while(1)
    {   
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS GAP");
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "%d", strTCT.usTMSGap);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 3, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            strTCT.usTMSGap=atoi(strOut);      
            vdMyEZLib_LogPrintf("strTCT.usTMSGap %d",strTCT.usTMSGap);
			inResult = inTCTSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }	

    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;	
    while(1)
    {   
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS COMM MODE");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "0-DIALUP 1-ETH");
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "2-GPRS");
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "%d", strTCT.inTMSComMode);
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if(strOut[0] == 0x30 || strOut[0] == 0x31 || strOut[0] == 0x32)
            {
                strTCT.inTMSComMode=atoi(strOut);      
                vdMyEZLib_LogPrintf("strTCT.usTMSGap %d",strTCT.inTMSComMode);
                inResult = inTCTSave(1);
                break;
            }
			else
				vdDisplayErrorMsg(1, 8, "INVALID INPUT");
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }	
    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;	
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS REMOTE IP");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szTMSRemoteIP);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strtemp, 0x00, sizeof(strtemp));
        memset(strOut,0x00, sizeof(strOut));
        ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
        if(ret==d_KBD_ENTER)
        {
            memcpy(strTCT.szTMSRemoteIP,strOut,strlen(strOut));
            strTCT.szTMSRemoteIP[strlen(strOut)]=0;
            vdMyEZLib_LogPrintf("strTCT.szTMSRemoteIP %s",strTCT.szTMSRemoteIP);
			inResult = inTCTSave(1);
            break;
        }	
        if(ret == d_KBD_CANCEL)
            break;
    }
	
    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;	
    while(1)
    {   
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS REMOTE PORT");
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "%d", strTCT.usTMSRemotePort);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            strTCT.usTMSRemotePort=atoi(strOut);      
            vdMyEZLib_LogPrintf("strTCT.usTMSRemotePort %d",strTCT.usTMSRemotePort);
			inResult = inTCTSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }	

    if(strTCT.inTMSComMode == DIAL_UP_MODE)
    {
    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;
	while(1)
	{
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS REMOTE PHONE");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szTMSRemotePhone);
	
		strcpy(strtemp,"New:") ; 
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		usLen = 32;
		ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>= 1)
		{
			memcpy(strTCT.szTMSRemotePhone,strOut,strlen(strOut));
			strTCT.szTMSRemotePhone[strlen(strOut)]=0;
			inResult = inTCTSave(1);
			break;
		}
		if(ret == d_KBD_CANCEL)
			break;
	}

    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;
	while(1)
	{
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS REMOTE ID");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szTMSRemoteID);
	
		strcpy(strtemp,"New:") ; 
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		usLen = 32;
		//ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
		ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>= 1)
		{
			memcpy(strTCT.szTMSRemoteID,strOut,strlen(strOut));
			strTCT.szTMSRemoteID[strlen(strOut)]=0;
			inResult = inTCTSave(1);
			break;
		}
		if(ret == d_KBD_CANCEL)
			break;
	}
	
    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return;
	while(1)
	{
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS REMOTE PASSWORD");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szTMSRemotePW);
	
		strcpy(strtemp,"New:") ; 
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		usLen = 32;
		//ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
		ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>= 1)
		{
			memcpy(strTCT.szTMSRemotePW,strOut,strlen(strOut));
			strTCT.szTMSRemotePW[strlen(strOut)]=0;
			inResult = inTCTSave(1);
			break;
		}
		if(ret == d_KBD_CANCEL)
			break;
	
	}
    }
    else if(strTCT.inTMSComMode == GPRS_MODE)
    {
		CTMS_GPRSInfo stgprs;
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);

		while(1)
		{	
			vduiClearBelow(3);
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "SIM SLOT");
			setLCDPrint(4, DISPLAY_POSITION_LEFT, "1~2");
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "%d", stgprs.bSIMSlot);
			setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
			
			strcpy(strtemp,"New:") ;
			CTOS_LCDTPrintXY(1, 7, strtemp);
			memset(strOut,0x00, sizeof(strOut));
			ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
			if (ret == d_KBD_CANCEL )
				break;
			else if(0 == ret )
				break;
			else if(ret>=1)
			{
				if(atoi(strOut) == 1 || atoi(strOut) == 2)
				{
					if(atoi(strOut) == 1)
					    bSIMSlot=1;
					else
					    bSIMSlot=2;	
					break;
				}
				else
					vdDisplayErrorMsg(1, 8, "INVALID INPUT");
			}	
			if(ret == d_KBD_CANCEL)
				break;
		}	
						
        while(1)
        {
			vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "APN");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szAPN);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
			usLen = 32;
            ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if(ret==d_KBD_ENTER)
            {
                memcpy(strTCP.szAPN, strOut,strlen(strOut));
                strTCP.szAPN[strlen(strOut)]=0;
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }               
    
        while(1)
        {
			vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS USER NAME");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szUserName);
    
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
			usLen = 32;
            ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if(ret==d_KBD_ENTER)
            {
                //BolDetachGPRSChangeSetting=TRUE;
                memcpy(strTCP.szUserName, strOut,strlen(strOut));
                strTCP.szUserName[strlen(strOut)]=0;
                //inResult = inTCPSave(1);
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }               
    
        while(1)
        {
			vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS PASSWORD");
            setLCDPrint(4, DISPLAY_POSITION_CENTER, strTCP.szPassword);
    
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
			usLen = 32;
            ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if(ret==d_KBD_ENTER)
            {
                //BolDetachGPRSChangeSetting=TRUE;
                memcpy(strTCP.szPassword, strOut,strlen(strOut));
                strTCP.szPassword[strlen(strOut)]=0;
                //inResult = inTCPSave(1);
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        } 

        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		stgprs.bSIMSlot=bSIMSlot;
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

		CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }
	
}

void vdCTOS_TMSReSet(void)
{
	vdCTOS_InputTMSReSet();
}

int  inCTOS_TMSPreConfigSetting(void)
{
	BYTE strOut[30],strtemp[17];
	BYTE szInputBuf[5];
    int inResult;
    int ret;
	USHORT usLen;

    CTOS_LCDTClearDisplay();
    vdDispTitleString("TMS SETTINGS");
#if 0
    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return d_NO;	
	
    while(1)
    {   
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS COMM MODE");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "0-DIALUP 1-ETH");
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "2-GPRS");
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "%d", strTCT.inTMSComMode);
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            return d_NO;
        else if(0 == ret )
            return d_NO;
        else if(ret>=1)
        {
			if(strOut[0] == 0x30 || strOut[0] == 0x31 || strOut[0] == 0x32)
			{
                strTCT.inTMSComMode=atoi(strOut);      
                vdMyEZLib_LogPrintf("strTCT.usTMSGap %d",strTCT.inTMSComMode);
			    inResult = inTCTSave(1);
                break;				
			}
            else
                vdDisplayErrorMsg(1, 8, "INVALID INPUT");
        }   
    }	
#else
	inResult = inCPTRead(1);
    if(inResult != d_OK)
        return d_NO;

	strTCT.inTMSComMode=strCPT.inCommunicationMode;//Use Comm Type on transaction as TMSComMode      
    vdDebug_LogPrintf("strTCT.inTMSComMode %d",strTCT.inTMSComMode);
    inResult = inTCTSave(1);
#endif	
	
    return d_OK;
}

//aaronnino for remote download setup 10 of 12 end


/* BDOCLG-00131: Separate password for lock screen - start -- jzg */
void vdChangeLockPassword()
{
    BYTE strOut[30] = {0},
    strtemp[17] = {0};
    USHORT usLen = 0, shMaxLen, shMinLen=4;
    int ret = 0;
		UCHAR szPassword[12+1];

		
    inTCTRead(1);    

    memset(szPassword, 0, sizeof(szPassword));
//    strcpy(szPassword, strTCT.szLockPassword);

//    setLCDPrint(2, DISPLAY_POSITION_LEFT, strTCT.szLockPassword); 
//    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Lock Screen PWD");

    CTOS_LCDTClearDisplay();
    vdDispTitleString("Lock Screen PWD");
		
    while(1)
    { 	
        vduiClearBelow(4);
        CTOS_LCDTPrintXY(1, 7, "Enter Old Pwd:");
        memset(strOut,0x00, sizeof(strOut));
				shMaxLen=6;
        ret = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            return;
        else if(0 == ret)
            return;
				else if(ret == 255)
				    return;	
        else if(ret>=1)
        {
            if(strcmp(strOut, szPassword) == 0) 	
                break;
            else
                vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
        } 	
    }

    while(1)
    { 	
        vduiClearBelow(4);
        CTOS_LCDTPrintXY(1, 7, "Enter New Pwd:");
        memset(strOut,0x00, sizeof(strOut));
				shMaxLen=6;
        ret = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            return;
        else if(0 == ret )
            break;
				else if(ret == 255)
				    return;	
        else if(ret>=1)
        {
            if(strlen(strOut) > 0)
            {
//                memset(strTCT.szLockPassword, 0, sizeof(strTCT.szLockPassword));
//                strcpy(strTCT.szLockPassword, strOut);
                inTCTSave(1);
                break;
            }
        } 	
    }
}
/* BDOCLG-00131: Separate password for lock screen - end -- jzg */
//aaronnino for BDOCLG ver 9.0 fix on issue #00241 No Manual Settle/Clear Batch prompt after 3 failed start
int inAutoManualSettle()
{
   int shHostIndex = 1;
   int inResult = 0;
   char szErrMsg[31] = {0};
	 ACCUM_REC srAccumRec;
   
   vdDebug_LogPrintf("MANUAL SETTLE AFTER FAILED SETTLEMENT");
   
   CTOS_LCDTClearDisplay();	
   vdDispTitleString("CLEAR BATCH");
   
   shHostIndex = strHDT.inHostIndex;
   //vdCTOS_PrintDetailReportForManualSettlement(shHostIndex);
   //vdCTOS_PrintManualPosting();
   
   inResult=inCTOS_PrintSettleReport(TRUE, FALSE);	
   if(d_OK != inResult)
	 {
      if (inGetErrorMessage(szErrMsg) > 0)
      {
         CTOS_LCDTClearDisplay();
         vdDisplayErrorMsg(1, 8, szErrMsg);
         vdSetErrorMessage("");
      }
      return;
   }
//	 if(srTransRec.fManualSettlement == TRUE)
//          vdCTOS_PrintManualPosting();
	 int inOnlineMMTSettleLimitVal = 0;
//   int inOnlineMMTSettleLimitVal = strMMT[0].inOnlineMMTSettleLimit;
	 if ((inOnlineMMTSettleLimitVal > 98) || (inOnlineMMTSettleLimitVal == 0))
	 	{
	 	     inOnlineMMTSettleLimitVal = 99;
	 	}
   
   //aaronnino for BDOCLG ver 9.0 fix on issue #00386 Settle failed retries is not resetting after you cancel the manual START
//   if (strMMT[0].inOnlineMMTSettleTries > inOnlineMMTSettleLimitVal)
   {
      inResult = inCTOS_SettlementClearBathAndAccum(TRUE);
      if(inResult == d_OK)
      {
      #if 0
      CTOS_LCDTClearDisplay();
      CTOS_LCDTPrintXY(1, 8, "BATCH DELETED");
      CTOS_Beep();
      CTOS_Delay(2000);
      #else
		  vdDisplayErrorMsgResp2(" ", " ","BATCH DELETED");
		  #endif
      return inResult;
      }
//      strMMT[0].inOnlineMMTSettleTries = 0;
   }
   /*
      inResult = vduiAskConfirmContinue();
      if(inResult != d_OK)
      {
      strMMT[0].inOnlineMMTSettleTries = 0;
      return inResult;
      }
      */
   //aaronnino for BDOCLG ver 9.0 fix on issue #00386 Settle failed retries is not resetting after you cancel the manual END
   inResult = inCTOS_SettlementClearBathAndAccum(TRUE);
	 
   if(inResult == d_OK)
   {
      CTOS_LCDTClearDisplay();
      CTOS_LCDTPrintXY(1, 8, "BATCH DELETED");
      CTOS_Beep();
      CTOS_Delay(2000);
      return inResult;
   }
   
   CTOS_KBDBufFlush();
   
   return d_OK;
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00241 No Manual Settle/Clear Batch prompt after 3 failed end

void vdDisplayEnvMenuOption(char *pszTag, char *pszValue)
{
	/*display current tag & value*/
	vduiClearBelow(3);
	setLCDPrint(3, DISPLAY_POSITION_LEFT, pszTag);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, pszValue);
		
	/*F1-F4 func display*/
	CTOS_LCDTPrintXY(1, 5, "1 - NEW");
	CTOS_LCDTPrintXY(20-10, 5, "2 - FIND");

	CTOS_LCDTPrintXY(1, 6, "3 - EDIT");
	CTOS_LCDTPrintXY(20-9, 6, "4 - DEL");

	CTOS_LCDTPrintXY(1, 8, "7 - PREV");
	CTOS_LCDTPrintXY(20-9, 8, "9 - NEXT");
}

void vdCTOS_EditEnvParamDB(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    USHORT usLen;


	unsigned char keyval;
    BOOL isKey;

	char szCurrTag[64+1];
	char szCurrValue[128+1];

	char szNewTag[64+1];
	char szNewValue[128+1];

	int inEnvTotal = 0;
	int inIdx = 0;

	int inFindIdx = 0;
    char szFindTag[64+1];

	/*title*/
    CTOS_LCDTClearDisplay();
    vdDispTitleString("EDIT ENV PARAM");

	vdDebug_LogPrintf("=====vdCTOS_EditEnvParamDB=====");
	
	inEnvTotal = inCTOSS_EnvTotalDB();

	vdDebug_LogPrintf("inCTOSS_EnvTotal inEnvTotal[%d]", inEnvTotal);
	
	//if (0 == inEnvTotal)
		//return;
	
	inIdx = 1;

	/*retrive Env data by index*/
	memset(szCurrTag, 0, sizeof(szCurrTag));
	memset(szCurrValue, 0, sizeof(szCurrValue));
	if (inEnvTotal > 0)
	{
		inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
	}
	else
	{
		strcpy(szCurrTag, "__________");
		strcpy(szCurrValue, "__________");
	}
	//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

	vdDisplayEnvMenuOption(szCurrTag, szCurrValue);

	/*set idle timout*/
	CTOS_TimeOutSet(TIMER_ID_3, 3000);
	
    while(1)
    {
    	/*check timeout*/
		if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return;
        }

		keyval = 0;
		
		/*wait for user*/
		CTOS_KBDInKey(&isKey);
        if (isKey)
		{ //If isKey is TRUE, represent key be pressed //
			vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&keyval);

			/*set idle timout agian*/
			CTOS_TimeOutSet(TIMER_ID_3, 3000);
        }

		switch (keyval)
		{
			case d_KBD_1: //New
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
						
				strcpy(strtemp,"TAG:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewTag, strOut);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						//inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
					}
				}

				/*new tag not set, break*/
				if (strlen(szNewTag) <= 0)
					break;

				vduiClearBelow(7);
				strcpy(strtemp,"VAL:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewValue, strOut);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						//inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
					}
				}

				/*new tag not set, break*/
				if (strlen(szNewValue) <= 0)
					break;

				
				//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
				inCTOSS_PutEnvDB(szNewTag, szNewValue);
				inEnvTotal = inCTOSS_EnvTotalDB();
				inIdx = inEnvTotal;

				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				
				break;
				
			case d_KBD_2: //Find
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
						
				strcpy(strtemp,"TAG:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						//strcpy(szNewValue, strOut);
						//strcpy(szNewTag, szCurrTag);
						strcpy(szFindTag, strOut);

						inCTOSS_GetEnvIdxDB(szFindTag, &inFindIdx);
						//vdDebug_LogPrintf("inIdx %d szFindTag [%s]", inFindIdx, szFindTag);
						inIdx = inFindIdx;
					}
					else
					{

					}
				}
				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_3: //Edit
				
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
				
				strcpy(strtemp,"New:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewValue, strOut);
						strcpy(szNewTag, szCurrTag);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						inCTOSS_PutEnvDB(szNewTag, szNewValue);
						//break;
					}
					else
					{
						vduiClearBelow(4);
				
						vduiDisplayStringCenter(5,"NO VALUE SET");
						//vduiDisplayStringCenter(7,"PREV[UP] NEXT[DOWN]");
						//vduiDisplayStringCenter(7,"PABX?");
						//vduiDisplayStringCenter(8,"NO[X] YES[OK]");
						//key=struiGetchWithTimeOut();
					}
				}

				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_4: //Del
				inCTOSS_DelEnvDB(szCurrTag);
				inEnvTotal = inCTOSS_EnvTotalDB();
				inIdx = 1;
				/*retrive Env data by index*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
					inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_7: //prev
				inIdx--;
				if (inIdx < 1)
					inIdx = inEnvTotal;
				/*retrive Env data by index*/
    			memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_9: //next
				inIdx++;
				if (inIdx > inEnvTotal)
					inIdx = 1;
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_CANCEL: //exit
				return ;
				//break;
			default:
				break;
		}
	}

}
void vdCTOS_ModemReceivingTime(void)
{
    BYTE strOut[30],strtemp[17];
		BYTE szInputBuf[5];
    USHORT ret;
    USHORT usLen;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("MODEM RECEIVING TIME");
    while(1)
      		{ 	
      				vduiClearBelow(3);
      				setLCDPrint(3, DISPLAY_POSITION_LEFT, "MODEM RECEIVING TIME");
      				memset(szInputBuf, 0x00, sizeof(szInputBuf));
//      				sprintf(szInputBuf, "%d", strTCT.inModemReceivingTime);
      				setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
      				
      				strcpy(strtemp,"New:") ;
      				CTOS_LCDTPrintXY(1, 7, strtemp);
      				memset(strOut,0x00, sizeof(strOut));
      				ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
      				if (ret == d_KBD_CANCEL )
      						break;
      				else if(0 == ret )
      						break;
      				else if(ret>=1)
      				{
//      						strTCT.inModemReceivingTime = atoi(strOut);
      						break;
      				} 	
      				if(ret == d_KBD_CANCEL)
      						break;
      		}
							
							inRet = inTCTSave(1);
       
    return ;
}


void vdCTOS_BINRouting(void)
{
    BYTE strOut[30],strtemp[17];
    USHORT ret;
    USHORT usLen;
    int inRet = 0;
	//int inCDTCount= 0;

	//inCDTCount = inCDTMAX();

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("BIN ROUTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "ENABLE BIN ROUTING?");
//        if(strTCT.fATPBinRoute== 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
//        if(strTCT.fATPBinRoute == 1)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        
        CTOS_LCDTPrintXY(1, 5, "0-NO        1-YES");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
				
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x31) 
            {
                 if(strOut[0] == 0x31)
                 {
//                        strTCT.fATPBinRoute = 1;
                 }
                 //inEnableDCTBIN(0,inCDTCount-1);
			     //inEnableDCTBIN(1, inCDTCount);
				 
                 inRet = inTCTSave(1);
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 inTCTRead(1);
                 return;
             }
						if (strOut[0]==0x30) 
            {
                 if(strOut[0] == 0x30)
                 {
//                        strTCT.fATPBinRoute = 0;
                 }
				 
                 //inEnableDCTBIN(1,inCDTCount-1);
			     //inEnableDCTBIN(0, inCDTCount);
                 inRet = inTCTSave(1);
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);

                 inTCTRead(1);
                 return; 
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"WRONG INPUT");
                CTOS_Delay(2000);  
            }
						 
        }
      
    }
       
    return ;
}

/*ECR Configuration -- sidumili*/
void vdSetECRConfig(void)
{
unsigned char chkey;
short shHostIndex;
int inResult,inRet;
char szStr[d_LINE_SIZE + 1];
ULONG ulTraceNo=0L;
USHORT usLen=0;
BYTE strOut[30],strtemp[17],key;
BYTE szInputBuf[15+1];
    int ret;

CTOS_LCDTClearDisplay();
vdDispTitleString("ECR CONFIG");

inTCTRead(1);
while(1)
{
	vduiClearBelow(3);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enable ECR");
    if(strTCT.fECR== 0)
        setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
    else
        setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");     
    
    CTOS_LCDTPrintXY(1, 5, "0-DISABLE    1-ENABLE");
    
    strcpy(strtemp,"New:") ;
    CTOS_LCDTPrintXY(1, 7, strtemp);
    memset(strOut,0x00, sizeof(strOut));
    ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
    if (ret == d_KBD_CANCEL)
        break;
    else if(ret == 0) 
        break;
    else if(ret == 1)
    {
       if (strOut[0]==0x30 || strOut[0]==0x31)
       {
	   	  if(strOut[0]==0x30)    
              strTCT.fECR=FALSE;
          else if(strOut[0]==0x31)
              strTCT.fECR=TRUE;

		  inRet = inTCTSave(1);
          vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
				
          break;
       }
       else
       {
           vduiWarningSound();
		   setLCDPrint(6, DISPLAY_POSITION_LEFT, "INVALID SELECTION");
           CTOS_Delay(2000);
       }
    }
}

 /***************************************************************************/
// ECR Port -- sidumili
/***************************************************************************/
while(1)
	{
		vduiClearBelow(2);
		setLCDPrint(2, DISPLAY_POSITION_LEFT, "ECR Port");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));
		sprintf(szInputBuf, "%d", strTCT.byRS232ECRPort);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf);

		CTOS_LCDTPrintXY(1, 4, "0-NONE     1-COM1");
		CTOS_LCDTPrintXY(1, 5, "2-COM2     8-USB");
		CTOS_LCDTPrintXY(1, 7, "New:");

		strcpy(strtemp,"New:") ;
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 2, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
	 	 break;
		else if(0 == ret )
		  break;
		else if(ret>=1)
		{
			if (strOut[0]==0x30 ||strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x38)
			{
				strTCT.byRS232ECRPort = atoi(strOut); 
				inRet = inTCTSave(1);
                vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
				break;
			}
			else
			{
				vduiWarningSound();
				setLCDPrint(6, DISPLAY_POSITION_LEFT, "INVALID SELECTION");
				CTOS_Delay(2000);
			}
			
		}   
		if(ret == d_KBD_CANCEL)
		break;
	}

/***************************************************************************/

 /***************************************************************************/
// ECR BaudRate -- sidumili
/***************************************************************************/
while(1)
	{
		vduiClearBelow(2);
		setLCDPrint(2, DISPLAY_POSITION_LEFT, "ECR Baudrate");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));
//		sprintf(szInputBuf, "%d", strTCT.fECRBaudRate);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf);

		CTOS_LCDTPrintXY(1, 4, "1-9600");
		CTOS_LCDTPrintXY(1, 5, "2-19200");
		CTOS_LCDTPrintXY(1, 6, "3-115200");
		CTOS_LCDTPrintXY(1, 7, "New:");

		strcpy(strtemp,"New:") ;
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 2, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
	 	 break;
		else if(0 == ret )
		  break;
		else if(ret>=1)
		{
			if (strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33)
			{
//				strTCT.fECRBaudRate = atoi(strOut); 
				inRet = inTCTSave(1);
                vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
				break;
			}
			else
			{
				vduiWarningSound();
				setLCDPrint(6, DISPLAY_POSITION_LEFT, "INVALID SELECTION");
				CTOS_Delay(2000);
			}
			
		}   
		if(ret == d_KBD_CANCEL)
		break;
	}

/***************************************************************************/

/***************************************************************************/
// Main Flag for ECR Menu
while(1)
		   {   
				   vduiClearBelow(2);
				   setLCDPrint(2, DISPLAY_POSITION_LEFT, "Enable ECR Menu");
				   memset(szInputBuf, 0x00, sizeof(szInputBuf));
//				   sprintf(szInputBuf, "%d", strTCT.fSelectECRTrxn);
				   setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf);
				   
				   strcpy(strtemp,"New:") ;
				   CTOS_LCDTPrintXY(1, 7, strtemp);
				   memset(strOut,0x00, sizeof(strOut));
				   ret= shCTOS_GetNum(8, 0x01,	strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
				   if (ret == d_KBD_CANCEL )
						   break;
				   else if(0 == ret )
						   break;
				   else if(ret>=1)
				   {
//						   strTCT.fSelectECRTrxn = atoi(strOut);
						   inRet = inTCTSave(1);
						   break;
				   }   
				   if(ret == d_KBD_CANCEL)
						   break;
		   }

/***************************************************************************/

/***************************************************************************/
// ECR Menu
/***************************************************************************/
while(1)
		   {   
				   vduiClearBelow(2);
				   setLCDPrint(2, DISPLAY_POSITION_LEFT, "ECR Menu Index");
				   memset(szInputBuf, 0x00, sizeof(szInputBuf));
//				   sprintf(szInputBuf, "%d", strTCT.inECRTrxnMenu);
				   setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf);
				   
				   strcpy(strtemp,"New:") ;
				   CTOS_LCDTPrintXY(1, 7, strtemp);
				   memset(strOut,0x00, sizeof(strOut));
				   ret= shCTOS_GetNum(8, 0x01,	strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
				   if (ret == d_KBD_CANCEL )
						   break;
				   else if(0 == ret )
						   break;
				   else if(ret>=1)
				   {
//						   strTCT.inECRTrxnMenu = atoi(strOut);
						   inRet = inTCTSave(1);
						   break;
				   }   
				   if(ret == d_KBD_CANCEL)
						   break;
		   }

/***************************************************************************/
// ECR ISO Logging
/***************************************************************************/
while(1)
		   {   
				   vduiClearBelow(2);
				   setLCDPrint(2, DISPLAY_POSITION_LEFT, "ECR ISO Log");
				   memset(szInputBuf, 0x00, sizeof(szInputBuf));
//				   sprintf(szInputBuf, "%d", strTCT.fECRISOLogging);
				   setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf);
				   
				   strcpy(strtemp,"New:") ;
				   CTOS_LCDTPrintXY(1, 7, strtemp);
				   memset(strOut,0x00, sizeof(strOut));
				   ret= shCTOS_GetNum(8, 0x01,	strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
				   if (ret == d_KBD_CANCEL )
						   break;
				   else if(0 == ret )
						   break;
				   else if(ret>=1)
				   {
//						   strTCT.fECRISOLogging = atoi(strOut);
						   inRet = inTCTSave(1);
						   break;
				   }   
				   if(ret == d_KBD_CANCEL)
						   break;
		   }

 /***************************************************************************/
// ECR mode -- sidumili
/***************************************************************************/
while(1)
	{
		vduiClearBelow(2);
		setLCDPrint(2, DISPLAY_POSITION_LEFT, "ECR Mode");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));
//		sprintf(szInputBuf, "%d", strTCT.inECRMode);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf);

		CTOS_LCDTPrintXY(1, 4, "0-Windows ECR");
		CTOS_LCDTPrintXY(1, 5, "1-Linux ECR");
		
		CTOS_LCDTPrintXY(1, 7, "New:");

		strcpy(strtemp,"New:") ;
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 2, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
	 	 break;
		else if(0 == ret )
		  break;
		else if(ret>=1)
		{
			if (strOut[0]==0x30 || strOut[0]==0x31)
			{
//				strTCT.inECRMode= atoi(strOut); 
				inRet = inTCTSave(1);
                vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
				break;
			}
			else
			{
				vduiWarningSound();
				setLCDPrint(6, DISPLAY_POSITION_LEFT, "INVALID SELECTION");
				CTOS_Delay(2000);
			}
			
		}   
		if(ret == d_KBD_CANCEL)
		break;
	}

/***************************************************************************/


vdCTOS_uiRestart(TRUE);

}

void vdCTOS_ECRConfig(void)
{
	vdSetECRConfig();	
}

void vdCTOS_SMFunctionKey(void)
{
	int ret = 0,
		inRet = d_OK;
	BYTE strOut[2] = {0}, 
		szErrMsg[22] = {0};
	USHORT usLen = 2;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("FUNCTION MENU");

	CTOS_LCDTPrintXY(1, 7, "FUNCTION?");

	memset(strOut, 0, sizeof(strOut));
	//ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
	
	ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, 60 * 100);
	if(ret == d_KBD_ENTER)
	{

		memset(szFuncTitleName,0,sizeof(szFuncTitleName)); //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 2 of 6
	 
		switch(atoi(strOut))
		{
			case 0:
				inCTOS_REPRINTF_LAST_SETTLEMENT();
				break;
				
			case 1:
				#if 0
				memcpy(szFuncTitleName,"CLEAR",11);
				vdCTOS_SetTransType(SETUP);
				vdFunctionOne();
				#endif

				vdCTOSS_CheckMemory();
				break;
				
			/*case 2:
				vdCTOS_HostInfo();
				break;*/

			case 3:
//				inCTOS_SMACLOGON();
				break;
				
			case 4:
				vdCTOS_PrintEMVTerminalConfig();
				break;
				
			case 5:
//				inCTOS_LOGON();
				break;

			case 6:
				vdPrintReportDisplayBMP();
				vdCTOS_PrintCRC();
				break;

			case 8:
				memcpy(szFuncTitleName,"CHANGE COMMS",12);
//				if(inFunctionKeyPassword("CHANGE COMMS", SUPER_ENGINEER_PWD)==d_OK)
//				{	
//					CTOS_LCDTClearDisplay();
//					vdDispTitleString("CHANGE COMMS");
//			    	vdChangeComms();
//				}
				break;

			/* FOR ECR LOG -- sidumili*/
			case 9:
//				WIFI_Scan();
				break;
				
			case 10:
				memcpy(szFuncTitleName,"ECR LOG",10);
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
				inCTOS_PromptPassword();
				inECRLogMenu();
			break;	
			/* FOR ECR LOG -- sidumili*/
			
			case 11:
				memcpy(szFuncTitleName,"SETUP",5);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 4 of 9
				vdCTOS_SetTransType(SETUP);
				inCTOS_PromptPassword();

				/* BDOCLG: change to BDO color scheme if in icon mode - start -- jzg */
				inTCTRead(1);
				if((strTCT.byTerminalType % 2) == 0)
				{
					CTOS_LCDForeGndColor(RGB(13, 43, 112));
					CTOS_LCDBackGndColor(RGB(255, 255, 255));
				}
				/* BDOCLG: change to BDO color scheme if in icon mode - end -- jzg */
				
				vdDisplaySetup();
				break;

			case 13:
				inCTOSS_UploadReceipt();
			break;	

			case 14:
				inCTOS_ERMAllHosts_DeInitialization();
			break;	

			case 21:
				memcpy(szFuncTitleName,"REPRINT CTMS REPORT",19);
				if(inFunctionKeyPasswordEx("REPRINT CTMS REPORT", 1 /*SUPER_PW*/) == d_OK) //SYSTEM PASSWORD	
				{
					if(inCheckTMSReprintBackUpFilesExists() == d_OK)
						inCTOSS_TMSReadReprintDetailData();
					else
					{
						vdDisplayErrorMsgResp2("","NO CTMS","REPORT SAVED");
						return;
					}
					fRePrintFlag = TRUE;	
					inPrintADCReport(FALSE);
				}
				
				break;
			
			case 30:
//				memcpy(szFuncTitleName,"BIN ROUTING",11);
//				vdCTOS_SetTransType(BIN_ROUTING);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
//				inCTOS_PromptPassword();
//				vdCTOS_BINRouting(); 
				break;

			case 31:
				memcpy(szFuncTitleName,"BIN ROUTING NII",15);
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
//				if(inFunctionKeyPassword("BIN ROUTING NII", SUPER_ENGINEER_PWD)==d_OK)
//				    vdCTOS_BINRoutingNII();
				break;

				case 33: 
				memcpy(szFuncTitleName,"PING IP ADDRESS",15);
				vdCTOS_SetTransType(SETUP);
				vdCTOS_PingIPAddress();
				break;	

				case 50:
				memcpy(szFuncTitleName,"MENU SETTINGS",13); //aaronnino for BDOCLG ver 9.0 fix on issue #00181 Display on Function 7 was carrying over the display of the previous funtion used
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 5 of 9
				inCTOS_PromptPassword(); ; //aaronnino for BDOCLG ver 9.0 fix on issue #000106 Input Password for Func 7 (Engineer Password)
				vdCTOS_ChangeMenuID();
				break;	

				case 51:
 				memcpy(szFuncTitleName,"PRINT ISO",9);
				vdCTOS_SetTransType(SETUP);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 6 of 9
				inCTOS_PromptPassword();
				vdCTOS_PrintISOMode(); 
				break;

			case 88:
				memcpy(szFuncTitleName,"TMS DOWNLOAD",12);
				if(inFunctionKeyPasswordEx("TMS DOWNLOAD", 2)==d_OK) //SYSTEM PASSWORD
				{	
					CTOS_LCDTClearDisplay();
					vdDispTitleString("TMS DOWNLOAD");
			    	inCTOSS_TMSDownloadRequest();
				}
				break;

#if 0
			case 89:
				//memcpy(szFuncTitleName,"WRITE EXPIRY",12);
					
				CTOS_LCDTClearDisplay();
				//vdDispTitleString("WRITE EXPIRY");
				
		    	inWriteExpiry();
				
				break;
#endif				
			case 90:
				memcpy(szFuncTitleName,"Lock Screen PWD",15); //aaronnino for BDOCLG ver 9.0 fix on issue #00219 carrying  over the header display
				//inCTOS_PromptPassword(); //aaronnino for BDOCLG ver 9.0 fix on issue #00219 
				vdChangeLockPassword();
				break;

			case 91:
				inTCTRead(1);
//				strTCT.fScreenLocked = TRUE;
				put_env_int("LOCKPROMPT", 1);
				inTCTSave(1);
				break;
				
			case 94:
				vdCTOS_SetTransType(SETUP); 	
				memcpy(szFuncTitleName,"MODEM RECEIVING TIME",20);
				inCTOS_PromptPassword();
				vdCTOS_ModemReceivingTime(); //aaronnino for BDOCLG ver 9.0 fix on issue #0092 Make Function 1 shorcut key to manual settlement function instead of clear batch
				break;
//tms
			case 95:
				vdCTOS_TMSReSet();
				break;
//SIT
#if 0
      		case 96: 
					inCTOSS_TMSDownloadRequest();
			    break;
			case 97: 
					vdCTOS_TMSSetting();
			    break;

//tms       
			case 97:
				CTOS_KMS2KeyDelete(0xC001, 0x0001);
				vdDisplayErrorMsgResp2("","SMAC KEYS","ERASED");
				break;
#endif				
			case 98:
				vdCTOS_uiRestart(TRUE);
				break; //BDO-00150: added break to make sure not to run vdCTOS_uiPowerOff() when cancel key is pressed -- jzg

			case 99: 
				vdCTOS_uiPowerOff();		
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

int inECRLogMenu(void){

	int inRet;
	char szTemp[7+1], szTemp2[6+1];
	char 	*chNull = 0;
	int inSelected=3;
	BYTE key;

	BYTE byItmesNum = 2;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	char szHeaderString[50] = "SELECT ECR LOG";
	char szTenderMenu[1024];
	int inLoop = 0;

	BYTE szTenderList[3][32] = 
		{	"PRINT ECR LOG",
			"DELETE ECR LOG"
		};



	CTOS_LCDTClearDisplay();
	vdDispTransTitle(SETUP);
	
	CTOS_KBDBufFlush();//cleare key buffer

    if(byItmesNum > 1)
    {
        
        //issue-00436: clear the buffer first to avoid garbage display
		memset(szTenderMenu, 0x00, sizeof(szTenderMenu));
		
        for (inLoop = 0; inLoop < byItmesNum; inLoop++)
        {
            strcat((char *)szTenderMenu, &szTenderList[inLoop]);
            if(inLoop + 1 != byItmesNum)
                strcat((char *)szTenderMenu, (char *)" \n");
        }

		if((strTCT.byTerminalType % 2) == 0)
		{
			CTOS_LCDForeGndColor(RGB(13, 43, 112));
			CTOS_LCDBackGndColor(RGB(255, 255, 255));
		}
			
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szTenderMenu, TRUE);
		vdDebug_LogPrintf("MenuDisplay select[%d]", key);

		
        inSetTextMode(); 
				 
        if (key == 1)
		{
			inRet = d_OK;
			inSelected = 1;
		}
		else if (key == 2)
		{
			inRet = d_OK;	
			inSelected = 2;
		}
		else if (key == d_KBD_CANCEL)
		{
			inRet = d_NO;
			return inRet;
		}
		else
			vduiWarningSound();
		
    }

	switch (inSelected){
	case 1:
		vdPrintISOECRLog();
	break;	
	case 2:
		vdDeleteISOECRLog();
	break;	
	}
	
	inRet = d_OK;
	
	return inRet;

}


int inCTOS_SelectHostSettingWithIndicator(int inIndicator)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[6144];
    char szHostName[400][400];
    int inCPTID[400];
    int inLoop = 0;
	BOOL fPutAsterisk = FALSE;

/*values for inIndicator = 1=check if batch has record; 2=if there's pending reversal*/
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    

    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));

    //inHDTReadHostName(szHostName, inCPTID);

	inMMTReadHostName(szHostName, inCPTID, srTransRec.MITid, inIndicator);


    for (inLoop = 0; inLoop < 150; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");  
		
		vdDebug_LogPrintf("szHostMenu %s", szHostMenu);
        }
        else
            break;
    }


	vdDebug_LogPrintf("szHostMenu2 %s %d", szHostMenu, strlen(szHostMenu));

		inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();
		
    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        //if(d_KBD_CANCEL == key)
            //return -1;

		if (get_env_int("INVALIDKEY") == 1)
			return -1;
        
        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = inCPTID[key-1];
        strHDT.inHostIndex = inCPTID[key-1];
        inHDTRead(inCPTID[key-1]);
        inCPTRead(inCPTID[key-1]);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		inCSTRead(strHDT.inCurrencyIdx); /*to load Currence and amount format*/
    }
    
    return inCPTID[key-1];
}


void vdCTOS_AuthenticationType(void)
{
	int inKeySelected = 0;
	char szHeaderString[50] = {0};
	char szChoiceMsg[1024] = {0};
	int bHeaderAttr = 0x01+0x04, key=0; 
	BYTE szTemp1[20 + 1] = {0};
	int inValue = 0;
	int inAuthType;

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SET GSM AUTH TYPE");

	inAuthType = get_env_int("GSMAUTHTYPE");
	
	memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	memset(szTemp1, 0x00, sizeof(szTemp1));
	
	switch(inAuthType)
	{
		case AUTH_TYPE_AUTO:
			strcpy(szTemp1, "AUTO");
			break;
		case AUTH_TYPE_PAP:
			strcpy(szTemp1, "PAP");
			break;
		case AUTH_TYPE_CHAP:
			strcpy(szTemp1, "CHAP");
			break;
		default:
			strcpy(szTemp1, "NONE");
			break;
	}

	sprintf(szHeaderString, "AUTH TYPE: %s", szTemp1);	
	strcpy((char*)szChoiceMsg, "AUTO \nPAP \nCHAP ");
	inKeySelected = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
	
	vdDebug_LogPrintf("::AUTH TYPE::inSetMenuSelection::inKeySelected[%d]", inKeySelected);
	
	if (inKeySelected != d_KBD_CANCEL)
	{
		inValue = inKeySelected - 1; // (0-AUTO, 1-PAP, 2-CHAP)
		put_env_int("GSMAUTHTYPE", inValue);		
	}
	
	vdDebug_LogPrintf("::AUTH TYPE::inKeySelected[%d]::szHeaderString[%s]::inValue[%d]", inKeySelected, szHeaderString, inValue);
       
    return ;
}

void vdCTOS_PingIPAddress(void)
{
		int inCPTNumRecs = 0,
    i = 0,
    inInput = 0,
    inCommMode =-1;
		BYTE strtemp[17];
		BYTE strOut[30] = {0};
		USHORT usLen = 0,
    inResult = 0,
		ret = 0;
		int x = 0;

		inTCTRead(1);
    inCPTRead(1);
	
		while(1)
					{
							vduiClearBelow(3);
							setLCDPrint(3, DISPLAY_POSITION_LEFT, "ENTER IP ADDRESS:");
							
							strcpy(strtemp,"IP:") ;
							CTOS_LCDTPrintXY(1, 7, strtemp);
							memset(strtemp, 0x00, sizeof(strtemp));
							memset(strOut,0x00, sizeof(strOut));
							ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
							
							if(ret==d_KBD_ENTER)
							{
									if ( strCPT.inCommunicationMode == ETHERNET_MODE)
                  {
      									inResult = CTOS_EthernetOpenEx();
										vdDebug_LogPrintf(". CTOS_EthernetOpenEx Ret=(%04x)",inResult);
										{//fix power on, first time connot connect
											CTOS_Delay(100);
											inResult = CTOS_EthernetClose();
											vdDebug_LogPrintf(". CTOS_EthernetClose Ret=(%04x)",inResult);
											CTOS_Delay(100);
											inResult = CTOS_EthernetOpenEx();
											vdDebug_LogPrintf(". CTOS_EthernetOpenEx2 Ret=(%04x)",inResult);
										}								
										 while(x < 3)
										 	{
												 inResult = 0;
												 setLCDPrint(5, DISPLAY_POSITION_LEFT, "PINGING IP...");
												 CTOS_Delay(500);
                         inResult = CTOS_EthernetPing(strOut,strlen(strOut));
                         vdDebug_LogPrintf("AAA - ETHERNENT inResult[%05x][%s]", inResult, strOut);
												 x++;
										 	}
										 CTOS_EthernetClose();
									}
									else if ( strCPT.inCommunicationMode == WIFI_MODE)
                  {
										 while(x < 3)
										 	{
												 inResult = 0;
												 setLCDPrint(5, DISPLAY_POSITION_LEFT, "PINGING IP...");
												 CTOS_Delay(500);
                         inResult = CTOS_WifiPing(strOut,strlen(strOut));
                         vdDebug_LogPrintf("AAA - WIFI inResult[%05x][%s]", inResult, strOut);
												 x++;
										 	}
									}
									else if ( strCPT.inCommunicationMode == GPRS_MODE)
                  {
										 while(x < 3)
										 	{
													 inResult = 0;
													 setLCDPrint(5, DISPLAY_POSITION_LEFT, "PINGING IP...");
													 CTOS_Delay(500);
                           inResult = inCTOSS_GPRSPing(strOut);
                           vdDebug_LogPrintf("AAA - GPRS inResult[%05x][%s]", inResult, strOut);
													 x++;
										 	}        
									}
									else
										break;

									if (inResult == 00000)
										{
                           vdDisplayErrorMsgResp2("","PING IP","SUCCESS");
													 break;
										}
                 else
                  	{
                          vdDisplayErrorMsgResp2("","PING IP","FAILED");   
     											break;
                  	}
									
									
							} 	
							if(ret == d_KBD_CANCEL)
													break;
						}
					CTOS_Delay(1000);
}

void vdCTOS_ISOLogger(void)
{
   BYTE strOut[30],strtemp[17];
   USHORT ret;
   USHORT usLen;
   int inRet = 0;
   
   inRet = inTCTRead(1);  
	 
   CTOS_LCDTClearDisplay();
   vdDispTitleString("PRINT ISO LOG");
   while(1)
   {
      clearLine(3);
      clearLine(4);
      clearLine(5);
      clearLine(6);
      clearLine(7);
      clearLine(8);
      //setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRINT ISO LOG");
      if(strTCT.fPrintISOMessage== 0)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "0");
      if(strTCT.byRS232DebugPort == 1)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "1");        
      
      //CTOS_LCDTPrintXY(1, 4, "0-RENAME ISO LOG");
      CTOS_LCDTPrintXY(1, 4, "1-PRINT ISO LOG");
			CTOS_LCDTPrintXY(1, 5, "2-DEL ALL ISO LOG");
      
      strcpy(strtemp,"Option:") ;
      CTOS_LCDTPrintXY(1, 7, strtemp);
      memset(strOut,0x00, sizeof(strOut));
      ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
      
      if (ret == d_KBD_CANCEL )
            break;
      else if(0 == ret )	
            break;

      else if(ret==1)
      {
           /* if (strOut[0]==0x30) 
            {
               CTOS_LCDTClearDisplay();
							 vdRenameISOLog(); 

               break;                 
            }*/
            if (strOut[0]==0x31) 
            {
               CTOS_LCDTClearDisplay();
               vdPrintISOLog();
               break;
            }
						if (strOut[0]==0x32) 
            {
               CTOS_LCDTClearDisplay();
               vdDeleteISOLog();
               break;
            }
            else
            {
               CTOS_LCDTClearDisplay();
               vduiWarningSound();
               vduiDisplayStringCenter(6,"WRONG INPUT");
               CTOS_Delay(2000);  
            }
      
      }
   
   }
   
   return ;
}


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


//powersave
void vdCTOS_uiIDLESleepMode(void)
{
    USHORT usRet1;
    BYTE  bPercentage ;
	UCHAR usRet;
    DWORD pdwStatus ;

	usRet1 = CTOS_PowerSource(&usRet);
	vdDebug_LogPrintf("CTOS_PowerSource=[%x]..usRet=[%d]..inSleepflag=[%d]....",usRet1,usRet,inSleepflag);

	if ((d_OK == usRet1) && (usRet == d_PWRSRC_BATTERY))
	{
		if (inSleepflag == 0)
		{
			vdDebug_LogPrintf("vdSetsysPowerLowSpeed......");
			//CTOS_BackLightSet (d_BKLIT_LCD, d_OFF);
			//CTOS_BackLightSet (d_BKLIT_KBD, d_OFF);
			///vdSetsysPowerLowSpeed();
			CTOS_PowerMode(d_PWR_STANDBY_MODE);
			inSleepflag = 1;

			vdCTOS_uiIDLEWakeUpSleepMode();
		}
	}
	else
		vdCTOS_uiIDLEWakeUpSleepMode();
	return;
	
}

int inCTOSS_CheckBatteryChargeStatus(void)
{
	USHORT usRet1;
	BYTE  bPercentage ;
	UCHAR usRet;
	DWORD pdwStatus ;

	usRet1 = CTOS_PowerSource(&usRet);
	//vdDebug_LogPrintf("CTOS_PowerSource=[%x]..usRet=[%d]......",usRet1,usRet);
	if ((d_OK == usRet1) && (usRet == d_PWRSRC_BATTERY))
	{
		vdDebug_LogPrintf("inCTOSS_CheckBatteryChargeStatus NO"); 
		return d_NO;
	}
	
	vdDebug_LogPrintf("inCTOSS_CheckBatteryChargeStatus OK"); 
	return d_OK;
	
}

void vdCTOS_uiIDLEWakeUpSleepMode(void)
{
	//vdDebug_LogPrintf("vdCTOS_uiIDLEWakeUpSleepMode,inSleepflag=[%d]......",inSleepflag);
	if (inSleepflag == 1)
	{
		//vdDebug_LogPrintf("vdSetsysPowerHighSpeed......");
		//vdSetsysPowerHighSpeed();
		CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
		CTOS_BackLightSet (d_BKLIT_KBD, d_ON);
		inSleepflag = 0;
		vdSetIdleEvent(1);
	}
	return;
}
//powersave


//adc
void vdCTOS_TMSRangeSetting(void)
{
	BYTE strOut[30],strtemp[17];
	BYTE szInputBuf[5];
    int inResult;
    int ret;
	USHORT usLen;
    BYTE szSerialNum[17 + 1];//, 

	//bSIMSlot=0x30;

	BYTE bSIMSlot = 0x32;

	int inStart;

	int inEnd;
	
		
    CTOS_LCDTClearDisplay();
    vdDispTitleString("AUTO DL TIME");
		
	while(1)
	{	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "RANGE 1 START");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

		inStart = 0;
		inStart= get_env_int("ADL1START");
		
		sprintf(szInputBuf, "%d", inStart);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
			
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
			if (ret == d_KBD_CANCEL )
				break;
			else if(0 == ret )
				break;
			else if(ret>=1)
			{
			    inStart=atoi(strOut);   
 				inResult = d_OK;
				break;
			}	
			if(ret == d_KBD_CANCEL)
				break;
	}	

	if(inResult != d_OK)
       return;	

	while(1)
	{	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "RANGE 1 END");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

	    inEnd = 0;
		inEnd= get_env_int("ADL1END");
	
		sprintf(szInputBuf, "%d", inEnd);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
		
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>=1)
		{	    
			inEnd=atoi(strOut);	
			inResult = d_OK;
			break;
		}	
		if(ret == d_KBD_CANCEL)
			break;
	}	


 	if(inResult != d_OK)
		return; 
	
	put_env_int("ADL1START",inStart);
	put_env_int("ADL1END",inEnd);


	while(1)
	{	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "RANGE 2 START");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

		inStart = 0;
		inStart= get_env_int("ADL2START");
	
		sprintf(szInputBuf, "%d", inStart);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
		
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>=1)
		{
			inStart=atoi(strOut);	
			inResult = d_OK;
			break;
		}	
		if(ret == d_KBD_CANCEL)
			break;
	}	

 	if(inResult != d_OK)
		return; 

	while(1)
	{	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "RANGE 2 END");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

		inEnd = 0;
		inEnd= get_env_int("ADL2END");

		sprintf(szInputBuf, "%d", inEnd);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
	
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>=1)
		{		
			inEnd=atoi(strOut); 
			inResult = d_OK;
			break;
		}	
		if(ret == d_KBD_CANCEL)
			break;
	}	


	if(inResult != d_OK)
		return; 

	put_env_int("ADL2START",inStart);
	put_env_int("ADL2END",inEnd);



	while(1)
	{	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "RANGE 3 START");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

		inStart = 0;
		inStart= get_env_int("ADL3START");
	
		sprintf(szInputBuf, "%d", inStart);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
		
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>=1)
		{
			inStart=atoi(strOut);	
			inResult = d_OK;
			break;
		}	
		if(ret == d_KBD_CANCEL)
			break;
	}	

 	if(inResult != d_OK)
		return; 

	while(1)
	{	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "RANGE 3 END");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

		inEnd = 0;
		inEnd= get_env_int("ADL3END");

		sprintf(szInputBuf, "%d", inEnd);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
	
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>=1)
		{		
			inEnd=atoi(strOut); 
			inResult = d_OK;
		break;
		}	

		if(ret == d_KBD_CANCEL)
		break;
	}	


	if(inResult != d_OK)
		return; 

	put_env_int("ADL3START",inStart);
	put_env_int("ADL3END",inEnd);

	vdSetADLParams();

	while(1)
	{	
	
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "DOWLOAD TYPE");
		memset(szInputBuf, 0x00, sizeof(szInputBuf));

		inStart = 0;
		inStart= get_env_int("ADLTYPE");

		sprintf(szInputBuf, "%d", inStart);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
	
		strcpy(strtemp,"New:");
		CTOS_LCDTPrintXY(1, 7, strtemp);
		memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
		if (ret == d_KBD_CANCEL )
			break;
		else if(0 == ret )
			break;
		else if(ret>=1)
		{
			inStart=atoi(strOut);	
			inResult = d_OK;
			break;
		}	
		if(ret == d_KBD_CANCEL)
			break;
	}	

	if(inResult != d_OK)
		return; 

	put_env_int("ADLTYPE", inStart);

}

//adc


int inCTOS_vdSetDCCMode(void)
{
}

int inCTOS_SelectPreAuthHostSettingWithIndicator(int inIndicator)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[6144];
    char szHostName[400][400];
    int inCPTID[400];
    int inLoop = 0;
	BOOL fPutAsterisk = FALSE;

/*values for inIndicator = 1=check if batch has record; 2=if there's pending reversal*/
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    

    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));

    //inHDTReadHostName(szHostName, inCPTID);

	inMMTReadPreAuthHostName(szHostName, inCPTID, srTransRec.MITid, inIndicator);


    for (inLoop = 0; inLoop < 150; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");  
		
		vdDebug_LogPrintf("szHostMenu %s", szHostMenu);
        }
        else
            break;
    }


	vdDebug_LogPrintf("szHostMenu2 %s %d", szHostMenu, strlen(szHostMenu));

		inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();
		
    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        //if(d_KBD_CANCEL == key)
            //return -1;

		if (get_env_int("INVALIDKEY") == 1)
			return -1;
        
        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = inCPTID[key-1];
        strHDT.inHostIndex = inCPTID[key-1];
        inHDTRead(inCPTID[key-1]);
        inCPTRead(inCPTID[key-1]);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		inCSTRead(strHDT.inCurrencyIdx); /*to load Currence and amount format*/
    }
    
    return inCPTID[key-1];
}


void put_env_charEx(char *tag, char *value)
{
	int     ret = -1;
	char    buf[32+1];

    memset (buf, 0, sizeof (buf));
    //int2str (buf, value);
    sprintf(buf, "%s", value);
    ret = inCTOSS_PutEnvDB (tag, buf);

	vdDebug_LogPrintf("put_env_char [%s]=[%s] ret[%d]", tag, value, ret);
}

int inBackUpAppFilesToPenDrive(void)
{
    int inRet = d_NO;
    char szSystemCmdPath[250];
    BYTE exe_dir[128]={0};
    BYTE exe_subdir[128]={0};
    int inExeAPIndex = 0;
    BYTE szScriptPath[128]={0};
    BYTE szScriptFile[128]={0};
    
	char caPathbk[64 + 1];
	

    
    memset(caPathbk,0x00,sizeof(caPathbk));
	strcpy(caPathbk, DB_MULTIAP);
    
    memset(szSystemCmdPath,0x00,sizeof(szSystemCmdPath));
    sprintf(szSystemCmdPath, "cp -r %s /media/udisk/",caPathbk);
    system(szSystemCmdPath);
	vdDebug_LogPrintf("szSystemCmdPath[%s]",szSystemCmdPath);



    memset(szSystemCmdPath,0x00,sizeof(szSystemCmdPath));
    strcpy(szSystemCmdPath, "sync");
    system(szSystemCmdPath);

    return d_OK;
}

//call this fun to export file to pen drive
void vdCTOSS_ExportAllAppFiles(void)
{
    int inResult = d_NO;
    BYTE szExportFilename[100];
    char szSystemCmdPath[250];
    int loop,inStatus;

    CTOS_LCDTClearDisplay();
    vdDispTitleString("Export app");

    CTOS_LCDTPrintAligned(7, "Exporting...                            ", d_LCD_ALIGNLEFT);
    CTOS_USBSelectMode(d_USB_HOST_MODE);
    loop = 0;
        
    while (1)
    {
        CTOS_LCDTPrintXY(1, 8, "Checking Pen Drive");
        inStatus = inCTOSS_GetRemovableStorageStatus();
        if (inStatus == 2 || inStatus == 3)
        {
//			vdDebug_LogPrintf("Pen Drive........  ");
                CTOS_LCDTPrintXY(1, 8, "                              ");
                break;
        }
        else
        {
            //don't why need open many time, then can check the pen drive
            CTOS_USBSelectMode(d_USB_HOST_MODE);

            WaitKey(2);
            loop++;
            if (loop >=5)
            {
              CTOS_LCDTPrintXY(1, 8, "Pen Drive Not Exist");
              CTOS_Beep();
              WaitKey(1);
              CTOS_USBSelectMode(d_USB_DEVICE_MODE);

              return;
            }
        }
    }
    
    inBackUpAppFilesToPenDrive();
    
    CTOS_LCDTPrintAligned(8, "Export completed", d_LCD_ALIGNLEFT);
    CTOS_Beep();
    CTOS_USBSelectMode(d_USB_DEVICE_MODE);
    WaitKey(5);

    CTOS_LCDTClearDisplay();
 
    return;
	
}


int inCTOS_SelectCardOrCash(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "BILLS PAYMENT";
    char szHostMenu[1024] = {0};
    char szHostName[100][100];
    int inCPTID[100];
    int inLoop = 0;
    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    

    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));

    strcat((char *)szHostMenu, (char *)"CARD");  
    strcat((char *)szHostMenu, (char *)" \n");  
    strcat((char *)szHostMenu, (char *)"CASH");  
//        strcat((char *)szHostMenu, (char *)" \n");  
//    
//    inHDTReadHostName(szHostName, inCPTID);
//
//    for (inLoop = 0; inLoop < 100; inLoop++)
//    {
//        if (szHostName[inLoop][0]!= 0)
//        {
//            strcat((char *)szHostMenu, szHostName[inLoop]);
//            if (szHostName[inLoop+1][0]!= 0)
//                strcat((char *)szHostMenu, (char *)" \n");      
//        }
//        else
//            break;
//    }
        
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
        
        vdDebug_LogPrintf("key[%d]", key);

    }
    
    return key;
}