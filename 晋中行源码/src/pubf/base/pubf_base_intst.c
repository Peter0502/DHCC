#include "find_debug.h"
/*********************************************************************
* �� �� ��:     pubf_base_intst.c       (������Ϣ)
* ��������:     ���ֻ��ڲ�Ʒ�Ͷ��ڲ�Ʒ����Ϣ����
* ���ú���˵��: pubf_base_CalIntstTd()  ������Ϣ�����ܿغ���
*               pubf_base_CalIntst()    ��Ϣ�����ܿغ���
*
* ��    ��:     rob
* �������:     2003��12��28��
*
* �޸ļ�¼:
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
*********************************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

/*********************************************************************
* �� �� ��:     pub_base_CalIntst()
* ��������:     ��Ϣ�����ܿغ���
* ����/ʱ��:    2003��12��28��
*
* ��    ��:
*     ����:
*     ���:
* �� �� ֵ:     0 �ɹ�
*
* �޸���ʷ:
*
*********************************************************************/
int pub_base_CalIntst(struct S_pub_intst *s_pub_intst)
{
    int mode;
    int year, month, day;
    double dealval = 0.00;
    double factval = 0.00;
    double val     = 0.00;
    double keepval = 0.00;
    char tmp_ind[2];

    memset(tmp_ind, 0x00, sizeof(tmp_ind));
    strcpy(tmp_ind, g_pub_intst.edu_ind);
    MEMSET_DEBUG(s_pub_intst, 0x00, sizeof(struct S_pub_intst));
    strcpy(g_pub_intst.edu_ind, tmp_ind);
    year  = g_pub_tx.tx_date / 10000;
    month = g_pub_tx.tx_date / 100 % 100;
    day   = g_pub_tx.tx_date % 100;

    mode = 1; /* ��ǰ���������ǽ�Ϣ�ռ�Ϣ ���ڲ��� */
    sprintf(acErrMsg, " mode = [%d]", mode);
    WRITEMSG

    switch (g_pub_tx.ac_id_type[0])
    {
        case '1':
            /* ���ڴ�� */
        TRACE
            if (pub_base_CalIntstHQ(mode, &dealval, &factval, &val,
                                    &keepval, g_pub_tx.reply))
            {
                strcpy(acErrMsg, "���������Ϣ����!!!");
                WRITEMSG
                return 1;
            }
            break;

        case '2':
            /* ���� */
        TRACE
            if (pub_base_CalIntstTd(&dealval, &factval, &val,
                                    &keepval, g_pub_tx.reply))
            {
                strcpy(acErrMsg, "���㶨����Ϣ����!!!");
                WRITEMSG
                return 1;
            }
            break;

        case '3':
            /* ���� */
            break;

        case '4':
            /* ͸֧ */
            break;

        case '9':
            /* �ڲ� */
            if (pub_base_CalIntstIn(&dealval, &factval, &val,
                                    &keepval, g_pub_tx.reply))
            {
                sprintf(acErrMsg, "�����ڲ��ʻ���Ϣ����!!");
                WRITEMSG
                return 1;
            }
            break;
        default:
                TRACE
            break;
    }
    sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        dealval, factval, val, keepval);
    WRITEMSG

    /* ����Ϣ�ṹ��ֵ */
    s_pub_intst->dealval = dealval;
    s_pub_intst->factval = factval;
    s_pub_intst->val     = val;
    s_pub_intst->keepval = keepval;
    return 0;
}

/*********************************************************************
* �� �� ��:     pub_base_CalIntstTd()
* ��������:     ���ڲ�Ʒ��Ϣ�����ܿغ���
* ����/ʱ��:    2003��12��28��
*
* ����:
*     ����:
*     ���:
*               ��Ӧ��      char(4)
* �� �� ֵ:     0 �ɹ�
*
* �޸���ʷ:
*
*********************************************************************/

