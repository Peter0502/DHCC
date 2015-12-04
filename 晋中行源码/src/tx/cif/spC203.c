/*************************************************
* 文 件 名: spC203.c
* 功能描述：柜员揽存信息录入
*              
*
* 作    者: andy
* 完成日期：2004年06月02日
* 修改记录：   
*     1. 日    期: 
*        修 改 人: 
*        修改内容:
*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "tel_save_inf_c.h"
#include "com_tel_c.h"

spC203()  
{
	struct 	tel_save_inf_c		tel_save_inf;		/* 柜员揽存信息表 */
	struct 	com_tel_c		com_tel_tmp;		/* 柜员信息表 */
	int ret=0;

	memset ( &tel_save_inf, 0x00, sizeof( struct tel_save_inf_c ) );
	memset ( &com_tel_tmp, 0x00, sizeof( struct com_tel_c ) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0370", tel_save_inf.ac_no );		/* 揽存账号 */
	get_zd_data( "0920", tel_save_inf.tel );		/* 柜员代码 */
	get_zd_long( "0340",  &tel_save_inf.ac_seqn ); 		/* 账户序号 */
	
	/* 检查账号是否存在 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , "ac_no='%s'" , tel_save_inf.ac_no );
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
        
        /* 检查账号＋序号是否存在 */
 	/*根据显示帐号和账号序号返回账户类型和主文件*/
	ret = pub_base_disac_file( tel_save_inf.ac_no , tel_save_inf.ac_seqn ,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"调用由显示帐号和账号序号返回账户类型和主文件出错!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto	ErrExit;
	}
	          			
	/* 检查柜员号是否存在 */
	ret = Com_tel_Sel( g_pub_tx.reply , &com_tel_tmp , "tel='%s'" , tel_save_inf.tel );
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
	
	/* 登记客户-客户经理关系表 */			
	ret = Tel_save_inf_Ins( tel_save_inf, g_pub_tx.reply );
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
