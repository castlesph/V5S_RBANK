
#ifndef _POSWAVE_H
#define	_POSWAVE_H

#ifdef	__cplusplus
extern "C" {
#endif

int inCTOS_WaveFlowProcess(void);
int inCTOS_WAVE_SALE(void);
int inCTOS_WAVE_REFUND(void);
int inCTOS_MultiAPReloadWaveData(void);
void vdCTOS_InitWaveData(void);
void vdCTOSS_SetWaveTransType(int type);
int inCTOSS_GetWaveTransType(void);
void vdCTOSS_GetALLWaveSetting(void);
int inCTOSS_CLMSetCustomEMVTags(void);
BOOL fEMVCheckCUPAID(void);
int inCTOS_WAVE_CUP_SALE(void);
int inCTOS_WAVE_START(void);


#ifdef	__cplusplus
}
#endif

#endif	/* _POSCTLS_H */

