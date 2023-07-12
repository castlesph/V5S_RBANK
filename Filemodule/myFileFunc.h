/* 
 * File:   dymaniclib.h
 * Author: PeyJiun
 *
 * Created on 2010
 */

#ifndef _MYFILEFUNC__H
#define	_MYFILEFUNC__H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../Includes/POSTypedef.h"
#include "../Includes/Trans.h"

typedef enum
{
	CREDIT_HOST,	
}HOST_INDEX_TABLE;
/*==========================================*
 *              File Setting                *	
 *	            D E F I N E S               *
 *==========================================*/           
#define d_BUFF_SIZE 128  //Buffer Size

typedef enum
{
    RC_FILE_READ_OUT_WRONG_SIZE         = -1,
    RC_FILE_REC_OR_RECSIZE_IS_NULL      = -2,	
    RC_FILE_NOT_EXIST                   = -3,
    RC_FILE_EXIST                       = -4,
    RC_FILE_TYPE_WRONG                  = -5,
    RC_FILE_FILE_NOT_FOUND              = -6,
    RC_FILE_DATA_WRONG                  = -7,
    RC_FILE_READ_OUT_NO_DATA            = -8,
}RESPONSE_CODE_TABLE;
 
 /*==================================================*
 * 	HOST Definition Table(CPT) Struct               *
 *==================================================*/
typedef struct
{
	int	HDTid ;
	BYTE	szHostName[50];	/*  */
	int		inCommunicationMode;
	BYTE	szPriTxnPhoneNumber[30];	/* ASCII string */
	BYTE	szSecTxnPhoneNumber[30];	/* ASCII string */		//5
	
	BYTE	szPriSettlePhoneNumber[30];
	BYTE	szSecSettlePhoneNumber[30];	/* ASCII string */
	BOOL	fFastConnect;          /*  */
	BOOL	fDialMode;	/* ASCII string */ 		//10
	
	int	inDialHandShake;	/* ASCII string */
	BYTE	szATCommand[40];
	int		inMCarrierTimeOut;	/* ASCII string */ 		//10         
        
    int   	inMRespTimeOut;
	BOOL	fPreDial;
	BYTE	szPriTxnHostIP[30];
	BYTE	szSecTxnHostIP[30] ;
	BYTE	szPriSettlementHostIP[30];									//15
	
	BYTE	szSecSettlementHostIP[30];
	int	inPriTxnHostPortNum;
	int	inSecTxnHostPortNum;
	int	inPriSettlementHostPort;
	int	inSecSettlementHostPort;									//20
	
	BOOL	fTCPFallbackDial;
	BOOL	fSSLEnable;
	int	inTCPConnectTimeout;
	int	inTCPResponseTimeout;
	int	inIPHeader;									//25
	int inCountryCode;
	int inHandShake;
	int inParaMode;
	
	BOOL fCommBackUpMode;
	int inCommunicationBackUpMode;	

	BYTE	szPriTxnHostIPGPRS[30];
	BYTE	szSecTxnHostIPGPRS[30] ;
	BYTE	szPriSettlementHostIPGPRS[30];									//15
	
	BYTE	szSecSettlementHostIPGPRS[30];
	int	inPriTxnHostPortNumGPRS;
	int	inSecTxnHostPortNumGPRS;
	int	inPriSettlementHostPortGPRS;
	int	inSecSettlementHostPortGPRS;				
} STRUCT_CPT;
 

/*=======================================*
 *          File Setting Struct			 *
 *=======================================*/

typedef struct
{	
	ULONG   ulHandle;
	int     inSeekCnt;		
	BYTE	bSeekType;
	char	szFileName[d_BUFF_SIZE];	
	BYTE	bStorageType;	
	
	/*be cardful for the parameter
	  if value was 'TRUE', means that File Would be closed immediately after File Opened
	  if value was 'False', user must be Close File by self*/
	BOOL    fCloseFileNow;
	
	ULONG   ulRecSize;
	void    *ptrRec;

}STRUCT_FILE_SETTING;

/*=======================================*
 *          File Setting Func			 *
 *=======================================*/
int inMyFile_RecSave(void *);
int inMyFile_RecRead(void *);

/*-------------------------File Setting End----------------------------*/





/*==========================================*
 *                  TCT                     *                             
 *          (Terminal Config Table)         *	
 *		        D E F I N E S               *
 *==========================================*/  
#define PWD_LEN 	6
#define DEBUG_LOG_OFF   0
#define DEBUG_LOG_ON    1
#define DEBUG_IGNORE    2

#define IDLE_CREDIT_SALE 1
#define IDLE_DEBIT_SALE 2
#define IDLE_INSTALLMENT 3
#define IDLE_LOYALTY 4
#define IDLE_BANCNET 5
#define IDLE_CREDIT_AND_BANCNET 6
#define IDLE_RBANK_OFFUS 7

/*==========================================*
 *                  TCT Struct              *
 *==========================================*/
