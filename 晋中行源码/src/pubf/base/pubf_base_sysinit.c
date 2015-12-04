#include "find_debug.h" 
/*************************************************
* 文 件 名:  pubf_base_sysinit.c
* 功能描述： 初始化 pub_tx 结构
* 作    者:  jack
* 完成日期： 2003/01/05
* 修改记录：  
* 1. 日   期:
*    修 改 人:
*    修改内容:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

/**********************************************************************
* 函 数 名：  pub_base_sysinit
* 函数功能：  初始化 pub_tx 结构
* 作    者：  lance
* 完成时间：  2003年01月05日
*
* 参    数：
*     输  入：
*      
*     输  出:  g_pub_tx结构
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_base_sysinit()
{
	MEMSET_DEBUG ( &g_pub_tx , 0x00 , sizeof(struct pub_tx) );
	/**--- xxx --- 20030918 ---- 如果下面变量有值会自动计流水 ---**/
	MEMSET_DEBUG ( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
   /*以下函数在文 件 名: pubf_base_data.c*/
	get_zd_data( DC_OPN_BR_NO , g_pub_tx.opn_br_no ); /* 开户机构号码 */
	get_zd_data( DC_TX_BR_NO , g_pub_tx.tx_br_no );   /* 交易机构 */
	get_zd_long( DC_TRACE_NO , &g_pub_tx.trace_no);   /* 流水号 */
	get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);   /* 流水笔次 */
	get_zd_long( DC_DATE , &g_pub_tx.tx_date);        /* 交易日期 */
	get_zd_long( DC_TIME , &g_pub_tx.tx_time);        /* 交易时间 */
	get_zd_data( DC_TEL , g_pub_tx.tel );             /* 操作员 */
	get_zd_data( DC_CHK , g_pub_tx.chk );             /* 复核员 */	
	get_zd_data( DC_AUTH , g_pub_tx.auth );           /* 授权人 */
	get_zd_data( DC_TTY , g_pub_tx.tty );             /* 前台终端号 */	
	get_zd_data( DC_FILE_SND , g_pub_tx.file_snd );   /* 批量传送标志 */	
	get_zd_data( DC_REPLY , g_pub_tx.reply );         /* 响应码 */				
	get_zd_long( DC_AUTH_CODE , &g_pub_tx.auth_code); /* 授权码 */		
	get_zd_data( DC_CODE , g_pub_tx.dc_code );        /* 会计代码 */
	get_zd_data( DC_TX_CODE , g_pub_tx.tx_code );     /* 交易代码 */	
	get_zd_data( DC_SUB_TX_CODE , g_pub_tx.sub_tx_code );     /* 交易代码 */	

	strcpy(g_pub_tx.reply,"0000");
	
	vtcp_log("[%s][%d]init_completed!",__FILE__,__LINE__ );	
	return 0;
}
