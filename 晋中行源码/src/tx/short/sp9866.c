/*************************************************
* 文 件 名:  sp9866.c
* 功能描述： 根据内部账号查询户名(人行部分为现金收付所用)
*
* 作    者:  jane
* 完成日期： 2003年3月9日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

int sp9866()
{
	struct	in_mst_c	sIn_mst_c;		/* 内部台账主文件 */
    	struct 	mdm_ac_rel_c 	sMdm_ac_rel_c;		/* 介质与账户关系表 */
    	struct 	com_item_c 	sCom_item_c;		/* 公共科目表 */
    	int ret;

    	memset(&sIn_mst_c,0x00,sizeof(struct in_mst_c));
    	memset(&sMdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));
    	memset(&sCom_item_c,0x00,sizeof(struct com_item_c));

    	get_zd_data("0300",sMdm_ac_rel_c.ac_no);
sprintf(acErrMsg,"__xxxxxx111__ac_no=[%s]",sMdm_ac_rel_c.ac_no);
WRITEMSG    	
    	pub_base_old_acno( sMdm_ac_rel_c.ac_no );   		/* 对旧帐号的处理 */
sprintf(acErrMsg,"__XXxxxx222__ac_no=[%s]",sMdm_ac_rel_c.ac_no);
WRITEMSG
/*	110科目现在改为有账户科目，所以此段程序于2003.7.31被封 jane
	if( strcmp( sMdm_ac_rel_c.ac_no, "110" ) == 0 )		* 人行用科目号110记账 *
	{
		ret = Com_item_Sel( g_pub_tx.reply, &sCom_item_c, "acc_no = '%s'","110" );
    		if( ret == 100 )
		{
			sprintf( acErrMsg, "公共科目表中不存在此科目号!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "O074" );
               		goto ErrExit;
       		}else if( ret )
       		{
       			sprintf( acErrMsg, "执行Com_item_Sel错![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
       		}

		set_zd_data( "0270", sCom_item_c.acc_name ); 	*户名*
	}*/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s'",sMdm_ac_rel_c.ac_no);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "介质与账户关系表中不存在此帐号!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B117" );
        	goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}

	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_c,"ac_id =%ld and ac_seqn = 0",\
  			sMdm_ac_rel_c.ac_id);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "内部台账主文件中不存在此帐号!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "A029" );
        	goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行In_mst_Sel错![%d]", ret );
        	WRITEMSG
       		goto ErrExit;
	}

	set_zd_data( "0270", sIn_mst_c.name); /*户名*/
	set_zd_data("0300",sMdm_ac_rel_c.ac_no);		/* 处理旧帐号 */
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
