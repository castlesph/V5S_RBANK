
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>


#include "accum.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\Includes\POSSetting.h"
#include "..\ui\Display.h"


void vdCTOS_GetAccumName(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal)
{
    sprintf(strFile->szFileName, "ACC%02d%02d.total"
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
                                
    strFile->bSeekType           = d_SEEK_FROM_BEGINNING;
    strFile->bStorageType        = d_STORAGE_FLASH ;
    strFile->fCloseFileNow       = TRUE;
    strFile->ulRecSize           = sizeof(ACCUM_REC);
    strFile->ptrRec              = strTotal;    
    
}


int inCTOS_ReadAccumTotal(ACCUM_REC *strTotal)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    
    vdCTOS_GetAccumName(&strFile, strTotal);

    return (inMyFile_RecRead(&strFile));    
}

int inCTOS_SaveAccumTotal(ACCUM_REC *strTotal)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    
    vdCTOS_GetAccumName(&strFile, strTotal);

    return (inMyFile_RecSave(&strFile));    
}



int inCTOS_UpdateAccumTotal(void)
{
	ACCUM_REC srAccumRec;
    BYTE        szTransAmt[12+1];
    BYTE        szTipAmt[12+1];
    BYTE        szOrgAmt[12+1];
    BYTE        szOrgTipAmt[12+1];
    BYTE        szDCCBaseAmt[12+1];
    BYTE        szDCCTipAmount[12+1];
    int         inResult;
	int inTranCardType = 0;

    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal");
	vdDebug_LogPrintf("byTransType[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("fDCCOptin[%d]", srTransRec.fDCCOptin);
    
    memset(szTransAmt, 0x00, sizeof(szTransAmt));
    memset(szTipAmt, 0x00, sizeof(szTipAmt));
    memset(szOrgAmt, 0x00, sizeof(szOrgAmt));
    memset(szOrgTipAmt, 0x00, sizeof(szOrgTipAmt));

    wub_hex_2_str(srTransRec.szTotalAmount, szTransAmt, 6);
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmt, 6);
    wub_hex_2_str(srTransRec.szOrgAmount, szOrgAmt, 6);
    wub_hex_2_str(srTransRec.szOrgTipAmount, szOrgTipAmt, 6);

   memcpy(szDCCBaseAmt, srTransRec.szForeignAmount, sizeof(szDCCBaseAmt));
   memcpy(szDCCTipAmount,srTransRec.szDCCTipAmount,sizeof(szDCCTipAmount));

#if 0		
	if (srTransRec.inCardType == CREDIT_CARD)
	{
		inTranCardType = 0;//save credit card accue total
	}
    else
    {
    	inTranCardType = 1;//save debit card accue total
    }
#else
		inTranCardType = 0;//save all card type to credit

#endif


	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }        

    vdDebug_LogPrintf("byTransType[%d].byOrgTransType[%d].szOriginTipTrType[%d]IITid[%d]", srTransRec.byTransType, srTransRec.byOrgTransType, szOriginTipTrType, srTransRec.IITid);
    
    vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);

    vdDebug_LogPrintf("DCC srTransRec.inCSTid[%d] SaleCount [%d] [%ld]", srTransRec.inCSTid, srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usDCCCount, srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount);
    vdDebug_LogPrintf("DCC srTransRec.inCSTid[%d] SaleCount [%d] [%ld]", srTransRec.inCSTid, srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usOffSaleDCCCount, srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulOffSaleTotalDCCAmount);
    vdDebug_LogPrintf("DCC usVoidSaleDCCCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usVoidSaleDCCCount, srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulVoidSaleTotalDCCAmount);
   

    switch(srTransRec.byTransType)
    {
        case SALE:
            if(strcmp(srTransRec.szPromoLabel, "REGULAR") == 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRegularCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRegularTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRegularTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount + atol(szTransAmt);            
            }
            else if(strcmp(srTransRec.szPromoLabel, "REDUCE") == 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usReduceCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReduceTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReduceTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount + atol(szTransAmt);            
            }
            else if(strcmp(srTransRec.szPromoLabel, "ZERO %") == 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usZeroCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulZeroTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulZeroTotalAmount + atol(szTransAmt);

                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount + atol(szTransAmt);            
            }		
            else if(strcmp(srTransRec.szPromoLabel, "BNPL") == 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usBNPLCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBNPLTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBNPLTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount + atol(szTransAmt);            
            }			
            else if(strcmp(srTransRec.szPromoLabel, "SNPL") == 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usBNPLCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBNPLTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBNPLTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount + atol(szTransAmt);			
            }			 
            else if(strcmp(srTransRec.szPromoLabel, "CASH2GO") == 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].us2GOCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ul2GOTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ul2GOTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount + atol(szTransAmt);            
            }		
            else
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt);            
                if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
                    srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;
                
                if(atol(szTipAmt) > 0)
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount+=atol(szTipAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount+=atol(szTipAmt);
                }
		if (srTransRec.fDCCOptin == TRUE)
                   {
                        vdDebug_LogPrintf("AAA - srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.incsTTid].ulSaleDCCTotalAmount[%ld]", srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount);
                        srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usDCCCount++;
                        srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount + atol(szDCCBaseAmt);

                       if (atol(szDCCTipAmount) > 0)	
                       {
                            srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulTipTotalDCCAmount+=atol(szDCCTipAmount);
                       }


		    vdDebug_LogPrintf("srAccumRec.stBankTotal[inTranCardType].stDCCTotal[%d].ulSaleDCCTotalAmount[%ld]",srTransRec.inCSTid, srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount);
                       vdDebug_LogPrintf("srAccumRec.stBankTotal[inTranCardType].stDCCTotal[%d].usDCCCount[%d]",srTransRec.inCSTid,  srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usDCCCount);
                       
                   
                   }
            }			
        break;      
		
        case PRE_COMP:	 
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCompCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCompTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount + atol(szTransAmt);            		
        break;	
		
        case CASH_ADV:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount + atol(szTransAmt);            
        break;
		
        case LOY_REDEEM_5050:
        case LOY_REDEEM_VARIABLE: 	
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRedeemCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRedeemTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRedeemTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount + atol(szTransAmt);            
        break;
		
        case SALE_OFFLINE:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt);

           if(srTransRec.fDCCOptin==TRUE)
           {
                srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usOffSaleDCCCount++;
                srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulOffSaleTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulOffSaleTotalDCCAmount + atol(szDCCBaseAmt);

	      if (atol(szDCCTipAmount) > 0)	
                       {
                            srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulTipTotalDCCAmount+=atol(szDCCTipAmount);
                       }		
           }
			
            if(atol(szTipAmt) > 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount+=atol(szTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount+=atol(szTipAmt);
            }
        break;
		
        case REFUND:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount + atol(szTransAmt);

	  if (srTransRec.fDCCOptin == TRUE)
           {
                srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usRefundDCCCount++;
                srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulRefundTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulRefundTotalDCCAmount + atol(szDCCBaseAmt);
           }
	  
         break;
		
        case VOID:
            if(srTransRec.byOrgTransType == LOY_REDEEM_5050 || srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidRedeemCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidRedeemTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidRedeemTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidRedeemCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidRedeemTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidRedeemTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRedeemCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRedeemTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRedeemTotalAmount - atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRedeemTotalAmount - atol(szTransAmt);
            }
            else if(srTransRec.byOrgTransType == CASH_ADV)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidCashCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidCashTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidCashTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidCashCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidCashTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidCashTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashTotalAmount - atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashTotalAmount - atol(szTransAmt);
            }		
            else if(srTransRec.byOrgTransType == PRE_COMP)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidCompCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidCompTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidCompCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidCompTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCompCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCompTotalAmount - atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount - atol(szTransAmt);
            }		            
            else if(srTransRec.byOrgTransType == SALE)
            {
                if(strcmp(srTransRec.szPromoLabel, "REGULAR") == 0)
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidRegularCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidRegularTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidRegularTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidRegularCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidRegularTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidRegularTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRegularCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRegularTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRegularTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRegularCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRegularTotalAmount - atol(szTransAmt);
                }
                else if(strcmp(srTransRec.szPromoLabel, "REDUCE") == 0)
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidReduceCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidReduceTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidReduceTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidReduceCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidReduceTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidReduceTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usReduceCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReduceTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReduceTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReduceCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReduceTotalAmount - atol(szTransAmt);
                }
                else if(strcmp(srTransRec.szPromoLabel, "ZERO %") == 0)	   		
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidZeroCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidZeroTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidZeroTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidZeroCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidZeroTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidZeroTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usZeroCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulZeroTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulZeroTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usZeroCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulZeroTotalAmount - atol(szTransAmt);
                }
                else if(strcmp(srTransRec.szPromoLabel, "SNPL") == 0)	   		
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidBNPLCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidBNPLTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidBNPLTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidBNPLCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidBNPLTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidBNPLTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usBNPLCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBNPLTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBNPLTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBNPLCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBNPLTotalAmount - atol(szTransAmt);
                }				
                else if(strcmp(srTransRec.szPromoLabel, "CASH2GO") == 0)	// 07212015   		
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoid2GOCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoid2GOTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoid2GOTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoid2GOCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoid2GOTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoid2GOTotalAmount + atol(szTransAmt);  
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].us2GOCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ul2GOTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ul2GOTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.us2GOCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ul2GOTotalAmount - atol(szTransAmt);
                }								
                else // credit sale
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atol(szTransAmt);

	           if (srTransRec.fDCCOptin == TRUE)
                    {                          
                          srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usVoidSaleDCCCount++;
                          srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulVoidSaleTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulVoidSaleTotalDCCAmount + atol(szDCCBaseAmt);
                    }
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atol(szTransAmt);  
                    if(srTransRec.byOrgTransType == SALE || srTransRec.byOrgTransType == LOY_REDEEM_5050 || srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE
                    || srTransRec.byOrgTransType == PREAUTH_COMP)
                    {
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);
                        
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                        
                        if(atol(szTipAmt) > 0)
                        {
                            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount-=atol(szTipAmt);
                            
                            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount-=atol(szTipAmt);
                        }

		    if (srTransRec.fDCCOptin == TRUE)
                      {                          
                           srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usDCCCount--;
                           srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulSaleDCCTotalAmount - atol(szDCCBaseAmt);
                      }
		     
                    }
               
                }
            }
            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                
                #if 1 //#00222 - VOID OFFLINE not printed on detailed and Summary Report.
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atol(szTransAmt);  
                #endif
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atol(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atol(szTransAmt);

               if(srTransRec.fDCCOptin==TRUE)
               {
                   srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usVoidSaleDCCCount++;
                   srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulVoidSaleTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulVoidSaleTotalDCCAmount + atol(szDCCBaseAmt);

                   srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usOffSaleDCCCount--;
                  srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulOffSaleTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulOffSaleTotalDCCAmount - atol(szDCCBaseAmt);   
	     }
                
                if(atol(szTipAmt) > 0)
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount-=atol(szTipAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount-=atol(szTipAmt);
                }
            }
            else if(srTransRec.byOrgTransType == SALE_TIP)
            {
                if (szOriginTipTrType == SALE_OFFLINE)
                {                    
                    #if 1 //6. #00058 - Void offline tip adjust not added on void total count. 
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atol(szTransAmt);  
                    #endif
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atol(szTransAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atol(szTipAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atol(szTipAmt);
                }
                else if(szOriginTipTrType == SALE) 
                {
                    #if 1 //Void tip adjust not added on void total count. 07292015 - #00217 (2)
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atol(szTransAmt);
                    #endif
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atol(szTipAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atol(szTipAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                }
            }
            else if(srTransRec.byOrgTransType == REFUND)
            {
                // add count and amount to sale count and amount for void printout
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidRefundCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidRefundTotalAmount + atol(szTransAmt);//#00218 - Incorrect Void refund total
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidRefundCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidRefundTotalAmount + atol(szTransAmt);//#00218 - Incorrect Void refund total
                
                // minus amount and countto refund printout
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount - atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount - atol(szTransAmt);

	     if(srTransRec.fDCCOptin == TRUE)
               {
                    srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usRefundDCCCount--;
                    srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulRefundTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulRefundTotalDCCAmount - atol(szDCCBaseAmt);	     
               }
            }
        break;
		
        case SALE_TIP: //#00202 - Incorrect DE 60 amount on tip adjust "1".
            if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);  
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);		   
            }
            else 
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                                                                                                   
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt); 		   
            }
        break;
		
        case SALE_ADJUST:            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szOrgAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szOrgAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt);
        break;
		
        default:
        break;
    }
    vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
    
    if((inResult = inCTOS_SaveAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Save Total Rec. error");
		vdSetErrorMessage("Save Accum Error");
        return ST_ERROR;    
    }

   	inCTLOS_Updatepowrfail(PFR_BATCH_UPDATE_COMPLETE);

    vdMyEZLib_LogPrintf("total file saved successfully");

    return ST_SUCCESS;
}


