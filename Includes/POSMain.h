
#ifndef ___POS_MAIN_H___
#define ___POS_MAIN_H___

//#define BATTERY_POSTION_X	290
//#define BATTERY_POSTION_Y	4

int inCTOS_DisplayIdleBMP(void);
int inCTOS_DisplayComTypeICO(void);
int inCTOS_ValidFirstIdleKey(void);
BYTE chGetFirstIdleKey(void);
void vdSetFirstIdleKey(BYTE bFirstKey);
int inCTOS_IdleEventProcess(void);
int inCTOSS_InitAP(void);
int inCTOS_ConfigureCommDevice(void);
int inCTOS_ConfigureCTLSReader(void);
void vdCTOSS_ClearTouchPanelTest(void);
void vdBattChargingDisplay(BOOL fCharging, int inPercentage);
int inCheckBatteryStatus(void);

int inCTOSS_JumpToSleepMode(void);
BYTE chGetIdleEvent(void);
void vdSetIdleEvent(BYTE bIdleEvent);
//android-removed
//int inCTOS_InitWIFI(void);
//end

// For Battery
BYTE chGetBatteryTimeOut(void);
void vdSetBatteryTimeOut(BYTE bTimeOut);

// For Signal
BYTE chGetSignalTimeOut(void);
void vdSetSignalTimeOut(BYTE bTimeOut);


int inCheckSignalStatus(void); // Check Signal Value -- sidumili
void vdCheckERMMode(void);
void vdSetGSMAuthType(void);
void vdDisplayLoadingApps(void);
void vdInitUSCDCBMODE(void);
void vdCheckISOLog(void);
void vdSetGSMNetworkType(void);

int inCTOS_APP_START(void);




#endif //end ___POS_MAIN_H___

