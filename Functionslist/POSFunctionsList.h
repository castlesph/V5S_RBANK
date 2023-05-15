
#ifndef POSFUNCTIONSLIST_H
#define POSFUNCTIONSLIST_H

#include "../Includes/POSTypedef.h"
#include "../Includes/V5IsoFunc.h"

#include "../Functionslist/POSFunctionKey.h"
#include "..\Includes\PosDiag.h"


typedef int (*DFUNCTION_LIST)(void);
typedef int (*DFUNCTION_inISOPack)(TRANS_DATA_TABLE *, unsigned char *);
typedef int (*DFUNCTION_inISOUnPack)(TRANS_DATA_TABLE *, unsigned char *);
typedef int (*DFUNCTION_inISOCheck)(TRANS_DATA_TABLE *, unsigned char *, unsigned char *);

typedef struct tagFunc_vdFunc
{
	unsigned char uszFunctionName[100];	
	DFUNCTION_LIST d_FunctionP;
} Func_vdFunc;

typedef struct tagFunc_inISOPack
{
	unsigned char uszFunctionName[100];	
	DFUNCTION_inISOPack d_FunctionP;
} Func_inISOPack;

typedef struct tagFunc_inISOUnPack
{
	unsigned char uszFunctionName[100];	
	DFUNCTION_inISOUnPack d_FunctionP;
} Func_inISOUnPack;

typedef struct tagFunc_inISOCheck
{
	unsigned char uszFunctionName[100];	
	DFUNCTION_inISOCheck d_FunctionP;
} Func_inISOCheck;

int inCTOSS_ExeFunction(char *INuszFunctionName);
int inExeFunction_PackISO(char *INuszFunctionName, unsigned char *uszSendData);
int inExeFunction_UnPackISO(char *INuszFunctionName, unsigned char *uszSendData);
int inExeFunction_CheckISO(char *INuszFunctionName, unsigned char *uszSendData, unsigned char *uszReceiveData);

int inQRPAY(void);
int inCTOS_Retrieve(void);
int inQRPAY(void);
int inCTOS_Retrieve(void);
int inPHQRSale(void);
int inCTOS_PHQRRetreive(void);
int inCTOS_SettlePHQR(void);



#endif //end POSFUNCTIONSLIST_H