int pub_base_CalIntstTd(double * dealval, double * factval, double * val,
                        double * keepval, char * reply)
{
    char    td_type;
    long    tmp_tx_date  = 0;
    long    tmp_ic_date  = 0;
    long    tmp_opn_date = 0;
    long    tmp_mtr_date = 0;
    double  tmp_dealval  = 0.00;
    double  tmp_factval  = 0.00;
    double  tmp_val      = 0.00;
    double  tmp_keepval  = 0.00;
    double  tmp_amt      = 0.00;
    int     l_term       = 0;

    td_type = g_td_parm.td_type[0];
    sprintf(acErrMsg, " td_type = [%c]", td_type);
    WRITEMSG
    sprintf(acErrMsg, "CGHCGH*****��������:[%ld]", g_pub_tx.tx_date);
    WRITEMSG
    switch(td_type)
    {
        case '0' :
            /* ������ȡ��Ϣ���� */
            strcpy(acErrMsg, "����������ȡ��Ϣ!");
            WRITEMSG

            /**------- hao -------- ���Ӷ����Զ�ת���Ϣ ---------**/
            tmp_tx_date  = g_pub_tx.tx_date;
            tmp_ic_date  = g_td_mst.ic_date;
            tmp_opn_date = g_td_mst.opn_date;
            tmp_mtr_date = g_td_mst.mtr_date;
            tmp_amt      = g_pub_tx.tx_amt1;
            do
            {
                if (g_td_mst.tfr_ind[0] == 'Y' && tmp_tx_date > g_td_mst.mtr_date)
                    g_pub_tx.tx_date = g_td_mst.mtr_date;
                else
                    g_pub_tx.tx_date = tmp_tx_date;
                tmp_dealval = 0.00;
                tmp_factval = 0.00;
                tmp_val     = 0.00;
                tmp_keepval = 0.00;
                if (pub_base_CalIntstTdZCZQ(&tmp_dealval, &tmp_factval,
                                           &tmp_val, &tmp_keepval, reply))
                {
                    strcpy(acErrMsg, "����������ȡ��Ϣ����!!");
                    WRITEMSG
                    return 1;
                }
                vtcp_log("AFTER [%d--%d]", g_td_mst.ic_date, g_td_mst.mtr_date);
                vtcp_log("deal[%.2lf] factval[%.2lf] val[%.2lf][%.2lf]",
                         tmp_dealval, tmp_factval, tmp_val, tmp_keepval);
                *dealval += tmp_dealval;
                *factval += tmp_factval;
                *val     += tmp_val;
                *keepval += tmp_keepval;
                /******XXXXXXXXX
                g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + (*factval) + (*keepval);
                ****************/
                g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + (tmp_factval) + (tmp_keepval);

                vtcp_log("after [%d--%d],tx_date[%ld]", g_td_mst.ic_date, g_td_mst.mtr_date, g_pub_tx.tx_date);
                vtcp_log("HAO --- LOOK [%lf][%lf][%lf][%lf][%lf]", *dealval, *factval, *keepval, g_pub_tx.tx_amt1, *val);
                g_td_mst.ic_date  = g_td_mst.mtr_date;
                g_td_mst.opn_date = g_td_mst.mtr_date;
                if (g_td_parm.term_type[0] == 'D')
                {
                    l_term = g_td_parm.term;
                    pub_base_deadlineD(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                }
                else if (g_td_parm.term_type[0] == 'Y')
                {
                    l_term = g_td_parm.term * 12;
                    pub_base_deadlineM(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                }
                else if (g_td_parm.term_type[0] == 'M')
                {
                    l_term = g_td_parm.term;
                    pub_base_deadlineM(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                }
            } while (tmp_tx_date > g_pub_tx.tx_date);
            g_pub_tx.tx_date  = tmp_tx_date;
            g_td_mst.ic_date  = tmp_ic_date;
            g_td_mst.opn_date = tmp_opn_date;
            g_td_mst.mtr_date = tmp_mtr_date;
            g_pub_tx.tx_amt1  = tmp_amt;
            /**------------------------------------------------------**/
            break;

        case '1' :
            /* �����ȡ��Ϣ���� */

            strcpy(acErrMsg,"���������ȡ��Ϣ!");
            WRITEMSG
            if (pub_base_CalIntstLCZQ(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"���������ȡ��Ϣ����!!");
                WRITEMSG
                return 1;
            }


            break;

        case '2' :
            /* ������ȡ��Ϣ����*/

            strcpy(acErrMsg,"����������ȡ��Ϣ!!");
            WRITEMSG
            if (pub_base_CalIntstZCLQ(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"����������ȡ��Ϣ����!!");
                WRITEMSG
                return 1;
            }

            break;

        case '3' :
            /* �汾ȡϢ��Ϣ����*/

            strcpy(acErrMsg,"�汾ȡϢ��Ϣ����!!");
            WRITEMSG
            if (pub_base_CalIntstCBQX(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                WRITEMSG
                return 1;
            }
            break;

        case '4' :
            /* ֪ͨ����Ϣ����*/

            strcpy(acErrMsg,"֪ͨ����Ϣ!!");
            WRITEMSG
            if (pub_base_CalIntstTZCK(dealval,factval,val,keepval,reply))
            {
                strcpy(acErrMsg,"����֪ͨ�����Ϣ����!!");
                WRITEMSG
                return 1;
            }
            break;

        case '5' :
            /* Э�����Ϣ����*/
            sprintf(acErrMsg,"����Э�����Ϣ��Ϣ!!");
            WRITEMSG
            break;

        case '6' :
            /* �������� */

            strcpy(acErrMsg,"���㶨��������Ϣ!!");
            WRITEMSG
            if (pub_base_CalIntstDHLB(dealval,factval,val,keepval,reply))
            {
                sprintf(acErrMsg,"���㶨��������Ϣ����!!");
                WRITEMSG
                return 1;
            }
            break;

        case '7' :
            /* ���������� �������� */
            strcpy(acErrMsg,"���㲻����������Ϣ!!");
            WRITEMSG
            sprintf(acErrMsg, "CGH*** ����֤��:[%s]", g_pub_intst.edu_ind);
            WRITEMSG
            if (pub_base_CalIntstJYCX(dealval, factval, val, keepval, \
                g_pub_intst.edu_ind, reply))
            {
                sprintf(acErrMsg, "���㲻����������Ϣ����--��������");
                WRITEMSG
                return 1;
            }
            break;
/* begin add by LiuHuafeng 2009-2-6 18:21:32 for �����֪ͨ��� */
    case '8' :

        strcpy(acErrMsg,"֪ͨ�����Ϣ!");
        WRITEMSG

        /**------- hao -------- ���Ӷ����Զ�ת���Ϣ ---------**/
        tmp_tx_date=g_pub_tx.tx_date;
        tmp_ic_date=g_td_mst.ic_date;
        vtcp_log("[%s][%d]test[%ld]",__FILE__,__LINE__,g_td_mst.ic_date);
        tmp_opn_date=g_td_mst.opn_date;
        tmp_mtr_date=g_td_mst.mtr_date;
        tmp_amt=g_pub_tx.tx_amt1;
        do
        {
            if (g_td_mst.tfr_ind[0]=='Y' && tmp_tx_date>g_td_mst.mtr_date)
                g_pub_tx.tx_date=g_td_mst.mtr_date;
            else
                g_pub_tx.tx_date=tmp_tx_date;
            tmp_dealval = 0.00;
            tmp_factval = 0.00;
            tmp_val = 0.00;
            tmp_keepval = 0.00;
            if (pub_base_CalIntstTdZCZQ_TZ(tmp_tx_date,&tmp_dealval,&tmp_factval,
                &tmp_val,&tmp_keepval,reply))
            {
                strcpy(acErrMsg,"����������ȡ��Ϣ����!!");
                WRITEMSG
                    return 1;
            }
            vtcp_log("AFTER [%d--%d]",g_td_mst.ic_date,g_td_mst.mtr_date);
            vtcp_log("deal[%.2lf] factval[%.2lf] val[%.2lf][%.2lf]",
                tmp_dealval,tmp_factval,tmp_val,tmp_keepval);
            *dealval+=tmp_dealval;
            *factval+=tmp_factval;
            *val+=tmp_val;
            *keepval+=tmp_keepval;
            /******XXXXXXXXX
            g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+(*factval)+(*keepval);
            ****************/
            g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+(tmp_factval)+(tmp_keepval);

            vtcp_log("after [%d--%d]",g_td_mst.ic_date,g_td_mst.mtr_date);
            vtcp_log("HAO --- LOOK [%lf][%lf][%lf][%lf][%lf]",*dealval,*factval,*keepval,g_pub_tx.tx_amt1,*val);
            g_td_mst.ic_date=g_td_mst.mtr_date;
            g_td_mst.opn_date=g_td_mst.mtr_date;
            if (g_td_parm.term_type[0]=='D')
            {
                l_term=g_td_parm.term;
                pub_base_deadlineD(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
            }
            else if (g_td_parm.term_type[0]=='Y')
            {
                l_term=g_td_parm.term*12;
                pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
            }
            else if (g_td_parm.term_type[0]=='M')
            {
                l_term=g_td_parm.term;
                pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
            }
        }while(tmp_tx_date>g_pub_tx.tx_date);
        g_pub_tx.tx_date=tmp_tx_date;
        g_td_mst.ic_date=tmp_ic_date;
        g_td_mst.opn_date=tmp_opn_date;
        g_td_mst.mtr_date=tmp_mtr_date;
        g_pub_tx.tx_amt1=tmp_amt;
        /**------------------------------------------------------**/
        break;
    /* end add by LiuHuafeng 2009-2-6 18:21:32 for �����֪ͨ��� */
        default :
            /* ���󷵻� */
            sprintf(acErrMsg, "���ڴ�����ʹ���: [%c]", td_type);
            WRITEMSG
            return 1;
    }

    return 0;
}

/**********************************************************************
* ������:     pub_base_CalIntstTdZCZQ()
* ��������:   ����������ȡ��Ϣ
* ����/ʱ��:  2003��12��28��
*
* ����:
*     ����:
*     ���: Ӧ����Ϣ          double
*           ʵ����Ϣ          double
*           ������Ϣ˰        double
*           ��ֵ��Ϣ          double
*           ��Ӧ��            char(4)
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ:
*
********************************************************************/
int pub_base_CalIntstTdZCZQ(double *dealval, double *factval,
       double *val, double * keepval, char *reply)
{
    int draw_flag; /* ���ڱ�־  1-��ǰ֧ȡ,2-����֧ȡ,3-����֧ȡ */
    int monterm,monterm1;
    long dayterm; /* �� �մ��� */
    long daytermxd;/**��λ����Э���������������**/
    long daytermval;      /* ���۸���������Ϣ���� */
    double  amtval;      /* ��������˰��� */
    double  sdsl;        /* ��������˰�� */
    double rate_val; /* ���� */
    double lx;      /* ��Ϣ */
    long date88,date89,date93,date9307,date99;
    long date91,date97,date98,date00;
    char   llbh[3]; /* ���ʱ�� */
    int ret;
    double a1,a2,a3,s1=0,s1_tmp=0,s2=0; /* ����*���� */
    char   intst_term_type[2] ; /* ������������ */
    char  Type[2];

    sprintf(acErrMsg,"g_td_parm.term_type=[%s],term=[%d],prdt_no=[%s], \
            tx_amt=[%lf]",g_td_parm.term_type,g_td_parm.term,
            g_td_parm.prdt_no, g_pub_tx.tx_amt1);
    WRITEMSG
    

    date88 = 19880910;
    date89 = 19890201;
    date91 = 19911201;
    date93 = 19930301;
    date9307 = 19930711;
    date97 = 19971023;
    date98 = 19981206;
    date99 = 19991101;
    date00 = 20000713;
    (*keepval) = 0.00; /* ��ֵ��Ϣ */
    (*dealval) = 0.00; /* Ӧ����Ϣ */
    (*factval) = 0.00; /* ʵ����Ϣ */
    (*val) = 0.00;     /* ������Ϣ˰ */
    strcpy(Type,"0");/*ʵ������ */

    /* �жϽ��������Ƿ��� */
    if (g_pub_tx.tx_date <  g_td_mst.mtr_date)
    {
        draw_flag = 1; /* ��ǰ */
    }
    else if (g_pub_tx.tx_date == g_td_mst.mtr_date)
    {
        draw_flag = 2; /* ���� */
    }
    else
    {
        draw_flag = 3; /* ���� */
    }

    /* ������� */
    switch (g_td_parm.term_type[0])
    {
        case 'Y' :
            monterm = g_td_parm.term * 12;
            break;
        case 'M' :
            monterm = g_td_parm.term;
            break;
        case 'D' :
            monterm = g_td_parm.term / 30;
            break;
        /**��λ����Э����������ڣ�������Ϣ����-�������ڼ������� ����30Ϊ�õ���ʵ�ʴ��ڣ�add by zhangguofeng strart 2014/4/11***/
        case 'H' : 
        		ret=pub_base_CalIntstDays(g_td_mst.ic_date,g_td_mst.mtr_date,Type,&daytermxd);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                return 1;
            }
            monterm = daytermxd / 30;
            break;
         /**zhangguofeng end 2014/4/11***/
        default:
            /* ���󷵻� */
            sprintf(acErrMsg,"���ڲ�Ʒ���д������ʹ��� %c",g_td_parm.term_type);
            WRITEMSG
            strcpy(reply,"W108");
            return 1;
    }
vtcp_log("[%s][%d]�������[%d]�ó�����[%ld]\n",__FILE__,__LINE__,daytermxd,monterm);
    /**���ݳ���Ҫ��,��20061104֮ǰ�İ������ʼ�����Ϣ,����֮��ĸ�Ϊ��ʵ������������Ϣ. modify by wanglei 20061118**  ���в���Ҫ��
    if (g_td_mst.opn_date < 20061104)
        strcpy(intst_term_type,"1");***��ÿ��30���Ϣ***
    else
    */
        strcpy(intst_term_type,g_td_parm.intst_term_type);/***��ʵ��������Ϣ***/

    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
         sprintf(acErrMsg,"��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
                        g_td_parm.intst_term_type);
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }


sprintf(acErrMsg,"��ʼ������Ϣmonterm=[%d],draw_flag=[%d]",monterm,draw_flag);
WRITEMSG

    if (monterm <= 24)
    {

sprintf(acErrMsg,"��ͨ��Ϣ����");
WRITEMSG

        /* ��ͨ��Ϣ���� */
        if (g_td_mst.ic_date <= date93)
        {
            /* 93.03.01֮ǰ��������ȡ */

sprintf(acErrMsg,"��ͨ��Ϣ����--930301֮ǰ��������ȡ");
WRITEMSG

            switch (draw_flag)
            {
                case 1:

                    /* ��ǰ֧ȡʵ��ͬ���ζ�������
                     1.���ݿ������� �������� �Ͳ�Ʒ �綨������
                     2.���������븳��Ӧ�����ʱ��,����ѯ������
                     3.������Ϣ
                     4.��������˰
                     */

sprintf(acErrMsg,"��ͨ��Ϣ����--��ǰ֧ȡ");
WRITEMSG

                    /* ��ʵ��ʱ��ͬ���ζ������ʼ�Ϣ �綨���� */
                    MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

                    ret = pub_base_intstJDQX(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"�綨���޴���");
                        WRITEMSG
                        return 1;
                    }

sprintf(acErrMsg,"�綨����:llbh=[%s]",llbh);
WRITEMSG
                    ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                         g_pub_tx.tx_date, intst_term_type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"������������!!");
                        WRITEMSG
                        strcpy(reply,"W102");
                        return 1;
                    }

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    /* llbh �������ʱ�źͽ������ڲ�ѯ���� */
                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * g_pub_tx.tx_amt1
                           * (1 + g_td_mst.flt_ratio) / L360 / L100;

                    (*dealval) = pub_base_PubDround_1(lx);

                    /* ��������˰ */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(date99,
               g_td_mst.ic_date),g_pub_tx.tx_date, intst_term_type,&daytermval);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }
                        /* �����Ϣ˰����Ϣ */
                        amtval = lx * daytermval / dayterm;

sprintf(acErrMsg,"dayterm=[%ld],daytermval=[%ld],lx=[%lf],amtval=[%lf],flt_ratio=[%lf]", dayterm,daytermval,lx,amtval,g_td_mst.flt_ratio);
WRITEMSG

                        /* ȡ��Ϣ˰˰�� */
                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
                        if ((ret != 0) && (ret == 1))
                        {
                            sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
                            WRITEMSG
                            strcpy(reply,"W100");
                            return 1;
                        }
                        else if (ret < 0)
                        {
                            sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ", ret);
                            WRITEMSG
                            strcpy(reply,"W101");
                            return 1;
                        }

sprintf(acErrMsg,"��Ϣ˰˰��sdsl=[%lf] ", sdsl);
WRITEMSG

                        g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                        (*val) = amtval * sdsl;
                        (*val) = pub_base_PubDround_1(*val) ;
                    }

                    (*factval) = (*dealval) -(*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    break;
                case 2 :
                    /* ����֧ȡ��Ϣ���� */
                case 3 :
                    /* ����֧ȡ��Ϣ����
                     1.�������ʱ�źͿ������� ��Ϣ���� �������ڲ�ѯ���ֶ����ʱ�
                       ����������ʺʹ���֮��
                     2.���㵽����Ϣ
                     3.�������,����������Ϣ
                     4.�����������˰
                     */

sprintf(acErrMsg,"��ͨ��Ϣ����--���ڻ�����֧ȡ");
WRITEMSG

                    /* ���㵽������*����ʱ�� �ֶμ��� */
                    MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                    strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg," llbh=[%s],cur_no=[%s],ic_date=[%ld],mtr_date=[%ld]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date,g_td_mst.mtr_date);
WRITEMSG

                    ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_td_mst.mtr_date,
                          intst_term_type, Lone, &a1);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"����ֶ�����*�������� [%d] ",ret);
                        WRITEMSG
                        strcpy(reply,"W111");
                        return ret;
                    }

