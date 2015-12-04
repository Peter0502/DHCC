#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intst_acm.c
* ����������    
*              1��pub_base_CalcIntstAcm������Ϣ����(��ÿ��ʵ����������)��
*              2��pub_base_CalcIntstAcm1������Ϣ����(��ÿ��30�����,����
*		  �����ʻ���Ϣ����,����Ҳ�ͬ)��
*              3��pub_base_CalcIntstAcm2������Ϣ����(��ÿ��30�����)��
*              4��pub_base_CalcIntstAcm3������Ϣ����(��ʵ���������㣬���
*		  ȡ����������)��
*
* ���ú���˵����pub_base_CalDays  ����ʵ��������(ԭPubJsts)
*               pub_base_CalTriDays ������������ÿ��30�����(ԭPubTs)
*          
* ��    ��:    
* ������ڣ�    2003��12��17��
*
* �޸ļ�¼��    
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#define loc_t sys_loc_t

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define EXTERN
#include "public.h"

/**********************************************************************
* ��������    pub_base_CalcIntstAcm(ԭCalLxjs)
* �������ܣ�  ������Ϣ����(��ÿ��ʵ����������)
* ����/ʱ�䣺 2003��12��17��
* 
* ������
*     ���룺  
*            double *pdAccInt : ��Ϣ����
*            long lLastDate   : �ϴ����䶯����
*            long lCurrDate   : �������䶯����
*            double dLastBal  : �ϴ����
*           
*     ����� double *pdAccInt : ��Ϣ����
*
* �� �� ֵ: 0 �ɹ�
*           
* �޸���ʷ��
*
********************************************************************/

int pub_base_CalcIntstAcm( double *pdAccInt, long lLastDate, long lCurrDate, double dLastBal, char xym[] )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;

    dOldAccInt = *pdAccInt;

    /* �������� */
    dCrassDate = (double)pub_base_CalDays(lLastDate,lCurrDate);
    if ( dCrassDate < 0 ) 
    {
        dCrassDate = 0.00;
        strcpy(xym,"0000");
    }

    /* ���������ӵĻ��� */
    dTmpAccInt = dLastBal * dCrassDate;
    /* ԭ�л���+�������� �� ��ǰ���� */
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;

    return 0;
}

/**********************************************************************
* ��������    pub_base_CalcIntstAcm1(ԭCalLxjs1)
* �������ܣ�  ������Ϣ����(��ÿ��30�����,��������ʻ���Ϣ����,����Ҳ�ͬ)
* ����/ʱ�䣺 2003��12��17��
* 
* ������
*     ���룺 
*            double *pdAccInt : ��Ϣ����
*            long lLastDate   : �ϴ����䶯����
*            long lCurrDate   : �������䶯����
*            double dLastBala : �ϴ����
*           
*     ����� double *pdAccInt : ��Ϣ����
*
* �� �� ֵ: 0 �ɹ� ��1 ʧ��
*           
* �޸���ʷ��
*
********************************************************************/

int pub_base_CalcIntstAcm1( double *pdAccInt, long lCurrDate, long lLastDate, double dLastBal, char pubjz_bz1[], char xym[] )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;
    long grjxr;            /* ���˼�Ϣ���� */
    char grhjxr[5];
    struct  ggxtcs_c    xtcs;

    dOldAccInt = *pdAccInt;
    dCrassDate = 0;
    
    if ( lLastDate == 0 )
    {
        dCrassDate = 1;
        /* ȡϵͳ���� */
        if (PubQxtcs(&xtcs,xym) )
        {
            return(1);
        }
        
        /* ȡ���˼�Ϣ���ڣ�����Ҳ�ͬ */
        if ( !strcmp(pubjz_bz1,RMB) )
        {
            strncpy(grhjxr,xtcs.grhjxrq+2,4);
        }else
        {
            strncpy(grhjxr,xtcs.wbgrhjxrq+2,4);
        }
        
        grhjxr[4]='\0';
        lLastDate  = lCurrDate/10000;
        lLastDate  = lLastDate*10000+atol(grhjxr);
        
        if (lLastDate < lCurrDate)
        {
            lLastDate += 10000; 
        }
    }
    
    dCrassDate += (double)pub_base_CalTriDays(lCurrDate,lLastDate) ;
    dTmpAccInt = floor(dLastBalL100)/L100 * dCrassDate;
    sprintf(acErrMsg,"dOldAccInt=[%lf],lLastDate=[%ld],lCurrDate=[%ld],dCrassDate=[%f],dLastBala=[%f],dTmpAccInt=[%f],dNewAccInt=[%f]",dOldAccInt,lLastDate,lCurrDate,dCrassDate,dLastBala,dTmpAccInt,dNewAccInt);
    WRITEMSG
    
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;
    return (0);
}

/**********************************************************************
* ��������    pub_base_CalcIntstAcm2(ԭCalLxjs2)
* �������ܣ�  ������Ϣ����(��ÿ��30�����)
* ����/ʱ�䣺 2003��12��17��
* 
* ������
*     ���룺 
*            double *pdAccInt : ��Ϣ����
*            long lCurrDate   : �������䶯����
*            double dLastBal  : �ϴ����
*           
*     ����� double *pdAccInt : ��Ϣ����
*
* �� �� ֵ: 0 �ɹ�
*           
* �޸���ʷ��
*
********************************************************************/

int pub_base_CalcIntstAcm2( double *pdAccInt, long lCurrDate, double dLastBal )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;
    long lLastDate;                    /*��Ϣ��*/

    dOldAccInt = *pdAccInt;
    dCrassDate = (double)PubTs(lCurrDate,lLastDate);
    if ( dCrassDate < 0 )
    {
        dCrassDate = 0.00;
    }
    
    dTmpAccInt = dLastBal * dCrassDate;
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;
    return(0);
}

/**********************************************************************
* ��������    pub_base_CalcIntstAcm3(CalLxjs3)
* �������ܣ�  ������Ϣ����(��ʵ���������㣬���ȡ����������)
* ����/ʱ�䣺 2003��12��17��
* 
* ������
*     ���룺 
*            double *pdAccInt : ��Ϣ����
*            long lLastDate   : �ϴ����䶯����
*            long lCurrDate   : �������䶯����
*            double dLastBal  : �ϴ����
*           
*     ����� double *pdAccInt : ��Ϣ����
*
* �� �� ֵ: 0 �ɹ�
*           
* �޸���ʷ��
*
********************************************************************/

int pub_base_CalcIntstAcm3( double *pdAccInt, long lLastDate, long lCurrDate, double dLastBal )
{
    double dOldAccInt, dNewAccInt;
    double dCrassDate, dTmpAccInt;

    dOldAccInt = *pdAccInt;

    dCrassDate = (double)pub_base_CalDays(lLastDate,lCurrDate);
    if ( dCrassDate < 0 )
    {
        dCrassDate = 0.00;
    }

    /* �����ȡ���������� */
    dTmpAccInt = floor(dLastBal*L100)/L100 * dCrassDate;
    dNewAccInt = dOldAccInt + dTmpAccInt;

    *pdAccInt = dNewAccInt;

    return(0);
}
