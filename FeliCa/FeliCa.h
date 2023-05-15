/**
\file	FeliCa.h
\brief	FeliCa Access Functions
\date	2014/3/13
\date   2014.03.13 Added MutualAuthWithFeliCaByGSKUSK, MutualAuthWithFeliCa, RequestService, RegisterIssueIDEx, RegisterIssueID, RegisterArea, and RegisterService in FeliCa.h/c
\date   2014.03.13 Put const for input parameters of APIs in FeliCa.h/c
\date   2014.03.13 Eliminated unnecessary parameters from RegisterIssueIDExv2 in FeliCa.h/c
\date   2014.05.01 Added a parameter for package lenght in RegisterIssueIDExv2, RegisterAreav2, and RegisterServicev2 in FeliCa.h/c 
\date   2014.05.01 Modified RegisterIssueIDEx v2, RegisterArea v2, and RegisterService v2 to support package identifier of both 0x41 and 0x4F in FeliCa.c
\author	Copyright 2013 Sony Corporation
*/
#ifndef     _FELICA_H_
#define     _FELICA_H_


/**
Send Polling command to FeliCa Card.
\return  APP_SUCCESS, APP_ERROR
*/
long PollingFeliCaCard(void);

/**
Establish AES Authentication between FeliCa card and SAM.

Pass concatenated data of service code and key version to "serviceCodeKeyVerList".\n
e.g. Authenticate with service code[key version] 4080[0002] and service code[key version]  122B[0F15]\n
       -> serviceCodeKeyVerList: 804002002B12150F

\param [in] systemCode				system code
\param [in] serviceCodeNum			number of services
\param [in] serviceCodeKeyVerList	list of service code(little endian) and key version(little endian) 
\return  APP_SUCCESS, APP_ERROR
*/
long MutualAuthV2WithFeliCa( const unsigned char systemCode[2],
							 const unsigned char serviceCodeNum,
							 const unsigned char serviceCodeKeyVerList[] );

/**
Establish DES Authentication between FeliCa card and SAM.

Pass concatenated data of service code and key version to "serviceCodeKeyVerList".\n
e.g. Authenticate with service code[key version] 4080[0002] and service code[key version]  122B[0F15]\n
       -> serviceCodeKeyVerList: 804002002B12150F

\param [in] systemCode				system code
\param [in] serviceCodeNum			number of services
\param [in] serviceCodeKeyVerList	list of service code(little endian) and key version(little endian) 
\return  APP_SUCCESS, APP_ERROR
*/
long MutualAuthWithFeliCa( const unsigned char systemCode[2],
							const unsigned char systemKeyVer[2],
							 const unsigned char areaCodeNum,
							 const unsigned char areaCodeKeyVerList[],
							 const unsigned char serviceCodeNum,
							 const unsigned char serviceCodeKeyVerList[] );


/**
Establish AES Authentication between FeliCa card and SAM by Group Key.

\param [in] systemCode				system code
\param [in] serviceCodeNum			number of services
\param [in] serviceCodeKeyVerList	list of service code(little endian) 
\param [in] gk						Group Key for authentication
\return  APP_SUCCESS, APP_ERROR
*/
long MutualAuthV2WithFeliCaByGK( const unsigned char systemCode[2],
								 const unsigned char serviceCodeNum, 
								 const unsigned char serviceCodeList[],
								 const unsigned char gk[]);

/**
Establish DES Authentication between FeliCa card and SAM by GSK and USK.

\param [in] systemCode				system code
\param [in] areaCodeNum			number of areas
\param [in] areaCodeKeyVerList	list of area code(little endian) 
\param [in] serviceCodeNum			number of services
\param [in] serviceCodeKeyVerList	list of service code(little endian) 
\param [in] gsk						Group Service Key for authentication
\param [in] usk						User Service Key for authentication
\return  APP_SUCCESS, APP_ERROR
*/
long MutualAuthWithFeliCaByGSKUSK( const unsigned char systemCode[2],
								 const unsigned char areaCodeNum, 
								 const unsigned char areaCodeList[],
								 const unsigned char serviceCodeNum, 
								 const unsigned char serviceCodeList[],
								 const unsigned char gsk[],
								 const unsigned char usk[]
								 );


