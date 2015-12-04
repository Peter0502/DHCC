#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstCBQX.c
* ����������    �汾ȡϢ������Ϣ
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
* ��������    pub_base_CalIntstCBQX()
* �������ܣ�  �汾ȡϢ������Ϣ
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
int pub_base_CalIntstCBQX(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    double rate_val;      /* ����ֵ */
    double sdsl;          /* ˰��ֵ */
    double lx;            /* ��Ϣ */
    double flt_ratio;     /* �������� */
    double tx_amt;        /* ������ */
    char intst_term_type[2]; /* ��Ϣ�������� */
    char llbh[4];         /* ���ʱ�� */
    char rate_no[4];      /* ���ʱ�� */
    char cur_no[3];       /* ���� */
    char intst_tax_no[4]; /*˰�ʱ�� */
    long tx_date;         /* �������� */
    long ic_date,mtr_date; /* ��Ϣ���� �������� */
    long date99;
    int ret;
    int tx_cnt;            /* ��ȡϢ���� */
    double td_amt;         /* ÿ��ȡϢ�� */
    long dayterm;
   
    date99 = 19991101;
    tx_date = g_pub_tx.tx_date;
    tx_amt = g_pub_tx.tx_amt1;
  
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*keepval) = 0.00;
    (*val)   = 0.00;

    mtr_date = g_td_mst.mtr_date;
    ic_date  = g_td_mst.ic_date;
    flt_ratio = g_td_mst.flt_ratio;
    tx_cnt = g_td_mst.tx_cnt;
    td_amt = g_td_mst.td_amt;

    strcpy(cur_no, g_td_parm.cur_no);
    strcpy(intst_tax_no,g_td_parm.intst_tax_no);
    strcpy(rate_no, g_td_parm.rate_no);

    strcpy(intst_term_type,g_td_parm.intst_term_type);

	if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
         sprintf(acErrMsg,"��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
                        g_td_parm.intst_term_type );
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }

    if (tx_date == ic_date && g_pub_tx.svc_ind == 1000)
    {
        tx_date = mtr_date;  /* �����ռ��㵽����Ϣ */
        td_amt = 0.00;
        tx_cnt = 0;
    }

sprintf(acErrMsg,"����汾ȡϢ��Ϣmtr_date=[%ld],tx_date=[%ld]",
        mtr_date,tx_date);
WRITEMSG

    if (mtr_date > tx_date)
    {

sprintf(acErrMsg,"�汾ȡϢ--��ǰ֧ȡ");
WRITEMSG

        /* ��ǰ֧ȡ */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld] ",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rate����ȡ�������ʴ���",ret);
            WRITEMSG
            MEMSET_DEBUG(reply,0x0,sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }

		ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
	    if (ret != 0)
	    {
	        sprintf(acErrMsg,"������������!!");
	        WRITEMSG
	        return 1;
	    }


sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

        lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
        (*dealval) = pub_base_PubDround_1(lx);
           

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			ret=pub_base_CalIntstDays(PubMax(date99,ic_date),tx_date,
	            intst_term_type,&dayterm);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"������������!!");
	            WRITEMSG
	            return 1;
	        }

            (*val) =dayterm * rate_val * tx_amt * (1 + flt_ratio)/L360/L100;
			g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
        }

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    }
    else
    {

sprintf(acErrMsg,"�汾ȡϢ--��������֧ȡ");
WRITEMSG

        /* ���㵽�ڻ�������Ϣ */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,rate_no);

sprintf(acErrMsg,"llbh = [%s]",llbh);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,ic_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rate����ȡ���ʴ���",ret);
            WRITEMSG
            MEMSET_DEBUG(reply,0x0,sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }

		ret=pub_base_CalIntstDays(ic_date,mtr_date,intst_term_type,&dayterm);
	    if (ret != 0)
	    {
	        sprintf(acErrMsg,"������������!!");
	        WRITEMSG
	        return 1;
	    }


sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

        lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
        (*dealval) = lx;
    
        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			ret=pub_base_CalIntstDays(PubMax(ic_date,date99),mtr_date,
	            intst_term_type,&dayterm);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"������������!!");
	            WRITEMSG
	            return 1;
	        }

            lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
            (*val) = lx;
			g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
        }    
       
sprintf(acErrMsg,"���ڣ�dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh = [%s] ",llbh);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rate����ȡ���ʴ���",ret);
            WRITEMSG
            MEMSET_DEBUG(reply,0x0,sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }
        
		if (g_td_parm.ovrd_intst_ind[0] == 'Y')
		{
			ret=pub_base_CalIntstDays(mtr_date,tx_date,
				intst_term_type,&dayterm);
	        if (ret != 0)
	        {
	            sprintf(acErrMsg,"������������!!");
	            WRITEMSG
	            return 1;
	        }
        }
        else
        {
			dayterm = 0;
		}

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

        lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
        (*dealval) += lx;
     
        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			if (g_td_parm.ovrd_intst_ind[0] == 'Y')
            {
				ret=pub_base_CalIntstDays(PubMax(mtr_date,date99),tx_date, 
					intst_term_type,&dayterm);
	        	if (ret != 0)
	        	{
	            	sprintf(acErrMsg,"������������!!");
	            	WRITEMSG
	            	return 1;
	        	}
            }
            else
            {
                dayterm = 0;
            }

            lx = dayterm * rate_val * tx_amt * (1 + flt_ratio) / L360 / L100;
            (*val) += lx;
			g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
        }   

sprintf(acErrMsg,"����+���ڣ�dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    }

    (*dealval) = pub_base_PubDround_1(*dealval);

    
    ret = pub_base_GetRate(SLBH,tx_date,&sdsl);
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

sprintf(acErrMsg,"td_amt = [%lf],tx_cnt=[%d]", td_amt,tx_cnt);
WRITEMSG

    (*dealval) -= td_amt * tx_cnt;
	g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
    (*val) = (*val) * sdsl ;
    (*val)  = pub_base_PubDround_1(*val);
    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG
    
    return 0;
}
