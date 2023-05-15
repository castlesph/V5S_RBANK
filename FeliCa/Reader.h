/**
\file	Reader.h
\brief	IC Card Reader Access functions
\date	2013/10/31
\author	Copyright 2013 Sony Corporation*/

#ifndef     _READER_H_
#define     _READER_H_

#define		DEBUG

//#define 	PC_SC_PROTOCOL

// Reader Writer related definition
#define UNKOWN_ERROR			1
#define RCS500_LCLE_ERROR		2
#define RCS500_P1P2_ERROR		3
#define RCS500_INS_ERROR		4
#define RCS500_CLA_ERROR		5
#define RCS500_MAC_ERROR		8

#define SMPL_ESC_KEY			0x1B


/**
Initialize IC Card Reader.
\return  APP_SUCCESS, APP_ERROR
*/
long InitializeReader(void);

/**
Release Reader resources.
*/
void DisconnectReader(void);

/**
Detect FeliCa Card.
This function blocks until a FeliCa card has detected or ESC Key has pressed.
\return  APP_SUCCESS, APP_ERROR
*/
long DetectFeliCaCard(void);

/**
Release FeliCa card connection.
\return  APP_SUCCESS, APP_ERROR
*/
long DisconnectFeliCaCard(void);

/**
Trasnmit APDU command packets to SAM.
\return  APP_SUCCESS, APP_ERROR
*/
long TransmitDataToSAM(unsigned char samCmdBuf[], 
					   unsigned long samCmdLen, 
					   unsigned char samResBuf[],
					   unsigned long* samResLen);

/**
Trasnmit APDU command packets to FeliCa Card.
\return  APP_SUCCESS, APP_ERROR
*/
long TransmitDataToFeliCaCard(unsigned long felicaCmdLen, 
							  unsigned char felicaCmdBuf[], 
							  unsigned long* felicaResLen,
							  unsigned char felicaResBuf[] );


USHORT usFelicaTransmit( IN BYTE *baTxBuf, IN USHORT usTxLen, OUT BYTE *baRxBuf, USHORT *usRxLen, USHORT usTimeOut);


#endif
