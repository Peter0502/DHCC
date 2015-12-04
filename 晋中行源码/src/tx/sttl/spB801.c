/*************************************************
* 文 件 名:  spB801.c
* 功能描述： 完成登记再贷款登记薄功能。
*
* 作    者:  jane
* 完成日期： 2003年7月23日
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
#include "mo_again_ln_c.h"

int spB801()
{
	struct	mo_again_ln_c	sMoAgainLn,sMoAgainLn_c;	/* 再贷款登记薄 */
	int	ret;
	char flag[2];

	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sMoAgainLn_c, 0x00, sizeof( struct mo_again_ln_c ) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0680", flag);	/* 0增加   1修改 */
	get_zd_data( "0670", sMoAgainLn.again_ln_type );	/* 再贷款类型 */
	get_zd_data( "0580", sMoAgainLn.pact_no );			/* 协议编号 */
	strcpy( sMoAgainLn.br_no, g_pub_tx.tx_br_no );		/* 机构编码 */
	strcpy( sMoAgainLn.tel, g_pub_tx.tel );				/* 操作员 */
	sMoAgainLn.reg_date = g_pub_tx.tx_date;				/* 登记日期 */
	get_zd_double( "1001", &sMoAgainLn.amt );			/* 贷款金额 */
	get_zd_long( "0440", &sMoAgainLn.to_date );			/* 到期日期 */
	get_zd_double( "0840", &sMoAgainLn.rate );			/* 利率 */

	switch( atoi(flag) ){
		case 0:
			ret = Mo_again_ln_Sel(g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'",sMoAgainLn.pact_no);
			if( ret == 0 )
			{
				sprintf( acErrMsg, "再贷款登记薄中已经存在此协议编号!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B106" );
				goto ErrExit;
			}else if( ret != 100 )
			{
				sprintf( acErrMsg, "执行Mo_again_ln_Sel错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			/* 取再贷款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
			ret = pub_base_GetParm_Str( "ZDK", 1, sMoAgainLn.prdt_no );
			if( ret )
			{
				sprintf( acErrMsg, "取再贷款产品编号错!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B255" );
				goto ErrExit;
			}

			/* 登记再贷款登记薄，标志为录入 */
			strcpy( sMoAgainLn.sts, "0" );				/* 标志：0录入 */
			ret = Mo_again_ln_Ins( sMoAgainLn, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_again_ln_Ins错误!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			break;
        case 1 :
            /* 修改 */
            ret = Mo_again_ln_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts ='0'",
                  sMoAgainLn.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            ret = Mo_again_ln_Fet_Upd(&sMoAgainLn_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"再贷款登记薄中不存在此协议编号!ret=[%d]",ret);
                WRITEMSG
                Mo_again_ln_Clo_Upd();
                strcpy(g_pub_tx.reply,"B107");
                goto ErrExit;
            }

            if (sMoAgainLn_c.sts[0] != '0')
            {
                sprintf(acErrMsg,"该笔再贷款状态不正确!");
                WRITEMSG
                Mo_again_ln_Clo_Upd();
                strcpy(g_pub_tx.reply,"B286");
                goto ErrExit;
            }

		strcpy( sMoAgainLn.sts, "0" );				/* 标志：0录入 */
	    strcpy(sMoAgainLn.rowid, sMoAgainLn_c.rowid);
            ret = Mo_again_ln_Upd_Upd(sMoAgainLn, g_pub_tx.reply);
            if ( ret != 0)
            {
                sprintf(acErrMsg,"修改再贷款登记簿错误!! ret =[%s]",ret);
                WRITEMSG
                Mo_again_ln_Clo_Upd();
                strcpy(g_pub_tx.reply,"B107");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"再贷款信息修改");
            Mo_again_ln_Clo_Upd();
            sprintf(acErrMsg,"修改再贷款登记簿pass!! ");
            WRITEMSG
            break;
        default :
            sprintf(acErrMsg,"操作类型出错!! type=[%c] 0录入1修改2删除",flag);
            goto ErrExit;
            break;
	}

	/* 记流水日志 */
	/**----- xxx ---- @20030821@ ---- 撤销必须要有协议编号 ----**/
	strcpy( g_pub_tx.brf, "再贷款维护" );
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	g_pub_tx.tx_amt1=sMoAgainLn.amt;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
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
