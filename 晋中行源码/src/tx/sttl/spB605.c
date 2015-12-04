/*************************************************
* �� �� ��:  spB605.c
* ���������� ��Ȩ������Ϣά����¼�롢�޸ġ�ɾ����
*
* ��    ��:  rob
* ������ڣ� 2003��1��25��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"

int spB605()
{
    struct mo_righ_invest_c sMo_righ_invest,sMo_righ_invest1;
    char flag[2];
    int ret;
  
    memset(&sMo_righ_invest,0x0,sizeof(struct mo_righ_invest_c));
    memset(&sMo_righ_invest1,0x0,sizeof(struct mo_righ_invest_c));
    
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0670",flag); /* ������־ 0 ¼�� 1 �޸� 2 ɾ�� */
    get_zd_data("0580",sMo_righ_invest.pact_no);    /* Ͷ��Э���� */
    get_zd_data("0680",sMo_righ_invest.invst_type); /* ��ȨͶ����� */
    get_zd_data("0810",sMo_righ_invest.name);   /* ��Ͷ�ʵ�λ���� */
    get_zd_double("0390",&sMo_righ_invest.amt); /* ����� */
    get_zd_long("0530",&sMo_righ_invest.num); /* ������ */
	vtcp_log("amt=[%lf]num=[%d]",sMo_righ_invest.amt,sMo_righ_invest.num);
    
    strcpy(sMo_righ_invest.br_no,g_pub_tx.tx_br_no);
    if (sMo_righ_invest.invst_type[0] == '1')
    {
        /* ����Ȩ����Ͷ�� */
        strcpy(sMo_righ_invest.bkrl_code,"938");
    }
    else
    {
        /* �г���Ȩ����Ͷ�� */
        strcpy(sMo_righ_invest.bkrl_code,"938");
    }
    sMo_righ_invest.wrk_date = g_pub_tx.tx_date;
    strcpy(sMo_righ_invest.sts,"1");
    sMo_righ_invest.trace_no = g_pub_tx.trace_no;

    switch (atoi(flag))
    {
        case 0 :
            /* ¼�� */
            
            sprintf(acErrMsg,"¼��!!!!");
            WRITEMSG
  
            /* ����Ͷ��Э�����Ƿ��Ѿ����� */
            ret = Mo_righ_invest_Sel(g_pub_tx.reply,&sMo_righ_invest1,
                  "pact_no='%s'",sMo_righ_invest.pact_no);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"��ѯȨ����Ͷ�ʵǼǲ�����!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"Ȩ����Ͷ�ʵǼǲ����Ѿ����ڸ�Э����");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B086");
                goto ErrExit;
            }

		vtcp_log("amt=[%lf]num=[%d]",sMo_righ_invest.amt,sMo_righ_invest.num);
            ret = Mo_righ_invest_Ins(sMo_righ_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�Ǽ�Ȩ����Ͷ�ʵǼǲ����� !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"��Ȩ������Ϣ¼��");
            sprintf(acErrMsg,"��Ȩ����Ͷ�ʵǼǲ��������� pass!!!");
            WRITEMSG
            break;

        case 1 :
            /* �޸� */
            ret = Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",
                  sMo_righ_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error [%d] ",ret);
                WRITEMSG
                goto ErrExit;
            }

            ret = Mo_righ_invest_Fet_Upd(&sMo_righ_invest1,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"fet for update error [%d]",ret);
                WRITEMSG
                Mo_righ_invest_Clo_Upd();
                goto ErrExit;
            }
            
            if (sMo_righ_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"��������޸�!!!");
                WRITEMSG
                Mo_righ_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }
            
            ret = Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�޸�Ȩ����Ͷ�ʵǼǲ����� !! [%d]",ret);
                WRITEMSG
                Mo_righ_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B036");
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"��Ȩ������Ϣ�޸�");
            Mo_righ_invest_Clo_Upd();
            sprintf(acErrMsg,"�޸�Ȩ����Ͷ�ʵǼǲ�  pass!! ");
            WRITEMSG
            break;

        case 2 :
            /* ɾ�� */

            ret = Mo_righ_invest_Sel(g_pub_tx.reply,&sMo_righ_invest1,
                  "pact_no='%s'",sMo_righ_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"select mo_invest error !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            if (sMo_righ_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"�������ɾ��!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }
            ret = sql_execute("delete from mo_righ_invest where pact_no='%s' \
                   and sts ='1'",sMo_righ_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"ɾ��Ȩ����Ͷ�ʵǼǲ�����!! [%d] ",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B037");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"��Ȩ������Ϣɾ��");
            sprintf(acErrMsg,"ɾ��Ȩ����Ͷ�ʵǼǲ��м�¼ pass!! ");
            WRITEMSG
            break;

        default:
            goto ErrExit;
    }
 
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
