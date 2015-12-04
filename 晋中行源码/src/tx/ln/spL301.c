/*************************************************
* 文 件 名: spL301.c
* 功能描述：   
*              贷款展期
*
* 作    者:    lance
* 完成日期：   2003年01月17日
* 修改记录：   
*     1. 日    期: 20030819
*        修 改 人: jane
*        修改内容: 增加修改删除功能
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "ln_expand_c.h"
#include "ln_auth_c.h"
	
	static long exp_end_date;
	static double exp_rate;
	static	char	exp_ind_v[2];
	static struct ln_expand_c	ln_expand;
	long	old_mtr_date=0;
	char 	old_rate[11];
	char 	old_exp_ind[5];
	double	acm_rate=0.00;
		
		
spL301()  
{ 		
	int ret=0;
	int flag=0;
	double ln_rate=0.00;
	char ln_flag[2];

	char oprt_type[2];
	get_zd_data( "0700", oprt_type );    /*0-录入 1-修改 2-删除*/
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
			
	/** 处理流程 **/
	
	/* 检查贷款状态 */
	if ( strcmp( g_ln_mst.ac_sts,"1") )
	{
		sprintf( acErrMsg,"该贷款账户状态非正常");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L018" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"检查贷款状态 PASS!");
	WRITEMSG
		
	/* 检查贷款余额是否为零 */
	if( pub_base_CompDblVal( g_ln_mst.bal , 0.00 )==0 )	
	{
		sprintf( acErrMsg,"贷款余额为零,不能做此交易!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L216" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"检查贷款余额 PASS!");
	WRITEMSG	
						
	/* 检查贷款展期次数 */
	if ( g_ln_parm.exp_ind[0]=='N' )
	{
		sprintf( acErrMsg,"此种贷款产品不允许展期[%s]",g_ln_mst.prdt_no);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L028" );
		ERR_DEAL		
	}else if ( atoi(g_ln_mst.exp_ind) >= atoi(g_ln_parm.exp_ind) && oprt_type[0] == '0' )
		{
			sprintf( acErrMsg,"该贷款户已经超过其展期最大次数");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L029" );
			ERR_DEAL
		}					
	sprintf(acErrMsg,"检查贷款展期次数 PASS!");
	WRITEMSG	
	if( ( oprt_type[0] == '1' || oprt_type[0] == '2' ) && g_ln_mst.exp_ind[0] == '0' )
	{
		strcpy( g_pub_tx.reply, "L225" );
       	sprintf( acErrMsg, "修改展期信息:此贷款尚未展期!");
       	WRITEMSG
       	ERR_DEAL
	}
	
	if( oprt_type[0] == '0' )
	{
		long date=0;
		date=pub_base_daynumSUB(g_ln_mst.mtr_date,10);
		sprintf(acErrMsg,"now[%d],min[%d],max[%d]",g_pub_tx.tx_date,g_ln_mst.mtr_date,date);
    WRITEMSG
    vtcp_log("展期:mtr_date=[%ld],tx_date=[%ld] date=[%ld] %s,%d",g_ln_mst.mtr_date,g_pub_tx.tx_date,date,__FILE__,__LINE__);
		if (g_pub_tx.tx_date>g_ln_mst.mtr_date||g_pub_tx.tx_date<date)
         {
            strcpy( g_pub_tx.reply, "L332" );
            sprintf(acErrMsg,"当前日期没有在展期期限内,不能录入now[%d],min[%d],max[%d]",g_pub_tx.tx_date,g_ln_mst.mtr_date,date);
            WRITEMSG
            ERR_DEAL
        }
		exp_ind_v[0] = exp_ind_v[0] + 1;
		/* 登记贷款展期登记簿 */
		ln_expand.ac_id = g_ln_mst.ac_id;	/* 账户ID */
		ln_expand.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		strcpy( ln_expand.exp_br_no , g_pub_tx.tx_br_no );/* 展期机构号 */
		ln_expand.exp_amt = g_ln_mst.bal;	/* 展期金额 */
		ln_expand.exp_rate = exp_rate;		/* 展期利率 */
		
		if ( g_ln_mst.exp_ind[0] ='0' )
		{
			ln_expand.exp_beg_date = g_ln_mst.mtr_date;/* 展期起始日 */
		}else{
			ln_expand.exp_beg_date = g_ln_mst.exp_mtr_date;/* 展期起始日 */
	     	}
		
		ln_expand.exp_mtr_date = exp_end_date;	/* 展期到期日 */
		
		strcpy( ln_expand.exp_ind , exp_ind_v );/* 展期标志(0未展期1-9已表示展期次数) */
		
		ln_expand.rgst_date = g_pub_tx.tx_date;	/* 登记日期 */
		strcpy( ln_expand.tel , g_pub_tx.tel );	/* 操作员 */
		ln_expand.trace_no=g_pub_tx.trace_no; /* 流水号 */
		
		ret = Ln_expand_Ins(ln_expand , g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记贷款展期登记簿失败![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L030");
			goto ErrExit;
		}			
		sprintf(acErrMsg,"登记贷款展期登记簿 PASS!");
		WRITEMSG
		/* 入分段积数 */	
		/* 取入分段积数表的利率 */
		ret = pub_ln_GetRate ( g_ln_mst , g_ln_parm , &acm_rate );
		if( ret )	
		{
			sprintf(acErrMsg,"取贷款利率失败!");
			WRITEMSG
			ERR_DEAL
		}
		ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, acm_rate );
		if( ret )	
		{
			sprintf(acErrMsg,"登记分段积数表失败!");
			WRITEMSG
			ERR_DEAL
		}		
		
		strcpy( g_pub_tx.brf,"贷款展期" );	
		/****
		g_pub_tx.svc_ind = 3065;			* 贷款展期(积数不改变) *
		****/
		g_pub_tx.svc_ind = 3063;
		ret = sql_execute( "UPDATE ln_auth SET sts='1' WHERE pact_no ='%s' and sts='X' and type='02'",g_ln_mst.pact_no );/* 20110811 更新sts状态为1 展期*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "更改ln_auth表出错!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
		}
	}/*上面是录入的处理*/
	else if( oprt_type[0] == '1' )
	{
	  if (g_pub_tx.tx_date>g_ln_mst.exp_mtr_date)
	     {
			strcpy( g_pub_tx.reply, "L331" );
            sprintf(acErrMsg,"当前日期大于展期日期,不能修改");
            WRITEMSG
            ERR_DEAL
		}
		ret = Ln_expand_Dec_Upd( g_pub_tx.reply, "ac_id=%d and ac_seqn=%d and exp_ind='%s'", \
			g_ln_mst.ac_id, g_ln_mst.ac_seqn, g_ln_mst.exp_ind );
		if( ret )
		{
        	sprintf( acErrMsg, "执行Ln_expand_Dec_Upd错!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
		}
	
		memset( &ln_expand, 0x00, sizeof( struct ln_expand_c ) );
		ret = Ln_expand_Fet_Upd( &ln_expand, g_pub_tx.reply );
    	if ( ret == 100 )
    	{
        	sprintf( acErrMsg, "贷款展期登记簿中无此记录" );
        	WRITEMSG
        	ERR_DEAL
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Ln_expand_Fet_Upd出错!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
    	}
	
		ln_expand.exp_rate = exp_rate;		/* 展期利率 */
		ln_expand.exp_mtr_date = exp_end_date;	/* 展期到期日 */
		ln_expand.rgst_date = g_pub_tx.tx_date;	/* 登记日期 */
		strcpy( ln_expand.tel , g_pub_tx.tel );	/* 操作员 */
		ln_expand.trace_no=g_pub_tx.trace_no; /* 流水号 */
			
		ret = Ln_expand_Upd_Upd( ln_expand, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg,"执行Ln_expand_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	
		Ln_expand_Clo_Upd();
	
		strcpy( g_pub_tx.brf,"贷款展期修改" );	
		g_pub_tx.svc_ind = 3065;			/* 贷款展期(积数不改变) */
		
		/*判断当前贷款户状态*/
		ret = pub_ln_check_acsts( ln_flag );
		if(ret)		ERR_DEAL
		/*若当前贷款户状态为"展期",登记分段积数*/
		if(ln_flag[0]=='Z')	
		{
			/*取当前贷款户利率值*/
			ret=pub_ln_GetRate ( g_ln_mst, g_ln_parm, &ln_rate );
			if(ret)		ERR_DEAL
			/*登记分段积数*/
			ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, ln_rate );
			if(ret)		ERR_DEAL
			g_pub_tx.svc_ind = 3063;			/* 贷款展期(积数清零) */						
		}
	}/*上面是修改*/
	else if( oprt_type[0] == '2' )
	{
		sprintf(acErrMsg,"贷款到期日期[%ld]",g_ln_mst.exp_mtr_date);
		sprintf(acErrMsg,"系统当前日期dddd[%ld]",g_pub_tx.tx_date);
		WRITEMSG
		if (g_pub_tx.tx_date>=g_ln_mst.exp_mtr_date)
         {
			strcpy( g_pub_tx.reply, "L330" );
			sprintf(acErrMsg,"当前日期大于展期日期，不能删除");
			WRITEMSG
			ERR_DEAL
		}
		ret = sql_execute( "delete from ln_expand where ac_id=%d and ac_seqn=%d and exp_ind='%s'", \
			g_ln_mst.ac_id, g_ln_mst.ac_seqn, g_ln_mst.exp_ind );
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "删除展期登记簿数据库操作出错!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
		}
		strcpy( g_pub_tx.brf,"贷款展期删除" );	
		g_pub_tx.svc_ind = 3065;			/* 贷款展期(积数不改变) */	
		
		/*判断当前贷款户状态*/
		ret = pub_ln_check_acsts( ln_flag );
		if(ret)		ERR_DEAL
		/*若当前贷款户状态为"展期",登记分段积数*/
		if(ln_flag[0]=='Z')	
		{
			/*取当前贷款户利率值*/
			ret=pub_ln_GetRate ( g_ln_mst, g_ln_parm, &ln_rate );
			if(ret)		ERR_DEAL
			/*登记分段积数*/
			ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, ln_rate );
			if(ret)		ERR_DEAL
			g_pub_tx.svc_ind = 3063;			/* 贷款展期(积数清零) */						
		}

		exp_ind_v[0] = exp_ind_v[0] - 1;
		if( exp_ind_v[0] == '0' )
		{
			exp_rate = 0;
			exp_end_date = 0;
		}
		else
		{
			memset( &ln_expand, 0x00, sizeof( struct ln_expand_c ) );
			ret = Ln_expand_Sel( g_pub_tx.reply, &ln_expand, "ac_id=%d and ac_seqn=%d and exp_ind='%s'", \
				g_ln_mst.ac_id, g_ln_mst.ac_seqn, exp_ind_v );
			if( ret && ret != 100 )
			{
        		sprintf( acErrMsg, "查询展期登记簿数据库操作出错!ret=[%d]",ret );
        		WRITEMSG
        		ERR_DEAL
			}
			else if( ret == 100 )
			{
        		sprintf( acErrMsg, "删除非第一次展期信息时查询上次展期记录不存在!ret=[%d]",ret );
        		WRITEMSG
        		ERR_DEAL
			}
			exp_rate = ln_expand.exp_rate;
			exp_end_date = ln_expand.exp_mtr_date;
		}
	}/*上面是删除*/

	/* 更新贷款主文件 */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "贷款主文件中无此记录" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }

	g_ln_mst.exp_rate = exp_rate ;			/* 展期利率 */
	sprintf(acErrMsg,"展期利率[%lf]",g_ln_mst.exp_rate);
	WRITEMSG
	
	g_ln_mst.exp_mtr_date = exp_end_date;		/* 贷款到期日期 */
	sprintf(acErrMsg,"贷款到期日期[%ld]",g_ln_mst.exp_mtr_date);
	WRITEMSG
	
	strcpy( g_ln_mst.exp_ind , exp_ind_v );	/* 展期标志 */
	sprintf(acErrMsg,"展期标志[%s]", g_ln_mst.exp_ind );
	WRITEMSG
	
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG
	/*仅在录入和修改时检查*/	
	if( oprt_type[0] == '0' || oprt_type[0] == '1' )
	{
		ret = pub_ln_check( "2" );
		if( ret )
		{
			sprintf( acErrMsg,"贷款展期检查出错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}	
	}	
	
	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );

	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );	
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
	
	/* 行内传票--贷款展期传票 */
	if( oprt_type[0] == '0' || oprt_type[0] == '1' )
	{
		ret = print_LNBKZQ( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKZQ,失败请检查!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}	
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款展期成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款展期失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset ( &ln_expand , 0x00 , sizeof(struct ln_expand_c) );
	return 0;	
}

int data_set_struct()
{    
	int ret;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款帐号 */
	sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
	WRITEMSG
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* 贷款账户序号 */
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_long("0450",&exp_end_date);		/* 贷款到期日期 */	
	get_zd_double("0840",&exp_rate);		/* 展期利率 */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
				&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}
	
	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		old_mtr_date = g_ln_mst.exp_mtr_date;
		strcpy( old_exp_ind , "(展)" );
		sprintf( old_rate , "%8.5lf‰" , g_ln_mst.exp_rate );		
	}else{
		old_mtr_date = g_ln_mst.mtr_date;
		strcpy( old_exp_ind , "" );
		sprintf( old_rate , "%8.5lf‰" , g_ln_mst.rate );
	}	
	
	strcpy( exp_ind_v, g_ln_mst.exp_ind );
					
	return 0;
}
/* 打印--贷款展期传票 */
int print_LNBKZQ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
	long exp_start_day=0;
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKZQ打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "贷款本金" );	
	
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	
	/* 欠本总额 */	
	tol_lo = g_ln_mst.ttl_prvd_amt ;
	
	/* 累计还本 */	
	tol_pay = g_ln_mst.ttl_prvd_amt - g_ln_mst.bal;
			
	/* 尚欠本金 */		
	tol_need_pay = g_ln_mst.bal;

	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(展)" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	}
		
	sprintf( rate , "%8.5lf‰" , exp_rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);
	/* 本次展期起始日期  */
	pub_base_deadlineD( old_mtr_date , 1 , 	&exp_start_day );
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.欠本总额、 10.累计还本     */
	/* 11.尚欠本金、 12.正常利率、 13.逾期利率、14.科目号、   15.展期标志  */
	/* 16.流水号、   17.展期利率、 18.展期起始日、19.展期中止日  */
	fprintf(fp,"LNBKZQ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%ld|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	0.00 , g_ln_mst.opn_date , old_mtr_date , tol_lo , tol_pay ,
	tol_need_pay , old_rate , over_rate , ln_acc_hrt, old_exp_ind , 
	g_pub_tx.trace_no , rate ,  exp_start_day ,end_date);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
