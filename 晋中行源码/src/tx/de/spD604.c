/***************************************************************
* 文 件 名: spD604.c
* 功能描述：证件信息维护
* 作    者: mike
* 完成日期：2003年3月9日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spD604()
{   
	int ret; 

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* 账号 */
	get_zd_data("0680",g_pub_tx.draw_id_yn);      	/* 证件支取 */
	get_zd_data("0670",g_pub_tx.id_type); 	     	/* 证件类型 */
	get_zd_data("0620",g_pub_tx.id_no); 	     	/* 证件号码 */

	if( g_pub_tx.ac_no[0]=='5' )
	{
		sprintf(acErrMsg,"请输入储蓄账户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O164");
		goto ErrExit;
	}

	if( g_pub_tx.draw_id_yn[0]=='Y' && !strlen(g_pub_tx.id_no) )
  	{
	   sprintf(acErrMsg,"证件号码不能为空!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"O220");
	   goto ErrExit;
	}

	/*根据帐户查找*/
	ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
  	if( ret )
  	{
	   sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
  	if( ret==100 )
  	{
	   sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"M003");
	   goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_strpack(g_mdm_ac_rel.id_no);
	pub_base_strpack(g_pub_tx.id_no);

	if( g_mdm_ac_rel.draw_id_yn[0]=='N' && g_pub_tx.draw_id_yn[0]=='N' )
  	{
	   sprintf(acErrMsg,"未做任何修改!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"O224");
	   goto ErrExit;
	}

	if( g_mdm_ac_rel.draw_id_yn[0]=='Y' && g_pub_tx.draw_id_yn[0]=='Y' )
	{
		if( (g_mdm_ac_rel.id_type[0]==g_pub_tx.id_type[0]) && \
			!strcmp(g_mdm_ac_rel.id_no,g_pub_tx.id_no) )
  		{
	   		sprintf(acErrMsg,"未做任何修改!");
	  		WRITEMSG
	   		strcpy(g_pub_tx.reply,"O224");
	  		goto ErrExit;
		}
	}

	if(g_pub_tx.draw_id_yn[0]=='N')
		strcpy(g_mdm_ac_rel.draw_id_yn,g_pub_tx.draw_id_yn);
	else
	{
		strcpy(g_mdm_ac_rel.draw_id_yn,g_pub_tx.draw_id_yn);
		strcpy(g_mdm_ac_rel.id_type,g_pub_tx.id_type);
		strcpy(g_mdm_ac_rel.id_no,g_pub_tx.id_no);
	}

	ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
  	if( ret )
  	{
	   sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"证件信息维护成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"证件信息维护失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
