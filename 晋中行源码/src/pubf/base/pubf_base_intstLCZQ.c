#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstLCZQ.c
* ����������    �����ȡ������Ϣ
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
#include "td_mst_hst_c.h"


/**********************************************************************
* ��������    pub_base_CalIntstLCZQ()
* �������ܣ�  �����ȡ������Ϣ
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
int pub_base_CalIntstLCZQ(double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    long tx_date=0;
    long mtr_date=0;
    long ic_date=0; 
    long date99=0;
    long tmprq=0;  /* ���date99֮ǰ���һ�ʽ������� */
    double	js99;	/*991101�Ļ���*/
	double	ye99;	/*991101�����*/
    long ac_id;     /* �ʻ�ID */
    int  ac_seqn;   /* �ʻ���� */
    long dayterm=0;
    char intst_term_type[2];
    double intst_acm;  /* ��Ϣ���� */
    double tx_amt;     /* ������ */
    double rate_val;   /* ����ֵ */
    char cur_no[3];    /* ���� */
    char llbh[4];      /* ���ʱ�� */
    char rate_no[4];   /* ��Ʒ���е����ʱ�� */
    char intst_tax_no[4]; /* ��Ϣ˰˰�ʱ�� */
    double lx;            /* ��Ϣ */
    double flt_ratio;     /* �������� */
    double sdsl;          /* ˰��ֵ */
    int ret;
    long lzsbrq;	 	/*�ϱʷ�������*/
	long lzzcrq=0;	 	/*���������������һ�ʵ�����*/
    double lzsbjs; 	/*�ϱʷ����Ļ���*/
	double lzzcjs; 	/*�������´��Ļ���*/
    double lzsbye;	/*�ϱ����*/
	double lzzcye; 	/*�������´������*/
    long lzrq;	 		/*������¼���׵�����;*/
    double lzjs;	 	/*������¼����*/
    double lzye;	/*���*/
    double lzfse;	/*������*/
    double	khje;	/*������������Ľ��*/
    int lzflag=0;		/*0:�������´��;
						 1:��ĳ���·�û�д��,���²���;
						 2:�¸���û�в����û�д��뵱�´��*/
    double bal;
    struct td_mst_hst_c td_mst_hst_c;
    double dd_rate_val=0,hh_rate_val=0; /*���ںͻ�������ֵ,�����ֶ���Ϣ˰����ʡȥ����ȡ���ʵĲ���*/  
    date99 = 19991101;
    tx_date = g_pub_tx.tx_date;
    tx_amt = g_pub_tx.tx_amt1;

    mtr_date = g_td_mst.mtr_date;
    ic_date = g_td_mst.ic_date;
    ac_id = g_td_mst.ac_id;
    ac_seqn = g_td_mst.ac_seqn;
    intst_acm = g_td_mst.intst_acm;
    flt_ratio = g_td_mst.flt_ratio;
    khje = g_td_mst.td_amt;
    bal = g_td_mst.bal;

    MEMSET_DEBUG(cur_no, 0x0, sizeof(cur_no));
    MEMSET_DEBUG(intst_tax_no, 0x0, sizeof(intst_tax_no));
    MEMSET_DEBUG(rate_no, 0x0, sizeof(rate_no));

    strcpy(cur_no,g_td_parm.cur_no);
    strcpy(intst_tax_no, g_td_parm.intst_tax_no);
    strcpy(rate_no,g_td_parm.rate_no);

    
    (*dealval) = 0.00;
    (*val) = 0.00;
    (*factval) = 0.00;
    (*keepval) = 0.00;

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

