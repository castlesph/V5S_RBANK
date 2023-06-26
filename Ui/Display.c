#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>


#include "../Includes/msg.h"
#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/POSTypedef.h"
#include "../Includes/POSTrans.h"

#include "display.h"
#include "../FileModule/myFileFunc.h"
#include "../print/Print.h"
#include "../Includes/CTOSinput.h"
#include "../UI/Display.h"
#include "../Comm/V5Comm.h"
#include "..\Debug\Debug.h"
//#include "../Includes/POSDCC.h"

extern BOOL fInstApp; 

extern char gblszAmt[20+1]; //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE 1 of 8
extern USHORT GPRSCONNETSTATUS;

//smac
extern BOOL fSMACTRAN;
extern fAdviceTras;

#define ERRORLEN 30
#define ERRORLEN1 30
static char szErrorMessage[ERRORLEN+1];
static char szErrorMessage1[ERRORLEN1+1];
extern BYTE byPackTypeBeforeDCCLog;
//extern BOOL fOptOutFlag;
extern BOOL fDimFlag;

void setLCDReverse(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(TRUE);  //the reverse enable // 
    
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (16 - lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 16 - lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(FALSE); //the reverse enable //     
}

/* BDO-00122: Change line busy to line busy, please try again - start -- jzg */
#if 1
void setLCDPrint(int line,int position, char *pbBuf)
{
	short shXPos = 0,
		shLen = 0;
    int lens = 0;
    int iInitX = 0;
	shLen = strlen(pbBuf);
	
	switch(position)
	{
		case DISPLAY_POSITION_LEFT:
			CTOS_LCDTPrintXY(1, line, pbBuf);
			break;

		case DISPLAY_POSITION_CENTER:
			lens = strlen(pbBuf);

			if((strTCT.byTerminalType == 1) || (strTCT.byTerminalType == 2))
				iInitX = ((22 - lens) / 2) + 1;
			else
				iInitX = ((30 - lens) / 2) + 1;

			CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			break;

		case DISPLAY_POSITION_RIGHT:
			lens = strlen(pbBuf);
			iInitX = 16 - lens + 1;
			CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			break;
	}
}
/* BDO-00122: Change line busy to line busy, please try again - end -- jzg */
#else
void setLCDPrint(int line,int position, char *pbBuf)
{
    short shXPos=0, shLen=0;
    int iInitX=0, lens=0;

    shLen = strlen(pbBuf);
    
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;

        case DISPLAY_POSITION_CENTER:
            #if 0
            lens = strlen(pbBuf);
            
            if((strTCT.byTerminalType == 1) || (strTCT.byTerminalType == 2))
               iInitX = ((22 - lens) / 2) + 1;
            else
                iInitX = ((30 - lens) / 2) + 1;
            
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            #endif
            lens = strlen(pbBuf);
            if(lens >= 20)
                iInitX=1;
            else    
                iInitX = (MAX_CHAR_PER_LINE - lens*2) / 2 ;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
        break;


        case DISPLAY_POSITION_RIGHT:
            shXPos = MAX_CHAR_PER_LINE - (shLen * 2);
            if(shXPos == 0)
                shXPos = 1;
            CTOS_LCDTPrintXY(shXPos, line, pbBuf);
            break;
    }
}


#endif

void showAmount(IN  BYTE bY, BYTE bStrLen, BYTE *baBuf)
{
    int i;
    
    if(bStrLen > 2)
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            if ((16 - bStrLen + 1 + i) > 14)
                CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
            else
                CTOS_LCDTPutchXY(16 - bStrLen + i, bY, baBuf[i]);
        }
    }
    else
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
        }
    }
}

void vduiDisplayInvalidTLE(void)
{
    
    vduiClearBelow(2);
    vduiWarningSound();
    vduiDisplayStringCenter(3,"INVALID SESSION");
    vduiDisplayStringCenter(4,"KEY, PLEASE");
    vduiDisplayStringCenter(5,"DWD SESSION KEY");
    vduiDisplayStringCenter(6,"--INSTRUCTION---");
    CTOS_LCDTPrintXY(1,7,"PRESS [F2] THEN");
    CTOS_LCDTPrintXY(1,8,"PRESS [3]");
    
    CTOS_Delay(2500);
}


void szGetTransTitle(BYTE byTransType, BYTE *szTitle)
{    
    int i;
    szTitle[0] = 0x00;
    
    //vdDebug_LogPrintf("**szGetTransTitle START byTransType[%d]Orig[%d]**", byTransType, srTransRec.byOrgTransType);
    switch(byTransType)
    {
        case SALE:
	if(srTransRec.fInstallment == TRUE)
		strcpy(szTitle, "INSTALLMENT");
	else if(srTransRec.fCash2Go== TRUE)
		strcpy(szTitle, "CASH2GO");
	//else if(srTransRec.fDebit == TRUE && strcmp(srTransRec.szCardLable, "BANCNET") == 0)//#00023
	else if(srTransRec.fDebit == TRUE)
		strcpy(szTitle, "DEBIT SALE");		//Please print DEBIT SALE on receipt instead of SALE for debit transaction - #00173	
	else	
		strcpy(szTitle, "STRAIGHT SALE");
            break;
        case PRE_AUTH:
            strcpy(szTitle, "PREAUTHORIZATION");
            break;
        case PRE_COMP:
            strcpy(szTitle, "SALE COMPLETION");
            break;
        case CASH_ADV:
            strcpy(szTitle, "CASH ADVANCE");
            break;			
        case REFUND:
            strcpy(szTitle, "REFUND");
            break;
        case VOID:
            if(srTransRec.byOrgTransType == REFUND)
                strcpy(szTitle, "VOID REFUND");
			else if(srTransRec.fInstallment == TRUE) 
				strcpy(szTitle, "VOID INSTALLMENT");
			else if(srTransRec.fCash2Go== TRUE) 
				strcpy(szTitle, "VOID CASH2GO");			
			else if(srTransRec.byOrgTransType == LOY_REDEEM_5050 || srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE)
				strcpy(szTitle, "VOID RTR SALE");
			else if(srTransRec.byOrgTransType == PRE_COMP)
				strcpy(szTitle, "VOID SALE COMP");
			else if(srTransRec.byOrgTransType == CASH_ADV)
				strcpy(szTitle, "VOID CASH ADV");			
            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
                strcpy(szTitle, "VOID OFFLINE");		//#00042 - Please indicate Void transactions			
            else if(srTransRec.byOrgTransType == PREAUTH_COMP)
                strcpy(szTitle, "VOID PRE COMP");		
            else
                strcpy(szTitle, "VOID");
            break;
        case SALE_TIP:
			if(srTransRec.byOrgTransType == SALE)
                strcpy(szTitle, "SALE TIP ADJUST");
			else if(srTransRec.byOrgTransType == SALE_OFFLINE)
                strcpy(szTitle, "OFFLINE TIP ADJUST");
			else
				strcpy(szTitle, "TIP ADJUST");
            break;
        case SALE_OFFLINE:
            strcpy(szTitle, "OFFLINE SALE");
            break;
        case SALE_ADJUST: 
            strcpy(szTitle, "ADJUST");
            break;
        case SETTLE:
            strcpy(szTitle, "SETTLEMENT");
            break;
        case SIGN_ON:
            strcpy(szTitle, "SIGN ON");
            break;
        case BATCH_REVIEW:
            strcpy(szTitle, "BATCH REVIEW");
            break;
        case BATCH_TOTAL:
            strcpy(szTitle, "BATCH TOTAL");
            break;
        case REPRINT_ANY:
            strcpy(szTitle, "REPRINT RECEIPT");
            break;
        case BAL_INQ:
           strcpy(szTitle, "BALANCE INQUIRY");
           break;
        case LOG_ON:
           strcpy(szTitle, "LOG ON");
           break;	
        case LOY_BAL_INQ:
            strcpy(szTitle, "RTR INQUIRY");		
            break;
        case LOY_REDEEM_5050:
            strcpy(szTitle, "50/50 REDEEM");	   
            break;
        case LOY_REDEEM_VARIABLE:
            strcpy(szTitle, "RTR SALE");	  
            break;
        case TERM_REG:
            strcpy(szTitle, "REGISTRATION");	  
            break;
        case CARD_VERIFY:
            strcpy(szTitle, "CARD VERIFY");	  
        break;   	
		
        case PREAUTH_VER:
        case PREAUTH_COMP:
            strcpy(szTitle, "PREAUTH COMPLETION"); 			
        break;

        case PREAUTH_VOID:
            strcpy(szTitle, "PREAUTH VOID"); 			
        break;

        case SETUP:
            strcpy(szTitle, "SETUP");	
        break; 			

		case HOST_INFO_REPORT:
			strcpy(szTitle, "HOST INFO REPORT");	
		break;

		case CRC_REPORT:
			strcpy(szTitle, "CRC REPORT");	
		break;

		case IP_REPORT:
			strcpy(szTitle, "IP REPORT");	
		break;

		case CITAS_SERVICES:
			strcpy(szTitle, "CITAS SERVICES");	
		break;

		case SET_CARD:
			strcpy(szTitle, "SET CARD");	
		break;

		case MERCHANT_SERVICES:
			strcpy(szTitle, "MERCHANT SERVICES");	
		break;

		case DCC_RATE_REQUEST:
			strcpy(szTitle, "RATE REQUEST");	
		break;

		case DCC_MERCH_RATE_REQ:
			strcpy(szTitle, "MERCH RATE REQ");	
		break;

		//case CASH_OUT:
			//strcpy(szTitle, "CASH OUT");	
		//break;

		case CTMS_UPDATE:
			strcpy(szTitle, "CTMS UPDATE");	
		break;

		case CASH_IN:
			strcpy(szTitle, "CASH IN");
		break;

		case CASH_OUT:
			strcpy(szTitle, "CASH OUT");
		break;

		case CARDLESS_CASH_OUT:
			strcpy(szTitle, "CARDLESS CASH OUT");
		break;
		
		case CARDLESS_BAL_INQ:
			strcpy(szTitle, "CARDLESS BALANCE INQ");
		break;

		case BILLS_PAYMENT:
			strcpy(szTitle, "BILLS PAYMENT");
		break;

		case FUND_TRANSFER:
			strcpy(szTitle, "FUND TRANSFER");
		break;
		
		default:
            strcpy(szTitle, "");
        break;
    }
    i = strlen(szTitle);
    szTitle[i]=0x00;
    return ;
}

