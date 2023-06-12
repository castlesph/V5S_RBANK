

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <CTOSInput.h>

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
#include "../DataBase/DatabaseFunc.h"
#include "../Includes/POSTrans.h"
#include "..\Debug\Debug.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\ISOEnginee.h"
#include "..\Includes\EFTSec.h"
#include "..\Print\Print.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\Includes\MultiApLib.h"
#include <epad.h>
#include "..\Includes\CfgExpress.h"
#include "..\Includes\POSSettlement.h"
#include "..\ApTrans\MultiShareCOM.h"
#include "..\Includes\POSDiag.h"



#include "Poserm.h"
//#include "ERMDefine.h"
#define ERM_RECEIPT_FILE		    "/home/ap/pub/logo.bmp"
#define LOCAL_RECEIPT_FILE			"./fs_data/logo.bmp"
#define ERM_BUFF_FILE				"/home/ap/pub/ermbuff.txt"
#define ERM_RECVVIS_FILE			"/home/ap/pub/ermrecvvia.txt"


#define MAX_BUF	6000


BYTE szERMSendData[8094];
BYTE szERMReceiveData[8094];
int inSendLen;
int inReceiveLen;

BYTE g_szPhoneNo[30];
BYTE g_szEmail[100];
	
int ginSingatureStatus = 0;
extern BOOL fERMUploadFailed;

void vdGetCenter(unsigned char *strIn,char *strOut)
{
	short i,spacestring;
    USHORT usCharPerLine = 32;
        
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(strOut,0x20,55);
	memcpy(strOut+spacestring,strIn,usCharPerLine);	
	
	strOut[i+spacestring]=0;
}



int inCTOSS_FromHeader_Footer(void)
{
	unsigned char uszERCPrinterBuffer [500 + 1];
	int inERCParsePrinterSize = 0;
	char szAPName[50];
	int inAPPID;
	int inHostIndex;
	int inNumOfRecords = 0;
	char tembuf[3+1] = "\n";
	char szOutbuf[100+1];

#if 0	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
	vdDebug_LogPrintf("szAPName[%s]....",szAPName);

	inHostIndex = inHDTReadByApname(szAPName);
	vdDebug_LogPrintf("inHostIndex[%d]....",strHDT.inHostIndex);
#endif
	//if (inHostIndex > 0)
	//if (strHDT.inHostIndex > 0)
	{
		//inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfRecords);
		//vdDebug_LogPrintf("inNumOfRecords[%d]....",inNumOfRecords);

		//if (inNumOfRecords > 0)
		{
			memset(uszERCPrinterBuffer,0x00,sizeof(uszERCPrinterBuffer));
			inERCParsePrinterSize = 0;
			if (strlen(strMMT[0].szRctHdr1) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctHdr1,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctHdr2) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctHdr2,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctHdr3) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctHdr3,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctHdr4) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctHdr4,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctHdr5) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctHdr5,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			inWriteFile(ERC_SHARE_HEADER_FILE, uszERCPrinterBuffer, inERCParsePrinterSize);

			memset(uszERCPrinterBuffer,0x00,sizeof(uszERCPrinterBuffer));
			inERCParsePrinterSize = 0;
			if (strlen(strMMT[0].szRctFoot1) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctFoot1,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctFoot2) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctFoot2,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctFoot3) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctFoot3,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
			inERCParsePrinterSize += strlen(tembuf);

			strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,"   ***** MERCHANT COPY *****  ");
			inERCParsePrinterSize += strlen("   ***** MERCHANT COPY *****  ");
			strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
			inERCParsePrinterSize += strlen(tembuf);
			inWriteFile(ERC_SHARE_FOOTER_FILE, uszERCPrinterBuffer, inERCParsePrinterSize);

			return d_OK;
		}
	}	

	return d_NO;
}