sprintf(acErrMsg," ���� : ����ֶ�����*���� = [%lf]",a1);
WRITEMSG

                    /* ������������*����ʱ�� */

                    /* date93 ֮ǰ��ԭ���������ʼ���
                       date93 ֮��֧ȡ�չ��ƹ���Ļ��ڴ������ʼ��� */

                    if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                    {
                        ret=pub_base_CalIntstDays(g_td_mst.mtr_date,date93,
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
                        dayterm = 0; /* ����Ϣ */
                    }

                    /* ȡ��Ӧ���ڵ����� */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

                    a2 = dayterm * rate_val; /* date93ǰ */

sprintf(acErrMsg," ���� (date93ǰ) : ����ֶ�����*���� = [%lf]",a2);
WRITEMSG


                    if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                    {
                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
                            date93),g_pub_tx.tx_date,intst_term_type,&dayterm);
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

                    /* �������ʱ�� ��������ȡ����ʱ���ƻ�����Ϣ */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);
                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

                    a3 = dayterm * rate_val; /* date93�� */

sprintf(acErrMsg," ���� (date93��) : ����ֶ�����*���� = [%lf]",a3);
WRITEMSG

                    lx = (a1+a2+a3) * floor(g_pub_tx.tx_amt1*L100) / L100
                        * (1+g_td_mst.flt_ratio) / L360 / L100;
                    (*dealval) = pub_base_PubDround_1(lx) ;

                    /* ������Ϣ˰ */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(date99,
                            g_td_mst.mtr_date),g_pub_tx.tx_date,
                            intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        /* Ӧ��˰��Ϣ */
                 (*val)= rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
                                * (1+g_td_mst.flt_ratio) / L360 / L100;

                        /* ȡ��Ϣ˰��Ų�ȡ���� */
                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
                        if (ret != 0 && ret == 1)
                        {
                            sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
                            WRITEMSG
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

                        g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
                        (*val) = pub_base_PubDround_1((*val) * sdsl);
                    }
                    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    break;
                default:
                    break;
            }
        }
        else      /* 93.03.01 ������������ȡ */
        {

sprintf(acErrMsg,"��ͨ��Ϣ����--930301֮������������ȡ");
WRITEMSG

            switch (draw_flag)
            {
                case 1:
                    /*
                    ��ǰ֧ȡ ��֧ȡ�չ��ƹ���Ļ��ڴ��������ʼƸ���Ϣ
                    1.�����Ϣ���ڵ��������ڵ�������
                    2.ȡ��ǰ���ƹ���Ļ��ڴ���������
                    3.��������
                    4.99��������Ҫ����������˰
                    */

sprintf(acErrMsg,"��ͨ��Ϣ����--��ǰ֧ȡ");
WRITEMSG

sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
WRITEMSG
                    /**ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                           g_pub_tx.tx_date, intst_term_type,&dayterm); ��ʵ�������� 20130608 wjl**/
                  ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                           g_pub_tx.tx_date,Type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"������������!!");
                        WRITEMSG
                        return 1;
                    }


                    /* ȡ�������� */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"���� : llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

