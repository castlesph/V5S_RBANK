
#ifndef ___POS_TRANS___
#define ___POS_TRANS___

#include <Typedef.h>

#include"..\Includes\POSTypedef.h"
#include"..\ApTrans\MultiShareEMV.h"


//CVV define
#define 	CVV2_NONE				0
#define 	CVV2_MANUAL				1
#define 	CVV2_MSR				2
#define 	CVV2_MANUAL_MSR			3
#define 	CVV2_MANUAL_MSR_CHIP	4

#define		CVV2_SIZE				6


#define d_ONLINE_KEYSET     0x2000
#define d_ONLINE_KEYINDEX   0x0000
#define d_OFFLINE_KEYINDEX  0x0001

#define d_ONLINE_KEY        "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11"

#define d_FONTSIZE 16

/* Card Types */

#define ADMIN_CARD          -1
#define CREDIT_CARD         0
#define DEBIT_CARD          1
//ALL_TOTALS is 2
#define NON_CARD            3
#define LOYALTY_CARD        6  
#define LOYCREDIT_CARD      7
#define EBT_CARD            8
#define PURCHASE_CARD       9 
#define COMMERCIAL_CARD     10
#define FLEET_CARD          11


#define CTOS_RET_OK						0
#define CTOS_RET_PARAM					-100
#define CTOS_RET_CERT_FAILED			-102
#define CTOS_RET_CALC_FAILED			-103
#define CTOS_PED_RSA_MAX				256



/* Debit Allowed    */

#define NODEBIT     0
#define DEBIT       1
											// added 509f12 // fix App label is  not printed on refund chip transaction. If 9F12 is missing terminal should print app label value (Tag 50)

#define GET_EMV_TAG_AFTER_SELECT_APP		"575A5F305F3482845F249F099F085F20509F12"
//#define GET_EMV_TAG_AFTER_1STAC		    "505F2A959A9C9F069F109F129F1A9F1E9F269F279F339F349F359F369F379F419F539F6E" //EMV: Added Tag50 to the list -- jzg 
#define GET_EMV_TAG_AFTER_1STAC		    "505F2A959A9C9F069F109F129F1A9F1E9F269F279F339F349F359F369F379F419F539F6E9F42" //EMV: Added Tag9F42 to the list -- AAA 


#define GET_EMV_TAG_AFTER_2NDAC		    "959F109F269F279F34"

#define PRINT_EMV_TAGS_LIST               "50575F2A5F34828A8F91959A9B9C9F029F039F109F129F1A9F1B9F269F279F339F349F359F369F379F5B7172"

// patrick test code 20141230 start
#ifndef d_EMVCL_RC_SEE_PHONE
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF // AMEX
#endif

#define d_EMVCL_RC_SEE_PHONE2		0xA00000E4
// patrick test code 20141230 end	
//EMV: Added error message handling "PLEASE SEE PHONE" -- mfl
#define d_EMVCL_RC_SEE_PHONE         0xA00000AF

#define CONTACTLESS

//android-removed
//#define UNFORK_APP

void vdSetECRTransactionFlg(BOOL flg);
BOOL fGetECRTransactionFlg(void);

unsigned char WaitKey(short Sec);
unsigned char struiApiGetStringSub(unsigned char	*strDisplay,short x, short y, unsigned char	*strOut, unsigned char	mode, short minlen, short maxlen);

USHORT getCardNO(OUT BYTE *baBuf);
USHORT getExpDate( OUT BYTE *baBuf);
void vdCTOS_FormatAmount(char *szFmt,char* szInAmt,char* szOutAmt);

//USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS);
USHORT shCTOS_GetExpDate(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS);
USHORT ushCTOS_EMV_NewDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
USHORT ushCTOS_EMV_NewTxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue);

void vdCTOS_SyncHostDateTime(void);
void vdCTOS_TxnsBeginInit(void);
void vdCTOS_TransEndReset(void);
void vdCTOS_SetTransEntryMode(BYTE bEntryMode);
void vdCTOS_SetTransType(BYTE bTxnType);
void vdCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len);
void vdCTOS_ResetMagstripCardData(void);
int inCTOS_CheckEMVFallbackTimeAllow(char* szStartTime, char* szEndTime, int inAllowTime);
int inCTOS_CheckEMVFallback(void);
int inCTOS_CheckIssuerEnable(void);
int inCTOS_CheckTranAllowd(void);
int inCTOS_CheckMustSettle(void);
int inCTOS_PreConnect(void);
int inCTOS_GetCardFields(void);
int inCTOS_GetCardFieldsNoEMV(void);
int inCTOS_WaveGetCardFields(void);
int inGetIssuerRecord(int inIssuerNumber) ;
//int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine);
//int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine);
int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine, BOOL fDisplayLogo);
short inCTOS_LoadCDTIndex(void);
int inCTOS_EMVCardReadProcess (void);
int inCTOS_ManualEntryProcess (BYTE *szPAN);
int inCTOS_SelectHost(void) ;
int inCTOS_CheckAndSelectMutipleMID(void);
int inCTOS_GetTxnPassword(void);
int inCTOS_GetCVV2(void);
int inCTOS_getCardCVV2(OUT BYTE *baBuf);
int inCTOS_EMVProcessing(void);
int inCTOS_EMVTCUpload(void);
int inCTOS_CheckTipAllowd(void);
int inCTOS_BootEMVTransInit(void);
void vdCTOS_FormatPANEx(char *szFmt,char* szInPAN,char* szOutPAN, BOOL fMask);
void vdCTOS_FormatPAN(char *szFmt,char* szInPAN,char* szOutPAN);
//int inCTOS_EMV1stProcessing(void);
short shCTOS_EMVAppSelectedProcess(void);
short shCTOS_EMVSecondGenAC(BYTE *szIssuerScript, UINT inIssuerScriptlen);
int inCTOS_GetPubKey(const char *filename, unsigned char *modulus, int *mod_len, unsigned char *exponent, int *exp_len);
short shCTOS_EMVGetChipDataReady(void);
int inCTOS_showEMV_TagLog (void);
int inCTOS_FirstGenACGetAndSaveEMVData(void);
int inCTOS_SecondGenACGetAndSaveEMVData(void);
void vdCTOSS_GetMemoryStatus(char *Funname);
int inCTOSS_CheckMemoryStatus(void);
void vdCTOSS_GetAmt(void);
void vdCTOSS_SetAmt(BYTE *baAmount);