typedef struct
{
    
    BYTE	inMainLine; 	 // Dial, RS486 or TCP/IP
    BOOL	fHandsetPresent; // ????
    
    BOOL    fDemo;
    BOOL    fCVVEnable;
    
    BYTE    szSuperPW[PWD_LEN+1];
    BYTE    szSystemPW[PWD_LEN+1];
    BYTE    szEngineerPW[PWD_LEN+1];
    BYTE    szPMpassword[8+1];
    BYTE    szFunKeyPW[8+1];
    BYTE    szBankVerNo[32];    		//30
    
    BYTE    szMyVerDate[32];		
    BYTE    szMyVerName[32];
    
    BYTE	byTerminalType;// 1 for V5S, 2 for V3
    BYTE    byRS232DebugPort;//0 not debug, 8= USB debug, 1 =COM1 debug, 2=COM2 debug
    BYTE	byPinPadType;//0 None, 1 for PCI100, 2 for Reserved
    BYTE    byPinPadPort;//0 None, 1 =COM1 , 2=COM2,3= USB , 
    USHORT	inPrinterHeatLevel;
    UINT	inWaitTime;
    BOOL	fTipAllowFlag;
    BOOL	fSignatureFlag;
    BYTE    byCtlsMode;
    BYTE    byERMMode;// 2-ERM2, 3=ERM3
    
    BOOL	fDebitFlag;
    BYTE    szFallbackTime[20];
    USHORT  inFallbackTimeGap;
    BOOL    fManualEntryInv ;   
    BYTE	szInvoiceNo[INVOICE_BCD_SIZE+1];
    BYTE	szPabx[4+1];
    BYTE	szLastInvoiceNo[INVOICE_BCD_SIZE+1];
    BOOL    fECR;
    BOOL    fDebugOpenedFlag;
    BOOL    fShareComEnable;
    USHORT  inReversalCount;
    USHORT  inTipAdjustCount;
    BOOL fPrintISOMessage;
    
    BOOL    fNSR;
    BOOL    fNSRCustCopy;
    BYTE	szNSRLimit[12+1];
    BYTE	szCTLSLimit[12+1];
    
    USHORT	usTMSGap;
    USHORT	inTMSComMode;
    USHORT	usTMSRemotePort;
    BYTE    szTMSRemoteIP[30+1];
    BYTE    szTMSRemotePhone[30+1];
    BYTE    szTMSRemoteID[32+1];
    BYTE    szTMSRemotePW[32+1];
    BOOL    fFirstInit; // patrick first Init
    
    BOOL	fAutoLogon;	
    BYTE    szLogonDate[DATE_BCD_SIZE+1];     /* BCD MMDD        */
    int     inLogonHostIndex;
    BOOL	fProductCode; 
    BOOL	fCustomerNo; 
    int  inMenuid;
    BOOL fPrintMerchCopy;
    BOOL fPrintBankCopy;
    BOOL fRegUSD;
    BOOL fAutoDebitLogon;
    BYTE szMaxTrxnAmt[15]; //mfl: Issue#:000076 [OUT OF RANGE]	
    BOOL fConfirmPAN; // mfl
    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime -- jzg */
    char szMaxCTLSAmount[AMT_ASC_SIZE + 1];	
    BOOL fTSNEnable;
    BYTE szMinInstAmt[12]; //mfl: installment minimum amount
    BOOL fManualEntry;
    int inFallbackCounter; /*start - 00116*/
    USHORT inMenuidOldHost;
    USHORT inMenuidNewHost;	
    USHORT inMenuidNHProfileMenu;
    BOOL fRegister;
    int inTipAdjCount;
    BYTE szOfflineCeilingAmt[AMT_BCD_SIZE+1];

    BYTE byIdleSaleType;/*Swipe idle Sale - 0 None, 1 =Credit Sale , 2=Debit, 3= Installment*/ 
    BOOL fIdleSwipeAllow; /*allow swipe on idle*/
    BOOL fIdleInsertAllow; /*allow insert on idle*/	
    BYTE byRS232ECRPort;//0=disabled, 8= USB debug, 1 =COM1 debug, 2=COM2 debug
    BOOL fECRSale;
    BOOL fECRDebit;
    BOOL fECRDiners;
    BOOL fECRBancnet;
	BOOL fECRQRPay;
	BOOL fECRQRPh;
    BOOL fPrintReceipt;
    BOOL fEMVOnlinePIN;
    BOOL fSettleAutoDL;	
    BYTE byPinPadMode;
    BYTE szPrintOption[3+1];
    BYTE byRS232ECRBaud;
    UINT inERMMaximum;
    BYTE byERMInit;
    BOOL fDebitEMVCapable;
    INT inPrintISOOption;
    INT inMinBattery;
    BOOL fPrintSettleDetailReport;
    BYTE szIdleLogo1[36+1];
    BYTE szIdleLogo2[36+1];
    BYTE szIdleLogo3[36+1];
    BYTE byTerminalModel;
    BOOL fTPSec;
    BOOL fImageAnimated;	
    char szUnitModel[20+1];
    char szUpdateReason[20+1]; 
    BYTE szMerchantPW[PWD_LEN+1];
    BOOL fDCCTerminal;
    BYTE szCTLSDCCMinAmt[AMT_ASC_SIZE + 1];
    INT  inPrintDCCChooseCurrency;	
	BYTE szMinTrxnAmt[15]; // transaction minimum amount -- sidumili
    BYTE szAppVersion[20];
    INT inMMTid;
	BOOL feReceiptEnable;
	BOOL    fOfflinePinbypass;
	INT inGPRSFallback;
	BOOL fTLESec;
	int inIdleMode;	
	BYTE	byDLNotFinished;
	BYTE    byCTMSCallAfterSettle;
	INT inISOLogger;
	INT inISOTxnLogLimit;
	INT inISOTxnLogCnt;
	BOOL byTMSParaDL;
	BOOL fECRFormatPan;
	BOOL fECRMaskExpiryDate;
	BYTE szUserName[12];
	BOOL fSwipeEnable;
	BOOL fCTLSEnable;
	BOOL fFalllbackEnable;
} STRUCT_TCT ;

/*-------------------------TCT End----------------------------*/




/*==========================================*
 *                  CDT                     *
 *          (Card Definition Table)         *
 *              D E F I N E S               *
 *==========================================*/ 
