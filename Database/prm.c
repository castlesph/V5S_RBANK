/** 
**    A Template for developing new terminal shared application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <sqlite3.h>

#include "../Database/DatabaseFunc.h"
#include "../Includes/POSTypedef.h"
#include "../FileModule/myFileFunc.h"

extern sqlite3 * db;
extern sqlite3_stmt *stmt;
extern int inStmtSeq;

extern void vdDebug_LogPrintf(const char * fmt,...);

int inPRMReadbyHDTid(int inHDTid, int *inNumRecs)
{     
	int result = 0,
		inCount = 0;
	char *sql = "SELECT PRMid,HDTid,szPromoLabel,szPromoCode, szTermsEX, szOffsetPeriod, szFundIndicator, szPlanID1, szPlanGroup, szPlanGroupIDX FROM PRM3 WHERE HDTid=?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inHDTid);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do 
	{
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) /* can read data */
		{ 
			inStmtSeq = 0;
			strMultiPRM[inCount].PRMid = sqlite3_column_int(stmt, inStmtSeq);
			strMultiPRM[inCount].HDTid = sqlite3_column_int(stmt, inStmtSeq+=1);
			strcpy((char *)strMultiPRM[inCount].szPromoLabel,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szPromoCode,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szTermsEX,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));			
			strcpy((char *)strMultiPRM[inCount].szOffsetPeriod,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szFundIndicator,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szPlanID1,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szPlanGroup,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szPlanGroupIDX,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			
			inCount++;
			if (inCount>20)
				break;
		}
	} 
	while (result == SQLITE_ROW);
	
	*inNumRecs = inCount;
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return(d_OK);
}

int inPRMReadbyPRMid(int inPRMid)
{     
	int result = 0;
	char *sql = "SELECT HDTid,szPromoLabel,szPromoCode, szTermsEX, szOffsetPeriod, szFundIndicator, szPlanID1, szPlanGroup, szPlanGroupIDX FROM PRM3 WHERE PRMid=?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inPRMid);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do 
	{
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) /* can read data */
		{ 
			inStmtSeq = 0;
			strPRM.HDTid = sqlite3_column_int(stmt, inStmtSeq);
			strcpy((char *)strPRM.szPromoLabel,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strPRM.szPromoCode,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strPRM.szTermsEX,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));			
			strcpy((char *)strPRM.szOffsetPeriod,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strPRM.szFundIndicator,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strPRM.szPlanID1,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strPRM.szPlanGroup,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strPRM.szPlanGroupIDX,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			
			
			
		}
	} 
	while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return(d_OK);
}


int inPRMSave(int inSeekCnt)
{
	int result = 0;
	char *sql = "UPDATE PRM3 SET PRMid=?,HDTid=?,szPromoLabel=?,szPromoCode=?, szTermsEX=?, szOffsetPeriod=?, szFundIndicator=?, szPlanID1=?, szPlanGroup=?, szPlanGroupIDX=? WHERE PRMid=?";

 	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	/* prepare the sql, leave stmt ready for loop */
	sqlite3_exec( db, "begin", 0, 0, NULL );
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}
	
	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPRM.PRMid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPRM.HDTid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szPromoLabel, strlen((char*)strPRM.szPromoLabel), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szPromoCode, strlen((char*)strPRM.szPromoCode), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szTermsEX, strlen((char*)strPRM.szTermsEX), SQLITE_STATIC);	
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szOffsetPeriod, strlen((char*)strPRM.szOffsetPeriod), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szFundIndicator, strlen((char*)strPRM.szFundIndicator), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szPlanID1, strlen((char*)strPRM.szPlanID1), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szPlanGroup, strlen((char*)strPRM.szPlanGroup), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPRM.szPlanGroupIDX, strlen((char*)strPRM.szPlanGroupIDX), SQLITE_STATIC);
	
	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE )
	{
		sqlite3_close(db);
		return 3;
	}  
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	CTOS_Delay(300);
	
	return(d_OK);
}

int inPRMNumRecord(void)
{
	int len = 0;
	int result = 0;
	char *sql = "SELECT COUNT(*) FROM PRM3";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do 
	{
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) /* can read data */
		{ 
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} 
	while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return(inCount);
}