sprintf(acErrMsg,"�����ȡ��Ϣ" );
WRITEMSG
    
    if (tx_date < mtr_date)
    {   /* ��ǰ֧ȡ */

sprintf(acErrMsg,"�����ȡ��Ϣ--��ǰ֧ȡ" );
WRITEMSG

        if (ic_date >= date99)
        {
TRACE
            js99 = 0.00;
            ye99 = 0.00;
        }
        else
        {   /* ��ѯdate99֮ǰ�Ķ��ڴ����ϸ����js99 ye99��ֵ */
TRACE
            tmprq = date99;
            ret = pub_base_gettdmsthst(ac_id,ac_seqn,&tmprq,&js99,&ye99,reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"ȡ���ڴ����ϸ���� [%d]",ret);
                WRITEMSG
                return 1;
            }
        }
        
        /********ȡ������Ѿ������˻���***
        ret = pub_base_CalIntstDays(tx_date,mtr_date,
                  g_td_parm.intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������!!!");
            WRITEMSG
            return 1;
        }
        dayterm = dayterm +  pub_base_tuneday(tx_date, ic_date, mtr_date);
        
        sprintf(acErrMsg,"intst_acm=[%lf]",intst_acm);
        WRITEMSG

        intst_acm -= dayterm * tx_amt;
        ******/
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date );
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
hh_rate_val=rate_val;
sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio );
WRITEMSG
        if (floor(intst_acm * 100) / 100 <= 0.00)
        {
            lx = 0.00;
            (*dealval) = 0.00;
            (*val)     = 0.00;
        }
        else if (ic_date >= date99)
        {

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio );
WRITEMSG

            lx = rate_val * intst_acm * (1 + flt_ratio) / L360 / L100;
            (*dealval) = pub_base_PubDround_1( lx );
            (*val) = (*dealval);
            if (strcmp(intst_tax_no,SLBH) != 0)
            { 
                (*val) = 0.00;  
            }
        }
        else
        {

sprintf(acErrMsg,"rate_val=[%lf],intst_acm=[%lf],flt_ratio=[%lf]",
        rate_val,intst_acm,flt_ratio );
WRITEMSG

            lx = rate_val * intst_acm * (1 + flt_ratio) / L360 / L100;
            (*dealval) = pub_base_PubDround_1(lx);

            if (strcmp(intst_tax_no,SLBH) == 0)
            { 
                ret=pub_base_CalIntstDays(date99,mtr_date,
                               intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }
                (*val) = lx * (1 - (js99 - ye99 * dayterm) / intst_acm);

            }
        }
    }   
    else
    {   /* ����֧ȡ ����֧ȡ */

sprintf(acErrMsg,"�����ȡ��Ϣ--���ڻ�����֧ȡ" );
WRITEMSG

sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ac_id,ac_seqn);
WRITEMSG

		/******
        ret = Td_mst_hst_Dec_Sel(reply,"ac_id = %ld and ac_seqn = %d \
        	order by tx_date,trace_no ",ac_id, ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg, "prepare error code =[%d]",ret);
            WRITEMSG
            return ret;
        }       

        MEMSET_DEBUG(&td_mst_hst_c,0x0,sizeof(struct td_mst_hst_c));
        ret = Td_mst_hst_Fet_Sel(&td_mst_hst_c, reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
            WRITEMSG
            * strcpy(g_pub_tx.reply, "D103"); *
            Td_mst_hst_Clo_Sel();
            return ret;

        }
        else if (ret == 100)
        {
            sprintf(acErrMsg, "NOFOUND in td_mst_hst !! code =[%d]",ret);
            WRITEMSG
            Td_mst_hst_Clo_Sel();
            return ret;
        }

        lzsbrq=td_mst_hst_c.tx_date;
        lzsbjs=td_mst_hst_c.intst_acm;
        lzsbye=td_mst_hst_c.bal;
        lzzcrq=td_mst_hst_c.tx_date;
        lzzcjs=td_mst_hst_c.intst_acm;
        lzzcye=td_mst_hst_c.bal;
        lzrq = td_mst_hst_c.tx_date;
        lzjs = td_mst_hst_c.intst_acm;
        lzye = td_mst_hst_c.bal;
        lzfse = td_mst_hst_c.tx_amt;

sprintf(acErrMsg,"�����ȡ��Ϣ--�ж����������ȡ" );
WRITEMSG
sprintf(acErrMsg,"lzsbrq=[%ld],lzsbjs=[%lf],lzsbye=[%lf]",lzsbrq,lzsbjs,lzsbye );
WRITEMSG

        while(1)
        {

sprintf(acErrMsg,"lzflag=[%d] ",lzflag);
WRITEMSG

            if (lzflag == 0)
            {

sprintf(acErrMsg,"0 ��������");
WRITEMSG

                if (pub_base_getyear(lzrq) == pub_base_getyear(lzsbrq) + 1)
                {

sprintf(acErrMsg,"����" );
WRITEMSG

                    *����*
               if (pub_base_getmonth(lzrq) + 12 <= pub_base_getmonth(lzrq) + 1)
                    {
                        lzzcrq = lzrq;
                        lzzcjs = lzjs;
                        lzzcye = lzye;
                    }
              else if (pub_base_getmonth(lzrq)+12 == pub_base_getmonth(lzrq)+2)
                    {
                        if (lzfse >= khje * 2)
                        {
                            lzzcrq = lzrq;
                            lzzcjs = lzjs;
                            lzzcye = lzye;
                        }
                        else
                        {
                            lzflag = 1;
                        }
                    }
                    else
                    {
                        lzflag = 2;
                        break;
                    }
                }
                else *������*
                {

sprintf(acErrMsg,"������" );
WRITEMSG

                    if (pub_base_getmonth(lzrq)<=pub_base_getmonth(lzsbrq)+1)
                    {
                        *  ÿ���������� *

sprintf(acErrMsg,"ÿ���������� lzrq=[%d],lzsbrq=[%d]",
        pub_base_getmonth(lzrq),pub_base_getmonth(lzsbrq));
WRITEMSG
                        lzzcrq = lzrq;
                        lzzcjs = lzjs;
                        lzzcye = lzye;
                    }
                    else* ĳ��©�� *
                    {
                  
sprintf(acErrMsg,"ĳ��©��" );
WRITEMSG

                   if (pub_base_getmonth(lzrq) == pub_base_getmonth(lzsbrq)+2)
                        {
                            if (lzfse >= khje * 2)
                            {
                                lzzcrq = lzrq;
                                lzzcjs = lzjs;
                                lzzcye = lzye;
                            }
                            else 
                            {
                                lzflag = 1;
                            }
                        }
                        else
                        {   * �ڶ���δ���� *
                            lzflag = 2;
                            break;
                        }
                    }
                } * end else ������*
            }  * end lzflag =0 *
            else if (lzflag == 1) *ĳ��©��,���²���*
            {

sprintf(acErrMsg,"1ĳ��©�棬���²���" );
WRITEMSG

                if ((pub_base_getyear(lzrq)==pub_base_getyear(lzsbrq)) \
                    && (pub_base_getmonth(lzrq)==pub_base_getmonth(lzsbrq)))
                {
                    lzflag = 0;
                    lzzcrq = lzrq;
                    lzzcjs = lzjs;
                    lzzcye = lzye;
                }
                else
                {
                    lzflag = 2;
                    break;
                }
            } *end lzflag==1*
            else
            {

sprintf(acErrMsg,"2ĳ��©�棬����δ����" );
WRITEMSG

                break; *�������* 
            }
            lzsbrq = lzrq;
            lzsbjs = lzjs;
            lzsbye = lzye;

            MEMSET_DEBUG(&td_mst_hst_c,0x0,sizeof(struct td_mst_hst_c));
            ret = Td_mst_hst_Fet_Sel(&td_mst_hst_c, reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
                WRITEMSG
                * strcpy(g_pub_tx.reply, "D103"); *
                Td_mst_hst_Clo_Sel();
                return ret;
            }
            else if (ret == 100)
            {
                break;
            }
            lzrq = td_mst_hst_c.tx_date;
            lzjs = td_mst_hst_c.intst_acm;
            lzye = td_mst_hst_c.bal;
            lzfse = td_mst_hst_c.tx_amt;

        }  *end whileȡ�������´������ںͻ������*
        Td_mst_hst_Clo_Sel();
		**/

