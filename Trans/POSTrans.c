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
#include <CTOSInput.h>
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
#include "../Database/gpt.h"
#include "../FeliCa/Util.h"
#include "../Database/DatabaseFunc.h"
#include "../Debug/debug.h"
#include "../Functionslist/POSFunctionsList.h"
#include "../Ctls/POSWave.h"
#include "../Includes/POSDebit.h"
//#include "../UIapi.h"
#include "../Includes/POSSetting.h"

// For Auto Settlement -- sidumili
BOOL fAutoSettle = FALSE;

extern VS_BOOL inGlobalCount;
//extern int inSettlementHost;
int inSettlementHost = 0;


int inFallbackToMSR = FAIL;
static char szBaseAmount[6+1];

static BOOL fECRTxnFlg = 0;
//issue:218 - mfl
extern BOOL fEntryCardfromIDLE;
//issue218

BYTE szPAN[PAN_SIZE+1];
BYTE szExpireDate[EXPIRY_DATE_BCD_SIZE+1];
char szGlobalAPName[25];

BOOL ErmTrans_Approved;
extern BOOL fPrinterStart;
extern BOOL fGPRSConnectOK;
extern int inSelectedIdleAppsHost;
extern BOOL fBancNetTrans;


BOOL fTimeOutFlag = FALSE;
//BOOL gblDCCTrans = FALSE;
//BOOL gblDCCReceipt = FALSE;
//extern int ing_KeyPressed;
int ing_KeyPressed = 0;
extern int gblinGPRSFallback;

extern int gblinNoKeys;


unsigned char gblszMarginRatePercentage[10+1], gblszExchangeRate[12+1], gblszDCCCurrencySymbol[3+1],  gblszCurrencyCode[3+1], gblszForeignAmount[15+1], gblszLocalAmount[15+1], gblszCurrForeignAmount[15+1], gblszDCCCurrBaseAmount[15+1], gblszDCCTipAmount[12+1], gblszCurrDCCTipAmount[15+1];
int gblinMinorUnit;

int inProcessFallback = 0;

extern BYTE gblszPrintExchangeRate[12+1];


void vdSetECRTransactionFlg(BOOL flg)
{
	put_env_int("ECRTRANS",flg);
    //fECRTxnFlg = flg;
}

BOOL fGetECRTransactionFlg(void)
{
	fECRTxnFlg = get_env_int("ECRTRANS");
    return fECRTxnFlg;
}

extern int isdigit ( int c );


//android-remove
#if 0
USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{

    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[128];
    USHORT  usInputStrLen;

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
        if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
    }

    while(1)
    {
//        vduiLightOn(); // patrick remark for flash light always
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
                bInputStrData[usInputStrLen] = 0x00;

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
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

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
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

#endif

USHORT shCTOS_GetExpDate(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS)
{

    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[20];
    BYTE    bInputFormatStr[20];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));

    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    while(1)
    {
        vduiLightOn();
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
                bInputStrData[usInputStrLen] = 0x00;

                memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
                if(usInputStrLen >= 2)
                {
                    memcpy(bInputFormatStr, bInputStrData, 2);
                    strcat(bInputFormatStr, "/");
                    if(usInputStrLen > 2)
                        strcat(bInputFormatStr, &bInputStrData[2]);
                }
                else
                {
                    strcpy(bInputFormatStr, bInputStrData);
                }

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, (usMaxLen+1)*2);

                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
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

                memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
                if(usInputStrLen >= 2)
                {
                    memcpy(bInputFormatStr, bInputStrData, 2);
                    strcat(bInputFormatStr, "/");
                    if(usInputStrLen > 2)
                        strcat(bInputFormatStr, &bInputStrData[2]);
                }
                else
                {
                    strcpy(bInputFormatStr, bInputStrData);
                }

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, (usMaxLen+1)*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
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
            break;
        default :
            break;
        }
    }

    return 0;
}

USHORT getExpDate( OUT BYTE *baBuf)
{
    BYTE    szMonth[3];
    USHORT  usRet;
    USHORT  usLens;
    USHORT  usMinLen = 4;
    USHORT  usMaxLen = 4;
    USHORT usInputLine = 8;

	BYTE szTitle[25+1];
    BYTE szDisplay[512];

	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	vdDebug_LogPrintf("srTransRec.szPAN");
    vdDebug_LogPrintf(srTransRec.szPAN);
    vdDebug_LogPrintf("szTitle");
    vdDebug_LogPrintf(szTitle);

    memset(szDisplay, 0x00, sizeof(szDisplay));

	strcpy(szDisplay, "4");
	strcat(szDisplay, "|");
    strcat(szDisplay, "4");
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "CARD NUMBER: ");
    strcat(szDisplay, "|");
    strcat(szDisplay, srTransRec.szPAN);
    strcat(szDisplay, "|");
    strcat(szDisplay, "EXPIRY DATE(MM/YY):");

    while(1)
    {
        usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
//        usRet = InputExpiryDateUI(baBuf, &usLens, usMinLen, d_INPUT_TIMEOUT, szDisplay);
        if (usRet == d_KBD_CANCEL ){
    		vdDebug_LogPrintf("*************************");
  	   		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//clear buffer on idle swipe on term reg.

            return (d_EDM_USER_CANCEL);
        }
		else if (usRet == 0xFF) {
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(2000);
			return TIME_OUT;
		}

        /*   TINE:  Input validation handled by android UI
        memset(szMonth, 0x00, sizeof(szMonth));
        memcpy(szMonth, baBuf, 2);
        if(atol(szMonth) > 12 || 0 == atol(szMonth))
        {
            baBuf[0] = 0x00;
            clearLine(14); // Error message "INVALID FORMAT" must be cleared after entering wrong date format
            vdDisplayErrorMsg(1, 8, "INVALID FORMAT");
            clearLine(8);
            clearLine(14); //Error message "INVALID FORMAT" must be cleared after entering wrong date format
            continue;
        }
        else
        {
            return (d_OK);
        }
        */

        return (d_OK);
    }
}

USHORT getCardNO(OUT BYTE *baBuf) {
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 14;
    USHORT usMaxLen = 19;
    USHORT usInputLine = 8;

    BYTE szTitle[25+1];
    BYTE szDisplay[100];

    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

    strcpy(szDisplay, "8");
    strcat(szDisplay, "|");
    strcat(szDisplay, "21");
    strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "CARD NUMBER: ");

    while (1) {

        //usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, d_INPUT_TIMEOUT);

        usRet = InputStringUI(0x01, 0x02, baBuf, &usLens, usMinLen, d_INPUT_TIMEOUT, szDisplay);

        vdDebug_LogPrintf("usRet[%d]atoi(baBuf)=[%d]usLens[%d]", usRet, atoi(baBuf), usLens);

        if (usRet == d_KBD_CANCEL)
            return (d_EDM_USER_CANCEL);
		else if (usRet == 0xFF) {
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(2000);
			return TIME_OUT;
		}
        //if (usRet >= usMinLen && usRet <= usMaxLen)
        if (usRet == 'A') {
            //TINE:  save PAN to srTransRec
            vdDebug_LogPrintf("AAA-baBuf[%s]", baBuf);
          //  if (usLens >= usMinLen && usLens <= usMaxLen) {
                strcpy(srTransRec.szPAN, baBuf);
                return (d_OK);
          //  }
        }

        baBuf[0] = 0x00;
    }
}


unsigned char WaitKey(short Sec)
{
    unsigned char c;
    long mlsec;

    mlsec=Sec*100;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,mlsec);
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //

            vduiLightOn();
            CTOS_KBDGet(&c);
            return c;
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {
            return d_KBD_CANCEL;
        }
    }
}


void vduiApiAmount(unsigned char *ou, unsigned char *ascamt, unsigned char len)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int    ii, jj, tt;
    unsigned char   ch;
    unsigned char   aa;
    unsigned char   buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;
    ii = 0;


    ou[jj++] = strCST.szCurSymbol[0];
    ou[jj++] = strCST.szCurSymbol[1];
    ou[jj++] = strCST.szCurSymbol[2];

    for(ii = 0; ii < len; ii++)
    {
        ch = ascamt[ii];
        if((tt == 0) && (ch == 'C'))
        {
            tt = 1;
        }
        else if((tt == 0) && (ch == 'D'))
        {
            tt = 1;
            ou[jj++] = '-';
        }
        else if(ch<0x30 && ch>0x39 )
        {
            break;
        }
    }


    len = ii;
    aa = 0;
    for(ii = tt; (ii + 3) < len; ii++)
    {
        ch = ascamt[ii];
        if((ch == '0') && (aa == 0))
        {
            continue;
        }

        if(ch>0x29 && ch<0x40 )//if(isdigit(ch) /* && (ch !='0') */ )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for(ii = 0; ii < len; ii++)
    {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

void vduiApiPoint(unsigned char *ou, unsigned char *ascamt, unsigned char len)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int    ii, jj, tt;
    unsigned char   ch;
    unsigned char   aa;
    unsigned char   buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;

    ou[jj++] = 'P';
    ou[jj++] = 'T';
    ou[jj++] = 'S';
    ou[jj++] = ' ';
    for(ii = 0; ii < len; ii++)
    {
        ch = ascamt[ii];
        if((tt == 0) && (ch == 'C'))
        {
            tt = 1;
        }
        else if((tt == 0) && (ch == 'D'))
        {
            tt = 1;
            ou[jj++] = '-';
        }
        else if(ch<0x30 && ch>0x39 )
        {
            break;
        }
    }


    len = ii;
    aa = 0;
    for(ii = tt; (ii + 3) < len; ii++)
    {
        ch = ascamt[ii];
        if((ch == '0') && (aa == 0))
        {
            continue;
        }

        if(ch>0x29 && ch<0x40 )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for(ii = 0; ii < len; ii++)
    {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

//mode  1=amount , 2=string, 3=IP  4=password, 5=Point
unsigned char struiApiGetStringSub
(
    unsigned char   *strDisplay,
    short   x,
    short   y,
    unsigned char   *ou,
    unsigned char   mode,
    short   minlen,
    short   maxlen
)
{

    unsigned char srDestIP[MAX_CHAR_PER_LINE+1];
    unsigned char amtdis[MAX_CHAR_PER_LINE+1];
    unsigned char c;
    int n;
    int i;

    memset(srDestIP,0x00,sizeof(srDestIP));
    n= 0;

    vduiClearBelow(y);

    if(mode == MODE_AMOUNT)
    {
        vduiClearBelow(8);
        vduiApiAmount(amtdis, srDestIP, n);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

    }
    else if(mode == MODE_POINT)
    {
        vduiClearBelow(8);
        vduiApiPoint(amtdis, srDestIP, n);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

    }

    while(1)
    {

        CTOS_LCDTPrintXY(x,y,strDisplay);
        c=WaitKey(30);//CTOS_KBDGet(&c);
        vduiLightOn();

        if (c == d_KBD_ENTER)
        {
            if((n>=minlen) && (n<=maxlen))
            {
                ou[n]=0;
                memcpy(ou,srDestIP,n+1);
                if(mode==MODE_FOODITEM && atoi(ou)==0)
                {
                    return d_KBD_CANCEL;
                }

                if(mode==MODE_AMOUNT && atoi(ou)==0)
                {
                    i = atoi(ou);
                    memset(srDestIP,0x00,sizeof(srDestIP));
                    n=0;
                    vduiWarningSound();
                }
                else
                    return d_KBD_ENTER;
            }

        }
        else if((c == d_KBD_CANCEL) && (n==0))
        {
            memset(srDestIP,0x00,sizeof(srDestIP));
            return d_KBD_CANCEL;
        }

        else
        {
            if (c==d_KBD_CLEAR)
            {
                if(n>0)
                {
                    n--;
                    srDestIP[n]='\0';
                }
            }
            else if((c == d_KBD_CANCEL))
            {
                memset(srDestIP,0x00,sizeof(srDestIP));
                n=0;
            }
            else if((c == d_KBD_DOT || c == d_KBD_DOWN) && mode==MODE_IPADDRESS)
            {
                srDestIP[n]='.';
                n++;
            }
            else if (c == d_KBD_DOT || c == d_KBD_F3  || c == d_KBD_F4 || c == d_KBD_00)
            {
                ;
            }
            else if(c == d_KBD_UP || c == d_KBD_DOWN)
            {
                return c;
            }
            else if(n<maxlen)
            {
                srDestIP[n]=c;
                n++;
            }
            else
            {

            }

            if(mode == MODE_AMOUNT)
            {
                vduiClearBelow(8);
                vduiApiAmount(amtdis, srDestIP, n);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

            }
            else if(mode == MODE_POINT)
            {
                vduiClearBelow(8);
                vduiApiPoint(amtdis, srDestIP, n);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

            }
            else if(mode == MODE_PASSWORD)
            {
                for(i=0;i<n;i++)
                    amtdis[i]='*';
                amtdis[n]=0;
                vduiClearBelow(8);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,amtdis);
            }
            else
            {
                vduiClearBelow(8);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,srDestIP);
            }
        }

    }

}

void vdCTOSS_GetMemoryStatus(char *Funname)
{
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("[%s][%ld],[%ld][%ld][%ld]",Funname,ulUsedDiskSize,ulTotalRamSize,ulUsedRamSize,ulAvailableRamSize);

    CTOS_LCDTClearDisplay();

    CTOS_LCDTPrintXY(1, 1, szTotalDiskSize);
    CTOS_LCDTPrintXY(1, 2, szUsedDiskSize);
    CTOS_LCDTPrintXY(1, 3, szAvailableDiskSize);

    CTOS_LCDTPrintXY(1, 5, szTotalRamSize);
    CTOS_LCDTPrintXY(1, 6, szUsedRamSize);
    CTOS_LCDTPrintXY(1, 7, szAvailableRamSize);
    WaitKey(60);

}

int inCTOSS_CheckMemoryStatus()
{
#define SAFE_LIMIT_SIZE 5000

    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);

    if (ulAvailDiskSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 7, "Settle  soon");
        vdDisplayErrorMsg(1, 8,  "Insufficient Memory");
        return FAIL;
    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("Insufficient RAM");
        return FAIL;
    }

    return d_OK;

}

void vdCTOS_SyncHostDateTime()
{
    CTOS_RTC SetRTC;
    char szDate[4+1];
    char szTime[6+1];
    char szBuf[2+1];
	BYTE szDateTime[30+1];


	vdDebug_LogPrintf("saturn vdCTOS_SyncHostDateTime");

    if(srTransRec.byOffline == CN_TRUE)
        return;
   // CTOS_RTCGet(&SetRTC);
   
   //usGetRTCAndroid(&SetRTC);
   //vdDebug_LogPrintf("sys year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

  //vdDebug_LogPrintf("year[%02X],time[%02x:%02x:%02x]date[%02x][%02x]",SetRTC.bYear,srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);

    memset(szDate,0,sizeof(szDate));
    memset(szTime,0,sizeof(szTime));

	//srTransRec.szDate[0] = 0x07;
	//srTransRec.szDate[1] = 0x17;

	//srTransRec.szTime[0] = 0x05;
	//srTransRec.szTime[1] = 0x06;
	//srTransRec.szTime[2] = 0x01;
	
	

    wub_hex_2_str(srTransRec.szDate, szDate, 2);
    wub_hex_2_str(srTransRec.szTime, szTime, 3);
	vdDebug_LogPrintf("saturn date %s", szDate);
	
	vdDebug_LogPrintf("saturn time %s", szTime);
    if((strlen(szDate)<=0) ||(strlen(szTime)<=0))
        return;

	memset(szDateTime, 0x00, sizeof(szDateTime));
	
    //sprintf(szBuf,"%02x",(unsigned int)atol(szDate)%100);
    //wub_str_2_hex(szBuf, &(SetRTC.bDay), 2);
	strncpy(szDateTime,&szDate[0],2);
	strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szDate)/100);
    //wub_str_2_hex(szBuf, &(SetRTC.bMonth), 2);
	strncat(szDateTime,&szDate[2],2);
	strcat(szDateTime,"|");

	//if year is "x", app will get the year setting of the terminal
	strcat(szDateTime,"x|");

	//strcat(szDateTime,"20");
	//strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szTime)/10000);
    //wub_str_2_hex(szBuf, &(SetRTC.bHour), 2);
	strncat(szDateTime,&szTime[0],2);
	strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%10000/100);
    //wub_str_2_hex(szBuf, &(SetRTC.bMinute), 2);
	strncat(szDateTime,&szTime[2],2);
	strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%100);
    //wub_str_2_hex(szBuf, &(SetRTC.bSecond), 2);
	strncat(szDateTime,&szTime[4],2);

    CTOS_RTCSet(&SetRTC);
    vdDebug_LogPrintf("saturn szDatetime %s", szDateTime);
//    usSetDateTime(szDateTime);			
	
    //vdDebug_LogPrintf("set year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

    return;
}

int file_exist (char *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0);
}

int inCLearTablesStructure(void){

memset(&strCDT,0x00, sizeof(STRUCT_CDT));
memset(&strIIT,0x00, sizeof(STRUCT_IIT));
memset(&strEMVT,0x00, sizeof(STRUCT_EMVT));
memset(&strHDT,0x00, sizeof(STRUCT_HDT));
memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
memset(&srTransRec,0x00, sizeof(TRANS_DATA_TABLE));
    return d_OK;
}

void vdCTOS_TxnsBeginInit(void) {
    BYTE bEntryMode = 0;
    int inRet = d_NO;
    BYTE szPAN[PAN_SIZE + 1];
    USHORT usTrack1Len = 0;
    USHORT usTrack2Len = 0;
    USHORT usTrack3Len = 0;
    BYTE szTrack1Data[TRACK_I_BYTES + 1];
    BYTE szTrack2Data[TRACK_II_BYTES + 1];
    BYTE szTrack3Data[TRACK_III_BYTES + 1];

    BYTE byPanLen;
    BYTE szCardholderName[CARD_HOLD_NAME_DIGITS + 1];
    BYTE szExpireDate[EXPIRY_DATE_BCD_SIZE + 1];
    BYTE szDataTmp[5] = {0};
    BYTE szServiceCode[3+1];
    BYTE byPINEntryCapability;


    vdDebug_LogPrintf("--vdCTOS_TxnsBeginInit--");

	//vdCTOSS_ReleaseAllDrvHandle();

    inCTOSS_GetRAMMemorySize("TXN INIT");

	srTransRec.fInstallment = FALSE;

#if 0
    CTOS_PrinterPutString("vdCTOS_TxnsBeginInit");
{
char temp[30+1];
memset(temp,0x00,sizeof(temp));
sprintf(temp, "byEntryMode::%d", srTransRec.byEntryMode);

    CTOS_PrinterPutString(temp);
}
#endif
    /*albert - do not prompt swipe/insert/etc... card*/
    vdDebug_LogPrintf("--vdCTOS_TxnsBeginInit-- Retain MSR Card Details");
    if (srTransRec.byEntryMode == CARD_ENTRY_MSR || srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        bEntryMode = srTransRec.byEntryMode;
        vdDebug_LogPrintf("bEntryMode = [%d]", bEntryMode);
        if (srTransRec.byEntryMode == CARD_ENTRY_MSR) {

            usTrack1Len = srTransRec.usTrack1Len;
            usTrack2Len = srTransRec.usTrack2Len;
            usTrack3Len = srTransRec.usTrack3Len;

            memset(szTrack1Data, 0, sizeof(szTrack1Data));
            memset(szTrack2Data, 0, sizeof(szTrack2Data));
            memset(szTrack3Data, 0, sizeof(szTrack3Data));
            memset(szPAN, 0, sizeof(szPAN));

            memcpy(szTrack1Data, srTransRec.szTrack1Data, srTransRec.usTrack1Len);
            memcpy(szTrack2Data, srTransRec.szTrack2Data, srTransRec.usTrack2Len);
            memcpy(szTrack3Data, srTransRec.szTrack3Data, srTransRec.usTrack3Len);
            strcpy(szPAN, srTransRec.szPAN);

            vdDebug_LogPrintf("TrackData and PAN saved");

            //AnalysisTrack1
            memset(szCardholderName, 0, sizeof(szCardholderName));
            memset(szExpireDate, 0, sizeof(szExpireDate));

            byPanLen = srTransRec.byPanLen;
            vdDebug_LogPrintf("byPanLen Saved...");

            strcpy(szCardholderName, srTransRec.szCardholderName);
            vdDebug_LogPrintf("szCardholderName Saved...");
            vdDebug_LogPrintf("szCardholderName:  %s", szCardholderName);


            szDataTmp[0] = srTransRec.szExpireDate[0];
            szDataTmp[1] = srTransRec.szExpireDate[1];
            vdDebug_LogPrintf("szExpireDate saved");

            memcpy(szServiceCode, srTransRec.szServiceCode, 3);
            vdDebug_LogPrintf("szServiceCode saved");

            byPINEntryCapability = srTransRec.byPINEntryCapability;
            vdDebug_LogPrintf("byPINEntryCapability saved");

        }
    }

    if (d_OK == inCTOS_ValidFirstIdleKey())
        bEntryMode = srTransRec.byEntryMode;

    memset(&srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
#if 0
    {
            char temp[30+1];
            memset(temp,0x00,sizeof(temp));
            sprintf(temp, "byEntryMode:A:%d", bEntryMode);

                CTOS_PrinterPutString(temp);
    }
#endif
    //Clear Previous Transaction Data
    //inCLearTablesStructure();

    if (0 != bEntryMode) {
        srTransRec.byEntryMode = bEntryMode;
        if (srTransRec.byEntryMode == CARD_ENTRY_MSR) {

            srTransRec.usTrack1Len = usTrack1Len;
            srTransRec.usTrack2Len = usTrack2Len;
            srTransRec.usTrack3Len = usTrack3Len;
            memcpy(srTransRec.szTrack1Data, szTrack1Data, srTransRec.usTrack1Len);
            memcpy(srTransRec.szTrack2Data, szTrack2Data, srTransRec.usTrack2Len);
            memcpy(srTransRec.szTrack3Data, szTrack3Data, srTransRec.usTrack3Len);
            strcpy(srTransRec.szPAN, szPAN);
            srTransRec.byPanLen = byPanLen;
            strcpy(srTransRec.szCardholderName, szCardholderName);
            srTransRec.szExpireDate[0] = szDataTmp[0];
            srTransRec.szExpireDate[1] = szDataTmp[1];
            memcpy(srTransRec.szServiceCode, szServiceCode, 3);
            srTransRec.byPINEntryCapability = byPINEntryCapability;

        }
    }
#if 0
    {
            char temp[30+1];
            memset(temp,0x00,sizeof(temp));
            sprintf(temp, "byEntryMode:B:%d", bEntryMode);

                CTOS_PrinterPutString(temp);
    }
#endif
    vdSetErrorMessage("");
    vdCTOSS_SetWaveTransType(0);

    //vdSetPowerHighSpeed();

    vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check if main app");
//add for ECR only MianAP get the ECR command, if Sub AP will cannot error
    //if (inMultiAP_CheckMainAPStatus() == d_OK)
    if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
    {
        vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check ECR on?[%d]", strTCT.fECR);
        if (strTCT.fECR && (fGetECRTransactionFlg() == 1)) // tct
        {
            vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check Database exist");
            if (file_exist(DB_MULTIAP)) {
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data");
                inRet = inCTOS_MultiAPGetData();
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data ret[%d]", inRet);
                if (d_OK != inRet)
                    return;
            }
        }

    }
}

void vdCTOS_TransEndReset(void)
{
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    char szErrMsg[30+1];

	vdDebug_LogPrintf("saturn --vdCTOS_TransEndReset--");

#if 1
	// patrick to print contactless EMV data 20151019
    //inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
	{
		usCTOSS_EMV_MultiDataGet(PRINT_EMV_TAGS_LIST, &srTransRec.usChipDataLen, srTransRec.baChipData);
	    inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
	}
	else if(srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
#endif
    DebugAddHEX("PRINT_EMV_TAGS_LIST",srTransRec.baChipData,srTransRec.usChipDataLen);
	// patrick to print contactless EMV data 20151019

    vdDebug_LogPrintf("saturn vdCTOS_TransEndReset ECR?[%d]", fGetECRTransactionFlg());
    //CTOS_LCDTClearDisplay();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        //vdDisplayErrorMsg(1, 8, szErrMsg);
		vdDisplayMessageBox(1, 8, "", szErrMsg, "", MSG_TYPE_ERROR);
		CTOS_Beep();
		CTOS_Delay(1500);
        if(srTransRec.byTransType==SETTLE)
             vdDisplayErrorMsg(1, 8, "SETTLE FAILED");
    }

	vdDebug_LogPrintf("saturn -test 2");

    if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
        vdRemoveCard();
	//
    //else
    //    CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

	// Minimize the white screend display -- sidumili
	//if (srTransRec.byTransType == SETTLE)
		//vdCTOS_DispStatusMessage("PLEASE WAIT...");

    if(!fGetECRTransactionFlg())
    {
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    }
	
	vdDebug_LogPrintf("saturn test 3");

	// Minimize the white screend display -- sidumili
	if (isCheckTerminalMP200() == d_OK && strTCT.byERMMode != 0)
		vdCTOS_DispStatusMessage("PLEASE WAIT...");

    CTOS_KBDBufFlush();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdSetErrorMessage("");
	vdSetErrorMessages("","");
	vdCTOSS_SetWaveTransType(0);

	//ushCTOS_ClearePadSignature();
	//gcitra
	inCLearTablesStructure(); // fix to clear saved card info on the terminal. - mfl
	//1128
	inFallbackToMSR = FAIL;
	//gcitra

	ErmTrans_Approved = FALSE;

	fPrinterStart = FALSE;

	inSelectedIdleAppsHost = 0;

	fTimeOutFlag = FALSE;

	fAutoSettle = FALSE;

	fBancNetTrans = FALSE;

	inProcessFallback = 0;

	if(get_env_int("ECRPROCESS")==1)
		 put_env_int("ECRPROCESS",0);

	 //gblDCCTrans = FALSE;

	 //gblDCCReceipt = FALSE;
	 vdDebug_LogPrintf("saturn test 4");

	vdSetIdleEvent(1); //set stgIdleEvent to 1 to reset sleep timer.

//	vdSetBatteryTimeOut(0);

//	vdSetSignalTimeOut(0);

	ushCTOS_ClearePadSignature();

	inMultiAP_Database_BatchDelete();

	inCTOSS_GetRAMMemorySize("TRANS END");

vdDebug_LogPrintf("saturn -test 5");

#ifdef UNFORK_APP
    inRecoverRAM();
#endif

    //CTOS_KBDBufPut(0);
	CTOS_KBDBufFlush();
    ing_KeyPressed = 0x00;
    fEntryCardfromIDLE = FALSE;
	
	inSettlementHost = 0;

//	gblinGPRSFallback = 0;

	vdDebug_LogPrintf("saturn test 6");

    return;
}

void vdCTOS_SetTransEntryMode(BYTE bEntryMode)
{
    srTransRec.byEntryMode = bEntryMode;
}

void vdCTOS_SetTransType(BYTE bTxnType)
{
    srTransRec.byTransType = bTxnType;
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
}

void vdCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len)
{

    if (usTk1Len > 0)
    {
         memcpy(srTransRec.szTrack1Data, &baTk1Buf[1], (usTk1Len -3));
         srTransRec.usTrack1Len = (usTk1Len - 3);// REMOVE %, ? AND LRC
         //vdDebug_LogPrintf("srTransRec.usTrack1Len");
    }

    if (usTk2Len > 0)
    {
         memcpy(srTransRec.szTrack2Data, &baTk2Buf[1], (usTk2Len - 3));
         srTransRec.usTrack2Len = (usTk2Len - 3);// REMOVE %, ? AND LRC
         //vdDebug_LogPrintf("srTransRec.usTrack2Len");
    }

     if (usTk3Len > 0)
     {	
          memcpy(srTransRec.szTrack3Data, baTk3Buf, usTk3Len);
          srTransRec.usTrack3Len = usTk3Len;
          //vdDebug_LogPrintf("srTransRec.usTrack3Len");
     }
	 
    vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);

    //Fix for card without Track 1
    //vdGetCardHolderName(baTk1Buf, usTk1Len);
    if (usTk1Len > 0)
       vdAnalysisTrack1( &baTk1Buf[1], usTk1Len);
    if (usTk2Len > 0)
    //Fix for card without Track 2

    shAnalysisTrack2( &baTk2Buf[1], usTk2Len);

}

void vdCTOS_ResetMagstripCardData(void)
{
    memset(srTransRec.szTrack1Data, 0x00, sizeof(srTransRec.szTrack1Data));
    srTransRec.usTrack1Len=0;

    memset(srTransRec.szTrack2Data, 0x00, sizeof(srTransRec.szTrack2Data));
    srTransRec.usTrack2Len=0;

    memset(srTransRec.szTrack3Data, 0x00, sizeof(srTransRec.szTrack3Data));
    srTransRec.usTrack3Len=0;

    memset(srTransRec.szCardholderName, 0x00, sizeof(srTransRec.szCardholderName));

    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
    srTransRec.byPanLen = 0;
    memset(srTransRec.szExpireDate, 0x00, sizeof(srTransRec.szExpireDate));
    memset(srTransRec.szServiceCode, 0x00, sizeof(srTransRec.szServiceCode));

    srTransRec.byEntryMode = 0;
}

int inCTOS_CheckEMVFallbackTimeAllow(char* szStartTime, char* szEndTime, int inAllowTime)
{
    char szTempBuf[20];
    int inHH1, inHH2, inMM1, inMM2, inSS1, inSS2, inGap;

    if ((strlen(szStartTime) == 0) || (strlen(szStartTime) == 0))
        return (d_OK);

    if(0 == atoi(szStartTime))
        return (d_OK);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[0], 2);
    inHH1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[2], 2);
    inMM1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[4], 2);
    inSS1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[0], 2);
    inHH2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[2], 2);
    inMM2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[4], 2);
    inSS2 = atoi(szTempBuf);

    inGap = ((inHH2*3600)+(inMM2*60)+inSS2) - ((inHH1*3600)+(inMM1*60)+inSS1);

    if (inGap < 0 )
        return (d_OK);

    if(inGap > inAllowTime)
        return (d_NO);

    return d_OK;;

}


int inCTOS_CheckEMVFallback(void)
{
    BYTE szFallbackStartTime[20];
    BYTE szCurrentTime[20];
    int inRet;
    CTOS_RTC SetRTC;

	vdDebug_LogPrintf("inCTOS_CheckEMVFallback...");
    if (strCDT.fChkServiceCode)
    {
        if(((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
            && (CARD_ENTRY_ICC != srTransRec.byEntryMode))
        {
			vdDebug_LogPrintf("inFallbackToMSR = [%d]", inFallbackToMSR);
			if(inFallbackToMSR == SUCCESS)
            {
                CTOS_RTCGet(&SetRTC);
                sprintf(szCurrentTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
                inRet = inCTOS_CheckEMVFallbackTimeAllow(strTCT.szFallbackTime, szCurrentTime, strTCT.inFallbackTimeGap);

                inFallbackToMSR = FAIL;
                memset(strTCT.szFallbackTime,0x00,sizeof(strTCT.szFallbackTime));

                if(d_OK != inRet)
                    return FAIL;
                else
                    vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
            }
            else
                return FAIL;
        }
    }

    return d_OK;;
}

int inCTOS_CheckIssuerEnable(void)
{
    int inEnable = 0;

//    CTOS_PrinterPutString("inCTOS_CheckIssuerEnable");

    inEnable = strIIT.inCheckHost;
    vdDebug_LogPrintf("inCTOS_CheckIssuerEnable: [%d]", inEnable);

    if(0 == inEnable)
    {
       vdSetErrorMessage("TRANS NOT ALLWD");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}


int inCTOS_CheckTranAllowd(void)
{
    int inEnable = 0;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inEnable = strPIT.fTxnEnable;
    vdDebug_LogPrintf("SATURN inCTOS_CheckTranAllowd: [%d]", inEnable);

//   CTOS_PrinterPutString("inCTOS_CheckTranAllowd");

    if(0 == inEnable)
    {
       vdSetErrorMessage("TRANS NOT ALLWD");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

int inCTOS_CheckMustSettle(void)
{
    int inEnable = 0;
    BYTE szTitle[25+1];
    BYTE szDisMsg[100];

    inEnable = strMMT[0].fMustSettFlag;

    if(1 == inEnable)
    {
        memset(szDisMsg, 0x00, sizeof(szDisMsg));
        memset(szTitle, 0x00, sizeof(szTitle));
        szGetTransTitle(srTransRec.byTransType, szTitle);
        strcpy(szDisMsg, szTitle);
        strcat(szDisMsg, "|");
        strcat(szDisMsg, "MUST SETTLE");
        //usCTOSS_LCDDisplay(szDisMsg);
        vdDisplayMessageBox(1, 8, "", "MUST SETTLE", "", MSG_TYPE_WARNING);
        CTOS_Beep();
        CTOS_Delay(1500);
        return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

void vdCTOS_FormatPANEx(char *szFmt,char* szInPAN,char* szOutPAN, BOOL fMask)
{
    char szCurrentPAN[20+1], szCurrentPANFormat[24+1];
    int inFmtIdx = 0, inIndex=0;
    int inPANIdx = 0;
    int inFmtPANSize;
    int inPanSize;

	memset(szCurrentPANFormat, 0x00, sizeof(szCurrentPANFormat));

    inPanSize = strlen(srTransRec.szPAN);
	switch(inPanSize)
	{
		case 12:
            if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** ****");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN");
			break;
		case 13:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** *");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN N");
			break;
		case 14:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** **");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NN");
			break;
		case 15:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** ***");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNN");
			break;
        case 16:
            if(fMask == TRUE)
                strcpy(szCurrentPANFormat,"**** **** **** ****");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNNN");
            break;

		case 17:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** **** *");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNNN N");
			break;
		case 18:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** **** **");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNNN NN");
			break;
		case 19:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"*******************");
			else
				strcpy(szCurrentPANFormat,"NNNNNNNNNNNNNNNNNNN");
			break;
	}

    if(fMask == TRUE)
    {
        inFmtPANSize = strlen(szCurrentPANFormat);
        inFmtPANSize--;
        vdDebug_LogPrintf("1. inFmtPANSize:[%d]", inFmtPANSize);
        for(inFmtIdx=inFmtPANSize; strIIT.inMaskDigit > inIndex; inFmtIdx--)
        {
            if(szCurrentPANFormat[inFmtIdx] == '*')
            {
                szCurrentPANFormat[inFmtIdx]='N';
                inIndex++;
            }
        }
    }

    inFmtPANSize = strlen(szCurrentPANFormat);
    if (strlen(szCurrentPANFormat) == 0)
    {
      strncpy(szOutPAN,szInPAN,inFmtPANSize);
      return;
    }

    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    inFmtIdx = 0;

    while(szCurrentPANFormat[inFmtIdx]!= 0x00)
    {
      if(szCurrentPANFormat[inFmtIdx] == 'N' || szCurrentPANFormat[inFmtIdx] == 'n')
      {
          szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
          inFmtIdx++;
          inPANIdx++;
      }
      else if (szCurrentPANFormat[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szCurrentPANFormat[inFmtIdx] == '*')
      {

          memcpy(&szOutPAN[inFmtIdx],&szCurrentPANFormat[inFmtIdx],1);
          inFmtIdx++;
          inPANIdx++;
      }
      else if (!isdigit(szCurrentPANFormat[inFmtIdx]))
      {
          szOutPAN[inFmtIdx] = szCurrentPANFormat[inFmtIdx];
          inFmtIdx++;
      }
    }

    while(szCurrentPAN[inPANIdx]!= 0x00)
    {
     szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
     inFmtIdx++;
     inPANIdx++;
    }

    return;
}

void vdCTOS_FormatAmount(char *szFmt,char* szInAmt,char* szOutAmt)
{
    char szCurrentAmt[20];
    char szBuf[20];
    char szFinalFmt[20];
    int inFmtIdx = 0;
    int inTotaActNum = 0;
    int inPadNum = 0;
    int inignore = 0;
    int inAmtIdx = 0;
    int inFmtAmtSize;

    inFmtAmtSize = strlen(szFmt);
    if (strlen(szFmt) == 0)
    {
      strncpy(szOutAmt,szInAmt,inFmtAmtSize);
      return;
    }

    inFmtIdx = 0;
    while(szFmt[inFmtIdx]!= 0x00)
    {
        if(szFmt[inFmtIdx] == 'n')
            inPadNum ++;

        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
            inTotaActNum ++;

        inFmtIdx ++;
    }

    inFmtIdx = 0;
    while(0x30 == szInAmt[inFmtIdx])
    {
        inFmtIdx ++;
    }

    memset(szCurrentAmt, 0x00, sizeof(szCurrentAmt));
    strcpy(szCurrentAmt,&szInAmt[inFmtIdx]);

    if(inPadNum > strlen(szCurrentAmt))
    {
        memset(szBuf, 0x00, sizeof(szBuf));
        memset(szBuf, 0x30, inPadNum-strlen(szCurrentAmt));
        strcat(szBuf, szCurrentAmt);

        strcpy(szCurrentAmt, szBuf);
        strcpy(szFinalFmt, szFmt);
    }

    if(inTotaActNum > strlen(szCurrentAmt))
    {
        inFmtIdx = 0;
        while(szFmt[inFmtIdx]!= 0x00)
        {
            if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
                inignore ++;

            inFmtIdx ++;

            if((inignore >= (inTotaActNum - strlen(szCurrentAmt))) && (szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n'))
                break;
        }

        strcpy(szFinalFmt, &szFmt[inFmtIdx]);
    }

    inFmtIdx = 0;
    inAmtIdx = 0;
    while(szFinalFmt[inFmtIdx]!= 0x00)
    {
      if(szFinalFmt[inFmtIdx] == 'N' || szFinalFmt[inFmtIdx] == 'n')
      {
          szOutAmt[inFmtIdx] = szCurrentAmt[inAmtIdx];
          inFmtIdx++;
          inAmtIdx++;
      }
      else if ((szFinalFmt[inFmtIdx]) == ' ' || (szFinalFmt[inFmtIdx]) == ',' || (szFinalFmt[inFmtIdx]) == '.')
      {
          szOutAmt[inFmtIdx] = szFinalFmt[inFmtIdx];
          inFmtIdx++;
      }
      else
      {
        inFmtIdx ++ ;
      }
    }

    szOutAmt[inFmtIdx] = 0x00;

    return;
}


int inGetIssuerRecord(int inIssuerNumber)
{
    int inRec = 1;
    do
    {
        if (inIITRead(inRec) != d_OK)
        {
            return(d_NO);
        }
        inRec++;
    } while (inIssuerNumber != strIIT.inIssuerNumber);

    return(d_OK);
}

#if 1
int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine, BOOL fDisplayLogo)
//int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine)
{
    USHORT EMVtagLen;
    char szStr[30+1];
    BYTE EMVtagVal[64+1];
    BYTE szTemp1[30+1];
//BYTE fDisplayLogo = 0;
    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));

  	//Display Issuer logo: Clear the display first, then re-display trans title - start -- mfl
	//CTOS_LCDTClearDisplay();
	//vdDispTransTitle(srTransRec.byTransType);
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- mfl

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;

        if(EMVtagLen > 0)
        {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s",szStr);
        }
    }

    if(0 == strlen(szStr))
    {
        //sprintf(szStr, "%s", strCDT.szCardLabel);
        //vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
        sprintf(szStr, "%s", strIIT.szIssuerLabel);
        vdDebug_LogPrintf("Card label: %s", strIIT.szIssuerLabel);
    }

	vdDebug_LogPrintf("inCTOS_DisplayCardTitle|Length[%d]|srTransRec.szPAN[%s]|fDisplayLogo[%d]", strlen(srTransRec.szPAN), srTransRec.szPAN, fDisplayLogo);

	// Support dual card -- sidumili
	if (inFLGGet("fDualCard") == TRUE && strCDT.fBancnet == TRUE && fBancNetTrans == FALSE && strCDT.inDualCardIITid == 0)
	{
		fDisplayLogo = FALSE;
		memset(szStr, 0x00, sizeof(szStr));
		
	}
	
	if (strlen(srTransRec.szPAN) > 0)
	{
	    memset(szTemp1,0,sizeof(szTemp1));
	    vdCTOS_FormatPANEx(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);

	    if(fDisplayLogo == TRUE)
	    {
		    vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
			displayAppbmpDataEx(140, 35, strIIT.szIssuerLogo, TRUE);
	    }
	    if(0>= usCardTypeLine || 0 >= usPANLine)
	    {
	        usCardTypeLine = 3;
	        usPANLine = 4;
	    }

		setLCDPrint(usCardTypeLine, DISPLAY_POSITION_LEFT , szStr);
		setLCDPrint(usPANLine, DISPLAY_POSITION_LEFT , szTemp1);
	}

	vdDebug_LogPrintf("inCTOS_DisplayCardTitle|szStr[%s]|szTemp1[%s]", szStr, szTemp1);

    return(d_OK);
}
#else

int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine)
{
    char szStr[50 + 1];
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];


	//gcitra-0806
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		   return d_OK;
	//gcitra-0806

    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));

	//Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;

        if(EMVtagLen > 0)
        {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s",szStr);
        }
    }

/*
    if(0 == strlen(szStr))
    {
        //1102
        if (srTransRec.byTransType != BIN_VER){
        	sprintf(szStr, "%s", strCDT.szCardLabel);
        	vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
        }
		//1102
    }
*/
    strcpy(szStr,"CARD NUMBER:");
    memset(szTemp1,0,sizeof(szTemp1));
	if (strTCT.fMaskPanDisplay == TRUE){
    	//vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
    	strcpy(szTemp1, srTransRec.szPAN);
		cardMasking(szTemp1, 3);
	}else{
		vdCTOS_FormatPAN(strTCT.DisplayPANFormat, srTransRec.szPAN, szTemp1);
	}

	//Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg

	//1102
	if (srTransRec.byTransType != BIN_VER){
		vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
		if (strCDT.HDTid != 5)
		displayAppbmpDataEx(75, 45, strIIT.szIssuerLogo);
	}
	//1102
//	displayAppbmpDataEx(140, 35, "mc.bmp");

    if(0>= usCardTypeLine || 0 >= usPANLine)
    {
        usCardTypeLine = 5;
        usPANLine = 6;
    }
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg

    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);
    CTOS_LCDTPrintXY(1, usPANLine, szTemp1);

    return(d_OK);
}

#endif

short inCTOS_LoadCDTIndex(void)
{
	signed int inRetVal=0;
	short   shStatus;
	int     i=0, selectedRDTIndex;
	BYTE    shSuitableRDTIndex[10];
	int inIssuer = 0;
	int inNumberOfMatches = 0;
	int inRecNumArray[4+1];
	char szChoiceMsg[30 + 1];
	int    inCardLen, j;
	int inMaxCDTid = 0;
	int inFindRecordNum=0;
	char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0;
    char szCardLabel[3][12+1];
	int inCardLabelIndex=0;

	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	//CTOS_LCDTClearDisplay();
	//vdClearNonTitleLines();

	if(isCheckTerminalMP200() == d_OK)
	    vdClearBelowLine(2);

	if ((strTCT.byTerminalType % 2) == 0)
		CTOS_LCDTPrintXY(1, V3_ERROR_LINE_ROW, "Checking Card... ");
	else
		CTOS_LCDTPrintXY(1, 8, "Checking Card... ");

	inCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);

	vdDebug_LogPrintf("saturn szPAN:[%s]",srTransRec.szPAN);

	vdDebug_LogPrintf("saturn inFindRecordNum[%d]", inFindRecordNum);

	if(inFindRecordNum == 0)
	{
		vdDebug_LogPrintf("saturn Not find in CDT");
		vdSetErrorMessage("CARD NOT SUPPORTED");
		return INVALID_CARD;
	}

	#if 0
	memset(szCardLabel, 0x00, sizeof(szCardLabel));
    for(j=0;j<inFindRecordNum;j++)
    {
        //if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL))
        if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) /*&& !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)*/)
        {
            if ((strMCDT[j].inType == DEBIT_CARD) || (strMCDT[j].inType == EBT_CARD) || (strMCDT[j].IITid != inIssuer))
            {
                vdDebug_LogPrintf("strMCDT[j].inType:[%d]", strMCDT[j].inType);

                if(srTransRec.fCredit == TRUE && strMCDT[j].fBancnet)
                    continue;

                if((srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE) && strMCDT[j].inType == DEBIT_CARD)
                    continue;

                if (strMCDT[j].inType != DEBIT_CARD && strMCDT[j].inType != EBT_CARD)
                    inIssuer = strMCDT[j].IITid;

                vdDebug_LogPrintf("srTransRec.fLoyalty:(%02x)", srTransRec.fLoyalty);
                vdDebug_LogPrintf("strMCDT[j].inLoyaltyGroup:(%d)", strMCDT[j].inLoyaltyGroup);

                inRecNumArray[inNumberOfMatches++] = strMCDT[j].CDTid;

                //#00211 - Terminal prompt to  MASTERCARD and DEBIT when user Swipe MC card on RTR sale and RTR inq
                if(srTransRec.fLoyalty == TRUE && strMCDT[j].inLoyaltyGroup == 1)
                    break;

                if(srTransRec.fCredit == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fInstallment == TRUE
                || srTransRec.fDebit == TRUE)
                    break;

                strcat(&szCardLabel[inCardLabelIndex][0],strMCDT[j].szCardLabel);
                inCardLabelIndex++;
                switch (strMCDT[j].inType)
                {
                    case DEBIT_CARD:
                        break;
                    case EBT_CARD:
                        break;
                    case PURCHASE_CARD:
                        break;
                    default:
                    break;
                }

                if (inNumberOfMatches > 3)
                    break;
            }
        }
    }
	#endif

	// Fix for manual entry
	#if 0
	for(j=0;j<inFindRecordNum;j++)
	{
		if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL))
		{
			if ((strMCDT[j].inType == DEBIT_CARD) || (strMCDT[j].inType == EBT_CARD) || (strMCDT[j].IITid != inIssuer))
			{
				if (strMCDT[j].inType != DEBIT_CARD && strMCDT[j].inType != EBT_CARD)
					inIssuer = strMCDT[j].IITid;

				inRecNumArray[inNumberOfMatches++] = strMCDT[j].CDTid;
				if(inNumberOfMatches > 0)
					break;

				if (inNumberOfMatches > 1)
					szChoiceMsg[strlen(szChoiceMsg)] = '~';

				switch (strMCDT[j].inType)
				{
					case DEBIT_CARD:
						break;
					case EBT_CARD:
						break;
					case PURCHASE_CARD:
						break;
					default:
						break;
				}

				if (inNumberOfMatches > 3)
					break;
			}
		}
    }
	#endif

	inNumberOfMatches = 1;
	inRecNumArray[0] = strMCDT[0].CDTid;	
	
	vdDebug_LogPrintf("inNumberOfMatches[%d]", inNumberOfMatches);

    if(inNumberOfMatches == 1)
    {
        inRetVal = inRecNumArray[0];
    }
    else if (inNumberOfMatches > 1)
    {
		int inLoop;

		memset(szHeaderString, 0x00, sizeof(szHeaderString));
        memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

        for (inLoop = 0; inLoop < 50; inLoop++)
        {
            if (szCardLabel[inLoop][0]!= 0)
            {
                strcat((char *)szChoiceMsg, szCardLabel[inLoop]);
                if (szCardLabel[inLoop+1][0]!= 0)
                    strcat((char *)szChoiceMsg, (char *)" \n");
            }
            else
                break;
        }

		strcpy(szHeaderString, "SELECT CARD");
		key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
		if (key > 0)
		{
			if (key == d_KBD_CANCEL)
			{
				return -1;
			}

			inRetVal = inRecNumArray[key-1];
			vdDebug_LogPrintf("key:[%d]",key);
			vdDebug_LogPrintf("inRetVal:[%d]",inRetVal);
            CTOS_LCDTClearDisplay();
            vdDispTransTitle(srTransRec.byTransType);
		}
    }

	vdDebug_LogPrintf("CDT::inRetVal=[%d]", inRetVal);
	
    if (inRetVal > 0)
    {
        inDatabase_TerminalOpenDatabase();
        inCDTReadEx(inRetVal);

		if(strCDT.fManEntry == FALSE && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
		{
		    inDatabase_TerminalCloseDatabase();
			vdSetErrorMessage("NO MANUAL ENTRY");
			memset(szPAN, 0, sizeof(szPAN));
			memset(szExpireDate, 0, sizeof(szExpireDate));
			return INVALID_CARD;
		}

        srTransRec.CDTid = inRetVal;// save the current CDT that is loaded

        /* Get the CDT also for card labels */
        //inGetIssuerRecord(strCDT.IITid);
        if(inIITReadEx(strCDT.IITid) != d_OK)
        {
             inDatabase_TerminalCloseDatabase();
             vdSetErrorMessage("LOAD IIT ERROR");
             return(d_NO);
        }

        //tine:  close terminal.s3db
        vdDebug_LogPrintf("closing terminal database...");
        inDatabase_TerminalCloseDatabase();
    }
    else
    {
        vdDebug_LogPrintf("2. Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    /* Check for proper card length */
    inCardLen = strlen(srTransRec.szPAN);

	vdDebug_LogPrintf("SATURN --LoadCDTIndex--");
	vdDebug_LogPrintf("strCDT.CDTid[%d]", strCDT.CDTid);
	vdDebug_LogPrintf("strCDT.IITid[%d]", strCDT.IITid);
	vdDebug_LogPrintf("strCDT.HDTid[%d]", strCDT.HDTid);
    vdDebug_LogPrintf("inCardLen: (%d)", inCardLen);
    vdDebug_LogPrintf("strCDT.inMinPANDigit: (%d)", strCDT.inMinPANDigit);
    vdDebug_LogPrintf("strCDT.inMaxPANDigit: (%d)", strCDT.inMaxPANDigit);
    vdDebug_LogPrintf("srTransRec.CDTid: (%d)",srTransRec.CDTid);
	vdDebug_LogPrintf("strCDT.szPANLo[%s]", strCDT.szPANLo);
	vdDebug_LogPrintf("strCDT.szPANHi[%s]", strCDT.szPANHi);
	vdDebug_LogPrintf("strCDT.szCardLabel[%s]", strCDT.szCardLabel);
	vdDebug_LogPrintf("strCDT.inType[%d]", strCDT.inType);
	vdDebug_LogPrintf("strCDT.inInstGroup[%d]", strCDT.inInstGroup);
	vdDebug_LogPrintf("strCDT.fBancnet[%d]", strCDT.fBancnet);
	vdDebug_LogPrintf("strCDT.fDCCEnable[%d]", strCDT.fDCCEnable);
	vdDebug_LogPrintf("strCDT.inDualCardIITid[%d]", strCDT.inDualCardIITid);
	vdDebug_LogPrintf("--LoadCDTIndex--");

    if ((inCardLen < strCDT.inMinPANDigit) ||
        (inCardLen > strCDT.inMaxPANDigit))
    {
        vdDebug_LogPrintf("BAD CARD LEN");
        vdSetErrorMessage("CARD NOT SUPPORTED"); //#00230 - INTERIM: Terminal should accept other cards on MCC ONE APP terminal. Swipe CUP VALID CARD terminal prompted to BAD CARD LEN
        return INVALID_CARD;
    }

    if (strCDT.fluhnCheck == VS_TRUE)   /* Check Luhn */
    {
        if (chk_luhn(srTransRec.szPAN))
        {
            vdDisplayErrorMsg(1, 8, "INVALID LUHN");
            return INVALID_CARD;
        }
    }


    if (strTCT.fDebitFlag != DEBIT && strCDT.inType == DEBIT_CARD )
    {
        vdDisplayErrorMsg(1, 8, "INVALID CARD");
        return INVALID_CARD;
    }

    if(strCDT.fExpDtReqd)
    {
        if(shChk_ExpireDate() != d_OK)
        {
            vdDisplayErrorMsg(1, 8, "CARD EXPIRED");
            if (fECRTxnFlg == TRUE)
            {
                 memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
                 memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
                 strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);
                 strcpy(srTransRec.szResponseText, ECR_EXPIRED_CARD_RESP);
            }
            return CARD_EXPIRED;
        }
    }

    //for save Accum file
    srTransRec.IITid= strCDT.IITid;
    srTransRec.HDTid = strCDT.HDTid;
    srTransRec.inCardType = strCDT.inType;
    srTransRec.CDTid2= srTransRec.CDTid;
    srTransRec.fDCCEnable = strCDT.fDCCEnable;

    vdDebug_LogPrintf("srTransRec.CDTid2: (%d)",srTransRec.CDTid2);

	//CTOS_LCDTClearDisplay();

    return d_OK;;
}

int inCTOS_EMVCardReadProcess (void)
{
    short shResult = 0;
    USHORT usMsgFailedResult = 0;

    vdDebug_LogPrintf("saturn -------shCT0S_EMVInitialize1---[%d]--",shResult);
    shResult = shCTOS_EMVAppSelectedProcess();

    vdDebug_LogPrintf("saturn -------shCT0S_EMVInitialize---[%d]--",shResult);

	if(shResult == EMV_TRANS_FAILED)
    {
        usMsgFailedResult = MSG_TRANS_ERROR;
        return EMV_TRANS_FAILED;
    }

     if(shResult == EMV_CHIP_FAILED)
    {
        usMsgFailedResult = MSG_TRANS_ERROR;
        return usMsgFailedResult;
    }
    else if(shResult == EMV_USER_ABORT)
    {
        usMsgFailedResult = MSG_USER_CANCEL;
        return usMsgFailedResult;
    }
	else if(shResult == EMV_TRANS_NOT_ALLOWED)
	{
			//usMsgFailedResult = EMV_TRANS_NOT_ALLOWED;
			usMsgFailedResult = EMV_APPL_NOT_AVAILABLE;
			return usMsgFailedResult;
	}
	else if(shResult == EMV_FAILURE_EX)
	{
		usMsgFailedResult = EMV_FAILURE_EX;
		return usMsgFailedResult;
	}

	vdDebug_LogPrintf("saturn before getchipdataready");

    shCTOS_EMVGetChipDataReady();

	vdDebug_LogPrintf("saturn emv read card process return");

    return (d_OK);
}


int inCTOS_ManualEntryProcess (BYTE *szPAN)
{
    USHORT  usMaxLen = 19;
    BYTE    szTempBuf[10];
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];

	//tine/sidumili: android - set ui thread to display nothing
	DisplayStatusLine(" ");
	
    //CTOS_LCDTClearDisplay();

//   CTOS_PrinterPutString("inCTOS_ManualEntryProcess");

// fix for issue #00095
//The terminal should no longer accept key entry if Manual Entry is disabled.
   inTCTRead(1);
   if(strTCT.fManualEntry == 0  || srTransRec.fDebit == TRUE)
   {
        vdSetErrorMessage("TRANS NOT ALLWD");
        return d_NO;
    }

    //vdDispTransTitle(srTransRec.byTransType);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "CARD NO: ");	//Tine: 10SEP2019

    if(getCardNO(szPAN) != d_OK)
    {
        return USER_ABORT;
    }

    //CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    //setLCDPrint(4, DISPLAY_POSITION_LEFT, "CARD NO: ");
    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
    memset(bDisplayStr, 0x20, usMaxLen*2);
    strcpy(&bDisplayStr[(usMaxLen-strlen(szPAN))*2], szPAN);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-usMaxLen*2, 5, bDisplayStr);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

    memset(szTempBuf,0,sizeof(szTempBuf));
    if(getExpDate(szTempBuf) != d_OK)
    {
	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];

  	CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//clear buffer on idle swipe on term reg.

        return USER_ABORT;
    }
    wub_str_2_hex(szTempBuf, &srTransRec.szExpireDate[1], 2);
    wub_str_2_hex(&szTempBuf[2], srTransRec.szExpireDate, 2);
    CTOS_KBDBufFlush ();

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MANUAL);

    //vdClearNonTitleLines();
    vdClearBelowLine(2);
//   CTOS_PrinterPutString("inCTOS_ManualEntryProcess.end");

    return d_OK;;
}

int inCTOS_GetCardFields(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    VS_BOOL fInsertOnlyDisp = VS_FALSE;  //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 1 of 3
    VS_BOOL fInsertChipOnly=VS_FALSE;

	short shReturn = d_OK; //Invalid card reading fix -- jzg
	int inEntryMode = 0;

	VS_BOOL fSwipeEnable=strTCT.fSwipeEnable;
	VS_BOOL fCTLSEnable=strTCT.fCTLSEnable;
	VS_BOOL fFalllbackEnable=strTCT.fFalllbackEnable;


	#define INSERT_ONLY 1
	#define SWIPE_ONLY	2
	#define READ_ALL    0

    BYTE szTitle[25+1];
    BYTE szDisMsg[200];	
    BYTE szDisplay[200];
    int inKey;
	ing_KeyPressed=0;
	BYTE szManualEntryFlag[2];

    DebugAddSTR("inCTOS_GetCardFields","Processing...",20);

	vdDebug_LogPrintf("saturn getcardfields");

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

CARD_PROMPT_AGAIN:

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	memset(szManualEntryFlag, 0x00, sizeof(szManualEntryFlag));
	//if(strTCT.fManualEntry == 1 && srTransRec.fDebit==FALSE && srTransRec.fInstallment == FALSE)
	if(strTCT.fManualEntry == 1 && srTransRec.fDebit==FALSE)
        strcpy(szManualEntryFlag, "1");
    else
        strcpy(szManualEntryFlag, "0");


	memset(szDisMsg, 0x00, sizeof(szDisMsg));

    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
	
    strcat(szDisMsg, "PLEASE ");
	if(fSwipeEnable == VS_TRUE)
		strcat(szDisMsg, "SWIPE/");
	strcat(szDisMsg, "INSERT CARD ");
	
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "CARD ENTRY");
	strcat(szDisMsg, "|");
    strcat(szDisMsg, szManualEntryFlag);
	strcat(szDisMsg, "|");
    strcat(szDisMsg, "0"); /*do not display "OTHER PAYMENT OPTIONS"*/
	

    if(fEntryCardfromIDLE != TRUE)
        inKey=usCARDENTRY(szDisMsg);
	else
       {
            vdDebug_LogPrintf("AAA- srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);
			if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
            {
                 vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
                 shReturn = inCTOS_EMVCardReadProcess();
				 vdDebug_LogPrintf("AAA - shReturn[%d]", shReturn);
                 if(shReturn == EMV_TRANS_FAILED) 
                      return EMV_FAILURE_EX;
#if 1
                if (shReturn != d_OK)
                {
                     if(inFallbackToMSR == SUCCESS)
                     {
                          //vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
                          vdDebug_LogPrintf("AAA saturn inFallbackToMSR is success");
                          vdRemoveCard();
                          vdDebug_LogPrintf("AAA saturn  strTCT.inFallbackCounter[%d]", strTCT.inFallbackCounter);
                          if(strTCT.inFallbackCounter > 0)
                          {
                          if(inCTOS_GetCardFieldsFallback() != d_OK)
                              return USER_ABORT;
                          }
                     }
                     else
                     {
                     vdDebug_LogPrintf("AAA inFallbackToMSR is ABORT");
                     //vdSetErrorMessage("Get Card Fail C");
                     return USER_ABORT;
                }
					
                }
#endif
            }
       }
	if (inProcessFallback==1)
		{
		   vdDebug_LogPrintf("AAA - inProcessFallback");
		   return d_OK;
		}

    // flush buffer - for testing
    /*
    fix for issues #00115, 00116 and 00117
    - Able to process card after swiping in Sale Menu
    - Able to process card after swiping Password Entry prompt
    - Able to process card after swiping on Main Menu
    */
    if(fEntryCardfromIDLE != TRUE)
    {
        
		vdDebug_LogPrintf("saturn fEntryCardfromIDLE ! TRUE");
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //aaronnino mcc v3 gprs fix on issue #00005 CARD READ ERROR without swiping / inserting card start
        if (byMSR_status==2)
        {
            //vdSetErrorMessage("CARD READ ERROR");
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "CARD READ ERROR");
            usCTOSS_LCDDisplay(szDisMsg);
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_Beep();
            return INVALID_CARD;
            //return READ_CARD_TIMEOUT;
        }
        //aaronnino mcc v3 gprs fix on issue #00005 CARD READ ERROR without swiping / inserting card end
    }

 SWIPE_AGAIN:

	vdDebug_LogPrintf("saturn SWIPE AGAIN");
	
    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        //if(isCheckTerminalMP200() != d_OK || isCheckTerminalNonTouch() != d_OK)
        //    inCTOS_DisplayIdleBMP();

        //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 2 of 3 start
        if((fEntryCardfromIDLE != TRUE) && (fInsertOnlyDisp == VS_FALSE))
        {
            if(srTransRec.fDebit == TRUE && strTCT.fDebitEMVCapable == TRUE)
            {
				#if 0
				if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
				{
                    setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
					setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT/SWIPE CARD");
				}
				else
				{
					inCTOS_DisplayIdleBMP();
                    displayAppbmpDataEx(1, 30, "SWIPE_INSERT.bmp", FALSE);
				}
				#else
				//setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
				//setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT/SWIPE CARD");
				#endif
            }
            else
            {
				#if 0
				if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
				{
					if (strTCT.fManualEntry)
					{
						setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE INSERT");
						setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE/KEY-IN CARD");
					}
					else
					{
						setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
						setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT/SWIPE CARD");
					}
				}
				else
				{
					inCTOS_DisplayIdleBMP();
					if (strTCT.fManualEntry)
						displayAppbmpDataEx(1, 30, "INSERT.bmp", FALSE);
					else
						displayAppbmpDataEx(1, 30, "SWIPE_INSERT.bmp", FALSE);
				}
				#else
                if (strTCT.fManualEntry)
                {
                    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE INSERT");
                    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE/KEY-IN CARD");
                }
                else
                {
                    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
                    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT/SWIPE CARD");
                }
				#endif
            }
        }
        else if(fInsertOnlyDisp == VS_TRUE)
        {
			#if 0
			if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
			{
				setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
				setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT CARD");
			}
			else
			{
				inCTOS_DisplayIdleBMP();
                displayAppbmpDataEx(1, 30, "INSERT_ONLY.bmp", FALSE);
			}
			#else
			//setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
			//setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT CARD");
			#endif
            fInsertOnlyDisp = VS_FALSE;
        }
        //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 2 of 3 end
    }



    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

	//tine: android - use timer on C
	CTOS_TimeOutSet(TIMER_ID_3, UI_TIMEOUT);

    while (1)
    {
		//tine: android - use timer on C
		if(CTOS_TimeOutCheck(TIMER_ID_3 )  == d_YES) {
             ing_KeyPressed = 'T';
             if (fECRTxnFlg)
             {
             memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
             memcpy(srTransRec.szECRRespCode,ECR_TIMEOUT_ERR,2);
             }
             vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
             CTOS_Beep();
             CTOS_Delay(1500);
             return READ_CARD_TIMEOUT ;
        }
		
		vdDebug_LogPrintf("saturn inside while");

		if (ing_KeyPressed == 'C')
		{
			CTOS_KBDBufPut('C');
			vdDebug_LogPrintf("putchar C");
			/*szGetTransTitle(srTransRec.byTransType, szTitle);
			strcpy(szDisplay, szTitle);
			strcat(szDisplay, "|");
			strcat(szDisplay, "USER CANCEL");
			usCTOSS_LCDDisplay(szDisplay);
			CTOS_Beep();
			CTOS_Delay(1500);
			vdDebug_LogPrintf("saturn key pressed C"); */
			return USER_ABORT;
		}
		else if (ing_KeyPressed == 'T')
		{
			CTOS_KBDBufPut('T');
			vdDebug_LogPrintf("putchar T");
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			vdDebug_LogPrintf("saturn key pressed T");
			CTOS_Beep();
			CTOS_Delay(1500);
			return READ_CARD_TIMEOUT ;
		}
		else if (ing_KeyPressed == 'M')     //Manual Entry
	{
		CTOS_KBDBufPut('M');
		vdDebug_LogPrintf("putchar M");
        inCTOSS_CLMCancelTransaction();
        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
        {
            vdSetFirstIdleKey(0x00);
            CTOS_KBDBufFlush ();
            //vdSetErrorMessage("Get Card Fail M");
            return USER_ABORT;
        }

        //Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

        break;
	}

	vdDebug_LogPrintf("saturn check ing_KeyPressed");

		vdDebug_LogPrintf("saturn check sc status");
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("saturn --EMV flow----" );
            //if (d_OK != inCTOS_EMVCardReadProcess ())
            	 
			vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
            
            shReturn = inCTOS_EMVCardReadProcess();
			vdDebug_LogPrintf("inCTOS_GetCardFields | shReturn[%d]", shReturn);
			if (shReturn != d_OK)
            {
            	if ((shReturn == EMV_TRANS_NOT_ALLOWED) || (shReturn == EMV_FAILURE_EX || shReturn == EMV_APPL_NOT_AVAILABLE))
            	{
					vdCTOS_ResetMagstripCardData();
					if((shReturn == EMV_APPL_NOT_AVAILABLE) && (fBancNetTrans==FALSE))
					{
						vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
						inFallbackToMSR = FAIL;
					}
					else
					{
						vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
						inFallbackToMSR = SUCCESS;
					}
            	}

                if(inFallbackToMSR == SUCCESS)
                {
                  	//vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");					
                    vdRemoveCard();
					
					if(fFalllbackEnable == VS_FALSE)
					{
					    vdRemoveCard();
						goto CARD_PROMPT_AGAIN;
						//return USER_ABORT;
					}
					
                    if(strTCT.inFallbackCounter > 0)
                    {
						if(inCTOS_GetCardFieldsFallback() != d_OK)
							return USER_ABORT;
                    }
                }
                else
                {
					if(shReturn == EMV_CHIP_FAILED)
					    goto CARD_PROMPT_AGAIN;
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                /*memset(szDisMsg, 0x00, sizeof(szDisMsg));
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "PLS INSERT CARD");
				
               // usCTOSS_LCDDisplay(szDisMsg);
               usCARDENTRY(szDisMsg);*/
               
             memset(szDisMsg, 0x00, sizeof(szDisMsg));               
             strcpy(szDisMsg, szTitle);
             strcat(szDisMsg, "|");
             strcat(szDisMsg, "PLEASE INSERT CARD");
             strcat(szDisMsg, "|");
             strcat(szDisMsg, "CARD ENTRY");
             strcat(szDisMsg, "|");
             strcat(szDisMsg, "0"); //szManualEntryFlag 0 is off 1 is on
			  
              ing_KeyPressed = 0;
              usCARDENTRY(szDisMsg);
				
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();
                fInsertChipOnly=VS_TRUE;
                goto SWIPE_AGAIN;

             }

             break;

         }


        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

		//aaronnino mcc v3 gprs fix on issue #0049 1 minute time-out in wrong swiping of card start
        if (byMSR_status==2)
        {
            if(fSwipeEnable == FALSE)
            {
                vdDisplayMessageBox(1, 8, "", "NOT SUPPORTED", "", MSG_TYPE_WARNING);
                CTOS_Beep();
                CTOS_Delay(1500);
                goto CARD_PROMPT_AGAIN;
            }
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "CARD READ ERROR");
            usCTOSS_LCDDisplay(szDisMsg);
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_Beep();
            return READ_CARD_TIMEOUT;
        }
		//aaronnino mcc v3 gprs fix on issue #0049 1 minute time-out in wrong swiping of card end

		//Fix for Track2 Len < 35
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
        {
			if(fSwipeEnable == FALSE)
			{
				vdDisplayMessageBox(1, 8, "", "NOT SUPPORTED", "", MSG_TYPE_WARNING);
				CTOS_Beep();
				CTOS_Delay(1500);
				goto CARD_PROMPT_AGAIN;
			}

			if(fInsertChipOnly == VS_TRUE)
			    continue;

            vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }

            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                
                /*memset(szDisMsg, 0x00, sizeof(szDisMsg));
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "PLS INSERT CARD");
				
                usCTOSS_LCDDisplay(szDisMsg);*/
                      /* memset(szDisMsg, 0x00, sizeof(szDisMsg));
                       strcpy(szDisMsg, szTitle);
                       strcat(szDisMsg, "|");
                       strcat(szDisMsg, "PLS INSERT CARD");
                       strcat(szDisMsg, "|");
                       strcat(szDisMsg, "CARD ENTRY");
                       strcat(szDisMsg, "|");
                       strcat(szDisMsg, szManualEntryFlag);
	
                       if(fEntryCardfromIDLE != TRUE)
                                  inKey=usCARDENTRY(szDisMsg);*/

                       memset(szDisMsg, 0x00, sizeof(szDisMsg));               
                       strcpy(szDisMsg, szTitle);
                       strcat(szDisMsg, "|");
                       strcat(szDisMsg, "PLEASE INSERT CARD");
                       strcat(szDisMsg, "|");
                       strcat(szDisMsg, "CARD ENTRY");
                       strcat(szDisMsg, "|");
                       strcat(szDisMsg, "0"); //szManualEntryFlag 0 is off 1 is on
                       
                       ing_KeyPressed = 0;
                       usCARDENTRY(szDisMsg);
				
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();
				fInsertOnlyDisp = VS_TRUE; //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 3 of 3
                fInsertChipOnly=VS_TRUE;
                goto SWIPE_AGAIN;

             }

            break;
        }

       }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}

int inCTOS_GetCardFieldsFallback(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    int inFallBackCtr=0;
	short shReturn = d_OK; //Invalid card reading fix -- jzg

     BYTE szTitle[25+1];
     BYTE szDisMsg[50];
     BYTE szDisplay[200];

//CTOS_PrinterPutString("inCTOS_GetCardFieldsFallback");

vdDebug_LogPrintf("saturn AAA - inCTOS_GetCardFieldsFallback start srTransRec.byEntryMode [%d]", srTransRec.byEntryMode );

    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        inFallBackCtr=strTCT.inFallbackCounter-1;
        if(inFallBackCtr <= 0)
        {
            //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
        }
    }
 DebugAddSTR("inCTOS_GetCardFieldsFallback","Processing...",20);

     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

 INSERT_AGAIN:
	// flush buffer - for testing
	/*
		fix for issues #00115, 00116 and 00117
	- Able to process card after swiping in Sale Menu
	- Able to process card after swiping Password Entry prompt
	- Able to process card after swiping on Main Menu
	*/
     byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
         
    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
         CTOS_LCDTClearDisplay();
         vdDispTransTitle(srTransRec.byTransType);
         
         //if(isCheckTerminalMP200() != d_OK)
         //    inCTOS_DisplayIdleBMP();
         
         if(inFallBackCtr > 0)
         {
              #if 0
              if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
              {
                   setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
                   setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT CARD");
              }
              else
              {
                   inCTOS_DisplayIdleBMP();
                   displayAppbmpDataEx(1, 30, "INSERT_ONLY.bmp", FALSE);
              }
              #else
              //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
              // setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT CARD");
              memset(szTitle, 0x00, sizeof(szTitle));
              szGetTransTitle(srTransRec.byTransType, szTitle);
              memset(szDisMsg, 0x00, sizeof(szDisMsg));
              //strcpy(szDisMsg, szTitle);
              //strcat(szDisMsg, "|");
              //strcat(szDisMsg, "PLEASE INSERT CARD");
              //usCTOSS_LCDDisplay(szDisMsg);
              
			  strcpy(szDisMsg, szTitle);
			  strcat(szDisMsg, "|");
			  strcat(szDisMsg, "PLEASE INSERT CARD");
			  strcat(szDisMsg, "|");
			  strcat(szDisMsg, "CARD ENTRY");
			  strcat(szDisMsg, "|");
			  strcat(szDisMsg, "0"); //szManualEntryFlag 0 is off 1 is on
			  strcat(szDisMsg, "|");
			  strcat(szDisMsg, "0"); /*do not display "OTHER PAYMENT OPTIONS"*/
			  
              ing_KeyPressed = 0;
              usCARDENTRY(szDisMsg);
              CTOS_Beep();
              CTOS_Delay(1500);
              
              
              if (ing_KeyPressed == 'C')
              {
                   CTOS_KBDBufPut('C');
                  /* vdDebug_LogPrintf("putchar C");
                   szGetTransTitle(srTransRec.byTransType, szTitle);
                   strcpy(szDisplay, szTitle);
                   strcat(szDisplay, "|");
                   strcat(szDisplay, "USER CANCEL");
                   usCTOSS_LCDDisplay(szDisplay);
                   CTOS_Beep();
                   CTOS_Delay(1500); */                 
                   return USER_ABORT;
              }
              else if (ing_KeyPressed == 'T')
              {
                   CTOS_KBDBufPut('T');
                   vdDebug_LogPrintf("putchar T");
                   vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
                   CTOS_Beep();
                   CTOS_Delay(2000);
                   return READ_CARD_TIMEOUT ;
              }
              #endif	
             
         }
         else
         {
         #if 0
         if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
         {
         setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
         setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
         }
         
         else
         {
         inCTOS_DisplayIdleBMP();
         displayAppbmpDataEx(1, 30, "SWIPE_ONLY.bmp", FALSE);	//#00201 - Incorrect terminal display on Fallback to MAGSTRIPE
         }
         #else
         //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
         //setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
         /*memset(szTitle, 0x00, sizeof(szTitle));
         szGetTransTitle(srTransRec.byTransType, szTitle);
         strcpy(szDisMsg, szTitle);
         strcat(szDisMsg, "|");
         strcat(szDisMsg, "PLS SWIPE CARD");
         //usCTOSS_LCDDisplay(szDisMsg);        //Tine:  24Apr2019
         //test
         
		 usCARDENTRY(szDisMsg);			 //Tine:  24Apr2019*/

			
			memset(szTitle, 0x00, sizeof(szTitle));
			szGetTransTitle(srTransRec.byTransType, szTitle);
			memset(szDisMsg, 0x00, sizeof(szDisMsg));
            memset(szDisMsg, 0x00, sizeof(szDisMsg));
            
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "PLEASE SWIPE CARD");
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "CARD ENTRY");
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "0");
            
            
            //if(fEntryCardfromIDLE != TRUE)
                    usCARDENTRY(szDisMsg);
         #endif
         
         }
    }

    #if 0
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30];
    		BYTE szTemp1[30+1];

    		CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);
    	}
    }
	#endif
    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , UI_TIMEOUT);

    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES) {
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);             
			CTOS_Beep();
			CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }

		
		if (ing_KeyPressed == 'C')
		{
			CTOS_KBDBufPut('C');
			/*vdDebug_LogPrintf("putchar C");
			szGetTransTitle(srTransRec.byTransType, szTitle);
			strcpy(szDisplay, szTitle);
			strcat(szDisplay, "|");
			strcat(szDisplay, "USER CANCEL");
			usCTOSS_LCDDisplay(szDisplay);
			CTOS_Beep();
			CTOS_Delay(1500);*/
			return USER_ABORT;
		}
		else if (ing_KeyPressed == 'T')
		{
			CTOS_KBDBufPut('T');
			vdDebug_LogPrintf("putchar T");
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);             
			CTOS_Beep();
			CTOS_Delay(1500);
			return READ_CARD_TIMEOUT ;
		}
#if 0
        CTOS_KBDInKey(&byKeyBuf);

        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
            BYTE bKey = 0x00;

            CTOS_KBDHit(&bKey);
            if(bKey == d_KBD_CANCEL)
            {
                vdSetErrorMessage("USER CANCEL");
                return USER_ABORT;
            }
            else
                continue;
        }
#endif
		if(inFallBackCtr > 0)
		{
            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if(bySC_status & d_MK_SC_PRESENT)
            {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
                 vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
                vdDebug_LogPrintf("--EMV flow----" );
                //if (d_OK != inCTOS_EMVCardReadProcess ())
                shReturn = inCTOS_EMVCardReadProcess();
                vdDebug_LogPrintf("inCTOS_GetCardFieldsFallback | shReturn[%d]", shReturn);
				if (shReturn != d_OK)
                {
                    if(inFallbackToMSR == SUCCESS)
                    {
                        //vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
                        vdRemoveCard();
                        inFallBackCtr--;
						if(inFallBackCtr <= 0){
//CTOS_PrinterPutString("inCTOS_GetCardFieldsFallback-1");
                                               vdDebug_LogPrintf("AAA - gotoinsert_again");
						    //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
							}
                        goto INSERT_AGAIN;
                    }
                    else
                    {
                        return USER_ABORT;
                    }
                }
                vdDebug_LogPrintf("--EMV Read succ----" );
                //Load the CDT table
                if (d_OK != inCTOS_LoadCDTIndex())
                {
                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                }
                break;
            }
		}
		else
		{

            byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
            if(byMSR_status == d_OK )
            {
                vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

                if (d_OK != inCTOS_LoadCDTIndex())
                {
                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                }

				if(d_OK != inCTOS_CheckEMVFallback())
				 {
					vdCTOS_ResetMagstripCardData();
					CTOS_KBDBufFlush();
					return USER_ABORT;
				 }
				inProcessFallback = 1;
                break;
            }
		}
    }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}

int inCTOS_GetCardFieldsNoEMV(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;

 DebugAddSTR("inCTOS_GetCardFieldsNoEMV","Processing...",20);

     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

 	// flush buffer - for testing
	/*
		fix for issues #00115, 00116 and 00117
	- Able to process card after swiping in Sale Menu
	- Able to process card after swiping Password Entry prompt
	- Able to process card after swiping on Main Menu
	*/

    if(fEntryCardfromIDLE != TRUE)
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);

	 //if(isCheckTerminalMP200() != d_OK)
	 //    inCTOS_DisplayIdleBMP();

	 if(fEntryCardfromIDLE != TRUE)
	 {
	 	if(srTransRec.fDebit == TRUE)
	 	{
			#if 0
            if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
            {
                setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
                setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
            }
            else
            {
            	inCTOS_DisplayIdleBMP();
                displayAppbmpDataEx(1, 30, "SWIPE_ONLY.bmp", FALSE);
            }
			#else
            //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
            //setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
			#endif
	 	}
	    else
	    {
			#if 0
            if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
            {
            	if (strTCT.fManualEntry)
            	{
            		setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
	                setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE/KEY-IN CARD");
            	}
				else
				{
					setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
	                setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
				}
            }
            else
            {
            	inCTOS_DisplayIdleBMP();
				if (strTCT.fManualEntry)
	            	displayAppbmpDataEx(1, 30, "SWIPE.bmp", FALSE);
				else
					displayAppbmpDataEx(1, 30, "SWIPE_ONLY.bmp", FALSE);
            }
			#else
            if (strTCT.fManualEntry)
            {
                //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
               // setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE/KEY-IN CARD");
            }
            else
            {
                //setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
                //setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
            }
			#endif
	    }
	 }
    }
	#if 0
// patrick ECR 20140516 start
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30];
    		BYTE szTemp1[30+1];

    		CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);
    	}
    }
// patrick ECR 20140516 end
#endif

    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES) {
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(1500);
        }
            return READ_CARD_TIMEOUT ;

        if(strlen(szPAN) <= 0)
        {
           CTOS_KBDInKey(&byKeyBuf);
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
            BYTE bKey = 0x00;

            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
            {
                bKey = srTransRec.szPAN[0] = chGetFirstIdleKey();
				vdSetFirstIdleKey(0x00);
            }
            else
                CTOS_KBDHit(&bKey); /*albert - start - only accept 0-9, exit on cancel key*/

            if((bKey >= d_KBD_1) && (bKey <= d_KBD_9))
                srTransRec.szPAN[0]=bKey;
            else if(bKey == d_KBD_CANCEL)
            {
				vdSetErrorMessage("USER CANCEL");
                return USER_ABORT;
            }
            else
                continue;
            /*albert - end - only accept 0-9, exit on cancel key*/

	    //if(srTransRec.fDebit == TRUE) {   // 00212 - NO MANUAL ENTRY for Debit Sale
        //	vdSetErrorMessage("TRANS NOT ALLWD");
	//	goto SWIPE_AGAIN;
	    //}

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }
        }
		else
		{
			strcpy(srTransRec.szPAN, szPAN);
			memcpy(srTransRec.szExpireDate, szExpireDate, 2);
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
			memset(szPAN, 0, sizeof(szPAN));
			memset(szExpireDate, 0, sizeof(szExpireDate));
			break;
		}
        #if 0 /*do not process installment with EMV*/
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }
        #endif

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }
             #if 0
             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");

                goto SWIPE_AGAIN;

             }
             #endif
             break;

         }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
		//Fix for Track2 Len < 35
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//Fix for Track2 Len < 35
        {
            vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }
            #if 0
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");

                goto SWIPE_AGAIN;

             }
            #endif
            break;
        }

       }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}

#if 1
void vdTrimLeadZeroes (char *pchString)
{
	int inIndex, inLen = 0;

	inIndex = 0;
	while (pchString[inIndex] == '0')
		inIndex++;
	if (inIndex != 0)
	{
		inLen = strlen(pchString);
		strcpy(pchString, &(pchString[inIndex]));
		pchString[inLen - inIndex] = '\0';	// clean up remaining char
	}
}

void InsertCardUI(void)
{	

    BYTE szTitle[25+1];
	BYTE szDisMsg[200];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];

    int inKey;

#if 0
	usCTOSS_DisplayUI("saturn insertcardUI");

    memset(szTitle, 0x00, sizeof(szTitle));
	memset(szDisMsg, 0x00, sizeof(szDisMsg));

    strcpy(szDisMsg, "cardidle");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "back");

    szGetTransTitle(srTransRec.byTransType, szTitle);


	vdDebug_LogPrintf("szTitle[%s]", szTitle);
    strcat(szDisMsg, "|");
	strcat(szDisMsg, szTitle);


    memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
	//wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
	wub_hex_2_str(srTransRec.szBaseAmount, szTotalAmt, AMT_BCD_SIZE);	
    
    vdTrimLeadZeroes(szTotalAmt);

    memset(szStr, 0x00, sizeof(szStr));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
    vdDebug_LogPrintf("szStr[%s]", szStr);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "Amount:");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szStr);
 
 	usCTOSS_DisplayUI(szDisMsg);

#else
strcpy(szDisMsg, szTitle);
 strcat(szDisMsg, "|");
 strcat(szDisMsg, "PLEASE SWIPE/INSERT/TAP CARD");
 strcat(szDisMsg, "|");
 strcat(szDisMsg, "CARD ENTRY");
 //usCTOSS_LCDDisplay(szDisMsg);
 inKey = usCARDENTRY(szDisMsg); 	

#endif
}


void ProcessingUI(void)
{	

    BYTE szTitle[25+1];
	BYTE szDisMsg[200];

	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, "process");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "front");
	
 	usCTOSS_DisplayUI(szDisMsg);


}


int inCTOS_WaveGetCardFields(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[4+1],szCurrCode[3+1];

	//0826
        int inchipswiped = 0; //mfl issue number 000113 1 of 3
	int inChipTries=0;
	int inEntryMode=0;
        #define INSERT_ONLY 1
        #define SWIPE_ONLY	2
        #define READ_ALL 0
	/*
	1= insert only
	2= swipe only
	0= will accept al
	*/
	// mfl

	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- mfl */
	BOOL fMaxCTLSAmt = FALSE;
	long amt1 = 0;
	long amt2 = 0;
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- mfl */
    BYTE szTitle[25+1];
    BYTE szDisMsg[200];

    int inKey;
	
	short shReturn = d_OK; //Invalid card reading fix -- jzg

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFields.........");


    memset(szDisMsg, 0x00, sizeof(szDisMsg));
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "SWIPE/INSERT/TAP CARD");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "CARD ENTRY");
    inKey=usCARDENTRY(szDisMsg);			


//    CTOS_PrinterPutString("inCTOS_WaveGetCardFields.start");
	 inEntryMode = READ_ALL;

	 //gcitra
	 //CTOS_LCDTClearDisplay();
	 //gcitra

     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	//CTOS_LCDTClearDisplay();

 // patrick fix code 20141222 case 179
  	if (fEntryCardfromIDLE != TRUE)
	 		byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    SWIPE_AGAIN:
    EntryOfStartTrans:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);

        inCTOS_DisplayIdleBMP();
    }

	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(temp,0x00,sizeof(temp));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
	CTOS_LCDTPrintXY(1, 4, temp);

	if (srTransRec.byTransType == REFUND)
		szTransType[0] = 0x20;

	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);

#if 0
	//ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
#else
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- mfl */
	amt1 = wub_str_2_long(szTotalAmount);
	amt2 = wub_str_2_long(strTCT.szMaxCTLSAmount);

	if (amt1 >= amt2)
		fMaxCTLSAmt = TRUE;
	else
		fMaxCTLSAmt = FALSE;
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- mfl */


    #if 0
    	{
	char sztemp[80+1];


	memset(sztemp,0x00,sizeof(sztemp));
	sprintf(sztemp,"fMaxCTLSAmt:[%ld]", fMaxCTLSAmt);
	CTOS_PrinterPutString(sztemp);
    	}
     #endif

	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- mfl */
	if ((inEntryMode == READ_ALL) && (!fMaxCTLSAmt))
	{
//	CTOS_PrinterPutString("inCTOS_WaveGetCardFields");

		ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
		if(ulAPRtn != d_EMVCL_NO_ERROR)
		{
			vdSetErrorMessage("CTLS InitTrans Fail!");
			return d_NO;
		}
	}
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- mfl */

#endif


	if(ulAPRtn != d_EMVCL_NO_ERROR)
	{
		vdSetErrorMessage("CTLS InitTrans Fail!");
		return d_NO;
	}

    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

    //CTOS_LCDTClearDisplay();

	//mfl-0728
	vdDispTransTitle(srTransRec.byTransType);
	if (inEntryMode  == SWIPE_ONLY){
            CTOS_LCDTPrintXY(1, 3, "Please Swipe");
            CTOS_LCDTPrintXY(1, 4, "Customer Card");
	}
        //mfl issue number 000113 2 of 3 START
        else if (inchipswiped == 1)
        {
            CTOS_LCDTPrintXY(1, 3, "              ");
            CTOS_LCDTPrintXY(1, 4, "              ");
            inchipswiped = 0;
        }
        //mfl issue number 000113 2 of 3 END
        else
        {
			//issue:218
		if (fEntryCardfromIDLE != TRUE){
	            	CTOS_LCDTPrintXY(1, 3, "Swipe/Enter/Insert");
	            	CTOS_LCDTPrintXY(1, 4, "Tap/Cancel");
		}
	}
	//mfl-0728

    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	inCTOSS_CLMCancelTransaction();
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf);

        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
        	inCTOSS_CLMCancelTransaction();
            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }



        CTOS_SCStatus(d_SC_USER, &bySC_status);
#if 0
{
		char temp[30+1];
		memset(temp,0x00,sizeof(temp));
		sprintf(temp, "*************::%d:%d", bySC_status, d_MK_SC_PRESENT);

    		CTOS_PrinterPutString(temp);
}
#endif

        if(bySC_status & d_MK_SC_PRESENT)
        {
//    CTOS_PrinterPutString("inCTOS_WaveGetCardFields.######");

        	inCTOSS_CLMCancelTransaction();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("--EMV flow----" );
            //if (d_OK != inCTOS_EMVCardReadProcess ())
            shReturn = inCTOS_EMVCardReadProcess();
            vdDebug_LogPrintf("inCTOS_WaveGetCardFields | shReturn[%d]", shReturn);
			if (shReturn != d_OK)
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLEASE SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }


        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
//    CTOS_PrinterPutString("@@@@@@@@@@@@@@@");

         	inCTOSS_CLMCancelTransaction();
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                /*vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                
                memset(szDisMsg, 0x00, sizeof(szDisMsg));
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "PLS INSERT CARD");
				
                usCTOSS_LCDDisplay(szDisMsg);*/

              memset(szDisMsg, 0x00, sizeof(szDisMsg));               
              strcpy(szDisMsg, szTitle);
              strcat(szDisMsg, "|");
              strcat(szDisMsg, "PLEASE INSERT CARD");
              strcat(szDisMsg, "|");
              strcat(szDisMsg, "CARD ENTRY");
              strcat(szDisMsg, "|");
              strcat(szDisMsg, "0"); //szManualEntryFlag 0 is off 1 is on
			  
              ing_KeyPressed = 0;
              usCARDENTRY(szDisMsg);
				
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();

                goto SWIPE_AGAIN;

             }

             break;

         }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        if((byMSR_status == d_OK ) && (usTk2Len > 35))
        {

        	inCTOSS_CLMCancelTransaction();
            vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }

            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                
                /*memset(szDisMsg, 0x00, sizeof(szDisMsg));
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "PLS INSERT CARD");
				
                usCTOSS_LCDDisplay(szDisMsg);*/

             memset(szDisMsg, 0x00, sizeof(szDisMsg));               
             strcpy(szDisMsg, szTitle);
             strcat(szDisMsg, "|");
             strcat(szDisMsg, "PLEASE INSERT CARD");
             strcat(szDisMsg, "|");
             strcat(szDisMsg, "CARD ENTRY");
             strcat(szDisMsg, "|");
             strcat(szDisMsg, "0"); //szManualEntryFlag 0 is off 1 is on
			  
              ing_KeyPressed = 0;
              usCARDENTRY(szDisMsg);
				
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();

                goto SWIPE_AGAIN;

             }

            break;
        }

		ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

		if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
		{
			vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
		}
        else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
        {
            vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
            CTOS_Delay(3000);
            goto EntryOfStartTrans;
        }
		else if(ulAPRtn == d_EMVCL_TX_CANCEL)
		{
			//vdDisplayErrorMsg(1, 8, "USER CANCEL");
			return USER_ABORT;
		}
		else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
		{
			inCTOSS_CLMCancelTransaction();
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
            CTOS_Beep();
            CTOS_Delay(2000);
			return USER_ABORT;
		}
		else if(ulAPRtn != d_EMVCL_PENDING)
		{
			vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
			break;
		}
	}

	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime -- jzg */
	if ((srTransRec.byEntryMode == CARD_ENTRY_WAVE) && (!fMaxCTLSAmt))
	{
		if(ulAPRtn != d_EMVCL_RC_DATA)
		{
			vdCTOSS_WaveCheckRtCode(ulAPRtn);
			inCTOSS_CLMCancelTransaction();
			return d_NO;
		}

		if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
		{
			inCTOSS_CLMCancelTransaction();
			return d_NO;
		}

		//Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
        	inCTOSS_CLMCancelTransaction();
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		inCTOSS_CLMCancelTransaction();
	}

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
//    CTOS_PrinterPutString("^^^^^^^^^^^^^^^^^^^");

        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
//    CTOS_PrinterPutString("inCTOS_WaveGetCardFields.end");

    return d_OK;;
}
#else
int inCTOS_WaveGetCardFields(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
        int inchipswiped = 0; //aaalcantara issue number 000113 1 of 3
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1],szCurrCode[3+1];

	CTOS_RTC SetRTC;

	//0826
	int inChipTries=0;
	int inEntryMode=0;
	/*
	1= insert only
	2= swipe only
	0= will accept al
	*/

	//CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - start -- jzg
    char szBaseAmount[20] = {0};
    char szTipAmount[20] = {0};
	//CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - end -- jzg

#define INSERT_ONLY 1
#define SWIPE_ONLY	2
#define READ_ALL 0
	//0826

	short shReturn = d_OK; //Invalid card reading fix -- jzg


	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
	BOOL fMaxCTLSAmt = FALSE;
	long amt1 = 0;
	long amt2 = 0;
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFields.........");

    CTOS_PrinterPutString("inCTOS_WaveGetCardFields");

	 inEntryMode = READ_ALL;


	 //gcitra
	 CTOS_LCDTClearDisplay();
	 //gcitra

     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	CTOS_LCDTClearDisplay();

 // patrick fix code 20141222 case 179
  	if (fEntryCardfromIDLE != TRUE)
	 		byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);


 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        //CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }

	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(temp,0x00,sizeof(temp));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));

//gcitra
#if 0
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
	CTOS_LCDTPrintXY(1, 4, temp);
#endif

	//CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - start -- jzg
    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);
	// patrick fix code 20141216
    sprintf(szTotalAmount, "%012.0f", atof(szBaseAmount) + atof(szTipAmount));
    wub_str_2_hex(szTotalAmount, srTransRec.szTotalAmount, 12);
//	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6); //CTLS - Fix for CTLS reader not accepting PayPass Cards  -- jzg
	//CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - end -- jzg

	if (srTransRec.byTransType == REFUND)
		szTransType[0] = 0x20;
	//CTLS - Fix for CTLS reader not accepting PayPass Cards - start  -- jzg
	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);
	//CTLS - Fix for CTLS reader not accepting PayPass Cards  - end -- jzg


	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
	amt1 = wub_str_2_long(szTotalAmount);
	amt2 = wub_str_2_long(strTCT.szMaxCTLSAmount);

	if (amt1 >= amt2)
		fMaxCTLSAmt = TRUE;
	else
		fMaxCTLSAmt = FALSE;
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
	if ((inEntryMode == READ_ALL) && (!fMaxCTLSAmt))
	{
		ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
		if(ulAPRtn != d_EMVCL_NO_ERROR)
		{
			vdSetErrorMessage("CTLS InitTrans Fail!");
			return d_NO;
		}
	}
	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

	//gcitra-0728
	vdDispTransTitle(srTransRec.byTransType);
	if (inEntryMode  == SWIPE_ONLY){
            CTOS_LCDTPrintXY(1, 3, "Please Swipe");
            CTOS_LCDTPrintXY(1, 4, "Customer Card");
	}
        //aaalcantara issue number 000113 2 of 3 START
        else if (inchipswiped == 1)
        {
            CTOS_LCDTPrintXY(1, 3, "              ");
            CTOS_LCDTPrintXY(1, 4, "              ");
            inchipswiped = 0;
        }
        //aaalcantara issue number 000113 2 of 3 END
        else
        {
			//issue:218
			if (fEntryCardfromIDLE != TRUE){
            	CTOS_LCDTPrintXY(1, 3, "Swipe/Enter/Insert");
            	CTOS_LCDTPrintXY(1, 4, "Tap/Cancel");
			}
	}
	//gcitra-0728


	//0826
		INSERT_AGAIN:
	//0826


    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	inCTOSS_CLMCancelTransaction();
            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf);
                 // patrick add code 20141209
        if (byKeyBuf)
        {
            CTOS_KBDGet(&byKeyBuf);
                         switch(byKeyBuf)
                         {
                                  case d_KBD_0:
                                  case d_KBD_1:
                                  case d_KBD_2:
                                  case d_KBD_3:
                                  case d_KBD_4:
                                  case d_KBD_5:
                                  case d_KBD_6:
                                  case d_KBD_7:
                                  case d_KBD_8:
                                  case d_KBD_9:
                                  case d_KBD_CANCEL:

                                  inCTOSS_CLMCancelTransaction();

                                  //gcitra-0728
                                  if (byKeyBuf == d_KBD_CANCEL){
                                          CTOS_KBDBufFlush();
                                          return USER_ABORT;
                                  }
                                  //gcitra-0728

                                  vdSetFirstIdleKey(byKeyBuf);

                                  memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
                                  //gcitra
                                  //if(d_OK == inCTOS_ValidFirstIdleKey())
                                  //       srTransRec.szPAN[0] = chGetFirstIdleKey();
                                  //gcitra

                                  vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
                                  //get the card number and ger Expire Date
                                  if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                                  {
                                          vdSetFirstIdleKey(0x00);
                                          CTOS_KBDBufFlush ();
                                          //vdSetErrorMessage("Get Card Fail M");
                                          return USER_ABORT;
                                  }

                                  //Load the CDT table
                                  if (d_OK != inCTOS_LoadCDTIndex())
                                  {
                                                                   //gcitra-120214
                                                                   CTOS_KBDBufFlush();
                                                                   return USER_ABORT;
                                                                   //gcitra-120214
                                  }

                                  break;
                         }
        }


        if (inEntryMode != SWIPE_ONLY){
//INSERT_AGAIN:

			if (inEntryMode == INSERT_ONLY){
				byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
				vdCTOS_ResetMagstripCardData();
				vdDispTransTitle(srTransRec.byTransType);
				CTOS_LCDTPrintXY(1, 3, "Please Insert Card/");
				CTOS_LCDTPrintXY(1, 4, "Cancel");
			}
//0826

        CTOS_SCStatus(d_SC_USER, &bySC_status);

{
		char temp[30+1];
		memset(temp,0x00,sizeof(temp));
		sprintf(temp, "*************::%d:%d", bySC_status, d_MK_SC_PRESENT);

    		CTOS_PrinterPutString(temp);
}


        if(bySC_status & d_MK_SC_PRESENT)
        {
        	inCTOSS_CLMCancelTransaction();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            //vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
				if(inFallbackToMSR == SUCCESS)
				{
					vdCTOS_ResetMagstripCardData();
					//0826
					//vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
					//goto SWIPE_AGAIN;
					vduiClearBelow(2);
					vdRemoveCard();
					clearLine(7);
					inChipTries= inChipTries+1;
					if (inChipTries < 3){
						inEntryMode = INSERT_ONLY;
						goto INSERT_AGAIN;
					}else{
						inEntryMode = SWIPE_ONLY;
						//1125
						vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
						//1125
						goto SWIPE_AGAIN;
					}
					//0826
				}else{
					//vdSetErrorMessage("Get Card Fail C");
					return USER_ABORT;
				}
            }
            //vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }


		if (inEntryMode == INSERT_ONLY)
			goto INSERT_AGAIN;
		}

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
         	inCTOSS_CLMCancelTransaction();
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");

                goto SWIPE_AGAIN;

             }

             break;

         }


#if 0 //uncommnet if neede - mfl

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
		//gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//gcitra

		//if((byMSR_status == d_OK ) && (usTk2Len > 35))
        {
        	inCTOSS_CLMCancelTransaction();

					//Invalid card reading fix - start -- jzg
					shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

					vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

					if (shReturn == INVALID_CARD)
					{
									CTOS_KBDBufFlush();
									vdDisplayErrorMsg(1, 8, "INVALID CARD");
									return INVALID_CARD;
					}
					//Invalid card reading fix - end -- jzg

            //1125
			if(inFallbackToMSR == SUCCESS)
				vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
			//1125

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }

            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                inchipswiped = 1; //aaalcantara issue number 000113 3 of  3
                goto SWIPE_AGAIN;

             }

            break;
        }
#endif
		if(inEntryMode == SWIPE_ONLY){
           //inCTOSS_CLMCancelTransaction();
		   goto SWIPE_AGAIN;
		}

		/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
		if(!fMaxCTLSAmt)
		{
			ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

			if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
			{
				vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
			}
			//EMV: Added error message handling "PLEASE SEE PHONE" - start -- jzg
			else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
			{
				vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
				CTOS_Delay(3000);
				vdDisplayErrorMsg(1, 8, "                ");
				goto SWIPE_AGAIN;
			}
			//EMV: Added error message handling "PLEASE SEE PHONE" - end -- jzg
			else if(ulAPRtn == d_EMVCL_TX_CANCEL)
			{
				vdDisplayErrorMsg(1, 8, "USER CANCEL");
				return USER_ABORT;
			}
			else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
			{
				inCTOSS_CLMCancelTransaction();
				vdDisplayErrorMsg(1, 8, "TIMEOUT");
				return USER_ABORT;
			}
			else if(ulAPRtn != d_EMVCL_PENDING)
			{
				vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
				break;
			}
		}
		/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */

	}




	/* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime -- jzg */
	if ((srTransRec.byEntryMode == CARD_ENTRY_WAVE) && (!fMaxCTLSAmt))
	{
		if(ulAPRtn != d_EMVCL_RC_DATA)
		{
			vdCTOSS_WaveCheckRtCode(ulAPRtn);
			inCTOSS_CLMCancelTransaction();
			return d_NO;
		}

		if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
		{
			inCTOSS_CLMCancelTransaction();
			return d_NO;
		}

		//Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
        	inCTOSS_CLMCancelTransaction();
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		inCTOSS_CLMCancelTransaction();
	}

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}

#endif

int inCTOS_PreConnect(void)
{
    int inRetVal, inReturn;
	BYTE szTitle[25+1];
	BYTE szDisMsg[40];
	BYTE szErrorMsg[40];


    vdDebug_LogPrintf("saturn --inCTOS_PreConnect--");

	//if (chGetInit_Connect() == TRUE)
	//		return(d_OK);

	if (strTCT.fDemo==TRUE)
		return(d_OK);

    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

	vdDebug_LogPrintf("saturn strTCT.byTerminalType[%d]",strTCT.byTerminalType);
	vdDebug_LogPrintf("ErmTrans_Approved[%d]",ErmTrans_Approved);
	vdDebug_LogPrintf("srTransRec.usTerminalCommunicationMode[%d]", srTransRec.usTerminalCommunicationMode);
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);

	vdDebug_LogPrintf("fDHCPEnable[%d]",strTCP.fDHCPEnable);
	vdDebug_LogPrintf("fShareComEnable[%d]",strTCT.fShareComEnable);
	vdDebug_LogPrintf("Primary[%s]port[%ld]",strCPT.szPriTxnHostIP,strCPT.inPriTxnHostPortNum);
	vdDebug_LogPrintf("Secondary[%s]port[%ld]",strCPT.szSecTxnHostIP,strCPT.inSecTxnHostPortNum);

    // Minimize the white screend display -- sidumili
	if ((isCheckTerminalMP200() == d_OK) && (ErmTrans_Approved))
		vdCTOS_DispStatusMessage("PROCESSING...");

    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
    {
        vdSetErrorMessage("COMM INIT ERR");
        return(d_NO);
    }

    vdDebug_LogPrintf("saturn --inCTOS_PreConnect2--");

    inReturn = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);
	vdDebug_LogPrintf("saturn inCTOS_CheckInitComm result %d", inReturn );

#if 1

	//if (srTransRec.usTerminalCommunicationMode == GPRS_MODE && !fGPRSConnectOK)
	//	inReturn = d_NO;

	vdDebug_LogPrintf("saturn inCTOS_CheckInitComm | inReturn[%d]|fGPRSConnectOK[%d]", inReturn, fGPRSConnectOK);

	if (inReturn != d_OK || !fGPRSConnectOK)
	{

		switch (srTransRec.usTerminalCommunicationMode )
		{
			case GPRS_MODE:
				//vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "GPRS PROBLEM", "PLEASE TRY AGAIN", MSG_TYPE_ERROR);
				vdSetErrorMessages("GPRS PROBLEM","PLEASE TRY AGAIN");
			    vdSetErrorMessage("");
				break;
			case WIFI_MODE:
				//vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "WIFI PROBLEM", "PLEASE TRY AGAIN", MSG_TYPE_ERROR);
				vdSetErrorMessages("WIFI PROBLEM","PLEASE TRY AGAIN");
			    vdSetErrorMessage("");
				break;
			case ETHERNET_MODE:
				//vdDisplayMultiLineMsgAlign("", "ETHERNET PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "ETHERNET PROBLEM", "PLEASE TRY AGAIN", MSG_TYPE_ERROR);
				vdSetErrorMessages("ETHERNET PROBLEM","PLEASE TRY AGAIN");
			    vdSetErrorMessage("");
				break;
			default:
				//vdDisplayMultiLineMsgAlign("", "COMM INIT", "ERROR", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "COMM INIT", "PLEASE TRY AGAIN", MSG_TYPE_ERROR);
				vdSetErrorMessages("COMM INIT","PLEASE TRY AGAIN");
			    vdSetErrorMessage("");
				break;

			//test
		}

		vdSetErrorMessage("");
		if(srTransRec.byTransType == SETTLE)
			vdDisplayErrorMsg2(1, 8, "SETTLE FAILED", " ", MSG_TYPE_ERROR);
		else
			vdDisplayErrorMsg2(1, 8, "", "CONNECT FAILED ", MSG_TYPE_ERROR);

		vdDebug_LogPrintf("saturn pre-connect return d_NO");

        if (fECRTxnFlg == TRUE)
        {
             memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
			 memset(srTransRec.szResponseText,0,sizeof(srTransRec.szResponseText));
             strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
			 strcpy(srTransRec.szResponseText, ECR_COMM_ERROR_RESP);
        }
		return d_NO;
	}
#else
      
	//if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE  || srTransRec.usTerminalCommunicationMode == WIFI_MODE) 
	//	&& !fGPRSConnectOK){

     if (inReturn != d_OK || !fGPRSConnectOK){

		memset(szErrorMsg, 0x00, sizeof(szErrorMsg));
	
		vdDebug_LogPrintf("saturn --inCTOS_PreConnect2 error ******************");
		if(srTransRec.byTransType == SETTLE)
			//vdDisplayMultiLineMsgAlign("", "", "SETTLE FAILED", DISPLAY_POSITION_CENTER);
			strcpy(szErrorMsg, "SETTLE FAILED");
		else{
			
			switch (srTransRec.usTerminalCommunicationMode )
			{
				case GPRS_MODE:
					//vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
					strcpy(szErrorMsg, "GPRS PROBLEM|PLESE TRY AGAIN");
					break;
				case WIFI_MODE:
					//vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
					strcpy(szErrorMsg, "WIFI PROBLEM|PLESE TRY AGAIN");
					break;
				case ETHERNET_MODE:
					//vdDisplayMultiLineMsgAlign("", "ETHERNET PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
					strcpy(szErrorMsg, "ETHERNET PROBLEM|PLESE TRY AGAIN");
					break;	
				
				default:
					strcpy(szErrorMsg, "COMM INIT|ERROR");
					break;
			}
		}

			
		memset(szDisMsg, 0x00, sizeof(szDisMsg));
		memset(szTitle, 0x00, sizeof(szTitle));
			
		szGetTransTitle(srTransRec.byTransType, szTitle);
			
			
        memset(szDisMsg, 0x00, sizeof(szDisMsg));
        strcpy(szDisMsg, szTitle);
    	strcat(szDisMsg, "|");
		strcat(szDisMsg, "");
		strcat(szDisMsg, "|");		
		strcat(szDisMsg, "");
		strcat(szDisMsg, "|");
    	strcat(szDisMsg, szErrorMsg);
    	usCTOSS_LCDDisplay(szDisMsg);           
        CTOS_Beep();
        CTOS_Delay(2000);

		vdSetErrorMessage("");

		return d_NO;

	}

#endif


    //vdDebug_LogPrintf("saturn --inCTOS_PreConnect3--");

    if (CN_FALSE == srTransRec.byOffline)
    {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
    }

	//vdDebug_LogPrintf("saturn --inCTOS_PreConnect4--");

    return(d_OK);
}



int inCTOS_CheckAndSelectMutipleMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;
    short shFalshMenu = 1;
     BYTE isUP = FALSE, isDOWN = FALSE;
	 BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT MERCHANT";
    char szMitMenu[1024];
    int inLoop = 0;

    unsigned char bstatus = 0;

    memset(szMitMenu, 0x00, sizeof(szMitMenu));
    vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID=[%d]",strHDT.inHostIndex);
    //get the index , then get all MID from the MMT list and prompt to user to select

    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfRecords);

	CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
	{
	    for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
	    {
            strcat((char *)szMitMenu, strMMT[inLoop].szMerchantName);
			if(inLoop + 1 != inNumOfRecords)
            	strcat((char *)szMitMenu, (char *)" \n");
	    }

	    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

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

	        vdDebug_LogPrintf("key[%d]-------", key);
			memcpy(&strMMT[0],&strMMT[key-1],sizeof(STRUCT_MMT));
	    }
	}
    srTransRec.MITid = strMMT[0].MITid;
    strcpy(srTransRec.szTID, strMMT[0].szTID);
    strcpy(srTransRec.szMID, strMMT[0].szMID);
    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    if(srTransRec.byTransType == BAL_INQ) // EXIT ON BAL INQ.
        return (SUCCESS);

    if(inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) == d_OK)
    {
        if(inGetMerchantPassword() != d_OK)
        	return d_NO;
    }

    vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);
	vdDebug_LogPrintf("MITid[%d] . szTID[%s] . szMID[%s] . szBatchNo[%s] . szHostLabel[%s]", srTransRec.MITid, srTransRec.szTID, srTransRec.szMID, srTransRec.szBatchNo, srTransRec.szHostLabel);

    return SUCCESS;
}



int inCTOS_CheckAndSelectMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;

    unsigned char key;
    unsigned char bstatus = 0;

    DebugAddSTR("inCTOS_CheckAndSelectMutipleMID","Processing...",20);

    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(srTransRec.HDTid,&inNumOfRecords);
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
    {
        shTotalPageNum = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (inNumOfRecords/ITEMS_PER_PAGE) :(inNumOfRecords/ITEMS_PER_PAGE + 1);
        shLastPageItems = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (ITEMS_PER_PAGE) : (inNumOfRecords%ITEMS_PER_PAGE);

        do
        {
                //display items perpage
                if(shTotalPageNum == 0)//the total item is amaller than ITEMS_PER_PAGE
                {
                    shPageItems = inNumOfRecords;
                }
                else if(shCurrentPageNum == shTotalPageNum)//Last page
                    shPageItems = shLastPageItems;
                else
                    shPageItems = ITEMS_PER_PAGE;

                CTOS_LCDTClearDisplay();
                for(shLoop=0 ; shLoop < shPageItems/*ITEMS_PER_PAGE*/ ; shLoop++)
                {

                    memset(szDisplay,0,sizeof(szDisplay));
                    sprintf(szDisplay,"%d: %s",shLoop+1,strMMT[shLoop + (shCurrentPageNum -1)*ITEMS_PER_PAGE].szMerchantName);
                    CTOS_LCDTPrintXY(2,shLoop + 2,szDisplay);

                }

                key=WaitKey(60);

                switch(key)
                {
                    case d_KBD_DOWN:

                        shCurrentPageNum ++;
                        if(shCurrentPageNum > shTotalPageNum)
                            shCurrentPageNum = 1;
                        bstatus = 2;
                        break;


                    case d_KBD_UP:

                        shCurrentPageNum --;
                        if(shCurrentPageNum < 1)
                            shCurrentPageNum = shTotalPageNum;
                        bstatus = 2;
                        break;

                    case d_KBD_CANCEL:
                        return FAIL;
                    case d_KBD_1:
                        //set the unique MMT num

                        memcpy(&strMMT[0],&strMMT[0 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        //vduiDisplayStringCenter(2,strMMT[0].szMID);
                        bstatus = 0;
                        break;
                    case d_KBD_2:
                        if(shPageItems < 2)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[1 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                    case d_KBD_3:
                        if(shPageItems < 3)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[2 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    case d_KBD_4:
                        if(shPageItems < 4)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[3 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                    case d_KBD_5:
                        if(ITEMS_PER_PAGE < 5)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 5)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[4 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_6:
                        if(ITEMS_PER_PAGE < 6)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 6)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[5 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_7:
                        if(ITEMS_PER_PAGE < 7)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 7)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[6 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_8:   //Max 8 items for one page
                        if(ITEMS_PER_PAGE < 8)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 8)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[7 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    default:
                        bstatus = -1 ;
                        break;

                }

                if((-1) == bstatus)
                {
                    return FAIL;

                }
                else if(0 == bstatus)
                {
                    break;
                }

            }while(1);

    }
    else
    {
        //One merchant only
        //vduiDisplayStringCenter(1,strMMT[0].szMID);

    }

    srTransRec.MITid = strMMT[0].MITid;
    return SUCCESS;


}

//Function Keys accepting 3 passwords Note: Occurs on all Function Keys

int inCTOS_GetTxnPassword(void)
{
     #define NO_PW           0
     #define SUPER_PW            1
     #define SYSTERM_PW          2
     #define ENGINEERPW         3
     #define MERCHANT_PW         4

	 BYTE szTitle[25+1];
	 BYTE szDisplay[128];
	 BYTE szMsg[100];
	 BYTE szDisMsg[100];

     unsigned char szOutput[30], szPassword[40+1];
     int  inTxnTypeID;
     short ret = 0;
     short shMinLen;
     short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
     BYTE key;
     BYTE Bret;
     short shCount =0;
     short shRsesult = d_OK;
     // DWORD dwWait=0, dwWakeup=0;

	 DebugAddSTR("inCTOS_GetTxnPassword","Processing...",20);
	 
     if (inMultiAP_CheckSubAPStatus() == d_OK)
          return d_OK;

	 memset(szTitle, 0x00, sizeof(szTitle));
	 szGetTransTitle(srTransRec.byTransType, szTitle);
	 
	 memset(szOutput,0,sizeof(szOutput));
	 memset(szDisplay,0,sizeof(szDisplay));
	 memset(szPassword,0,sizeof(szPassword));
	 memset(szMsg,0,sizeof(szMsg));
	 
	 strcpy(szMsg, "4");
	 strcat(szMsg, "|");
	 strcat(szMsg, "6");
	 strcat(szMsg, "|");
	 strcat(szMsg, szTitle);
	 strcat(szMsg, "|");
	 strcat(szMsg, "ENTER PASSWORD");
	 

     inTxnTypeID = srTransRec.byTransType;

     //vduiLightOn();

     inPITRead(inTxnTypeID);
     inTCTRead(1);

	 vdDebug_LogPrintf("inCTOS_GetTxnPassword:::byTransType=[%d]", srTransRec.byTransType);
	 vdDebug_LogPrintf("inCTOS_GetTxnPassword:::inPasswordLevel=[%d]", strPIT.inPasswordLevel);
	 
     if(fGetECRTransactionFlg() == TRUE)
          return d_OK;

     if (NO_PW== strPIT.inPasswordLevel)
     {
          return d_OK;
     }

     //vdDispTransTitle(srTransRec.byTransType);

     CTOS_KBDHit(&key);//clear key buffer
     while(shCount < 3)
     {
     switch(srTransRec.byTransType)
     {
          case SALE:
          case PRE_AUTH:
          case REFUND:
          case SALE_OFFLINE:
          case EPP_SALE:
          case VOID:
          case SALE_TIP:
          case SALE_ADJUST:
          case SETTLE:
          case BATCH_REVIEW:
          case BATCH_TOTAL:
          case CITAS_SERVICES:
          case SETUP:
          case REPRINT_ANY:
          case MERCHANT_SERVICES:
		  case CTMS_UPDATE:
          if(SUPER_PW == strPIT.inPasswordLevel)
          {
          shMinLen = PWD_MIN_LEN;
          shMaxLen = PWD_MAX_LEN;

          //CTOS_LCDTClearDisplay();
          //vdDispTransTitle(srTransRec.byTransType);
          //CTOS_LCDTPrintXY(1, 7,"SUPER PASSWORD:");


          //dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
          //if ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR)

          //Bret = InputString(1, 8, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

		  //strcpy(szPassword,strTCT.szSuperPW);
		  //memset(szOutput,0,sizeof(szOutput));
		  //Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword, "ENTER PASSWORD");

		  strcpy(szPassword,szMsg);
		  strcat(szPassword, "|");
		  strcat(szPassword,strTCT.szSuperPW);
		  memset(szOutput,0,sizeof(szOutput));
		  Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword);
		  vdDebug_LogPrintf("2. SUPER_PW szOutput[%s], Bret[%d]", szOutput, Bret);

          //CTOS_PrinterPutString(szOutput);
          //CTOS_PrinterPutString(strTCT.szSuperPW);

		  // Return char 'C' - 67 for decimal -- sidumili
		  if(Bret == 67 || 0 == strcmp(szOutput, "CANCEL"))
		  {
			   vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			   CTOS_Beep();
			   CTOS_Delay(1000);
			   return d_NO;
		  }
		  
          if(d_KBD_CANCEL == Bret)
               return Bret;
          else if(Bret == 255)
          	return Bret;		  
          else if(strcmp(szOutput,strTCT.szSuperPW) == 0)
          {
               //clearLine(7);
               //clearLine(8);
               return d_OK;
          }
          else
          {
               //CTOS_LCDTClearDisplay();
               vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
               //vdClearBelowLine(8);
               shRsesult = FAIL;
               break;
          }
          }
          else if(SYSTERM_PW== strPIT.inPasswordLevel)
          {
               shMinLen = PWD_MIN_LEN;
               shMaxLen = PWD_MAX_LEN;

               //CTOS_LCDTClearDisplay();
               //vdDispTransTitle(srTransRec.byTransType);
               //CTOS_LCDTPrintXY(1, 7,"SYSTEM PASSWORD:");
			   //strcpy(szPassword, strTCT.szSystemPW);
			   //memset(szOutput,0,sizeof(szOutput));

			   //Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword, "ENTER PASSWORD");

               //CTOS_PrinterPutString(szOutput);
               //CTOS_PrinterPutString(strTCT.szSuperPW);
               //CTOS_PrinterPutString(strTCT.szSystemPW);
               //CTOS_PrinterPutString(strTCT.szEngineerPW);
               
			   strcpy(szPassword,szMsg);
			   strcat(szPassword, "|");
			   strcat(szPassword, strTCT.szSystemPW);
			   memset(szOutput,0,sizeof(szOutput));
			   Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword);
			   vdDebug_LogPrintf("2. SYSTERM_PW szOutput[%s], Bret[%d]", szOutput, Bret);

			   // Return char 'C' - 67 for decimal -- sidumili
			   if(Bret == 67 || 0 == strcmp(szOutput, "CANCEL"))
			   {
				   	vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
					CTOS_Beep();
					CTOS_Delay(1000);
					return d_NO;
			   }
			   
               if(d_KBD_CANCEL == Bret)
                    return Bret;
               else if(Bret == 255)
                    return Bret;			   
               else if((strcmp(szOutput,strTCT.szSuperPW) == 0) ||
               (strcmp(szOutput,strTCT.szSystemPW) == 0)||
               (strcmp(szOutput,strTCT.szEngineerPW) == 0) )
               {
                    //clearLine(7);
                    //clearLine(8);
                    return d_OK;
               }
               else
               {
                    //CTOS_LCDTClearDisplay();
                    vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
                    //vdClearBelowLine(8);
                    shRsesult = FAIL;
                    break;
               }
          }
          else if(ENGINEERPW== strPIT.inPasswordLevel)
          {
               shMinLen = PWD_MIN_LEN;
               shMaxLen = PWD_MAX_LEN;

               //CTOS_LCDTClearDisplay();
               //vdDispTransTitle(srTransRec.byTransType);
               //CTOS_LCDTPrintXY(1, 7,"PASSWORD:");

			   //Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword, "ENTER PASSWORD");
			   strcpy(szPassword,szMsg);
			   strcat(szPassword, "|");
			   strcat(szPassword, strTCT.szEngineerPW);
			   memset(szOutput,0,sizeof(szOutput));
			   Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword);
			   vdDebug_LogPrintf("3. ENGINEERPW szOutput[%s], Bret[%d]", szOutput, Bret);

               //CTOS_PrinterPutString(szOutput);
               //CTOS_PrinterPutString(strTCT.szEngineerPW);

               // Return char 'C' - 67 for decimal -- sidumili
			   if(Bret == 67 || 0 == strcmp(szOutput, "CANCEL"))
			   {
				   	vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
					CTOS_Beep();
					CTOS_Delay(1000);
					return d_NO;
			   }

               if(d_KBD_CANCEL == Bret)
                    return Bret;
               else if(Bret == 255)
                    return Bret;			  
               else if(strcmp(szOutput,strTCT.szEngineerPW) == 0)
               {
                    //clearLine(7);
                    //clearLine(8);
                    return d_OK;
               }
               else
               {
                    //CTOS_LCDTClearDisplay();
                    vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
                    vdClearBelowLine(8);
                    shRsesult = FAIL;
                    break;
               }

          }
          else if(MERCHANT_PW== strPIT.inPasswordLevel)
          {
               shMinLen = PWD_MIN_LEN;
               shMaxLen = PWD_MAX_LEN;

               //CTOS_LCDTClearDisplay();
               //vdDispTransTitle(srTransRec.byTransType);
               //CTOS_LCDTPrintXY(1, 7,"MERCHANT PASSWORD:");

               //Bret = InputString(1, 8, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
			   //Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword, "ENTER PASSWORD");
			   
			   //Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword, "ENTER PASSWORD");
			   strcpy(szPassword,szMsg);
			   strcat(szPassword, "|");
			   strcat(szPassword, strTCT.szMerchantPW);
			   memset(szOutput,0,sizeof(szOutput));
			   Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szPassword);
			   vdDebug_LogPrintf("4. MERCHANT_PW szOutput[%s], Bret[%d]", szOutput, Bret);
			   
               //CTOS_PrinterPutString(szOutput);
               //CTOS_PrinterPutString(strTCT.szSuperPW);
               //CTOS_PrinterPutString(strTCT.szSystemPW);
               //CTOS_PrinterPutString(strTCT.szMerchantPW);

			   // Return char 'C' - 67 for decimal -- sidumili
			   if(Bret == 67 || 0 == strcmp(szOutput, "CANCEL"))
			   {
				   	vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
					CTOS_Beep();
					CTOS_Delay(1000);
					return d_NO;
			   }
			   
               if(d_KBD_CANCEL == Bret)
                    return Bret;
               else if(Bret == 255)
                    return Bret;			   
               else if (strcmp(szOutput,strTCT.szMerchantPW) == 0)
               {
                    //clearLine(7);
                    //clearLine(8);
                    return d_OK;
               }
               else
               {
                    //CTOS_LCDTClearDisplay();
                    vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
                    //vdClearBelowLine(8);
                    shRsesult = FAIL;
                    break;
               }
          }
          else
          {
          //clearLine(7);
          //clearLine(8);
          return d_OK;
          }


          default:

          return d_OK;

     }
     if(FAIL == shRsesult)
          shCount ++ ;
     }

     return shRsesult;

}




int inCTOS_SelectHost(void)
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;

	vdDebug_LogPrintf("saturn --inCTOS_SelectHost--");

	inDatabase_TerminalOpenDatabase();
	
    inHostIndex = (short) strCDT.HDTid;

   vdDebug_LogPrintf("saturn inHostIndex[%d]", inHostIndex);
   
   inGlobalCount = 0; // fix for 1. Performed sale transaction 2. Prints DE 63 first: Parsing not in order
   
    vdDebug_LogPrintf("saturn inCTOS_SelectHost =[%d]",inHostIndex);

     if (inMultiAP_CheckSubAPStatus() == d_OK)
     {
     	inDatabase_TerminalCloseDatabase();
        return d_OK;
     }
     
    if ( inHDTReadEx(inHostIndex) != d_OK)
    {
        inDatabase_TerminalCloseDatabase();
        vdSetErrorMessage("HOST SELECTION ERR");
        return(d_NO);
    }
    else
    {

        srTransRec.HDTid = inHostIndex;

		vdDebug_LogPrintf("saturn srTransRec.HDTid[%d]", srTransRec.HDTid);

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTReadEx(inCurrencyIdx) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }


        if ( inCPTReadEx(inHostIndex) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }
        //inDatabase_TerminalCloseDatabase();
        //return (d_OK);
    }
	inDatabase_TerminalCloseDatabase();
    return (d_OK);
}
#if 1
int inCTOS_getCardCVV2(BYTE *baBuf)
{
    BYTE    szMonth[3];
    USHORT  usRet;
    USHORT  usLens;
    USHORT  usMinLen = 4;
    USHORT  usMaxLen = 4;
    USHORT usInputLine = 8;
	BYTE szTemp[5];
	BYTE szExpYear[3]={0};
    BYTE szExpMonth[3]={0};
	BYTE szExpiryDate[5]={0};

	BYTE szTitle[25+1];
    BYTE szDisplay[512];

	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	vdDebug_LogPrintf("srTransRec.szPAN");
    vdDebug_LogPrintf(srTransRec.szPAN);
    vdDebug_LogPrintf("szTitle");
    vdDebug_LogPrintf(szTitle);

	wub_hex_2_str(srTransRec.szExpireDate, szTemp, 2);
	memcpy(szExpYear,&szTemp[0],2);
    memcpy(szExpMonth,&szTemp[2],2);

	sprintf(szExpiryDate, "%s/%s", szExpMonth, szExpYear);

    memset(szDisplay, 0x00, sizeof(szDisplay));

	strcpy(szDisplay, "0");
	strcat(szDisplay, "|");
    strcat(szDisplay, "4");
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, srTransRec.szPAN);
    strcat(szDisplay, "|");
    strcat(szDisplay, szExpiryDate);
    strcat(szDisplay, "|");
    strcat(szDisplay, "CVV2: ");

    while(1)
    {
        //usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        usRet = InputCVVUI(baBuf, &usLens, usMinLen, d_INPUT_TIMEOUT, szDisplay);
        if (usRet == d_KBD_CANCEL ){
    		vdDebug_LogPrintf("*************************");
  	   		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//clear buffer on idle swipe on term reg.

            return (d_EDM_USER_CANCEL);
        }
		else if (usRet == 0xFF) {
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(2000);
			return TIME_OUT;
		}

        /*   TINE:  Input validation handled by android UI
        memset(szMonth, 0x00, sizeof(szMonth));
        memcpy(szMonth, baBuf, 2);
        if(atol(szMonth) > 12 || 0 == atol(szMonth))
        {
            baBuf[0] = 0x00;
            clearLine(14); // Error message "INVALID FORMAT" must be cleared after entering wrong date format
            vdDisplayErrorMsg(1, 8, "INVALID FORMAT");
            clearLine(8);
            clearLine(14); //Error message "INVALID FORMAT" must be cleared after entering wrong date format
            continue;
        }
        else
        {
            return (d_OK);
        }
        */

        return (d_OK);
    }
}

#else
int inCTOS_getCardCVV2(BYTE *baBuf)
{
    USHORT usRet;
    USHORT usInputLen;
    USHORT usLens;
    //USHORT usMinLen = 3;
    //USHORT usMaxLen = 6;
    USHORT usMaxLen = 4;
    USHORT usMinLen = 1;

    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
    {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NUMBER: ");
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr+3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);

        usInputLen = 7;
    }
    else
    {
        inCTOS_DisplayCardTitle(4, 5, FALSE);
        usInputLen = 7;
    }

    setLCDPrint(usInputLen, DISPLAY_POSITION_LEFT, "CVV2: ");

    while(1)
    {
        usRet = shCTOS_GetNum(usInputLen+1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, d_INPUT_TIMEOUT); // #00242 - usByPassAllow = 0 no bypass.
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen)
        {
            return (d_OK);
        }
        else if(0 == usRet)
        {
            return (d_OK);
        }

        baBuf[0] = 0x00;
    }
}
#endif

int inCTOS_GetCVV2()
{
    BYTE szCVV2Code[CVV2_SIZE + 1];
    int inResult = FAIL;
    BYTE key;
    short shCount = 0;
    TRANS_DATA_TABLE* srTransPara;

    DebugAddSTR("inCTOS_GetCVV2","Processing...",20);

    memset(srTransRec.szCVV2 , 0x00, sizeof(srTransRec.szCVV2));
    memset(szCVV2Code , 0x00, sizeof(szCVV2Code));

    vdDebug_LogPrintf("inCTOS_GetCVV2:[%d]:[%d]",srTransRec.CDTid2, strCDT.inCVV_II);

    inCDTRead(srTransRec.CDTid2);//#00234 - Incorrect CVV format DE 63

    vdDebug_LogPrintf("inCTOS_GetCVV2:[%d]:[%d]",srTransRec.CDTid2, strCDT.inCVV_II);

    if(CVV2_NONE == strCDT.inCVV_II)
    {
        return d_OK;;
    }

    while(shCount < 3)
    {

        if(((CVV2_MANUAL == strCDT.inCVV_II) &&( CARD_ENTRY_MANUAL == srTransRec.byEntryMode))
        || ((CVV2_MSR == strCDT.inCVV_II) &&(( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode)))
        || ((CVV2_MANUAL_MSR == strCDT.inCVV_II) &&(( CARD_ENTRY_MANUAL == srTransRec.byEntryMode) ||( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode)))
        || ((CVV2_MANUAL_MSR_CHIP == strCDT.inCVV_II) &&(( CARD_ENTRY_MANUAL == srTransRec.byEntryMode) ||( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode) || ( CARD_ENTRY_ICC == srTransRec.byEntryMode))))
        {
            //CTOS_KBDBufFlush();

            inResult = inCTOS_getCardCVV2(szCVV2Code);
            if(d_OK == inResult)
            {
                strcpy(srTransRec.szCVV2,szCVV2Code);
                return d_OK;;
            }
            else
            {
                if(d_EDM_USER_CANCEL == inResult)
                {
                    vdSetErrorMessage("USER CANCEL");
                    return inResult;
                }

                memset(szCVV2Code , 0x00, sizeof(szCVV2Code));
                vdDisplayErrorMsg(1, 8, "INVALID CVV");

                break;
            }
        }
        else
        {
            return d_OK;;
        }

        shCount ++ ;
    }
    vdSetErrorMessage("Get CVV ERR");
    return FAIL;

}

int inCTOS_EMVSetTransType(BYTE byTransType)
{
    switch(byTransType)
    {
        case REFUND:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x20");
        break;

        case PRE_AUTH:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x30");
        break;

        default:
        break;
    }
}

void vdCTOSS_EMV_SetAmount(void)
{
	BYTE szBaseAmount[20];
	BYTE szTipAmount[20];
	BYTE szTotalAmount[20];
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	BYTE  byDataTmp1[32];
	BYTE  byDataTmp2[32];
	BYTE  bPackSendBuf[2560];
	USHORT usPackSendLen = 0;
	USHORT ushEMVtagLen;
	ULONG lnTmp;
	OFFLINEPINDISPLAY_REC szDisplayRec;

	vdDebug_LogPrintf("--vdCTOSS_EMV_SetAmount--");

	memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
	memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
	wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
	lnTmp = atol(byDataTmp1);
	wub_long_2_array(lnTmp, byDataTmp2);

	inCTOSS_PutEnvDB ("MCCAMOUT", byDataTmp1);
	inCTOSS_PutEnvDB ("MCCCURS", strCST.szCurSymbol);


	memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
	memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
	wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
	lnTmp = atol(byDataTmp1);
	wub_long_2_array(lnTmp, byDataTmp2);

	memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
	bPackSendBuf[usPackSendLen++] = 0x04;
	memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
	usPackSendLen += 4;

	memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);

	memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
	usPackSendLen += 2;
	bPackSendBuf[usPackSendLen++] = 0x06;
	memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
	usPackSendLen += 6;

	if(atol(szTipAmount) > 0)
	{
		memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
	}
	else
	{
		memset(szTipAmount, 0x00, sizeof(szTipAmount));
		memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
	}

	memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
	usPackSendLen += 2;
	bPackSendBuf[usPackSendLen++] = 0x06;
	memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
	usPackSendLen += 6;

	//usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);


	//for offline pin display
	memcpy(&bPackSendBuf[usPackSendLen], "\xFF\xFE", 2);
	usPackSendLen += 2;
	//bPackSendBuf[usPackSendLen++] = 0x06;
	memset(&szDisplayRec,0x00,sizeof(OFFLINEPINDISPLAY_REC));
	vdCTOSS_PackOfflinepinDisplay(&szDisplayRec);
	memcpy(&bPackSendBuf[usPackSendLen], &szDisplayRec, sizeof(OFFLINEPINDISPLAY_REC));
	usPackSendLen += sizeof(OFFLINEPINDISPLAY_REC);

	//vdPCIDebug_HexPrintf("EMV_SetAmount",bPackSendBuf,usPackSendLen);
	usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);

}


int inCTOS_EMVProcessing()
{
    int inRet;
    BYTE   EMVtagVal[64];
    BYTE szTitle[25+1];
    BYTE szDisMsg[100];

	vdDebug_LogPrintf("--inCTOS_EMVProcessing--");
	vdDebug_LogPrintf("byEntryMode[%d]", srTransRec.byEntryMode);

	//test 
	//return d_OK;

    if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
		vdCTOSS_EMV_SetAmount();

		// No pin entry on installment transaction -- sidumili
		if (srTransRec.fInstallment == TRUE)
		{
			ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,"\xE0\x20\xC8");
			srTransRec.fEMVPINEntered = TRUE;
		}

        if (inCTOSS_CheckCVMAmount()== d_OK)
        {
             ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,"\xE0\x08\xC8");
        }
		
        inRet = usCTOSS_EMV_TxnPerform();
		
		vdDebug_LogPrintf("saturn after emvtxn perform cdtid %d", srTransRec.CDTid);

        inCTOS_FirstGenACGetAndSaveEMVData();
        inCTOS_EMVSetTransType(srTransRec.byTransType);
        EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

        switch( EMVtagVal[0] & 0xC0)
        {
            case 0: //Declined --- AAC
                strcpy(srTransRec.szAuthCode,"Z1");
                //vdSetErrorMessage("EMV Decline");
               if(gblinNoKeys ==0)
               {
                   /* memset(szDisMsg, 0x00, sizeof(szDisMsg));
                    memset(szTitle, 0x00, sizeof(szTitle));
                    szGetTransTitle(srTransRec.byTransType, szTitle);
                    strcpy(szDisMsg, szTitle);
                    strcat(szDisMsg, "|");
                    strcat(szDisMsg, "EMV Decline");
                    usCTOSS_LCDDisplay(szDisMsg);*/
                     vdDisplayErrorMsg(1, 8, "EMV Decline");
                    CTOS_Beep();
		
               }
                vdDebug_LogPrintf("1st ACs, card dec");

                return EMV_CRITICAL_ERROR;

            case 0x40: //Approval --- TC
                //strcpy(srTransRec.szAuthCode,"Y1");
                //srTransRec.shTransResult = TRANS_AUTHORIZED;
                //vdDebug_LogPrintf("1nd AC app");
                //break;
                vdDisplayErrorMsg(1, 8, "EMV Decline");
                CTOS_Beep();
				return EMV_CRITICAL_ERROR;
				
            case 0x80: //ARQC
                vdDebug_LogPrintf("go online");
                break;

            default:
                strcpy(srTransRec.szAuthCode,"Z1");
                //vdSetErrorMessage("EMV Decline");
               /* memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "EMV Decline");
                usCTOSS_LCDDisplay(szDisMsg);*/
                vdDisplayErrorMsg(1, 8, "EMV Decline");
                CTOS_Beep();
                return EMV_CRITICAL_ERROR;

        }

        vdDebug_LogPrintf("usCTOSS_EMV_TxnPerform return[%d]", inRet);
        if (inRet != d_OK)
            vdSetErrorMessage("First GenAC ERR");
        return inRet;

    }

    return d_OK;
}

int inCTOS_CheckTipAllowd()
{

    if(SALE_TIP == srTransRec.byTransType)
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;
    }

    if (TRUE !=strHDT.fTipAllowFlag)
    {
        if (SALE_TIP == srTransRec.byTransType)
            vdSetErrorMessage("TIP NOT ALLWD");

        return d_NO;
    }

    return d_OK;
}



SHORT shCTOS_EMVAppSelectedProcess(void)
{
	short shResult;
	BYTE SelectedAID[16];
	USHORT SelectedAIDLen = 0;
	BYTE label[32];
	USHORT label_len = 0;
	BYTE PreferAID[128];
	CTOS_RTC SetRTC;

         BYTE szTitle[25+1];
         BYTE szDisplay[100];
         BYTE szDisMsg[100];

	vdDebug_LogPrintf("saturn -------EMV_TxnAppSelect-----");
	memset(PreferAID,0,sizeof(PreferAID));
	memset(label,0,sizeof(label));

	shResult = usCTOSS_EMV_TxnAppSelect(PreferAID, 0, SelectedAID, &SelectedAIDLen, label, &label_len);
	vdDebug_LogPrintf("saturn -EMV_TxnAppSelect=[%d] SelectedAIDLen[%d] label[%s]fback[%d]",shResult, SelectedAIDLen, label,strEMVT.inEMVFallbackAllowed);

	if(d_OK == shResult)
		DebugAddHEX("SelectedAIDLen", SelectedAID, SelectedAIDLen);

	if(EMV_READ_FAILED == shResult)
    {
         vdDisplayMessageBox(1, 8, "", "READ CARD FAILED", "", MSG_TYPE_WARNING);
         CTOS_Beep();
         return EMV_TRANS_FAILED;
    }

 	
	if((shResult != PP_OK) && (shResult != EMV_USER_ABORT))
	{
		CTOS_LCDTClearDisplay();

		//EMV: should display "CHIP NOT DETECTED" instead of doing fallback - start -- jzg
		if (shResult == EMV_CHIP_NOT_DETECTED)
		{
			//vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
			vdDisplayMessageBox(1, 8, "", "CHIP NOT DETECTED", "", MSG_TYPE_WARNING);
                            CTOS_Beep();
			CTOS_Delay(1500);
			return EMV_CHIP_FAILED;
		}
		//EMV: should display "CHIP NOT DETECTED" instead of doing fallback - end -- jzg

		//EMV: If AID not found display "TRANS NOT ALLOWED" - start -- jzg
		if (shResult == EMV_TRANS_NOT_ALLOWED)
		{
		  //0424
			//vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
			vduiClearBelow(2);
			//vdDisplayErrorMsg(1, 8, "PLEASE SWIPE CARD");
			//vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
			vdDisplayErrorMsg(1, 8, "APPL NOT AVAILABLE");
			//strTCT.inFallbackCounter = 1;
			//inFallbackToMSR = SUCCESS;
			return EMV_TRANS_NOT_ALLOWED;
			//0424
		}
		//EMV: If AID not found display "TRANS NOT ALLOWED" - end -- jzg

		//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - start -- jzg
		if (shResult == EMV_CARD_BLOCKED)
		{
			inFallbackToMSR = FAIL;
			vdDisplayErrorMsg(1, 8, "CARD BLOCKED");
			return EMV_CHIP_FAILED;
		}
		//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - end -- jzg

		// BANCNET - T007- TIC024 - APPLICATION BLOCKED -- sidumili
		if (shResult == EMV_APP_BLOCKED)
		{
			inFallbackToMSR = FAIL;
			vdDisplayErrorMsg(1, 8, "APPLICATION BLOCKED");
			return EMV_CHIP_FAILED;
		}
		// BANCNET - T007- TIC024 - APPLICATION BLOCKED -- sidumili

                   // AMEX - AXP EMV 018 - APPLICATION BLOCKED -- AAA
		if (shResult == EMV_APP_BLOCKED_AMEX)
		{
			inFallbackToMSR = FAIL;
			vdDisplayErrorMsg(1, 8, "APPLICATION BLOCKED");
			return EMV_CHIP_FAILED;
		}


		if(EMV_FALLBACK == shResult)
		{
			//0826
			//vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
			vdDebug_LogPrintf("AAA - Chip not detected");
                            /*memset(szDisMsg, 0x00, sizeof(szDisMsg));
                            strcpy(szDisMsg, " ");
                            strcat(szDisMsg, "|");
                            strcat(szDisMsg, "CHIP NOT DETECTED");
                            usCTOSS_LCDDisplay(szDisMsg);*/
                            vdDisplayMessageBox(1, 8, "", "CHIP NOT DETECTED", "", MSG_TYPE_WARNING);
                            CTOS_Beep();
                            CTOS_Delay(1500);
			//vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
			//0826

			CTOS_RTCGet(&SetRTC);
			inFallbackToMSR = SUCCESS;
			sprintf(strTCT.szFallbackTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
			vdDebug_LogPrintf("AAA - strTCT.szFallbackTime [%s]", strTCT.szFallbackTime);
		}
		else if (EMV_FAILURE_EX == shResult){
                            vdDebug_LogPrintf("AAA READ CARD FAILED 1");
			vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
			return EMV_FAILURE_EX;

		}
		else
		{
		         vdDebug_LogPrintf("AAA READ CARD FAILED 2");
			vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
		}

		return EMV_CHIP_FAILED;
	}

	if(shResult == EMV_USER_ABORT)
	{


		if(strEMVT.inEMVFallbackAllowed)
		{
			//0826
			//vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
			//vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
			vdDisplayMessageBox(1, 8, "", "CHIP NOT DETECTED", "", MSG_TYPE_WARNING);
                            CTOS_Beep();
			CTOS_Delay(1500);
			
			//0826

			CTOS_RTCGet(&SetRTC);
			inFallbackToMSR = SUCCESS;
			sprintf(strTCT.szFallbackTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
		}
		else
		{
		          vdDebug_LogPrintf("AAA READ CARD FAILED 3");
			vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
		}


		return EMV_USER_ABORT;
	}

	return d_OK;

}


short shCTOS_EMVSecondGenAC(BYTE *szIssuerScript, UINT inIssuerScriptlen)
{
#define ACT_ONL_APPR 1
#define ACT_ONL_DENY 2
#define ACT_UNAB_ONL 3
#define ACT_ONL_ISSUER_REFERRAL 4           //From Host
#define ACT_ONL_ISSUER_REFERRAL_APPR 4
#define ACT_ONL_ISSUER_REFERRAL_DENY 5
BYTE szTitle[20+1];
BYTE szDisMsg[100];


    USHORT usResult;
    EMV_ONLINE_RESPONSE_DATA st2ACResponseData;
    BYTE   EMVtagVal[64];

    memset(&st2ACResponseData,0,sizeof(st2ACResponseData));

    //st2ACData.iAction will decide trans approve or not
    if(srTransRec.shTransResult == TRANS_COMM_ERROR)
        st2ACResponseData.bAction = ACT_UNAB_ONL;
    else if(srTransRec.shTransResult == TRANS_REJECTED)
        st2ACResponseData.bAction = ACT_ONL_DENY;
    else if(srTransRec.shTransResult == TRANS_CALL_BANK)
        st2ACResponseData.bAction = ACT_ONL_ISSUER_REFERRAL;
    else if(srTransRec.shTransResult == TRANS_AUTHORIZED)
        st2ACResponseData.bAction = ACT_ONL_APPR;
    //Fix for Issue Script data
    //memset(szIssuerScript,0,sizeof(szIssuerScript));
    st2ACResponseData.pAuthorizationCode = srTransRec.szRespCode;
    st2ACResponseData.pIssuerAuthenticationData = srTransRec.stEMVinfo.T91;
    st2ACResponseData.IssuerAuthenticationDataLen = srTransRec.stEMVinfo.T91Len;
    st2ACResponseData.pIssuerScript = szIssuerScript;
    st2ACResponseData.IssuerScriptLen = inIssuerScriptlen;

    usResult = TRANS_AUTHORIZED;

    DebugAddHEX("shCTOS_EMVSecondGenAC ", st2ACResponseData.pIssuerScript, st2ACResponseData.IssuerScriptLen);

	// No pin entry on installment transaction -- sidumili
	if (srTransRec.fInstallment == TRUE)
	{
		ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,"\xE0\x20\xC8");
		srTransRec.fEMVPINEntered = TRUE;
	}

	if (inCTOSS_CheckCVMAmount()== d_OK)
	{
		ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,"\xE0\x08\xC8");
	}
	
    usResult = usCTOSS_EMV_TxnCompletion(&st2ACResponseData);

    vdDebug_LogPrintf("PP_iCompletion:%d ", usResult);

    inCTOS_SecondGenACGetAndSaveEMVData();

    if(VS_TRUE == strTCT.fDemo)
    {
        usResult = PP_OK;
        EMVtagVal[0] = 0x40;
    }
    if(usResult != PP_OK)
    {
        if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)
        	{
            //vdSetErrorMessage("EMV Decline");
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "EMV Decline");
                usCTOSS_LCDDisplay(szDisMsg);
                CTOS_Beep();
        	}
        return EMV_CRITICAL_ERROR;
    }

    EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

    switch( EMVtagVal[0] & 0xC0)
    {
        case 0: //Declined --- AAC
            if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)  //approve by host, but not by card
            {
                //vdSetErrorMessage("EMV Decline");
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "EMV Decline");
                usCTOSS_LCDDisplay(szDisMsg);
                CTOS_Beep();
                vdDebug_LogPrintf("Host app, card dec");
            }
            else
            {
                vdDebug_LogPrintf("Host reject");
            }
            return EMV_CRITICAL_ERROR;

        case 0x40: //Approval --- TC
            vdDebug_LogPrintf("2nd AC app");
            break;

        default:
            //vdSetErrorMessage("EMV Decline");
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "EMV Decline");
            usCTOSS_LCDDisplay(szDisMsg);
            CTOS_Beep();
            return EMV_CRITICAL_ERROR;

    }

    vdDebug_LogPrintf("End 2nd GenAC shTransResult=%d iAction=%d",srTransRec.shTransResult, st2ACResponseData.bAction);
    return PP_OK;

}

// called in inCTOS_WaveFlowProcess
int inCTOS_EMVTCUpload(void)
{
    int inRet;

    //if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    if((CARD_ENTRY_ICC == srTransRec.byEntryMode)|| (CARD_ENTRY_WAVE == srTransRec.byEntryMode) ) // mfl
    {
        inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
        inRet = inProcessEMVTCUpload(&srTransRec, -1);// TC upload

            vdDebug_LogPrintf("szFileName, %s%02d%02d.rev"
                                , strHDT.szHostLabel
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
        inCTOS_inDisconnect();
    }

    return d_OK;
}


int inCTOS_GetPubKey(const char *filename, unsigned char *modulus, int *mod_len, unsigned char *exponent, int *exp_len)
{
    unsigned char tmp[1024];
    int iRead;
    int iMod;
    int iExp;
    FILE  *fPubKey;
    UINT uintRet ;

    fPubKey = fopen( (char*)filename, "rb" );
    if (fPubKey == NULL)
        return CTOS_RET_PARAM;

    uintRet = fread( tmp, 1, sizeof(tmp), fPubKey );
    fclose(fPubKey);
    vdDebug_LogPrintf("CAPK=Len[%d]==[%s]",uintRet,tmp);

    if(uintRet >0)
    {
        iMod=(int)(tmp[0]-0x30)*100+(tmp[1]-0x30)*10+(tmp[2]-0x30);
        vdDebug_LogPrintf("iMod===[%d]",iMod);
        if(iMod%8 != 0)
            return(CTOS_RET_PARAM);

        if(iMod > CTOS_PED_RSA_MAX)
            return(CTOS_RET_PARAM);

        *mod_len=iMod;
        wub_str_2_hex((tmp+3), (modulus), iMod*2);

        vdDebug_LogPrintf("*mod_len===[%d]",*mod_len);
        DebugAddHEX("Module HEX string===", modulus, iMod);


        iExp=(int)tmp[iMod*2+4] - 0x30;
        wub_str_2_hex((&tmp[5+(iMod*2)]), (exponent), iExp*2);

        vdDebug_LogPrintf("iExp===[%d]",iExp);
        DebugAddHEX("Exponent HEX string===", exponent, iExp);

        *exp_len = iExp;
    }
    else
        return(CTOS_RET_CALC_FAILED);

    return(CTOS_RET_OK);
}

USHORT ushCTOS_EMV_NewDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue)
{
    USHORT usResult;
    USHORT usTagLen;
    static USHORT usGetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataGet(usTag, &usTagLen, pValue);
    *pLen = usTagLen;

    usGetEMVTimes ++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewDataGet Times[%d] usTagLen[%d]",usGetEMVTimes, usTagLen);
    return usResult;

}

USHORT ushCTOS_EMV_NewTxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue)
{
    USHORT usResult;
    static USHORT usSetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataSet(usTag, usLen, pValue);
    usSetEMVTimes ++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewTxnDataSet Times[%d] usResult[%d]",usSetEMVTimes, usResult);

    return usResult;
}

short shCTOS_EMVGetChipDataReady(void)
{
    short       shResult;
    BYTE        byDataTmp1[64];
    BYTE        byVal[64];
    USHORT      usLen;
    USHORT      inIndex ;
    BYTE        szDataTmp[5];
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    char chTemp;
    USHORT inTagLen = 0;
    BYTE szAID[16] = {0};

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szOutEMVData,0,sizeof(szOutEMVData));

	vdDebug_LogPrintf("saturn before emv multidataget");

    //for improve transaction speed
    //shResult = usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_SELECT_APP, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_SELECT_APP",szOutEMVData,inTagLen);

	
	vdDebug_LogPrintf("saturn after emv multidataget");

    shResult = usCTOSS_FindTagFromDataPackage(TAG_57, byVal, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn -------TAG_57[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, byVal[0], byVal[1], byVal[2]);

    memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
    wub_hex_2_str(byVal, byDataTmp1, usLen);
    memcpy(srTransRec.szTrack2Data, byDataTmp1, (usLen*2));
    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] == 'F')
            srTransRec.szTrack2Data[inIndex]=0;
    }
    vdDebug_LogPrintf("saturn szTrack2Data: %s %d", srTransRec.szTrack2Data, inIndex);

    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] != 'D')
            srTransRec.szPAN[inIndex] = byDataTmp1[inIndex];
        else
            break;
    }
    srTransRec.byPanLen = inIndex;
    vdDebug_LogPrintf("PAN: %s %d", srTransRec.szPAN, inIndex);
    inIndex++;
    memset(szDataTmp, 0x00, sizeof(szDataTmp));
    wub_str_2_hex(&byDataTmp1[inIndex], szDataTmp, 4);
    srTransRec.szExpireDate[0] = szDataTmp[0];
    srTransRec.szExpireDate[1] = szDataTmp[1];
    vdMyEZLib_LogPrintf("saturn EMV functions Expiry Date [%02x%02x]",srTransRec.szExpireDate[0],srTransRec.szExpireDate[1]);
    inIndex = inIndex + 4;
    memcpy(srTransRec.szServiceCode, &byDataTmp1[inIndex], 3);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5A_PAN, srTransRec.stEMVinfo.T5A, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn -------TAG_5A_PAN[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, srTransRec.stEMVinfo.T5A[0], srTransRec.stEMVinfo.T5A[1], srTransRec.stEMVinfo.T5A[2]);

    srTransRec.stEMVinfo.T5A_len = (BYTE)usLen;
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F30_SERVICE_CODE, srTransRec.stEMVinfo.T5F30, &usLen, szOutEMVData, inTagLen);

    memset(byVal, 0x00, sizeof(byVal));
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F34_PAN_IDENTFY_NO, byVal, &usLen, szOutEMVData, inTagLen);
    vdMyEZLib_LogPrintf("saturn F34: %02x %d", byVal[0], usLen);
    srTransRec.stEMVinfo.T5F34_len = usLen;
    srTransRec.stEMVinfo.T5F34 = byVal[0];

    shResult = usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn -------TAG_82_AIP-[%02x][%02x]-", srTransRec.stEMVinfo.T82[0], srTransRec.stEMVinfo.T82[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T84_len = (BYTE)usLen;

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F24_EXPIRE_DATE, srTransRec.stEMVinfo.T5F24, &usLen, szOutEMVData, inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F08_IC_VER_NUMBER, szDataTmp, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn -------TAG_9F08_IC_VER_NUMBER-[%02x][%02x]-",szDataTmp[0],szDataTmp[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn -------TAG_9F09_TERM_VER_NUMBER-[%02x][%02x]-",srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1]);
    if(usLen == 0)
        memcpy(srTransRec.stEMVinfo.T9F09, "\x00\x4C", 2);// can not get value from api like verifone,so i hardcode a value from EMV level 2 cert document
    vdDebug_LogPrintf("saturn 9F09: %02x%02x %d", srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1], usLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F20, srTransRec.szCardholderName, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn 5F20,szCardholderName: %s", srTransRec.szCardholderName);

// fix App label is  not printed on refund chip transaction. If 9F12 is missing terminal should print app label value (Tag 50)
//#00125 - App label is  not printed on refund chip transaction. If 9F12 is missing terminal should print app label value (Tag 50)
#if 1
    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F12, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("saturn 9F12,szChipLabel: %s", srTransRec.stEMVinfo.szChipLabel);

    if(strlen(srTransRec.stEMVinfo.szChipLabel) <= 0)
    {
		shResult = usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
		vdDebug_LogPrintf("saturn 50,szChipLabel: %s", srTransRec.stEMVinfo.szChipLabel);
    }
#endif

	// Get AID
	memset(szAID, 0x00, sizeof(szAID));
	shResult = usCTOSS_FindTagFromDataPackage(TAG_4F_AID, szAID, &usLen, szOutEMVData, inTagLen);
	vdDebug_LogPrintf("saturn -------TAG_4F_AID[%d] usLen[%d] [%02x%02x%02x%02x%02x%02x]--", shResult, usLen, szAID[0], szAID[1], szAID[2], szAID[3], szAID[4], szAID[5]);

}

int inCTOS_FirstGenACGetAndSaveEMVData(void)
{
    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
    int ret = 0;

    //Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - start -- jzg
    unsigned char szTransSeqCounter[8+1] = {0};
    //Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - end -- jzg

    unsigned char szHEXTransSeqCounter[3+1];

    BYTE bAppLabel[64] = {0}; // EMV: Get Application Label -- jzg
    BYTE bAppPrefName[64] = {0}; // EMV: Get Application Preferred Name -- jzg


//    	CTOS_PrinterPutString("inCTOS_FirstGenACGetAndSaveEMVData");

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

      //for improve transaction speed
    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_1STAC, &inTagLen, szOutEMVData);
    inDatabase_TerminalOpenDatabaseEx(DB_EMV);
    inMultiAP_Database_EMVTransferDataReadEx(&inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataInitEx();
    inDatabase_TerminalCloseDatabase();
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData");

    usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F12, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("9F12,szChipLabel: %s", srTransRec.stEMVinfo.szChipLabel);

    //#00125 - App label is  not printed on refund chip transaction. If 9F12 is missing terminal should print app label value (Tag 50)
    if(strlen(srTransRec.stEMVinfo.szChipLabel) <= 0)
    {
        usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
        vdDebug_LogPrintf("50,szChipLabel: %s", srTransRec.stEMVinfo.szChipLabel);
    }

#if 0
	/* EMV: Get Application Preferred Name - start -- jzg */
	usCTOSS_FindTagFromDataPackage(TAG_9F12, bAppPrefName, &usLen, szOutEMVData, inTagLen);
	vdDispAppLabel(bAppPrefName, usLen, srTransRec.stEMVinfo.szChipLabel);
	vdDebug_LogPrintf("TAG 9F12 = [%s]", srTransRec.stEMVinfo.szChipLabel);
	/* EMV: Get Application Preferred Name - end -- jzg */
#if 0
	usCTOSS_FindTagFromDataPackage(TAG_50, bAppLabel, &usLen, szOutEMVData, inTagLen);
	vdDispAppLabel(bAppLabel, usLen, srTransRec.stEMVinfo.szChipLabel);
#else
	/* EMV: Get Application Label - start -- jzg */
	if ((!((srTransRec.stEMVinfo.szChipLabel[0] >= 'a') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'z'))) ||
	(!((srTransRec.stEMVinfo.szChipLabel[0] >= 'A') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'Z'))))
	{
		//memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
		usCTOSS_FindTagFromDataPackage(TAG_50, bAppLabel, &usLen, szOutEMVData, inTagLen);

		if (strlen(bAppLabel) > 0){
			memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
			vdDispAppLabel(bAppLabel, usLen, srTransRec.stEMVinfo.szChipLabel);
			vdDebug_LogPrintf("TAG 50 = [%s]", srTransRec.stEMVinfo.szChipLabel);
		}
	}

#endif
#endif

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F1A_TERM_COUNTRY_CODE, srTransRec.stEMVinfo.T9F1A, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F1E, srTransRec.stEMVinfo.T9F1E, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F33_TERM_CAB, srTransRec.stEMVinfo.T9F33, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F35_TERM_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9F35), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F36_ATC, srTransRec.stEMVinfo.T9F36, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F36_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F37_UNPREDICT_NUM, srTransRec.stEMVinfo.T9F37, &usLen, szOutEMVData, inTagLen);

   

    //inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);
    inDatabase_TerminalOpenDatabase();
    ret = inIITReadEx(srTransRec.IITid);
    vdDebug_LogPrintf("inIITRead[%d]",ret);

	usCTOSS_FindTagFromDataPackage(TAG_9F53, (BYTE *)&(srTransRec.stEMVinfo.T9F53), &usLen, szOutEMVData, inTagLen);
    //Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - start -- jzg
    memset(szTransSeqCounter, 0, sizeof(szTransSeqCounter));
    if(strTCT.fEMVOnlinePIN == FALSE)
    {
        sprintf(szTransSeqCounter, "%06ld", strIIT.ulTransSeqCounter);
        wub_str_2_hex(szTransSeqCounter, (char *)szHEXTransSeqCounter, 6);
        memcpy(srTransRec.stEMVinfo.T9F41,szHEXTransSeqCounter,3);
    }
    else
    {
        sprintf(szTransSeqCounter, "%08ld", strIIT.ulTransSeqCounter);
        wub_str_2_hex(szTransSeqCounter, (char *)szHEXTransSeqCounter, 8);
        memcpy(srTransRec.stEMVinfo.T9F41,szHEXTransSeqCounter,4);

		DebugAddHEX("1. srTransRec.stEMVinfo.T9F41",srTransRec.stEMVinfo.T9F41,4);
    }
    strIIT.ulTransSeqCounter++;
    //Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - end -- jzg

    ret = inIITSaveEx(srTransRec.IITid);
    vdDebug_LogPrintf(" ret[%d] srTransRec.IITid[%d]strIIT.ulTransSeqCounter[%ld]",ret, srTransRec.IITid,strIIT.ulTransSeqCounter);

    //usCTOSS_FindTagFromDataPackage(TAG_9F53, (BYTE *)&(srTransRec.stEMVinfo.T9F53), &usLen, szOutEMVData, inTagLen);

	DebugAddHEX("2. srTransRec.stEMVinfo.T9F41",srTransRec.stEMVinfo.T9F41,4);

    usCTOSS_FindTagFromDataPackage(TAG_9F42_APP_CURRENCY_CODE, srTransRec.stEMVinfo.T9F42, &usLen, szOutEMVData, inTagLen);
    DebugAddHEX("AAA. srTransRec.stEMVinfo.T9F42",srTransRec.stEMVinfo.T9F42,4);
    inDatabase_TerminalCloseDatabase();
    return d_OK;
}

int inCTOS_SecondGenACGetAndSaveEMVData(void)
{

    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_2NDAC, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);//Improve 2nd Gen AC speed
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_SecondGenACGetAndSaveEMVData");

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);

    return d_OK;
}

int inCTOS_showEMV_TagLog (void)
{
    #define TOTAL_TAGS  24
    int i;
    unsigned short tagLen;
    char outp[40];

    typedef struct
    {
            unsigned short Tags;
            char description[20];
    }print_tag;

    print_tag EMVTag[TOTAL_TAGS] =
    {
        {   TAG_95,                     "TVR"},
        {   TAG_9B,                     "TSI"},
        {   TAG_9F26_EMV_AC,            "ARQC"},
        {   TAG_9F27,                   "Crypt Info Data"},
        {   TAG_9F10_IAP,               "Issuer Appl Data"},
        {   TAG_9F37_UNPREDICT_NUM,     "Unpredicte number"},
        {   TAG_9F36_ATC,               "Appl Trans Counter"},
        {   TAG_9A_TRANS_DATE,          "Trans Date"},
        {   TAG_9C_TRANS_TYPE,          "Trans Type"},
        {   TAG_5A_PAN,                 "PAN"},
        {   TAG_5F34_PAN_IDENTFY_NO,    "Sequence Num"},
        {   TAG_9F02_AUTH_AMOUNT,       "Amount Authorized"},
        {   TAG_9F03_OTHER_AMOUNT,      "Add Amount"},
        {   TAG_5F2A_TRANS_CURRENCY_CODE, "Trans Currency Code"},
        {   TAG_82_AIP,                 "AIP"},
        {   TAG_9F1A_TERM_COUNTRY_CODE, "Term Country Code"},
        {   TAG_9F34_CVM,               "CVR"},
        {   TAG_9F10_IAP,               "Issuer auth Data"},
        {   TAG_9F06,                   "AID"},
        {   TAG_50,                     "Appl Label Name"},
        {   TAG_8F,                     "CA Public Key Index"},
        {   TAG_9F0D,                   "IAC Default"},
        {   TAG_9F0E,                   "IAC Denial"},
        {   TAG_9F0F,                   "IAC Online"}

    };


    for(i = 0; i<TOTAL_TAGS; i++)
    {
        memset(outp,0x00,sizeof(outp));
        ushCTOS_EMV_NewDataGet(EMVTag[i].Tags, &tagLen, outp);
        vdDebug_LogPrintf("----TAG[%s][%x]=====Len[%d]----",EMVTag[i].description,EMVTag[i].Tags,tagLen);
        DebugAddHEX("Value===",outp,tagLen);
    }
    return d_OK;
}

void vdCTOSS_GetAmt(void)
{
	memcpy(srTransRec.szBaseAmount, szBaseAmount, 6);
}

void vdCTOSS_SetAmt(BYTE *baAmount)
{
	BYTE szTemp[20];

	memset(szTemp, 0x00, sizeof(szTemp));
	sprintf(szTemp, "%012ld", atol(baAmount));
	wub_str_2_hex(szTemp, szBaseAmount,12);
}

void vdDispAppLabel(unsigned char *ucHex, int inLen, char *szOutStr)
{
	int i;
	char szBuf[80] = {0};

	for (i = 0; i < inLen; i++)
		szBuf[i] = ucHex[i];
	szBuf[i] = 0x00;

	memcpy(szOutStr, szBuf, inLen);
}
int inCTOS_FraudControl(void)
{
    int inRet=0,inPANLen;
	BYTE strOut[4+1];//, szPAN[4+1];
	short shMaxLen=4, shMinLen=4;

	BYTE szTitle[25+1];
    BYTE szDisplay[100];
    BYTE szDisMsg[100];

    if(strHDT.fLast4Digit != TRUE)
        return d_OK;

//#00120 - Fallback to Magtripe should prompt last 4 digit
    if((srTransRec.byEntryMode != CARD_ENTRY_MSR) &&
    (srTransRec.byEntryMode != CARD_ENTRY_FALLBACK))
        return d_OK;

    //CTOS_LCDTClearDisplay();
    ///vduiLightOn();

    //vdDispTransTitle(srTransRec.byTransType);
    //vdClearNonTitleLines();
	//vdClearBelowLine(2);

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	//displayAppbmpDataEx(140, 35, strIIT.szIssuerLogo, TRUE);


    inPANLen=strlen(srTransRec.szPAN)-4;
    //memset(szPAN, 0, sizeof(szPAN));
	//memcpy(szPAN, &srTransRec.szPAN[inPANLen-4], 4);

	strcpy(szDisplay, "4");
	strcat(szDisplay, "|");
    strcat(szDisplay, "4");
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "LAST 4 DIGITS ACCNT:");

	while(1)
	{
		//vduiClearBelow(8);

		//CTOS_LCDTPrintXY(1, 7, "LAST 4 DIGITS ACCNT:");
		//setLCDPrint(7, DISPLAY_POSITION_LEFT, "LAST 4 DIGITS ACCNT:");
		memset(strOut,0x00, sizeof(strOut));
		shMaxLen=4; //aaronnino mcc v3 gprs fix on issue #0020 Unable to enter last 4 digits. If entered only 1 / 2 / 3 digits only was entered first
		//inRet = InputString(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
		//inRet = InputString2(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

		inRet = InputStringUI(0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szDisplay);

                  if (inRet == d_KBD_CANCEL )
                   {
                        //memset(szDisMsg, 0x00, sizeof(szDisMsg));
                        //strcpy(szDisMsg, szTitle);
                        //strcat(szDisMsg, "|");
                        //strcat(szDisMsg, "USER CANCEL");
                        //usCTOSS_LCDDisplay(szDisMsg);
                       /* strcpy(szDisMsg, "|PROCESSING...");
                        usCTOSS_LCDDisplay(szDisMsg);*/
                        vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
                        return (d_NO);
                   }
                   else if (inRet == 0xFF )
                   {
                        memset(szDisMsg, 0x00, sizeof(szDisMsg));
                        vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
                   		CTOS_Beep();
                   		CTOS_Delay(2000);
                        return (d_NO);
                   }
                   else if(inRet>=1)
                   {
                        if(memcmp(strOut, srTransRec.szPAN+inPANLen, 4) == 0)
                             break;
                        else
                        {
                             //vdDisplayErrorMsg(1, 8, "INVALID ACCOUNT NO.");
                             //vdClearBelowLine(8);
                             memset(szDisMsg, 0x00, sizeof(szDisMsg));
                             strcpy(szDisMsg, szTitle);
                             strcat(szDisMsg, "|");
                             strcat(szDisMsg, "INVALID ACCOUNT NO.");
                             //usCTOSS_LCDDisplay(szDisMsg);
                             vdDisplayMessageBox(1, 8, "", "INVALID ACCOUNT NO.", "", MSG_TYPE_INFO);
                             CTOS_Beep();
                             CTOS_Delay(1500);
                        }
                   }

     }
      //strcpy(szDisMsg, "|PROCESSING...");
      //usCTOSS_LCDDisplay(szDisMsg);
      vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
     return d_OK;
}

void vdCTOS_SetDateTime(void)
{
    CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];

    CTOS_RTCGet(&SetRTC);
    memset(szCurrentTime, 0, sizeof(szCurrentTime));
    sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
    wub_str_2_hex(szCurrentTime,srTransRec.szDate,DATE_ASC_SIZE);

    memset(szCurrentTime, 0, sizeof(szCurrentTime));
    sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
    wub_str_2_hex(szCurrentTime,srTransRec.szTime,TIME_ASC_SIZE);
}

int inCheckValidApp(int HDTid)
{
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if(strCDT.HDTid != HDTid)
    {
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    return d_OK;
}

// added by mfl
int inConfirmPAN(void)
{
	unsigned char key;
	int inRet = d_NO;

	vdDebug_LogPrintf("--inConfirmPAN--");
	vdDebug_LogPrintf("inConfirmPAN[%d]", strTCT.fConfirmPAN);

	setLCDPrint(8, DISPLAY_POSITION_CENTER, "CONFIRM?YES[OK]NO[X]");

	if (strTCT.fConfirmPAN)
	{
		while (1)
		{
			CTOS_KBDGet(&key);

			if (key == d_KBD_ENTER)
			{
				inRet = d_OK;
				break;
			}

			if (key == d_KBD_CANCEL)
			{
				inRet = d_NO;
				break;
			}
		}
	}
	else
		inRet = d_OK;

	if (inRet != d_OK)
		vdSetErrorMessage("USER CANCEL");

	return(inRet);
}


int inGetMerchantPassword(void)
{
    USHORT shMaxLen, shMinLen;    
     BYTE szTitle[25+1];
     BYTE szDisplay[128];
     BYTE szMsg[100];
     BYTE szDisMsg[100];

     unsigned char szOutput[30];
     int  inTxnTypeID;
     BYTE Bret;

    if(strMMT[0].fPasswordEnable == TRUE)
    {
        memset(szTitle, 0x00, sizeof(szTitle));
	 	szGetTransTitle(srTransRec.byTransType, szTitle);

        while(1)
        {
            shMinLen = PWD_MIN_LEN;
            shMaxLen = PWD_MAX_LEN;

           if (inMultiAP_CheckSubAPStatus() == d_OK)
          return d_OK;

	 memset(szTitle, 0x00, sizeof(szTitle));
	 szGetTransTitle(srTransRec.byTransType, szTitle);
	 
	 memset(szOutput,0,sizeof(szOutput));
	 memset(szMsg,0,sizeof(szMsg));
	 
	 strcpy(szMsg, "4");
	 strcat(szMsg, "|");
	 strcat(szMsg, "6");
	 strcat(szMsg, "|");
	 strcat(szMsg, szTitle);
	 strcat(szMsg, "|");
	 strcat(szMsg, "MERCHANT PASSWORD");
	 strcat(szMsg, "|");
	 strcat(szMsg, strMMT[0].szPassword);

	 Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT, szMsg);
          vdDebug_LogPrintf("AAA - inGetMerchantPassword Bret[%c]", Bret);
	 if (Bret == d_KBD_CANCEL )
         {
               vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
               CTOS_Delay(500);
               CTOS_Beep();
               return d_NO;
         }
         else if (Bret == d_KBD_ENTER ) 
         {
              return d_OK;
         }
	else
         {
              vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
              CTOS_Delay(500);
              CTOS_Beep();
	     return d_NO;
         }

	return d_OK;

        }
	}
	else
        return d_OK;
}

int inGetKeyPressMsg(int key)
{
	if (key == d_KBD_CANCEL || key == 0)
		vdSetErrorMessage("USER CANCEL");
	else if(key == 0xFF)
		vdSetErrorMessage("TIME OUT");
    else if(key > 0)
        return d_OK;

    return d_NO;
}

// app switch module 06052015
void vdCTOSS_ChangeDefaultApplication(void)
{
    CTOS_stCAPInfo stinfo;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	short shHostIndex;
	int inResult;
    unsigned char ckey;

//    	CTOS_PrinterPutString("ChangeDefaultApplication");

	if (inMultiAP_CheckSubAPStatus() != d_OK)
	{
        char szChoiceMsg[30 + 1];
        char szHeaderString[24+1];
        int bHeaderAttr = 0x01+0x04, key=0;

        memset(szHeaderString, 0, sizeof(szHeaderString));
        memset(szChoiceMsg, 0, sizeof(szChoiceMsg));

        strcpy(szHeaderString, "SWITCH APP");
        strcat(szChoiceMsg,"OLD HOST");
        key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
        /*
              MCC shHostIndex=1
              MCC ONE APP shHostIndex=19
		*/
        if (key > 0)
        {
            if (key == 1)
            {
				inCTOSS_ForceSettle();
                shHostIndex=1;
            }
            else
                return;
        }
        //inEnableNewHost(TRUE);
        //vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = shHostIndex;
        strHDT.inHostIndex = shHostIndex;
        inHDTRead(shHostIndex);
        inCPTRead(shHostIndex);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
	}

    CTOS_LCDTClearDisplay();
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
		inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_FORCE_SETTLE);

        inResult = inCTOS_MultiAPSaveData(d_IPC_CMD_CHANGE_DEF_APP);
        if(d_OK != inResult)
            return ;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inResult = inCTOS_MultiAPGetData();
            if(d_OK != inResult)
                return ;

            inResult = inCTOS_MultiAPReloadHost();
            if(d_OK != inResult)
                return ;
        }
    }

    getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	strcpy(exe_subdir, &exe_dir[9]);
	inExeAPIndex = atoi(exe_subdir);
	memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
    if(CTOS_APGet(inExeAPIndex, &stinfo) != d_OK)
		CTOS_APGet(inExeAPIndex, &stinfo);

	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 7, "PLEASE WAIT");
	CTOS_LCDTPrintXY(1, 8, "SWITCHING APP...");

    //android-removed
    //inResult=CTOS_APSet(inExeAPIndex, d_AP_FLAG_DEF_SEL);
    //end

	inTCTRead(1);
    if(strHDT.inHostIndex == 1)/*switch to OLD HOST*/
    {
        strTCT.inMenuidNewHost=strTCT.inMenuid;
		strTCT.inMenuid=strTCT.inMenuidOldHost;
		inTCTMenuSave(1);
        inUpdateDefaultApp(1, 19);
		inUpdateBancnetCDT(12);
		//inEnableNewHost(FALSE);
    }
    else if(strHDT.inHostIndex == 19) /*switch to NEW HOST*/
    {
        strTCT.inMenuidOldHost=strTCT.inMenuid;
		strTCT.inMenuid=strTCT.inMenuidNewHost;
		inTCTMenuSave(1);
        inUpdateDefaultApp(19, 1);
		//inUpdateBancnetCDT(19);
		//inEnableOldHost(FALSE);
    }
	CTOS_Delay(3000); // for testing. just to make all table all transfered correctly from old to new and vise versa.

	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 7, "PLEASE PRESS POWER");
	CTOS_LCDTPrintXY(1, 8, "BUTTON TO RESTART");

    while(1)
	    CTOS_KBDGet(&ckey);

    CTOS_SystemReset();
}

int inCTOSS_ForceSettle(void)
{
	int inResult;
	int shHostIndex = 1;
	int inNum = 0;
	int inNumOfHost = 0;
	int inNumOfMerchant = 0;
	int inLoop =0 ;
	ACCUM_REC srAccumRec;
	STRUCT_FILE_SETTING strFile;
	char szAPName[50];
	int inAPPID;

	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 7, "PLEASE WAIT");
	CTOS_LCDTPrintXY(1, 8, "CHECKING BATCH...");

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	//check host num
	inNumOfHost = inHDTNumRecord();

	vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}

			inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);

			vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
			for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
			{
					strMMT[0].HDTid = strHDT.inHostIndex;
					strMMT[0].MITid = strMMT[inLoop-1].MITid;
                    			strcpy(strMMT[0].szTID, strMMT[inLoop-1].szTID);
                    			strcpy(strMMT[0].szMID, strMMT[inLoop-1].szMID);
					memcpy(strMMT[0].szBatchNo, strMMT[inLoop-1].szBatchNo, 4);

					srTransRec.HDTid = strMMT[0].HDTid;
					srTransRec.MITid = strMMT[inLoop-1].MITid;
					strcpy(srTransRec.szTID, strMMT[0].szTID);
					strcpy(srTransRec.szMID, strMMT[0].szMID);
					memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
					strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

					vdDebug_LogPrintf("srTransRec.MITid[%d]strHDT.inHostIndex[%d]", srTransRec.MITid, strHDT.inHostIndex);
					memset(&srAccumRec,0,sizeof(srAccumRec));
					memset(&strFile,0,sizeof(strFile));
					memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
					memset(&strFile,0,sizeof(strFile));
					vdCTOS_GetAccumName(&strFile, &srAccumRec);

				if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
				{
					vdCTOS_SetDateTime(); // fix for issues #00007, 00011 and 00015
                    inCTOS_PrintSettleReport(TRUE, TRUE);
                    inCTOS_SettlementClearBathAndAccum(TRUE);
                    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
				}
				inMyFile_ReversalDelete(); /*delete reversal*/
			}
		}
		else
			continue;

	}

	return (d_OK);
}




//sidumili: pad string
void vdCTOS_Pad_String(char* str,int padlen,char padval,int padtype) {
    int padno;

    if ((padno = padlen - strlen(str)) > 0) {
        if (padtype == POSITION_LEFT)
            memmove(str + padno, str, strlen(str) + 1);
        else
            str += strlen(str);
        memset(str, padval, padno);
        if (padtype == POSITION_RIGHT)
            *(str + padno) = '\0';
    } else if (padno < 0) {
        // Truncate string if too long!!
        memmove(str, str + abs(padno), padlen + 1);
    }
}
//sidumili: pad string

int inCTOS_GetCardFieldsCtls(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    VS_BOOL fInsertOnlyDisp = VS_FALSE;  //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 1 of 3
    VS_BOOL fInsertChipOnly=VS_FALSE;
	short shReturn = d_OK; //Invalid card reading fix -- jzg
    BYTE szTitle[25+1];
    BYTE szDisMsg[100];
    BYTE szManualEntryFlag[2];
    BYTE szBillsPaymentCash[2];

	VS_BOOL fSwipeEnable=strTCT.fSwipeEnable;
	VS_BOOL fCTLSEnable=strTCT.fCTLSEnable;
	VS_BOOL fFalllbackEnable=strTCT.fFalllbackEnable;
	
    #ifdef CONTACTLESS
        ULONG ulAPRtn;
        BYTE temp[64];
        char szTotalAmount[AMT_ASC_SIZE+1];
		char szBaseAmount[AMT_ASC_SIZE+1];
		char szTipAmount[AMT_ASC_SIZE+1];
        EMVCL_RC_DATA_EX stRCDataEx;
        BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1],szCurrCode[3+1];
        BOOL fMaxCTLSAmt = FALSE;
        long amt1 = 0;
        long amt2 = 0;
    #endif

    int inKey;

	BYTE szAmountBuff[30], szTemp1[30], szTemp2[30];
	
    BYTE szDisplay[200];
	BYTE szAmtMsg[40 + 1] = {0};

    DebugAddSTR("inCTOS_GetCardFieldsCtls","Processing...",20);

	vdDebug_LogPrintf("saturn getcard fields");
	
	//InsertCardUI();

	ing_KeyPressed=0;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

CARD_PROMPT_AGAIN:
	

    memset(szDisMsg, 0x00, sizeof(szDisMsg));
    memset(szTitle, 0x00, sizeof(szTitle));
    memset(szManualEntryFlag, 0x00, sizeof(szManualEntryFlag));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	vdDispTransTitle(srTransRec.byTransType);

    if(strTCT.fManualEntry == 1)
        strcpy(szManualEntryFlag, "1");
    else
        strcpy(szManualEntryFlag, "0");

    memset(szBillsPaymentCash, 0, sizeof(szBillsPaymentCash));
	szBillsPaymentCash[0]='0';
    if(srTransRec.byTransType == BILLS_PAYMENT)
    {
		szBillsPaymentCash[0]='1';
    }
	
	memset(szAmountBuff, 0x00, sizeof(szAmountBuff));
	wub_hex_2_str(srTransRec.szTotalAmount, szAmountBuff, 6);
	memset(szTemp1, 0x00, sizeof(szTemp1));	
	memset(szTemp2, 0x00, sizeof(szTemp2));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmountBuff, szTemp1);
	sprintf(szTemp2, "%s %s", strCST.szCurSymbol, szTemp1);					

	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
	sprintf(szAmtMsg, "TOTAL AMOUNT\n%s", szTemp2);

        CTOS_LCDTPrintXY(1, 2, szAmtMsg);
    //InsertCardUI();  //--Tine
    //DisplayMessage();
//    strcpy(szDisMsg, szTitle);
//    strcat(szDisMsg, "|");
	strcat(szDisMsg, "PLEASE ");
	if(fSwipeEnable == VS_TRUE)
        strcat(szDisMsg, "SWIPE/");
	
	strcat(szDisMsg, "INSERT");
	
	if(fCTLSEnable == VS_TRUE)
		strcat(szDisMsg, "/TAP");

	strcat(szDisMsg, " CARD");
	CTOS_LCDTPrintXY(1, 3, szDisMsg);
        
    strcat(szDisMsg, "|");
    //strcat(szDisMsg, "CARD ENTRY");
    //strcat(szDisMsg, szTemp2);
    strcat(szDisMsg, szAmtMsg);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szManualEntryFlag);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szBillsPaymentCash);
    //usCTOSS_LCDDisplay(szDisMsg);
//    inKey = usCARDENTRY(szDisMsg);			//Tine:  24Apr2019

    // flush buffer - for testing
    /*
    fix for issues #00115, 00116 and 00117
    - Able to process card after swiping in Sale Menu
    - Able to process card after swiping Password Entry prompt
    - Able to process card after swiping on Main Menu
    */
    if(fEntryCardfromIDLE != TRUE)
    {
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //aaronnino mcc v3 gprs fix on issue #00005 CARD READ ERROR without swiping / inserting card start
        if (byMSR_status==2)
        {
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "CARD READ ERROR");
            usCTOSS_LCDDisplay(szDisMsg);
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_Beep();
            return READ_CARD_TIMEOUT;
        }
        //aaronnino mcc v3 gprs fix on issue #00005 CARD READ ERROR without swiping / inserting card end
    }

 ing_KeyPressed = 0;

 SWIPE_AGAIN:

	vdDebug_LogPrintf("saturn swipe again");

#ifdef CONTACTLESS
    memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
    memset(szOtherAmt,0x00,sizeof(szOtherAmt));
    memset(szTransType,0x00,sizeof(szTransType));
    memset(szCatgCode,0x00,sizeof(szCatgCode));
    memset(szCurrCode,0x00,sizeof(szCurrCode));
    memset(temp,0x00,sizeof(temp));
    memset(szTotalAmount,0x00,sizeof(szTotalAmount));

    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);

    sprintf(szTotalAmount, "%012.0f", atof(szBaseAmount) + atof(szTipAmount));
    wub_str_2_hex(szTotalAmount, srTransRec.szTotalAmount, 12);

vdDebug_LogPrintf("V3 display...[%s]",szDisMsg);
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
    CTOS_LCDTPrintXY(1, 4, temp);

	if (srTransRec.byTransType == REFUND)
		szTransType[0] = 0x20;

	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);

	amt1 = wub_str_2_long(szTotalAmount);
	amt2 = wub_str_2_long(strTCT.szMaxCTLSAmount);

	if (amt1 >= amt2)
		fMaxCTLSAmt = TRUE;
	else
		fMaxCTLSAmt = FALSE;

    if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
    {
        ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
        if(ulAPRtn != d_EMVCL_NO_ERROR)
        {
            vdSetErrorMessage("CTLS InitTrans Fail!");
            return d_NO;
        }
    }
#endif

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        
		vdDebug_LogPrintf("saturn validate first idle key");
		
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);

        //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 2 of 3 start
        //if((fEntryCardfromIDLE != TRUE) && (fInsertOnlyDisp == VS_FALSE))
        //{
          //  displayAppbmpDataEx(1, 30, "INSERT.bmp", FALSE);
        //}
        if(fInsertOnlyDisp == VS_TRUE)
        {
            
			vdDebug_LogPrintf("saturn finsertonly");
        	if(isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
			{
				setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE");
				setLCDPrint(4, DISPLAY_POSITION_CENTER, "INSERT CARD");
			}
			/*else
			{
				inCTOS_DisplayIdleBMP();
	            displayAppbmpDataEx(1, 30, "INSERT_ONLY.bmp", FALSE);
	            
			}*/
            fInsertOnlyDisp = VS_FALSE;
        }
		else
		{
			if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch() == d_OK)
			{
                            vdDebug_LogPrintf("inCTOSS_GetCtlsMode(%d)", inCTOSS_GetCtlsMode());
				if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
				{
					if (strTCT.fManualEntry)
					{
						setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE TAP/INSERT");
						setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE/KEY-IN CARD");
					}
					else
					{
						setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE TAP/INSERT");
						setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
						
					}
				}
			}
			
                        setLCDPrint(3, DISPLAY_POSITION_CENTER, "PLEASE TAP/INSERT");
			setLCDPrint(4, DISPLAY_POSITION_CENTER, "SWIPE CARD");
                }
        //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 2 of 3 end
	}
	#if 0
    // patrick ECR 20140516 start
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30];
    		BYTE szTemp1[30+1];

    		CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);
    	}
    }
    // patrick ECR 20140516 end
	#endif

    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , UI_TIMEOUT);

    while (1)
    {

    //usCARDENTRY(szDisMsg);
	vdDebug_LogPrintf("saturn inside while loop,ing_KeyPressed=%d",ing_KeyPressed);

	if (ing_KeyPressed == 'C')
	{
		CTOS_KBDBufPut('C');
		/*vdDebug_LogPrintf("putchar C");
		szGetTransTitle(srTransRec.byTransType, szTitle);
		strcpy(szDisplay, szTitle);
		strcat(szDisplay, "|");
		strcat(szDisplay, "USER CANCEL");
		usCTOSS_LCDDisplay(szDisplay);
		CTOS_Beep();
		CTOS_Delay(1500);*/
            if(fECRTxnFlg==TRUE)
            {
                 vdSetECRResponse(ECR_OPER_CANCEL_RESP);
            }
		return USER_ABORT;
	}
	else if (ing_KeyPressed == 'T')
	{	
		CTOS_KBDBufPut('T');
		vdDebug_LogPrintf("putchar T");
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(2000);
		 if(fECRTxnFlg==TRUE)
        {
             vdSetECRResponse(ECR_TIMEOUT_ERR);
        }
		return READ_CARD_TIMEOUT;	
	}
	else if (ing_KeyPressed == 'M')     //Manual Entry
	{
		CTOS_KBDBufPut('M');
		vdDebug_LogPrintf("putchar M");
        inCTOSS_CLMCancelTransaction();
        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

		usCTOSS_LCDDisplay(" ");

        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
        {
            vdSetFirstIdleKey(0x00);
            CTOS_KBDBufFlush ();
            //vdSetErrorMessage("Get Card Fail M");
            return USER_ABORT;
        }

        //Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

        break;
	}
	else if (ing_KeyPressed == 'K')     //Manual Entry
	{
		CTOS_KBDBufPut('K');
		vdDebug_LogPrintf("putchar K");
        inCTOSS_CLMCancelTransaction();
        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

		usCTOSS_LCDDisplay(" ");
        srTransRec.fBillsPaymentCash=VS_TRUE;
        return d_OK;
	}

	vdDebug_LogPrintf("saturn check ing_KeyPressed");

#if 1
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
			#ifdef CONTACTLESS
			if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
			#endif
			vdDebug_LogPrintf("GetCardFieldsCtls timed out...");
			ing_KeyPressed = 'T';
			if (fECRTxnFlg)
            {
                 memset(srTransRec.szECRRespCode,0,sizeof(srTransRec.szECRRespCode));
                 memcpy(srTransRec.szECRRespCode,ECR_TIMEOUT_ERR,2);
            }
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }

#endif
        //if(strlen(szPAN) <= 0)
        //{

		vdDebug_LogPrintf("saturn check sc status");

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
			vdDebug_LogPrintf("saturn d_MK_SC_PRESENT");
            
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();


			vdDebug_LogPrintf("saturn card entry ICC");

			//vduiClearBelow(2);
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            //if (d_OK != inCTOS_EMVCardReadProcess ())

            vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
            shReturn = inCTOS_EMVCardReadProcess();
            vdDebug_LogPrintf("saturn inCTOS_GetCardFieldsCtls | shReturn[%d]", shReturn);
			if (shReturn != d_OK)
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    //vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
                    if(fFalllbackEnable == VS_FALSE)
                    {
						vdRemoveCard();
						goto CARD_PROMPT_AGAIN;
                    }

					if(shReturn == EMV_CHIP_FAILED)
						
                    vdDebug_LogPrintf("saturn inFallbackToMSR is success");
                    vdRemoveCard();
					vdDebug_LogPrintf("saturn  strTCT.inFallbackCounter[%d]", strTCT.inFallbackCounter);
                    if(strTCT.inFallbackCounter > 0)
                    {
                        if(inCTOS_GetCardFieldsFallback() != d_OK)
                            return USER_ABORT;
                    }
                }
                else
                {
					if(shReturn == EMV_CHIP_FAILED)
						goto CARD_PROMPT_AGAIN;
					
                      vdDebug_LogPrintf("AAA inFallbackToMSR is ABORT");
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
        {
			vdDebug_LogPrintf("Process idle swipe...");
			 if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
				 inCTOSS_CLMCancelTransaction();

             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
               /* //vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "PLS INSERT CARD");
                
                usCTOSS_LCDDisplay(szDisMsg);*/

	   		 	memset(szDisMsg, 0x00, sizeof(szDisMsg));               
             	strcpy(szDisMsg, szTitle);
             	strcat(szDisMsg, "|");
             	strcat(szDisMsg, "PLEASE INSERT CARD");
             	strcat(szDisMsg, "|");
             	strcat(szDisMsg, "CARD ENTRY");
             	strcat(szDisMsg, "|");
             	strcat(szDisMsg, "0"); //szManualEntryFlag 0 is off 1 is on
			  
              	ing_KeyPressed = 0;
              	usCARDENTRY(szDisMsg);		
                CTOS_Beep();
                //CTOS_Delay(1500);
                //CTOS_Beep();
                fInsertChipOnly=VS_TRUE;
				vdDebug_LogPrintf("fInsertChipOnly = [%d]", fInsertChipOnly);
				vdDebug_LogPrintf("goto SWIPE_AGAIN...");
                goto SWIPE_AGAIN;

             }

             break;

         }


            byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
    
    		
     		//aaronnino mcc v3 gprs fix on issue #0049 1 minute time-out in wrong swiping of card start
            if (byMSR_status==2)
            {
                if(fSwipeEnable == FALSE)
	            {
					vdDisplayMessageBox(1, 8, "", "NOT SUPPORTED", "", MSG_TYPE_WARNING);
					CTOS_Beep();
					CTOS_Delay(1500);
				    goto CARD_PROMPT_AGAIN;
	            }
				
                strcpy(szDisMsg, " ");
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "CARD READ ERROR");
                usCTOSS_LCDDisplay(szDisMsg);
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();
                return READ_CARD_TIMEOUT;
            }
    		//aaronnino mcc v3 gprs fix on issue #0049 1 minute time-out in wrong swiping of card end
    
    		//Fix for Track2 Len < 35
            //if((byMSR_status == d_OK ) && (usTk2Len > 35))
            if(byMSR_status == d_OK )
            {
            	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
            		inCTOSS_CLMCancelTransaction();
				
	            if(fSwipeEnable == FALSE)
	            {
					vdDisplayMessageBox(1, 8, "", "NOT SUPPORTED", "", MSG_TYPE_WARNING);
					CTOS_Beep();
					CTOS_Delay(1500);
					goto CARD_PROMPT_AGAIN;
	            }

    			if(fInsertChipOnly == VS_TRUE)
    			    continue;
    
                vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);
    
                if (d_OK != inCTOS_LoadCDTIndex())
                 {
                     CTOS_KBDBufFlush();
                     return USER_ABORT;
                 }
    
                if(d_OK != inCTOS_CheckEMVFallback())
                 {
                    vdCTOS_ResetMagstripCardData();
                    //vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                    #if 0
                    strcpy(szDisMsg, szTitle);
                    strcat(szDisMsg, "|");
                    strcat(szDisMsg, "PLS INSERT CARD");
    				
                    usCTOSS_LCDDisplay(szDisMsg);
                    
                    CTOS_Beep();
                    CTOS_Delay(1500);
                    CTOS_Beep();
    				#endif
    				
    				strcpy(szDisMsg, szTitle);
    				strcat(szDisMsg, "|");
    				strcat(szDisMsg, "PLEASE INSERT CARD");
    				strcat(szDisMsg, "|");
    				strcat(szDisMsg, "CARD ENTRY");
    				strcat(szDisMsg, "|");
    				strcat(szDisMsg, "0");
    				//usCTOSS_LCDDisplay(szDisMsg);
    				inKey = usCARDENTRY(szDisMsg);			//Tine:  24Apr2019
    
    
    				fInsertOnlyDisp = VS_TRUE; //aaronnino mcc v3 gprs fix on issue #0021 Incorrect terminal display upon swiping chip card on idle screen 3 of 3
                    fInsertChipOnly=VS_TRUE;
    
    				vdDebug_LogPrintf("fInsertChipOnly = [%d]", fInsertChipOnly);
    				vdDebug_LogPrintf("goto SWIPE_AGAIN...");
    				
                    goto SWIPE_AGAIN;
    
                 }
    
                break;
            }
		
        #ifdef CONTACTLESS

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
		{
			if(fInsertChipOnly == VS_TRUE)
			    continue;

            ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);
            if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
            {
                vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
            }
            else if(ulAPRtn == d_EMVCL_TX_CANCEL)
            {
                //vdDisplayErrorMsg(1, 8, "USER CANCEL");
                return USER_ABORT;
            }
            else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
            {
				if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
	        		inCTOSS_CLMCancelTransaction();

				vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
				CTOS_Beep();
				CTOS_Delay(1500);

                return USER_ABORT;
            }
            else if(ulAPRtn != d_EMVCL_PENDING)
            {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                break;
            }
		}
		else
		{
			if(fCTLSEnable == VS_FALSE)
                continue;

            if(fInsertChipOnly == VS_TRUE)
                continue;

			memset(szTransType,0x00,sizeof(szTransType));
			strcpy(szOtherAmt,"000000000000");
			if (srTransRec.byTransType == REFUND)
				strcpy(szTransType,"32");
			else
				strcpy(szTransType,"00");
			ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);
			vdDebug_LogPrintf("saturn return from usCTOSS_CtlsV3Trans");
            //TEST-REMOVE FOR S1F2
			//CTOS_Beep();
		    CTOS_Delay(50);
			//TEST-REMOVE FOR S1F2
		    //CTOS_Beep();
			//CTOS_Delay(50);
		    //CTOS_Beep();
            if(ulAPRtn == d_OK)
            {
                //TEST
                CTOS_Beep();
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                ulAPRtn = d_EMVCL_RC_DATA;
                break;
            }
            else if (ulAPRtn == d_NO)
            {
                memset(temp,0x00,sizeof(temp));
                inCTOSS_GetEnvDB("CTLSRESP", temp);
                ulAPRtn = atof(temp);
                vdDebug_LogPrintf("CTLSRESP[%s],ulAPRtn=[%x]", temp,ulAPRtn);

				if(ulAPRtn == d_EMVCL_RC_FALLBACK)
                    vdDisplayErrorMsg2(1, 8, "CTLS ERROR PLEASE", "INSERT/SWIPE CARD", MSG_TYPE_ERROR);
                else if (ulAPRtn == d_EMVCL_RC_FAILURE)
                    vdDisplayErrorMsg2(1, 8, "CTLS READ CARD ERROR", "PLS INSERT CARD", MSG_TYPE_ERROR);
                else if(ulAPRtn == d_EMVCL_RC_EXCEED_OR_EQUAL_CL_TX_LIMIT)
                    vdDisplayErrorMsg3(1, 8, "CTLS TRANSACTION", "LIMIT EXCEEDED PLS", "INSERT/SWIPE CARD");
                else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
                    vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
	      	
                if(ing_KeyPressed == 'T' || CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES) {
					vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
					CTOS_Beep();
					CTOS_Delay(1500);
                } else if(ing_KeyPressed == 'D') {
                	vdDisplayMessageBox(1, 8, "", "Please try again", "", MSG_TYPE_ERROR);
					CTOS_Beep();
					CTOS_Delay(300);
					//CTOS_Delay(1500);
                } else
                     vdDisplayErrorMsg2(1, 8, "CTLS READ CARD ERROR", "PLS INSERT CARD", MSG_TYPE_ERROR);

                return USER_ABORT;
            }
		}
		#endif
    }

    #ifdef CONTACTLESS
    if ((srTransRec.byEntryMode == CARD_ENTRY_WAVE) && (!fMaxCTLSAmt))
    {
        if(ulAPRtn != d_EMVCL_RC_DATA)
        {
            vdCTOSS_WaveCheckRtCode(ulAPRtn);
			if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
				inCTOSS_CLMCancelTransaction();
            return d_NO;
        }

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
		{
            if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
            {
                inCTOSS_CLMCancelTransaction();
                return d_NO;
            }
		}
        else
        {
            if (d_OK != inCTOSS_V3AnalyzeTransaction(&stRCDataEx))
                return d_NO;
        }

		vdDebug_LogPrintf("saturn before load cdt index");

        if (d_OK != inCTOS_LoadCDTIndex())
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }
        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
            inCTOSS_CLMCancelTransaction();
    }
    #endif

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("saturn srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}

void vdDisplayECRAmount(void)
{
    BYTE key;

    if (strTCT.fECR) // tct
    {
		if(fGetECRTransactionFlg() != TRUE)
			return;

        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
        {
            BYTE szTemp1[30+1];
            BYTE szStr[30+1];

            inCSTRead(1);

			CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
			wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
			memset(szStr, 0x00, sizeof(szStr));
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, szStr);
			setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
			setLCDPrint(8, DISPLAY_POSITION_RIGHT, szStr);
			CTOS_Delay(1000);
			//CTOS_Delay(3000);
        }
    }
}

void vdSaveLastInvoiceNo(void)
{
    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
    if(inTCTSave(1) != ST_SUCCESS)
        vdDisplayErrorMsg(1, 8, "Update TCT fail");
}

void vdClearMSRAndRemoveCard(void)
{
    USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];

    CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
    inSetTextMode();
    vdRemoveCard();
    CTOS_LCDTClearDisplay();
}

int inCTOS_ByPassPIN(void)
{
    BYTE key = 0;
    int inRet = d_NO;
    char szDebug[41] = {0},
    szAscBuf[5] = {0},
    szBcdBuf[3] = {0};

    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	BYTE Menukey = 0;

    srTransRec.fCUPPINEntry=FALSE;

	//if(strIIT.fCardPIN == FALSE)
        //return d_OK;

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        return d_OK;

    CTOS_LCDTClearDisplay();
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

    strcpy(szHeaderString, "CARD WITH PIN?");
    strcat(szChoiceMsg,"YES \n");
    strcat(szChoiceMsg,"NO");

	//inSetColorMenuMode();
    Menukey = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //Menukey = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE, 30); /*Menu with timeout parameter*/
    //Menukey = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE, inGetIdleTimeOut(TRUE)); /*Menu with timeout parameter*/
	vdDebug_LogPrintf("::inCTOS_SelectAccountType::MenuDisplayEx::Menukey[%d]", Menukey);
    inSetTextMode();

    if (Menukey > 0)
    {
        if(Menukey == 1)
        {
			srTransRec.fCUPPINEntry=TRUE;
            inRet = d_OK;
        }
        else if(Menukey == 2)
        {
			inRet = d_OK;
        }
        else if(Menukey == d_KBD_CANCEL)
        {
            vdSetErrorMessage("USER CANCEL");
			inRet = d_NO;
        }
		else if (Menukey == 0xFF) /*MCC: For timeout occured -- sidumili*/
		{
			inRet = d_NO;
		}
    }
    else
        inRet=d_NO;

	return inRet;
}

#ifdef UNFORK_APP
int inRecoverRAM(void)
{
   #define SAFE_LIMIT_SIZE 7000
   #define RESTART_LIMIT_SIZE 6000

   ULONG ulUsedDiskSize = 0;
   ULONG ulTotalDiskSize = 0;
   ULONG ulUsedRamSize = 0;
   ULONG ulTotalRamSize = 0;

   ULONG ulAvailableRamSize = 0;
   ULONG ulAvailDiskSize = 0;

   UCHAR szUsedDiskSize[50];
   UCHAR szTotalDiskSize[50];
   UCHAR szUsedRamSize[50];
   UCHAR szTotalRamSize[50];

   UCHAR szAvailableRamSize[50];
   UCHAR szAvailableDiskSize[50];

   int inCTR, inMainAppLimit=25, inSubAppLimit=10;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
       return d_OK;

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);


    vdDebug_LogPrintf("APP NAME %s",szGlobalAPName);

    if (ulAvailableRamSize < RESTART_LIMIT_SIZE)
    {
        vdDebug_LogPrintf("CTOS_SystemReset | RESTART_LIMIT_SIZE");
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 7, "RECLAIMING FLASH");
        CTOS_LCDTPrintXY(1, 8, "RESTARTING...");

		put_env_int("CRDTCTR",0);
		put_env_int("INSTCTR",0);
		put_env_int("DEBTCTR",0);
		put_env_int("DINCTR",0);
		put_env_int("DININSTCTR",0);

        CTOS_SystemReset();
    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE)
    {
    	vdDebug_LogPrintf("RECLAIMING FLASH | SAFE_LIMIT_SIZE");
        //CTOS_LCDTClearDisplay();
        //CTOS_LCDTPrintXY(1, 7, "RECLAIMING FLASH");
        //CTOS_LCDTPrintXY(1, 8, "RESTARTING...");

		put_env_int("MAINRESET",1);
        put_env_int("CRDTCTR",0);
        vdCTOSS_MainAPMemoryRecover();
    }


    if (strcmp (szGlobalAPName, "V5S_MCCNH") == 0)
    {

        inCTR = get_env_int("CRDTCTR");

        inCTR++;

        if (inCTR >= inMainAppLimit)
        {
        	vdDebug_LogPrintf("RECLAIMING FLASH | MAIN APP LIMIT");
			//CTOS_LCDTClearDisplay();
			//CTOS_LCDTPrintXY(1, 7, "RECLAIMING FLASH");
			//CTOS_LCDTPrintXY(1, 8, "RESTARTING...");

            put_env_int("CRDTCTR",0);
            vdDebug_LogPrintf("vdCTOSS_MainAPMemoryRecover %d %s", inCTR, szGlobalAPName);

            put_env_int("MAINRESET",1);
            vdCTOSS_MainAPMemoryRecover();
        }
        else
            put_env_int("CRDTCTR",inCTR);
    }
    else  if (strcmp(szGlobalAPName, "V5S_MCCINST") == 0)
    {
        inCTR = get_env_int("INSTCTR");
        inCTR++;

        if (inCTR >= inSubAppLimit)
        {
            put_env_int("INSTCTR",0);
            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            vdCTOSS_ReForkSubAP(szGlobalAPName);
        }
        else
           put_env_int("INSTCTR",inCTR);
    }
    else  if (strcmp (szGlobalAPName, "V5S_BANCNET") == 0)
    {

        inCTR = get_env_int("DEBTCTR");

        inCTR++;

        if (inCTR >= inSubAppLimit)
        {
            put_env_int("DEBTCTR",0);

            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            vdCTOSS_ReForkSubAP(szGlobalAPName);
        }
        else
            put_env_int("DEBTCTR",inCTR);
    }
    else if (strcmp (szGlobalAPName, "V5S_DINERS") == 0)
    {
        inCTR = get_env_int("DINCTR");

        inCTR++;

        if (inCTR >= inSubAppLimit)
        {
            put_env_int("DINCTR",0);

            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            vdCTOSS_ReForkSubAP(szGlobalAPName);
        }
        else
            put_env_int("DINCTR",inCTR);
    }
    else if (strcmp (szGlobalAPName, "V5S_DINERSINST") == 0)
    {
        inCTR = get_env_int("DININSTCTR");

        inCTR++;

        if (inCTR >= inSubAppLimit)
        {
            put_env_int("DININSTCTR",0);

            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            vdCTOSS_ReForkSubAP(szGlobalAPName);
        }
        else
            put_env_int("DININSTCTR",inCTR);
    }

   return d_OK;
}

int inReforkSubAPP(void)
{
    if (strcmp(szGlobalAPName, "V5S_MCCINST") == 0)
    {
        put_env_int("INSTCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);
        vdCTOSS_ReForkSubAP(szGlobalAPName);
    }
    else  if (strcmp (szGlobalAPName, "V5S_MCCDEBIT") == 0)
    {
        put_env_int("DEBTCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);
        vdCTOSS_ReForkSubAP(szGlobalAPName);
    }
    else  if (strcmp (szGlobalAPName, "V5S_CUP") == 0)
    {
        put_env_int("CUPCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);
        vdCTOSS_ReForkSubAP(szGlobalAPName);
    }

    return d_OK;
}
#endif UNFORK_APP

int inCTOSS_GetRAMMemorySize(char *Funname)
{
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

	int i = 0;

	/*disable inCTOSS_GetRAMMemorySize*/
	return d_OK;

	for (i = 0; i < 10000; i++)	//RAM usage is fluctuating, loop first
		;

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    vdDebug_LogPrintf("[%s], Used RAM[%ld], Total RAM[%ld], Available RAM[%ld]", Funname, ulUsedRamSize,ulTotalRamSize,ulAvailableRamSize);

    return d_OK;

}

void vdSetCtlsEntryImage(void)
{
	vdDebug_LogPrintf("--vdSetCtlsEntryImage--");

	inTCTRead(1);

	if (strTCT.byCtlsMode == 3)
	{
		if (strTCT.fManualEntry == 1)
			put_env_int("MCCMANUAL", 1);
		else
			put_env_int("MCCMANUAL", 0);
	}

	vdDebug_LogPrintf("byCtlsMode[%d] | fManualEntry[%d]", strTCT.byCtlsMode, strTCT.fManualEntry);

}

void vdSetPowerHighSpeed(void)
{

	vdDebug_LogPrintf("--vdSetPowerHighSpeed--");

	if (isCheckTerminalMP200() == d_OK)
	{
		vdSetsysPowerHighSpeed();

		vdDebug_LogPrintf("Set to vdSetsysPowerHighSpeed");
	}

}

int isCheckTerminalNonTouch(void)
{
    vdDebug_LogPrintf("--isCheckTerminalNonTouch,strTCT.byTerminalType=%d--",strTCT.byTerminalType);

    if (strTCT.byTerminalType == 3)
        return d_OK;

    return d_NO;
}

int inCTOS_PreConnectAndInit(void)
{
    int inRetVal;

	vdDebug_LogPrintf("--inCTOS_PreConnectAndInit--");
	vdDebug_LogPrintf("inCommunicationMode[%d]", strCPT.inCommunicationMode);
	vdDebug_LogPrintf("fGPRSConnectOK[%d]", fGPRSConnectOK);

    if(strCPT.inCommunicationMode == DIAL_UP_MODE || strCPT.inCommunicationMode == ETHERNET_MODE)
		return d_OK;

	if ((fGPRSConnectOK != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)) // flag to determine whether need do comm connection or not.
	{
		vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLEASE TRY AGAIN", DISPLAY_POSITION_CENTER);
		vdSetErrorMessages("GPRS PROBLEM","PLEASE TRY AGAIN");
		CTOS_Delay(2000);
		return d_NO;
	}

    vdDebug_LogPrintf("inCTOS_PreConnectAndInit1");
	vdSetInit_Connect(1);

    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
    {
		vdDisplayMultiLineMsgAlign("", "INITIALIZATION", "ERROR", DISPLAY_POSITION_CENTER);
		vdSetErrorMessages("INITIALIZATION","ERROR");
		vdSetErrorMessage("");
        return(d_NO);
    }

	if (get_env_int("CONNECTED") == 1)
		return(d_OK);

	if(VS_TRUE == strTCT.fDemo)
		return(d_OK);

	vdDebug_LogPrintf("inCTOS_PreConnectAndInit2");
    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

		if (inRetVal != d_OK)
		{
			if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
			{
				 vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLEASE TRY AGAIN", DISPLAY_POSITION_CENTER);
				 vdSetErrorMessages("GPRS PROBLEM","PLEASE TRY AGAIN");
				 vdSetErrorMessage("");
			}
			//wifi-mod2
			else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)
			{
				vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLEASE TRY AGAIN", DISPLAY_POSITION_CENTER);
				 vdSetErrorMessages("WIFI PROBLEM","PLEASE TRY AGAIN");
				vdSetErrorMessage("");
			}
			//wifi-mod2

			else
			{
				vdDisplayMultiLineMsgAlign("", "INITIALIZATION", "ERROR", DISPLAY_POSITION_CENTER);
				vdSetErrorMessages("INITIALIZATION","ERROR");
				vdSetErrorMessage("");
			}

			if(srTransRec.byTransType == SETTLE) {
				vdDisplayMultiLineMsgAlign("", "", "SETTLE FAILED", DISPLAY_POSITION_CENTER);
				vdSetErrorMessages("SETTLE FAILED","PLEASE TRY AGAIN");
				}
			return(d_NO);
		}

	vdDebug_LogPrintf("inCTOS_PreConnectAndInit3");

	if (CN_FALSE == srTransRec.byOffline)
    {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
	    //for improve transaction speed
    }

	put_env_int("CONNECTED",1);

	vdDebug_LogPrintf("inCTOS_PreConnectAndInit4");

    return(d_OK);
}

int inCTOS_SelectTelco(void)
{
    int inNumRecs = 0,
    key = 0,
    i,
    bHeaderAttr = 0x01+0x04;
    BYTE szHeaderString[21];
    BYTE szTelcoList[999] = {0};
    int inIndex=0;

	// Repository variable
	int  GPTid;
	BYTE szGPTName[30] ;
	BYTE szGPTAPN[30] ;
	BYTE szGPTUserName[30] ;
	BYTE szGPTPassword[30] ;
	BYTE szGPTPriTxnHostIP[30];
	BYTE szGPTSecTxnHostIP[30] ;
	BYTE szGPTPriSettlementHostIP[30];
	BYTE szGPTSecSettlementHostIP[30];
	int	inGPTPriTxnHostPortNum;
	int	inGPTSecTxnHostPortNum;
	int	inGPTPriSettlementHostPort;
	int	inGPTSecSettlementHostPort;
	int	inGPTAuthType;
	int	inGPTNetworkType;
	BOOL fGPTEnable;

	vdDebug_LogPrintf("--inCTOS_SelectTelco--");

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    //Select Telco
    memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szTelcoList, 0x00,sizeof(szTelcoList));
    strcpy(szHeaderString,"SELECT TELCO");

    memset(strMultiGPT, 0, sizeof(strMultiGPT));

	vdDebug_LogPrintf("sizeof(strMultiGPT):(%d)", sizeof(strMultiGPT));

    inGPTMultiRead(&inNumRecs);

	vdDebug_LogPrintf("inNumRecs(%d)", inNumRecs);

	if(inNumRecs == 0)
	{
    		CTOS_LCDTClearDisplay();
            	vdDisplayErrorMsg(1, 8, "TELCO UNAVAILABLE");
		return(ST_ERROR);
	}

	vdDebug_LogPrintf("----------");

	for (i = 0; i < inNumRecs; i++)
	{
		if (strMultiGPT[i].szGPTName[0] != 0)
		{
			inIndex=i;
			strcat((char *)szTelcoList, strMultiGPT[i].szGPTName);

			if (strMultiGPT[i+1].szGPTName[0] != 0)
				strcat((char *)szTelcoList, (char *)" \n");

			vdDebug_LogPrintf("i[%d] szTelcoList[%s]", i, szTelcoList);
		}
	}

    if(inNumRecs > 1)
        key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szTelcoList, TRUE);
    else
        key = inIndex+1;

	if (key == 0xFF)
	{
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(1500);
		return -1;
	}
	else if (key > 0)
	{
		if (key == d_KBD_CANCEL)
		{
			//vdDisplayErrorMsg(1, 8, "USER CANCEL");
			return -2;
		}

		GPTid = strMultiGPT[key-1].GPTid;
        strcpy(szGPTName, strMultiGPT[key-1].szGPTName);
		strcpy(szGPTAPN, strMultiGPT[key-1].szGPTAPN);
		strcpy(szGPTUserName, strMultiGPT[key-1].szGPTUserName);
		strcpy(szGPTPassword, strMultiGPT[key-1].szGPTPassword);
		strcpy(szGPTPriTxnHostIP, strMultiGPT[key-1].szGPTPriTxnHostIP);
		strcpy(szGPTSecTxnHostIP, strMultiGPT[key-1].szGPTSecTxnHostIP);
		strcpy(szGPTPriSettlementHostIP, strMultiGPT[key-1].szGPTPriSettlementHostIP);
		strcpy(szGPTSecSettlementHostIP, strMultiGPT[key-1].szGPTSecSettlementHostIP);
		inGPTPriTxnHostPortNum = strMultiGPT[key-1].inGPTPriTxnHostPortNum;
		inGPTSecTxnHostPortNum = strMultiGPT[key-1].inGPTSecTxnHostPortNum;
		inGPTPriSettlementHostPort = strMultiGPT[key-1].inGPTPriSettlementHostPort;
		inGPTSecSettlementHostPort = strMultiGPT[key-1].inGPTSecSettlementHostPort;
		inGPTAuthType = strMultiGPT[key-1].inGPTAuthType;
		inGPTNetworkType = strMultiGPT[key-1].inGPTNetworkType;

		vdDebug_LogPrintf("szGPTName:(%s)\nkey:(%d)", szGPTName, key);
	}

	vdDebug_LogPrintf("GPTid[%d]", GPTid);
	vdDebug_LogPrintf("szGPTName[%s]", szGPTName);
	vdDebug_LogPrintf("szGPTAPN[%s]", szGPTAPN);
	vdDebug_LogPrintf("szGPTUserName[%s]", szGPTUserName);
	vdDebug_LogPrintf("szGPTPassword[%s]", szGPTPassword);
	vdDebug_LogPrintf("szGPTPriTxnHostIP[%s]", szGPTPriTxnHostIP);
	vdDebug_LogPrintf("szGPTSecTxnHostIP[%d]", szGPTSecTxnHostIP);
	vdDebug_LogPrintf("szGPTPriSettlementHostIP[%s]", szGPTPriSettlementHostIP);
	vdDebug_LogPrintf("szGPTSecSettlementHostIP[%s]", szGPTSecSettlementHostIP);
	vdDebug_LogPrintf("inGPTPriTxnHostPortNum[%d]", inGPTPriTxnHostPortNum);
	vdDebug_LogPrintf("inGPTSecTxnHostPortNum[%d]", inGPTSecTxnHostPortNum);
	vdDebug_LogPrintf("inGPTPriSettlementHostPort[%d]", inGPTPriSettlementHostPort);
	vdDebug_LogPrintf("inGPTSecSettlementHostPort[%d]", inGPTSecSettlementHostPort);
	vdDebug_LogPrintf("inGPTAuthType[%d]", inGPTAuthType);
	vdDebug_LogPrintf("inGPTNetworkType[%d]", inGPTNetworkType);

	if (GPTid > 0)
	{
		vduiClearBelow(2);
		CTOS_LCDTPrintXY(1, 7, "Configuring Telco");
		CTOS_LCDTPrintXY(1, 8, "Please wait...");

   	    //usCTOSS_LCDDisplay(" |Configuring Telco\nPlease wait...");
   	    vdDisplayMessageStatusBox(1, 8, "Configuring telco", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		CTOS_Delay(1000);
		
		inTCPRead(1);

		strcpy(strTCP.szAPN, szGPTAPN);
		strcpy(strTCP.szUserName, szGPTUserName);
		strcpy(strTCP.szPassword, szGPTPassword);

		put_env_int("GSMAUTHTYPE", inGPTAuthType);
		put_env_int("NETWORKTYPE", inGPTNetworkType);

		inTCPSave(1);

		// Update CPT Table
		inCPT_IPUpdate(szGPTPriTxnHostIP, szGPTSecTxnHostIP, szGPTPriSettlementHostIP, szGPTSecSettlementHostIP, inGPTPriTxnHostPortNum, inGPTSecTxnHostPortNum, inGPTPriSettlementHostPort, inGPTSecSettlementHostPort);

		// Update SHARLS_ERM IP
		if (strTCT.byERMMode != 0)
			inERMTCT_IPUpdate(szGPTPriTxnHostIP, szGPTSecTxnHostIP, inGPTPriTxnHostPortNum, inGPTSecTxnHostPortNum, 1);

		vduiClearBelow(2);
		CTOS_LCDTPrintXY(1, 7, "Configuring Telco");
		CTOS_LCDTPrintXY(1, 8, "Done!            ");

		vdDisplayMessageBox(1, 8, "Configuring Telco", "Done!", "", MSG_TYPE_SUCCESS);
		
		CTOS_Beep();
	    CTOS_Delay(1000);
	}

	CTOS_LCDTClearDisplay();

	return(d_OK);
}

void vdGetTimeDate1(TRANS_DATA_TABLE *srTransPara)
{
	CTOS_RTC SetRTC;
	BYTE szCurrentTime[20];

	vdDebug_LogPrintf("--vdGetTimeDate1--");

	CTOS_RTCGet(&SetRTC);
	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
	wub_str_2_hex(szCurrentTime,srTransPara->szDate,DATE_ASC_SIZE);
	vdDebug_LogPrintf("Date = [%s]", szCurrentTime);

	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	wub_str_2_hex(szCurrentTime,srTransPara->szTempTime,TIME_ASC_SIZE);
	vdDebug_LogPrintf("Time = [%s]", szCurrentTime);

}

void vdGetCRC(void)
{
	FILE* file;
	int nLen;
	unsigned char chBuffer[1024];
	BYTE baTemp[10+1];
	//BOOL fHD = TRUE;
	BOOL fHD = FALSE;

	vdDebug_LogPrintf("--vdGetCRC--");

	memset(chBuffer, 0, 1024);

	if ((file = fopen ("V5S_MCCNH", "rb")) != NULL)
	{
		while (nLen = fread (chBuffer, 1, 512, file))
		{
		}

		memset(baTemp, 0x00, sizeof(baTemp));
		sprintf(baTemp, "%02x%02x%02x%02x",
			wub_lrc((unsigned char *) &chBuffer[0], 128),
			wub_lrc((unsigned char *) &chBuffer[127], 128),
			wub_lrc((unsigned char *) &chBuffer[255], 128),
			wub_lrc((unsigned char *) &chBuffer[511], 128));


		if (fHD)
		{
			//put_env_char("V5S_MCCNH_CRC", "13283af4");
			//put_env_char("V5S_MCCNH_CRC", "352b3af9");
			put_env_char("V5S_MCCNH_CRC", "253a3a76");
		}
		else
		{
			put_env_char("V5S_MCCNH_CRC",baTemp);
		}

		fclose (file);
	}

}

void vdCTOSS_PackOfflinepinDisplay(OFFLINEPINDISPLAY_REC *szDisplayRec)
{
	BYTE szCurSymbol[20+1], szTotalAmount[20+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
	int inCount=0;

	vdDebug_LogPrintf("--vdCTOSS_PackOfflinepinDisplay--");
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);

	szDisplayRec->inDispFlag = 1;
	szGetTransTitle(srTransRec.byTransType, szDisplayRec->szDisplay1);

	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
	memset(szAmtBuff,0x00,sizeof(szAmtBuff));
	memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));

	strcpy(szDisplayRec->szDisplay3,"TOTAL:");

	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);

	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmount, szAmtBuff);

	sprintf(szCurAmtBuff,"%s %s",strCST.szCurSymbol, szAmtBuff);
	strcpy(szDisplayRec->szDisplay4,szCurAmtBuff);

	strcpy(szDisplayRec->szDisplay6,"Enter PIN:");
}

int inCTOS_ERMPreConnect(BOOL fDisplay)
{
    int inRetVal, inReturn;

    vdDebug_LogPrintf("--inCTOS_ERMPreConnect--");
	vdDebug_LogPrintf("fDisplay[%d]", fDisplay);

	inERMTCT_IPTRead(1);

    srTransRec.usTerminalCommunicationMode = strERMTCT.inCommMode;

	strcpy(strCPT.szPriTxnHostIP, strERMTCT.szPriTxnHostIP);
	strcpy(strCPT.szSecTxnHostIP, strERMTCT.szSecTxnHostIP);
	strCPT.inPriTxnHostPortNum = strERMTCT.inPriTxnHostPortNum;
	strCPT.inSecTxnHostPortNum = strERMTCT.inSecTxnHostPortNum;

	vdDebug_LogPrintf("fShareComEnable[%d]",strTCT.fShareComEnable);
	vdDebug_LogPrintf("Primary[%s]port[%ld]",strCPT.szPriTxnHostIP,strCPT.inPriTxnHostPortNum);
	vdDebug_LogPrintf("Secondary[%s]port[%ld]",strCPT.szSecTxnHostIP,strCPT.inSecTxnHostPortNum);

    // Minimize the white screend display -- sidumili
    if (fDisplay)
		vdCTOS_DispStatusMessage("PROCESSING...");

    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
    {
        vdSetErrorMessage("COMM INIT ERR");
        return(d_NO);
    }

    inReturn = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

	if (srTransRec.usTerminalCommunicationMode == GPRS_MODE && !fGPRSConnectOK)
		inReturn = d_NO;

	vdDebug_LogPrintf("inCTOS_CheckInitComm | inReturn[%d]|fGPRSConnectOK[%d]", inReturn, fGPRSConnectOK);

	if (inReturn != d_OK)
	{
		switch (srTransRec.usTerminalCommunicationMode )
		{
			case GPRS_MODE:
				vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLEASE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdSetErrorMessages("GPRS PROBLEM","PLEASE TRY AGAIN");
				break;
			case WIFI_MODE:
				vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLEASE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdSetErrorMessages("WIFI PROBLEM","PLEASE TRY AGAIN");
				break;
			case ETHERNET_MODE:
				vdDisplayMultiLineMsgAlign("", "ETHERNET PROBLEM", "PLEASE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdSetErrorMessages("ETHERNET PROBLEM","PLEASE TRY AGAIN");
				break;
			default:
				vdDisplayMultiLineMsgAlign("", "COMM INIT", "ERROR", DISPLAY_POSITION_CENTER);
				vdSetErrorMessages("COMM INIT","ERROR");
				break;
		}

		vdSetErrorMessage("");

		return inReturn;
	}

    if (CN_FALSE == srTransRec.byOffline)
    {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
    }

    return(d_OK);
}

BOOL isERMMode(void)
{
	BOOL isERM = FALSE;

	vdDebug_LogPrintf("--isERMMode--");
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);

	if (strTCT.byERMMode != 0)
		isERM = TRUE;

	return isERM;
}

int inCTOS_DisplayFSRMode(void)
{
	BOOL fFSR = FALSE;
	int iFSR = get_env_int("FSRMODE");
	BYTE szTemp[40 + 1] = {0};

	memset(szTemp, 0x00, sizeof(szTemp));
	switch (iFSR)
	{
		case 1:
			sprintf(szTemp, "FSR MODE-CITAS SVC");
			fFSR = TRUE;
			break;
		case 2:
			sprintf(szTemp, "FSR MODE-MERCH SVC");
			fFSR = TRUE;
			break;
		case 3:
			sprintf(szTemp, "FSR MODE-MERCH ORIENT");
			fFSR = TRUE;
			break;
		default:
			fFSR = FALSE;
			break;
	}

	if (fFSR)
	{
		CTOS_LCDTSelectFontSize(d_FONT_16x16);
		CTOS_LCDTPrintXY (1, 5, szTemp);
		CTOS_LCDTSelectFontSize(d_FONT_16x30);
	}

	return d_OK;
}

void vdSetFSRMode(BYTE byTransType)
{
	vdDebug_LogPrintf("--vdSetFSRMode--");
	vdDebug_LogPrintf("byTransType[%d]", byTransType);

	switch (byTransType)
	{
		case CITAS_SERVICES:
			put_env_int("FSRMODE", 1);
			break;
		case MERCHANT_SERVICES:
			put_env_int("FSRMODE", 2);
			break;
		case MERCHANT_ORIENT:
			put_env_int("FSRMODE", 3);
			break;
		default:
			put_env_int("FSRMODE", 0);
			break;
	}

}

BOOL fFSRMode(void)
{
	BOOL fFSR = FALSE;
	int iFSR = get_env_int("FSRMODE");

	vdDebug_LogPrintf("--fFSRMode--");
	vdDebug_LogPrintf("iFSR[%d]", iFSR);

	if (iFSR > 0)
		fFSR = TRUE;

	return fFSR;

}

void SetTransYear(void)
{
	CTOS_RTC SetRTC;
	char szYear[4 + 1];

	vdDebug_LogPrintf("--SetTransYear--");

	memset(szYear, 0x00, sizeof(szYear));
	CTOS_RTCGet(&SetRTC);
	sprintf(szYear, "%02lu", SetRTC.bYear);
	vdDebug_LogPrintf("szYear[%s]", szYear);

	memset(srTransRec.szTranYear, 0x00, sizeof(srTransRec.szTranYear));
	strcpy(srTransRec.szTranYear, szYear);

	vdDebug_LogPrintf("srTransRec.szTranYear[%s]", srTransRec.szTranYear);
}

int inDCCRateRequest(void)
{
    int inResult,i;
    char szErrorMessage[30+1];
    char szBcd[INVOICE_BCD_SIZE+1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;

	BYTE szTitle[25+1];
	BYTE szDisMsg[150];

    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
        srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;
    inHDTSave(strHDT.inHostIndex);

    srTransPara = &srTransRec;

//TINE:  remove in android app
#if 0
	if ((strTCT.byTerminalType % 2) == 0)
         {
              setLCDPrint(6, DISPLAY_POSITION_LEFT, "CONNECTING TO");
	     setLCDPrint(7, DISPLAY_POSITION_LEFT, "FX RATE HOST");
         }	
	else
	{
	     setLCDPrint(4, DISPLAY_POSITION_LEFT, "CONNECTING TO");
	     setLCDPrint(5, DISPLAY_POSITION_LEFT, "FX RATE HOST");
	}
#endif

	memset(szTitle, 0x00, sizeof(szTitle));
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "CONNECTING TO FX RATE HOST");

	//usCTOSS_LCDDisplay(szDisMsg);
	vdDisplayMessageBox(1, 8, "CONNECTING TO FX RATE", MSG_PLS_WAIT, "", MSG_TYPE_INFO);
	CTOS_Delay(1000);
	
//TINE - end

	srTransPara->byOffline = CN_FALSE;

    if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
    {
        if(srTransPara->shTransResult == 0)
            srTransPara->shTransResult = TRANS_COMM_ERROR;
        inCTOS_inDisconnect();
        vdSetErrorMessage("CONNECT FAILED");
        return RATE_REQUEST_CONN_FAIL;
    }

    vdIncSTAN(srTransPara);

	if ((inResult = inBuildOnlineMsg(srTransPara)) != ST_SUCCESS)
	{
		vdDebug_LogPrintf("inDCCRateRequest:  inBuildOnlineMsg, inResult = [%d]", inResult);
		inTCTSave(1);
		vdDebug_LogPrintf("AAA - inResult[%d]", inResult);
		if (ST_RESP_MATCH_ERR == inResult)
		{
			vdDebug_LogPrintf("inBuildOnlineMsg %d",inResult);
			return inResult;
		}
		return inResult;
	}
	else if ((srTransPara->shTransResult!=ST_SUCCESS) && (srTransPara->shTransResult!=TRANS_AUTHORIZED) &&  ((srTransPara->byTransType==DCC_MERCH_RATE_REQ) ||(srTransPara->byTransType==DCC_RATE_REQUEST))) 
	{
	     vdDebug_LogPrintf("AAA - srTransPara->shTransResult[%d]", srTransPara->shTransResult);
	     inCTOS_inDisconnect();
	     return ST_ERROR;
	}
    else
    {
		vdDebug_LogPrintf("RATE SUCCESSFUL");
		if ((strTCT.byTerminalType % 2) == 0)
        {
            //setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, " RATE SUCCESSFUL");
			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);

			//strcpy(szDisMsg, szTitle);
			strcpy(szDisMsg, "SALE1");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "RATE SUCCESSFUL");
			CTOS_Delay(1500);
        }
        else
        {
            //setLCDPrint(8, DISPLAY_POSITION_LEFT, " RATE SUCCESSFUL");
			//CTOS_Delay(1000);
			
			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);

			//strcpy(szDisMsg, szTitle);
			strcpy(szDisMsg, "SALE2");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "RATE SUCCESSFUL");
			CTOS_Delay(1500);
        }

		inCTOS_inDisconnect();
    }

    return ST_SUCCESS;
}

int inDCCDisplayFxRate (char * szDCCTransactionType)
{
     BYTE key;
     unsigned char szIncludesExchangeRate[100], szExchangeRate[100], szDCCCurrencySymAmt[15+1], szLocalCurrencySymAmt[45], szLocalCurrencySymAmtX[45], szTotalAmt[12+1], szTotalAmountformat[45], szTotalDCCAmountformat[45], szDCCCurrencySymAmtOK[18+1], szDCCAmt[12+1];
     int inMinorUnit;
     unsigned long inDCCAmount, ldinTipAmount;// ldDccAmtTotal;
     BYTE szAmtBuff[20+1], szTipAmtBuff[20+1], szTipAmountFmt[20+1], szDCCTip[20+1], szTipDCCAmtBuff[20+1], szAmtBuff2[20+1]; 
     double ulDCCForeignAmt={0}, ulDCCForeignTipAmt={0}, ldDccAmtTotal={0}, flRate={0}, flDCCTip={0}, flTipAmount={0}, flTipAmountFmt={0}, flDCCTipRndOff={0}, flDCCForeignAmount3MinorUnits={0}, flDCCForeignAmountLessTip3MinorUnits={0};
     BYTE szTipAmount[20], szDCCTipAmount[20], szDCCTipAmtRndOff[20+1];
     BYTE szformatTipAmount[20];

     char * ptr;
     char * ptr2;
     int indecimalplace, intiplen, indelperiod, inAmtTiplen, inAmtlen,  x;
     BYTE szTipAmtBuffBatch[12+1], szAmtBuffBatch[12+1], szBaseAmtBuffBatch[12+1];
     // unsigned long ulDCCForeignAmt, ulDCCForeignTipAmt;
     BYTE szDccAmtTotal[12+1], szDccAmtTotalFormatted[12+1], szDccAmtTotalBuff[20+1], szDCCForeignAmountLessTip3MinorUnits[12+1];

	 //TINE:  android
	 BYTE szDCCFxRateDetails[512];
	 BYTE inKey;
	 
	 int inTimeOut = 0;
	 BYTE szTimeOut[3]; 
	 
	
     TRANS_DATA_TABLE *srTransPara;

     srTransPara = &srTransRec;

     //vdClearNonTitleLines();
     //vduiClearBelow(2);
     usCTOSS_LCDDisplay(" ");

   vdDebug_LogPrintf("inDCCDisplayFxRate: inCSTReadCurrencySymbol -- START");
   inCSTReadCurrencySymbol(srTransPara->szCurrencyCode, srTransPara->szDCCCurrencySymbol, srTransPara->szAmountFormat);
   vdDebug_LogPrintf("inDCCDisplayFxRate: inCSTReadCurrencySymbol -- END");
   srTransPara->inMinorUnit = strCST.inMinorUnit;
   wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
   vdDebug_LogPrintf("inDCCDisplayFxRate:  szTotalAmt = [%s]", szTotalAmt);
   memset(szTotalAmountformat, 0x00, sizeof(szTotalAmountformat));
   vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szTotalAmountformat);
   vdDebug_LogPrintf("inDCCDisplayFxRate:  szTotalAmountformat = [%s]", szTotalAmountformat);

   //vdDebug_LogPrintf("srTransPara->szExchangeRate (%s)", srTransRec.szExchangeRate);
   vdDebug_LogPrintf("srTransPara->szExchangeRate (%s)", srTransPara->szExchangeRate);
   flRate=atof((char*)srTransPara->szExchangeRate);
   //flRate=atof((char*)srTransRec.szExchangeRate);
   wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
	
   ldinTipAmount=atol((char*)szTipAmount); //dcc local tip amt long
   sprintf(szTipAmtBuff,"%ld",ldinTipAmount); //dcc local tip amt char
   memset(szTipAmountFmt, 0x00, sizeof(szTipAmountFmt));
   vdCTOS_FormatAmount("NNNNNNNNn.nn", szTipAmtBuff, szTipAmountFmt); //dcc local tip amt format char
   vdDebug_LogPrintf("inDCCDisplayFxRate:  szTipAmountFmt = [%s]", szTipAmountFmt);

   flTipAmountFmt = atof((char*)szTipAmountFmt); //dcc local tip amt format float

   flDCCTip = flRate * flTipAmountFmt; //dcc foreign tip amt float (flrate x fltipamountfmt)
   vdDebug_LogPrintf("AAA - flDCCTip[%lf], flRate[%lf], flTipAmountFmt[%lf], indelperiod[%d]", flDCCTip, flRate, flTipAmountFmt);
   sprintf (szDCCTip,"%.02f",flDCCTip); //dcc foreign tip amt char

   vdDebug_LogPrintf("AAA - szDCCTip[%s]", szDCCTip);

   memset(szDCCTipAmtRndOff,0,sizeof(szDCCTipAmtRndOff));
   memset(szTipAmtBuffBatch,0,sizeof(szTipAmtBuffBatch));

   flDCCTipRndOff = atof(szDCCTip);
   //if(srTransRec.inMinorUnit==0)
   if(srTransPara->inMinorUnit==0)
       sprintf(szDCCTipAmtRndOff,"%.f",flDCCTipRndOff);
   else 
   {
        sprintf(szDCCTipAmtRndOff,"%.02f",flDCCTipRndOff);
        ptr = strchr(szDCCTipAmtRndOff,'.');
        indecimalplace = strlen(ptr);
        intiplen = strlen(szDCCTipAmtRndOff);
        indelperiod = intiplen - indecimalplace;
        indecimalplace--;
        vdDebug_LogPrintf("AAA - ptr[%s], indecimalplace[%d], intiplen[%d], indelperiod[%d]", ptr, indecimalplace, intiplen, indelperiod);
        memmove(&szDCCTipAmtRndOff[indelperiod], &szDCCTipAmtRndOff[indelperiod + 1], intiplen - indelperiod);
        vdDebug_LogPrintf("AAA - szDCCTipAmtRndOff[%s]  strlen(szDCCTipAmtRndOff)[%d]", szDCCTipAmtRndOff,  strlen(szDCCTipAmtRndOff));
   }
   
   inAmtTiplen = 12 - strlen(szDCCTipAmtRndOff);

   vdDebug_LogPrintf("AAA - inAmtTiplen[%d]", inAmtTiplen);

   x=0;
   while (x <  inAmtTiplen)
   {
       szTipAmtBuffBatch[x] = '0';
       x++;
   }

   strcat(&szTipAmtBuffBatch[inAmtTiplen], szDCCTipAmtRndOff);
   strcpy(srTransPara->szDCCTipAmount, szTipAmtBuffBatch);
   vdDebug_LogPrintf("AAA - szTipAmtBuffBatch[%s]", szTipAmtBuffBatch);

   vdDebug_LogPrintf("srTransPara->szForeignAmount = [%s]", srTransPara->szForeignAmount);
   ulDCCForeignAmt = atof (&srTransPara->szForeignAmount);
   //ulDCCForeignAmt = atof (srTransRec.szForeignAmount);
   vdDebug_LogPrintf("ulDCCForeignAmt = [%f]", ulDCCForeignAmt);
   ulDCCForeignTipAmt = atof(srTransPara->szDCCTipAmount);
   //ulDCCForeignTipAmt = atof(srTransRec.szDCCTipAmount);
   vdDebug_LogPrintf("ulDCCForeignTipAmt = [%f]", ulDCCForeignTipAmt);

   memset(szDccAmtTotalBuff, 0, sizeof(szDccAmtTotalBuff));
   ldDccAmtTotal = 0;
    //if (srTransRec.inMinorUnit!=3)
    if (srTransPara->inMinorUnit!=3)
    {
        vdDebug_LogPrintf("srTransRec.inMinorUnit!=3");
        ldDccAmtTotal = ulDCCForeignAmt - ulDCCForeignTipAmt;
        vdDebug_LogPrintf("AAA >> ldDccAmtTotal[%lf], ulDCCForeignAmt[%lf], ulDCCForeignTipAmt[%lf] srTransPara->szForeignAmount[%s], srTransPara->szDCCTipAmount[%s]", ldDccAmtTotal, ulDCCForeignAmt, ulDCCForeignTipAmt, srTransPara->szForeignAmount, srTransPara->szDCCTipAmount);
        //vdDebug_LogPrintf("AAA >> ldDccAmtTotal[%lf], ulDCCForeignAmt[%lf], ulDCCForeignTipAmt[%lf] srTransPara->szForeignAmount[%s], srTransPara->szDCCTipAmount[%s]", ldDccAmtTotal, ulDCCForeignAmt, ulDCCForeignTipAmt, srTransRec.szForeignAmount, srTransRec.szDCCTipAmount);
        sprintf(szDccAmtTotalBuff,"%012.0lf",ldDccAmtTotal);
		  
        if(strlen(szDccAmtTotalBuff) > 12)
            szDccAmtTotalBuff[11]='\0';
		 
        vdDebug_LogPrintf("AAA - szDccAmtTotalBuff[%s]", szDccAmtTotalBuff);
     }
    else
    {
      vdDebug_LogPrintf("AAA - srTransPara->szForeignAmount[%s], szTotalDCCAmountformat[%s]", srTransPara->szForeignAmount, szTotalDCCAmountformat);
      //vdDebug_LogPrintf("AAA - srTransPara->szForeignAmount[%s], szTotalDCCAmountformat[%s]", srTransRec.szForeignAmount, szTotalDCCAmountformat);
      vdCTOS_FormatAmount("NNNNNNNNNn.nnn",srTransPara->szForeignAmount,szTotalDCCAmountformat);
      //vdCTOS_FormatAmount("NNNNNNNNNn.nnn",srTransRec.szForeignAmount,szTotalDCCAmountformat);
	  vdDebug_LogPrintf("AAA - szTotalDCCAmountformat[%s]", szTotalDCCAmountformat);
	  flDCCForeignAmount3MinorUnits = atof(szTotalDCCAmountformat);
	  vdDebug_LogPrintf("AAA - flDCCForeignAmount3MinorUnits[%f], flDCCTipRndOff[%f]", flDCCForeignAmount3MinorUnits, flDCCTipRndOff);
	  flDCCForeignAmountLessTip3MinorUnits = flDCCForeignAmount3MinorUnits - flDCCTipRndOff;

	  sprintf(szDCCForeignAmountLessTip3MinorUnits,"%.03f", flDCCForeignAmountLessTip3MinorUnits);
	  vdDebug_LogPrintf("AAA - szDCCForeignAmountLessTip3MinorUnits[%s] flDCCForeignAmountLessTip3MinorUnits[%f]", szDCCForeignAmountLessTip3MinorUnits, flDCCForeignAmountLessTip3MinorUnits);
	
    
	  indecimalplace = 0;
	  intiplen = 0;
	  indelperiod = 0;
    	   
	  ptr2 = strchr(szDCCForeignAmountLessTip3MinorUnits,'.');
	  indecimalplace = strlen(ptr2);
	  intiplen = strlen(szDCCForeignAmountLessTip3MinorUnits);
	  indelperiod = intiplen - indecimalplace;
    
	  vdDebug_LogPrintf("AAA - ptr2[%s], indecimalplace[%d], intiplen[%d], indelperiod[%d]", ptr2, indecimalplace, intiplen, indelperiod);
	  memmove(&szDCCForeignAmountLessTip3MinorUnits[indelperiod], &szDCCForeignAmountLessTip3MinorUnits[indelperiod + 1], intiplen - indelperiod);

	  inAmtTiplen = 0;
	   x = 0;
			
	   inAmtTiplen = 12 - strlen(szDCCForeignAmountLessTip3MinorUnits);
	   vdDebug_LogPrintf("AAA2 - inAmtTiplen[%d]", inAmtTiplen);
            
	   while (x <  inAmtTiplen)
	   {
	       szBaseAmtBuffBatch[x] = '0';
	       x++;
	   }
            
	   strcat(&szBaseAmtBuffBatch[inAmtTiplen], szDCCForeignAmountLessTip3MinorUnits);
	   strcpy(srTransPara->szBaseForeignAmount, szBaseAmtBuffBatch);
	   vdDebug_LogPrintf("AAA - szBaseAmtBuffBatch[%s]", szBaseAmtBuffBatch);
			
	   strcpy(szDccAmtTotalBuff, srTransPara->szBaseForeignAmount);
	   vdDebug_LogPrintf("AAA - szDccAmtTotalBuff[%s], srTransPara->szBaseForeignAmount[%s]", szDccAmtTotalBuff, srTransPara->szBaseForeignAmount);
          
    }
      
	
   //vdDebug_LogPrintf("AAA >> ldDccAmtTotal[%ld], ulDCCForeignAmt[%ld], ulDCCForeignTipAmt[%ld]", ldDccAmtTotal, ulDCCForeignAmt, ulDCCForeignTipAmt);

   //sprintf(szDccAmtTotalBuff,"%ld",ldDccAmtTotal);

   vdDebug_LogPrintf("AAA - ldDccAmtTotal[%lf] szDccAmtTotalBuff[%s] strlen(szDccAmtTotalBuff)[%d]", ldDccAmtTotal, szDccAmtTotalBuff, strlen(szDccAmtTotalBuff));

   inAmtlen = 12 - strlen(szDccAmtTotalBuff);
   vdDebug_LogPrintf("AAA - inAmtlen[%d]",inAmtlen);

   x = 0;

   memset(szAmtBuffBatch,0,sizeof(szAmtBuffBatch));

    while (x <  inAmtlen)
   	{
   	    szAmtBuffBatch[x] = '0';
	    x++;
   	}

   strcat(&szAmtBuffBatch[inAmtlen], szDccAmtTotalBuff);
   strcpy(srTransPara->szBaseForeignAmount, szAmtBuffBatch);

   vdDebug_LogPrintf("AAA - srTransPara->szBaseForeignAmount[%s]", srTransPara->szBaseForeignAmount);

   //inCSTReadCurrencySymbol(srTransPara->szCurrencyCode, srTransPara->szDCCCurrencySymbol, srTransPara->szAmountFormat);
   sprintf(szDccAmtTotal,"%lf",ldDccAmtTotal);
   vdCTOS_FormatAmount(srTransPara->szAmountFormat, szDccAmtTotal, szDccAmtTotalFormatted);
   
   vdDebug_LogPrintf("AAA -ldDccAmtTotal[%lf] szDccAmtTotalFormatted[%s] ", ldDccAmtTotal, szDccAmtTotalFormatted);
  
    //gblinMinorUnit=strCST.inMinorUnit;
   
    vdCTOS_FormatAmount(srTransPara->szAmountFormat, (char*)srTransPara->szForeignAmount, &szAmtBuff);
    if(srTransPara->inMinorUnit==0)
		purgeleadingchar('0',szAmtBuff,szAmtBuff2);
	
    memset(szIncludesExchangeRate, 0, sizeof(szIncludesExchangeRate));
    memset(szExchangeRate, 0, sizeof(szExchangeRate));
	
     sprintf(szIncludesExchangeRate,"INCLUDES %s MARGIN", srTransPara->szMarginRatePercentage);
     sprintf(szExchangeRate,"%s", srTransPara->szExchangeRate);
     vdDebug_LogPrintf("AAA - szExchangeRate[%s] srTransPara->szExchangeRate[%s]", szExchangeRate, srTransPara->szExchangeRate);
     if(srTransPara->inMinorUnit==0)
          sprintf(szDCCCurrencySymAmt,"%s %s", srTransPara->szDCCCurrencySymbol, szAmtBuff2);
     else
          sprintf(szDCCCurrencySymAmt,"%s %s", srTransPara->szDCCCurrencySymbol, szAmtBuff);
     sprintf(szLocalCurrencySymAmt, "%s %s",strCST.szCurSymbol,szTotalAmountformat);
     sprintf(gblszDCCCurrBaseAmount, "%s %s", strCST.szCurSymbol, szDccAmtTotalFormatted);
     sprintf(gblszCurrDCCTipAmount, "%s %s",srTransPara->szDCCCurrencySymbol, szDCCTip);
     sprintf(szDCCCurrencySymAmtOK,"%s", szDCCCurrencySymAmt);
     sprintf(szLocalCurrencySymAmtX, "%s",szLocalCurrencySymAmt);

    memcpy(&gblszDCCCurrBaseAmount,szDccAmtTotalFormatted,sizeof(gblszDCCCurrBaseAmount));
    memcpy(&gblszForeignAmount,(char *)srTransPara->szForeignAmount,sizeof(gblszForeignAmount));
    memcpy(&gblszMarginRatePercentage,(char *)srTransPara->szMarginRatePercentage,sizeof(gblszMarginRatePercentage));
    memcpy(&gblszExchangeRate,(char *)srTransPara->szExchangeRate,sizeof(gblszExchangeRate));
    memcpy(&gblszDCCCurrencySymbol, (char *)srTransPara->szDCCCurrencySymbol,sizeof(gblszDCCCurrencySymbol));
    memcpy(&gblszLocalAmount,(char *)szLocalCurrencySymAmt,sizeof(gblszLocalAmount));
    memcpy(&gblszCurrForeignAmount,(char *)szDCCCurrencySymAmt,sizeof(gblszCurrForeignAmount));

     //vdDebug_LogPrintf("AAA gblszForeignAmount[%s], gblszCurrForeignAmount[%s]", gblszForeignAmount, gblszCurrForeignAmount);
    vdDebug_LogPrintf("AAA srTransPara->inMinorUnit[%d]", srTransPara->inMinorUnit);

     if(strTCT.inPrintDCCChooseCurrency== 1)
           vdPrintDCCChooseCurrency(szDCCTransactionType);

     //TINE:  android - removed 24SEP2019
#if 0
      vdDispTransTitle(srTransRec.byTransType);
      CTOS_LCDTSelectFontSize(d_FONT_12x24);    
      vduiDisplayStringCenter(3, srTransPara->szFXSourceName);
      vduiDisplayStringCenter(4, szIncludesExchangeRate);
      memset(szExchangeRate, 0, sizeof(szExchangeRate));
      sprintf(szExchangeRate,"       EXCH RATE: %s", srTransPara->szExchangeRate);
      vdDebug_LogPrintf("AAA - szExchangeRate[%s] srTransPara->szExchangeRate[%s]", szExchangeRate, srTransPara->szExchangeRate);
      vduiDisplayStringCenter(5, szExchangeRate);
      vduiDisplayStringCenter(6, "     SELECT CURRENCY");
      inDisplayLeftRight(7, szDCCCurrencySymAmtOK," ",20);
      inDisplayLeftRight(8, szLocalCurrencySymAmtX," ",20);
      CTOS_LCDTSelectFontSize(d_FONT_16x30);
       //vduiDisplayStringCenter(9, szDCCCurrencySymAmtOK);
      //vduiDisplayStringCenter(10, szLocalCurrencySymAmtX);
#endif

	memset(szExchangeRate, 0, sizeof(szExchangeRate));
	
    if((srTransRec.IITid == VISA_ISSUER) || (srTransRec.IITid == MASTERCARD_ISSUER))
         sprintf(szExchangeRate,"1 %s = %s %s", srTransPara->szDCCCurrencySymbol, gblszPrintExchangeRate, strCST.szCurSymbol);
    else
         sprintf(szExchangeRate,"%s", srTransPara->szExchangeRate);

	vdDebug_LogPrintf("AAA - szExchangeRate[%s] srTransPara->szExchangeRate[%s]", szExchangeRate, srTransPara->szExchangeRate);
    //vdDebug_LogPrintf("srTransRec.byTransType [%s]",srTransRec.byTransType);
    vdDebug_LogPrintf("srTransRec.byTransType [%s]",szDCCTransactionType);
    vdDebug_LogPrintf("srTransPara->szFXSourceName [%s]",srTransPara->szFXSourceName);
    vdDebug_LogPrintf("szIncludesExchangeRate [%s]",szIncludesExchangeRate);
    vdDebug_LogPrintf("szDCCCurrencySymAmtOK [%s]",szDCCCurrencySymAmtOK);
    vdDebug_LogPrintf("szLocalCurrencySymAmtX [%s]",szLocalCurrencySymAmtX);
	  
	memset(szDCCFxRateDetails, 0, sizeof(szDCCFxRateDetails));
	strcpy(szDCCFxRateDetails, szDCCTransactionType);
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, srTransPara->szFXSourceName);
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, szIncludesExchangeRate);
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, "Exchange Rate:");
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, szExchangeRate);
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, szDCCCurrencySymAmtOK);
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, szLocalCurrencySymAmtX);


	inTimeOut = get_env_int("DCCUITIMER");
	if((inTimeOut <= 0) ||(inTimeOut >=100))
		inTimeOut = 10;
         sprintf(szTimeOut,"%d",inTimeOut);
		 
	strcat(szDCCFxRateDetails, "|");
	strcat(szDCCFxRateDetails, szTimeOut);

   if(strTCT.inPrintDCCChooseCurrency== 2)
   	{
	     strcat(szDCCFxRateDetails, "|");
	     strcat(szDCCFxRateDetails, "2");
   	}
   else
   	{
   	    strcat(szDCCFxRateDetails, "|");
	    strcat(szDCCFxRateDetails, "1");
   	}

	vdDebug_LogPrintf("AAA - szDCCFxRateDetails[%s]", szDCCFxRateDetails);
		 
	inKey = inDCCFxRate(szDCCFxRateDetails);

	vdDebug_LogPrintf("AAA>> - inKey[%d]",inKey);

       //CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
       //vduiWarningSound();

	 while (1)
	{
		// Check if Cancel key is pressed //
		//TINE: android - removed
		#if 0
		CTOS_KBDHit(&key);
		if(key == d_KBD_2)
		{
		    srTransPara->fDCCOptin = FALSE;
		    //gblDCCTrans = FALSE;
		    //gblDCCReceipt = FALSE;
		    CTOS_KBDBufFlush ();
		    return DCC_LOCAL_OPT;
		}
		if (key == d_KBD_1)
		{
		    srTransPara->fDCCOptin = TRUE;
		    //gblDCCTrans = TRUE;
		    //gblDCCReceipt = TRUE;
		    CTOS_KBDBufFlush ();
		    break;
		}
		if (key == d_KBD_CANCEL)
		{
		    CTOS_KBDBufFlush ();
		    return USER_ABORT;
		}
		if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		{
		    vdSetErrorMessage("TIME OUT");
		    return USER_ABORT;
		}
		#endif

        if(inKey == d_KBD_3)
		{
		    //gblDCCTrans = FALSE;
		    //gblDCCReceipt = FALSE;
		    CTOS_KBDBufFlush ();
		    return DCC_PRINT_OPT;
		}
		if(inKey == d_KBD_2)
		{
		    srTransPara->fDCCOptin = FALSE;
		    //gblDCCTrans = FALSE;
		    //gblDCCReceipt = FALSE;
		    CTOS_KBDBufFlush ();
		    return DCC_LOCAL_OPT;
		}
		if (inKey == d_KBD_1)
		{
		    srTransPara->fDCCOptin = TRUE;
		    //gblDCCTrans = TRUE;
		    //gblDCCReceipt = TRUE;
		    //CTOS_KBDBufFlush ();
		    break;
		}
		if (inKey == d_USER_CANCEL)
		{
		    CTOS_KBDBufFlush ();
		    //vdSetErrorMessage("CANCEL");
		    vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_WARNING);
            CTOS_Beep();
            CTOS_Delay(1000);
		    return USER_ABORT;
		}
		if(inKey == 0xFF)
		{
		    vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
            CTOS_Beep();
            CTOS_Delay(1500);
		    return DCC_TIMEOUT;
		}

		
	}
return 0;
}


void GetDateAndTime(unsigned char * szDateandTime)
{
	CTOS_RTC SetRTC;
	BYTE szCurrentdate[8+1];
	BYTE szCurrentime[7+1];

	CTOS_RTCGet(&SetRTC);
	sprintf(szDateandTime,"%02d%02d%02d%02d%02d%02d",SetRTC.bYear,SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
	//vdDebug_LogPrintf("AAA - szDateandTime[%s]", szDateandTime);

}

void vdDCCModifyAmount(BYTE *szAmtBuffIn, BYTE *szAmtBuffOut)
{

	BYTE szAmtBuff2[20+1], szAmtBuff3[20+1];
	BYTE szAmtBuffTemp1[20+1];
	int inLength=0;

	vdDebug_LogPrintf("*** vdDCCModifyAmount :: szAmtBuffIn[%s]***",szAmtBuffIn);
	memset(szAmtBuff2, 0x00, sizeof(szAmtBuff2));
	memset(szAmtBuff3, 0x00, sizeof(szAmtBuff3));
	memset(szAmtBuffTemp1, 0x00, sizeof(szAmtBuff3));

	vdDebug_LogPrintf("gblinMinorUnit[%d]", gblinMinorUnit);

	if(strlen(szAmtBuffIn) == 1 || strlen(szAmtBuffIn) == 2 || strlen(szAmtBuffIn) == 3)
	{
		if(gblinMinorUnit == 0 || gblinMinorUnit == 2)
			sprintf(szAmtBuffTemp1,"%03ld",atol(szAmtBuffIn));
		else if(gblinMinorUnit == 3)
			sprintf(szAmtBuffTemp1,"%04ld",atol(szAmtBuffIn));

		memset(szAmtBuffIn,0x00,sizeof(szAmtBuffIn));
		strcpy(szAmtBuffIn,szAmtBuffTemp1);
	}

    //inLength=12-strCST.inMinorUnit;
    inLength=strlen(szAmtBuffIn)-gblinMinorUnit;
	vdDebug_LogPrintf("*** vdDCCModifyAmount :: inLength[%d]***",inLength);
	vdDebug_LogPrintf("*** vdDCCModifyAmount :: strCST.inMinorUnit[%d]***",gblinMinorUnit);
	memcpy(szAmtBuff2, szAmtBuffIn, inLength);

    if(gblinMinorUnit > 0)
    {
        strcat(szAmtBuff2, ".");
        //memcpy(szAmtBuff2+inLength+1, srTransRec.szDCCCurAmt+inLength, strCST.inMinorUnit);
        memcpy(szAmtBuff2+inLength+1, szAmtBuffIn+inLength, gblinMinorUnit);
    }
	//vdDCCFormatAmount(&szAmtBuff2, &szAmtBuff3);
	vdCTOS_FormatAmount(strCST.szAmountFormat, szAmtBuff3, szAmtBuffOut);

}

void vdDCCFormatAmount(BYTE *szAmtBuff, BYTE *szAmtBuffOut)
{
	BYTE szTempAmtBuff[20+1];
	BYTE szTempAmtBuff2[20+1];
	float fDCCCurAmt_Temp=0;
	int inLen;

	vdDebug_LogPrintf("*** vdDCCFormatAmount ***");

	if(gblinMinorUnit == 3)//if currency has 3 decimal places/minor units. Round off to two decimal places.
	{
		fDCCCurAmt_Temp = atof(szAmtBuff);
		memset(szTempAmtBuff,0x00,sizeof(szTempAmtBuff));
		sprintf(szTempAmtBuff,"%.2f",fDCCCurAmt_Temp);


		*szAmtBuffOut=0;

		memset(szTempAmtBuff2, 0x00, sizeof(szTempAmtBuff2));

		inLen=strlen(szTempAmtBuff)-3;
		memcpy(szTempAmtBuff2, szTempAmtBuff, inLen);
		memcpy(&szTempAmtBuff2[inLen], &szTempAmtBuff[inLen+1], 2);
		memcpy(szAmtBuffOut, szTempAmtBuff2, strlen(szTempAmtBuff2));
	}
	else if(gblinMinorUnit == 0)
	{
		memset(szTempAmtBuff,0x00,sizeof(szTempAmtBuff));
		strcat(szTempAmtBuff,szAmtBuff);
		strcat(szTempAmtBuff,"00");

		memset(szAmtBuff,0x00,sizeof(szAmtBuff));
		strcpy(szAmtBuffOut,szTempAmtBuff);
	}
	else
	{
		*szAmtBuffOut=0;

		memset(szTempAmtBuff2, 0x00, sizeof(szTempAmtBuff2));
		inLen=strlen(szAmtBuff)-3;
		memcpy(szTempAmtBuff2, szAmtBuff, inLen);

		memcpy(&szTempAmtBuff2[inLen], &szAmtBuff[inLen+1], 2);
		memcpy(szAmtBuffOut, szTempAmtBuff2, strlen(szTempAmtBuff2));
		//memcpy(szAmtBuffOut, szAmtBuff, strlen(szAmtBuff));
	}
}
#if 0 //temporarily disabled function replaced with hardcoded menu until this function is fixed
int GetMenuTransTypes(void)
{
    char szTrxn[20][20];
    char szItemImage[20][40];
    char szTrxnMenu[256 + 1];
    int inTrxnID[20];

    char szTrxnItems[20][20];
    char szItemFunc[20][40];
    char szTrxnListMenu[512+1], szTransFunc[256+1];
    int inTrxnItemID[20];

    char trxlogo[20], trxname[20], sztrxfunc[20];

    int inRet;
    int inLoop = 0;
    int key;

    memset(szTrxn, 0x00, sizeof(szTrxn));
    memset(szItemImage, 0x00, sizeof(szItemImage));

    inRet = inReadTrxTypes(szTrxn, szItemImage, inTrxnID);
    if (inRet != d_OK)
    return "FAIL";

    memset(szTrxnMenu, 0x00, sizeof(szTrxnMenu));
    strcat(szTrxnMenu, "Select Transaction Type: |");

    for (inLoop = 0; inLoop < 150; inLoop++) {
    if (szTrxn[inLoop][0] != 0) {
    strcat((char *) szTrxnMenu, szTrxn[inLoop]);
    strcat((char *) szTrxnMenu, (char *) "|");
    strcat((char *) szTrxnMenu, szItemImage[inLoop]);
    if (szTrxn[inLoop + 1][0] != 0)
    strcat((char *) szTrxnMenu, (char *) " \n");

    vdDebug_LogPrintf("szCustomerMenu %s", szTrxnMenu);
	 vdDebug_LogPrintf("saturn menu ID %d",strTCT.inMenuid);
    } else
    break;
    }

    vdDebug_LogPrintf("szCustomerMenu2 %s %d", szTrxnMenu, strlen(szTrxnMenu));

    key = MenuTransType(szTrxnMenu);
	vdDebug_LogPrintf("SATURN MenuTransType KEY %d", key);

    memset(trxlogo, 0x00, sizeof(trxlogo));
    memset(trxname, 0x00, sizeof(trxname));
    strcpy(trxlogo, szItemImage[key-1]);
    vdDebug_LogPrintf("trxlogo = [%s]", trxlogo);
    strcpy(trxname, szTrxn[key-1]);
    vdDebug_LogPrintf("trxname = [%s]", trxname);

    if(key == 1)
    {
        inRet = inReadTrxMenu(szTrxnItems, szItemFunc, inTrxnItemID);

        memset(szTrxnListMenu,0x00, sizeof(szTrxnListMenu));
        strcpy(szTrxnListMenu, trxlogo);
        strcat(szTrxnListMenu, "|");

        for (inLoop = 0; inLoop < 150; inLoop++) {
            if (szTrxnItems[inLoop][0] != 0) {
                strcat((char *) szTrxnListMenu, szTrxnItems[inLoop]);
                strcat((char *) szTrxnListMenu, (char *) "|");
                strcat((char *) szTrxnListMenu, szItemFunc[inLoop]);
                if (szTrxnItems[inLoop + 1][0] != 0)
                    strcat((char *) szTrxnListMenu, (char *) " \n");

                vdDebug_LogPrintf("saturn szCustomerMenu %s", szTrxnListMenu);
            } else
                break;
        }

        vdDebug_LogPrintf("saturn zCustomerMenu2 %s %d", szTrxnListMenu, strlen(szTrxnListMenu));


        key = MenuTransactions(szTrxnListMenu, szTransFunc);
        if(key == d_OK)
        {
            inRet = inCTOSS_ExeFunction(szTransFunc);
            if(inRet != d_OK)
                return -1;
        }

    }

	if(key == 2)
    {
        //inRet = inReadTrxMenu(szTrxnItems, szItemFunc, inTrxnItemID);

        memset(szTrxnListMenu,0x00, sizeof(szTrxnListMenu));
        strcpy(szTrxnListMenu, trxlogo);
        strcat(szTrxnListMenu, "|");

        strcat((char *) szTrxnListMenu, "SALE|inCTOS_BancnetSale \nBALANCE|inCTOS_BancnetBalInq \nLOGON|inCTOS_DebitLogon");
        vdDebug_LogPrintf("szCustomerMenu2 %s %d", szTrxnListMenu, strlen(szTrxnListMenu));


        key = MenuTransactions(szTrxnListMenu, szTransFunc);
        if(key == d_OK)
        {
            inRet = inCTOSS_ExeFunction(szTransFunc);
            if(inRet != d_OK)
                return -1;
        }

    }

	//reports
	if(key == 82)
    {
        //inRet = inReadTrxMenu(szTrxnItems, szItemFunc, inTrxnItemID);

        memset(szTrxnListMenu,0x00, sizeof(szTrxnListMenu));
        strcpy(szTrxnListMenu, trxlogo);
        strcat(szTrxnListMenu, "|");

		strcat((char *) szTrxnListMenu, "DETAIL REPORT|inCTOS_PRINTF_DETAIL \n");
		strcat((char *)	szTrxnListMenu, "SUMMARY REPORT|inCTOS_PRINTF_SUMMARY \n");
		strcat((char *)	szTrxnListMenu, "BATCH REVIEW|inCTOS_BATCH_REVIEW \n");
		strcat((char *)	szTrxnListMenu, "BATCH TOTAL|inCTOS_BATCH_TOTAL");
        vdDebug_LogPrintf("szCustomerMenu2 %s %d", szTrxnListMenu, strlen(szTrxnListMenu));


        key = MenuTransactions(szTrxnListMenu, szTransFunc);
        if(key == d_OK)
        {
            inRet = inCTOSS_ExeFunction(szTransFunc);
            if(inRet != d_OK)
                return -1;
        }

    }
	
    //settings
	if(key == 83)
	{
		inEditDatabase();
		return d_OK;

	}

    return inRet;

}
#else
int GetMenuTransTypes(void) {
    char szTrxn[20][20];
    char szItemImage[20][40];
    char szTrxnMenu[256 + 1];
    int inTrxnID[20];

    char szTrxnItems[20][20];
    char szItemFunc[20][40];
    char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
    int inTrxnItemID[20];

    char trxlogo[20], trxname[20], sztrxfunc[20];

    int inRet;
    int inLoop = 0, inIndex = 0;
    int key;
    int inNumRecs = 0;
    char szSaleMenu[1024];
    char szSelectedMenu[100];
    char szTrxFunc[100];
    int inSaleType = 0;
    BOOL isValid = TRUE;
    int inCount=0;
	
    memset(szTrxn, 0x00, sizeof(szTrxn));
    memset(szItemImage, 0x00, sizeof(szItemImage));

#if 0
    inRet = inReadTrxTypes(szTrxn, szItemImage, inTrxnID);
    if (inRet != d_OK)
    return "FAIL";
    memset(szTrxnMenu, 0x00, sizeof(szTrxnMenu));
    strcat(szTrxnMenu, "Select Transaction Type: |");

    for (inLoop = 0; inLoop < 150; inLoop++) {
    if (szTrxn[inLoop][0] != 0) {
    strcat((char *) szTrxnMenu, szTrxn[inLoop]);
    strcat((char *) szTrxnMenu, (char *) "|");
    strcat((char *) szTrxnMenu, szItemImage[inLoop]);
    if (szTrxn[inLoop + 1][0] != 0)
    strcat((char *) szTrxnMenu, (char *) " \n");

    vdDebug_LogPrintf("szCustomerMenu %s", szTrxnMenu);
     vdDebug_LogPrintf("saturn menu ID %d",strTCT.inMenuid);
    } else
    break;
    }
#else
    vdDebug_LogPrintf("saturn GetMenuTransTypes");

    memset(szTrxnMenu, 0x00, sizeof(szTrxnMenu));
    strcpy(szTrxnMenu, "Select Transaction Type: |");
#endif

#if 0
    strcat((char *) szTrxnMenu, "MCC|credit.bmp \n");
    strcat((char *) szTrxnMenu, "BANCNET|bancnet.bmp");
    vdDebug_LogPrintf("saturn szCustomerMenu2 %s %d", szTrxnMenu, strlen(szTrxnMenu));
    vdDebug_LogPrintf("szCustomerMenu2 %s %d", szTrxnMenu, strlen(szTrxnMenu));
#endif

    inCount=inReadBaseDMTrxMenu(&inNumRecs);

    vdDebug_LogPrintf("inGetSelectionMenu.inNumRecs[%d]", inNumRecs);

    memset(szSaleMenu, 0x00, sizeof(szSaleMenu));
    strcat(szSaleMenu, "Select Transaction Type: |");

    for (inLoop = 0; inLoop < inNumRecs; inLoop++) {
        if (strTrxMenu[inLoop].szButtonItemLabel[0] != 0) {
            inIndex = inLoop;
            strcat((char *) szSaleMenu, strTrxMenu[inLoop].szButtonItemLabel);
            strcat((char *) szSaleMenu, (char *) "|");
            strcat((char *) szSaleMenu, strTrxMenu[inLoop].szButtonItemImage);


            if (strTrxMenu[inLoop + 1].szButtonItemLabel[0] != 0)
                strcat((char *) szSaleMenu, (char *) " \n");
        }
    }

    vdDebug_LogPrintf("saturn inSelectIdleTrxn.szSaleMenu[%s] len [%d]", szSaleMenu, strlen(szSaleMenu));
    #if 1
	if(inNumRecs == 1)
    {
		vdDebug_LogPrintf("inNumRecs == 1");	
		//strTrxMenu[0].szButtonItemLabel;
		
		memset(szSelectedMenu, 0, sizeof(szSelectedMenu));
		strcpy(szSelectedMenu, strTrxMenu[0].szButtonItemLabel);
		vdDebug_LogPrintf("szSelectedMenu", szSelectedMenu);

		memset(trxlogo, 0x00, sizeof(trxlogo));
		strcpy(trxlogo, strTrxMenu[0].szButtonItemImage);
		vdDebug_LogPrintf("trxlogo", trxlogo);
		
        if ((memcmp(szSelectedMenu, "MC/VISA/JCB/UPI", strlen(szSelectedMenu)) == 0) ||
        (memcmp(szSelectedMenu, "MC/VISA", strlen(szSelectedMenu)) == 0) ||
        (memcmp(szSelectedMenu, "SALE", strlen(szSelectedMenu)) == 0) ||
        (memcmp(szSelectedMenu, "INSTALLMENT", strlen(szSelectedMenu)) == 0)) {
            inSaleType = 1;
        } else if (memcmp(szSelectedMenu, "BANCNET", strlen(szSelectedMenu)) == 0) {
            inSaleType = 2;
        }else if (memcmp(szSelectedMenu, "MCCPAY", strlen(szSelectedMenu)) == 0) {
            inSaleType = 3;
        }else if (memcmp(szSelectedMenu, "PHQR", strlen(szSelectedMenu)) == 0) {
            inSaleType = 4;
        }

        switch (inSaleType) 
        {
            case 1: // Credit
                vdDebug_LogPrintf("saturn credit selected");            	
                inSelectedIdleAppsHost = MCC_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
            break;
            
            case 2: // Bancnet
                vdDebug_LogPrintf("saturn bancnet selected");
                inSelectedIdleAppsHost = BANCNET_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
            break;
            
            case 3:
                //inQRPAY();	
                vdDebug_LogPrintf("saturn wallet selected");
                inSelectedIdleAppsHost = WECHAT_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
            break;
            
            case 4:
                //inQRPAY();	
                vdDebug_LogPrintf("saturn PHQR_HOST selected");
                inSelectedIdleAppsHost = PHQR_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
            break;
        }
        
        vdDebug_LogPrintf("inSaleType[%d]", inSaleType);
        vdDebug_LogPrintf("inSelectedIdleAppsHost[%d]", inSelectedIdleAppsHost);
        
        strTCT.inMMTid = inSaleType;	
        inTCTMMTidSave(1);
    }
    else 
		#endif
		if (inNumRecs > 0) {
        key = MenuTransType(szSaleMenu);
        vdDebug_LogPrintf("SATURN MenuTransType KEY %d", key);

		//if menu is NO QRPAY sale - return value 3 is QRPH selected - set return value to 4
		//if (strGBLVar.inGBLvAppType == 4){
		//	if (key == 3)
		//		key = 4;

		//}

		if (key == 1)//credit selected
		    inSettlementHost = 1;
		else if(key == 2) //bancnet selected
			inSettlementHost = 2;
		else if(key == 3){ //MCCPAY selected
		
			if (strGBLVar.inGBLvAppType == 4) //if type 4, option 3 is QRPH host
				inSettlementHost = 4;
			else
				inSettlementHost = 3;
		
		}else if(key == 4) //PHQR selected
			inSettlementHost = 4;

        if (key == 'C') {
            //vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return d_NO;
        } else if (key == 0xFF) {
            //vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return d_NO;
        } else if (key == 'R') {
            //Reports
			/*
			memset(szTrxnListMenu, 0x00, sizeof(szTrxnListMenu));
            strcpy(szTrxnListMenu, trxlogo);
            strcat(szTrxnListMenu, "|");

            strcat((char *) szTrxnListMenu, "DETAIL REPORT|inCTOS_PRINTF_DETAIL \n");
            strcat((char *) szTrxnListMenu, "SUMMARY REPORT|inCTOS_PRINTF_SUMMARY \n");
            strcat((char *) szTrxnListMenu, "BATCH REVIEW|inCTOS_BATCH_REVIEW \n");
            strcat((char *) szTrxnListMenu, "BATCH TOTAL|inCTOS_BATCH_TOTAL \n");
            strcat((char *) szTrxnListMenu, "IP REPORT|vdCTOS_PrintIP \n");
            strcat((char *) szTrxnListMenu, "HOST INFO REPORT|vdCTOS_HostInfo \n");
            strcat((char *) szTrxnListMenu, "CRC REPORT|vdCTOS_PrintCRC");

            vdDebug_LogPrintf("szCustomerMenu2 %s %d", szTrxnListMenu, strlen(szTrxnListMenu));


            key = MenuTransactions(szTrxnListMenu, szTransFunc);
            if (key == d_OK) {
                inRet = inCTOSS_ExeFunction(szTransFunc);
                if (inRet != d_OK)
                    return -1;
            }
            */
            //TINE:  read transaction list from Dynamic Menu
			inSaleType = 'R';
            memset(trxlogo, 0x00, sizeof(trxlogo));
            strcpy(trxlogo, "allcards.bmp");
            vdDisplayTrxn(inSaleType, trxlogo);

            return d_OK;
			
        } else if (key == 'P') {
                //Settlement
                srTransRec.byTransType = SETTLE;
              /*  inRet = inCTOS_GetTxnPassword();
                if (d_OK != inRet)
                     return inRet;
			
			memset(szTrxnListMenu, 0x00, sizeof(szTrxnListMenu));
            strcpy(szTrxnListMenu, trxlogo);
            strcat(szTrxnListMenu, "|");

            strcat((char *) szTrxnListMenu, "LAST RECEIPT|inCTOS_REPRINT_LAST \n");
            strcat((char *) szTrxnListMenu, "ANY RECEIPT|inCTOS_REPRINT_ANY \n");
            strcat((char *) szTrxnListMenu, "LAST SETTLEMENT|inCTOS_REPRINTF_LAST_SETTLEMENT");
            vdDebug_LogPrintf("szCustomerMenu2 %s %d", szTrxnListMenu, strlen(szTrxnListMenu));


            key = MenuTransactions(szTrxnListMenu, szTransFunc);
            if (key == d_OK) {
                inRet = inCTOSS_ExeFunction(szTransFunc);
                if (inRet != d_OK)
                    return -1;
            }
            */
			//TINE:  read transaction list from Dynamic Menu
			inSaleType = 'P';
            memset(trxlogo, 0x00, sizeof(trxlogo));		
			sprintf(trxlogo,"supported%d.bmp", strGBLVar.inGBLvAppType);
            //strcpy(trxlogo, "allcards.bmp");
            vdDisplayTrxn(inSaleType, trxlogo);
			
        } else if (key == 'S') {
            //Settings
#if 0
            inEditDatabase();
            return d_OK;
#else
            int inRet;

            srTransRec.byTransType = SETUP;
            inRet = inCTOS_GetTxnPassword();
            if (d_OK != inRet)
                return inRet;

            inSaleType = 0;
            memset(trxlogo, 0x00, sizeof(trxlogo));
            strcpy(trxlogo, "allcards.bmp");
			vdDebug_LogPrintf("trxlogo [%s], inSaleType [%d]", trxlogo, inSaleType);
            vdDisplayTrxn(inSaleType, trxlogo);
#endif

            return d_OK;

        } 
		else 
		{
            memset(szSelectedMenu, 0x00, sizeof(szSelectedMenu));
            memset(szTrxFunc, 0x00, sizeof(szTrxFunc));
            strcpy(szSelectedMenu, strTrxMenu[key - 1].szButtonItemLabel);
            //check if need to read submenu
            strcpy(szTrxFunc, strTrxMenu[key - 1].szButtonItemFunc);
            memset(trxlogo, 0x00, sizeof(trxlogo));
            strcpy(trxlogo, strTrxMenu[key - 1].szButtonItemImage);
            vdDebug_LogPrintf("szSelectedMenu", strTrxMenu[key - 1]);
			
			vdDebug_LogPrintf("testlang15 logo = %s", trxlogo);
            if ((memcmp(szSelectedMenu, "MC/VISA/JCB/UPI", strlen(szSelectedMenu)) == 0) ||
                (memcmp(szSelectedMenu, "MC/VISA", strlen(szSelectedMenu)) == 0) ||
                (memcmp(szSelectedMenu, "SALE", strlen(szSelectedMenu)) == 0) ||
                (memcmp(szSelectedMenu, "INSTALLMENT", strlen(szSelectedMenu)) == 0)) {
                inSaleType = 1;
            } else if (memcmp(szSelectedMenu, "BANCNET", strlen(szSelectedMenu)) == 0) {
                inSaleType = 2;
            }else if (memcmp(szSelectedMenu, "MCCPAY", strlen(szSelectedMenu)) == 0) {
            	inSaleType = 3;
            }else if (memcmp(szSelectedMenu, "PHQR", strlen(szSelectedMenu)) == 0) {
            	inSaleType = 4;
            }
        }

		// sidumili: added "HeaderMMTid" for env, use to check what header to load in MMT AppHeaderDetails
        switch (inSaleType) {
            case 1: // Credit
            	vdDebug_LogPrintf("saturn credit selected");            	
                inSelectedIdleAppsHost = MCC_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
                break;

            case 2: // Bancnet
            	vdDebug_LogPrintf("saturn bancnet selected");
                inSelectedIdleAppsHost = BANCNET_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
                break;

			case 3:
				//inQRPAY();	
				vdDebug_LogPrintf("saturn wallet selected");
                inSelectedIdleAppsHost = WECHAT_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
                break;
			
			case 4:
				//inQRPAY();	
				vdDebug_LogPrintf("saturn PHQR_HOST selected");
                inSelectedIdleAppsHost = PHQR_HOST;
                vdDisplayTrxn(inSaleType, trxlogo);
                break;

				//break;

        }

        vdDebug_LogPrintf("inSaleType[%d]", inSaleType);
        vdDebug_LogPrintf("inSelectedIdleAppsHost[%d]", inSelectedIdleAppsHost);

		strTCT.inMMTid = inSaleType;	
		inTCTMMTidSave(1);
    } else {
        isValid = FALSE;
        vdDisplayErrorMsg(1, 8, "NO APP MENU CREATED");
    }


    if (!isValid)
        return d_NO;
    else
        return d_OK;


}

#endif
int  inEditDatabase(void){

	int inRet;

	BYTE bInBuf[40];
    BYTE bOutBuf[40];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;

    srTransRec.byTransType = SETUP;

#if 1
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
#endif

//	usEditDatabase("edit");

	
#if 1
		//removed for testing
		
		inTCTRead(1);
		inCPTRead(1);
	
		inPCTRead(1);
	
		inCSTRead(1);
	
#endif		
		inTCPRead(1);
    vdDebug_LogPrintf("after edid db.");
    inTCTSave(1);

	//inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_EMV.SHARLS_EMV", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
    //vdDebug_LogPrintf("EditDatabase: SHARLS_EMV done - inRet = [%d]", inRet);
    vdCTOS_TransEndReset();

	return d_OK;

}

#if 0
int inCTOS_AutoSettlement(void)
{
	CTOS_RTC rtcClock;
	CTOS_RTC SetRTC;
  	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;
	BYTE szCurrDate[8] = {0};
	BYTE szASTime[8] = {0};
	int inASTime = 0;
	BYTE szASDate[8] = {0};
	int inRetryCnt;
	int inBatchCnt = 0;
	int inSettledCnt = 0;
	int inResult = d_NO;
	BYTE szASRetry[8] = {0};
	int inASRetry = 0;
	BYTE szASGap[8] = {0};
	int inASGap = 0;
	BYTE szASEnable[8] = {0};
	int inASEnable = 0;
	BYTE szASRetryCounter[8] = {0};
	int inASRetryCounter = 0;
	BYTE szASExecute[8] = {0};
	int inASExecute = 0;
	BYTE szASLastSettle[8] = {0};
	BYTE szTemp[8 + 1] = {0};
	BOOL fPerformed = TRUE;
	BYTE szTemp1[40+1]={0};
	int iPerformed = 0;
	
	vdDebug_LogPrintf("saturn --inCTOS_AutoSettlement--");

	int fSettlePerformed = 0;


    BYTE szTitle[20+1];
    BYTE szDisMsg[30];

	//memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	strcpy(szDisMsg, "SETTLE");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PROCESSING...");
	usCTOSS_LCDDisplay(szDisMsg);  

	fAutoSettle = FALSE;

	//vdSetASParams();

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASType = 0;
	inCTOSS_GetEnvDB("ASTYPE", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASType = wub_str_2_long(szTemp);
	
	switch (strAST.inASType)
	{
		case 0:
			vdDebug_LogPrintf("Disabled AS...");
			break;
		case 1:
			
			vdDebug_LogPrintf("saturn --inCTOS_AutoSettlement-- enabled");
			vdDebug_LogPrintf("Enabled AS...");

			vdSetASParams();
			
			// Current Date
			memset(szCurrDate, 0x00, sizeof(szCurrDate));
			CTOS_RTCGet(&SetRTC);
			sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
			
			// Current Time
			memset(szCurrTime, 0x00, sizeof(szCurrTime));
		    CTOS_RTCGet(&rtcClock);
		    sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
		    inCurrTime = wub_str_2_long(szCurrTime);			

			memset(szASDate, 0x00, sizeof(szASDate));
			inCTOSS_GetEnvDB("ASDATE", szASDate);

			vdDebug_LogPrintf("DATE Len=[%d],szASDate=[%s] |Len=[%d],szCurrDate=[%s]",strlen(szASDate), szASDate, strlen(szCurrDate), szCurrDate);

			if (strcmp(szASDate,szCurrDate) != 0) // New Date
			{
				put_env_int("ASPERFORMED",1);
				put_env_char("ASDATE",szCurrDate);
				put_env_char("ASNEXTTIME",szCurrTime);
				put_env_int("ASRCOUNTER", 0);
				
				inBatchCnt = inCheckBatcheNotEmtpy();
				vdDebug_LogPrintf("New Date:::strAST.inASTime=[%d], inCurrTime[%d], inBatchCnt[%d]", strAST.inASTime, inCurrTime, inBatchCnt);				
				if (inBatchCnt <= 0 && (inCurrTime >= strAST.inASTime))
				{
					put_env_int("ASPERFORMED",0);
				}

				vdSetASParams();
			}
			
			iPerformed = get_env_int("ASPERFORMED");
			if (iPerformed <= 0)
				fPerformed = FALSE;
			
			vdDebug_LogPrintf("saturn 111::fPerformed=[%d], strAST.inASRetryCounter=[%d]", fPerformed, strAST.inASRetryCounter);

			if (fPerformed == FALSE) // No autosettlement will process 
			{
			    
				vdDebug_LogPrintf("saturn --inCTOS_AutoSettlement-- return false");
				return FALSE;
			}

			if (strAST.inASRetryCounter > 0)
			{
				strAST.inASTime = strAST.inASNextTime + (strAST.inASGap * strAST.inASRetryCounter);
			}

			inBatchCnt = inCheckBatcheNotEmtpy();
			vdDebug_LogPrintf("saturn strAST.inASTime=[%d], inCurrTime[%d], inASNextTime[%d], inBatchCnt[%d], strAST.inASRetryLimit[%d], strAST.inASGap[%d], strAST.inASRetryCounter[%d]", strAST.inASTime, inCurrTime, strAST.inASNextTime, inBatchCnt, strAST.inASRetryLimit, strAST.inASGap, strAST.inASRetryCounter);
						
			if (inBatchCnt > 0)
			{		
				if (inCurrTime >= strAST.inASTime)
				{	
					inUpdateSettled(1);
					
					CTOS_LCDTClearDisplay();
					memset(szTemp1, 0x00, sizeof(szTemp1));
					CTOS_LCDTPrintXY(1, 6, "Perform auto settle");
					sprintf(szTemp1, "Retry counter %d of %d", strAST.inASRetryCounter+1, strAST.inASRetryLimit);
					CTOS_LCDTPrintXY(1, 7, szTemp1);
					CTOS_LCDTPrintXY(1, 8, "Please wait...");
		
					fAutoSettle = TRUE;
					CTOS_Delay(2000);

					CTOS_LCDTClearDisplay();
					srTransRec.fAutoSettleFlag = TRUE;
				
					if (inMultiAP_CheckMainAPStatus() == d_OK)
					{
						inCTOS_SettleAllHosts();
						//inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);

						inCTOS_SettleBancnet();

						//SettlementHost = 2;
						//CTOS_SETTLEMENT();
						//SettlementHost=0;
					}
					else
					{
						inCTOS_SettleAllHosts();
					}

					
					//memset(szTitle, 0x00, sizeof(szTitle));
					//szGetTransTitle(srTransRec.byTransType, szTitle);
					strcpy(szDisMsg, "SETTLE");
					strcat(szDisMsg, "|");
					strcat(szDisMsg, "PROCESSING...");
					usCTOSS_LCDDisplay(szDisMsg);  
					fSettlePerformed = 1;

					// Check for failed settlement and increment failed counter
					vdDebug_LogPrintf("Check here for failed settlement counter!!!");
					inSettledCnt = inCheckSettled();
					vdDebug_LogPrintf("AS inSettledCnt=[%d]", inSettledCnt);					
					if (inSettledCnt > 0)
					{
						vdDebug_LogPrintf("strAST.inASRetryCounter=[%d], inASRetryLimit=[%d]", strAST.inASRetryCounter, strAST.inASRetryLimit);
						
						strAST.inASRetryCounter++;

						if (strAST.inASRetryCounter >= strAST.inASRetryLimit)
						{
							vdDebug_LogPrintf("AS Limit Exceed");
							put_env_char("ASDATE",szCurrDate);

							// For Next Time To Settle
							memset(szCurrTime, 0x00, sizeof(szCurrTime));
						    CTOS_RTCGet(&rtcClock);
						    sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
						    inCurrTime = wub_str_2_long(szCurrTime);		
							put_env_char("ASNEXTTIME",szCurrTime);								
							put_env_int("ASPERFORMED", 0);
							strAST.inASRetryCounter = 0;
							vdSetASParams();
						}

						put_env_int("ASRCOUNTER", strAST.inASRetryCounter);
						
					}
					else
					{
						vdDebug_LogPrintf("Successfull autosettlement...");
						
						put_env_char("ASDATE",szCurrDate);
						put_env_char("ASNEXTTIME",strAST.szASTime);
						put_env_int("ASPERFORMED", 0);
						strAST.inASRetryCounter = 0;	
						put_env_int("ASRCOUNTER", strAST.inASRetryCounter);
						inUpdateSettled(1);
						vdSetASParams();
						
						if (fFSRMode() == TRUE)
						{															
							put_env_int("FAILEDERMTRXN", 0);
							vdSetFSRMode(0);
						}

						CTOS_LCDTClearDisplay();	
						inCTOSS_TMSDownloadRequestAfterSettle(TRUE);
					}
					
				}
			}
			else
			{
				put_env_char("ASDATE",szCurrDate);
			}
	
			break;

			default:
				break;
	}

	return fSettlePerformed;
}
#else
int inCTOS_AutoSettlement(void)
{
	CTOS_RTC rtcClock;
	CTOS_RTC SetRTC;
	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;
	BYTE szCurrDate[8] = {0};
	BYTE szASTime[8] = {0};
	int inASTime = 0;
	BYTE szASDate[8] = {0};
	int inRetryCnt;
	int inBatchCnt = 0;
	int inSettledCnt = 0;
	int inResult = d_NO;
	BYTE szASRetry[8] = {0};
	int inASRetry = 0;
	BYTE szASGap[8] = {0};
	int inASGap = 0;
	BYTE szASEnable[8] = {0};
	int inASEnable = 0;
	BYTE szASRetryCounter[8] = {0};
	int inASRetryCounter = 0;
	BYTE szASExecute[8] = {0};
	int inASExecute = 0;
	BYTE szASLastSettle[8] = {0};
	BYTE szTemp[8 + 1] = {0};
	BOOL fPerformed = FALSE;
	BYTE szTemp1[40+1]={0};
	int iPerformed = 0;
	int iMustSettleCount = 0;
	int inYear, inMonth, inDate,inDateGap;
	int inCurrDate = 0;
	char buf[6+1]={0};
	char szYear[2+1]={0};
	char szMonth[2+1]={0};
	char szDate[2+1]={0};
	int inRet = 0;

	BYTE szDisMsg[100];
	
	vdDebug_LogPrintf("saturn -- DO inCTOS_AutoSettlement--");

	//if (fFSRMode() == TRUE)
	//	return d_OK;

	inBatchCnt = inCheckBatcheNotEmtpy();

	vdDebug_LogPrintf("saturn strAST.inASPerformed = %d inBatchCnt = %d", strAST.inASPerformed, inBatchCnt);
	put_env_int("AUTOSETTLETRIGGERED", 1);


	if (strAST.inASPerformed > 0 && inBatchCnt > 0)
	{
	   	vdDebug_LogPrintf("saturn perform autosettlement");
		//CTOS_LCDTClearDisplay();
		memset(szTemp1, 0x00, sizeof(szTemp1));
		//CTOS_LCDTPrintXY(1, 6, "Perform auto settle");
		sprintf(szTemp1, "RETRY COUNTER %d of %d", strAST.inASRetryCounter+1, strAST.inASRetryLimit);
		//CTOS_LCDTPrintXY(1, 7, szTemp1);
		//CTOS_LCDTPrintXY(1, 8, "Please wait...");
	
		//CTOS_Delay(2000);
	
		//CTOS_LCDTClearDisplay();
		memset(szDisMsg, 0x00, sizeof(szDisMsg));
		strcpy(szDisMsg, "SETTLE");	
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "");		
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "");			
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PERFORM AUTO SETTLE");	
		strcat(szDisMsg, "|");
		strcat(szDisMsg, szTemp1);
		//usCTOSS_LCDDisplay(szDisMsg);  

		vdDisplayMessageBox(1, 8, "PERFORM AUTO SETTLEMENT", szTemp1, "Please wait...", MSG_TYPE_INFO);		
		CTOS_Delay(2000);
		
		srTransRec.fAutoSettleFlag = TRUE;
	
		if (inMultiAP_CheckMainAPStatus() == d_OK)
		{
			inCTOS_SettleAllHosts();
			inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);

			inCTOS_SettleBancnet(); // Bancnet

			//inCTOS_SettleDigitalWallet(); // Wechat/Alipay
			//inCTOS_SettleDigitalWalletAndPHQR();//settle all wallet - including phqr
		}
		else
		{
			inCTOS_SettleAllHosts();
		}

		// Must settle, error during settlement
		iMustSettleCount = inMMTMustSettleNumRecord();
		vdDebug_LogPrintf("saturn AS::222iMustSettleCount=[%d]", iMustSettleCount);
	
		if (iMustSettleCount <= 0)
		{
			vdDebug_LogPrintf("saturn AS::Successfull auto settlement...");
			strAST.inASPerformed = 0;
			put_env_int("ASPERFORMED", strAST.inASPerformed);
			CTOS_RTCGet(&SetRTC);
			sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
			vdDebug_LogPrintf("szCurrDate[%s]", szCurrDate);
			put_env_char("ASDATE",szCurrDate);
	
			strAST.inASRetryCounter = 0;
			put_env_int("ASRCOUNTER", strAST.inASRetryCounter);
	
			strAST.inASNextTime = 0;
			put_env_int("ASNEXTTIME", strAST.inASNextTime);
	
			//vdDisplayMultiLineMsgAlign("AUTO SETTLEMENT", "PROCESSED", "COMPLETE", DISPLAY_POSITION_CENTER);
					
			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, "SETTLE"); 
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "");		
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "");			
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "AUTO SETTLEMENT");	
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "PROCESS COMPLETE");

			//usCTOSS_LCDDisplay(szDisMsg);  
			vdDisplayMessageBox(1, 8, "AUTO SETTLEMENT", "PROCESS COMPLETE", "", MSG_TYPE_INFO);
			put_env_int("AUTOSETTLETRIGGERED", 0);
			CTOS_Beep();
			CTOS_Delay(2000);
			//CTOS_LCDTClearDisplay();
			vdDebug_LogPrintf("saturn Auto settlement completed!!!");
	
            if(strTCT.byCTMSCallAfterSettle == 1){

			inRet = inCheckAllBatchEmtpy();
			vdDebug_LogPrintf("AAA - inCheckAllBatchEmtpy[%d]", inRet);
            if(inRet > 0)
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
	
		}
		else
		{
			strAST.inASRetryCounter++;
			vdDebug_LogPrintf("saturn AS::Error during auto settlement...inASRetryCounter=[%d], inASRetryLimit=[%d]", strAST.inASRetryCounter, strAST.inASRetryLimit); 				
			if (strAST.inASRetryCounter >= strAST.inASRetryLimit)
			{
				strAST.inASRetryCounter = 0;
				put_env_int("ASRCOUNTER", strAST.inASRetryCounter);
	
				strAST.inASNextTime = 0;
				put_env_int("ASNEXTTIME", strAST.inASNextTime);
	
				strAST.inASPerformed = 0;
				put_env_int("ASPERFORMED", strAST.inASPerformed);

				vdDebug_LogPrintf("AAA - szCurrDate[%s]", szCurrDate);
				
	                            CTOS_RTCGet(&SetRTC);
			         sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
				put_env_char("ASDATE",szCurrDate);
				
				//vdDisplayMultiLineMsgAlign("AUTO SETTLEMENT", "RETRY COUNTER", "EXCEEDED", DISPLAY_POSITION_CENTER);
				//CTOS_LCDTClearDisplay();
				
				vdDebug_LogPrintf("Retry counter exceeded!!!");
			}
			else
			{
				// For New Time To Settle
				memset(szCurrTime, 0x00, sizeof(szCurrTime));
				CTOS_RTCGet(&rtcClock);
				sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
				inCurrTime = wub_str_2_long(szCurrTime);
				strAST.inASNextTime = inCurrTime;						
				put_env_int("ASRCOUNTER", strAST.inASRetryCounter);
			}
	
			vdDebug_LogPrintf("saturn AS::Before New settlement inASTime=[%d], strAST.inASNextTime=[%d], inASRetryCounter=[%d]", strAST.inASTime, strAST.inASNextTime);
			
			if (strAST.inASRetryCounter > 0)
			{
				strAST.inASTime = strAST.inASNextTime + strAST.inASGap;
				put_env_int("ASNEXTTIME", strAST.inASTime);
			}
	
			vdDebug_LogPrintf("saturn AS::After New settlement inASTime=[%d], inCurrTime=[%d], inASNextTime=[%d], inASRetryCounter=[%d]", strAST.inASTime, inCurrTime, strAST.inASNextTime, strAST.inASRetryCounter);
		}
	}
	
}


#endif

void vdSetASParams(void)
{
	BYTE szTemp[8 + 1] = {0};
	BYTE szTime[8 + 1] = {0};
	int inTime = 0;
	CTOS_RTC SetRTC;
	CTOS_RTC rtcClock;

	vdDebug_LogPrintf("--vdSetASParams--");
	
	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASType = 0;
	inCTOSS_GetEnvDB("ASTYPE", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASType = wub_str_2_long(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASRetryLimit = 0;
	inCTOSS_GetEnvDB("ASRLIMIT", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASRetryLimit = wub_str_2_long(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASRetryCounter = 0;
	inCTOSS_GetEnvDB("ASRCOUNTER", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASRetryCounter = wub_str_2_long(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASGap = 0;
	inCTOSS_GetEnvDB("ASGAP", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASGap = wub_str_2_long(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASTime = 0;
	inCTOSS_GetEnvDB("ASTIME", szTemp);
	if (strlen(szTemp) > 0)
	{
		strAST.inASTime = wub_str_2_long(szTemp);
		strcpy(strAST.szASTime, szTemp);
	}
		
	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASNextTime = 0;
	inCTOSS_GetEnvDB("ASNEXTTIME", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASNextTime = wub_str_2_long(szTemp);
		
	memset(szTemp, 0x00, sizeof(szTemp));
	memset(strAST.szASDate, 0x00, sizeof(strAST.szASDate));
	inCTOSS_GetEnvDB("ASDATE", szTemp);
	vdDebug_LogPrintf("AAA - strlen(szTemp)[%d] szTemp[%s]", strlen(szTemp), szTemp);
	if ((strlen(szTemp) > 0) && (strcmp(szTemp,"szValue")!=0))
	{
		strcpy(strAST.szASDate, szTemp);
	}
	else
	{
		memset(szTemp, 0x00, sizeof(szTemp));
		CTOS_RTCGet(&SetRTC);
		sprintf(szTemp,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
		vdDebug_LogPrintf("AAA - szTemp[%s]", szTemp);
		inCTOSS_PutEnvDB("ASDATE", szTemp);

		// Read
		memset(szTemp, 0x00, sizeof(szTemp));
		memset(strAST.szASDate, 0x00, sizeof(strAST.szASDate));
		inCTOSS_GetEnvDB("ASDATE", szTemp);
		strcpy(strAST.szASDate, szTemp);
	}
	
	strAST.inASDate = wub_str_2_long(strAST.szASDate);

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASPerformed = 0;
	inCTOSS_GetEnvDB("ASPERFORMED", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASPerformed = wub_str_2_long(szTemp);

	
	vdDebug_LogPrintf("Get::inASType=[%d]", strAST.inASType);
	vdDebug_LogPrintf("Get::inASRetryLimit=[%d]", strAST.inASRetryLimit);
	vdDebug_LogPrintf("Get::inASRetryCounter=[%d]", strAST.inASRetryCounter);
	vdDebug_LogPrintf("Get::inASGap=[%d]", strAST.inASGap);
	vdDebug_LogPrintf("Get::inASTime=[%d]", strAST.inASTime);
	vdDebug_LogPrintf("Get::inASNextTime=[%d]", strAST.inASNextTime);
	vdDebug_LogPrintf("Get::szASDate=[%s]", strAST.szASDate);
	vdDebug_LogPrintf("Get::inASPerformed=[%d]", strAST.inASPerformed);
	vdDebug_LogPrintf("Get::inASDate=[%d]", strAST.inASDate);
		
	//return d_OK;
}

#if 0
int inCTOS_CheckAutoSettlement(void)
{
	CTOS_RTC rtcClock;
	CTOS_RTC SetRTC;
  	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;
	BYTE szCurrDate[8] = {0};
	BYTE szASTime[8] = {0};
	int inASTime = 0;
	BYTE szASDate[8] = {0};
	int inRetryCnt;
	int inBatchCnt = 0;
	int inSettledCnt = 0;
	int inResult = d_NO;
	BYTE szASRetry[8] = {0};
	int inASRetry = 0;
	BYTE szASGap[8] = {0};
	int inASGap = 0;
	BYTE szASEnable[8] = {0};
	int inASEnable = 0;
	BYTE szASRetryCounter[8] = {0};
	int inASRetryCounter = 0;
	BYTE szASExecute[8] = {0};
	int inASExecute = 0;
	BYTE szASLastSettle[8] = {0};
	BYTE szTemp[8 + 1] = {0};
	BOOL fPerformed = TRUE;
	BYTE szTemp1[40+1]={0};
	int iPerformed = 0;
	

	int fDoAutoSettle = 0;

	fAutoSettle = FALSE;

	//vdSetASParams();

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASType = 0;
	inCTOSS_GetEnvDB("ASTYPE", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASType = wub_str_2_long(szTemp);
	
	switch (strAST.inASType)
	{
		case 0:
			vdDebug_LogPrintf("Disabled AS...");
			break;
		case 1:
			vdDebug_LogPrintf("Enabled AS...");

			vdSetASParams();
			
			// Current Date
			memset(szCurrDate, 0x00, sizeof(szCurrDate));
			CTOS_RTCGet(&SetRTC);
			sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
			
			// Current Time
			memset(szCurrTime, 0x00, sizeof(szCurrTime));
		    CTOS_RTCGet(&rtcClock);
		    sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
		    inCurrTime = wub_str_2_long(szCurrTime);			

			memset(szASDate, 0x00, sizeof(szASDate));
			inCTOSS_GetEnvDB("ASDATE", szASDate);

			vdDebug_LogPrintf("DATE Len=[%d],szASDate=[%s] |Len=[%d],szCurrDate=[%s]",strlen(szASDate), szASDate, strlen(szCurrDate), szCurrDate);

			if (strcmp(szASDate,szCurrDate) != 0) // New Date
			{
				put_env_int("ASPERFORMED",1);
				put_env_char("ASDATE",szCurrDate);
				put_env_char("ASNEXTTIME",szCurrTime);
				put_env_int("ASRCOUNTER", 0);
				
				inBatchCnt = inCheckBatcheNotEmtpy();
				vdDebug_LogPrintf("saturn New Date:::strAST.inASTime=[%d], inCurrTime[%d], inBatchCnt[%d]", strAST.inASTime, inCurrTime, inBatchCnt);				
				if (inBatchCnt <= 0 && (inCurrTime >= strAST.inASTime))
				{
				    vdDebug_LogPrintf("set ASPERFORMED  to 0");
					put_env_int("ASPERFORMED",0);
				}

				vdSetASParams();
			}
			
			iPerformed = get_env_int("ASPERFORMED");
			if (iPerformed <= 0)
				fPerformed = FALSE;
			
			vdDebug_LogPrintf("saturn 111::fPerformed=[%d], strAST.inASRetryCounter=[%d]", fPerformed, strAST.inASRetryCounter);

			if (fPerformed == FALSE) // No autosettlement will process 
				fDoAutoSettle  = FALSE;
			else
				fDoAutoSettle =  TRUE;
	
			break;

			default:
				break;
	}

	return fDoAutoSettle;
}
#else

int inCTOS_CheckAutoSettlement(void)
{
	CTOS_RTC rtcClock;
	CTOS_RTC SetRTC;
	BYTE szCurrTime[7] = {0};
	int inCurrTime = 0;
	BYTE szCurrDate[8] = {0};
	BYTE szASTime[8] = {0};
	int inASTime = 0;
	BYTE szASDate[8] = {0};
	int inRetryCnt;
	int inBatchCnt = 0;
	int inSettledCnt = 0;
	int inResult = d_NO;
	BYTE szASRetry[8] = {0};
	int inASRetry = 0;
	BYTE szASGap[8] = {0};
	int inASGap = 0;
	BYTE szASEnable[8] = {0};
	int inASEnable = 0;
	BYTE szASRetryCounter[8] = {0};
	int inASRetryCounter = 0;
	BYTE szASExecute[8] = {0};
	int inASExecute = 0;
	BYTE szASLastSettle[8] = {0};
	BYTE szTemp[8 + 1] = {0};
	BOOL fPerformed = FALSE;
	BYTE szTemp1[40+1]={0};
	int iPerformed = 0;
	int iMustSettleCount = 0;
	int inCurrDate = 0; 

	int inSettle = 0;
	
	vdDebug_LogPrintf("saturn --inCTOS_CheckAutoSettlement--");

	//if (fFSRMode() == TRUE)
	//	return d_OK;

	memset(szTemp, 0x00, sizeof(szTemp));
	strAST.inASType = 0;
	inCTOSS_GetEnvDB("ASTYPE", szTemp);
	if (strlen(szTemp) > 0)
		strAST.inASType = wub_str_2_long(szTemp);


    if (strAST.inASType == 0)
		return FALSE;
	else{
		// Current Date
		memset(szCurrDate, 0x00, sizeof(szCurrDate));
		CTOS_RTCGet(&SetRTC);
		sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
		inCurrDate = wub_str_2_long(szCurrDate);
		
		// Current Time
		memset(szCurrTime, 0x00, sizeof(szCurrTime));
		CTOS_RTCGet(&rtcClock);
		sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
		inCurrTime = wub_str_2_long(szCurrTime);
					
		inBatchCnt = inCheckBatcheNotEmtpy();
		vdDebug_LogPrintf("saturn AS::inBatchCnt=[%d]", inBatchCnt);			
		
		vdSetASParams();
		
		vdDebug_LogPrintf("AS::strAST.inASNextTime=[%d], inASRetryCounter=[%d], inASRetryLimit=[%d]", strAST.inASNextTime, strAST.inASRetryCounter, strAST.inASRetryLimit);
		if (strAST.inASNextTime > 0 && strAST.inASRetryCounter > 0)
		{
			strAST.inASTime = strAST.inASNextTime;
		}


		vdDebug_LogPrintf("saturn currdate = %d", inCurrDate);
		vdDebug_LogPrintf("saturn inASDate = %s %d", strAST.szASDate, atoi(strAST.szASDate));

		
		vdDebug_LogPrintf("saturn inCurrTime = %d", inCurrTime);
		vdDebug_LogPrintf("saturn inASTime = %d", strAST.inASTime);
		if (inCurrTime >= strAST.inASTime && strAST.inASDate < inCurrDate) 
		{
		    vdDebug_LogPrintf("saturn inASPerformed true");
			strAST.inASPerformed = 1;				
		}
		else
		{
		    
		    vdDebug_LogPrintf("saturn inASPerformed false");
			strAST.inASPerformed = 0;
		}



		
		if (strAST.inASPerformed > 0 && inBatchCnt <=0)
		{
			vdDebug_LogPrintf("Performed with no batch record found.!!!");
			vdDebug_LogPrintf("AAA - szCurrDate[%s]", szCurrDate);
			CTOS_RTCGet(&SetRTC);
			sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
			vdDebug_LogPrintf("szCurrDate[%s]", szCurrDate);
			put_env_char("ASDATE",szCurrDate);
		}


	}

	return strAST.inASPerformed;
	

}

#endif

// Transfer to function vdUpdatePreCompTotal, failed or not sending of 0220 must update precomp total -- sidumili
void vdUpdatePreCompTotal(void)
{
	vdDebug_LogPrintf("--vdUpdatePreCompTotal--");
	vdDebug_LogPrintf("byTransType=[%d]", srTransRec.byTransType);
	
	if(srTransRec.byTransType == PREAUTH_VER)
	{
		srTransRec.byTransType=PREAUTH_COMP;
	    inCTOS_UpdateAccumPreCompTotal(&srTransRec);						
	}
}

void vdCheckForDualCard(void)
{
	BOOL fDualCardSupport = inFLGGet("fDualCard");
	
	vdDebug_LogPrintf("--vdCheckForDualCard--");
	vdDebug_LogPrintf("byTransType=[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("fDualCardSupport=[%d]", fDualCardSupport);
	vdDebug_LogPrintf("fBancnet=[%d]", strCDT.fBancnet);
	vdDebug_LogPrintf("inDualCardIITid=[%d]", strCDT.inDualCardIITid);
	vdDebug_LogPrintf("fBancNetTrans=[%d]", fBancNetTrans);

	if(inFLGGet("fDualCard") == TRUE)
	{
		if (strCDT.fBancnet == TRUE)
		{
			if (fBancNetTrans == FALSE)
			{
				strCDT.HDTid = MCC_HOST;

				if (strCDT.inDualCardIITid > 0)
				{				
					strCDT.IITid = strCDT.inDualCardIITid;
					srTransRec.IITid = strCDT.inDualCardIITid;
					inIITRead(strCDT.IITid);
					strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
				}
			}
			else
			{
				strCDT.HDTid = BANCNET_HOST;
				srTransRec.IITid = BANCNET_ISSUER;
				strCDT.IITid = BANCNET_ISSUER;
				inIITRead(strCDT.IITid);
				strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
			}
									
		}
	}
	
	vdDebug_LogPrintf("strCDT.HDTid=[%d]", strCDT.HDTid);
	vdDebug_LogPrintf("strCDT.IITid=[%d]", strCDT.IITid);	
	vdDebug_LogPrintf("strIIT.szIssuerLabel=[%s]", strIIT.szIssuerLabel);
	vdDebug_LogPrintf("strIIT.szIssuerLogo=[%s]", strIIT.szIssuerLogo);
	vdDebug_LogPrintf("srTransRec.szCardLable=[%s]", srTransRec.szCardLable);
}

#if 0
int inCheckBattery(void){
	int inRet;

	if (strTCT.inMinBattery > 0){
		inRet = inGetBatteryLevel();
		if (inRet < strTCT.inMinBattery){
			vdDisplayErrorMsg2(1, 8, "BATTERY LOW", "PLEASE CHARGE");
			return d_NO;
		}	
		
	}else{		
		return d_OK;
	}

	return d_OK;
}
#endif

// sidumili: For global variable
void vdGetGlobalVariable(void)
{
	vdDebug_LogPrintf("--vdGetGlobalVariable--");
		
	strGBLVar.fGBLvConfirmPrint= (get_env_int("CONFIRMPRINTRPT") > 0 ? TRUE : TRUE);
	strGBLVar.fGBLvDCCDev= (get_env_int("DCCDEV") > 0 ? TRUE : FALSE);

	vdDebug_LogPrintf("fGBLvConfirmPrint=[%d]", strGBLVar.fGBLvConfirmPrint);
	vdDebug_LogPrintf("fGBLvDCCDev=[%d]", strGBLVar.fGBLvDCCDev);
	
}

// sidumili: added for edit host
//void vdCTOS_EditHost(void)  //tine: modify to return int value, for handling Cancel function
int vdCTOS_EditHost(void)
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
    int inNum = 0, inRet;
    BYTE inKey = d_NO;
    BYTE szOutputBuf[512] = {0};
    BYTE szTemp[40 + 1] = {0};
    USHORT inLenMID =0;

    EditHostFlow:
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	//CTOS_LCDTClearDisplay();

    shHostIndex=strHDT.inHostIndex;

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return -1;

	if(inCheckBatcheNotEmtpy() > 0)
	{	
		vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
	    return -1;
	}
	
	inTCTRead(1);
    inHDTRead(shHostIndex);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Enable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fHostEnable", "Host Enable?", strHDT.fHostEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// TLE Enable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fHostTLESec", "TLE Enable?", strHDT.fHostTLESec, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// TID
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTID", "Terminal ID", strMMT[0].szTID, 8, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

         // MID
         if ((strHDT.inHostIndex==WECHAT_HOST) || (strHDT.inHostIndex==ALIPAY_HOST) || (strHDT.inHostIndex==GCASH_HOST) || (strHDT.inHostIndex==GRAB_HOST) || (strHDT.inHostIndex==UPI_HOST))
              inLenMID = 12;
	else
	     inLenMID = 15;
         	
              memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
              sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szMID", "Merchant ID", strMMT[0].szMID, inLenMID, TRUE, FALSE);
              strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
              strcat(strWriteFile.szWriteData, "\n");
      
	// TPDU
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTPDU", "TPDU", szTempBuf, 10, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// NII
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strHDT.szNII,szTempBuf,2);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szNII", "NII", szTempBuf, 4, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// System PWD
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSystemPW", "System PWD", strTCT.szSystemPW, PWD_LEN, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Engineer PWD
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szEngineerPW", "Engineer PWD", strTCT.szEngineerPW, PWD_LEN, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Tip Allow
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fTipAllowFlag", "Tip Allow?", strHDT.fTipAllowFlag, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Batch No
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strMMT[0].szBatchNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szBatchNo", "Batch No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Trace No
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strHDT.szTraceNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTraceNo", "Trace No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Invoice No
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strTCT.szInvoiceNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szInvoiceNo", "Invoice No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("HOST SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("AAA - szOutputBuf[%s]", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // fHostEnable
			strHDT.fHostEnable = (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));	
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // fHostTLESec
			strHDT.fHostTLESec = (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			GetDelimitedString(szOutputBuf, 6, '|', strMMT[0].szTID); // szTID
			
			GetDelimitedString(szOutputBuf, 7, '|', strMMT[0].szMID); // szMID

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 8, '|', szTemp); // szTPDU
			inAscii2Bcd(szTemp, strHDT.szTPDU, 5);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // szNII
			inAscii2Bcd(szTemp, strHDT.szNII, 2);
			
			GetDelimitedString(szOutputBuf, 10, '|', strTCT.szSystemPW); // szSystemPW
			
			GetDelimitedString(szOutputBuf, 11, '|', strTCT.szEngineerPW); // szEngineerPW
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 12, '|', szTemp); // fTipAllowFlag
			strHDT.fTipAllowFlag = (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // szBatchNo
			inAscii2Bcd(szTemp, strMMT[0].szBatchNo, 3);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // szTraceNo
			inAscii2Bcd(szTemp, strHDT.szTraceNo, 3);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 15, '|', szTemp); // szInvoiceNo
			inAscii2Bcd(szTemp, strTCT.szInvoiceNo, 3);

			// Update MMT
			inMMTSave(strMMT[0].MMTid);

			// Update TCT
			inTCTSave(1);

			// Update HDT
			inHDTSave(strHDT.inHostIndex);
		
			vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "HOST SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			break;
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	goto EditHostFlow;
	
}

void vdDeleteFile(char* szFileName)
{
	char szSystemCmdPath[128+1];    

	vdDebug_LogPrintf("--vdDeleteFile--");
	
    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
    sprintf(szSystemCmdPath, "rm %s", szFileName);
    system(szSystemCmdPath);

	vdDebug_LogPrintf("szSystemCmdPath[%s]", szSystemCmdPath);
}

int inSaveFile(char* szWriteData, int inFileSize, char* szFileName)
{
	int inRetVal = 0;

	vdDebug_LogPrintf("--inSaveFile--");
	vdDebug_LogPrintf("inFileSize=[%d]", inFileSize);
	vdDebug_LogPrintf("szWriteData=[%s]", szWriteData);
	
	if (inFileSize > 0)
	{
		inRetVal = inWriteFile(szFileName,szWriteData,inFileSize);
		if (inRetVal <= 0)
		  return d_NO;
	}

	return d_OK;
}

int inGetSERNUM(char* szOutValue)
{
	BYTE szTemp[100] = {0};

	memset(szTemp, 0x00, sizeof(szTemp));
	inCTOSS_GetEnvDB("SERNUM", szTemp);

	if (strcmp(szTemp, "szValue") == 0)
	{
		memset(szTemp, 0x00, sizeof(szTemp));
		strcpy(szOutValue, szTemp);
	}
	else
	{
		strcpy(szOutValue, szTemp);
	}

	return d_OK;
}

// sidumili: added for edit IP
//void vdCTOS_EditIP(void)
int vdCTOS_EditIP(void)		//tine:  modify to return int value, for handling Cancel function
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
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};

    EditIPFlow:
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	CTOS_LCDTClearDisplay();

    shHostIndex=strHDT.inHostIndex;
	
	//tine:  no need to check multiple MID for EditIP
    //inResult = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inResult)
    //    return -1;

	if(inCheckBatcheNotEmtpy() > 0)
	{	
		vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
	    return -1;
	}
	
	inTCTRead(1);
    inHDTRead(shHostIndex);
	inCPTRead(shHostIndex);
	inTCPRead(1);

	vdDeleteFile(EDIT_FILE); // Delete file
	
	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// inCommunicationMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inCommunicationMode", "Connection Type", strCPT.inCommunicationMode, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inIPHeader
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inIPHeader", "IP Header", strCPT.inIPHeader, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPriTxnHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPriTxnHostIP", "Primary Host IP", strCPT.szPriTxnHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inPriTxnHostPortNum
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPriTxnHostPortNum", "Primary Host Port", strCPT.inPriTxnHostPortNum, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szSecTxnHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSecTxnHostIP", "Secondary Host IP", strCPT.szSecTxnHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inSecTxnHostPortNum
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inSecTxnHostPortNum", "Secondary Host Port", strCPT.inSecTxnHostPortNum, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPriSettlementHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPriSettlementHostIP", "Settle Primary Host IP", strCPT.szPriSettlementHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inPriSettlementHostPort
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPriSettlementHostPort", "Settle Primary Host Port", strCPT.inPriSettlementHostPort, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szSecSettlementHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSecSettlementHostIP", "Settle Secondary Host IP", strCPT.szSecSettlementHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inSecSettlementHostPort
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inSecSettlementHostPort", "Settle Secondary Host Port", strCPT.inSecSettlementHostPort, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fDHCPEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDHCPEnable", "[0-Static/1-DHCP]", strTCP.fDHCPEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// szTerminalIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTerminalIP", "Terminal IP", strTCP.szTerminalIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szSubNetMask
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSubNetMask", "SubNet Mask IP", strTCP.szSubNetMask, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szGetWay
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szGetWay", "Gateway IP", strTCP.szGetWay, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szHostDNS1
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostDNS1", "DNS1 IP", strTCP.szHostDNS1, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szHostDNS2
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostDNS2", "DNS2 IP", strTCP.szHostDNS2, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("IP SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel
						
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // inCommunicationMode
			strCPT.inCommunicationMode = atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // inIPHeader
			strCPT.inIPHeader= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 6, '|', strCPT.szPriTxnHostIP); // szPriTxnHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // inPriTxnHostPortNum
			strCPT.inPriTxnHostPortNum= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 8, '|', strCPT.szSecTxnHostIP); // szSecTxnHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // inSecTxnHostPortNum
			strCPT.inSecTxnHostPortNum= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 10, '|', strCPT.szPriSettlementHostIP); // szPriSettlementHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 11, '|', szTemp); // inPriSettlementHostPort
			strCPT.inPriSettlementHostPort= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 12, '|', strCPT.szSecSettlementHostIP); // szSecSettlementHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // inSecSettlementHostPort
			strCPT.inSecSettlementHostPort= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // fDHCPEnable
			strTCP.fDHCPEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 15, '|', strTCP.szTerminalIP); // szTerminalIP
			GetDelimitedString(szOutputBuf, 16, '|', strTCP.szSubNetMask); // szSubNetMask
			GetDelimitedString(szOutputBuf, 17, '|', strTCP.szGetWay); // szGetWay
			GetDelimitedString(szOutputBuf, 18, '|', strTCP.szHostDNS1); // szHostDNS1
			GetDelimitedString(szOutputBuf, 19, '|', strTCP.szHostDNS2); // szHostDNS2

			// Update CPT
			inCPTSave(shHostIndex);

			// Update TCP
			inTCPSave(1);
		
			vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "IP SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			break;
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	goto EditIPFlow;
	
}

// sidumili: added for edit Terminal
void vdCTOS_EditTerminal(void)
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
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};
    
	//CTOS_LCDTClearDisplay();

    if(inCheckAllBatchEmtpy() > 0)
    {
        vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
        return;
    }
	
	inTCTRead(1);
    inHDTRead(MCC_HOST);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inMenuid
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inMenuid", "Menu ID", strTCT.inMenuid, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inMenuidNHProfileMenu
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inMenuidNHProfileMenu", "Profile Menu ID", strTCT.inMenuidNHProfileMenu, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// fPrintBankCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintBankCopy", "Print Bank Copy?", strTCT.fPrintBankCopy, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintMerchCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintMerchCopy", "Print Merch Copy?", strTCT.fPrintMerchCopy, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fManualEntry
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fManualEntry", "Enable Manual Entry?", strTCT.fManualEntry, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fRegUSD
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fRegUSD", "Terminal USD Reg?", strTCT.fRegUSD, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fTSNEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fTSNEnable", "Enable TSN (Terminal SN)?", strTCT.fTSNEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szOfflineCeilingAmt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szOfflineCeilingAmt", "Offline Ceilling Amt", strTCT.szOfflineCeilingAmt, 12, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintReceipt", "Enable Print Receipt?", strTCT.fPrintReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fEMVOnlinePIN
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fEMVOnlinePIN", "Enable EMV Offline PIN?", strTCT.fEMVOnlinePIN, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byIdleSaleType
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byIdleSaleType", "Idle Sale Type", strTCT.byIdleSaleType, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fIdleSwipeAllow
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fIdleSwipeAllow", "Enable Idle Swipe?", strTCT.fIdleSwipeAllow, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fIdleInsertAllow
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fIdleInsertAllow", "Enable Idle Insert?", strTCT.fIdleInsertAllow, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ISO Log On/Off
    memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
    sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inISOLogger", "ISO LOG 1-ON 0-OFF", strTCT.inISOLogger, 1, FALSE, FALSE);
    strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
    strcat(strWriteFile.szWriteData, "\n");
        
    // inISOLog No. of Trans Limit
    memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
    sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inISOTxnLogLimit", "ISO LOG No. of Trans Limit", strTCT.inISOTxnLogLimit, 3, FALSE, FALSE);
    strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
    strcat(strWriteFile.szWriteData, "\n");

    memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
    sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inISOTxnLogLimit", "ISO LOG No. of Trans Limit", strTCT.inISOTxnLogLimit, 3, FALSE, FALSE);
    strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
    strcat(strWriteFile.szWriteData, "\n");

	// fPrintISOMessage
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintISOMessage", "Enable Print ISO?", strTCT.fPrintISOMessage, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("TERMINAL SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // inMenuid
			strTCT.inMenuid= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // inMenuidNHProfileMenu
			strTCT.inMenuidNHProfileMenu= atoi(szTemp);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 6, '|', szTemp); // fPrintBankCopy
			strTCT.fPrintBankCopy = (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // fPrintMerchCopy
			strTCT.fPrintMerchCopy= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 8, '|', szTemp); // fManualEntry
			strTCT.fManualEntry= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // fRegUSD
			strTCT.fRegUSD= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 10, '|', szTemp); // fTSNEnable
			strTCT.fTSNEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 11, '|', strTCT.szOfflineCeilingAmt); // szOfflineCeilingAmt	

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 12, '|', szTemp); // fPrintReceipt
			strTCT.fPrintReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // fEMVOnlinePIN
			strTCT.fEMVOnlinePIN= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // byIdleSaleType
			strTCT.byIdleSaleType= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 15, '|', szTemp); // fIdleSwipeAllow
			strTCT.fIdleSwipeAllow= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 16, '|', szTemp); // fIdleInsertAllow
			strTCT.fIdleInsertAllow= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 17, '|', szTemp); // inISOLogger
			strTCT.inISOLogger= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 18, '|', szTemp); // inISOTxnLogLimit
			strTCT.inISOTxnLogLimit= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 19, '|', szTemp); // fPrintISOMessage
			strTCT.fPrintISOMessage = (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			// Update TCT
			inTCTSave(1);
		
			vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "TERMINAL SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			break;
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");	
}

// sidumili: added for edit Printer
//void vdCTOS_EditPrinter(void)
int vdCTOS_EditPrinter(void)		////tine:  modify to return int value, for handling Cancel function
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
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};

    EditPrinter:
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	CTOS_LCDTClearDisplay();

    shHostIndex=strHDT.inHostIndex;

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return -1;

	if(inCheckBatcheNotEmtpy() > 0)
	{	
		vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
	    return -1;
	}
		
	inTCTRead(1);
    inHDTRead(shHostIndex);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// szRctHdr1
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr1", "Receipt Header Line 1", strMMT[0].szRctHdr1, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctHdr2
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr2", "Receipt Header Line 2", strMMT[0].szRctHdr2, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctHdr3
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr3", "Receipt Header Line 3", strMMT[0].szRctHdr3, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctHdr4
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr4", "Receipt Header Line 4", strMMT[0].szRctHdr4, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szRctHdr5
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr5", "Receipt Header Line 5", strMMT[0].szRctHdr5, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctFoot1
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctFoot1", "Receipt Footer Line 1", strMMT[0].szRctFoot1, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctFoot2
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctFoot2", "Receipt Footer Line 2", strMMT[0].szRctFoot2, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctFoot3
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctFoot3", "Receipt Footer Line 3", strMMT[0].szRctFoot3, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("PRINTER SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			GetDelimitedString(szOutputBuf, 4, '|', strMMT[0].szRctHdr1); // szRctHdr1
			GetDelimitedString(szOutputBuf, 5, '|', strMMT[0].szRctHdr2); // szRctHdr2
			GetDelimitedString(szOutputBuf, 6, '|', strMMT[0].szRctHdr3); // szRctHdr3
			GetDelimitedString(szOutputBuf, 7, '|', strMMT[0].szRctHdr4); // szRctHdr4
			GetDelimitedString(szOutputBuf, 8, '|', strMMT[0].szRctHdr5); // szRctHdr5
			GetDelimitedString(szOutputBuf, 9, '|', strMMT[0].szRctFoot1); // szRctFoot1
			GetDelimitedString(szOutputBuf, 10, '|', strMMT[0].szRctFoot2); // szRctFoot2
			GetDelimitedString(szOutputBuf, 11, '|', strMMT[0].szRctFoot3); // szRctFoot3

			// Update MMT
			inMMTSave(strMMT[0].MMTid);
		
			vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "PRINTER SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			break;
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	goto EditPrinter;
}

void vdCTOS_DeleteISOLog(void)	
{
   inDatabase_DeleteISOLogAll();

   strTCT.inISOTxnLogCnt = 0;
   inTCTSave(1);
   
   vdDisplayMessageStatusBox(1, 8, "ISO LOGS", " DELETED", MSG_TYPE_INFO);

   CTOS_Delay(1000);
   CTOS_Beep();
}

// sidumili - view user manual
void vdCTOS_UserManual(void)
{
	BYTE szTemp[40 + 1] = {0};
	BYTE szAppName[40 + 1] = {0};
	int inRet = d_NO;
	char szChoiceMsg[30 + 1];
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04, key=0; 
	
	vdDebug_LogPrintf("--vdCTOS_UserManual--");

	ShowAgain:
		
	memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	strcpy(szHeaderString, "SELECT USER MANUAL");
	strcat(szChoiceMsg,"CREDIT \n");
	strcat(szChoiceMsg,"BANCNET \n");
	strcat(szChoiceMsg,"QRPAY");
	key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);

	memset(szTemp, 0x00, sizeof(szTemp));
	if (key > 0)
	{
		if (key == 1)
		{
			strcpy(szAppName, "s1_credit_tog.pdf");
			sprintf(szTemp, "%s|%s", "CREDIT USER MANUAL", szAppName);
		}
		if (key == 2)
		{
			strcpy(szAppName, "s1_bancnet_tog.pdf");
			sprintf(szTemp, "%s|%s", "BANCNET USER MANUAL", szAppName);
		}

		if (key == 3)
		{
			strcpy(szAppName, "s1_qrpay_tog.pdf");
			sprintf(szTemp, "%s|%s", "QRPAY USER MANUAL", szAppName);
		}
		
		if (key == d_KBD_CANCEL)
		{
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(1500);
			return;
		}

		if (key == 0xFF)
		{
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(1500);
			return;
		}

//		inRet = usCTOSS_UserManualUI(szTemp);
			
		DisplayStatusLine(" "); // Clear previous display
		
		goto ShowAgain;		
	}	
	
}

void vdCTOS_ReceiptSelect(void)
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
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};
    
	//CTOS_LCDTClearDisplay();

    if(inCheckAllBatchEmtpy() > 0)
    {
        vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
        return;
    }
	
	inTCTRead(1);
    inHDTRead(MCC_HOST);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// fPrintBankCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintBankCopy", "Print Bank Copy?", strTCT.fPrintBankCopy, 1, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintMerchCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintMerchCopy", "Print Merch Copy?", strTCT.fPrintMerchCopy, 1, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("SELECT RECEIPT|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			break;
		case d_OK:
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 3, '|', szTemp); // fPrintBankCopy
			strTCT.fPrintBankCopy = (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // fPrintMerchCopy
			strTCT.fPrintMerchCopy= (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			// Update TCT
			inTCTSave(1);
		
			vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "RECEIPT SELECTION", " UPDATE COMPLETE", MSG_TYPE_INFO);
			break;
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");	
}

int vdCTOS_CTMSUPDATE(void)
{
     int inRet = 0;

	 vdCTOS_SetTransType(CTMS_UPDATE);
	 
     inRet = inCTOS_GetTxnPassword();
     if(d_OK != inRet)
          return inRet;

    inRet = inCheckAllBatchEmtpy();
    vdDebug_LogPrintf("AAA - vdCTOS_CTMSUPDATE inCheckAllBatchEmtpy[%d]", inRet);
    if(inRet > 0)
    {
         vdDisplayErrorMsg(1, 8, "BATCH NOT EMPTY.");
         return d_NO;
    }
	
    inCTOS_CTMSUPDATE();

	return d_OK;
}

void vdFormatPANForECR(char* szInPAN, char* szOutPAN){
int inLen1, inLen2;
char szTemp1[6 + 1] = {0};
char szTemp2[4 + 1] = {0};
char szTemp3[10 + 1] = {0};

inLen1 = strlen(szInPAN);

memset(szTemp1, 0x00, sizeof(szTemp1));
memset(szTemp2, 0x00, sizeof(szTemp2));
memset(szTemp3, 0x00, sizeof(szTemp3));

memcpy(szTemp1, &szInPAN[0], 6);
memcpy(szTemp2, &szInPAN[inLen1 - 4], 4);

inLen2 = inLen1 - 10;

vdCTOS_Pad_String(szTemp3, inLen2, '0', POSITION_LEFT);

sprintf(szOutPAN, "%s%s%s", szTemp1, szTemp3, szTemp2);

vdDebug_LogPrintf("szTemp1[%s]|szTemp2[%s]|szTemp3[%s]", szTemp1, szTemp2, szTemp3);
vdDebug_LogPrintf("inLen1[%d]|inLen2[%d]", inLen1, inLen2);
vdDebug_LogPrintf("szOutPAN[%s]", szOutPAN);

}

void vdSetECRResponse(char* szECRResponse)
{
	vdDebug_LogPrintf("vdSetECRResponse :: szECRResponse :: [%s]",szECRResponse);
	memset(srTransRec.szECRRespCode,0x00,sizeof(srTransRec.szECRRespCode));
	strcpy(srTransRec.szRespCode,"");
	strcpy(srTransRec.szECRRespCode, szECRResponse); //make responce code as -1 - for ECR transaction 
}

void vdCTOS_ECRSettings(void)
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
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};
    
	//CTOS_LCDTClearDisplay();

    /*if(inCheckAllBatchEmtpy() > 0)
    {
        vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
        return;
    }*/
	
	inTCTRead(1);
    inHDTRead(MCC_HOST);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// ECR Flag
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fECR", "ECR Flag", strTCT.fECR, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ECR Port
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byRS232ECRPort", "ECR Port", strTCT.byRS232ECRPort, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ECR Baud Rate
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byRS232ECRBaud", "ECR Baud Rate", strTCT.byRS232ECRBaud, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// ECR Credit
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fECRSale", "ECR Credit?", strTCT.fECRSale, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ECR Debit
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fECRBancnet", "ECR Bancnet?", strTCT.fECRBancnet, 1, FALSE, FALSE );
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ECR QRPay
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fECRQRPay", "ECR QRPay?", strTCT.fECRQRPay, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("ECR SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // fECR
			strTCT.fECR= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // byRS232ECRPort
			strTCT.byRS232ECRPort= atoi(szTemp);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 6, '|', szTemp); // byRS232ECRBaud
			strTCT.byRS232ECRBaud = (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // fECRSale
			strTCT.fECRSale= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 8, '|', szTemp); // fECRBancnet
			strTCT.fECRBancnet= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // fECRQRPay
			strTCT.fECRQRPay= (atoi(szTemp) > 0 ? TRUE: FALSE);

			// Update TCT
			inTCTSave(1);
		
			vdDisplayMessageBox(1, 8, "ECR UPDATE SETTING", "COMPLETE", "PLEASE RESTART TERMINAL", MSG_TYPE_INFO);
			break;
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(5000);
	usCTOSS_LCDDisplay(" ");	
}

int inCTOS_SetupMenu(void)
{
    int inRet, inSaleType=0;
    char trxlogo[20+1];
	
    srTransRec.byTransType = SETUP;
    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;
    
    inSaleType = 0;
    memset(trxlogo, 0x00, sizeof(trxlogo));
    strcpy(trxlogo, "allcards.bmp");
    vdDebug_LogPrintf("trxlogo [%s], inSaleType [%d]", trxlogo, inSaleType);
    vdDisplayTrxn(inSaleType, trxlogo);
			
	return d_OK;
}

int inCTOS_ReportMenu(void)
{
    int inRet, inSaleType=0;
    char trxlogo[20+1];
	
	inSaleType = 'R';
	memset(trxlogo, 0x00, sizeof(trxlogo));
	strcpy(trxlogo, "allcards.bmp");
	vdDisplayTrxn(inSaleType, trxlogo);

	return d_OK;
}

int inCTOS_SettlementMenu(void)
{
    int inRet, inSaleType=0;
    char trxlogo[20+1];

    srTransRec.byTransType = SETUP;
    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;
	
	inSaleType = 'P';
	memset(trxlogo, 0x00, sizeof(trxlogo));
	strcpy(trxlogo, "allcards.bmp");
	vdDisplayTrxn(inSaleType, trxlogo);

	return d_OK;
}

int inDeleteUser(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[64+1] = "DELETE USER";
	char szUserName[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
	
	unsigned char szOutput[30], szPassword[40+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	int inResult=0;
	
    memset(szUserName, 0x00, sizeof(szUserName));
    //memset(szTerm, 0x00, sizeof(szTerm));

//    CTOS_PrinterPutString("inCTOS_SelectInstallmentTerm");

	vdDebug_LogPrintf("inDeleteUser");

	inNumRecs=inUSRNumRecord();
    
	vdDebug_LogPrintf("inNumRecs: %d", inNumRecs);

	inDatabase_TerminalOpenDatabase();
	memset(strMultiUSR, 0, sizeof(strMultiUSR));
	inUSRReadEx(1);
    inDatabase_TerminalCloseDatabase();

	for (i=0; i < inNumRecs; i++)
	{
		if (strMultiUSR[i].szUserName[0] != 0)
		{
			inIndex=i;
			strcat((char *)szUserName, strMultiUSR[i].szUserName);

			
			if (strMultiUSR[i+1].szUserName[0] != 0)
				strcat((char *)szUserName, (char *)" \n");
		}
	}

    vdDebug_LogPrintf("all szUserName[%s]", szUserName);

	if(inNumRecs > 0)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szUserName, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        //CTOS_LCDTClearDisplay();
        //setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        //vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

        memset(szHeaderString, 0, sizeof(szHeaderString));
		sprintf(szHeaderString, "DELETE %s?", strMultiUSR[key-1].szUserName);
		inResult=vduiAskConfirmation(szHeaderString);
		if(inResult == d_OK)
		{
            inUSRDeleteRecord(strMultiUSR[key-1].szUserName);
            
            vdDisplayMessageBox(1, 8, "USER DELETED", "RECORD DONE", "", MSG_TYPE_INFO);
            CTOS_Beep();
            CTOS_Delay(1000);
		}
		
        key=d_OK;
    }

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	
    return key;
}

int inDeleteBiller(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[64+1] = "DELETE BILLER";
	char szBillerName[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
	
	unsigned char szOutput[30], szPassword[40+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	int inResult=0;
	
    memset(szBillerName, 0x00, sizeof(szBillerName));
    //memset(szTerm, 0x00, sizeof(szTerm));

//    CTOS_PrinterPutString("inCTOS_SelectInstallmentTerm");

	vdDebug_LogPrintf("inDeleteBiller");

	inNumRecs=inBLRNumRecord();
    
	vdDebug_LogPrintf("inNumRecs: %d", inNumRecs);

	inDatabase_TerminalOpenDatabase();
	memset(strMultiBLR, 0, sizeof(strMultiBLR));
	inBLRReadEx(1);
    inDatabase_TerminalCloseDatabase();

	for (i=0; i < inNumRecs; i++)
	{
		if (strMultiBLR[i].szBillerName[0] != 0)
		{
			inIndex=i;
			strcat((char *)szBillerName, strMultiBLR[i].szBillerName);

			
			if (strMultiBLR[i+1].szBillerName[0] != 0)
				strcat((char *)szBillerName, (char *)" \n");
		}
	}

    vdDebug_LogPrintf("all szBillerName[%s]", szBillerName);

	if(inNumRecs > 0)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szBillerName, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        //CTOS_LCDTClearDisplay();
        //setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        //vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

        memset(szHeaderString, 0, sizeof(szHeaderString));
		sprintf(szHeaderString, "DELETE BILLER: %s?", strMultiBLR[key-1].szBillerName);
		inResult=vduiAskConfirmation(szHeaderString);
		if(inResult == d_OK)
		{
            inDLRDeleteRecord(strMultiBLR[key-1].szBillerName);
            
            vdDisplayMessageBox(1, 8, "BILLER DELETED", "RECORD DONE", "", MSG_TYPE_INFO);
            CTOS_Beep();
            CTOS_Delay(1000);
		}
		
        key=d_OK;
    }

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	
    return key;
}

int inEditUser(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[64+1] = "EDIT USER";
	char szUserName[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
	BYTE szOutputBuf[512] = {0};
	
	unsigned char szOutput[30], szPassword[40+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	int inResult=0;
	
	vdDebug_LogPrintf("inEditUser");

	EditIPFlow:
	memset(szUserName, 0x00, sizeof(szUserName));
	inNumRecs=inUSRNumRecord();
    
	vdDebug_LogPrintf("inNumRecs: %d", inNumRecs);

	inDatabase_TerminalOpenDatabase();
	memset(strMultiUSR, 0, sizeof(strMultiUSR));
	inUSRReadEx(1);
    inDatabase_TerminalCloseDatabase();

	for (i=0; i < inNumRecs; i++)
	{
		if (strMultiUSR[i].szUserName[0] != 0)
		{
			inIndex=i;
			strcat((char *)szUserName, strMultiUSR[i].szUserName);

			
			if (strMultiUSR[i+1].szUserName[0] != 0)
				strcat((char *)szUserName, (char *)" \n");
		}
	}

    vdDebug_LogPrintf("all szUserName[%s]", szUserName);

	if(inNumRecs > 0)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szUserName, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        //CTOS_LCDTClearDisplay();
        //setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        //vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

		if(inCheckBatcheNotEmtpy() > 0)
		{	
			vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
			return -1;
		}

		inIndex=key-1;
		
		#if 1
		inTCTRead(1);
		inHDTRead(2);
		inCPTRead(2);
		inTCPRead(1);
		#endif
		
		vdDeleteFile(EDIT_FILE); // Delete file
		
		memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
		strWriteFile.inWriteSize = 0;
		
		// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
		
		// Host Index
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
		
		// Host Name
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");

		// Host Index
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "USRid", "User Index", strMultiUSR[inIndex].USRid, 2, FALSE, TRUE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
		
		// Host Name
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szUserName", "User Name", strMultiUSR[inIndex].szUserName, 12, FALSE, FALSE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
		
		// inCommunicationMode
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szPassword", "Password", strMultiUSR[inIndex].szPassword, 12, FALSE, FALSE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
			
		strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
		vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
			
		// Save to file
		inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);
		
		memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
		key = usCTOSS_EditInfoListViewUI("EDIT USER|x", szOutputBuf);

        switch (key)
        {
            case d_KBD_CANCEL:
                vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
            break;
			
            case d_OK:

				//memset(strUSR, 0, sizeof(strUSR));
				memset(&strUSR, 0x00, sizeof(STRUCT_USR));
				vdDebug_LogPrintf("inIndex: %d", inIndex);
				
                memset(szOutput, 0x00, sizeof(szOutput));
                GetDelimitedString(szOutputBuf, 4, '|', szOutput); // inCommunicationMode
                strUSR.USRid = atoi(szOutput);

                GetDelimitedString(szOutputBuf, 5, '|', strUSR.szUserName); // szPriTxnHostIP
                GetDelimitedString(szOutputBuf, 6, '|', strUSR.szPassword); // szPriTxnHostIP
				
                vdDebug_LogPrintf("strMultiUSR[inIndex].USRid: %d", strUSR.USRid);
				vdDebug_LogPrintf("strMultiUSR[inIndex].szUserName: %s", strUSR.szUserName);
                vdDebug_LogPrintf("strMultiUSR[inIndex].szPassword: %s", strUSR.szPassword);
				
                inUSRSave(strUSR.USRid);
                
                vdDisplayMessageBox(1, 8, "", "USER SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
            break;
			
            case 0xFF:
                vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
            break;
        }

        CTOS_Beep();
        CTOS_Delay(1000);
        usCTOSS_LCDDisplay(" ");
        goto EditIPFlow;
		
        key=d_OK;
    }

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	
    return key;
}

int inEditBiller(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[64+1] = "EDIT USER";
	char szBillerName[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
	BYTE szOutputBuf[512] = {0};
	
	unsigned char szOutput[30], szPassword[40+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	int inResult=0;
	
	vdDebug_LogPrintf("inEditUser");

	EditIPFlow:
	memset(szBillerName, 0x00, sizeof(szBillerName));
	inNumRecs=inBLRNumRecord();
    
	vdDebug_LogPrintf("inNumRecs: %d", inNumRecs);

	inDatabase_TerminalOpenDatabase();
	memset(strMultiBLR, 0, sizeof(strMultiBLR));
	inBLRReadEx(1);
    inDatabase_TerminalCloseDatabase();

	for (i=0; i < inNumRecs; i++)
	{
		if (strMultiBLR[i].szBillerName[0] != 0)
		{
			inIndex=i;
			strcat((char *)szBillerName, strMultiBLR[i].szBillerName);

			
			if (strMultiBLR[i+1].szBillerName[0] != 0)
				strcat((char *)szBillerName, (char *)" \n");
		}
	}

    vdDebug_LogPrintf("all szBillerName[%s]", szBillerName);

	if(inNumRecs > 0)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szBillerName, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        //CTOS_LCDTClearDisplay();
        //setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        //vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

		if(inCheckBatcheNotEmtpy() > 0)
		{	
			vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
			return -1;
		}

		inIndex=key-1;
		
		#if 1
		inTCTRead(1);
		inHDTRead(2);
		inCPTRead(2);
		inTCPRead(1);
		#endif
		
		vdDeleteFile(EDIT_FILE); // Delete file
		
		memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
		strWriteFile.inWriteSize = 0;
		
		// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length
		
		// Host Index
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
		
		// Host Name
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");

		// Host Index
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "BLRid", "Biller Index", strMultiBLR[inIndex].BLRid, 2, FALSE, TRUE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
		
		// Host Name
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szBillerName", "Biller Name", strMultiBLR[inIndex].szBillerName, 36, FALSE, FALSE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");
		
		// szBillerCode
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szBillerCode", "Biller Code", strMultiBLR[inIndex].szBillerCode, 4, FALSE, FALSE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");

		/* szBillerFee */
		memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
		sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szBillerFee", "Biller Fee", strMultiBLR[inIndex].szBillerFee, 12, FALSE, FALSE);
		strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
		strcat(strWriteFile.szWriteData, "\n");

		strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
		vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
			
		// Save to file
		inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);
		
		memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
		key = usCTOSS_EditInfoListViewUI("EDIT BILLER|x", szOutputBuf);

        switch (key)
        {
            case d_KBD_CANCEL:
                vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
            break;
			
            case d_OK:

				//memset(strUSR, 0, sizeof(strUSR));
				memset(&strBLR, 0x00, sizeof(STRUCT_BLR));
				vdDebug_LogPrintf("inIndex: %d", inIndex);
				
                memset(szOutput, 0x00, sizeof(szOutput));
                GetDelimitedString(szOutputBuf, 4, '|', szOutput); // inCommunicationMode
                strBLR.BLRid = atoi(szOutput);

                GetDelimitedString(szOutputBuf, 5, '|', strBLR.szBillerName); // szPriTxnHostIP
                GetDelimitedString(szOutputBuf, 6, '|', strBLR.szBillerCode); // szPriTxnHostIP
				GetDelimitedString(szOutputBuf, 7, '|', strBLR.szBillerFee); // szPriTxnHostIP
				
                vdDebug_LogPrintf("strBLR.BLRid: %d", strBLR.BLRid);
				vdDebug_LogPrintf("strBLR.szBillerName: %s", strBLR.szBillerName);
                vdDebug_LogPrintf("strBLR.szBillerCode: %s", strBLR.szBillerCode);
				vdDebug_LogPrintf("strBLR.szBillerFee: %s", strBLR.szBillerFee);
				
                inBLRSave(strBLR.BLRid);
                
                vdDisplayMessageBox(1, 8, "", "BILLER SETTING", "UPDATE COMPLETE", MSG_TYPE_INFO);
            break;
			
            case 0xFF:
                vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
            break;
        }

        CTOS_Beep();
        CTOS_Delay(1000);
        usCTOSS_LCDDisplay(" ");
        goto EditIPFlow;
		
        key=d_OK;
    }

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	
    return key;
}

int inInsertUser(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[64+1] = "ADD USER";
	char szUserName[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
	BYTE szOutputBuf[512] = {0};
	
	unsigned char szOutput[30], szPassword[40+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	int inResult=0;
	
	vdDebug_LogPrintf("inInsertUser");

	//EditIPFlow:
	if(inCheckBatcheNotEmtpy() > 0)
	{	
		vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
		return -1;
	}
	
	inIndex=key-1;
	
    #if 1
	inTCTRead(1);
	inHDTRead(2);
	inCPTRead(2);
	inTCPRead(1);
    #endif
	
	vdDeleteFile(EDIT_FILE); // Delete file
	memset(&strUSR, 0x00, sizeof(STRUCT_USR));
	
	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;
	
	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "USRid", "User Index", strUSR.USRid, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szUserName", "User Name", strUSR.szUserName, 12, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// inCommunicationMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szPassword", "Password", strUSR.szPassword, 12, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
		
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);
	
	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	key = usCTOSS_EditInfoListViewUI("ADD USER|ADD", szOutputBuf);
	
	switch (key)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		break;
		
		case d_OK:
	
			//memset(strUSR, 0, sizeof(strUSR));
			memset(&strUSR, 0x00, sizeof(STRUCT_USR));
			vdDebug_LogPrintf("inIndex: %d", inIndex);
			
			memset(szOutput, 0x00, sizeof(szOutput));
			GetDelimitedString(szOutputBuf, 4, '|', szOutput); // inCommunicationMode
			strUSR.USRid = atoi(szOutput);
	
			GetDelimitedString(szOutputBuf, 5, '|', strUSR.szUserName); // szPriTxnHostIP
			GetDelimitedString(szOutputBuf, 6, '|', strUSR.szPassword); // szPriTxnHostIP
			
			vdDebug_LogPrintf("strMultiUSR[inIndex].USRid: %d", strUSR.USRid);
			vdDebug_LogPrintf("strMultiUSR[inIndex].szUserName: %s", strUSR.szUserName);
			vdDebug_LogPrintf("strMultiUSR[inIndex].szPassword: %s", strUSR.szPassword);
			
			//inUSRSave(strUSR.USRid);
			if(strlen(strUSR.szUserName) == 0 && strlen(strUSR.szUserName) == 0)
			{
				vdDisplayMessageBox(1, 8, "", "USER SETTING", "UPDATE NOT COMPLETE", MSG_TYPE_INFO);
			}
			else
			{
			    inUSRInsert();
			    vdDisplayMessageBox(1, 8, "", "USER SETTING", "UPDATE COMPLETE", MSG_TYPE_INFO);
			}
		break;
		
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
		break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	//goto EditIPFlow;

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	key=d_OK;
	
    return key;
}

int inInsertBiller(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[64+1] = "ADD USER";
	char szUserName[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	char szTrxnListMenu[512 + 1], szTransFunc[256 + 1];
	BYTE szOutputBuf[512] = {0};
	
	unsigned char szOutput[30], szPassword[40+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	int inResult=0;
	
	vdDebug_LogPrintf("inInsertUser");

	//EditIPFlow:
	if(inCheckBatcheNotEmtpy() > 0)
	{	
		vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
		return -1;
	}
	
	inIndex=key-1;
	
    #if 1
	inTCTRead(1);
	inHDTRead(2);
	inCPTRead(2);
	inTCPRead(1);
    #endif
	
	vdDeleteFile(EDIT_FILE); // Delete file
	memset(&strBLR, 0x00, sizeof(STRUCT_BLR));
	
	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;
	
	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "BLRid", "Biller Index", strBLR.BLRid, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szBillerName", "Biller Name", strBLR.szBillerName, 12, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// inCommunicationMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szBillerCode", "Biller Code", strBLR.szBillerCode, 12, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/* szBillerFee */
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_STRING, "szBillerFee", "Biller Fee", strMultiBLR[inIndex].szBillerFee, 12, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
		
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);
	
	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	key = usCTOSS_EditInfoListViewUI("ADD USER|ADD", szOutputBuf);
	
	switch (key)
	{
		case d_KBD_CANCEL:
			vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		break;
		
		case d_OK:
	
			//memset(strUSR, 0, sizeof(strUSR));
			memset(&strUSR, 0x00, sizeof(STRUCT_USR));
			vdDebug_LogPrintf("inIndex: %d", inIndex);
			
			memset(szOutput, 0x00, sizeof(szOutput));
			GetDelimitedString(szOutputBuf, 4, '|', szOutput); // inCommunicationMode
			strUSR.USRid = atoi(szOutput);
	
			GetDelimitedString(szOutputBuf, 5, '|', strBLR.szBillerName); // szPriTxnHostIP
			GetDelimitedString(szOutputBuf, 6, '|', strBLR.szBillerCode); // szPriTxnHostIP
			GetDelimitedString(szOutputBuf, 7, '|', strBLR.szBillerFee); // szPriTxnHostIP
			
			vdDebug_LogPrintf("strBLR.USRid: %d", strBLR.BLRid);
			vdDebug_LogPrintf("strBLR.szBillerCode: %s", strBLR.szBillerName);
			vdDebug_LogPrintf("strBLR.szBillerName: %s", strBLR.szBillerCode);
			vdDebug_LogPrintf("strBLR.szBillerFee: %s", strBLR.szBillerFee);
			
			//inUSRSave(strUSR.USRid);
			if(strlen(strBLR.szBillerName) == 0 && strlen(strBLR.szBillerCode) == 0)
			{
				vdDisplayMessageBox(1, 8, "", "BILLER SETTING", "UPDATE NOT COMPLETE", MSG_TYPE_INFO);
			}
			else
			{
			    inBLRInsert();
			    vdDisplayMessageBox(1, 8, "", "BILLER SETTING", "UPDATE COMPLETE", MSG_TYPE_INFO);
			}
		break;
		
		case 0xFF:
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
		break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	//goto EditIPFlow;

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	key=d_OK;
	
    return key;
}

int inGetIdleTimeOut(BOOL fSecond){
int inTimeOut;

//inTimeOut = strTCT.inIdleTimeOut;
inTimeOut = 30;
if (inTimeOut <= 0) inTimeOut = 30;

// MS
if (fSecond == FALSE)
	inTimeOut = (inTimeOut * 100);

vdDebug_LogPrintf("::inGetIdleTimeOut inTimeOut[%d]", inTimeOut);

return(inTimeOut);

}
