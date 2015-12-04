/*************************************************************
* �� �� ��: gD019.c
* ������������������ת����
*
* ��    ��: jack
* �������: 2003��03��15��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

gD019()
{
	int ret = 0;

	double tram = 0.00;
	char cAdd_ind[2];


	struct in_mst_c sIn_mst;
	struct in_parm_c sIn_parm;
	struct com_item_c sCom_item;
	struct mdm_ac_rel_c sMdm_ac_rel;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	ret = In_mst_Dec_Sel( g_pub_tx.reply ,"bal != 0.00 and sts = '1'" );
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯ�ڲ��˳��� !!!" );
		WRITEMSG
		goto ErrExit;
	}

	while( 1 )
	{
		memset( &sIn_mst, 0x00, sizeof(sIn_mst) );

		ret = In_mst_Fet_Sel ( &sIn_mst , g_pub_tx.reply );
		if (ret == 100)
		{
			break;
		}
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ�ڲ������ļ���[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*-------- ��ѯ�����˺Ŷ��ձ� ----------*/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel , \
				" ac_id = %d and ac_seqn = %d", \
					sIn_mst.ac_id,sIn_mst.ac_seqn);
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ�˺Ž��ʶ��ձ��[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*--- ��ѯ��Ʒ������Ӧ�� ---*/
		memset( &sIn_parm, 0x00, sizeof(sIn_parm) );
		ret = In_parm_Sel( g_pub_tx.reply , &sIn_parm , \
				"prdt_no = '%s'",sIn_mst.prdt_no);
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ�ڲ��˲�Ʒ������[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( sIn_parm.acc_hrt[0] != '5' )
		{
			continue;	/*------ ���������Ŀ ------*/
		}

		memset( &sCom_item, 0x00, sizeof(sCom_item) );
		ret = Com_item_Sel( g_pub_tx.reply, &sCom_item, \
				"acc_hrt = '%s'", sIn_parm.acc_hrt);
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ������Ŀ���[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( cAdd_ind, 0x0, sizeof(cAdd_ind) );
		strcpy( cAdd_ind, sCom_item.dc_ind );

		tram = sIn_mst.bal;

		if (sCom_item.dc_ind[0] == '0')
		{
			if (sIn_mst.bal > 0.00)
			{
				strcpy( cAdd_ind, "1" );	/*���跽*/
			}
			else
			{
				tram = -tram;
				strcpy( cAdd_ind, "2" );	/*������*/
			}
		}

		sprintf(acErrMsg,"-------��ʼ�ڲ��˼���ȡ��------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");

		/*--��������ڽ跽���31100,�������ڴ����������--*/
		if ( cAdd_ind[0] == '1' )
		{
			strcpy(g_pub_tx.ac_no,"31100");	
		}
		if ( cAdd_ind[0] == '2' )
		{
			strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
		}

		g_pub_tx.tx_amt1 = tram;		/*���׽��*/
		strcpy(g_pub_tx.brf,"�ڲ���ȡ��");
		strcpy(g_pub_tx.ct_ind,"2");

		strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
		strcpy( g_pub_tx.ac_wrk_ind,"000");
		strcpy(g_pub_tx.add_ind,"0");		/*���ٱ�־*/
		strcpy( g_pub_tx.prt_ind,"0" ); 	/*������*/
		g_pub_tx.svc_ind=9001;  			/*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;

		sprintf(acErrMsg,"------��ʼ�ڲ��˼��ʴ��------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");

		/*--��������ڽ跽�������,����ڴ������31100--*/
		if ( cAdd_ind[0] == '1' )
		{
			strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
		}
		if ( cAdd_ind[0] == '2' )
		{
			strcpy(g_pub_tx.ac_no,"31100");	
		}

		g_pub_tx.tx_amt1 = tram;		/*���׽��*/
		strcpy(g_pub_tx.brf,"�ڲ��˴��");
		strcpy(g_pub_tx.ct_ind,"2");

		strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
		strcpy( g_pub_tx.ac_wrk_ind,"000"); /*���˶�ƾ֤��,�������ˮ,��ϸ*/
		strcpy(g_pub_tx.add_ind,"1");	/*���ӱ�־*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
		g_pub_tx.svc_ind=9001;  		/*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); 	/*�ռ�����ϸ*/

		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
	}

	In_mst_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ת����ͨ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	In_mst_Clo_Sel();
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��������ת����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
