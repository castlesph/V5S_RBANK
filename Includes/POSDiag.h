
#ifndef ___POS_DIAG___
#define ___POS_DIAG___

#include <Typedef.h>
#include"..\Includes\POSTypedef.h"

#define MCC_CARD_ID			1
#define INST_CARD_ID		2
#define BANCNET_CARD_ID		3

#define VOID_FAILED 		1
#define SETTLEMENT_FAILED 	2

#define INSTALLATION		1
#define PULL_OUT			2
#define REPROGRAMMING		3
#define NEGATIVE			4
#define SOFTWARE_TEST		5
#define HARDWARE_TEST		6
#define SOFTWARE_HARDWARE_TEST		7

#define ENGINEER_COPY_RECEIPT 4

#define SCHEDULED_HARDWARE	1
#define SCHEDULED_SOFTWARE	2
#define SCHEDULED_FULL		3

typedef struct
{
	int inLCD_Status;
	int inPrinter_Status;
	int inCOMMS_Status;
    int inCTLSReader_Status;
	int inMSReader_Status;
	int inICCReader_Status;
	int inHWErrorCount;
	BYTE szHWStartTime[TIME_BCD_SIZE+1];    
	BYTE szHWEndTime[TIME_BCD_SIZE+1];
}HW_DIAG_TABLE;

HW_DIAG_TABLE strHWDiag;


typedef struct
{
	int inCredit_Status;
	int inInst_Status;
	int inBancnet_Status;
	int inSWErrorCount;	
	BYTE szSWStartTime[TIME_BCD_SIZE+1];    
	BYTE szSWEndTime[TIME_BCD_SIZE+1];
	
}SW_DIAG_TABLE;

SW_DIAG_TABLE strSWDiag;

typedef struct
{
	BYTE szCustName[20+1] ;
	BYTE szCustPhoneNo[11+1];
	BYTE szUnitModel[20+1];
	BYTE szUnitSN[20+1];
	BYTE szPowerSN[20+1];
	BYTE szSIMSN[20+1];
	BYTE szDockSN[20+1];
	BOOL fOriented;
	BYTE szFEName[20+1];
	BYTE szOrigin[20+1];
	BYTE szDestination[20+1];
	BYTE szFare[20+1];
	BYTE szAccomodation[20+1];
	BYTE szTime_Start[TIME_BCD_SIZE+1];     /* BCD hhmmss        */
	BYTE szTime_End[TIME_BCD_SIZE+1];     /* BCD hhmmss        */
	BYTE szTotalSvcExpense[20+1];     /* BCD hhmmss        */
	BYTE szReason[30+1] ;
	BYTE szSvcStartTime[TIME_BCD_SIZE+1];    
	BYTE szSvcEndTime[TIME_BCD_SIZE+1];
	BYTE szNegativeTitle[46+1];
	BYTE szArrivalTime[TIME_BCD_SIZE+1];
	BYTE szServiceID[MERCHANT_ID_BYTES+1];
}SERVICE_INFO_TABLE;

SERVICE_INFO_TABLE strService;

typedef struct
{
	BYTE	szHostLabel[16] ;
    USHORT  inHostIndex;
	BYTE	szTPDU[5+1];
	BYTE	szNII[NII_BYTES+1] ;
	BOOL	fReversalEnable;
	BOOL	fHostEnable;
    USHORT inCurrencyIdx;
	char	szAPName[25];
    BYTE	szTID[10] ;
    BYTE	szMID[20] ;	
}FRS_ERM_INFO_TABLE;

FRS_ERM_INFO_TABLE strFSR;



