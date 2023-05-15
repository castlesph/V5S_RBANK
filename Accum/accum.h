
#ifndef ACCUM_H
#define ACCUM_H

#include "../Includes/Trans.h"
#include "../Includes/POSTypedef.h"
#include "../FileModule/myFileFunc.h"


#define MAX_ISSUERS		20
#define MAX_TOTAL_SETS		2
#define MAX_CURRENCY             40


typedef struct TagACCUM_REC
{
    BYTE        szTID[TERMINAL_ID_BYTES+1];
    BYTE        szMID[MERCHANT_ID_BYTES+1];
    BYTE        szYear[DATE_BCD_SIZE+1];
    BYTE        szDate[DATE_BCD_SIZE+1];
    BYTE        szTime[TIME_BCD_SIZE+1];
    BYTE        szBatchNo[BATCH_NO_BCD_SIZE+1];
	TRANS_TOTAL stBankTotal[MAX_TOTAL_SETS];
	BOOL fManualSettle;	
} ACCUM_REC;

int inCTOS_UpdateAccumTotal(void);
int inCTOS_SaveAccumTotal(ACCUM_REC *strTotal);
int inCTOS_ReadAccumTotal(ACCUM_REC *strTotal);
int inCTOS_ClearAccumTotal(void);

int inCTOS_FileCopy(char *szSourceFile, char *szDesFile,ULONG ulFileSize);

int inCTOS_DeleteBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid);
int inCTOS_ReadBKAccumTotal(ACCUM_REC *strTotal,int inhostIndex, int inMITid);
void vdCTOS_SetBackupAccumFile(char *szOriFileName);
void vdCTOS_GetBackupAccumFile(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal, int HostIndex, int MITid);
void vdCTOS_GetAccumName(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal);

void vdCTOS_GetAccumName(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal);

int inCTOS_UpdateAccumPreCompTotal(TRANS_DATA_TABLE *transData);

#endif


