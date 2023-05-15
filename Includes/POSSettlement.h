
#ifndef ___POS_SETTLEMENT___
#define ___POS_SETTLEMENT___


int inCTOS_SettlementFlowProcess(void);
int inCTOS_SETTLEMENT(void);
int inCTOS_BATCH_TOTAL(void);
int inCTOS_BATCH_REVIEW(void);
int inCTOS_SETTLE_ALL(void);
int inCTOS_SettleAllOperation(void);
int inCTOS_SettleAllHosts(void);
int inCTOS_SettleBancnet(void);
int inCTOS_SettleDigitalWallet(void);
int inDisplayHostToSettle(char* szHostName);
int inCTOS_SettleDigitalWalletAndPHQR(void);


#endif //end ___POS_SETTLEMENT___