int inCTOS_ClearAccumTotal(void)
{
	short shHostIndex;
	int inResult;
	ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return ST_ERROR;
	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	memset(&strFile,0,sizeof(strFile));
	inResult = inCTOS_CheckAndSelectMutipleMID();
    vdCTOS_GetAccumName(&strFile, &srAccumRec);
	
	if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdMyEZLib_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        return ST_ERROR;
    }     
	
    return ST_SUCCESS;
	
}

void vdCTOS_SetBackupAccumFile(char *szOriFileName)
{
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	char szBKAccumeFileName[30];
	ULONG ulFileSize;
	int inResult;
	
	memset(&srAccumRec,0x00,sizeof(srAccumRec));
	memset(&strFile,0x00,sizeof(strFile));
	memset(szBKAccumeFileName,0x00,sizeof(szBKAccumeFileName));
	strcpy(szBKAccumeFileName,szOriFileName);
	CTOS_FileGetSize(szOriFileName, &ulFileSize);
	strcat(szBKAccumeFileName,".BK");
		
	vdDebug_LogPrintf("delete old BK acculFileSize[%d],del[%d]szOriFileName[%s][%s]",ulFileSize,inResult,szOriFileName,szBKAccumeFileName);
	inCTOS_FileCopy(szOriFileName,szBKAccumeFileName,ulFileSize);

	vdDebug_LogPrintf("szOriFileName[%s]set BKAccum[%s]ulFileSize[%ld]",szOriFileName,szBKAccumeFileName,ulFileSize);
	CTOS_FileGetSize(szBKAccumeFileName, &ulFileSize);
	vdDebug_LogPrintf("BK Acc ulFileSize[%ld]",ulFileSize);

	
}

