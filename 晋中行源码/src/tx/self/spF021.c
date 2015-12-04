/*************************************************
* �� �� ��:  spF021.c
* ���������� POS ����(�������ʻ���ת���ӽ���)(���п�)
*
* ��    ��:  rob
* ������ڣ� 2003��2��19��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
int spF021()
{
    	int ret=0;
    	char tty[9];
    	char note_no[21];
    	char in_type[5];
    	char tmpstr[100];

    	if (pub_base_sysinit())
    	{
        	sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        	WRITEMSG
        	goto ErrExit;
    	}

	memset(tty, 0x0, sizeof(tty));
	memset(note_no, 0x0, sizeof(note_no));
	memset(in_type, 0x0, sizeof(in_type));

    	get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    	get_zd_data("0800",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
	get_zd_data("0380", tty); /* �ն˺� */
	get_zd_data("0620", note_no); /* �̻��� */
	get_zd_data("0910", in_type); /* �̻�����ʽ */

	sprintf(acErrMsg, " ----����: [%s], ����:, ���׽��: [%lf]", 
		g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	WRITEMSG

    	/* ��鿨�ĺϷ��� */
    	if (pub_base_CheckCrdNo())
    	{
       	 	sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        	WRITEMSG
			strcpy(g_pub_tx.reply, "AT01");
        	goto ErrExit;
    	}

	if ( pub_mobile_pay(in_type, tty, note_no) )
	{
		/* �������ƶ�֧�� */
    	/* ��鿨��֧ȡ��ʽ */
    	if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    	{
        	sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT02");
        	goto ErrExit;
    	}
		
		/*vtcp_log(" g_pub_tx.draw_pwd====[%s]  g_mdm_ac_rel.draw_pwd===[%s]",g_pub_tx.draw_pwd ,g_mdm_ac_rel.draw_pwd);*/
    	/* ���֧ȡ��ʽ�ĺϷ��� */
    	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"����֧ȡ�������!!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT04");
        	goto ErrExit;
    	}
	}

    /* ����ȡ��� */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

	/********
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;
    strcpy(g_pub_tx.ct_ind,"2"); * ת�� *
    g_pub_tx.hst_ind[0] = '1';   * �ռ�����ϸ *

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos����--�ʻ�ȡ�����!!");
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

