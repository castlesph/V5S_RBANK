
#ifndef __POSERM_H__
#define	__POSERM_H__


typedef struct  
{
    USHORT usSaleCount;
    double  ulSaleTotalAmount;
    USHORT usRefundCount;
    double  ulRefundTotalAmount;
    USHORT usTipCount;
    double  ulTipTotalAmount;   
    USHORT usVoidSaleCount;
    double  ulVoidSaleTotalAmount;
    USHORT usOffSaleCount;
    double  ulOffSaleTotalAmount; 
}STRUCT_TOTAL_ERM;

typedef struct
{
    STRUCT_TOTAL_ERM stUploadTotal;
    STRUCT_TOTAL_ERM stAllTotal;
}TRANS_TOTAL_ERM;


typedef enum
{
    d_IPC_CMD_ERM_INITIALIZATION		        = 0x01,
	d_IPC_CMD_ERM_FORM_RECEIPT		    = 0x02,
	d_IPC_CMD_ERM_UPLOAD_RECEIPT		        = 0x03,
	d_IPC_CMD_ERM_GET_RECEIPTTOT		        = 0x04,
	d_IPC_CMD_ERM_DELETE_RECEIPT		        = 0x05,
	d_IPC_CMD_ERM_GET_ERMINFO			        = 0x06,
	d_IPC_CMD_ERM_RESTORE_BACKUPRECEIPT			= 0x07,
	d_IPC_CMD_ERM_PRINT_BACKUPRECEIPT			= 0x08,
	d_IPC_CMD_ERM_FORWARD_TRANS_REQ		        = 0x09,
	d_IPC_CMD_ERM_FORWARD_TRANS_RESP		    = 0x0A,
	d_IPC_CMD_ERM_CARDINFO_ENQ		    		= 0x0B,
	d_IPC_CMD_ERM_GET_BACKUPRECEIPTTOT		    = 0x10,
//    d_IPC_CMD_COMM_RECEIVE	        = 0x04,
//    d_IPC_CMD_COMM_DISCONNECT		= 0x05,
//    d_IPC_CMD_COMM_END		        = 0x06,
//    d_IPC_CMD_GPRS_SIGNAL		        = 0x07,

}IPC_ERM_CMD_TYPES;

#define	SM_RET_PARAM					-500

#define SIGP_SEND_SIZE					(1024 * 8)           /* SIGNATURE */
#define ERC_LOGO_FILE					"ERCLOGO.TXT"
#define ERC_HEADER_FILE					"ERCHEADER.TXT"
#define ERC_FOOTER_FILE					"ERCFOOTER.TXT"
#define BMP_RECEIPT_FILE_NAME		    "/home/ap/pub/logo.bmp"
#define ERC_SHARE_HEADER_FILE		    "/home/ap/pub/ERCHEADER.TXT"

#define ERC_SHARE_FOOTER_FILE		    "/home/ap/pub/ERCFOOTER.TXT"

#define VFBMPORINAME	"signVFOri.bmp"	//128 * 64
#define VFBMPONAME		"signVFO.bmp"	//256 * 128

#define ERM_NONE_TYPE   0
#define ERM_SMS_TYPE    1
#define ERM_EMAIL_TYPE  2
#define ERM_BOTH_TYPE   3

long lnGetFileSize(const char* pchFileName);

int inWriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize);
int inReadGzipFile(unsigned char *pchFileName, unsigned char* uszBufferData, int *inBufferSize);


int inInitializePrinterBufferLib(void);
int inSIGPStartCapturePrinterDataLib(void);
int inSIGPEndCapturePrinterDataLib(void);

int inCTOSS_CapturePrinterBuffer(unsigned char* pchTemp,CTOS_FONT_ATTRIB* ATTRIB);
int inCTOSS_ERM_Form_Receipt(int inSettle);
int inCTOS_ERMAllHosts_Initialization(BOOL fAllHost);
int inCTOSS_ERM_Initialization(void);

USHORT usCTOSS_Erm_Initialization(void);
USHORT usCTOSS_Erm_FromReceipt(void);
USHORT usCTOSS_Erm_UploadReceipt(BOOL fSettle);
int usCTOSS_ReadLGOfile(void);

int inCTOSS_UploadReceipt(BOOL fSettle, BOOL fPending);
int inCTOSS_ERMInit(void);
int inCTOSS_ERM_CheckSlipImage(void);
int usCTOSS_Erm_GetERMInfo(char* pchFileName,TRANS_TOTAL_ERM *srErmRec);


int inDeleteGzipFile(unsigned char *pchFileName);
int inDeleteFile(unsigned char *pchFileName);
int inCopyFile(unsigned char *pchFileName1, unsigned char *pchFileName2);

void vdLinuxCommandFileCopy(void);
void vdLinuxCommandClearERMBitmap(void); /*albert - ERM*/
void vdLinuxCommandFileDelete(void);
int inCTOSS_ERM_Send(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen);
int inCTOSS_ERM_Receive(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData);

int inCTOSS_ERM_ReceiptRecvVia(void);
void vdCTOSS_DisplayStatus(int inRet);

int inCTOSS_ERM_ChoicePhoneNo(char *szPhoneNo);
int inCTOSS_ERM_ChoiceEmail(char *szEmail);
int inCTOSS_ERM_ChoicePhoneNoAndEmail(char *szPhoneNo,char *szEmail);
int inCTOSS_ERM_EnterPhoneNo(void);
int inCTOSS_ERM_EnterEmail(void);
int inCTOSS_ERM_EnterPhoneNoAndEmail(void);
void vdDisplayProcessing(void);
void vdCTOSS_DisplaySettleSuccess(void);
void vdClearERMBatch(BOOL fConfirm);
BOOL fERM_ChkBatchEmpty(BOOL fPrompt); // Check if ERM Batch is empty
BOOL fERM_ChkInitialize(void); // Check if ERM Initialize
BOOL fERM_ChkFuncAvailable(BOOL fPrintReport);

int inERM_Confirmation(int inType, char *szConfirm); // SMS / EMAIL Confirmation -- sidumili
void vdCTOS_ERMNotification(void);

int inCTOSS_REUploadReceipt(void);
//int inCTOSS_HOST_ReUpload_ERMInit(void);
int inCTOSS_HOST_ERMInit(void);
void vdReFork_SHARLS_ERM(void);
int inCTOSS_ERMInitAllHost(void);
void vdHostToInitialize(BOOL fPrompt, BOOL fSuccess);
void vdHostToDeInitialize(void);
USHORT usCTOSS_Erm_RestoreBackupReceipt(void);
UINT usCTOSS_Erm_GetBackupReceiptTotal(void);

void vdSetLastMerchant(void);
void vdGetLastMerchant(void);
int inCTOSS_HOST_ReERMInit(void);
int inCTOSS_ERM_Form_Receipt_Diag(int inSettle, int inPurpose);

#endif