#define	DF_CDT_FILE_NAME_TXT	"CDT.txt"
#define	DF_CDT_FILE_NAME_DAT	"CDT.dat"

/*=======================================*
 * Card Definition Table(CDT) Struct     *
 *=======================================*/
typedef struct
{
    char    szPANLo[19];
	char    szPANHi[19];
	BYTE	szCardLabel[12];
	int  	inType;
        int     inMinPANDigit;
	int     inMaxPANDigit;
	int     inCVV_II;
	long    InFloorLimitAmount;
	BOOL    fExpDtReqd;
	BOOL    fPinpadRequired;
	BOOL    fManEntry;
	BOOL    fCardPresent;
	BOOL    fChkServiceCode;	
	BOOL	fluhnCheck ;//luhn Check
	BOOL	fCDTEnable ;//Card Type enable?
    int     IITid;
	int     HDTid;
	int			CDTid;
	BOOL 	fInstallmentEnable;
	int 	inInstGroup;	
	int 	inLoyaltyGroup;	
	BOOL    fBancnet;
	BOOL fDCCEnable;
	int  inDualCardIITid;
	BOOL fOnUs;
} STRUCT_CDT ;/*Card Definition Table*/

/*-------------------------CDT End----------------------------*/




/*=======================================*
 * Card Definition Table(CST) Struct     *
 *=======================================*/
typedef struct
{
	int	inCurrencyIndex;
	char	szCurSymbol[10];
         char	szCurCode[10];	
	int    inMinorUnit;
	char szAmountFormat[18+1];
} STRUCT_CST ;/*Card Definition Table*/

/*=======================================*
 * Card Definition Table(PCT) Struct     *
 *=======================================*/
typedef struct
{
	char	szRctHdr1[50];
	char	szRctHdr2[50];
	char	szRctHdr3[50];
    char	szRctHdr4[50];
	char	szRctHdr5[50];
	char	szRctFoot1[50];
	char	szRctFoot2[50];
	char	szRctFoot3[50];
	int	inPrinterType;
	int	inPrintOption;
	int	inCustCopyOption;
	BOOL	fPrintDisclaimer;// expired Check
	BOOL	fEnablePrinter;
	
} STRUCT_PCT ;/*Card Definition Table*/





/*==========================================*
 *                  EMVT                    *	
 *              D E F I N E S               *
 *==========================================*/ 
          
/*==========================================*
 *                  EMNT Struct             *
 *==========================================*/
typedef struct
{
	short	inSchemeReference;
	short	inIssuerReference;
	short	inTRMDataPresent;
	unsigned long	lnEMVFloorLimit;
	unsigned long	lnEMVRSThreshold;
	short	inEMVTargetRSPercent;
	short	inEMVMaxTargetRSPercent;

	// Two new fields are added for APR ID:3,5
	short	inMerchantForcedOnlineFlag;
	short	inBlackListedCardSupportFlag;

	unsigned char	szEMVTACDefault[EMV_TAC_SIZE];
	unsigned char	szEMVTACDenial[EMV_TAC_SIZE];
	unsigned char	szEMVTACOnline[EMV_TAC_SIZE];
	unsigned char	szDefaultTDOL[EMV_MAX_TDOL_SIZE];
	unsigned char	szDefaultDDOL[EMV_MAX_DDOL_SIZE];
	short	inEMVFallbackAllowed;
	short	inNextRecord;
	unsigned long    ulEMVCounter;
	short	inEMVAutoSelectAppln;
	unsigned char	szEMVTermCountryCode[EMV_COUNTRY_CODE_SIZE];
	unsigned char	szEMVTermCurrencyCode[EMV_CURRENCY_CODE_SIZE];
	short	inEMVTermCurExp;
	unsigned char	szEMVTermCapabilities[EMV_TERM_CAPABILITIES_BCD_SIZE];
	unsigned char	szEMVTermAddCapabilities[EMV_ADD_TERM_CAPABILITIES_BCD_SIZE];
	unsigned char	szEMVTermType[EMV_TERM_TYPE_SIZE];
	unsigned char	szEMVMerchantCategoryCode[EMV_MERCH_CAT_CODE_SIZE];
	unsigned char	szEMVTerminalCategoryCode[EMV_TERM_CAT_CODE_SIZE];
	short	inModifyCandListFlag;        
	short	shRFU1;
	short	shRFU2;
	short	shRFU3;
	unsigned char	szRFU1[EMV_STRING_SIZE];
	unsigned char	szRFU2[EMV_STRING_SIZE];
	unsigned char	szRFU3[EMV_STRING_SIZE];
} STRUCT_EMVT;
 



/*==================================================*
 *                      HDT                         *
 *              (HOST Definition Table)             *
 *                  D E F I N E S                   *
 *==================================================*/
#define IP_LEN			16


/*==================================================*
 * 	HOST Definition Table(HDT) Struct               *
 *==================================================*/
typedef struct
{
	BYTE	szHostLabel[16] ;
    USHORT  inHostIndex;

	BYTE	szTPDU[5+1];
	BYTE	szNII[NII_BYTES+1] ;
	BOOL	fReversalEnable;
	BOOL	fHostEnable;
	BYTE	szTraceNo[TRACE_NO_BCD_SIZE+1];
    BOOL    fSignOn ;
    ULONG   ulLastTransSavedIndex;
    USHORT inCurrencyIdx;
	char	szAPName[50];
    USHORT inFailedREV;
	USHORT inDeleteREV;
    USHORT inNumAdv;       
	ULONG ulCRC;      
	BYTE    szMinInstAmt[AMT_BCD_SIZE+1];
	BOOL    fFooterLogo; 
	BOOL    fLast4Digit;
	char	szFooterLogoName[25];
	char	szHeaderLogoName[25];
	BOOL    fMaskDetail;
    BOOL fTipAllowFlag;	
    ULONG ulTipPercent;
	BOOL fHostTPSec;
	BOOL fHostTLESec;
	
	
} STRUCT_HDT;
 

