/* 
 * File:   typedef.h
 * Author: sam
 *
 * Created on 2012年8月15日, 下午 11:19
 */

#ifndef TYPEDEF_H
#define    TYPEDEF_H

#ifdef    __cplusplus
extern "C" {
#endif

//#define TLE 1
//#define CVV  //CVV enable changed to TCT.txt

#define PUBLIC_PATH 			"/home/ap/pub/"
#define LOCAL_PATH 				"./fs_data/"
#define UNFORK_AP_FILE			"unforkap.ini"

#define IN
#define OUT
#define INOUT

#define char    char
#define UCHAR   unsigned char
#define BYTE    unsigned char

#ifndef STR
    #define STR        unsigned char    
#endif


#define SHORT   short                // 2byte
#define USHORT  unsigned short       // 2byte
#define WORD    unsigned short       // 2byte
#define int     int                  // 4byte    
#define UINT    unsigned int         // 4byte
#define DWORD   unsigned int         // 4byte
#define BOOL    unsigned char        // 1byte
#define LONG    long                 // 4byte
#define ULONG   unsigned long        // 4byte
#define DOUBLE   double        		 // 8byte
#define DUKPT_3DESONLINEPIN

#define TRUE     1
#define FALSE    0

#ifndef NULL
#define NULL    0
#endif

#ifndef CTOSNULL
#define CTOSNULL    0
#endif

#define RUN_SAHRE_EMV   1


#define V3_STATUS_LINE_ROW   14
#define V3_ERROR_LINE_ROW   14

#define d_VW_SID_CUP_EMV					0x91

/* Commuction mode */
#define DIAL_UP_MODE        0
#define ETHERNET_MODE       1
#define GPRS_MODE           2    
#define WIFI_MODE			4


typedef unsigned char VS_BOOL;


#define K_STATUS        BYTE

#define SUCCESS     0
#define FAIL       -1

#define VS_FALSE           ((VS_BOOL) 0)
#define VS_TRUE            ((VS_BOOL) 1)

#define MAX_CHAR_PER_LINE       40

/*Tags definition*/

#define TAG_4F_AID                    0x004F //Application Identifier (AID)
#define TAG_50                        0x0050 //Application Label
#define TAG_52                        0x0052 //Command To Perform
#define TAG_55                        0x0055 //TSignature CVM
#define TAG_57                        0x0057 //Track 2 Equivalent Data
#define TAG_5A_PAN                    0x005A //Application Primary Account Number (PAN)
#define TAG_61                        0x0061 //Application Template
#define TAG_6F                        0x006F //FCI Template
#define TAG_70                        0x0070 //Application Elementary File (AEF) Data Template
#define TAG_71                        0x0071 //Issuer Script Template 1
#define TAG_72                        0x0072 //Issuer Script Template 2
#define TAG_73                        0x0073 //Directory Discretionary Template
#define TAG_77                        0x0077 //Response Message Template Format 2
#define TAG_80                        0x0080 //Response Message Template Format 1
#define TAG_81_BIN_AMOUNT             0x0081 //Amount, Authorised (Binary)
#define TAG_82_AIP                    0x0082 //Application Interchange Profile (AIP)
#define TAG_83                        0x0083 //Command Template
#define TAG_84_DF_NAME                0x0084 //Dedicated File (DF) Name
#define TAG_86                        0x0086 //Issuer Script Command
#define TAG_87                        0x0087 //Application Priority Indicator
#define TAG_88                        0x0088 //Short File Identifier (SFI)
#define TAG_8A_AUTH_CODE              0x008A //Authorisation Response Code
#define TAG_8C                        0x008C //Card Risk Management Data 1 (CDOL1)
#define TAG_8D                        0x008D //Card Risk Management Data 2 (CDOL2)
#define TAG_8E                        0x008E //Cardholder Verification Method (CVM) List
#define TAG_8F                        0x008F //Certification Authority Public Key Index (ICC)
#define TAG_90                        0x0090 //Issuer Public Key Certificate
#define TAG_91_ARPC                   0x0091 //Issuer Authentication Data
#define TAG_92                        0x0092 //Issuer Public Key Remainder
#define TAG_93                        0x0093 //Signed Application Data
#define TAG_94                        0x0094 //Application File Locator (AFL)
#define TAG_95                        0x0095 //Terminal Verification Results
#define TAG_97_TDOL                   0x0097 //Transaction Certificate Data Object List (TDOL)
#define TAG_98                        0x0098 //Transaction Certificate (TC) Hash Value
#define TAG_99                        0x0099 //Transaction PIN Data
#define TAG_9A_TRANS_DATE             0x009A //Transaction Date
#define TAG_9B                        0x009B //Transaction Status Information
#define TAG_9C_TRANS_TYPE             0x009C //Transaction Type
#define TAG_9D                        0x009D //Directory Definition File (DDF) Name
                                      
                                      
#define TAG_5F20                      0x5F20 //Cardholder Name
#define TAG_5F24_EXPIRE_DATE          0x5F24 //Application Expiration Date
#define TAG_5F25                      0x5F25 //Application Effective Date
#define TAG_5F28                      0x5F28 //Issuer Country Code
#define TAG_5F2A_TRANS_CURRENCY_CODE  0x5F2A //Transaction Currency Code
#define TAG_5F2D                      0x5F2D //Language Preference
#define TAG_5F30_SERVICE_CODE         0x5F30 //Service Code
#define TAG_5F34_PAN_IDENTFY_NO       0x5F34 //Application PAN Sequence Number
#define TAG_5F36                      0x5F36 //Transaction Currency Exponent
                                      
                                      
#define TAG_9F01                      0x9F01 //Acquirer Identifier
#define TAG_9F02_AUTH_AMOUNT          0x9F02 //Amount, Authorised (Numeric)
#define TAG_9F03_OTHER_AMOUNT         0x9F03 //Amount, Other (Numeric)
#define TAG_9F04                      0x9F04 //Amount, Other (Binary)
#define TAG_9F05                      0x9F05 //Application Discretionary Data
#define TAG_9F06                      0x9F06 //Application Identifier (AID) (Terminal)
#define TAG_9F07                      0x9F07 //Application Usage Control
#define TAG_9F08_IC_VER_NUMBER        0x9F08 //Application Version Number
#define TAG_9F09_TERM_VER_NUMBER      0x9F09 //Application Version Number (Terminal)
#define TAG_9F0B                      0x9F0B //Cardholder Name Extended
#define TAG_9F0D                      0x9F0D //Issuer Action Code - Default
#define TAG_9F0E                      0x9F0E //Issuer Action Code - Denial
#define TAG_9F0F                      0x9F0F //Issuer Action Code - Online
                                      
#define TAG_9F10_IAP                  0x9F10 //Issuer Application Data
#define TAG_9F11                      0x9F11 //Issuer Code Table Index
#define TAG_9F12                      0x9F12 //Application Preferred Name
#define TAG_9F13                      0x9F13 //Last Online ATC Register
#define TAG_9F14                      0x9F14 //Lower Consecutive Offline Limit (EMV Generic)
#define TAG_9F15                      0x9F15 //Merchant Category Code
#define TAG_9F16                      0x9F16 //Merchant Identifier
#define TAG_9F17                      0x9F17 //PIN Try Counter
#define TAG_9F18                      0x9F18 //Issuer Script Identifier
#define TAG_9F1A_TERM_COUNTRY_CODE    0x9F1A //Terminal Country Code
#define TAG_9F1B_TERM_FLOOR_LIMIT     0x9F1B //Terminal Floor Limit
#define TAG_9F1C_TID                  0x9F1C //Terminal Identification
#define TAG_9F1D                      0x9F1D //Terminal Risk Management Data
#define TAG_9F1E                      0x9F1E //Interface Device Serial Number
#define TAG_9F1F                      0x9F1F //Track 1 Discretionary Data
                                      
#define TAG_9F20                      0x9F20 //Track 2 Discretionary Data
#define TAG_9F21_TRANS_TIME           0x9F21 //Transaction Time
#define TAG_9F22                      0x9F22 //Certification Authority Public Key Index (Terminal)
#define TAG_9F23                      0x9F23 //Upper Consecutive Offline Limit (EMV Generic)
#define TAG_9F26_EMV_AC               0x9F26 //Application Cryptogram (AC)
#define TAG_9F27                      0x9F27 //Cryptogram Information Data (CID)
#define TAG_9F2D                      0x9F2D //ICC PIN Encipherment Public Key Certificate
#define TAG_9F2E                      0x9F2E //ICC PIN Encipherment Public Key Exponent
#define TAG_9F2F                      0x9F2F //ICC PIN Encipherment Public Key Remainder
                                      
#define TAG_9F32                      0x9F32 //Issuer Public Key Exponent
#define TAG_9F33_TERM_CAB             0x9F33 //Terminal Capabilities
#define TAG_9F34_CVM                  0x9F34 //Cardholder Verification Method (CVM) Results
#define TAG_9F35_TERM_TYPE            0x9F35 //Terminal Type
#define TAG_9F36_ATC                  0x9F36 //Application Transaction Counter (ATC)
#define TAG_9F37_UNPREDICT_NUM        0x9F37 //Unpredictable Number
#define TAG_9F38                      0x9F38 //Processing Options Data Object List (PDOL)
#define TAG_9F39                      0x9F39 //POS Entry Mode
#define TAG_9F3A_AMOUNT_REF_CURRENCY  0x9F3A //Amount, Reference Currency
#define TAG_9F3B                      0x9F3B //Application Reference Currency
#define TAG_9F3C_TRANS_REF_CURRENCY   0x9F3C //Transaction Reference Currency Code
#define TAG_9F3D                      0x9F3D //Transaction Reference Currency Exponent
                                      
#define TAG_9F40_ADD_TERM_CAB         0x9F40 //Additional Terminal Capabilities
#define TAG_9F41                      0x9F41 //Transaction Sequence Counter
#define TAG_9F42_APP_CURRENCY_CODE    0x9F42 //Application Currency Code
#define TAG_9F43                      0x9F43 //Application Reference Currency Exponent
#define TAG_9F44                      0x9F44 //Application Currency Exponent
#define TAG_9F45                      0x9F45 //Data Authentication Code
#define TAG_9F46                      0x9F46 //ICC Public Key Certificate
#define TAG_9F47                      0x9F47 //ICC Public Key Exponent
#define TAG_9F48                      0x9F48 //ICC Public Key Remainder
#define TAG_9F49_DDOL                 0x9F49 //Dynamic Data Authentication Data (DDOL)
#define TAG_9F4A                      0x9F4A //Static Data Authentication Tag List
#define TAG_9F4B                      0x9F4B //Signed Dynamic Application Data
#define TAG_9F4C                      0x9F4C //ICC Dynamic Number
                                      
#define TAG_9F52                      0x9F52 //Application Default Action
#define TAG_9F53                      0x9F53 //Transaction Category Code
#define TAG_9F58                      0x9F58 //Lower Consecutive Offline Limit (Proprietary)
#define TAG_9F59                      0x9F59 //Upper Consecutive Offline Limit (Proprietary)
#define TAG_9F5B                      0x9F5B //POS_ISS_SCRIPT_RESULT

#define TAG_9F6E                      0x9F6E //Form Factor Indicator

#define TAG_9F72                      0x9F72 //Consecutive Transaction Limit (international - country)
#define TAG_9F73                      0x9F73 //Currency Conversion Factor
#define TAG_9F75                      0x9F75 //Cumulative Total Transaction Amount Limit - Dual Currency
#define TAG_9F76                      0x9F76 //Secondary Application Currency Code
#define TAG_9F7F                      0x9F7F //Card Product Life Cycle History File Identifiers

#define TAG_DF31_ISR                  0xDF31//issuer script result    

/*definition of EMV status*/
#define EMV_READ_FAILED      1
#define EMV_TRANS_FAILED     101
#define EMV_CHIP_FAILED      102
#define EMV_USER_ABORT       103
#define EMV_CRITICAL_ERROR   104
#define EMV_NO_AP_FOUND      105
#define EMV_POOL_FULL        106
#define EMV_FALLBACK         107

#define EMV_CARD_BLOCKED 108 //VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback -- jzg
#define EMV_CHIP_NOT_DETECTED 109 //EMV - should display "CHIP NOT DETECTED" instead of doing fallback -- jzg
#define EMV_TRANS_NOT_ALLOWED 110 //EMV - If AID not found display "TRANS NOT ALLOWED" -- jzg
#define EMV_APPL_NOT_AVAILABLE 111

#define EMV_FAILURE_EX          40//add for FAILED EMV app selection
#define EMV_APP_BLOCKED			9 // BANCNET - T007- TIC024 - APPLICATION BLOCKED -- sidumili
#define EMV_APP_BLOCKED_AMEX	7// AMEX - AXP EMV 018 - APPLICATION BLOCKED -- AAA


/*EMV chip trans result*/
#define EMV_OFFLINE_APPROVED_TC    0x0001
#define EMV_OFFLINE_DECLINE_AAC    0x0002
#define EMV_ONLINE_ARQC            0x0003
#define EMV_ONLINE_AAR             0x0004

    
#define AID_MAX                    22

#define EMV_VISA_CARD              1
#define EMV_MASTER_CARD            2
#define EMV_JCB_CARD               3
#define EMV_AMEX_CARD              4    



/* Transaction Code */
#define SALE         101
#define PRE_AUTH     102
//#define PRE_COMP     103
#define TC_UPLOAD    104
#define SEND_ADVICE  105
#define REVERSAL     106
#define REFUND       107
#define VOID         108
#define SALE_TIP     109
#define SALE_OFFLINE 110
#define SETTLE       111
#define CLS_BATCH    112
#define BATCH_UPLOAD 113
#define SALE_ADJUST  114
#define SIGN_ON      115
#define EPP_SALE      116
#define REPRINT_ANY      117
#define BATCH_REVIEW    118
#define BATCH_TOTAL     119
#define VOID_REFUND         120
#define OFFLINE_VOID        121
#define OFFLINE_REFUND      122
#define VOID_REVERSAL       123
#define REFUND_REVERSAL     124
#define PREAUTH_REVERSAL    125
#define VOIDREFUND_REVERSAL 126
#define BAL_INQ				127
#define LOG_ON				128

#define LOY_BAL_INQ			130
#define LOY_REDEEM_5050		131
#define LOY_REDEEM_VARIABLE	132
#define LOY_REDEEM_REVERSAL 133 

#define INSTALLMENT_SALE 134 

#define TERM_REG 135

#define CARD_VERIFY 136

#define PRECOMP_REVERSAL 137

#define PRE_COMP 138
#define CASH_ADV 139
#define CASHADV_REVERSAL 140
#define VOID_REDEEM 141
#define VOIDREDEEM_REVERSAL 142
#define VOID_PRECOMP 143

#define PREAUTH_VER 144
#define PREAUTH_COMP 145
#define PREAUTH_VER_REVERSAL 146
#define PREAUTH_VOID 147
#define PREAUTH_VOID_REVERSAL 148


// For MP200
#define HOST_INFO_REPORT 149
#define CRC_REPORT       150
#define IP_REPORT        151
#define CITAS_SERVICES	 153
#define SET_CARD		 154
#define MERCHANT_SERVICES	 155

#define REPRINT_FSR_RPT	 156

#define DCC_RATE_REQUEST 157
#define DCC_MERCH_RATE_REQ 158
#define MERCHANT_ORIENT	 159

#define PRINT_CTMS_RPT   160
#define CASH_OUT        161
#define CTMS_UPDATE     162

/* Cash Mo Transaction Code */
#define CASH_IN    170
#define CARDLESS_CASH_OUT    171
#define CARDLESS_BAL_INQ    172
#define BILLS_PAYMENT    173
#define FUND_TRANSFER    174

#define SETUP				15

//unused txn type or pack type
#define DCC_RATEREQUEST 0
#define DCC_RATEREQUEST_RETRY 0

/* To determine the type of results */
#define CN_FALSE   0
#define CN_TRUE    1

/* Results of the state */
#define ST_SUCCESS          0
#define ST_ERROR            (-1)
#define ST_BUILD_DATD_ERR   (-2)
#define ST_SEND_DATA_ERR    (-3)
#define ST_UNPACK_DATA_ERR  (-4)
#define ST_RESP_MATCH_ERR  (-5)
#define RATE_REQUEST_CONN_FAIL  (-6)


/* MCC: Added settlement reconciliation return state -- jzg */
#define ST_CLS_BATCH_ERR (-6)

/* Values for chip status flag */
#define NOT_USING_CHIP       0
#define EMV_CARD             1
#define EMV_EASY_ENTRY_CARD  2
#define EMV_TABLE_NOT_USED  -1

/* The status of the transaction */
#define TRANS_AUTHORIZED    99
#define TRANS_COMM_ERROR    98
#define TRANS_CALL_BANK     97
#define TRANS_CANCELLED     96
#define TRANS_REJECTED      95
#define TRANS_TERMINATE     94

/* Commuction mode */
#define DIAL_UP_MODE        0
#define ETHERNET_MODE       1
#define GPRS_MODE           2    
#define COM1_MODE           3//for COM&USB communication
#define COM2_MODE           4
#define USB_MODE           	5
#define FORWARD_MODE        8
#define NULL_MODE           9

#define STORE_ID_DIGITS            18
#define TRACK_I_BYTES              85
#define TRACK_II_BYTES             41
#define TRACK_III_BYTES            200 // mfl
#define CVV2_BYTES                4//6
#define CARD_HOLD_NAME_DIGITS      75//30
#define AUTH_CODE_DIGITS           6
#define PAN_SIZE                   19
#define RRN_BYTES                  12
#define RESP_CODE_SIZE             2
#define TERMINAL_ID_BYTES          8
#define MERCHANT_ID_BYTES          15
#define CHIP_DATA_LEN              1024
#define ADD_DATA_LEN             1024
#define MMTENABLE_BYTE       1


/*kobe added >>>*/
#define TRACE_NO_ASC_SIZE   6
#define TRACE_NO_BCD_SIZE   ((TRACE_NO_ASC_SIZE+1)/2)

#define BATCH_NO_ASC_SIZE   6
#define BATCH_NO_BCD_SIZE   ((BATCH_NO_ASC_SIZE+1)/2)

#define	EMV_TAC_SIZE            6
#define	EMV_MAX_TDOL_SIZE	    65
#define	EMV_MAX_DDOL_SIZE	    65
#define	EMV_COUNTRY_CODE_SIZE	3
#define	EMV_CURRENCY_CODE_SIZE	3
#define	EMV_TERM_CAPABILITIES_BCD_SIZE	4
#define EMV_ADD_TERM_CAPABILITIES_BCD_SIZE  6
#define	EMV_TERM_TYPE_SIZE	3
#define	EMV_MERCH_CAT_CODE_SIZE	5
#define EMV_TERM_CAT_CODE_SIZE	3
#define EMV_STRING_SIZE	25
#define TEL_DIGITS 		24				/* BCD   */
#define PABX_BYTES		4
#define PABX_DIGITS		8
#define NII_BYTES               4   //Meena 3 incresed to 4 as strNII size is 4
#define TPDU_BYTES              10   //Meena 3 incresed to 4 as strNII size is 4

#define DF_BATCH_APPEND 0
#define DF_BATCH_UPDATE 1


#define TPDU_ASC_SIZE              10
#define TPDU_BCD_SIZE           ((TPDU_ASC_SIZE+1)/2)

#define PRO_CODE_ASC_SIZE       6
#define PRO_CODE_BCD_SIZE       ((PRO_CODE_ASC_SIZE+1)/2)

#define MTI_ASC_SIZE            4
#define MTI_BCD_SIZE            ((MTI_ASC_SIZE+1)/2)

#define INVOICE_ASC_SIZE        6
#define INVOICE_BCD_SIZE        ((INVOICE_ASC_SIZE+1)/2)

#define DATE_ASC_SIZE                     4
#define DATE_BCD_SIZE                      ((DATE_ASC_SIZE+1)/2)

#define TIME_ASC_SIZE                     6
#define TIME_BCD_SIZE                      ((TIME_ASC_SIZE+1)/2)

#define EXPIRY_DATE_ASC_SIZE              4
#define EXPIRY_DATE_BCD_SIZE               ((EXPIRY_DATE_ASC_SIZE+1)/2)

#define AMT_ASC_SIZE                      12/* BCD - includes cents */
#define AMT_BCD_SIZE                       ((AMT_ASC_SIZE+1)/2)

#define GET_CARD_DATA_TIMEOUT_VALUE  60*100 //60 seconds
#define UI_TIMEOUT                   30*100

#define CARD_ENTRY_MSR                  1
#define CARD_ENTRY_MANUAL               2
#define CARD_ENTRY_ICC                  3
#define CARD_ENTRY_FALLBACK             4
#define CARD_ENTRY_WAVE                 5
#define CARD_ENTRY_MANUAL2               6

    
#define READ_CARD_TIMEOUT              -1001
#define USER_ABORT                     -1002
#define INVALID_CARD                   -1003
#define PLS_INSERT_CARD                -1004
#define MSR_NOT_ALLOW                  -1005
#define MANUAL_NOT_ALLOW               -1006
#define HOST_NOT_OPEN                  -1007
#define RECORD_INVALID                 -1008
#define CARD_EXPIRED                   -1009
#define TIME_OUT                       -1010
#define MODEM_FAILED                   -1011
#define BANCNET_APPL_NOT_AVAIL         -1012
#define DCC_TIMEOUT                    -1013



#define CDT_AMEX                    1
#define CDT_VISA                    2
#define CDT_MASTER                  3
#define CDT_VISA1                   4
#define CDT_MASTER1                 5
#define CDT_VISA2                   6  // To include multiple host for same card type - Meena 03/01/13 
#define CDT_MASTER2                 7  // To include multiple host for same card type - Meena 03/01/13
    
#define SIGN_ON_HOST                0
#define MAIN_HOST                   1
#define MOTO_HOST                   3

#define PWDTYPE_TERMINAL	1
#define PWDTYPE_SETTLEMENT	2
#define PWDTYPE_VOID		3
#define PWDTYPE_TLE		4
    
#define d_MEGSTRIPE_SIZE   128
    
#define INTERACTTIMEOUT 3000 
#define DECIMAL_POINT_MODERATOR 10000    
    
#define GPRS_TIMEOUT_DISABLE	0
#define GPRS_TIMEOUT_ENABLE_VALUE  1500    

#define CURRENT_ACCOUNT 1
#define SAVINGS_ACCOUNT 2

#define DCC_LOCAL_OPT 1
#define DCC_PRINT_OPT 2


/* MCC: Added settlement reconciliation return state -- jzg */
#define ST_CLS_BATCH_ERR (-6)
//int inAccountType;

typedef struct
{
    BYTE byCardTypeNum;
    BYTE byEMVTransStatus;
    BYTE T5A_len;    // PAN_len
    BYTE T5A[10];    // PAN    
    BYTE T5F2A[2];    // Currency Code
    BYTE T5F30[3];        // Service code.
    BYTE T5F34;        // PAN SEQ No.
    BYTE T5F34_len;        // PAN SEQ No len.
    BYTE T82[2];        // Application Interchange Profile (AIP)
    BYTE T84_len;    // AID Len
    BYTE T84[16];    // AID
    BYTE T8A[2];
    BYTE T91[16];    // Issuer Application data (IAD)
    BYTE T91Len;     //IAD length
    BYTE T95[5];        // Terminal Verification Results
    BYTE T9A[3];        // Transaction Date
    BYTE T9C;        // Trans Type
    BYTE T9F02[6];    // Amount, Authorised (Numeric)
    BYTE T9F03[6];    // Amount, others (Numeric)
    BYTE T9F09[2];    // ICC Application Version No.
    BYTE T9F10_len;
    BYTE T9F10[32];    // Issuer Application data
    BYTE T9F1A[2];    // Terminal Country Code
    BYTE T9F26[8];    // Application Cryptogram (AC)
    BYTE T9F27;        // Cryptogram Information Data (CID)
    BYTE T9F33[3];    // Terminal Capabilities
    BYTE T9F34[3];    // Cardholder Verification Method (CVM) Results
    BYTE T9F35;        // Terminal Type
    BYTE T9F36_len;
    BYTE T9F36[2];    // Application Transaction Counter (ATC)
    BYTE T9F37[4];    // Unpredictable Number, (random number)
	//Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - start -- jzg
    BYTE T9F41[3];    // Original Trace No (Field 11)
//    BYTE T9F41[4];    // Original Trace No (Field 11) -- for emv online PIN enhancements
	//Metrobank ADVT 6.1: Fix for VISA net not responding to terminal request - end -- jzg
    BYTE T9F53;        // TCC,
    BYTE ISR[16];        // Issuer Script Result.
    BYTE ISRLen;        // Issuer Script Result.
    BYTE T9B[2];        // Transaction Status Information
    BYTE T5F24[3];    // Application Expiration Date(YYMMDD)
      
    BYTE T71Len;    // Application Interchange Profile (AIP)
    BYTE T71[258];    // Application Interchange Profile (AIP)
    BYTE T72Len;    // Application Interchange Profile (AIP)
    BYTE T72[258];    // Application Interchange Profile (AIP)
    BYTE T9F06[3];  // AID
    BYTE T9F1E[8];  // terminal hardware serial number
    BYTE T9F28[8];  // ARQC
    BYTE T9F29[8];  // TC
    BYTE szChipLabel[32];  // AP label    
    BYTE T9F6E_len;
    BYTE T9F6E[7+1];        //visa form factor indicator
    BYTE T9F42[3];     //DCC Currency 
}emvinfo_t;


typedef struct
{
    BYTE        byTransType;
    BYTE        byPanLen;
    BYTE        szExpireDate[EXPIRY_DATE_BCD_SIZE+1];    /* BCD YYMM        */
    BYTE        byEntryMode;
    BYTE        szTotalAmount[AMT_BCD_SIZE+1];   /* BCD total   amount    */
    BYTE        szBaseAmount[AMT_BCD_SIZE+1];
    BYTE        szTipAmount[AMT_BCD_SIZE+1];
    BYTE        szTID[TERMINAL_ID_BYTES+1];
    BYTE        szMID[MERCHANT_ID_BYTES+1];
    BYTE        szHostLabel[16];
    BYTE        szBatchNo[BATCH_NO_BCD_SIZE+1];
    BYTE        byOrgTransType ;
    BYTE        szMacBlock[8] ;
    BYTE        szYear[2];                 /* int 1997=97,2003=103    */
    BYTE        szDate[DATE_BCD_SIZE+1];     /* BCD MMDD        */
    BYTE        szTime[TIME_BCD_SIZE+1];     /* BCD hhmmss        */
    BYTE        szOrgDate[DATE_BCD_SIZE+1];     /* BCD MMDD        */
    BYTE        szOrgTime[TIME_BCD_SIZE+1];     /* BCD hhmmss        */
    BYTE        szAuthCode[AUTH_CODE_DIGITS+1];
    BYTE        szRRN[RRN_BYTES+1];
    BYTE        szInvoiceNo[INVOICE_BCD_SIZE+1];
    BYTE        szOrgInvoiceNo[INVOICE_BCD_SIZE+1];
    BYTE        byPrintType;
    BYTE        byVoided;
    BYTE        byAdjusted;
    BYTE        byUploaded;
    BYTE        byTCuploaded;    /* For upload TC*/
    BYTE        szCardholderName[CARD_HOLD_NAME_DIGITS+1];
    BYTE        szzAMEX4DBC[4+1];
    BYTE        szStoreID[STORE_ID_DIGITS+1]; //use this one to store how much amount fill up in DE4 for VOID
    BYTE        szRespCode[RESP_CODE_SIZE+1];
    BYTE        szServiceCode[3+1];
    BYTE        byContinueTrans;
    BYTE        byOffline;
    BYTE        byReversal;
    BYTE        byEMVFallBack;
    SHORT       shTransResult;
    BYTE        szTpdu[TPDU_ASC_SIZE+1];
    BYTE        szIsoField03[PRO_CODE_BCD_SIZE+1];
    BYTE        szMassageType[MTI_BCD_SIZE+1];
    BYTE        szPAN[PAN_SIZE+1];
    BYTE        szCardLable[20+1];
    USHORT      usTrack1Len;
    USHORT      usTrack2Len;
    USHORT      usTrack3Len;
    BYTE        szTrack1Data[TRACK_I_BYTES+1];    
    BYTE        szTrack2Data[TRACK_II_BYTES+1];
    BYTE        szTrack3Data[TRACK_III_BYTES+1];
	USHORT      usChipDataLen;		//Chip Data
	BYTE        baChipData[CHIP_DATA_LEN+1];
	USHORT      usAdditionalDataLen;	//Additional Data
	BYTE        baAdditionalData[ADD_DATA_LEN+1];
	BYTE		bWaveSID;
	USHORT		usWaveSTransResult;	// Transction Result
	BYTE		bWaveSCVMAnalysis;
    ULONG       ulTraceNum; 
    ULONG       ulOrgTraceNum;
    USHORT      usTerminalCommunicationMode;
    BYTE        IITid;
    BYTE        HDTid; //kobe added
    ULONG       ulSavedIndex;// kobea added  
    BYTE        byPINEntryCapability;
    BYTE        byPackType;
	int  		inCardType;
	UINT 		MITid;
    BYTE        szOrgAmount[AMT_BCD_SIZE+1];
    BYTE        szCVV2[CVV2_BYTES+1];
    BYTE        byTCFailUpCnt;//20121204
    BYTE		szPINBlock[8+1];
    SHORT  		CDTid;
    emvinfo_t   stEMVinfo;
	BYTE 	szTerms[4+1];
	BYTE 	szTerms1[4+1];	
	BYTE 	szFactorRate[12+1];
	BYTE        szMoAmort[12+1];
	BYTE        szTotalInstAmnt[12+1];
        BYTE 	szPlanID[10+1];
	BYTE 	szPlanType[1+1];
	BYTE 	szTermsEX[4+1];
	BYTE 	szOffsetPeriod[4+1];
	BYTE 	szOffsetType[1+1];
	BYTE 	szAdjustedAmount[12+1];
	BYTE 	szInterestAmount[12+1];
	BYTE 	szFeeAmount[12+1];
	BYTE 	szTaxAmount[12+1];
	BYTE 	szIntFeeAmount[12+1];
	BYTE 	szAPR[12+1];
	BYTE 	szWaveFromPeriod[4+1];
	BYTE 	szWaveToPeriod[4+1];
	
	BYTE 		szPromo[2+1];	
	BYTE 		szInstallmentData[129+1];
	BOOL 	    fInstallment;
	BOOL 	    fCash2Go;
	BOOL		fTipAllowFlag;
	
	int         inAccountType;
	BOOL 	    fLogon;
	BYTE 		szProductCode[16+1];
	BYTE 		szCustomerNo[16+1];
	BYTE 		szLoyaltyData[180+1];
	BOOL 	    fLoyalty;
	BOOL 		fDebit;	
	BOOL 		fCredit;
	BOOL		fEMVPIN; //EMV: Online PIN enchancement -- jzg	
	BOOL 	fRegPHP;
	BOOL	fRegUSD;
        BYTE        szOrgTipAmount[AMT_BCD_SIZE+1];

		// installment data [90] request
	BYTE 	szPromoLabel[20+1];
	BYTE 	szOffsetPeriodEX[4+1];		
        BYTE		szFundIndicator[1+1];
        BYTE	       szPlanID1[10+1];
        BYTE		szPlanGroup[10+1];
        BYTE		szPlanGroupIDX[5+1];

	// loyalty Account Type 1
   	BYTE	szAccount1[2+1];
	BYTE szAmount1Curr[3+1];
	BYTE szAmount1type[1+1];
	BYTE szDrCrFlag1[1+1];
	BYTE szAmount1[12+1];
	BYTE szFiller1[11+1];

	// loyalty Account Type 2
   	BYTE	szAccount2[2+1];
	BYTE szAmount2Curr[3+1];
	BYTE szAmount2type[1+1];
	BYTE szDrCrFlag2[1+1];
	BYTE szAmount2[12+1];
	BYTE szFiller2[11+1];

	// loyalty Account Type 3
   	BYTE	szAccount3[2+1];
	BYTE szAmount3Curr[3+1];
	BYTE szAmount3type[1+1];
	BYTE szDrCrFlag3[1+1];
	BYTE szAmount3[12+1];
	BYTE szFiller3[11+1];

	// loyalty Account Type 4
   	BYTE	szAccount4[2+1];
	BYTE szAmount4Curr[3+1];
	BYTE szAmount4type[1+1];
	BYTE szDrCrFlag4[1+1];
	BYTE szAmount4[12+1];
	BYTE szFiller4[11+1];

	// loyalty Account Type 5
   	BYTE	szAccount5[2+1];
	BYTE szAmount5Curr[3+1];
	BYTE szAmount5type[1+1];
	BYTE szDrCrFlag5[1+1];
	BYTE szAmount5[12+1];
	BYTE szFiller5[11+1];

	// loyalty Account Type 6
   	BYTE	szAccount6[2+1];
	BYTE szAmount6Curr[3+1];
	BYTE szAmount6type[1+1];
	BYTE szDrCrFlag6[1+1];
	BYTE szAmount6[12+1];
	BYTE szFiller6[11+1];


	//transactions type
    	BYTE	szBeginPointBalance[12+1];
	BYTE szRedeemPoint[12+1];
	BYTE szPointBalance[12+1];
	BYTE szSalesAmount[12+1];
	BYTE szRedeemAmount[12+1];
	BYTE szNetSaleAmount[12+1];


	//BYTE szKSN[20+1];
	BYTE szKSN[10+1];


	// Debit response data
	BYTE szDebitData[17+1];
	BYTE szBalCurrency[3+1];
	BYTE szSpace[1+1];
	BYTE szBalSign[1+1];
	BYTE szBalAmount[12+1];

	BOOL fManualSettle;
    	SHORT  		CDTid2;
    BYTE        fTipUploaded;
		
	int			inTipAdjCount;
	
    BYTE szResponseText[40+1];
	BYTE szMerchantName[69+1];
	BOOL fECRTxnFlg;	
	BOOL fEMVPINEntered; //Pin entered	
	BOOL fCUPPINEntry;

	// Mercury Requirements -- sidumili
	BYTE szECRMessage1[40+1];
	BYTE szECRMessage2[40+1];
	BYTE szECRMessage3[40+1];
	BYTE szECRMessage4[40+1];

	BYTE byCeillingAmt;
	BYTE szTempDate[DATE_BCD_SIZE+1];     /* BCD MMDD        */
	BYTE szTempTime[TIME_BCD_SIZE+1];     /* BCD hhmmss        */

	int inSelectedIdleAppsHost;
	int INSHDTid;
         BYTE szTranYear[4 + 1];
         BYTE szForeignAmount[12+1];
         BYTE szDCCOffered[10+1];
         BYTE szCurrencyCode[3+1];
         BYTE szExchangeRate[12+1];
         BYTE szFXSourceName[100+1];
         BYTE szExchangeRateSourceStamp[20+1];
         BYTE szMarginRatePercentage[10+1];
         BYTE szCommisionPercentage[10+1];
         BYTE szValidHours[10+1];
         BYTE szDCCCurrencySymbol[3+1];
         int       inMinorUnit;
         BOOL fDCCOptin;	
         int        inExchangeRate_len;
         int        inCSTid;
         BYTE  szDCCTipAmount[12+1];
         BYTE  szBaseForeignAmount[12+1];
         char szAmountFormat[18+1];
		 BOOL fDCCEnable;
		 BOOL fEMVPINBYPASS;
		  BOOL fAutoSettleFlag;
		  BOOL fBRVoidFlag;
		  BYTE szForeignRate[12+1];
		 BOOL fBit45ONSwipe;

		 BYTE szECRPANFormatted[PAN_SIZE+1];
         BYTE szECRRespText[40 + 1];
         BYTE szECRMerchantName[69 + 1];
         BYTE szECRRespCode[2 + 1];
		 BOOL fBillsPaymentCash;
}TRANS_DATA_TABLE;
	
TRANS_DATA_TABLE srTransRec;

typedef struct
{
    BYTE        szISOLogData[1000];
	BYTE        szISOLogTransType[50];
	BYTE        szISOLogDateTime[50];
    ULONG       ulSavedIndex;// kobea added  
		 
}TRANS_ISOLOG_TABLE;
	
TRANS_ISOLOG_TABLE srTransISOLog;



//sidumili
#define POSITION_RIGHT 0
#define POSITION_LEFT 1
#define IS_SPACE 0x20
#define IS_ZERO 0x30
#define IS_NULL_CH ((char) 0)
//sidumili

#define ECR_APPROVED			"00"
#define ECR_DECLINED_ERR		"ND"
#define ECR_REFERAL_ERR			"01"
#define ECR_TIMEOUT_ERR			"TO"
#define ECR_CANNOT_ERR			"ED"
#define ECR_COMMS_ERR			"EN"
#define ECR_UNKNOWN_ERR			"NA"
#define ECR_APPROVED_RESP		"APPROVED"
#define ECR_OPER_CANCEL_RESP    "OPERATOR CANCEL"
#define ECR_EXPIRED_CARD_RESP	"CARD EXPIRED"
#define ECR_AMOUNT_ERR_RESP	    "OUT OF RANGE"
#define ECR_RECORD_VOIDED       "RECORD VOIDED"


#define	ECR_BINVER 		  1
#define	ECR_SALE   		  2
#define	ECR_INSTALLMENT   3
#define ECR_WALLET        4
#define ECR_BDOPAY        5

#define ECR_TIMEOUT_RESP    	"TIMEOUT"
#define ECR_COMM_ERROR_RESP    	"COMM ERROR"




#define RGB(r,g,b) (unsigned long)( ((DWORD)(BYTE)r&0xff)|((DWORD)((BYTE)g&0xff)<<8)|((DWORD)((BYTE)b&0xff)<<16) )

/* <<< end*/

//#define _TRANS_CONNECT_USE_RS232_

#define MCC_HOST 19
#define DINERS_HOST 6
#define BANCNET_HOST 2 //change from 12 to 2
#define WECHAT_HOST 20
#define ALIPAY_HOST 21
#define GCASH_HOST 22
#define GRAB_HOST 23
#define UPI_HOST 24
#define PHQR_HOST 25




#define VISA_ISSUER 1
#define MASTERCARD_ISSUER 2
#define AMEX_ISSUER 3
#define JCB_ISSUER 4
#define BANCNET_ISSUER 6
#define UPI_ISSUER 8

/* Authentication Type -- sidumili*/
#define AUTH_TYPE_AUTO       0
#define AUTH_TYPE_PAP        1
#define AUTH_TYPE_CHAP       2

// FOR ECR
#define ECR_USB_TERMINAL  8
#define ECR_USB_BASE 	  7
#define ECR_COM1		  1
#define ECR_COM2		  2
#define ECR_NONE		  0

// For Password Length
#define PWD_MAX_LEN       6
#define PWD_MIN_LEN       4

#define V5S_MCCNH "com.Source.S1_RBANK.RBANK"

// For message type image -- sidumili
#define MSG_TYPE_NONE  	    0
#define MSG_TYPE_SUCCESS    1
#define MSG_TYPE_ERROR      2
#define MSG_TYPE_WARNING    3
#define MSG_TYPE_INFO       4
#define MSG_TYPE_QUESTION   5
#define MSG_TYPE_PRINT      6
#define MSG_TYPE_PROCESS    7
#define MSG_TYPE_SEND       8
#define MSG_TYPE_RECEIVE    9
#define MSG_TYPE_WAIT       10
#define MSG_TYPE_TIMEOUT    11


#define MSG_PLS_WAIT       "Please wait..."
#define MSG_PROCESSING     "Processing..."

#define EDIT_FILE   	   "/data/data/pub/editfile.dat"
#define FLD_TYPE_STRING    0
#define FLD_TYPE_NUMERIC   1
#define FLD_TYPE_BOOLEAN   2
#define DELIM_CARET        "^"
#define DELIM_PIPE         "|"

//#define ERM_MODE
#define TRACK_II_BYTES_50          50
#define MANUAL_DOWNLOAD				1
#define AUTO_DOWNLOAD				2

#define DATE 1
#define TIME 2

#define d_MORE_PROCESSING					0x0010
#define d_USER_CANCEL						0x0020
#define d_TIMEOUT							0x0030


#define ST_CLS_BATCH_ERR (-6)

//Error if Comms was disconnected during receiving part
#define ST_COMMS_DISCONNECT (-7)

/* POC: 2 dip online pin - start -- jzg*/
#define ST_RESP_DO_ONLINE_PIN (-8)
/* POC: 2 dip online pin - end -- jzg*/

#define ST_NO_CONNECT_ERR (-9) //aaronnino for BDOCLG ver 9.0 fix on issue #00460 Settle failed retries should not count terminal level connection error
#define ST_REVERSAL_SETTLE_ERR (-10) //aaronnino for BDOCLG ver 9.0 fix on issue #00500 and #00501 

#define ST_CONNECT_FAILED (-11)

#define ST_RECEIVE_TIMEOUT_ERR    (-12)
#define CTLS_ANALYZE_ERROR    (-14)
#define CTLS_EXPIRED_CARD		(-15)
#define ADVICE_FAILED (-16)
#define TRANSLOG_FAILED (-17)



#define SUPER_ENGINEER_PWD 1
#define ENGINEER_PWD 2
#define SUPER_PWD 3

#define BIN_ROUTING    0

#ifdef    __cplusplus
}
#endif

#endif    /* TYPEDEF_H */

