/***************************************************************
* �� �� ��:     spD294.c
* ����������	����ͬ������(ͨ��)
* ��    ��:     mike
* ������ڣ�    2004��1��25��
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
#include "mo_city_c.h"
#include "com_branch_c.h"

spD294()
{
	char tmp_memo1[50],tmp_memo2[50];
	long date1,date2;
	double bal;

	struct mo_city_c	g_mo_city;
	struct com_branch_c	g_com_branch;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct dd_parm_c	g_dd_parm;
	struct td_parm_c	g_td_parm;

	memset(&g_mo_city,0x00,sizeof(struct mo_city_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0310",g_mo_city.payee_ac_no);			/*�տ����ʺ�*/
	get_zd_data("0250",g_mo_city.payee_name);			/*�տ�������*/
	get_zd_double("0410",&bal);							/*�տ��ʻ����*/
	get_zd_data("0860",g_mo_city.payee_br_no);			/*�տ��˿����к�*/
	get_zd_data("0270",g_mo_city.payee_br_name);		/*�տ��˿�������*/

	get_zd_data("0890",g_mo_city.bill_type);			/*Ʊ������*/
	strcpy(g_pub_tx.note_type,g_mo_city.bill_type);
	get_zd_data("0580",g_mo_city.bill_no);				/*Ʊ�ݺ���*/
	get_zd_data("0810",g_pub_tx.brf);					/*ժҪ*/
	get_zd_double("0420",&g_mo_city.sttl_amt);			/*������*/

	get_zd_data("0320",g_mo_city.payer_ac_no);			/*�������ʺ�*/
	get_zd_data("0260",g_mo_city.payer_name);			/*����������*/
	get_zd_data("0870",g_mo_city.payer_br_no);			/*�����˿�������*/
	get_zd_data("0820",g_mo_city.payer_br_name);		/*�����˿�������*/
	
	get_zd_double("0430",&g_mo_city.bill_amt);			/*Ʊ����*/
	get_zd_data("0830",tmp_memo2);						/*��ע*/
	strcpy(g_mo_city.sts,"0");							/*0-¼��*/

	strcpy(g_mo_city.in_out_ind,"2");					/*2-����*/
	strcpy(g_mo_city.cd_ind,"2");						/*2-��*/
	strcpy(g_mo_city.trans_mode,"2");					/*2-�Զ�*/
	strcpy(g_mo_city.tx_br_no,g_pub_tx.tx_br_no);		/*������*/
	g_mo_city.tx_date = g_pub_tx.tx_date;				/*��������*/
	g_mo_city.tx_no = g_pub_tx.trace_no;				/*��ˮ��*/

	strcpy(g_pub_tx.ac_no,g_mo_city.payee_ac_no);		/*�տ����ʺ�*/
	strcpy(g_pub_tx.add_ind,"1");						/*���տ��˴��*/
	g_pub_tx.tx_amt1 = g_mo_city.sttl_amt;				/*�������*/

	/*ȡ���˺�*/
 	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
							"ac_no='%s'",g_mo_city.payee_ac_no);
 	if( g_reply_int==100 )
 	{
	 	sprintf( acErrMsg,"�����˺Ŷ��ձ��в����ڴ˼�¼!!erro code=[%d]",
				g_reply_int);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg,"��ѯ�����ʺŶ��ձ��쳣!!erro code=[%d]",
				 g_reply_int);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;			/*�˺���Ÿ�ֵ*/

	g_reply_int=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",
								g_pub_tx.tx_br_no);
	if (g_reply_int==100)
	{
		sprintf(acErrMsg,"�����������в����ڸü�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}
	else if (g_reply_int)
	{
		sprintf(acErrMsg,"��ѯ�����������쳣!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	g_mo_city.no = g_com_branch.city_no;				/*����*/

	/*�Ǽ�ͬ�ǵǼǱ�*/
	g_reply_int=Mo_city_Ins(g_mo_city,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"�Ǽ�ͬ�ǵǼǱ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O099");
		goto ErrExit;
	}

	if (pub_acct_trance())
	{
	 	sprintf(acErrMsg,"���ʴ���!!");
	  	WRITEMSG
	    goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ͬ������(ͨ��)�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ͬ������(ͨ��)ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