void vdDispTransTitle(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
	vdDebug_LogPrintf("****vdDispTransTitle****");
//	vdDebug_LogPrintf("fOptOut[%d]",srTransRec.fOptOut);
    memset(szTitle, 0x00, sizeof(szTitle));
//	if(srTransRec.fOptOut)
//		strcpy(szTitle,"OPT OUT");
//	else
	    szGetTransTitle(byTransType, szTitle);
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2 ;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    //memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));  
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);

		
}


//aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE start 2 of 8
void vdDispTransTitleCardTypeandTotal(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    int iInitX = 1;
		int inCardLabellen, inCardDispStart, inMaxDisplen;
		char szDisplayCardLable [MAX_CHAR_PER_LINE+1];
		char szVoidCurrSymbol [10+1];
#if 0
//issue-00229: do not display amount on TC upload

    //if (inGetATPBinRouteFlag() == TRUE)		
		//return d_OK;

    if(srTransRec.byPackType == TC_UPLOAD || fAdviceTras == TRUE)
        return d_OK;

		/* BDOCLG-00318: Fix for garbage display problem - start -- jzg */
		//inTCTRead(1);
		//if(((strTCT.fFleetGetLiters == TRUE) || (strTCT.fGetDescriptorCode == TRUE)) && (srTransRec.fFleetCard == TRUE))
		//	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
		//else
			CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		/* BDOCLG-00318: Fix for garbage display problem - end -- jzg */
   
    memset(szTitle, 0x00, sizeof(szTitle));
	if(srTransRec.fOptOut)
		strcpy(szTitle,"OPT OUT");
	else if(byTransType == VOID)
		strcpy(szTitle,"VOID");
	else		
		szGetTransTitle(byTransType, szTitle);

	iInitX = 40;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
	memset(szDisplayCardLable, 0x00, sizeof(szDisplayCardLable));

	if(strcmp(srTransRec.szCardLable,"CITI MASTER")==0)
		memcpy(&szDisplayCardLable[0],"MASTERCARD",10);
	else if(strcmp(srTransRec.szCardLable,"CITI VISA")==0)
		memcpy(&szDisplayCardLable[0],"VISA",4);
	else
		memcpy(&szDisplayCardLable[0],srTransRec.szCardLable,strlen(srTransRec.szCardLable));
		
    inCardLabellen = strlen(szDisplayCardLable);
		
   
   if ((srTransRec.byTransType == SALE) ||(srTransRec.byTransType == PRE_AUTH) || (srTransRec.byTransType == CASH_ADVANCE) || (srTransRec.byTransType == VOID) || (srTransRec.byTransType == BIN_VER))
   {
          
		BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
		//BYTE szBaseAmt[20 + 1] = {0};

		/* BDOCLG-00318: Fix for garbage display problem - start -- jzg */
		//if(((strTCT.fFleetGetLiters == TRUE) || (strTCT.fGetDescriptorCode == TRUE)) && (srTransRec.fFleetCard == TRUE))
		//{
		//	CTOS_LCDTSetReverse(TRUE);
		//	CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
		//	CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
		//	CTOS_LCDTSetReverse(FALSE);
		//}else
			/* BDOCLG-00318: Fix for garbage display problem - end -- jzg */
		{
	        inCardDispStart = iInitX  - inCardLabellen * 2;
				
			CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
			//memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
			CTOS_LCDTSetReverse(TRUE);
			//CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
			//CTOS_LCDTPrintAligned(1, szTitleDisplay, d_LCD_ALIGNLEFT);
			CTOS_LCDTPrintXY(1,1,szTitleDisplay);
			if ((inGetATPBinRouteFlag() != TRUE) && (srTransRec.byTransType != BIN_VER)	)
				   CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
			
			CTOS_LCDTSetReverse(FALSE);
		}
    if (srTransRec.byTransType != BIN_VER)
    {
       CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
       wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 

	   if(srTransRec.fDCC && strTCT.fFormatDCCAmount == TRUE)
		   vdDCCModifyAmount(szBaseAmt,&szAmtBuff); //vdDCCModifyAmount(&szAmtBuff);
	   else	   
	       vdCTOS_FormatAmount(strCST.szAmountFormat, szBaseAmt,szAmtBuff); // patrick fix case #229
       
       setLCDPrint(3, DISPLAY_POSITION_LEFT, "TOTAL:");
       
       
       if (srTransRec.byTransType == VOID)   
       {
          memset(szVoidCurrSymbol,0,sizeof(szVoidCurrSymbol));
          strcpy(szVoidCurrSymbol,strCST.szCurSymbol); 
          strcat(szVoidCurrSymbol,"-");
          sprintf(szCurAmtBuff,"%s%s",szVoidCurrSymbol, szAmtBuff);
          setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
       }
       else
       {
          sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol, szAmtBuff);
          //issue-00371
          //inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);	 
          setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
       }
    }
	 }
	 #if 0
   else if(srTransRec.byTransType == BIN_VER)
   {
      inCardDispStart = iInitX  - inCardLabellen * 2;
      CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
      //memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
      CTOS_LCDTSetReverse(TRUE);
      //CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
      //CTOS_LCDTPrintAligned(1, szTitleDisplay, d_LCD_ALIGNLEFT);
	  
	  CTOS_LCDTPrintXY(1,1,szTitleDisplay);
	  if (inGetATPBinRouteFlag() == TRUE)	
      	CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
      CTOS_LCDTSetReverse(FALSE);
   }
	 #endif
	 else
	 	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE); 
