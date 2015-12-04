/***************************************************************
* �� �� ��: spD413.c
* �������������޶�ά��
* ��    ��: mike
* ������ڣ�2003��03��25��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card_consum_lmt_c.h"

spD413()
{
	int ret = 0;
	char tmp_ac_no[20];

	struct  card_consum_lmt_c   g_card_consum_lmt;
	struct  card_consum_lmt_c   card_consum_lmt;
	struct  mdm_ac_rel_c   		f_mdm_ac_rel;

	memset(&card_consum_lmt,0x00,sizeof(struct card_consum_lmt_c));
	memset(&g_card_consum_lmt,0x00,sizeof(struct card_consum_lmt_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*-------- ȡ�������  ---------*/
	get_zd_data("1021",g_pub_tx.ac_no);						/*������*/
	get_zd_data("0310",tmp_ac_no);							/*������*/
	get_zd_double("0390",&card_consum_lmt.da_cash_lmt);    	/*��ȡ���޶�*/
	get_zd_double("0400",&card_consum_lmt.da_amt_lmt);     	/*��ȡ���޶�*/
	get_zd_double("0410",&card_consum_lmt.da_con_lmt); 	 	/*�������޶�*/
	get_zd_double("0420",&card_consum_lmt.od_cash_lmt);     /*�ֽ�͸֧�޶�*/
	get_zd_double("0430",&card_consum_lmt.od_amt_lmt);		/*͸֧�޶�*/
	get_zd_double("1001",&card_consum_lmt.mo_cash_lmt);    	/*��ȡ���޶�*/
	get_zd_double("1002",&card_consum_lmt.mo_amt_lmt);     	/*��ȡ���޶�*/
	get_zd_double("1003",&card_consum_lmt.mo_con_lmt);  	/*�������޶�*/

	get_zd_data("1142",g_pub_tx.draw_pwd_yn);
	get_zd_data("1144",g_pub_tx.draw_pwd);
 	get_zd_data("1145",g_pub_tx.draw_id_yn);
	get_zd_data("1148",g_pub_tx.id_type);
	get_zd_data("1146",g_pub_tx.id_no);

	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.ac_no,&g_mdm_ac_rel,"ac_no='%s'", \
								 g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
	  	sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P237");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.main_ind[0]!='1')
	{
		sprintf(acErrMsg,"������������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P236");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]!='0')
	{
		sprintf(acErrMsg,"�������ڷ�����״̬!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P239");
		goto ErrExit;
	}

	/*���֧ȡ��ʽ�Ƿ�Ϸ�*/
	ret = pub_base_check_draw("001");
	if( ret )
	{
	 	sprintf(acErrMsg,"���ü��֧ȡ��ʽ�Ƿ�Ϸ���������!!");
		WRITEMSG
		goto ErrExit;
	}

/**********************************************************************/

	/*��鸱���Ƿ����*/
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.ac_no,&f_mdm_ac_rel,"ac_no='%s'", \
								 tmp_ac_no);
	if( g_reply_int==100 )
	{
	  	sprintf(acErrMsg,"�����˻����ձ��в����ڸø�����¼");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P238");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(f_mdm_ac_rel.note_sts[0]!='0')
	{
		sprintf(acErrMsg,"�������ڷ�����״̬!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P240");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.ac_id!=f_mdm_ac_rel.ac_id)
	{
		sprintf(acErrMsg,"�����������ʺ�ID��һ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P235");
		goto ErrExit;
	}

	if(f_mdm_ac_rel.main_ind[0]!='2')
	{
		sprintf(acErrMsg,"�������Ǹ���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P241");
		goto ErrExit;
	}

	/*�������޶�ǼǱ�*/
	ret=Card_consum_lmt_Sel(g_pub_tx.reply,&g_card_consum_lmt, \
	  						"ac_no='%s'",tmp_ac_no);
    if( ret==100 )
  	{
		strcpy(card_consum_lmt.ac_no,tmp_ac_no);

		ret = Card_consum_lmt_Ins(card_consum_lmt,g_pub_tx.reply);
		if( ret==-239 )
		{
		  	sprintf(acErrMsg,"�Ǽǿ��޶�ǼǱ��쳣[%s]",ret);
		 	WRITEMSG
		 	strcpy( g_pub_tx.reply, "D117" );
		  	goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg, "insert error !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
 	}
  	else if(ret==0)
	{
    	ret=sql_execute("update card_consum_lmt set da_cash_lmt=%.2lf, \
						da_amt_lmt=%.2lf,da_con_lmt=%.2lf,od_cash_lmt=%.2lf, \
						od_amt_lmt=%.2lf,mo_cash_lmt=%.2lf,mo_amt_lmt=%.2lf, \
						mo_con_lmt=%.2lf where ac_no='%s'", \
						card_consum_lmt.da_cash_lmt, \
						card_consum_lmt.da_amt_lmt, \
						card_consum_lmt.da_con_lmt, \
						card_consum_lmt.od_cash_lmt, \
						card_consum_lmt.od_amt_lmt, \
						card_consum_lmt.mo_cash_lmt, \
						card_consum_lmt.mo_amt_lmt, \
						card_consum_lmt.mo_con_lmt, \
						tmp_ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"���¿��޶�ǼǱ��쳣[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "D118" );
			goto ErrExit;
		}
	}
	else{
		  sprintf(acErrMsg,"��ѯ���޶�ǼǱ��쳣[%s]");
		  WRITEMSG
		  strcpy( g_pub_tx.reply, "D119" );
		  goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���޶�ά���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���޶�ά��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