sprintf(acErrMsg,"��Ϣ : dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;
                    lx = pub_base_PubDround_1(lx) ;
                    (*dealval) = lx;

                    if (floor(lx * 100) / 100 < 0.005)
                    {
                        lx = 0.00;
                        (*dealval) = 0.00;
                        (*factval) = 0.00;
                        (*val) = 0.00;
                    }

                    /* �����������˰ */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(date99,
                               g_td_mst.ic_date),g_pub_tx.tx_date,
                               intst_term_type,&daytermval);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        amtval = lx * daytermval / dayterm;

                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
                        if ((ret != 0) && (ret == 1))
                        {
                           sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
                           WRITEMSG
                           strcpy(reply,"W100");
                           return 1;
                        }
                        else if (ret < 0)
                        {
                            sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ", ret);
                            WRITEMSG
                            strcpy(reply,"W101");
                            return 1;
                        }
                        g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                        (*val) = amtval * sdsl;
                        (*val) = pub_base_PubDround_1(*val);
                    }

                    (*factval) = (*dealval) -(*val);

sprintf(acErrMsg,"��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    break;
                case 2:

                    /* ����֧ȡ
                     1. ���㵽������
                     2. ȡ��Ӧ��Ʒ��������
                     3. ������Ϣ
                     4. 99������������˰
                     */
                case 3:
                    /* ����֧ȡ
                     1.930301--930711 ��������Ϣ����
                                         2.930711--������ ��930711ͬ�������ʼ���
                                         3.������--������ �������ջ������ʼ���
                                         4.����99��ĸ�������˰

                     */

sprintf(acErrMsg,"��ͨ��Ϣ����-- ���ڻ�����֧ȡ");
WRITEMSG

sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
WRITEMSG

                    if (monterm <= 6)
                    {
                        ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                               g_td_mst.mtr_date, intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        /* ȡ�ò�Ʒ�Ķ�Ӧ������ */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[%ld],��������[%d]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date,dayterm);
WRITEMSG

					/**���е�λ����Э������ڼ�Ϣʱ��ʹ�÷ֻ����� add by zhangguofeng strart 2014/4/11**/
					if (memcmp(g_td_parm.prdt_no,"259",3)==0)
					{
              rate_val=g_td_mst.rate;
              vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
          }else
          {	

              ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                    g_td_mst.ic_date,&rate_val);
              if (ret != 0)
              {
                  sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                  WRITEMSG
                  strcpy(reply,"W110");
                  return ret;
              }
          }
					/**���е�λ����Э������ڼ�Ϣʱ��ʹ�÷ֻ����� add by zhangguofeng end 2014/4/11**/
sprintf(acErrMsg,"����:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = lx;
sprintf(acErrMsg,"ע�⵽����Ϣ:[%lf]",lx);
WRITEMSG

                        /* ������Ϣ */
                        if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                        {
                            /*
                            ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
                                 date99), g_pub_tx.tx_date, intst_term_type,
                                 &dayterm);
                            */
                            ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                                 g_pub_tx.tx_date, intst_term_type,
                                 &dayterm);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }
                            vtcp_log("[%s][%d]����[%ld][%ld]�������[%d]\n",__FILE__,__LINE__,g_td_mst.mtr_date,g_pub_tx.tx_date,dayterm);
                        }
                        else
                        {
                            dayterm = 0;

                        }
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"����: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%d]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date,dayterm);
WRITEMSG

                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_pub_tx.tx_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                     lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"������Ϣ: dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],\
        flt_ratio=[%lf] lx[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio,lx);
WRITEMSG
                        (*dealval) = (*dealval) + lx;
sprintf(acErrMsg,"ע������Ϣ: dealval[[%lf]",*dealval);
WRITEMSG
                         /* �����������˰ */

                        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                        {
                            ret=pub_base_CalIntstDays(PubMax(date99,
                                g_td_mst.ic_date), g_td_mst.mtr_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }

                            /* ���ʱ�ŴӲ�Ʒ��ȡ */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"��Ϣ˰:llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  g_td_mst.ic_date,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"���ڲ���Ӧ�ý���Ϣ˰�Ĳ���1:dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                            amtval = rate_val * daytermval
                                     * floor(g_pub_tx.tx_amt1*L100)/L100
                                     * (1 + g_td_mst.flt_ratio) / L360 / L100;

                            ret=pub_base_CalIntstDays(PubMax(date99,
                                g_td_mst.mtr_date), g_pub_tx.tx_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"��Ϣ˰:llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  g_pub_tx.tx_date,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"���ڲ���Ӧ�ý���Ϣ˰�Ĳ���2:dayterm=[%d],rate_val=[%lf],tx_amt=[%lf], \
   flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

              lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                                 * (1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"���ڲ���Ӧ�ý�˰����Ϣ��:lx[%lf]",lx);
WRITEMSG
sprintf(acErrMsg,"����Ӧ��Ӧ��˰����Ϣ��amtval[%lf]",amtval);
WRITEMSG
                            amtval = amtval + lx;
sprintf(acErrMsg,"�ϼ�Ӧ�ý�˰����Ϣ��amtval[%lf]",amtval);
WRITEMSG
                            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                            if ((ret != 0) && (ret == 1)) /*�������С��19991101������Ϊ0 gujy 20070419*/
                            {
                                sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
                                      ret);
                                WRITEMSG
                                strcpy(reply,"W100");
                                return 1;
                            }
                            else if (ret < 0)
                            {
                                sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ",
                                     ret);
                                WRITEMSG
                                strcpy(reply,"W101");
                                return 1;
                            }
                            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                            (*val) = amtval * sdsl;
                            (*val) = pub_base_PubDround_1(*val);
                        }

                        (*dealval)= pub_base_PubDround_1(*dealval);
                        (*factval) = (*dealval) -(*val);
                        if (floor((*dealval)*100)/100<0.005)
                        {
                            lx = 0.00;
                            (*dealval) = 0.00;
                            (*factval) = 0.00;
                            (*val) = 0.00;
                        }

sprintf(acErrMsg,"��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    }
                    else /* monterm >6 */
                    {

sprintf(acErrMsg,"���ڴ���������");
WRITEMSG
                        ret=pub_base_CalIntstDays(g_td_mst.ic_date,date9307,
                               intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }


                        /* ȡ930711ʱ�������� */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[19930711]",
        llbh,g_td_parm.cur_no);
WRITEMSG

												/**���е�λ����Э������ڼ�Ϣʱ��ʹ�÷ֻ����� add by zhangguofeng strart 2014/4/11**/
												if (memcmp(g_td_parm.prdt_no,"259",3)==0)
												{
							              rate_val=g_td_mst.rate;
							              vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
							          }else
							          {
	                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
	                              date9307,&rate_val);
	                        if (ret != 0)
	                        {
	                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
	                            WRITEMSG
	                            strcpy(reply,"W110");
	                            return ret;
	                        }
	                      }
												/**���е�λ����Э������ڼ�Ϣʱ��ʹ�÷ֻ����� add by zhangguofeng end 2014/4/11**/
sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = lx;

sprintf(acErrMsg,"lx=[%lf]",lx);
WRITEMSG

                        ret=pub_base_CalIntstDays(PubMax(date9307,
                            g_td_mst.ic_date), g_td_mst.mtr_date,
                            intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        /* ��930711�󿪻�,ȡ�ò�Ʒ�Ķ�Ӧ������
                           ����,ȡpubmax(930711,ic_date)����ͬ�������� */

                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                        strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[19930711]",
        llbh,g_td_parm.cur_no);
WRITEMSG
     /**  ���ݽ���ͬҵ��Ŷ��ڿ������� ��ӡCZFY���ʲ����ά��ԭ���� add by martin 2009/7/27 10:46:50 **/
          if (memcmp(g_td_parm.prdt_no,"25A",3)==0)
              {
                rate_val=g_td_mst.rate;
                vtcp_log("[%s] [%d] martin look----g_td_mst.rate=rate_val=[%lf]\n",__FILE__,__LINE__,rate_val);
              }else
                   {
                        ret = pub_base_getllz(llbh, g_td_parm.cur_no,
                              PubMax(date9307,g_td_mst.ic_date), &rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }
               }

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                            * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = (*dealval) + lx;

