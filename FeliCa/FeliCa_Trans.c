/**
\file Main_PC.c
\brief    Main Application functions
\date     2013/11/6
\author   Copyright 2013 Sony Corporation
*/
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <ctoserr.h>
#include <stdlib.h>
#include <stdarg.h>
#include <emv_cl.h>
	
#include "..\Includes\Wub_lib.h"
#include "..\Debug\Debug.h"
//#include "..\sam\sam.h"

#include "..\Includes\POSTypedef.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\ui\Display.h"


#include "FeliCa.h"
#include "Reader.h"

#if 0
#include "SAM.h"
#include "Util.h"
#include "Parameter.h"
#endif

/**
Read E-Money balance data from FeliCa card.

\param [out] balance		balance value 

\return  APP_SUCCESS, APP_ERROR
*/
long ReadBalance(unsigned int* balance);


/**
Update E-Money balance and history data in FeliCa card.

\param [in] newBalance	new balance data to be written
\param [in] amount		charge/cayment amount
\param [in] method		METHOD_PAYMENT or METHOD_CHARGE

\return  APP_SUCCESS, APP_ERROR
*/
long UpdateBalance(	unsigned int	newBalance,
					int				amount,
					unsigned char	method);

/**
Read history data from FeliCa card.
\param [in] mode		READ_WO_ENC or READ_ENC
\param [in] history		each history data block(16 bytes) has following structure.\n
[DD][MM][YY][YY][hh][mm][ss][PP][AA][AA][AA][AA][BB][CC][DD][DD]\n
DD: Date\n
MM: Month\n
YYYY: Year\n
hh: Hour\n
mm: Minute\n
ss: Second\n
PP: Padding\n
AAAAAAAA: Charged/Paid Value(big endian)\n
BB: Operation type (0x00: "Charge", 0x01: "Payment")\n
CC: Padding\n
DDDD: Execution ID\n


\return  APP_SUCCESS, APP_ERROR
*/

long ReadHistory(int mode, unsigned char history[6][16]);

void _update_exec_id(unsigned char execID[] );

long ShowMainUI(void);

void ShowPaymentUI(void);
void ShowChargeUI(void);
void ShowBalanceUI(void);
long ShowHistory(unsigned char history[6][16]);
#if 0
#if 0
int inTestFelica_Trans(void)
{
	long _ret = 0;

	//Initiaize PC/SC Connection
	_ret = InitializeReader();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("Initialize Error\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		getch();
		return 0;
	}


	// Authenticate with SAM(RC-S500)
	_ret = InitializeSAM();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("RC-S500 Authentication Failed\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		getch();
		return 0;
	}

	#if 0
	//Start service sequence
	while( ShowMainUI() != APP_CANCEL ){}
	#endif

	
	

	//Disconnect PC/SC Connection
	DisconnectReader();

	vdDebug_LogPrintf("\nPress any key\n");
	getch();

	return 0;
}
#endif

#if 0
long ShowMainUI(void)
{
	int c;

	printf("\n\n");
	printf("*******************************************\n");
	printf("*   SERVICE  MENU                         *\n");
	printf("*-----------------------------------------*\n");
	printf("* 1: Payment                              *\n");
	printf("* 2: Charge                               *\n");
	printf("* 3: Balance                              *\n");
	printf("*-----------------------------------------*\n");
	printf("* 0: Exit                                 *\n");
	printf("*******************************************\n");
	
	fflush(stdin);
	while((c = getchar()) != EOF){
		if(c == '0'){
			//Exit Menu
			return APP_CANCEL;
		}else if(c == '1'){
			c = getchar(); // Catch \n code
			ShowPaymentUI();
			return APP_SUCCESS;
		}else if(c == '2'){
			c = getchar(); // Catch \n code
			ShowChargeUI();
			return APP_SUCCESS;
		}else if(c == '3'){
			c = getchar(); // Catch \n code
			ShowBalanceUI();
			return APP_SUCCESS;
		}else{
			printf("Invalid Number\n");
		}
	}

	return APP_SUCCESS;
}
#endif

void vdFelica_ShowErrorMsg(int inColumn, int inRow,  char *msg)
{
	int inRowtmp;
        inRowtmp = inRow;
			
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg);

	CTOS_Beep();CTOS_Beep();CTOS_Beep();CTOS_Beep();
	CTOS_LEDSet(d_LED1, d_ON);
    CTOS_Delay(2000);
	CTOS_LEDSet(d_LED1, d_OFF);
}

