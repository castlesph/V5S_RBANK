#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <ctype.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <vwdleapi.h>

#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/msg.h"

#include "Print.h"
#include "../FileModule/myFileFunc.h"
#include "../UI/Display.h"
#include "../Includes/POSTypedef.h"
#include "..\Includes\CTOSInput.h"
#include "../accum/accum.h"
#include "../DataBase/DataBaseFunc.h"
#include "..\debug\debug.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"
#include <epad.h>
#include "..\Includes\PosLoyalty.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Erm\PosErm.h"
#include "..\Comm\V5Comm.h"
//#include "../UIapi.h"
#include "../Debug/debug.h"
#include <emv_cl.h>



const BYTE baPrinterBufferLogo_Single[]={ //Width=60, Height=49
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x18,0x8C,0xC4,0xC6,
    0x66,0x66,0x22,0x22,0x22,0x22,0x22,0x62,0x66,0x46,0xC4,0x84,0x8C,0x18,0x10,0x30,
    0x60,0xC0,0xC0,0xE0,0x38,0x0E,0x07,0x1E,0x38,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0xC0,0xF8,0x0E,0x03,0x81,0xF0,0x1C,0x0E,0x03,0xC1,0xF0,0xF8,
    0x7C,0x3C,0x1C,0x1E,0x1E,0x1E,0xFE,0xFC,0xFC,0xFC,0xF8,0xF0,0xC1,0x83,0xCE,0x7C,
    0x1C,0x07,0x81,0xE0,0x70,0x1C,0x0E,0x1C,0x70,0xC0,0x83,0x07,0x1C,0x70,0xE0,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0x0F,0x7F,0xC0,0x00,0x03,0x3F,0xF0,0x80,0x00,0x0F,0x3F,0x7F,
    0xFF,0xFC,0xFC,0xFC,0xFC,0xFE,0xF3,0xFB,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x81,0xE0,
    0x38,0x0E,0x03,0xC0,0xF0,0x1C,0x1C,0x38,0xE0,0x81,0x03,0x0E,0x38,0x70,0xC0,0x03,
    0x0E,0x1C,0x70,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,
    0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x08,0x18,0x38,0x30,0x00,0x00,0x00,0x00,
    0x80,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x80,0xC0,
    0xC0,0xC0,0xC0,0x80,0x00,0x00,0xC0,0xFC,0xFC,0xFC,0xC0,0xC0,0x00,0xFC,0xFC,0xFC,
    0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,
    0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,
    0x0C,0xFC,0xFC,0xFC,0x0C,0x0C,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,
    0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,
    0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,
    0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
    0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,
    0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,
    0xC0,0xC0,0xC0,0x80,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x40,0xC0,0xC0,0x80,0x00,0x00,
    0x00,0x00,0x80,0xC0,0xC0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,
    0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x38,0x30,0x00,0x00,0x00,0x00,
    0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0xC0,0xC0,0xFC,0xFC,0xFC,0xC0,0x00,0x00,
    0x00,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0E,0x18,0x30,0x61,0x63,0xC6,0x8C,0x88,
    0x98,0x98,0x11,0x11,0x11,0x11,0x11,0x11,0x90,0x98,0x88,0x8C,0xC6,0x63,0x61,0x30,
    0x18,0x0C,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x1C,0x70,0xE0,0x81,0x07,
    0x0E,0x38,0xE0,0xC1,0x07,0x0E,0x38,0x70,0xC0,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,
    0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x00,0x3F,
    0xFF,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0xCF,0x8F,
    0x9E,0x9C,0xFC,0xF8,0x20,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,
    0x00,0x00,0x3E,0x7F,0xFF,0xCD,0x8C,0x8C,0x8C,0x8C,0xCF,0xCF,0x0E,0x08,0x00,0x03,
    0xC7,0x8F,0x8E,0x9C,0xFC,0xF9,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3E,0x7F,0xFF,0xCD,0xCC,0x8C,0x8C,0x8C,
    0xCF,0xCF,0x0F,0x0C,0x00,0x00,0x1C,0x7F,0xFF,0xE3,0xC1,0x80,0x80,0x80,0xC1,0xC1,
    0x41,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
    0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x01,0xFF,0xFF,0xFF,0x00,0x00,0x08,0x3E,0x7F,0xFF,
    0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x3E,
    0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xE3,0x7F,0x3F,0x08,0x00,0x0C,0x7F,0xFF,0xE3,
    0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0x0F,0x3F,0xFC,0xF0,
    0xF8,0x7E,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,
    0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x08,0x3E,
    0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0x80,0xC0,0xC0,0x00,
    0x00,0x00,0x00,0x00,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x1C,
    0x7F,0xFF,0xE1,0xC0,0x80,0x80,0xC0,0xE1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xC0,0xC0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
    0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0E,0x0E,
    0x0D,0x09,0x09,0x08,0x0C,0x0F,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x0C,0x0F,0x0F,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
    0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

#define DB_SIGN_BMP_GZ "signtest.bmp.gz"

TRANS_TOTAL stBankTotal;
BOOL fRePrintFlag=FALSE;
BOOL fRePrintPwrFailFlag=FALSE;

static CTOS_FONT_ATTRIB stgFONT_ATTRIB;

BYTE szgTempDate[DATE_BCD_SIZE+1];
BYTE szgTempTime[TIME_BCD_SIZE+1];

extern BYTE szIP[50+1];
extern BYTE szDNS1[50+1];
extern BYTE szDNS2[50+1];
extern BYTE szGateWay[50+1];
extern BYTE szSubnetMask[50+1];

int printcopies_cntr = 0;

BOOL fPrinterStart;
BOOL fERMTransaction = FALSE; // Enhancement for printing speed
//extern BOOL gblDCCTrans;
//extern BOOL gblDCCReceipt;


extern unsigned char gblszMarginRatePercentage[10+1], gblszExchangeRate[12+1], gblszDCCCurrencySymbol[3+1],  gblszCurrencyCode[3+1], gblszForeignAmount[15+1],  gblszLocalAmount[15+1], gblszCurrForeignAmount[15+1], gblszDCCCurrBaseAmount[15+1], gblszCurrDCCTipAmount[15+1];
extern int gblinMinorUnit;

void vdSetGolbFontAttrib(USHORT FontSize, USHORT X_Zoom, USHORT Y_Zoom, USHORT X_Space, USHORT Y_Space)
{
    memset(&stgFONT_ATTRIB, 0x00, sizeof(stgFONT_ATTRIB));

    stgFONT_ATTRIB.FontSize = FontSize;      // Font Size = 12x24
	stgFONT_ATTRIB.X_Zoom = X_Zoom;		    // The width magnifies X_Zoom diameters
	stgFONT_ATTRIB.Y_Zoom = Y_Zoom;		    // The height magnifies Y_Zoom diameters

    stgFONT_ATTRIB.X_Space = X_Space;      // The width of the space between the font with next font
    stgFONT_ATTRIB.Y_Space = Y_Space;      // The Height of the space between the font with next font

}

int isCheckTerminalMP200(void)
{
    if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
        return d_OK;
    return d_NO;
}

short printCheckPaper(void)
{
	unsigned short inRet;
	unsigned char key;
	int inLine = 3;

	BYTE szTitle[25+1];
	BYTE szDisMsg[256];
	int inKey=0;

	if( (strTCT.byTerminalType%2) == 0 )
		inLine=2;

	//CTOS_LCDTClearDisplay();

	while(1)
	{
		inRet = CTOS_PrinterStatus();
		vdDebug_LogPrintf("saturn  - printCheckPaper inRet[%02x]",inRet );
		if (inRet==d_OK)
			return d_OK;
		else if(inRet==d_PRINTER_PAPER_OUT)
		{
		          #if 0
			vduiClearBelow(inLine);
			vduiWarningSound();
			vduiDisplayStringCenter(3,"PRINTER OUT OF");
			vduiDisplayStringCenter(4,"PAPER, INSERT");
			vduiDisplayStringCenter(5,"PAPER AND PRESS");
			vduiDisplayStringCenter(6,"ANY TO PRINT.");
			//vduiDisplayStringCenter(8,"[X] CANCEL PRINT");

			CTOS_KBDGet(&key);
			/*
			if(key==d_KBD_CANCEL)
			{
				CTOS_LCDTClearDisplay();
				return -1;
			}
			*/
			#else
			//CTOS_LCDTClearDisplay();
			memset(szTitle, 0x00, sizeof(szTitle));
			szGetTransTitle(srTransRec.byTransType, szTitle);
			
			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "PRINTER OUT OF PAPER");	
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "INSERT PAPER AND PRESS OK");	
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "TO PRINT");	
              			
			CTOS_Beep();
			//usCTOSS_ConfirmOK(szDisMsg);
			inDisplayMessageBoxWithButton(1,8,"PRINTER OUT OF PAPER","INSERT PAPER AND PRESS OK","TO PRINT", MSG_TYPE_WARNING); // sidumili: added

			#endif
		}
		if(isCheckTerminalMP200() == d_OK)
			return d_OK;
	}
}


#if 0
inRet = CTOS_PrinterStatus();
        vdDebug_LogPrintf("CTOS_PrinterStatus inRet=[%d]",inRet);
        if (inRet==d_OK)
 {
            CTOS_KBDBufFlush();//cleare key buffer
            return 0;
        }
        else if(inRet==d_PRINTER_PAPER_OUT)
 {
            vduiClearBelow(3);
            vduiWarningSound();
{
            if (d_OK  isCheckTerminalMP200())
                return d_OK;
       }
    }
}
#endif

void cardMasking(char *szPan, int style)
{
    int num;
    int i;
	char szTemp[30+1];

    if (style == PRINT_CARD_MASKING_1)
    {
        num = strlen(szPan) - 10;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[6+i] = '*';
            }
        }
    }
    else if (style == PRINT_CARD_MASKING_2)
    {
        num = strlen(szPan) - 12;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[12+i] = '*';
            }
        }
    }
    else if (style == PRINT_CARD_MASKING_3)
    {
        memset(szTemp, 0x00, sizeof(szTemp));
        strcpy(szTemp,"****************");
        num = strlen(szPan) - 4;
        memcpy(&szTemp[num],&szPan[num],4);
		szTemp[strlen(szPan)]=0x00;
        strcpy(szPan,szTemp);
    }
}

void vdCTOS_FormatDate(char *pchDate)
 {
	char szLocal[6 + 1] = {0};
	char szMonthNames[36 + 1] = {0};
	short Month = 0;

	strcpy(szLocal, pchDate);
	Month = (szLocal[2] - '0') * 10 + (szLocal[3] - '0') - 1;
	strcpy(szMonthNames, "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	sprintf(pchDate, "%.3s.%.2s,20%.2s", &(szMonthNames[Month * 3]), &(szLocal[4]), szLocal);

	pchDate[13] = ((char) 0);
}


USHORT printDateTime(BOOL fTrans)
{
	BYTE   EMVtagVal[64];
	USHORT EMVtagLen;
	int result;
	char szStr[35 + 1];
	CTOS_RTC SetRTC;
	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
	BYTE baTemp[PAPER_X_SIZE * 64];

	vdDebug_LogPrintf("--printDateTime--");
	vdDebug_LogPrintf("szTranYear[%s]", srTransRec.szTranYear);
	vdDebug_LogPrintf("fTrans[%d]", fTrans);

	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);

	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);

	memset(szTempDate, 0x00, sizeof(szTempDate));
	if (fTrans)
	{
		if (strlen(srTransRec.szTranYear) > 0)
			sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szTranYear), atol(szTemp)/100, atol(szTemp)%100);
		else
			sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	}
	else
		sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);

	vdCTOS_FormatDate(szTempDate);
	sprintf(szTemp3, "DATE: %s", szTempDate);
	sprintf(szTemp4, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	inPrintLeftRight(szTemp3, szTemp4, 46);

	return(ST_SUCCESS);

}

USHORT printTIDMID(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr1[d_LINE_SIZE + 1];

    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];

	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "TID: %s", srTransRec.szTID);

	memset(szStr1, ' ', d_LINE_SIZE);
	sprintf(szStr1, "MID: %s", srTransRec.szMID);

	inPrintLeftRight(szStr, szStr1, 46);

    return d_OK;

}

USHORT printBatchInvoiceNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr1[d_LINE_SIZE + 1];

    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];

    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp2, ' ', d_LINE_SIZE);

	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
	wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
	vdMyEZLib_LogPrintf("invoice no: %s",szTemp2);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH NUM: %s", szTemp1);
	//result=inPrint(szStr);

	memset(szStr1, 0x00, d_LINE_SIZE);
	sprintf(szStr1, "TRACE NO.: %s", szTemp2);
	//result=inPrint(szStr);

	inPrintLeftRight(szStr, szStr1, 46);

    return d_OK;
}

USHORT printBatchNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl

    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);

    wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
    sprintf(szStr, "BATCH NO: %s", szTemp);
    //memset (baTemp, 0x00, sizeof(baTemp));

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
    inPrint(szStr);

    //Terminal serial number - mfl
    memset(szTemp, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));
    memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
    //CTOS_GetFactorySN(szTermSerialNum);
    usGetSerialNumber(szTermSerialNum);
    szTermSerialNum[15]=0; // eliminate check digit

    //sprintf(szStr, "TERMINAL SERIAL NUM: %s", szTermSerialNum);
    sprintf(szStr, "TSN: %s", szTermSerialNum);

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
    inPrint(szStr);

    return(result);
}

USHORT printHostLabel(void)
{
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;

    memset(szStr, 0x00, sizeof(szStr));
	if(memcmp(srTransRec.szHostLabel, "MCC ONE APP", strlen("MCC ONE APP")) == 0)
	    sprintf(szStr, "HOST: %s", "MC/VISA/JCB/UPI/AMEX");
    else
        sprintf(szStr, "HOST: %s", srTransRec.szHostLabel);
    //memset (baTemp, 0x00, sizeof(baTemp));

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
    inPrint(szStr);
    return(result);
}


void vdPrintCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];

	//vdDebug_LogPrintf("vdPrintCenter | fERMTransaction[%d]", fERMTransaction);

    if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 32;
    else if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 16;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 48;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 24;
    else
        usCharPerLine = 32;

    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;

	memset(tucPrint,0x20,55);
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);

	tucPrint[i+spacestring]=0;
    memset (baTemp, 0x00, sizeof(baTemp));

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB, fERMTransaction);
}

void vdPrintTitleCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 24;

    //BYTE baTemp[PAPER_X_SIZE * 64];

	//vdDebug_LogPrintf("vdPrintTitleCenter | fERMTransaction[%d]", fERMTransaction);
	if ((isCheckTerminalMP200() == d_OK && strTCT.byERMMode == 2))
	{
		usCharPerLine = 46;
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
    	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);


    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;

	memset(tucPrint,0x20,30);

	memcpy(tucPrint+spacestring,strIn,usCharPerLine);

	tucPrint[i+spacestring]=0;

    //memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB, fERMTransaction);
	//inPrint(tucPrint);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

}

USHORT printDividingLine(int style)
{
    if (style == DIVIDING_LINE_DOUBLE)
        return(inPrint("============================================="));
    else
        return(inPrint("---------------------------------------------"));
}

void vdCTOSS_GetEMVTagsDescription(USHORT usTag, char *szValue)
{
    switch(usTag)
    {
        case TAG_50:
            strcpy(szValue, "Appl Label");
            break;

        case TAG_57:
            strcpy(szValue, "Track2 Data");
            break;

        case TAG_5F2A_TRANS_CURRENCY_CODE:
            strcpy(szValue, "Txn CurrCode");
            break;

        case TAG_5F34_PAN_IDENTFY_NO:
            strcpy(szValue, "PAN Seq Num");
            break;

        case TAG_82_AIP:
            strcpy(szValue, "AIP");
            break;

        case TAG_8A_AUTH_CODE:
            strcpy(szValue, "Auth Code");
            break;

        case TAG_8F:
            strcpy(szValue, "CAPK Index");
            break;

        case TAG_91_ARPC:
            strcpy(szValue, "ARPC");
            break;

        case TAG_95:
            strcpy(szValue, "TVR");
            break;

        case TAG_9A_TRANS_DATE:
            strcpy(szValue, "Txn Date");
            break;

        case TAG_9B:
            strcpy(szValue, "TSI");
            break;

        case TAG_9C_TRANS_TYPE:
            strcpy(szValue, "Txn Type");
            break;

        case TAG_9F02_AUTH_AMOUNT:
            strcpy(szValue, "Txn Amt");
            break;

        case TAG_9F03_OTHER_AMOUNT:
            strcpy(szValue, "Other Amt");
            break;

        case TAG_9F10_IAP:
            strcpy(szValue, "IAP");
            break;

        case TAG_9F12:
            strcpy(szValue, "Appl Pref Name");
            break;

        case TAG_9F1A_TERM_COUNTRY_CODE:
            strcpy(szValue, "Term CurrCode");
            break;

        case TAG_9F1B_TERM_FLOOR_LIMIT:
            strcpy(szValue, "FloorLimit");
            break;

        case TAG_9F26_EMV_AC:
            strcpy(szValue, "AC");
            break;

        case TAG_9F27:
            strcpy(szValue, "CID");
            break;

        case TAG_9F33_TERM_CAB:
            strcpy(szValue, "Term Cab");
            break;

        case TAG_9F34_CVM:
            strcpy(szValue, "CVM");
            break;

        case TAG_9F36_ATC:
            strcpy(szValue, "ATC");
            break;

        case TAG_9F37_UNPREDICT_NUM:
            strcpy(szValue, "Unpredict Num");
            break;

        case TAG_9F5B:
            strcpy(szValue, "Script Result");
            break;

        case TAG_71:
            strcpy(szValue, "Issuer Script1");
            break;

        case TAG_72:
            strcpy(szValue, "Issuer Script2");
            break;

        default :
            szValue[0] = 0x00;
            break;
    }
}

void vdPrintEMVTags(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    USHORT usPrintTag;
    USHORT usPrintTagLen;
    USHORT usOffset;
    char szStr[d_LINE_SIZE + 1];
    char szTagDesp[50];
    char szEMVTagList[512];
    char szEMVTagListHex[256];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
    int shHostIndex, inRet=0;

    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

	CTOS_LCDTClearDisplay();

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRINT_EMV_TAGS);
        if(d_OK != inRet)
              return ;
    }
	else
	{
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return ;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return ;
        }
    }

    if( printCheckPaper()==-1)
        return;

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);

	if (isCheckTerminalMP200() != d_OK)
	    inCTOS_DisplayPrintBMP();

    //vdSetPrintThreadStatus(1); //Not displaying "Printing" while receipt is printing. White screen / Host Selection Menu appears while printing - ##00129

    CTOS_LanguagePrinterFontSize(d_FONT_16x16, 0, TRUE);

	vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

    vdPrintTitleCenter("EMV TAGS DATA");

	vdCTOS_PrinterFline(1);
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    memset(szOutEMVData,0x00,sizeof(szOutEMVData));

//	  usCTOSS_EMV_MultiDataGet(PRINT_EMV_TAGS_LIST, &inTagLen, szOutEMVData);
//	  DebugAddHEX("PRINT_EMV_TAGS_LIST",szOutEMVData,inTagLen);

///////////////////////////////////////////////////////////////
// patrick to print contactless EMV data 20151019
	inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
	vdPCIDebug_HexPrintf("CARD_ENTRY_WAVE", srTransRec.baChipData, srTransRec.usChipDataLen);
////////////////////////////////////////////////////////////////////

    memset(szEMVTagList,0x00,sizeof(szEMVTagList));
    memset(szEMVTagListHex,0x00,sizeof(szEMVTagListHex));
    strcpy(szEMVTagList, PRINT_EMV_TAGS_LIST);
    wub_str_2_hex(szEMVTagList, szEMVTagListHex, strlen(szEMVTagList));

    usOffset = 0;
    while(szEMVTagListHex[usOffset] != 0x00)
    {
        vdCTOS_PrinterFline(1);

        memset(szStr, 0x00, sizeof(szStr));
        if ((szEMVTagListHex[usOffset] & 0x1F) == 0x1F)	// If the least 5 bits of the first byte are set, it is a two byte Tag
    	{
    		usPrintTag = szEMVTagListHex[usOffset];
    		usPrintTag = ((usPrintTag << 8) | szEMVTagListHex[usOffset+1]);
    		usPrintTagLen = 2;
    	}
    	else
    	{
    		usPrintTag = szEMVTagListHex[usOffset];
            usPrintTag = usPrintTag & 0x00FF;
            usPrintTagLen = 1;
    	}
        memcpy(szStr, &szEMVTagList[usOffset*2], usPrintTagLen*2);
        usOffset += usPrintTagLen;

        memset(szTagDesp,0x00,sizeof(szTagDesp));
        vdCTOSS_GetEMVTagsDescription(usPrintTag, szTagDesp);
        vdDebug_LogPrintf("szStr[%s] usPrintTag[%X] szTagDesp[%s]", szStr, usPrintTag, szTagDesp);
        vdMyEZLib_Printf("%s (%s)", szStr,szTagDesp);
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;
        usCTOSS_FindTagFromDataPackage(usPrintTag, EMVtagVal, &EMVtagLen, szOutEMVData, inTagLen);
        memset(szStr, ' ', d_LINE_SIZE);
        if(EMVtagLen == 0)
            memcpy(szStr,"No DATA" , 7);
        else
            wub_hex_2_str(EMVtagVal, szStr, EMVtagLen);
        vdMyEZLib_Printf("Len:%d val:%s", EMVtagLen, szStr);
    }

    vdPrintTitleCenter("*** END OF REPORT ***");

	vdCTOS_PrinterFline(1);

    vdLineFeed(FALSE);

	vdCTOSS_PrinterEnd();
}

void vdPrintTerminalConfig(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    BYTE szStr[50];
    BYTE szBuf[50];
    BYTE baTemp[PAPER_X_SIZE * 64];

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    CTOS_PrinterSetHeatLevel(4);

	ushCTOS_PrintHeader(0);

    CTOS_PrinterFline(d_LINE_DOT * 1);

    memset(szStr, 0x00, sizeof(szStr));
    memset(szBuf, 0x00, sizeof(szBuf));
    wub_hex_2_str(strTCT.szInvoiceNo, szBuf, 3);
    sprintf(szStr, "INVOICE NO: %s", szBuf);
    memset (baTemp, 0x00, sizeof(baTemp));
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "PABX: %s", strTCT.szPabx);
    memset (baTemp, 0x00, sizeof(baTemp));
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

    memset (baTemp, 0x00, sizeof(baTemp));
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

    //check host num
    inNumOfHost = inHDTNumRecord();

    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            inCPTRead(inNum);

            CTOS_PrinterFline(d_LINE_DOT * 1);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "HOST: %s", strHDT.szHostLabel);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            memset(szBuf, 0x00, sizeof(szBuf));
            wub_hex_2_str(strHDT.szTPDU, szBuf, 5);
            sprintf(szStr, "TPDU: %s", szBuf);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            if(strCPT.inCommunicationMode == ETHERNET_MODE)
            {
                sprintf(szStr, "COMM TYPE: %s", "ETHERNET");
            }
            else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
            {
                sprintf(szStr, "COMM TYPE: %s", "DIAL_UP");
            }
            else if(strCPT.inCommunicationMode == GPRS_MODE)
            {
                sprintf(szStr, "COMM TYPE: %s", "GPRS");
            }
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI NUM : %s", strCPT.szPriTxnPhoneNumber);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC NUM : %s", strCPT.szSecTxnPhoneNumber);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI IP  : %s", strCPT.szPriTxnHostIP);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC IP  : %s", strCPT.szSecTxnHostIP);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum);
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);

            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
                if((inResult = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else
                {
                    if(strMMT[0].fMMTEnable)
                    {
                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MERCHANT:%s", strMMT[0].szMerchantName);
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "TID : %s", strMMT[0].szTID);
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MID : %s", strMMT[0].szMID);
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

                        memset(szStr, 0x00, sizeof(szStr));
                        memset(szBuf, 0x00, sizeof(szBuf));
                        wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
                        sprintf(szStr, "BATCH NO: %s", szBuf);
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

                    }

                }
            }
        }
        else
            continue;

    }

    CTOS_PrinterFline(d_LINE_DOT * 5);

    return ;
}

void printCardHolderName(void)
{
    USHORT shLen;
    char szStr[d_LINE_SIZE + 1];

    shLen = strlen(srTransRec.szCardholderName);

    if(shLen > 0)
    {
        memset(szStr, 0x00, d_LINE_SIZE);
        strcpy(szStr, srTransRec.szCardholderName);
		szStr[31] = 0x00; //#00192 - Incorrect number of characters of Cardholder's Name in original and duplicate receipt

	// old code
        //while(0x20 == szStr[--shLen] && shLen >= 0)
           // szStr[shLen] = 0x00;

    	//vdPrintCenter(szStr);
    	inPrint(szStr);
    }
}



void printCardHolderNameEntryMode(void)
{
    USHORT shLen;
    char szStr[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];

    shLen = strlen(srTransRec.szCardholderName);

    memset(szStr, 0x00, d_LINE_SIZE);
    if(shLen > 0)
    {
        strcpy(szStr, srTransRec.szCardholderName);
		szStr[31] = 0x00; //#00192 - Incorrect number of characters of Cardholder's Name in original and duplicate receipt
    }

    memset(szTemp1, 0, sizeof(szTemp1));

    if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
        memcpy(szTemp1,"ICC",3);
    else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
        memcpy(szTemp1,"Manual",6);
    else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
        memcpy(szTemp1,"MSR",3);
    else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
        memcpy(szTemp1,"Fallback",8);
    //else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
    //memcpy(szTemp1,"CONTACTLESS",11);

	//#00059 - Terminal prints card no. Twice when process as manual entry
	if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL || srTransRec.fDebit == TRUE)
		inPrint(szTemp1);
	else
		inPrintLeftRight(szStr, szTemp1, 46);


}

void vdPrintFormattedAmount(unsigned char *prtDisplay, unsigned char *prtAmount, int inWidth)
{
    char szAmtBuf[24+1] = { 0 };
	char szStr[24+1] = { 0 };
	char szStr2[24+1] = { 0 };
	unsigned long inDCCAmount;
	char szDCCAmt[12+1];

	memset(szStr, 0, sizeof(szStr));
	memset(szStr2, 0, sizeof(szStr2));
	memset(szAmtBuf, 0, sizeof(szAmtBuf));

	if(srTransRec.byVoided == TRUE)
         {
              if(srTransRec.fDCCOptin== TRUE)
               {
                    szAmtBuf[0]='-';
		 inCSTReadCurrencySymbol(srTransRec.szCurrencyCode, srTransRec.szDCCCurrencySymbol, srTransRec.szAmountFormat);
                    gblinMinorUnit= strCST.inMinorUnit;
                    inDCCAmount=atol((char *)srTransRec.szForeignAmount);
                    sprintf(szDCCAmt,"%ld",inDCCAmount);
                    //vdDCCModifyAmount(szDCCAmt,&szAmtBuf[1]);
                     vdCTOS_FormatAmount(srTransRec.szAmountFormat,szDCCAmt, &szAmtBuf[1]);
               }
              else
              {
                   szAmtBuf[0]='-';
                   vdCTOS_FormatAmount("NNN,NNN,NNn.nn", prtAmount, &szAmtBuf[1]);
              }
         }
	else
	    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", prtAmount, szAmtBuf);

	vdCTOS_Pad_String(szAmtBuf, 12, 0x20, POSITION_LEFT);
	strcpy(szStr, prtDisplay);

         if((srTransRec.fDCCOptin== TRUE) && (srTransRec.byTransType==VOID))
              sprintf(szStr2, "%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf);
         else
              sprintf(szStr2, "%s %s", strCST.szCurSymbol, szAmtBuf);

	inPrintLeftRight(szStr, szStr2, inWidth);
}

USHORT ushCTOS_PrintDemo(void)
{
    BYTE strIn[40];
    unsigned char tucPrint [24*4+1];
    short i,spacestring;
    USHORT usCharPerLine = 24;
    BYTE baTemp[PAPER_X_SIZE * 64];

    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    //vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    memset(strIn, 0x00, sizeof(strIn));
    strcpy(strIn, "DEMO");
    i = strlen(strIn);
    spacestring=(usCharPerLine-i)/2;

    memset(tucPrint,0x20,30);

    memcpy(tucPrint+spacestring,strIn,usCharPerLine);

    tucPrint[i+spacestring]=0;

    //CTOS_PrinterFline(d_LINE_DOT * 1);

    //memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
    inPrint(tucPrint);

    //CTOS_PrinterFline(d_LINE_DOT * 1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    return d_OK;

}



USHORT ushCTOS_PrintHeader(int page)
{

    vdDebug_LogPrintf("ushCTOS_PrintHeader-1");
    vdDebug_LogPrintf("Len[%d].szHeaderLogoName[%s]", strlen(strHDT.szHeaderLogoName), strHDT.szHeaderLogoName);
    vdDebug_LogPrintf("fRePrintFlag[%d]", fRePrintFlag);

	if(strTCT.feReceiptEnable == 0 || page == 0)
	{		
		//print Logo
		if(strlen(strHDT.szHeaderLogoName) > 0)
        {
		    vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
        }
		else
			vdCTOSS_PrinterBMPPic(0, 0, "./fs_data/logo.bmp");  
	}
	
    
    vdDebug_LogPrintf("ushCTOS_PrintHeader-2");


    //vdCTOSS_PrinterBMPPicEx(0, 0, "/data/data/com.Source.S1_MCC.MCC/logo.bmp");
#if 0
    {
	char szTemp[50+1];
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp, "ushCTOS_PrintHeader:[%d]:[%d]", srTransRec.HDTid, srTransRec.MITid);
    	CTOS_PrinterPutString(szTemp);

}
#endif

    if(fRePrintFlag == TRUE)
    {
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
        if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }

    vdDebug_LogPrintf("ushCTOS_PrintHeader-3");

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    //merhcant name and address
    if(strlen(strMMT[0].szRctHdr1) > 0)
        vdPrintCenter(strMMT[0].szRctHdr1);
    if(strlen(strMMT[0].szRctHdr2) > 0)
        vdPrintCenter(strMMT[0].szRctHdr2);
    if(strlen(strMMT[0].szRctHdr3) > 0)
        vdPrintCenter(strMMT[0].szRctHdr3);
    if(strlen(strMMT[0].szRctHdr4) > 0)
        vdPrintCenter(strMMT[0].szRctHdr4);
    if(strlen(strMMT[0].szRctHdr5) > 0)
        vdPrintCenter(strMMT[0].szRctHdr5);

    //CTOS_PrinterFline(d_LINE_DOT * 1);

    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

    if(fRePrintFlag == TRUE)
    {
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        vdPrintCenter("DUPLICATE");
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        //vdCTOS_PrinterFline(1);
    }

    vdCTOS_PrinterFline(1);

    vdDebug_LogPrintf("ushCTOS_PrintHeader-end");
    return d_OK;
}

USHORT ushCTOS_PrintInstallmentDetails(void)
{
	char szTemp[d_LINE_SIZE+1];
	char szStr[d_LINE_SIZE+1];
	char szTempAmount[12+1];
	char szTempAmountHex[6+1];
    	TRANS_DATA_TABLE* srTransPara;

    	//CTOS_PrinterPutString("ushCTOS_PrintInstallmentDetails");
	//vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	memset(szTemp, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	vdPrintFormattedAmount("TOTAL               :", szTemp, 46); // #00207 - Amount on Installment receipt should be left justify

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	inPrintLeftRight("PROMO               :", srTransRec.szPromoLabel, 46);

	inPrintLeftRight("TERMS               :", srTransRec.szTerms1, 46);//srTransRec.szTerms, 46);

	memset(szTemp, 0x00, d_LINE_SIZE);
	sprintf(szTemp, "%012ld", atol(srTransRec.szTotalInstAmnt));
	vdPrintFormattedAmount("GROSS AMT           :", szTemp, 46);

	// original script
	memset(szStr, 0x00, d_LINE_SIZE);
	memset(szTemp, 0x00, d_LINE_SIZE);
	//sprintf(szTemp, "%d", (int)srTransRec.szFactorRate);
	// #00033 Incorrect percentage data printed on receipt
	vdCTOSvdTrimLeadZeroes(srTransRec.szFactorRate);

	inPrintLeftRight("RATE                :", srTransRec.szFactorRate, 46);

	//inPrintLeftRight("RATE                :", srTransRec.szFactorRate, 46);

	memset(szTemp, 0x00, d_LINE_SIZE);
	sprintf(szTemp, "%012ld", atol(srTransRec.szMoAmort));
	vdPrintFormattedAmount("MONTHLY AMORTIZATION:", szTemp, 46);
}

