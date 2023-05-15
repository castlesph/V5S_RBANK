
#ifndef ___POS_HOST___
#define ___POS_HOST___



int inCTOS_GetTxnBaseAmount(void);
int inCTOS_GetTxnTipAmount(void);
int inCTOS_UpdateTxnTotalAmount(void);
int inCTOS_GetInvoice(void);
int inCTOS_CustComputeAndDispTotal(void);

int inCTOS_DisplayResponse(void);
int inCTOS_BatchSearch(void);

int inCTOS_GeneralGetInvoice(void);
int inCTOS_CheckTipadjust(void);
int inCTOS_CheckVOID(void);
VS_BOOL fAmountLessThanFloorLimit(void) ;


int inCTOS_VoidSelectHost(void);
int inCTOS_ChkBatchEmpty(void);
int inCTOS_ConfirmInvAmt(void);
int inCTOS_GetTipAfjustAmount(void);
int inCTOS_GetOffApproveNO(void);
int inCTOS_GetOffRefNoEntry(void);

int inCTOS_SettlementSelectAndLoadHost(void);
int inCTOS_LoadCDTandIIT(void);
int inCTOS_DisplayBatchRecordDetail(int inType, int inNavRecType);
int inCTOS_SettlementClearBathAndAccum(BOOL fManualSettle);

//int inCTOS_DisplayBatchTotal(void);

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo);
int inCTOSS_BatchCheckDuplicateInvoice(void);
int inCTOS_DisplaySettleBatchTotal(BYTE byTransType, BOOL fConfirm);
BYTE InputAmount2(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter);

int inCTOS_ValidateTrxnAmount(void); // mfl

BYTE InputStringAlpha2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS);
BYTE InputString2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS);
int inCTOS_CheckOfflineCeilingAmt(void);
int inNSRFlag(void);
int inCTOS_ConfirmTipAdj(void);
int inCTOS_ChkBatchEmpty_AllHosts(void);
int inPing(void);

#endif //end ___POS_HOST___