void vdCTOS_GetBackupAccumFile(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal, int HostIndex, int MITid)
{

	sprintf(strFile->szFileName, "ACC%02d%02d.total%s"
								, HostIndex
								, MITid,".BK");
	
	vdDebug_LogPrintf("get BKAccum[%s]",strFile->szFileName);

								
	strFile->bSeekType			 = d_SEEK_FROM_BEGINNING;
	strFile->bStorageType		 = d_STORAGE_FLASH ;
	strFile->fCloseFileNow		 = TRUE;
	strFile->ulRecSize			 = sizeof(ACCUM_REC);
	strFile->ptrRec 			 = strTotal;	
		
}

int inCTOS_ReadBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    vdCTOS_GetBackupAccumFile(&strFile, strTotal,HostIndex, MITid);

    return (inMyFile_RecRead(&strFile));    
}

int inCTOS_DeleteBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid)
{
	
    STRUCT_FILE_SETTING strFile;
	int inResult;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    vdCTOS_GetBackupAccumFile(&strFile, strTotal,HostIndex, MITid);
	
	if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdMyEZLib_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        return ST_ERROR;
    }     
	
}
int inCTOS_FileCopy(char *szSourceFile, char *szDesFile,ULONG ulFileSize)
{

	FILE *in = NULL;
	FILE *out = NULL;
	unsigned long ulFileHandle;
	unsigned long ulOutFileHandle;
	char ch;
	ULONG ulFileSizeTemp;
	char *pAccumeData;
	int inresult;
	
	CTOS_FileGetSize(szSourceFile, &ulFileSizeTemp);
    inresult = CTOS_FileOpen(szSourceFile, d_STORAGE_FLASH, &ulFileHandle);
	vdDebug_LogPrintf("CTOS_FileOpen[%d]ulFileSizeTemp[%d]",inresult,ulFileSizeTemp);

	inresult = CTOS_FileSeek(ulFileHandle, 0, d_SEEK_FROM_BEGINNING);
	
	vdDebug_LogPrintf("CTOS_FileSeek[%d]ulFileSizeTemp[%d]",inresult,ulFileSizeTemp);
	pAccumeData = (unsigned char *) malloc(ulFileSizeTemp+1);
	vdDebug_LogPrintf("ulFileSizeTemp[%d]",ulFileSizeTemp);
	
	if(pAccumeData == NULL)
	{	
		vdDisplayErrorMsg(1, 8,	"accume bk fail");
		free(pAccumeData);
		return FAIL;
	}
	
	
    inresult = CTOS_FileRead(ulFileHandle, pAccumeData, &ulFileSizeTemp);
	
	vdDebug_LogPrintf("CTOS_FileRead[%d]ulFileSizeTemp[%d]ulFileSize[%d]",inresult,ulFileSizeTemp,ulFileSize);

	if((inresult  = CTOS_FileClose(ulFileHandle)) != d_OK)
	{				 
		vdDebug_LogPrintf("[CTOS_FileClose]---FileClz err[%04x]", inresult);
		free(pAccumeData);

		return ST_ERROR;
	}

	inresult = CTOS_FileDelete(szDesFile);
	vdDebug_LogPrintf("CTOS_FileDelete[%d]",inresult);


	vdDebug_LogPrintf("33333[%d]",ulFileSize);

	CTOS_Delay(50);
	
	
	inresult = CTOS_FileOpen(szDesFile , d_STORAGE_FLASH , &ulOutFileHandle);
	vdDebug_LogPrintf("CTOS_FileOpen[%d]",inresult);
	if(inresult == d_OK)
	{																						
		/* Move the file pointer to a specific position. 
		* Move backward from the end of the file.		 */
		inresult = CTOS_FileSeek(ulOutFileHandle, 0, d_SEEK_FROM_BEGINNING);
		if (inresult != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_RecSave]---Rec Seek inResult[%04x]", inresult);
			CTOS_FileClose(ulOutFileHandle);
			free(pAccumeData);
			return ST_ERROR;																		
		}
		else
			;
		/* Write data into this opened file */
		inresult = CTOS_FileWrite(ulOutFileHandle ,pAccumeData ,ulFileSize); 										
		if (inresult != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_RecSave]---Rec Write error, inResult[%04x]", inresult);
			CTOS_FileClose(ulOutFileHandle);
			
			free(pAccumeData);
			return ST_ERROR; 
		}																		
		
		vdDebug_LogPrintf("[inMyFile_RecSave]---Write finish,  inResult[%d]",  inresult);
		
		{	 
			if((inresult  = CTOS_FileClose(ulOutFileHandle)) != d_OK)
			{				 
				vdDebug_LogPrintf("[inMyFile_RecSave]---FileClz err[%04x]", inresult);
				free(pAccumeData);

				return ST_ERROR;
			}
			else
				ulOutFileHandle = 0x00;

			vdDebug_LogPrintf("[inMyFile_RecSave]---User close immed.");
		}

        vdDebug_LogPrintf("[inMyFile_RecSave]---User did not close immed.");
	}

	free(pAccumeData);
	
	vdDebug_LogPrintf(("last--CTOS_FileWrite,inresult[%d]", (char*)&inresult));
} 

