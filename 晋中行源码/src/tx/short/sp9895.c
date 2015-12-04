/*************************************************
* 文 件 名:  sp9895.c
* 功能描述： 客户别名信息维护快捷查询
*
* 作    者:  rob
* 完成日期： 2003年2月3日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"

int sp9895()
{
	int ret = 0;
	int alias_seqn;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_alias_inf_c g_cif_alias_inf;			

	/*清空*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));

	/*初始化*/
	pub_base_sysinit();

	/*取值*/
	get_zd_data("0670",sts);				/*操作类型*/
	get_zd_long("0280",&cif_no);			/*客户号*/
	get_zd_int("0360",&alias_seqn);			/*别名序号*/

	/*流程*/
	/*查询客户基本信息表*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户基本信息表中不存在该客户号!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C105");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*查询客户别名信息表*/
		sprintf(acErrMsg,"sts=%s,cif_no=%ld and alias_seqn=%d",sts,cif_no,alias_seqn);
		WRITEMSG
	ret=Cif_alias_inf_Sel(g_pub_tx.reply,&g_cif_alias_inf,
						"cif_no=%ld and alias_seqn=%d",cif_no,alias_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户别名信息表不存在该记录!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C057");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户别名信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

		sprintf( acErrMsg, "异常!!g_cif_alias_inf.alias=[%s]",
							g_cif_alias_inf.alias );
		WRITEMSG
		
	if( sts[0]=='2' && alias_seqn==1 )	/* 删除第一条记录 */
	{
		TRACE
		sprintf( acErrMsg, "此条记录不可删除!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C104");		
		goto ErrExit;
	}		
	if( sts[0]=='1' && alias_seqn==1 )	/* 修改第一条记录 */
	{
		TRACE
		sprintf( acErrMsg, "此条记录不可修改!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C111");		
		goto ErrExit;
	}
		
	set_zd_data( "0270",g_cif_alias_inf.alias);          /*客户原别名*/

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
