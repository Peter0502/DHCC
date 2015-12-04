/*************************************************
* �� �� ��:     pubf_eod_gltsub.c
* ����������    ͳ����������
* ��    ��:     rob
* ������ڣ�    2004��03��17��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "gl_mst_hst_c.h"
#include "gl_mst_c.h"
#include "gl_tsub_c.h"

/**********************************************************************
 * �� �� ����   pub_eod_ins_gltsub
 * �������ܣ�   ͳ����������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_ins_gltsub()
{
    int ret;

    ret = pub_eod_stat_five_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�������������ʴ���!!");
        WRITEMSG
        return ret;
    }

    ret = pub_eod_stat_seven_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�������������ʴ���!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_td_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ��Ѯ�������ʴ���!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_halfmonth_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�ư����������ʴ���!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_month_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�����������ʴ���!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_season_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�Ƽ��������ʴ���!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_halfyear_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�ư����������ʴ���!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_year_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�����������ʴ���!!");
        WRITEMSG
        return ret;
    }
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_five_gltsub
 * �������ܣ�   ͳ��������������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_five_gltsub()
{
    int ret;
    long last_date;
    char val[10];
    
    struct com_parm_c com_parm_c;
    struct gl_mst_hst_c gl_mst_hst_c;
    struct gl_tsub_c gl_tsub_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&gl_mst_hst_c, 0x0, sizeof(struct gl_mst_hst_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='ZQWT' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯϵͳ���������!![%d]",ret);
        WRITEMSG
        return ret;
    }
    last_date = atol(com_parm_c.val);

    if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) < 5)
    {
        sprintf(acErrMsg,"���ϴ�ͳ�Ʋ�������!!");
        return 0;
    }
	/*
    else if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) != 5)
    {
        sprintf(acErrMsg,"����ͳ���ﲻ�����м�����������!!");
        WRITEMSG
        sprintf(acErrMsg,"�ϴ�ͳ������=[%ld],����ͳ������=[%ld]",
                last_date, g_pub_tx.tx_date);
        WRITEMSG
        return -1;
    }
	*/

    /* ����������������ͳ�� */
    ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",last_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Gl_mst_hst_Fet_Sel(&gl_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_hst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_hst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_hst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_hst_c.dc_ind);
        gl_tsub_c.bt_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_hst_c.cr_bal;
        strcpy(gl_tsub_c.term_type,"1");
    
        /* ͳ�Ʊ��ڽ跽������ */
        ret = sql_sum_double("gl_mst_hst","rdd_amt",&gl_tsub_c.r_dr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڽ跽���������!! [%d]",ret);
            WRITEMSG
            return ret;
        }
  
        /* ͳ�Ʊ��ڴ��������� */
        ret = sql_sum_double("gl_mst_hst","rcd_amt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڴ������������!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        /* ͳ�Ʊ��ڽ跽���� */
        ret = sql_sum_long("gl_mst_hst","rdd_cnt",&gl_tsub_c.r_dr_cnt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڽ跽��������!! [%d]",ret);
            WRITEMSG
            return ret;
        }
 
        /* ͳ�Ʊ��ڴ������� */
        ret = sql_sum_long("gl_mst_hst","rcd_cnt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڴ�����������!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        ret = Gl_mst_hst_Sel(g_pub_tx.reply,&gl_mst_hst_c,"\"date\"=%ld and \
              acc_hrt='%s'",g_pub_tx.tx_date,gl_mst_hst_c.acc_hrt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��ѯ��ʷ���ʱ����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        gl_tsub_c.r_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_hst_c.cr_bal;

        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������������ݴ���!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_mst_hst_Clo_Sel();

    /* ����ϵͳ����������������ͳ��ʱ�� */
    ret = Com_parm_Dec_Upd(g_pub_tx.reply,"parm_code='ZQWT' and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Com_parm_Fet_Upd(&com_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(val,"%lf",g_pub_tx.tx_date);
    val[8] = 0;
    strcpy(com_parm_c.val,val);
    ret = Com_parm_Upd_Upd(com_parm_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    Com_parm_Clo_Upd();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_seven_gltsub
 * �������ܣ�   ͳ��������������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_seven_gltsub()
{
    int ret;
    long last_date;
    char val[10];
    
    struct com_parm_c com_parm_c;
    struct gl_mst_hst_c gl_mst_hst_c;
    struct gl_tsub_c gl_tsub_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&gl_mst_hst_c, 0x0, sizeof(struct gl_mst_hst_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='ZQQT' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯϵͳ���������!![%d]",ret);
        WRITEMSG
        return ret;
    }
    last_date = atol(com_parm_c.val);

    if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) < 7)
    {
        sprintf(acErrMsg,"���ϴ�ͳ�Ʋ�������!!");
        return 0;
    }
	/*
    else if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) != 7)
    {
        sprintf(acErrMsg,"����ͳ���ﲻ�����м�����������!!");
        WRITEMSG
        sprintf(acErrMsg,"�ϴ�ͳ������=[%ld],����ͳ������=[%ld]",
                last_date, g_pub_tx.tx_date);
        WRITEMSG
        return -1;
    }
	*/

    /* ����������������ͳ�� */
    ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",last_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Gl_mst_hst_Fet_Sel(&gl_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_hst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_hst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_hst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_hst_c.dc_ind);
        gl_tsub_c.bt_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_hst_c.cr_bal;
        strcpy(gl_tsub_c.term_type,"2");
    
        /* ͳ�Ʊ��ڽ跽������ */
        ret = sql_sum_double("gl_mst_hst","rdd_amt",&gl_tsub_c.r_dr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڽ跽���������!! [%d]",ret);
            WRITEMSG
            return ret;
        }
  
        /* ͳ�Ʊ��ڴ��������� */
        ret = sql_sum_double("gl_mst_hst","rcd_amt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڴ������������!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        /* ͳ�Ʊ��ڽ跽���� */
        ret = sql_sum_long("gl_mst_hst","rdd_cnt",&gl_tsub_c.r_dr_cnt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڽ跽��������!! [%d]",ret);
            WRITEMSG
            return ret;
        }
 
        /* ͳ�Ʊ��ڴ������� */
        ret = sql_sum_long("gl_mst_hst","rcd_cnt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڴ�����������!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        ret = Gl_mst_hst_Sel(g_pub_tx.reply,&gl_mst_hst_c,"\"date\"=%ld and \
              acc_hrt='%s'",g_pub_tx.tx_date,gl_mst_hst_c.acc_hrt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��ѯ��ʷ���ʱ����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        gl_tsub_c.r_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_hst_c.cr_bal;

        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������������ݴ���!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_mst_hst_Clo_Sel();

    /* ����ϵͳ����������������ͳ��ʱ�� */
    ret = Com_parm_Dec_Upd(g_pub_tx.reply,"parm_code='ZQQT' and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Com_parm_Fet_Upd(&com_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(val,"%lf",g_pub_tx.tx_date);
    val[8] = 0;
    strcpy(com_parm_c.val,val);
    ret = Com_parm_Upd_Upd(com_parm_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    Com_parm_Clo_Upd();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_td_gltsub
 * �������ܣ�   ͳ��Ѯ��������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_td_gltsub()
{
    int ret;
    long xcrq;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_PubXmcl(g_pub_tx.tx_date,&xcrq);
    if (ret != 0)
    {
        sprintf(acErrMsg,"���첻��Ѯĩ,����ͳ��Ѯ��������!![%ld]",
                g_pub_tx.tx_date);
        WRITEMSG
        return 0;
    }
   
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"date=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.tddr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.tdcr_bal;
        strcpy(gl_tsub_c.term_type,"3");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.tddr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.tdcr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.tddr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.tdcr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�����������в���Ѯ��¼����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_halfmonth_gltsub
 * �������ܣ�   ͳ�ư�����������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_halfmonth_gltsub()
{
    int ret;
    long year,month,day,bz;
    long last_date;
    char val[10];
    
    struct com_parm_c com_parm_c;
    struct gl_mst_hst_c gl_mst_hst_c;
    struct gl_tsub_c gl_tsub_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&gl_mst_hst_c, 0x0, sizeof(struct gl_mst_hst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ȡ�����մ���!!");
        WRITEMSG
        return ret;
    }
    
    if (day != 15 && bz != 4 && bz != 6 && bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"û�е�����ĩ!!");
        WRITEMSG
        return 0;
    }

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='ZQBY' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯϵͳ���������!![%d]",ret);
        WRITEMSG
        return ret;
    }
    last_date = atol(com_parm_c.val);
    
    /* ���а�����������ͳ�� */
    ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",last_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Gl_mst_hst_Fet_Sel(&gl_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_hst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_hst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_hst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_hst_c.dc_ind);
        gl_tsub_c.bt_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_hst_c.cr_bal;
        strcpy(gl_tsub_c.term_type,"4");
    
        /* ͳ�Ʊ��ڽ跽������ */
        ret = sql_sum_double("gl_mst_hst","rdd_amt",&gl_tsub_c.r_dr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڽ跽���������!! [%d]",ret);
            WRITEMSG
            return ret;
        }
  
        /* ͳ�Ʊ��ڴ��������� */
        ret = sql_sum_double("gl_mst_hst","rcd_amt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڴ������������!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        /* ͳ�Ʊ��ڽ跽���� */
        ret = sql_sum_long("gl_mst_hst","rdd_cnt",&gl_tsub_c.r_dr_cnt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڽ跽��������!! [%d]",ret);
            WRITEMSG
            return ret;
        }
 
        /* ͳ�Ʊ��ڴ������� */
        ret = sql_sum_long("gl_mst_hst","rcd_cnt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ�Ʊ��ڴ�����������!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        ret = Gl_mst_hst_Sel(g_pub_tx.reply,&gl_mst_hst_c,"\"date\"=%ld and \
              acc_hrt='%s'",g_pub_tx.tx_date,gl_mst_hst_c.acc_hrt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��ѯ��ʷ���ʱ����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        gl_tsub_c.r_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_hst_c.cr_bal;

        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"������������������ݴ���!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_mst_hst_Clo_Sel();

    /* ����ϵͳ�������а�������ͳ��ʱ�� */
    ret = Com_parm_Dec_Upd(g_pub_tx.reply,"parm_code='ZQBY' and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Com_parm_Fet_Upd(&com_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(val,"%lf",g_pub_tx.tx_date);
    val[8] = 0;
    strcpy(com_parm_c.val,val);
    ret = Com_parm_Upd_Upd(com_parm_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    Com_parm_Clo_Upd();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_month_gltsub
 * �������ܣ�   ͳ������������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_month_gltsub()
{
    int ret;
    long y,m,d,bz;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&y,&m,&d,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ȡ�����մ���!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 4 && bz != 6 && bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"û�е���ĩ!!");
        WRITEMSG
        return 0;
    }
   
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.mdr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.mcr_bal;
        strcpy(gl_tsub_c.term_type,"5");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.mdr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.mcr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.mdr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.mcr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�����������в����¼�¼����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_season_gltsub
 * �������ܣ�   ͳ�Ƽ���������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_season_gltsub()
{
    int ret;
    long year,month,day,bz;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ȡ�����մ���!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 6 && bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"û�е���ĩ!!");
        WRITEMSG
        return 0;
    }
   
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.qdr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.qcr_bal;
        strcpy(gl_tsub_c.term_type,"6");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.qdr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.qcr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.qdr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.qcr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�����������в��뼾��¼����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_halfyear_gltsub
 * �������ܣ�   ͳ�ư�����������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_halfyear_gltsub()
{
    int ret;
    long year,month,day,bz;
    long half_date;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c,gl_tsub;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    memset(&gl_tsub, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ȡ�����մ���!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"û�е�����ĩ!!");
        WRITEMSG
        return 0;
    }
    
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        if (bz == 8)
        {
            /* ����ĩ */
            gl_tsub_c.bt_dr_bal = gl_mst_c.ydr_bal;
            gl_tsub_c.bt_cr_bal = gl_mst_c.ycr_bal;
            strcpy(gl_tsub_c.term_type,"7");
        
            gl_tsub_c.r_dr_amt = gl_mst_c.ydr_amt;
            gl_tsub_c.r_cr_amt = gl_mst_c.ycr_amt;
            gl_tsub_c.r_dr_cnt = gl_mst_c.ydr_cnt;
            gl_tsub_c.r_cr_cnt = gl_mst_c.ycr_cnt;
            gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
            gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        }
        if (bz == 10)
        {
            /* ��ĩ */ 
            half_date = (g_pub_tx.tx_date / 10000 ) * 10000 + 630;
            ret = Gl_tsub_Sel(g_pub_tx.reply,&gl_tsub,"\"date\"=%ld and \
                  br_no = '%s' and acc_hrt ='%s'",half_date,gl_tsub_c.br_no,
                  gl_tsub_c.acc_hrt);
            if (ret == 100)
			{
				memset( &gl_tsub,0,sizeof(gl_tsub) );
			}
            else if (ret != 0)
            {
                sprintf(acErrMsg,"��ѯ630���������ʴ���!! [%d]",ret);
                WRITEMSG
                return ret;
            }
            gl_tsub_c.bt_dr_bal = gl_tsub.r_dr_bal;
            gl_tsub_c.bt_cr_bal = gl_tsub.r_cr_bal;
            strcpy(gl_tsub_c.term_type,"7");
        
            gl_tsub_c.r_dr_amt = gl_mst_c.ydr_amt - gl_tsub.r_dr_amt;
            gl_tsub_c.r_cr_amt = gl_mst_c.ycr_amt - gl_tsub.r_cr_amt;
            gl_tsub_c.r_dr_cnt = gl_mst_c.ydr_cnt - gl_tsub.r_dr_cnt;
            gl_tsub_c.r_cr_cnt = gl_mst_c.ycr_cnt - gl_tsub.r_cr_cnt;
            gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
            gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        }
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�����������в������¼����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_year_gltsub
 * �������ܣ�   ͳ������������
 * ����/ʱ�䣺  rob/20040317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_year_gltsub()
{
    int ret;
    long year,month,day,bz;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ȡ�����մ���!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 10)
    {
        sprintf(acErrMsg,"û�е���ĩ!!");
        WRITEMSG
        return 0;
    }
 
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.ydr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.ycr_bal;
        strcpy(gl_tsub_c.term_type,"8");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.ydr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.ycr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.ydr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.ycr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�����������в������¼����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
