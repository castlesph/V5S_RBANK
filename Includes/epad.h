/* 
 * File:   dymaniclib.h
 * Author: PeyJiun
 *
 * Created on 2010�~12��6��, �U�� 8:57
 */

#ifndef _EPAD_H
#define	_EPAD_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include <ctosapi.h>
#include <ctos_qrcode.h>

#define PRINT_BMP		"/home/ap/pub/Print_BMP.bmp"

//typedef struct RGBApixel {
//	unsigned char Blue;
//	unsigned char Green;
//	unsigned char Red;
//	unsigned char Alpha;
//} RGBApixel;

typedef struct TagDISPLAY_REC
{
    BYTE        szDisplay1[50+1];
    BYTE        szDisplay2[50+1];
    BYTE        szDisplay3[50+1];
    BYTE        szDisplay4[50+1];
    BYTE        szDisplay5[50+1];
    BYTE        szDisplay6[50+1];
	BYTE        szDisplay7[50+1];
	BYTE        szDisplay8[50+1];
} DISPLAY_REC;

typedef struct TagBMP_REC
{
    char	szOrgBMPname[50+1];
    int ToX;
	int ToY;
} BMP_REC;

typedef struct TagRGB_REC {
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Alpha;
} RGB_REC;



int ePad_SignatureCaptureLib(void);
int ePad_PrintSignatureCaptureLib(void);
int ePad_ClearSignatureCaptureLib(void);
int ePad_SignatureCaptureLibEx(void);
int ePad_SignatureCaptureLibEex(DISPLAY_REC *szDisplayRec);

void vdCTOSS_CombineBMP(char *szBKBMPname, char *szOrgBMPname,char *szNewBMPname,int FromX1 , int FromY1, int FromX2, int FromY2,int ToX, int ToY);
void vdCTOSS_CombineMenuBMP(char *szBKBMPname);
void vdCTOSS_CombineIdleBMP(void);
void vdCTOSS_DelBackupS3DB(void);
void vdBackUpWhiteBMP(char *szBMPname,char *szOrgBMP);
void vdCTOSS_ModifyBMP(char *szBMPFileName, int *Width, int *Height);
void vdCTOSS_CombineBMPEx(char *szBKBMPname, char *szOrgBMPname,char *szNewBMPname,int FromX1 , int FromY1, int FromX2, int FromY2,int ToX, int ToY);
void vdCTOSS_CombineMultiBMP(char *szBKBMPname, char *szNewBMPname,int FromX1 , int FromY1, int FromX2, int FromY2,int number, BMP_REC *szOrgBMP,RGB_REC Rgb_rec);

void vdCTOSS_CombineBMPSetBit(char *szBKBMPname, char *szOrgBMPname,char *szNewBMPname,int FromX1 , int FromY1, int FromX2, int FromY2,int ToX, int ToY,int Bit,unsigned char Blue,unsigned char Green,unsigned char Red);


int inCovertBMPToLGO(char *szBMPFileName, char *szLGOFileName);
int inCovertLGOToBMP(char *szLGOFileName, char *szBMPFileName);

void vdCTOSS_PrinterStart(int inLineCount);
void vdCTOSS_PrinterBMPPicEx(unsigned int usX,unsigned int y,  char *path);
int inCCTOS_PrinterBufferOutput(unsigned char* pchTemp,CTOS_FONT_ATTRIB* ATTRIB,int ERMCapture);
int inCCTOS_PrinterBufferOutputAligned(unsigned char* pchTemp,CTOS_FONT_ATTRIB* ATTRIB,BYTE bMode, BYTE IsNextline, int ERMCapture);
void vdCTOSS_PrinterEnd(void);
void vdCTOSS_SetLineSpace(int inLineSpace);
void vdCTOSS_PrinterEnd_CConvert2BMP(char *szBMPfilename);


int inCTOSS_SetERMFontType(char *szFontName,int inFontStyle);
int inCTOSS_FormERMReceipt(char *szFileName);
int inCTOSS_ReadERMSignatureFile(char *szSIGPfilename);
void vdCTOSS_SaveBMPForERM(char *szFileName);

void vdCTOSS_PrinterBufferCode39Barcode(IN USHORT x, IN USHORT y, IN BYTE *baCodeContent, IN BYTE bContentLen, IN BYTE bXExtend, IN BYTE bY8Extend, IN BOOL bShowChar);
void vdCTOSS_PrinterBufferCode128Barcode(IN USHORT x, IN USHORT y, IN BYTE *baCodeContent, IN BYTE bContentLen, IN BYTE bXExtend, IN BYTE bY8Extend, IN BOOL bShowChar);
void vdCTOSS_PrinterBufferQRCode(CTOS_QRCODE_INFO *pInfo, const char *pMessageText, USHORT usXPos, USHORT usYPost);

/*********************************
width = 384,
*********************************/
void vdCTOSS_RotateBMPFile(char *szFileName, int inRotate);


//Call back function  for check the print status whill printing
typedef int  (*CallBackFunc)(void);

void vdCTOSS_PrinterStartStatus(int inLineCount,CallBackFunc EventFunc);
void vdCTOSS_PrinterBMPPicExStatus(unsigned int usX,unsigned int y,  char *path);
int inCCTOS_PrinterBufferOutputStatus(unsigned char* pchTemp,CTOS_FONT_ATTRIB* ATTRIB,int ERMCapture);
void vdCTOSS_PrinterEndStatus(void);


#ifdef	__cplusplus
}
#endif

#endif	/* _DYNAMICLIB_H */

