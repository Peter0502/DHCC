/*************************************************************
* 文 件 名:  sp9673.c
* 功能描述： 授权之交易显示名称
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "com_item_c.h"
int sp9673()
{
	int i=0,ret=0;
	struct com_item_c g_com_item;
	memset(&g_com_item,0x0,sizeof(struct com_item_c));
	get_zd_data( "0300", g_pub_tx.ac_no );
	pub_base_old_acno( g_pub_tx.ac_no );
vtcp_log("AC_NO[%s]",g_pub_tx.ac_no );
	if( !strlen(g_pub_tx.ac_no) ) goto OkExit;
	i=strlen(g_pub_tx.ac_no);
	if(i==3 || i==5 || i==7)							/*科目与内部帐号*/
	{
    vtcp_log("对科目的处理");
	 	/*根据科目号查询科目名称*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 g_pub_tx.ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]", g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "查询公共科目表异常!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		if( g_com_item.acc_knd[0]!='3' )
		{
		 	sprintf(acErrMsg,"该科目下有账户或有子科目!!acc_knd=[%s]",g_com_item.acc_knd);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P406");
		 	goto ErrExit;
		}
			set_zd_data( "0250", g_com_item.acc_name );
			set_zd_data( "0300", g_pub_tx.ac_no );
	}
	else
	{	
			if( pub_base_acgetname(g_pub_tx.ac_no, &g_pub_tx.cif_no, g_pub_tx.name) )
			{
				strcpy( g_pub_tx.reply,"M003" );
				goto ErrExit;
			}
			pub_base_strpack( g_pub_tx.name );
			set_zd_data( "0250", g_pub_tx.name );
			set_zd_data( "0300", g_pub_tx.ac_no );
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
