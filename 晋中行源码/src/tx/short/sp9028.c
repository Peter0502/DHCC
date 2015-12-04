/*********************************************************************
* 版 本 号 :  V1.0
* 程序作者 :  武大为
* 日    期 :  20140613
* 所属模块 :  揽存关系维护交易信息查询
* 程序名称 :  sp9028.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
* 修改记录 :
交易配置：
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('9028', '揽存关系查询', '10000000000000000000000000000000000000000000000000000000000000000000000000000000100000000001000000000000000000000000000000000000', '1', '4', '2');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('9028', 0, '9028', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('9028', '揽存关系查询', 'sp9028', '0', '0');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "tel_save_rel_c.h"

int sp9028()
{
		int ret = 0;
    struct 	tel_save_rel_c		tel_save_rel;		/* 柜员揽存信息表 */
    memset ( &tel_save_rel, 0x00, sizeof( struct tel_save_rel_c ) );
    
		 /**初始化公用结构**/
		 if (pub_base_sysinit())
		 {
		     vtcp_log("初始化公用结构错误!!");
		     strcpy(g_pub_tx.reply, "CU06");
		     goto ErrExit;
		 }
    
    /* 从前台屏幕取值 */
		get_zd_data( "0370", tel_save_rel.ac_no );		/* 揽存账号 */
		get_zd_data( "0680", tel_save_rel.tx_type );	/* 绑定类型 1帐号 2客户号*/
		get_zd_long( "0290", &tel_save_rel.cif_no ); 
  
		if(tel_save_rel.tx_type[0] == '1')
		{
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
				if( ret && ret!=100 )
				{
						sprintf( acErrMsg,"读取揽存关系失败,请与中心机房联系!ret=[%d]",ret);
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "S048" );
			                goto ErrExit;
			  }else if ( ret==100 ){
			  		sprintf( acErrMsg,"该账号不存在,请检查!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C115" );
			                goto ErrExit;
			  }
			  set_zd_data( "0920", tel_save_rel.tel );
			  set_zd_data( "0810", tel_save_rel.brf );
		}else if(tel_save_rel.tx_type[0] == '2')
		{
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel , "cif_no='%ld' and sts='1'" , tel_save_rel.cif_no );
				if( ret && ret!=100 )
				{
						sprintf( acErrMsg,"读取揽存关系失败,请与中心机房联系!ret=[%d]",ret);
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "S048" );
			                goto ErrExit;
			  }else if ( ret==100 ){
			  		sprintf( acErrMsg,"该客户号不存在,请检查!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C051" );
			                goto ErrExit;
			  }
			  set_zd_data( "0920", tel_save_rel.tel );
			  set_zd_data( "0810", tel_save_rel.brf );
		}
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
 