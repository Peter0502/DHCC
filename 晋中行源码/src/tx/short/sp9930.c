/*************************************************
* 文 件 名: sp9930.c
* 功能描述：根据帐号，帐号序号回显相关信息[单边记帐交易][万能记账]
*
* 作    者: 
* 完成日期：2003年06月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#include <math.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "cash_mst_c.h"

sp9930()  
{ 		
	char	trance_type[2];
	int i=0,ret=0;

	struct  prdt_ac_id_c   sPrdt_ac_id;
	struct  com_item_c   g_com_item;
	struct  gl_mst_c     g_gl_mst;
	struct  cash_mst_c   sCash_mst;

	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&sCash_mst,0x00,sizeof(struct cash_mst_c));
	memset(trance_type,0x00,sizeof(trance_type));

	/** 取值、赋值 **/	
	get_zd_data("0680",trance_type);						/*记账类型*/
	vtcp_log(" [%s][%d]trance_type is [%s]\n",__FILE__,__LINE__,trance_type);
	if( trance_type[0]=='7' )	/* 柜员钱箱 */
	{
		get_zd_data("0210",g_pub_tx.cur_no);				/*币种*/
		get_zd_data("0070",g_pub_tx.tel);					/*操作员*/
		get_zd_data("0030",g_pub_tx.tx_br_no);				/*机构号*/
		ret=Cash_mst_Sel(g_pub_tx.reply,&sCash_mst," br_no='%s' and tel='%s' and cur_no='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.cur_no);
		if( ret==100 )
		{
		 	sprintf(acErrMsg,"该柜员没有此币种的钱箱[%s][%s][%s]",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.cur_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P414");
		 	goto ErrExit;
		}
		if( ret )
		{
		 	sprintf(acErrMsg,"取柜员钱箱错误[%s][%s][%s]",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.cur_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P153");
		 	goto ErrExit;
		}
		set_zd_data("0220","借");			
		sCash_mst.bal=floor(sCash_mst.bal);
		set_zd_double("0410",sCash_mst.bal);			
		set_zd_data("0250","钱箱");       			/*户名*/
		set_zd_data("0310",sCash_mst.ac_no);
		goto OkExit;
	}
	else
	{
		get_zd_data("0300",g_pub_tx.ac_no);					/*帐号*/
		pub_base_old_acno( g_pub_tx.ac_no );
	}

	i=strlen(g_pub_tx.ac_no); /***根据ac_no的长度来判断是否是科目***/
	if(i==3||i==5||i==7)							/*输入科目*/
	{
		if( trance_type[0]!='6' )
	 	{
		 	sprintf(acErrMsg,"请选择表内记账!!trance_type=[%s]",trance_type);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P405");
		 	goto ErrExit;
	 	}
	 	/*根据科目号查询科目名称*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 g_pub_tx.ac_no);
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
		if( g_com_item.acc_knd[0]!='3' )
		{
		 	sprintf(acErrMsg,"该科目下有账户或有子科目!!acc_knd=[%s]",g_com_item.acc_knd);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P406");
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
			set_zd_data("0220","借");			
			set_zd_double("0410",g_gl_mst.dr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("0220","贷");			
			set_zd_double("0410",g_gl_mst.cr_bal);			
		}
		else if(g_com_item.dc_ind[0]=='0')		/*双方*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("0220","借");			
				set_zd_double("0410",g_gl_mst.dr_bal);			
			}
			else if(ret==-1)
			{
				set_zd_data("0220","贷");
				set_zd_double("0410",g_gl_mst.cr_bal);			
			}
		}
		set_zd_data("0250",g_com_item.acc_name);       /*户名*/
		set_zd_data("0210",g_gl_mst.cur_no);           /*币种*/
	}
	else					/*输入非科目帐号*/
	{
		/*根据帐号找账户ID和序号 */
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%ld]",g_pub_tx.ac_no);
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

		g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;
		
		/****若存款户为贷款的还款户，返回标志以提示柜员 add by wanglei 20070621****/
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id = %ld and ac_sts in('1','2','3','4','5') \
			and (bal >0 or in_lo_intst >0 or out_lo_intst >0 or cmpd_lo_intst >0) ", g_mdm_ac_rel.ac_id);
		if(ret == 0)
		{
			sprintf(acErrMsg,"贷款还款帐号且有本金或利息未还!!");
			WRITEMSG
			set_zd_data("0710","1"); /****返回提示标志****/
		}
		/****ended 20070621****/

		/*** 取产品种类 ***/
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%d and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"取prdt_ac_id表出错!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 		WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"取prdt_ac_id表出错!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
			WRITEMSG
			goto ErrExit;
		}
				
		if( sPrdt_ac_id.ac_id_type[0]=='1' )	/*** 活期 ***/	
		{
			/*	对于旧帐号前台没法判断所以直接从后台赋值 
			if( trance_type[0]!='1' )
			{
				sprintf(acErrMsg,"活期账户，请选择活期交易类型!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P409");
				goto ErrExit;
			}
			*/
			trance_type[0]=sPrdt_ac_id.ac_id_type[0];

			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
					 	g_dd_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_dd_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_dd_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			if( g_dd_parm.cif_type[0]=='1' )
			{
				sprintf(acErrMsg,"请输入对公账户!!cif_type=[%s]",g_dd_parm.cif_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O165");
				goto ErrExit;
			}
			/* 输出变量 */
			set_zd_data("0220","贷");
			set_zd_data("0250",g_dd_mst.name);				/*户名*/
			set_zd_data("0210",g_dd_parm.cur_no);			/*币种*/
			set_zd_double("0410",g_dd_mst.bal);				/*余额*/
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='2' )	/*** 定期 ***/	
		{
			sprintf(acErrMsg,"定期帐号不能作此交易!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;

			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",
					 	g_td_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_td_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_td_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/* 输出变量 */
			set_zd_data("0220","贷");
			set_zd_data("0250",g_td_mst.name);				/*户名*/
			set_zd_data("0210",g_td_parm.cur_no);			/*币种*/
			set_zd_double("0410",g_td_mst.bal);				/*余额*/
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='3' )	/*** 贷款 ***/	
		{
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",
					 	g_ln_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_ln_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_ln_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}
			/* 输出变量 */
			set_zd_data("0220","借");
			set_zd_data("0250",g_ln_mst.name);				/*户名*/
			set_zd_data("0210",g_ln_parm.cur_no);			/*币种*/
			if( trance_type[0]=='2' )
				set_zd_double("0410",g_ln_mst.bal);				/*本金余额*/
			else if( trance_type[0]=='3' )
				set_zd_double("0410",g_ln_mst.in_lo_intst);		/*表内余额*/
			else if( trance_type[0]=='4' )
				set_zd_double("0410",g_ln_mst.out_lo_intst);	/*表外余额*/
			else if( trance_type[0]=='5' )
				set_zd_double("0410",g_ln_mst.cmpd_lo_intst);	/*复利余额*/
			else
			{
				sprintf(acErrMsg,"贷款账户，请选择贷款交易类型!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P407");
				goto ErrExit;
			}
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='4' )	/*** 透支 ***/	
		{
			ret=Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]", g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}

			ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'",
					 	g_od_mst.prdt_no);
			if( ret==100 )
			{
				sprintf(acErrMsg,"不存在产品号!!prdt_no=[%s]",g_od_mst.prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"不存在该产品号!!prdt_no=[%s],ret=[%d]",g_od_mst.prdt_no,ret);
				WRITEMSG
				goto ErrExit;
			}


			/* 输出变量 */
			set_zd_data("0220","借");
			set_zd_data("0210",g_in_parm.cur_no);			/*币种*/
			set_zd_double("0410",g_in_mst.bal);				/*余额*/

			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",
						g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d]",g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn);
	 			WRITEMSG
				strcpy(g_pub_tx.reply,"M003");
				goto ErrExit;
			}
			else if( ret )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld],ac_seqn=[%d],ret=[%d]",g_od_mst.pay_ac_id,g_od_mst.pay_ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			set_zd_data("0250",g_dd_mst.name);				/*户名*/
		}
		else
		{
			if( trance_type[0]!='6' )
			{
				sprintf(acErrMsg,"内部帐帐户，请选择内部帐交易类型!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P408");
				goto ErrExit;
			}

			ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if( ret==100 )
			{
	 			sprintf(acErrMsg,"不存在该帐号!!ac_id=[%ld]", g_mdm_ac_rel.ac_id);
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
				set_zd_data("0220","借");
			else if(g_com_item.dc_ind[0]=='2')
				set_zd_data("0220","贷");
			else if(g_com_item.dc_ind[0]=='0')      /*双方*/
			{
				ret=pub_base_CompDblVal(g_in_mst.bal,0.00);
				if(ret==1)
			   		set_zd_data("0220","借");
				else 
			   		set_zd_data("0220","贷");
	  		}

			/* 输出变量 */
			set_zd_data("0250",g_in_mst.name);				/*户名*/
			set_zd_data("0210",g_in_parm.cur_no);			/*币种*/
			set_zd_double("0410",g_in_mst.bal);				/*余额*/
		}
		set_zd_data("0300",g_pub_tx.ac_no);
	}
	/* 前台不能显示负值所以负值都显示成0返回去 */
	double tmp_bal=0;
	get_zd_double("0410",&tmp_bal);
	if(tmp_bal<0){
		vtcp_log("!!!!!!!小于0[%.2f]\n",tmp_bal);
		set_zd_double("0410",0);	
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
   
