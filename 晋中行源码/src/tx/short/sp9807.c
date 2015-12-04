/*************************************************
* 文 件 名:    sp9807.c
* 功能描述：   
*              根据贷款账号取出账户序号,多个交易用, 得用65域取主交易的tx_code
*
* 作    者:    lance
* 完成日期：   2003年01月14日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "ln_auth_c.h"
	
sp9807()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	char tx_code[5];
	char cif_type[2];	
	char rate_flag[2];
	long beg_end_date;  /* 到期日- 开户日 */
	long beg_end_days;  /* 到期日- 开户日 */
	long max_exp_date;  /* 最大展期日期 */	
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char save_ac_no[20];
		
	FILE *fp;
	
	struct ln_mst_c		ln_mst_tmp;
	struct cif_basic_inf_c 	cif_basic_inf;	
	struct mdm_ac_rel_c	mdm_ac_rel_tmp;
	struct ln_auth_c	ln_auth_tmp;		
	
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	memset( &mdm_ac_rel_tmp,0x00,sizeof(struct mdm_ac_rel_c) );
	memset( &ln_auth_tmp,0x00,sizeof(struct ln_auth_c) );
			
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0370" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	get_zd_data( "0650" , tx_code ); /* 交易代码 */

	set_zd_data("0370",ac_no);	/* 处理旧帐号 */
	pub_base_strpack(tx_code);
		sprintf( acErrMsg,"0650 交易代码=[%s]",tx_code);
		WRITEMSG	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* 根据贷款账户ID查询贷款主文件个数 */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"根据贷款账户ID查询贷款主文件个数[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	 {
 		set_zd_int( "0500", redflag );	/* 是否输入序号标志 */   
 		sprintf(acErrMsg,"贷款主文件表中无此记录[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L114");
 		goto ErrExit;
   	 }	

	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L114" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"取贷款主文件异常 erro code=[%s]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	

    	sprintf(acErrMsg,"取贷款主文件PASS");
	WRITEMSG 
		vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
		set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
		set_zd_double( "0410", g_ln_mst.bal );		/* 余额 */	
		if(memcmp(ln_mst_tmp.opn_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1)){
    			sprintf( acErrMsg,"不是同一个机构不可做此交易");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D124" );
    			ERR_DEAL
		}
	
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	redflag=1;
	set_zd_long( "0340", ln_mst_tmp.ac_seqn );	/* 贷款账户序号 */	
	set_zd_int( "0500", redflag );	/* 是否输入序号标志 1-唯一记录*/    
    	sprintf(acErrMsg,"redflag[%d]ac_seqn[%d]",redflag,ln_mst_tmp.ac_seqn);
	WRITEMSG 

	/* 非卡上开户专用通道 */

	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(ac_no,ln_mst_tmp.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息 这是以下各个分支的通用部分20110729*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}
	TRACE
		vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
	/* 贷款状态为销户时，不能做交易 */
	if ( g_ln_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"此贷款已销户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L035");
		goto ErrExit;
	}
	TRACE	
	/* 现转标志判断 */
	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	TRACE	
	strcpy( dc_flg1 , "1" );
  strcpy( dc_inf1 , "现金" );
	strcpy( dc_flg , "2" );
  strcpy( dc_inf , "转账" );
	set_zd_data("0660",dc_flg);
	set_zd_data("0810",dc_inf);  
		vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);