USHORT ushCTOS_PrintBody(int page)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr2[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    short spacestring;
    BYTE   key;
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl


    // fix for Wrong implementation of USD Currency
    //USD currency prompts upon voiding & settlement
    //Terminal prints incorrect amount currency on duplicate receipt (PHP Profile only)
    // #00166
    inTCTRead(1);
    if(strTCT.fRegUSD == 1)
        inCSTRead(2);
    else
        inCSTRead(1);


    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    if(CUSTOMER_COPY_RECEIPT == page)
    {
		if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
			vdPrintCenter("***CONTACTLESS***");
		
         if (strcmp(srTransRec.szAuthCode,"Y1")==0)
         	{
         	       inPrint("SALE");
         	}
	else
         {
              szGetTransTitle(srTransRec.byTransType, szStr);
              inPrint(szStr);
         }
        printDateTime(TRUE);
        printTIDMID();
        printBatchInvoiceNO();
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

        memset(szStr, ' ', d_LINE_SIZE);
        //if(srTransRec.fDebit == TRUE && strcmp(srTransRec.szCardLable, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
        if(srTransRec.fDebit == TRUE)
			strcpy(szStr,"MCC DEBIT");
        else
            sprintf(szStr, "%s", srTransRec.szCardLable);

        memset (baTemp, 0x00, sizeof(baTemp));
        vdCTOS_FormatPANEx(strIIT.szMaskCustomerCopy, srTransRec.szPAN, baTemp, strIIT.fMaskCustCopy);

        inPrintLeftRight(szStr, baTemp, 46);


        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        printCardHolderNameEntryMode();

        //Reference num
        if(srTransRec.fDebit != TRUE)
        {
            memset(szStr, ' ', d_LINE_SIZE);
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szStr, ' ', d_LINE_SIZE);
            sprintf(szStr, "REF NO.   : %s", srTransRec.szRRN);
            inPrintLeftRight(szStr, " ", 46);
	  
        }

        //Auth response code
        // #00158 No Account type printed on DEBIT receipt
        //if(strCDT.inType == DEBIT_CARD)
        //if(srTransRec.fDebit == TRUE && strCDT.inType == DEBIT_CARD)
        if(srTransRec.fDebit == TRUE)
        {
            char szTemp[100+1];
            char szAccntType[40+1];
            memset(szTemp, ' ', sizeof(szTemp));
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szAccntType, ' ', sizeof(szAccntType));

            if(srTransRec.inAccountType == CURRENT_ACCOUNT)
                strcpy(szAccntType, "CURRENT");
            else if(srTransRec.inAccountType == SAVINGS_ACCOUNT)
                strcpy(szAccntType, "SAVINGS");

            sprintf(szTemp, "APPR. CODE: %s   ACCNT TYPE:  %s", srTransRec.szAuthCode, szAccntType);
            inPrint(szTemp);
        }
        else
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            memset(szStr2, ' ', d_LINE_SIZE);
            sprintf(szStr2, "APPR. CODE: %s", srTransRec.szAuthCode);
            inPrintLeftRight(" ", szStr2, 24);
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        }

        //Terminal serial number - mfl
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);

		usGetSerialNumber(szTermSerialNum);
		
		vdDebug_LogPrintf("saturn print serial number %s", szTermSerialNum);
        szTermSerialNum[15]=0;

        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

		vdPrintFSRMode();

        vdCTOS_PrinterFline(1); // space after printing trans title.

		vdPrintECRMessage(); // Mercury Requirements -- sidumili

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);
        memset(szTemp1, ' ', d_LINE_SIZE);
        memset(szTemp3, ' ', d_LINE_SIZE);
        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
        wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        if ((srTransRec.byTransType == SALE || srTransRec.byTransType == VOID ||srTransRec.byTransType == SALE_OFFLINE)&&(srTransRec.byPackType != VOID_REFUND))
        {
            if(srTransRec.fInstallment || srTransRec.fCash2Go)
            {
                ushCTOS_PrintInstallmentDetails();
            }
            else
            {
                //Tip amount
                memset(szStr, ' ', d_LINE_SIZE);
                sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
                memset (baTemp, 0x00, sizeof(baTemp));
                memset (szTemp4, 0x00, sizeof(szTemp4));
                wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);

                DebugAddSTR("TIP:",szTemp4,12);

                //#00157 Debit transaction has tipping amount on receipt.
                if (TRUE ==strHDT.fTipAllowFlag && srTransRec.fDebit != TRUE
                && srTransRec.byOrgTransType != PRE_COMP
                && srTransRec.byOrgTransType != CASH_ADV)
                {
                    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                    if(srTransRec.byVoided == TRUE && (atol(szTemp4) <= 0)) /*if voided and tip is 0*/
						vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
					else
					{
                        vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
						vdCTOS_PrinterFline(1);

                        if(atol(szTemp4) > 0)
                        {
                            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                            vdPrintFormattedAmount("TIP   :", szTemp4, 24);
                            vdPrintFormattedAmount("TOTAL :", szTemp, 24);
                        }
                        else
                        {
							// Tip
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TIP:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
							vdCTOS_PrinterFline(1);

							// Total
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TOTAL:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
                        }
					}
                }
                else
                {
                    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                    vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
                }
            }

        }
        else if (srTransRec.byTransType == SALE_TIP)
        {
            memset(szTemp3, ' ', d_LINE_SIZE);
            wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
            vdPrintFormattedAmount("TIPS  :", szTemp3, 24);
            vdPrintFormattedAmount("TOTAL :", szTemp, 24);
        }
        else
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        }


		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);

		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
		    (srTransRec.bWaveSID == d_EMVCL_SID_DISCOVER_DPAS) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == 0x63) ||
			(srTransRec.bWaveSID == 0x65))
        {
            vdCTOS_PrinterFline(1); // space after printing trans title.

            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

            //AID
            memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
            memset(szTemp, ' ', d_LINE_SIZE);
            wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
            sprintf(szStr, "AID: %s",szTemp);
            memset(baTemp, ' ', d_LINE_SIZE);
            sprintf(baTemp, "APP: %s", srTransRec.stEMVinfo.szChipLabel);
            inPrintLeftRight(szStr, baTemp, 46);

            if((srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND  && srTransRec.byTransType != SALE_OFFLINE  && srTransRec.byOrgTransType != SALE_OFFLINE)
	   && (srTransRec.byUploaded==TRUE && srTransRec.byOffline==FALSE))
            {
                //AC
                memset(szStr, ' ', d_LINE_SIZE);
                wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
                sprintf(szStr, "TC: %s", szTemp);

                //TVR
                EMVtagLen = 5;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
                memset(baTemp, ' ', d_LINE_SIZE);
                sprintf(baTemp, "TVR: %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
                inPrintLeftRight(szStr, baTemp, 46);
            }
        }
	}
    else if(MERCHANT_COPY_RECEIPT == page || BANK_COPY_RECEIPT== page)
    {
        if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
            vdPrintCenter("***CONTACTLESS***");

        if (strcmp(srTransRec.szAuthCode,"Y1")==0)
         	{
         	       inPrint("SALE");
         	}
	else
          {
               // - print adjustment 07132015
               szGetTransTitle(srTransRec.byTransType, szStr);
               inPrint(szStr);
               // - print adjustment 07132015
          }
        printDateTime(TRUE);
        inResult = printTIDMID();

        printBatchInvoiceNO(); // pat confirm hang

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

        // print CARD LABEL AND CARD NUMBER
        memset(szStr, ' ', d_LINE_SIZE);
        //if(srTransRec.fDebit == TRUE && strcmp(srTransRec.szCardLable, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
        if(srTransRec.fDebit == TRUE)
			strcpy(szStr,"MCC DEBIT");
        else
            sprintf(szStr, "%s", srTransRec.szCardLable);

        memset (baTemp, 0x00, sizeof(baTemp));
        vdCTOS_FormatPANEx(strIIT.szMaskMerchantCopy, srTransRec.szPAN, baTemp, strIIT.fMaskMerchCopy);

        inPrintLeftRight(szStr, baTemp, 46);

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		printCardHolderNameEntryMode();

        //Reference num
        if(srTransRec.fDebit != TRUE)
        {
            memset(szStr, ' ', d_LINE_SIZE);
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szStr, ' ', d_LINE_SIZE);
            sprintf(szStr, "REF NO.   : %s", srTransRec.szRRN);
            inPrintLeftRight(szStr, " ", 46);
        }

        //Auth response code
        // #00158 No Account type printed on DEBIT receipt
        //if(srTransRec.fDebit == TRUE && strCDT.inType == DEBIT_CARD)
        if(srTransRec.fDebit == TRUE)
        {
            char szTemp[100+1];
            char szAccntType[40+1];
            memset(szTemp, ' ', sizeof(szTemp));
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szAccntType, ' ', sizeof(szAccntType));

            if(srTransRec.inAccountType == CURRENT_ACCOUNT)
                strcpy(szAccntType, "CURRENT");
            else if(srTransRec.inAccountType == SAVINGS_ACCOUNT)
                strcpy(szAccntType, "SAVINGS");

            sprintf(szTemp, "APPR. CODE: %s   ACCNT TYPE:  %s", srTransRec.szAuthCode, szAccntType);
            inPrint(szTemp);
        }
        else
        {
           vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            memset(szStr2, ' ', d_LINE_SIZE);
            sprintf(szStr2, "APPR. CODE: %s", srTransRec.szAuthCode);
            inPrintLeftRight(" ", szStr2, 24);
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        }

        //Terminal serial number - mfl
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);
	
		//test
		usGetSerialNumber(szTermSerialNum);
		vdDebug_LogPrintf("saturn print serial number %s", szTermSerialNum);
		
        szTermSerialNum[15]=0;
        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

		vdPrintFSRMode();

        vdCTOS_PrinterFline(1); // space after printing trans title.

		vdPrintECRMessage(); // Mercury Requirements -- sidumili

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);
        memset(szTemp1, ' ', d_LINE_SIZE);
        memset(szTemp3, ' ', d_LINE_SIZE);
        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
        wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        if ((srTransRec.byTransType == SALE || srTransRec.byTransType == VOID ||srTransRec.byTransType == SALE_OFFLINE)&&(srTransRec.byPackType != VOID_REFUND))
        {
            if(srTransRec.fInstallment || srTransRec.fCash2Go)
            {
                ushCTOS_PrintInstallmentDetails();
            }
            else
            {
                //Tip amount
                memset(szStr, ' ', d_LINE_SIZE);
                sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
                memset (baTemp, 0x00, sizeof(baTemp));
                memset (szTemp4, 0x00, sizeof(szTemp4));
                wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);

                DebugAddSTR("TIP:",szTemp4,12);

                //#00157 Debit transaction has tipping amount on receipt.
                if (TRUE ==strHDT.fTipAllowFlag && srTransRec.fDebit != TRUE
                && srTransRec.byOrgTransType != PRE_COMP
                && srTransRec.byOrgTransType != CASH_ADV)
                {
                    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                    if(srTransRec.byVoided == TRUE && (atol(szTemp4) <= 0)) /*if voided and tip is 0*/
						vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
					else
					{
                        vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
						vdCTOS_PrinterFline(1);

                        if(atol(szTemp4) > 0)
                        {
                            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                            vdPrintFormattedAmount("TIP   :", szTemp4, 24);
                            vdPrintFormattedAmount("TOTAL :", szTemp, 24);
                        }
                        else
                        {
							// Tip
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TIP:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
							vdCTOS_PrinterFline(1);

							// Total
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TOTAL:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
                        }
					}
                }
                else
                {
                    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                    vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
                }
            }

        }
        else if (srTransRec.byTransType == SALE_TIP)
        {
            memset(szTemp3, ' ', d_LINE_SIZE);
            wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
            vdPrintFormattedAmount("TIPS  :", szTemp3, 24);
            vdPrintFormattedAmount("TOTAL :", szTemp, 24);
        }
        else
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            vdPrintFormattedAmount("TOTAL :", szTemp, 24);
        }

// #00060 - start - Revision on Chip Receipt:
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);

        if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
        /* EMV: Revised EMV details printing - start -- jzg */
            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
		    (srTransRec.bWaveSID == d_EMVCL_SID_DISCOVER_DPAS) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == 0x63) ||
			(srTransRec.bWaveSID == 0x65))
        {
            vdCTOS_PrinterFline(1); // space after printing trans title.
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

            //AID
            memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
            memset(szTemp, ' ', d_LINE_SIZE);
            wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
            sprintf(szStr, "AID: %s",szTemp);


            memset(baTemp, ' ', d_LINE_SIZE);
            sprintf(baTemp, "APP: %s", srTransRec.stEMVinfo.szChipLabel);
            inPrintLeftRight(szStr, baTemp, 46);

          if((srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND  && srTransRec.byTransType != SALE_OFFLINE  && srTransRec.byOrgTransType != SALE_OFFLINE)
          && (srTransRec.byUploaded==TRUE && srTransRec.byOffline==FALSE)) 
            {
                //AC
                memset(szStr, ' ', d_LINE_SIZE);
                wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
                sprintf(szStr, "TC: %s", szTemp);

                //TVR
                EMVtagLen = 5;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
                memset(baTemp, ' ', d_LINE_SIZE);
                sprintf(baTemp, "TVR: %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
                inPrintLeftRight(szStr, baTemp, 46);
            }
        }
    }

    return d_OK;
}
USHORT ushCTOS_PrintAgreement()
{
    if(srTransRec.fDebit != TRUE)
    {
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        vdPrintCenter("I AGREE TO PAY THE ABOVE TOTAL AMOUNT");
        vdPrintCenter("ACCORDING TO THE CARD ISSUER AGREEMENT");
		vdCTOS_PrinterFline(1);
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
}

USHORT ushCTOS_PrintFooter(int page)
{
     BYTE   EMVtagVal[64];
     USHORT EMVtagLen;
     
	int fNSRflag = inNSRFlag();
	int iFSR = get_env_int("FSRMODE");

	vdDebug_LogPrintf("--ushCTOS_PrintFooter--");
	vdDebug_LogPrintf("byTransType[%d],page[%d]",srTransRec.byTransType, page);
	vdDebug_LogPrintf("byEntryMode[%d]",srTransRec.byEntryMode);
	vdDebug_LogPrintf("CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
	vdDebug_LogPrintf("srTransRec.byEntryMode = [%d] | bWaveSCVMAnalysis = [%d] = bWaveSID = [%d]", srTransRec.byEntryMode, srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSID);
	vdDebug_LogPrintf("fCUPPINEntry[%d]", srTransRec.fCUPPINEntry);
	vdDebug_LogPrintf("fEMVPIN[%d]", srTransRec.fEMVPIN);
	vdDebug_LogPrintf("fEMVPINEntered[%d]", srTransRec.fEMVPINEntered);
	vdDebug_LogPrintf("fDCCOptin[%d]", srTransRec.fDCCOptin);
	vdDebug_LogPrintf("fInstallment[%d]", srTransRec.fInstallment);
	vdDebug_LogPrintf("fCash2Go[%d]", srTransRec.fCash2Go);
	vdDebug_LogPrintf("fDebit[%d]", srTransRec.fDebit);
	vdDebug_LogPrintf("fNSRflag[%d],iFSR[%d]", fNSRflag, iFSR);
	vdDebug_LogPrintf("byOffline[%d]", srTransRec.byOffline);
	vdDebug_LogPrintf("fEMVPINBYPASS[%d]", srTransRec.fEMVPINBYPASS);
	
     vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
     if(page == CUSTOMER_COPY_RECEIPT)
     {
     
          vdCTOS_PrinterFline(1);
          
          /*if ((strTCT.fDCCTerminal==TRUE) && (strCDT.fDCCEnable==TRUE) && (gblDCCReceipt==TRUE))
          ushCTOS_DCCPrintAgreement();
          else 
          ushCTOS_PrintAgreement();*/
          
          //if (gblDCCReceipt==FALSE)
          if(srTransRec.fDCCOptin==FALSE)
               ushCTOS_PrintAgreement();
          
          vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
          if(strlen(strMMT[0].szRctFoot1) > 0)
               vdPrintCenter(strMMT[0].szRctFoot1);
          if(strlen(strMMT[0].szRctFoot2) > 0)
               vdPrintCenter(strMMT[0].szRctFoot2);
          if(strlen(strMMT[0].szRctFoot3) > 0)
               vdPrintCenter(strMMT[0].szRctFoot3);
          
          if(strHDT.fFooterLogo == TRUE)
          {
               if(strlen(strHDT.szFooterLogoName))
                    vdCTOSS_PrinterBMPPic(0, 0, strHDT.szFooterLogoName);
               else
                    vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
          }
          
          vdCTOS_PrinterFline(1);
          vdPrintCenter("***** CUSTOMER COPY *****");
          vdCTOS_PrinterFline(2);
     }
     else if(page == MERCHANT_COPY_RECEIPT || page == BANK_COPY_RECEIPT)
     {
          memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
          EMVtagLen = 0;
          
          //DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);
          //vdDebug_LogPrintf("MERCH:CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
          //vdDebug_LogPrintf("MERCH:srTransRec.byEntryMode = [%d] | bWaveSCVMAnalysis = [%d] = bWaveSID = [%d]", srTransRec.byEntryMode, srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSID);
	      //vdDebug_LogPrintf("MERCH:fCUPPINEntry[%d]", srTransRec.fCUPPINEntry);
		  //vdDebug_LogPrintf("MERCH:fEMVPIN[%d]", srTransRec.fEMVPIN);
		  //vdDebug_LogPrintf("MERCH:fEMVPINEntered[%d]", srTransRec.fEMVPINEntered);
		  //vdDebug_LogPrintf("MERCH:fDCCOptin[%d]", srTransRec.fDCCOptin);
	
          EMVtagLen = 3;
          memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
          
          vdDebug_LogPrintf("EMVtagVal [%02X %02X %02X]",EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
		  vdDebug_LogPrintf("srTransRec.bWaveSCVMAnalysis=[%d]", srTransRec.bWaveSCVMAnalysis);
          
          if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
          {	
          	   vdDebug_LogPrintf("Entry Mode: CARD_ENTRY_ICC");
               if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
               (EMVtagVal[0] != 0x1E) && (EMVtagVal[0] != 0x41) && (EMVtagVal[0] != 0x44) &&
               (EMVtagVal[0] != 0x5E)) && (EMVtagVal[0] == 0x3F)&& (EMVtagVal[0] != 0x02))
               {
					vdDebug_LogPrintf("ICC IF Condition...");
                    if((srTransRec.byTransType == REFUND || srTransRec.byOrgTransType == REFUND) ||
                    (srTransRec.byTransType == SALE_OFFLINE || srTransRec.byOrgTransType == SALE_OFFLINE) ||
                    (srTransRec.byTransType == PRE_AUTH || srTransRec.byOrgTransType == PRE_AUTH) ||
                    (srTransRec.byTransType == PREAUTH_COMP || srTransRec.byOrgTransType == PREAUTH_COMP)||
                    (srTransRec.byTransType == PREAUTH_VOID|| srTransRec.byOrgTransType == PREAUTH_VOID) ||
                    (srTransRec.byTransType == PREAUTH_VER))
                    {
                         vdCTOS_PrinterFline(1);
                         
                         if ((srTransRec.fEMVPIN == TRUE || srTransRec.fEMVPINEntered == TRUE) && (srTransRec.fEMVPINBYPASS == FALSE))
                         {
                              vdCTOS_PrinterFline(1);
                              vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                         }
                         else
                         {
                              if(ushCTOS_ePadPrintSignature() != d_OK)
                                   vdCTOS_PrinterFline(1);
							  
                              inPrint("SIGN:_______________________________________");
                         }

                         if((EMVtagVal[0] & 0x0F) == 0x01 || (EMVtagVal[0] & 0x0F) == 0x02 || (EMVtagVal[0] & 0x0F) == 0x04 || srTransRec.bWaveSCVMAnalysis == 2)
                         {
                              if (srTransRec.fEMVPINBYPASS == TRUE || srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE)
                              {
                              // do nothing...
                              }
                              else
                              {
                                   if (srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered== TRUE && (srTransRec.fEMVPINBYPASS == FALSE))
                                   {
                                       if (srTransRec.byTransType!=SALE_TIP)
                                       {
                                             if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                             {
                                             }
                                             else
                                             {
                                                  vdCTOS_PrinterFline(1);
                                                  vdPrintCenter("(PIN VERIFY SUCCESS)");	
                                             }
                                        }
                                   }
                              }							
                         }
                    }
                    else
                   {
                         if ((fNSRflag == 1 || inCTOSS_CheckCVMAmount()==d_OK) && (srTransRec.fEMVPINBYPASS == FALSE))
                         {
                              vdCTOS_PrinterFline(1);
                              vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                         }
                         else //if (EMVtagVal[0] != 0x41) && srTransRec.fEMVPINEntered == FALSE)
                         {				  
                              if(ushCTOS_ePadPrintSignature() != d_OK)
                                   vdCTOS_PrinterFline(1);
							  
                              inPrint("SIGN:_______________________________________");				  						  
                         }
                         
                         if((EMVtagVal[0] & 0x0F) == 0x01 || (EMVtagVal[0] & 0x0F) == 0x02 || (EMVtagVal[0] & 0x0F) == 0x04 || (EMVtagVal[0] & 0x0F) == 0x44 || srTransRec.bWaveSCVMAnalysis == 2)
                         {
                              if (srTransRec.fEMVPINBYPASS == TRUE || srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE  || inCTOSS_CheckCVMAmount()==d_OK)
                              {
                              // do nothing...
                              }
                              else
                              {
                                   if (srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered== TRUE && (srTransRec.fEMVPINBYPASS == FALSE))
                                   {
                                        if (srTransRec.byTransType!=SALE_TIP)
                                        {
                                             if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                             {
                                             }
                                             else
                                             {
                                                  vdCTOS_PrinterFline(1);
                                                  vdPrintCenter("(PIN VERIFY SUCCESS)");	
                                             }
                                        }
                                   }
                              }		                            
                         }                                        
                   }
               }
               else //sale and other transaction
               {
               		vdDebug_LogPrintf("ICC ELSE Condition...");
                    if(srTransRec.fDebit != TRUE)
                    {
                         //vdDebug_LogPrintf("AAA 2- fNSRflag[%d], srTransRec.fDCCOptin[%d], EMVtagVal[%02X]", fNSRflag, srTransRec.fDCCOptin, EMVtagVal[0]);
                         vdCTOS_PrinterFline(1);
                         if ((fNSRflag == 1) || (inCTOSS_CheckCVMAmount()==d_OK && srTransRec.fEMVPINBYPASS == FALSE))
                         {
                         	vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                         }                              
                         else //if (EMVtagVal[0] != 0x41)
                         {
                            if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                            
                            inPrint("SIGN:_______________________________________");
                                   
							  					  
                         }
                         if (srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE  || inCTOSS_CheckCVMAmount()==d_OK)
                         {
                              // do nothing
                         }
                         else
                         {
                              if ((EMVtagVal[0] == 0x41 || EMVtagVal[0] == 0x44 || srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE) && (srTransRec.fEMVPINBYPASS == FALSE)) 
                              {
                                   if (srTransRec.byTransType!=SALE_TIP)
                                   {
                                        if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                        {
                                        }
                                        else
                                        {
                                             vdCTOS_PrinterFline(1);
                                             vdPrintCenter("(PIN VERIFY SUCCESS)");	
                                        }
                                   }
                              }
                         }		     			 
                    }
               }
          }
          else if( srTransRec.byEntryMode == CARD_ENTRY_WAVE)
          {
          	   vdDebug_LogPrintf("Entry Mode: CARD_ENTRY_WAVE");
               if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
			   {
			   		vdDebug_LogPrintf("CTLS IF Condition...");
					if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
					{
						vdCTOS_PrinterFline(1);
						if ((fNSRflag == 1) || (inCTOSS_CheckCVMAmount()==d_OK && srTransRec.fEMVPINBYPASS == FALSE))
							vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
						else
                                                        {
                                                             if(ushCTOS_ePadPrintSignature() != d_OK)
                                                                  vdCTOS_PrinterFline(1);
                                                             
                                                             inPrint("SIGN:_______________________________________");
                                                        }
					}
					else
					{
							if(srTransRec.fDCCOptin== TRUE)
							{
								if (srTransRec.fEMVPINBYPASS == TRUE)
								{
									vdCTOS_PrinterFline(1);
									inPrint("SIGN:_______________________________________");
								}
								else
								{
									if (srTransRec.fEMVPIN == TRUE)
									{
										vdCTOS_PrinterFline(1);
										vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
									}
									else
									{
										vdCTOS_PrinterFline(1);
										inPrint("SIGN:_______________________________________");
									}
								}
								
							}
							else
							{
								vdCTOS_PrinterFline(1);
								if (inCTOSS_CheckCVMAmount()==d_OK ||(fNSRflag == 1 && srTransRec.fDCCOptin== FALSE))
									vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
								else
								{

										vdCTOS_PrinterFline(1);
										if (((srTransRec.fEMVPIN == TRUE) && (EMVtagVal[0] == 0x01 || EMVtagVal[1] == 0x02 || srTransRec.bWaveSCVMAnalysis == 2)) && (srTransRec.byTransType!=SALE_TIP))
                                            {
                                                 if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                                 {
                                                      if(ushCTOS_ePadPrintSignature() != d_OK)
                                                           vdCTOS_PrinterFline(1);
                                                      
                                                      inPrint("SIGN:_______________________________________");
                                                 }
                                                 else
                                                      vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                                            }
										else
											{
                                                                                                             if(ushCTOS_ePadPrintSignature() != d_OK)
                                                                                                                  vdCTOS_PrinterFline(1);
																											 
                                                                                                             inPrint("SIGN:_______________________________________");
											}
																		
								}
							}

						//if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
						if(EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || srTransRec.bWaveSCVMAnalysis == 2)
						{
							if (srTransRec.fEMVPINBYPASS == TRUE || srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE || inCTOSS_CheckCVMAmount()==d_OK)
							{
								// do nothing...
							}
							else
							{
								if (srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered== TRUE && (srTransRec.fEMVPINBYPASS == FALSE))
							    {
							        if (srTransRec.byTransType!=SALE_TIP)
                                   {
                                        if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                        {
                                        }
                                        else
                                        {
                                             vdCTOS_PrinterFline(1);
                                             vdPrintCenter("(PIN VERIFY SUCCESS)");	
                                        }
                                   }
							    }
							}
						}
					}
			   }
               else
               {
               		vdDebug_LogPrintf("CTLS ELSE Condition...");
                    if(srTransRec.fDebit != TRUE)
                    {
                         vdCTOS_PrinterFline(1);
                         if ((fNSRflag == 1) || (inCTOSS_CheckCVMAmount()==d_OK))
                              vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                         else
                         {
                              if(ushCTOS_ePadPrintSignature() != d_OK)
                                   vdCTOS_PrinterFline(1);
                              
                              inPrint("SIGN:_______________________________________");
                         }
						 
                    }

                    if (srTransRec.fEMVPINBYPASS == TRUE || srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE || inCTOSS_CheckCVMAmount()==d_OK)
                    {
                           // do nothing...
                    }
                    else
                    {
                         if (srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered== TRUE && (srTransRec.fEMVPINBYPASS == FALSE))
                         {
                             if (srTransRec.byTransType!=SALE_TIP)
                                   {
                                        if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                        {
                                        }
                                        else
                                        {
                                             vdCTOS_PrinterFline(1);
                                             vdPrintCenter("(PIN VERIFY SUCCESS)");	
                                        }
                                   }
                         }
                    }
               }
          }
          else
          {
          	   vdDebug_LogPrintf("Entry Mode: MSR/FALLBACK");
               if(srTransRec.fDebit != TRUE)
               {
                        vdCTOS_PrinterFline(1);
                        if ((fNSRflag == 1) || (inCTOSS_CheckCVMAmount()==d_OK && srTransRec.byEntryMode==CARD_ENTRY_FALLBACK && srTransRec.IITid==AMEX_ISSUER) || (inCTOSS_CheckCVMAmount()==d_OK && srTransRec.byEntryMode==CARD_ENTRY_MSR))
                        {
                             vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                        }                         
                        else
                        {
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                             vdCTOS_PrinterFline(1);	
                        
                        inPrint("SIGN:_______________________________________");
                        
                        }
                       if (srTransRec.fEMVPINBYPASS == TRUE || srTransRec.fInstallment == TRUE || srTransRec.fCash2Go == TRUE || srTransRec.fDebit == TRUE || inCTOSS_CheckCVMAmount()==d_OK && (srTransRec.fCUPPINEntry == FALSE))
                       {
                            // do nothing...
                       }
                       else
                       {
                            if (srTransRec.fEMVPIN == TRUE || srTransRec.fCUPPINEntry == TRUE || srTransRec.fEMVPINEntered== TRUE && (srTransRec.fEMVPINBYPASS == FALSE))
                            {
                                 if (srTransRec.byTransType!=SALE_TIP)
                                   {
                                        if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType==SALE_TIP)
                                        {
                                        }
                                        else
                                        {
                                             vdCTOS_PrinterFline(1);
                                             vdPrintCenter("(PIN VERIFY SUCCESS)");	
                                        }
                                   }
                            }
                       }
               }
          }
          
          vdCTOS_PrinterFline(1);
          
          /*if ((strTCT.fDCCTerminal==TRUE) && (strCDT.fDCCEnable==TRUE))
          ushCTOS_DCCPrintAgreement();
          else 
          ushCTOS_PrintAgreement();*/
          
          //if (gblDCCReceipt==FALSE)
          if (srTransRec.fDCCOptin==FALSE)
               ushCTOS_PrintAgreement();
          
          vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
          if(strlen(strMMT[0].szRctFoot1) > 0)
               vdPrintCenter(strMMT[0].szRctFoot1);
          if(strlen(strMMT[0].szRctFoot2) > 0)
               vdPrintCenter(strMMT[0].szRctFoot2);
          if(strlen(strMMT[0].szRctFoot3) > 0)
               vdPrintCenter(strMMT[0].szRctFoot3);
          
          if(strHDT.fFooterLogo == TRUE)
          {
               if(strlen(strHDT.szFooterLogoName))
                    vdCTOSS_PrinterBMPPic(0, 0, strHDT.szFooterLogoName);
               else
                    vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
          }
          vdCTOS_PrinterFline(1);

		  if (page == MERCHANT_COPY_RECEIPT )
	          vdPrintCenter("***** MERCHANT COPY *****");

		  if (page == BANK_COPY_RECEIPT)
			  	vdPrintCenter("***** BANK COPY *****");
		  
          vdCTOS_PrinterFline(2);
     }
	 
     vdLineFeed((strHDT.fFooterLogo == TRUE)?TRUE:FALSE); // added -- sidumili
     
     return d_OK;

}



USHORT ushCTOS_printAll(int page) {
    int inResult, print_counter = 0;
    int inKey = d_NO;
    BYTE szLogoPath[256 + 1];
    BYTE szMsg[256];
    BYTE szPrintMsg[512];
	BYTE szTitle[25+1];
	BYTE szDisMsg[100];
    TRANS_DATA_TABLE srTransPara;
	BYTE szCopy[256];

    vdDebug_LogPrintf("saturn --ushCTOS_printAll--");

    if (printCheckPaper() == -1)
        return -1;

	//tine/sidumili: android - set ui thread to display nothing
	if (fRePrintFlag!=TRUE)
	     DisplayStatusLine(" ");
	
    //vdSetPrintThreadStatus(1);

    vdDebug_LogPrintf("saturn ushCTOS_printAll-1");

#if 0 /*albert (ecr tct update fail) - save last invoice number, moved to vdSaveLastInvoiceNo*/
    if(VS_FALSE == fRePrintFlag)
        memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);

    if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
        vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
#endif

    inCTOS_SelectFont(d_FONT_FNT_MODE, d_FONT_24x24, 0, " ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    //if (isCheckTerminalMP200() != d_OK)
    //vdCTOSS_PrinterStart(500);

	vdCTOSS_PrinterStart(200);
    CTOS_PrinterSetHeatLevel(4);
    //vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

    vdDebug_LogPrintf("saturn ushCTOS_printAll-2");

    ushCTOS_PrintHeader(page);

    vdDebug_LogPrintf("saturn ushCTOS_printAll-3");

    if (srTransRec.fLoyalty == TRUE && srTransRec.byTransType == LOY_BAL_INQ ||
        srTransRec.byTransType == LOY_REDEEM_5050 ||
        srTransRec.byTransType == LOY_REDEEM_VARIABLE) {
        ushCTOS_PrintLoyaltyBody(page);
        ushCTOS_PrintLoyaltyFooter(page);
    }// fix for wrong void redeem printout
    else if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE ||
             srTransRec.byOrgTransType == LOY_REDEEM_5050) {
        ushCTOS_PrintLoyaltyBody(page);
        ushCTOS_PrintLoyaltyFooter(page);
    }
        // else if (gblDCCTrans==TRUE)
        // else if ((gblDCCReceipt==TRUE) && (gblDCCTrans==TRUE) || (srTransRec.fDCCOptin == TRUE))// && (srTransRec.byTransType == REPRINT_ANY))
    else if (srTransRec.fDCCOptin == TRUE) {
        ushCTOS_DCCPrintBody(page);
        //ushCTOS_DCCPrintFooter(page);
        ushCTOS_PrintFooter(page);
    } else {
        ushCTOS_PrintBody(page);
        ushCTOS_PrintFooter(page);
    }

	memset(szLogoPath, 0x00, sizeof(szLogoPath));
	sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
	
	if(strTCT.feReceiptEnable == 0)
	{		
		vdCTOSS_PrinterEnd();
		vdDebug_LogPrintf("PRINTING...........");
		//CTOS_Delay(5000);		//TINE: 04OCT2019 - temporary, for debugging only
    	//vdSetPrintThreadStatus(0);
		return d_OK;
	}
    //vdCTOSS_PrinterEnd();
    //vdSetPrintThreadStatus(0);

    //if (isCheckTerminalMP200() != d_OK)
    //    CTOS_LCDTClearDisplay();

    //TINE:  19AUG2019 modified for android terminal to display receipt on screen
    
    memset(szMsg, 0x00, sizeof(szMsg));
	memset(szCopy, 0x00, sizeof(szCopy));
    memset(szPrintMsg, 0x00, sizeof(szPrintMsg));

    if (page == BANK_COPY_RECEIPT)
    	{
    		strcpy(szMsg, "Print Bank Copy?");
			strcpy(szCopy, "Bank Copy");
    	}        
    else if (page == CUSTOMER_COPY_RECEIPT)
    	{
    		strcpy(szMsg, "Print Customer Copy?");
			strcpy(szCopy, "Customer Copy");
    	}        
    else if (page == MERCHANT_COPY_RECEIPT)
    	{	
    		strcpy(szMsg, "Print Merchant Copy?");
			strcpy(szCopy, "Merchant Copy");
    	}
    
        //sprintf(szLogoPath,"%s%s",LOCAL_PATH,strHDT.szHeaderLogoName);
        //vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);

#if 0
        if(print_counter == 1)
        {
            if( printCheckPaper()==-1)
                return -1;

            vdDebug_LogPrintf("print_counter = [%d]", print_counter);
            //vdPrintReportDisplayBMP();
            vdCTOSS_PrinterEnd();
            print_counter = 0;
            return d_OK;
        }
#endif

		vdDebug_LogPrintf("Capturing erm receipt...");
        vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
        CTOS_KBDBufFlush();
        inCTOSS_ERM_Form_Receipt(0);

	if (strTCT.feReceiptEnable == 1) {

        strcpy(szPrintMsg, szLogoPath);
        strcat(szPrintMsg, "|");
        strcat(szPrintMsg, szMsg);

        if (strTCT.byTerminalModel != 0 ) {
            //vdCTOSS_PrinterStart(200);
            //ushCTOS_printAll(BANK_COPY_RECEIPT);
            
            if (printcopies_cntr == 0) {
                //inKey = PrintFirstReceiptUI(szLogoPath);
                memset(szTitle, 0x00, sizeof(szTitle));
                                    if (fRePrintFlag != TRUE)
                                    {
                                         szGetTransTitle(srTransRec.byTransType, szTitle);
                                         strcpy(szDisMsg, szTitle);
                                    }
                                    else
                                         strcpy(szDisMsg, "REPRINT");

				strcat(szDisMsg, "|");
				strcat(szDisMsg, "PRINTING...");
				//usCTOSS_LCDDisplay(szDisMsg);		//tine:  fix on delay on display while printing
				//vdDisplayMessageBox(1, 8, "PRINTING", szCopy, MSG_PLS_WAIT, MSG_TYPE_PRINT);
	
                vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
                vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
				usCTOSS_LCDDisplay(szDisMsg);		//tine:  fix on delay on display while printing
                vdCTOSS_PrinterEnd();
                return d_OK;
            }
	else {
            
            	inKey = PrintReceiptUI(szPrintMsg);
				
				if (inKey == d_OK || inKey == 0xFF) {
	                vdDebug_LogPrintf("vdCTOSS_PrinterEnd() - START");
					memset(szTitle, 0x00, sizeof(szTitle));
                                             if (fRePrintFlag != TRUE)
                                             {
                                                  szGetTransTitle(srTransRec.byTransType, szTitle);
                                                  strcpy(szDisMsg, szTitle);
                                             }
                                             else
                                                  strcpy(szDisMsg, "REPRINT");
											 
					#if 1
						strcat(szDisMsg, "|");
						strcat(szDisMsg, "PRINTING...");
						//usCTOSS_LCDDisplay(szDisMsg);
						//vdDisplayMessageBox(1, 8, "PRINTING", szCopy, MSG_PLS_WAIT, MSG_TYPE_PRINT);
					#else
						//animate receipt printing - on-going development
						vdDebug_LogPrintf("animate printing...");
						AnimatePrint();
					#endif
	                vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
	                vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
					usCTOSS_LCDDisplay(szDisMsg);		//tine:  fix on delay on display while printing
	                vdCTOSS_PrinterEnd();
	                return d_OK;
	                //print_counter = 1;
	                //goto INPRINTRECEIPT;
	            }
	            if (inKey == d_USER_CANCEL)
	            {
					return d_OK;		
	            }
	                		
            }            
                          
        } else {
            vdDebug_LogPrintf("strTCT.byTerminalModel = [%d]", strTCT.byTerminalModel);
            inKey = EliteReceiptUI(szLogoPath);
        }
    } else {
        //sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);  
                  memset(szTitle, 0x00, sizeof(szTitle));
                  if (fRePrintFlag != TRUE)
                  {
                       szGetTransTitle(srTransRec.byTransType, szTitle);
                       strcpy(szDisMsg, szTitle);
                  }
                  else
                       strcpy(szDisMsg, "REPRINT");
				  
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PRINTING...");
		usCTOSS_LCDDisplay(szDisMsg);
		//vdDisplayMessageBox(1, 8, "PRINTING", szCopy, MSG_PLS_WAIT, MSG_TYPE_PRINT);
					
        vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
		vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");  //TINE:  temporarily remove for debugging purposes on on-screen-signing functionality
        vdCTOSS_PrinterEnd();
        //vdSetPrintThreadStatus(0);

        //if (isCheckTerminalMP200() != d_OK)
        //    CTOS_LCDTClearDisplay();

        return d_OK;

    }

    return inKey;
}

int inCTOS_rePrintReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];
	BOOL   needSecond = TRUE;
	BYTE   key;
	BOOL fERM = strTCT.fSignatureFlag;

	vdDebug_LogPrintf("--inCTOS_rePrintReceipt--");

//    	CTOS_PrinterPutString("inCTOS_rePrintReceipt");

	//for MP200 no need print
	if (isCheckTerminalMP200() == d_OK)
		return (d_OK);

	CTOS_PrinterSetHeatLevel(4);
	if( printCheckPaper()==-1)
		return d_NO;

	if(get_env_int("#ORIENTATIONTRANS")!=1)
         {
              inResult = inCTOS_GeneralGetInvoice();
              if(d_OK != inResult)
              {
                   return inResult;
              }
         }


	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_MultiAPBatchSearch(d_IPC_CMD_REPRINT_ANY);
		if(d_OK != inResult)
			return inResult;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inResult = inCTOS_MultiAPGetVoid();
			if(d_OK != inResult)
				return inResult;
		}
		inResult = inCTOS_BatchSearch();
		if(d_OK != inResult)
			return inResult;
	}

	if(d_OK != inResult)
	{
		return inResult;
	}

    inIITRead(srTransRec.IITid);
    inCDTRead(srTransRec.CDTid);
    inHDTRead(srTransRec.HDTid);////fix for No TIP and total amount printed on duplicate receipt issue. 2.#00021 - Tip amount is not included in the receipt.

	inTCTRead(1);

	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);

	if(get_env_int("#ORIENTATIONTRANS")!=1)
		vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy, fERM);
	     //vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy, FALSE);
	else
		vdCTOS_PrintReceiptCopies(TRUE, FALSE, FALSE, fERM);
	     //vdCTOS_PrintReceiptCopies(TRUE, FALSE, FALSE, FALSE);

	return d_OK;
}

