/*************************************************
* 文 件 名:  sp9932.c
* 功能描述： 由帐号回显客户信息_通知存款登记
*
* 作    者:  rob 
* 完成日期： 2003年1月15日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9932()
{
	int	ret;
	
	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	get_zd_data( "0310", g_pub_tx.ac_no ); 		/* 帐号 */
	pub_base_old_acno( g_pub_tx.ac_no );        /**对旧帐号的处理**/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						  g_pub_tx.ac_no );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "介质账户对照表中不存在该账户!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查找介质账户对照表异常!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W011" );
		goto ErrExit;
	}

	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
							 g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf( acErrMsg, "帐户不存在!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M003" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查找出错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D103" );
			goto ErrExit;
		}
		
		sprintf( acErrMsg, "非通知存款帐号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D213" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查找错误!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
					  g_td_mst.prdt_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "定期参数表不存在该产品!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查找错误!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	if( strncmp(g_td_mst.prdt_no,"242",3) && \
		strncmp(g_td_mst.prdt_no,"243",3) && \
		strncmp(g_td_mst.prdt_no,"244",3) && \
		strncmp(g_td_mst.prdt_no,"281",3) && \
		strncmp(g_td_mst.prdt_no,"282",3) && \
		strncmp(g_td_mst.prdt_no,"283",3) )
	{
		sprintf( acErrMsg, "请输入通知存款账号!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O213" );
		goto ErrExit;
	}

	set_zd_data( "0810", g_td_parm.title );				/* 产品名称 */
	set_zd_double( "0430", g_td_mst.bal );				/* 帐户余额 */

	set_zd_data( "0310", g_pub_tx.ac_no );			

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
