/*************************************************
* �� �� ��:  spB701.c
* ���������� ֤ȯ�ع���Ϣά��
*
* ��    ��:  
* ������ڣ� 2003��1��29��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"

int spB701()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
    char flag[2];
    char cashflag[2];
    int ret;
  
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0670",flag); /* ������־ 0 ¼�� 1 �޸� 2 ɾ�� */
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no); /* ֤ȯ�ع�Э���� */
    get_zd_data("0700",sMo_stok_rebuy_prot.tcht_type); /* ֤ȯ���� */
    get_zd_double("0390",&sMo_stok_rebuy_prot.par_value);/* ֤ȯ��ֵ */
    get_zd_double("0940",&sMo_stok_rebuy_prot.pledge_rate);/* ��Ѻ�� */ 
    get_zd_data("0680",sMo_stok_rebuy_prot.buy_type); /* ��������־ */
    get_zd_double("0400",&sMo_stok_rebuy_prot.bal); /* ��������� */
    get_zd_long("0440",&sMo_stok_rebuy_prot.matr_date); /* Э�鵽������ */
    get_zd_long("0480",&sMo_stok_rebuy_prot.term); /* ֤ȯ�ع����� */
    get_zd_double("0850",&sMo_stok_rebuy_prot.rate); /* ������ */
    get_zd_data("0690",cashflag); /* ��ת��־ */
    get_zd_data("0270",sMo_stok_rebuy_prot.opt_name); /* �Է������� */

	strcpy(sMo_stok_rebuy_prot.opt_acno,cashflag);
    strcpy(sMo_stok_rebuy_prot.br_no,g_pub_tx.tx_br_no);
    sMo_stok_rebuy_prot.sign_date = g_pub_tx.tx_date;
    sMo_stok_rebuy_prot.reg_date = g_pub_tx.tx_date;
    sMo_stok_rebuy_prot.trace_no = g_pub_tx.trace_no;
    strcpy(sMo_stok_rebuy_prot.sts,"1");
    strcpy(sMo_stok_rebuy_prot.tel,g_pub_tx.tel);
    strcpy(sMo_stok_rebuy_prot.chk,g_pub_tx.chk);
    strcpy(sMo_stok_rebuy_prot.auth,g_pub_tx.auth);

    switch(atoi(flag))
    {
        case 0:
            sprintf(acErrMsg,"¼��!!!!");
            WRITEMSG
           
            /* ����֤ȯ�ع�Э�����Ƿ����ظ� */
            ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
                  "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
            if (ret == 0)
            {
                sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ��Ѵ��ڼ�¼ [%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B087");
                goto ErrExit;
            }
            else if (ret != 100)
            {
                sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����!!");
                WRITEMSG
                goto ErrExit;
            }

            sprintf(acErrMsg,"AAAAAAAAAA");
            WRITEMSG
          
            ret = Mo_stok_rebuy_prot_Ins(sMo_stok_rebuy_prot,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��Ǽǲ��в����¼����!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }

            break;

        case 1 :
            /* �޸� */
            
            sprintf(acErrMsg,"�޸�!! ");
            WRITEMSG

            ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
                  "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
  
            if (sMo_stok_rebuy_prot1.sts[0] != '1')
            {
                sprintf(acErrMsg,"��¼��״̬�������޸�!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }

            ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",
                  sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Dec for update error  [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }

            ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot1,
                  g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Fetch error [%d]",ret);
                WRITEMSG
                Mo_stok_rebuy_prot_Clo_Upd();
                goto ErrExit;
            }

            ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,
                  g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Update error [%d]",ret);
                WRITEMSG
                Mo_stok_rebuy_prot_Clo_Upd();
                goto ErrExit;
            }

            Mo_stok_rebuy_prot_Clo_Upd();
            sprintf(acErrMsg,"�޸�֤ȯ�ع�Э��Ǽǲ�  pass!! ");
            WRITEMSG
            
            break;

        case 2 :
            /* ɾ�� */
            sprintf(acErrMsg,"ɾ��!! ");
            WRITEMSG

            ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
                  "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
  
            if (sMo_stok_rebuy_prot1.sts[0] != '1')
            {
                sprintf(acErrMsg,"��¼��״̬������ɾ��!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }

            ret = sql_execute("delete from mo_stok_rebuy_prot where \
                  pact_no='%s' and sts ='1'",sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"ɾ��֤ȯ�ع�Э��Ǽǲ�����!! [%d] ",ret);
                WRITEMSG
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"֤ȯ�ع���Ϣɾ��");
            sprintf(acErrMsg,"ɾ��֤ȯ�ع�Э��Ǽǲ��м�¼ pass!! ");
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