#endif	 
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE end 2 of 8



void vdDispTitleString(BYTE *szTitle)
{
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
       
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle)); //aaronnino for BDOCLG ver 9.0 fix on issue #00072 Incorrrect transaction type displayed for INSTALLMENT 1 of 2 
    //memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
		CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);
}




USHORT clearLine(int line)
{
    CTOS_LCDTGotoXY(1,line);
    CTOS_LCDTClear2EOL();
}

void vdDisplayTxnFinishUI(void)
{
    
    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "Transaction");
    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "Approved");
    //setLCDPrint(5, DISPLAY_POSITION_CENTER, srTransRec.szAuthCode);     
   if(srTransRec.byTransType == LOG_ON)
      return;
   
//   if(strlen(srTransRec.szBinRouteRespCode))
//   {
//       if(memcmp(srTransRec.szBinRouteRespCode, "00", 2) != 0)
//       {
//           memset(srTransRec.szBinRouteRespCode, 0, sizeof(srTransRec.szBinRouteRespCode));
//           return;
//       }
//   }
	
//    if(strTCT.fDisplayAPPROVED == TRUE && srTransRec.byTransType != BALANCE_INQUIRY && srTransRec.byTransType != SMAC_ACTIVATION
//		&& srTransRec.byTransType != BIN_VER) // Terminal will display the SMAC balance instead of the "APPROVED" message. 
	{
		CTOS_LCDTClearDisplay();
		
		if ((strTCT.byTerminalType % 2) == 1) 
	        vduiDisplayStringCenter(5,"APPROVED");
	    else 
	        vduiDisplayStringCenter(8, "APPROVED");
		CTOS_Beep();
	}
#if 0
	else
	{
		if(srTransRec.byTransType != BIN_VER && srTransRec.byTransType != BALANCE_INQUIRY && srTransRec.byTransType != SMAC_BALANCE)
		{
			if ((strTCT.byTerminalType % 2) == 1) 
				CTOS_LCDTPrintXY(1, 8, "PRINTING...");
			else 
				CTOS_LCDTPrintXY(1, 16, "PRINTING...");
		}
	}
#endif	
    
}

void vdDispErrMsg(IN BYTE *szMsg)
{
    char szDisplayMsg[40];
    BYTE byKeyBuf;
    
    CTOS_LCDTClearDisplay();
    if(srTransRec.byTransType != 0)
        vdDispTransTitle(srTransRec.byTransType);

    memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
    strcpy(szDisplayMsg, szMsg);
    vduiClearBelow(8);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szDisplayMsg);
    CTOS_TimeOutSet (TIMER_ID_2 , 2*100);
    CTOS_Sound(1000, 50);
    
    while (1)
    {        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL ||byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return ;
        }
    }
}


int vdDispTransactionInfo(void)
{
    BYTE byKeyBuf;
    BYTE szTmp1[16+1];
    BYTE szTmp2[16+1];
	BYTE szTmp[130+1];
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "Card NO.");
    setLCDPrint(3, DISPLAY_POSITION_LEFT, srTransRec.szPAN);
    memset(szTmp1, 0x00, sizeof(szTmp1));
    memset(szTmp2, 0x00, sizeof(szTmp2));
	memset(szTmp, 0x00, sizeof(szTmp));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTmp1, 3);
    sprintf(szTmp2,"Inv No:%s", szTmp1);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szTmp2);
    
    wub_hex_2_str(srTransRec.szTotalAmount, szTmp1, 6);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Amount:");
	//format amount 10+2
	vdCTOS_FormatAmount(strCST.szAmountFormat, szTmp1, szTmp);
	sprintf(szTmp2,"%s%s", strCST.szCurSymbol,szTmp);
    //sprintf(szTmp2,"SGD%7lu.%02lu", (atol(szTmp1)/100), (atol(szTmp1)%100));
    setLCDPrint(6, DISPLAY_POSITION_RIGHT, szTmp2);  
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRS ENTR TO CONF");
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
            return  READ_CARD_TIMEOUT;
        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return d_OK;
        }
    }
}

USHORT showBatchRecord(TRANS_DATA_TABLE *strTransData)
{
    char szStr[DISPLAY_LINE_SIZE + 1];
    char szTemp[DISPLAY_LINE_SIZE + 1];
    BYTE byKeyBuf;
    CTOS_LCDTClearDisplay();
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szPAN);
    setLCDPrint(1, DISPLAY_POSITION_LEFT, "Card NO:");
    setLCDPrint(2, DISPLAY_POSITION_LEFT, szStr);
    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szBaseAmount, szTemp, AMT_BCD_SIZE);
    sprintf(szStr, "%lu.%lu", atol(szTemp)/100, atol(szTemp)%100);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Amount:");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szStr);

    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szAuthCode);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Auth Code:");
    setLCDPrint(6, DISPLAY_POSITION_LEFT,  szStr);


    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
    sprintf(szStr, "%s", szTemp);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "Invoice NO:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szTemp);
     
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);   
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
        {
            CTOS_LCDTClearDisplay();
            return  READ_CARD_TIMEOUT;
        }
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return d_OK;
        }
    }
}

void vduiLightOn(void)
{
    if (strTCT.fHandsetPresent)  
        CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,80000);
    else
        CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
}

void vduiKeyboardBackLight(BOOL fKeyBoardLight)
{
    if (strTCT.fHandsetPresent) 
    {
        if(VS_TRUE == fKeyBoardLight)
        {
            
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffff);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,0xffffff);
        }
        else
        {
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_OFF,3000);
        }

    }
    else
    {
        if(VS_TRUE == fKeyBoardLight)
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffff);
        else
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
    }
}

void vduiPowerOff(void)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;
        
    //vduiClearBelow(1);
    CTOS_LCDTClearDisplay(); /*BDO: Clear window -- sidumili*/
	
    vduiDisplayStringCenter(4,"POWER OFF TERMINAL?");
	//gcitra-0728
    
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

void vduiDisplayStringCenter(unsigned char  y,unsigned char *sBuf)
{
	//1027
	//setLCDPrint27(y, DISPLAY_POSITION_CENTER,sBuf);	
	setLCDPrint(y,DISPLAY_POSITION_CENTER,sBuf);

}

void vduiClearBelow(int line)
{
	int i = 0,
		inNumOfLine = 8;

	/* BDOCLG-00005: should clear the rest of the line even for V3 terminals - start -- jzg */
	//inTCTRead(1);
	if((strTCT.byTerminalType % 2) == 0)
		inNumOfLine = 16;

	for(i=line; i<=inNumOfLine; i++)
		clearLine(i);
	/* BDOCLG-00005: should clear the rest of the line even for V3 terminals - end -- jzg */
}

void vduiWarningSound(void)
{
    CTOS_LEDSet(d_LED1, d_ON);
    CTOS_LEDSet(d_LED2, d_ON);
    CTOS_LEDSet(d_LED3, d_ON);
    
    CTOS_Beep();
    CTOS_Delay(300);
    CTOS_Beep();
    
    CTOS_LEDSet(d_LED1, d_OFF);
    CTOS_LEDSet(d_LED2, d_OFF);
    CTOS_LEDSet(d_LED3, d_OFF);
}


