/*************************************************
* 文 件 名:    sp9712.c
* 功能描述：   
*              根据帐号回显相关信息[转帐借贷记帐交易--借]
*
* 作    者:    rob
* 完成日期：   2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "gl_mst_c.h"

sp9712()  
{ 		
	int i=0,ret=0,ac_seqn;
	char ac_no[20];

	struct  com_item_c   g_com_item;
	struct  gl_mst_c   	 g_gl_mst;

	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));

	/** 取值、赋值 **/	
	get_zd_data("1201",ac_no);						/*帐号*/
	pub_base_old_acno(ac_no);
  vtcp_log("[%s][%d]xyy ac_no====[%s]",__FILE__,__LINE__,ac_no);
	i=strlen(ac_no);
	if(i==3||i==5||i==7)							/*输入科目*/
	{
	 	/*根据科目号查询科目名称*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]", g_in_parm.acc_hrt);
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

	 	/*根据科目号查询余额及借贷方向*/
	 	ret=Gl_mst_Sel(g_pub_tx.reply,&g_gl_mst,"acc_hrt='%s'",
					   g_com_item.acc_hrt);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]",g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"查询公共科目表异常!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		/*科目借贷类型*/
		if(g_com_item.dc_ind[0]=='1')
		{
			set_zd_data("120B","借");			
			set_zd_double("1203",g_gl_mst.dr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("120B","贷");			
			set_zd_double("1203",g_gl_mst.cr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='0')		/*双方*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("120B","借");			
				set_zd_double("1203",g_gl_mst.dr_bal);			
			}
			else if(ret==-1)
			{
				set_zd_data("120B","贷");
				set_zd_double("1203",g_gl_mst.cr_bal);			
			}
		}
		set_zd_data("1202",g_com_item.acc_name);       /*户名*/
		set_zd_data("1204",g_gl_mst.cur_no);           /*币种*/
	}
	else												/*输入内部帐号*/
	{
		/*根据帐号找账户ID和序号 */
	 	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"不存在该帐号!!ac_no=[%ld]",ac_no);
			 		 WRITEMSG
		 	strcpy(g_pub_tx.reply,"M003");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
			sprintf( acErrMsg, "查询介质帐号表异常!!ret=[%d]", ret );
					 WRITEMSG
		 	goto ErrExit;
	 	}

	 	ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	 	if( ret==100 )
	 	{
			sprintf(acErrMsg,"不存在该帐号!!ac_no=[%ld]",ac_no);
				 	 WRITEMSG
		 	strcpy(g_pub_tx.reply,"M003");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "查询内部帐户主文件异常!!ret=[%d]", ret );
					 WRITEMSG
		 	goto ErrExit;
	 	}

		/*根据产品号找到科目号*/
	 	ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
						 g_in_mst.prdt_no);
	 	if( ret==100 )
	 	{
			sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%ld]",g_in_mst.prdt_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"M003");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
			sprintf( acErrMsg, "查询内部帐户参数表异常!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
		}

	 	/*根据科目号查询余额借贷方向*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_hrt='%s'",
						  g_in_parm.acc_hrt);
	 	if( ret==100 )
	 	{
			sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]", 
						g_in_parm.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
			sprintf( acErrMsg, "查询内部帐户参数表异常!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		/*余额方向*/
		if(g_com_item.dc_ind[0]=='1')
			set_zd_data("120B","借");
		else if(g_com_item.dc_ind[0]=='2')
			set_zd_data("120B","贷");
		else if(g_com_item.dc_ind[0]=='0')      /*双方*/
		{
			ret=pub_base_CompDblVal(g_in_mst.bal,0.00);
			if(ret==1)
		   		set_zd_data("120B","借");
		   	else
			  	set_zd_data("120B","贷");
		}

		/* 输出变量 */
		set_zd_data("1202",g_in_mst.name);              /*户名*/
		set_zd_data("1204",g_in_parm.cur_no);           /*币种*/
		set_zd_double("1203",g_in_mst.bal);             /*余额*/
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
   
