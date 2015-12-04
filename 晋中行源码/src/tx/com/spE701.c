/*************************************************
* 文 件 名:  spE701.c
* 功能描述： 撤销
*
* 作    者:  andy
* 完成日期： 2004年3月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "com_parm_c.h"

int spE701()
{
	long vtrace_no;

	int ret = 0;
	char cAc_no[25];
	double dTram = 0.00;
	
	char cTrcode[3];	/*取交易码的前两位*/

	struct trace_log_c sTrace_log;
	struct com_parm_c  sCom_parm;

	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_long("0440",&vtrace_no);
 	sprintf(acErrMsg," q[%ld]!!",vtrace_no);
    	WRITEMSG
	/*---------add by jack 20040729------begin----*/
	memset( cAc_no, 0x0, sizeof(cAc_no));
	get_zd_data( "0300", cAc_no );
	get_zd_double( "0400", &dTram );

	sprintf(acErrMsg,"-------------> 撤销流水号为[%ld]账号[%s]金额[%lf]", vtrace_no, cAc_no, dTram );
	WRITEMSG

	pub_base_strpack( cAc_no );
	memset( &sTrace_log, 0x0, sizeof(sTrace_log) );

	if ( strlen( cAc_no ) != 0 )	/*--外围或非帐务类等业务--*/
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld \
				and ac_no = '%s' and amt = %lf and sts = '0'", \
				vtrace_no, g_pub_tx.tx_date, cAc_no, dTram );
	}
	else
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld and sts = '0'", \
				vtrace_no, g_pub_tx.tx_date );
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
	if ( !strncmp(sTrace_log.tx_code,"41",2) &&
		strcmp(sTrace_log.tx_code,"4137") && 
		strcmp(sTrace_log.tx_code,"4141") )
	{
		strcpy( g_pub_tx.reply, "S053" );
    	sprintf( acErrMsg, "不能撤销" );
    	WRITEMSG
		goto ErrExit;
	}
	
	sprintf(acErrMsg,"-------------> 撤销流水号为[%ld]账号[%s]金额[%lf],交易tx_code[%s]", vtrace_no, cAc_no, dTram,sTrace_log.tx_code );
	WRITEMSG
	
	if( !strcmp(sTrace_log.tx_code,"5110") )/*** 2008-11-20 17:06 add by martin 取消撤销柜员凭证注销 ***/
		{
		strcpy( g_pub_tx.reply, "S053" );
    	sprintf( acErrMsg, "不能撤销" );
    	WRITEMSG
		goto ErrExit;
	  }
	  
	/*** 2008/12/9 14:16 add by martin 根据com_parm 表查找不能撤销的交易 ***/
	memset( &sCom_parm, 0x0, sizeof( struct com_parm_c ) );
	ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm,
                        "parm_code='REVOC' and val='%s'",sTrace_log.tx_code );
                if( ret==0 )
                {
                        sprintf(acErrMsg,"%s不能撤销![%d]",sCom_parm.parm_name,ret);
                        WRITEMSG
                        
                        switch(sCom_parm.val_type[0])
                        {
                        	case '1':
                             strcpy( g_pub_tx.reply, "S081" );
                             break;
                          default:
			                       strcpy( g_pub_tx.reply, "S053" );
			                       break;
                        }
                        goto ErrExit;
                }

	
	pub_base_strpack( sTrace_log.tx_code );
	memset( cTrcode, 0x00, sizeof(cTrcode) );
	memcpy( cTrcode, sTrace_log.tx_code, 2 );
	if ( !strcmp( g_pub_tx.tx_code, "5701" ) )
	{
		if ( !strcmp( cTrcode, "62" ) )
		{
			strcpy( g_pub_tx.reply, "E031" );
    			sprintf( acErrMsg, "[%s]有专门交易撤销", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
		if ( !strcmp( cTrcode, "64" ) )
		{
			strcpy( g_pub_tx.reply, "E032" );
    			sprintf( acErrMsg, "[%s]有专门交易撤销", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
		/*-----------------------------------------------*
		if ( !strcmp( cTrcode, "67" ) )
		{
			strcpy( g_pub_tx.reply, "E033" );
    			sprintf( acErrMsg, "[%s]有专门交易撤销", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
		*-----------------------------------------------*/
		if ( !strcmp( cTrcode, "77" ) )
		{
			strcpy( g_pub_tx.reply, "E034" );
    			sprintf( acErrMsg, "[%s]有专门交易撤销", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
	}
	/*---------add by jack 20040729------end------*/
        sprintf(acErrMsg,"开始调用函数[%ld]!!",vtrace_no);
    	WRITEMSG
	/*调用函数*/
	ret=pub_acct_rollback(vtrace_no);
	if(ret)
	{
    	sprintf(acErrMsg,"调用函数出错[%s]!!",g_pub_tx.reply);
    	WRITEMSG
		goto ErrExit;
	}

    /*** 登记交易流水 ***/
    if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
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