void ShowPaymentUI(void)
{
	long  result;
	unsigned int balance, amount, newBalance;
	unsigned char history[6][16];

	BYTE szBal[32];
	BYTE szAmtStr[32];
	
	long _ret = 0;

	//Initiaize PC/SC Connection
	_ret = InitializeReader();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("Initialize Error\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		vdFelica_ShowErrorMsg(1, 8, "Init Reader Error");
		return;
	}


	// Authenticate with SAM(RC-S500)
	_ret = InitializeSAM();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("RC-S500 Authentication Failed\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		vdFelica_ShowErrorMsg(1, 8, "Init SAM Error");
		return;
	}

	vdDebug_LogPrintf("Input Amount of Payment\n");
	//amount = GetNumericValue();
	memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
	result = inCTOS_GetTxnBaseAmount();
	if(d_OK != result)
        return;

	memset(szAmtStr, 0x00, sizeof(szAmtStr));
	wub_hex_2_str(srTransRec.szBaseAmount, szAmtStr, AMT_BCD_SIZE);
	szAmtStr[AMT_BCD_SIZE*2] = 0x00;
	amount = atol(szAmtStr);
	vdDebug_LogPrintf("amount[%ld]", amount);


	//Detect FeliCa Card
	result = DetectFeliCaCard();
	if(result != APP_SUCCESS){
		return;
	}

	// Polling FeliCa Card
	result = PollingFeliCaCard();
	if(result != APP_SUCCESS){
		vdDebug_LogPrintf("Polling Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Polling Error");
		return;
	}

	//Read E-money Balance
	result = ReadBalance(&balance);
	if(result != APP_SUCCESS){
		vdDebug_LogPrintf("Read Balance Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Read Balance Error");
		return;
	}

	//Check balance and amount
	if(amount > balance){
		vdDebug_LogPrintf("Insufficient Balance\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Insufficient Balance");
		return;
	}

	// Write new balance data
	newBalance = balance - amount;
	result = UpdateBalance(newBalance, amount, METHOD_PAYMENT);
	if( result != APP_SUCCESS){
		vdDebug_LogPrintf("Write Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Write Balance Error");
		return;
	}

	#if 0
	//Read History
	result = ReadHistory(READ_ENC, history);
	if( result != APP_SUCCESS){
		vdDebug_LogPrintf("Read History Error\n");
		DisconnectFeliCaCard();
		return;
	}
	#endif

	// Disconnect FeliCa Card
	DisconnectFeliCaCard();

	//CTOS_Sound(2000, 40);
    //CTOS_LEDSet(d_LED2, d_ON);

	// Show Balance
	vdDebug_LogPrintf("\n");
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("  BALANCE: %u\n", newBalance);
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("\n");

	CTOS_LCDTClearDisplay();

	memset(szAmtStr, 0x00, sizeof(szAmtStr));
	sprintf(szAmtStr, "Pay Amt: %u", amount);
	vduiDisplayStringCenter(3, szAmtStr);
	
	memset(szBal, 0x00, sizeof(szBal));
	sprintf(szBal, "BALANCE: %u", newBalance);
	vduiDisplayStringCenter(5, szBal);

	#if 0
	// Show History
	result = ShowHistory(history);
	#endif

	CTOS_LEDSet(d_LED2, d_OFF);

	WaitKey(20);

}