sprintf(acErrMsg,"lzzcrq=[%ld],lzzcjs=[%lf],lzzcye=[%lf]",
        lzzcrq,lzzcjs,lzzcye);
WRITEMSG
        
        if (ic_date >= date99)
        {
            js99 = 0.00;
            ye99 = 0.00;
        }
        else
        {
            /* ��ѯdate99֮ǰ�Ķ��ڴ����ϸ����js99 ye99��ֵ */
            tmprq = date99;
            js99 = 0.00;
            ye99 = 0.00;
            ret = pub_base_gettdmsthst(ac_id,ac_seqn,&tmprq,&js99,&ye99,reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"ȡ���ڴ����ϸ���� [%d]",ret);
                WRITEMSG
                return 1;
            }

sprintf(acErrMsg,"js99=[%lf],ye99=[%lf],tmprq=[%ld],ac_id=[%ld],ac_seqn=[%d]",
        js99,ye99,tmprq,ac_id,ac_seqn);
WRITEMSG

        }
        
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],ic_date=[%ld],rate_val=[%lf]",
        llbh,cur_no,ic_date,rate_val);
WRITEMSG

        ret = pub_base_getllz(llbh,cur_no,ic_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            strcpy(reply,"W110");
            return ret;
        }
dd_rate_val=rate_val;
sprintf(acErrMsg,"�����ȡ������Ϣ���� rate_val=[%lf],lzzcjs=[%lf], \
	flt_ratio=[%lf], js=[%lf]", rate_val,lzzcjs,flt_ratio, g_td_mst.intst_acm);
WRITEMSG

        /* lx = lzzcjs * rate_val * (1 + flt_ratio) / L360 / L100; */
        lx = g_td_mst.intst_acm * rate_val * (1 + flt_ratio) / L360 / L100;
        (*dealval) = lx;

sprintf(acErrMsg,"�����ȡ������Ϣ���� dealval =[%lf]",*dealval);
WRITEMSG

        if (ic_date >= date99)
        {
            if (strcmp(intst_tax_no,SLBH) == 0)
            {
                (*val) = lx;
            }
        }
        else if( lzzcjs > 0.00)
        {
            if(strcmp(intst_tax_no,SLBH) == 0)
            {
                ret=pub_base_CalIntstDays(date99,mtr_date,
                               intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }

                if (lzzcrq < date99)
                {
                    (*val) = lx * (1 - (lzzcjs - lzzcye * dayterm) / lzzcjs);
                }
                else
                {
                    (*val) = lx * (1 - (js99 - ye99 * dayterm) /lzzcjs);
                }
            }
        }
        else
        {
            (*val) = 0.00;
        }
       