/*==================================================*
 *                      MMT                         *
 *              (MMT Definition Table)             *
 *                  D E F I N E S                   *
 *==================================================*/

/*==================================================*
 * 	MMT Definition Table(MMT) Struct               *
 *==================================================*/
typedef struct
{
    UINT    MMTid;
	BYTE	szHostName[50] ;
    UINT    HDTid;
    BYTE	szMerchantName[50] ;
    UINT    MITid;
	BOOL	fMMTEnable ;
    BYTE	szTID[10] ;
    BYTE	szMID[20] ;
    BYTE	szBatchNo[BATCH_NO_BCD_SIZE+1] ;
	BYTE	szATCMD1[50] ;
	BYTE	szATCMD2[50] ;
	BYTE	szATCMD3[50] ;
	BYTE	szATCMD4[50] ;
	BYTE	szATCMD5[50] ;	
    BOOL    fMustSettFlag ;
	BYTE	szRctHdr1[50];
	BYTE	szRctHdr2[50];
	BYTE	szRctHdr3[50];
    BYTE	szRctHdr4[50];
	BYTE	szRctHdr5[50];
	BYTE	szRctFoot1[50];
	BYTE	szRctFoot2[50];
	BYTE	szRctFoot3[50];
	BYTE	szCurrency[3];
	BOOL	fPasswordEnable;
	BYTE	szPassword[6];
	UINT    inFailedREV;
	BOOL	fBatchNotEmpty;

} STRUCT_MMT;






/*==================================================*
 *                      PIT                         *
 *              (PIT Definition Table)             *
 *                  D E F I N E S                   *
 *==================================================*/

/*==================================================*
 * 	PIT Definition Table(PIT) Struct               *
 *==================================================*/
typedef struct
{
	BYTE	szTransactionTypeName[20] ;
    UINT    inTxnTypeID;
    BOOL	fTxnEnable ;
    UINT    inPasswordLevel;
    
} STRUCT_PIT;



 
/*==================================================*
 *                      TCP                         *
 *              (TCP Definition Table)             *
 *                  D E F I N E S                   *
 *==================================================*/

/*==================================================*
 * 	TCP Definition Table(TCP) Struct               *
 *==================================================*/
typedef struct
{
	BYTE	szTerminalIP[30] ;
    BYTE	szGetWay[30] ;
    BYTE	szSubNetMask[30] ;
    BYTE	szHostDNS1[30] ;
    BYTE	szHostDNS2[30] ;
    BOOL	fDHCPEnable ;
    BYTE	szAPN[50] ;
    BYTE	szUserName[30] ;
    BYTE	szPassword[30] ;
	BYTE	szWifiSSID[100] ;
		BYTE	szWifiPassword[30] ;
		BYTE	szWifiProtocal[8] ;
		BYTE	szWifiPairwise[8] ;
		BYTE	szWifiGroup[8] ;
		USHORT inSIMSlot;

} STRUCT_TCP;


/*==================================================*
 *                      AIDT
 *                  (AID Table)
 *                  D E F I N E S
 *==================================================*/

typedef struct 
{
	ULONG   ulRef;
	BYTE    bAIDLen;
	BYTE    pbAID[16];
	BYTE    bApplication_Selection_Indicator;
	int		inEMVid;
	int		inCAPKindex1;
	BYTE    pbExpireDate1[6+1];
	int		inCAPKindex2;
	BYTE    pbExpireDate2[6+1];
	int		inCAPKindex3;
	BYTE    pbExpireDate3[6+1];
	int		inCAPKindex4;
	BYTE    pbExpireDate4[6+1];
	int		inCAPKindex5;
	BYTE    pbExpireDate5[6+1];
	int		inCAPKindex6;
	BYTE    pbExpireDate6[6+1];
	int		inCAPKindex7;
	BYTE    pbExpireDate7[6+1];
	int		inCAPKindex8;
	BYTE    pbExpireDate8[6+1];
	int		inCAPKindex9;
	BYTE    pbExpireDate9[6+1];
	int		inCAPKindex10;
	BYTE    pbExpireDate10[6+1];
	BYTE	pbTerminalAVN[2+1];
	BYTE	pb2ndTerminalAVN[2+1];
	
} STRUCT_AIDT;


/*==================================================*
 *                  Advice Struct
 *                  D E F I N E S
 *==================================================*/
#define AUTO_GET_INVOICE     -1

typedef struct 
{
	BYTE    szInvoiceNo[INVOICE_BCD_SIZE+1];
	ULONG   ulBatchIndex;//index of batch Rec. stored in xxxx000001.bat File
	BYTE    byTransType;//sam
} STRUCT_ADVICE;
/*==================================================*
 *                  Advice Func                       
 *==================================================*/
int inMyFile_AdviceRead(int *inSeekCnt, STRUCT_ADVICE *strAdvice, TRANS_DATA_TABLE *transData);
int inMyFile_AdviceReadByIndex(int inSeekCnt, STRUCT_ADVICE *strAdvice, TRANS_DATA_TABLE *transData);
int inMyFile_AdviceUpdate(int inSeekCnt);
int inMyFile_AdviceSave(TRANS_DATA_TABLE *transData, BYTE byTransType);

