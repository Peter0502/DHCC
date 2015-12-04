#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstDHLB.c
* ����������    �������������Ϣ
* ���ú���˵����
*
*
* ��    ��:     rob
* ������ڣ�    2003��12��28��
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
* ��������    pub_base_CalIntstDHLB()
* �������ܣ�  �������������Ϣ
* ����/ʱ�䣺 2003��12��28��
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
int pub_base_CalIntstDHLB(double *dealval, double *factval,
       double *val, double * keepval, char *reply)
{   
    char  intst_term_type[2] ;/* ��Ϣ�������� */
    long  dayterm,dayterm1; 
    int monterm;
    char llbh[4];  /* ���ʱ�� */
    char cur_no[3];   /* ���� */
    double rate_val,rate_val1; /* ����ֵ */
    double lx ,kslx;      /* ��Ϣֵ */
    long tx_date;    /* �������� */
    long ic_date;    /* ��Ϣ���� */
    double tx_amt;   /* ������ */
    double flt_ratio; /* �������� */
    double sdsl;      
    long date99,date9303,date9201,date8910;
    int ret;
   
    MEMSET_DEBUG(cur_no, 0x0, sizeof(cur_no));
    strcpy(cur_no, g_td_parm.cur_no);
    tx_date = g_pub_tx.tx_date;
    tx_amt  = g_pub_tx.tx_amt1;
    flt_ratio = g_td_mst.flt_ratio;
    ic_date = g_td_mst.ic_date;
    date99 = 19991101;
    date9303 = 19930301;
    date9201 = 19920101;
    date8910 = 19891001;
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val) = 0.00;
    (*keepval) = 0.00;
    
sprintf(acErrMsg,"tx_amt=[%lf],[%lf]",tx_amt,g_pub_tx.tx_amt1);
WRITEMSG
    if (ic_date == 0 || tx_date == 0)
    {
        sprintf(acErrMsg,"��Ϣ��[%ld]������[%ld]Ϊ���޷���Ϣ",
                ic_date, tx_date);
        WRITEMSG
        MEMSET_DEBUG(reply, 0x0, sizeof(reply));
        strcpy(reply, "W112");
        return 1; 
    }

		strcpy(intst_term_type,g_td_parm.intst_term_type);/***��ʵ��������Ϣ***/
    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
	{
	    sprintf(acErrMsg,"��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
	            g_td_parm.intst_term_type );
	    WRITEMSG
	    MEMSET_DEBUG(reply, 0x0, sizeof(reply));
	    strcpy(reply, "W107");
	    return 1;
	}

    ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"������������!!");
        WRITEMSG
        return 1;
    }
    
    monterm = dayterm / 30;
    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
    
sprintf(acErrMsg,"��ʼ���㶨��������Ϣ");
WRITEMSG
    if (g_td_mst.ic_date >= date9303)
    {
        if (monterm < 3)
        {
            strcpy(llbh,HQLLBH);
        }      
        else if (monterm < 6)
        {
            strcpy(llbh,ZCZQ03);
            
        }
        else if (monterm < 12)
        {
            strcpy(llbh,ZCZQ06);
        }
        else 
        {
            strcpy(llbh,ZCZQ12);
        }
 sprintf(acErrMsg,"llbh =  [%s]",llbh);
 WRITEMSG

        ret = pub_base_getllz(HQLLBH,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }

sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG

        if (strcmp(llbh,HQLLBH) != 0)
        {
            ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val1);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply,"W110");
                return ret;
            }
            rate_val1 = rate_val1 * 0.60; 
            if (rate_val <= rate_val1)
            {
                rate_val = rate_val1;
            }
        }
sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG
    }
    else if (g_td_mst.ic_date >= date9201)
    {
    }
    else if (g_td_mst.ic_date >= date8910)
    {
    }
    else 
    {
    }
    
sprintf(acErrMsg,"day =[%d],rate_val=[%f] tx_amt=[%lf],flt_ratio=[%f]",
       dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

    lx = dayterm * rate_val * floor(tx_amt*L100)/L100 * (1 + flt_ratio) / L360 / L100;
    lx = pub_base_PubDround_1(lx);

sprintf(acErrMsg,"��Ϣlx =[%lf]", lx);
WRITEMSG
    
    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
    {
		ret=pub_base_CalIntstDays(PubMax(ic_date,date99),tx_date,
			intst_term_type,&dayterm1);
	    if (ret != 0)
	    {
	        sprintf(acErrMsg,"������������!!");
	        WRITEMSG
	        return 1;
	    }
    
        kslx = lx * dayterm1 / dayterm;
        
        ret = pub_base_GetRate(SLBH, tx_date, &sdsl);
        if (ret != 0 && ret == 1)
        {
             sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
             WRITEMSG
             MEMSET_DEBUG(reply, 0x0, sizeof(reply));
             strcpy(reply,"W100");
             return 1;
        }
        else if (ret < 0)
        {
             sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d",ret);
             WRITEMSG
             strcpy(reply,"W101");
             return 1;
        }

		g_pub_intst.tax_intst = pub_base_PubDround_1(kslx);
        kslx = pub_base_PubDround_1(kslx * sdsl);
        (*val) = kslx;
    }
    (*dealval) = lx;
    (*factval) = (*dealval) - (*val);

WRITEMSG
sprintf(acErrMsg, "------>Ӧ˰��Ϣtax_intst: [%lf]", g_pub_intst.tax_intst);
WRITEMSG
        /* added by liuxuan 2007-08-01 �޸���Ϣ˰ */
	if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
		char edu_ind[2];
		vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����\n",__FILE__,__LINE__);
		if(iGetAreaTax(val,factval,dealval,intst_term_type,6,rate_val,rate_val,edu_ind)){
			vtcp_log("[%s][%d]���÷ֶ�����Ϣ˰��������\n",__FILE__,__LINE__);
			return(1);
		}
	}

sprintf(acErrMsg,"���÷ֶ�����Ϣ˰������dealval =[%lf],val=[%lf] fact_val=[%lf]",
       *dealval,*val,*factval);
	/* add over */
    return 0;
}