void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg)
{
	int inRowtmp;
	
    if ((strTCT.byTerminalType % 2) == 0)
		inRowtmp = V3_ERROR_LINE_ROW;
	else
        inRowtmp = inRow;

    clearLine(inRowtmp);
		
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg);
    CTOS_Beep();
    CTOS_Delay(1500);

	clearLine(inRowtmp);
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated start 3 of 5
void vdDisplayErrorMsgResp (int inColumn, int inColumn2, int inColumn3, int inRow, int inRow2, int inRow3,  char *msg, char *msg2, char *msg3)
{
    
    CTOS_LCDTPrintXY(inColumn, inRow, "                                        ");
		CTOS_LCDTPrintXY(inColumn2, inRow2, "                                        ");
		CTOS_LCDTPrintXY(inColumn3, inRow3, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRow, msg);
		CTOS_LCDTPrintXY(inColumn2, inRow2, msg2);
		CTOS_LCDTPrintXY(inColumn3, inRow3, msg3);
    CTOS_Beep();
    CTOS_Delay(1500);
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated end 3 of 5

void vdDisplayErrorMsgResp2 (char *msg, char *msg2, char *msg3)
{
   CTOS_LCDTClearDisplay();

   if ((strTCT.byTerminalType % 2) == 1) 
   {
		vduiDisplayStringCenter(3,msg);
		vduiDisplayStringCenter(4,msg2);
		vduiDisplayStringCenter(5,msg3);
			
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
   }
	 
   CTOS_Beep();
   CTOS_Delay(1500);
   CTOS_LCDTClearDisplay();
}

void vdDisplayErrorMsgResp2Ex(char *msg, char *msg2, char *msg3)
{
   //CTOS_LCDTClearDisplay();
   vdClearBelowLine(2);
   
   if ((strTCT.byTerminalType % 2) == 1) 
   {
		vduiDisplayStringCenter(3,msg);
		vduiDisplayStringCenter(4,msg2);
		vduiDisplayStringCenter(5,msg3);
			
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
   }
	 
   CTOS_Beep();
   CTOS_Delay(1500);
   vdClearBelowLine(2);
}

/* functions for loyalty - Meena 15/01/2012 - start*/
short vduiAskConfirmContinue(int inDisplay)
{
    unsigned char key;
  
    //vduiClearBelow(1);
    CTOS_LCDTClearDisplay();/*BDO: Clear window -- sidumili*/
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"YOU WANT TO");
	if (inDisplay == 1)		
    	vduiDisplayStringCenter(5,"CLEAR BATCH?");
	else if (inDisplay == 2)	
    	vduiDisplayStringCenter(5,"DELETE REVERSAL?");
	else		
		vduiDisplayStringCenter(5,"CONTINUE?");
	//gcitra-0728
    //CTOS_LCDTPrintXY(1,7,"NO[X]   YES[OK] ");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
	//gcitra-0728
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL)
            return -1;
        else
            vduiWarningSound();
    }
    
}



BYTE struiGetchWithTimeOut(void)
{
    unsigned char c;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,3000);
    
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //
            vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&c);
            return c;   
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return d_KBD_CANCEL;
        }
    }
}

/* functions for loyalty - Meena 15/01/2012 - End*/

short inuiAskSettlement(void)
{
    unsigned char key;
    while(1) 
    {
        vduiClearBelow(2);
        vduiDisplayStringCenter(2,"DAILY SETTLEMENT");
        vduiDisplayStringCenter(3,"NOTIFICATION");

        vduiDisplayStringCenter(5,"PERFORM");
        vduiDisplayStringCenter(6,"SETTLEMENT?");
        vduiDisplayStringCenter(8,"NO[X] YES[OK]");

        CTOS_KBDGet(&key);
        if(key==d_KBD_ENTER)
            return d_OK;
        else if(key==d_KBD_CANCEL)
            return d_KBD_CANCEL;
        else if(key==d_KBD_F1)
            vduiPowerOff();
    }
        
}

void vduiDisplaySignalStrengthBatteryCapacity(void)
{
    
    BYTE bCapacity, msg2[50];
    USHORT dwRet;
    short insign;
    
    
    if(GPRSCONNETSTATUS== d_OK && strTCT.inMainLine == GPRS_MODE)
    {
        insign=incommSignal();
        if(insign==-1)
        {
            CTOS_LCDTPrintXY (9,1, "SIGNAL:NA");
        }
        else
        {           
            if(insign/6 == 0)
                CTOS_LCDTPrintXY (9,1, "NO SIGNAL");
            else if(insign/6 == 1)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:l____"); 
            }
            else if(insign/6 == 2)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:ll___"); 
            }
            else if(insign/6 == 3)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lll__"); 
            }
            else if(insign/6 == 4)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:llll_"); 
            }
            else if(insign/6 == 5)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lllll"); 
            }
            
        }
    }
    
    dwRet= CTOS_BatteryGetCapacityByIC(&bCapacity);  
    if(dwRet==d_OK)
    {
        sprintf(msg2, "B:%d%% ", bCapacity);
        CTOS_LCDTPrintXY (3,1, msg2);
    }
                
}

void vdSetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szMessage);
    memset(szErrorMessage,0x00,sizeof(szErrorMessage));
	memset(szErrorMessage1,0x00,sizeof(szErrorMessage1));
    
    if (inErrLen > 0)
    {
        if (inErrLen > ERRORLEN)
            inErrLen = ERRORLEN;
        
        memcpy(szErrorMessage,szMessage,inErrLen);
    }
}

void vdSetErrorMessages(char *szMessage, char *szMessage1)
{
    int inErrLen=0, inErrLen1=0;

    inErrLen = strlen(szMessage);
	inErrLen1 = strlen(szMessage1);

    memset(szErrorMessage,0x00,sizeof(szErrorMessage));
	memset(szErrorMessage1,0x00,sizeof(szErrorMessage1));
    
    if (inErrLen1 > 0)
    {
            inErrLen = ERRORLEN;
		    inErrLen1 = ERRORLEN1;
        
        memcpy(szErrorMessage,szMessage,inErrLen);
		memcpy(szErrorMessage1,szMessage1,inErrLen1);
    }
}


int inGetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szErrorMessage);

    if (inErrLen > 0)
    {       
        memcpy(szMessage,szErrorMessage,inErrLen);
    }
    
    return inErrLen;
}

int inGetErrorMessages(char *szMessage, char *szMessage1)
{
    int inErrLen=0, inErrLen1=0;

    inErrLen = strlen(szErrorMessage);
	inErrLen1 = strlen (szErrorMessage1);
      
        memcpy(szMessage,szErrorMessage,inErrLen);
		memcpy(szMessage1,szErrorMessage1,inErrLen1);

    return inErrLen1;
}

//gcitra
void setLCDPrint27(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);

    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (20- lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 20- lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }

		
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
}

//gcitra


//sidumili: display message
void vdDisplayMessage(char *szLine1Msg, char *szLine2Msg, char *szLine3Msg)
{
    CTOS_LCDTClearDisplay();
		vduiClearBelow(2);
		vduiDisplayStringCenter(4, szLine1Msg);
		vduiDisplayStringCenter(5, szLine2Msg);
		vduiDisplayStringCenter(6, szLine3Msg);
		CTOS_Beep(); /*BDO: Added BEEP -- sidumili*/
		WaitKey(1);
}
//sidumili

//sidumili: confirmation
short vduiAskEnterToConfirm(void)
{
    unsigned char key;
  
    
    CTOS_LCDTPrintXY(1,8,"CONFIRM?NO[X]YES[OK]");
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL){
					
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
						if (strCPT.inCommunicationMode == DIAL_UP_MODE){
										inCTOS_inDisconnect();
						}
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
					
            return -1;
        	}
        else
            vduiWarningSound();
    }
    
}
//sidumili:

