/* 
 * File:   display.h
 * Author: 
 *
 * Created on 2014
 */

#ifndef ___CTOS_CFGEXPRESS_H___
#define	___CTOS_CFGEXPRESS_H___

#ifdef	__cplusplus
extern "C" {
#endif

#define PUBLIC_PATH 				"/home/ap/pub/"
//#define PUBLIC_PATH 				"/data/data/pub/"
#define LOCAL_PATH 				"./fs_data/"
//#define LOCAL_PATH 					"/data/data/com.Source.S1_RBANK.RBANK/"
#define DATABASE_FILE_EXT 			".S3DB"
#define CASTLES_CONFIG_FILE			"parameter.prm"
#define UNFORK_AP_FILE			"unforkap.ini"


typedef char DataBaseName[50+1];
typedef char TableName[50+1];
typedef char FieldData[50+1];
typedef char FieldName[50+1];
typedef char RecordNum[50+1];

int inCTOSS_ProcessCfgExpress(void);
int inCTOSS_BackupDataScript(unsigned char *szScriptFile, unsigned char *szOutputFile);
int inCTOSS_RestoreDataByScript(void);


int inCTOSS_GetEnvDB(char *tag, char *value);
int inCTOSS_PutEnvDB(char *tag, char *value);

#ifdef	__cplusplus
}
#endif

#endif	/* ___CTOS_CFGEXPRESS_H___ */

