/**
\file	FeliCa.c
\brief	FeliCa Access Functions
\date	2013/10/31
\date   2014.03.13 Added MutualAuthWithFeliCaByGSKUSK, MutualAuthWithFeliCa, RequestService, RegisterIssueIDEx, RegisterIssueID, RegisterArea, and RegisterService in FeliCa.h/c
\date   2014.03.13 Put const for input parameters of APIs in FeliCa.h/c
\date   2014.03.13 Eliminated unnecessary parameters from RegisterIssueIDExv2 in FeliCa.h/c
\date   2014.03.13 Modified MutualAuthV2WithFeliCaByGK and RegisterIssueIDExv2 not to call ToBCD()
\date   2014.05.01 Added a parameter for package lenght in RegisterIssueIDExv2, RegisterAreav2, and RegisterServicev2 in FeliCa.h/c 
\date   2014.05.01 Modified RegisterIssueIDEx v2, RegisterArea v2, and RegisterService v2 to support package identifier of both 0x41 and 0x4F in FeliCa.c
\author	Copyright 2013 Sony Corporation
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
	/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include <ctos_clapi.h>
#include <Vwdleapi.h>
#include <ctos_clapi.h>


#include "FeliCa.h"
#include "Reader.h"
//#include "../sam/SAM.h"
#include "Util.h"
//#include "Parameter.h"
#include "..\Debug\Debug.h"


USHORT usSecTimeOut	= 60; // unit -- ETU

// Transaction data to communicate with FeliCa Card
unsigned char IDm[8];
unsigned char IDt[8];

#if 0
long PollingFeliCaCard(void)
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_felica_cmd[5], _felica_res[262], _work_buf[262];
	unsigned long	_felica_cmd_len, _felica_res_len, _work_buf_len;

	//CTOS_FelicaPolling (IDm, IDt);

	// Generate params sent to SAM
	// Set packet data after Snr, and call AskFeliCaCmdToSAM function.
	// For packet structure, see RC-S500 Command Interface Manual.
	_felica_cmd_params[0]	= SYSTEM_CODE[0];	//System code
	_felica_cmd_params[1]	= SYSTEM_CODE[1];	//System code
	_felica_cmd_params[2]	= 0x00;				//Timeslot
	_felica_cmd_params_len	= 3;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_POLLING, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	DebugAddHEX("_felica_cmd", _felica_cmd, _felica_cmd_len);

	// Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if( (_ret != APP_SUCCESS) || (_felica_res_len == 0) ) {
		// Report FeliCa Error to SAM
		_ret = SendCardErrorToSAM(&_work_buf_len, _work_buf);
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendPollingResToSAM(_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	memcpy(IDm, &_felica_res[1], 8);

	return APP_SUCCESS;
}


long MutualAuthV2WithFeliCa( const unsigned char systemCode[2],
							 const unsigned char serviceCodeNum, 
							 const unsigned char serviceCodeKeyVerList[] )
{
	long			_ret;
	
	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	// Generate params sent to SAM
	memcpy(&_felica_cmd_params[0],				IDm, 8);			// IDm
	_felica_cmd_params[8] =						0x00;				// Reserved
	_felica_cmd_params[8+1] =					0x03;				// Key Type(Node key, Diversification Code specified)
	memcpy(&_felica_cmd_params[8+1+1],			systemCode, 2);		// SystemCode(Big endian)
	_felica_cmd_params[8+1+1+2] =				0x00;				// Operation Parameter(No Diversification, AES128)
	memset(&_felica_cmd_params[8+1+1+2+1],		0x00, 16);			// Diversification code(All Zero)
	_felica_cmd_params[8+1+1+2+1+16] =			serviceCodeNum;		// Number of Service 
	memcpy(&_felica_cmd_params[8+1+1+2+1+16+1], serviceCodeKeyVerList, serviceCodeNum * 4);	// Service Code List
	_felica_cmd_params_len = 8+1+1+2+1+16+1+serviceCodeNum*4;
	
	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAMSC(SAM_COMMAND_CODE_MUTUAL_AUTH_V2_RWSAM, SAM_SUB_COMMAND_CODE_MUTUAL_AUTH_V2_RWSAM, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1V2 command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1V2 result to SAM
	_ret = SendAuth1V2ResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2V2 to FeliCa
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2V2 result to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	if(_sam_res_buf[0] != 0){
		return APP_ERROR;
	}

	//Get IDt
	IDt[0] = _sam_res_buf[1+8+8];
	IDt[1] = _sam_res_buf[1+8+8+1];

	return APP_SUCCESS;
}

long MutualAuthWithFeliCa( const unsigned char systemCode[2],
							const unsigned char systemKeyVer[2],
							 const unsigned char areaCodeNum,
							 const unsigned char areaCodeKeyVerList[],
							 const unsigned char serviceCodeNum,
							 const unsigned char serviceCodeKeyVerList[] )
{
	long			_ret;
	
	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	// Generate params sent to SAM
	memcpy(&_felica_cmd_params[0], IDm, 8);			// IDm
	_felica_cmd_params[8] =	0x00;	// Reserved
	_felica_cmd_params[9] =	0x01;	// Key Type(Node key)

	memcpy(&_felica_cmd_params[10], systemCode, 2);		// SystemCode(Big endian)
	memcpy(&_felica_cmd_params[12], systemKeyVer, 2);		// System Key Version (Little endian)
	
	_felica_cmd_params[14] = areaCodeNum;		// Number of Area 
	memcpy(&_felica_cmd_params[15], areaCodeKeyVerList, areaCodeNum * 4);	// Area Code List
	
	_felica_cmd_params[15 + areaCodeNum * 4] = serviceCodeNum;		// Number of Service 
	memcpy(&_felica_cmd_params[15 + 1 + areaCodeNum * 4], serviceCodeKeyVerList, serviceCodeNum * 4);	// Service Code List

	_felica_cmd_params_len = 15 + 1 + areaCodeNum * 4 + serviceCodeNum * 4;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_MUTUAL_AUTH_RWSAM, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1 command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1 result to SAM
	_ret = SendAuth1V2ResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2 to FeliCa
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2 result to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	if(_sam_res_buf[0] != 0){
		return APP_ERROR;
	}

	//Get IDt
	IDt[0] = _sam_res_buf[1+8+8];
	IDt[1] = _sam_res_buf[1+8+8+1];

	return APP_SUCCESS;
}

long MutualAuthV2WithFeliCaByGK( const unsigned char systemCode[2],
								 const unsigned char serviceCodeNum, 
								 const unsigned char serviceCodeList[],
								 const unsigned char gk[])
{
	long			_ret;
	
	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	// Generate params sent to SAM
	memcpy(&_felica_cmd_params[0],				IDm, 8);			// IDm
	_felica_cmd_params[8] =						0x00;				// Operation parameter
	memset(&_felica_cmd_params[9],			    0x00, 2);			// Reserved
	_felica_cmd_params[11] =					serviceCodeNum;		// Number of Service 
	memcpy(&_felica_cmd_params[12], serviceCodeList, serviceCodeNum * 2);	// Service Code List
	memcpy(&_felica_cmd_params[12+serviceCodeNum * 2], gk, 16);	// GK
	memset(&_felica_cmd_params[12 + 16 +serviceCodeNum * 2], 0x00, 16); 	// Diversification code
	_felica_cmd_params_len = 12 + 16 +serviceCodeNum * 2 + 16;
	
	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_MUTUAL_AUTH_V2, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1V2 command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1V2 result to SAM
	_ret = SendAuth1V2ResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2V2 to FeliCa
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2V2 result to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	if(_sam_res_buf[0] != 0){
		return APP_ERROR;
	}

	//Get IDt
	IDt[0] = _sam_res_buf[1+8+8];
	IDt[1] = _sam_res_buf[1+8+8+1];

	return APP_SUCCESS;
}


long MutualAuthWithFeliCaByGSKUSK( const unsigned char systemCode[2],
								 const unsigned char areaCodeNum, 
								 const unsigned char areaCodeList[],
								 const unsigned char serviceCodeNum, 
								 const unsigned char serviceCodeList[],
								 const unsigned char gsk[],
								 const unsigned char usk[]
								 )
{
	long			_ret;
	
	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	// Generate params sent to SAM
	memcpy(&_felica_cmd_params[0], IDm, 8);			// IDm
	_felica_cmd_params[8] = areaCodeNum;		// Number of Area
	memcpy(&_felica_cmd_params[9], areaCodeList, areaCodeNum * 2);	// Area Code List
	_felica_cmd_params[9 + areaCodeNum * 2] = serviceCodeNum;		// Number of Service
	memcpy(&_felica_cmd_params[10 + areaCodeNum * 2], serviceCodeList, serviceCodeNum * 2);	// Service Code List
	memcpy(&_felica_cmd_params[10 + areaCodeNum * 2 + serviceCodeNum * 2], gsk, 8);	// GSK
	memcpy(&_felica_cmd_params[18 + areaCodeNum * 2 + serviceCodeNum * 2], usk, 8);	// USK

	_felica_cmd_params_len = 19 + areaCodeNum * 2 + serviceCodeNum * 2 + 8;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_MUTUAL_AUTH, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1 command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication1 result to SAM
	_ret = SendAuth1V2ResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2 to FeliCa
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	//Send authentication2 result to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return _ret;
	}

	if(_sam_res_buf[0] != 0){
		return APP_ERROR;
	}

	//Get IDt
	IDt[0] = _sam_res_buf[1+8+8];
	IDt[1] = _sam_res_buf[1+8+8+1];

	return APP_SUCCESS;
}

long RequestServiceV2(const unsigned char nodeNum, 
				   const unsigned char nodeList[], 
				   unsigned long* readLen,
				   unsigned char readData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_felica_cmd[262], _felica_res[262], _work_buf[262];
	unsigned long	_felica_cmd_len, _felica_res_len, _work_buf_len;

	// Generate params sent to SAM
	memcpy(_felica_cmd_params,						IDm, 8);					// IDm
	_felica_cmd_params[8] =							nodeNum;					// Number of Service
	memcpy(&_felica_cmd_params[8+1],				nodeList, nodeNum*2 );// ServiceList
	_felica_cmd_params_len = 8 + 1 + nodeNum*2;
	PrintText("Request Service v2\n");

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAMSC(SAM_COMMAND_CODE_REQUEST_SERVICE_V2_EX, SAM_SUB_COMMAND_CODE_REQUEST_SERVICE_V2_RWSAM, _felica_cmd_params_len,  _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		// Report FeliCa Error to SAM
		_ret = SendCardErrorToSAM(&_work_buf_len, _work_buf);
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}
	PrintText("Request Service v2 end\n");

	return APP_SUCCESS;
}


long RequestService(const unsigned char nodeNum, 
				   const unsigned char nodeList[], 
				   unsigned long* readLen,
				   unsigned char readData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_felica_cmd[262], _felica_res[262], _work_buf[262];
	unsigned long	_felica_cmd_len, _felica_res_len, _work_buf_len;

	// Generate params sent to SAM
	memcpy(_felica_cmd_params, IDm, 8);					// IDm
	_felica_cmd_params[8] = nodeNum;					// Number of Service
	memcpy(&_felica_cmd_params[8+1], nodeList, nodeNum*2 );// ServiceList
	_felica_cmd_params_len = 8 + 1 + nodeNum*2;
	PrintText("Request Service\n");

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_REQUEST_SERVICE, _felica_cmd_params_len,  _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		// Report FeliCa Error to SAM
		_ret = SendCardErrorToSAM(&_work_buf_len, _work_buf);
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}
	PrintText("Request Service end\n");

	return APP_SUCCESS;
}

long RegisterIssueIDExv2(unsigned long* readLen,
				         unsigned char readData[],
						 const unsigned int pkgLen,
						 const unsigned char pkgData[],
						 const unsigned char IDi[],
						 const unsigned char PMi[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	PrintText("Regsiter Issue ID Ex v2\n");
	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt

	if( pkgLen == 112){
	_felica_cmd_params[2] =			 0x00;					// Package identifier
	_felica_cmd_params[3] =			 0x4F;					// Package identifier
	}else if( pkgLen == 128){
	_felica_cmd_params[2] =			 0x00;					// Package identifier
	_felica_cmd_params[3] =			 0x41;					// Package identifier
	};

	memcpy(&_felica_cmd_params[4], IDi, 8);		// IDi
	memcpy(&_felica_cmd_params[12], PMi, 8);	// PMi
	// Copy Extended Issuging Package
	memcpy(&_felica_cmd_params[20], pkgData, pkgLen);

	_felica_cmd_params_len = 20 + pkgLen;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_REG_ISSUE_IDEX_v2, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Issue ID Ex v2 to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Issue ID Ex v2 result to a SAM\n");

	//Send Register Issue ID Ex v2 result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block v2
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

long RegisterIssueIDEx(  unsigned long* readLen,
				         unsigned char readData[],
						 const unsigned char pkgData[],
						 const unsigned char IDi[],
						 const unsigned char PMi[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	PrintText("Regsiter Issue ID Ex\n");
	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt
	memcpy(&_felica_cmd_params[2], IDi, 8);		// IDi
	memcpy(&_felica_cmd_params[10], PMi, 8);	// PMi
	// Copy Extended Issuging Package
	memcpy(&_felica_cmd_params[18], pkgData, 48);

	_felica_cmd_params_len = 18+48;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_REG_ISSUE_IDEX, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Issue ID Ex to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Issue ID Ex result to a SAM\n");

	//Send Register Issue ID Ex result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

long RegisterIssueID(  unsigned long* readLen,
				         unsigned char readData[],
						 const unsigned char pkgData[],
						 const unsigned char IDi[],
						 const unsigned char PMi[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	PrintText("Regsiter Issue ID Ex\n");
	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt
	memcpy(&_felica_cmd_params[2], IDi, 8);		// IDi
	memcpy(&_felica_cmd_params[10], PMi, 8);	// PMi
	// Copy Extended Issuging Package
	memcpy(&_felica_cmd_params[18], pkgData, 24);

	_felica_cmd_params_len = 18+24;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_REG_ISSUE_ID, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Issue ID Ex to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Issue ID Ex result to a SAM\n");

	//Send Register Issue ID Ex result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	return APP_SUCCESS;
}


long RegisterAreav2(const unsigned char areaCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned int pkgLen,
					const unsigned char pkgData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	PrintText("Regsiter Area v2\n");

	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt

	if( pkgLen == 48){
	_felica_cmd_params[2] =			 0x00;					// Package identifier
	_felica_cmd_params[3] =			 0x4F;					// Package identifier
	}else if( pkgLen == 64){
	_felica_cmd_params[2] =			 0x00;					// Package identifier
	_felica_cmd_params[3] =			 0x41;					// Package identifier
	};

	memcpy(&_felica_cmd_params[4], areaCode, 2);

	// Copy Register Area Package
	memcpy(&_felica_cmd_params[6], pkgData, pkgLen);

	_felica_cmd_params_len = 6 + pkgLen;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAMSC(SAM_COMMAND_CODE_REGISTER_AREA_v2, SAM_SUB_COMMAND_CODE_REGISTER_AREA_V2, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Area v2 to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Area v2 result to a SAM\n");

	//Send Regsiter Area v2 result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block v2
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}


	return APP_SUCCESS;
}

long RegisterArea(const unsigned char areaCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned char pkgData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;

	PrintText("Regsiter Area\n");
	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt
	memcpy(&_felica_cmd_params[2], areaCode, 2);
	// Copy Register Area Package
	memcpy(&_felica_cmd_params[4], pkgData, 24);

	_felica_cmd_params_len = 4 + 24;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_REGISTER_AREA, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Area to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Area result to a SAM\n");

	//Send Regsiter Area result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}


	return APP_SUCCESS;
}

long RegisterServicev2(const unsigned char serviceCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned int pkgLen,
					const unsigned char pkgData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;


	PrintText("Regsiter Service v2\n");
	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt

	if( pkgLen == 48){
	_felica_cmd_params[2] =			 0x00;					// Package identifier
	_felica_cmd_params[3] =			 0x4F;					// Package identifier
	}else if( pkgLen == 64){
	_felica_cmd_params[2] =			 0x00;					// Package identifier
	_felica_cmd_params[3] =			 0x41;					// Package identifier
	};

	memcpy(&_felica_cmd_params[4], serviceCode, 2);

	// Copy Register Service Package
	memcpy(&_felica_cmd_params[6], pkgData, pkgLen);

	_felica_cmd_params_len = 6 + pkgLen;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAMSC(SAM_COMMAND_CODE_REGISTER_SERVICE_V2, SAM_SUB_COMMAND_CODE_REGISTER_SERVICE_V2, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Service v2 to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Service v2 result to a SAM\n");

	//Send Regsiter Area v2 result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block v2
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}


	return APP_SUCCESS;
}

long RegisterService(const unsigned char serviceCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned char pkgData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_sam_res_buf[262], _felica_cmd[262], _felica_res[262];
	unsigned long	_sam_res_len, _felica_cmd_len, _felica_res_len;


	PrintText("Regsiter Service\n");
	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt
	memcpy(&_felica_cmd_params[2], serviceCode, 2);
	// Copy Register Service Package
	memcpy(&_felica_cmd_params[4], pkgData, 24);

	_felica_cmd_params_len = 4 + 24;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_REGISTER_SERVICE, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Service to a card\n");

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	PrintText("Send Regsiter Service result to a SAM\n");

	//Send Regsiter Service result to SAM
	_ret = SendRegisterResultToSAM(_felica_res_len, _felica_res, &_felica_cmd_len, _felica_cmd);	//Send FeliCa Response to SAM
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Change System Block
	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_sam_res_len, _sam_res_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_sam_res_len, _sam_res_buf, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}


	return APP_SUCCESS;
}

long ReadDataWOEnc(const unsigned char serviceNum, 
				   const unsigned char serviceList[], 
				   const unsigned char blockNum, 
				   const unsigned char blockList[], 
				   unsigned long* readLen,
				   unsigned char readData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_felica_cmd[262], _felica_res[262], _work_buf[262];
	unsigned long	_felica_cmd_len, _felica_res_len, _work_buf_len;

	// Generate params sent to SAM
	memcpy(_felica_cmd_params,						IDm, 8);					// IDm
	_felica_cmd_params[8] =							serviceNum;					// Number of Service
	memcpy(&_felica_cmd_params[8+1],				serviceList, serviceNum*2 );// ServiceList
	_felica_cmd_params[8+1+serviceNum*2] =			blockNum;					// Number of Service
	memcpy(&_felica_cmd_params[8+1+serviceNum*2+1], blockList, blockNum*2);		// BlockList
	_felica_cmd_params_len = 8 + 1 + serviceNum*2 + 1 + blockNum*2;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_READ_WO_ENC, _felica_cmd_params_len,  _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		// Report FeliCa Error to SAM
		_ret = SendCardErrorToSAM(&_work_buf_len, _work_buf);
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	return APP_SUCCESS;
}



long ReadDataBlock(const unsigned char blockNum,
				   const unsigned char blockList[], 
				   unsigned long* readLen,
				   unsigned char readData[] )
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_felica_cmd[262], _felica_res[262];
	unsigned long	_felica_cmd_len, _felica_res_len;

	// Generate params sent to SAM
	_felica_cmd_params[0] =			 IDt[0];					// IDt
	_felica_cmd_params[1] =			 IDt[1];					// IDt
	_felica_cmd_params[2] =			 blockNum;					// Number of Blocks
	memcpy(&_felica_cmd_params[2+1], blockList, blockNum*2);	// Block List
	_felica_cmd_params_len = 2 + 1 + blockNum*2;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_READ, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, readLen, readData);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

long WriteDataBlock(const unsigned char	blockNum,
					const int             blockLen,
					const unsigned char	blockList[], 
					const unsigned char	blockData[])
{
	long			_ret;

	unsigned char	_felica_cmd_params[256];
	unsigned int	_felica_cmd_params_len=0;

	unsigned char	_felica_cmd[262], _felica_res[262], _work_buf[262];
	unsigned long	_felica_cmd_len, _felica_res_len, work_len;

	// Generate params sent to SAM
	_felica_cmd_params[0] =						IDt[0];						// IDt
	_felica_cmd_params[1] =						IDt[1];						// IDt
	_felica_cmd_params[2] =						blockNum;					// Number of Blocks 
	memcpy(&_felica_cmd_params[2+1],			blockList, blockLen);		// block list
	memcpy(&_felica_cmd_params[2+1+blockLen], blockData, blockNum*16);	// block data
	_felica_cmd_params_len = 2 + 1 + blockLen + blockNum*16;

	// Ask to SAM to generate FeliCa Command
	_ret = AskFeliCaCmdToSAM(SAM_COMMAND_CODE_WRITE, _felica_cmd_params_len, _felica_cmd_params, &_felica_cmd_len, _felica_cmd);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send Command to FeliCa Card
	_ret = TransmitDataToFeliCaCard(_felica_cmd_len, _felica_cmd, &_felica_res_len, _felica_res);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Send FeliCa Response to SAM
	_ret = SendCardResultToSAM(_felica_res_len, _felica_res, &work_len, _work_buf);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}
	// XX AA means that failed to change key
	return APP_SUCCESS;
}

#if 0
extern USHORT cepas_Sub_CLCardDetect( IN BOOL isBeep );

void ShowBalanceUI(void){
	long result;
	unsigned int balance;
	unsigned char history[6][16];

	cepas_Sub_CLCardInit();

	//Detect FeliCa Card
	//result = DetectFeliCaCard();
	result = cepas_Sub_CLCardDetect(1);
	if(result != APP_SUCCESS){
		return;
	}

	result = DetectFeliCaCardEx();
	if(result != APP_SUCCESS){
		return;
	}

	// Polling FeliCa Card
	result = PollingFeliCaCard();
	if(result != APP_SUCCESS){
		printf("Polling Error\n");
		DisconnectFeliCaCard();
		return;
	}

	//Read E-money Balance
	//result = ReadBalance(&balance);
	if(result != APP_SUCCESS){
		printf("Read Balance Error\n");
		DisconnectFeliCaCard();
		return;
	}

	//Read History
	//result = ReadHistory(READ_WO_ENC, history);
	if( result != APP_SUCCESS){
		printf("Read History Error\n");
		DisconnectFeliCaCard();
		return;
	}

	// Disconnect FeliCa Card
	DisconnectFeliCaCard();

	// Show Balance
	printf("\n");
	printf("*******************************************\n");
	printf("  BALANCE: %u\n", balance);
	printf("*******************************************\n");
	printf("\n");

	// Show History
	//result = ShowHistory(history);
}
#endif

#endif