int inCTOSS_ERM_Form_Receipt(int inSettle)
{
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	BYTE szfilename[128];
	int inResult;	

	vdDebug_LogPrintf("--inCTOSS_ERM_Form_Receipt--");
	vdDebug_LogPrintf("strTCT.byERMMode[%d] | strTCT.byTerminalType[%d] | inSettle[%d] | byTransType[%d]", strTCT.byERMMode, strTCT.byTerminalType, inSettle, srTransRec.byTransType);
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	// Minimize the white screend display -- sidumili
	vdCTOS_DispStatusMessage("PROCESSING...");
	
	memset(&strERMTransData,0x00,sizeof(strERMTransData));

	//vdCTOSS_SIGPField61();
	inCTOSS_FormERMReceipt(ERC_SHARE_HEADER_FILE);
	
	inERMAP_Database_BatchDelete();

	strERMTransData.shType = srTransRec.byTransType;
	//strcpy(strERMTransData.szTPDU,);
	strcpy(strERMTransData.szPAN,srTransRec.szPAN);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	strcpy(strERMTransData.szAmt,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
	strcpy(strERMTransData.szInvNum,szTemp);

	sprintf(strERMTransData.szSTAN, "%06ld", srTransRec.ulTraceNum);

    
	memset (szTemp, 0x00, sizeof(szTemp));
    if(srTransRec.byTransType == VOID)
    {
        vdGetTimeDate1(&srTransRec);
        wub_hex_2_str(srTransRec.szTempTime, szTemp,TIME_BCD_SIZE);
        strcpy(strERMTransData.szTime,szTemp);
    }
    else
    {
        wub_hex_2_str(srTransRec.szTime, szTemp,TIME_BCD_SIZE);
        strcpy(strERMTransData.szTime,szTemp);
    }
	
	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	strcpy(strERMTransData.szDate,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
	strcpy(strERMTransData.szExpDate,szTemp);

	strcpy(strERMTransData.szRefNum,srTransRec.szRRN);
	strcpy(strERMTransData.szApprCode,srTransRec.szAuthCode);

	strcpy(strERMTransData.szTID,srTransRec.szTID);
	strcpy(strERMTransData.szMID,srTransRec.szMID);
	
	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szBatchNo, szTemp,BATCH_NO_BCD_SIZE);
	strcpy(strERMTransData.szBatchNum,szTemp);

	szGetTransTitle(srTransRec.byTransType, strERMTransData.szPaymentType);
	vdDebug_LogPrintf("byTransType=[%d]---szPaymentType[%s]--",srTransRec.byTransType,strERMTransData.szPaymentType);
	strcpy(strERMTransData.szPaymentMedia,srTransRec.szCardLable);
	strcpy(strERMTransData.szReserved3,srTransRec.szHostLabel);	
	
//#define STATUS_SIGNATURE_OK			0
//#define STATUS_SIGPAD_FAILED			1
//#define STATUS_NO_SIGN_REQU			2
//#define	STATUS_CUST_SKIP				3
	//strcpy(strERMTransData.szSingatureStatus,"02");
	sprintf(strERMTransData.szSingatureStatus,"%02d",ginSingatureStatus);

	//getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	//strcpy(exe_subdir, &exe_dir[9]);
	//inExeAPIndex = atoi(exe_subdir);
	inExeAPIndex = srTransRec.HDTid;
	sprintf(strERMTransData.szReceiptLogoIndex, "%02d", inExeAPIndex);
	vdDebug_LogPrintf("inExeAPIndex=[%d]-----",srTransRec.HDTid);

	if (inSettle == 1)
	{
		strERMTransData.fSettle = 1;
		strcpy(strERMTransData.szPaymentMedia,"SIGP");
	}
	else
	{
		strERMTransData.fSettle = 0;
		if(strTCT.byERMMode != 0)
		{
			if (strlen(g_szPhoneNo) > 0)
				strcpy(strERMTransData.szBankCode,g_szPhoneNo);
			if (strlen(g_szEmail) > 0)
				strcpy(strERMTransData.szReserved2,g_szEmail);
			vdDebug_LogPrintf("g_szPhoneNoe=[%s]-g_szEmail=[%s]----",g_szPhoneNo,g_szEmail);
		}
		else
		{
		sprintf(strERMTransData.szReserved1, "ACC%02d%02d.total"
								, strHDT.inHostIndex
								, srTransRec.MITid);
		vdDebug_LogPrintf("local filename=[%s]-----",strERMTransData.szReserved1);

		sprintf(strERMTransData.szReserved2, "%d", srTransRec.byOrgTransType);
		}
		memcpy(strERMTransData.szReceiptImageFileName,srTransRec.szOrgAmount,AMT_BCD_SIZE);
		vdPCIDebug_HexPrintf("szOrgAmount",srTransRec.szOrgAmount,AMT_BCD_SIZE);
	}

	// Debug
	vdDebug_LogPrintf("-------------------------------------------------------------------");
	vdDebug_LogPrintf("srTransRec.HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("srTransRec.MITid[%d]", srTransRec.MITid);
	vdDebug_LogPrintf("srTransRec.szHostLabel[%s]", srTransRec.szHostLabel);
	vdDebug_LogPrintf("szPAN[%s]", strERMTransData.szPAN);
	vdDebug_LogPrintf("szAmt[%s]", strERMTransData.szAmt);
	vdDebug_LogPrintf("szInvNum[%s]", strERMTransData.szInvNum);
	vdDebug_LogPrintf("szSTAN[%s]", strERMTransData.szSTAN);
	vdDebug_LogPrintf("szTime[%s]", strERMTransData.szTime);
	vdDebug_LogPrintf("szDate[%s]", strERMTransData.szDate);
	vdDebug_LogPrintf("szRefNum[%s]", strERMTransData.szRefNum);
	vdDebug_LogPrintf("szApprCode[%s]", strERMTransData.szApprCode);
	vdDebug_LogPrintf("szTID[%s]", strERMTransData.szTID);
	vdDebug_LogPrintf("szMID[%s]", strERMTransData.szMID);
	vdDebug_LogPrintf("szBatchNum[%s]", strERMTransData.szBatchNum);
	vdDebug_LogPrintf("szPaymentMedia[%s]", strERMTransData.szPaymentMedia);
	vdDebug_LogPrintf("szReserved3[%s]", strERMTransData.szReserved3);
	vdDebug_LogPrintf("-------------------------------------------------------------------");
	
	inERMAP_Database_BatchInsert(&strERMTransData);	

	// Minimize the white screend display -- sidumili
	vdCTOS_DispStatusMessage("PROCESSING...");
	
	usCTOSS_Erm_FromReceipt();

	vdSetLastMerchant(); // Set Last Merchant -- sidumili

}

	

int inCTOS_ERMAllHosts_Initialization(BOOL fAllHost)
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

	vdDebug_LogPrintf("--inCTOS_ERMAllHosts_Initialization--");
	vdDebug_LogPrintf("fAllHost[%d]", fAllHost);
	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	inNumOfHost = inHDTNumRecord();
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			//vdDisplayProcessing();
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

			inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
			vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
			vdDebug_LogPrintf("MMT inNum=[%d]-----",inNum);
			for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
			{
                //vdDisplayProcessing();
                vdHostToInitialize(FALSE, FALSE);

				setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
				
				memcpy(&strMMT[0],&strMMT[inMitNum],sizeof(STRUCT_MMT));

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

	
int inCTOSS_ERM_Initialization(void)
{
	int result;
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	long length;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	BYTE szParaName[100];
	BYTE tmpbuf[100];
	BYTE szMID[MERCHANT_ID_BYTES + 1];

	vdDebug_LogPrintf("--inCTOSS_ERM_Initialization--");
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	//getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	//strcpy(exe_subdir, &exe_dir[9]);
	//inExeAPIndex = atoi(exe_subdir);
	inExeAPIndex = strHDT.inHostIndex;

	memset(szParaName,0x00,sizeof(szParaName));
	//sprintf(szParaName,"%s%02d","ERM",inExeAPIndex);
	sprintf(szParaName,"%s%02d%02d","ERM",inExeAPIndex,strMMT[0].MITid);
	vdDebug_LogPrintf("szParaName =[%s]",szParaName);
	inCTOSS_GetEnvDB(szParaName,tmpbuf);
	vdDebug_LogPrintf("[%s] =[%s]",szParaName,tmpbuf);
	if (strlen(tmpbuf) > 0 && atoi(tmpbuf) == 1)
		return ST_SUCCESS;
	
	memset(&strERMTransData,0x00,sizeof(strERMTransData));	

	result = inCTOSS_FromHeader_Footer();
	
	strcpy(strERMTransData.szTID,strMMT[0].szTID);
	strcpy(strERMTransData.szMID,strMMT[0].szMID);

	// pack 8 digits of TID into ISO field 42 -- sidumili
	if (strHDT.inHostIndex == 12) // BANCNET
	{
		if (get_env_int("ERMPACKDEBITMID") > 0)
		{		
			memset(strERMTransData.szMID, 0x00, sizeof(strERMTransData.szMID));
			memset(szMID, 0x00, sizeof(szMID));
			memcpy(szMID, &strMMT[0].szMID[0], 7);
			strcat(szMID, strMMT[0].szTID);
			strcpy(strERMTransData.szMID, szMID);
		}
	}
	
	vdDebug_LogPrintf("inHostIndex[%d] | szHostLabel[%s]", strHDT.inHostIndex, strHDT.szHostLabel);
	vdDebug_LogPrintf("strERMTransData.szTID[%s]", strERMTransData.szTID);
	vdDebug_LogPrintf("strERMTransData.szMID[%s]", strERMTransData.szMID);
	
	strcpy(strERMTransData.szLogoFileName,"logo.bmp");
	sprintf(strERMTransData.szReceiptLogoIndex, "%02d", inExeAPIndex);
	vdDebug_LogPrintf("inExeAPIndex=[%d]-----",inExeAPIndex);

#if 1
//receipt logo will upload in whole receipt
	length = lnGetFileSize(ERM_RECEIPT_FILE);
	if (length > 0)
		inDeleteFile(ERM_RECEIPT_FILE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "./fs_data/%s", strHDT.szHeaderLogoName);
	vdDebug_LogPrintf("new LOCAL_RECEIPT_FILE =[%s]",szTemp);
	inCopyFile(szTemp, ERM_RECEIPT_FILE);
	//inCopyFile(LOCAL_RECEIPT_FILE,ERM_RECEIPT_FILE);
	lnGetFileSize(ERM_RECEIPT_FILE);
#endif	
	inERMAP_Database_BatchDelete();

	inERMAP_Database_BatchInsert(&strERMTransData);

	result = usCTOSS_Erm_Initialization();

	vdDebug_LogPrintf("usCTOSS_Erm_Initialization :: result[%d]", result);
	
	memset(szParaName,0x00,sizeof(szParaName));
	//sprintf(szParaName,"%s%02d","ERM",inExeAPIndex);
	sprintf(szParaName,"%s%02d%02d","ERM",inExeAPIndex,strMMT[0].MITid);
	vdDebug_LogPrintf("szParaName =[%s]",szParaName);
	if (result == d_OK)
	{
		inCTOSS_PutEnvDB(szParaName,"1");
		return d_OK;
	}
	else
	{
		inCTOSS_PutEnvDB(szParaName,"0");
		return d_NO;
	}
	
}


USHORT usCTOSS_Erm_Initialization(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	vdDebug_LogPrintf("--usCTOSS_Erm_Initialization--");
	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_INITIALIZATION, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_EMV_MOL_Wget usOutLen[%d] bOutBuf[%s]",usOutLen, bOutBuf);

	vdDebug_LogPrintf("usCTOSS_Erm_Initialization |usOutLen[%d]|bOutBuf[%02x%02x%02x%02x%02x%02x]|usResult[%d]", usOutLen, bOutBuf[0], bOutBuf[1], bOutBuf[2], bOutBuf[3], bOutBuf[4], bOutBuf[5], usResult);
	
	if (bOutBuf[1] == d_SUCCESS)
		usResult = d_OK;
	else
		usResult = d_NO;

	vdDebug_LogPrintf("usCTOSS_Erm_Initialization :: usResult[%d]", usResult);
		
    return usResult;
}

USHORT usCTOSS_Erm_FromReceipt(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_FORM_RECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_EMV_MOL_Wget usOutLen[%d] bOutBuf[%s]",usOutLen, bOutBuf);
    return usResult;
}

USHORT usCTOSS_Erm_UploadReceipt(BOOL fSettle)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	int inNotificationType = 0; 	
	int iFSR = get_env_int("FSRMODE");
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;	

	vdDebug_LogPrintf("--usCTOSS_Erm_UploadReceipt--");
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("iFSR[%d]", iFSR);

	put_env_int("FAILEDERMTRXN", 0);
		
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_UPLOAD_RECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt |usOutLen[%d]|bInBuf[%02x%02x%02x%02x%02x%02x]|usResult[%d]", usOutLen, bOutBuf[0], bOutBuf[1], bOutBuf[2], bOutBuf[3], bOutBuf[4], bOutBuf[5], usResult);
	
	if (bOutBuf[1] == d_SUCCESS)
		usResult = d_OK;
	else
		usResult = d_NO;

	inNotificationType = get_env_int("ERMNOTIFICATIONTYPE");
	
	if (usResult == d_OK)
	{
		if (!fSettle)
		{
			switch (inNotificationType)
			{
				case ERM_SMS_TYPE:
					vdDisplayMultiLineMsgAlign("", "SMS SENT", "COMPLETE", DISPLAY_POSITION_CENTER);
					break;
				case ERM_EMAIL_TYPE:
					vdDisplayMultiLineMsgAlign("", "EMAIL SENT", "COMPLETE", DISPLAY_POSITION_CENTER);
					break;
				default:
					vdDisplayMultiLineMsgAlign("", "UPLOAD ERM RECEIPT", "COMPLETE", DISPLAY_POSITION_CENTER);
					break;
			}
		}
		else
		{
			vdDisplayMultiLineMsgAlign("", "UPLOAD ERM RECEIPT", "COMPLETE", DISPLAY_POSITION_CENTER);
		}
		
	}		
	else
	{
		if (!fSettle)
		{
			switch (inNotificationType)
			{
				case ERM_SMS_TYPE:
					vdDisplayMultiLineMsgAlign("", "SMS SENDING", "FAILED", DISPLAY_POSITION_CENTER);
					break;
				case ERM_EMAIL_TYPE:
					vdDisplayMultiLineMsgAlign("", "EMAIL SENDING", "FAILED", DISPLAY_POSITION_CENTER);
					break;
				default:
					vdDisplayMultiLineMsgAlign("", "UPLOAD ERM RECEIPT", "FAILED", DISPLAY_POSITION_CENTER);
					break;
			}

			put_env_int("FAILEDERMTRXN", 1);
		}
		else
		{
			vdDisplayMultiLineMsgAlign("", "UPLOAD ERM RECEIPT", "FAILED", DISPLAY_POSITION_CENTER);

			put_env_int("FAILEDERMTRXN", 2);
		}
	}

	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt | usResult[%d]", usResult);

	if (usResult == d_OK && (iFSR > 0 && iFSR != 3) && !fSettle)
	{
		vdSetFSRMode(0);
	}
	
    return usResult;
}

UINT usCTOSS_Erm_GetReceiptTotal(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	vdDebug_LogPrintf("--usCTOSS_Erm_GetReceiptTotal--");
	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_GET_RECEIPTTOT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_GetReceiptTotal |usOutLen[%d]|bOutBuf[%02x%02x%02x%02x%02x%02x]|usResult[%d]", usOutLen, bOutBuf[0], bOutBuf[1], bOutBuf[2], bOutBuf[3], bOutBuf[4], bOutBuf[5], usResult);
	
	if(d_OK == usResult)
	{
		if(bOutBuf[0] == d_IPC_CMD_ERM_GET_RECEIPTTOT)
			//inTotal = bOutBuf[1];
			inTotal = (bOutBuf[1] | bOutBuf[2] << 8);

		vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt inTotal[%d] ",inTotal);
	}
	
    return inTotal;
}


int usCTOSS_Erm_GetERMInfo(char* pszACCUMFileName,TRANS_TOTAL_ERM *srErmRec)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[2048];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;
	

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	strcpy(bInBuf,pszACCUMFileName);
	usInLen = strlen(bInBuf);
	vdDebug_LogPrintf("usInLen = [%d] bInBuf[%s]",usInLen, bInBuf);

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_GET_ERMINFO, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%x,%d]",usOutLen, bOutBuf[0],bOutBuf[1]);
	if(d_OK == usResult)
	{
		if(bOutBuf[0] == d_IPC_CMD_ERM_GET_ERMINFO)
		{
			memcpy(srErmRec,&bOutBuf[1],sizeof(TRANS_TOTAL_ERM));

			vdDebug_LogPrintf("stUploadTotal SaleCount[%d] [%12.0f]", srErmRec->stUploadTotal.usSaleCount, srErmRec->stUploadTotal.ulSaleTotalAmount);
		    vdDebug_LogPrintf("stUploadTotal OfflCount[%d] [%12.0f]", srErmRec->stUploadTotal.usOffSaleCount, srErmRec->stUploadTotal.ulOffSaleTotalAmount);
		    vdDebug_LogPrintf("stUploadTotal RefdCount[%d] [%12.0f]", srErmRec->stUploadTotal.usRefundCount, srErmRec->stUploadTotal.ulRefundTotalAmount);            
		    vdDebug_LogPrintf("stUploadTotal VoidCount[%d] [%12.0f]", srErmRec->stUploadTotal.usVoidSaleCount, srErmRec->stUploadTotal.ulVoidSaleTotalAmount);    
		    vdDebug_LogPrintf("stUploadTotal TipCount [%d] [%12.0f]", srErmRec->stUploadTotal.usTipCount, srErmRec->stUploadTotal.ulTipTotalAmount);
		}
	}
	
    return ST_SUCCESS;
}



UINT usCTOSS_Erm_DeleteReceipt(int DelTotal)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	sprintf(bInBuf,"%03d",DelTotal);
	usInLen = 3;

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_DELETE_RECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%x,%d]",usOutLen, bOutBuf[0],bOutBuf[1]);
	
    return usResult;
}

int inCTOSS_ERM_CheckSlipImage(void)
{
	int inTotal = 0;
	int loop = 0;
	int inRet;

	if(strTCT.byERMMode == 0)
		return d_OK;
	
    vdDebug_LogPrintf("--inCTOSS_ERM_CheckSlipImage--");
	
	// Minimize the white screend display -- sidumili
	CTOS_LCDTClearDisplay();
	vdCTOS_DispStatusMessage("PROCESSING...");
	
	inTotal = usCTOSS_Erm_GetReceiptTotal();
	
	vdDebug_LogPrintf("usCTOSS_Erm_GetReceiptTotal inTotal[%d],inERMMaximum=[%d]",inTotal,strTCT.inERMMaximum);

	if (inTotal >= strTCT.inERMMaximum)
	{		
		// Minimize the white screend display -- sidumili
		vdCTOS_DispStatusMessage("PROCESSING...");
		
		if (inCTOS_ERMPreConnect(TRUE) != d_OK)
		return d_NO;
		
		inRet = inCTOSS_HOST_ReERMInit();
		vdDebug_LogPrintf("inCTOSS_HOST_ReERMInit | inRet[%d]", inRet);
		if (inRet != d_OK)
		return inRet;
	
		CTOS_LCDTClearDisplay();
		vdDisplayMultiLineMsgAlign("PLEASE WAIT", "UPLOAD PENDING", "ERM RECEIPT", DISPLAY_POSITION_CENTER);
	
		inRet = inCTOSS_REUploadReceipt();
		if (inRet != d_OK)
			return inRet;
		else
		{
			// Signature file by Invoice
			vdLinuxCommandFileDelete();
		}
	}

	return d_OK;
}


