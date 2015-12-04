/*************************************************************
* �� �� ��:     spD698.c
* ����������    �ʻ�����ѯ 
* ��    ��:     
* ������ڣ�    2003��8��25��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spD698()
{
	int ret=0;
	char name[50];
	long cif_no;
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/
	get_zd_data("0300",g_pub_tx.ac_no);
    pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);		
	get_zd_data("0800",g_pub_tx.draw_pwd);			/*** ֧ȡ���� ***/
	get_zd_data("0680",g_pub_tx.draw_id_yn);
	get_zd_data("0690",g_pub_tx.id_type);			/*** ֤������ ***/
	get_zd_data("0310",g_pub_tx.id_no);				/*** ֤������ ***/
/*vtcp_log("������:[%s]",g_pub_tx.draw_pwd);*/
	/* ȡ�ʻ���Ϣ */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
							g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d]");
		WRITEMSG
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ�[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}

	/*** �����˻�״̬ ***/
	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if(g_mdm_ac_rel.note_sts[0]=='2')
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if(g_mdm_ac_rel.note_sts[0]=='3')
	{
		sprintf(acErrMsg,"�ý�����������֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����������ᣬ���������ʺ�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O217");
		goto ErrExit;
	}

	if( g_pub_tx.draw_pwd_yn[0]=='Y' )
	{
		pub_base_EnDes( g_pub_tx.tx_date,"",g_pub_tx.draw_pwd );
		pub_base_strpack( g_mdm_ac_rel.draw_pwd );
		if( strcmp(g_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) )
		{
			sprintf(acErrMsg, "�������!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "O123");
			goto ErrExit;
		}
	}

	if( g_pub_tx.draw_id_yn[0]=='Y' )
	{
		if( g_pub_tx.id_type[0]!=g_mdm_ac_rel.id_type[0] )
		{
		 	sprintf(acErrMsg, "֤�����Ͳ���!! [%s]", g_pub_tx.id_type);
		   	WRITEMSG
			strcpy(g_pub_tx.reply, "P444");
			goto ErrExit;
		}

		pub_base_strpack( g_mdm_ac_rel.id_no );
		pub_base_strpack( g_pub_tx.id_no );
		if( strcmp(g_pub_tx.id_no,g_mdm_ac_rel.id_no) )
		{
		 	sprintf(acErrMsg, "֤�����벻��ȷ!! [%s]", g_pub_tx.id_type);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P445");
			goto ErrExit;
		}
	}

	/*** ȡ�˻���� ***/
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, "ac_id=%ld", \
							g_mdm_ac_rel.ac_id );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�˺Ŵ���![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}

	if( g_prdt_ac_id.ac_id_type[0]=='1' )
	{
		ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
		ERR_DEAL

		if( g_dd_mst.ac_sts[0]=='*' )
		{
			sprintf(acErrMsg,"�˻�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P245" );
			goto ErrExit;
		}	
		bal = g_dd_mst.bal ;
	}
	else if( g_prdt_ac_id.ac_id_type[0]=='2' )
	{
		ret = Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
		ERR_DEAL
		if( g_td_mst.ac_sts[0]=='*' )
		{
			sprintf(acErrMsg,"�˻�����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P245" );
			goto ErrExit;
		}	
		bal = g_td_mst.bal ;
	}
	else 
	{
		sprintf(acErrMsg,"�˻����ʹ���!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"O166" );
		goto ErrExit;
	}

	set_zd_double( "0420" , bal );
/*
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
*/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯ��ӡ�ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"��ѯ��ӡʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
