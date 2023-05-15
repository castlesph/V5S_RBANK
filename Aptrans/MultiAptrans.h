
#ifndef MULTIAPTRANS_H
#define MULTIAPTRANS_H

#define d_MAX_APP				25
#define	d_AP_FLAG_DEF_SEL_EX	0xC0

#define	d_CTLS_EVENT			99
#define	d_SUCCESS				100
#define	d_FAIL					101
#define	d_NOT_RECORD			102

#define d_CTLS_EXCEED_AMT       155
#define d_CTLS_ERROR       100

#define d_MAX_IPC_BUFFER		2048

typedef enum
{
    d_IPC_CMD_ONLINES_SALE		= 0x01,
	d_IPC_CMD_OFFLINE_SALE		= 0x02,
	d_IPC_CMD_VOID_SALE			= 0x03,
    d_IPC_CMD_PRE_AUTH			= 0x04,
    d_IPC_CMD_REFUND				= 0x05,
    d_IPC_CMD_TIP_ADJUST			= 0x06,
    d_IPC_CMD_SETTLEMENT			= 0x07,
    d_IPC_CMD_BATCH_REVIEW		= 0x08,
    d_IPC_CMD_BATCH_TOTAL			= 0x09,
    d_IPC_CMD_REPRINT_LAST		= 0x0A,
    d_IPC_CMD_REPRINT_ANY			= 0x0B,
    d_IPC_CMD_REPRINT_LAST_SETT	= 0x0C,
    d_IPC_CMD_SUMMARY_REPORT		= 0x0D,
    d_IPC_CMD_DETAIL_REPORT		= 0x0E,
    d_IPC_CMD_GET_PID				= 0x10,
    d_IPC_CMD_POWER_FAIL			= 0x11,
    d_IPC_CMD_DELETE_BATCH		= 0x12,
    d_IPC_CMD_DELETE_REVERSAL	= 0x13,
    d_IPC_CMD_SETTLE_ALL			= 0x14,
    d_IPC_CMD_CHECK_DUP_INV      = 0x15,
    d_IPC_CMD_WAVE_SALE			= 0x16,
    d_IPC_CMD_WAVE_REFUND			= 0x17,
    d_IPC_CMD_FUN_KEY1			= 0x18,
    d_IPC_CMD_FUN_KEY2		    = 0x19,
    d_IPC_CMD_FUN_KEY3			= 0x1A,
    d_IPC_CMD_FUN_KEY4		    = 0x1B,
    d_IPC_CMD_CHK_BATCH_EMPTY    = 0x1C,
    d_IPC_CMD_BAL_INQ    = 0x1D,
    d_IPC_CMD_LOGON    = 0x1E,
    d_IPC_CMD_LOY_BAL_INQ    = 0x1F,    
    //d_IPC_CMD_LOY_REDEEM_5050    = 0x20,
    d_IPC_CMD_LOY_REDEEM_VARIABLE    = 0x21,    
    d_IPC_CMD_INSTALLMENT    = 0x22,
    d_IPC_CMD_DEBIT_SALE    = 0x23,
    d_IPC_CMD_CARD_VERIFICATION    = 0x24,
    d_IPC_CMD_EDC_SETTINGS    = 0x25,
    d_IPC_CMD_PRINT_CRC    = 0x26,
    d_IPC_CMD_PRINT_EMV_TAGS    = 0x27,
    d_IPC_CMD_PRINT_HOST_INFO    = 0x28,
    d_IPC_CMD_PRE_COMP    = 0x29,
    d_IPC_CMD_CASH_ADV    = 0x2A,
    d_IPC_CMD_CHANGE_DEF_APP = 0x2B,
    d_IPC_CMD_FORCE_SETTLE = 0x2C,
    d_IPC_CMD_PREAUTH_VER = 0x2D,
    d_IPC_CMD_PREAUTH_VOID = 0x2E,
    d_IPC_CMD_LOY_REDEEM_5050 = 0x2F, /*changed same with d_IPC_CMD_ERM_INIT*/
    d_IPC_CMD_PUT_ENV_CRC    = 0x30, /* Put Env CRC*/
    d_IPC_CMD_ONLINES_SALE_DIAG	= 0x31, /*Debit Diagnostics*/
       
	//FOR QRPAY
    d_IPC_CMD_QRPAY         = 0x35,  
	d_IPC_CMD_RETRIEVE 		= 0x36,  
	
	//END-QRPAY

	// For testing
	d_IPC_CMD_WECHAT_SALE		= 0x37,
	d_IPC_CMD_ALIPAY_SALE		= 0x38,
	d_IPC_CMD_WECHAT_RETRIEVE	= 0x39,
	d_IPC_CMD_ALIPAY_RETRIEVE	= 0x3A,

	d_IPC_CMD_ONLINES_DEBIT_CASH_OUT	= 0x3B, /*For Bancnet CashOut -- sidumili*/

	//phqr
	d_IPC_CMD_PHQR_SALE 		= 0x3C,  
	d_IPC_CMD_PHQR_LOGON 		= 0x3D,  
	d_IPC_CMD_PHQR_RETRIEVE     = 0x3E,
	d_IPC_CMD_PHQR_SETTLE       = 0x3F,

	
    d_IPC_CMD_SETTLE_ALL_WALLET			= 0x40,
    d_IPC_CMD_CASH_IN		= 0x41,
	d_IPC_CMD_CASH_OUT		= 0x42,
	d_IPC_CMD_CARLESS_CASH_OUT		= 0x43,
	d_IPC_CMD_CARLESS_BAL_INQ		= 0x44,
	d_IPC_CMD_BILLS_PAYMENT		= 0x45,
	d_IPC_CMD_FUND_TRANSFER		= 0x46,
	d_IPC_CMD_REQUEST_TERMINAL_KEY     = 0x47,
	
    d_IPC_CMD_WIFISCAN			= 0x98,
    d_IPC_CMD_SETFONT			= 0x99,
    
}IPC_CMD_TYPES;

