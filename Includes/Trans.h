/* 
 * File:   TransFunc.h
 * Author: Administrator
 *
 * Created on 2012年8月23日, 下午 6:35
 */

#ifndef TRANSFUNC_H
#define	TRANSFUNC_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct  
{
    USHORT usSaleCount;
    ULONG  ulSaleTotalAmount;
    USHORT usRefundCount;
    ULONG  ulRefundTotalAmount;
    USHORT usTipCount;
    ULONG  ulTipTotalAmount;   
    USHORT usVoidSaleCount;
    ULONG  ulVoidSaleTotalAmount;
    USHORT usOffSaleCount;
    ULONG  ulOffSaleTotalAmount; 

    USHORT usVoidRefundCount;
    ULONG  ulVoidRefundTotalAmount;	

    USHORT usRedeemCount;
    ULONG  ulRedeemTotalAmount; 
    USHORT usVoidRedeemCount;
    ULONG  ulVoidRedeemTotalAmount;

    USHORT usRegularCount;
    ULONG  ulRegularTotalAmount; 
    USHORT usVoidRegularCount;
    ULONG  ulVoidRegularTotalAmount;

    USHORT usReduceCount;
    ULONG  ulReduceTotalAmount; 
    USHORT usVoidReduceCount;
    ULONG  ulVoidReduceTotalAmount;
	
    USHORT usZeroCount;
    ULONG  ulZeroTotalAmount; 
    USHORT usVoidZeroCount;
    ULONG  ulVoidZeroTotalAmount;

    USHORT usBNPLCount;
    ULONG  ulBNPLTotalAmount; 
    USHORT usVoidBNPLCount;
    ULONG  ulVoidBNPLTotalAmount;

// CASH2GO
    USHORT us2GOCount;
    ULONG  ul2GOTotalAmount; 
    USHORT usVoid2GOCount;
    ULONG  ulVoid2GOTotalAmount;


// cash advance
    USHORT usCashCount;
    ULONG  ulCashTotalAmount; 
    USHORT usVoidCashCount;
    ULONG  ulVoidCashTotalAmount;

// sale completion
    USHORT usCompCount;
    ULONG  ulCompTotalAmount; 
    USHORT usVoidCompCount;
    ULONG  ulVoidCompTotalAmount;

   // DCC 
   USHORT usDCCCount;
   ULONG  ulSaleDCCTotalAmount; 
    USHORT usRefundDCCCount;
    ULONG  ulRefundTotalDCCAmount;
    USHORT usTipDCCCount;
    ULONG  ulTipTotalDCCAmount;   
    USHORT usVoidSaleDCCCount;
    ULONG  ulVoidSaleTotalDCCAmount;
    USHORT usOffSaleDCCCount;
    ULONG  ulOffSaleTotalDCCAmount; 
    USHORT usCompDCCCount;
    ULONG  ulCompTotalDCCAmount; 


	
}STRUCT_TOTAL;

typedef struct
{
    STRUCT_TOTAL stCardTotal[20];
    STRUCT_TOTAL stHOSTTotal;
    STRUCT_TOTAL stDCCTotal[40];
    USHORT usEMVTCCount;
}TRANS_TOTAL;


BYTE szOriginTipTrType;


typedef struct
{
    STRUCT_TOTAL stSettledCardTotal[10];
    BYTE	szSettledCardLabel[10][25] ;
    BYTE        szSettledDate[3+1];
    BYTE	szSettledBatchNo[3+1];
    BYTE        szSettledTime[3+1];
    BYTE	szSettledHostLabel[16] ;
    BYTE	szSettledMID[15+1];
    BYTE	szSettledTID[8+1];
}SETTLEDTRANS_TOTAL;



#ifdef	__cplusplus
}
#endif

#endif	/* TRANSFUNC_H */

