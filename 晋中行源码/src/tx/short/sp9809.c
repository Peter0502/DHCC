/*************************************************
* �� �� ��:  sp9809.c
* ���������� �ɲ�������\�ʺ�\�˻����\֧ȡ����\֧ȡ��� 
*			 ���Կͻ���Ϣ
*
* ��    ��:  rob
* ������ڣ� 2003��1��15�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"


int sp9809()
{
	int	ret;
	char tag[2],tmp_ind2[2];				/*��������\ΥԼ��־*/
	long tmp_date1;
	long tmp_date2;
	double tmp_amt;							/*�����֧ȡ���*/

	struct	mo_infrm_c	g_mo_infrm;		
	struct  cif_basic_inf_c     g_cif_basic_inf;
	struct  cif_email_inf_c     g_cif_email_inf;
	struct  cif_id_code_rel_c   g_cif_id_code_rel;
	struct  prdt_ac_id_c        g_prdt_ac_id;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	get_zd_data( "0670", tag ); 	    			/* �������ͱ�־ */
	get_zd_data( "0310", g_pub_tx.ac_no ); 			/* �ʺ� */
	get_zd_long( "0440", &tmp_date1 ); 				/* ֧ȡ���� */
	get_zd_double( "0410", &g_pub_tx.tx_amt1 ); 	/* ֧ȡ��� */
	get_zd_long( "0450", &tmp_date2 ); 				/* �����֧ȡ���� */
	get_zd_double( "0420", &tmp_amt ); 				/* �����֧ȡ��� */
	get_zd_data( "0690", tmp_ind2 ); 				/* ΥԼ��־ */

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						  g_pub_tx.ac_no );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "�����˻����ձ��в����ڸ��˻�!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "���ҽ����˻����ձ��쳣!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W011" );
		goto ErrExit;
	}

	/*** �жϽ���״̬ ***/
	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý����ѱ���֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����Ѳ��ỻ֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf( acErrMsg, "�ʻ�������!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "�����쳣!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	ret =Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",g_td_mst.prdt_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "���ڲ��������ڸò�Ʒ!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "�����쳣!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	vtcp_log("---------------------------->g_td_parm.td_type==[%s]",
									g_td_parm.td_type);

	if( g_td_parm.td_type[0]!='4' )
	{
		sprintf( acErrMsg, "������֪ͨ����˺�!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O213" );
		goto ErrExit;
	}

	if( tag[0]=='2'||tag[0]=='3' )					/* ���������� */
	{
	 	/*�����ʺ�ID��֧ȡ���ڡ�֧ȡ����ҿͻ�������Ϣ*/
		ret=Mo_infrm_Sel( g_pub_tx.reply , &g_mo_infrm ,
					"ac_id=%ld and prm_draw_date=%ld and prm_draw_amt=%.2lf",
	     			g_mdm_ac_rel.ac_id,tmp_date1,g_pub_tx.tx_amt1 );	
		if( ret==100 )
		{
			sprintf( acErrMsg, "֪ͨ�ǼǱ���δ�ҵ��ñ��˻���¼!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "W057" );
			goto	ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "��ѯ֪ͨ�ǼǱ��쳣!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "W056" );
			goto	ErrExit;
		}

		set_zd_data( "0250", g_mo_infrm.name );			/* �˻����� */
		set_zd_data( "0610", g_mo_infrm.phone );		/* �绰���� */
		set_zd_data( "0680", g_mo_infrm.id_knd );		/* ֤������ */
		set_zd_data( "0620", g_mo_infrm.id_no );		/* ֤������ */
	}
	else if( tag[0]=='1' )								/* �Ǽ� */
	{
		ret = Cif_email_inf_Sel(g_pub_tx.reply,&g_cif_email_inf,"cif_no=%ld", \
								g_td_mst.cif_no);
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�绰�ǼǱ�����!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "C062" );
			goto ErrExit;
		}

		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
								  "cif_no=%ld", g_td_mst.cif_no);
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�ͻ���Ϣ�����!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "B115" );
			goto ErrExit;
		}

		ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
								g_td_mst.cif_no);
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�绰�ǼǱ�����!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "C062" );
			goto ErrExit;
		}

		set_zd_data( "0250", g_cif_basic_inf.name );		/* �˻����� */
		set_zd_data( "0610", g_cif_email_inf.email );		/* �绰���� */
		set_zd_data( "0680", g_cif_id_code_rel.id_type );	/* ֤������ */
		set_zd_data( "0620", g_cif_id_code_rel.id_no );		/* ֤������ */
	}
	else 
	{
		sprintf( acErrMsg, "��������ѡ�����!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "C062" );
		goto ErrExit;
	}

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
