#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstin.c
* ����������    �ڲ��ʻ�������Ϣ
* ���ú���˵����
*
*
* ��    ��:     rob
* ������ڣ�    2003��03��12��
*
* �޸ļ�¼��
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"


/**********************************************************************
* ��������    pub_base_CalIntstIn()
* �������ܣ�  �ڲ��ʻ�������Ϣ
* ����/ʱ�䣺 2003��03��12��
*
* ������
*     ���룺
*     ���: Ӧ����Ϣ          double
*           ʵ����Ϣ          double
*           ������Ϣ˰        double
*           ��ֵ��Ϣ          double
*           ��Ӧ��            char(4)
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_CalIntstIn(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    double lxjs;          /* ��Ϣ���� */   
    double rate_val;      /* ����ֵ */
    double lx;            /* ��Ϣ */
    int ret;

    long tx_date;

    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val)   = 0.00;
    (*keepval) = 0.00;   
    
    lxjs = g_in_mst.intst_acm;
    tx_date = g_pub_tx.tx_date;
  
sprintf(acErrMsg,"FFFFFFFFFFF intst_acm = [%lf]", g_in_mst.intst_acm);
WRITEMSG
    
    if (g_pub_tx.tx_date > g_in_mst.end_date)
    {
        rate_val = g_in_mst.over_rate;
    }
    else
    {
        rate_val = g_in_mst.rate;
    }

sprintf(acErrMsg,"rate_val=[%lf],lxjs=[%lf]", rate_val,lxjs);
WRITEMSG

    lx = rate_val * lxjs / L360 / L100;
            
    (*dealval) = pub_base_PubDround_1(lx);

    if (*dealval < 0)
    {
        (*dealval) = 0.00;
        (*factval) = 0.00;
    }
            
    (*val)     = 0.00;
    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    return 0;

}