void vdCTOS_PrintSummaryReport()
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i, j;
         char szStr[d_LINE_SIZE + 1];
         BYTE baTemp[PAPER_X_SIZE * 64];
         int inDCCTransactions = 0;

	BYTE szLogoPath[256 + 1];

	BYTE szMsg[256];
	BYTE szPrintMsg[512];

//    	CTOS_PrinterPutString("vdCTOS_PrintSummaryReport");
	int inIITCount=0;
         int inCSTCount=0;

	vdDebug_LogPrintf("--vdCTOS_PrintSummaryReport--");
	
	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(1000);

	// Commented to support print confirmation -- sidumili
	#if 0
	strcpy(szDisMsg, "SUMMARY  REPORT");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PRINTING...");
	usCTOSS_LCDDisplay(szDisMsg);
	#endif
	
	strHDT.inHostIndex = shHostIndex;

	DebugAddINT("summary host Index",shHostIndex);
	inCSTRead(strHDT.inCurrencyIdx);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SUMMARY_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}


    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;
    }

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


    vdCTOSS_PrinterStart(1000);
	CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();

	ushCTOS_PrintHeader(0);
	    
    //memset(szLogoPath, 0x00, sizeof(szLogoPath));
    memset(szMsg, 0x00, sizeof(szMsg));
    memset(szPrintMsg, 0x00, sizeof(szPrintMsg));
	
    //sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
	//vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);

	//ushCTOS_PrintHeaderAddress();

    vdPrintTitleCenter("SUMMARY REPORT");
	vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    printHostLabel();

	printTIDMID();

	vdCTOS_SetDateTime();

	printDateTime(FALSE);

	printBatchNO();

    vdCTOS_PrinterFline(1);
	inIITCount=inIITNumRecord()+1;
	for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
	{
		BOOL fPrintTotal=FALSE;
		inReportType = PRINT_CARD_TOTAL;

		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <inIITCount; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);

				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRedeemCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRegularCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usReduceCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usZeroCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBNPLCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].us2GOCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCompCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidBNPLCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoid2GOCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRegularCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidReduceCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidZeroCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidCashCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidCompCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRefundCount== 0))
					continue;

				vdDebug_LogPrintf("Count[%d]", i);
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				//memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);
				 //if(srTransRec.fDebit == TRUE && strcmp(strIIT.szIssuerLabel, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
				 if(srTransRec.fDebit == TRUE)
				 		strcpy(szStr,"MCC DEBIT");
				 else
					strcpy(szStr,strIIT.szIssuerLabel);

				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
				inPrint(szStr);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				fPrintTotal=TRUE;
			}
			//after print issuer total, then print host toal
			if(fPrintTotal == TRUE)
			{
				memset(szStr, ' ', d_LINE_SIZE);
				//memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"GRAND TOTAL");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
                                     inPrint(szStr);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
				
			}
			
		}
		else
		{

			memset(szStr, ' ', d_LINE_SIZE);
			//memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
			inPrint(szStr);
            vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
		}
	}

	if (strTCT.fDCCTerminal==TRUE)
         {
         
              //memset(szStr, 0x00, d_LINE_SIZE);
              //strcpy(szStr,"TOTALS BY CURRENCY:");
              //inPrint(szStr);
              inCSTCount=inCSTNumRecord()+1;
              
              for(j = 0; j < inCSTCount;j ++)
              {
                   if((srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount == 0)
		&&(srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount == 0)
                   && (srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount == 0))
                   continue;

		 inDCCTransactions++;

	        if(inDCCTransactions == 1)
                  {
                       memset(szStr, 0x00, d_LINE_SIZE);
                       strcpy(szStr,"TOTALS BY CURRENCY:");
                       inPrint(szStr);
                  }
                   
                   inCSTRead(j);
                   vdDebug_LogPrintf("Count[%d]", j);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usCompDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usVoidSaleCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount);
                   vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usRefundDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].ulTipTotalDCCAmount[%ul]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount);		
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usOffSaleDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount);
                   memset(szStr, 0x00, d_LINE_SIZE);
                   strcpy(szStr, strCST.szCurSymbol);
                   inPrint(szStr);
                   
                   vdCTOS_PrintAccumeByDCCCurrency(inReportType, srAccumRec.stBankTotal[0].stDCCTotal[j]);
              }
             
         }

	 if (srTransRec.byTransType !=SETTLE)
     {
          vdPrintTitleCenter("END OF REPORT");
          vdCTOS_PrinterFline(2);
     }

	vdLineFeed(FALSE);

	//vdCTOSS_PrinterEnd(); // Commented to support print confirmation -- sidumili

	// Capture report -- sidumili
	vdDebug_LogPrintf("Capturing erm report...");
	vdDeleteBMP();
	vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
	CTOS_KBDBufFlush();
	inCTOSS_ERM_Form_Receipt(0);

	/* tine:  no screen display for summary report due to indeterminate report length
	if (strGBLVar.fGBLvConfirmPrint)
	{
		// Print confirmation -- sidumili
		if (inPrintConfirmation("SUMMARY REPORT", FALSE) == d_OK)
		{
			vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
			vdCTOSS_PrinterEnd();
		}
		else
		{			
			vdCTOSS_PrinterEnd();
		}
	}
	else  */
	{
		//usCTOSS_LCDDisplay("SUMMARY REPORT|PRINTING...");
		vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
		vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
		vdCTOSS_PrinterEnd();
	}

	//return;
}

#if 1
void vdCTOS_PrintDetailReport()
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;
	ACCUM_REC srAccumRec;
	char szStr[d_LINE_SIZE + 1];
         BYTE baTemp[PAPER_X_SIZE * 64];
	int j, inCSTCount=0;
	int inDCCTransactions = 0;

//    CTOS_PrinterPutString("vdCTOS_PrintDetailReport");

	vdDebug_LogPrintf("--vdCTOS_PrintDetailReport--");
	 
	if( printCheckPaper()==-1)
		return;


	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
         CTOS_Delay(1000);
		 
	//vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);

		 

	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("detail host Index",shHostIndex);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DETAIL_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

	vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("detail MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

    inResult = inCTOS_ChkBatchEmpty();
    if(d_OK != inResult)
        return;

	inBatchNumOfRecord = inBatchNumRecord();

	DebugAddINT("batch record",inBatchNumOfRecord);
	if(inBatchNumOfRecord <= 0)
	{
		vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
		return;
	}

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
    //vdCTOSS_PrinterStart(2000);
    CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();
	ushCTOS_printBatchRecordHeader();

	if (!strGBLVar.fGBLvConfirmPrint)
		vdCTOSS_PrinterEnd();

	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;
	inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
	vdDebug_LogPrintf("inBatchByMerchandHost::inBatchNumOfRecord=[%d]", inBatchNumOfRecord);
	for(i=0; i<inBatchNumOfRecord; i++)
	{
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;

		vdDebug_LogPrintf("------------------------------------------------------------------------");
		vdDebug_LogPrintf("inDatabase_BatchReadByTransId::byTransType=[%d]", srTransRec.byTransType);
		vdDebug_LogPrintf("inDatabase_BatchReadByTransId::fDCCOptin=[%d]", srTransRec.fDCCOptin);
		DebugAddHEX("inDatabase_BatchReadByTransId::szInvoiceNo", srTransRec.szInvoiceNo, 3);
		DebugAddHEX("inDatabase_BatchReadByTransId::szBatchNo", srTransRec.szBatchNo, 3);
		vdDebug_LogPrintf("------------------------------------------------------------------------");
		
		if(/*srTransRec.byTransType != PREAUTH_VER && */srTransRec.byTransType != PREAUTH_VOID)
		{
						if (!strGBLVar.fGBLvConfirmPrint)
						{
							vdCTOSS_PrinterStart(100);
                        	CTOS_PrinterSetHeatLevel(4);
						}
                        
                        ushCTOS_printBatchRecordFooter();
						
		      if (srTransRec.fDCCOptin == TRUE)
		      {
		      	ushCTOS_printBatchDCCRecordFooter(); // Print Foreign Currency
		      }
                             
              printDividingLine(DIVIDING_LINE);

			  if (!strGBLVar.fGBLvConfirmPrint)
              	vdCTOSS_PrinterEnd();
		}
	}

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;
    }

	if (!strGBLVar.fGBLvConfirmPrint)
	{
		vdCTOSS_PrinterStart(100);
		CTOS_PrinterSetHeatLevel(4);
	}
		
    vdCTOS_PrinterFline(1);

    for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
    {
        if((srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount== 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidCashCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidCompCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidBNPLCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoid2GOCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidRefundCount == 0))
            continue;

        memset(szStr, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        strcpy(szStr,"GRAND TOTAL");
        inPrint(szStr);
        inReportType = PRINT_CARD_TOTAL;
        vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
    }

   if (strTCT.fDCCTerminal==TRUE)
         {
         
             // memset(szStr, 0x00, d_LINE_SIZE);
              //strcpy(szStr,"TOTALS BY CURRENCY:");
             // inPrint(szStr);
              inCSTCount=inCSTNumRecord()+1;
              
              for(j = 0; j < inCSTCount;j ++)
              {
                   if((srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount == 0)
	         &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount == 0)
                   && (srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount == 0))
                   continue;

		 inDCCTransactions++;

		 if(inDCCTransactions == 1)
                   {
                        memset(szStr, 0x00, d_LINE_SIZE);
                        strcpy(szStr,"TOTALS BY CURRENCY:");
                        inPrint(szStr);
                   }
                   
                   inCSTRead(j);
                   vdDebug_LogPrintf("Count[%d]", j);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usCompDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usVoidSaleCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount);
                   vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usRefundDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].ulTipTotalDCCAmount[%ul]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount);		
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usOffSaleDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount);
                   memset(szStr, 0x00, d_LINE_SIZE);
                   strcpy(szStr, strCST.szCurSymbol);
                   inPrint(szStr);
                   
                   vdCTOS_PrintAccumeByDCCCurrency(inReportType, srAccumRec.stBankTotal[0].stDCCTotal[j]);
              }
             
         }

      if (srTransRec.byTransType !=SETTLE)
      {
           vdPrintTitleCenter("END OF REPORT");
           vdCTOS_PrinterFline(2);
      }

     vdLineFeed(FALSE);

	//vdCTOSS_PrinterEnd(); // Commented to support print confirmation -- sidumili
	
	// Capture report -- sidumili		
	vdDebug_LogPrintf("Capturing erm report...");
	vdDeleteBMP();
	vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
	CTOS_KBDBufFlush();
	inCTOSS_ERM_Form_Receipt(0);

	/*  tine:  no screen display for detail report due to indeterminate report length
	if (strGBLVar.fGBLvConfirmPrint)
	{
		// Print confirmation -- sidumili
		if (inPrintConfirmation("DETAIL REPORT", FALSE) == d_OK)
		{
			vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
			vdCTOSS_PrinterEnd();
		}
		else
		{			
			vdCTOSS_PrinterEnd();
		}
	}
	else  */
	{
		//usCTOSS_LCDDisplay("DETAIL REPORT|PRINTING REPORT...");
		vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
		vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
		vdCTOSS_PrinterEnd();
	}
	
    //return;
}

#else
void vdCTOS_PrintDetailReport()
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;
	ACCUM_REC srAccumRec;
	char szStr[d_LINE_SIZE + 1];
         BYTE baTemp[PAPER_X_SIZE * 64];
	int j, inCSTCount=0;
	int inDCCTransactions = 0;

//    CTOS_PrinterPutString("vdCTOS_PrintDetailReport");

	if( printCheckPaper()==-1)
		return;


	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("detail host Index",shHostIndex);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DETAIL_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("detail MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

    inResult = inCTOS_ChkBatchEmpty();
    if(d_OK != inResult)
        return;

	inBatchNumOfRecord = inBatchNumRecord();

	DebugAddINT("batch record",inBatchNumOfRecord);
	if(inBatchNumOfRecord <= 0)
	{
		vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
		return;
	}

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();
	ushCTOS_printBatchRecordHeader();
	vdCTOSS_PrinterEnd();

	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;
	inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
	for(i=0; i<inBatchNumOfRecord; i++)
	{
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
		if(/*srTransRec.byTransType != PREAUTH_VER &&*/ srTransRec.byTransType != PREAUTH_VOID)
		{
			vdCTOSS_PrinterStart(100);
			CTOS_PrinterSetHeatLevel(4);
		    ushCTOS_printBatchRecordFooter();
			vdCTOSS_PrinterEnd();
		}
	}

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;
    }

	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);
    vdCTOS_PrinterFline(1);

    for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
    {
        if((srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount== 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidCashCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidCompCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidBNPLCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoid2GOCount == 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidRefundCount == 0))
            continue;

        memset(szStr, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        strcpy(szStr,"GRAND TOTAL");
        inPrint(szStr);
        inReportType = PRINT_CARD_TOTAL;
        vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
    }

   if (strTCT.fDCCTerminal==TRUE)
         {
         
              //memset(szStr, 0x00, d_LINE_SIZE);
              //strcpy(szStr,"TOTALS BY CURRENCY:");
              //inPrint(szStr);
              inCSTCount=inCSTNumRecord()+1;
              
              for(j = 0; j < inCSTCount;j ++)
              {
                   if((srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount == 0)
                   && (srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount == 0))
                   continue;

		 inDCCTransactions++;

		 if(inDCCTransactions == 1)
                   {
                        memset(szStr, 0x00, d_LINE_SIZE);
                        strcpy(szStr,"TOTALS BY CURRENCY:");
                        inPrint(szStr);
                   }
                   
                   inCSTRead(j);
                   vdDebug_LogPrintf("Count[%d]", j);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stCardTotal[%d].usVoidSaleCount[%d]", j,srAccumRec.stBankTotal[0].stCardTotal[j].usVoidSaleCount);
                   vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stCardTotal[%d].usRefundDCCCount[%d]", j,srAccumRec.stBankTotal[0].stCardTotal[j].usRefundDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stCardTotal[%d].ulTipTotalDCCAmount[%ul]", j,srAccumRec.stBankTotal[0].stCardTotal[j].ulTipTotalDCCAmount);		
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stCardTotal[%d].usOffSaleDCCCount[%d]", j,srAccumRec.stBankTotal[0].stCardTotal[j].usOffSaleDCCCount);
                   memset(szStr, " ", d_LINE_SIZE);
                   strcpy(szStr, strCST.szCurSymbol);
                   inPrint(szStr);
                   
                   vdCTOS_PrintAccumeByDCCCurrency(inReportType, srAccumRec.stBankTotal[0].stDCCTotal[j]);
              }
             
         }

    if (srTransRec.byTransType !=SETTLE)
  	            vdPrintTitleCenter("END OF REPORT");

    vdLineFeed(FALSE);

	vdCTOSS_PrinterEnd();
    return;
}
#endif
int inCTOS_rePrintLastReceipt(void)
{
    int   inResult;
    BYTE  szInvNoAsc[6+1];
    BYTE  szInvNoBcd[3];
    BOOL   needSecond = TRUE;
    BYTE   key;
   BOOL fERM = strTCT.fSignatureFlag;

    //for MP200 no need print
	if (isCheckTerminalMP200() == d_OK)
    	return (d_OK);

    CTOS_PrinterSetHeatLevel(4);
    if( printCheckPaper()==-1)
        return d_NO;

	memcpy(srTransRec.szInvoiceNo,strTCT.szLastInvoiceNo,INVOICE_BCD_SIZE);
	DebugAddHEX("last invoice", strTCT.szLastInvoiceNo,3 );

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_MultiAPBatchSearch(d_IPC_CMD_REPRINT_LAST);
		if(d_OK != inResult)
			return inResult;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inResult = inCTOS_MultiAPGetVoid();
			if(d_OK != inResult)
				return inResult;
		}
		inResult = inCTOS_BatchSearch();
		if(d_OK != inResult)
			return inResult;
	}

        inIITRead(srTransRec.IITid);
        inCDTRead(srTransRec.CDTid);
        inHDTRead(srTransRec.HDTid);

	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);

	//vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy, FALSE);
	vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy, fERM);

	return d_OK;
}

USHORT ushCTOS_printReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	ULONG ulFileSize;
	BOOL CustCopy=TRUE;
    int fNSRflag;

    BYTE szTitle[25+1];
    BYTE szDisMsg[30];
   BOOL fERM = strTCT.fSignatureFlag;

	vdDebug_LogPrintf("--ushCTOS_printReceipt--");

    if( printCheckPaper()==-1)
    	return -1;

	if (isCheckTerminalMP200() == d_OK)
	{
		if (srTransRec.byTransType == LOY_BAL_INQ)
		return (d_OK);
	}

//TINE:  removed "PROCESSING" before "PRINTING" display
#if 0
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PROCESSING...");
	usCTOSS_LCDDisplay(szDisMsg);  
#endif

	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);

	fNSRflag = inNSRFlag();
	if(fNSRflag == 1 && strTCT.fNSRCustCopy == FALSE)
		CustCopy=FALSE;

	if ((isCheckTerminalMP200() != d_OK) && (get_env_int("#ORIENTATION")!=1))
		vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, CustCopy, strTCT.fPrintMerchCopy, fERM);
	     //vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, CustCopy, strTCT.fPrintMerchCopy, FALSE);
	else
	      vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, CustCopy, strTCT.fPrintMerchCopy, fERM);
	     //vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, CustCopy, strTCT.fPrintMerchCopy, TRUE);

    return (d_OK);
}

USHORT ushCTOS_printErrorReceipt(void)
{
	char szTemp[42+1];

	if(srTransRec.byTransType == SETTLE && memcmp(srTransRec.szRespCode, "95", 2) == 0)
        return (d_OK);

	vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	memset(szTemp, 0x00, sizeof(szTemp));

    if(memcmp(srTransRec.szResponseText, "CALL MCC HELP DESK", strlen(srTransRec.szResponseText)) == 0)
        sprintf(szTemp, "ERROR: %s, %s", srTransRec.szRespCode, srTransRec.szResponseText);
    else
		sprintf(szTemp, "ERROR: %s", srTransRec.szResponseText);
        //sprintf(szTemp, "ERROR: %02d, %s", atoi(srTransRec.szRespCode), srTransRec.szResponseText);

	vdPrintCenter(szTemp);
	vdPrintCenter("============================================");
    ushCTOS_PrintHeader(0);
	vdCTOS_PrinterFline(6);

	vdCTOSS_PrinterEnd();
    return (d_OK);
}


int inCTOS_REPRINT_ANY()
{
    char szErrMsg[30+1];
    int   inResult;

	fRePrintFlag = TRUE;
         vdCTOS_SetTransType(REPRINT_ANY);
	inResult = inCTOS_rePrintReceipt();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }

    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	return inResult;
}

int inCTOS_REPRINT_LAST()
{
    char szErrMsg[30+1];
	int   inResult;

	fRePrintFlag = TRUE;
	inResult = inCTOS_rePrintLastReceipt();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }

    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;

	return inResult;
}

int inCTOS_PRINTF_SUMMARY()
{
    char szErrMsg[30+1];
    //BYTE szDisMsg[100];

    //strcpy(szDisMsg, "SUMMARY REPORT");
    //strcat(szDisMsg, "|");
    //strcat(szDisMsg, "PRINTING...");
    //usCTOSS_LCDDisplay(szDisMsg);

	vdCTOS_PrintSummaryReport();
	
    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }

    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();

	return d_OK;

}

int inCTOS_PRINTF_DETAIL()
{
    char szErrMsg[30+1];
    BYTE szDisMsg[100];

	// Commented to support print confirmation -- sidumili
    //strcpy(szDisMsg, "DETAIL REPORT");
    //strcat(szDisMsg, "|");
    //strcat(szDisMsg, "PRINTING...");
    //usCTOSS_LCDDisplay(szDisMsg);

	vdCTOS_PrintDetailReport();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }

    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();

	return d_OK;

}

int inCTOS_REPRINTF_LAST_SETTLEMENT()
{
    char szErrMsg[30+1];

    ushCTOS_ReprintLastSettleReport();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
		CTOS_LCDTClearDisplay();
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();

    return d_OK;
}

void vdCTOS_PrintAccumeByHostAndCard (int inReportType, STRUCT_TOTAL Totals)
{
    USHORT usSaleCount=Totals.usSaleCount-Totals.usOffSaleCount;
    ULONG ulSaleTotalAmount=Totals.ulSaleTotalAmount-Totals.ulOffSaleTotalAmount;
    USHORT usRefundCount=Totals.usRefundCount;
    ULONG  ulRefundTotalAmount=Totals.ulRefundTotalAmount;
    USHORT usVoidSaleCount=Totals.usVoidSaleCount;// + Totals.usRefundCount; #00189
    ULONG  ulVoidSaleTotalAmount=Totals.ulVoidSaleTotalAmount;// + Totals.ulRefundTotalAmount; #00189
    USHORT usOffSaleCount=Totals.usOffSaleCount;
    ULONG  ulOffSaleTotalAmount=Totals.ulOffSaleTotalAmount;
    USHORT usTipCount=Totals.usTipCount;
    ULONG  ulTipAmount=Totals.ulTipTotalAmount;

    USHORT usVoidRefundCount=Totals.usVoidRefundCount;
    ULONG  ulVoidRefundTotalAmount=Totals.ulVoidRefundTotalAmount;

    //RTR
    USHORT usRedeemCount=Totals.usRedeemCount;
    ULONG  ulRedeemTotalAmount=Totals.ulRedeemTotalAmount;
    USHORT usVoidRedeemCount=Totals.usVoidRedeemCount;
    ULONG  ulVoidRedeemTotalAmount=Totals.ulVoidRedeemTotalAmount;

    // Regular
    USHORT usRegularCount=Totals.usRegularCount;
    ULONG  ulRegularTotalAmount=Totals.ulRegularTotalAmount;
    USHORT usVoidRegularCount=Totals.usVoidRegularCount;
    ULONG  ulVoidRegularTotalAmount=Totals.ulVoidRegularTotalAmount;


    // Reduce
    USHORT usReduceCount=Totals.usReduceCount;
    ULONG  ulReduceTotalAmount=Totals.ulReduceTotalAmount;
    USHORT usVoidReduceCount=Totals.usVoidReduceCount;
    ULONG  ulVoidReduceTotalAmount=Totals.ulVoidReduceTotalAmount;

    // Zero
    USHORT usZeroCount=Totals.usZeroCount;
    ULONG  ulZeroTotalAmount=Totals.ulZeroTotalAmount;
    USHORT usVoidZeroCount=Totals.usVoidZeroCount;
    ULONG  ulVoidZeroTotalAmount=Totals.ulVoidZeroTotalAmount;

    // bnpl
    USHORT usBNPLCount=Totals.usBNPLCount;
    ULONG  ulBNPLTotalAmount=Totals.ulBNPLTotalAmount;
    USHORT usVoidBNPLCount=Totals.usVoidBNPLCount;
    ULONG  ulVoidBNPLTotalAmount=Totals.ulVoidBNPLTotalAmount;

    //cash2Go
    USHORT us2GOCount=Totals.us2GOCount;
    ULONG  ul2GOTotalAmount=Totals.ul2GOTotalAmount;
    USHORT usVoid2GOCount=Totals.usVoid2GOCount;
    ULONG  ulVoid2GOTotalAmount=Totals.ulVoid2GOTotalAmount;

    //Cash
    USHORT usCashCount=Totals.usCashCount;
    ULONG  ulCashTotalAmount=Totals.ulCashTotalAmount;
    USHORT usVoidCashCount=Totals.usVoidCashCount;
    ULONG  ulVoidCashTotalAmount=Totals.ulVoidCashTotalAmount;

    // sale completion
    USHORT usCompCount=Totals.usCompCount;
    ULONG  ulCompTotalAmount=Totals.ulCompTotalAmount;
    USHORT usVoidCompCount=Totals.usVoidCompCount;
    ULONG  ulVoidCompTotalAmount=Totals.ulVoidCompTotalAmount;

         char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szFormattedAmount[d_LINE_SIZE + 1];
    USHORT usTotalCount;
    ULONG  ulTotalAmount;
    BYTE baTemp[PAPER_X_SIZE * 64];

    CTOS_PrinterSetHeatLevel(4);
    memset (baTemp, 0x00, sizeof(baTemp));

    // fix for Wrong implementation of USD Currency
    //USD currency prompts upon voiding & settlement
    inTCTRead(1);
    if(strTCT.fRegUSD == 1)
        inCSTRead(2);
    else
        inCSTRead(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    //types
    memset(szStr, 0x00, d_LINE_SIZE);
    strcpy(szStr,"TYPES           COUNT     CUR             AMT");
    inPrint(szStr);

    //sale
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulSaleTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"SALES           %3d       %s",usSaleCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    if(usRedeemCount > 0)
    {
        //RTR
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulRedeemTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        //sprintf(szStr,"RTR REDEEM      %3d       %s",usRedeemCount, strCST.szCurSymbol);
        sprintf(szStr,"RTR SALES       %3d       %s",usRedeemCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usRegularCount > 0)
    {
        //Regular sale
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulRegularTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"INST REGULAR    %3d       %s",usRegularCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usReduceCount > 0)
    {
        //Reduce sale
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulReduceTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"INST REDUCE     %3d       %s",usReduceCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }



    if(usZeroCount > 0)
    {
        //Zero sale
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulZeroTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"INST ZERO       %3d       %s",usZeroCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usBNPLCount > 0)
    {
        //Zero sale
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulBNPLTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"INST SNPL       %3d       %s",usBNPLCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(us2GOCount > 0)
    {
        //Cash2Go
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ul2GOTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"INST C2GO       %3d       %s",us2GOCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }


    if(usCashCount > 0)
    {
        //CASH ADV
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulCashTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"CASH ADV        %3d       %s",usCashCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usCompCount > 0)
    {
        //SALE COMP
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulCompTotalAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        sprintf(szStr,"PRE COMP        %3d       %s",usCompCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    //void
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulVoidSaleTotalAmount);
    szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
    //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"VOID            %3d       %s", usVoidSaleCount,strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    if(usVoidRedeemCount > 0)
    {
        //VOID RTR
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidRedeemTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"RTR VOID        %3d       %s",usVoidRedeemCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }


    if(usVoidRegularCount > 0)
    {
        //VOID Regular
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidRegularTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID INST REG   %3d       %s",usVoidRegularCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usVoidReduceCount > 0)
    {
        //VOID Reduce
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidReduceTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID INST RED   %3d       %s",usVoidReduceCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }


    if(usVoidZeroCount > 0)
    {
        //VOID Zero
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidZeroTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID INST ZERO  %3d       %s",usVoidZeroCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usVoidBNPLCount > 0)
    {
        //VOID BNPL
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidBNPLTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID INST SNPL  %3d       %s",usVoidBNPLCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usVoid2GOCount > 0)
    {
        //VOID 2GO
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoid2GOTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID CASH2GO    %3d       %s",usVoid2GOCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }


    if(usVoidCashCount > 0)
    {
        //VOID CASH ADV
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidCashTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID CASH ADV   %3d       %s",usVoidCashCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usVoidCompCount > 0)
    {
        //VOID SALE COMP
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidCompTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID SALE COMP  %3d       %s",usVoidCompCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

    if(usVoidRefundCount > 0)
    {
        //VOID REFUND
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, 0x00, d_LINE_SIZE);
        memset(szFormattedAmount, 0x00, d_LINE_SIZE);
        sprintf(szTemp,"%ld", ulVoidRefundTotalAmount);
        szFormattedAmount[0]='-'; // #00068 - *Void  total should have negative sign
        //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
        sprintf(szStr,"VOID REFUND     %3d       %s",usVoidRefundCount, strCST.szCurSymbol);
        inPrintLeftRight(szStr, szFormattedAmount, 46);
    }

		//#00210 - Terminal prints the TIP/OFFLINE AND  REFUND on settlement/summary and detailed report/ on shopcard host - for testing
		//if(inInstallmentGroup == 2 && strCDT.inInstGroup == 4)
		   	//CTOS_PrinterPutString("inInstallmentGroup");
		//else
		//{
			//refund
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulRefundTotalAmount);
    szFormattedAmount[0]='-'; // #00068 - *Refund  total should have negative sign
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
    //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"REFUNDS         %3d       %s", usRefundCount,strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //tips sale
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulTipAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"TIPS                      %s", strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //offline sale
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulOffSaleTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"OFFLINE         %3d       %s", usOffSaleCount,strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);
		//}

    inPrint("---------------------------------------------");

    usTotalCount = usSaleCount + usOffSaleCount + usRefundCount + usRedeemCount + usRegularCount + usReduceCount + usZeroCount + usBNPLCount +us2GOCount+ usCashCount + usCompCount;
    //usTotalCount = usSaleCount + usOffSaleCount + usRefundCount;
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    if (inReportType == PRINT_HOST_TOTAL)
    {
        if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulRedeemTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount+ulCashTotalAmount+ulCompTotalAmount))
        {
            ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulRedeemTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount+ulCashTotalAmount+ulCompTotalAmount);
            sprintf(szTemp,"%ld", ulTotalAmount);
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
            szFormattedAmount[0]='-';
        }
        else
        {
            ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulRedeemTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount+ulCashTotalAmount+ulCompTotalAmount) - ulRefundTotalAmount;
            sprintf(szTemp,"%ld", ulTotalAmount);
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
        }
    }
    else if(inReportType == PRINT_CARD_TOTAL)
    {
        if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulRedeemTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount+ulCashTotalAmount+ulCompTotalAmount))
        {
            ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulRedeemTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount+ulCashTotalAmount+ulCompTotalAmount);
            sprintf(szTemp,"%ld", ulTotalAmount);
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
            szFormattedAmount[0]='-';
        }
        else
        {
            ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulRedeemTotalAmount+ulRegularTotalAmount+ulReduceTotalAmount+ulZeroTotalAmount+ulBNPLTotalAmount+ul2GOTotalAmount+ulCashTotalAmount+ulCompTotalAmount) - ulRefundTotalAmount;
            sprintf(szTemp,"%ld", ulTotalAmount);
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
        }
    }
    sprintf(szStr,"TOTALS          %3d       %s", usTotalCount,strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);
    vdCTOS_PrinterFline(1);
}

int inCTOS_SelectFont(int inFontMode,int inFontSize ,int inFontStyle,char * szFontName)
{
	if(inFontMode == d_FONT_TTF_MODE)
	{
		CTOS_PrinterFontSelectMode(d_FONT_TTF_MODE);	//set the printer with TTF Mode
		CTOS_PrinterTTFSelect("times.ttf", inFontStyle);
	}
	else
	{

		CTOS_PrinterFontSelectMode(d_FONT_FNT_MODE);	//set the printer with default Mode
		CTOS_LanguagePrinterFontSize(inFontSize, 0, TRUE);
	}
	return d_OK;

}

int inCTOS_PrintSettleReport(BOOL fManualSettle, BOOL fCaptureReceipt)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult;
	int inTranCardType;
	int inReportType;
	int i;
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];
    int *pinTransDataid;
	int inBatchNumOfRecord;
	int inCount;
    int inIITCount=0;
    int j, inCSTCount=0;
    BYTE szLogoPath[256 + 1];
	 int inDCCTransactions = 0;

	BOOL fPrintSettleDetailReport = (VS_BOOL)strTCT.fPrintSettleDetailReport;

       vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
       vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");


	vdDebug_LogPrintf("--inCTOS_PrintSettleReport--");
	vdDebug_LogPrintf("fCaptureReceipt[%d]", fCaptureReceipt);
	vdDebug_LogPrintf("strTCT.byERMMode[%d]", strTCT.byERMMode);

	if( printCheckPaper()==-1)
		return d_NO;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return d_NO;
	}

	if (fFSRMode() == TRUE || (isCheckTerminalMP200() == d_OK))
		fPrintSettleDetailReport = FALSE;

	srTransRec.fManualSettle=fManualSettle;	//#00246 - On powerfail terminal prints incorrect footer message for Manual POSTING receipt

	inCTLOS_Updatepowrfail(PFR_BATCH_SETTLEMENT_PRINT);//1204

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;
    }

	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);

	//if (isCheckTerminalMP200() != d_OK)
		//inCTOS_DisplayPrintBMP();

	//ushCTOS_PrintHeader(0);

    memset(szLogoPath, 0x00, sizeof(szLogoPath));
    sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
	vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);

	
	ushCTOS_PrintHeaderAddress();

	vdPrintTitleCenter("SETTLEMENT REPORT");

	CTOS_PrinterFline(d_LINE_DOT * 1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    printHostLabel();

	vdDebug_LogPrintf("time[%02x:%02x:%02x]date[%02x][%02x]",srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);

	printTIDMID();

	vdBackUpReportTime(); // Fix for time on last settle receipt -- sidumili

	printDateTime(FALSE);

	printBatchNO();

	vdPrintFSRMode();

    // additional - mfl

    vdCTOS_PrinterFline(1);
    vdPrintTitleCenter("*** SUMMARY REPORT ***");
    vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0); // 00034 - Card sheme in Settlement receipt is not fully printed

    inIITCount=inIITNumRecord()+1;
	for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
	{
		BOOL fPrintTotal=FALSE;
		inReportType = PRINT_CARD_TOTAL;

		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <inIITCount; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRedeemCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCompCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRegularCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usReduceCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usZeroCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBNPLCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].us2GOCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRegularCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidReduceCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidZeroCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidBNPLCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoid2GOCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRedeemCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidCashCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidCompCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRefundCount== 0))
					continue;

				vdDebug_LogPrintf("Count[%d]", i);
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				//memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);
				 //if(srTransRec.fDebit == TRUE && strcmp(strIIT.szIssuerLabel, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
				 if(srTransRec.fDebit == TRUE)
				 	strcpy(szStr,"MCC DEBIT");
				 else
					strcpy(szStr,strIIT.szIssuerLabel);

				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				fPrintTotal=TRUE;
			}
			//after print issuer total, then print host toal
			if(fPrintTotal == TRUE)
			{
				memset(szStr, ' ', d_LINE_SIZE);
				//memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"GRAND TOTAL");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
			}
		}
		else
		{

			memset(szStr, ' ', d_LINE_SIZE);
			//memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
			inPrint(szStr);
            vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
		}
	}

	if (strTCT.fDCCTerminal==TRUE)
         {
         
              //memset(szStr, 0x00, d_LINE_SIZE);
              //strcpy(szStr,"TOTALS BY CURRENCY:");
              //inPrint(szStr);
              inCSTCount=inCSTNumRecord()+1;
              
              for(j = 0; j < inCSTCount;j ++)
              {
                   if((srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount == 0)
		&&(srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount == 0)
                   && (srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount == 0))
                   continue;

		inDCCTransactions++;

		 if(inDCCTransactions == 1)
                   {
                        memset(szStr, 0x00, d_LINE_SIZE);
                        strcpy(szStr,"TOTALS BY CURRENCY:");
                        inPrint(szStr);
                   }
                   
                   inCSTRead(j);
                   vdDebug_LogPrintf("Count[%d]", j);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usCompDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usVoidSaleCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount);
                   vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usRefundDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].ulTipTotalDCCAmount[%ul]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount);		
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usOffSaleDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount);
                   memset(szStr, 0x00, d_LINE_SIZE);
                   strcpy(szStr, strCST.szCurSymbol);
                   inPrint(szStr);
                   
                   vdCTOS_PrintAccumeByDCCCurrency(inReportType, srAccumRec.stBankTotal[0].stDCCTotal[j]);
              }
         }

	if (srTransRec.byTransType !=SETTLE)
    {
         vdPrintTitleCenter("END OF REPORT");
		 vdCTOS_PrinterFline(2);
    }

    if(strTCT.fPrintReceipt == TRUE)
    {
    	if (isCheckTerminalMP200() != d_OK)
     	   inCTOS_DisplayPrintBMP();

		//if (isCheckTerminalMP200() != d_OK)
     	//	vdCTOSS_PrinterEnd();
    }

    /*do not include detail report in ERM*/
	if (isCheckTerminalMP200() != d_OK)
	{
		//vdCTOSS_PrinterStart(100);
		//CTOS_PrinterSetHeatLevel(4);
	}

	// Flag to print detail report during settlement
	if (fPrintSettleDetailReport)
	{

		vdCTOS_PrinterFline(1);
		vdPrintTitleCenter("*** DETAIL REPORT ***");
		vdCTOS_PrinterFline(1);

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);


		// additional - mfl
		//inPrint("CARD NAME			 CARD NUMBER");
		//inPrint("EXP DATE			 TRACE NO.");
		//inPrint("TRANSACTION		 AMOUNT");
		//inPrint("APPR. CODE 		 ENTRY MODE");

		// Print Detail Report Header Details
		vdPrintDetailReportHeader();

		printDividingLine(DIVIDING_LINE_DOUBLE);

		//if (isCheckTerminalMP200() != d_OK)
		//	vdCTOSS_PrinterEnd();

		inBatchNumOfRecord = inBatchNumRecord();


		pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
		inCount = 0;
		inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
		for(i=0; i<inBatchNumOfRecord; i++)
		{
			inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
			inCount ++;
			if(/*srTransRec.byTransType != PREAUTH_VER &&*/ srTransRec.byTransType != PREAUTH_VOID)
			{
				//if (isCheckTerminalMP200() != d_OK)
				//{
					//vdCTOSS_PrinterStart(100);
					//CTOS_PrinterSetHeatLevel(4);
				//}

				ushCTOS_printBatchRecordFooter();

				if (srTransRec.fDCCOptin == TRUE)
				{
					ushCTOS_printBatchDCCRecordFooter(); // Print Foreign Currency
				}

				printDividingLine(DIVIDING_LINE);
				//if (isCheckTerminalMP200() != d_OK)
					//vdCTOSS_PrinterEnd();
			}
		}

	}
	// fix for Wrong format of Settlement receipt
	//Detailed report is printed first. should be summary 1st the detail report

	vdRestoreReportTime(); // Fix for time on last settle receipt -- sidumili

	//if (isCheckTerminalMP200() != d_OK)
	//{
	//	vdCTOSS_PrinterStart(100);
	//	CTOS_PrinterSetHeatLevel(4);
	//}

	//#00014 - No "Settlement confirmed" upon printing Last settlement Report
    if(fManualSettle == TRUE)
    {
        vdPrintTitleCenter("DELETED BATCH FOR");
		vdPrintTitleCenter("MANUAL POSTING");
		vdCTOS_PrinterFline(2);
    }
    else
    {
    	vdPrintTitleCenter("SETTLEMENT CONFIRMED");
		vdCTOS_PrinterFline(2);
    }

    vdLineFeed(FALSE);

	// Capture erm receipt -- sidumili
	if (fCaptureReceipt == TRUE)
	{
		vdDebug_LogPrintf("Capturing erm receipt...");
		vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");

		if (isERMMode() == TRUE);
		    inCTOSS_ERM_Form_Receipt(1);
	}
	else
	{     
		vdCTOSS_PrinterEnd();
	}

	return d_OK;
}

