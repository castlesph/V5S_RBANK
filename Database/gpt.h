

typedef struct
{
	UINT 	GPTid;
	BYTE	szGPTName[30] ;
	BYTE	szGPTAPN[50] ;
	BYTE	szGPTUserName[30] ;
	BYTE	szGPTPassword[30] ;
	BYTE	szGPTPriTxnHostIP[30];
	BYTE	szGPTSecTxnHostIP[30] ;
	BYTE	szGPTPriSettlementHostIP[30];
	BYTE	szGPTSecSettlementHostIP[30];
	int	inGPTPriTxnHostPortNum;
	int	inGPTSecTxnHostPortNum;
	int	inGPTPriSettlementHostPort;
	int	inGPTSecSettlementHostPort;
	int	inGPTAuthType;
	int	inGPTNetworkType;
	BOOL fGPTEnable;

}STRUCT_GPT;

STRUCT_GPT strGPT;
STRUCT_GPT strMultiGPT[20];

int inGPTRead(int inSeekCnt);
int inGPTMultiRead(int *inNumRecs);
int inGPTSave(int inSeekCnt);
int inGPTNumRecord(void);