sprintf(acErrMsg,"�����ȡ������Ϣ���� val =[%lf]",*val);
WRITEMSG

        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
        strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],ic_date=[%ld],rate_val=[%lf]",
        llbh,cur_no,ic_date,rate_val);
WRITEMSG

        ret = pub_base_getllz(llbh, cur_no, tx_date, &rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
            strcpy(reply,"W110");
            return ret;
        }
hh_rate_val=rate_val;
sprintf(acErrMsg,"�����ڲ����������������ֵļ�Ϣ");
WRITEMSG

sprintf(acErrMsg,"intst_acm=[%lf],lzzcjs=[%lf],rate_val=[%lf],flt_ratio=[%lf]",
        intst_acm,lzzcjs,rate_val,flt_ratio);
WRITEMSG

        /*intst_acm-lzzcjs:�����ڲ����������������ֵĻ��� delete by rob at 1029*
        (*dealval) +=(intst_acm-lzzcjs) * rate_val * (1+flt_ratio)/L360/L100;*/

sprintf(acErrMsg,"�����ȡ���������ֺ���������֮�� dealval =[%lf]",*dealval);
WRITEMSG

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            if (date99 <= lzzcrq)
            {vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                ret=pub_base_CalIntstDays(lzzcrq,mtr_date,
                               intst_term_type,&dayterm);
                vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }

                (*val) +=(intst_acm-lzzcjs)*rate_val*(1+flt_ratio)/L360/L100;
            }
            else if (date99 <= mtr_date)
            {vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                ret=pub_base_CalIntstDays(date99,mtr_date,
                               intst_term_type,&dayterm);
                vtcp_log("[%s][%d]lzzcrq=[%ld],mtr_date=[%ld]",__FILE__,__LINE__,lzzcrq,mtr_date );
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }

                (*val) +=(intst_acm-js99+(ye99-lzzcye)*dayterm)*rate_val \
                         * (1 + flt_ratio) / L360 / L100;
            }
        }

sprintf(acErrMsg,"�����ȡ���������ֺ���������֮�� val =[%lf]",*val);
WRITEMSG

        /*���ڲ��ֵ���Ϣ��������*/
        ret=pub_base_CalIntstDays(mtr_date,tx_date,
                               intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
            return 1;
        }


sprintf(acErrMsg,"���� bal=[%lf],dayterm=[%d],rate_val=[%lf],flt_ratio=[%lf]",
        bal,dayterm,rate_val,flt_ratio);
WRITEMSG

		sprintf(acErrMsg, "--------CGH---���ڼ�������Ϣ:[%lf]", *dealval);
		WRITEMSG

        (*dealval) += floor(bal*L100)/L100 * dayterm * rate_val * (1+flt_ratio)/L360/L100;

		sprintf(acErrMsg, "--------CGH---���ڼ�������Ϣ:[%lf]", *dealval);
		WRITEMSG

        if (strcmp(intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(PubMax(mtr_date,date99),tx_date,
                               intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }

            (*val) +=floor(bal*L100)/L100 *dayterm *rate_val * (1+flt_ratio)/L360/L100;
        }        
        
    }
    
	if( g_pub_intst.edu_ind[0]=='1' )
	{
		*val=0.00;
	}
    else if( strcmp(intst_tax_no, SLBH) == 0)
    {
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
		if (*val > *dealval)
			*val = *dealval;
        (*val) = (*val) * sdsl;
        (*val) = pub_base_PubDround_1(*val);
    }
    (*dealval) = pub_base_PubDround_1(*dealval);  
    (*factval) = (*dealval) - (*val);
    (*keepval) = 0.00;

sprintf(acErrMsg,"���÷ֶ���˰ǰ����dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf] ",
        *dealval,*factval,*val,*keepval);
WRITEMSG
	/* added by liuxuan 2007-08-01 ������Ϣ˰ */
	if( strcmp(intst_tax_no, SLBH) == 0){
		char edu_ind[2];
		vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����\n",__FILE__,__LINE__);
		if(iGetAreaTax(val,factval,dealval,intst_term_type,1,dd_rate_val,hh_rate_val,edu_ind)){
			vtcp_log("[%s][%d]���÷ֶ�����Ϣ˰��������\n",__FILE__,__LINE__);
			return(1);
		}
	}

sprintf(acErrMsg,"���÷ֶ���˰����dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf] ",
        *dealval,*factval,*val,*keepval);
WRITEMSG
	/* add over */
    return 0;
}
