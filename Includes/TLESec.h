/* 
 * File:   newfile.h
 * Author: Administrator
 *
 * Created on 2012年9月18日, 下午 3:31
 */

#ifndef NEWFILE_H
#define	NEWFILE_H

#ifdef	__cplusplus
extern "C" {
#endif

int inEncryptTLESec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult);
int inDecryptTLESec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult);
int inEncryptTLE(BYTE *szMsg, int inMsgLen, BYTE *ptrResult);
void iDES_CBC(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *DESResult, BYTE *szMKey);
void iDES_ECB(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *DESResult, BYTE *szMKey);
int inGetMultiplePadded(int inMsgLen, BYTE *ptrResult);
int inGetHexLength(int inLen, BYTE *ptrResult);
int inGetDataKey(BYTE *szTID, BYTE *szDefaultPadded, BYTE *ptrResult);
int inGetMACKey(BYTE *szTID, BYTE *szDefaultPadded, BYTE *ptrResult);
int inGetMAC(BYTE *szMsg, int inMsgLen, BYTE *szKey, BYTE *ptrResult);
int inCheckTLESecKey(void);
int inCheckTLEResponseCode(BYTE *szMsg, int inMsgLen);

#define CTOS_KMS2_VERSION         0x01
#define TLESEC_KEY_SET            0xC006
#define TLESEC_KEY_INDEX          0x0001

#ifdef	__cplusplus
}
#endif

#endif	/* NEWFILE_H */

