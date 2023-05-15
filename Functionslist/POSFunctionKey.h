
#ifndef POSFUNCTIONKEY_H
#define POSFUNCTIONKEY_H

#define SUPER_ENGINEER_PW 1
#define ENGINEER_PW 2
#define SUPER_PW 3

void vdCTOS_FunctionKey(void);
void vdDisplayHostDetails(void);
void vdSTANNo(BOOL fGetSTABNo);
void vdTraceNo(BOOL fGetTraceNo);
void vdBatchNo(BOOL fGetBatchNo);
void vdShareCom(BOOL fGetShareCom);

extern int inCTOS_CheckAndSelectMutipleMID(void);
extern int inCTOS_BATCH_TOTAL_Process(void);
extern int inCTOS_BATCH_REVIEW_Process(void);
extern USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS);
extern unsigned int wub_hex_2_str(unsigned char *hex, unsigned char *str, unsigned int len);
extern unsigned long wub_bcd_2_long(unsigned char *buf, unsigned char len);
extern int inAscii2Bcd(BYTE *szAscii, BYTE *szBcd, int inBcdLen);
void vdCTOS_DevFunctionKey(void);
int inCTOS_CheckHardware(int inHW);
void vdCTOS_ManualConfigureCTLSReader(void);
int inCheckPromptHW(int inCommMode); // Check / Prompt Hardware -- sidumili
void vdSetERMMode(void);
void vdSetPabx(void);
void vdMenuidNHProfileMenu(BOOL fTerminalType);

#endif //end POSFUNCTIONKEY_H