int inCTOSS_ERM_CheckInit(void)
{
	BYTE tmpbuf[100];
	BYTE szParaName[100];
	int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	int inNumOfMit = 0,inMitNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	int inAPPID;

	vdDebug_LogPrintf("inCTOSS_ERM_CheckInit ............");
	
	inNumOfHost = inHDTNumRecord();
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	if (inNumOfHost >= 1)
	{
		for(inNum =1 ;inNum <= inNumOfHost; inNum++)
		{
			if(inHDTRead(inNum) == d_OK)
			{
				//vdDisplayProcessing();

				setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
				
				vdDebug_LogPrintf("szHostLabel-[%s]-inHostIndex=[%d]---",strHDT.szHostLabel,strHDT.inHostIndex);
                if (strcmp(strHDT.szHostLabel, "V5S_MCCNH")!=0)
                {
                    continue;
                }
							
				if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
				{
					continue;
				}

				inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
				vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
				for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
				{
					//vdDisplayProcessing();

					setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
					
					memcpy(&strMMT[0],&strMMT[inMitNum],sizeof(STRUCT_MMT));

					vdDebug_LogPrintf("HostName[%s]..MerchantName[%s]..szTID[%s]..szMID=[%s]..", strMMT[0].szHostName, strMMT[0].szMerchantName, strMMT[0].szTID,strMMT[0].szMID);
					
					memset(szParaName,0x00,sizeof(szParaName));
					sprintf(szParaName,"%s%02d%02d","ERM",strHDT.inHostIndex,strMMT[0].MITid);
					vdDebug_LogPrintf("szParaName =[%s]",szParaName);
					
					inCTOSS_GetEnvDB(szParaName,tmpbuf);
					vdDebug_LogPrintf("[%s] =[%s]",szParaName,tmpbuf);
					if (strlen(tmpbuf) > 0 && atoi(tmpbuf) == 1)
						continue;
					else
						return d_NO;
				}
			}
		}
	}
	else
		return d_NO;
	
	vdDebug_LogPrintf("inCTOSS_ERM_CheckInit-----");
	return d_OK;
	

}



long lnGetFileSize(const char* pchFileName)
{
    FILE  *fPubKey;
	long curpos,length;

	vdDebug_LogPrintf("lnGetFileSize[%s]", pchFileName);
	fPubKey = fopen( (char*)pchFileName, "rb" );
	vdDebug_LogPrintf("fPubKey[%x]", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fclose(fPubKey);
	vdDebug_LogPrintf("lnGetFileSize[%d],length=[%d]", curpos,length);

    return(length);
}


int inWriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	
	vdDebug_LogPrintf("inWriteFile[%s]", pchFileName);
	if((inRetVal = lnGetFileSize(pchFileName)) >= 0)
		remove(pchFileName);
	
	fPubKey = fopen((char *)pchFileName, "wb+" );
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	inRetVal = fwrite(pchRecBuf,inMaxRecSize, 1, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}

int inCopyFile(unsigned char *pchFileName1, unsigned char *pchFileName2)
{
	int h_file;
	int inRetVal = 0,curpos;
	FILE  *fPubKey1,*fPubKey2;
	unsigned char uszBufferData [(SIGP_SEND_SIZE) + 1];
	int infileSize = 0;
	
	vdDebug_LogPrintf("inCopyFile[%s][%s]", pchFileName1,pchFileName2);
	
	fPubKey1 = fopen( (char*)pchFileName1, "rb" );
	vdDebug_LogPrintf("fPubKey1[%x]....", fPubKey1);
	if(fPubKey1 == NULL)
		return SM_RET_PARAM;

	curpos=ftell(fPubKey1);
	fseek(fPubKey1,0L,SEEK_END);
	infileSize=ftell(fPubKey1);
	fseek(fPubKey1,curpos,SEEK_SET);
	fread( (char *)uszBufferData, 1, infileSize, fPubKey1 );
	fclose(fPubKey1);
	
	
	fPubKey2 = fopen((char *)pchFileName2, "wb+" );
	vdDebug_LogPrintf("fPubKey2[%x]....", fPubKey1);
	if(fPubKey2 == NULL)
		return SM_RET_PARAM;

	inRetVal = fwrite(uszBufferData,infileSize, 1, fPubKey2);
	fclose(fPubKey2);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,infileSize);
	vdDebug_LogPrintf("uszBufferData[%s]....", uszBufferData);

	return inRetVal;
}

int inDeleteGzipFile(unsigned char *pchFileName)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	unsigned char uszFileName[20 + 1];
	memset(uszFileName, 0x00, sizeof(uszFileName));
	strcpy(uszFileName, pchFileName);
	strcat(uszFileName, ".gz");
	
	vdDebug_LogPrintf("inDeleteGzipFile[%s]", uszFileName);
	if((inRetVal = lnGetFileSize(uszFileName)) >= 0)
		remove(uszFileName);
	
	fPubKey = fopen((char *)uszFileName, "wb+" );
	vdDebug_LogPrintf("inDeleteGzipFile[%x]", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	fclose(fPubKey);

	return ST_SUCCESS;
}


int inDeleteFile(unsigned char *pchFileName)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	unsigned char uszFileName[20 + 1];
	memset(uszFileName, 0x00, sizeof(uszFileName));
	strcpy(uszFileName, pchFileName);
	
	vdDebug_LogPrintf("inDeleteGzipFile[%s]", uszFileName);
	if((inRetVal = lnGetFileSize(uszFileName)) >= 0)
		remove(uszFileName);
	
	fPubKey = fopen((char *)uszFileName, "wb+" );
	vdDebug_LogPrintf("inDeleteGzipFile[%x]", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	fclose(fPubKey);

	return ST_SUCCESS;
}


int inReadGzipFile(unsigned char *pchFileName, unsigned char* uszBufferData, int *inBufferSize)
{
	int h_file;
	int inRetVal = 0, inFileSize = 0;
	unsigned char uszFileName [20 + 1];
	FILE  *fPubKey;
	long curpos,length;
	

	memset(uszFileName, 0x00, sizeof(uszFileName));
	strcpy(uszFileName, pchFileName);
	strcat(uszFileName, ".gz");

	vdDebug_LogPrintf("inReadGzipFile[%s]", uszFileName);

	fPubKey = fopen( (char*)uszFileName, "rb" );
	vdDebug_LogPrintf("fPubKey[%x]....", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fread( (char *)uszBufferData, 1, length, fPubKey );
	fclose(fPubKey);

	*inBufferSize = length;
	vdDebug_LogPrintf("inReadGzipFile=[%d][%d]", length,*inBufferSize);

	return ST_SUCCESS;
}

int inCopyBMP2Buffer(unsigned char* uszFileName, unsigned char* uszBufferData, int *inBufferSize)
{
	int h_HandleLogoFile;
	int inFileSize = 0;
	FILE  *fPubKey;

	vdDebug_LogPrintf("inCopyBMP2Buffer[%s]", uszFileName);
	if((inFileSize = lnGetFileSize(uszFileName)) < 0)
    {
    	return ST_SUCCESS;
    }

	fPubKey = fopen( (char*)uszFileName, "rb" );
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	*inBufferSize = fread( (char *)uszBufferData, 1, inFileSize, fPubKey );
	fclose(fPubKey);

	vdDebug_LogPrintf("inCopyBMP2Buffer=[%d][%d]", inFileSize,*inBufferSize);
	
	return(d_OK);		
}


int inCTOSS_ERMInit(void)
{
	int inResult = d_NO;

	vdDebug_LogPrintf("inCTOSS_ERMInit.........");

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;
	
		
	vdDebug_LogPrintf("inCTOSS_ERMInit..byERMInit=[%d]",strTCT.byERMInit);
	if(strTCT.byERMInit == 1)
		return ST_SUCCESS;

    vdDispTitleString("ERM INIT");

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_ERMAllHosts_Initialization(FALSE);	

		if (inResult == d_OK)
		{
			inResult = inCTOSS_ERM_CheckInit();		
			
			if (inResult == d_OK)
			{
				strTCT.byERMInit = 1;
				inTCTSave(1);
				inTCTRead(1);
			}
		}					
	}
	else
	{
		inResult = inCTOS_ERMAllHosts_Initialization(TRUE);
	}

	vdDebug_LogPrintf("inCTOSS_ERM_CheckInit =[%d]",inResult);
	
	if (inResult == d_OK)	
		vdDisplayMultiLineMsgAlign("", "INITIALIZE ERM", "COMPLETE", DISPLAY_POSITION_CENTER);	
	else	
		vdDisplayMultiLineMsgAlign("", "INITIALIZE ERM", "FAILED", DISPLAY_POSITION_CENTER);	
	

	
	return inResult;
}


int inCTOSS_UploadReceipt(BOOL fSettle, BOOL fPending)
{
	BYTE szlastHost[5];
	BYTE szLastMerch[5];
	int inRet;
	int inTotal = 0;	

	vdDebug_LogPrintf("--inCTOSS_UploadReceipt--");
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;	

	if (inCTOS_ERMPreConnect(TRUE) != d_OK)
		return d_NO;
	
	inTotal = usCTOSS_Erm_GetReceiptTotal();
	vdDebug_LogPrintf("inTotal[%d]", inTotal);
	if (inTotal > 0)
	{
		CTOS_LCDTClearDisplay();
		vdDisplayMultiLineMsgAlign("PLEASE WAIT", "UPLOADING ERM", "RECEIPT", DISPLAY_POSITION_CENTER);
	}
	
	if (!fPending)
	{				
		inRet = inCTOSS_HOST_ERMInit();	
		vdDebug_LogPrintf("inCTOSS_HOST_ERMInit | inRet[%d]", inRet);
		if (inRet != d_OK)
			return inRet;
	}

	if (inTotal > 0)
	{
		CTOS_LCDTClearDisplay();
		vdCTOS_DispStatusMessage("PROCESSING...");
		inRet = usCTOSS_Erm_UploadReceipt(fSettle);
		vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt | inRet[%d]", inRet);

		// Restore and ReUpload ERM Receipt  -- sidumili
		if (inRet != d_OK)
		{			
			inTotal = usCTOSS_Erm_GetBackupReceiptTotal();
			if (inTotal > 0)		
			{
				CTOS_LCDTClearDisplay();
				vdDisplayMultiLineMsgAlign("PLEASE WAIT", "RE-UPLOAD", "ERM RECEIPT", DISPLAY_POSITION_CENTER);		
				usCTOSS_Erm_RestoreBackupReceipt();
				inRet = usCTOSS_Erm_UploadReceipt(fSettle);
			}
			vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt | inRet[%d]", inRet);
		}
	}
	
	// For ERM
	if (inRet == d_OK && fSettle)
	{
		vdLinuxCommandClearERMBitmap();
	}
	return inRet;
}


/*albert - ERM, copy original to new, delete original*/
void vdLinuxCommandFileCopy(void)
{
    char szSystemCmdPath[128+1];    
    char szNewFileName[24+1];

	memset(szNewFileName, 0x00, sizeof(szNewFileName)); 
	vdGetSignatureFilename(&szNewFileName);

	memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
	sprintf(szSystemCmdPath, "cp %s %s", DB_SIGN_BMP, szNewFileName);
	system(szSystemCmdPath);

    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
    sprintf(szSystemCmdPath, "rm %s", DB_SIGN_BMP);
    system(szSystemCmdPath);
}

/*albert - ERM, delete ERM bitmap files*/
void vdLinuxCommandClearERMBitmap(void)
{
    char szSystemCmdPath[128+1];

	vdDebug_LogPrintf("--vdLinuxCommandClearERMBitmap--");
	
	memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
	strcpy(szSystemCmdPath, "rm signERM*.bmp");
	system(szSystemCmdPath);
}

void vdLinuxCommandFileDelete(void)
{
    char szSystemCmdPath[128+1];    
    char szNewFileName[24+1];

	vdDebug_LogPrintf("--vdLinuxCommandFileDelete--");
	
	memset(szNewFileName, 0x00, sizeof(szNewFileName)); 
	vdGetSignatureFilename(&szNewFileName);

    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
    sprintf(szSystemCmdPath, "rm %s", DB_SIGN_BMP);
    system(szSystemCmdPath);

	vdDebug_LogPrintf("szSystemCmdPath[%s]", szSystemCmdPath);
}	

USHORT usCTOSS_Erm_RestoreBackupReceipt(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	vdDebug_LogPrintf("--usCTOSS_Erm_RestoreBackupReceipt--");
	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_RESTORE_BACKUPRECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);   

	vdDebug_LogPrintf("usCTOSS_Erm_RestoreBackupReceipt |usOutLen[%d]|bOutBuf[%02x%02x%02x%02x%02x%02x]|usResult[%d]", usOutLen, bOutBuf[0], bOutBuf[1], bOutBuf[2], bOutBuf[3], bOutBuf[4], bOutBuf[5], usResult);
	
	if (bOutBuf[1] == d_SUCCESS)
		usResult = d_OK;
	else
		usResult = d_NO;

	vdDebug_LogPrintf("usCTOSS_Erm_RestoreBackupReceipt :: usResult[%d]", usResult);
	
    return usResult;
}



