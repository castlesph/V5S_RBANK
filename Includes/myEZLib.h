/* 
 * File:   _MYEZLIB_H.h
 * Author: kobe
 *
 * Created on 2010�~12��6��, �U�� 8:57
 */

#ifndef _MYEZLIB_H
#define	_MYEZLIB_H

#ifdef	__cplusplus
extern "C" {
#endif


#define	DBG_PORT        d_COM1
#define RS232DEBUGPORT_ON  1
#define RS232DEBUGPORT_OFF  0    
  

void vdMyEZLib_ComPortInit(void);
void vdMyEZLib_Printf(const char* fmt, ...);
void vdMyEZLib_Printf2(const char* fmt, ...);
void vdMyEZLib_LogPrintf(const char* fmt, ...);
void vdMyEZLib_LogPrintff(unsigned char *fmt, short inlen);
int inBcd2Ascii(BYTE *szBcd, BYTE *szAscii, int inBcdLen);
int inAscii2Bcd(BYTE *szAscii, BYTE *szBcd, int inBcdLen);
int inBcd2Bin(BYTE *szBcd, int inBcdLen);
int inBin2Bcd(int inBin, BYTE *szBcd, int inBcdLen);
int inBcd2Bin(BYTE *szBcd, int inBcdLen);
int inBcdAddOne(BYTE *szInBcd, BYTE *szOutBcd, int inBcdLen);
void TrimTrail(unsigned char *strTrim);
void vdMyEZLib_Printf3(BOOL inLabelFlag, unsigned char *DELabel, const char* fmt, ...);
void vdMyEZLib_Printf3_ERM(BOOL inLabelFlag, unsigned char *DELabel, const char* fmt, ...);
void vdMyEZLib_HexDump(unsigned char *msg, unsigned char *fmt, short inlen);
void vdMyEZLib_HexDump_ERM(unsigned char *msg, unsigned char *fmt, short inlen);
int GetDelimitedString(char *buf, int fldnum, unsigned char sep, char *dest);

#ifdef	__cplusplus
}
#endif

#endif	/* _MYEZLIB_H */