void vdCITASServices(void);
void vdJobType(void);
int vdService(void);
void vdSWDiagnostics(void);
void vdHWDiagnostics(void);
void vdManualHWDiagnostics(void);
int inCTOS_vdInstallation(void);
void vdPullOut(void);
void vdReprogramming(void);
void vdNegative(void);
void vdAutoHWDiagnostics(char* szTitle);
int inCheckCommsHW(int inLine);
void vdCheckPrinter(int inLine);
void vdCheckMSRReader(int inLine);
void vdCheckICCReader(int inLine);
void vdCheckCTLSReader(int inLine);
void vdResetAllCardData(void);
int inSetCardForInstallation(void);
void vdDisplaySummary(int inPurpose);
int inDatabase_TestCardInsert(TRANS_DATA_TABLE *transData, int inCardDataID);
int inDatabase_TestCardUpdate(TRANS_DATA_TABLE *transData, int inCardDataID);
int inTestCardRead(TRANS_DATA_TABLE *transData, int inSeekCnt);
void vdDisplayHWDetailReport(void);
void vdDisplaySWDetailReport(void);
int inInputReceiverName(void);
BYTE InputStringLetters(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS);
int inCTOSS_EnterNumber(BYTE* szBuff, int inMaxChar, int inMinChar, int inLine);
int ushCTOS_ePadSignatureEx(void);
int inCTOS_GetTerminalDetails(BYTE* szBuff, int inColumn, int inRow, USHORT inMax, int inMin);
void vdEnterTerminalDetails(int inPurpose, char* szTitle);
void vdConfirmFEDetails(int inPurpose, char* szTitle);
void vdTrimSpaces(char* pchString);
int inInputName(BYTE* szBuff, USHORT usColumn, USHORT usRow);
int inCheckBatchEmtpy(void);
void vdModifyTIDMIDForService(int inPurpose);
void vdRevertTIDMIDForService(void);
void inCTOS_vdAutoInstallation(void);
void inCTOS_vdManualInstallation(void);
void vdEnterFEDetails(char* szTitle);
USHORT shCTOS_GetTime(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS);
int inCTOS_AutoDiag_ParamCheck(void);
int inCTOSS_Scheduled_Diagnostics(void);
void vdSetADiagParams(void);
void put_env_char(char *tag, char *value);
void vdScheduledHWDiagnostics(char* szTitle);
void vdScheduledTerminalDiagnostics(void);
BYTE InputStringAlpha2Ex(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS);
BYTE InputAmount3(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter);
USHORT ushCTOS_printReceipt_Diag(void);
USHORT ushCTOS_PrintServiceBody(int page, int inPurpose);
USHORT ushCTOS_PrintScheduledDiagBody(void);
int inCTOSS_FSR_ERMInitAllHost(void);
int inCTOS_FSR_ERMAllHosts_Initialization(BOOL fAllHost);
void CTOSS_SetRTCEx(void);
void vdPreventive_Maintenance(void);
void vdSetMaintenance(void);
void vdSetDaysGap(void);
int inCTOS_ADC_Download(void);
void vdSetADLParams(void);
int inCTOSS_ADLSettlementCheckTMSDownloadRequest(void);
int inCheckBatcheNotEmtpy(void);
int inCTOSS_TMSPreConfig3(int inComType);
int inCTOSS_TMSDownloadviaReprogramming(void);
int inTMSRead(int inSeekCnt);
int inRetryDiagnosticsPrompt(void);
int inDisplaySummarywithRetry(int inPurpose);
ULONG EMVCL_Open(void);
int inMerchantCheckList(void);
int inGetAnswer(int inLine);
void vdSetMerchantResponseToTxt(int inAnswer, char* szResponse);

int vdMerchantServices(void);
int inChooseMerchantorWitness (void);
void vdOrientationInvoiceNumbers (char * InvoiceNumber);
void vdPrintOrientationInvoiceNumbers(void);
void vdFSRPrintMCCHostInfo(void);
int inPrintLastFSRReport(void);
void vdDiagRemoveCard(void);
void vdEnterManualSWDiagStatus(void);
void vdFormatDisplayTime(BYTE* bInputStrData, BYTE* bOutputStrData);


#endif //end ___POS_DIAG___

