#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstZCLQ.c
* ����������    ������ȡ������Ϣ
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
* ��������    pub_base_CalIntstZCLQ()
* �������ܣ�  ������ȡ������Ϣ
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
*          1. ԭ������Ϣ��ʽ����ȷ����Ϊ���õ��ۻ�������Ϣ��ʽ�����������
*          �������յĻ������Ժ�ÿ��ȡ��۳�ȡ�����������ղ����Ļ�����
*          ���Կ���Ӧ�ü���������ŵ�intst_acm�ֶΡ�
*          2. ����ʱ������Ӧ��ʹ�õ����ռ�����Ϣ��
*
********************************************************************/
int pub_base_CalIntstZCLQ(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    long tx_date,ic_date,mtr_date;
    double  js99;    /*991101�Ļ���*/
	double  ye99;    /*991101�����*/
    double  intst_acm; 
    double  tx_amt;
    double  bal;
    long    tmprq,date99;
    long ac_id;
    int  ac_seqn;
    int ret;
    long dayterm;
    char intst_term_type[2];
    char llbh[4];
    char cur_no[3];
    double rate_val;
    double sdsl;
    double lx;
    double flt_ratio;
    char intst_tax_no[4];
    char rate_no[4];


sprintf(acErrMsg,"������ȡ��Ϣ");
WRITEMSG

    date99 = 19991101;  

    tx_date = g_pub_tx.tx_date;
    tx_amt = g_pub_tx.tx_amt1;

    ic_date = g_td_mst.ic_date;
    mtr_date = g_td_mst.mtr_date;  
    ac_id = g_td_mst.ac_id;
    ac_seqn = g_td_mst.ac_seqn;
    intst_acm = g_td_mst.intst_acm;
    flt_ratio = g_td_mst.flt_ratio;
    bal = g_td_mst.bal;

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

    MEMSET_DEBUG(cur_no,0x0,sizeof(cur_no));
    MEMSET_DEBUG(intst_tax_no,0x0,sizeof(intst_tax_no));
    MEMSET_DEBUG(rate_no,0x0,sizeof(rate_no));

    strcpy(cur_no,g_td_parm.cur_no);
    strcpy(intst_tax_no,g_td_parm.intst_tax_no);
    strcpy(rate_no,g_td_parm.rate_no);

    /*ȡ991101�յĻ��������*/
    if (ic_date >= date99)
    {
        js99 = 0.00;
        ye99 = 0.00;
    }
    else
    {
         /* ��ѯdate99֮ǰ�Ķ��ڴ����ϸ����js99 ye99��ֵ */

sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ac_id,ac_seqn);
WRITEMSG

         tmprq = date99;
         ret = pub_base_gettdmsthst(ac_id,ac_seqn,&tmprq,&js99,&ye99,reply);
         if (ret != 0)
         {
             sprintf(acErrMsg,"ȡ���ڴ����ϸ���� [%d]",ret);
             WRITEMSG
             return 1;
         }
    }   
    
sprintf(acErrMsg,"js99=[%lf],ye99=[%lf]",js99,ye99);
WRITEMSG

		/* begin.gyf.20060918 */
    if (tx_date == ic_date && g_pub_tx.svc_ind == 1000)
    {
        tx_date = mtr_date;  /* �����ռ��㵽����Ϣ */
    }
    /* end.gyf.20060918 */

    if (mtr_date > tx_date)
    {
        /* ��ǰ֧ȡ */
 		ret=pub_base_CalIntstDays(tx_date,mtr_date,intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
            return 1;
        }
        dayterm = dayterm + pub_base_tuneday(tx_date,ic_date,mtr_date);

sprintf(acErrMsg,"+++++++++intst_acm=[%lf]+++++++++++++",intst_acm);
WRITEMSG
        /***begin.gyf.20060918 ������ר�ź����������ﲻ�ô���
        intst_acm -= dayterm * tx_amt;
            end.gyf.20060918 ***/
        
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
            strcpy(reply,"W110");
            return ret;

        }

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio);
WRITEMSG

        lx = rate_val * intst_acm * (1 + flt_ratio) / L360 / L100;

        if (ic_date >= date99)
        {
            (*val) = lx;
        }
        else
        {
            ret = pub_base_CalIntstDays(date99,mtr_date,
                  g_td_parm.intst_term_type,&dayterm);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"������������!!!");
            	WRITEMSG
            	return 1;
        	}

sprintf(acErrMsg,"dayterm=[%ld],js99=[%lf],ye99=[%lf],lx=[%lf],intst_acm=[%lf]",
        dayterm,js99,ye99,lx,intst_acm);
WRITEMSG

            (*val) = lx *(1 - (js99 -ye99 * dayterm) / intst_acm);
        }

        if (strcmp(intst_tax_no,SLBH) != 0)
        {
            (*val) = 0.00;
        }
        (*dealval) = pub_base_PubDround_1(lx);
        (*val) = pub_base_PubDround_1(*val);
    }
    else
    {
        /* ����֧ȡ */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            strcpy(reply,"W110");
            return ret;
        }

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio);
WRITEMSG

        lx = intst_acm * rate_val * (1 + flt_ratio) / L360 / L100;       
        
        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            if (ic_date >= date99)
            {
                (*val) = lx;
            }
            else
            {
                ret = pub_base_CalIntstDays(date99,mtr_date,
                  	intst_term_type,&dayterm);
        		if (ret != 0)
        		{
            		sprintf(acErrMsg,"������������!!!");
            		WRITEMSG
            		return 1;
        		}

sprintf(acErrMsg,"dayterm=[%ld],js99=[%lf],ye99=[%lf],lx=[%lf],intst_acm=[%lf]",
        dayterm,js99,ye99,lx,intst_acm);
WRITEMSG

                (*val) = lx * (1 - (js99 - ye99 * dayterm) / intst_acm);
            }
        }
        
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
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

sprintf(acErrMsg,"dayterm=[%ld],rate_val=[%lf],bal=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,bal,flt_ratio);
WRITEMSG

        lx += bal * dayterm * rate_val * (1 + flt_ratio) / L360 / L100;

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(PubMax(date99,mtr_date),tx_date,
                intst_term_type,&dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                return 1;
            }

sprintf(acErrMsg,"dayterm=[%ld],rate_val=[%lf],bal=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,bal,flt_ratio);
WRITEMSG

            (*val) += bal *dayterm * rate_val * (1 + flt_ratio) / L360 / L100;
        }
    }

    lx = pub_base_PubDround_1(lx);
    (*dealval) = lx;

    /* ȡ��Ϣ˰˰�� */
    ret = pub_base_GetRate(SLBH, tx_date, &sdsl);
    if ((ret != 0) && (ret == 1))
    {
        sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
        WRITEMSG
        strcpy(reply,"W100");
        return 1;
    }
    else if(ret < 0)
    {
        sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ", ret);
        WRITEMSG
        strcpy(reply,"W101");
        return 1;
    }

	g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
    (*val) = (*val) * sdsl;
    (*val) = pub_base_PubDround_1(*val);
    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

    return 0;
}