int inDisplayDCCRateScreen(void)
{
	VS_BOOL fDisplayForExRate = inFLGGet("fForExRate");
	VS_BOOL fDisplayMarkup = inFLGGet("fDCCMarkUp");
	char szTemp[MAX_CHAR_PER_LINE+1];
	char szTemp1[MAX_CHAR_PER_LINE+1];
	char szTemp2[MAX_CHAR_PER_LINE+1];
	BYTE szAmtBuff[20+1], szCurAmtBuff[20+1];
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	int iLine,inLength=0;
	BYTE key=0;
	float inMarkup = 0;
#if 0	
	//#define RATE_RESPONSE_FULL "\x01\x70\x37\x31\x30\x30\x39\x63\x66\x34\x63\x32\x64\x64\x34\x38\x66\x34\x63\x32\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x31\x31\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x30\x30\x33\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x34\x37\x32\x38\x30\x30\x31\x38\x34\x38\x39\x34\x33\x32\x30\x32\x30\x31\x32\x30\x39\x32\x32\x31\x32\x32\x38\x31\x38\x33\x35\x36\x30\x33\x36\x56\x53\x41\x64\x64\x37\x35\x34\x30\x38\x34\x39\x35\x35\x36\x30\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x32\x20\x20\x20\x20\x20\x20\x20\x20\x30\x30\x30\x30\x30\x31\x41\x55\x44\x34\x38\x33\x2E\x30\x32\x20\x20\x20\x20\x20\x30\x33\x36"
	
	//inUnPackIsoFunc61(&srTransRec,RATE_RESPONSE_FULL);
	
	CTOS_LCDTClearDisplay();
	
	iLine = ((strTCT.byTerminalType%2)?3:4);

	wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff); 
	sprintf(szCurAmtBuff,"(1)%s %s", srTransRec.szDCCLocalSymbol, szAmtBuff);
	setLCDPrint(2, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	memset(szAmtBuff,0x00,sizeof(szAmtBuff));
	memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));
	memset(szBaseAmt,0x00,sizeof(szBaseAmt));

	inCSTReadHostID(srTransRec.szDCCCur);

    if(strTCT.fFormatDCCAmount == TRUE)
    	vdDCCModifyAmount(srTransRec.szDCCCurAmt,szAmtBuff);
	else
		vdCTOS_FormatAmount(strCST.szAmountFormat, srTransRec.szDCCCurAmt,szAmtBuff);
	
	
	sprintf(szCurAmtBuff,"(2)%s %s",srTransRec.szDCCCurSymbol, szAmtBuff);// Wait for strCST for foreign currency
	setLCDPrint((strTCT.byTerminalType%2)?3:4, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	if(fDisplayForExRate)
	{
		inLength=strlen(srTransRec.szDCCFXRate)-srTransRec.inDCCFXRateMU;
		memset(szTemp,0x00,sizeof(szTemp));
		memcpy(szTemp,srTransRec.szDCCFXRate,inLength);
		memcpy(&szTemp[inLength],".",1);
		memcpy(&szTemp[inLength+1],&srTransRec.szDCCFXRate[inLength],srTransRec.inDCCFXRateMU);
			
		setLCDPrint((strTCT.byTerminalType%2)?5:7, DISPLAY_POSITION_LEFT, "Exchange Rate:");
		setLCDPrint((strTCT.byTerminalType%2)?6:8, DISPLAY_POSITION_RIGHT,szTemp);	
	}
	
	if(fDisplayMarkup)
	{
		memset(szTemp,0x00,sizeof(szTemp));
		memset(szTemp1,0x00,sizeof(szTemp1));

		
		inMarkup = atof(srTransRec.szDCCMarkupPer);
		sprintf(szTemp,"%.2f",inMarkup);
		sprintf(szTemp1,"%20.20s",szTemp);
		//sprintf(szTemp1,"%s",szTemp);
		sprintf(szTemp2,"Markup:%s",szTemp1);
		strcat(szTemp2,"%");
		
		setLCDPrint((strTCT.byTerminalType%2)?7:10,DISPLAY_POSITION_LEFT,szTemp2);
	}
	
	srTransRec.fDCC = VS_FALSE;
	
	if(strTCT.inDCCMode == PRINT_DCC_RATES)
	{
		while(1)
		{
			key=WaitKey(inGetIdleTimeOut(TRUE));
			
	       	if(key == d_KBD_1)
	       	{
				return d_OK;
	       	}
	       	else if(key == d_KBD_2)
	       	{	
				srTransRec.fDCC = VS_TRUE;			
			
				memset(srTransRec.szDCCLocalAmount,0x00,sizeof(srTransRec.szDCCLocalAmount));
				memcpy(srTransRec.szDCCLocalAmount, srTransRec.szTotalAmount,sizeof(srTransRec.szTotalAmount));

				return d_OK;
			}
		}
	}
	else
	{
		while(1)
		{
			key=WaitKey(inGetIdleTimeOut(TRUE));
			
	       	if(key == d_KBD_1)
	       	{
				return d_OK;
	       	}
	       	else if(key == d_KBD_2)
	       	{	
				srTransRec.fDCC = VS_TRUE;			
			
				memset(srTransRec.szDCCLocalAmount,0x00,sizeof(srTransRec.szDCCLocalAmount));
				memcpy(srTransRec.szDCCLocalAmount, srTransRec.szTotalAmount,sizeof(srTransRec.szTotalAmount));

				return d_OK;
			}
	        else if(key == d_KBD_CANCEL)
	        {
				return FAIL;
	        } 
			else if(key == 0xFF)
			{
				return FAIL;
			}

		}
	}
#endif	
	
}


int inDisplayRateHostError(void)
{
	int iLine=0;
	BYTE key=0;

	CTOS_LCDTClearDisplay();
	setLCDPrint27((strTCT.byTerminalType%2)?2:3,DISPLAY_POSITION_CENTER,"RATE HOST ERROR");
	setLCDPrint((strTCT.byTerminalType%2)?4:5,DISPLAY_POSITION_LEFT,"PROCEED TO?");
	setLCDPrint((strTCT.byTerminalType%2)?5:6,DISPLAY_POSITION_LEFT,"BDO HOST");
	setLCDPrint((strTCT.byTerminalType%2)?6:7,DISPLAY_POSITION_RIGHT,"(1) YES");
	setLCDPrint((strTCT.byTerminalType%2)?8:9,DISPLAY_POSITION_RIGHT,"(2) NO");

	while(1)
	{
		key=WaitKey(inGetIdleTimeOut(TRUE));

       	if(key == d_KBD_1)
       	{
			return d_OK;
       	}
       	else if(key == d_KBD_2)
      	{	
			return FAIL;
			
		}
        else if(key == d_KBD_CANCEL)
        {
			return FAIL;
        } 
		else if(key == 0xFF)
		{
			return FAIL;
		}

	}
	
}

void vdDisplayErrorMsgResp3 (char *msg, char *msg2, char *msg3, char *msg4)
{
   CTOS_LCDTClearDisplay();

   if ((strTCT.byTerminalType % 2) == 1) 
   {
		vduiDisplayStringCenter(3,msg);
		vduiDisplayStringCenter(4,msg2);
		vduiDisplayStringCenter(5,msg3);
		vduiDisplayStringCenter(6,msg4);
			
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
	  vduiDisplayStringCenter(9,msg4);
	  
   }
	 
   CTOS_Beep();
   CTOS_Delay(1500);
   CTOS_LCDTClearDisplay();
}

