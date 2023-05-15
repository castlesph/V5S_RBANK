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

#include "..\Debug\Debug.h"

extern sqlite3 * db;
extern sqlite3_stmt *stmt;
extern int inStmtSeq;



int inINSNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM INS";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inINSRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "select inHDTid, fINSEnable, fTerm1Enable, fTerm2Enable, fTerm3Enable, fTerm4Enable, fTerm5Enable, fTerm6Enable, fTerm7Enable, fTerm8Enable, fTerm9Enable, fTerm10Enable, fTerm11Enable, fTerm12Enable, szTerm1, szTerm2, szTerm3, szTerm4, szTerm5, szTerm6, szTerm7, szTerm8, szTerm9, szTerm10, szTerm11, szTerm12 from INS where inHDTid = ?";
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /* HDTid */
            strINS.inHDTid = sqlite3_column_int(stmt, inStmtSeq );   
            strINS.fINSEnable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            
            strINS.fTerm1Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm2Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm3Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm4Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm5Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm6Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm7Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm8Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm9Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm10Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm11Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm12Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  

            strcpy((char*)strINS.szTerm1, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm2, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm3, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm4, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm5, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm6, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm7, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm8, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm9, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm10, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm11, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm12, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));

   		}
	} while (result == SQLITE_ROW);	

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inINSSave(int inSeekCnt)
{   
    return d_OK;
}

int inUSRRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "select inHDTid, fINSEnable, fTerm1Enable, fTerm2Enable, fTerm3Enable, fTerm4Enable, fTerm5Enable, fTerm6Enable, fTerm7Enable, fTerm8Enable, fTerm9Enable, fTerm10Enable, fTerm11Enable, fTerm12Enable, szTerm1, szTerm2, szTerm3, szTerm4, szTerm5, szTerm6, szTerm7, szTerm8, szTerm9, szTerm10, szTerm11, szTerm12 from INS where inHDTid = ?";
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /* HDTid */
            strINS.inHDTid = sqlite3_column_int(stmt, inStmtSeq );   
            strINS.fINSEnable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            
            strINS.fTerm1Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm2Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm3Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm4Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm5Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm6Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm7Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm8Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm9Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm10Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm11Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  
            strINS.fTerm12Enable= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));  

            strcpy((char*)strINS.szTerm1, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm2, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm3, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm4, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm5, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm6, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm7, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm8, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm9, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm10, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm11, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            strcpy((char*)strINS.szTerm12, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));

   		}
	} while (result == SQLITE_ROW);	

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