/**
Send Register Issue ID Ex v2 command to FeliCa card
\param [out] readLen				length of read data (card response) 
\param [out] readData				read data (card response) 
\param [in] pkgLen					Package length
\param [in] pkgData					Issue package data
\param [in] IDi						IDi to be set to FeliCa card
\param [in] PMi						PMi to be set to FeliCa card
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterIssueIDExv2(unsigned long* readLen,
				         unsigned char readData[],
						 const unsigned int pkgLen,
						 const unsigned char pkgData[],
						 const unsigned char IDi[],
						 const unsigned char PMi[]);

/**
Send Register Issue ID Ex command to FeliCa card
\param [out] readLen				length of read data (card response) 
\param [out] readData				read data (card response) 
\param [in] pkgData					Issue package data
\param [in] IDi						IDi to be set to FeliCa card
\param [in] PMi						PMi to be set to FeliCa card
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterIssueIDEx(  unsigned long* readLen,
				         unsigned char readData[],
						 const unsigned char pkgData[],
						 const unsigned char IDi[],
						 const unsigned char PMi[]);

/**
Send Register Issue ID command to FeliCa card
\param [out] readLen				length of read data (card response) 
\param [out] readData				read data (card response) 
\param [in] pkgData					Issue package data
\param [in] IDi						IDi to be set to FeliCa card
\param [in] PMi						PMi to be set to FeliCa card
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterIssueID(unsigned long* readLen,
				         unsigned char readData[],
						 const unsigned char pkgData[],
						 const unsigned char IDi[],
						 const unsigned char PMi[]);

/**
Send Request Service v2 command to FeliCa card to get key version.

\param [in] nodeNum			 	    number of nodes
\param [in] nodeList				node list
\param [out] readLen				length of received data 
\param [out] readData				received data from FeliCa card
\return  APP_SUCCESS, APP_ERROR
*/
long RequestServiceV2(const unsigned char nodeNum, 
				      const unsigned char nodeList[], 
				      unsigned long* readLen,
				      unsigned char readData[]);

/**
Send Request Service command to FeliCa card to get key version.

\param [in] nodeNum			 	    number of nodes
\param [in] nodeList				node list
\param [out] readLen				length of received data 
\param [out] readData				received data from FeliCa card
\return  APP_SUCCESS, APP_ERROR
*/
long RequestService(const unsigned char nodeNum, 
				      const unsigned char nodeList[], 
				      unsigned long* readLen,
				      unsigned char readData[]);


/**
Send Register Area v2 command to FeliCa card
\param [in]  areaCode			    area code
\param [out] readLen				length of read data (card response)
\param [out] readData				read data
\param [in] pkgLen					Package length
\param [in]  pkgData				register area packge data
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterAreav2(const unsigned char areaCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned int pkgLen,
					const unsigned char pkgData[]);

/**
Send Register Area command to FeliCa card
\param [in]  areaCode			    area code
\param [out] readLen				length of read data (card response)
\param [out] readData				read data
\param [in]  pkgData				register area packge data
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterArea(const unsigned char areaCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned char pkgData[]);

/**
Send Register Service v2 command to FeliCa card
\param [in]  serviceCode		    registered service code 
\param [out] readLen				length of read data (card response) 
\param [out] readData				read data (card response) 
\param [in] pkgLen					Package length
\param [in]  pkgData			    register service package data
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterServicev2(const unsigned char serviceCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned int pkgLen,
					const unsigned char pkgData[]);
					
/**
Send Register Service command to FeliCa card
\param [in]  serviceCode		    registered service code 
\param [out] readLen				length of read data (card response) 
\param [out] readData				read data (card response) 
\param [in]  pkgData			    register service package data
\return  APP_SUCCESS, APP_ERROR
*/
long RegisterService(const unsigned char serviceCode[], 
				    unsigned long* readLen,
				    unsigned char readData[],
					const unsigned char pkgData[]);
					

/**
Send Read without Encryption command to FeliCa card.
"readData" contains [status flag1], [status flag2], [block num], [block data(16*n)]  

\param [in] serviceNum		number of services
\param [in] serviceList		service code list(little endian)
\param [in] blockNum		number of blocks
\param [in] blockList		block list
\param [out] readLen		length of read data 
\param [out] readData		read data
\return  APP_SUCCESS, APP_ERROR
*/
long ReadDataWOEnc(const unsigned char serviceNum, 
				   const unsigned char serviceList[], 
				   const unsigned char blockNum, 
				   const unsigned char blockList[], 
				   unsigned long* readLen,
				   unsigned char readData[]);

/**
Send Read command to FeliCa card.
"readData" contains [status flag1], [status flag2], [block num], [block data(16*n)]  

\param [in] blockNum		number of blocks
\param [in] blockList		block list
\param [out] readLen		length of read data 
\param [out] readData		read data
\return  APP_SUCCESS, APP_ERROR
*/
long ReadDataBlock(const unsigned char	blockNum,
				   const unsigned char	blockList[], 
				   unsigned long*	readLen,
				   unsigned char	readData[] );

/**
Send Register Write command to FeliCa card
\param [in]  blockNum				Number of blocks to be written 
\param [in]  blockLen				Length of block data
\param [in]  blockList				block list
\param [in]  blockData			    write block data
\return  APP_SUCCESS, APP_ERROR
*/
long WriteDataBlock(const unsigned char	blockNum,
					const int             blockLen,
					const unsigned char	blockList[], 
					const unsigned char	blockData[]);


#endif