#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstTZCK.c
* ����������    ֪ͨ��������Ϣ
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
#include "mo_infrm_c.h"

/**********************************************************************
* ��������    pub_base_CalIntstTZCK()
* �������ܣ�  ֪ͨ��������Ϣ
* ����/ʱ�䣺 2003��12��28��
*
* ������
*     ���룺
*           ģʽ              int
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
int pub_base_CalIntstTZCK(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    struct mo_infrm_c mo_infrm_c,mo_infrm_c1;
    int ret=0;
    long ac_id=0;
    int ac_seqn=0;
    int found = 1;  /* 0 δ��֧ȡ, 1 �еǼǵ�֧ȡ */
    char sts[2]; /* ״̬  0 ֪ͨ�Ǽ�  1��ǰ֧ȡ  2����֧ȡ  3����֧ȡ 
                          4 ȡ���Ǽ�  5δ��֧ȡ  6δ֧ȡ   */
    char intst_term_type[2];
    long dayterm=0;
    int flag=0;       /* 0 - 1��֪ͨ���, 1 - 7��֪ͨ��� */
    int ttl_cnt=0;    /* ��ΥԼ���� */
    char llbh[4];
    char cur_no[3];
    char intst_tax_no[4];
    long tx_date=0;
    long ic_date=0;
    long date99=0;
    long trace_no=0;
    double rate_val=0.00, rate_val1=0.00;
    double flt_ratio=0.00;
    double sdsl=0.00;
    double tx_amt=0.00;
    double lx=0.00;
    double draw_min_amt=0.00;  /* ���֧ȡ��� */


    date99 = 19991101;
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val) = 0.00;
    (*keepval) = 0.00;

    tx_date = g_pub_tx.tx_date;
    tx_amt = floor(g_pub_tx.tx_amt1 * 100) / 100;
    trace_no = g_pub_tx.trace_no;

    ac_id = g_td_mst.ac_id;
    ac_seqn = g_td_mst.ac_seqn;
    ic_date = g_td_mst.ic_date;
    flt_ratio = g_td_mst.flt_ratio;
    
    draw_min_amt = g_td_parm.draw_min_amt;
    MEMSET_DEBUG(cur_no,0x0,sizeof(cur_no));
    MEMSET_DEBUG(intst_tax_no,0x0,sizeof(intst_tax_no));
    strcpy(cur_no, g_td_parm.cur_no);
    strcpy(intst_tax_no,g_td_parm.intst_tax_no);
 
    sprintf(acErrMsg,"֪ͨ�����Ϣ����");
    WRITEMSG

	if ( strcmp(g_td_mst.prdt_no, "242") == 0 || \
		 strcmp(g_td_mst.prdt_no, "281") == 0)
	{
		flag = 0;
	}
	if ( strcmp(g_td_mst.prdt_no, "243") == 0 || \
		 strcmp(g_td_mst.prdt_no, "282") == 0)
	{
		flag = 1;
	}

	strcpy(intst_term_type,g_td_parm.intst_term_type);/***��ʵ��������Ϣ***/
    if (g_td_parm.intst_term_type[0]!='0' && g_td_parm.intst_term_type[0]!='1')
    {
         sprintf(acErrMsg,"���ڲ�Ʒ���еļ�Ϣ�������ʹ��� type=[%c]",
                        intst_term_type );
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }


    /* ��ѯ֪ͨ�Ǽǲ� */
    MEMSET_DEBUG(sts,0x0,sizeof(sts));
    strcpy(sts,"0");

