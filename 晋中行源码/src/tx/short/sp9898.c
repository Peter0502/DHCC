/*************************************************
* 文 件 名:  sp9898.c
* 功能描述： 客户基本信息维护
*
* 作    者:  rob
* 完成日期： 2003年2月3日
*
* 修改记录： 
* 日   期:   2003-05-31	
* 修 改 人:  lance
* 修改内容:  添加联系方式和联系地址
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

int sp9898()
{
	int ret = 0;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_email_inf_c cif_email_inf;	
	struct cif_addr_inf_c cif_addr_inf;	
		
	/*清空*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&cif_addr_inf,0x00,sizeof(struct cif_addr_inf_c));
		
	/*初始化*/
	pub_base_sysinit();

	/*取值*/
	get_zd_data("0670",sts);			/*操作类型*/
	get_zd_long("0280",&cif_no);		/*客户号*/

	/*流程*/
	/*查询客户基本信息表*/
	if( cif_no )
	{
		ret = Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf , \
								 "cif_no=%ld",cif_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"不存在该客户号!cif_no=[%ld]",cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C007");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查询客户信息表异常!!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"客户号未输入!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C075");
		goto ErrExit;
	}
	
	/*查询客户联系信息表*/
	ret = sql_count( "cif_email_inf", "cif_no=%ld and addr_seqn=1", cif_no );
	if( ret<0 )
	{
		sprintf(acErrMsg,"TRANSFER SQL_COUNT ERROR!");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret>0 )
	{
		ret = Cif_email_inf_Sel( g_pub_tx.reply , &cif_email_inf ,
					 			"cif_no=%ld and addr_seqn=1",cif_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"该客户尚未录入联系电话!",cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C106");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查询客户联系方式信息表异常!!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else strcpy(cif_email_inf.email,"无记录");

	/*查询客户地址信息表*/
	ret = sql_count( "cif_addr_inf", "cif_no=%ld and addr_seqn=1", cif_no );
	if( ret<0 )
	{
		sprintf(acErrMsg,"TRANSFER SQL_COUNT ERROR!");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret>0 )
	{
		ret = Cif_addr_inf_Sel( g_pub_tx.reply , &cif_addr_inf ,
					 			"cif_no=%ld and addr_seqn=1",cif_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"该客户尚未录入联系地址!",cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C107");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查询客户地址方式信息表异常!!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else strcpy( cif_addr_inf.addr , "暂无" );

	set_zd_data( "0670",g_cif_basic_inf.type);            	/*客户类型*/
	set_zd_data( "0260",g_cif_basic_inf.name);            	/*客户名称*/
	set_zd_data( "0680",g_cif_basic_inf.local);      	  	/*本地标志*/
	set_zd_data( "0690",g_cif_basic_inf.poten);         	/*潜在标志*/
	set_zd_data( "0700",g_cif_basic_inf.shareholder_ind);	/*本行股东*/
	set_zd_data( "0710",g_cif_basic_inf.lvl);				/*客户级别*/
	set_zd_data( "0930",g_cif_basic_inf.sts);				/*客户当前状态*/
	set_zd_data( "0810",cif_email_inf.email);				/*客户联系电话*/
	set_zd_data( "0820",cif_addr_inf.addr);					/*客户联系地址*/	
	set_zd_data( "0830",cif_email_inf.addr_type);				/*客户联系方式*/

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
