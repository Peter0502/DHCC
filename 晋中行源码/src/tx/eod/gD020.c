/*************************************************************
* �� �� ��: gD020.c
* �������������������ת(31100->31204)
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

gD020()
{
	int ret = 0;

	double tram = 0.00;
	char cAdd_ind[2];


	struct in_mst_c sIn_mst;
	struct in_parm_c sIn_parm;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	/*--- ��ѯ��Ʒ������Ӧ�� ---*/
	memset( &sIn_parm, 0x00, sizeof(sIn_parm) );
	ret = In_parm_Sel( g_pub_tx.reply , &sIn_parm , \
			"acc_hrt = '31100'");
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯ�ڲ��˲�Ʒ������[%s] !!!", g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	/*----------��ѯ�ڲ������ļ�--------*/ 
	memset( &sIn_mst, 0x00, sizeof(sIn_mst) );
	ret = In_mst_Sel( g_pub_tx.reply , &sIn_mst , \
			"prdt_no = '%s'", sIn_parm.prdt_no);
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯ�ڲ��˲�Ʒ������[%s] !!!", g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"-------��ʼ�ڲ��˼���ȡ��------");
	WRITEMSG
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");


	strcpy(g_pub_tx.ac_no,"31100");	
	g_pub_tx.tx_amt1 = sIn_mst.bal;		/*���׽��*/
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

	sprintf(acErrMsg,"------��ʼ�ڲ��˼��ʴ��------");
	WRITEMSG
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");

	strcpy(g_pub_tx.ac_no,"31204");	
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
	strcpy(g_pub_tx.beg_note_no,"");

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���������ת�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���������תʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