USHORT ushCTOS_printBatchRecordHeader(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];

    BYTE szLogoPath[256 + 1];
	
    BYTE szMsg[256];
    BYTE szPrintMsg[512];
    ushCTOS_PrintHeader(0);	
    
    //memset(szLogoPath, 0x00, sizeof(szLogoPath));
    memset(szMsg, 0x00, sizeof(szMsg));
    memset(szPrintMsg, 0x00, sizeof(szPrintMsg));
	
    //sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
	//vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);

	//ushCTOS_PrintHeaderAddress();

    vdPrintTitleCenter("DETAIL REPORT");
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    printHostLabel();

	printTIDMID();

	vdCTOS_SetDateTime();

	printDateTime(FALSE);

	printBatchNO();

	vdPrintFSRMode();

    vdCTOS_PrinterFline(1);
    inPrint("CARD NAME           CARD NUMBER");
    inPrint("EXP DATE            TRACE NO.");
    inPrint("TRANSACTION         DATE/TIME");
    inPrint("CURRENCY            AMOUNT");
    inPrint("APPR. CODE          ENTRY MODE");
	
	printDividingLine(DIVIDING_LINE_DOUBLE);
	return d_OK;
}


USHORT ushCTOS_printBatchRecordFooter(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];
         char szTempDate[d_LINE_SIZE + 1];
	BYTE szBaseAmount[20];
	BYTE szTipAmount[20];
	BYTE szTotalAmount[20];

	int i;

	vdDebug_LogPrintf("--ushCTOS_printBatchRecordFooter--");
	
	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);
	memset(szTempBuf1,0,sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	strcpy(szTempBuf1,srTransRec.szPAN);
	inIITRead(srTransRec.IITid);
	inCSTRead(1);

	 //if(srTransRec.fDebit == TRUE && strcmp(strIIT.szIssuerLabel, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
	 if(srTransRec.fDebit == TRUE)
	 	strcpy(szTempBuf2,"MCC DEBIT");
	 else
		strcpy(szTempBuf2,strIIT.szIssuerLabel);

	if(strHDT.fMaskDetail == TRUE)
	cardMasking(szTempBuf1, PRINT_CARD_MASKING_3);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	strcpy(szPrintBuf, szTempBuf2);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szTempBuf2));
	strcat(szPrintBuf, szTempBuf1);
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

	//Exp date and inv num
	memset(szTempBuf1, 0, sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	memset(szTempBuf3, 0, sizeof(szTempBuf3));

	wub_hex_2_str(srTransRec.szExpireDate, szTempBuf1,EXPIRY_DATE_BCD_SIZE);
	DebugAddSTR("detail--EXP",szTempBuf1,12);


    if(strHDT.fMaskDetail == TRUE)
    {
       for (i =0; i<4;i++)
       szTempBuf1[i] = '*';
    }
	memcpy(szTempBuf2,&szTempBuf1[0],2);
	memcpy(szTempBuf3,&szTempBuf1[2],2);

	memset(szStr, 0,sizeof(szStr));
	wub_hex_2_str(srTransRec.szInvoiceNo, szStr, INVOICE_BCD_SIZE);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	strcpy(szPrintBuf, szTempBuf2);
	strcat(szPrintBuf, szTempBuf3);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szTempBuf2)-strlen(szTempBuf3));
	strcat(szPrintBuf, szStr);
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

    memset(szStr, 0,sizeof(szStr));
    memset(szTempBuf1, 0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0,sizeof(szTempBuf2));
	if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
	    strcpy(szStr, "RTR SALE");
	else
        szGetTransTitle(srTransRec.byTransType, szStr);

         memset(szPrintBuf, 0x20, sizeof(szPrintBuf));
	memcpy(szPrintBuf, szStr, strlen(szStr));
	printDateTimeReportFormat(szTempDate);
	memcpy(&szPrintBuf[20], szTempDate, 15);
	szPrintBuf[46]=0x00;
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

         memset(szStr, 0,sizeof(szStr));
         memset(szTempBuf1, 0,sizeof(szTempBuf1));
         memset(szTempBuf2, 0,sizeof(szTempBuf2));
		 
	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memcpy(szPrintBuf, strCST.szCurSymbol, strlen(strCST.szCurSymbol));
         vdDebug_LogPrintf("AAA - szPrintBuf[%s]", szPrintBuf);
	wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf1, AMT_BCD_SIZE); // amount printout for detail report.
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTempBuf1, szTempBuf2);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, sizeof(szPrintBuf));
	memcpy(&szPrintBuf[20], szTempBuf2, strlen(szTempBuf2));
	szPrintBuf[46]=0x00;
	  vdDebug_LogPrintf("AAA - szPrintBuf2[%s]", szPrintBuf);
         inPrint(szPrintBuf);
	

	memset(szTempBuf2, 0,sizeof(szTempBuf2));
	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		memcpy(szTempBuf2,"ICC",3);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
		memcpy(szTempBuf2,"Manual",6);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
		memcpy(szTempBuf2,"MSR",3);
	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
		memcpy(szTempBuf2,"Fallback",8);
	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		memcpy(szTempBuf2,"Contactless",11);

    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
    strcpy(szPrintBuf, srTransRec.szAuthCode);
    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(srTransRec.szAuthCode));
    strcat(szPrintBuf, szTempBuf2);
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

	vdCTOS_PrinterFline(1);
}


USHORT ushCTOS_printBatchDCCRecordFooter(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];
	 char szTempDate[d_LINE_SIZE + 1];
	BYTE szBaseAmount[20];
	BYTE szTipAmount[20];
	BYTE szTotalAmount[20];

	int i;

  	vdDebug_LogPrintf("--ushCTOS_printBatchDCCRecordFooter--");
	//vdDebug_LogPrintf("fDCCOptin=[%d]", srTransRec.fDCCOptin);
	
    //      if(srTransRec.fDCCOptin==FALSE)
    //           return 0;

	DebugAddSTR("ushCTOS_printBatchDCCRecordFooter", srTransRec.szPAN, 10);
	memset(szTempBuf1,0,sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	strcpy(szTempBuf1,srTransRec.szPAN);
	//inIITRead(srTransRec.IITid);

	 //if(srTransRec.fDebit == TRUE && strcmp(strIIT.szIssuerLabel, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
	 if(srTransRec.fDebit == TRUE)
	 	strcpy(szTempBuf2,"MCC DEBIT");
	 else
		strcpy(szTempBuf2,strIIT.szIssuerLabel);

	if(strHDT.fMaskDetail == TRUE)
	cardMasking(szTempBuf1, PRINT_CARD_MASKING_3);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	strcpy(szPrintBuf, szTempBuf2);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szTempBuf2));
	strcat(szPrintBuf, szTempBuf1);
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

	//Exp date and inv num
	memset(szTempBuf1, 0, sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	memset(szTempBuf3, 0, sizeof(szTempBuf3));

	wub_hex_2_str(srTransRec.szExpireDate, szTempBuf1,EXPIRY_DATE_BCD_SIZE);
	DebugAddSTR("detail--EXP",szTempBuf1,12);


    if(strHDT.fMaskDetail == TRUE)
    {
       for (i =0; i<4;i++)
       szTempBuf1[i] = '*';
    }
	memcpy(szTempBuf2,&szTempBuf1[0],2);
	memcpy(szTempBuf3,&szTempBuf1[2],2);

	memset(szStr, 0,sizeof(szStr));
	wub_hex_2_str(srTransRec.szInvoiceNo, szStr, INVOICE_BCD_SIZE);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	strcpy(szPrintBuf, szTempBuf2);
	strcat(szPrintBuf, szTempBuf3);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szTempBuf2)-strlen(szTempBuf3));
	strcat(szPrintBuf, szStr);
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

    memset(szStr, 0,sizeof(szStr));
    memset(szTempBuf1, 0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0,sizeof(szTempBuf2));
	if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
	    strcpy(szStr, "RTR SALE");
	else
        szGetTransTitle(srTransRec.byTransType, szStr);

         memset(szPrintBuf, 0x20, sizeof(szPrintBuf));
	memcpy(szPrintBuf, szStr, strlen(szStr));
	printDateTimeReportFormat(szTempDate);
	memcpy(&szPrintBuf[20], szTempDate, 15);
	szPrintBuf[46]=0x00;
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

         memset(szStr, 0,sizeof(szStr));
         memset(szTempBuf1, 0,sizeof(szTempBuf1));
         memset(szTempBuf2, 0,sizeof(szTempBuf2));
		 
	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memcpy(szPrintBuf, srTransRec.szDCCCurrencySymbol, strlen(srTransRec.szDCCCurrencySymbol));
         vdDebug_LogPrintf("AAA - szPrintBuf[%s]", szPrintBuf);
	//wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf1, AMT_BCD_SIZE); // amount printout for detail report.
	memcpy(szTempBuf1, srTransRec.szForeignAmount, strlen(srTransRec.szForeignAmount));
	vdCTOS_FormatAmount(srTransRec.szAmountFormat, szTempBuf1, szTempBuf2);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, sizeof(szPrintBuf));
	memcpy(&szPrintBuf[20], szTempBuf2, strlen(szTempBuf2));
	szPrintBuf[46]=0x00;
	  vdDebug_LogPrintf("AAA - szPrintBuf2[%s]", szPrintBuf);
         inPrint(szPrintBuf);

	memset(szTempBuf2, 0,sizeof(szTempBuf2));
	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		memcpy(szTempBuf2,"ICC",3);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
		memcpy(szTempBuf2,"Manual",6);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
		memcpy(szTempBuf2,"MSR",3);
	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
		memcpy(szTempBuf2,"Fallback",8);
	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		memcpy(szTempBuf2,"Contactless",11);

    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
    strcpy(szPrintBuf, srTransRec.szAuthCode);
    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(srTransRec.szAuthCode));
    strcat(szPrintBuf, szTempBuf2);
	//inPrintNoErm(szPrintBuf);
	inPrint(szPrintBuf);

	vdCTOS_PrinterFline(1);

	return 0;
}


USHORT ushCTOS_GetFontInfor(void)
{
	USHORT usASCIIFontID;
	USHORT usFontSize;
	USHORT usFontStyle;

	CTOS_LanguagePrinterGetFontInfo( &usASCIIFontID, &usFontSize, &usFontStyle );
	vdDebug_LogPrintf("usASCIIFontID[%d]usFontSize[%d]usFontStyle[%d]",usFontSize,usFontStyle );
}

USHORT ushCTOS_ReprintLastSettleReport(void)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i;
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];
    	int *pinTransDataid;
	int inBatchNumOfRecord;
         int inIITCount=0;
         int j, inCSTCount=0;
	int inDCCTransactions = 0;

         BYTE szLogoPath[256 + 1];
	BYTE szDisMsg[100];
	
    memset(szLogoPath, 0x00, sizeof(szLogoPath));
    sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
	vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);

	//for MP200 no need print
	if (isCheckTerminalMP200() == d_OK)
    	return (d_OK);

//    	CTOS_PrinterPutString("ushCTOS_ReprintLastSettleReport");

	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return d_NO;
	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);
	inCSTRead(strHDT.inCurrencyIdx);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_REPRINT_LAST_SETT);
		if(d_OK != inRet)
			return inRet;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return inRet;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return inRet;
		}
	}

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return ST_ERROR;

          /*strcpy(szDisMsg, "PRINT LAST SETTLE");
          strcat(szDisMsg, "|");
          strcat(szDisMsg, "PRINTING...");
          usCTOSS_LCDDisplay(szDisMsg);*/

          vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
          vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
		  
	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	inResult = inCTOS_ReadBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);
	if(inResult == ST_ERROR)
	{
    CTOS_LCDTClearDisplay(); // fix on issue #00106 Error message must not appear on Host Selection after selecting host

	    vdSetErrorMessage("NO RECORD FOUND");
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return ST_ERROR;
	}
	else if(inResult == RC_FILE_READ_OUT_NO_DATA)
	{
    CTOS_LCDTClearDisplay(); // fix on issue #00106 Error message must not appear on Host Selection after selecting host

	    vdSetErrorMessage("NO RECORD FOUND");
		return ST_ERROR;
	}
    strcpy(srTransRec.szTID, srAccumRec.szTID);
    strcpy(srTransRec.szMID, srAccumRec.szMID);
    memcpy(srTransRec.szYear, srAccumRec.szYear, DATE_BCD_SIZE);
    memcpy(srTransRec.szDate, srAccumRec.szDate, DATE_BCD_SIZE);
    memcpy(srTransRec.szTime, srAccumRec.szTime, TIME_BCD_SIZE);
    memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);

	vdDebug_LogPrintf("ushCTOS_ReprintLastSettleReport|time[%02x:%02x:%02x]date[%02x][%02x]",srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

	if( printCheckPaper()==-1)
		return ST_ERROR;

    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();

	ushCTOS_PrintHeader(0);	
    //memset(szLogoPath, 0x00, sizeof(szLogoPath));
    //sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
	//vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);

    vdPrintTitleCenter("LAST SETTLEMENT REPORT");
	vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    printHostLabel();

	printTIDMID();

	printDateTime(FALSE);

	printBatchNO();

    vdCTOS_PrinterFline(1);
    vdPrintTitleCenter("*** SUMMARY REPORT ***");
    vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	inIITCount=inIITNumRecord()+1;
	for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
	{
		BOOL fPrintTotal=FALSE;
		inReportType = PRINT_CARD_TOTAL;

		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <inIITCount; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRedeemCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCompCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRegularCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usReduceCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usZeroCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBNPLCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].us2GOCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRegularCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidReduceCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidZeroCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidBNPLCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoid2GOCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRedeemCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidCashCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidCompCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidRefundCount== 0))

					continue;

				vdDebug_LogPrintf("Count[%d]", i);
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				//memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);
				 //if(srTransRec.fDebit == TRUE && strcmp(strIIT.szIssuerLabel, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
				 if(srTransRec.fDebit == TRUE)
				 	strcpy(szStr,"MCC DEBIT");
				 else
					strcpy(szStr,strIIT.szIssuerLabel);

				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				fPrintTotal=TRUE;
			}
			//after print issuer total, then print host toal
			if(fPrintTotal == TRUE)
			{

				memset(szStr, ' ', d_LINE_SIZE);
				//memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"GRAND TOTAL");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
			}
		}
		else
		{

			memset(szStr, ' ', d_LINE_SIZE);
			//memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
			inPrint(szStr);
            vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
		}
	}

	if (strTCT.fDCCTerminal==TRUE)
         {
         
              //memset(szStr, 0x00, d_LINE_SIZE);
              //strcpy(szStr,"TOTALS BY CURRENCY:");
              //inPrint(szStr);
              inCSTCount=inCSTNumRecord()+1;
              
              for(j = 0; j < inCSTCount;j ++)
              {
                   if((srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount == 0)
		&&(srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount == 0)
                   &&(srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount == 0)
                   && (srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount == 0))
                   continue;

		inDCCTransactions++;

		 if(inDCCTransactions == 1)
                   {
                        memset(szStr, 0x00, d_LINE_SIZE);
                        strcpy(szStr,"TOTALS BY CURRENCY:");
                        inPrint(szStr);
                   }

                   
                   inCSTRead(j);
                   vdDebug_LogPrintf("Count[%d]", j);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usCompDCCCount[%d]",  j,srAccumRec.stBankTotal[0].stDCCTotal[j].usCompDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usVoidSaleCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usVoidSaleDCCCount);
                   vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usRefundDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usRefundDCCCount);
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].ulTipTotalDCCAmount[%ul]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].ulTipTotalDCCAmount);		
		vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[0].stDCCTotal[%d].usOffSaleDCCCount[%d]", j,srAccumRec.stBankTotal[0].stDCCTotal[j].usOffSaleDCCCount);
                   memset(szStr, 0x00, d_LINE_SIZE);
                   strcpy(szStr, strCST.szCurSymbol);
                   inPrint(szStr);
                   
                   vdCTOS_PrintAccumeByDCCCurrency(inReportType, srAccumRec.stBankTotal[0].stDCCTotal[j]);
              }
         }

	 if (srTransRec.byTransType !=SETTLE)
  	            vdPrintTitleCenter("END OF REPORT");

    if(srAccumRec.fManualSettle == TRUE)
    {
        vdPrintTitleCenter("DELETED BATCH FOR");
		vdPrintTitleCenter("MANUAL POSTING");
    }
    else
        vdPrintTitleCenter("SETTLEMENT CONFIRMED");

	vdCTOS_PrinterFline(2);

	vdLineFeed(FALSE);

	vdCTOSS_PrinterEnd();

	return d_OK;
}

void vdCTOSS_PrinterBMPPic(unsigned int usX,unsigned int y, const char *path)
{
    char szBmpfile[100+1];

	vdDebug_LogPrintf("--vdCTOSS_PrinterBMPPic--");

	// No need to print logo when terminal is mp200 -- sidumili
	if (isCheckTerminalMP200() == d_OK)
		return;

    memset(szBmpfile, 0x00, sizeof(szBmpfile));
    sprintf(szBmpfile,"%s%s", LOCAL_PATH, path);
    vdDebug_LogPrintf("Len[%d], szBmpfile[%s]", strlen(szBmpfile), szBmpfile);

    //if(strTCT.byERMMode != 0)
    //   CTOS_PrinterBMPPic(usX, szBmpfile);
    //else
        vdCTOSS_PrinterBMPPicEx(0, 0, szBmpfile);

    return ;
}

int inPrint(unsigned char *strPrint)
{
	char szStr[46+1] = {0};
	//BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	//vdDebug_LogPrintf("inPrint | fERMTransaction[%d]", fERMTransaction);

	memset(szStr, 0x00, sizeof(szStr));
	memcpy(szStr, strPrint, strlen(strPrint));

	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//return CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);
	return d_OK;
}

int inPrintNoErm(unsigned char *strPrint)
{
	char szStr[46+1] = {0};
	//BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	//vdDebug_LogPrintf("inPrint | fERMTransaction[%d]", fERMTransaction);

	memset(szStr, 0x00, sizeof(szStr));
	memcpy(szStr, strPrint, strlen(strPrint));

	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//return CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);
	return d_OK;
}

int inPrintLeftRight(unsigned char *strLeft, unsigned char *strRight, int inWidth)
{
	char szStr[64+1] = {0};
	BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	//vdDebug_LogPrintf("inPrintLeftRight | fERMTransaction[%d]", fERMTransaction);

	if ((isCheckTerminalMP200() == d_OK) && (strTCT.byERMMode == 2))
	{
		inWidth = 46;
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}

    inWidth--;

	memset(szStr, 0x20, sizeof(szStr));
    memcpy(szStr, strLeft, strlen(strLeft));
    memcpy(&szStr[inWidth-strlen(strRight)], strRight, strlen(strRight));
	szStr[inWidth]=0x00;

	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//return CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);
	return d_OK;
}

/*albert - start - Aug2014 - compute crc*/
void vdCTOS_PrintCRC(void)
{
    int shHostIndex = 1, inNumOfMerchant=0;
    int inResult,inRet;
    unsigned char szCRC[8+1];
	unsigned char szTemp[8+1];
    char ucLineBuffer[d_LINE_SIZE];
    BYTE baTemp[PAPER_X_SIZE * 64];
	int inNumRecs = 0;
	int i = 0;
	BYTE szTempCRC[8 + 1] = {0};
	BYTE szPrintCRC[d_LINE_SIZE + 1] = {0};
	BYTE szAPName[25 + 1];

	 BYTE szDisMsg[100];
	 BYTE szErrMsg[30+1];

	vdDebug_LogPrintf("--vdCTOS_PrintCRC--");
    //vduiLightOn();

    //fix for issue #00092
    //Must display "Out of Paper..."

	// Commented to support print confirmation -- sidumili
	#if 0
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
    strcpy(szDisMsg, "CRC REPORT");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "PRINTING...");
    usCTOSS_LCDDisplay(szDisMsg);
	#endif
	
    if( printCheckPaper()==-1)
      return;

    //CTOS_LCDTClearDisplay();

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();

    fRePrintFlag = FALSE;

	//print Logo
	if(strlen(strHDT.szHeaderLogoName) > 0)
		vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
	else
		vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");
	
	ushCTOS_PrintHeaderAddress();

    vdPrintTitleCenter("CRC REPORT");
    //CTOS_PrinterFline(d_LINE_DOT * 1);
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	vdCTOS_SetDateTime();

    printDateTime(FALSE);
    //CTOS_PrinterFline(d_LINE_DOT * 1);
	vdCTOS_PrinterFline(1);

	inPrint("APPLICATION:");

	#if 1
	// Print CRC
	inReadDistinctAppName(&inNumRecs);

	vdDebug_LogPrintf("inNumRecs(%d)", inNumRecs);

	for (i = 0; i < inNumRecs; i++)
	{
		memset(szTempCRC, 0x00, sizeof(szTempCRC));
		memset(szCRC, 0x00, sizeof(szCRC));
		memset(szPrintCRC, 0x00, sizeof(szPrintCRC));

		strcpy(szTempCRC, strMultiHDT[i].szAPName);
		strcat(szTempCRC, "_CRC");
		vdDebug_LogPrintf("szTempCRC[%s]", szTempCRC);

		inCTOSS_GetEnvDB(szTempCRC, szCRC);
		vdDebug_LogPrintf("szCRC[%s]", szCRC);

		//TINE:  android
//		getAppPackageInfo(szTempCRC, szCRC);
		vdDebug_LogPrintf("szTempCRC[%s], szCRC[%s]", szTempCRC, szCRC);

		if (strlen(szCRC) > 0)
		{
			memset(szAPName, 0x00, sizeof(szAPName));

			//if (strcmp(strMultiHDT[i].szAPName, "V5S_BANCNET") == 0)
			if (strcmp(strMultiHDT[i].szAPName, "V5S_RBANK") == 0)	
				strcpy(szAPName, "V5S_RBANK");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_RBDEBIT") == 0)
				strcpy(szAPName, "V5S_RBDEBIT");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_DINERSINST") == 0)
				strcpy(szAPName, "SB INST");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_MCCINST") == 0)
				strcpy(szAPName, "SHOPCARD");

			//if (strcmp(strMultiHDT[i].szAPName, "V5S_MCCNH") == 0)
			if (strcmp(strMultiHDT[i].szAPName, "com.Source.S1_MCC.MCC") == 0)
				strcpy(szAPName, "MCC ONE APP");

			sprintf(szPrintCRC, "%16s:  %s", szAPName, szCRC);
			inPrint(szPrintCRC);
		}
		else
			break;
	}
	#else
		vdPrintCRC();
	#endif

    vdCTOS_PrinterFline(1);

	vdLineFeed(FALSE);

	vdCTOSS_PrinterEnd(); // Commented to support print confirmation -- sidumili
#if 0
	// Capture report -- sidumili
	vdDebug_LogPrintf("Capturing erm report...");
	vdDeleteBMP();
	vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
	CTOS_KBDBufFlush();
	inCTOSS_ERM_Form_Receipt(0);
			
	if (strGBLVar.fGBLvConfirmPrint)
	{
		// Print confirmation -- sidumili
		if (inPrintConfirmation("CRC REPORT", FALSE) == d_OK)
		{
			vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
			vdCTOSS_PrinterEnd();
		}
		else
		{			
			vdCTOSS_PrinterEnd();
		}
	}
	else
	{
		usCTOSS_LCDDisplay("CRC REPORT|PRINTING...");
		vdCTOSS_PrinterEnd();
	}
#endif        
}
/*albert - end - Aug2014 - compute crc*/
void vdCTOS_HostInfo(void)
{
    int shHostIndex = 1, inNumOfMerchant=0;
    int inResult,inRet;
    unsigned char szCRC[8+1];
    char ucLineBuffer[d_LINE_SIZE];
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inLoop = 0;
    char szBillerName[1024] = {0};
    char szBillerCode[1024] = {0};
    char szStr[d_LINE_SIZE + 1];
    BYTE szTempBuf[12+1];
    BYTE szBuf[50];

    vdDebug_LogPrintf("-->>vdCTOS_HostInfo[START]");

    vduiLightOn();

    //fix for issue #00092
    //Must display "Out of Paper..."
    if( printCheckPaper()==-1)
        return;


    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    CTOS_LCDTClearDisplay();

#if 0
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRINT_HOST_INFO);
        if(d_OK != inRet)
            return ;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return ;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return ;
        }
    }
#endif

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    // print CRC for all host - 08112015
    //memset(szCRC, 0, sizeof(szCRC));
    //vdComputeCRC(strHDT.ulCRC, szCRC);


    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();
    //vdSetPrintThreadStatus(1); //##00129

    //print Logo
    if(strlen(strHDT.szHeaderLogoName) > 0)
        vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
    else
        vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

    vdPrintTitleCenter("HOST INFO REPORT");
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    vdCTOS_SetDateTime();

    printDateTime(FALSE);

    inPrint("---------------------------------------------");

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "HOST        : %s", (char *)strHDT.szHostLabel);
    inPrint(szStr);

    memset(szTempBuf, 0x00, sizeof(szTempBuf));
    wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
    sprintf(szStr, "TPDU        : %s", szTempBuf);
    inPrint(szStr);

    memset(szTempBuf, 0x00, sizeof(szTempBuf));
    wub_hex_2_str(strHDT.szNII,szTempBuf,2);
    sprintf(szStr, "NII         : %s", szTempBuf);
    inPrint(szStr);

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "PRI TEL NUM : %s", (char *)strCPT.szPriTxnPhoneNumber);
    inPrint(szStr);

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "SEC TEL NUM : %s", (char *)strCPT.szSecTxnPhoneNumber);
    inPrint(szStr);

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "PRI IP      : %s", (char *)strCPT.szPriTxnHostIP);
    inPrint(szStr);

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "PRI IP PORT : %04ld", strCPT.inPriTxnHostPortNum);
    inPrint(szStr);

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "SEC IP      : %s", (char *)strCPT.szSecTxnHostIP);
    inPrint(szStr);

    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "SEC IP PORT : %04ld", strCPT.inSecTxnHostPortNum);
    inPrint(szStr);

	/*print Serial No*/
    {
        char szTermSerialNumber[15]; // print terminal serial number on all txn receipt - mfl
        char szStr1[35+1]; // print terminal serial number on all txn receipt - mfl

        memset(szStr1, 0x00, sizeof(szStr1));
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNumber,0x00,sizeof(szTermSerialNumber));

        CTOS_GetFactorySN(szTermSerialNumber);
        szTermSerialNumber[15]=0;
        sprintf(szStr1, "SERIAL NO   : %s", szTermSerialNumber);
        inPrint(szStr1);
    }

	// Print CRC
	vdPrintCRC();

    inPrint("---------------------------------------------");

    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);

    vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
    for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
    {
        if((inResult = inMMTReadRecord(strHDT.inHostIndex,inLoop)) !=d_OK)
        {
            vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", inLoop,strHDT.inHostIndex,inResult);
            continue;
            //break;
        }
        else
        {
            if(strMMT[0].fMMTEnable)
            {
                memset(szStr, 0x00, sizeof(szStr));
                sprintf(szStr, "  %s", strMMT[0].szMerchantName);
                inPrint(szStr);

                memset(szStr, 0x00, sizeof(szStr));
                sprintf(szStr, "  TID      : %s", strMMT[0].szTID);
                inPrint(szStr);

                memset(szStr, 0x00, sizeof(szStr));
                sprintf(szStr, "  MID      : %s", strMMT[0].szMID);
                inPrint(szStr);

                memset(szStr, 0x00, sizeof(szStr));
                memset(szBuf, 0x00, sizeof(szBuf));
                wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
                sprintf(szStr, "  BATCH NO : %s", szBuf);
                inPrint(szStr);
                vdCTOS_PrinterFline(1);
            }
        }
    }

    vdLineFeed(FALSE);

	vdCTOSS_PrinterEnd();

    vdDebug_LogPrintf("-->>vdCTOS_HostInfo[END]");
}

void vdGetSignatureFilename(char *ptrNewFilename)
{
	char szNewFileName[24+1];
    char szInvoiceNo[6+1];

	vdDebug_LogPrintf("--vdGetSignatureFilename--");

    memset(szInvoiceNo, 0x00, sizeof(szInvoiceNo));
    wub_hex_2_str(srTransRec.szInvoiceNo, szInvoiceNo, INVOICE_BCD_SIZE);

    memset(szNewFileName, 0x00, sizeof(szNewFileName));
    sprintf(szNewFileName, "signERM%06ld.bmp", atol(szInvoiceNo));
	memcpy(ptrNewFilename, szNewFileName, strlen(szNewFileName));

	vdDebug_LogPrintf("AAA szInvoiceNo[%s]", szInvoiceNo);
	vdDebug_LogPrintf("AAA szNewFileName[%s]", szNewFileName);
}

int ushCTOS_ePadSignature(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    ULONG ulFileSize;
	char szNewFileName[24+1];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    DISPLAY_REC szDisplayRec;
	int fNSRflag = inNSRFlag();
	BYTE szDisMsg[50];
	BYTE szFormattedAmount[45];
	BYTE szTitle[25+1];
	
	vdDebug_LogPrintf("--ushCTOS_ePadSignature--");
	vdDebug_LogPrintf("fSignatureFlag[%d]", strTCT.fSignatureFlag);
	vdDebug_LogPrintf("byPinPadType[%d]", strTCT.byPinPadType);
	vdDebug_LogPrintf("fNSRflag[%d]", fNSRflag);
	vdDebug_LogPrintf("byEntryMode[%d]", srTransRec.byEntryMode);
	vdDebug_LogPrintf("byTerminalType[%d]", strTCT.byTerminalType);
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("fDCCOptin[%d]", srTransRec.fDCCOptin);

	//tine: android - set ui thread to display nothing
	DisplayStatusLine(" ");

    if (inCTOSS_CheckCVMAmount()==d_OK)
          return d_OK;
	
	if (FALSE == strTCT.fSignatureFlag || fNSRflag == TRUE)
		return d_OK;

	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));

	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	
	memset(szStr, 0x00, sizeof(szStr));
	memset(szFormattedAmount, 0x00, sizeof(szFormattedAmount));

	if((strCDT.fDCCEnable==TRUE) && (strTCT.fDCCTerminal==TRUE))
	{
		if(srTransRec.fDCCOptin == TRUE)
		{
			strcpy(szFormattedAmount,gblszCurrForeignAmount);
		}
		else
		{
			wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
			sprintf(szFormattedAmount, "%s %s", strCST.szCurSymbol,szStr);
		}
	}
	else
	{	
		wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
		sprintf(szFormattedAmount, "%s %s", strCST.szCurSymbol,szStr);
	}
	
	
	if (isCheckTerminalMP200() == d_OK)
	{
		strcpy(szDisplayRec.szDisplay1, "SIGN ON SCREEN 		");
		sprintf(szDisplayRec.szDisplay2,"AMOUNT: %s %s",strCST.szCurSymbol,szStr);
		strcpy(szDisplayRec.szDisplay8, "CLEAR[<-]	  DONE[OK]");
		vdDebug_LogPrintf("EPAD szDisplayRec.szDisplay1 = Len[%d][%s]", strlen(szDisplayRec.szDisplay1), szDisplayRec.szDisplay1);
		vdDebug_LogPrintf("EPAD szDisplayRec.szDisplay2 = Len[%d][%s]", strlen(szDisplayRec.szDisplay2), szDisplayRec.szDisplay2);
		vdDebug_LogPrintf("EPAD szDisplayRec.szDisplay8 = Len[%d][%s]", strlen(szDisplayRec.szDisplay8), szDisplayRec.szDisplay8);
	}

    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    EMVtagLen = 0;

	vdDebug_LogPrintf("EPAD CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
	vdDebug_LogPrintf("EPAD srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);

	if ((isCheckTerminalMP200() == d_OK) &&  (strTCT.byERMMode!= 0)  && (srTransRec.byTransType == REFUND))
	{
		ePad_SignatureCaptureLibEex(&szDisplayRec);
		return d_OK;
	}

    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
        if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
        (EMVtagVal[0] != 0x1E) &&
        (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
        {
			vdDebug_LogPrintf("SIGNATURE PANE - check1");
			if (srTransRec.byTransType != SALE_OFFLINE && srTransRec.byTransType != PRE_AUTH && srTransRec.byTransType != REFUND )
			{
				if(srTransRec.byOrgTransType != SALE_OFFLINE && srTransRec.byTransType != VOID)
					return d_OK;
			}
            	
        }
    }
	else if( srTransRec.byEntryMode == CARD_ENTRY_WAVE && srTransRec.byTransType != SALE_TIP) // sidumili: Always ask for signature
	{
		vdDebug_LogPrintf("SIGNATURE PANE - check2");
		if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
		{


		    //return d_OK;
		}
	}

	//if (srTransRec.byTransType == PREAUTH_VOID)
	//	return d_OK;
	
	vdDebug_LogPrintf("Before epad signature...");

	//TINE:  android
	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));
	//strcpy(szDisplayRec.szDisplay1, "SIGN ON SCREEN 		");

	// sidumili: include total label and amount in signature panel display.
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "Total Amount:");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szFormattedAmount);

	strcpy(szDisplayRec.szDisplay1, szDisMsg);
		  
	ePad_SignatureCaptureLibEex(&szDisplayRec);

	vdDebug_LogPrintf("After epad signature...");

	return d_OK;
}

