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

void vdGenerateRandomKey(int inLen, BYTE *ptrResult);
int inEncryptTPSec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult);
int inDecryptTPSec(BYTE *szMsg, int inMsgLen, BYTE *ptrResult);
int inCheckTPSecKey(void);

#define     CTOS_KMS2_VERSION            	0x01

#define TPSEC_KEY_SET 0xC002
#define TPSEC_KEY_INDEX 0x0001

#ifdef	__cplusplus
}
#endif

#endif	/* NEWFILE_H */

