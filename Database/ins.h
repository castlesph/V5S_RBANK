
typedef struct
{
    UINT    inHDTid;
	BOOL    fINSEnable;
	BOOL    fTerm1Enable;
	BYTE    szTerm1[2+1];
	BOOL    fTerm2Enable;
	BYTE    szTerm2[2+1];
	BOOL    fTerm3Enable;
	BYTE    szTerm3[2+1];
	BOOL    fTerm4Enable;
	BYTE    szTerm4[2+1];
	BOOL    fTerm5Enable;
	BYTE    szTerm5[2+1];
	BOOL    fTerm6Enable;
	BYTE    szTerm6[2+1];
	BOOL    fTerm7Enable;
	BYTE    szTerm7[2+1];
	BOOL    fTerm8Enable;
	BYTE    szTerm8[2+1];
	BOOL    fTerm9Enable;
	BYTE    szTerm9[2+1];
	BOOL    fTerm10Enable;
	BYTE    szTerm10[2+1];
	BOOL    fTerm11Enable;
	BYTE    szTerm11[2+1];
	BOOL    fTerm12Enable;
	BYTE    szTerm12[2+1];
} STRUCT_INS;

STRUCT_INS   strINS;

int inINSNumRecord(void);
int inINSRead(int inSeekCnt);
int inINSSave(int inSeekCnt);

