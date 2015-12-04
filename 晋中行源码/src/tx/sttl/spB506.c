/*************************************************
* �� �� ��:  spB506.c
* ���������� ͬҵ�������
*
* ��    ��:  rob
* ������ڣ� 2003��1��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"

int spB506()
{
    struct mo_ibl_c sMo_ibl_c;
    struct mdm_ac_rel_c sMdm_ac_rel;
    double  bal;
    char ac_no[20];
    int ret;
	long trace_no;
  
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",sMo_ibl_c.pact_no);      /*��ͬ��*/
    get_zd_double("0420",&g_pub_tx.tx_amt1);    /*������*/
    get_zd_double("0410",&bal);                 /*����˻����*/

    
    /* �޸Ĳ��Ǽǲ�״̬ */
    ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts = '2' and \
          sign = '2'", sMo_ibl_c.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_ibl_Fet_Upd(&sMo_ibl_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"fecth for update error !! [%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        goto ErrExit;
    }

    /* ������˺������Ƿ�Ͳ��Ǽǲ����˻�һ�� *
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",
          sMdm_ac_rel.ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ���ʺ��˻���ϵ�����!! [%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
    if (sMdm_ac_rel.ac_id != sMo_ibl_c.ac_id)
    {
        sprintf(acErrMsg,"����˺ź͵Ǽǲ��еĵǼ��˺Ų�ͬ");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B078");
        goto ErrExit;
    }
	*****/

    /* ������Ƿ����δ������ */
    if (pub_base_CompDblVal(bal,g_pub_tx.tx_amt1) == -1)
    {
        sprintf(acErrMsg,"��������ڵǼǲ��е�δ������!!");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B079");
        goto ErrExit;
    }

    g_pub_tx.hst_ind[0] = '1';
	g_pub_tx.ac_id = sMo_ibl_c.ac_id;
	g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.brf,"ͬҵ�������");

	if (pub_base_CompDblVal(bal, g_pub_tx.tx_amt1) == 0)
    {
        /* ���� */
        g_pub_tx.add_ind[0] = '1';
		g_pub_tx.svc_ind = 1003;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"�������� ");
            WRITEMSG
            Mo_ibl_Clo_Upd();
            goto ErrExit;
        }

        sprintf(acErrMsg,"����˻������ɹ�!!!! pass!!");
        WRITEMSG
       
        strcpy(sMo_ibl_c.sts,"*");
        sMo_ibl_c.no_pay_bal = bal - g_pub_tx.tx_amt1;

    }
    else
    {
        /* �����˻���ȡ�� */
        g_pub_tx.add_ind[0] = '1';
        g_pub_tx.ac_seqn = 0;
		g_pub_tx.svc_ind = 1001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"�����˻�������");
            WRITEMSG
            Mo_ibl_Clo_Upd();
            goto ErrExit;
        }

        sMo_ibl_c.no_pay_bal = bal - g_pub_tx.tx_amt1;

        sprintf(acErrMsg,"����˻����˳ɹ�!!!! pass!!");
        WRITEMSG
    }

	trace_no = sMo_ibl_c.trance_no;
	sMo_ibl_c.trance_no = g_pub_tx.trace_no;

    ret = Mo_ibl_Upd_Upd(sMo_ibl_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"���²��Ǽǲ�����!! ret =[%s]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B008");
        goto ErrExit;
    }
    Mo_ibl_Clo_Upd();

    sprintf(acErrMsg," PASS!!!!!");
    WRITEMSG

	/******
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
	********/

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