long inCTOSS_ERM_GetFileSize(const char* pchFileName)
{
    FILE  *fPubKey;
	long curpos,length;

	vdDebug_LogPrintf("lnGetFileSize[%s]", pchFileName);
	fPubKey = fopen( (char*)pchFileName, "rb" );
	if(fPubKey == NULL)
		return -1;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fclose(fPubKey);
	vdDebug_LogPrintf("lnGetFileSize[%d],length=[%d]", curpos,length);

    return(length);
}


int inCTOSS_ERM_WriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	vdDebug_LogPrintf("inWriteFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "wb+" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_BUF)
	{
		times = inMaxRecSize/MAX_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_BUF],MAX_BUF, 1, fPubKey);
		}

		times = inMaxRecSize%MAX_BUF;
		if(times>0)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_BUF],times, 1, fPubKey);
		}
		
	}
	else
	inRetVal = fwrite(pchRecBuf,inMaxRecSize, 1, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}

int inCTOSS_ERM_ReadFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	vdDebug_LogPrintf("ReadFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "rb" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_BUF)
	{
		times = inMaxRecSize/MAX_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_BUF], 1, MAX_BUF, fPubKey);
		}

		times = inMaxRecSize%MAX_BUF;
		if(times>0)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_BUF], 1, times, fPubKey);
		}
		
	}
	else
	inRetVal = fread (pchRecBuf, 1, inMaxRecSize, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("ReadFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}


int inMultiAP_Database_ERM_ClearEx(void)
{
	int inRetVal = 0;

	memset(szERMSendData,0x00,sizeof(szERMSendData));
	memset(szERMReceiveData,0x00,sizeof(szERMReceiveData));

	inRetVal = inCTOSS_ERM_GetFileSize(ERM_BUFF_FILE);
	vdDebug_LogPrintf("[%s] =[%d] ", ERM_BUFF_FILE,inRetVal);
	if (inRetVal >= 0)
		remove(ERM_BUFF_FILE);

	return d_OK;
}

int inMultiAP_Database_ERM_SaveEx(void)
{
	int inRetVal = 0;

	vdDebug_LogPrintf("inMultiAP_Database_ERM_SaveEx inSendLen=[%d] ", inSendLen);
	if (inSendLen > 0)
	{
		inRetVal = inCTOSS_ERM_WriteFile(ERM_BUFF_FILE,szERMSendData,inSendLen);
		if (inRetVal <= 0)
		return d_NO;
	}

	return d_OK;
}

int inMultiAP_Database_ERM_ReadEx(void)
{
	int inRetVal = 0;

	inReceiveLen = inCTOSS_ERM_GetFileSize(ERM_BUFF_FILE);
	vdDebug_LogPrintf("inMultiAP_Database_ERM_ReadEx inReceiveLen=[%d] ", inReceiveLen);
	if (inReceiveLen > 0)
	{
		memset(szERMReceiveData,0x00,sizeof(szERMReceiveData));
		inRetVal = inCTOSS_ERM_ReadFile(ERM_BUFF_FILE,szERMReceiveData,inReceiveLen);
		if (inRetVal <= 0)
			return d_NO;
	}

	return d_OK;
}



int inCTOSS_ERM_SaveERMInfo(void)
{
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];

	memset(&strERMTransData,0x00,sizeof(strERMTransData));
	
	inERMAP_Database_BatchDelete();

	sprintf(strERMTransData.szSTAN, "%06ld", srTransRec.ulTraceNum);
	//testing
	//sprintf(strERMTransData.szSTAN, "%06ld", 99);
	vdDebug_LogPrintf("ulTraceNum=[%06ld]", srTransRec.ulTraceNum);


    CTOS_RTCGet(&SetRTC);
    sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
    wub_str_2_hex(szCurrentTime,srTransRec.szDate,DATE_ASC_SIZE);
    sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
    wub_str_2_hex(szCurrentTime,srTransRec.szTime,TIME_ASC_SIZE);
		
	memset (szTemp, 0x00, sizeof(szTemp));
    wub_hex_2_str(srTransRec.szTime, szTemp,TIME_BCD_SIZE);
	strcpy(strERMTransData.szTime,szTemp);
	vdDebug_LogPrintf("szTime=[%s]", strERMTransData.szTime);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	strcpy(strERMTransData.szDate,szTemp);
	vdDebug_LogPrintf("szDate=[%s]", strERMTransData.szDate);


	strcpy(strERMTransData.szTID,srTransRec.szTID);
	strcpy(strERMTransData.szMID,srTransRec.szMID);
	//testing
	//strcpy(strERMTransData.szTID,"10000001");
	//strcpy(strERMTransData.szMID,"100000000000001");
	vdDebug_LogPrintf("szTID=[%s],szMID=[%s]", strERMTransData.szTID,strERMTransData.szMID);

	strERMTransData.fReserved1 = 1;//Request Contact Info

	strcpy(strERMTransData.szPAN,srTransRec.szPAN);
	//testing
	//strcpy(strERMTransData.szPAN,"4119110002304324");
	
	inERMAP_Database_BatchInsert(&strERMTransData);
	
}




int inCTOSS_ERM_Send(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	//BYTE bySendTemp[2048];
	BYTE bySendTemp[8094];
	BYTE byTempLen[10];
	BYTE byTempHEXLen[10];
	int inHeader;
	
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

	//inMultiAP_Database_COM_Clear();
	inMultiAP_Database_ERM_ClearEx();

	inHeader = strCPT.inIPHeader;

	//inHeader = HEX_EXCLUDE_LEN;//for test
	vdDebug_LogPrintf("inCTOSS_ERM_Send[%ld]",ulSendLen);
	
	switch(inHeader)
	{
		case NO_HEADER_LEN:
			inSendLen = ulSendLen;
			vdDebug_LogPrintf("---***inCTOSS_COMM_Send[%ld]",ulSendLen);
			
			memcpy(bySendTemp, uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);
			break;
			
		case HEX_EXCLUDE_LEN:
			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);

			inSendLen = ulSendLen + 2;
			
			vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);

			break;

		case BCD_EXCLUDE_LEN:
			sprintf(byTempLen,"%04ld",ulSendLen);
			wub_str_2_hex(byTempLen,byTempHEXLen,4);
			memcpy(bySendTemp,byTempHEXLen,2);			 
			inSendLen = ulSendLen + 2;
			
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);
			break;

		case HEX_INCLUDE_LEN:
			ulSendLen += 2;

			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);

			inSendLen = ulSendLen + 2;
			vdDebug_LogPrintf("---HEX_INCLUDE_LEN[%ld]",ulSendLen);
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);   
			break;
			

		case BCD_INCLUDE_LEN:			 
			ulSendLen += 2;

			sprintf(byTempLen,"%04ld",ulSendLen);
			wub_str_2_hex(byTempLen,byTempHEXLen,4);
			memcpy(bySendTemp,byTempHEXLen,2);			 
			inSendLen = ulSendLen + 2;
			
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);
			
			break;

		default:
			
			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);

			inSendLen = ulSendLen + 2;
			
			vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);

			break;
	}
	
	//strCOM.inHeaderFormat = inHeader;
	
	//inMultiAP_Database_COM_Save();
	inMultiAP_Database_ERM_SaveEx();
	inCTOSS_ERM_SaveERMInfo();

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_FORWARD_TRANS_REQ, bInBuf, usInLen, bOutBuf, &usOutLen);
	
	vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",usResult);
	if(d_OK == usResult)
	{
		//status
		if(bOutBuf[0] == IPC_COMM_SUCC) 		   
			return d_OK;
		else
			return d_NO;
	}
	else
		return d_NO;
	
	//return usResult;
}



int inCTOSS_ERM_Receive(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	int inHeader;

	memset(bOutBuf, 0x00, sizeof(bOutBuf));    
	inHeader = strCPT.inIPHeader;

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_FORWARD_TRANS_RESP, bInBuf, usInLen, bOutBuf, &usOutLen);
	
	vdDebug_LogPrintf("---inCTOSS_ERM_Receive.usResult[%d]bOutBuf[0]=[%d]",usResult,bOutBuf[0]);
	if(d_OK == usResult)
	{
		//status
		if(bOutBuf[0] == IPC_COMM_SUCC) 
		{	   
			//inMultiAP_Database_COM_Read();
			inMultiAP_Database_ERM_ReadEx();
			vdDebug_LogPrintf("---strCOM.inReceiveLen[%d],.inIPHeader=[%d]",inReceiveLen,strCPT.inIPHeader);
			vdDebug_LogPrintf("rec data==[%d][%s]",inReceiveLen,szERMReceiveData);
			if(inHeader == NO_HEADER_LEN)
			{
				usResult = inReceiveLen;
				memcpy(uszRecData,szERMReceiveData,inReceiveLen);

				//puts("Server reply :");
				//puts(uszRecData); 
			}
			else
			{
				usResult = inReceiveLen-2;
				memcpy(uszRecData,&szERMReceiveData[2],inReceiveLen-2);
			}
			//after receive clear database
			//inMultiAP_Database_COM_Clear();
			inMultiAP_Database_ERM_ClearEx();
			return usResult;
		}
		else
			return 0;
	}
	
	return usResult;
}


int inCTOSS_ERM_CardinfoENQ(void)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	//BYTE bySendTemp[2048];
	BYTE bySendTemp[8094];
	BYTE byTempLen[10];
	BYTE byTempHEXLen[10];
	int inHeader;
	
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

	inCTOSS_ERM_SaveERMInfo();

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_CARDINFO_ENQ, bInBuf, usInLen, bOutBuf, &usOutLen);
	
	vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",usResult);
	if(d_OK == usResult)
	{
		//status
		if(bOutBuf[0] == IPC_COMM_SUCC) 		   
			return d_OK;
		else
			return d_NO;
	}
	else
		return d_NO;
	
	//return usResult;
}



int inCTOSS_ERM_ReceiptRecvVia(void)
{
	int inRecvViaLen;
	BYTE szERMRecvViaData[1024];
	char ReqInfo;
	BYTE szPhoneNo[30];
	BYTE szEmail[100];
	BOOL fRequestPhone = FALSE;
	BOOL fRequestEmail = FALSE;
	int inNotificationType = 0;
	int inRet = d_NO;

	//fRequestPhone = (VS_BOOL)get_env_int("ERMREQNUM");
	//fRequestEmail = (VS_BOOL)get_env_int("ERMREQEMAIL");
	inNotificationType = get_env_int("ERMNOTIFICATIONTYPE");

	vdDebug_LogPrintf("--inCTOSS_ERM_ReceiptRecvVia--[start]");
	//vdDebug_LogPrintf("fRequestPhone[%d]", fRequestPhone);
	//vdDebug_LogPrintf("fRequestEmail[%d]", fRequestEmail);
	vdDebug_LogPrintf("inNotificationType[%d]", inNotificationType);
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
	
	memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
	memset(g_szEmail,0x00,sizeof(g_szEmail));

	if(strTCT.byERMMode == 0)
		return d_OK;

	//CTOS_LCDTClearDisplay();

	switch (inNotificationType)
	{
		case ERM_SMS_TYPE:
			memset(szPhoneNo, 0x00, sizeof(szPhoneNo));
			inCTOSS_GetEnvDB("ERMTESTNUM", szPhoneNo);
			inCTOSS_ERM_ChoicePhoneNo(szPhoneNo);
			vdDebug_LogPrintf("g_szPhoneNo[%s]", g_szPhoneNo);
			inRet = d_OK;
		break;

		case ERM_EMAIL_TYPE:
			memset(szEmail, 0x00, sizeof(szEmail));
			inCTOSS_GetEnvDB("ERMTESTEMAIL", szEmail);
			inCTOSS_ERM_ChoiceEmail(szEmail);
			vdDebug_LogPrintf("g_szEmail[%s]", g_szEmail);
			inRet = d_OK;
		break;

		default:
			inRet = d_OK;
		break;	
	}

	if (inRet == d_OK) return d_OK;

	// -----------------------------------------------------------------------------------
	inRecvViaLen = inCTOSS_ERM_GetFileSize(ERM_RECVVIS_FILE);
	vdDebug_LogPrintf("inCTOSS_ERM_ReceiptRecvVia inRecvViaLen=[%d] ", inRecvViaLen);
	if (inRecvViaLen <= 0)
		inCTOSS_ERM_CardinfoENQ();

	inRecvViaLen = inCTOSS_ERM_GetFileSize(ERM_RECVVIS_FILE);
	if (inRecvViaLen > 0)
	{
		memset(szERMRecvViaData,0x00,sizeof(szERMRecvViaData));
		memset(szPhoneNo,0x00,sizeof(szPhoneNo));
		memset(szEmail,0x00,sizeof(szEmail));
		inCTOSS_ERM_ReadFile(ERM_RECVVIS_FILE,szERMRecvViaData,inRecvViaLen);
		vdPCIDebug_HexPrintf("ERM_RECVVIS_FILE",szERMRecvViaData,inRecvViaLen);
		remove(ERM_RECVVIS_FILE);
		
		ReqInfo = szERMRecvViaData[0];
		vdDebug_LogPrintf("ReqInfo=[%c][%d]",ReqInfo,ReqInfo);

		memcpy(szPhoneNo,&szERMRecvViaData[1],15);
		vdDebug_LogPrintf("szPhoneNo=[%s]",szPhoneNo);
		if (strlen(szPhoneNo) > 0)
			strcpy(g_szPhoneNo,szPhoneNo);
		memcpy(szEmail,&szERMRecvViaData[16],50);
		vdDebug_LogPrintf("szEmail=[%s]",szEmail);
		if (strlen(szEmail) > 0)
			strcpy(g_szEmail,szEmail);

		if (ReqInfo == '0')
			return d_OK;

		//harcode to SMS only whatever is the response from card enq
		ReqInfo = '1';

		//else if (ReqInfo == '1')
		if (ReqInfo == '1')
		{
			inCTOSS_ERM_ChoicePhoneNo(szPhoneNo);
		}
		else if (ReqInfo == '2')
		{
			inCTOSS_ERM_ChoiceEmail(szEmail);
		}
		else if (ReqInfo == '3')
		{
			inCTOSS_ERM_ChoicePhoneNoAndEmail(szPhoneNo,szEmail);
		}

		CTOS_LCDTClearDisplay();
	}

	vdDebug_LogPrintf("g_szPhoneNo=[%s]",g_szPhoneNo);
	vdDebug_LogPrintf("g_szEmail=[%s]",g_szEmail);

	vdDebug_LogPrintf("--inCTOSS_ERM_ReceiptRecvVia--[end]");
	
	return d_OK;
}


