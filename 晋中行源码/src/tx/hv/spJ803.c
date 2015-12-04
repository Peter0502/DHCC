/***************************************************************
* 文 件 名: spJ803.c
* 功能描述：日间透支计息来报处理
*			登记日间透支计息汇总表
*			登记日间透支计息明细表
*
* 作    者: jane
* 完成日期：2004年07月14日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "com_parm_c.h"
#include "find_debug.h"

spJ803()
{
	int i;
	char *accrualinfo_p;
	char accrualinfo_buf[30];

	struct mbfeoverdraftcl_c overdraftcl_v;
	struct mbfeoverdraftlt_c overdraftlt_v;
	struct com_parm_c com_parm_v;

	memset( &overdraftcl_v, 0x00, sizeof( struct mbfeoverdraftcl_c ) );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );

	overdraftcl_v.consigndate = atol( mbfetran.consigndate );
	strcpy( overdraftcl_v.rdficenter, mbfetran.rdficenter );
	strcpy( overdraftcl_v.osdficode, mbfetran.osdficode );
	strcpy( overdraftcl_v.rdficode, mbfetran.osdficode );
	overdraftcl_v.accrualamnt = atof( mbfetran.accrualamnt ) / 100;
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='5'");
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询大额日间透支计息科目出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( overdraftcl_v.accruacno, com_parm_v.val );
	overdraftcl_v.printno = 0;

	g_reply_int = Mbfeoverdraftcl_Ins( overdraftcl_v, g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "登记日间透支计息汇总表数据库操作失败[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	for( i=0; i<atol( mbfetran.number ); i++ )
	{
		memset( &overdraftlt_v, 0x00, sizeof( struct mbfeoverdraftlt_c ) );

		overdraftlt_v.row_id = i + 1;
		overdraftlt_v.consigndate = atol( mbfetran.consigndate );
		MEMCPY_DEBUG( overdraftlt_v.accrualtime, accrualinfo_p, 6 ); accrualinfo_p += 6;
		MEMCPY_DEBUG( accrualinfo_buf, accrualinfo_p, 22 );
		accrualinfo_buf[22] = '\0'; accrualinfo_p += 22;
		overdraftlt_v.overdraftamnt = atof( accrualinfo_buf ) / 100;
		MEMCPY_DEBUG( accrualinfo_buf, accrualinfo_p, 6 );
		accrualinfo_buf[6] = '\0'; accrualinfo_p += 6;
		overdraftlt_v.rate = atof( accrualinfo_buf ) / 10000;
		MEMCPY_DEBUG( accrualinfo_buf, accrualinfo_p, 22 );
		accrualinfo_buf[22] = '\0'; accrualinfo_p += 22;
		overdraftlt_v.accrualamnt = atof( accrualinfo_buf ) / 100;

		g_reply_int = Mbfeoverdraftlt_Ins( overdraftlt_v, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "登记日间透支计息明细表数据库操作失败[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}

	/*为记账子交易传值*/
	set_zd_data( "1201", com_parm_v.val );
	set_zd_data( "1204", "01" );
	set_zd_data( "1205", "2" );
	set_zd_double( "1208", overdraftcl_v.accrualamnt );
	set_zd_data( "120A", "大额日间透支计息" );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='YHZBJ' and parm_seqn='1'");
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询央行准备金账号出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "1211", com_parm_v.val );
	set_zd_data( "1214", "01" );
	set_zd_data( "1215", "2" );
	set_zd_double( "1218", overdraftcl_v.accrualamnt );
	set_zd_data( "121A", "大额日间透支计息" );


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"处理日间透支计息报文成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"处理日间透支计息报文失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