void vdDispTransTitleAndCardType(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    int iInitX = 1;
		int inCardLabellen, inCardDispStart, inMaxDisplen;
		char szDisplayCardLable [MAX_CHAR_PER_LINE+1];
		char szVoidCurrSymbol [10+1];
#if 0	
//issue-00229: do not display amount on TC upload
    //SIT
    if (inGetATPBinRouteFlag() == TRUE)		
		return d_OK;

	if(srTransRec.byPackType == TC_UPLOAD || fAdviceTras == TRUE)
		return d_OK;

	vdDebug_LogPrintf("****vdDispTransTitleAndCardType****");
		/* BDOCLG-00318: Fix for garbage display problem - start -- jzg */
		//inTCTRead(1);
		//if(((strTCT.fFleetGetLiters == TRUE) || (strTCT.fGetDescriptorCode == TRUE)) && (srTransRec.fFleetCard == TRUE))
		//	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
		//else
			//CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		/* BDOCLG-00318: Fix for garbage display problem - end -- jzg */
   
    memset(szTitle, 0x00, sizeof(szTitle));

	if(srTransRec.fOptOut)
		strcpy(szTitle,"OPT OUT");
	else if(byTransType == VOID)
		strcpy(szTitle,"VOID");
	else		
		szGetTransTitle(byTransType, szTitle);

	iInitX = 40;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
	memset(szDisplayCardLable, 0x00, sizeof(szDisplayCardLable));

	if(strcmp(srTransRec.szCardLable,"CITI MASTER")==0)
		memcpy(&szDisplayCardLable[0],"MASTERCARD",10);
	else if(strcmp(srTransRec.szCardLable,"CITI VISA")==0)
		memcpy(&szDisplayCardLable[0],"VISA",4);
	else
		memcpy(&szDisplayCardLable[0],srTransRec.szCardLable,strlen(srTransRec.szCardLable));
		
    inCardLabellen = strlen(szDisplayCardLable);
		
   
   if ((srTransRec.byTransType == SALE) ||(srTransRec.byTransType == PRE_AUTH) || (srTransRec.byTransType == CASH_ADVANCE) || (srTransRec.byTransType == VOID) || (srTransRec.byTransType == BIN_VER))
   {
          
		BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
		{
	        inCardDispStart = iInitX  - inCardLabellen * 2;
				
			CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
			CTOS_LCDTSetReverse(TRUE);
			CTOS_LCDTPrintXY(1,1,szTitleDisplay);
			if ((inGetATPBinRouteFlag() != TRUE) && (srTransRec.byTransType != BIN_VER)	)
				   CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
			
			CTOS_LCDTSetReverse(FALSE);
		}
#if 0
    if (srTransRec.byTransType != BIN_VER)
    {
       CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
       wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
       vdCTOS_FormatAmount(strCST.szAmountFormat, szBaseAmt,szAmtBuff); // patrick fix case #229
       
       setLCDPrint(3, DISPLAY_POSITION_LEFT, "TOTAL:");
       
       
       if (srTransRec.byTransType == VOID)   
       {
          memset(szVoidCurrSymbol,0,sizeof(szVoidCurrSymbol));
          strcpy(szVoidCurrSymbol,strCST.szCurSymbol); 
          strcat(szVoidCurrSymbol,"-");
          sprintf(szCurAmtBuff,"%s%s",szVoidCurrSymbol, szAmtBuff);
          setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
       }
       else
       {
          sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol, szAmtBuff);
          //issue-00371
          //inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);	 
          setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
       }

    }
#endif
	 }
#if 0
   else if(srTransRec.byTransType == BIN_VER)
   {
      inCardDispStart = iInitX  - inCardLabellen * 2;
      CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
      //memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
      CTOS_LCDTSetReverse(TRUE);
      //CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
      //CTOS_LCDTPrintAligned(1, szTitleDisplay, d_LCD_ALIGNLEFT);
	  
	  CTOS_LCDTPrintXY(1,1,szTitleDisplay);
	  if (inGetATPBinRouteFlag() == TRUE)	
      	CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
      CTOS_LCDTSetReverse(FALSE);
   }
#endif
	 else
	 	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE); 
#endif	 
}

int inDisplayDCCRateScreenEx(void)
{
	VS_BOOL fDisplayForExRate = inFLGGet("fForExRate");
	VS_BOOL fDisplayMarkup = inFLGGet("fDCCMarkUp");
	char szTemp[MAX_CHAR_PER_LINE+1];
	char szTemp1[MAX_CHAR_PER_LINE+1];
	char szTemp2[MAX_CHAR_PER_LINE+1];
	BYTE szAmtBuff[20+1], szCurAmtBuff[20+1];
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	int iLine,inLength=0;
	BYTE key=0;
	float inMarkup = 0;
#if 0
	//#define RATE_RESPONSE_FULL "\x01\x70\x37\x31\x30\x30\x39\x63\x66\x34\x63\x32\x64\x64\x34\x38\x66\x34\x63\x32\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x31\x31\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x30\x30\x33\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x34\x37\x32\x38\x30\x30\x31\x38\x34\x38\x39\x34\x33\x32\x30\x32\x30\x31\x32\x30\x39\x32\x32\x31\x32\x32\x38\x31\x38\x33\x35\x36\x30\x33\x36\x56\x53\x41\x64\x64\x37\x35\x34\x30\x38\x34\x39\x35\x35\x36\x30\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x32\x20\x20\x20\x20\x20\x20\x20\x20\x30\x30\x30\x30\x30\x31\x41\x55\x44\x34\x38\x33\x2E\x30\x32\x20\x20\x20\x20\x20\x30\x33\x36"
	
	//inUnPackIsoFunc61(&srTransRec,RATE_RESPONSE_FULL);
	
	CTOS_LCDTClearDisplay();
	
	iLine = ((strTCT.byTerminalType%2)?3:4);
	
	wub_hex_2_str(srTransRec.szDCCLocalAmount, szBaseAmt, 6); 
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff); 
	sprintf(szCurAmtBuff,"(1)%s %s", srTransRec.szDCCLocalSymbol, szAmtBuff);
	setLCDPrint(2, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	memset(szAmtBuff,0x00,sizeof(szAmtBuff));
	memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));
	memset(szBaseAmt,0x00,sizeof(szBaseAmt));

	inCSTReadHostID(srTransRec.szDCCCur);


    if(strTCT.fFormatDCCAmount == TRUE)
    	vdDCCModifyAmount(srTransRec.szDCCCurAmt,szAmtBuff);
	else
		vdCTOS_FormatAmount(strCST.szAmountFormat, srTransRec.szDCCCurAmt,szAmtBuff);
	
	
	sprintf(szCurAmtBuff,"(2)%s %s",srTransRec.szDCCCurSymbol, szAmtBuff);// Wait for strCST for foreign currency
	setLCDPrint((strTCT.byTerminalType%2)?3:4, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	if(fDisplayForExRate)
	{
		inLength=strlen(srTransRec.szDCCFXRate)-srTransRec.inDCCFXRateMU;
		memset(szTemp,0x00,sizeof(szTemp));
		memcpy(szTemp,srTransRec.szDCCFXRate,inLength);
		memcpy(&szTemp[inLength],".",1);
		memcpy(&szTemp[inLength+1],&srTransRec.szDCCFXRate[inLength],srTransRec.inDCCFXRateMU);
			
		setLCDPrint((strTCT.byTerminalType%2)?5:7, DISPLAY_POSITION_LEFT, "Exchange Rate:");
		setLCDPrint((strTCT.byTerminalType%2)?6:8, DISPLAY_POSITION_RIGHT,szTemp);	
	}

	if(fDisplayMarkup)
	{
		memset(szTemp,0x00,sizeof(szTemp));
		memset(szTemp1,0x00,sizeof(szTemp1));
		
		inMarkup = atof(srTransRec.szDCCMarkupPer);
		sprintf(szTemp,"%.2f",inMarkup);
		sprintf(szTemp1,"%20.20s",szTemp);
		//sprintf(szTemp1,"%s",szTemp);
		sprintf(szTemp2,"Markup:%s",szTemp1);
		strcat(szTemp2,"%");
		
		setLCDPrint((strTCT.byTerminalType%2)?7:10,DISPLAY_POSITION_LEFT,szTemp2);
	}

	while(1)
	{
		key=WaitKey(inGetIdleTimeOut(TRUE));

       	if(key == d_KBD_1)
       	{
			//srTransRec.fDCC = VS_FALSE;
			return FAIL;
       	}
       	else if(key == d_KBD_2)
       	{	
			return VS_CONTINUE;
		}

	}

#endif	
	
}