sprintf(acErrMsg,"ac_id = [%ld],ac_seqn=[%d]",ac_id,ac_seqn);
WRITEMSG

    ret = Mo_infrm_Dec_Sel(reply," ac_id = %ld and ac_seqn = %d and sts='0' order by infrm_date ", ac_id, ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Declare for Select error sqlcode=[%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(acErrMsg,"ȡ֪ͨ�Ǽǲ�����һ����¼");
    WRITEMSG

    while(1)
    {
        MEMSET_DEBUG(&mo_infrm_c1,0x0,sizeof(struct mo_infrm_c));
        ret = Mo_infrm_Fet_Sel(&mo_infrm_c1, reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"Fetch for update error sqlcode=[%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        MEMSET_DEBUG(&mo_infrm_c,0x0,sizeof(struct mo_infrm_c));

		sprintf(acErrMsg, "**************[%s]", mo_infrm_c1.sts);
		WRITEMSG

        MEMCPY_DEBUG(&mo_infrm_c,&mo_infrm_c1,sizeof(struct mo_infrm_c));
       
sprintf(acErrMsg,"DDDDD ac_id=[%ld] [%ld]",mo_infrm_c.ac_id,mo_infrm_c1.ac_id);
WRITEMSG

    }

    Mo_infrm_Clo_Sel();

	ttl_cnt = 0;
	if (g_pub_intst.mode == 1)
	{
		ttl_cnt = sql_count("mo_infrm", "ac_id=%ld and ac_seqn=%ld and \
				sts='4'", ac_id, ac_seqn);
		if (ttl_cnt < 0)
		{
			sprintf(acErrMsg, "sql_count error !! [%d]", ttl_cnt);
			WRITEMSG
			return ret;
		}

	}
	else
	{
		ttl_cnt = 0;
	}

sprintf(acErrMsg,"ȡ֪ͨ�Ǽǲ���¼");
WRITEMSG

    if (mo_infrm_c.ac_id != ac_id)
    {
        found = 0; 

sprintf(acErrMsg,"֪ͨ�Ǽǲ�û�м�¼");
WRITEMSG

    } 
    else
    {

sprintf(acErrMsg,"֪ͨ�Ǽǲ��м�¼");
WRITEMSG

        found = 1;
        dayterm = pub_base_CalTrueDays(mo_infrm_c.infrm_date,
                  mo_infrm_c.prm_draw_date);
        if (dayterm < 0)
        {
            sprintf(acErrMsg,"Լ��֧ȡ������֪ͨ����֮ǰ");
            WRITEMSG
            return 1;
        }
sprintf(acErrMsg,"֪ͨ�Ǽǲ��м�¼11111 ֪ͨ����: [%ld]", dayterm);
WRITEMSG

        if (dayterm < 7)
        {
            flag = 0; 
        }
        else
        {
            flag = 1;
        }

sprintf(acErrMsg,"֪ͨ�Ǽǲ��м�¼,֪ͨ����=[%d]",flag*6+1);
WRITEMSG

    }

sprintf(acErrMsg,"found = [%d]",found);
WRITEMSG

    MEMSET_DEBUG(sts,0x0,sizeof(sts));
    strcpy(sts,"0");

    if (found == 1)
    {

sprintf(acErrMsg,"dec for update!!");
WRITEMSG

        ret = Mo_infrm_Dec_Upd(reply," ac_id = %ld and ac_seqn = %d \
              and sts = '%s' and infrm_date = %ld ", ac_id, ac_seqn,
              sts,mo_infrm_c.infrm_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"Declare for Select error sqlcode=[%d]",ret);
            WRITEMSG
            return 1;
        }

        MEMSET_DEBUG(&mo_infrm_c,0x0,sizeof(struct mo_infrm_c));
        ret = Mo_infrm_Fet_Upd(&mo_infrm_c, reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"Fetch for update error sqlcode=[%d]",ret);
            WRITEMSG
            return ret;
        }

    }

    if (found == 0) /* δ��֧ȡ */
    {

sprintf(acErrMsg,"δ��֧ȡ");
WRITEMSG

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
        ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
            return 1;
        }

sprintf(acErrMsg,"rate_val=[%lf],day=[%d],tx_amt=[%lf],flt_ratio=[%lf]",
       rate_val,dayterm,tx_amt,flt_ratio);
WRITEMSG

		if (g_pub_intst.mode == 1 && ttl_cnt != 0)
		{
			/* ������Ϣ */
			dayterm = dayterm - (flag * 6 + 1);
		}
sprintf(acErrMsg,"rate_val=[%lf],day=[%d],tx_amt=[%lf],flt_ratio=[%lf], flag=[%d]",
       rate_val,dayterm,tx_amt,flt_ratio, flag);
WRITEMSG

       	lx = rate_val * dayterm * tx_amt * (1 + flt_ratio) / L360 / L100;

sprintf(acErrMsg,"intst_tax_no=[%s], lx=[%lf]",intst_tax_no, lx);
WRITEMSG

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
			ret=pub_base_CalIntstDays(PubMax(ic_date,date99),tx_date,
                intst_term_type,&dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                return 1;
            }

            (*val) = rate_val * dayterm * tx_amt * (1+flt_ratio) / L360 / L100;
        }

        (*dealval) = pub_base_PubDround_1(lx);
        
        /* ��֪ͨ�Ǽǲ�������һ����¼ */
        MEMSET_DEBUG(&mo_infrm_c,0x0,sizeof(struct mo_infrm_c));
        mo_infrm_c.ac_id = ac_id;
        mo_infrm_c.ac_seqn = ac_seqn;
        mo_infrm_c.infrm_date = 0;
        mo_infrm_c.prm_draw_date = 0;
        mo_infrm_c.prm_draw_amt = 0.00;
        mo_infrm_c.fct_wd_amt = tx_amt;
        mo_infrm_c.occ_date = tx_date;
        mo_infrm_c.sts[0] = '5';
        mo_infrm_c.sts[1] = '\0';
        mo_infrm_c.wrk_date = tx_date;
        mo_infrm_c.trace_no = trace_no;

sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d],tx_amt=[%lf],tx_date=[%ld],\
        trace_no=[%ld]",ac_id,ac_seqn,tx_amt,tx_date,trace_no);
WRITEMSG

        ret = Mo_infrm_Ins(mo_infrm_c,reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��֪ͨ�Ǽǲ��в������ݴ���sqlcode=[%d]",ret);
            WRITEMSG
            return ret;
        }       

    }  /* end of δ��֧ȡ */
    else  /* �еǼǵ�֧ȡ */
    {

sprintf(acErrMsg,"�еǼ�֧ȡ");
WRITEMSG

        if (mo_infrm_c.prm_draw_date != tx_date) /* ��ǰ������֧ȡ */
        {

sprintf(acErrMsg,"��ǰ������֧ȡ");
WRITEMSG

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
			ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                return 1;
            }

			if (g_pub_intst.mode == 1)
            	dayterm = dayterm - (flag * 6 + 1); /* ֪ͨ�����ڲ���Ϣ */

			if (dayterm < 0)
				dayterm = 0;

sprintf(acErrMsg,"rate_val=[%lf],dayterm=[%ld],tx_amt=[%lf],flt_ratio=[%lf]",
       rate_val,dayterm,tx_amt,flt_ratio);
WRITEMSG

            lx = rate_val * dayterm * tx_amt * (1 + flt_ratio) / L360 / L100;
            (*dealval) = pub_base_PubDround_1(lx);
   
sprintf(acErrMsg,"intst_tax_no=[%s]",intst_tax_no);
WRITEMSG

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

				if (g_pub_intst.mode == 1)
                	dayterm = dayterm - (flag *6 + 1);

                (*val) = rate_val * dayterm * tx_amt * (1+flt_ratio)/L360/L100;
            }
           
            /* �޸�֪ͨ�Ǽǲ� */
            if (mo_infrm_c.prm_draw_date > tx_date) /* ��ǰ֧ȡ */
            {

sprintf(acErrMsg,"��ǰ֧ȡ ,�޸�֪ͨ�Ǽǲ�");
WRITEMSG

                mo_infrm_c.fct_wd_amt = tx_amt;
                mo_infrm_c.occ_date = tx_date;
                strcpy(mo_infrm_c.sts,"1");
                mo_infrm_c.trace_no = trace_no;
                mo_infrm_c.wrk_date = tx_date;

            }
            else  /* ����֧ȡ */
            {

sprintf(acErrMsg,"����֧ȡ ,�޸�֪ͨ�Ǽǲ�");
WRITEMSG

                mo_infrm_c.fct_wd_amt = tx_amt;
                mo_infrm_c.occ_date = tx_date;
                strcpy(mo_infrm_c.sts,"3");
                mo_infrm_c.trace_no = trace_no;
                mo_infrm_c.wrk_date = tx_date;

            }
            ret = Mo_infrm_Upd_Upd(mo_infrm_c, reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Update mo_infrm error ! sqlcode= [%d]",ret);
                WRITEMSG
                return ret;
            }

        }       /* end of ��ǰ������֧ȡ */
        else    /* ����֧ȡ */ 
        {

sprintf(acErrMsg,"����֧ȡ,draw_min_amt=[%lf]",draw_min_amt);
WRITEMSG

            if (tx_amt < draw_min_amt)
            {   /* С�����֧ȡ�����ڼ��� */

sprintf(acErrMsg,"����֧ȡ,֧ȡ���С��֧ȡ��ͽ��");
WRITEMSG

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

				ret=pub_base_CalIntstDays(ic_date,tx_date,
					intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }

				if (g_pub_intst.mode == 1)
                	dayterm = dayterm - (flag * 6 +1) * ttl_cnt;

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_rate=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

                lx = dayterm * rate_val * tx_amt * (1+flt_ratio) / L360 / L100;
                (*dealval) = pub_base_PubDround_1(lx);
                
                if (strcmp(intst_tax_no,SLBH) == 0)
                {
					ret=pub_base_CalIntstDays(PubMax(ic_date,date99), tx_date,
                        intst_term_type,&dayterm);
                	if (ret != 0)
                	{
                    	sprintf(acErrMsg,"������������!!");
                    	WRITEMSG
                    	return 1;
                	}

					if (g_pub_intst.mode == 1)
                    	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;
                    (*val)=dayterm * rate_val * tx_amt*(1+flt_ratio)/L360/L100;
                }
            }
            else
            {
                if (tx_amt < mo_infrm_c.prm_draw_amt)
                {   /* С��Լ��ȡ������ڼ��� */

sprintf(acErrMsg,"����֧ȡ,֧ȡ���С��Լ��֧ȡ���");
WRITEMSG
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
					ret=pub_base_CalIntstDays(ic_date,tx_date, 
						intst_term_type,&dayterm);
                	if (ret != 0)
                	{
                    	sprintf(acErrMsg,"������������!!");
                    	WRITEMSG
                    	return 1;
                	}

					sprintf(acErrMsg, "***dayterm=[%d]", dayterm);
					WRITEMSG

					if (g_pub_intst.mode == 1)
                    	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%ld],flt_rate=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * tx_amt * (1+flt_ratio)/L360/L100;
                    (*dealval) = pub_base_PubDround_1(lx);
                    
                    if (strcmp(intst_tax_no,SLBH) == 0)
                    {
						ret=pub_base_CalIntstDays(PubMax(ic_date,date99),
							tx_date,intst_term_type,&dayterm);
                		if (ret != 0)
                		{
                    		sprintf(acErrMsg,"������������!!");
                    		WRITEMSG
                    		return 1;
                		}

						if (g_pub_intst.mode == 1)
                        	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;
                        (*val)=dayterm*rate_val*tx_amt*(1+flt_ratio)/L360/L100;
                    }
                }
                else
                {   /* ��֪ͨ������ʼ�Ϣ */

sprintf(acErrMsg,"����֪ͨ���������֧ȡ");
WRITEMSG

                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    if (flag == 0)
                    {
                        strcpy(llbh,YTTZCK);
                    }
                    else
                    {
                        strcpy(llbh,QTTZCK);
                    }

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG
                    ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
                    if (ret != 0)
                    {
                        Mo_infrm_Clo_Upd();
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                        strcpy(reply,"W110");
                        return ret;
                    }
                    
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG
                    ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val1);
                    if (ret != 0)
                    {
                        Mo_infrm_Clo_Upd();
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                        strcpy(reply,"W110");
                        return ret;
                    }

					ret=pub_base_CalIntstDays(ic_date,tx_date, 
						intst_term_type,&dayterm);
                	if (ret != 0)
                	{
                    	sprintf(acErrMsg,"������������!!");
                    	WRITEMSG
                    	return 1;
                	}

					/* ֻ�۰���֧ȡ���� */
					if (g_pub_intst.mode == 1)
                    	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_rate=[%lf]",
        dayterm,rate_val,tx_amt,flt_ratio);