sprintf(acErrMsg,"1lx=[%lf]",dayterm * rate_val);
WRITEMSG
sprintf(acErrMsg,"2lx=[%lf]", floor(g_pub_tx.tx_amt1*L100));
WRITEMSG
sprintf(acErrMsg,"3lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100);
WRITEMSG
sprintf(acErrMsg,"4lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100*(1+g_td_mst.flt_ratio));
WRITEMSG
sprintf(acErrMsg,"4lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100*(1+g_td_mst.flt_ratio)/L360);
WRITEMSG
sprintf(acErrMsg,"4lx=[%lf]",dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100*(1+g_td_mst.flt_ratio)/L360/L100);
WRITEMSG
sprintf(acErrMsg,"lx=[%lf]",lx);
WRITEMSG
                        /* ������Ϣ���� */
                        if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                        {
                           /** ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                                g_pub_tx.tx_date,intst_term_type,&dayterm); ������Ϣ��ʵ����������wjl 20130608**/
                               ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                                g_pub_tx.tx_date,Type,&dayterm);
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

                        /* ȡ��������  */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"����: llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_pub_tx.tx_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ����[%s][%s][%ld][%.2lf]",llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

sprintf(acErrMsg,"dayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        (*dealval) = (*dealval) + lx;

sprintf(acErrMsg,"lx=[%lf]",lx);
WRITEMSG

                        /* �����������˰ */
                        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                        {
                            ret=pub_base_CalIntstDays(PubMax(date99,
                                g_td_mst.ic_date), g_td_mst.mtr_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }

                             /* ȡ��Ʒ������ʱ�� */
                             MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                             strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"��Ϣ˰: llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date);
WRITEMSG

                             ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                   g_td_mst.ic_date,&rate_val);
                             if (ret != 0)
                             {
                                 sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                                 WRITEMSG
                                 strcpy(reply,"W110");
                                 return ret;
                             }

sprintf(acErrMsg,"��Ϣdayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG
                             amtval = rate_val * daytermval
                                      * floor(g_pub_tx.tx_amt1*L100)/L100
                                      * (1 + g_td_mst.flt_ratio) / L360 /L100;


                            if (g_td_parm.ovrd_intst_ind[0] == 'Y')
                            {
                                ret=pub_base_CalIntstDays(PubMax(date99,
                                    g_td_mst.mtr_date), g_pub_tx.tx_date,
                                    intst_term_type,&daytermval);
                                if (ret != 0)
                                {
                                    sprintf(acErrMsg,"������������!!");
                                    WRITEMSG
                                    return 1;
                                }
                            }
                            else
                            {
                                daytermval = 0;

                            }

                            /* ȡ�������� */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"��Ϣ: llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_td_mst.ic_date);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  g_pub_tx.tx_date,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"��Ϣdayterm=[%d],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                            amtval += rate_val * daytermval
                                     * floor(g_pub_tx.tx_amt1*L100)/L100
                                     * (1 + g_td_mst.flt_ratio) / L360 /L100;

                            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                            if ((ret != 0) && (ret == 1))
                            {
                                sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
                                       ret);
                                WRITEMSG
                                strcpy(reply,"W100");
                                return 1;
                            }
                            else if (ret < 0)
                            {
                                sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ",
                                     ret);
                                WRITEMSG
                                strcpy(reply,"W101");
                                return 1;
                            }
                            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
                            (*val) = amtval * sdsl;
                            (*val) = pub_base_PubDround_1(*val);
                        }
                        (*dealval) = pub_base_PubDround_1(*dealval);
                        (*factval) = (*dealval) -(*val);
                        if (floor((*dealval) * 100)/100<0.005)
                        {
                            (*dealval) = 0.00;
                            (*factval) = 0.00;
                            (*val) = 0.00;
                        }

sprintf(acErrMsg,"��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                    }
                    break;
                default:
                    /* ���󷵻� */
                    return 1;
            }
        }
    }
    else  /* monterm >24 */
    {

sprintf(acErrMsg,"��ֵ��Ϣ����");
WRITEMSG

        /* ��ֵ��Ϣ���� */
        switch (draw_flag)
        {
            case 1 :
                /* δ����֧ȡ */

sprintf(acErrMsg,"δ����֧ȡ ");
WRITEMSG

                if (g_td_mst.ic_date >= date93) /* date93֮�󿪻��� */
                {

sprintf(acErrMsg,"date93 ֮�󿪻���,���������ʼ�Ϣ ");
WRITEMSG

                    /* �����ڻ������ʼ��� */
                    ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                        g_pub_tx.tx_date, intst_term_type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"������������!!");
                        WRITEMSG
                        return 1;
                    }

                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[%ld]",
        llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                        WRITEMSG
                        strcpy(reply,"W110");
                        return ret;
                    }

sprintf(acErrMsg,"rate_val=[%lf],dayterm=[%d],tx_amt=[%lf],flt_ratio=[%lf]",
        rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;

                    (*dealval) = pub_base_PubDround_1(lx);

                    /* ������Ϣ˰ */
                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                            date99), g_pub_tx.tx_date,intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                     lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;

                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                        if (ret != 0 && ret == 1)
                        {
                            sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
                                     ret);
                            WRITEMSG
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
                        g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                        (*val) = pub_base_PubDround_1(lx * sdsl);
                    }
                    if ((*dealval) < 0.005)
                    {
                        (*dealval) = 0.00;
                        (*val) = 0.00;
                    }
                    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG
                }  /* end of date93֮�󿪻��� */
                else  /* date93 ֮ǰ������ */
                {

sprintf(acErrMsg,"date93 ֮ǰ������,���������ʼ�Ϣ ");
WRITEMSG

                    /* ��ic_date,tx_date�ֶμ�Ϣ,��������,���ָߵ� */

                    MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                    ret = pub_base_intstJDQX(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"�綨���޴���");
                        WRITEMSG
                        return 1;
                    }

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],ic_date=[%ld],tx_date=[%ld],\
        intst_term_type=[%d]",llbh,g_td_parm.cur_no,g_td_mst.ic_date,
        g_pub_tx.tx_date,intst_term_type);
WRITEMSG

                    /* rate_val = ���� * ����֮�� */
                    ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                          g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type,
                          0,&rate_val);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"����ֶλ�������");
                        WRITEMSG
                        return 1;
                    }

sprintf(acErrMsg,"�ֶλ���=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                    lx = rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                         * (1 + g_td_mst.flt_ratio) / L360 / L100;
                    (*dealval) = lx;

                    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
                    {
                        /* �綨���� */
                        /* �ӿ����յ�PubMin(������,date99) */
                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

                        ret = pub_base_intstJDQX(llbh,g_td_parm.cur_no,
                            g_td_mst.ic_date,PubMin(g_td_mst.mtr_date,date99),
                            intst_term_type);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"�綨���޴���");
                            WRITEMSG
                            return 1;
                        }

                        /* rate_val = ���� * ����֮�� */
                        ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                             g_td_mst.ic_date,g_pub_tx.tx_date,intst_term_type,
                             0,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"����ֶλ�������");
                            WRITEMSG
                            return 1;
                        }

sprintf(acErrMsg,"lx=[%lf],�ֶλ���=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
        lx,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                        lx = lx - rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                             * (1 + g_td_mst.flt_ratio) / L360 / L100;

                        ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                        if (ret != 0 && ret == 1)
                        {
                            sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
                                     ret);
                            WRITEMSG
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
                        g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                        (*val) = pub_base_PubDround_1(lx * sdsl);
                    }

                    if (floor((*dealval) * 100) / 100 < 0.005)
                    {
                        (*dealval) = 0.00;
                        (*val) = 0.00;
                    }
                    (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                }
                break;
            case 2 :
                /* ����֧ȡ */
                /*
                  1. 88/09/10 ��ǰ�����ı�ֵ����,��ֵ�ڴ�88/09/10��������
                  2. 89/02/01 �Ժ��ڵı�ֵ����,��88/09/10��ִ��89/02/01������
                     �Ĵ���������
                  3. 91/12/01--93/03/01 ֮�����ı�ֵ����,��ֵ�ڴ�93/07/11
                     ��������
                  4. 93/03/01--93/07/11 ֮������һ�������ϴ�����,�ӿ�����
                     ��93/07/11 ��93/07/11�����Ļ������ʼ�Ϣ,93/07/11��������
                     ��93/07/11������ͬ���ζ������ʼ�Ϣ
                  5. 93/07/11(��)�������������ڰ��浥��ԭ�������ʼ�Ϣ
                */
            case 3 :
                /* ����֧ȡ */

