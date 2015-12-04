/*************************************************
* 文 件 名:  sp9779.c
* 功能描述： 由账号和序号回显户名(0250)
*
* 作    者:  lance
* 完成日期： 2003年06月02日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9779()
{

	int	ret=0;

	get_zd_data( "0370", g_pub_tx.ac_no ); 			/* 账号 */
	get_zd_int( "0340", &g_pub_tx.ac_seqn );		/* 账号序号 */
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , g_pub_tx.ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"该账号不存在,请检查! erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"C115" );
		goto	ErrExit;
	}else if( ret !=0 ){
    		sprintf( acErrMsg,"读取介质账号对照表失败,请与中心机房联系!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"C114" );
    		goto	ErrExit;
    	}
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG
	
	/* 查询客户名称*/
	ret = pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"调用由显示帐号和账号序号返回账户类型和主文件出错!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto	ErrExit;
	}

	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':
				strcpy(g_pub_tx.name,g_dd_mst.name);
				break;
		case '2':
				strcpy(g_pub_tx.name,g_td_mst.name);
				break;
		case '3':
				strcpy(g_pub_tx.name,g_ln_mst.name);
				break;		
		case '9':
				strcpy(g_pub_tx.name,g_in_mst.name);
				break;							
		default	:
				sprintf(acErrMsg,"账号对应产品类型错误,请与中心机房联系!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "C120" );
				goto	ErrExit;
	}	
	
	set_zd_data( "0250" , g_pub_tx.name );
 			

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

