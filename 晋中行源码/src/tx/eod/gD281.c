/***************************************************************
* �� �� ��: gD281.c
* ��������: 40710ת�뵽114�ʻ�0828
*
* ��    ��: rob
* �������: 2003��6��24��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

char  def_br_no[6];
char  def_tel[5];

gD281()
{
    int ret=0;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_parm_c sCom_parm;

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	memset(def_br_no, 0x0, sizeof(def_br_no));
	memset(def_tel, 0x0, sizeof(def_tel));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);

	sprintf(acErrMsg, "-------->��������Ϊ:[%ld]", g_pub_tx.tx_date);
	WRITEMSG

	/* ���ܽ跽������ */
	ret = sql_sum_double("dc_log_rz", "amt", &c_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='1' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����40710�Ľ跽������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log_rz", "amt", &d_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='2' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����40710�Ĵ���������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	sprintf(acErrMsg, "40710�Ľ跽���Ϊ:[%lf], 40710�Ĵ������Ϊ:[%lf]",
		c_bal, d_bal);
	WRITEMSG

	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.brf, "ͨ������0828��");
	c_bal = 300.00; /* ��20030828 */
	d_bal = 0.00;
	if (pub_base_CompDblVal(c_bal, d_bal) == 1)
	{
		/* �跽�� */
		strcpy(g_pub_tx.add_ind, "1"); /* 40710���� */
		g_pub_tx.tx_amt1 = c_bal - d_bal;
		dc_ind = '1';
	}
	else if (pub_base_CompDblVal(c_bal, d_bal) == -1)
	{
		/* ������ */
		strcpy(g_pub_tx.add_ind, "0"); /* ��40710���� */
		g_pub_tx.tx_amt1 = d_bal - c_bal;
		dc_ind = '0';
	}
	else
	{
		sprintf(acErrMsg, "����40710���ܷ�����Ϊ��!!");
		WRITEMSG
		goto OkExit;
	}

	strcpy(g_pub_tx.ac_no, "40710"); 
	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/ 
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "���ô�ȡ������ʧ��!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	/* ��40710����� */
	if (dc_ind == '0')
	{
		/* �ڲ���40710 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '1')
	{
		/* �ڲ��� A017 40710*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "�����־����!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "��40710����ʴ���!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg, "*******114 ************");
	WRITEMSG

	/* ��114�˺� */
	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c)); 
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}
	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);
	strcpy(g_pub_tx.cur_no, "01"); 
	strcpy(g_pub_tx.ct_ind, "2"); 
	strcpy(g_pub_tx.brf, "ͨ������0828��");
	if (dc_ind == '0') /* �� */
	{
		strcpy(g_pub_tx.add_ind, "1");
		g_pub_tx.tx_amt1 = d_bal - c_bal;
	}
	else /* �� */
	{
		strcpy(g_pub_tx.add_ind, "0");
		g_pub_tx.tx_amt1 = c_bal - d_bal;
	}
	sprintf(acErrMsg, "------------>[%lf]", g_pub_tx.tx_amt1);
	WRITEMSG

	ret= Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='YDTD' and \
		parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯ�������������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sCom_parm.val);
	strcpy(g_pub_tx.ac_no, sCom_parm.val);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/ 
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/ 
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

	sprintf(acErrMsg, "--------->[%s]", g_pub_tx.add_ind);
	WRITEMSG
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "���ô�ȡ������ʧ��!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	if (dc_ind == '1') /* 40710�� */
	{
		/* �ڲ���114 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '0')
	{
		/* �ڲ���114 A017*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "�����־����!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "��114�˺Ż���ʴ���!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"40710ת��114�˺Ŵ���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"40710ת��114�˺Ŵ���ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