typedef enum
{
	d_IPC_CMD_CTLS_TRANS		= 0x02,
	d_IPC_CMD_CTLS_CANCLE		= 0x03,
	d_IPC_CMD_CTLS_GETDATA		= 0x04,
	d_IPC_CMD_CTLS_SETPOWPARA	= 0x05,
    d_IPC_CMD_ERM_INIT			= 0x50,	
	d_IPC_CMD_CTLS_PARTIALSETTAGS	= 0x0E,

}IPC_CTLSCMD_TYPES;

int inMultiAP_RunIPCCmdTypes(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);
int inMultiAP_RunIPCCmdTypesEx(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);
USHORT inMultiAP_HandleIPC(BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);
int inMultiAP_GetMainroutine(void);
int inMultiAP_SendChild(BYTE *inbuf, USHORT inlen);

int inCTOS_MultiAPALLAppEventID(int IPC_EVENT_ID);
int inCTOS_MultiAPALLAppEventIDEx(int IPC_EVENT_ID);
int inCTOS_MultiAPBatchSearch(int IPC_EVENT_ID);
int inCTOS_MultiAPSaveData(int IPC_EVENT_ID);
int inCTOS_MultiAPGetData(void);
int inCTOS_MultiAPGetpowrfail(void);
int inCTOS_MultiAPGetVoid(void);
int inCTOS_MultiAPReloadTable(void);
int inCTOS_MultiAPReloadHost(void);
int inCTOS_MultiAPCheckAllowd(void);
int inMultiAP_ECRGetMainroutine(void);
int inCTOSS_MultiDislplayMenu(int IPC_EVENT_ID);

int inCTOS_MultiSwitchApp(const char szAppName[25], int IPC_EVENT_ID);

#endif


