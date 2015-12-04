/*************************************************
* �� �� ��:  spF008.c
* ���������� ATM ת��(ת�����ӽ���)
*
* ��    ��:  rob
* ������ڣ� 2003��2��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_tradelimit_c.h"

int spF008()
{

    int ret;

    struct chnl_tradeadd_c chnl_tradeadd;
    struct chnl_tradelimit_c chnl_tradelimit;

    memset(&chnl_tradeadd, 0x0, sizeof(struct chnl_tradeadd_c));
    memset(&chnl_tradelimit, 0x0, sizeof(struct chnl_tradelimit_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* ת������ */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_double("0400",&g_pub_tx.tx_amt1); /* ���׽�� */

    strcpy(g_pub_tx.cur_no,"01"); /* ����� */

    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ��鵥��ȡ�����Ƿ��� * 
    ret = pub_base_Checktradelimit('0'); 
    if (ret != 0) 
    { 
	sprintf(acErrMsg,"���atmȡ��Ľ������Ƴ���!!!"); 
	WRITEMSG 
	goto ErrExit; 
    }
    Remmed for Test */ 


    /* ��鿨��֧ȡ��ʽ */
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        goto ErrExit;
    }

    /* ���֧ȡ��ʽ�ĺϷ��� */
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����֧ȡ�������!!!");
        WRITEMSG 
		ret = sql_commit(); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"�ύ�������!!"); 
			WRITEMSG 
			goto ErrExit; 
		} 
		ret = sql_begin(); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"��ʼ�������!!"); 
			WRITEMSG 
			goto ErrExit; 
		} 
		
		ret = Chnl_tradeadd_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and \ 
				ac_no = '%s' and tx_type = '1' and stat='0'", 
				g_pub_tx.tx_date, g_pub_tx.crd_no); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"Declare error!! [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		
		sprintf(acErrMsg,"tx_date=[%ld],   ac_no=[%s]", g_pub_tx.tx_date, 
			g_pub_tx.crd_no); 
		WRITEMSG 
		
		ret = Chnl_tradeadd_Fet_Upd(&chnl_tradeadd,g_pub_tx.reply); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"Fetch error !! [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		
		/* ��ѯ�������������� */ 
		ret = Chnl_tradelimit_Sel(g_pub_tx.reply,&chnl_tradelimit,"chnl='0'"); 
		if (ret != 0) 
		{ 	
			sprintf(acErrMsg,"��ѯ�����������Ʊ����!!",ret); 
			WRITEMSG 
			chnl_tradelimit.pwd_cnt = 3; 
		} 
		
		/* ���½��׼�¼�ۼƿ��Ʊ� */ 
		chnl_tradeadd.add_pswd_cnt = chnl_tradeadd.add_pswd_cnt + 1; 
		
		if (chnl_tradeadd.add_pswd_cnt > chnl_tradelimit.pwd_cnt) 
		{ 
			/* atm�̿���Ӧ�� */ 
			strcpy(g_pub_tx.reply,"AT05"); 
			chnl_tradeadd.add_pswd_cnt = 0; 
			/* �޸Ľ����˺Ŷ��ձ� */ 
			ret  = sql_execute("update mdm_ac_rel set coll_sts='2' where \ 
				ac_no = '%s' ", g_pub_tx.crd_no); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg, "����mdm_ac_rel�����!! [%d]", ret); 
				WRITEMSG 
				strcpy(g_pub_tx.reply, "AT08"); 
				goto ErrExit; 
			} 
		} 
		else 
		{ 
			/* ������� */ 
			strcpy(g_pub_tx.reply,"AT04"); 
		} 
		/* ���¿��Ʊ� */ 
		ret = Chnl_tradeadd_Upd_Upd(chnl_tradeadd,g_pub_tx.reply); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"���½��׼�¼�ۼƿ��Ʊ����!! [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		Chnl_tradeadd_Clo_Upd(); 
		ret = sql_commit(); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"�ύ�������!!"); 
			WRITEMSG 
		} 
		ret = sql_begin(); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"��ʼ�������!!"); 
			WRITEMSG 
		} 
		goto ErrExit; 
    }

	if (g_mdm_ac_rel.note_sts[0] != '0') 
	{ 
		sprintf(acErrMsg, "�ÿ�Ϊ��Ч��!! "); 
		WRITEMSG 
		if (g_mdm_ac_rel.note_sts[0] == '1' || g_mdm_ac_rel.note_sts[0] == '2') 
		{ 
			strcpy(g_pub_tx.reply, "AT12"); 
		} 
		else 
		{ 
			strcpy(g_pub_tx.reply, "AT01"); 
		} 
		goto ErrExit; 
	}

    set_zd_data("0210",g_pub_tx.cur_no);
	ret = sql_execute("update chnl_tradeadd set add_pswd_cnt=0, \
		add_amt=add_amt + %lf, add_qk_cnt = add_qk_cnt + 1 where \
		tx_date=%ld and ac_no='%s' and stat='0' and tx_type='1'",
		g_pub_tx.tx_amt1, g_pub_tx.tx_date, g_pub_tx.crd_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "update chnl_tradeadd error [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
		goto ErrExit;
	}

	/*************
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }

    * ����ȡ��� *
    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); * ת�� *
    g_pub_tx.hst_ind[0] = '1';   * �ռ�����ϸ *

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"atmת��--ת���ʻ�ȡ�����!!");
        WRITEMSG
        goto ErrExit;
    }
	*******/

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

