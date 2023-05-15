/**
\file	Util.h
\brief	Utility functions
\date	2013/10/31
\date   2014.03.13 Deleted ToBCD()
\author	Copyright 2013 Sony Corporation
*/
#ifndef     _UTIL_H_
#define     _UTIL_H_

#ifndef APP_ERROR
#define APP_ERROR		-1
#endif
#define APP_CANCEL		-2

#ifndef APP_SUCCESS
#define APP_SUCCESS		0
#endif

/**
Convert big endian byte array to int value.
*/
unsigned int CharArrayToInt(unsigned char in[],
							unsigned int len);

/**
Convert int value to 4 bytes big endian byte array.
*/
void IntToCharArray(unsigned int in, 
					unsigned char out[4]);

/**
Convert little endian byte array to int value.
*/
unsigned int CharArrayToIntLE(unsigned char in[],
							unsigned int len);

/**
Convert int value to 4 bytes little endian byte array.
*/
void IntToCharArrayLE(unsigned int in, 
					unsigned char out[4]);


/**
Print out unsigned char array. 
*/
void PrintHexArray(char* header, 
				   unsigned int len, 
				   unsigned char byte_array[]);

/**
Print out text.
*/
void PrintText (char* text, ...);

/**
Get numeric value input from keyboard.
*/
int GetNumericValue(void);

/**
Get current time in BCD format

\param [out] timeData	current time in BCD format ([DD][MM][YY][YY][hh][mm][ss])
*/
void GetTimeAtBCD(unsigned char* timeData);

/**
Generate Random value.
*/
void GenerateRandom (unsigned char *buf,int num);

#endif
