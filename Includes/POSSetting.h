#ifndef ___POS_SETTING_H___
#define ___POS_SETTING_H___

#define STR_HEAD            0
#define STR_BOTTOM          1
#define STR_ALL             2


void vdCTOS_uiPowerOff(void);
void vdCTOS_IPConfig(void);

int inCTOS_SelectHostSetting(void);
void vdCTOS_DialConfig(void);
void vdCTOS_TerminalConfig(void); // mfl

void vdCTOS_ModifyEdcSetting(void);
void vdCTOS_DeleteBatch(void);
//int vdCTOS_DeleteBatch(void);
void vdSetECRConfig(void);

int vdCTOS_DeleteReversal(void);

void vdCTOS_PrintEMVTerminalConfig(void);
int inSelectTelcoSetting();
void vdCTOSS_ModifyStanNumber(void);
void vdCTOS_TMSRangeSetting(void);


void vdCTOS_GPRSSetting(void);
void vdCTOS_Debugmode(void);
void vdCTOSS_CtlsMode(void);
void vdCTOS_TipAllowd(void);
void vdCTOS_DemoMode(void);
void DelCharInStr(char *str, char c, int flag);
void vdConfigEditAddHeader(void);
void vdCTOSS_DownloadMode(void);
void vdCTOSS_CheckMemory(void);
void CTOSS_SetRTC(void);
int inCTOSS_GetCtlsMode(void);
int inCTOS_PromptPassword(void);
void vdCTOS_uiRestart(BOOL fConfirm);
void vdCTOSS_PrintTerminalConfig(void);
void vdCTOSS_SelectPinpadType(void);
void vdCTOSS_InjectMKKey(void);
void vdCTOS_TMSSetting(void);
int  inCTOS_TMSPreConfigSetting(void);
void vdCTOS_ERMConfig(void);
void vdCTOS_ECRConfig(void);
void vdCTOSS_EditTable(void);
void vdCTOS_AuthenticationType(void);
void vdCTOS_PingIPAddress(void);
void vdCTOS_uiIDLESleepMode(void);
int inCTOSS_CheckBatteryChargeStatus(void);
void vdCTOS_uiIDLEWakeUpSleepMode(void);
int vdDisplayIdleEMVApp(int inIdleSaleType);
int inSelectIdleApps(void);
void vdCTOS_TMSSetting2(void);
int inSelectIdleApps2(void);
void put_env_char(char *tag, char *value);
int inCTOS_PromptReprintPassword(void);
void put_env_int(char *tag, int value);
//Tine:  for android terminal
int inSelectIdleTrxn(void);
int vdDisplayIdleEMVTrxn(int inIdleSaleType);
int vdDisplayIdleEMVTrxn2(int inIdleSaleType);
int vdDisplayTrxn(int inSaleType, char *sztrxlogo);
int inCTOS_AutoSelectHostSetting(void);
int vdCTOS_ChangeComm(void);
int vdCTOS_ChangeTelco(void);
int inSelectECRTrxn(void);
int vdCTOS_EditBiller(void);
int vdCTOS_EditUser(void);
void vdCTOS_EditEnvParamDB(void);

int inPing(void);


#endif //end ___POS_SETTING_H___