int inCTOSS_ERM_ChoicePhoneNo(char *szPhoneNo)
{
	unsigned char key;
	BYTE sztmpPhoneNo[30];
	int usLen;
	int inRetVal;
	
	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("inCTOSS_ERM_ChoicePhoneNo=[%s]",szPhoneNo);
	if (strlen(szPhoneNo) > 0)
	{
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);
		
		if (isCheckTerminalMP200() == d_OK)
		{
			setLCDPrint27(3, d_LCD_ALIGNCENTER, "Receipt SMS to");
			setLCDPrint27(4, d_LCD_ALIGNCENTER, szPhoneNo);
			setLCDPrint27(8, d_LCD_ALIGNCENTER, "NO[X] YES[OK]");
		}
		else
		{
			CTOS_LCDTPrintAligned(3, "Receipt SMS to", d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(4, szPhoneNo, d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);
		}

		while (1)
		{
			CTOS_KBDGet(&key);
			if (key == d_KBD_ENTER)
			{
				inRetVal = inERM_Confirmation(ERM_SMS_TYPE, szPhoneNo);
				if (inRetVal == d_OK)
				{
					memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
					strcpy(g_szPhoneNo,szPhoneNo);
					break;
				}
				else
				{
					inRetVal = inCTOSS_ERM_EnterPhoneNo();
					if (inRetVal == d_OK) break;
				}
			}	
			else if (key == d_KBD_CANCEL)
			{
				inRetVal = inCTOSS_ERM_EnterPhoneNo();
				if (inRetVal == d_OK) break;
			}
		}
	}
	else
	{
		inCTOSS_ERM_EnterPhoneNo();
		return d_OK;
	}

	return d_OK;
}


int inCTOSS_ERM_ChoiceEmail(char *szEmail)
{
	unsigned char key;
	BYTE sztmpEmail[100];
	BYTE sztmpEmail2[100];
	int usLen;
	BOOL fEmail = FALSE;
	int inRetVal;
	
	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("inCTOSS_ERM_ChoiceEmail=[%s]",szEmail);
	if (strlen(szEmail) > 0)
	{
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);

		if (strlen(szEmail) > 20)
		{
			fEmail = TRUE;
			memset(sztmpEmail, 0x00, sizeof(sztmpEmail));
			memset(sztmpEmail2, 0x00, sizeof(sztmpEmail2));
			memcpy(sztmpEmail, szEmail, 20);
			memcpy(sztmpEmail2, &szEmail[20], strlen(szEmail) - 20);
		}
		
		if (isCheckTerminalMP200() == d_OK)
		{
			setLCDPrint27(3, d_LCD_ALIGNCENTER, "Email Receipt to");

			if (fEmail)
			{
				setLCDPrint27(4, d_LCD_ALIGNCENTER, sztmpEmail);
				setLCDPrint27(5, d_LCD_ALIGNCENTER, sztmpEmail2);
				setLCDPrint27(8, d_LCD_ALIGNCENTER, "NO[X] YES[OK]");
			}
			else
			{
				setLCDPrint27(4, d_LCD_ALIGNCENTER, szEmail);
				setLCDPrint27(8, d_LCD_ALIGNCENTER, "NO[X] YES[OK]");
			}
		}
		else
		{
			CTOS_LCDTPrintAligned(3, "Email Receipt to", d_LCD_ALIGNCENTER);

			if (fEmail)
			{
				CTOS_LCDTPrintAligned(4, sztmpEmail, d_LCD_ALIGNCENTER);
				CTOS_LCDTPrintAligned(5, sztmpEmail2, d_LCD_ALIGNCENTER);
				CTOS_LCDTPrintAligned(6, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);
			}
			else
			{
				CTOS_LCDTPrintAligned(4, szEmail, d_LCD_ALIGNCENTER);
				CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);
			}
		}
		
		while (1)
		{
			CTOS_KBDGet(&key);
			if (key == d_KBD_ENTER)
			{
				inRetVal = inERM_Confirmation(ERM_EMAIL_TYPE, szEmail);
				if (inRetVal == d_OK)
				{
					memset(g_szEmail,0x00,sizeof(g_szEmail));
					strcpy(g_szEmail,szEmail);
					break;
				}
				else
				{
					inRetVal = inCTOSS_ERM_EnterEmail();
					if (inRetVal == d_OK) break;
				}
			}
			else if (key == d_KBD_CANCEL)
			{
				inRetVal = inCTOSS_ERM_EnterEmail();
				if (inRetVal == d_OK) break;
			}
		}
	}
	else
	{
		inCTOSS_ERM_EnterEmail();
		return d_OK;
	}

	return d_OK;
}


int inCTOSS_ERM_ChoicePhoneNoAndEmail(char *szPhoneNo,char *szEmail)
{
	unsigned char key;
	BYTE sztmpPhoneNo[30];
	BYTE sztmpEmail[100];
	int usLen;
	
	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("inCTOSS_ERM_ChoicePhoneNoAndEmail=[%s][%s]",szPhoneNo, szEmail);
	if (strlen(szPhoneNo) > 0)
	{
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);

		if (isCheckTerminalMP200() == d_OK)
		{
			setLCDPrint27(3, d_LCD_ALIGNCENTER, "Receipt SMS to");
			setLCDPrint27(4, d_LCD_ALIGNCENTER, szPhoneNo);
			setLCDPrint27(5, d_LCD_ALIGNCENTER, "NO[X] YES[OK]");
		}
		else
		{
			CTOS_LCDTPrintAligned(3, "Receipt SMS to", d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(4, szPhoneNo, d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);
		}


		while (1)
		{
			CTOS_KBDGet(&key);
	        if(key==d_KBD_ENTER){
				strcpy(g_szPhoneNo,szPhoneNo);
	            return d_OK;
	        }
	        else if(key==d_KBD_CANCEL)
	        {
				inCTOSS_ERM_EnterPhoneNoAndEmail();
	        	return d_OK;
	        }
		}
	}
	else if (strlen(szEmail) > 0)
	{
		
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);

		if (isCheckTerminalMP200() == d_OK)
		{
			setLCDPrint27(3, d_LCD_ALIGNCENTER, "Email Receipt to");
			setLCDPrint27(4, d_LCD_ALIGNCENTER, szEmail);
			setLCDPrint27(5, d_LCD_ALIGNCENTER, "NO[X] YES[OK]");
		}
		else
		{
			CTOS_LCDTPrintAligned(3, "Email Receipt to", d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(4, szEmail, d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);
		}

		while (1)
		{
			CTOS_KBDGet(&key);
	        if(key==d_KBD_ENTER)
	            return d_OK;
	        else if(key==d_KBD_CANCEL)
	        {
				inCTOSS_ERM_EnterPhoneNoAndEmail();
	        	return d_OK;
	        }
		}
	}
	else
	{
		inCTOSS_ERM_EnterPhoneNoAndEmail();
	}

	return d_OK;
}


int inCTOSS_ERM_EnterPhoneNoAndEmail(void)
{
    unsigned char key;
    BYTE sztmpPhoneNo[30];
    BYTE sztmpEmail[100];
    int usLen;

    CTOS_KBDBufFlush();
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

	if (isCheckTerminalMP200() == d_OK)
	{
		setLCDPrint27(3, d_LCD_ALIGNLEFT, "Receipt receive via");
		setLCDPrint27(4, d_LCD_ALIGNLEFT, "     1) SMS");
		setLCDPrint27(5, d_LCD_ALIGNLEFT, "     2) Email");
	}
	else
	{
	    CTOS_LCDTPrintAligned(3, "Receipt receive via", d_LCD_ALIGNLEFT);
	    CTOS_LCDTPrintAligned(4, "     1) SMS", d_LCD_ALIGNLEFT);
	    CTOS_LCDTPrintAligned(5, "     2) Email", d_LCD_ALIGNLEFT);
	}
	
    while (1)
    {
        CTOS_KBDGet(&key);
        if(key==d_KBD_1)
        {
            inCTOSS_ERM_EnterPhoneNo();
            memset(g_szEmail, 0x00, sizeof(g_szEmail));
            return d_OK;
        }
        else if(key==d_KBD_2)
        {
            inCTOSS_ERM_EnterEmail();
            memset(g_szPhoneNo, 0x00, sizeof(g_szPhoneNo));
            return d_OK;
        }
    }
}


int inCTOSS_ERM_EnterPhoneNo(void)
{
	unsigned char key;
	BYTE sztmpPhoneNo[100];
	USHORT usLen;
	int inRetVal;

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	while(1)
	{
		CTOS_KBDBufFlush();
		vdDebug_LogPrintf("inCTOSS_ERM_EnterPhoneNo...");
		
		if (isCheckTerminalMP200() == d_OK)
			setLCDPrint27(3, d_LCD_ALIGNLEFT, "Enter Phone No.");
		else
			CTOS_LCDTPrintAligned(3, "Enter Phone No.", d_LCD_ALIGNLEFT);
		
		usLen = 50;
		memset(sztmpPhoneNo,0x00,sizeof(sztmpPhoneNo));
		//key = InputStringAlphaEx2(1, 4, 0x00, 0x02, sztmpEmail, &usLen, 1, d_INPUT_TIMEOUT);
		//key = InputStringAlphaEx2(1, 4, 0x05, 0x02, sztmpPhoneNo, &usLen, 0, d_INPUT_TIMEOUT);
		//key = InputStringAlphaEx2(1, 4, 0x00, 0x02, sztmpPhoneNo, &usLen, 0, d_INPUT_TIMEOUT);
		key = shCTOS_GetNum(4, 0x01,  sztmpPhoneNo, &usLen, 1, 11, 0, d_INPUT_TIMEOUT);
		vdDebug_LogPrintf("sztmpPhoneNo=[%s]",sztmpPhoneNo);
		memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
		strcpy(g_szPhoneNo,sztmpPhoneNo);

		//if ((key == d_KBD_ENTER) || (key == d_KBD_CANCEL))
		if ((key >= 1) || (key == d_KBD_CANCEL))
		{
			inRetVal = inERM_Confirmation(ERM_SMS_TYPE, sztmpPhoneNo);
			if (inRetVal == d_OK)
				break;
			else
				continue;
		}
			
	}
	
	return d_OK;
}



