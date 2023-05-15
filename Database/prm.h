
typedef struct
{
	int PRMid;
	int HDTid;
	char szPromoLabel[21];
	char szPromoCode[3];
	int inGroup;
	char szTermsEX[4];	
	char szOffsetPeriod[4];
	char szFundIndicator[1];
	char szPlanID1[10];
	char szPlanGroup[10];
	char szPlanGroupIDX[5];
	int INSHDTid;
} STRUCT_PRM;

STRUCT_PRM strPRM;
STRUCT_PRM strMultiPRM[20];

typedef struct
{
	int PRMid2;
	int HDTid2;
	char szPromoLabel2[50];
	char szPromoCode2[3];
	int inGroup2;
	char szTermsEX2[4];		
	char szOffsetPeriod2[4];
	char szFundIndicator2[1];
	char szPlanID12[10];
	char szPlanGroup2[10];
	char szPlanGroupIDX2[5];
} STRUCT_PRM2;

STRUCT_PRM2 strPRM2;
STRUCT_PRM2 strMultiPRM2[50]; //[20] - fix system on cash2go and installment if added cash2go data to PRM2 table .


int inPRMReadbyHDTid(int inHDTid, int *inNumRecs);
int inPRMReadbyPRMid(int inPRMid);
int inPRMSave(int inSeekCnt);
int inPRMNumRecord(void);
int inPRMReadbyinInstGroup(int inGroup, int *inNumRecs);
int inPRMReadbyinInstGroup2(int inGroup, int *inNumRecs);