void vdDisplayMultiLineMsgAlign(char *msg, char *msg2, char *msg3, int inPosition)
{
   int inAlign = 0;
   
   CTOS_LCDTClearDisplay();
   vdDispTransTitle(srTransRec.byTransType);

   switch (inPosition)
   {
   	 case DISPLAY_POSITION_LEFT:
	 	inAlign = d_LCD_ALIGNLEFT;
	 	break;
	 case DISPLAY_POSITION_RIGHT:
	 	inAlign = d_LCD_ALIGNRIGHT;
	 	break;
	 case DISPLAY_POSITION_CENTER:
	 	inAlign = d_LCD_ALIGNCENTER;
	 	break;
	  default:
	  	inAlign = d_LCD_ALIGNCENTER;
	  	break;
   }
   
	if ((strTCT.byTerminalType % 2) == 0)
   {	
   		if (strlen(msg) > 0)
			CTOS_LCDTPrintAligned(6, msg, inAlign);

		if (strlen(msg2) > 0)
			CTOS_LCDTPrintAligned(7, msg2, inAlign);

		if (strlen(msg3) > 0)
			CTOS_LCDTPrintAligned(8, msg3, inAlign);
   }
   else 
   {
   		if (strlen(msg) > 0)
			setLCDPrint27(4, inAlign, msg);

		if (strlen(msg2) > 0)
			setLCDPrint27(5, inAlign, msg2);

		if (strlen(msg3) > 0)
			setLCDPrint27(6, inAlign, msg3);
   }
	 
   CTOS_Beep();
   CTOS_Delay(1000);
   vduiClearBelow(2);
   
}

void vduiScreenBackLight(BOOL fTerminalActive)
{
	int iBrightness = get_env_int("BRIGHTNESS");
	int iIdleBrightness = get_env_int("IDLEBRIGHTNESS");

	if(get_env_int("DIMONIDLE") <= 0)
		return;
	
	vdDebug_LogPrintf("--vduiScreenBackLight--");
	vdDebug_LogPrintf("fTerminalActive=[%d]", fTerminalActive);

	if(VS_TRUE == fTerminalActive)
	{	
		vdSetIdleEvent(1);
		fDimFlag = FALSE;
		CTOS_BackLightSetBrightness(d_BKLIT_LCD, (iBrightness > 0 ? iBrightness : 50));
	}
	else
	{
		fDimFlag = TRUE;
		CTOS_BackLightSetBrightness(d_BKLIT_LCD, (iIdleBrightness > 0 ? iIdleBrightness : 1));						
	}

	return;
}


//From S1 code
/*Display message with image -- sidumili*/
void vdDisplayMessageBox(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, int msgType)
{
        CTOS_LCDTPrintAligned(8,"                          ", d_LCD_ALIGNLEFT);	  
    CTOS_LCDTPrintAligned(8,msg, DISPLAY_POSITION_LEFT);   
}

void vdDisplayMessageStatusBox(int inColumn, int inRow,  char *msg, char *msg2, int msgType)
{
}

int inCheckBatteryStatus(void)
{
}

int inCheckBattery(void)
{
    return 0;
}
#if 0
int inCheckTMSBackUpFilesExists()
{
    
}

int inCTOSS_TMSReadDetailData()
{
    
}
#endif

int inCheckFileExist(const char *szFileName)
{
    
}

BYTE MenuTransactions(BYTE *szTrxnMenu, BYTE *szTransFunc)
{
}

int vdDisplayTrxn(int inSaleType, char *sztrxlogo)
{
}

BYTE InputStringUI(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *szInput)
{
    int Bret = 0;
    int shMaxLen = 20;    
    Bret = InputString(1, 4, 0x01, 0x02, pbaStr, &shMaxLen, usMinLen, d_INPUT_TIMEOUT);
    vdDebug_LogPrintf("InputStringUI pbaStr[%s], Bret[%d]", pbaStr, Bret);
    if (Bret == d_KBD_CANCEL) {

        return (d_KBD_CANCEL);
    }
    if (Bret == 255) //timeout
      return 255;

    return Bret;
}
void vdDisplayErrorMsg2(int inColumn, int inRow,  char *msg, char *msg2, int msgType)
{
	int inRowtmp;
	
    if ((strTCT.byTerminalType % 2) == 0)
		inRowtmp = V3_ERROR_LINE_ROW;
	else
        inRowtmp = inRow;
			
    CTOS_LCDTPrintXY(inColumn, inRowtmp-1, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp-1, msg);
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg2);	
    CTOS_Beep();
    CTOS_Delay(1500);
}

void vdDisplayErrorMsg3(int inColumn, int inRow,  char *msg, char *msg2, char *msg3)
{
	int inRowtmp;
	
    if ((strTCT.byTerminalType % 2) == 0)
		inRowtmp = V3_ERROR_LINE_ROW;
	else
        inRowtmp = inRow;

    CTOS_LCDTPrintXY(inColumn, inRowtmp-2, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp-2, msg);
    CTOS_LCDTPrintXY(inColumn, inRowtmp-1, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp-1, msg2);
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg3);	
    CTOS_Beep();
    CTOS_Delay(1500);
}

void vdClearNonTitleLines(void)
{
	if(isCheckTerminalMP200() == d_OK)
	    setLCDPrint(2, DISPLAY_POSITION_LEFT, "                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    ");
	else	
	    setLCDPrint(2, DISPLAY_POSITION_LEFT, "                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    \n                    ");
}

void vdClearBelowLine(int inLine)
{
    int i=0, inIndex=0;
    int inNoLine=8;
	char szTemp[360+1];

    memset(szTemp, 0, sizeof(szTemp));
	
	if ((strTCT.byTerminalType % 2) == 0)
        inNoLine=16;


	inNoLine-=inLine;
	inNoLine--;

	memset(szTemp, 0, sizeof(szTemp));
    for(i=0; i<=inNoLine; i++)
    {
        memcpy(&szTemp[inIndex], "                    \n", 21);
	    inIndex+=21;
    }

    memcpy(&szTemp[inIndex], "                     ", 21);
	
    //vdDebug_LogPrintf("vdClearBelowLine");
	//vdDebug_LogPrintf("szTemp:[%s]", szTemp);
	//vdDebug_LogPrintf("inNoLine:[%d]", inNoLine);
	//vdDebug_LogPrintf("inLine:[%d]", inLine);

	setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szTemp);
	
}

USHORT usCTOSS_LCDDisplay(BYTE *szStringMsg)
{
}

#if 0
USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{
    
}
#endif

USHORT usCARDENTRY(BYTE *szStringMsg)
{
    
}

USHORT usCTOSS_BackToProgress(BYTE *szDispString)
{

}
void inDisplayLeftRight(int inLine, unsigned char *strLeft, unsigned char *strRight, int inMode)
{
    
}

USHORT usCTOSS_EditInfoListViewUI(BYTE *szDispString, BYTE *szOutputBuf)
{
    
}

short vduiAskConfirmation(char *szHeaderString)
{
    
}

USHORT usCTOSS_Confirm(BYTE *szDispString)
{
    BYTE key;
    int result = 0;
    CTOS_KBDBufFlush(); //cleare key buffer
    CTOS_LCDTPrintXY(1, 4, "PLS ENTER TO PROCEED");
    CTOS_TimeOutSet (TIMER_ID_2 , 45*100);
    while (1) {

        CTOS_KBDHit(&key);
        vdDebug_LogPrintf("key=%d", key);
        if (key == d_KBD_ENTER) {
			vduiClearBelow(7);
            result = d_OK;
            break;
        } else if ((key == d_KBD_CANCEL)) {
            result = d_NO;
            vdSetErrorMessage("USER CANCEL");
            break;
        }
        
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
            return  d_NO;
    }
    
    CTOS_KBDBufFlush ();
    return result;
}