int inMyFile_TCUploadFileSave(TRANS_DATA_TABLE *transData, BYTE byTransType);
int inMyFile_TCUploadFileUpdate(int inSeekCnt);
int inMyFile_TCUploadFileRead(int *inSeekCnt, STRUCT_ADVICE *strAdvice, TRANS_DATA_TABLE *transData);
int inMyFile_TCUploadDelete(void);


STRUCT_AIDT strAIDT;


#define PASSWORD_LEN     6
#define DATE_LEN	 5
#define d_BUFF1_SIZE   576





/*==================================================*
 *                  Message Struct
 *                  D E F I N E S
 *==================================================*/

typedef struct 
{	
	USHORT   usMsgIndex;
	BYTE     szMsg[128+1];
	char     szResponseCode[4];
	int      inHostIndex;
	char     szHostName[30];
} STRUCT_MSG;


/*==================================================*
 *                  TLE Struct
 *                  D E F I N E S
 *==================================================*/

typedef struct 
{	
    BYTE   szVERSION[2] ;//"\x00\x03"
    BYTE   szTMKRefNum[8+1];//"12345678"
    BYTE   szZMK[16];	//"\x11\x22"....
    BYTE   szTMK[16];//"\x11\x22"....
    BYTE   szTAK[16];//for sign on mac key
    BYTE   szTermPinEncryuptKey[16];	//"\x11\x22"....
    BYTE   szMACKey[16];	//"\x11\x22"....
    BYTE   szLineEncryptKey[16];	//"\x11\x22"....
} STRUCT_TLE;

STRUCT_TLE   stTLE;

/*=======================================*
 * Card Definition Table(IIT) Struct     *
 *=======================================*/
typedef struct
{
	int		inIssuerNumber;
	char	szIssuerAbbrev[20];
	char	szIssuerLabel[20];
	char szIssuerLogo[20]; //Display Issuer logo: issuer logo filename -- jzg
    char	szPANFormat[30];
	char	szMaskMerchantCopy[30];
	char	szMaskCustomerCopy[30];
	char	szMaskExpireDate[20];
	char	szMaskDisplay[30];	
	BOOL	fMerchExpDate ;// expired Check
	BOOL	fCustExpDate ;
	BOOL	fMerchPANFormat ;//Card Type enable?
	int     inCheckHost;
    ULONG   ulTransSeqCounter;
	BOOL    fMaskCustCopy;
	BOOL    fMaskMerchCopy;
	BOOL    fMaskPan;
	int     inMaskDigit;
	BOOL    fCardPIN; 
	
	BOOL    fNSR;
	BYTE	szNSRLimit[12+1];
	BOOL    fNSRCustCopy;
	BYTE	szCVMLimit[12+1];
	BOOL	fBit45ONSwipe; // bitmap 45 swipe
	BOOL	fTag20; // Deferred Authorization Indicator
} STRUCT_IIT ;/*Card Definition Table*/


typedef struct
{
    int      inCommMode;
    int      inHeaderFormat; // 1 is BCD exclude header len, 2 is HEX exclude header len, 3 is BCD include header len, 4 is HEX include header len

    BOOL     bSSLFlag;   
    BYTE     szCAFileName[30];
    BYTE     szClientFileName[30];
    BYTE     szClientKeyFileName[30];
        
    BYTE     szPriPhoneNum[30];
    BYTE     szSecPhoneNum[30];
    
    BYTE     szPriHostIP[50];
    ULONG    ulPriHostPort;
    BYTE     szSecHostIP[50];
    ULONG    ulSecHostPort;
    
    int      inSendLen;
    //BYTE     szSendData[2048];//change DB to txt file
    int      inReceiveLen;
    //BYTE     szReceiveData[2048];//change DB to txt file

	BOOL     bDialBackUp;
    BOOL     bPredialFlag;  
    int      inParaMode;
    int      inHandShake;
    int      inCountryCode;
	BYTE	szATCMD1[50];
	BYTE	szATCMD2[50];
	BYTE	szATCMD3[50];
	BYTE	szATCMD4[50];
	BYTE	szATCMD5[50];
    int      inConnectionTimeOut;
    int      inReceiveTimeOut;
    int      inGPRSSingal;
       
} STRUCT_COM;

typedef struct
{
	BYTE	byEnableExt;	//Enable COM ext data
	BYTE	bySSLVerCtrl;	// for SSL Version control , 20171012
	BYTE	byReserve01;
	BYTE	byReserve02;
	BYTE	byReserve03;

	
	int		inReserve01;
	int		inReserve02;
	int		inReserve03;

	BYTE 	szReserve01[64];
	BYTE 	szReserve02[64];
	BYTE 	szReserve03[128];

} S_COM_EXT;


typedef struct 
{
	ULONG   ulRef;
	int		fAIDEnable;
	int     inAIDLen;
	BYTE    pbAID[16];
	int     inApplication_Selection_Indicator;
	int		EMVid;
	int		inCAPKindex1;
	BYTE    pbExpireDate1[6+1];
	BYTE	pbCAPKExponent1[9+1];
	int		inCAPKindex2;
	BYTE    pbExpireDate2[6+1];
	BYTE	pbCAPKExponent2[9+1];
	int		inCAPKindex3;
	BYTE    pbExpireDate3[6+1];
	BYTE	pbCAPKExponent3[9+1];
	int		inCAPKindex4;
	BYTE    pbExpireDate4[6+1];
	BYTE	pbCAPKExponent4[9+1];
	int		inCAPKindex5;
	BYTE    pbExpireDate5[6+1];
	BYTE	pbCAPKExponent5[9+1];
	int		inCAPKindex6;
	BYTE    pbExpireDate6[6+1];
	BYTE	pbCAPKExponent6[9+1];
	int		inCAPKindex7;
	BYTE    pbExpireDate7[6+1];
	BYTE	pbCAPKExponent7[9+1];
	int		inCAPKindex8;
	BYTE    pbExpireDate8[6+1];
	BYTE	pbCAPKExponent8[9+1];
	int		inCAPKindex9;
	BYTE    pbExpireDate9[6+1];
	BYTE	pbCAPKExponent9[9+1];
	int		inCAPKindex10;
	BYTE    pbExpireDate10[6+1];
	BYTE	pbCAPKExponent10[9+1];
	BYTE	pbTerminalAVN[2+1];
	BYTE	pb2ndTerminalAVN[2+1];
	
} STRUCT_WAVE_AIDT;