int ushCTOS_ePadSignature_Tip(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    ULONG ulFileSize;
	char szNewFileName[24+1];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    DISPLAY_REC szDisplayRec;
	int fNSRflag = inNSRFlag();
	BYTE szDisMsg[50];
	BYTE szFormattedAmount[45];
	BYTE szTitle[25+1];
	
	vdDebug_LogPrintf("--ushCTOS_ePadSignature_Tip--");
	vdDebug_LogPrintf("fSignatureFlag[%d]", strTCT.fSignatureFlag);
	vdDebug_LogPrintf("byPinPadType[%d]", strTCT.byPinPadType);
	vdDebug_LogPrintf("fNSRflag[%d]", fNSRflag);
	vdDebug_LogPrintf("byEntryMode[%d]", srTransRec.byEntryMode);
	vdDebug_LogPrintf("byTerminalType[%d]", strTCT.byTerminalType);
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("fDCCOptin[%d]", srTransRec.fDCCOptin);

	//tine: android - set ui thread to display nothing
	DisplayStatusLine(" ");

	
	if (FALSE == strTCT.fSignatureFlag)
		return d_OK;

	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));

	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	
	memset(szStr, 0x00, sizeof(szStr));
	memset(szFormattedAmount, 0x00, sizeof(szFormattedAmount));

	if((strCDT.fDCCEnable==TRUE) && (strTCT.fDCCTerminal==TRUE))
	{
		if(srTransRec.fDCCOptin == TRUE)
		{
			strcpy(szFormattedAmount,gblszCurrForeignAmount);
		}
		else
		{
			wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
			sprintf(szFormattedAmount, "%s %s", strCST.szCurSymbol,szStr);
		}
	}
	else
	{	
		wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
		sprintf(szFormattedAmount, "%s %s", strCST.szCurSymbol,szStr);
	}
	
	
	if (isCheckTerminalMP200() == d_OK)
	{
		strcpy(szDisplayRec.szDisplay1, "SIGN ON SCREEN 		");
		sprintf(szDisplayRec.szDisplay2,"AMOUNT: %s %s",strCST.szCurSymbol,szStr);
		strcpy(szDisplayRec.szDisplay8, "CLEAR[<-]	  DONE[OK]");
		vdDebug_LogPrintf("EPAD szDisplayRec.szDisplay1 = Len[%d][%s]", strlen(szDisplayRec.szDisplay1), szDisplayRec.szDisplay1);
		vdDebug_LogPrintf("EPAD szDisplayRec.szDisplay2 = Len[%d][%s]", strlen(szDisplayRec.szDisplay2), szDisplayRec.szDisplay2);
		vdDebug_LogPrintf("EPAD szDisplayRec.szDisplay8 = Len[%d][%s]", strlen(szDisplayRec.szDisplay8), szDisplayRec.szDisplay8);
	}

    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    EMVtagLen = 0;

	vdDebug_LogPrintf("EPAD CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
	vdDebug_LogPrintf("EPAD srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);

	if ((isCheckTerminalMP200() == d_OK) &&  (strTCT.byERMMode!= 0)  && (srTransRec.byTransType == REFUND))
	{
		ePad_SignatureCaptureLibEex(&szDisplayRec);
		return d_OK;
	}
#if 0
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
        if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
        (EMVtagVal[0] != 0x1E) &&
        (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
        {
			vdDebug_LogPrintf("SIGNATURE PANE - check1");
			if (srTransRec.byTransType != SALE_OFFLINE && srTransRec.byTransType != PRE_AUTH && srTransRec.byTransType != REFUND )
			{
				if(srTransRec.byOrgTransType != SALE_OFFLINE && srTransRec.byTransType != VOID)
					return d_OK;
			}
            	
        }
    }
	else if( srTransRec.byEntryMode == CARD_ENTRY_WAVE && srTransRec.byTransType != SALE_TIP) // sidumili: Always ask for signature
	{
		vdDebug_LogPrintf("SIGNATURE PANE - check2");
		if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
		{


		    //return d_OK;
		}
	}
#endif
	//if (srTransRec.byTransType == PREAUTH_VOID)
	//	return d_OK;
	
	vdDebug_LogPrintf("Before epad signature...");

	//TINE:  android
	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));
	//strcpy(szDisplayRec.szDisplay1, "SIGN ON SCREEN 		");

	// sidumili: include total label and amount in signature panel display.
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "Total Amount:");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szFormattedAmount);

	strcpy(szDisplayRec.szDisplay1, szDisMsg);
		  
	ePad_SignatureCaptureLibEex(&szDisplayRec);

	vdDebug_LogPrintf("After epad signature...");

	return d_OK;
}


int ushCTOS_ePadPrintSignature(void)
{
    char szNewFileName[24+1];
    ULONG ulFileSize;


	if (FALSE ==strTCT.fSignatureFlag)
		return d_NO;

	/*if(srTransRec.byTransType == VOID
		|| srTransRec.byTransType == SALE_TIP)
		return d_NO; */

    vdDebug_LogPrintf("ushCTOS_ePadPrintSignature");

    if(fRePrintFlag == TRUE && fRePrintPwrFailFlag == FALSE)
    {
        memset(szNewFileName, 0x00, sizeof(szNewFileName));
        vdGetSignatureFilename(&szNewFileName);
        ulFileSize=lnGetFileSize(szNewFileName);
        vdDebug_LogPrintf("AAA szNewFileName:[%s], AAA ulFileSize:[%ld]",szNewFileName, ulFileSize);
        if(ulFileSize > 0)
        {
            vdCTOS_PrinterFline(1);
            vdCTOSS_PrinterBMPPicEx(0, 0, szNewFileName);
            inCTOSS_ReadERMSignatureFile(VFBMPONAME);
            return d_OK;
        }
    }
	else
	{
		ulFileSize=lnGetFileSize(DB_SIGN_BMP);
		vdDebug_LogPrintf("AAA DB_SIGN_BMP:[%s], AAA ulFileSize:[%ld]",DB_SIGN_BMP, ulFileSize);
		if(ulFileSize > 0)
		{
			vdCTOS_PrinterFline(1);
			vdCTOSS_PrinterBMPPicEx(0, 0, DB_SIGN_BMP);
			inCTOSS_ReadERMSignatureFile(VFBMPONAME);
			return d_OK;
		}
	}

	return d_NO;
}

int ushCTOS_ClearePadSignature(void)
{
	vdDebug_LogPrintf("--ushCTOS_ClearePadSignature--");
	vdDebug_LogPrintf("fSignatureFlag[%d]", strTCT.fSignatureFlag);

	if (FALSE ==strTCT.fSignatureFlag)
		return d_OK;

	ePad_ClearSignatureCaptureLib();


	return d_OK;
}

USHORT ushCTOS_PrintLoyaltyBody(int page)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    short spacestring;
    BYTE   key;
    struct LOYALTY_DATA stLoyData;
    int inIndex=0;
    TRANS_DATA_TABLE *srTransPara;

    if(CUSTOMER_COPY_RECEIPT == page)
    {
        vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        if(srTransRec.byTransType == LOY_BAL_INQ)
            //vdPrintCenter("POINTS INQUIRY");
            inPrint("POINTS INQUIRY");
        else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
            //vdPrintCenter("RTR SALE");// fix for wrong void redeem printout - 05252015
            inPrint("RTR SALE");
        else if(srTransRec.byTransType == VOID && srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE || srTransRec.byOrgTransType == LOY_REDEEM_5050)//else if(srTransRec.byTransType == VOID_REDEEM)
            //vdPrintCenter("VOID RTR SALE");
            inPrint("VOID RTR SALE");

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        //CTOS_PrinterFline(d_LINE_DOT * 1);

        memset (szTemp, 0x00, sizeof(szTemp));
        vdCTOS_FormatPANEx(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp, strIIT.fMaskCustCopy);
        inPrintLeftRight("CARD NUMBER", szTemp, 46);

        TrimTrail(srTransRec.szCardholderName);
        inPrintLeftRight("CARDHOLDER NAME", srTransRec.szCardholderName, 46);

        vdCTOS_PrinterFline(1);

        if(srTransRec.byTransType == LOY_BAL_INQ)
        {
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount1, szTemp);
            inPrintLeftRight("POINTS BALANCE:", szTemp, 46);

            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount2, szTemp);	//ePurse Balance
            inPrintLeftRight("PESO VALUE OF POINTS:", szTemp, 46);
        }
        else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
        {
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount4, szTemp);
            inPrintLeftRight("TOTAL AMOUNT DUE", szTemp, 46);

            // redeemed amount
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount5, szTemp);
            inPrintLeftRight("AMOUNT CHARGE TO POINTS", szTemp, 46);

            //Net sales Amount
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount6, szTemp);
            inPrintLeftRight("AMOUNT CHARGE TO C. CARD", szTemp, 46);

            vdCTOS_PrinterFline(1);

            //Beginning points balance
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount1, szTemp);
            inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);

            //redeemed points
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount2, szTemp);
            inPrintLeftRight("POINTS USED FOR PAYMENT", szTemp, 46);

            //points balance
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount3, szTemp);
            inPrintLeftRight("ENDING BALANCE POINTS", szTemp, 46);
        }
        else if(srTransRec.byTransType == VOID && srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE || srTransRec.byOrgTransType == LOY_REDEEM_5050)//else if(srTransRec.byTransType == VOID_REDEEM)
        {
            char szAmtBuf[24+1] = { 0 };

            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount1, szTemp);
            inPrintLeftRight("AMOUNT REVERSED", szTemp, 46);

            // convert points for void
            memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
            srTransRec.szAmount5[0]='0';
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount2, szTemp);
            strcpy(szAmtBuf, "-");
            strcat(szAmtBuf, szTemp);
            inPrintLeftRight("AMOUNT REVERSED TO POINTS", szAmtBuf, 46);

            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount3, szTemp);
            inPrintLeftRight("AMOUNT REVERSED TO CARD", szTemp, 46);

            vdCTOS_PrinterFline(1);

            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount4, szTemp);
            inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);


            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount5, szTemp);
            inPrintLeftRight("POINTS REVERSED", szTemp, 46);

            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount6, szTemp);
            inPrintLeftRight("ENDING BALANCE POINTS", szTemp, 46);
        }
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
    else if((MERCHANT_COPY_RECEIPT == page)||(BANK_COPY_RECEIPT == page))
    {
        vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        if(srTransRec.byTransType == LOY_BAL_INQ)
            //vdPrintCenter("POINTS INQUIRY");
            inPrint("POINTS INQUIRY");
        else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
            //vdPrintCenter("RTR SALE");// fix for wrong void redeem printout - 05252015
            inPrint("RTR SALE");
        else if(srTransRec.byTransType == VOID && srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE || srTransRec.byOrgTransType == LOY_REDEEM_5050)//else if(srTransRec.byTransType == VOID_REDEEM)
            //vdPrintCenter("VOID RTR SALE");
            inPrint("VOID RTR SALE");

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        //CTOS_PrinterFline(d_LINE_DOT * 1);

        memset (szTemp, 0x00, sizeof(szTemp));
        vdCTOS_FormatPANEx(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp, strIIT.fMaskMerchCopy);
        inPrintLeftRight("CARD NUMBER", szTemp, 46);

        TrimTrail(srTransRec.szCardholderName);
        inPrintLeftRight("CARDHOLDER NAME", srTransRec.szCardholderName, 46);

        vdCTOS_PrinterFline(1);

        if(srTransRec.byTransType == LOY_BAL_INQ)
        {
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount1, szTemp); // Beginning Points Balance
            inPrintLeftRight("POINTS BALANCE:", szTemp, 46);

            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount2, szTemp);	    // ePurse Balance
            inPrintLeftRight("PESO VALUE OF POINTS:", szTemp, 46);
        }
        else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
        {
            // sales amount
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount4, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100));
            inPrintLeftRight("TOTAL AMOUNT DUE", szTemp, 46);

            // redeemed amount
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount5, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("AMOUNT CHARGE TO POINTS", szTemp, 46);

            //Net sales Amount
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount6, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szNetSalesAmount)/100), (atol(stLoyData.szNetSalesAmount)%100));
            inPrintLeftRight("AMOUNT CHARGE TO C. CARD", szTemp, 46);

            vdCTOS_PrinterFline(1);

            //Beginning points balance
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount1, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szBeginPointBalance)/100), (atol(stLoyData.szBeginPointBalance)%100));
            inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);

            //redeemed points
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount2, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemPoints)/100), (atol(stLoyData.szRedeemPoints)%100));
            inPrintLeftRight("POINTS USED FOR PAYMENT", szTemp, 46);

            //points balance
            memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nnv", srTransRec.szAmount3, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szPointBalance)/100), (atol(stLoyData.szPointBalance)%100));
            inPrintLeftRight("ENDING BALANCE POINTS", szTemp, 46);
        }
        else if(srTransRec.byTransType == VOID && srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE || srTransRec.byOrgTransType == LOY_REDEEM_5050)//else if(srTransRec.byTransType == VOID_REDEEM)
        {
               char szAmtBuf[24+1] = { 0 };

           memset(szTemp, 0x00, sizeof(szTemp));
           vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount1, szTemp);
           //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100));
           inPrintLeftRight("AMOUNT REVERSED", szTemp, 46);

           // convert points for void
           memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
           srTransRec.szAmount5[0]='0';
           vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount2, szTemp);
           strcpy(szAmtBuf, "-");
           strcat(szAmtBuf, szTemp);	//Incorrect Merchant & Bank copy receipt on Void RTR		- #00190
           inPrintLeftRight("AMOUNT REVERSED TO POINTS", szAmtBuf, 46);

           memset(szTemp, 0x00, sizeof(szTemp));
           vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount3, szTemp);
           inPrintLeftRight("AMOUNT REVERSED TO CARD", szTemp, 46);

           vdCTOS_PrinterFline(1);

           memset (szTemp, 0x00, sizeof(szTemp));
           vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount4, szTemp);
           inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);


           memset (szTemp, 0x00, sizeof(szTemp));
           vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount5, szTemp);
           inPrintLeftRight("POINTS REVERSED", szTemp, 46);

           memset (szTemp, 0x00, sizeof(szTemp));
           vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", srTransRec.szAmount6, szTemp);
           inPrintLeftRight("ENDING BALANCE: POINTS", szTemp, 46);
        }
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	return d_OK;

}

USHORT ushCTOS_PrintLoyaltyFooter(int page)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    int result;
    char szStr[35 + 1];
    CTOS_RTC SetRTC;
    char szYear[3];
    char szTempDate[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
    BYTE baTemp[PAPER_X_SIZE * 64];

	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    if(page == CUSTOMER_COPY_RECEIPT)
    {
		#if 0
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

        DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);
        if(srTransRec.byTransType != LOY_BAL_INQ)
        {
            if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
            {
                EMVtagLen = 3;
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
                if((EMVtagVal[2]& 0x0F) == 2)
                {
                    vdCTOS_PrinterFline(1);
                    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
                }
            }
        }
		#endif

		vdCTOS_PrinterFline(1);

		//ushCTOS_PrintAgreement();
		if(srTransRec.byTransType == LOY_BAL_INQ)
		{
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			if(strlen(strMMT[0].szRctFoot2) > 0)
		    	vdPrintCenter(strMMT[0].szRctFoot2);
			if(strlen(strMMT[0].szRctFoot3) > 0)
		    	vdPrintCenter(strMMT[0].szRctFoot3);
		}

		vdCTOS_PrinterFline(1);

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szTemp2, 0x00, sizeof(szTemp2));
		memset(szTemp3, 0x00, sizeof(szTemp3));
		memset(szTemp4, 0x00, sizeof(szTemp4));
		wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
		wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
		memset(szTempDate, 0x00, sizeof(szTempDate));
		sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
		vdCTOS_FormatDate(szTempDate);
		sprintf(szTemp3, "DATE: %s", szTempDate);
		sprintf(szTemp4, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
		inPrintLeftRight(szTemp3, szTemp4, 46);

		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szTemp2, 0x00, sizeof(szTemp2));
		sprintf(szTemp, "TID: %s", srTransRec.szTID);
		sprintf(szTemp2, "MID: %s", srTransRec.szMID);
		inPrintLeftRight(szTemp, szTemp2, 46);

		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szTemp2, 0x00, sizeof(szTemp2));
		memset(szTemp3, 0x00, sizeof(szTemp3));
		memset(szTemp4, 0x00, sizeof(szTemp4));
		wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
		sprintf(szTemp3, "BATCH NUM: %s", szTemp);
		sprintf(szTemp4, "TRACE NO.: %s", szTemp2);
		inPrintLeftRight(szTemp3, szTemp4, 46);


		// PRINT RRN AND HOST
		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szStr, 0x00, sizeof(szStr));
		memset(szTemp3, 0x00, sizeof(szTemp3));
		memset(szTemp4, 0x00, sizeof(szTemp4));
        sprintf(szTemp3, "REF NO.: %s", srTransRec.szRRN);		//Terminal prints incorrect value of RRN for RTR Points Inquiry and RTR Redeem
        sprintf(szTemp4, "HOST: %s", srTransRec.szHostLabel);
        inPrintLeftRight(szTemp3, szTemp4, 46);

		// PRINT APPR CODE
		memset(szStr, 0x00, d_LINE_SIZE);
		sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
		result=inPrint(szStr);

        if(strTCT.fProductCode == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "PRODUCT CODE: %s", srTransRec.szProductCode);
            result=inPrint(szStr);
        }

        if(strTCT.fCustomerNo == TRUE)
        {
	        memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "CUSTOMER NUMBER: %s", srTransRec.szCustomerNo);
	        result=inPrint(szStr);
        }

        //Terminal serial number - mfl
        memset(szTemp4, 0x00, sizeof(szTemp4));
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);
        
		//test
		usGetSerialNumber(szTermSerialNum);
		vdDebug_LogPrintf("saturn print serial number %s", szTermSerialNum);
		
        szTermSerialNum[15]=0;
        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		if(strHDT.fFooterLogo == TRUE)
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");

		vdCTOS_PrinterFline(1);
		vdPrintCenter("***** CUSTOMER COPY *****");
	}
	else if(page == MERCHANT_COPY_RECEIPT)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;

		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);
		vdDebug_LogPrintf("MERCH:LOYALTY CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("MERCH:LOYALTY srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);

        if(srTransRec.byTransType != LOY_BAL_INQ)
        {
            if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
            {
                EMVtagLen = 3;
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
                if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
                (EMVtagVal[0] != 0x1E) &&
                (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
                {
                    vdCTOS_PrinterFline(2);
                    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
                }
                else
                {
                    if(strCDT.inType != DEBIT_CARD)
                    {
                        vdCTOS_PrinterFline(1);
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
						vdCTOS_PrinterFline(1);
                        vdPrintCenter(srTransRec.szCardholderName);
                    }
                }
            }
            else
            {
                if(strCDT.inType != DEBIT_CARD)
                {
                    vdCTOS_PrinterFline(1);
                    if(ushCTOS_ePadPrintSignature() != d_OK)
                        vdCTOS_PrinterFline(1);
                    inPrint("SIGN:_______________________________________");
					vdCTOS_PrinterFline(1);
					vdPrintCenter(srTransRec.szCardholderName);
                }
            }
        }


		vdCTOS_PrinterFline(1);

        if(srTransRec.byTransType == LOY_BAL_INQ)
        {
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
            if(strlen(strMMT[0].szRctFoot1) > 0)
                vdPrintCenter(strMMT[0].szRctFoot1);
            if(strlen(strMMT[0].szRctFoot2) > 0)
                vdPrintCenter(strMMT[0].szRctFoot2);
            if(strlen(strMMT[0].szRctFoot3) > 0)
                vdPrintCenter(strMMT[0].szRctFoot3);
        }

        vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
        wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
        memset(szTempDate, 0x00, sizeof(szTempDate));
        sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
        vdCTOS_FormatDate(szTempDate);
        sprintf(szTemp3, "DATE: %s", szTempDate);
        sprintf(szTemp4, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
        inPrintLeftRight(szTemp3, szTemp4, 46);

		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szTemp2, 0x00, sizeof(szTemp2));
		sprintf(szTemp, "TID: %s", srTransRec.szTID);
		sprintf(szTemp2, "MID: %s", srTransRec.szMID);
		inPrintLeftRight(szTemp, szTemp2, 46);

		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szTemp2, 0x00, sizeof(szTemp2));
		memset(szTemp3, 0x00, sizeof(szTemp3));
		memset(szTemp4, 0x00, sizeof(szTemp4));
		wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
		sprintf(szTemp3, "BATCH NUM: %s", szTemp);
		sprintf(szTemp4, "TRACE NO.: %s", szTemp2);
		inPrintLeftRight(szTemp3, szTemp4, 46);


		// PRINT RRN AND HOST
		memset(szTemp, 0x00, sizeof(szTemp));
		memset(szTemp2, 0x00, sizeof(szTemp2));
		memset(szTemp3, 0x00, sizeof(szTemp3));
		memset(szTemp4, 0x00, sizeof(szTemp4));
        sprintf(szTemp3, "REF NO.: %s", srTransRec.szRRN); //Terminal prints incorrect value of RRN for RTR Points Inquiry and RTR Redeem
        sprintf(szTemp4, "HOST: %s", srTransRec.szHostLabel);
        inPrintLeftRight(szTemp3, szTemp4, 46);

        // PRINT APPR CODE
        memset(szStr, 0x00, d_LINE_SIZE);
        sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
        result=inPrint(szStr);
        if(strTCT.fProductCode == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "PRODUCT CODE: %s", srTransRec.szProductCode);
            result=inPrint(szStr);
        }

        if(strTCT.fCustomerNo == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "CUSTOMER NUMBER: %s", srTransRec.szCustomerNo);
            result=inPrint(szStr);
        }

        //Terminal serial number - mfl
        memset(szTemp4, 0x00, sizeof(szTemp4));
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);
		//test
		usGetSerialNumber(szTermSerialNum);
		vdDebug_LogPrintf("saturn print serial number %s", szTermSerialNum);
        
        szTermSerialNum[15]=0;
        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        if(strHDT.fFooterLogo == TRUE)
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");

        vdCTOS_PrinterFline(1);
        vdPrintCenter("***** MERCHANT COPY *****");
    }
    else if(page == BANK_COPY_RECEIPT)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;

		vdDebug_LogPrintf("BANK:LOYALTY CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("BANK:LOYALTY srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);

        if(srTransRec.byTransType != LOY_BAL_INQ)
        {
            if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
            {
                EMVtagLen = 3;
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
                if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
                (EMVtagVal[0] != 0x1E) &&
                (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
                {
                    vdCTOS_PrinterFline(2);
                    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
                }
                else
                {
                    if(strCDT.inType != DEBIT_CARD)
                    {
                        vdCTOS_PrinterFline(1);
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
						vdCTOS_PrinterFline(1);
						vdPrintCenter(srTransRec.szCardholderName);
                    }
                }
            }
            else
            {
                if(strCDT.inType != DEBIT_CARD)
                {
                    vdCTOS_PrinterFline(1);
                    if(ushCTOS_ePadPrintSignature() != d_OK)
                        vdCTOS_PrinterFline(1);
                    inPrint("SIGN:_______________________________________");
					vdCTOS_PrinterFline(1);
					vdPrintCenter(srTransRec.szCardholderName);
                }
            }
        }

		vdCTOS_PrinterFline(1);

        if(srTransRec.byTransType == LOY_BAL_INQ)
        {
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
            if(strlen(strMMT[0].szRctFoot1) > 0)
                vdPrintCenter(strMMT[0].szRctFoot1);
            if(strlen(strMMT[0].szRctFoot2) > 0)
                vdPrintCenter(strMMT[0].szRctFoot2);
            if(strlen(strMMT[0].szRctFoot3) > 0)
                vdPrintCenter(strMMT[0].szRctFoot3);
        }

        vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
        wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
        memset(szTempDate, 0x00, sizeof(szTempDate));
        sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
        vdCTOS_FormatDate(szTempDate);
        sprintf(szTemp3, "DATE: %s", szTempDate);
        sprintf(szTemp4, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
        inPrintLeftRight(szTemp3, szTemp4, 46);

        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        sprintf(szTemp, "TID: %s", srTransRec.szTID);
        sprintf(szTemp2, "MID: %s", srTransRec.szMID);
        inPrintLeftRight(szTemp, szTemp2, 46);

        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
        wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
        sprintf(szTemp3, "BATCH NUM: %s", szTemp);
        sprintf(szTemp4, "TRACE NO.: %s", szTemp2);
        inPrintLeftRight(szTemp3, szTemp4, 46);

        // PRINT RRN AND HOST
        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        sprintf(szTemp3, "REF NO.: %s", srTransRec.szRRN);//Terminal prints incorrect value of RRN for RTR Points Inquiry and RTR Redeem
        sprintf(szTemp4, "HOST: %s", srTransRec.szHostLabel);
        inPrintLeftRight(szTemp3, szTemp4, 46);

        // PRINT APPR CODE
        memset(szStr, 0x00, d_LINE_SIZE);
        sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
        result=inPrint(szStr);

        if(strTCT.fProductCode == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "PRODUCT CODE: %s", srTransRec.szProductCode);
            result=inPrint(szStr);
        }

        if(strTCT.fCustomerNo == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "CUSTOMER NUMBER: %s", srTransRec.szCustomerNo);
            result=inPrint(szStr);
        }

        //Terminal serial number - mfl
        memset(szTemp4, 0x00, sizeof(szTemp4));
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);
        
		//test
		usGetSerialNumber(szTermSerialNum);
		vdDebug_LogPrintf("saturn print serial number %s", szTermSerialNum);
        szTermSerialNum[15]=0;
        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        if(strHDT.fFooterLogo == TRUE)
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");

        vdCTOS_PrinterFline(1);
        vdPrintCenter("***** BANK COPY *****");
    }

	vdLineFeed(FALSE);

return d_OK;

}

#if 0
int inPrintISOPacket(BOOL fSendPacket, unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];


      if (inLen <= 0)
          return(ST_SUCCESS);


    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

      if (fSendPacket)
      {
		memset (baTemp, 0x00, sizeof(baTemp));
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Send ISO Packet\n");
		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

      }
      else
      {
		memset (baTemp, 0x00, sizeof(baTemp));
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Receive ISO Packet\n");

		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

      }

      CTOS_PrinterFline(d_LINE_DOT);


      pucBuff = pucMessage + inLen;
      while (pucBuff > pucMessage)
      {
          memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
          for (inBuffPtr = 0; (inBuffPtr < 32) && (pucBuff > pucMessage); inBuffPtr += 3)
          {
              sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
              pucMessage++;
          }
              ucLineBuffer[32] = '\n';
		memset (baTemp, 0x00, sizeof(baTemp));
		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, ucLineBuffer, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

      }
      CTOS_PrinterFline(d_LINE_DOT * 2);


      return (ST_SUCCESS);
}
#else
int inPrintISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE+4];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 4];

	//vdDebug_LogPrintf("inPrintISOPacket | fERMTransaction[%d]", fERMTransaction);

    if (inLen <= 0)
        return(ST_SUCCESS);

    //if (isCheckTerminalMP200() != d_OK)
	//	vdCTOSS_PrinterStart(100);
	vdDebug_LogPrintf("saturn print iso packet");

    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);

    //andoid-removed
	//CTOS_PrinterSetWorkTime(50000,1000);
	//end
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    memset(szStr, 0x00, sizeof(szStr));
	memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(szStr,"[%s] [%d] \n", pucTitle, inLen);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);

	vdCTOS_PrinterFline(1);

    pucBuff = pucMessage + inLen;
    while (pucBuff > pucMessage)
    {
    	memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
	    for (inBuffPtr = 0; (inBuffPtr < 44) && (pucBuff > pucMessage); inBuffPtr += 3)
	    {
		    sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
		    pucMessage++;
	    }
	    ucLineBuffer[44] = '\n';
	    memset (baTemp, 0x00, sizeof(baTemp));
		inCCTOS_PrinterBufferOutput(ucLineBuffer,&stgFONT_ATTRIB, fERMTransaction);

    }

	vdCTOS_PrinterFline(1);

	if (isCheckTerminalMP200() != d_OK)
	    vdCTOSS_PrinterEnd();

	return (ST_SUCCESS);
}

#endif

/*
 * All leading spaces from a string is removed, string is a null terminated
 * string.
 */
void vdCTOSvdTrimLeadZeroes (char *pchString)

{
	int inIndex;

	inIndex = 0;
	while (pchString[inIndex] == '0')
		inIndex++;
	if (inIndex != 0)
		strcpy(pchString, &(pchString[inIndex]));
}

void vdCTOS_PrinterFline(USHORT usLines)
{
	//vdDebug_LogPrintf("vdCTOS_PrinterFline | fERMTransaction[%d]", fERMTransaction);

	while(usLines > 0)
	{
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB, fERMTransaction);
		usLines--;
	}
}

int inPrintParseISOPacket(unsigned char *pucTitle, unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];


      if (inLen <= 0)
          return(ST_SUCCESS);


    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
#if 1
    memset (baTemp, 0x00, sizeof(baTemp));
    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr,"%s Len: %d", pucTitle, inLen);
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
#else
      if (fSendPacket)
      {
		memset (baTemp, 0x00, sizeof(baTemp));
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Send ISO Packet\n");
		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

      }
      else
      {
		memset (baTemp, 0x00, sizeof(baTemp));
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Receive ISO Packet\n");

		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

      }
#endif
      //CTOS_PrinterFline(d_LINE_DOT);


      pucBuff = pucMessage + inLen;
      while (pucBuff > pucMessage)
      {
          memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
          for (inBuffPtr = 0; (inBuffPtr < 32) && (pucBuff > pucMessage); inBuffPtr += 3)
          {
              sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
              pucMessage++;
          }
              ucLineBuffer[32] = '\n';
		memset (baTemp, 0x00, sizeof(baTemp));
		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, ucLineBuffer, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

      }
      CTOS_PrinterFline(d_LINE_DOT);


      return (ST_SUCCESS);
}

void vdCTOS_PrintReceiptCopies(BOOL BankCopy, BOOL CustCopy, BOOL MercCopy, BOOL fERM)
{
    BOOL fFirstReceipt=TRUE;
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;
    BYTE  key;
    ULONG ulFileSize;
    BYTE szDisMsg[100];
	BYTE szTitle[25+1];

    BOOL fNextCopy = TRUE;

    vdDebug_LogPrintf("--vdCTOS_PrintReceiptCopies--");
    vdDebug_LogPrintf("BankCopy[%d], CustCopy[%d], MercCopy[%d]", BankCopy, CustCopy, MercCopy);
    vdDebug_LogPrintf("byTerminalType[%d], byTerminalModel[%d]", strTCT.byTerminalType, strTCT.byTerminalModel);
    vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
    vdDebug_LogPrintf("fERM[%d]", fERM);

    memset(szPrintOption, 0x00, sizeof(szPrintOption));
    memcpy(szPrintOption, strTCT.szPrintOption, 3);
    inReceiptCtr=0;
    fFirstReceipt=TRUE;
    if (isCheckTerminalMP200() == d_OK && fERM == TRUE)
    {
        // Minimize the white screend display -- sidumili
        //vdCTOS_DispStatusMessage("PROCESSING...");

        ushCTOS_printAll(BANK_COPY_RECEIPT);
        vdCTOSS_PrinterEnd(); // For fast printing
        CTOS_KBDBufFlush();//cleare key buffer
        inCTOSS_ERM_Form_Receipt(0);
    }

    if (isCheckTerminalMP200() != d_OK && fFSRMode() == TRUE)
    {
        vdDebug_LogPrintf("Capturing erm receipt...");

        //vdCTOS_DispStatusMessage("PROCESSING...");

        fERMTransaction = TRUE;
        vdCTOSS_PrinterStart(500);
        ushCTOS_printAll(BANK_COPY_RECEIPT);
        vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
        CTOS_KBDBufFlush();
        inCTOSS_ERM_Form_Receipt(0);
    }
    //else
    {
        do
        {
            chPrintOption=(unsigned char)szPrintOption[inReceiptCtr];
            if((chPrintOption == '1') && (BankCopy == TRUE)) /*bank copy*/
            {
                if(fFirstReceipt == TRUE)
                {
                    printcopies_cntr = 0;
                    ushCTOS_printAll(BANK_COPY_RECEIPT);
                    CTOS_KBDBufFlush();
                    fFirstReceipt=FALSE;
                    //TINE:  modification for android terminal:  move all print processing to ushCTOS_printAll
                    //if(strTCT.fPrintReceipt == TRUE)
                    //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                    //{
                    //    inCTOS_DisplayPrintBMP();
                    //    vdCTOSS_PrinterEnd();


                    //}
                    //CTOS_Delay(500);

                }
                else
                {
                    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
                    //clearLine(7);
                    //CTOS_LCDTClearDisplay();
                    while(1)
                    {
                        if(strTCT.fPrintReceipt != TRUE)
                            break;

                        //setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT BANK COPY?");
                        //setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");

                        if(strTCT.feReceiptEnable == 0)
                        {

                            memset(szDisMsg, 0x00, sizeof(szDisMsg));
                            strcat(szDisMsg, "BANK COPY");
                            fNextCopy = usCTOSS_Confirm(szDisMsg);
                            //vduiDisplayStringCenter(7,"PRINT BANK COPY?");
                            //vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                            vdDebug_LogPrintf("next copy %d", fNextCopy );

                            //if (strTCT.fPrintWarningSound == TRUE)
                            vduiWarningSound();

                            //CTOS_KBDHit(&key);
                            if(fNextCopy == d_OK)
                            {
                                
                                ushCTOS_printAll(BANK_COPY_RECEIPT);
                                CTOS_KBDBufFlush();//cleare key buffer
                                //if(strTCT.fPrintReceipt == TRUE)
                                //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                                //{
                                //    inCTOS_DisplayPrintBMP();
                                //    vdCTOSS_PrinterEnd();
                                //}
                                break;
                            }
                            else if((fNextCopy == d_USER_CANCEL))
                            {
                                //CTOS_LCDTClearDisplay();
                                break;
                            }
                            if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                            {
                                ushCTOS_printAll(BANK_COPY_RECEIPT);
                                //if(strTCT.fPrintReceipt == TRUE)
                                //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                                //{
                                //    inCTOS_DisplayPrintBMP();
                                //    vdCTOSS_PrinterEnd();
                                //}
                                //CTOS_Delay(500);
                                break;
                            }
                        }
						else
						{
							ushCTOS_printAll(BANK_COPY_RECEIPT);
	                        CTOS_KBDBufFlush();
							//CTOS_Delay(500);
	                        break;
						}

                        
                    }
                }
            }
            else if((chPrintOption == '2') && (CustCopy == TRUE)) /*customer copy*/
            {
                if(fFirstReceipt == TRUE)
                {
                    printcopies_cntr = 0;
                    ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
                    CTOS_KBDBufFlush();//cleare key buffer
                    fFirstReceipt=FALSE;
                    //if(strTCT.fPrintReceipt == TRUE)
                    //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                    //{
                    //    inCTOS_DisplayPrintBMP();
                    //    vdCTOSS_PrinterEnd();
                    //}        
                    //CTOS_Delay(500);
                }
                else
                {
                    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
                    //clearLine(7);
                    //CTOS_LCDTClearDisplay();
                    while(1)
                    {
                        if(strTCT.fPrintReceipt != TRUE)
                            break;

                        //setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT CUSTOMER COPY?");
                        //setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");

                        if(strTCT.feReceiptEnable == 0)
                        {

                            memset(szDisMsg, 0x00, sizeof(szDisMsg));
                            strcat(szDisMsg, "CUSTOMER COPY");

                            fNextCopy = usCTOSS_Confirm(szDisMsg);
                            //vduiDisplayStringCenter(7,"PRINT CUSTOMER COPY?");
                            //vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                            vduiWarningSound();

                            // CTOS_KBDHit(&key);
                            if(fNextCopy == d_OK)
                            {
                                ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
                                //if(strTCT.fPrintReceipt == TRUE)
                                //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                                //{
                                //    inCTOS_DisplayPrintBMP();
                                //    vdCTOSS_PrinterEnd();
                                //}
                                CTOS_KBDBufFlush();//cleare key buffer
                                //CTOS_Delay(500);
                                break;
                            }
                            else if((fNextCopy == d_USER_CANCEL))
                            {
                                //CTOS_LCDTClearDisplay();
                                break;
                            }
                            if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                            {
                                ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
                                //if(strTCT.fPrintReceipt == TRUE)
                                //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                                //{
                                //    inCTOS_DisplayPrintBMP();
                                //    vdCTOSS_PrinterEnd();
                                //}
                                //CTOS_Delay(500);
                                break;
                            }
                        }
						else
						{
							ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
	                        CTOS_KBDBufFlush();//cleare key buffer
	                        //CTOS_Delay(500);
	                        break;
						}

                        
                    }
                }

            }
            else if((chPrintOption == '3') && (MercCopy== TRUE)) /*merchant copy*/
            {
                if(fFirstReceipt == TRUE)
                {
                    printcopies_cntr = 0;
                    ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
                    CTOS_KBDBufFlush();//cleare key buffer
                    fFirstReceipt=FALSE;
                    //if(strTCT.fPrintReceipt == TRUE)
                    //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                    //{
                    //    inCTOS_DisplayPrintBMP();
                    //    vdCTOSS_PrinterEnd();
                    //}
                    //CTOS_Delay(500);
                }
                else
                {
                    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
                    //clearLine(7);
                    //CTOS_LCDTClearDisplay();
                    while(1)
                    {
                        if(strTCT.fPrintReceipt != TRUE)
                            break;

                        //setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT MERCHANT COPY?");
                        //setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");

                        if(strTCT.feReceiptEnable == 0)
                        {
                            memset(szDisMsg, 0x00, sizeof(szDisMsg));
                            strcat(szDisMsg, "MERCHANT COPY");

                            fNextCopy = usCTOSS_Confirm(szDisMsg);
                            //vduiDisplayStringCenter(7,"PRINT MERCHANT COPY?");
                            //vduiDisplayStringCenter(8,"NO[X] YES[OK]");

                            vdDebug_LogPrintf("saturn next copy %d", fNextCopy );

                            vduiWarningSound();

                            //CTOS_KBDHit(&key);
                            if(fNextCopy == d_OK)
                            {
                                ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
                                CTOS_KBDBufFlush();//cleare key buffer
                                //if(strTCT.fPrintReceipt == TRUE)
                                //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                                //{
                                //    inCTOS_DisplayPrintBMP();
                                //    vdCTOSS_PrinterEnd();
                                //}
                                //CTOS_Delay(500);
                                break;
                            }
                            else if((fNextCopy == d_USER_CANCEL))
                            {
                                //CTOS_LCDTClearDisplay();								
                                break;
                            }
                            if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                            {
                                ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
                                //if(strTCT.fPrintReceipt == TRUE)
                                //if(strTCT.fPrintReceipt == TRUE && strTCT.feReceiptEnable == FALSE)
                                //{
                                //    inCTOS_DisplayPrintBMP();
                                //    vdCTOSS_PrinterEnd();
                                //}
                                //CTOS_Delay(500);
                                break;
                            }
                        }
						else
						{
							ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
	                        CTOS_KBDBufFlush();//cleare key buffer
	                        //CTOS_Delay(500);
	                        break;
						}

                        
                    }
                }
            }
			usCTOSS_LCDDisplay(" ");
			inReceiptCtr++;
            printcopies_cntr = inReceiptCtr;
        }while(inReceiptCtr < 3);

		
    }

    if(fERM == TRUE)
    {
        if(strTCT.fSignatureFlag == TRUE)
        {
            ulFileSize=lnGetFileSize(DB_SIGN_BMP);
            if(ulFileSize > 0)
                vdLinuxCommandFileCopy();
        }
    }

    vdCheckERMMode(); // Enhancement for printing speed -- sidumili
}