void ShowChargeUI(void)
{
	long result;
	unsigned int balance, amount, newBalance;
	unsigned char history[6][16];

	BYTE szBal[32];
	BYTE szAmtStr[32];

	long _ret = 0;

	//Initiaize PC/SC Connection
	_ret = InitializeReader();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("Initialize Error\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		vdFelica_ShowErrorMsg(1, 8, "Init Reader Error");
		return;
	}


	// Authenticate with SAM(RC-S500)
	_ret = InitializeSAM();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("RC-S500 Authentication Failed\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		vdFelica_ShowErrorMsg(1, 8, "Init SAM Error");
		return;
	}

	vdDebug_LogPrintf("Input Amount of Charge\n");
	//amount = GetNumericValue();
	memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
	result = inCTOS_GetTxnBaseAmount();
	if(d_OK != result)
        return;

	memset(szAmtStr, 0x00, sizeof(szAmtStr));
	wub_hex_2_str(srTransRec.szBaseAmount, szAmtStr, AMT_BCD_SIZE);
	szAmtStr[AMT_BCD_SIZE*2] = 0x00;
	amount = atol(szAmtStr);
	vdDebug_LogPrintf("amount[%ld]", amount);


	//Detect FeliCa Card
	result = DetectFeliCaCard();
	if(result != APP_SUCCESS){
		return;
	}

	// Polling FeliCa Card
	result = PollingFeliCaCard();
	if(result != APP_SUCCESS){
		vdDebug_LogPrintf("Polling Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Polling Error");
		return;
	}

	//Read E-money Balance
	result = ReadBalance(&balance);
	if(result != APP_SUCCESS){
		vdDebug_LogPrintf("Read Balance Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Read Balance Error");
		return;
	}


	// Write new balance data
	newBalance = amount + balance;
	result = UpdateBalance(newBalance, amount, METHOD_CHARGE);
	if( result != APP_SUCCESS){
		vdDebug_LogPrintf("Write Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Write Balance Error");
		return;
	}

	#if 0
	//Read History
	result = ReadHistory(READ_ENC, history);
	if( result != APP_SUCCESS){
		vdDebug_LogPrintf("Read History Error\n");
		DisconnectFeliCaCard();
		return;
	}
	#endif

	// Disconnect FeliCa Card
	DisconnectFeliCaCard();
	
	//CTOS_Sound(2000, 40);
    //CTOS_LEDSet(d_LED2, d_ON);

	// Show Balance
	vdDebug_LogPrintf("\n");
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("  BALANCE: %u\n", newBalance);
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("\n");

	
	CTOS_LCDTClearDisplay();

	memset(szAmtStr, 0x00, sizeof(szAmtStr));
	sprintf(szAmtStr, "Charge Amt: %u", amount);
	vduiDisplayStringCenter(3, szAmtStr);
	
	memset(szBal, 0x00, sizeof(szBal));
	sprintf(szBal, "BALANCE: %u", newBalance);
	vduiDisplayStringCenter(5, szBal);

	CTOS_LEDSet(d_LED2, d_OFF);
	#if 0
	// Show History
	result = ShowHistory(history);
	#endif

	WaitKey(20);
	

}

void ShowBalanceUI(void)
{
	long result;
	unsigned int balance;
	unsigned char history[6][16];

	BYTE szBal[32];

	long _ret = 0;

	//Initiaize PC/SC Connection
	_ret = InitializeReader();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("Initialize Error\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		vdFelica_ShowErrorMsg(1, 8, "Init Reader Error");
		return;
	}


	// Authenticate with SAM(RC-S500)
	_ret = InitializeSAM();
	if(_ret != APP_SUCCESS){
		vdDebug_LogPrintf("RC-S500 Authentication Failed\n");
		vdDebug_LogPrintf("\nPress any key\n");
		DisconnectReader();
		vdFelica_ShowErrorMsg(1, 8, "Init SAM Error");
		return;
	}


	//Detect FeliCa Card
	result = DetectFeliCaCard();
	if(result != APP_SUCCESS){
		return;
	}

	// Polling FeliCa Card
	result = PollingFeliCaCard();
	if(result != APP_SUCCESS){
		vdDebug_LogPrintf("Polling Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Polling Error");
		return;
	}

	//Read E-money Balance
	result = ReadBalance(&balance);
	if(result != APP_SUCCESS){
		vdDebug_LogPrintf("Read Balance Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Read Balance Error");
		return;
	}

	CTOS_Sound(2000, 35);
    CTOS_LEDSet(d_LED2, d_ON);

	// Show Balance
	vdDebug_LogPrintf("\n");
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("  BALANCE: %u\n", balance);
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("\n");

	CTOS_LCDTClearDisplay();
	
	memset(szBal, 0x00, sizeof(szBal));
	sprintf(szBal, "BALANCE: %u", balance);
	vduiDisplayStringCenter(1, szBal);

	//Read History
	result = ReadHistory(READ_WO_ENC, history);
	if( result != APP_SUCCESS){
		vdDebug_LogPrintf("Read History Error\n");
		DisconnectFeliCaCard();
		vdFelica_ShowErrorMsg(1, 8, "Read History Error");
		return;
	}

	// Disconnect FeliCa Card
	DisconnectFeliCaCard();
	
	// Show History
	result = ShowHistory(history);

	CTOS_LEDSet(d_LED2, d_OFF);

	WaitKey(20);
}