typedef struct
{	 
	 char szAmount[12+1];//szAmount    Tag 9F02   (Amount Authorized(Numeric))
	 char szOtherAmt[12+1];//szOtherAmt  Tag 9F03	(Amount Other(Numeric))
	 char szTransType[2+1];//szTransType Tag 9C 	(Transaction Type)
	 char szCatgCode[4+1];//szCatgCode	Tag 9F53   (Transaction Category Code)
	 char szCurrCode[4+1];//szCurrCode	Tag 5F2A   (Transaction Currency Code)
	 int bTagNum;
	 int usTransactionDataLen;
	 BYTE pbaTransactionData[100+1];
} CTLS_Trans;

typedef struct
{
	unsigned char	szASI[10+1];
	unsigned char	szSchemeID[20+1];
	int szActivate;
	unsigned char	szTerminalType[10+1];	//9F35
	unsigned char	szEMVTermCapabilities[EMV_TERM_CAPABILITIES_BCD_SIZE];//9F33
	unsigned char	szEMVTermAddCapabilities[EMV_ADD_TERM_CAPABILITIES_BCD_SIZE];//9F40
	unsigned char	szAppVerNo[3];//9F09
	unsigned char	szEMVTermCountryCode[EMV_COUNTRY_CODE_SIZE];//9F1A
	unsigned char	szEMVTermCurrencyCode[EMV_CURRENCY_CODE_SIZE];//5F2A
	unsigned char	szTransactionType[2];//9C
	unsigned char	lnEMVFloorLimit[5];//9F1B
	unsigned char	szVisaWaveVLPIndicator[2];//9F7A
	unsigned char	szVisaWaveTTQ[5];//9F66
	unsigned char	szDefaultTDOL[EMV_MAX_TDOL_SIZE];//DF8F40
	unsigned char	szDefaultDDOL[EMV_MAX_DDOL_SIZE];//DF8F41
	unsigned char	inEMVTargetRSPercent[2];//DF8F42
	unsigned char	inEMVMaxTargetRSPercent[2];//DF8F43
	unsigned char	lnEMVRSThreshold[5];//DF8F44
	unsigned char	szEMVTACDefault[EMV_TAC_SIZE];//DF8121
	unsigned char	szEMVTACDenial[EMV_TAC_SIZE];
	unsigned char	szEMVTACOnline[EMV_TAC_SIZE];//DF8122
	unsigned char	szUploadDOL[16];//DF8F49
	unsigned char	szAEPara[7];//DF51
	unsigned char	szTransactionResult[2];//DF8F4F
	unsigned char	szVisMSDCVN17Enable[2];//DF06
	unsigned char	szTerminalIdentification[9];//9F1C
	unsigned char	szIFDSerialNumber[9];//9F1E
	unsigned char	szMerchantIdentifier[16];//9F16
	unsigned char	inVisaMSDTrack1Enable[2];//DF21
	unsigned char	inVisaMSDTrack2Enable[2];//DF22
	unsigned char	inDisplayOfflineIndicator[2];//DF05
	unsigned char	szStatusCheck[2];//DF25
	unsigned char	szAmount0Check[2];//DF8F4B
	unsigned char	szCLTranslimit[7];//DF00
	unsigned char	szCLCVMlimit[7];//DF01
	unsigned char	szCLFloorlimit[7];//DF02
	unsigned char	szCVN17UploadDOL[70];//DF8F4C
	unsigned char	szAdditionalDataUploadDOL[7];//DF8F4E
} STRUCT_WAVE_EMVT;
 
typedef struct
{
	short shType;	
	
	char szTPDU[10+1];									// TPDU
	char szPAN[19+1];										// 02
	char szAmt[12+1];										// 04
	char szSTAN[6+1];									// 11
	char szTime[6+1];									// 12
	char szDate[8+1];									// 13
	char szExpDate[4+1];							// 14
	char szRefNum[12+1];								// 37
	char szApprCode[6+1];							// 38
	char szTID[8+1];										// 41
	char szMID[15+1];										// 42
	char szTerminalSerialNO[17+1];			// 46
	char szBankCode[22+1];							// 55
	char szMerchantCode[22+1];					// 56
	char szStoreCode[22+1];						// 57
	char szPaymentType[20+1];					// 58
	char szPaymentMedia[20+1];					// 59
	char szBatchNum[6+1];							// 60
	char szReceiptImageFileName[19+1];					// 61   store image for txn, init will not use this buf
	char szInvNum[6+1];									// 62
	char szLogoFileName[19+1];						    // 63   store Logo for init, store INV for txn

	char szSingatureStatus[2+1];									// signature status
	char szRSAKEKVersion[4+1];									// KEK version
	char szReceiptLogoIndex[2+1];								// receipt logo index
	
	VS_BOOL 	fGzipReceiptImage;								// 0 - the Receipt Image havn't zip,  1 - the Receipt Image zip already
	VS_BOOL		fSettle;											// 0 - transaction receipt, 1 - settlement receipt
	
	VS_BOOL		fReserved1;
	VS_BOOL		fReserved2;
	VS_BOOL		fReserved3;

	char 		szReserved1[65];								
	char 		szReserved2[65];
	char 		szReserved3[65];

	
} ERM_TransData;