/**** 按长沙商行要求修改 贷款还款时，无论对私、对公客户均支持现金和转账		
	if( g_ln_parm.cif_type[0]=='2' )
	{		    
		pub_base_strpack(dc_inf);
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
	}else{
		pub_base_strpack(dc_inf1);
		pub_base_strpack(dc_inf);
	
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
	}	
**************/

		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		
	set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);	

	TRACE
	if( strcmp( tx_code , "3102" )==0 )	
	{
			sprintf(acErrMsg,"交易3102!");
			WRITEMSG
		  /* 判断放款计划类型 N无 M手动 */
			if ( g_ln_parm.pay_pln_type[0]!='M' )
			{
				sprintf(acErrMsg,"放款计划类型非手动[%s],不可做此交易!",g_ln_parm.pay_pln_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L075");
				goto ErrExit;
			}	
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* 计划放款额 */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* 累计发放额 */
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
			set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */	
			set_zd_data( "0720" , g_ln_parm.pay_pln_crt ); /* 还款计划联动标志  */
		
	}			
			
	if( strcmp( tx_code , "3103" )==0 )	
	{
			sprintf(acErrMsg,"交易3103!");
			WRITEMSG
			/* 判断放款计划类型 N无 M手动 */
			if ( g_ln_parm.pay_pln_crt[0]!='M' )
			{
				sprintf(acErrMsg,"还款计划类型非手动[%s],不可做此交易!",g_ln_parm.pay_pln_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L080");
				goto ErrExit;
			}	
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* 计划放款额 */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* 累计发放额 */
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
			set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */	
		
	}	
	
	if( strcmp( tx_code , "3104" )==0 )	
	{
			sprintf(acErrMsg,"交易3104!");
			WRITEMSG

			/* 贷款状态为审批时，不能做交易 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"此贷款尚未经过审批!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			
			*****/
			/* 取客户类型 */
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , "cif_no=%ld" , g_ln_mst.cif_no);
			if( ret )
			{
				goto ErrExit;
			}
			if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
			{
				strcpy( cif_type , "0" );	/* 对私 */
			}else{
				strcpy( cif_type , "1" );	/* 对公 */
			}		
			set_zd_data("109L" , cif_type);				
			sprintf(acErrMsg,"客户类型[%s]!",cif_type);
			WRITEMSG
						
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* 计划放款额 */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* 累计发放额 */
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
			set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */	
			set_zd_data( "0210", g_ln_parm.cur_no );		/* 币种 */
			set_zd_data("0670", g_ln_parm.ln_type); /*贷款类型,added by gujy on 060916*/
			set_zd_data("0630",g_ln_mst.pact_no); /**借据号** added by ligl 2006-9-28 12:05*/
			set_zd_data("0370",g_ln_mst.trust_no);/**委托协议 added by ligl 2006-9-28 15:55**/
			TRACE					
			sprintf( acErrMsg,"(!!~~选择贷款类型-[%s]", g_ln_parm.ln_type);
			WRITEMSG	
			
			/* 根据存款账户ID取存款账户 */
			ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth_tmp , "pact_no='%s'" , g_ln_mst.pact_no );
			if(ret)
			{
				sprintf(acErrMsg,"查询贷款审批登记簿异常出错![%s][%d]",g_ln_mst.pact_no,ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L181");
				goto ErrExit;
			}			
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &mdm_ac_rel_tmp , "ac_id=%ld and mdm_code='%s' and note_sts in( '0','1' )" , g_ln_mst.save_ac_id , ln_auth_tmp.save_mdm_code );
			if(ret==100)
			{
				sprintf(acErrMsg,"存款账户状态不正常,请与中心机房联系!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L183");
				goto ErrExit;
			}else if(ret){						
				sprintf(acErrMsg,"查询介质账号对照表异常出错![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L182");
				goto ErrExit;	
			}		
			strcpy( save_ac_no , mdm_ac_rel_tmp.ac_no );
			
			set_zd_data("1061" , save_ac_no);	/* 存款账号 */
						
	}

	TRACE					
	sprintf( acErrMsg,"交易代码=[%s]",tx_code);
	WRITEMSG	
	if( strcmp( tx_code , "3204" )==0 )	
	{
			sprintf(acErrMsg,"交易3204!");
			WRITEMSG
			
			/* 贷款状态为审批时，不能做交易 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"此贷款尚未经过审批!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			************/
			/* 查询贷款是否放款 */ 
			ret = pub_base_CompDblVal( g_ln_mst.bal, 0.00 );
			if( ret==0 )
			{
    				ret = pub_base_CompDblVal( g_ln_mst.ys_bal, 0.00 );
    				if( ret==0 )
    				{
    					sprintf( acErrMsg,"此按揭贷款尚未放款!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L109" );		
					goto ErrExit;
				}
			}		
			sprintf(acErrMsg,"检查贷款是否放款 PASS!");
			WRITEMSG
					
			/* 检查贷款类型是否是按揭贷款 */
			if ( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
			{
				sprintf( acErrMsg,"此贷款非按揭贷款[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L042" );
				ERR_DEAL
			}	
			sprintf(acErrMsg,"检查贷款类型是否是按揭贷款 PASS!");
			WRITEMSG		
			TRACE

			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
			set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */	
			sprintf(acErrMsg,"贷款余额[%lf] PASS!",g_ln_mst.bal);
			WRITEMSG					
			/* 取客户类型 */
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , "cif_no=%ld" , g_ln_mst.cif_no);
			if( ret )
			{
				goto ErrExit;
			}
			if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
			{
				strcpy( cif_type , "0" );	/* 对私 */
			}else{
				strcpy( cif_type , "1" );	/* 对公 */
			}		
			set_zd_data("109L" , cif_type);	
					
	}	

	TRACE			
	if( strcmp( tx_code , "3205" )==0 )	
	{
			sprintf(acErrMsg,"交易3205!");
			WRITEMSG
			
			/* 贷款状态为审批时，不能做交易 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"此贷款尚未经过审批!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			******************/
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */
			if( g_ln_mst.repay_type[0]=='3' || g_ln_mst.repay_type[0]=='4')	/* 按揭贷款 */
			{
				set_zd_data( "0220", "0" );		/* 贷款类型 */
			}else{
				set_zd_data( "0220", "1" );		/* 贷款类型 */
			}
	}	  
