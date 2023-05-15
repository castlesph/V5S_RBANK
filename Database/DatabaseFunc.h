#ifndef _DATABASEFUNC__H
#define	_DATABASEFUNC__H

#ifdef	__cplusplus
extern "C" {
#endif
#include "../Database/dct.h"
#include "../Database/ins.h"
#include "../Database/prm.h"
#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"
#include "../FileModule/myFileFunc.h"


//#include "../Database/prm2.h"

//��ע�⣬��Ҫ����ļ���Ĵ�Сд
//#define DB_TERMINAL "/data/data/pub/TERMINAL.S3DB"
#define DB_TERMINAL "/home/ap/pub/TERMINAL.S3DB"
#define DB_BATCH	"./fs_data/S1_MCC.S3DB"
//#define DB_BATCH	"/data/data/com.Source.S1_RBANK.RBANK/S1_RBANK.S3DB"


//#define DB_EMV	"/data/data/pub/EMV.S3DB"
#define DB_EMV "/home/ap/pub/EMV.S3DB"
//#define DB_COM	"/data/data/pub/COM.S3DB"
#define DB_COM	"/home/ap/pub/COM.S3DB"
    
#define DB_MULTIAP	"/home/ap/pub/MULTIAP.S3DB"
#define DB_WAVE		"/home/ap/pub/WAVE.S3DB"
//#define DB_WAVE		"C:\V5S_MCCAPP\V5S_MCCCREDIT\Dist\V5S\MCCCREDIT\21Wave_data\WAVE.S3DB"

#define DB_ERM	"/home/ap/pub/ERM.S3DB"
#define DB_ERM_BATCH  "/home/ap/pub/ERMBATCH.S3DB"
#define DB_CTLS	"/home/ap/pub/CTLS.S3DB"
#define DB_ERM_TERMINAL	"/home/ap/pub/ERMTCT.S3DB"

#define DB_MULTIAP_JOURNAL "/home/ap/pub/MULTIAP.S3DB-journal"
#define DB_INVOICE "/home/ap/pub/INVOICE.S3DB"

#define DB_APPSELECTMENU "./fs_data//DYNAMICMENU99.S3DB"

#define FILENAME_SIZE 200

#define ISOLOG_DB "./fs_data/ISOLOG.S3DB"


BOOL fGetDBOpen(void);
void vdSetDBOpen(BOOL fOpen);

void vdSetJournalModeOff(void);


int inAIDNumRecord(void);
int inHDTNumRecord(void);
int inTCTNumRecord(void);
int inCDTNumRecord(void);
int inRDTNumRecord(void);
int inEMVNumRecord(void);
int inAIDNumRecord(void);
int inMSGNumRecord(void);
int inTLENumRecord(void);
int inIITNumRecord(void);
int inMITRead(int inSeekCnt);

int inCPTRead(int inSeekCnt);
int inCPTReadEx(int inSeekCnt);

int inCPTSave(int inSeekCnt);

int inPCTRead(int inSeekCnt);
int inPCTReadEx(int inSeekCnt);

int inPCTSave(int inSeekCnt);

int inHDTRead(int inSeekCnt);
int inHDTReadEx(int inSeekCnt);

int inHDTSave(int inSeekCnt);
int inCDTRead(int inSeekCnt);
int inCDTReadEx(int inSeekCnt);
int inCDTReadMulti(char *szPAN, int *inFindRecordNum);
int inCDTMAX(void);

int inEMVRead(int inSeekCnt);
int inAIDRead(int inSeekCnt);
int inAIDReadbyRID(int inSeekCnt, char * PRid);
int inTCTRead(int inSeekCnt);
int inTCTReadEx(int inSeekCnt);
int inTCTSave(int inSeekCnt);
int inTCTMenuSave(int inSeekCnt);
int inTCPRead(int inSeekCnt);
int inTCPSave(int inSeekCnt);
int inMSGRead(int inSeekCnt);
int inTLERead(int inSeekCnt);
int inTLESave(int inSeekCnt);
int inIITRead(int inSeekCnt);
int inIITReadEx(int inSeekCnt);

int inIITSave(int inSeekCnt);
int inIITSaveEx(int inSeekCnt);


int inCSTNumRecord(void);
int inCSTRead(int inSeekCnt);
int inCSTReadEx(int inSeekCnt);
int inCSTSave(int inSeekCnt);

int inMMTReadRecord(int inHDTid,int inMITid);
int inMMTReadRecordEx(int inHDTid,int inMITid);


int inMMTNumRecord(void);

int inMMTReadNumofRecords(int inSeekCnt,int *inFindRecordNum);

int inMMTSave(int inSeekCnt);
int inMMTSaveEx(int inSeekCnt);
int inMMTRead(int inSeek, char szTrxn[][20]);


int inPITNumRecord(void);
int inPITRead(int inSeekCnt);
int inPITSave(int inSeekCnt);

int inWaveAIDRead(int inSeekCnt);
int inWaveAIDNumRecord(void);
int inWaveEMVNumRecord(void);
int inWaveEMVRead(int inSeekCnt);



int inMSGResponseCodeRead(char* szMsg, char *szMsg2, int inMsgIndex, int inHostIndex);
int inMSGResponseCodeReadByHostName(char* szMsg, char *szMsg2, int inMsgIndex, char *szHostName);
int inMSG2ResponseCodeReadByHostName(char* szMsg, char *szMsg2, int inMsgIndex, int inHostIndex);




int inDatabase_BatchDeleteHDTidMITid(void);
int inDatabase_BatchDelete(void);
int inDatabase_BatchInsert(TRANS_DATA_TABLE *transData);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_BatchCheckDuplicateInvoice(char *hexInvoiceNo);
int inBatchNumRecord(void);
int inDatabase_BatchReadByHostidAndMITid(TRANS_DATA_TABLE *transData,int inHDTid,int inMITid);
int inBatchByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID);
int inDatabase_BatchReadByTransId(TRANS_DATA_TABLE *transData, int inTransDataid);


