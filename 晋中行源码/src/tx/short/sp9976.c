/*************************************************************
* �� �� ��:  sp9976.c
* ���������� ������ʵ���ӡ��ݲ�ѯ
*
* ��    ��:  
* ������ڣ� 2003��8��14��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "prdt_ac_id_c.h"

int sp9976()
{
	struct prdt_ac_id_c g_prdt_ac_id;

	memset( &g_prdt_ac_id, 0x00, sizeof( struct prdt_ac_id_c ) );

	pub_base_sysinit();

	get_zd_data( "0310", g_pub_tx.ac_no );   		/* �����˺� */
	pub_base_old_acno( g_pub_tx.ac_no );           /****�����ʺŴ���****/

	g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, 
								 "ac_no='%s'", g_pub_tx.ac_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "���˺Ų�����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "���ҳ���[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, " ac_id=%ld ", 
							  	  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "���˺Ų�����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "���Ҵ���[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_prdt_ac_id.prdt_no[0]!='3' )
	{
		sprintf( acErrMsg, "�Ǵ����˺�" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L114");
		goto ErrExit;
	}

	g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst, " ac_id=%ld ", 
							  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "�����˺Ų�����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "���Ҵ���[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/****����ͬһ����������ͨ�� 20070528****/
	vtcp_log("[%s][%d] ln.opn_br_no = [%s]\n",__FILE__,__LINE__,g_ln_mst.opn_br_no);
	if(memcmp(g_ln_mst.opn_br_no,g_pub_tx.tx_br_no,5) != 0)
	{
		sprintf( acErrMsg, "���ʺŲ��Ǳ������ʺ�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D124");
		goto ErrExit;
	}
	/****end****/
	sprintf( acErrMsg, "����[%s]",g_ln_mst.name );
	WRITEMSG
	set_zd_data( "0250", g_ln_mst.name );
	set_zd_double( "0410", g_ln_mst.bal ); /****�������****/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