/************3301开始 20110729 晋中商行因信贷系统扩展查询内容***********/
	if( strcmp( tx_code , "3301" )==0 )	
	{
			sprintf(acErrMsg,"交易3301!");
			WRITEMSG
			/* 按揭贷款不可展期 */
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"此贷款为按揭贷款,不可展期[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L121" );
				ERR_DEAL
			}	
							  
	    /* 计算贷款最大展期日期 */
    	beg_end_date = g_ln_mst.mtr_date - g_ln_mst.opn_date ;
	sprintf( acErrMsg,"beg_end_date[%ld]",beg_end_date);
	WRITEMSG	
	    beg_end_days = pub_base_CalTrueDays( g_ln_mst.opn_date , g_ln_mst.mtr_date );
	    sprintf( acErrMsg,"opn_date[%ld]mtr_date=[%ld]beg_end_days[%ld]",g_ln_mst.opn_date , g_ln_mst.mtr_date,beg_end_days);
	    WRITEMSG	
	    if ( beg_end_date <= 10000 )
	    {
	      	pub_base_deadlineD( g_ln_mst.mtr_date , beg_end_days , &max_exp_date );
	    }else if ( beg_end_date <= 50000 ){
			    pub_base_deadlineD( g_ln_mst.mtr_date , beg_end_days/2 , &max_exp_date );
		  }else{
				  max_exp_date = g_ln_mst.mtr_date + 30000;
			}
	    sprintf(acErrMsg,"计算贷款最大展期日期[%ld] PASS!",max_exp_date);
	    WRITEMSG
	
	    /* 长沙商行----添加5天空余量 *****/
	    max_exp_date = max_exp_date+5;
	    sprintf(acErrMsg,"计算贷款最大展期日期[%ld] PASS!",max_exp_date);
	    WRITEMSG
	    	    
	    /* 贷款到期日期 */
	/**根据实际要求贷款到期日不应为贷款展期到期日    
	if ( g_ln_mst.exp_ind[0]!='0' )
	    {
		     g_ln_mst.mtr_date = g_ln_mst.exp_mtr_date ;
	    }
    ******/
    /*20110731 晋中商行 采用信贷系统 由此表确定可以进行的操作*/
      ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth_tmp , "pact_no='%s' and sts='X' and type='02' " , g_ln_mst.pact_no );
			if(ret)
			{
				if(ret ==100){
					sprintf(acErrMsg,"查询贷款审批登记簿无此记录![%s][%d]",g_ln_mst.pact_no,ret);
				  WRITEMSG
				  strcpy(g_pub_tx.reply,"D104");
				  goto ErrExit;
				}
				else{
				  sprintf(acErrMsg,"查询贷款审批登记簿异常出错![%s][%d]",g_ln_mst.pact_no,ret);
				  WRITEMSG
				  strcpy(g_pub_tx.reply,"L181");
				  goto ErrExit;
			  }
			}						
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */
			set_zd_double("0420",g_ln_mst.bal);             /*余额*/	
			set_zd_long( "0440", g_ln_mst.mtr_date );	/* 贷款到期日期 */
			set_zd_long( "0470", g_ln_mst.opn_date );	/* 贷款开户日期 */
			set_zd_long("0460",ln_auth_tmp.mtr_date);/*20110729 根据信贷返回展期日期*/
			set_zd_double("0840",ln_auth_tmp.rate);/*利率*/
			if ( g_ln_mst.exp_ind[0]!='0' )
        {
            set_zd_long("0450",g_ln_mst.exp_mtr_date); /*展期到期日*/
		}

			set_zd_long( "0530", max_exp_date );		/* 最大展期日期 */
    	sprintf(acErrMsg,"最大展期日期[%ld]",max_exp_date );
		  WRITEMSG 					
	}	
	/*************3301到此为止*******************/ 
	
	if( strcmp( tx_code , "3302" )==0 )	
	{
			sprintf(acErrMsg,"交易3302!");
			WRITEMSG
			
			/* 贷款状态为审批时，不能做交易 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"此贷款尚未经过审批!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			************/
			
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
		  set_zd_data( "0700", g_ln_mst.ac_sts );		/* 账户状态 */
		
	}					  

	if( strcmp( tx_code , "3305" )==0 )	
	{
			sprintf(acErrMsg,"交易3305!");
			WRITEMSG
			
			/* 贷款状态为审批时，不能做交易 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"此贷款尚未经过审批!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			************/
			
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
		  set_zd_data( "0700", g_ln_mst.five_sts );		/* 五级分类状态 */
		
	}	
			
	if( strcmp( tx_code , "3314" )==0 )	
	{
		sprintf(acErrMsg,"交易3314!");
		WRITEMSG
						
		set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
		set_zd_data( "0630", g_ln_mst.pact_no );	/* 借据号 */	
		set_zd_double( "0400", g_ln_mst.gage_amt );	/* 抵押品金额 */
		set_zd_double( "0410", g_ln_mst.bal );		/* 贷款余额 */		
	}
					
	if( strcmp( tx_code , "3306" )==0 )	
	{
			sprintf(acErrMsg,"交易3306!");
			WRITEMSG

			/***** 检查贷款类型是否是按揭贷款 
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"此贷款为按揭贷款[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L122" );
				ERR_DEAL
			}	
			按揭贷款检查罚息利率*****/			
	    if( g_ln_mst.ac_sts[0] =='1' )
	    {
		    sprintf( acErrMsg,"!!!!!!!!!!![%ld]",g_pub_tx.tx_date);
	      WRITEMSG
	 	    if ( g_ln_mst.mtr_date >= g_pub_tx.tx_date )
	 	    {
	 		    rate_flag[0]='1';	/* 正常 */
	     	}else{
	 		    rate_flag[0]='2';	/* 展期 */	
	     	}
      }

	    if( g_ln_mst.ac_sts[0] =='2' || g_ln_mst.ac_sts[0] =='3' ||g_ln_mst.ac_sts[0] =='4' || g_ln_mst.ac_sts[0] =='5')
	    {
	 	    rate_flag[0]='3';	/* 逾期 */
    	}

	    /* 输出变量 */

	    set_zd_data( "0250", g_ln_mst.name );		/* 户名 */		
	    set_zd_data( "0700", rate_flag );		/* 账户状态 */	
	    set_zd_data( "0690", g_ln_parm.ln_pay_type );	/* 账户类型 */		
	}
	
	if( strcmp( tx_code , "3317" )==0 )	
	{
			sprintf(acErrMsg,"交易3317!");
			WRITEMSG
			/*按揭贷款不可做此交易*/
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"此贷款为按揭贷款,不可做此交易[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L230" );
				ERR_DEAL
			}
			ret = pub_base_CompDblVal( g_ln_mst.fine_rate, 0.00 );
			if ( ret <= 0 )
			{
				strcpy(g_pub_tx.reply,"L229");
				sprintf(acErrMsg,"该账户尚未定义罚息利率!");
				WRITEMSG
				goto ErrExit;
			}	
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* 计划放款额 */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* 累计发放额 */
			set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
			set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */	
			set_zd_double( "0840", g_ln_mst.fine_rate );		/* 罚息利率 */	
			set_zd_data( "0720" , g_ln_parm.pay_pln_crt ); /* 还款计划联动标志  */
		
	}			
	/****给4519交易返回一些数据  add 20070524
	if( strcmp( tx_code , "3399" )==0 )	
	{
		sprintf(acErrMsg,"交易3399!");
		WRITEMSG
						
		set_zd_data( "0250", g_ln_mst.name );		* 户名 *	
		set_zd_data( "0630", g_ln_mst.pact_no );	* 借据号 *	
		set_zd_double( "0410", g_ln_mst.bal );		* 贷款余额 *		
	}
	end 20070524****/
	vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
	set_zd_double( "0560", g_ln_mst.intst_acm );    /* 本金积数 add by martin 2008/12/10 15:18 */	 		
    	goto OkExit;
    	sprintf(acErrMsg,"redflag[%d]",redflag);
	WRITEMSG 
    	  	
  }  

	vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
    	TRACE
  	redflag=9;
	set_zd_int( "0500", redflag );	/* 是否输入序号标志 9-多条记录*/ 	
	
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	sprintf(acErrMsg,"redflag[%d]ac_seqn[%d]",redflag,ln_mst_tmp.ac_seqn);
	WRITEMSG 
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