int inCLearTablesStructure(void);
/* EMV: Get Application Label -- jzg */
void vdDispAppLabel(unsigned char *ucHex, int inLen, char *szOutStr);

//void displayAppbmpDataEx(unsigned int x,unsigned int y, const char *filename, BOOL fPublic);
int inCTOS_FraudControl(void);

void vdCTOS_SetDateTime(void);
int inCheckValidApp(int HDTid);
int inGetMerchantPassword(void);
int inGetKeyPressMsg(int key);
void vdCTOSS_ChangeDefaultApplication(void);
int inCTOSS_ForceSettle(void);
int inCTOS_GetCardFieldsFallback(void);

void vdCTOS_Pad_String(char* str,int padlen,char padval,int padtype);

int inCTOS_EnterAppCode(void);
int inCTOS_EnterRRN(void);

int inCTOS_GetCardFieldsCtls(void);
void vdDisplayECRAmount(void);
void vdSaveLastInvoiceNo(void);
void vdClearMSRAndRemoveCard(void);
int inCTOS_ByPassPIN(void);

int inRecoverRAM(void);
int inReforkSubAPP(void);	

int inCTOSS_GetRAMMemorySize(char *Funname);
void vdSetCtlsEntryImage(void);
void vdSetPowerHighSpeed(void);
int isCheckTerminalNonTouch(void);
int inCTOS_PreConnectAndInit(void);
int inCTOS_SelectTelco(void);
void vdGetTimeDate1(TRANS_DATA_TABLE *srTransPara);
void vdGetCRC(void);
void vdCTOSS_PackOfflinepinDisplay(OFFLINEPINDISPLAY_REC *szDisplayRec);
int inCTOS_ERMPreConnect(BOOL fDisplay);
BOOL isERMMode(void);
int inCTOS_DisplayFSRMode(void);
void vdSetFSRMode(BYTE byTransType);
BOOL fFSRMode(void);
void SetTransYear(void);

int inDCCRateRequest(void);
int inDCCDisplayFxRate (char * szDCCTransactionType);


void vdDCCModifyAmount(BYTE *szAmtBuffIn, BYTE *szAmtBuffOut);
void vdDCCFormatAmount(BYTE *szAmtBuff, BYTE *szAmtBuffOut);

int inCTOS_AutoSettlement(void);
void vdSetASParams(void);

void InsertCardUI (void);
int GetMenuTransTypes(void);
int inEditDatabase(void);

void vdUpdatePreCompTotal(void);
void vdGetGlobalVariable(void);

//void vdCTOS_EditHost(void); // sidumili: added for edit host
int vdCTOS_EditHost(void); // sidumili: added for edit host

void vdDeleteFile(char* szFileName);
int inSaveFile(char* szWriteData, int inFileSize, char* szFileName);

int inGetSERNUM(char* szOutValue);

//void vdCTOS_EditIP(void);
int vdCTOS_EditIP(void);
void vdCTOS_EditTerminal(void);
//void vdCTOS_EditPrinter(void);
int vdCTOS_EditPrinter(void);
void vdCTOS_DeleteISOLog(void);

void vdCTOS_UserManual(void); // sidumili - view user manual
void vdCTOS_ReceiptSelect(void);
int vdCTOS_CTMSUPDATE(void);
void vdFormatPANForECR(char* szInPAN, char* szOutPAN);
void vdSetECRResponse(char* szECRResponse);
void vdCTOS_ECRSettings(void);

int inCTOS_SetupMenu(void);
int inCTOS_SetupPw(void);
int inCTOS_ReportMenu(void);

int inDeleteUser(void);
int inDeleteBiller(void);

int inEditUser(void);
int inEditBiller(void);

int inInsertUser(void);
int inInsertBiller(void);

int inCTOS_EMVSetTransType(BYTE byTransType);

#endif //end ___POS_TRANS___

