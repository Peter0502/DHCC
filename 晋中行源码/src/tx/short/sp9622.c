/*************************************************
* �� �� ��:  sp9622.c
* ���������� ��ѯ���̿�Ǽǲ�
*
* ��    ��:  robbit
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:   20080416
* �� �� ��:  
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_rmny_smny_c.h"

int sp9622()
{
       int ret=0;
       int cdk_seqn =0;
	int cdk_date;
	struct mo_rmny_smny_c sMo;
	memset(&sMo,0,sizeof(sMo));
	pub_base_sysinit();
       get_zd_int( "0280",&cdk_seqn );
       get_zd_int("0440",&cdk_date);
       vtcp_log("%s,%d,���̿����==[%d],�Ǽ�����[%d]",cdk_seqn,cdk_date);
       
       ret = Mo_rmny_smny_Sel( g_pub_tx.reply,&sMo," seqn=%d and occ_date=%d ",cdk_seqn,cdk_date );
	if( ret )
	{
		sprintf( acErrMsg,"���̿�Ǽǲ������ڸü�¼[%d]",cdk_seqn );
		WRITEMSG
		strcpy( g_pub_tx.reply,"CD30" );
		goto ErrExit;
	}
	set_zd_double("0420",sMo.amt);	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