void vdPrintISOOption(unsigned char *pucTitle,unsigned char *uszData, int inLen)
{
	BYTE szTitle[25 + 1] = {0};
	BOOL fPrintParseISO = TRUE;

	if(strTCT.inPrintISOOption == 1)
	{
		if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
			inPrintISOPacket(pucTitle , uszData, inLen-2);
		else
			inPrintISOPacket(pucTitle , uszData, inLen);
	}
	else if(strTCT.inPrintISOOption == 2)
	{
		if (memcmp(pucTitle,"RX",2) == 0)
		{
			if (srTransRec.byTransType == DCC_MERCH_RATE_REQ || srTransRec.byTransType == DCC_RATE_REQUEST || srTransRec.byTransType == TERM_REG)
			{
				if (strCPT.inCommunicationMode == DIAL_UP_MODE)
					inPrintISOPacket(pucTitle , uszData, inLen-2);
				else
					inPrintISOPacket(pucTitle , uszData, inLen);
			}				
			else
			{
				szGetTransTitle(srTransRec.byTransType, szTitle);
				vdPrintParseISO(szTitle,pucTitle ,uszData+5);
			}
				
		}
		else
		{
			szGetTransTitle(srTransRec.byTransType, szTitle);
			vdPrintParseISO(szTitle,pucTitle ,uszData+5);
		}
	}
	else if(strTCT.inPrintISOOption == 3)
	{
		if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
			inPrintISOPacket(pucTitle , uszData, inLen-2);
		else
			inPrintISOPacket(pucTitle , uszData, inLen);
		
		if ((memcmp(pucTitle,"RX",2) == 0) && (srTransRec.byTransType == DCC_MERCH_RATE_REQ || srTransRec.byTransType == DCC_RATE_REQUEST || srTransRec.byTransType == TERM_REG))
		{
			fPrintParseISO = FALSE;
		}
		
		if (fPrintParseISO)
		{	
			szGetTransTitle(srTransRec.byTransType, szTitle);
			vdPrintParseISO(szTitle,pucTitle ,uszData+5);
		}
		

	}

}

void SetReportTime(void)
{
	BYTE szCurrTime[7] = {0};
	CTOS_RTC rtcClock;
	CTOS_RTC SetRTC;

	CTOS_RTCGet(&SetRTC);

	sprintf(szCurrTime,"%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);

	wub_str_2_hex(szCurrTime,srTransRec.szTime,6);

}

void vdBackUpReportTime(void)
{
	SetReportTime();
    memset(szgTempDate, 0x00, sizeof(szgTempDate));
    memset(szgTempTime, 0x00, sizeof(szgTempTime));
	memcpy(szgTempDate, srTransRec.szDate, DATE_BCD_SIZE);
	memcpy(szgTempTime, srTransRec.szTime, TIME_BCD_SIZE);

}

void vdRestoreReportTime(void)
{
	memcpy(srTransRec.szDate, szgTempDate, DATE_BCD_SIZE);
	memcpy(srTransRec.szTime, szgTempTime,  TIME_BCD_SIZE);
}

// Mercury Requirements -- sidumili
void vdPrintECRMessage(void)
{
	BOOL fMsg1 = FALSE;
	BOOL fMsg2 = FALSE;
	BOOL fMsg3 = FALSE;
	BOOL fMsg4 = FALSE;

	TrimTrail(srTransRec.szECRMessage1);
	TrimTrail(srTransRec.szECRMessage2);
	TrimTrail(srTransRec.szECRMessage3);
	TrimTrail(srTransRec.szECRMessage4);

	vdDebug_LogPrintf("szECRMessage1[%s]", srTransRec.szECRMessage1);
	vdDebug_LogPrintf("szECRMessage2[%s]", srTransRec.szECRMessage2);
	vdDebug_LogPrintf("szECRMessage3[%s]", srTransRec.szECRMessage3);
	vdDebug_LogPrintf("szECRMessage4[%s]", srTransRec.szECRMessage4);

	if (strlen(srTransRec.szECRMessage1) > 1)
	{
		fMsg1 = TRUE;
		inPrint(srTransRec.szECRMessage1);
	}

	if (strlen(srTransRec.szECRMessage2) > 1)
	{
		fMsg2 = TRUE;
		inPrint(srTransRec.szECRMessage2);
	}

	if (strlen(srTransRec.szECRMessage3) > 1)
	{
		fMsg3 = TRUE;
		inPrint(srTransRec.szECRMessage3);
	}

	if (strlen(srTransRec.szECRMessage4) > 1)
	{
		fMsg4 = TRUE;
		inPrint(srTransRec.szECRMessage4);
	}

	if ((fMsg1) || (fMsg2) || (fMsg3) || (fMsg4))
		vdCTOS_PrinterFline(1);
}

void vdDisplayHostInforMP200(void){
	int inResult = d_OK;
	int inCount = 1;
	BYTE key = 0;
	int inNumberofItems = 4;
	int inRet;
	int shHostIndex;

	vduiLightOn();

	//shHostIndex = inCTOS_SelectHostSetting();
    //if (shHostIndex == -1)
    //    return;

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(HOST_INFO_REPORT);

	inTCPRead(1);

	DisplayHostInfoItems(1);

	while(1)
	{
		CTOS_KBDHit(&key);
		if(key == d_KBD_CANCEL)
		{
			return;
		}
		else if((key == d_KBD_DOWN)||(key == d_KBD_DOT))
		{
			inCount++;
			DebugAddIntX("inCount", inCount);

			if(inCount > inNumberofItems)
			{
				inCount = 1;
			}

			DisplayHostInfoItems(inCount);

		}
		else if((key == d_KBD_UP)||(key == d_KBD_00))
		{
			inCount--;

			if(inCount < 1)
			{
				inCount = 4;
			}

			DisplayHostInfoItems(inCount);
		}
	}

}

void DisplayHostInfoItems(int inItem){

	BYTE szTemp1[40] = {0};
	char szStr[40 + 1];
	int i,x;
	char szTemp[40 + 1];
	int inCSCNumRecs;
	BYTE szTempBuf[12+1];
	BYTE szBuf[50];
	char szTermSerialNumber[15]; // print terminal serial number on all txn receipt - mfl

	vduiClearBelow(3);
	x=3;
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	switch(inItem){

		case 1:

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "HOST        : %s", (char *)strHDT.szHostLabel);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szTempBuf, 0x00, sizeof(szTempBuf));
			wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
			sprintf(szStr, "TPDU         : %s", szTempBuf);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szTempBuf, 0x00, sizeof(szTempBuf));
			wub_hex_2_str(strHDT.szNII,szTempBuf,2);
			sprintf(szStr, "NII          : %s", szTempBuf);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			#if 0
			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI TEL NUM : %s", (char *)strCPT.szPriTxnPhoneNumber);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC TEL NUM : %s", (char *)strCPT.szSecTxnPhoneNumber);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;
			#endif

			break;

		case 2:
			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI IP		: %s", (char *)strCPT.szPriTxnHostIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI IP PORT : %04d", strCPT.inPriTxnHostPortNum);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC IP		: %s", (char *)strCPT.szSecTxnHostIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC IP PORT : %04d", strCPT.inSecTxnHostPortNum);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			break;

		case 3:

			inMMTReadRecord(strHDT.inHostIndex,1);

			memset(szStr, 0x00, sizeof(szStr));
			sprintf(szStr, "  MERCHANT : %s", strMMT[0].szMerchantName);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, sizeof(szStr));
			sprintf(szStr, "  TID	   : %s", strMMT[0].szTID);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, sizeof(szStr));
			sprintf(szStr, "  MID	   : %s", strMMT[0].szMID);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, sizeof(szStr));
			memset(szBuf, 0x00, sizeof(szBuf));
			wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
			sprintf(szStr, "  BATCH NO : %s", szBuf);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			break;

		case 4:

	        memset(szStr, 0x00, sizeof(szStr));
	        memset(szTermSerialNumber,0x00,sizeof(szTermSerialNumber));

	        //CTOS_GetFactorySN(szTermSerialNumber);
	        usGetSerialNumber(szTermSerialNumber);
	        szTermSerialNumber[15]=0;
	        sprintf(szStr, "SERIAL NO   : %s", szTermSerialNumber);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			// Print CRC
			memset(szStr, 0x00, sizeof(szStr));
			memset(szBuf, 0x00, sizeof(szBuf));
			inCTOSS_GetEnvDB(strHDT.szAPName, szBuf); // Get CRC value by App Name -- sidumili
			sprintf(szStr, "CRC   : %s", szBuf);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

	}

   vdCTOS_LCDGShowUpDown(1,1);

   return;
}

void vdComputeGetAppCRC(char *szAppName, char *ptrCRC, BOOL fMP200)
{
	char szStr[40 + 1];
	FILE* file;
	int nLen;
	unsigned char chBuffer[1024];
	BOOL fAppExist = FALSE;

	vdDebug_LogPrintf("--vdComputeGetAppCRC--");

	memset(szStr, 0x00, sizeof(szStr));
	memset(chBuffer, 0, 1024);

	strcpy(szAppName, "V5S_MCCNH");

	if ((file = fopen (szAppName, "rb")) != NULL)
	{
		fAppExist = TRUE;

	    while (nLen = fread (chBuffer, 1, 256, file))
	    {
	    }

	    sprintf(szStr, "%02x%02x%02x%02x", wub_lrc((unsigned char *) &chBuffer[0], 64),
	    wub_lrc((unsigned char *) &chBuffer[63], 64),
	    wub_lrc((unsigned char *) &chBuffer[127], 64),
	    wub_lrc((unsigned char *) &chBuffer[191], 64));
	    fclose (file);
	}

	#if 0
	// hardcode crc
	if (isCheckTerminalMP200() == d_OK)
	{
		strcpy(szStr, "1006147D");
	}
	else
	{
		if (!strTCT.fNSR)
			strcpy(szStr, "560B4446");
	}
	#endif

	// Put CRC to ENV
	put_env_char(szAppName, szStr);

	if (fAppExist)
	{
		if (fMP200)
			sprintf(ptrCRC, "%s", szStr);
		else
			sprintf(ptrCRC, "CRC: %s", szStr);
	}
	else
		strcpy(ptrCRC, "N/A");

	vdDebug_LogPrintf("szAppName[%s] | ptrCRC[%s] | fAppExist[%d] | fMP200[%d]", szAppName, ptrCRC, fAppExist, fMP200);

}

void vdDisplayCRCForMP200(void)
{
	char szStr[40 + 1] = {0};
	int x;
    unsigned char szCRC[8+1] = {0};
	BYTE key;
	int inNumRecs = 0;
	int i = 0;
	BYTE szTempCRC[8 + 1] = {0};
	BYTE szPrintCRC[d_LINE_SIZE + 1] = {0};
	BYTE szAPName[25 + 1];

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(CRC_REPORT);

	x=3;
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	vduiClearBelow(3);

	// Print CRC
	inReadDistinctAppName(&inNumRecs);

	vdDebug_LogPrintf("inNumRecs(%d)", inNumRecs);

	for (i = 0; i < inNumRecs; i++)
	{
		memset(szTempCRC, 0x00, sizeof(szTempCRC));
		memset(szCRC, 0x00, sizeof(szCRC));
		memset(szPrintCRC, 0x00, sizeof(szPrintCRC));

		strcpy(szTempCRC, strMultiHDT[i].szAPName);
		strcat(szTempCRC, "_CRC");
		vdDebug_LogPrintf("szTempCRC[%s]", szTempCRC);

		inCTOSS_GetEnvDB(szTempCRC, szCRC);
		vdDebug_LogPrintf("szCRC[%s]", szCRC);

		if (strlen(szCRC) > 0)
		{
			memset(szAPName, 0x00, sizeof(szAPName));

			if (strcmp(strMultiHDT[i].szAPName, "V5S_BANCNET") == 0)
				strcpy(szAPName, "BANCNET");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_DINERS") == 0)
				strcpy(szAPName, "DINERS");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_DINERSINST") == 0)
				strcpy(szAPName, "SB INST");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_MCCINST") == 0)
				strcpy(szAPName, "SHOPCARD");

			if (strcmp(strMultiHDT[i].szAPName, "V5S_MCCNH") == 0)
				strcpy(szAPName, "MCC ONE APP");

			sprintf(szPrintCRC, "%16s:  %s", szAPName, szCRC);
			CTOS_LCDTPrintAligned(x, szPrintCRC, d_LCD_ALIGNLEFT);
			x++;
		}
		else
		{
			break;
		}
	}

	WaitKey(15);
}

int vdCTOS_PrintIP(void)
{
	int shHostIndex = 1, inNumOfMerchant=0;
	int inResult,inRet;
	unsigned char szCRC[8+1];
	char ucLineBuffer[d_LINE_SIZE];
	BYTE baTemp[PAPER_X_SIZE * 64];
	int i;

	char szStr[d_LINE_SIZE + 1];
	char szIPMode[d_LINE_SIZE + 1];
	char szIPHeader[d_LINE_SIZE + 1];

	vdDebug_LogPrintf("--vdCTOS_PrintIP--");

	vduiLightOn();

	shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(IP_REPORT);

	inTCPRead(1);

	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);

	inCTOS_DisplayPrintBMP();

    fRePrintFlag = FALSE;

    //print Logo
	if(strlen(strHDT.szHeaderLogoName) > 0)
		vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
	else
		vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

    vdPrintTitleCenter("IP REPORT");
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	vdCTOS_SetDateTime();

    printDateTime(FALSE);

	vdCTOS_PrinterFline(1);

	inPrint("TERMINAL");

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    vdCTOS_PrinterFline(1);

	vdDebug_LogPrintf("--TCP TABLE--");
	vdDebug_LogPrintf("fDHCPEnable[%d]", strTCP.fDHCPEnable);
	vdDebug_LogPrintf("szTerminalIP[%s]", strTCP.szTerminalIP);
	vdDebug_LogPrintf("szGetWay[%s]", strTCP.szGetWay);
	vdDebug_LogPrintf("szSubNetMask[%s]", strTCP.szSubNetMask);
	vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]", strCPT.inCommunicationMode);
	vdDebug_LogPrintf("strTCT.fShareComEnable[%d]", strTCT.fShareComEnable);


	memset(szIPMode, 0x00, sizeof(szIPMode));
	if (strTCP.fDHCPEnable == IPCONFIG_STATIC)
		strcpy(szIPMode, "STATIC");

	if (strTCP.fDHCPEnable == IPCONFIG_DHCP)
		strcpy(szIPMode, "DHCP");


  if (strCPT.inCommunicationMode == GPRS_MODE)
  {
     inCTOSS_SIMGetGPRSIPInfo();
	 //memset(szStr, 0x00, d_LINE_SIZE);
	 //sprintf(szStr, "        IP MODE: %s", szIPMode);
	 //inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "                 IP: %s", szIP);
     inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "              DNS 1: %s", szDNS1);
     inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "              DNS 2: %s", szDNS2);
		 inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "            GATEWAY: %s", szGateWay);
     inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "        SUBNET MASK: %s", szSubnetMask);
     inPrint(szStr);
		 /*AAA temp only need to change value when we have function to get data end*/
  }
  else if (strCPT.inCommunicationMode == WIFI_MODE)
  {
     inWIFI_GetConnectConfig();
	 //memset(szStr, 0x00, d_LINE_SIZE);
	 //sprintf(szStr, "        IP MODE: %s", szIPMode);
	 //inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "                 IP: %s", szIP);
     inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "              DNS 1: %s", szDNS1);
     inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "              DNS 2: %s", szDNS2);
		 inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "            GATEWAY: %s", szGateWay);
     inPrint(szStr);

     memset(szStr, 0x00, d_LINE_SIZE);
     sprintf(szStr, "        SUBNET MASK: %s", szSubnetMask);
     inPrint(szStr);

  }
  else
  {
  	if(strTCP.fDHCPEnable == 1)
  	{
	  	 vdPrint_EthernetStatus();
		 //memset(szStr, 0x00, d_LINE_SIZE);
		 //sprintf(szStr, "        IP MODE: %s", szIPMode);
		 //inPrint(szStr);

		 memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "             IP: %s", szIP);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "          DNS 1: %s", szDNS1);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "          DNS 2: 0.0.0.0");
		 inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "        GATEWAY: %s", szGateWay);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "    SUBNET MASK: %s", szSubnetMask);
	     inPrint(szStr);
  	}
	else
	{
		 //memset(szStr, 0x00, d_LINE_SIZE);
		 //sprintf(szStr, "        IP MODE: %s", szIPMode);
		 //inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "             IP: %s", (char *)strTCP.szTerminalIP);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "          DNS 1: %s", (char *)strTCP.szHostDNS1);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "          DNS 2: %s", (char *)strTCP.szHostDNS2);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "        GATEWAY: %s", (char *)strTCP.szGetWay);
	     inPrint(szStr);

	     memset(szStr, 0x00, d_LINE_SIZE);
	     sprintf(szStr, "    SUBNET MASK: %s", (char *)strTCP.szSubNetMask);
	     inPrint(szStr);
	}
  }

	vdCTOS_PrinterFline(1);

	inPrint("HOST");

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	vdCTOS_PrinterFline(1);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "           NAME: %s", (char *)strCPT.szHostName);
	inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	memset(szIPHeader, 0x00, sizeof(szIPHeader));
	sprintf(szIPHeader, "%d", strCPT.inIPHeader);
	sprintf(szStr, "      IP HEADER: %s", szIPHeader);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	if (strlen(strCPT.szPriTxnHostIP) > 25)
	     sprintf(szStr, "PRI IP: %s", (char *)strCPT.szPriTxnHostIP);
	else
	     sprintf(szStr, "         PRI IP: %s", (char *)strCPT.szPriTxnHostIP);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	if (strlen(strCPT.szSecTxnHostIP) > 25)
	     sprintf(szStr, "SEC IP: %s", (char *)strCPT.szSecTxnHostIP);
	else
		 sprintf(szStr, "         SEC IP: %s", (char *)strCPT.szSecTxnHostIP);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "    PRI IP PORT: %04d", strCPT.inPriTxnHostPortNum);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "    SEC IP PORT: %04d", strCPT.inSecTxnHostPortNum);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	if (strlen(strCPT.szPriSettlementHostIP) > 25)
	     sprintf(szStr, "PRI SET: %s", (char *)strCPT.szPriSettlementHostIP);
	else
		 sprintf(szStr, "  PRI SETTLE IP: %s", (char *)strCPT.szPriSettlementHostIP);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	if (strlen(strCPT.szSecSettlementHostIP) > 25)
	     sprintf(szStr, "SEC SET: %s", (char *)strCPT.szSecSettlementHostIP);
	else
		 sprintf(szStr, "  SEC SETTLE IP: %s", (char *)strCPT.szSecSettlementHostIP);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "PRI SETTLE PORT: %04d", strCPT.inPriSettlementHostPort);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "SEC SETTLE PORT: %04d", strCPT.inSecSettlementHostPort);
	inPrint(szStr);

	vdCTOS_PrinterFline(1);

	vdLineFeed(FALSE);

	vdCTOSS_PrinterEnd();
}

void vdDisplayIPInforMP200(void){
	int inResult = d_OK;
	int inCount = 1;
	BYTE key = 0;
	int inNumberofItems = 3;
	int inRet;
	int shHostIndex;

	vduiLightOn();

	shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(IP_REPORT);

	inTCPRead(1);

	DisplayIPInfoItems(1);

	while(1)
	{
		CTOS_KBDHit(&key);
		if(key == d_KBD_CANCEL)
		{
			return;
		}
		else if((key == d_KBD_DOWN)||(key == d_KBD_DOT))
		{
			inCount++;
			DebugAddIntX("inCount", inCount);

			if(inCount > inNumberofItems)
			{
				inCount = 1;
			}

			DisplayIPInfoItems(inCount);

		}
		else if((key == d_KBD_UP)||(key == d_KBD_00))
		{
			inCount--;

			if(inCount < 1)
			{
				inCount = 3;
			}

			DisplayIPInfoItems(inCount);
		}
	}

}

void DisplayIPInfoItems(int inItem){

	BYTE szTemp1[40] = {0};
	char szStr[40 + 1];
	int i,x;
	char szTemp[40 + 1];
	int inCSCNumRecs;
	BYTE szTempBuf[12+1];
	BYTE szBuf[50];
	char szTermSerialNumber[15]; // print terminal serial number on all txn receipt - mfl

	vduiClearBelow(3);
	x=3;
	CTOS_LCDTSelectFontSize(d_FONT_12x24);

	memset(szIP, 0x00, sizeof(szIP));
	memset(szDNS1, 0x00, sizeof(szDNS1));
	memset(szDNS2, 0x00, sizeof(szDNS2));
	memset(szGateWay, 0x00, sizeof(szGateWay));
	memset(szSubnetMask, 0x00, sizeof(szSubnetMask));

	switch(inItem){

		case 1:

			if (strCPT.inCommunicationMode == GPRS_MODE)
			{
				inCTOSS_SIMGetGPRSIPInfo();
			}
			else if (strCPT.inCommunicationMode == WIFI_MODE)
			{
			    //android-removed
				//inWIFI_GetConnectConfig();
				//end
			}
			else
			{
				if(strTCP.fDHCPEnable == IPCONFIG_DHCP)
				{
					//Print_EthernetStatus();
				}
				else
				{
					strcpy(szIP, (char *)strTCP.szTerminalIP);
					strcpy(szDNS1, (char *)strTCP.szHostDNS1);
					strcpy(szDNS2, (char *)strTCP.szHostDNS2);
					strcpy(szGateWay, (char *)strTCP.szHostDNS1);
					strcpy(szSubnetMask, (char *)strTCP.szSubNetMask);
				}
			}

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "%s", "[TERMINAL IP DETAILS]");
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI IP      :%s", (char *)szIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "DNS 1       :%s", (char *)szDNS1);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "DNS 2       :%s", (char *)szDNS2);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "GATEWAY     :%s", (char *)szGateWay);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SUBNET MASK :%s", (char *)szSubnetMask);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			break;

		case 2:

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "%s", "[HOST IP DETAILS]");
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI IP		:%s", (char *)strCPT.szPriTxnHostIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI IP PORT :%04d", strCPT.inPriTxnHostPortNum);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC IP		:%s", (char *)strCPT.szSecTxnHostIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC IP PORT :%04d", strCPT.inSecTxnHostPortNum);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

		break;

		case 3:

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "%s", "[SETTLE HOST IP DETAILS]");
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI SETTLE IP:%s", (char *)strCPT.szPriSettlementHostIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "PRI SETTLE IP PORT:%04d", strCPT.inPriSettlementHostPort);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC SETTLE IP:%s", (char *)strCPT.szSecSettlementHostIP);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

			memset(szStr, 0x00, d_LINE_SIZE);
			sprintf(szStr, "SEC SETTLE IP PORT:%04d", strCPT.inSecTxnHostPortNum);
			CTOS_LCDTPrintAligned(x, szStr, d_LCD_ALIGNLEFT);
			x++;

		break;

		// SHARLS_ERM Details
		case 4:
		break;

	}

   vdCTOS_LCDGShowUpDown(1,1);

   return;
}

// Put CRC value to ENV -- sidumili
void vdPutEnvCRC(void)
{
    unsigned char szCRC[8+1];

	memset(szCRC, 0x00, sizeof(szCRC));

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
    	vdGetCRC();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_PUT_ENV_CRC);
    }
    else
    {
        vdGetCRC();
    }
}

void vdPrintDetailReportHeader(void)
{
	char szPrintBuf[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "CARD NAME");
	strcpy(szStrRight, "CARD NUMBER");
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "EXP DATE");
	strcpy(szStrRight, "TRACE NO.");
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "TRANSACTION");
	strcpy(szStrRight, "DATE/TIME");
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "CURRENCY");
	strcpy(szStrRight, "AMOUNT");
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "APPR. CODE");
	strcpy(szStrRight, "ENTRY MODE");
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

}

void vdLogISOOption(unsigned char *pucTitle,unsigned char *uszData, int inLen)
{
	//vdDebug_LogPrintf2("============================================");

	if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
		inLogISOPacket(pucTitle , uszData, inLen-2);
	else
		inLogISOPacket(pucTitle , uszData, inLen);

	vdLogParseISO(srTransRec.byTransType,pucTitle ,uszData+5);

	//vdDebug_LogPrintf2("============================================");

}

int inLogISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE+4];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 4];

    if (inLen <= 0)
        return(ST_SUCCESS);

	//if (isCheckTerminalMP200() != d_OK)
	//	vdCTOSS_PrinterStart(100); // For fast printing

    //CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);

	//CTOS_PrinterSetWorkTime(50000,1000);
    //inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
	//vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    memset(szStr, 0x00, sizeof(szStr));
	memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(szStr,"[%s] [%d] \n", pucTitle, inLen);
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	//if (isCheckTerminalMP200() == d_OK)
	//	vdDebug_LogPrintf2(szStr);


    //CTOS_PrinterFline(d_LINE_DOT);
	  //vdCTOS_PrinterFline(1);


    pucBuff = pucMessage + inLen;
    while (pucBuff > pucMessage)
    {
    memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
    for (inBuffPtr = 0; (inBuffPtr < 44) && (pucBuff > pucMessage); inBuffPtr += 3)
    {
    sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
    pucMessage++;
    }

	//if (isCheckTerminalMP200() == d_OK)
	//	vdDebug_LogPrintf2(ucLineBuffer);

    ucLineBuffer[44] = '\n';
    memset (baTemp, 0x00, sizeof(baTemp));
    //inCCTOS_PrinterBufferOutput(ucLineBuffer, &stgFONT_ATTRIB,1);
    }
	//vdCTOS_PrinterFline(2);
    //CTOS_PrinterFline(d_LINE_DOT * 2);
   	//if (isCheckTerminalMP200() != d_OK)
    //	vdCTOSS_PrinterEnd();
    return (ST_SUCCESS);
}

void vdLineFeed(BOOL fFooterLogo)
{
    int inLineFeed;

/*
	if(strTCT.byTerminalModel == V3_DESKTOP)
        inLineFeed=((fFooterLogo)?6:7);
    else if(strTCT.byTerminalModel == V3_MOBILE)
		inLineFeed=((fFooterLogo)?2:3);
	else
		inLineFeed=((fFooterLogo)?4:5);//for V5
*/
    inLineFeed=((fFooterLogo)?3:4);
	vdCTOS_PrinterFline(inLineFeed);
}

int inCTOS_DisplayPrintBMP(void)
{
	vdDebug_LogPrintf("--inCTOS_DisplayPrintBMP--");
	vdDebug_LogPrintf("fImageAnimated[%d]", strTCT.fImageAnimated);
	vdDebug_LogPrintf("byTerminalType[%d]", strTCT.byTerminalType);

	if (strTCT.fImageAnimated == TRUE)
	{
		if ((strTCT.byTerminalType % 2) == 0)
			vdDisplayAnimateBmp(0, 0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
		else
			vdDisplayAnimateBmp(0, 0, "Print4.bmp", "Print5.bmp", "Print6.bmp", NULL, NULL);
	}
	else
	{
		if ((strTCT.byTerminalType % 2) == 0)
			displayAppbmpDataEx(0, 0, "Print1.bmp");
		else
			displayAppbmpDataEx(0, 0, "Print4.bmp");
	}

}

void vdPrintCRC(void)
{
	BYTE szCRC[50];
	//BYTE szStr[d_LINE_SIZE + 1];
	int inNumOfHost = 0;
	int inNum = 0;
	BYTE szAPName[50];
	BYTE szPrintCRC[50] = {0};
	BOOL fQRPrnt = FALSE;

	vdDebug_LogPrintf("--vdPrintCRC--");
	//memset(szCRC, 0x00, sizeof(szCRC));
	//memset(szStr, 0x00, d_LINE_SIZE);

	vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(1000);

	#if 0
	//inCTOSS_GetEnvDB("V5S_MCCNH_CRC", szCRC);
	//TINE:  android
	getAppPackageInfo("com.Source.S1_MCC.MCC", szCRC);
	vdDebug_LogPrintf("szCRC[%s]", szCRC);
	
	sprintf(szStr, "CRC         : %s", szCRC);
	inPrint(szStr);
	#else

	
	inNumOfHost = inHDTNumRecord();
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			memset(szAPName, 0x00, sizeof(szAPName));
			//memset(szCRC, 0x00, sizeof(szCRC));
			if (strcmp("com.Source.S1_RBANK.RBANK", strHDT.szAPName)==0)
			{				
				strcpy(szAPName, "MCC ONE APP ");
                memset(szCRC, 0x00, sizeof(szCRC));
                strcpy(szCRC, "636AE8548");

				getAppPackageInfo(strHDT.szAPName, szCRC);
				
                memset(szPrintCRC, 0x00, sizeof(szPrintCRC));
                sprintf(szPrintCRC, "%s: %s", szAPName, szCRC);
                inPrint(szPrintCRC);
			}

			if (strcmp("com.Source.S1_BANCNET.BANCNET", strHDT.szAPName)==0)
			{				
				strcpy(szAPName, "BANCNET     ");
				memset(szCRC, 0x00, sizeof(szCRC));
                strcpy(szCRC, "2139D3B09");
				
				getAppPackageInfo(strHDT.szAPName, szCRC);
				
                memset(szPrintCRC, 0x00, sizeof(szPrintCRC));
                sprintf(szPrintCRC, "%s: %s", szAPName, szCRC);
                inPrint(szPrintCRC);
				//strcpy(szCRC, "728933D95");
			}

			if (strcmp("com.Source.S1_MCCPAY.MCCPAY", strHDT.szAPName)==0 && !fQRPrnt)
			{				
				strcpy(szAPName, "QR PAY      ");
				fQRPrnt  = TRUE;
				memset(szCRC, 0x00, sizeof(szCRC));
                strcpy(szCRC, "0065A19CD");
				//getAppPackageInfo(strHDT.szAPName, szCRC);
                memset(szPrintCRC, 0x00, sizeof(szPrintCRC));
                sprintf(szPrintCRC, "%s: %s", szAPName, szCRC);
                inPrint(szPrintCRC);
			}

			//if ((strlen(szAPName) > 0) && (strcmp("com.Source.S1_MCC.MCC", strHDT.szAPName)!=0))
			/*if (strlen(szAPName) > 0)
			{
                 memset(szCRC, 0x00, sizeof(szCRC));
                 getAppPackageInfo(strHDT.szAPName, szCRC);
               memset(szPrintCRC, 0x00, sizeof(szPrintCRC));
                 sprintf(szPrintCRC, "%s: %s", szAPName, szCRC);
                 inPrint(szPrintCRC);
			}*/	
		}
	}
	
	#endif	
}



void vdPrintFSRMode(void)
{
	vdDebug_LogPrintf("--vdPrintFSRMode--");

	if (fFSRMode()== TRUE)
		inPrint("MODE: FSR");
}


