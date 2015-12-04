/*************************************************
* 文 件 名:    sp9935.c
* 功能描述：   
*              根据账号查询户名
*
* 作    者:    jane
* 完成日期：   2003年6月23日
* 修改记录：   
*     日    期:
*     修 改 人:
*     修改内容:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9935()
{
	int ret=0,ac_seqn;
	char ac_no[20],name[51];

	/** 取值、赋值 **/	
	get_zd_data("0310",ac_no);		/*帐号*/

	pub_base_old_acno( ac_no );   		/* 对旧帐号的处理 */

	/* 根据帐号查询账户序号和账户ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"执行Mdm_ac_rel_Sel错误!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

 	/*根据长治商行会计部郭国萍要求可以向不冻户买支票*/
	if (ac_no[0]=='9')
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",\
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if(ret==100)
		{
			sprintf(acErrMsg,"该账户非不动户!ac_id=[%ld] and ac_seqn=[%d]",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M018");
			goto ErrExit;
		}
		else if(ret)
			{
             sprintf(acErrMsg,"执行Id_mst_Sel错误!ret=[%d]",ret);
			 WRITEMSG
			 goto ErrExit;
			}
		set_zd_data("0250",g_in_mst.name);
	    set_zd_data("0310",ac_no);

	}
	else
{
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该账户非活期户!ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M018");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"执行Dd_mst_Sel错误!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0250",g_dd_mst.name);
    set_zd_data("0310",ac_no);
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
