/*************************************************
* �� �� ��:  spB601.c
* ���������� ծȯͶ��������Ϣά��
*
* ��    ��:  
* ������ڣ� 2003��1��20��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"

int spB601()
{
    struct mo_invest_c sMo_invest,sMo_invest1;
    char flag[2];
    int ret;
  
    memset(&sMo_invest,0x0,sizeof(struct mo_invest_c));
    memset(&sMo_invest1,0x0,sizeof(struct mo_invest_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0670",flag); /* ������־ 0 ¼�� 1 �޸� 2 ɾ�� */
    get_zd_data("0580",sMo_invest.pact_no);    /* Ͷ��Э���� */
    get_zd_data("0230",sMo_invest.bkrl_code);  /* ��Ʒ��� */
    get_zd_data("0860",sMo_invest.ticket_type);/* ����ծȯƷ�� */
    get_zd_double("0390",&sMo_invest.buy_amt); /* ����� */
    get_zd_double("0400",&sMo_invest.par_bal); /* Ʊ��� */
    get_zd_long("0440",&sMo_invest.isuue_date);/* �������� */
    get_zd_long("0480",&sMo_invest.term);       /* ծȯ���� */
    get_zd_long("0450",&sMo_invest.mtr_date);  /* �������� */
    get_zd_long("0460",&sMo_invest.buy_date);  /* �������� */
    get_zd_data("0680",sMo_invest.intst_type); /* ծȯ��Ϣ��ʽ */
    get_zd_double("0840",&sMo_invest.rate);    /* ������ */
	get_zd_double("0410",&sMo_invest.pre_intst); /* Ӧ����Ϣ */
	get_zd_double("0420",&sMo_invest.overflow_amt); /* ��� */
	get_zd_double("0430",&sMo_invest.abate_amt);	  /* �ۼ� */
 /*	get_zd_data("0810",&sMo_invest.ticket_type); */    /*Ͷ��ծȯƷ�� */
/*	get_zd_double("1003",&sMo_invest.singl_feeval);	   ������ */
    strcpy(sMo_invest.sts,"1");
    sMo_invest.wrk_date = g_pub_tx.tx_date;
    sMo_invest.trace_no = g_pub_tx.trace_no;
    strcpy(sMo_invest.br_no,g_pub_tx.tx_br_no);
    strcpy(sMo_invest.tel,g_pub_tx.tel);
    strcpy(sMo_invest.chk,g_pub_tx.chk);
    strcpy(sMo_invest.auth,g_pub_tx.auth);

    
    switch (atoi(flag))
    {
        case 0 :
            /* ¼�� */
            
            sprintf(acErrMsg,"¼��!!!!");
            WRITEMSG
           
            /* ����Ͷ�ʺ�ͬ���Ƿ����ظ� */
            ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest1," pact_no = '%s' ",
                  sMo_invest.pact_no);
            vtcp_log("aaaaaaaaaaaaaaaaaaa [%s]\n",sMo_invest1.pact_no);
            if (ret != 0 && ret == !100)
            {
                sprintf(acErrMsg,"��ѯͶ�ʵǼǲ�����!! [%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B082");
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"��Ͷ��Э�����Ѿ�����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B083");
                goto ErrExit;
            }

            ret = Mo_invest_Ins(sMo_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�Ǽ�Ͷ�ʵǼǲ����� !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"ծȯͶ��������Ϣ¼��");
            sprintf(acErrMsg,"��Ͷ�ʵǼǲ��������� pass!!!");
            WRITEMSG
            break;

        case 1 :
            /* �޸� */
            
            ret = Mo_invest_Dec_Upd(g_pub_tx.reply,"pact_no ='%s'",
                  sMo_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error [%d] ",ret);
                WRITEMSG
                goto ErrExit;
            }
            ret = Mo_invest_Fet_Upd(&sMo_invest1,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"fet for update error [%d]",ret);
                WRITEMSG
                Mo_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B082");
                goto ErrExit;
            }
            
            if (sMo_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"��������޸�!!!");
                WRITEMSG
                Mo_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }
	/*	memcpy(&sMo_invest1, &sMo_invest, sizeof(sMo_invest));*/
		strcpy(sMo_invest.rowid, sMo_invest1.rowid);
            
        sprintf(acErrMsg,"UPDATE %d@%s ����۽ṹ[%lf]\n", __LINE__, __FILE__, sMo_invest1.buy_amt);
        WRITEMSG
            ret = Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�޸�Ͷ�ʵǼǲ����� !! [%d]",ret);
                WRITEMSG
                Mo_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B036");
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"ծȯͶ����Ϣ�޸�");
            Mo_invest_Clo_Upd();
            sprintf(acErrMsg,"�޸�Ͷ�ʵǼǲ�  pass!! ");
            WRITEMSG
            break;

        case 2 :
            /* ɾ�� */

            ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest1,"pact_no = '%s'",
                  sMo_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"select mo_invest error !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            if (sMo_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"�������ɾ��!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }
            ret = sql_execute("delete from mo_invest where pact_no='%s' and \
                  sts ='%s'",sMo_invest.pact_no, sMo_invest.sts);
            if (ret != 0)
            {
                sprintf(acErrMsg,"ɾ��Ͷ�ʵǼǲ�����!! [%d] ",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B037");
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"ծȯͶ����Ϣɾ��");
            sprintf(acErrMsg,"ɾ��Ͷ�ʵǼǲ��м�¼ pass!! ");
            WRITEMSG
            break;

        default:
            goto ErrExit;
    }
 
    strcpy(g_pub_tx.prdt_code,sMo_invest.bkrl_code);
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