sprintf(acErrMsg,"����������Ϣ����");
WRITEMSG

                if (g_td_mst.ic_date < date88)
                {

    sprintf(acErrMsg, "----------->��Ϣ����С��88.09.10");
    WRITEMSG

                    if (g_td_mst.mtr_date >= date89)
                    {
                        /* �ӿ����յ�date89�ֶμ�Ϣ */
                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                        strcpy(llbh, g_td_parm.rate_no);
                        ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                              g_td_mst.opn_date,date89,intst_term_type,
                              Lone, &a1);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"����ֶ�����*�������� [%d] ",ret);
                            WRITEMSG
                            strcpy(reply,"W111");
                            return ret;
                        }

                        ret=pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
                               intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        /* date89 ��Ӧ���ֵ����� */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,g_td_parm.rate_no);
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              date89,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                        a1 = a1 + dayterm * rate_val;
                    }
                    else
                    {
                        /* �ӿ����յ������շֶμ�Ϣ */
                        MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                        strcpy(llbh, g_td_parm.rate_no);
                        ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                              g_td_mst.opn_date,g_td_mst.mtr_date,
                              intst_term_type, Lone, &a1);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"����ֶ�����*�������� [%d] ",ret);
                            WRITEMSG
                            strcpy(reply,"W111");
                            return ret;
                        }

                    }

                    /* ���㱣ֵ��Ϣ */
                    /* ȡ�����յı�ֵ���� */
                    MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                    strcpy(llbh,BAOZHI);
                    ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                          g_pub_tx.tx_date,&rate_val);
                    if (ret != 0)
                    {
                         sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                         WRITEMSG
                         strcpy(reply,"W110");
                         return ret;
                    }

                    ret=pub_base_CalIntstDays(date88,g_td_mst.mtr_date,
                               intst_term_type,&dayterm);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"������������!!");
                        WRITEMSG
                        return 1;
                    }

                    (*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 / L360 / L100;
                }
                else /* 88.9.10�� */
                {

    sprintf(acErrMsg, "----------->��Ϣ���ڴ���88.9.10");
    WRITEMSG
                    if (g_td_mst.ic_date < date91)
                    {

sprintf(acErrMsg,"����������Ϣ����,88.9.10�󿪻���,date91ǰ����");
WRITEMSG

                        ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                            g_td_mst.mtr_date,intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        /* ȡdate89���յ���Ӧ�Ĵ��ֵ����� */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,g_td_parm.rate_no);
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                   date89,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                        a1 = rate_val * dayterm ;

                        ret=pub_base_CalIntstDays(date99, g_td_mst.mtr_date,
                               intst_term_type,&daytermval);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        s1 += rate_val * (double)daytermval;


                        /* ȡ�����յı�ֵ���� */
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

                        (*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
                        s2 += rate_val * daytermval;

                    } /* end of date91 ǰ���� */
                    else  /* date91 �󿪻��� */
                    {
                        if (g_td_mst.ic_date < date93)
                        {

sprintf(acErrMsg,"����������Ϣ����,date91�󿪻�,date93 ǰ����");
WRITEMSG

                            /* �Ӽ�Ϣ�յ������հ��ֶ����ʼ�Ϣ */
                            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                            strcpy(llbh, g_td_parm.rate_no);
                            ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                                  g_td_mst.ic_date,g_td_mst.mtr_date,
                                  intst_term_type, Lone, &a1);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"����ֶ�����*�������� [%d] ",ret);
                                WRITEMSG
                                strcpy(reply,"W111");
                                return ret;
                            }

                            /* ������ PubMin(�����գ�date99) */
                            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                            strcpy(llbh, g_td_parm.rate_no);
                            ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                                  g_td_mst.ic_date,PubMin(g_td_mst.mtr_date,
                                  date99), intst_term_type, Lone, &s1_tmp);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"����ֶ�����*�������� [%d] ",ret);
                                WRITEMSG
                                strcpy(reply,"W111");
                                return ret;
                            }
                            s1 += a1 - s1_tmp;

                        }
                        else /*1993/03/01֮��*/
                        {

sprintf(acErrMsg,"����������Ϣ����,date93 �󿪻�");
WRITEMSG

                            ret=pub_base_CalIntstDays(g_td_mst.ic_date,
                                date9307, intst_term_type,&dayterm);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }

                            /* ȡdate9307�������� */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"date93,date9307���ʱ�䰴���ڼ�Ϣ,llbh=[%s],cur_no=[%s],\
        date=9307",llbh,g_td_parm.cur_no);
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                  date9307,&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"date93,date9307���ʱ�䰴���ڼ�Ϣ,date9307ʱ��rate_val\
        =[%lf],dayterm=[%d]",rate_val,dayterm);
WRITEMSG

                            a1 = rate_val * dayterm ;

sprintf(acErrMsg,"date93,date9307���ʱ�䰴���ڼ�Ϣ,����Ϊ=[%lf]",a1);
WRITEMSG

                            ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                                date9307),g_td_mst.mtr_date,
                                intst_term_type,&dayterm);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }

                            /* ȡPubMax(��������,date9307)�յ���Ӧ������ */
                            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                            strcpy(llbh,g_td_parm.rate_no);

sprintf(acErrMsg,"date9307�󵽵����հ�date97ͬ�������ʼ�Ϣ,llbh=[%s], \
        cur_no=[%s],date=[%ld]",llbh,g_td_parm.cur_no,
        PubMax(g_td_mst.ic_date,date9307));
WRITEMSG

                            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                                PubMax(g_td_mst.ic_date,date9307),&rate_val);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                                WRITEMSG
                                strcpy(reply,"W110");
                                return ret;
                            }

sprintf(acErrMsg,"date93,date9307���ʱ�䰴���ڼ�Ϣ,date9307ʱ��rate_val\
        =[%lf],dayterm=[%d]",rate_val,dayterm);
WRITEMSG

                            a2 = rate_val * dayterm ;

sprintf(acErrMsg,"date9307�󵽵�����,���ʱ�����Ϊ=[%lf]",a2);
WRITEMSG

                            a1 = a1 + a2 ;

                            ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                                date99),g_td_mst.mtr_date,
                                intst_term_type,&daytermval);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������������!!");
                                WRITEMSG
                                return 1;
                            }

                            s1 = 0.00;
                            s1 = daytermval * rate_val ;
                        }

                        /******* delete by rob at 20030728 ***
                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                            date9307),g_td_mst.mtr_date,
                            intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        * ȡ�����յı�ֵ���� *
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);

sprintf(acErrMsg,"�����յı�ֵ���� llbh=[%s],cur_no=[%s],mtr_date=[%ld]",
       llbh,g_td_parm.cur_no,g_td_mst.mtr_date);
WRITEMSG

                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }

sprintf(acErrMsg,"��ֵ���� rate_val=[%lf],dayterm=[%d],tx_amt=[%lf],\
        flt_ratio=[%lf]",rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

                        (*keepval) = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;

sprintf(acErrMsg, "��ֵ��Ϣ: keepval=[%lf]", *keepval);
WRITEMSG
                        ******/

                        ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
                            date99),g_td_mst.mtr_date,intst_term_type,&dayterm);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"������������!!");
                            WRITEMSG
                            return 1;
                        }

                        s2 = 0.00;
                        s2 = rate_val * dayterm ;

                    }

                }

                /* ������Ϣ */
                /* �����յ�date93���ֶ����ʼ�Ϣ */

                MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
                strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg,"�������� �����յ�date93,llbh=[%s],cur_no=[%s]",
       llbh,g_td_parm.cur_no);
WRITEMSG
                if (g_td_mst.mtr_date < date93)
                {
                    ret = pub_base_getsecrate(llbh,g_td_parm.cur_no,
                          g_td_mst.mtr_date,date93,intst_term_type, Lone, &a2);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"����ֶ�����*�������� [%d] ",ret);
                        WRITEMSG
                        strcpy(reply,"W111");
                        return ret;
                    }

                }
                else
                {
                    a2 = 0.00;
                }

