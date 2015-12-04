/***********************************************************
* 文 件 名: spF704.c
* 功能描述：异地活期最后交易查询(不要密码查询余额)
* 作    者: jack
* 完成日期：2003年2月21日
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:	
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"

spF704()
{
	int ret = 0;
	char cCity[5];

	long lMax_date = 0;
	long lMax_trace = 0;
	
	struct dd_mst_hst_c sDd_mst_hst;

	memset(cCity,0x0,sizeof(cCity));
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    memset( &sDd_mst_hst, 0x0, sizeof(sDd_mst_hst) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0360", cCity );

	if( strcmp(cCity, "4302") != 0)
	{
			strcpy(g_pub_tx.reply,"F001");
			sprintf(acErrMsg,"非本行卡,不能在本行主机查询![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
	}

    get_zd_data("0370",g_mdm_ac_rel.ac_no);

    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, \
				"ac_no='%s'", g_mdm_ac_rel.ac_no );
    if(  ret == 100 )
    {
       	sprintf(acErrMsg,"----------->查询的账号不存在!! [%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
    }
	if( ret ) 
	{
       	sprintf(acErrMsg,"------->查询账号信息错误!! [%d]", ret );
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and \
			ac_seqn = 1", g_mdm_ac_rel.ac_id );
    if( ret )
    {
        sprintf(acErrMsg,"查询户名和余额错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
    
    ret = sql_max_long ( "dd_mst_hst", "tx_date", &lMax_date , \
		"ac_id = %ld", g_mdm_ac_rel.ac_id );
	if ( ret )
	{
        sprintf( acErrMsg, "查询该账户最大交易日期出错!!!" );
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
	}

    ret = sql_max_long ( "dd_mst_hst", "trace_no", &lMax_trace , \
		"ac_id = %ld and tx_date = %ld", g_mdm_ac_rel.ac_id, lMax_date );
	if ( ret )
	{
        sprintf( acErrMsg, "查询[%ld]最大流水出错!!!", lMax_date );
        WRITEMSG
        strcpy(g_pub_tx.reply, "D103");
        goto ErrExit;
	}

	ret = Dd_mst_hst_Sel( g_pub_tx.reply, &sDd_mst_hst, \
		"ac_id = %ld and tx_date = %ld and trace_no = %ld", \
		g_mdm_ac_rel.ac_id, lMax_date, lMax_trace );
	if ( ret )
	{
        sprintf( acErrMsg, "查询[%ld][%ld][%ld]交易明细错!", \
			g_mdm_ac_rel.ac_id, lMax_date, lMax_trace );
        WRITEMSG
        goto ErrExit;
	}

	set_zd_data( "0250", g_dd_mst.name );
	set_zd_data( "0270", g_dd_mst.name );
	set_zd_double( "0400", sDd_mst_hst.tx_amt );
	set_zd_long( "0050", sDd_mst_hst.tx_date );
	set_zd_double( "0420", g_dd_mst.bal );

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询余额处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询余额处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