long ShowHistory(unsigned char history[6][16])
{
	int i;
	unsigned int value;
	unsigned char work[4];

	BYTE szDispLine1[32];
	//BYTE szDispLine2[32];
	BYTE szTime[7][32];
	BYTE szTemp[32];

	// History data block
	//  DDMMYYYYHHMMSS: Time and Date (7 bytes)
	//  PP			  : Padding(0x00)
	//  AAAAAAAA      : Value (4 bytes)
	//  BB            : Kind (0x00 means "Charge", 0x01 means "Payment") (1 byte)
	//  CC            : Padding (1 byte)
	//  DDDD          : Execution ID (2 byte)
	vdDebug_LogPrintf("\n");
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("  HISTORY\n");
	vdDebug_LogPrintf("-------------------------------------------\n");

	CTOS_LCDTPrintAligned(2, "-----HISTORY-----", d_LCD_ALIGNCENTER);

	for(i=0; i<6; i++) {
		vdDebug_LogPrintf("%02x", history[i][0]);
		vdDebug_LogPrintf("/");
		vdDebug_LogPrintf("%02x", history[i][1]);
		vdDebug_LogPrintf("/");
		vdDebug_LogPrintf("%02x", history[i][2]);
		vdDebug_LogPrintf("%02x", history[i][3]);
		vdDebug_LogPrintf(" ");
		vdDebug_LogPrintf("%02x", history[i][4]);
		vdDebug_LogPrintf(":");
		vdDebug_LogPrintf("%02x", history[i][5]);
		vdDebug_LogPrintf(":");
		vdDebug_LogPrintf("%02x", history[i][6]);
		vdDebug_LogPrintf(" ");

		memset(szDispLine1, 0x00, sizeof(szDispLine1));
		memset(szTime, 0x00, sizeof(szTime));

		#if 0
		sprintf(&szTime[0], "%02x", history[i][0]);
		strcat(szDispLine1, &szTime[0]);
		strcat(szDispLine1, "/");

		sprintf(&szTime[1], "%02x", history[i][1]);
		strcat(szDispLine1, &szTime[1]);
		strcat(szDispLine1, "/");

		sprintf(&szTime[2], "%02x", history[i][2]);
		strcat(szDispLine1, &szTime[2]);

		sprintf(&szTime[3], "%02x", history[i][3]);
		strcat(szDispLine1, &szTime[3]);

		strcat(szDispLine1, " ");
		

		sprintf(&szTime[4], "%02x", history[i][4]);
		strcat(szDispLine1, &szTime[4]);
		strcat(szDispLine1, ":");

		sprintf(&szTime[5], "%02x", history[i][5]);
		strcat(szDispLine1, &szTime[5]);
		strcat(szDispLine1, ":");

		sprintf(&szTime[6], "%02x", history[i][6]);
		strcat(szDispLine1, &szTime[6]);
		strcat(szDispLine1, " ");
		#endif
		
		work[0] = history[i][8];
		work[1] = history[i][9];
		work[2] = history[i][10];
		work[3] = history[i][11];
		if ( history[i][12] == 0x00 ) {
			vdDebug_LogPrintf("Charged ");
			strcat(szDispLine1, "Charged ");
			value = CharArrayToInt(work, 4);
		} else {
			vdDebug_LogPrintf("Paid    ");
			strcat(szDispLine1, "Paid    ");
			value = 0 - CharArrayToInt(work, 4);
		}

		vdDebug_LogPrintf("%6d", value);
		memset(szTemp, 0x00, sizeof(szTemp));
		sprintf(szTemp, "%6d", value);
		strcat(szDispLine1, szTemp);

		work[0] = history[i][14];
		work[1] = history[i][15];
		value = CharArrayToInt(work, 2);
		vdDebug_LogPrintf(" ID:%5d", value);
		//memset(szTemp, 0x00, sizeof(szTemp));
		//sprintf(szTemp, " ID:%5d", value);
		//strcat(szDispLine1, szTemp);

		CTOS_LCDTPrintAligned(i+3, szDispLine1, d_LCD_ALIGNLEFT);
		//CTOS_LCDTPrintAligned(i+3+1, szDispLine2, d_LCD_ALIGNRIGHT);

		vdDebug_LogPrintf("\n");
	}
	vdDebug_LogPrintf("*******************************************\n");
	vdDebug_LogPrintf("\n");
	return APP_SUCCESS;
}