sprintf(acErrMsg,"�������� �����յ�date93,��ηֶλ���Ϊ: [%lf]",a2);
WRITEMSG

                /* ȡ���׵��յĻ������� */
                MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"�������� date93��,llbh=[%s],cur_no=[%s],tx_date=[%ld]",
       llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
WRITEMSG

                ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                      g_pub_tx.tx_date,&rate_val);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                    WRITEMSG
                    strcpy(reply,"W110");
                    return ret;
                }

                ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,date93),
                    g_pub_tx.tx_date, intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }


sprintf(acErrMsg,"�������� date93��  rate_val= [%lf],dayterm=[%d]",
        rate_val,dayterm);
WRITEMSG
                a3 = 0;
                a3 = rate_val * dayterm ;

sprintf(acErrMsg,"�������� date93����Ϣ����Ϊ: [%lf]",a3);
WRITEMSG

                ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date99),
                    g_pub_tx.tx_date, intst_term_type,&dayterm);
                if (ret != 0)
                {
                    sprintf(acErrMsg,"������������!!");
                    WRITEMSG
                    return 1;
                }

                s1 += rate_val * dayterm ;

sprintf(acErrMsg,"������Ϣ����=[%lf],����9307ǰ����=[%lf],����9307�����=\
        [%lf],tx_amt=[%lf],flt_ratio=[%lf]",a1,a2,a3,g_pub_tx.tx_amt1,
        g_td_mst.flt_ratio);
WRITEMSG
                lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1*L100)/L100
                     * (1 + g_td_mst.flt_ratio) / L360 / L100;
                (*dealval) = lx;

sprintf(acErrMsg, "--------->��ϢֵΪ:[%lf]", *dealval);
WRITEMSG

                if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0)
                {
                    ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
                    if (ret != 0 && ret == 1)
                    {
                        sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
                                 ret);
                        WRITEMSG
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
                    lx = s1 * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio)
                         / L360 / L100;
                    /* add by rob at 20030904 */
                    g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                    (*val) = lx * sdsl ;
                }

                (*dealval) = pub_base_PubDround_1(*dealval);
                (*val) = pub_base_PubDround_1(*val);
                (*keepval) = pub_base_PubDround_1(*keepval);

                (*factval) = (*dealval) - (*val);
                (*keepval) = (*keepval);/* ˰��ֵ��Ϣ */

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

                break;
            default:
                return 1;

        }
    }
        /* added by liuxuan 2007-08-01 �޸���Ϣ˰ */
    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
        char edu_ind[2];
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����\n",__FILE__,__LINE__);
        if (iGetAreaTax(val,factval,dealval,intst_term_type,0,rate_val,rate_val,edu_ind)){
            vtcp_log("[%s][%d]���÷ֶ�����Ϣ˰��������\n",__FILE__,__LINE__);
            return(1);
        }
    }
    /* add over */