USHORT usCTOSS_Confirm2(BYTE *szDispString)
{
    
}

USHORT usCTOSS_Confirm3(BYTE *szDispString)
{
    
}

void DisplayStatusLine(char *szDisplay) 
{
    
}

USHORT PrintReceiptUI(BYTE *szStringMsg)
{
    
}
BYTE InputQWERTY(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usMaxLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *display)
{

}
    
BYTE inDCCFxRate(BYTE *szDCCFxRateDetails)
{
}

USHORT usCTOSS_BatchReviewUI(BYTE *szDispString)
{
    
}

int inCallJAVA_Ping(BYTE *pbOutStr)
{
    
}

void vdGetCRC_ALL()
{
    
}

int inFLGRead(int id)
{
    
}

int inDetailReportHeader()
{
    
}

int usCTOSS_DisplayUI(char *szDisMsg)
{

}

int InputCVVUI(BYTE *baBuf, USHORT *usLens, USHORT usMinLen, int TO, char *szDisplay)
{
    
}

int is_utf8(const char * string)
{

    vdDebug_LogPrintf("AAA -is_utf8 start ");
    if(!string)
        return 0;
    vdDebug_LogPrintf("AAA -is_utf8 1 ");
    const unsigned char * bytes = (const unsigned char *)string;
    while(*bytes)
    {
        if( (// ASCII
             // use bytes[0] <= 0x7F to allow ASCII control characters
                bytes[0] == 0x09 ||
                bytes[0] == 0x0A ||
                bytes[0] == 0x0D ||
                (0x20 <= bytes[0] && bytes[0] <= 0x7E)
            )
        ) {
            bytes += 1;
            continue;
        }

        if( (// non-overlong 2-byte
                (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF)
            )
        ) {
            bytes += 2;
            continue;
        }

        if( (// excluding overlongs
                bytes[0] == 0xE0 &&
                (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// excluding surrogates
                bytes[0] == 0xED &&
                (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            )
        ) {
            bytes += 3;
            continue;
        }

        if( (// planes 1-3
                bytes[0] == 0xF0 &&
                (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// planes 4-15
                (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// plane 16
                bytes[0] == 0xF4 &&
                (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            )
        ) {
            bytes += 4;
            continue;
        }
         vdDebug_LogPrintf("AAA -is_utf8 start return 0");
        return 0;
    }
     vdDebug_LogPrintf("AAA -is_utf8 return 1 ");
    return 1;
}

int inMMTReadPreAuthHostName(char *szHostName, int inCPTID, int MITid, int inIndicator)
{

}

int inMMTReadHostName(char *szHostName, int inCPTID, int MITid, int inIndicator)
{
    
}

void vdDeleteISOLog()
{
}

void vdPrintISOLog(void)
{
    
}

void vdDeleteISOECRLog()
{
    
}
void vdPrintISOECRLog()
{
    
}
#if 0
int inCTOSS_TMSDownloadRequest()
{
}
#endif

int inFunctionKeyPasswordEx(char *p, int inPasswordLevel)
{
    
}

int inPrintADCReport(BYTE flag)
{
    
}
#if 0
int inCTOSS_TMSReadReprintDetailData(void)
{
    
}

int inCheckTMSReprintBackUpFilesExists(void)
{
    
}
#endif
int inDCCMMTUpdate()
{
    
}

#if 0
int inCTOSS_TMSRestoreTxnData()
{
    
}
#endif

int inCTOS_SelectCurrencyfromIDLE()
{
    
}

USHORT usGetSerialNumber(BYTE baFactorySN)
{
    
}

int inDisplayMessageBoxWithButton(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, int msgType)
{
    
}

void vdCheckERMMode(void)
{
    
}

int inMOBILE_GetConnectConfig(void)
{
    
}

USHORT getAppPackageInfo(BYTE *szAppName, BYTE *szStringMsg)
{
    
}
#if 0
int inCTOSS_TMSChkBatchEmpty(void)
{
    
}
#endif

USHORT EliteReceiptUI(BYTE *szStringMsg)
{
    
}

void vdPrintReportDisplayBMP(void)
{
    
}

int inCTOS_TEMPCheckAndSelectMutipleMID(void)
{
    return 0;
}

#if 0
int inCTOSS_CheckIntervialDateFrom2013(int y,int m,int d)
{
    int x[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    long i,s=0;

	vdDebug_LogPrintf("AAA>> Year[%d]Month[%d]Date[%d]", y,m,d);
    
    for(i=2013;i<y;i++)
    {
        if(i%4==0&&i%100!=0||i%400==0)
            s+=366;
        else 
            s+=365;
    }
            
    if(y%4==0&&y%100!=0||y%400==0)
        x[2]=29;
    
    for(i=1;i<m;i++)
        s+=x[i];
        
    s+=d;

    vdDebug_LogPrintf("Date[%ld]", s);
    return s;
}

int inCTOSS_TMSCheckIfDefaultApplication(void)
{
    #define	d_AP_FLAG_DEF_SEL_EX	0xC0
    CTOS_stCAPInfo stinfo;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;

    getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	strcpy(exe_subdir, &exe_dir[9]);
	inExeAPIndex = atoi(exe_subdir);
	memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
    if(CTOS_APGet(inExeAPIndex, &stinfo) != d_OK)
	{
		CTOS_APGet(inExeAPIndex, &stinfo);
	}

    vdDebug_LogPrintf("stinfo.bFlag[%02X]", stinfo.bFlag);
    if (stinfo.bFlag != d_AP_FLAG_DEF_SEL_EX)
        return d_NO;
    else
        return d_OK;
    
}
#endif
int vdCTOS_EditBiller(void)
{
    
}

int vdCTOS_EditUser(void)
{
    int inNumRecs = 0, ret;
    BYTE strOut[30],strtemp[64] = {0};
    USHORT usLen;
    int line = 2;
    int i;
    inNumRecs=inUSRNumRecord();
    vdDebug_LogPrintf("inNumRecs: %d", inNumRecs);
    inDatabase_TerminalOpenDatabase();
    memset(strMultiUSR, 0, sizeof(strMultiUSR));
    inUSRReadEx(1);
    inDatabase_TerminalCloseDatabase();
    while(1)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("EDIT USER");
	for (i=0; i < inNumRecs; i++)
	{
		if (strMultiUSR[i].szUserName[0] != 0)
		{
                    sprintf(strtemp, "%d %s", i+1, strMultiUSR[i].szUserName);
                    setLCDPrint(line++, DISPLAY_POSITION_LEFT, strtemp);
		}
	}       
        
        //select User
        ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
        
        if(ret==d_KBD_ENTER)
    	{
    		if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32)
    		{
                    	if(strOut[0] == 0x32)
    			{
    				;
    			}
    			if(strOut[0] == 0x31)
    			{
    				;
    			}
    			if(strOut[0] == 0x30)
    			{
    				;
    			}
    			break;
    		}
    		else
    		{
                    vduiClearBelow(2);
                    vduiWarningSound();
                    vduiDisplayStringCenter(7,"PLEASE SELECT");
                    vduiDisplayStringCenter(8,"A VALID VALUE");
                    CTOS_Delay(2000);		
    		}
    	}
    	if(ret == d_KBD_CANCEL || ret == 0xFF)
    		break;
        //Edit user
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        usLen = 32;
        ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {

            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }
    return 0;
}

BYTE MenuTransType(BYTE *szTrxnMenu)
{
    
}

int inCTOS_CTMSUPDATE(void)
{
    
}

int inCTOSS_TMSDownloadRequestAfterSettle(void)
{
    
}

int inBVTRead(int inSeekCnt)
{
    
}
int inBVTSave(int inSeekCnt)
{
    
}
    