/*************************************************
* �� �� ��:    sp9675.c
* ����������   �����깤����״̬
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
	
sp9675()  
{ 		
	int ret=0;
	struct com_branch_c com_branch;
	
	/** ���ݳ�ʼ�� **/
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
		ret=Com_branch_Sel( g_pub_tx.reply,&com_branch,"br_no='%s'",
			g_pub_tx.tx_br_no );
		if( ret )
		{
			sprintf(acErrMsg,"br_no='%s'",g_pub_tx.tx_br_no );
			WRITEMSG
			strcpy( g_pub_tx.reply,"H063" );
	    	goto OkExit;
		}
		com_branch.wrk_sts[1]='\0';
		set_zd_data( "0680", com_branch.wrk_sts );		/* �����˻����� */	
	
			
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
   