sprintf(acErrMsg,"[%s][%d]dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        __FILE__,__LINE__,*dealval,*factval,*val,*keepval);

    return 0;
}
/**********************************************************************
* ������:     pub_base_CalIntstTdZCZQ_TZ()
* ��������:   ����1,7��������ȡ��Ϣ�������֪ͨ��
* ����/ʱ��:  LiuHuafeng 2009-2-6 18:16:21
*
* ����:
*     ����:
*     ���: Ӧ����Ϣ          double
*           ʵ����Ϣ          double
*           ������Ϣ˰        double
*           ��ֵ��Ϣ          double
*           ��Ӧ��            char(4)
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ:
* ע������: ��������֧�������֪ͨ����ʽ����������ǰ�ϵ�֪ͨ����ģʽ
********************************************************************/
int pub_base_CalIntstTdZCZQ_TZ(long real_tx_date,double *dealval, double *factval,
    double *val, double * keepval, char *reply)
{
    int draw_flag; /* ���ڱ�־  1-��ǰ֧ȡ,2-����֧ȡ,3-����֧ȡ */
    int monterm,monterm1;
    long dayterm; /* �� �մ��� */
    long daytermval;      /* ���۸���������Ϣ���� */
    double  amtval;      /* ��������˰��� */
    double  sdsl;        /* ��������˰�� */
    double rate_val; /* ���� */
    double lx;      /* ��Ϣ */
    long date88,date89,date93,date9307,date99;
    long date91,date97,date98,date00;
    char   llbh[4]; /* ���ʱ�� */
    int ret;
    double a1,a2,a3,s1,s1_tmp,s2; /* ����*���� */
    char   intst_term_type[2] ; /* ������������ */

    memset(llbh,0,sizeof(llbh));
    memset(intst_term_type,0,sizeof(intst_term_type));

    sprintf(acErrMsg,"g_td_parm.term_type=[%s],term=[%d],prdt_no=[%s], \
        tx_amt=[%lf],mtr_date[%ld]",g_td_parm.term_type,g_td_parm.term,
        g_td_parm.prdt_no, g_pub_tx.tx_amt1,g_td_mst.mtr_date);
    WRITEMSG

        date88 = 19880910;
    date89 = 19890201;
    date91 = 19911201;
    date93 = 19930301;
    date9307 = 19930711;
    date97 = 19971023;
    date98 = 19981206;
    date99 = 19991101;
    date00 = 20000713;
    (*keepval) = 0.00; /* ��ֵ��Ϣ */
    (*dealval) = 0.00; /* Ӧ����Ϣ */
    (*factval) = 0.00; /* ʵ����Ϣ */
    (*val) = 0.00;     /* ������Ϣ˰ */

    /* �жϽ��������Ƿ��� */
    if (g_pub_tx.tx_date <  g_td_mst.mtr_date)
    {
        /******************start modified by ligl ���ӽڼ����ж�********************************/
        if (g_td_parm.holi_ind[0]=='1')
        {
            ret=pub_base_checktxdate(g_pub_tx.tx_date,g_td_mst.mtr_date,'2');
            if (ret==100)
            {
                draw_flag = 1; /* ��ǰ */
            }
            else if (ret==0)
            {
                draw_flag = 2; /* ���� */
            }
            else
            {
                draw_flag = 1; /* ��ǰ */
            }
        }
        else
        {
            draw_flag = 1; /* ��ǰ */
        }
        /********************end modified by ligl*************************************/
    }
    else if (g_pub_tx.tx_date == g_td_mst.mtr_date)
    {
        draw_flag = 2; /* ���� */
    }
    else
    {
        draw_flag = 3; /* ���� */
    }
    /* ������� */
    switch (g_td_parm.term_type[0])
    {
    case 'Y' :
        monterm = g_td_parm.term * 12;
        break;
    case 'M' :
        monterm = g_td_parm.term;
        break;
    case 'D' :
        monterm = g_td_parm.term / 30;
        break;
    default:
        /* ���󷵻� */
        sprintf(acErrMsg,"���ڲ�Ʒ���д������ʹ��� %c",g_td_parm.term_type);
        WRITEMSG
            strcpy(reply,"W108");
        return 1;
    }
    strcpy(intst_term_type,g_td_parm.intst_term_type);/***��ʵ��������Ϣ***/
    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
        sprintf(acErrMsg,"��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
            g_td_parm.intst_term_type);
        WRITEMSG
            MEMSET_DEBUG(reply, 0x0, sizeof(reply));
        strcpy(reply, "W107");
        return 1;
    }
    sprintf(acErrMsg,"��ʼ������Ϣmonterm=[%d],draw_flag=[%d]",monterm,draw_flag);
    WRITEMSG
    vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
    sprintf(acErrMsg,"��ͨ��Ϣ����");
    WRITEMSG
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
    /* ��ͨ��Ϣ���� */
    sprintf(acErrMsg,"��ͨ��Ϣ����--930301֮������������ȡ");
    WRITEMSG
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
    switch (draw_flag)
    {
    case 1:
        /*
        ��ǰ֧ȡ ��֧ȡ�չ��ƹ���Ļ��ڴ��������ʼƸ���Ϣ
        1.�����Ϣ���ڵ��������ڵ�������
        2.ȡ��ǰ���ƹ���Ļ��ڴ���������
        3.��������
        4.99��������Ҫ����������˰
        */
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        sprintf(acErrMsg,"��ͨ��Ϣ����--��ǰ֧ȡ");
        WRITEMSG
            sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
        WRITEMSG
            ret=pub_base_CalIntstDays(g_td_mst.ic_date,
            g_pub_tx.tx_date, intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
                return 1;
        }
        /* ȡ�������� */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        sprintf(acErrMsg,"���� : llbh=[%s],cur_no=[%s],date=[%ld]",
            llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
        WRITEMSG

        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
            g_pub_tx.tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
                strcpy(reply,"W110");
            return ret;
        }
        sprintf(acErrMsg,"��Ϣ : dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
        WRITEMSG

        lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
            * (1 + g_td_mst.flt_ratio) / L360 / L100;
        lx = pub_base_PubDround_1(lx) ;
        (*dealval) = lx;

        if (floor(lx * 100) / 100 < 0.005)
        {
            lx = 0.00;
            (*dealval) = 0.00;
            (*factval) = 0.00;
            (*val) = 0.00;
        }
        /* �����������˰ */
        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(PubMax(date99,
                g_td_mst.ic_date),g_pub_tx.tx_date,
                intst_term_type,&daytermval);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                    return 1;
            }
            amtval = lx * daytermval / dayterm;

            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &sdsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
                WRITEMSG
                    strcpy(reply,"W100");
                return 1;
            }
            else if (ret < 0)
            {
                sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ", ret);
                WRITEMSG
                    strcpy(reply,"W101");
                return 1;
            }
            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
            (*val) = amtval * sdsl;
            (*val) = pub_base_PubDround_1(*val);
        }
        (*factval) = (*dealval) -(*val);

        sprintf(acErrMsg,"��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
            *dealval,*factval,*val,*keepval);
        WRITEMSG
            vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        break;
    case 2:
        /* ����֧ȡ
        1. ���㵽������
        2. ȡ��Ӧ��Ʒ��������
        3. ������Ϣ
        4. 99������������˰
        */
    case 3:
        /* ����֧ȡ
        1.930301--930711 ��������Ϣ����
        2.930711--������ ��930711ͬ�������ʼ���
        3.������--������ �������ջ������ʼ���
        4.����99��ĸ�������˰
        */
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

        sprintf(acErrMsg,"��ͨ��Ϣ����-- ���ڻ�����֧ȡ");
        WRITEMSG

        sprintf(acErrMsg,"ic_date=[%ld],tx_date=[%ld]",g_td_mst.ic_date,g_pub_tx.tx_date);
        WRITEMSG
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        ret=pub_base_CalIntstDays(g_td_mst.ic_date,
            g_td_mst.mtr_date, intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
                return 1;
        }
        /* ȡ�ò�Ʒ�Ķ�Ӧ������ */
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,g_td_parm.rate_no);

        sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],date=[%ld],��������[%ld]",
            llbh,g_td_parm.cur_no,g_td_mst.ic_date,dayterm);
        WRITEMSG
        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
            real_tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
                strcpy(reply,"W110");
            return ret;
        }
        sprintf(acErrMsg,"����:dayterm=[%ld],rate_val=[%lf],tx_amt=[%d],flt_ratio=[%lf]",
            dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
        WRITEMSG
        lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
            * (1 + g_td_mst.flt_ratio) / L360 / L100;
        (*dealval) = lx;
        sprintf(acErrMsg,"ע�⵽����Ϣ:[%lf]",lx);
        WRITEMSG
            vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

        /* ������Ϣ */
        if (g_td_parm.ovrd_intst_ind[0] == 'Y')
        {
            /*
            ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
            date99), g_pub_tx.tx_date, intst_term_type,
            &dayterm);
            */
            ret=pub_base_CalIntstDays(g_td_mst.mtr_date,
                g_pub_tx.tx_date, intst_term_type,
                &dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                    return 1;
            }
            vtcp_log("[%s][%d]���ں������[%ld]\n",__FILE__,__LINE__,dayterm);
        }
        else
        {
            dayterm = 0;

        }
        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
        strcpy(llbh,HQLLBH);

        sprintf(acErrMsg,"����: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%ld]",
            llbh,g_td_parm.cur_no,g_pub_tx.tx_date,dayterm);
        WRITEMSG

        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
            g_pub_tx.tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            strcpy(reply,"W110");
            return ret;
        }
        lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100
            * (1 + g_td_mst.flt_ratio) / L360 / L100;
        sprintf(acErrMsg,"������Ϣ: dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],\
            flt_ratio=[%lf] lx[%lf]",dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio,lx);
        WRITEMSG
            (*dealval) = (*dealval) + lx;
        sprintf(acErrMsg,"ע������Ϣ: dealval[[%lf]",*dealval);
        WRITEMSG
        /* �����������˰ */
        if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
        {
            ret=pub_base_CalIntstDays(g_td_mst.ic_date, g_td_mst.mtr_date,
                intst_term_type,&daytermval);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                    return 1;
            }
            /* ���ʱ�ŴӲ�Ʒ��ȡ */
            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
            strcpy(llbh,g_td_parm.rate_no);

            sprintf(acErrMsg,"��Ϣ˰:llbh=[%s],cur_no=[%s],date=[%ld]",
                llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
            WRITEMSG
            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                real_tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                    strcpy(reply,"W110");
                return ret;
            }
            sprintf(acErrMsg,"���ڲ���Ӧ�ý���Ϣ˰�Ĳ���1:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
            WRITEMSG
                vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
            amtval = rate_val * daytermval
                * floor(g_pub_tx.tx_amt1*L100)/L100
                * (1 + g_td_mst.flt_ratio) / L360 / L100;

            ret=pub_base_CalIntstDays(PubMax(date99,
                g_td_mst.mtr_date), g_pub_tx.tx_date,
                intst_term_type,&daytermval);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                    return 1;
            }
            MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
            strcpy(llbh,HQLLBH);

            sprintf(acErrMsg,"��Ϣ˰:llbh=[%s],cur_no=[%s],date=[%ld]",
                llbh,g_td_parm.cur_no,g_pub_tx.tx_date);
            WRITEMSG

            ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                g_pub_tx.tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                    strcpy(reply,"W110");
                return ret;
            }
            sprintf(acErrMsg,"���ڲ���Ӧ�ý���Ϣ˰�Ĳ���2:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf], \
                flt_ratio=[%lf]", daytermval,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
            WRITEMSG

            lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
                * (1 + g_td_mst.flt_ratio) / L360 / L100;
            sprintf(acErrMsg,"���ڲ���Ӧ�ý�˰����Ϣ��:lx[%lf]",lx);
            WRITEMSG
                sprintf(acErrMsg,"����Ӧ��Ӧ��˰����Ϣ��amtval[%lf]",amtval);
            WRITEMSG
                amtval = amtval + lx;
            sprintf(acErrMsg,"�ܵ�Ӧ�ý�˰����Ϣ��amtval[%lf]",amtval);
            WRITEMSG
                ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date,&sdsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
                    ret);
                WRITEMSG
                    strcpy(reply,"W100");
                return 1;
            }
            else if (ret < 0)
            {
                sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ",
                    ret);
                WRITEMSG
                    strcpy(reply,"W101");
                return 1;
            }
            g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
            (*val) = amtval * sdsl;
            (*val) = pub_base_PubDround_1(*val);
        }
        (*dealval)= pub_base_PubDround_1(*dealval);
        (*factval) = (*dealval) -(*val);
        if (floor((*dealval)*100)/100<0.005)
        {
            lx = 0.00;
            (*dealval) = 0.00;
            (*factval) = 0.00;
            (*val) = 0.00;
        }
        sprintf(acErrMsg,"��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
            *dealval,*factval,*val,*keepval);
        WRITEMSG
        break;
    default:
        /* ���󷵻� */
        return 1;
    }
    vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

    vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

    vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);

    /* added by liuxuan 2007-08-01 �޸���Ϣ˰ */
    if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
    {
        char edu_ind[2];
        vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����intst_term_type==%s\n",__FILE__,__LINE__,intst_term_type);
        if (iGetAreaTax_ZCZQ_TK(real_tx_date,val,factval,dealval,intst_term_type,0,rate_val,rate_val,edu_ind))
        {
            vtcp_log("[%s][%d]���÷ֶ�����Ϣ˰��������\n",__FILE__,__LINE__);
            return(1);
        }
    }
    /* add over */

    sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);

    return 0;
}