int inPRMReadbyinInstGroup(int inGroup, int *inNumRecs)
{     
	int result = 0,
		inCount = 0;
	char *sql = "SELECT PRMid,HDTid,szPromoLabel,szPromoCode, inGroup, szTermsEX, szOffsetPeriod, szFundIndicator, szPlanID1, szPlanGroup, szPlanGroupIDX, INSHDTid FROM PRM3 WHERE inGroup=? and fPRMEnable=1";


	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);

	
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inGroup);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do 
	{
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) /* can read data */
		{ 
			inStmtSeq = 0;
			strMultiPRM[inCount].PRMid = sqlite3_column_int(stmt, inStmtSeq);
			strMultiPRM[inCount].HDTid = sqlite3_column_int(stmt, inStmtSeq+=1);
			strcpy((char *)strMultiPRM[inCount].szPromoLabel,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szPromoCode,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strMultiPRM[inCount].inGroup = sqlite3_column_int(stmt, inStmtSeq+=1);
			strcpy((char *)strMultiPRM[inCount].szTermsEX,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));					
			strcpy((char *)strMultiPRM[inCount].szOffsetPeriod,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM[inCount].szFundIndicator,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM[inCount].szPlanID1,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM[inCount].szPlanGroup,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM[inCount].szPlanGroupIDX,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strMultiPRM[inCount].INSHDTid = sqlite3_column_int(stmt, inStmtSeq+=1);
			
			inCount++;
			if (inCount>20)
				break;
		}
	} 
	while (result == SQLITE_ROW);
	
	*inNumRecs = inCount;
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	
	return(d_OK);
}


int inPRMReadbyinInstGroup2(int inGroup, int *inNumRecs)
{     
	int result = 0,
		inCount = 0;
	char *sql = "SELECT PRMid,HDTid,szPromoLabel,szPromoCode, inGroup, szTermsEX, szOffsetPeriodEX, szFundIndicator, szPlanID1, szPlanGroup, szPlanGroupIDX FROM PRM2 WHERE inGroup=?";

    	//CTOS_PrinterPutString("inPRMReadbyinInstGroup2");


	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);

	
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inGroup);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do 
	{
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) /* can read data */
		{ 
			inStmtSeq = 0;
			strMultiPRM2[inCount].PRMid2= sqlite3_column_int(stmt, inStmtSeq);
			strMultiPRM2[inCount].HDTid2 = sqlite3_column_int(stmt, inStmtSeq+=1);
			strcpy((char *)strMultiPRM2[inCount].szPromoLabel2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM2[inCount].szPromoCode2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strMultiPRM2[inCount].inGroup2 = sqlite3_column_int(stmt, inStmtSeq+=1);
			strcpy((char *)strMultiPRM2[inCount].szTermsEX2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));					
			strcpy((char *)strMultiPRM2[inCount].szOffsetPeriod2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM2[inCount].szFundIndicator2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM2[inCount].szPlanID12,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM2[inCount].szPlanGroup2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strcpy((char *)strMultiPRM2[inCount].szPlanGroupIDX2,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
				
			inCount++;
			if (inCount>50)
				break;
		}
	} 
	while (result == SQLITE_ROW);
	
	*inNumRecs = inCount;
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	
	return(d_OK);
}

int inPRMReadbyinInstGroupOldHost(int inGroup, int *inNumRecs)
{     
	int result = 0,
		inCount = 0;
	//char *sql = "SELECT PRMid,HDTid,szPromoLabel,szPromoCode, inGroup FROM PRM WHERE inGroup=?";
	char *sql = "SELECT PRMid,HDTid,szPromoLabel,szPromoCode, inGroup FROM PRM WHERE inGroup=? and fPRMEnable=1";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) 
	{
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inGroup);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do 
	{
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) /* can read data */
		{ 
			inStmtSeq = 0;
			strMultiPRM[inCount].PRMid = sqlite3_column_int(stmt, inStmtSeq);
			strMultiPRM[inCount].HDTid = sqlite3_column_int(stmt, inStmtSeq+=1);
			strcpy((char *)strMultiPRM[inCount].szPromoLabel,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			strcpy((char *)strMultiPRM[inCount].szPromoCode,  (char *)sqlite3_column_text(stmt,inStmtSeq +=1));		
			strMultiPRM[inCount].inGroup = sqlite3_column_int(stmt, inStmtSeq+=1);
			inCount++;
			if (inCount>20)
				break;
		}
	} 
	while (result == SQLITE_ROW);
	
	*inNumRecs = inCount;
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	
	return(d_OK);
}


