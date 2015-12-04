/*************************************************
* 文 件 名:  spN113.c
* 功能描述： 撤销凭证管理部分交易
*
* 作    者:  andy
* 完成日期： 2004年1月22日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "trace_log_c.h"
int spN113()
{
	char	cHstFlag[2];		/* 是否删除明细标志Y删除N不删除*/
								/* 如果不删除，将新增一条明细 */
	long	trace_no;			/* 流水号 */
	int	ret;

	/* 初始化g_pub_tx结构 */
	pub_base_sysinit();
	get_zd_long( "0520" , &trace_no );
	
	/* 调用撤销函数，明细默认不删除 */
	ret = pub_com_NoteCX( trace_no, 0, g_pub_tx.tx_date, "N" );
	if ( ret )
	{
		sprintf( acErrMsg, "撤销凭证管理交易错" );
		WRITEMSG
		goto ErrExit;
	}
	/*if ( strlen( cAc_no ) != 0 )	
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld \
				and ac_no = '%s' and amt = %lf and sts = '0'", \
				trace_no, g_pub_tx.tx_date, cAc_no, dTram );
	}
	else
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld and sts = '0'", \
				trace_no, g_pub_tx.tx_date );
	}
	if ( ret == 100 )
	{
		strcpy( g_pub_tx.reply, "E001" );
    	sprintf( acErrMsg, "要被撤销的交易记录不存在" );
    	WRITEMSG
		goto ErrExit;
	}
	else if ( ret )
	{
		strcpy( g_pub_tx.reply, "D103" );
    	sprintf( acErrMsg, "查询被撤流水记录错!!!" );
    	WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack( sTrace_log.tel );
	vtcp_log("原操作员是[%s],做取消交易的操作员是[%s]\n",sTrace_log.tel,g_pub_tx.tel);
	if ( strcmp( sTrace_log.tel, g_pub_tx.tel ) )
	{
		strcpy( g_pub_tx.reply, "E002" );
    	sprintf( acErrMsg, "非本柜员交易,不能撤销" );
    	WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"开始调用函数[%ld]!!",trace_no);
  WRITEMSG
ret=pub_acct_rollback(trace_no);
	if(ret)
	{
    	sprintf(acErrMsg,"调用函数出错[%s]!!",g_pub_tx.reply);
    	WRITEMSG
		goto ErrExit;
	}
*/
OkExit:
        strcpy( g_pub_tx.reply, "0000" );
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;
}
