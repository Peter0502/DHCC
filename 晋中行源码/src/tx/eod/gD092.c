/*************************************************************
* �� �� ��: gD092.c
* ������������֤�𰴼���ȡ������
*
* �������: 2003��08��19��
*
* ��    �ߣ�dong
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "protect_note_c.h"

gD092()
{
	int ret = 0;
	struct	protect_note_c	pro_note;

	memset( &pro_note , 0x00 , sizeof( struct protect_note_c ));


	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	/** ȡ����ǰ��Ҫ ��ȡ�����ѵļ�¼ **/
	ret = Protect_note_Dec_Upd(g_pub_tx.reply,"nex_date=%d AND protect_sts='0' ",g_pub_tx.tx_date);
	while(1)
	{
		ret = Protect_note_Fet_Upd( &pro_note , g_pub_tx.reply);
		if ( ret ) break;
		ret=sql_begin(); /*������*/
		if( ret ) 
		{
			sprintf( acErrMsg, "������ʧ��!!!" );
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.tx_amt1 = pro_note.amt*pro_note.ratio/100;

		vtcp_log("PACT_NO[%s] amt=[%lf] ratio=[%d] amt1=[%lf] [%s->%d]"
			,pro_note.pact_no,pro_note.amt,pro_note.ratio,g_pub_tx.tx_amt1,__FILE__,__LINE__);
		/*************************
		*   ��������201 
		*************************/
		set_zd_data("1021",pro_note.pro_ac_no);
		set_zd_int ("1022",1);
		set_zd_data("102J","01");
		set_zd_data("102K","2");					/*ת��*/
		set_zd_data("102O","0");					/*�Ƿ��ӡ����(�������۱�־):1��0��*/
		set_zd_data("102Y","�� �������ʻ�");
		set_zd_double("102F",g_pub_tx.tx_amt1);
		if ( pub_base_my_func("D003","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�� �������ʻ�D003��![%s]",g_pub_tx.reply);
			WRITEMSG
			sql_rollback();	/*--����ع�--*/
			continue;
		}
		/**************************
		*   ������ҵ������������51106
		***************************/
		set_zd_data("1211","51106");
		set_zd_data("121A","������ҵ������������" );
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
		if ( pub_base_my_func("A017","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�� ����Ӧ��Ӧ��A017��![%s]",g_pub_tx.reply);
			WRITEMSG
			sql_rollback();	/*--����ع�--*/
			continue;
		}
		/** ������һ��Ϣ�� **/
		ret = pub_base_deadlineM( g_pub_tx.tx_date, 3 ,&pro_note.nex_date );
		if ( ret )
		{
			sprintf(acErrMsg,"net_date error [%d] [%d]",pro_note.nex_date,ret );
			WRITEMSG
			sql_rollback();	/*--����ع�--*/
			continue;
		}
		ret = Protect_note_Upd_Upd( pro_note , g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"note upd error [%d] [%d]",pro_note.pro_ac_no,ret );
			WRITEMSG
			sql_rollback();	/*--����ع�--*/
			continue;
		}
		ret = sql_commit();	/*--�ύ����--*/
		if( ret ) 
		{
			sprintf( acErrMsg, "�ύ����ʧ��!!!" );
			WRITEMSG
			goto ErrExit;
		}

	}
	Protect_note_Clo_Upd();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��֤�𰴼���ȡ�����Ѵ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��֤�𰴼���ȡ������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