WRITEMSG
                    /* Լ��������Ӧ֪ͨ�����Ϣ���㡡*/ 
                    lx = dayterm * rate_val * mo_infrm_c.prm_draw_amt * (1+flt_ratio)/L360/L100;
                    (*dealval) = lx;

sprintf(acErrMsg,"Լ��֧ȡ�����Ϣ:lx = [%lf]",lx);
WRITEMSG
					if (g_pub_intst.mode == 1)
                    	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;

                    lx = dayterm * rate_val1 * (tx_amt - mo_infrm_c.prm_draw_amt) * (1 + flt_ratio) / L360 / L100;

sprintf(acErrMsg,"����Լ��֧ȡ���Ĳ�����Ϣ:lx = [%lf]",lx);
WRITEMSG

                    (*dealval) = lx + (*dealval);
                    (*dealval) = pub_base_PubDround_1(*dealval);
                    
                    if (strcmp(intst_tax_no,SLBH) == 0)
                    {
						ret=pub_base_CalIntstDays(PubMax(ic_date,date99),
							tx_date,intst_term_type,&dayterm);
                		if (ret != 0)
                		{
                    		sprintf(acErrMsg,"������������!!");
                    		WRITEMSG
                    		return 1;
                		}

						if (g_pub_intst.mode == 1)
                        	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;
                        (*val) = dayterm * rate_val * mo_infrm_c.prm_draw_amt * (1 + flt_ratio) / L360 / L100;

						if (g_pub_intst.mode == 1)
                        	dayterm = dayterm - (flag * 6 + 1) * ttl_cnt;
                        (*val) += dayterm * rate_val1 * (tx_amt - mo_infrm_c.prm_draw_amt) * (1 + flt_ratio) / L360 / L100;
                    }

                    
                } /* end of ֪ͨ�����Ϣ��Ϣ */
            }    

            /* �޸�֪ͨ���Ǽǲ� */
            mo_infrm_c.fct_wd_amt = tx_amt;
            mo_infrm_c.occ_date = tx_date;
            strcpy(mo_infrm_c.sts,"2");
            mo_infrm_c.trace_no = trace_no;
            mo_infrm_c.wrk_date = tx_date;
            
            ret = Mo_infrm_Upd_Upd(mo_infrm_c, reply);
            if (ret != 0)
            {
                Mo_infrm_Clo_Upd();
                sprintf(acErrMsg,"Update mo_infrm error ! sqlcode= [%d]",ret);
                WRITEMSG
                return ret;
            }

        }       /* end of  ����֧ȡ */ 

        Mo_infrm_Clo_Upd();

    }  /* end of �еǼǵ�֧ȡ */
    
 
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
    (*dealval) = pub_base_PubDround_1(*dealval);   
	g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
    (*val) = pub_base_PubDround_1((*val) * sdsl);
    (*factval) = (*dealval) - (*val);
    
sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
      *dealval,*factval,*val,*keepval);
WRITEMSG


sprintf(acErrMsg, "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
WRITEMSG
    return 0;
}
