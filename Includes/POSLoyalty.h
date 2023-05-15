
#ifndef ___POS_LOYALTY___
#define ___POS_LOYALTY___

int inCTOS_Redeem5050FlowProcess(void);
int inCTOS_Redeem5050(void);
int inCTOS_RedeemVariableFlowProcess(void);
int inCTOS_RedeemVariable(void);
int inCTOS_LoyaltyBalanceInquiryFlowProcess(void);
int inCTOS_LoyaltyBalanceInquiry(void);
int inEnterProductCode(void);
int inEnterRedeemAmount(void);
int inEnterCustomerNumber(void);

struct LOYALTY_DATA
{
    char szSaleAmount[12+1]; //Sale amount - 12 bytes
    char szRedeemAmountAmount[12+1]; //Redeem amount - 12 bytes
    char szNetSalesAmount[12+1]; //Net Sales amount - 12 bytes
    char szRedeemPoints[11+1]; //Redeem Points - 11 bytes
    char szAwardPoints[11+1]; //Award Points - 11 bytes
    char szPointBalance[11+1]; //Point Balance - 11 bytes
    char szProductCode[2+1]; //Product Code - 2 bytes
    char szBeginPointBalance[10+1]; //Beginning Point Balance - 10 bytes
    char szBeginEPurseBalance[11+1]; //Beginning E-Purse Balance - 11 bytes
    char szEPurseBalance[11+1]; //E-Purse Balance - 11 bytes
    char szTransType[2+1]; //Card Block Code, Transaction Type indicators (default both to 0x20)	
};

void vdExtractLoyaltyData(struct LOYALTY_DATA *stLoyData, char *ptrLoyData);

#endif //end ___POS_LOYALTY___

