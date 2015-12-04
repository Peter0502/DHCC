/*************************************************
* �� �� ��:  spB704.c
* ���������� ֤ȯ�ع��������δ������
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

int spB704()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
    struct in_mst_c sIn_mst;
    char flag[2],gDc_code[5];
    int ret;
  
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(flag,0x0,sizeof(flag));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);
    strcpy(flag,"2");		/** ת�� **/
    get_zd_data("0210",g_pub_tx.cur_no);
    get_zd_data("0890",g_pub_tx.note_type);
    get_zd_data("0580",g_pub_tx.beg_note_no);
    get_zd_data("0580",g_pub_tx.end_note_no);

    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
     " pact_no='%s' and buy_type='1' and sts='2'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    pub_base_strpack(sMo_stok_rebuy_prot.opt_acno);

    /* ���֤ȯ�ع�����˻����ͽ��׽��Ĵ�С */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,g_pub_tx.tx_amt1) != 0)
    {
        sprintf(acErrMsg,"���׽�����֤ȯ�ع�����ʻ����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B103");
        goto ErrExit;
    }
	/* ����Ƿ��� */
	if( sMo_stok_rebuy_prot.matr_date>g_pub_tx.tx_date )
    {
        sprintf(acErrMsg,"�ñ�֤ȯ�ع�δ����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P303");
        goto ErrExit;
    }
    
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.ac_seqn = 0;

    /* �Խ���ʻ��������� */
    g_pub_tx.add_ind[0] = '1';	/*** �ڲ�����Ϊ����,�������� ***/
	g_pub_tx.svc_ind==9002;

    /* �޸ĵǼǲ���״̬Ϊ���� */

    memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
    memset(g_pub_tx.name,0x0,sizeof(g_pub_tx.name));
    g_pub_tx.ac_id = sMo_stok_rebuy_prot.ac_id;
vtcp_log("[%s][%s]=========g_pub_tx.ac_id=[%d],ac_seqn=[%d]",__FILE__,__LINE__,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"��֤ȯ�ع�����ʻ��������������!!!");
        WRITEMSG
        goto ErrExit;
    }
	strcpy(gDc_code,g_pub_tx.dc_code);

    /* ������ ת����Ӧ�տ� 13903*/
/*	��������ͨ��ǰ̨����
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
    strcpy(g_pub_tx.prdt_code,REST_AC_RECE); 
	strcpy(g_pub_tx.note_type,"");
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.end_note_no,"");
	g_pub_tx.tx_amt1=0.00;
  
vtcp_log("[%s][%d]--------g_pub_tx.prdt_code=[%s]",__FILE__,__LINE__,g_pub_tx.prdt_code);
    ret = pub_acct_opn_in_mdm();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ʳ���");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    WRITEMSG
    sprintf(acErrMsg,"�����ʳɹ� PASS!!! ");
    WRITEMSG

    strcpy(g_pub_tx.ac_id_type,"9");
	get_zd_double("0840",&g_pub_tx.rate);
    g_pub_tx.add_ind[0] = '1';
	g_pub_tx.hst_ind[0] = '1';
	strcpy(g_pub_tx.tmp_ind,"1");
    get_zd_double("0400",&g_pub_tx.tx_amt1);

    if(pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"����13903����");
        WRITEMSG
        goto ErrExit;
    }
*/

    sMo_stok_rebuy_prot.ac_id = g_pub_tx.ac_id;
    sMo_stok_rebuy_prot.reg_date = g_pub_tx.tx_date;
    sMo_stok_rebuy_prot.trace_no = g_pub_tx.trace_no;
	sMo_stok_rebuy_prot.over_rate=g_pub_tx.rate;
    strcpy(sMo_stok_rebuy_prot.sts,"3");
  	sMo_stok_rebuy_prot.bal -=g_pub_tx.tx_amt1;

    /* �޸�֤ȯ�ع�Э��Ǽǲ� */
    ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"br_no='%s' and pact_no='%s' and buy_type='1'",g_pub_tx.tx_br_no,sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE for update error [%d] ",ret);
        WRITEMSG
        goto ErrExit;
    }   

    ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch error [%d] ",ret);
        WRITEMSG
        Mo_stok_rebuy_prot_Clo_Upd();
        goto ErrExit;
    }

    ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update error [%d]",ret);
        WRITEMSG
        Mo_stok_rebuy_prot_Clo_Upd();
        goto ErrExit;
    }
    Mo_stok_rebuy_prot_Clo_Upd();

	set_zd_data(DC_CODE,gDc_code);
	/* ��������ֱ�Ӵ�ǰ̨�õ����Բ�����  modified by liuxuan 2006-12-2
    set_zd_data("0310",g_pub_tx.ac_no);
	*/
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