long UpdateBalance(unsigned int newBalance, int amount, unsigned char method)
{
	long			_ret =0;
	unsigned char	_service_code_list[128], _block_list[128], _block_data[128], _read_data[128];
	unsigned char	_block_num;
	unsigned char	_exec_id[2], _current_time[7], _work_buf[4];
	unsigned long	_read_len;
	unsigned int	_balance;

	if ( method == METHOD_PAYMENT) {
		_service_code_list[0]	= 0x92; //Service code(Cashback/Decrement)
		_service_code_list[1]	= 0x40; //Service code(Cashback/Decrement)
	}
	else {
		_service_code_list[0]	= 0x90; //Service code(Direct Access)
		_service_code_list[1]	= 0x40; //Service code(Direct Access)
	}

	_service_code_list[2]	= 0x02; //Service key ver
	_service_code_list[3]	= 0x01; //Service key ver
	_service_code_list[4]	= 0x8C; //Service code
	_service_code_list[5]	= 0x40; //Service code
	_service_code_list[6]	= 0x02; //Service key ver
	_service_code_list[7]	= 0x01; //Service key ver

	_ret = MutualAuthV2WithFeliCa(SYSTEM_CODE, 2, _service_code_list);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	CTOS_Sound(2000, 35);
    CTOS_LEDSet(d_LED2, d_ON);

	// Read Current Data
	_block_num = 1;			//Number of Blocks
	_block_list[0] = 0x80;	//Block list
	_block_list[1] = 0x00;	//Block list

	_ret = ReadDataBlock(_block_num, _block_list, &_read_len, _read_data);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	// Check Status FLAG
	if((_read_data[0] != 0x00) || (_read_data[1] != 0x00)){
		return APP_ERROR;
	}

	// Get Balance(convert little endian to big endian)
	_balance = CharArrayToIntLE(&_read_data[3],4);

	// Get current exec id
	_exec_id[0] = _read_data[3+14];
	_exec_id[1] = _read_data[3+15];

	// Check data consistency
	if ( method == METHOD_PAYMENT) {
		if ( _balance - amount != newBalance) {
			return APP_ERROR;
		}
	}
	if ( method == METHOD_CHARGE) {
		if ( _balance + amount != newBalance) {
			return APP_ERROR;
		}
	}

	// Write new data
	_block_num = 0x02;		// Number of Block
	_block_list[0] = 0x80;	// Block list
	_block_list[1] = 0x00;	// Block list
	_block_list[2] = 0x81;	// Block list
	_block_list[3] = 0x00;	// Block list

	// E-money data block
	if ( method == METHOD_PAYMENT) {
		IntToCharArrayLE(amount, _work_buf);
	} else {
		IntToCharArrayLE(newBalance, _work_buf);
	}
	_update_exec_id (_exec_id);

	memcpy(&_block_data[0], _work_buf, 4);	// amount of payment/ new balance data
	memset(&_block_data[4], 0x00, 10);		// Zero
	_block_data[14] = _exec_id[0];			// Exec ID
	_block_data[15] = _exec_id[1];			// Exec ID


	// History data block
	//  DDMMYYYYHHMMSS: Time and Date (7 bytes)
	//  PPPP		  : Padding(0x00, 0x00)
	//  AAAAAAAA      : Charged/Paid Value(big endian 4 bytes)
	//  BB            : Operation type (0x00 means "Charge", 0x01 means "Payment") (1 byte)
	//  CC            : Padding (1 byte)
	//  DDDD          : Execution ID (2 byte)
	GetTimeAtBCD(_current_time);
	IntToCharArray(amount, _work_buf);

	memcpy(&_block_data[16], _current_time, 7);	// DateTime
	_block_data[16+7] = 0x00;					// Padding
	memcpy(&_block_data[16+8], _work_buf, 4);	// Charged/Paid value
	_block_data[16+12] = method;				// Operation type
	_block_data[16+13] = 0x00;					// Padding
	_block_data[16+14] = _exec_id[0];			// Exec ID
	_block_data[16+15] = _exec_id[1];			// Exec ID

	_ret = WriteDataBlock(_block_num, _block_num*2, _block_list, _block_data);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}
	
	return APP_SUCCESS;
}


