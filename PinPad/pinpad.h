#ifndef ___PINPAD_H___
#define	___PINPAD_H___

#ifdef	__cplusplus
extern "C" {
#endif

    int inInitializePinPad(void);
    void TEST_Write3DES_Plaintext(void);
    void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine);
    void OnGetPINDigit(BYTE NoDigits);
    void OnGetPINCancel(void);
    void OnGetPINBackspace(BYTE NoDigits);
    int inGetIPPPin(void);
    int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC);
    int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC);
    int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex);

    int inCTOS_KMS2PINGetExDukpt(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow);
    USHORT inCTOS_KMS2PINGetEx3Des(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow);
	void vdCTOSS_EFT_TestHardcodeKey(void);

	void vdSetDummyMKey(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ___PINPAD_H___ */