typedef struct
{	 
	 char szAmount[12+1];//szAmount    Tag 9F02   (Amount Authorized(Numeric))
	 char szOtherAmt[12+1];//szOtherAmt  Tag 9F03	(Amount Other(Numeric))
	 char szTransType[2+1];//szTransType Tag 9C 	(Transaction Type)
	 char szCatgCode[4+1];//szCatgCode	Tag 9F53   (Transaction Category Code)
	 char szCurrCode[4+1];//szCurrCode	Tag 5F2A   (Transaction Currency Code)
 
	 int status;
	 BYTE		 bSID;				 //Scheme Identifier
	 BYTE		 baDateTime[15];	 //YYYYMMDDHHMMSS format
	 BYTE		 bTrack1Len;
	 BYTE		 baTrack1Data[100];  //ANS 
	 BYTE		 bTrack2Len;
	 BYTE		 baTrack2Data[100];  //ASCII
	 USHORT 	 usChipDataLen; 	 //Chip Data
	 BYTE		 baChipData[1024];
	 USHORT 	 usAdditionalDataLen;	 //Additional Data
	 BYTE		 baAdditionalData[1024];

	 USHORT	usTransResult;		// Transction Result
	 BYTE	bCVMAnalysis;		//
	 BYTE	baCVMResults[3];	//CVM Result
	 BYTE	bVisaAOSAPresent;	//Visa Available Offline Spending Amount(AOSA) Present
	 BYTE	baVisaAOSA[6];
	 BOOL	bODAFail;			//Offline Data Authentication Fail , ODA FAIL -> TRUE, ODA OK --> FALSE
	
	 int	 inReserved1;
	 int	 inReserved2;
	 int	 inReserved3;
 
	 char		 szReserved1[100];								 
	 char		 szReserved2[100];
	 char		 szReserved3[100];
 
 } CTLS_TransData;

// ERM TCT
typedef struct
{
	short shType;		
	char szTPDU[10+1];									// TPDU
	char szPAN[19+1];										// 02
	char szAmt[12+1];										// 04
	char szSTAN[6+1];									// 11
	char szTime[6+1];									// 12
	char szDate[8+1];									// 13
	char szExpDate[4+1];							// 14
	char szRefNum[12+1];								// 37
	char szApprCode[6+1];							// 38
	char szTID[8+1];										// 41
	char szMID[15+1];										// 42
	char szTerminalSerialNO[17+1];			// 46
	char szBankCode[22+1];							// 55
	char szMerchantCode[22+1];					// 56
	char szStoreCode[22+1];						// 57
	char szPaymentType[20+1];					// 58
	char szPaymentMedia[20+1];					// 59
	char szBatchNum[6+1];							// 60
	char szReceiptImageFileName[19+1];					// 61   store image for txn, init will not use this buf
	char szInvNum[6+1];									// 62
	char szLogoFileName[19+1];						    // 63   store Logo for init, store INV for txn

	char szSingatureStatus[2+1];									// signature status
	char szRSAKEKVersion[4+1];									// KEK version
	char szReceiptLogoIndex[2+1];								// receipt logo index
	
	VS_BOOL 	fGzipReceiptImage;								// 0 - the Receipt Image havn't zip,  1 - the Receipt Image zip already
	VS_BOOL		fSettle;											// 0 - transaction receipt, 1 - settlement receipt
	
	VS_BOOL		fReserved1;
	VS_BOOL		fReserved2;
	VS_BOOL		fReserved3;

	char 		szReserved1[65];								
	char 		szReserved2[65];
	char 		szReserved3[65];

	// For ERM -- sidumili
	int ERMTctid;
	int inCommMode;
	BYTE szPriTxnHostIP[30];
	BYTE szSecTxnHostIP[30] ;
	int	inPriTxnHostPortNum;
	int	inSecTxnHostPortNum;
	
} STRUCT_ERMTCT;

typedef struct
{
	BYTE szButtonItemLabel[110];
	BYTE szButtonItemFunc[110];
	BYTE szSubMenuName[110];
} STRUCT_DYNAMIC_MENU;

STRUCT_DYNAMIC_MENU strDynamicMenu[100];

/*==================================================*
 *                  TMS Struct
 *                  D E F I N E S
 *==================================================*/

typedef struct
{
	BYTE szTMSNACProtocol[2+1];
	BYTE szTMSNACSourceAddr[4+1];
	BYTE szTMSNACDestAddr[4+1];
	
} STRUCT_TMS;

STRUCT_TMS strTMS;

 /*==================================================*
  * 				 Auto Settle Struct
  * 				 D E F I N E S
  *==================================================*/
 
 typedef struct
 { 
	 int inASType;
	 int inASRetryLimit;
	 int inASRetryCounter;
	 int inASGap;
	 BYTE szASDate[8+1];
	 int inASTime;
	 int inASNextTime;
	 BYTE szASTime[4 + 1];
	 int inASPerformed;
	 int inASDate;
	 
	 
 }STRUCT_AST;

 /*==================================================*
  * 				 FXT Struct
  * 				 D E F I N E S
  *==================================================*/


 typedef struct
{
	UINT 	FXTid;
	BYTE	szFXTTPDU[5+1];
	BYTE	szFXTNII[4+1];
	BYTE	szFXTTID[10];
	BYTE	szFXTMID[20];
    BYTE    szDCCAuthTPDU[5+1];
	BYTE    szDCCAuthNII[4+1];
	BYTE    szACQID[10];

}STRUCT_FXT;

