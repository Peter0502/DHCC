/*************************************************
* 文 件 名: spC204.c
* 功能描述：柜员揽存信息录入
*              
*
* 作    者: 武大为
* 完成日期：2014年06月13日
* 修改记录：   
*     1. 日    期: 
*        修 改 人: 
*        修改内容:
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('1204', '柜员揽存关系维护', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '6', '2');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('1204', 0, 'C204', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('C204', '柜员揽存关系维护', 'spC204', '0', '0');

insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('1204', 1, '', '', '', '', '', '', '', '', '0000', '柜员揽存维护');

insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '1204', 1, '柜员揽存维护', 'sq(1237)');
*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "tel_save_rel_c.h"
#include "com_tel_c.h"
#include "mdm_ac_rel_c.h"

spC204()  
{
	struct 	tel_save_rel_c		tel_save_rel;		/* 柜员揽存信息表 */
	struct 	tel_save_rel_c		tel_save_rel_tmp;	
	struct 	com_tel_c		com_tel_tmp;		/* 柜员信息表 */
	struct 	mdm_ac_rel_c		g_mdm_ac_rel		;		
	int ret=0;
	char flag[2];

	memset ( &tel_save_rel, 0x00, sizeof( struct tel_save_rel_c ) );
	memset ( &tel_save_rel_tmp, 0x00, sizeof( struct tel_save_rel_c ) );
	memset ( &g_mdm_ac_rel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset ( &com_tel_tmp, 0x00, sizeof( struct com_tel_c ) );
	memset ( flag, 0x00, sizeof(flag) );
	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0370", tel_save_rel.ac_no );		/* 揽存账号 */
	get_zd_data( "0920", tel_save_rel.tel );		/* 柜员代码 */
	get_zd_long( "0340",  &tel_save_rel.ac_seqn ); 		/* 账户序号 */
	get_zd_data( "0680", tel_save_rel.tx_type );	/* 绑定类型 1帐号 2客户号*/
	get_zd_long( "0290", &tel_save_rel.cif_no ); 
	get_zd_data( "0810", tel_save_rel.brf );  /*  备注 */
	get_zd_data( "0690", flag );
	
	sprintf( acErrMsg,"spC204 !flag=[%s]",flag);
	WRITEMSG
	
	/* 检查柜员号是否存在 */
	/* 按要求去掉交易机构的检查 wudawei 20141008 */
	ret = Com_tel_Sel( g_pub_tx.reply , &com_tel_tmp , "tel='%s' " , tel_save_rel.tel );
	if( ret && ret!=100 )
	{
			sprintf( acErrMsg,"读取柜员信息失败,请与中心机房联系!ret=[%d]",ret);
                WRITEMSG
                strcpy( g_pub_tx.reply, "C118" );
                goto ErrExit;
  }else if ( ret==100 ){
  		sprintf( acErrMsg,"该柜员不存在,请检查!");
                WRITEMSG
                strcpy( g_pub_tx.reply, "C119" );
                goto ErrExit;
  }     	
	
	if(tel_save_rel.tx_type[0] == '1')
	{
		/* 检查账号是否存在 介质状态是否正常 是否本机构的帐号  */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , "ac_no='%s'" , tel_save_rel.ac_no );
		if( ret && ret!=100 )
		{
			sprintf( acErrMsg,"读取介质账号对照表失败,请与中心机房联系!ret=[%d]",ret);
		              WRITEMSG
		              strcpy( g_pub_tx.reply, "C114" );
		              goto ErrExit;
		}else if ( ret==100 ){
				sprintf( acErrMsg,"该账号不存在,请检查!");
		              WRITEMSG
		              strcpy( g_pub_tx.reply, "C115" );
		              goto ErrExit;
		}   
		if(strcmp(g_mdm_ac_rel.opn_br_no , g_pub_tx.tx_br_no)!= 0)
		{
					sprintf( acErrMsg,"请在开户行办理此业务!!ret=[%d]",ret);
			            WRITEMSG
		              strcpy( g_pub_tx.reply, "D245" );
		              goto ErrExit;
		}
		if(g_mdm_ac_rel.note_sts[0] != '0') 
		{
			  	sprintf( acErrMsg,"该帐号介质状态不正常!ret=[%d]",ret);
			            WRITEMSG
		              strcpy( g_pub_tx.reply, "D140" );
		              goto ErrExit;
		}
	}
	
	/* 关联 */
	if(flag[0] == '1')
	{
		/* 关联帐号 */
		if(tel_save_rel.tx_type[0] == '1')
		{		
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
				if(ret == 0)
				{
						sprintf( acErrMsg,"已经存在了此帐号的关联信息!ret=[%d]",ret);
			      WRITEMSG
						strcpy( g_pub_tx.reply, "H001" );
			      goto ErrExit;
			  }
				
			  
		} 
		else if(tel_save_rel.tx_type[0] == '2'){
			
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "cif_no=%ld and sts='1'" , tel_save_rel.cif_no );
				if(ret == 0)
				{
						sprintf( acErrMsg,"已经存在了此帐号的关联信息!ret=[%d]",ret);
			      WRITEMSG
						strcpy( g_pub_tx.reply, "H001" );
			      goto ErrExit;
			  }
		}    
		tel_save_rel.tx_date = g_pub_tx.tx_date;
		tel_save_rel.tx_time = g_pub_tx.tx_time;
		tel_save_rel.trace_no = g_pub_tx.trace_no;
		strcpy(tel_save_rel.tx_tel, g_pub_tx.tel);
		strcpy(tel_save_rel.br_no , g_pub_tx.tx_br_no);
		strcpy(tel_save_rel.sts , "1");   	
		/* 登记揽存关联表 */			
		ret = Tel_save_rel_Ins( tel_save_rel, g_pub_tx.reply );
		if( ret == -239 )
	    	{
	      	 	sprintf( acErrMsg, "此条揽存信息已登记!" );
	      	 	WRITEMSG
	      	 	strcpy( g_pub_tx.reply, "C116" );
	      	 	goto ErrExit;
		}else if ( ret ){
	       		sprintf( acErrMsg, "登记柜员揽存信息登记薄异常出错,请与中心机房联系!ret=[%d]", ret );
	       		WRITEMSG
	      	 	strcpy( g_pub_tx.reply, "C117" );
	       		goto ErrExit;
		}		
	}
	
	/* 维护 */
	else if(flag[0] == '2')
	{
			if(tel_save_rel.tx_type[0] == '1')
			{		
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
					if(ret != 0)
					{
							sprintf( acErrMsg,"该账号不存在,请检查!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C115" );
			                goto ErrExit;
				  }
			} 
			else if(tel_save_rel.tx_type[0] == '2')
			{	
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "cif_no=%ld and sts='1'" , tel_save_rel.cif_no );
					if(ret != 0)
					{
								sprintf( acErrMsg,"该客户号不存在,请检查!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C051" );
			                goto ErrExit;
				  }
			}    
			tel_save_rel_tmp.tx_date = g_pub_tx.tx_date;
			tel_save_rel_tmp.tx_time = g_pub_tx.tx_time;
			tel_save_rel_tmp.trace_no = g_pub_tx.trace_no;
			strcpy(tel_save_rel_tmp.tx_tel, g_pub_tx.tel);
			strcpy(tel_save_rel_tmp.tel, tel_save_rel.tel);
			strcpy(tel_save_rel_tmp.brf, tel_save_rel.brf);
			/*修改*/
			ret = Tel_save_rel_Upd_Upd( tel_save_rel_tmp , g_pub_tx.reply );
			if (ret != 0)
	    {
	        sprintf(acErrMsg,"update error !! [%d]",ret);
	        WRITEMSG
	        Tel_save_rel_Clo_Upd();
	        goto ErrExit;
	    }
	    Tel_save_rel_Clo_Upd();

	}
	
  /* 移除 */
	else if(flag[0] == '3')
	{
			if(tel_save_rel.tx_type[0] == '1')
			{		
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
					if(ret != 0)
					{
							sprintf( acErrMsg,"该账号不存在,请检查!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C115" );
			                goto ErrExit;
				  }
			} 
			else if(tel_save_rel.tx_type[0] == '2')
			{	
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "cif_no=%ld and sts='1'" , tel_save_rel.cif_no );
					if(ret != 0)
					{
								sprintf( acErrMsg,"该客户号不存在,请检查!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C051" );
			                goto ErrExit;
				  }
			}    
			tel_save_rel_tmp.tx_date = g_pub_tx.tx_date;
			tel_save_rel_tmp.tx_time = g_pub_tx.tx_time;
			tel_save_rel_tmp.trace_no = g_pub_tx.trace_no;
			strcpy(tel_save_rel_tmp.tx_tel, g_pub_tx.tel);
			strcpy(tel_save_rel_tmp.sts , "*");  
			/*修改sts=*   */
			ret = Tel_save_rel_Upd_Upd( tel_save_rel_tmp , g_pub_tx.reply );
			if (ret != 0)
	    {
	        sprintf(acErrMsg,"update error !! [%d]",ret);
	        WRITEMSG
	        Tel_save_rel_Clo_Upd();
	        goto ErrExit;
	    }
	    Tel_save_rel_Clo_Upd();

	}
	
	  	
	/* 记流水日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"登记柜员揽存信息登记薄成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"登记柜员揽存信息登记薄失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
