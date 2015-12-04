/*************************************************
* 文 件 名:  sp9867.c
* 功能描述： 根据内部账号查询户名
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

int sp9867()
{
	struct	in_mst_c	sIn_mst_c;		/* 内部台账主文件 */
    	struct 	mdm_ac_rel_c 	sMdm_ac_rel_c;		/* 介质与账户关系表 */
    	int ret;

    	memset(&sIn_mst_c,0x00,sizeof(struct in_mst_c));
    	memset(&sMdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));

    	get_zd_data("0330",sMdm_ac_rel_c.ac_no);

    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s' \
          			and ac_seqn= 0 ",sMdm_ac_rel_c.ac_no);
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

	set_zd_data( "0250", sIn_mst_c.name); /*户名*/

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
