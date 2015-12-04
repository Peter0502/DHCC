/*************************************************
* 文 件 名: pubf_ln_modify.c
* 功能描述：   
*              pub_ln_change      贷款产品变更
*
* 作    者:    rob
* 完成日期：   2003年12月27日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

/**********************************************************************
* 函 数 名：  pub_ln_change
* 函数功能：  贷款产品变更，将账户对应得产品编号改变
* 作    者：  rob
* 完成时间：  2003年12月27日
*
* 参    数：
*     输  入：ac_id     integer   账户ID   
*             ac_seqn   smailint  账户序号
*             prdt_no   char(3)   新产品编号
*      
*     输  出:           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/

int pub_ln_change( int ac_id, int ac_seqn , char *prdt_no )
{
    struct prdt_ac_id_c prdt_ac_id;

    int ret;
    char tx_type[2];
    char prdt_no_tmp[4];

    memset( &prdt_ac_id , 0x00 , sizeof ( struct prdt_ac_id_c ) );
	memset( &g_ln_mst, 0x00, sizeof ( struct ln_mst_c ) );

	/* 取贷款信息 */
	ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst , "ac_id=%ld and ac_seqn=%d" ,
						 ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		return -1;
    	}
    	
    strcpy( prdt_no_tmp , g_ln_mst.prdt_no );
    	
    /* 调用贷款开户检查函数 */
   	tx_type[0] = '1' ;
    strcpy( g_ln_mst.prdt_no , prdt_no );
    pub_ln_check(&tx_type[0]);
    
    strcpy( g_ln_mst.prdt_no , prdt_no_tmp );
        
    /* 更新ln_mst表中产品代码 */

    ret = Ln_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d" ,
						 ac_id,ac_seqn );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
	} else if ( ret !=0 )
		{
			sprintf( acErrMsg,"取贷款主文件表数据异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
		}

    ret = Ln_mst_Fet_Upd ( &g_ln_mst, g_pub_tx.reply);
	if ( ret == 100 ) 
	{
		sprintf( acErrMsg,"FETCH ln_mst 出错 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D104" );
		return 1;
    } else if ( ret != 0 )
		{
			sprintf( acErrMsg,"执行ln_mst时出错 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
    	}
    
	strcpy( g_ln_mst.prdt_no , prdt_no );

    ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply);
    if ( ret == -239 )
	{
		sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错 code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D106" );
		return -1;
	} else if ( ret != 0 )
		{
			sprintf( acErrMsg,"更新ln_mst时出错 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D105" );
			return -1;
		}

	/* 更新prdt_ac_id表中产品代码 */
    ret = Prdt_ac_id_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d" ,
						   ac_id,ac_seqn );
	if ( ret==100 )
	{
		sprintf( acErrMsg,"产品账号对照表中无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"W012" );
		return 1;
	} else if ( ret !=0 )
		{
			sprintf( acErrMsg,"取产品账号对照表数据异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
		}

    ret = Prdt_ac_id_Fet_Upd ( &prdt_ac_id, g_pub_tx.reply);
	if ( ret == 100 ) 
	{
		sprintf( acErrMsg,"FETCH prdt_ac_id 出错 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D104" );
		return 1;
    } else if ( ret != 0 )
		{
			sprintf( acErrMsg,"执行prdt_ac_id时出错 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
			return -1;
    	}
    
	strcpy( prdt_ac_id.prdt_no , prdt_no );
    ret = Prdt_ac_id_Upd_Upd( &prdt_ac_id, g_pub_tx.reply);
    if ( ret == -239 )
	{
		sprintf( acErrMsg,"执行Prdt_ac_id_Upd_Upd错 code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D106" );
		return -1;
	} else if ( ret != 0 )
		{
			sprintf( acErrMsg,"更新prdt_ac_id时出错 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D105" );
			return -1;
		}

    return 0;
}