int inCTOSS_ERM_EnterEmail(void)
{
	unsigned char key;
	BYTE sztmpEmail[100];
	USHORT usLen;
	int inRetVal;

	vdDebug_LogPrintf("--inCTOSS_ERM_EnterEmail--");
	
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_KBDBufFlush();

	while(1)
	{
		if (isCheckTerminalMP200() == d_OK)
			setLCDPrint27(3, d_LCD_ALIGNLEFT, "Enter Email Address");
		else
			CTOS_LCDTPrintAligned(3, "Enter Email Address", d_LCD_ALIGNLEFT);
	
		usLen = 50;
		memset(sztmpEmail,0x00,sizeof(sztmpEmail));
		key = InputStringAlphaEx2(1, 4, 0x00, 0x02, sztmpEmail, &usLen, 0, d_INPUT_TIMEOUT);
		//key = InputStringAlphaEx(1, 4, 0x00, 0x02, sztmpEmail, &usLen, 0, d_INPUT_TIMEOUT);
		vdDebug_LogPrintf("sztmpEmail=[%s]",sztmpEmail);
		memset(g_szEmail,0x00,sizeof(g_szEmail));
		strcpy(g_szEmail,sztmpEmail);

		if ((key == d_KBD_ENTER) || (key == d_KBD_CANCEL))
		{
			inRetVal = inERM_Confirmation(ERM_EMAIL_TYPE, sztmpEmail);
			if (inRetVal == d_OK)
				break;
			else
				continue;
		}
			
	}

	return d_OK;
	
}




void vdCTOSS_DisplayStatus(int inRet)
{
	BYTE bySC_status;
	BYTE	szTotalAmt[12+1];
	BYTE	szTempBuf[12+1];
	BYTE	szTempBuf1[12+1]; 
	BYTE	szDisplayBuf[30];
	BYTE	szStr[45];
	unsigned char key;

    vdRemoveCard();
	
	return;
	
	if (inRet == d_OK)
	{
		displaybmpEx(0, 0, "approved.bmp");
	}
	else
	{
		displaybmpEx(0, 0, "declined.bmp");
	}

/*
	if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
	{
		CTOS_SCStatus(d_SC_USER, &bySC_status);
		if(bySC_status & d_MK_SC_PRESENT)
		{
			if ((strTCT.byTerminalType%2) == 0)
				vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"PLEASE REMOVE CARD");
			else
				vduiDisplayStringCenter(7,"PLEASE REMOVE CARD");
			CTOS_Beep();
			CTOS_Delay(300);
			CTOS_Beep();
		}
	}
*/

	if (inRet == d_OK){

		wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);		
		memset(szStr, 0x00, sizeof(szStr)); 
		//format amount 10+2
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
		CTOS_LCDTPrintAligned(1,"AMOUNT:",d_LCD_ALIGNLEFT);
		CTOS_LCDTPrintAligned(1,szStr,d_LCD_ALIGNRIGHT);

		CTOS_LCDTPrintAligned(7, "TRANSACTION APPROVED", d_LCD_ALIGNCENTER);
	}



	if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
	{
		while(1){
			CTOS_SCStatus(d_SC_USER, &bySC_status);
			if(bySC_status & d_MK_SC_PRESENT)
			{
				if ((strTCT.byTerminalType%2) == 0)
					vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"PLEASE REMOVE CARD");
				else
					vduiDisplayStringCenter(8,"PLEASE REMOVE CARD");
				CTOS_Beep();
				CTOS_Delay(300);
				CTOS_Beep();
				continue;
			}
				break;
		}
	}else{

		vduiDisplayStringCenter(8,"PLEASE PRESS ENTER");

		while(1){
			key=WaitKey(15);
			
			if(key == d_KBD_ENTER){
			inRet =  d_OK;	
				break;
		}
	
		}
	}	
}