int inHDTReadHostName(char szHostName[][100], int inCPTID[]);
int inHDTReadHostNameCredit(char szHostName[][100], int inCPTID[]);

int inERMAP_Database_BatchDelete(void);
int inERMAP_Database_BatchInsert(ERM_TransData *strERMTransData);

int inMultiAP_Database_BatchRead(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchDelete(void);
int inMultiAP_Database_BatchDeleteEx(void);

int inMultiAP_Database_BatchInsert(TRANS_DATA_TABLE *transData);

int inMultiAP_Database_EMVTransferDataInit(void);
int inMultiAP_Database_EMVTransferDataWrite(USHORT usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataWriteEx(USHORT usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataRead(USHORT *usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataReadEx(USHORT *usDataLen, BYTE *bEMVData);


//int inMultiAP_Database_COM_Read(void);
//int inMultiAP_Database_COM_Save(void);
//int inMultiAP_Database_COM_Clear(void);
int inHDTReadByApname(char *szAPName);

int inUpdateDefaultApp(int inHostIndexNew, int inHostIndexOld);
int inUpdateBancnetCDT(int inHostIndex);
int inEnableOldHost(int fHostEnable);
int inEnableNewHost(int fHostEnable);
int inUpdateCommsMode(int inCommMode);
int inDatabase_InvoiceNumInsert(TRANS_DATA_TABLE *transData);
int inDatabase_InvoiceNumSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_InvoiceNumDelete(int HDTid, int MITid);
int inMultiAP_Database_CTLS_Delete(void);
int inMultiAP_Database_CTLS_Read(CTLS_TransData *strCTLSTransData);
int inMultiAP_Database_CTLS_Insert(CTLS_TransData *strCTLSTransData);

int inCheckAllBatchEmtpy(void);
int inCheckBatchEmtpyPerHost(int inHDTid);
int inCPTNumRecord(void);

// ERM
int inERMAP_Database_ERMAdviceBatchDelete(void);
int inERMAP_Database_ERMTransDataBatchDelete(void);
int inERMAP_Database_ERMTransDataBackupBatchDelete(void);
int inERMTCT_Update(int inCommMode, int inSeekCnt);
int inERMTCT_IPUpdate(char *szHostIP, char *szSecHostIP, int ulHostPort, int ulSecHostPort, int inSeekCnt);
int inERMTCT_IPTRead(int inSeekCnt);
int inERMNumRecord(void);

// CPT
int inCPT_Update(int inCommMode, int inIPHeader);
int inCPT_IPUpdate(char *szPriTxnHostIP, char *szSecTxnHostIP, char *szPriSettlementHostIP, char *szSecSettlementHostIP, int inPriTxnHostPortNum, int inSecTxnHostPortNum, int inPriSettlementHostPort, int inSecSettlementHostPort);

//int inDatabase_TerminalOpenDatabase(const char *szDBName);
int inDatabase_TerminalOpenDatabase(void);
int inDatabase_TerminalCloseDatabase(void);
int inDatabase_TerminalOpenDatabaseEx(const char *szDBName);
//int inReadDynamicMenu(int inIdleSaleType);
int inReadDynamicMenu2(int inIdleSaleType);

int inReadDistinctAppName(int *inNumRecs);
int inMMTBatchNotEmptySave(int inSeekCnt);
int inGetSelectionMenu(int *inNumRecs);


typedef struct
{
	int DMTrxMenuid;
	int usButtonID;
	char szButtonItemImage[100];
	char szButtonItemLabel[100];
	char szButtonTitleLabel[100];
	char szButtonItemFunc[100];
	char szSubMenuName[100];
} STRUCT_TRXMENU;

STRUCT_TRXMENU strTrxMenu[20];

int inReadBaseDMTrxMenu(int *inNumRecs);
int inDeleteToSelectionMenu(void);
int inProcessSelectionMenu(void);
int inInsertSelectionMenu(int DMTrxMenuid, int usButtonID, char* szButtonItemImage, char* szButtonItemLabel, char* szButtonTitleLabel, char* szButtonItemFunc, char* szSubMenuName);

int inCSTReadCurrencySymbol(char *szCurCode, unsigned char * szCurrencySymbol, unsigned char * szAmountFormat);

int inFXTRead(int inSeekCnt);

int inReadTrxMenu(char szTrxn[][20], char szItemFunc[][40],int inTrxnID[]);
int inReadTrxTypes(char szTrxn[][20], char szItemImage[][40],int inTrxnID[]);
int inTRXNumRecord(void);
int inReadDMTrxAppMenu(int inIdleSaleType);
int inReadDMTrxSubMenu(int inIdleSaleType);
int inReadDMTrx(int inSaleType);
int inHDTReadEx2(int inSeekCnt);
int inCheckSettled(void);
int inUpdateSettled(int inSettled);

int inDatabase_BatchUpdatePreComp(TRANS_DATA_TABLE *transData);
int inMMTMustSettleNumRecord(void);
int inTLESecSave(int inSeekCnt);
int inTLESecRead(int inSeekCnt);

int inTCTMMTidSave(int inSeekCnt);

int inHDTReadHostNameBancnet(char szHostName[][100], int inCPTID[]);
int inHDTReadHostNameDigitalWallet(char szHostName[][100], int inCPTID[]);
int inTMSEXRead(int inSeekCnt);
int inTMSEXSave(int inSeekCnt);

int inUSRReadEx(int inSeekCnt);
int inUSRNumRecord(void);

int inUSRDeleteRecord(char *szUserName);
int inDLRDeleteRecord(char *szBillerName);

int inUSRSave(int inSeekCnt);
int inBLRSave(int inSeekCnt);

int inUSRInsert(void);
int inBLRInsert(void);
int inUSRReadAdminEx(int inSeekCnt);

#endif	/* _DATABASEFUNC__H */

