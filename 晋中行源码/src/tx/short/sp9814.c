/*************************************************
* 文 件 名:    sp9814.c
* 功能描述：   
*              根据贷款账号取出账户序号
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
	
sp9814()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	char tx_code[5];
	char cif_type[2];
		
	double vbjlx,vbnlx,vbylx,vfllx;
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char dc_flg2[2];
	char dc_inf2[5];
		
	FILE *fp;			
	struct ln_mst_c		ln_mst_tmp;
	struct cif_basic_inf_c cif_basic_inf;
		
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "1081" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("1081",ac_no);	/* 处理旧帐号 */	
	
	get_zd_data( "0650" , tx_code ); /* 交易代码 */

	
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
		set_zd_int( "108G", redflag );	/* 是否输入序号标志 1-唯一记录*/
 		sprintf(acErrMsg,"贷款主文件表中无此记录[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L002");
 		goto ErrExit;
   	 }	
	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	
    			
    	sprintf(acErrMsg,"取贷款主文件PASS");
	WRITEMSG 

	
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	redflag=1;
	set_zd_long( "1082", ln_mst_tmp.ac_seqn );	/* 贷款账户序号 */
	
	set_zd_int( "108G", redflag );	/* 是否输入序号标志 1-唯一记录*/
	
	/* 非卡上开户专用通道 */

	
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(ac_no,ln_mst_tmp.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

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
	
	strcpy( dc_flg1 , "1" );
  strcpy( dc_inf1 , "现金" );
	strcpy( dc_flg , "2" );
  strcpy( dc_inf , "转账" );
	strcpy( dc_flg2 , "8" );
  strcpy( dc_inf2 , "扣划" );
	set_zd_data("0660",dc_flg);
	set_zd_data("0810",dc_inf);  
	
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
***************************/
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		fprintf( fp,"0660|%s|%s|\n",dc_inf2,dc_flg2 );
		
	set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);	
	
	if( strcmp( tx_code , "3202" )==0 )	
	{
			sprintf(acErrMsg,"交易3202!");
			WRITEMSG	
						
			/* 检查贷款类型是否是按揭贷款 */
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"此贷款为按揭贷款,不能在此交易![%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L119" );
				ERR_DEAL
			}	
			sprintf(acErrMsg,"检查贷款类型是否是普通贷款 PASS!");
			WRITEMSG		
			TRACE				
			/* 检查贷款是否已经放款 老系统数据问题不检查了 modified by liuxuan 2006-11-29
			ret = pub_base_CompDblVal( g_ln_mst.ttl_prvd_amt, 0.00 );
			if( ret==0 )
			{
    				sprintf( acErrMsg,"此贷款尚未放款!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L159" );		
				goto ErrExit;
			}		
			sprintf(acErrMsg,"检查贷款是否放款 PASS!");
			WRITEMSG
			*/
									
			vbjlx=vbnlx=vbylx=vfllx=0.00;
vtcp_log("--------bjlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bnlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bylx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------fllx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
			if( ret ) { ERR_DEAL }	

vtcp_log("name is [%s]",g_ln_mst.name);
vtcp_log("bal is [%.2lf]",g_ln_mst.bal);
			set_zd_data("1083",g_ln_mst.name);
			set_zd_double("1084",g_ln_mst.bal);
			set_zd_double("1085",g_ln_mst.in_lo_intst);
			set_zd_double("1086",g_ln_mst.out_lo_intst);
			set_zd_double("1087",g_ln_mst.cmpd_lo_intst);
			set_zd_double("1088",vbjlx);
			set_zd_double("1089",vbnlx+vbylx+vfllx);	
			set_zd_data("108J",g_ln_parm.repay_ind);
						   	
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
			set_zd_data("108P" , cif_type);	   
  }
  
  
	if( strcmp( tx_code , "3304" )==0 )	
	{
			sprintf(acErrMsg,"交易3304!");
			WRITEMSG	
														
			vbjlx=vbnlx=vbylx=vfllx=0.00;
vtcp_log("--------bjlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bnlx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------bylx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
			if( ret ) { ERR_DEAL }	
vtcp_log("--------fllx----------");
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
			if( ret ) { ERR_DEAL }	

vtcp_log("name is [%s]",g_ln_mst.name);
vtcp_log("bal is [%.2lf]",g_ln_mst.bal);
			set_zd_data("1083",g_ln_mst.name);
			set_zd_double("1084",g_ln_mst.bal);
			set_zd_double("1085",g_ln_mst.in_lo_intst);
			set_zd_double("1086",g_ln_mst.out_lo_intst);
			set_zd_double("1087",g_ln_mst.cmpd_lo_intst);
			set_zd_double("1088",vbjlx);
			set_zd_double("1089",vbnlx+vbylx+vfllx);	
			set_zd_data("108J",g_ln_parm.repay_ind);			   	
			set_zd_double("108B",g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst);
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
			set_zd_data("108P" , cif_type);	
			   
  }  
  

   	goto OkExit;
  }
    	
    	
  	redflag=9;
	set_zd_int( "108G", redflag );	/* 是否输入序号标志 1-唯一记录*/	 		
		
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