USHORT ushCTOS_DCCPrintBody(int page)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr2[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    short spacestring;
    BYTE   key;
    char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
    char szLocalTotalAmountformat[20+1];
    BYTE  szDCCFormatTipAmount[20+1];
    BYTE  szDCCFormatTipAmount3MinorUnits[20+1];
    BYTE  szDCCFormatBaseAmount[20+1];
    BYTE  szDCCFormatTotalAmount[20+1];
    BYTE szAmtBuf[20+1];
    BYTE szAmtBuf2[20+1];
    char szAmountBuff[20];
    BYTE  szLocalFormatTotalAmount[20+1];
    BYTE  szLocalFormatTipAmount[20+1];
    BYTE  szLocalFormatAmount[20+1];


    // fix for Wrong implementation of USD Currency
    //USD currency prompts upon voiding & settlement
    //Terminal prints incorrect amount currency on duplicate receipt (PHP Profile only)
    // #00166
    inTCTRead(1);
    if(strTCT.fRegUSD == 1)
        inCSTRead(2);
    else
        inCSTRead(1);


    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    if(CUSTOMER_COPY_RECEIPT == page)
    {
        if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
            vdPrintCenter("***CONTACTLESS***");

        szGetTransTitle(srTransRec.byTransType, szStr);
        if(srTransRec.byTransType==VOID && srTransRec.byOffline ==1 && srTransRec.byUploaded == 0)
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            inPrint("VOID OFFLINE");
        }
        else
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            inPrint(szStr);
        }
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        printDateTime(TRUE);
        printTIDMID();
        printBatchInvoiceNO();
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

        memset(szStr, ' ', d_LINE_SIZE);
        //if(srTransRec.fDebit == TRUE && strcmp(srTransRec.szCardLable, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
        if(srTransRec.fDebit == TRUE)
            strcpy(szStr,"MCC DEBIT");
        else
            sprintf(szStr, "%s", srTransRec.szCardLable);

        memset (baTemp, 0x00, sizeof(baTemp));
        vdCTOS_FormatPANEx(strIIT.szMaskCustomerCopy, srTransRec.szPAN, baTemp, strIIT.fMaskCustCopy);

        inPrintLeftRight(szStr, baTemp, 46);


        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        printCardHolderNameEntryMode();

        //Reference num
        if(srTransRec.fDebit != TRUE)
        {
            memset(szStr, ' ', d_LINE_SIZE);
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szStr, ' ', d_LINE_SIZE);
            sprintf(szStr, "REF NO.   : %s", srTransRec.szRRN);
            inPrintLeftRight(szStr, " ", 46);
        }

        //Auth response code
        // #00158 No Account type printed on DEBIT receipt
        //if(strCDT.inType == DEBIT_CARD)
        //if(srTransRec.fDebit == TRUE && strCDT.inType == DEBIT_CARD)
        if(srTransRec.fDebit == TRUE)
        {
            char szTemp[100+1];
            char szAccntType[40+1];
            memset(szTemp, ' ', sizeof(szTemp));
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szAccntType, ' ', sizeof(szAccntType));

            if(srTransRec.inAccountType == CURRENT_ACCOUNT)
                strcpy(szAccntType, "CURRENT");
            else if(srTransRec.inAccountType == SAVINGS_ACCOUNT)
                strcpy(szAccntType, "SAVINGS");

            sprintf(szTemp, "APPR. CODE: %s   ACCNT TYPE:  %s", srTransRec.szAuthCode, szAccntType);
            inPrint(szTemp);
        }
        //else
        //{
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        memset(szStr2, ' ', d_LINE_SIZE);
        sprintf(szStr2, "APPR. CODE: %s", srTransRec.szAuthCode);
        inPrintLeftRight(" ", szStr2, 24);
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        //}

        //Terminal serial number - mfl
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);
        usGetSerialNumber(szTermSerialNum);
        szTermSerialNum[15]=0;

        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

        vdCTOS_PrinterFline(1); // space after printing trans title.

        vdPrintECRMessage(); // Mercury Requirements -- sidumili

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);
        memset(szTemp1, ' ', d_LINE_SIZE);
        memset(szTemp3, ' ', d_LINE_SIZE);
        memset(szAmtBuf, ' ', d_LINE_SIZE);
        memset(szLocalTotalAmountformat,0,sizeof(szLocalTotalAmountformat));
        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
        wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

        if(srTransRec.byVoided == TRUE)
        {
            szLocalTotalAmountformat[0] = '-';
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, &szLocalTotalAmountformat[1]);
            sprintf(szStr,"%s %s", strCST.szCurSymbol, szLocalTotalAmountformat);
        }
        else
        {
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szLocalTotalAmountformat);
            sprintf(szStr,"%s %s", strCST.szCurSymbol, szLocalTotalAmountformat);
        }
        inPrintLeftRight("TRANSACTION AMOUNT:", szStr, 46);

        vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        memset(szStr, ' ', d_LINE_SIZE);
        //sprintf(szStr,"%s", gblszExchangeRate);
       	if((srTransRec.IITid == VISA_ISSUER) || (srTransRec.IITid == MASTERCARD_ISSUER))
        {
             sprintf(szStr,"1 %s  = %s %s", srTransRec.szDCCCurrencySymbol, srTransRec.szForeignRate, strCST.szCurSymbol);
             inPrintLeftRight("EX RATE:", szStr, 46);
        }
		else
        {
             sprintf(szStr,"%s", srTransRec.szExchangeRate);
             inPrintLeftRight("FX RATE:", szStr, 46);
        }

        //vdCTOS_PrinterFline(1);

        memset(szStr, ' ', d_LINE_SIZE);
        //sprintf(szStr,"%s", gblszMarginRatePercentage);
        //sprintf(szStr,"%s", srTransRec.szMarginRatePercentage);
        //inPrintLeftRight("MARGIN:", szStr, 46);
        inPrint(srTransRec.szFXSourceName);
        sprintf(szStr,"includes %s%% margin", srTransRec.szMarginRatePercentage);
        inPrint(szStr);

        vdCTOS_PrinterFline(1);

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szStr2, ' ', d_LINE_SIZE);
        if (srTransRec.fDCCOptin==FALSE)
        {
            sprintf(szStr,"[X] %s AMOUNT", strCST.szCurSymbol);
            sprintf(szStr2,"[  ] %s AMOUNT", srTransRec.szDCCCurrencySymbol);
        }
        else
        {
            sprintf(szStr,"[  ] %s AMOUNT", strCST.szCurSymbol);
            sprintf(szStr2,"[X] %s AMOUNT", srTransRec.szDCCCurrencySymbol);
        }
        inPrintLeftRight(szStr, szStr2, 46);

        // vdCTOS_PrinterFline(1);
        memset(szLocalTotalAmountformat,0,sizeof(szLocalTotalAmountformat));
        memset(szDCCFormatBaseAmount,0,sizeof(szDCCFormatBaseAmount));
        memset(szAmtBuf2,0,sizeof(szAmtBuf2));
        if(srTransRec.byVoided == TRUE)
        {
            szLocalTotalAmountformat[0] = '-';
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, &szLocalTotalAmountformat[1]);

            if(srTransRec.inMinorUnit==0)
            {
                szAmtBuf2[0] = '-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, szDCCFormatBaseAmount);
                purgeleadingchar('0',szDCCFormatBaseAmount, &szAmtBuf2[1]);
            }
            else
            {
                szDCCFormatBaseAmount[0] = '-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, &szDCCFormatBaseAmount[1]);
            }
        }
        else
        {
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, szLocalTotalAmountformat);
            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, szDCCFormatBaseAmount);
            if(srTransRec.inMinorUnit==0)
                purgeleadingchar('0',szDCCFormatBaseAmount, szAmtBuf2);
        }

        memset(szStr, ' ', d_LINE_SIZE);
        sprintf(szStr,"AMT: %s", szLocalTotalAmountformat);
        memset(szStr2, ' ', d_LINE_SIZE);
        if(srTransRec.inMinorUnit==0)
            sprintf(szStr2,"AMT: %s", szAmtBuf2);
        else
            sprintf(szStr2,"AMT: %s", szDCCFormatBaseAmount);
        inPrintLeftRight(szStr, szStr2, 46);

        //vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        if ((srTransRec.byTransType == SALE || srTransRec.byTransType == VOID ||srTransRec.byTransType == SALE_OFFLINE)&&(srTransRec.byPackType != VOID_REFUND))
        {
            if(srTransRec.fInstallment || srTransRec.fCash2Go)
            {
                ushCTOS_PrintInstallmentDetails();
            }
            else
            {
                //Tip amount
                memset(szStr, ' ', d_LINE_SIZE);
                sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
                memset (baTemp, 0x00, sizeof(baTemp));
                memset (szTemp4, 0x00, sizeof(szTemp4));
                wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);

                DebugAddSTR("TIP:",szTemp4,12);

                //#00157 Debit transaction has tipping amount on receipt.
                if (TRUE ==strHDT.fTipAllowFlag && srTransRec.fDebit != TRUE
                    && srTransRec.byOrgTransType != PRE_COMP
                    && srTransRec.byOrgTransType != CASH_ADV)
                {
                    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                    if (srTransRec.fDCCOptin==TRUE)
                    {
                        if(atol(szTemp4) > 0)
                        {
                            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

                            memset(szAmtBuf,0,sizeof(szAmtBuf));
                            memset(szDCCFormatTipAmount,0,sizeof(szDCCFormatTipAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {

                                szAmtBuf[0] ='-';
                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp4, &szAmtBuf[1]);

                                if(srTransRec.inMinorUnit==3)
                                {
                                    szDCCFormatTipAmount[0] = '-';
                                    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, &szDCCFormatTipAmount[1]);
                                    strcpy(szDCCFormatTipAmount3MinorUnits, szDCCFormatTipAmount);
                                    strcat(szDCCFormatTipAmount3MinorUnits, "0");
                                }
                                else if (srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] ='-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    purgeleadingchar('0',szDCCFormatTipAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTipAmount[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, &szDCCFormatTipAmount[1]);
                                }

                            }
                            else
                            {

                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp4, szAmtBuf);

                                if(srTransRec.inMinorUnit==3)

                                {
                                    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    strcpy(szDCCFormatTipAmount3MinorUnits, szDCCFormatTipAmount);
                                    strcat(szDCCFormatTipAmount3MinorUnits, "0");
                                }
                                else if (srTransRec.inMinorUnit==0)
                                {
                                    //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    purgeleadingchar('0',szDCCFormatTipAmount,szAmtBuf2);
                                }
                                else
                                {
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                }

                            }
                            memset(szStr, ' ', d_LINE_SIZE);
                            memset(szStr2, ' ', d_LINE_SIZE);
                            sprintf(szStr,"TIP: %s", szAmtBuf);
                            if(srTransRec.inMinorUnit==3)
                                sprintf(szStr2,"TIP: %s", szDCCFormatTipAmount3MinorUnits);
                            else if(srTransRec.inMinorUnit==0)
                            {
                                 if(strlen(szAmtBuf2)==0)
                                     sprintf(szStr2,"TIP: %c", '0');
                                 else
                                      sprintf(szStr2,"TIP: %s", szAmtBuf2);
                            }
                            else
                                sprintf(szStr2,"TIP: %s", szDCCFormatTipAmount);

                            inPrintLeftRight(szStr, szStr2, 46);

                            memset(szAmtBuf,0,sizeof(szAmtBuf));
                            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {

                                szAmtBuf[0] = '-';
                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, &szAmtBuf[1]);

                                if(srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTotalAmount[0]='-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                                }

                            }
                            else
                            {
                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szAmtBuf);

                                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                if(srTransRec.inMinorUnit==0)
                                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);

                            }
                            memset(szStr, ' ', d_LINE_SIZE);
                            memset(szStr2, ' ', d_LINE_SIZE);
                            sprintf(szStr,"TOT: %s", szAmtBuf);
                            if(srTransRec.inMinorUnit==0)
                                sprintf(szStr2,"TOT: %s", szAmtBuf2);
                            else
                                sprintf(szStr2,"TOT: %s", szDCCFormatTotalAmount);
                            inPrintLeftRight(szStr, szStr2, 46);


                            vdCTOS_PrinterFline(1);
                            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {
                                if(srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTotalAmount[0]='-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                                }
                            }
                            else
                            {
                                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                if(srTransRec.inMinorUnit==0)
                                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                            }
                            memset(szStr, ' ', d_LINE_SIZE);
                            if(srTransRec.inMinorUnit==0)
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                            else
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
                            inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
                            inPrintLeftRight("AMOUNT:", szStr, 46);


                        }
                        else
                        {
                            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
                            vdCTOS_PrinterFline(1);
                            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {
                                if(srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTotalAmount[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                                }
                            }
                            else
                            {
                                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                if(srTransRec.inMinorUnit==0)
                                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                            }

                            if(srTransRec.inMinorUnit==0)
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                            else
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);

                            inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
                            inPrintLeftRight("AMOUNT:", szStr, 46);
                        }
                    }
                }
               else
               {
                    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
                    vdCTOS_PrinterFline(1);
                    memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                    memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                    if(srTransRec.byVoided == TRUE)
                    {
                         if(srTransRec.inMinorUnit==0)
                         {
                              szAmtBuf2[0] = '-';
                              vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                              purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                         }
                         else
                         {
                              szDCCFormatTotalAmount[0] = '-';
                              vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                         }
                    }
                    else
                    {
                         vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                         if(srTransRec.inMinorUnit==0)
                         purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                    }
                    
                    if(srTransRec.inMinorUnit==0)
                         sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                    else
                         sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
                    
                    inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
                    inPrintLeftRight("AMOUNT:", szStr, 46);
               }
            }

        }
        else if (srTransRec.byTransType == SALE_TIP)
        {
            memset(szTemp3, ' ', d_LINE_SIZE);
            wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            //vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
            // vdPrintFormattedAmount("TIPS  :", szTemp3, 24);
            // vdPrintFormattedAmount("TOTAL :", szTemp, 24);
            memset(szLocalFormatAmount,0,sizeof(szLocalFormatAmount));
            vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp1, szLocalFormatAmount);
            sprintf(szAmountBuff,"%s %s",strCST.szCurSymbol,szLocalFormatAmount);
            inPrintLeftRight("AMOUNT:",szAmountBuff,24);

            memset(szLocalFormatTipAmount,0,sizeof(szLocalFormatTipAmount));
            vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp3, szLocalFormatTipAmount);
            sprintf(szAmountBuff,"%s %s",strCST.szCurSymbol,szLocalFormatTipAmount);
            inPrintLeftRight("TIP:",szAmountBuff,24);

            memset(szLocalFormatTotalAmount,0,sizeof(szLocalFormatTotalAmount));
            vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp, szLocalFormatTotalAmount);
            sprintf(szAmountBuff,"%s %s",strCST.szCurSymbol,szLocalFormatTotalAmount);
            inPrintLeftRight("TOTAL:",szAmountBuff,24);
        }
        else if (((srTransRec.byTransType == REFUND) || (srTransRec.byTransType == PREAUTH_VER) || (srTransRec.byTransType == PRE_AUTH)) && (srTransRec.fDCCOptin==TRUE))
        {
            vdCTOS_PrinterFline(1);
            memset(szStr, ' ', d_LINE_SIZE);
            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
            memset(szAmtBuf2,0,sizeof(szAmtBuf2));

            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
            if(srTransRec.inMinorUnit==0)
                purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);

            memset(szStr, ' ', d_LINE_SIZE);
            if(srTransRec.inMinorUnit==0)
                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
            else
                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
            inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
            inPrintLeftRight("AMOUNT:", szStr, 46);
        }
        else
        {
            vdCTOS_PrinterFline(1);
            memset(szStr, ' ', d_LINE_SIZE);
            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
            if (srTransRec.byVoided == TRUE)
            {
                if(srTransRec.inMinorUnit==0)
                {
                    szAmtBuf2[0] = '-';
                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                }
                else
                {
                    szDCCFormatTotalAmount[0] = '-';
                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
                }

            }
            else
            {
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                if(srTransRec.inMinorUnit==0)
                {
                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                }
                else
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
            }

            inPrintLeftRight("TOTAL TRANSACTION"," ", 24);
            inPrintLeftRight("AMOUNT:", szStr, 46);

        }

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);

        if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
            /* EMV: Revised EMV details printing - start -- jzg */
            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
            (srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
            (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
            (srTransRec.bWaveSID == d_EMVCL_SID_DISCOVER_DPAS) ||
            (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == 0x63) ||
            (srTransRec.bWaveSID == 0x65))
        {
            vdCTOS_PrinterFline(1); // space after printing trans title.

            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

            //AID
            memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
            memset(szTemp, ' ', d_LINE_SIZE);
            wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
            sprintf(szStr, "AID: %s",szTemp);
            memset(baTemp, ' ', d_LINE_SIZE);
            sprintf(baTemp, "APP: %s", srTransRec.stEMVinfo.szChipLabel);
            inPrintLeftRight(szStr, baTemp, 46);

            //if(srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND && srTransRec.byTransType != SALE_OFFLINE  && srTransRec.byOrgTransType != SALE_OFFLINE)
            if((srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND  && srTransRec.byTransType != SALE_OFFLINE  && srTransRec.byOrgTransType != SALE_OFFLINE)
               && (srTransRec.byUploaded==TRUE && srTransRec.byOffline==FALSE))
            {
                //AC
                memset(szStr, ' ', d_LINE_SIZE);
                wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
                sprintf(szStr, "TC: %s", szTemp);

                //TVR
                EMVtagLen = 5;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
                memset(baTemp, ' ', d_LINE_SIZE);
                sprintf(baTemp, "TVR: %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
                inPrintLeftRight(szStr, baTemp, 46);
            }
        }

        //vdCTOSS_PrinterBMPPic(0, 0, "dccoption.bmp");

        ushCTOS_DCCPrintAgreement();

    }
    else if(MERCHANT_COPY_RECEIPT == page || BANK_COPY_RECEIPT== page)
    {
        if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
            vdPrintCenter("***CONTACTLESS***");

        // - print adjustment 07132015
        szGetTransTitle(srTransRec.byTransType, szStr);
        if(srTransRec.byTransType==VOID && srTransRec.byOffline ==1 && srTransRec.byUploaded == 0)
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            inPrint("VOID OFFLINE");
        }
        else
        {
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            inPrint(szStr);
        };
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        // - print adjustment 07132015

        printDateTime(TRUE);
        inResult = printTIDMID();

        printBatchInvoiceNO(); // pat confirm hang

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

        // print CARD LABEL AND CARD NUMBER
        memset(szStr, ' ', d_LINE_SIZE);
        //if(srTransRec.fDebit == TRUE && strcmp(srTransRec.szCardLable, "BANCNET") == 0) //#00232 - When card process as DEBIT SALE terminal prints BANCNET instead of MCC DEBIT
        if(srTransRec.fDebit == TRUE)
            strcpy(szStr,"MCC DEBIT");
        else
            sprintf(szStr, "%s", srTransRec.szCardLable);

        memset (baTemp, 0x00, sizeof(baTemp));
        vdCTOS_FormatPANEx(strIIT.szMaskMerchantCopy, srTransRec.szPAN, baTemp, strIIT.fMaskMerchCopy);

        inPrintLeftRight(szStr, baTemp, 46);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        printCardHolderNameEntryMode();

        //Reference num
        if(srTransRec.fDebit != TRUE)
        {
            memset(szStr, ' ', d_LINE_SIZE);
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szStr, ' ', d_LINE_SIZE);
            sprintf(szStr, "REF NO.   : %s", srTransRec.szRRN);
            inPrintLeftRight(szStr, " ", 46);
        }

        //Auth response code
        // #00158 No Account type printed on DEBIT receipt
        //if(srTransRec.fDebit == TRUE && strCDT.inType == DEBIT_CARD)
        if(srTransRec.fDebit == TRUE)
        {
            char szTemp[100+1];
            char szAccntType[40+1];
            memset(szTemp, ' ', sizeof(szTemp));
            memset (baTemp, 0x00, sizeof(baTemp));
            memset(szAccntType, ' ', sizeof(szAccntType));

            if(srTransRec.inAccountType == CURRENT_ACCOUNT)
                strcpy(szAccntType, "CURRENT");
            else if(srTransRec.inAccountType == SAVINGS_ACCOUNT)
                strcpy(szAccntType, "SAVINGS");

            sprintf(szTemp, "APPR. CODE: %s   ACCNT TYPE:  %s", srTransRec.szAuthCode, szAccntType);
            inPrint(szTemp);
        }
        //else
        //{
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        memset(szStr2, ' ', d_LINE_SIZE);
        sprintf(szStr2, "APPR. CODE: %s", srTransRec.szAuthCode);
        inPrintLeftRight(" ", szStr2, 24);
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        // }

        //Terminal serial number - mfl
        memset(szTemp, ' ', d_LINE_SIZE);
        memset (baTemp, 0x00, sizeof(baTemp));
        memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
        //CTOS_GetFactorySN(szTermSerialNum);
        usGetSerialNumber(szTermSerialNum);
        szTermSerialNum[15]=0;
        sprintf(szStr, "TSN: %s", szTermSerialNum);
        inPrint(szStr);

        vdCTOS_PrinterFline(1); // space after printing trans title.

        vdPrintECRMessage(); // Mercury Requirements -- sidumili

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);
        memset(szTemp1, ' ', d_LINE_SIZE);
        memset(szTemp3, ' ', d_LINE_SIZE);
        memset(szAmtBuf, ' ', d_LINE_SIZE);
        memset(szLocalTotalAmountformat,0,sizeof(szLocalTotalAmountformat));
        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
        wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

        if(srTransRec.byVoided == TRUE)
        {
            szLocalTotalAmountformat[0] = '-';
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, &szLocalTotalAmountformat[1]);
            sprintf(szStr,"%s %s", strCST.szCurSymbol, szLocalTotalAmountformat);
        }
        else
        {
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szLocalTotalAmountformat);
            sprintf(szStr,"%s %s", strCST.szCurSymbol, szLocalTotalAmountformat);
        }
        inPrintLeftRight("TRANSACTION AMOUNT:", szStr, 46);
        vdCTOS_PrinterFline(1);

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        memset(szStr, ' ', d_LINE_SIZE);
        //sprintf(szStr,"%s", gblszExchangeRate);
         if((srTransRec.IITid == VISA_ISSUER) || (srTransRec.IITid == MASTERCARD_ISSUER))
        {
             sprintf(szStr,"1 %s  = %s %s", srTransRec.szDCCCurrencySymbol, srTransRec.szForeignRate, strCST.szCurSymbol);
             inPrintLeftRight("EX RATE:", szStr, 46);
        }
		else
        {
             sprintf(szStr,"%s", srTransRec.szExchangeRate);
             inPrintLeftRight("FX RATE:", szStr, 46);
        }

        //vdCTOS_PrinterFline(1);

        memset(szStr, ' ', d_LINE_SIZE);
        //sprintf(szStr,"%s", gblszMarginRatePercentage);
        //sprintf(szStr,"%s", srTransRec.szMarginRatePercentage);
        //inPrintLeftRight("MARGIN:", szStr, 46);
        inPrint(srTransRec.szFXSourceName);
        sprintf(szStr,"includes %s%% margin", srTransRec.szMarginRatePercentage);
        inPrint(szStr);

        vdCTOS_PrinterFline(1);

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szStr2, ' ', d_LINE_SIZE);
        if (srTransRec.fDCCOptin==FALSE)
        {
            sprintf(szStr,"[X] %s AMOUNT", strCST.szCurSymbol);
            sprintf(szStr2,"[  ] %s AMOUNT", srTransRec.szDCCCurrencySymbol);
        }
        else
        {
            sprintf(szStr,"[  ] %s AMOUNT", strCST.szCurSymbol);
            sprintf(szStr2,"[X] %s AMOUNT", srTransRec.szDCCCurrencySymbol);
        }
        inPrintLeftRight(szStr, szStr2, 46);

        memset(szLocalTotalAmountformat,0,sizeof(szLocalTotalAmountformat));
        memset(szDCCFormatBaseAmount,0,sizeof(szDCCFormatBaseAmount));
        memset(szAmtBuf2,0,sizeof(szAmtBuf2));
        if(srTransRec.byVoided == TRUE)
        {
            szLocalTotalAmountformat[0] = '-';
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, &szLocalTotalAmountformat[1]);
            if(srTransRec.inMinorUnit==0)
            {
                szAmtBuf2[0] = '-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, szDCCFormatBaseAmount);
                purgeleadingchar('0',szDCCFormatBaseAmount,&szAmtBuf2[1]);
            }
            else
            {
                szDCCFormatBaseAmount[0]='-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, &szDCCFormatBaseAmount[1]);
            }
        }
        else
        {
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, szLocalTotalAmountformat);
            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, szDCCFormatBaseAmount);
            if(srTransRec.inMinorUnit==0)
                purgeleadingchar('0',szDCCFormatBaseAmount,szAmtBuf2);
        }
        memset(szStr, ' ', d_LINE_SIZE);
        sprintf(szStr,"AMT: %s", szLocalTotalAmountformat);
        memset(szStr2, ' ', d_LINE_SIZE);
        if(srTransRec.inMinorUnit==0)
            sprintf(szStr2,"AMT: %s", szAmtBuf2);
        else
            sprintf(szStr2,"AMT: %s", szDCCFormatBaseAmount);
        inPrintLeftRight(szStr, szStr2, 46);

        //vdCTOS_PrinterFline(1);

        //vdCTOSS_PrinterBMPPic(0, 0, "dccoption.bmp");

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

        if ((srTransRec.byTransType == SALE || srTransRec.byTransType == VOID ||srTransRec.byTransType == SALE_OFFLINE)&&(srTransRec.byPackType != VOID_REFUND))
        {
            if(srTransRec.fInstallment || srTransRec.fCash2Go)
            {
                ushCTOS_PrintInstallmentDetails();
            }
            else
            {
                //Tip amount
                memset(szStr, ' ', d_LINE_SIZE);
                sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
                memset (baTemp, 0x00, sizeof(baTemp));
                memset (szTemp4, 0x00, sizeof(szTemp4));
                wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);

                DebugAddSTR("TIP:",szTemp4,12);

                //#00157 Debit transaction has tipping amount on receipt.
                if (TRUE ==strHDT.fTipAllowFlag && srTransRec.fDebit != TRUE
                    && srTransRec.byOrgTransType != PRE_COMP
                    && srTransRec.byOrgTransType != CASH_ADV)
                {
                    if (srTransRec.fDCCOptin==TRUE)
                    {
                        if(atol(szTemp4) > 0)
                        {
                            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

                            memset(szAmtBuf,0,sizeof(szAmtBuf));
                            memset(szDCCFormatTipAmount,0,sizeof(szDCCFormatTipAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {

                                szAmtBuf[0] ='-';
                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp4, &szAmtBuf[1]);

                                if(srTransRec.inMinorUnit==3)
                                {
                                    szDCCFormatTipAmount[0] = '-';
                                    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, &szDCCFormatTipAmount[1]);
                                    strcpy(szDCCFormatTipAmount3MinorUnits, szDCCFormatTipAmount);
                                    strcat(szDCCFormatTipAmount3MinorUnits, "0");
                                }
                                else if (srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] ='-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    purgeleadingchar('0',szDCCFormatTipAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTipAmount[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, &szDCCFormatTipAmount[1]);
                                }

                            }
                            else
                            {

                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp4, szAmtBuf);

                                if(srTransRec.inMinorUnit==3)

                                {
                                    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    strcpy(szDCCFormatTipAmount3MinorUnits, szDCCFormatTipAmount);
                                    strcat(szDCCFormatTipAmount3MinorUnits, "0");
                                }
                                else if (srTransRec.inMinorUnit==0)
                                {
                                    //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                    purgeleadingchar('0',szDCCFormatTipAmount,szAmtBuf2);
                                }
                                else
                                {
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                                }

                            }
                            memset(szStr, ' ', d_LINE_SIZE);
                            memset(szStr2, ' ', d_LINE_SIZE);
                            sprintf(szStr,"TIP: %s", szAmtBuf);
                            if(srTransRec.inMinorUnit==3)

                                sprintf(szStr2,"TIP: %s", szDCCFormatTipAmount3MinorUnits);
                            else if(srTransRec.inMinorUnit==0)
                            {
                                 if(strlen(szAmtBuf2)==0)
                                      sprintf(szStr2,"TIP: %c", '0');
                                 else
                                      sprintf(szStr2,"TIP: %s", szAmtBuf2);
                            }
                            else
                                sprintf(szStr2,"TIP: %s", szDCCFormatTipAmount);

                            inPrintLeftRight(szStr, szStr2, 46);

                            memset(szAmtBuf,0,sizeof(szAmtBuf));
                            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {

                                szAmtBuf[0] = '-';
                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, &szAmtBuf[1]);

                                if(srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTotalAmount[0]='-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                                }

                            }
                            else
                            {
                                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szAmtBuf);

                                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                if(srTransRec.inMinorUnit==0)
                                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);

                            }
                            memset(szStr, ' ', d_LINE_SIZE);
                            memset(szStr2, ' ', d_LINE_SIZE);
                            sprintf(szStr,"TOT: %s", szAmtBuf);
                            if(srTransRec.inMinorUnit==0)
                                sprintf(szStr2,"TOT: %s", szAmtBuf2);
                            else
                                sprintf(szStr2,"TOT: %s", szDCCFormatTotalAmount);
                            inPrintLeftRight(szStr, szStr2, 46);


                            vdCTOS_PrinterFline(1);
                            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {
                                if(srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTotalAmount[0]='-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                                }
                            }
                            else
                            {
                                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                if(srTransRec.inMinorUnit==0)
                                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                            }
                            memset(szStr, ' ', d_LINE_SIZE);
                            if(srTransRec.inMinorUnit==0)
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                            else
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
                            inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
                            inPrintLeftRight("AMOUNT:", szStr, 46);


                        }
                        else
                        {
                            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
                            vdCTOS_PrinterFline(1);
                            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                            if(srTransRec.byVoided == TRUE)
                            {
                                if(srTransRec.inMinorUnit==0)
                                {
                                    szAmtBuf2[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                                }
                                else
                                {
                                    szDCCFormatTotalAmount[0] = '-';
                                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                                }
                            }
                            else
                            {
                                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                                if(srTransRec.inMinorUnit==0)
                                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                            }

                            if(srTransRec.inMinorUnit==0)
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                            else
                                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);

                            inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
                            inPrintLeftRight("AMOUNT:", szStr, 46);
                        }
                    }
                }
               else
               {
                    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
                    vdCTOS_PrinterFline(1);
                    memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
                    memset(szAmtBuf2,0,sizeof(szAmtBuf2));
                    if(srTransRec.byVoided == TRUE)
                    {
                         if(srTransRec.inMinorUnit==0)
                         {
                              szAmtBuf2[0] = '-';
                              vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                              purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                         }
                         else
                         {
                              szDCCFormatTotalAmount[0] = '-';
                              vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                         }
                    }
                    else
                    {
                         vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                          if(srTransRec.inMinorUnit==0)
                              purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                    }
                    
                    if(srTransRec.inMinorUnit==0)
                         sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                    else
                         sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
                    
                    inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
                    inPrintLeftRight("AMOUNT:", szStr, 46);
               }

            }

        }
        else if (srTransRec.byTransType == SALE_TIP)
        {
            memset(szTemp3, ' ', d_LINE_SIZE);
            wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            //vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
            //vdPrintFormattedAmount("TIPS  :", szTemp3, 24);
            //vdPrintFormattedAmount("TOTAL :", szTemp, 24);
            memset(szLocalFormatAmount,0,sizeof(szLocalFormatAmount));
            vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp1, szLocalFormatAmount);
            sprintf(szAmountBuff,"%s %s",strCST.szCurSymbol,szLocalFormatAmount);
            inPrintLeftRight("AMOUNT:",szAmountBuff,24);

            memset(szLocalFormatTipAmount,0,sizeof(szLocalFormatTipAmount));
            vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp3, szLocalFormatTipAmount);
            sprintf(szAmountBuff,"%s %s",strCST.szCurSymbol,szLocalFormatTipAmount);
            inPrintLeftRight("TIP:",szAmountBuff,24);

            memset(szLocalFormatTotalAmount,0,sizeof(szLocalFormatTotalAmount));
            vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp, szLocalFormatTotalAmount);
            sprintf(szAmountBuff,"%s %s",strCST.szCurSymbol,szLocalFormatTotalAmount);
            inPrintLeftRight("TOTAL:",szAmountBuff,24);
        }
            //else if (((srTransRec.byTransType == REFUND) || (srTransRec.byTransType == PREAUTH_VER) || (srTransRec.byTransType == PRE_AUTH)) && (gblDCCTrans==TRUE))
        else if (((srTransRec.byTransType == REFUND) || (srTransRec.byTransType == PREAUTH_VER) || (srTransRec.byTransType == PRE_AUTH)) && (srTransRec.fDCCOptin==TRUE))
        {
            vdCTOS_PrinterFline(1);
            memset(szStr, ' ', d_LINE_SIZE);
            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
            memset(szAmtBuf2,0,sizeof(szAmtBuf2));

            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
            if(srTransRec.inMinorUnit==0)
                purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);

            memset(szStr, ' ', d_LINE_SIZE);
            if(srTransRec.inMinorUnit==0)
                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
            else
                sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
            inPrintLeftRight("TOTAL TRANSACTION", " ", 24);
            inPrintLeftRight("AMOUNT:", szStr, 46);
        }
        else
        {
            vdCTOS_PrinterFline(1);
            memset(szStr, ' ', d_LINE_SIZE);
            memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
            memset(szAmtBuf2,0,sizeof(szAmtBuf2));
            if (srTransRec.byVoided == TRUE)
            {
                if(srTransRec.inMinorUnit==0)
                {
                    szAmtBuf2[0] = '-';
                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                    purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                }
                else
                {
                    szDCCFormatTotalAmount[0] = '-';
                    vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
                }

            }
            else
            {
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                if(srTransRec.inMinorUnit==0)
                {
                    purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf2);
                }
                else
                    sprintf(szStr,"%s %s", srTransRec.szDCCCurrencySymbol, szDCCFormatTotalAmount);
            }

            inPrintLeftRight("TOTAL TRANSACTION"," ", 24);
            inPrintLeftRight("AMOUNT:", szStr, 46);

        }

// #00060 - start - Revision on Chip Receipt:
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

        memset(szStr, ' ', d_LINE_SIZE);
        memset(szTemp, ' ', d_LINE_SIZE);

        if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
            /* EMV: Revised EMV details printing - start -- jzg */
            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
            (srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
            (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
            (srTransRec.bWaveSID == d_EMVCL_SID_DISCOVER_DPAS) ||
            (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == 0x63) ||
            (srTransRec.bWaveSID == 0x65))
        {
            vdCTOS_PrinterFline(1); // space after printing trans title.
            vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

            //AID
            memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
            memset(szTemp, ' ', d_LINE_SIZE);
            wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
            sprintf(szStr, "AID: %s",szTemp);


            memset(baTemp, ' ', d_LINE_SIZE);
            sprintf(baTemp, "APP: %s", srTransRec.stEMVinfo.szChipLabel);
            inPrintLeftRight(szStr, baTemp, 46);

            //if(srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND  && srTransRec.byTransType != SALE_OFFLINE  && srTransRec.byOrgTransType != SALE_OFFLINE)
            if((srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND  && srTransRec.byTransType != SALE_OFFLINE  && srTransRec.byOrgTransType != SALE_OFFLINE)
               && (srTransRec.byUploaded==TRUE && srTransRec.byOffline==FALSE))
            {
                //AC
                memset(szStr, ' ', d_LINE_SIZE);
                wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
                sprintf(szStr, "TC: %s", szTemp);

                //TVR
                EMVtagLen = 5;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
                memset(baTemp, ' ', d_LINE_SIZE);
                sprintf(baTemp, "TVR: %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
                inPrintLeftRight(szStr, baTemp, 46);
            }
        }

        //vdCTOS_PrinterFline(1);

        ushCTOS_DCCPrintAgreement();

    }

    return d_OK;
}


USHORT printDCCTIDMID(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr1[d_LINE_SIZE + 1];
    
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
    
    memset(szStr, ' ', d_LINE_SIZE);
    sprintf(szStr, "TERMINAL: %s", srTransRec.szTID);
    
    memset(szStr1, ' ', d_LINE_SIZE);
    sprintf(szStr1, "MERCHANT: %s", srTransRec.szMID);
    
    inPrint(szStr1);
    inPrint(szStr);
    
    return d_OK;
}

USHORT printDCCInvoiceNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr1[d_LINE_SIZE + 1];

    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];

    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp2, ' ', d_LINE_SIZE);

	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
	wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
	vdMyEZLib_LogPrintf("invoice no: %s",szTemp2);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH NUM: %s", szTemp1);
	//result=inPrint(szStr);

	memset(szStr1, 0x00, d_LINE_SIZE);
	sprintf(szStr1, "SEQUENCE.: %s", szTemp2);
	//result=inPrint(szStr);

	inPrint(szStr1);
	//inPrintLeftRight(szStr, szStr1, 46);

    return d_OK;
}