typedef struct
{
	USHORT  inHostindex;
	BYTE	szHostLabel[16];    
	BYTE	szTPDU[5+1];
	BYTE	szNII[NII_BYTES+1] ;
	BYTE	szTID[10] ;
	BYTE 	szMACKey[32 + 1];	
	BYTE 	szDataKey[32 + 1];
	
} STRUCT_TLESEC;

// sidumili: For global variable
typedef struct
{
	BOOL fGBLvConfirmPrint;
	BOOL fGBLvDCCDev;
	int inGBLvAppType;
	BOOL fGBLApprovedDisplay;
	BYTE szGBLvProcessType[2 + 1];
	BYTE szGBLvProcessDesc[10 + 1]; 
	BYTE szGBLvSelectedValue[500 + 1];

} STRUCT_GBLVar;

typedef struct
{
	BYTE szWriteData[6000 + 1];
	BYTE szWriteTemp[1000 + 1];
	BYTE szTemp[100 + 1];
	int inWriteSize;

} STRUCT_WRITEFILE;


/*==================================================*
 * 	TMSEX Definition Table(TMSEX) Struct               *
 *==================================================*/
typedef struct
{
	BYTE	szTMSID[8+1] ;
	BYTE	szSerialNo[24+1] ;
	BYTE	szGroupName[999+1] ;
	BYTE	szTMSHostIP[30+1] ;
	int 	inTMSHostPortNum;
	BYTE	szUserName[30+1] ;
	BYTE	szPassword[30+1] ;
	BYTE	szTLSHostIP[30+1] ;
	int 	inTLSHostPortNum;
	BYTE	szTMSMode[2+1] ;
	BOOL    fTMSEnabled;
	
	
	
} STRUCT_TMSEX;

typedef struct
{
	int	USRid;
	BYTE	szUserName[12];	/*  */
	BYTE	szPassword[12];	/* ASCII string */		
} STRUCT_USR;

typedef struct
{
	int	BLRid;
	BYTE	szBillerName[36+1];	/*  */
	BYTE	szBillerCode[4+1];	/* ASCII string */		
	BYTE	szBillerFee[12+1];
	BOOL    fBLREnable;
} STRUCT_BLR;

/*==================================================*
 *                  Reversal Func                       
 *==================================================*/
int inMyFile_ReversalSave(BYTE *szReversal8583Data, ULONG ulReversalSize);
int inMyFile_ReversalRead(BYTE *szReadOutBuffer, ULONG ulReadOutBufferSize);
int inMyFile_ReversalDelete(void);

/*==================================================*
 *                  Other Func                       
 *==================================================*/
BOOL fGetNumber(BYTE *szInBuffer, void *szOutBuffer, int inInBufferLen);
BOOL fGetBoolean(BYTE *szInBuffer);
BOOL fGetString(BYTE *szInBuffer, BYTE *szOutBuffer, int inInBufferLen);
BOOL inMyFile_feof(ULONG ulFileHandle);

int inMyFile_CheckFileExist(char *szFileName);
int inMyFile_GetFileLine(ULONG ulFileHandle, char *szBuffer, int inMaxOneLine);
int inMyFile_BatchSearch(TRANS_DATA_TABLE *trasnData, char *hexInvoiceNo);
int inMyFile_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inMyFile_BatchDelete(void);
int inMyFile_ContinueReadRec(void *p1);
int inMyFile_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt);

int inMyFile_HDTTraceNoAdd(BYTE byHostIndex) ;

void vdMyFile_TotalInit(STRUCT_FILE_SETTING *strFile, TRANS_TOTAL *strTotal);
int inMyFile_TotalRead(TRANS_TOTAL *strTotal);
int inMyFile_TotalSave(TRANS_TOTAL *strTotal);
int inMyFile_TotalDelete(void);
int inMyFile_AdviceDelete(void);
int inExceedMaxTimes_ReversalDelete(void);


/*==================================================*
 *                  Global Var                       
 *==================================================*/
STRUCT_TCT      strTCT;
STRUCT_CDT      strCDT;
STRUCT_CDT      strMCDT[10];

STRUCT_IIT      strIIT;
STRUCT_PCT      strPCT;
STRUCT_CST      strCST;
STRUCT_EMVT     strEMVT;
STRUCT_HDT      strHDT;
STRUCT_HDT      strMultiHDT[20];

STRUCT_CPT      strCPT;

STRUCT_MMT      strMMT[10];
STRUCT_TCP      strTCP;
STRUCT_PIT      strPIT;
STRUCT_MSG      strMSG;
STRUCT_TLE      strTLE;

STRUCT_COM      strCOM;
STRUCT_WAVE_AIDT	strWaveAIDT;
STRUCT_WAVE_EMVT	strWaveEMVT;

STRUCT_ERMTCT   strERMTCT;

STRUCT_FXT      strFXT;

STRUCT_AST      strAST;


S_COM_EXT      	strCOMExt;

STRUCT_TLESEC   strTLESEC;

STRUCT_GBLVar   strGBLVar; // sidumili: For global variable

STRUCT_WRITEFILE strWriteFile; // sidumili: For writi to file

STRUCT_TMSEX    strTMSEX;

STRUCT_USR      strMultiUSR[12];

STRUCT_BLR      strMultiBLR[50];

STRUCT_USR      strUSR;

STRUCT_BLR      strBLR;

//STRUCT_TLE strTLE;
#endif	/* _MYFILEFUNC__H */

