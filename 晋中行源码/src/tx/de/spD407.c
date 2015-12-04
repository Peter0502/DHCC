/***************************************************************
* 文 件 名: spD407.c
* 功能描述：补登折交易
* 作    者: Azz
* 完成日期：2005年1月13日
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

spD407()
{
	int ret=0;
	char ac_type[2];
	int unprt_cnt;
	struct mdm_attr_c	g_mdm_attr;

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0310",g_pub_tx.ac_no);			/*帐号*/
	g_pub_tx.ac_seqn=DD_BOOK_AC_SEQN;			/*帐号序号*/

	pub_base_old_acno( g_pub_tx.ac_no );        /**对旧帐号的处理**/

  	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						   g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质属性表中不存在该记录[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy (g_pub_tx.reply,"N055");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*------add by xxx 20050801---for stat-begin------*/
	if( g_mdm_ac_rel.note_sts[0] == '*' )
	{
		sprintf( acErrMsg, "介质已销户!" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0] == '1' )
	{
		sprintf( acErrMsg, "此凭证已挂失!" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"N081");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0] != '0' )
	{
		sprintf( acErrMsg, "此凭证状态不正常" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"M013");
		goto ErrExit;
	}
	/*------add by xxx 20050801---for stat-end------*/

	if( strncmp(g_mdm_ac_rel.mdm_code,"0010",4) && \
		strncmp(g_mdm_ac_rel.mdm_code,"0011",4) )
	{
		sprintf(acErrMsg,"请输入存折账号!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"O189");
		goto ErrExit;
	}

  	ret = Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					   g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质属性表中不存在该记录[%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy (g_pub_tx.reply,"N055");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;

	if( g_mdm_attr.prt_ind[0]=='Y' )
	{
		ret = pub_base_patch_book(g_mdm_attr);
		if( ret<0 )
		{
			sprintf(acErrMsg,"调用补登折函数失败!");
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==0 )
		{
			sprintf(acErrMsg,"该户没有未登明细!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O190");
			goto ErrExit;
		}

		set_zd_data("0110","1");
	}
	else if( g_mdm_attr.prt_ind[0]=='N' )
	{
		sprintf(acErrMsg,"该介质为非打印类介质!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"N053");
		goto ErrExit;
	}
	else 
	{
		sprintf(acErrMsg,"该介质打印参数不正确!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"N054");
		goto ErrExit;
	}

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log( ) )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}
	unprt_cnt=sql_count("mdm_unprt_hst","ac_id=%ld " ,g_pub_tx.ac_id);
	if (unprt_cnt>0)
		set_zd_data("0200","1");
    else
		set_zd_data("0200","0");
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"补登折成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"补登折失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