int inCTOS_ERMAllHosts_DeInitialization(void)
{
	int inNumOfHost = 0,inNum;
	int inNumOfMit = 0,inMitNum;
	char szAPName[50+1];
	char szParaName[12+1];
	int inAPPID;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	vdDebug_LogPrintf("--inCTOS_ERMAllHosts_DeInitialization--");
	vdDebug_LogPrintf("byERMMode[%d]",strTCT.byERMMode);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("ERM DE-INIT");
		
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	inNumOfHost = inHDTNumRecord();
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			vdHostToDeInitialize();

			setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
			
			vdDebug_LogPrintf("szAPName=[%s]-[%s]-inHostIndex=[%d]---",szAPName,strHDT.szAPName,strHDT.inHostIndex);
			if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
			{
				continue;
			}

			inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
			vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
			vdDebug_LogPrintf("MMT inNum=[%d]-----",inNum);
			for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
			{
				/*
                if(inMitNum == 0)
                     setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING.    ");
                else if(inMitNum == 1)
                     setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING..   ");
                else if(inMitNum == 2)
                     setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...  ");
                else if(inMitNum == 3)
                     setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING.... ");
                else if(inMitNum == 4)
                     setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING.....");
                */

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

	vdDisplayMultiLineMsgAlign("ALL HOST", "DEINITIALIZE ERM", "COMPLETE", DISPLAY_POSITION_CENTER);
	
	vdDebug_LogPrintf("end inCTOS_ERMAllHosts_DeInitialization-----");	
		
	return ST_SUCCESS;
}


void vdDisplayProcessing(void)
{
	int inLine=8;
	
    if((strTCT.byTerminalType % 2) == 0)
        inLine=V3_ERROR_LINE_ROW;
			
	setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PROCESSING    ");
	setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PROCESSING.   ");
	setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PROCESSING..  ");
	setLCDPrint(inLine,DISPLAY_POSITION_LEFT,"PROCESSING... ");
}

void vdClearERMBatch(BOOL fConfirm)
{
	int inResult;

	if (fConfirm)
		inResult = vduiAskConfirmation("DELETE ERM BATCH?");
	else
		inResult = d_OK;
	
	if (inResult == d_OK)
	{
		vdDebug_LogPrintf("::CLEAR ERM BATCH------[start]");
		inResult = inERMAP_Database_BatchDelete();
		vdDebug_LogPrintf("::vdClearERMBatch::inERMAP_Database_BatchDelete::inResult[%d]", inResult);
		inResult = inERMAP_Database_ERMAdviceBatchDelete();
		vdDebug_LogPrintf("::vdClearERMBatch::inERMAP_Database_ERMAdviceBatchDelete::inResult[%d]", inResult);
		inResult = inERMAP_Database_ERMTransDataBatchDelete();
		vdDebug_LogPrintf("::vdClearERMBatch::inERMAP_Database_ERMTransDataBatchDelete::inResult[%d]", inResult);
		inResult = inERMAP_Database_ERMTransDataBackupBatchDelete();
		vdDebug_LogPrintf("::vdClearERMBatch::inERMAP_Database_ERMTransDataBackupBatchDelete::inResult[%d]", inResult);

		if (fConfirm)
		{
			CTOS_LCDTClearDisplay();	
		    vdDispTitleString("CLEAR ERM BATCH");
			vdCTOS_DispStatusMessage("ERM BATCH DELETED");
			CTOS_KBDBufFlush();

			CTOS_Beep();
		    CTOS_Delay(2000);
		}		
	}
		
	vdDebug_LogPrintf("::CLEAR ERM BATCH------[end]");
	
}

void vdCTOSS_DisplaySettleSuccess(void)
{
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	setLCDPrint27(4,DISPLAY_POSITION_CENTER, strHDT.szHostLabel);
	setLCDPrint27(5,DISPLAY_POSITION_CENTER, "  SETTLEMENT  ");
	setLCDPrint27(6,DISPLAY_POSITION_CENTER, "  SUCCESSFUL  ");

	CTOS_Beep();
	CTOS_Delay(2000);

	vduiClearBelow(3);
}

// Check ERM data for batch empty -- sidumili
BOOL fERM_ChkBatchEmpty(BOOL fPrompt)
{
	BOOL fEmpty = TRUE;
	int inRecNum = 0;

	vdDebug_LogPrintf("--fERM_ChkBatchEmpty--[start]");
	
	inRecNum = inERMNumRecord();

	vdDebug_LogPrintf("inRecNum[%d]",inRecNum);
	
	if (inRecNum > 0)
	{
		fEmpty = FALSE;
	}

	if (fPrompt == TRUE && fEmpty == TRUE)
		vdDisplayErrorMsg(1, 8, "ERM BATCH EMPTY");
	

	vdDebug_LogPrintf("--fERM_ChkBatchEmpty--[end]");
	
	return(fEmpty);	
}

BOOL fERM_ChkInitialize(void)
{
	BOOL fInit = FALSE;
	
	if (strTCT.byERMInit == 1)
	{
		fInit = TRUE;
		CTOS_LCDTClearDisplay();
		setLCDPrint27(8,DISPLAY_POSITION_LEFT, "ERM ALREADY INIT");
		CTOS_Beep();
		CTOS_Delay(2000);
	}

	return(fInit);
}

BOOL fERM_ChkFuncAvailable(BOOL fPrintReport)
{
	BOOL fAvailable = TRUE;
vdDebug_LogPrintf("fPrintReport[%d],strTCT.byERMMode=%d", fPrintReport, strTCT.byERMMode);
	if (fPrintReport == TRUE)
	{
		if (isCheckTerminalMP200() == d_OK)
			fAvailable = FALSE;
	}
	else
	{
		if(strTCT.byERMMode == 0)
			fAvailable = FALSE;
	}

	if (!fAvailable)
	{
		CTOS_LCDTClearDisplay();
		setLCDPrint27(8,DISPLAY_POSITION_LEFT, "FUNC NOT AVAILABLE");
		CTOS_Beep();
		CTOS_Delay(2000); 
	}
	vdDebug_LogPrintf("fERM_ChkFuncAvailable[%d]", fAvailable);
	return(fAvailable);
}

// SMS / EMAIL Confirmation -- sidumili
int inERM_Confirmation(int inType, char *szConfirm)
{
	unsigned char key = 0;
	BYTE sztmpEmail[100];
	BYTE sztmpEmail2[100];
	int usLen;
	BOOL fEmail = FALSE;
	int inRet = d_NO;
	
	CTOS_KBDBufFlush();

	CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

	if (inType == ERM_EMAIL_TYPE)
	{
		if (strlen(szConfirm) > 20)
		{
			fEmail = TRUE;
			memset(sztmpEmail, 0x00, sizeof(sztmpEmail));
			memset(sztmpEmail2, 0x00, sizeof(sztmpEmail2));
			memcpy(sztmpEmail, szConfirm, 20);
			memcpy(sztmpEmail2, &szConfirm[20], strlen(szConfirm) - 20);
		}
	}
	
	if (strlen(szConfirm) <= 0)
		strcpy(szConfirm, "NONE");
	
	if (isCheckTerminalMP200() == d_OK)
	{
		setLCDPrint27(2, d_LCD_ALIGNCENTER, "***CONFIRMATION***");
		
		switch (inType)
		{
			case ERM_EMAIL_TYPE:		
				setLCDPrint27(3, d_LCD_ALIGNCENTER, "Email Receipt to");	
			break;
			case ERM_SMS_TYPE:
				setLCDPrint27(3, d_LCD_ALIGNCENTER, "Receipt SMS to");	
			break;
		}

		if (fEmail)
		{
			setLCDPrint27(4, d_LCD_ALIGNCENTER, sztmpEmail);
			setLCDPrint27(5, d_LCD_ALIGNCENTER, sztmpEmail2);
			setLCDPrint27(8, d_LCD_ALIGNCENTER, "BACK[X] CONTINUE[OK]");
		}
		else
		{
			setLCDPrint27(4, d_LCD_ALIGNCENTER, szConfirm);
			setLCDPrint27(8, d_LCD_ALIGNCENTER, "BACK[X] CONTINUE[OK]");
		}
	}
	else
	{
		switch (inType)
		{
			case ERM_EMAIL_TYPE:
				CTOS_LCDTPrintAligned(3, "Email Receipt to", d_LCD_ALIGNCENTER);
			break;
			case ERM_SMS_TYPE:
				CTOS_LCDTPrintAligned(3, "Receipt SMS to", d_LCD_ALIGNCENTER);
			break;	
		}

		if (fEmail)
		{
			CTOS_LCDTPrintAligned(4, sztmpEmail, d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(5, sztmpEmail2, d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(6, "BACK[X] CONTINUE[OK]", d_LCD_ALIGNCENTER);
		}
		else
		{
			CTOS_LCDTPrintAligned(4, szConfirm, d_LCD_ALIGNCENTER);
			CTOS_LCDTPrintAligned(5, "BACK[X] CONTINUE[OK]", d_LCD_ALIGNCENTER);
		}
	}

	CTOS_KBDBufFlush();
	
	while (1)
	{
		CTOS_Beep();
		CTOS_Delay(300);
		CTOS_Beep();
		
		CTOS_KBDGet(&key);
		
        if(key == d_KBD_ENTER)
        { 	
        	CTOS_LCDTClearDisplay();
        	inRet = d_OK;
			break;
        }
        else if(key == d_KBD_CANCEL)
        {
        	CTOS_LCDTClearDisplay();
			vdDispTransTitle(srTransRec.byTransType);
		
        	inRet = d_NO;
			break;
        }		
	}
	
	return inRet;
	
}

void vdCTOS_ERMNotification(void)
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

	int inKeySelected = 0;
	char szHeaderString[50] = {0};
	int inHeaderAttr = 0x01+0x04;
	char szMenu[1024] = {0};
	BYTE szTemp1[20 + 1] = {0};
	int inValue = 0;
	int inNotif;
	int inNotificationType;
	
	inNotificationType = get_env_int("ERMNOTIFICATIONTYPE");

	CTOS_LCDTClearDisplay();
	vdDispTitleString("SET ERM NOTIFICATION");
	
	memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szMenu, 0x00, sizeof(szMenu));
	memset(szTemp1, 0x00, sizeof(szTemp1));
	
	switch (inNotificationType)
	{	
		case ERM_NONE_TYPE:
			strcpy(szTemp1, "NONE");
		break;	
		case ERM_SMS_TYPE:
			strcpy(szTemp1, "SMS");
		break;
		case ERM_EMAIL_TYPE:
			strcpy(szTemp1, "EMAIL");
		break;
		default:
			strcpy(szTemp1, "NONE");
		break;
	}

	sprintf(szHeaderString, "ERM NOTIF: %s", szTemp1);	
	strcpy((char*)szMenu, "NONE \nSMS \nEMAIL ");	
	inKeySelected = MenuDisplay(szHeaderString, strlen(szHeaderString), inHeaderAttr, 1, 1, szMenu, TRUE);
	vdDebug_LogPrintf("::ERM NOTIF::inSetMenuSelection::inKeySelected[%d]", inKeySelected);
	
	if (inKeySelected != d_USER_CANCEL)
	{
		inValue = inKeySelected - 1; // (0-NONE, 1-COM1, 2-COM2, 3-USB)

		switch(inValue)
		{
			case 0:
				inNotif = ERM_NONE_TYPE; 
				break;
			case 1:
				inNotif = ERM_SMS_TYPE; 
				break;
			case 2:
				inNotif = ERM_EMAIL_TYPE; 
				break;
			default:
				inNotif = ERM_NONE_TYPE;
				break;
		}
		
		put_env_int("ERMNOTIFICATIONTYPE", inNotif);
	}
	
	vdDebug_LogPrintf("::ERM NOTIF::inKeySelected[%d]::szHeaderString[%s]::inValue[%d]::inRet[%d]::inPort[%d]", inKeySelected, szHeaderString, inValue, inRet, inNotif);
	   
	return ;
}

int inCTOSS_REUploadReceipt(void)
{
	int inRet;
	int inTotal = 0;
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	//CTOS_LCDTClearDisplay();    
	
	vdDebug_LogPrintf("--inCTOSS_REUploadReceipt--");

	//inRet = inCTOSS_HOST_ERMInit();
	//if (inRet != d_OK)
	//	return inRet;

	inRet = usCTOSS_Erm_UploadReceipt(FALSE);
	
	if (inRet != d_OK)
	{		
		inTotal = usCTOSS_Erm_GetBackupReceiptTotal();

		if (inTotal > 0)
		{
			CTOS_LCDTClearDisplay();
			vdDisplayMultiLineMsgAlign("PLEASE WAIT", "RE-UPLOAD", "ERM RECEIPT", DISPLAY_POSITION_CENTER);		
		
			usCTOSS_Erm_RestoreBackupReceipt();
			
			inRet = usCTOSS_Erm_UploadReceipt(FALSE);
		}

		vdDebug_LogPrintf("inCTOSS_REUploadReceipt | inRet[%d]", inRet);
	
		return inRet;
	}

	return d_OK;
}

/*
int inCTOSS_HOST_ReUpload_ERMInit(void)
{
	int inResult = d_NO;
	BYTE szParaName[100];
	
	BYTE tmpbuf[100];
	BYTE szlastHost[5];
	BYTE szLastMerch[5];


	memset(szlastHost, 0x00, sizeof(szlastHost));
	memset(szLastMerch, 0x00, sizeof(szlastHost));

	inCTOSS_GetEnvDB("LASTHOST", szlastHost);
	inCTOSS_GetEnvDB("LASTMERC", szLastMerch);


	memset(szParaName,0x00,sizeof(szParaName));
	sprintf(szParaName,"%s%02d%02d","ERM",szlastHost,szLastMerch);
	vdDebug_LogPrintf("ERM TEST szParaName =[%s]",szParaName);
	
	inCTOSS_GetEnvDB(szParaName,tmpbuf);


	vdDebug_LogPrintf("ERM TEST %s", tmpbuf);

	if (strlen(tmpbuf) > 0 && atoi(tmpbuf) == 1)
		return ST_SUCCESS;


	inCTOSS_ERM_Initialization();
	
	//inCTOS_MultiAPALLAppEventID(d_IPC_CMD_ERM_INIT);


	return ST_SUCCESS;

}
*/

int inCTOSS_HOST_ERMInit(void)
{
	int inResult = d_NO;
	BYTE szParaName[100];
	
	BYTE tmpbuf[100];

	vdDebug_LogPrintf("--inCTOSS_HOST_ERMInit--");
	vdDebug_LogPrintf("strHDT.szHostLabel[%s]", strHDT.szHostLabel);
	vdDebug_LogPrintf("strHDT.inHostIndex[%d]", strHDT.inHostIndex);
	vdDebug_LogPrintf("strMMT[0].MITid[%d]", strMMT[0].MITid);
	
	memset(szParaName,0x00,sizeof(szParaName));
	sprintf(szParaName,"%s%02d%02d","ERM",strHDT.inHostIndex,strMMT[0].MITid);
	vdDebug_LogPrintf("ERM TEST szParaName =[%s]",szParaName);
	
	inCTOSS_GetEnvDB(szParaName,tmpbuf);

	vdDebug_LogPrintf("ERM TEST %s", tmpbuf);

	if (strlen(tmpbuf) > 0 && atoi(tmpbuf) == 1)
		return ST_SUCCESS;

	CTOS_LCDTClearDisplay();
    vdDispTitleString("ERM INIT");

	//vdCTOS_DispStatusMessage("PLEASE WAIT...");
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	inResult = inCTOSS_ERM_Initialization();
	
	if (inResult == d_OK)	
		vdDisplayMultiLineMsgAlign("", "INITIALIZE ERM", "COMPLETE", DISPLAY_POSITION_CENTER);
	else
		vdDisplayMultiLineMsgAlign("", "INITIALIZE ERM", "FAILED", DISPLAY_POSITION_CENTER);

	CTOS_Delay(1000);
	CTOS_LCDTClearDisplay();
	
	return inResult;

}

// ReFork SHARLS_ERM -- sidumli
//android-removed
#if 0
void vdReFork_SHARLS_ERM(void)
{
	BYTE processID[100];
	char szAppname[100+1];
	pid_t pid = -1;
	
	if (strTCT.byERMMode == 0)
	return;

	vdDebug_LogPrintf("--vdReFork_SHARLS_ERM--");

	strcpy(szAppname, "SHARLS_ERM");

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("vdReFork_SHARLS_ERM =[%d][%s]",pid, szAppname);
	if (pid <= 0)
	{
		vdCTOSS_ReForkSubAP(szAppname);

		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);

		//here need delay, wait sub AP initialization 
		if (pid > 0)
			CTOS_Delay(3000);

		vdDebug_LogPrintf("pid[%d] | szAppname[%s] | processID[%s]", pid, szAppname, processID);
	}		
}
#endif

int inCTOSS_ERMInitAllHost(void)
{
	int inResult = d_NO;

	vdDebug_LogPrintf("--inCTOSS_ERMInitAllHost--");
	vdDebug_LogPrintf("byERMMode[%d]..byERMInit[%d]",strTCT.byERMMode, strTCT.byERMInit);
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;
	
	vdDebug_LogPrintf("inCTOSS_ERMInit..byERMInit=[%d]",strTCT.byERMInit);
	if(strTCT.byERMInit == 1)
		return ST_SUCCESS;

	if (inCTOS_ERMPreConnect(TRUE) != d_OK)
		return ST_SUCCESS;

	CTOS_LCDTClearDisplay();
    vdDispTitleString("ERM INIT");	
							
	inResult = inCTOS_ERMAllHosts_Initialization(TRUE);

	if (inResult != d_OK)
		vdDisplayMultiLineMsgAlign("ALL HOST", "INITIALIZE ERM ", "FALIED", DISPLAY_POSITION_CENTER);
	else
		vdDisplayMultiLineMsgAlign("ALL HOST", "INITIALIZE ERM ", "COMPLETE", DISPLAY_POSITION_CENTER);
	
	return ST_SUCCESS;
}

void vdHostToInitialize(BOOL fPrompt, BOOL fSuccess)
{
	char szErrMsg[30+1];

	vdDebug_LogPrintf("--vdHostToInitialize--");
	
	memset(szErrMsg,0x00,sizeof(szErrMsg));
	sprintf(szErrMsg,"INIT %s",strHDT.szHostLabel);
	vdCTOS_Pad_String(szErrMsg, 23, IS_SPACE, POSITION_RIGHT);
	setLCDPrint27(7,DISPLAY_POSITION_LEFT,szErrMsg);

	if (fPrompt)
	{
		if (fSuccess)
		{
			setLCDPrint27(8,DISPLAY_POSITION_LEFT, "SUCCESS");
			CTOS_Beep();
		}			
		else
		{
			setLCDPrint27(8,DISPLAY_POSITION_LEFT, "FAILED");
			CTOS_Beep();
			CTOS_Delay(300);
			CTOS_Beep();
		}			
	}

	vdDebug_LogPrintf("fSuccess[%d] -- HostName[%s]", fSuccess, strHDT.szHostLabel);
	//vdDebug_LogPrintf("TID[%s]", strMMT[0].szTID);
	//vdDebug_LogPrintf("MID[%s]", strMMT[0].szMID);
	
}

void vdHostToDeInitialize(void)
{
	char szErrMsg[30+1];
	
	memset(szErrMsg,0x00,sizeof(szErrMsg));
    sprintf(szErrMsg,"DE-INIT %s",strHDT.szHostLabel);
	vdCTOS_Pad_String(szErrMsg, 23, IS_SPACE, POSITION_RIGHT);
    setLCDPrint27(7,DISPLAY_POSITION_LEFT,szErrMsg);
}

UINT usCTOSS_Erm_GetBackupReceiptTotal(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	vdDebug_LogPrintf("--usCTOSS_Erm_GetBackupReceiptTotal--");

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_ERM.SHARLS_ERM", d_IPC_CMD_ERM_GET_BACKUPRECEIPTTOT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_GetBackupReceiptTotal usOutLen[%d] bOutBuf[%x,%d,%d]",usOutLen, bOutBuf[0],bOutBuf[1],bOutBuf[2]);
	if(d_OK == usResult)
	{
		if(bOutBuf[0] == d_IPC_CMD_ERM_GET_BACKUPRECEIPTTOT)
			//inTotal = bOutBuf[1];
			inTotal = (bOutBuf[1] | bOutBuf[2] << 8);

		vdDebug_LogPrintf("usCTOSS_Erm_GetBackupReceiptTotal inTotal[%d] ",inTotal);
	}
	
    return inTotal;
}