int inCTOS_UpdateAccumPreCompTotal(TRANS_DATA_TABLE *transData)
{
	ACCUM_REC srAccumRec;
    BYTE        szTransAmt[12+1];
    BYTE        szTipAmt[12+1];
    BYTE        szOrgAmt[12+1];
    BYTE        szOrgTipAmt[12+1];
    int         inResult;
	int inTranCardType = 0;
    BYTE        szDCCBaseAmt[12+1];

    vdDebug_LogPrintf("inCTOS_UpdateAccumPreCompTotal");
	vdDebug_LogPrintf("byTransType=[%d]", transData->byTransType);
    
    memset(szTransAmt, 0x00, sizeof(szTransAmt));
    memset(szTipAmt, 0x00, sizeof(szTipAmt));
    memset(szOrgAmt, 0x00, sizeof(szOrgAmt));
    memset(szOrgTipAmt, 0x00, sizeof(szOrgTipAmt));

    wub_hex_2_str(transData->szTotalAmount, szTransAmt, 6);
    wub_hex_2_str(transData->szTipAmount, szTipAmt, 6);
    wub_hex_2_str(transData->szOrgAmount, szOrgAmt, 6);
    wub_hex_2_str(transData->szOrgTipAmount, szOrgTipAmt, 6);

   memcpy(szDCCBaseAmt, srTransRec.szForeignAmount, sizeof(szDCCBaseAmt));

    inTranCardType = 0;//save all card type to credit

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
        return ST_ERROR;    

    if(transData->byTransType == PREAUTH_COMP)
    {
        //srAccumRec.stBankTotal[inTranCardType].stCardTotal[transData->IITid].usSaleCount++;
        //srAccumRec.stBankTotal[inTranCardType].stCardTotal[transData->IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[transData->IITid].ulSaleTotalAmount + atol(szTransAmt);
        
        //srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
        //srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt); 		   

        srAccumRec.stBankTotal[inTranCardType].stCardTotal[transData->IITid].usCompCount++;
        srAccumRec.stBankTotal[inTranCardType].stCardTotal[transData->IITid].ulCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCompTotalAmount + atol(szTransAmt);
        
        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCompCount++;
        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCompTotalAmount + atol(szTransAmt); 			

	if (srTransRec.fDCCOptin == TRUE)
                  {                          
                       srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].usCompDCCCount++;
                       srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulCompTotalDCCAmount = srAccumRec.stBankTotal[inTranCardType].stDCCTotal[srTransRec.inCSTid].ulCompTotalDCCAmount + atol(szDCCBaseAmt);
                  }
	}
	
    if((inResult = inCTOS_SaveAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Save Total Rec. error");
        vdSetErrorMessage("Save Accum Error");
        return ST_ERROR;    
    }
    
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdMyEZLib_LogPrintf("total file saved successfully");
    return ST_SUCCESS;
}