USHORT ushCTOS_DCCPrintFooter(int page)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;

    int fNSRflag = inNSRFlag();

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	if(page == CUSTOMER_COPY_RECEIPT)
	{
		#if 0
		vdCTOS_PrinterFline(1);

		ushCTOS_PrintAgreement();

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);

		if(strHDT.fFooterLogo == TRUE)
		{
			if(strlen(strHDT.szFooterLogoName))
				vdCTOSS_PrinterBMPPic(0, 0, strHDT.szFooterLogoName);
			else
                vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
		}
		#endif
		vdCTOS_PrinterFline(1);
		vdPrintCenter("***** CUSTOMER COPY *****");
		vdCTOS_PrinterFline(2);
	}
	else if(page == MERCHANT_COPY_RECEIPT)
	{
	#if 0
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

        DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);
		vdDebug_LogPrintf("MERCH:CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("MERCH:srTransRec.byEntryMode = [%d] | bWaveSCVMAnalysis = [%d] = bWaveSID = [%d]", srTransRec.byEntryMode, srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSID);

		EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

		vdDebug_LogPrintf("MERCH:EMVtagVal [%02X %02X %02X]",EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);

        if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
        {

            if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
            (EMVtagVal[0] != 0x1E) &&
            (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {
				if((srTransRec.byTransType == REFUND || srTransRec.byOrgTransType == REFUND) ||
				  (srTransRec.byTransType == SALE_OFFLINE || srTransRec.byOrgTransType == SALE_OFFLINE) ||
				  (srTransRec.byTransType == PRE_AUTH || srTransRec.byOrgTransType == PRE_AUTH))
				{
                    vdCTOS_PrinterFline(1);
                    if(ushCTOS_ePadPrintSignature() != d_OK)
                        vdCTOS_PrinterFline(1);
                    inPrint("SIGN:_______________________________________");
				}

				else
				{
					vdCTOS_PrinterFline(1);
					vdPrintCenter("*****NO SIGNATURE REQUIRED*****");

				    //if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
				    if((EMVtagVal[0] & 0x0F) == 0x01 || (EMVtagVal[0] & 0x0F) == 0x02 || srTransRec.bWaveSCVMAnalysis == 2)
				    {
                        if(srTransRec.fEMVPIN != TRUE && srTransRec.fCUPPINEntry != TRUE) /*check PIN BYPASSED*/
                        {

                        }
						else
						{
                            vdCTOS_PrinterFline(1);
                            vdPrintCenter("(PIN VERIFY SUCCESS)");
						}
				    }


				}
            }
            else
            {
                if(srTransRec.fDebit != TRUE)
                {
                    vdCTOS_PrinterFline(1);
					if(fNSRflag == 1)
					    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
					else
					{
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
					}
                }
            }
        }
		else if( srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		{
            if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
            {
            	if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
            	{
            		vdCTOS_PrinterFline(1);
					if(fNSRflag == 1)
					    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
					else
					{
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
					}
            	}
				else
				{
					vdCTOS_PrinterFline(1);
					vdPrintCenter("*****NO SIGNATURE REQUIRED*****");

				    //if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
				    if(EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || srTransRec.bWaveSCVMAnalysis == 2)
				    {
                        if(srTransRec.fEMVPIN != TRUE && srTransRec.fCUPPINEntry != TRUE) /*check PIN BYPASSED*/
                        {

                        }
						else
						{
                            vdCTOS_PrinterFline(1);
                            vdPrintCenter("(PIN VERIFY SUCCESS)");
						}


				    }
				}
            }
			else
			{
                if(srTransRec.fDebit != TRUE)
                {
                    vdCTOS_PrinterFline(1);
					if(fNSRflag == 1)
					    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
					else
					{
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
					}
                }
			}
		}
        else
        {
            if(srTransRec.fDebit != TRUE)
            {
				vdCTOS_PrinterFline(1);
				if(fNSRflag == 1)
					vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
				else
				{
                    if(ushCTOS_ePadPrintSignature() != d_OK)
                        vdCTOS_PrinterFline(1);
                    inPrint("SIGN:_______________________________________");
				}
            }
        }

		vdCTOS_PrinterFline(1);

		ushCTOS_PrintAgreement();

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);

        if(strHDT.fFooterLogo == TRUE)
        {
        if(strlen(strHDT.szFooterLogoName))
            vdCTOSS_PrinterBMPPic(0, 0, strHDT.szFooterLogoName);
        else
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
        }
		#endif
		vdCTOS_PrinterFline(1);
		vdPrintCenter("***** MERCHANT COPY *****");
		vdCTOS_PrinterFline(2);
	}
	else if(page == BANK_COPY_RECEIPT)
	{
	#if 0
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;

		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);
		vdDebug_LogPrintf("BANK:CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("BANK:srTransRec.byEntryMode = [%d] | bWaveSCVMAnalysis = [%d] = bWaveSID = [%d]", srTransRec.byEntryMode, srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSID);

		EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

		vdDebug_LogPrintf("BANK:EMVtagVal [%02X %02X %02X]",EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);

        if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
        {
        	if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
	            (EMVtagVal[0] != 0x1E) &&
	            (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
	            {
					if((srTransRec.byTransType == REFUND || srTransRec.byOrgTransType == REFUND) ||
					   (srTransRec.byTransType == SALE_OFFLINE || srTransRec.byOrgTransType == SALE_OFFLINE) ||
				  	   (srTransRec.byTransType == PRE_AUTH || srTransRec.byOrgTransType == PRE_AUTH))
					{
	                    vdCTOS_PrinterFline(1);
	                    if(ushCTOS_ePadPrintSignature() != d_OK)
	                        vdCTOS_PrinterFline(1);
	                    inPrint("SIGN:_______________________________________");
					}
					else
					{
	                    vdCTOS_PrinterFline(1);
						vdPrintCenter("*****NO SIGNATURE REQUIRED*****");

					    //if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
					    if((EMVtagVal[0] & 0x0F) == 0x01 || (EMVtagVal[0] & 0x0F) == 0x02 || srTransRec.bWaveSCVMAnalysis == 2)
					    {
							if(srTransRec.fEMVPIN != TRUE && srTransRec.fCUPPINEntry != TRUE) /*check PIN BYPASSED*/
							{

							}
							else
							{
								vdCTOS_PrinterFline(1);
								vdPrintCenter("(PIN VERIFY SUCCESS)");
							}

					    }
					}
	            }
	            else
	            {
	                if(srTransRec.fDebit != TRUE)
	                {
	                    vdCTOS_PrinterFline(1);
						if(fNSRflag == 1)
							vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
						else
						{
                            if(ushCTOS_ePadPrintSignature() != d_OK)
                                vdCTOS_PrinterFline(1);
                            inPrint("SIGN:_______________________________________");
						}
	                }
	            }
        }
		else if( srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		{
            if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
            {
            	if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
            	{
            		vdCTOS_PrinterFline(1);
					if(fNSRflag == 1)
					    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
					else
					{
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
					}
            	}
				else
				{
					vdCTOS_PrinterFline(1);
					vdPrintCenter("*****NO SIGNATURE REQUIRED*****");

				    //if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
				    if(EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || srTransRec.bWaveSCVMAnalysis == 2)
				    {
                        if(srTransRec.fEMVPIN != TRUE && srTransRec.fCUPPINEntry != TRUE) /*check PIN BYPASSED*/
                        {

                        }
						else
						{
                            vdCTOS_PrinterFline(1);
                            vdPrintCenter("(PIN VERIFY SUCCESS)");
						}

				    }
				}
            }
			else
			{
                if(srTransRec.fDebit != TRUE)
                {
                    vdCTOS_PrinterFline(1);
					if(fNSRflag == 1)
					    vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
					else
					{
                        if(ushCTOS_ePadPrintSignature() != d_OK)
                            vdCTOS_PrinterFline(1);
                        inPrint("SIGN:_______________________________________");
					}
                }
			}
		}
        else
        {
            if(srTransRec.fDebit != TRUE)
            {
				vdCTOS_PrinterFline(1);
				if(fNSRflag == 1)
					vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
				else
				{
                    if(ushCTOS_ePadPrintSignature() != d_OK)
                        vdCTOS_PrinterFline(1);
                    inPrint("SIGN:_______________________________________");
				}
            }
        }

		vdCTOS_PrinterFline(1);

		ushCTOS_PrintAgreement();

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
        if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);

        if(strHDT.fFooterLogo == TRUE)
        {
        if(strlen(strHDT.szFooterLogoName))
            vdCTOSS_PrinterBMPPic(0, 0, strHDT.szFooterLogoName);
        else
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
        }
		#endif
		vdCTOS_PrinterFline(1);
		vdPrintCenter("***** BANK COPY *****");
		vdCTOS_PrinterFline(2);
	}

	vdLineFeed((strHDT.fFooterLogo == TRUE)?TRUE:FALSE); // added -- sidumili

return d_OK;

}

void vdPrintDCCFormattedAmount(unsigned char *prtDisplay, unsigned char *prtAmount, int inWidth)
{
         char szAmtBuf[24+1] = { 0 };
	char szStr[24+1] = { 0 };
	char szStr2[24+1] = { 0 };
	unsigned long inDCCAmount;
	char szDCCAmt[12+1];

	memset(szStr, 0, sizeof(szStr));
	memset(szStr2, 0, sizeof(szStr2));
	memset(szAmtBuf, 0, sizeof(szAmtBuf));

	if(srTransRec.byVoided == TRUE)
         {
              if(srTransRec.fDCCOptin== TRUE)
               {
                    szAmtBuf[0]='-';
		 inCSTReadCurrencySymbol(srTransRec.szCurrencyCode, srTransRec.szDCCCurrencySymbol, srTransRec.szAmountFormat);
                    gblinMinorUnit= strCST.inMinorUnit;
                    inDCCAmount=atol((char *)srTransRec.szForeignAmount);
                    sprintf(szDCCAmt,"%ld",inDCCAmount);
                    //vdDCCModifyAmount(szDCCAmt,&szAmtBuf[1]);
                     vdCTOS_FormatAmount(srTransRec.szAmountFormat,szDCCAmt, &szAmtBuf[1]);
               }
              else
              {
                   szAmtBuf[0]='-';
                   vdCTOS_FormatAmount("NNN,NNN,NNn.nn", prtAmount, &szAmtBuf[1]);
              }
         }
	else
	    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", prtAmount, szAmtBuf);

	vdCTOS_Pad_String(szAmtBuf, 12, 0x20, POSITION_LEFT);
	strcpy(szStr, prtDisplay);

         if((srTransRec.fDCCOptin== TRUE) && (srTransRec.byTransType==VOID))
              sprintf(szStr2, "%s %s", srTransRec.szDCCCurrencySymbol, szAmtBuf);
         else
              sprintf(szStr2, "%s %s", strCST.szCurSymbol, szAmtBuf);

	inPrintLeftRight(szStr, szStr2, inWidth);
}


int vdPrintDCCChooseCurrency (char * szDCCTransactionType)
{
    char szStr[d_LINE_SIZE + 1];
    char szStr2[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    short spacestring;
    BYTE   key;
    char szTermSerialNum[15+1];
    char szLocalTotalAmountformat[20+1];
    BYTE  szDCCFormatTipAmount[20+1];
    BYTE  szDCCFormatTipAmount3MinorUnits[20+1];
    BYTE  szDCCFormatBaseAmount[20+1];
    BYTE  szDCCFormatTotalAmount[20+1];
    BYTE szAmtBuf[20+1];
    BYTE szAmtBuf2[20+1];
    char szTermSerialNumber[15];

    if( printCheckPaper()==-1)
        return -1;

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);
    //inCTOS_DisplayPrintBMP();
    vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
    ushCTOS_PrintHeader(0);


    if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
        vdPrintCenter("***CONTACTLESS***");

    vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    vdPrintCenter("*DCC RATE LOOK-UP ONLY*");    
    vdCTOS_PrinterFline(1);
    inPrint(szDCCTransactionType);
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    vdCTOS_SetDateTime();

    printDateTime(TRUE);

    printTIDMID();

    //printBatchInvoiceNO();
    inPrintLeftRight("BATCH NUM: 000000",  "TRACE NO.: 000000", 46);

    sprintf(szStr, "%s", srTransRec.szCardLable);

    memset (baTemp, 0x00, sizeof(baTemp));
    vdCTOS_FormatPANEx(strIIT.szMaskCustomerCopy, srTransRec.szPAN, baTemp, strIIT.fMaskCustCopy);

    inPrintLeftRight(szStr, baTemp, 46);

    printCardHolderNameEntryMode();

    memset(szStr, 0x00, sizeof(szStr));
    memset(szTermSerialNumber,0x00,sizeof(szTermSerialNumber));
    //CTOS_GetFactorySN(szTermSerialNumber);
     usGetSerialNumber(szTermSerialNumber);
    szTermSerialNumber[15]=0;
    sprintf(szStr, "TSN : %s", szTermSerialNumber);
    inPrint(szStr);

    vdCTOS_PrinterFline(1);

    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp3, ' ', d_LINE_SIZE);
    memset(szLocalTotalAmountformat,0,sizeof(szLocalTotalAmountformat));
    wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
    wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

    if(srTransRec.byVoided == TRUE)
    {
        szLocalTotalAmountformat[0] = '-';
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, &szLocalTotalAmountformat[1]);
        sprintf(szStr,"%s %s", strCST.szCurSymbol, szLocalTotalAmountformat);
    }
    else
    {
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szLocalTotalAmountformat);
        sprintf(szStr,"%s %s", strCST.szCurSymbol, szLocalTotalAmountformat);
    }
    inPrintLeftRight("TRANSACTION AMOUNT:", szStr, 46);

    vdCTOS_PrinterFline(1);

    vdPrintCenter("SELECT [X] TRANSACTION CURRENCY");

    vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    // vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    memset(szStr, ' ', d_LINE_SIZE);
    //sprintf(szStr,"%s", gblszExchangeRate);
    if((srTransRec.IITid == VISA_ISSUER) || (srTransRec.IITid == MASTERCARD_ISSUER))
    {
         sprintf(szStr,"1 %s  = %s %s", srTransRec.szDCCCurrencySymbol, srTransRec.szForeignRate, strCST.szCurSymbol);
         inPrintLeftRight("EX RATE:", szStr, 46);
    }
    else
    {
         sprintf(szStr,"%s", srTransRec.szExchangeRate);
         inPrintLeftRight("FX RATE:", szStr, 46);
    }

    memset(szStr, ' ', d_LINE_SIZE);
    //sprintf(szStr,"%s", gblszMarginRatePercentage);
    //sprintf(szStr,"%s", srTransRec.szMarginRatePercentage);
    //inPrintLeftRight("MARGIN:", szStr, 46);
    inPrint(srTransRec.szFXSourceName);
    sprintf(szStr,"includes %s%% margin", srTransRec.szMarginRatePercentage);
    inPrint(szStr);

    vdCTOS_PrinterFline(1);

    memset(szStr, ' ', d_LINE_SIZE);
    memset(szStr2, ' ', d_LINE_SIZE);
    sprintf(szStr,"[  ] %s AMOUNT", strCST.szCurSymbol);
    sprintf(szStr2,"[  ] %s AMOUNT", srTransRec.szDCCCurrencySymbol);

    inPrintLeftRight(szStr, szStr2, 46);

    // vdCTOS_PrinterFline(1);
    memset(szLocalTotalAmountformat,0,sizeof(szLocalTotalAmountformat));
    memset(szDCCFormatBaseAmount,0,sizeof(szDCCFormatBaseAmount));
    memset(szAmtBuf2,0,sizeof(szAmtBuf2));
    if(srTransRec.byVoided == TRUE)
    {
        szLocalTotalAmountformat[0] = '-';
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, &szLocalTotalAmountformat[1]);

        if(srTransRec.inMinorUnit==0)
        {
            szAmtBuf2[0] = '-';
            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, szDCCFormatBaseAmount);
            purgeleadingchar('0',szDCCFormatBaseAmount, &szAmtBuf2[1]);
        }
        else
        {
            szDCCFormatBaseAmount[0] = '-';
            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, &szDCCFormatBaseAmount[1]);
        }
    }
    else
    {
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp1, szLocalTotalAmountformat);
        vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szBaseForeignAmount, szDCCFormatBaseAmount);
        if(srTransRec.inMinorUnit==0)
            purgeleadingchar('0',szDCCFormatBaseAmount, szAmtBuf2);
    }

    memset(szStr, ' ', d_LINE_SIZE);
    sprintf(szStr,"AMT: %s", szLocalTotalAmountformat);
    memset(szStr2, ' ', d_LINE_SIZE);
    if(srTransRec.inMinorUnit==0)
        sprintf(szStr2,"AMT: %s", szAmtBuf2);
    else
        sprintf(szStr2,"AMT: %s", szDCCFormatBaseAmount);
    inPrintLeftRight(szStr, szStr2, 46);

    //Tip amount
    memset(szStr, ' ', d_LINE_SIZE);
    sprintf(szStr,"%s	 %s","TIP",strCST.szCurSymbol);
    memset (baTemp, 0x00, sizeof(baTemp));
    memset (szTemp4, 0x00, sizeof(szTemp4));
    wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
    DebugAddSTR("TIP:",szTemp4,12);

    if(atol(szTemp4) > 0)
    {
        memset(szAmtBuf,0,sizeof(szAmtBuf));
        memset(szDCCFormatTipAmount,0,sizeof(szDCCFormatTipAmount));
        memset(szAmtBuf2,0,sizeof(szAmtBuf2));
        if(srTransRec.byVoided == TRUE)
        {

            szAmtBuf[0] ='-';
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp4, &szAmtBuf[1]);

            if(srTransRec.inMinorUnit==3)
            {
                szDCCFormatTipAmount[0] = '-';
                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, &szDCCFormatTipAmount[1]);
                strcpy(szDCCFormatTipAmount3MinorUnits, szDCCFormatTipAmount);
                strcat(szDCCFormatTipAmount3MinorUnits, "0");
            }
            else if (srTransRec.inMinorUnit==0)
            {
                szAmtBuf2[0] ='-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                purgeleadingchar('0',szDCCFormatTipAmount,&szAmtBuf2[1]);
            }
            else
            {
                szDCCFormatTipAmount[0] = '-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, &szDCCFormatTipAmount[1]);
            }

        }
        else
        {

            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp4, szAmtBuf);

            if(srTransRec.inMinorUnit==3)

            {
                vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                strcpy(szDCCFormatTipAmount3MinorUnits, szDCCFormatTipAmount);
                strcat(szDCCFormatTipAmount3MinorUnits, "0");
            }
            else if (srTransRec.inMinorUnit==0)
            {
                //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
                purgeleadingchar('0',szDCCFormatTipAmount,szAmtBuf2);
            }
            else
            {
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szDCCTipAmount, szDCCFormatTipAmount);
            }

        }
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szStr2, ' ', d_LINE_SIZE);
        sprintf(szStr,"TIP: %s", szAmtBuf);
        if(srTransRec.inMinorUnit==3)

            sprintf(szStr2,"TIP: %s", szDCCFormatTipAmount3MinorUnits);
        else if(srTransRec.inMinorUnit==0)
        {
             if(strlen(szAmtBuf2)==0)
                  sprintf(szStr2,"TIP: %c", '0');
             else
                  sprintf(szStr2,"TIP: %s", szAmtBuf2);
        }
        else
            sprintf(szStr2,"TIP: %s", szDCCFormatTipAmount);

        inPrintLeftRight(szStr, szStr2, 46);

        memset(szAmtBuf,0,sizeof(szAmtBuf));
        memset(szDCCFormatTotalAmount,0,sizeof(szDCCFormatTotalAmount));
        memset(szAmtBuf2,0,sizeof(szAmtBuf2));
        if(srTransRec.byVoided == TRUE)
        {

            szAmtBuf[0] = '-';
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, &szAmtBuf[1]);

            if(srTransRec.inMinorUnit==0)
            {
                szAmtBuf2[0] = '-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
                purgeleadingchar('0',szDCCFormatTotalAmount,&szAmtBuf2[1]);
            }
            else
            {
                szDCCFormatTotalAmount[0]='-';
                vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, &szDCCFormatTotalAmount[1]);
            }

        }
        else
        {
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szAmtBuf);

            vdCTOS_FormatAmount(srTransRec.szAmountFormat, srTransRec.szForeignAmount, szDCCFormatTotalAmount);
            if(srTransRec.inMinorUnit==0)
                purgeleadingchar('0',szDCCFormatTotalAmount,szAmtBuf2);

        }
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szStr2, ' ', d_LINE_SIZE);
        sprintf(szStr,"TOT: %s", szAmtBuf);
        if(srTransRec.inMinorUnit==0)
            sprintf(szStr2,"TOT: %s", szAmtBuf2);
        else
            sprintf(szStr2,"TOT: %s", szDCCFormatTotalAmount);
        inPrintLeftRight(szStr, szStr2, 46);

    }

    vdCTOS_PrinterFline(1);
    vdCTOSS_PrinterBMPPic(0, 0, "dccoption.bmp");
    vdCTOS_PrinterFline(3);
    inPrint("SIGN:_______________________________________");

    vdLineFeed(FALSE);

    //vdCTOS_PrinterFline(3);

    vdCTOSS_PrinterEnd();
}


USHORT ushCTOS_DCCPrintAgreement()
{
    char szTemp[100] = {0};
    if((srTransRec.fDebit != TRUE) && (srTransRec.byTransType !=PREAUTH_VER) && (srTransRec.byTransType !=PRE_AUTH) && (srTransRec.byTransType !=PREAUTH_VOID) && (srTransRec.byTransType !=PREAUTH_COMP)) 
    {
        if(srTransRec.fDCCOptin==TRUE)
             sprintf(szTemp,"AND AGREE TO PAY IN [%s].", srTransRec.szDCCCurrencySymbol);
        else
	   sprintf(szTemp,"AND AGREE TO PAY IN [%s].", strCST.szCurSymbol);	

	 vdCTOS_PrinterFline(1);
           vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
           vdPrintCenter("[X] I HAVE BEEN OFFERED A CHOICE OF CURRENCIES");
           vdPrintCenter(szTemp);
           vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
           vdCTOS_PrinterFline(1);
           vdPrintCenter("THIS CONVERSION IS PROVIDED BY METROBANK");
           vdCTOS_PrinterFline(1);
    }
    else
    {
          if(srTransRec.fDCCOptin==TRUE)
              sprintf(szTemp,"[X] CARDHOLDER HAS CHOSEN TO PAY IN [%s].", srTransRec.szDCCCurrencySymbol);
        else
	    sprintf(szTemp,"[X] CARDHOLDER HAS CHOSEN TO PAY IN [%s].", strCST.szCurSymbol);
        
         vdCTOS_PrinterFline(1);
         vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
         vdPrintCenter(szTemp);
         vdPrintCenter("THIS TRANSACTION IS BASED ON TODAY'S EXCHANGE");
         vdPrintCenter("RATE.AT TRANSACTION COMPLETION THE PREVAILING");
         vdPrintCenter("EXCHANGE RATE WILL BE USED.THIS IS NOT A DEBIT");
	vdPrintCenter("FROM YOUR ACCOUNT.AUTHORIZATION OF FUNDS ONLY.");
         vdCTOS_PrinterFline(1);
         vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
}

void vdCTOS_PrintAccumeByDCCCurrency (int inReportType, STRUCT_TOTAL Totals)
{
      // DCC
    USHORT usDCCCount=Totals.usDCCCount;
    ULONG  ulSaleDCCTotalAmount=Totals.ulSaleDCCTotalAmount;
    USHORT usCompDCCCount=Totals.usCompDCCCount;
    ULONG  ulCompTotalDCCAmount=Totals.ulCompTotalDCCAmount;
    USHORT usVoidSaleDCCCount=Totals.usVoidSaleDCCCount;
    ULONG  ulVoidSaleTotalDCCAmount=Totals.ulVoidSaleTotalDCCAmount;
    USHORT usRefundDCCCount=Totals.usRefundDCCCount;
    ULONG  ulRefundTotalDCCAmount=Totals.ulRefundTotalDCCAmount;
    USHORT usOffSaleDCCCount=Totals.usOffSaleDCCCount;
    ULONG  ulOffSaleTotalDCCAmount=Totals.ulOffSaleTotalDCCAmount;
    USHORT usTipDCCCount=Totals.usTipDCCCount;
    ULONG  ulTipTotalDCCAmount=Totals.ulTipTotalDCCAmount;

    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szFormattedAmount[d_LINE_SIZE + 1];
    USHORT usTotalCount;
    ULONG  ulTotalAmount;
    BYTE baTemp[PAPER_X_SIZE * 64];

    BYTE  szDCCFormatTipAmount3MinorUnits[20+1];

    CTOS_PrinterSetHeatLevel(4);
    memset (baTemp, 0x00, sizeof(baTemp));

   vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    //DCC sale
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulSaleDCCTotalAmount);
    vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp,szFormattedAmount);
    sprintf(szStr,"SALE           %3d       %s",usDCCCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //DCC SALE COMPLETION
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulCompTotalDCCAmount);
    vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp,szFormattedAmount);
    sprintf(szStr,"PRECOMP        %3d       %s",usCompDCCCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //DCC VOID
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulVoidSaleTotalDCCAmount);
    vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp,&szFormattedAmount[1]);
    szFormattedAmount[0]='-'; 
    sprintf(szStr,"VOID           %3d       %s",usVoidSaleDCCCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

     //DCC REFUND
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulRefundTotalDCCAmount);
    vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp,&szFormattedAmount[1]);
    szFormattedAmount[0]='-'; 
    sprintf(szStr,"REFUNDS        %3d       %s",usRefundDCCCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

   //DCC TIP  
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE); 
    memset(szDCCFormatTipAmount3MinorUnits, 0x00, d_LINE_SIZE); 	
    sprintf(szTemp,"%ld", ulTipTotalDCCAmount);

   if(strCST.inMinorUnit==3)

   {
        vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
        strcpy(szDCCFormatTipAmount3MinorUnits, szFormattedAmount);
        strcat(szDCCFormatTipAmount3MinorUnits, "0");
        sprintf(szStr,"TIPS                     %s",strCST.szCurSymbol);
       inPrintLeftRight(szStr, szDCCFormatTipAmount3MinorUnits, 46);
   }
   else
  {
       vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp,szFormattedAmount);
       sprintf(szStr,"TIPS                     %s",strCST.szCurSymbol);
       inPrintLeftRight(szStr, szFormattedAmount, 46);
  }
    //DCC OFFLINE SALE
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulOffSaleTotalDCCAmount);
    vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp,szFormattedAmount);
    sprintf(szStr,"OFFLINE        %3d       %s", usOffSaleDCCCount,strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);
   
    vdCTOS_PrinterFline(1);

}

USHORT printDateTimeReportFormat(unsigned char * szTempDate)
{
	BYTE   EMVtagVal[64];
	USHORT EMVtagLen;
	int result;
	char szStr[35 + 1];
	CTOS_RTC SetRTC;
	char szYear[3];
	//char szTempDate[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTermSerialNum[15+1]; // print terminal serial number on all txn 
//receipt - mfl
	BYTE baTemp[PAPER_X_SIZE * 64];

	vdDebug_LogPrintf("--printDateTime--");
	vdDebug_LogPrintf("szTranYear[%s]", srTransRec.szTranYear);

	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);

	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);

	memset(szTempDate, 0x00, sizeof(szTempDate));
	
		if (strlen(srTransRec.szTranYear) > 0)
			sprintf(szTempDate, "%02lu/%02lu/%02lu  %02lu:%02lu", atol(szTemp)/100, 
atol(szTemp)%100, atol(srTransRec.szTranYear),  atol(szTemp2)/10000,atol(
szTemp2)%10000/100);
		else
			sprintf(szTempDate, "%02lu/%02lu/%02lu  %02lu:%02lu", atol(szTemp)/100, 
atol(szTemp)%100, atol(srTransRec.szYear), atol(szTemp2)/10000,atol(szTemp2)%
10000/100);

	return(ST_SUCCESS);

}


void purgeleadingchar(unsigned char * INchar, unsigned char * INasc, unsigned char * OUTasc)
{
    char sztemp [20+1];
    char sztemp1 [20+1];
    int x = 0, y = 0;

    strcpy(sztemp, INasc);

    vdDebug_LogPrintf("INchar::%c || sztemp::%s", INchar, sztemp);

    while(x < strlen(sztemp))
    {
        if (sztemp[x] == INchar)
        {
            vdDebug_LogPrintf("AAA -[%s]",&sztemp[x]);
        }
        else
        {
            strcpy(OUTasc, &sztemp[x]);
            break;
        }

        x++;
    }

}


USHORT ushCTOS_PrintHeaderAddress(void)
{


    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    //merhcant name and address
    if(strlen(strMMT[0].szRctHdr1) > 0)
        vdPrintCenter(strMMT[0].szRctHdr1);
    if(strlen(strMMT[0].szRctHdr2) > 0)
        vdPrintCenter(strMMT[0].szRctHdr2);
    if(strlen(strMMT[0].szRctHdr3) > 0)
        vdPrintCenter(strMMT[0].szRctHdr3);
    if(strlen(strMMT[0].szRctHdr4) > 0)
        vdPrintCenter(strMMT[0].szRctHdr4);
    if(strlen(strMMT[0].szRctHdr5) > 0)
        vdPrintCenter(strMMT[0].szRctHdr5);

    //CTOS_PrinterFline(d_LINE_DOT * 1);

    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

    if(fRePrintFlag == TRUE)
    {
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        vdPrintCenter("DUPLICATE");
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        //vdCTOS_PrinterFline(1);
    }

    vdCTOS_PrinterFline(1);

    vdDebug_LogPrintf("ushCTOS_PrintHeader-end");
    return d_OK;
}

int inPrintConfirmation(char* szTitle, BOOL fIncludeLogoHeader)
{
	int inKey = d_NO;
	BYTE szLogoPath[512];
    BYTE szMsg[512];
    BYTE szPrintMsg[512];
	BYTE szDisMsg[512];

	vdDebug_LogPrintf("--inPrintConfirmation--");
	vdDebug_LogPrintf("feReceiptEnable=[%d]", strTCT.feReceiptEnable);
	vdDebug_LogPrintf("szHeaderLogoName=[%s]", strHDT.szHeaderLogoName);
	
	if (strTCT.feReceiptEnable != 1)
		return d_NO;

	memset(szMsg, 0x00, sizeof(szMsg));
    memset(szPrintMsg, 0x00, sizeof(szPrintMsg));
	memset(szLogoPath, 0x00, sizeof(szLogoPath));	
	
	if (fIncludeLogoHeader)
	{
		if (strlen(strHDT.szHeaderLogoName) > 0)
			sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
		else
			sprintf(szLogoPath, "%s%s", LOCAL_PATH, "logo.bmp");
	}
	else
	{
		sprintf(szLogoPath, "%s%s", LOCAL_PATH, "blank.bmp");
	}

	vdDebug_LogPrintf("szLogoPath=[%s]", szLogoPath);
	
    strcpy(szMsg, "Print Confirmation");

	strcpy(szPrintMsg, szLogoPath);
	strcat(szPrintMsg, "|");
	strcat(szPrintMsg, szMsg);
		
	inKey = PrintReceiptUI(szPrintMsg);
	vdDebug_LogPrintf("inKey=[%d]", inKey);
				
	if (inKey == d_OK) 
	{
		// sidumili: check printer when selecting "PRINT"
		if( printCheckPaper()==-1)
    		return d_NO;
	
		memset(szDisMsg, 0x00, sizeof(szDisMsg));
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PRINTING...");
		usCTOSS_LCDDisplay(szDisMsg);
	        
        return d_OK;
    }

	if (inKey == 0xFF) // timeout
    {
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(1000);
		
		return d_NO;		
    }
	
    if (inKey == d_USER_CANCEL)
    {
		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		CTOS_Beep();
		CTOS_Delay(1000);
		return d_NO;		
    }
}

// Delete bmp file -- sidumili
void vdDeleteBMP(void)
{
	char szSystemCmdPath[128+1];    
    char szNewFileName[24+1];

	vdDebug_LogPrintf("--vdDeleteBMP--");
	
    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
    sprintf(szSystemCmdPath, "rm %s", "/home/ap/pub/Print_BMP.bmp");
    system(szSystemCmdPath);

	vdDebug_LogPrintf("szSystemCmdPath[%s]", szSystemCmdPath);
}

void vdPrintBiller(void)
{
	int inNumRecs=0, i=0;
    char szStr[d_LINE_SIZE + 1], szTempBuf1[d_LINE_SIZE + 1], szTempBuf2[d_LINE_SIZE + 1];
	char szDate[d_LINE_SIZE + 1], szTime[d_LINE_SIZE + 1];
	
	inHDTRead(2);
	inCTOS_CheckAndSelectMutipleMID();
	
	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	
    vdCTOSS_PrinterStart(1000);
	CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();

	ushCTOS_PrintHeader(0);

    vdPrintTitleCenter("BILLER REPORT");
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "HOST: %s", strHDT.szHostLabel);
	inPrint(szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "MERCHANT ID: %s", strMMT[0].szMID);
	inPrint(szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "TERMINAL ID: %s", strMMT[0].szTID);
	inPrint(szStr);

	/*date and time*/ 
    memset(szTempBuf1,0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    printDateTimeEx(szTempBuf1,szTempBuf2, TRUE);

	// Print Date, Time
	memset(szDate, 0x00, sizeof(szDate));
	memset(szTime, 0x00, sizeof(szTime));
	sprintf(szDate, "DATE: %s", szTempBuf1);
	sprintf(szTime, "TIME: %02lu:%02lu:%02lu", atol(szTempBuf2)/10000,atol(szTempBuf2)%10000/100, atol(szTempBuf2)%100);
	inPrintLeftRight(szDate, szTime, 46);

	vdCTOS_PrinterFline(1);
	
    inPrintLeftRight("BILLER NAME", "CODE", 46);
	
	printDividingLine(DIVIDING_LINE);

	inNumRecs=inBLRNumRecord();

	inDatabase_TerminalOpenDatabase();
	memset(strMultiBLR, 0, sizeof(strMultiBLR));
	inBLRReadEx(1);
    inDatabase_TerminalCloseDatabase();

	for (i=0; i < inNumRecs; i++)
	{
		if(strMultiBLR[i].fBLREnable == VS_FALSE)
			continue;
		
		memset(szTempBuf1,0,sizeof(szTempBuf1));
		memset(szTempBuf2, 0, sizeof(szTempBuf2));
		strcpy(szTempBuf1, &strMultiBLR[i].szBillerName);
		strcpy(szTempBuf2, &strMultiBLR[i].szBillerCode);
		szTempBuf2[4]=0x00;
		inPrintLeftRight(szTempBuf1, szTempBuf2, 46);
	}

    printDividingLine(DIVIDING_LINE);
	
	vdCTOS_PrinterFline(3);
	vdCTOSS_PrinterEnd();

}


void vdPrintUser(void)
{
	int inNumRecs=0, i=0;
    char szStr[d_LINE_SIZE + 1], szTempBuf1[d_LINE_SIZE + 1], szTempBuf2[d_LINE_SIZE + 1];
	char szDate[d_LINE_SIZE + 1], szTime[d_LINE_SIZE + 1];
	
	inHDTRead(2);
	inCTOS_CheckAndSelectMutipleMID();
	
	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	
    vdCTOSS_PrinterStart(1000);
	CTOS_PrinterSetHeatLevel(4);
    inCTOS_DisplayPrintBMP();

	ushCTOS_PrintHeader(0);

    vdPrintTitleCenter("USER REPORT");
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "HOST: %s", strHDT.szHostLabel);
	inPrint(szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "MERCHANT ID: %s", strMMT[0].szMID);
	inPrint(szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "TERMINAL ID: %s", strMMT[0].szTID);
	inPrint(szStr);

	/*date and time*/ 
    memset(szTempBuf1,0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    printDateTimeEx(szTempBuf1,szTempBuf2, TRUE);

	// Print Date, Time
	memset(szDate, 0x00, sizeof(szDate));
	memset(szTime, 0x00, sizeof(szTime));
	sprintf(szDate, "DATE: %s", szTempBuf1);
	sprintf(szTime, "TIME: %02lu:%02lu:%02lu", atol(szTempBuf2)/10000,atol(szTempBuf2)%10000/100, atol(szTempBuf2)%100);
	inPrintLeftRight(szDate, szTime, 46);

	vdCTOS_PrinterFline(1);
	
    inPrint("USER NAME");
	
	printDividingLine(DIVIDING_LINE);
	
	inNumRecs=inUSRNumRecord();

	inDatabase_TerminalOpenDatabase();
	memset(strMultiUSR, 0, sizeof(strMultiUSR));
	inUSRReadEx(1);
    inDatabase_TerminalCloseDatabase();

	for (i=0; i < inNumRecs; i++)
	{
        memset(szTempBuf1,0,sizeof(szTempBuf1));
		strcpy(szTempBuf1, &strMultiUSR[i].szUserName);
		inPrint(szTempBuf1);
	}

    printDividingLine(DIVIDING_LINE);
	
	vdCTOS_PrinterFline(3);
	vdCTOSS_PrinterEnd();

}

USHORT printDateTimeEx(char *ptrDate, char *ptrTime, BOOL fTrans)
{
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
	CTOS_RTC SetRTC;
	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];

	vdDebug_LogPrintf("--printDateTime--");
	vdDebug_LogPrintf("szTranYear[%s]", srTransRec.szTranYear);
	vdDebug_LogPrintf("fTrans[%d]", fTrans);
	
	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	
	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );	

	if(atol(szTemp) == 0)
	{
		sprintf(szTemp ,"%02d%02d",SetRTC.bMonth,SetRTC.bDay); //aaronnino BANCNET bancnet fix on issue #0002 Incomplete Date in Settlement  and Last settlement	
	}
	if(atol(szTemp1) == 0)
	{
		sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	

	}
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
	
	memset(szTempDate, 0x00, sizeof(szTempDate));

	if (fTrans)
	{
		if (strlen(srTransRec.szTranYear) > 0)
			sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szTranYear), atol(szTemp)/100, atol(szTemp)%100);
		else
			sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	}		
	else
	{
		sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	}
	
	vdCTOS_FormatDate(szTempDate);
	//if(strCDT.inType == DEBIT_CARD)
		//sprintf(szStr,"DATE %s	  TIME %02lu:%02lu:%02lu",szTempDate,atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	//else	
		//sprintf(szStr,"DATE/TIME: %s %02lu:%02lu:%02lu",szTempDate,atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);

	//inPrint(szStr);
	memcpy(ptrDate, szTempDate, strlen(szTempDate));
	memcpy(ptrTime, szTemp1, strlen(szTemp1));

	vdDebug_LogPrintf("ptrDate[%s].ptrTime[%s]", ptrDate, ptrTime);
	
	return(result);
	 
}