long ReadHistory(int mode, unsigned char history[6][16])
{
	long _ret =0;
	unsigned char _service_num, _block_num;
	unsigned char _service_list[64], _block_list[128], _read_data[128];
	unsigned long _read_len;
	int i;

	if(mode == READ_WO_ENC){
		_service_num = 1;			// Number of service
		_service_list[0] = 0x8F;	// Service code
		_service_list[1] = 0x40;	// Service code

		_block_num = 6;			// Number of Blocks
		_block_list[0] = 0x80;	// Block list1
		_block_list[1] = 0x00;	// Block list1
		_block_list[2] = 0x80;	// Block list2
		_block_list[3] = 0x01;	// Block list2
		_block_list[4] = 0x80;	// Block list3
		_block_list[5] = 0x02;	// Block list3
		_block_list[6] = 0x80;	// Block list4
		_block_list[7] = 0x03;	// Block list4
		_block_list[8] = 0x80;	// Block list5
		_block_list[9] = 0x04;	// Block list5
		_block_list[10] = 0x80;	// Block list6
		_block_list[11] = 0x05;	// Block list6

		_ret = ReadDataWOEnc(_service_num, _service_list, _block_num, _block_list, &_read_len, _read_data);
	} else { // Read Block
		_block_num = 6;			// Number of Blocks
		_block_list[0] = 0x81;	// Block list1
		_block_list[1] = 0x00;	// Block list1
		_block_list[2] = 0x81;	// Block list2
		_block_list[3] = 0x01;	// Block list2
		_block_list[4] = 0x81;	// Block list3
		_block_list[5] = 0x02;	// Block list3
		_block_list[6] = 0x81;	// Block list4
		_block_list[7] = 0x03;	// Block list4
		_block_list[8] = 0x81;	// Block list5
		_block_list[9] = 0x04;	// Block list5
		_block_list[10] = 0x81;	// Block list6
		_block_list[11] = 0x05;	// Block list6

		_ret = ReadDataBlock(_block_num, _block_list, &_read_len, _read_data);
	}

	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	for(i=0; i<6; i++ ) {
		memcpy(history[i], &_read_data[3 + 16*i], 16);
	}

	return APP_SUCCESS;
}
long ReadBalance(unsigned int* balance)
{
	long			_ret =0;
	unsigned char	_service_num, _block_num;
	unsigned char	_service_list[64], _block_list[128], _read_data[128];
	unsigned long	_read_len;

	_service_num = 1;			// Number of service
	_service_list[0]	= 0x97; // Service code
	_service_list[1]	= 0x40; // Service code

	_block_num = 1;				// Number of Blocks
	_block_list[0] = 0x80;		// Block list
	_block_list[1] = 0x00;		// Block list

	vdDebug_LogPrintf("======ReadBalance=====");
	
	_ret = ReadDataWOEnc(_service_num, _service_list, _block_num, _block_list, &_read_len, _read_data);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	//Check Status FLAG
	if((_read_data[0] != 0x00) || (_read_data[0] != 0x00)){
		return APP_ERROR;
	}

	//Get Balance(little endian)
	*balance = CharArrayToIntLE(&_read_data[3],4);

	return APP_SUCCESS;
}

void _update_exec_id( unsigned char execID[] ) 
{
	unsigned int _id_val, _new_id_val;
	unsigned char _tmp[4];

	_id_val = CharArrayToInt(execID, 2);
	if ( _id_val == 0xFFFF ) {
		_new_id_val = 0;
	} else {
		_new_id_val = _id_val + 1;
	}

	IntToCharArray(_new_id_val, _tmp);
	execID[0] = _tmp[2];
	execID[1] = _tmp[3];
}

void vdFelica_PaymentFlow(void)
{
	ShowPaymentUI();
}

void vdFelica_ChargeFlow(void)
{
	ShowChargeUI();
}

void vdFelica_BalanceFlow(void)
{
	ShowBalanceUI();
}

#endif