void vdSetLastMerchant(void)
{
	char szLastHost[10];
	char szLastMerch[10];

	vdDebug_LogPrintf("--vdSetLastMerchant--");
	vdDebug_LogPrintf("strHDT.inHostIndex[%d]", strHDT.inHostIndex);
	vdDebug_LogPrintf("strHDT.szHostLabel[%s]", strHDT.szHostLabel);
    vdDebug_LogPrintf("strMMT[0].MITid[%d]", strMMT[0].MITid);
	vdDebug_LogPrintf("strMMT[0].szMerchantName[%s]", strMMT[0].szMerchantName);
	vdDebug_LogPrintf("strMMT[0].szTID[%s]", strMMT[0].szTID);
	vdDebug_LogPrintf("strMMT[0].szMID[%s]", strMMT[0].szMID);
	
	memset(szLastHost, 0x00, sizeof(szLastHost));
	memset(szLastMerch, 0x00, sizeof(szLastMerch));

	sprintf(szLastHost,"%02d",strHDT.inHostIndex);
	sprintf(szLastMerch,"%02d",strMMT[0].MITid);

	inCTOSS_PutEnvDB("ERMLASTHOST",szLastHost);
	inCTOSS_PutEnvDB("ERMLASTMERC",szLastMerch);

	vdDebug_LogPrintf("szLastHost[%s]", szLastHost);
	vdDebug_LogPrintf("szLastMerch[%s]", szLastMerch);	
}

void vdGetLastMerchant(void)
{
	char szParaName[12+1];
	char szLastHost[10];
	char szLastMerch[10];
	int inLastHost = 0;
	int inLastMerch = 0;

	vdDebug_LogPrintf("--vdGetLastMerchant--");

	memset(szLastHost, 0x00, sizeof(szLastHost));
	memset(szLastMerch, 0x00, sizeof(szLastMerch));

	inCTOSS_GetEnvDB("ERMLASTHOST", szLastHost);
	inCTOSS_GetEnvDB("ERMLASTMERC", szLastMerch);

	vdDebug_LogPrintf("szLastHost[%s]", szLastHost);
	vdDebug_LogPrintf("szLastMerch[%s]", szLastMerch);

	inLastHost = atoi(szLastHost);
	inLastMerch = atoi(szLastMerch);			
	strHDT.inHostIndex = (USHORT)inLastHost;
	srTransRec.HDTid = atoi(szLastHost);

	vdDebug_LogPrintf("strHDT.inHostIndex[%d]", strHDT.inHostIndex);
	vdDebug_LogPrintf("srTransRec.HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("inLastHost[%d]", inLastHost);
	vdDebug_LogPrintf("inLastMerch[%d]", inLastMerch);
	
	if (inMMTReadRecord(inLastHost, inLastMerch) != d_OK)
    {
        vdSetErrorMessage("LOAD MMT ERR");
        //return(d_NO);
    }

	if (inHDTRead(inLastHost) != d_OK)
    {
        vdSetErrorMessage("LOAD HDT ERR");
        //return(d_NO);
    }

	// Deinit
	memset(szParaName,0x00,sizeof(szParaName));
	sprintf(szParaName,"%s%02d%02d","ERM",strHDT.inHostIndex,strMMT[0].MITid);
	vdDebug_LogPrintf("szParaName =[%s]",szParaName);
	inCTOSS_PutEnvDB(szParaName,"0");
	
}

int inCTOSS_HOST_ReERMInit(void)
{
	int inResult = d_NO;
	
	vdDebug_LogPrintf("--inCTOSS_HOST_ReERMInit--");

	vdGetLastMerchant();	
	
	CTOS_LCDTClearDisplay();
    vdDispTitleString("ERM INIT");

	vdCTOS_DispStatusMessage("PLEASE WAIT...");
	
	inResult = inCTOSS_ERM_Initialization();
	
	if (inResult == d_OK)	
		vdDisplayMultiLineMsgAlign("", "INITIALIZE ERM", "COMPLETE", DISPLAY_POSITION_CENTER);
	else
		vdDisplayMultiLineMsgAlign("", "INITIALIZE ERM", "FAILED", DISPLAY_POSITION_CENTER);

	CTOS_Delay(1000);
	CTOS_LCDTClearDisplay();
	
	return inResult;

}


int inCTOSS_ERM_Form_Receipt_Diag(int inSettle, int inPurpose)
{
	char szTemp[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	BYTE szfilename[128];
	int inResult;	

	vdDebug_LogPrintf("--inCTOSS_ERM_Form_Receipt_Diag--");
	vdDebug_LogPrintf("strTCT.byERMMode[%d] | strTCT.byTerminalType[%d]", strTCT.byERMMode, strTCT.byTerminalType);
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	// Minimize the white screend display -- sidumili
	vdCTOS_DispStatusMessage("PROCESSING...");
	
	memset(&strERMTransData,0x00,sizeof(strERMTransData));

	//vdCTOSS_SIGPField61();
	inCTOSS_FormERMReceipt(ERC_SHARE_HEADER_FILE);
	
	inERMAP_Database_BatchDelete();

	strERMTransData.shType = srTransRec.byTransType;
	//strcpy(strERMTransData.szTPDU,);

	strcpy(strERMTransData.szPAN,strService.szServiceID); // szServiceIDs


	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	strcpy(strERMTransData.szAmt,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(strService.szArrivalTime, szTemp, TIME_BCD_SIZE);// ARRIVAL

	if(strcmp(szTemp,"000000") == 0)
		strcpy(szTemp,"999999");
	
	strcpy(strERMTransData.szInvNum,szTemp);
	
	sprintf(strERMTransData.szSTAN, "%06ld", srTransRec.ulTraceNum);
    
	memset (szTemp, 0x00, sizeof(szTemp));
    if(srTransRec.byTransType == VOID)
    {
        vdGetTimeDate1(&srTransRec);
        wub_hex_2_str(srTransRec.szTempTime, szTemp,TIME_BCD_SIZE);
        strcpy(strERMTransData.szTime,szTemp);
    }
    else
    {
        wub_hex_2_str(srTransRec.szTime, szTemp,TIME_BCD_SIZE);
        strcpy(strERMTransData.szTime,szTemp);
    }
	
	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	strcpy(strERMTransData.szDate,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
	strcpy(strERMTransData.szExpDate,szTemp);
	

	memcpy(strERMTransData.szRefNum,&strService.szUnitSN[3],12); // UNIT SERIAL NUMBER



	memset (szTemp2, 0x00, sizeof(szTemp2));
	if(inPurpose == SOFTWARE_HARDWARE_TEST || inPurpose == SOFTWARE_TEST)
		wub_hex_2_str(strSWDiag.szSWEndTime, szTemp2,TIME_BCD_SIZE);
	else if(inPurpose == HARDWARE_TEST)
		wub_hex_2_str(strHWDiag.szHWEndTime, szTemp2,TIME_BCD_SIZE);
	else
		wub_hex_2_str(strService.szSvcEndTime, szTemp2,TIME_BCD_SIZE);
	
	strcpy(strERMTransData.szApprCode,szTemp2);

	strcpy(strERMTransData.szTID,srTransRec.szTID);
	strcpy(strERMTransData.szMID,srTransRec.szMID);

	memset (szTemp, 0x00, sizeof(szTemp));
	if(inPurpose == SOFTWARE_HARDWARE_TEST || inPurpose == HARDWARE_TEST)
		wub_hex_2_str(strHWDiag.szHWStartTime, szTemp,TIME_BCD_SIZE);
	else if(inPurpose == SOFTWARE_TEST)
		wub_hex_2_str(strSWDiag.szSWStartTime, szTemp,TIME_BCD_SIZE);
	else
		wub_hex_2_str(strService.szSvcStartTime, szTemp,TIME_BCD_SIZE); // TIME START AND TIME END
		
	sprintf(strERMTransData.szBatchNum,"%s",szTemp);	

	szGetTransTitle(srTransRec.byTransType, strERMTransData.szPaymentType);
	vdDebug_LogPrintf("byTransType=[%d]---szPaymentType[%s]--",srTransRec.byTransType,strERMTransData.szPaymentType);

	strcpy(strERMTransData.szPaymentMedia,srTransRec.szCardLable);

	strcpy(strERMTransData.szReserved3,strTCT.szUnitModel);	

	sprintf(strERMTransData.szSingatureStatus,"%02d",ginSingatureStatus);

	inExeAPIndex = srTransRec.HDTid;
	sprintf(strERMTransData.szReceiptLogoIndex, "%02d", inExeAPIndex);
	vdDebug_LogPrintf("inExeAPIndex=[%d]-----",srTransRec.HDTid);

	{
		strERMTransData.fSettle = 0;
		if(strTCT.byERMMode != 0)
		{

				vdDebug_LogPrintf("REINER :: strService.szCustPhoneNo[%s]",strService.szCustPhoneNo);
				vdDebug_LogPrintf("REINER :: strService.szCustName[%s]",strService.szCustName);
				strcpy(strERMTransData.szBankCode,strService.szCustPhoneNo);//MERCHANT CONTACT
				strcpy(strERMTransData.szReserved2,strService.szCustName);//MERCHANT NAME

				sprintf(strERMTransData.szReserved1, "%s|%s|",
					strlen(strService.szSIMSN)>0?strService.szSIMSN:"00",
					strlen(strService.szDockSN)>0?strService.szDockSN:"00");
				
				vdDebug_LogPrintf("REINER :: Additional_Info[%s]",strERMTransData.szReserved1);
				sprintf(strERMTransData.szMerchantCode,"%s",strService.szFEName);
				vdDebug_LogPrintf("REINER :: NRIC[%s]",strERMTransData.szMerchantCode);		
			
		}
		else
		{
			sprintf(strERMTransData.szReserved1, "ACC%02d%02d.total"
									, strHDT.inHostIndex
									, srTransRec.MITid);
			vdDebug_LogPrintf("local filename=[%s]-----",strERMTransData.szReserved1);

			sprintf(strERMTransData.szReserved2, "%d", srTransRec.byOrgTransType);
		}
		memcpy(strERMTransData.szReceiptImageFileName,srTransRec.szOrgAmount,AMT_BCD_SIZE);
		vdPCIDebug_HexPrintf("szOrgAmount",srTransRec.szOrgAmount,AMT_BCD_SIZE);
	}

	// Debug
	vdDebug_LogPrintf("-------------------------------------------------------------------");
	vdDebug_LogPrintf("srTransRec.HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("srTransRec.MITid[%d]", srTransRec.MITid);
	vdDebug_LogPrintf("srTransRec.szHostLabel[%s]", srTransRec.szHostLabel);
	vdDebug_LogPrintf("szPAN[%s]", strERMTransData.szPAN);
	vdDebug_LogPrintf("szAmt[%s]", strERMTransData.szAmt);
	vdDebug_LogPrintf("szInvNum[%s]", strERMTransData.szInvNum);
	vdDebug_LogPrintf("szSTAN[%s]", strERMTransData.szSTAN);
	vdDebug_LogPrintf("szTime[%s]", strERMTransData.szTime);
	vdDebug_LogPrintf("szDate[%s]", strERMTransData.szDate);
	vdDebug_LogPrintf("szRefNum[%s]", strERMTransData.szRefNum);
	vdDebug_LogPrintf("szApprCode[%s]", strERMTransData.szApprCode);
	vdDebug_LogPrintf("szTID[%s]", strERMTransData.szTID);
	vdDebug_LogPrintf("szMID[%s]", strERMTransData.szMID);
	vdDebug_LogPrintf("szBatchNum[%s]", strERMTransData.szBatchNum);
	vdDebug_LogPrintf("szPaymentMedia[%s]", strERMTransData.szPaymentMedia);
	vdDebug_LogPrintf("szReserved3[%s]", strERMTransData.szReserved3);
	vdDebug_LogPrintf("-------------------------------------------------------------------");
	
	inERMAP_Database_BatchInsert(&strERMTransData);	

	// Minimize the white screend display -- sidumili
	vdCTOS_DispStatusMessage("PROCESSING...");
	
	usCTOSS_Erm_FromReceipt();

	vdSetLastMerchant(); // Set Last Merchant -- sidumili

}

