
#ifndef ___POS_DEBIT___
#define ___POS_DEBIT___

int inCTOS_DebitSaleFlowProcess(void);
int inCTOS_DebitSale(void);
int inCTOS_DebitBalanceInquiryFlowProcess(void);
int inCTOS_DebitBalInq(void);
int inCTOS_DebitLogonFlowProcess(void);
int inCTOS_DebitLogon(void);

int inCTOS_DebitSelection(void);
int inCTOS_SelectAccountType(void);
int inCTOS_CheckSelectDebit(void);
int InDisplayBalance(void);
int inCTOS_BancnetSale(void);
int inCTOS_BancnetBalInq(void);
int inCTOS_BancnetCashOut(void);
int inCTOS_DebitCashOutFlowProcess(void);

int inCTOS_CashIn(void);
int inCTOS_CashInFlowProcess(void);
int inCTOS_CashOut(void);
int inCTOS_CashOutFlowProcess(void);
int inCTOS_CardlessBanking(void);

int inCTOS_CardlessCashOut(void);
int inCTOS_CardlessCashOutFlowProcess(void);

int inCTOS_CardlessBalInq(void);
int inCTOS_CardlessBalanceInquiryFlowProcess(void);

int inCTOS_Payment(void);
int inCTOS_PaymentFlowProcess(void);

int inCTOS_FundTransfer(void);
int inCTOS_FundTransferFlowProcess(void);

int inCTOS_RequestTerminalKey(void);

#endif //end ___POS_DEBIT___
