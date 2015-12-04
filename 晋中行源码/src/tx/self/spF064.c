/*************************************************
* 文 件 名:  spF064.c
* 功能描述： 自助终端补登存折(补登到客户手里存折满页为止)
*
* 作    者:  jack
* 完成日期： 2003年6月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_unprt_hst_c.h"

int spF064()
{

	int ret = 0;
	long num = 0;
	long icount = 0;
	char cAcno[20];

	char filename[128];
	FILE *fp;

	char tmpstr[128];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_unprt_hst_c sMdm_unprt_hst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( filename ,0x00,sizeof(filename));
	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sMdm_unprt_hst,0x00,sizeof(sMdm_unprt_hst));
	memset( tmpstr,0x00,sizeof(tmpstr));
	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );

    if ( pub_base_sysinit() )
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

	ret = Mdm_ac_rel_Dec_Upd( g_pub_tx.reply , "ac_no = '%s'", cAcno );
	if ( ret )
	{
        sprintf( acErrMsg, "定义游标Mdm_ac_rel_Dec_Upd错[%d]", ret );
        WRITEMSG
        goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd( &sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
        sprintf( acErrMsg, "查询账号介质对照表出错[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	if ( sMdm_ac_rel.prt_line % BANK_BOOK_LENG == 0 )
	{
		strcpy( g_pub_tx.reply, "0001" );
        sprintf( acErrMsg, "--------->存折满页,请更换存折!!!!!" );
        WRITEMSG
        goto ErrExit;
	}

	pub_base_AllDwnFilName( filename );

	sprintf( acErrMsg,"------>生成补登折文件名为[%s]", filename );
	WRITEMSG

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf( acErrMsg,"生成文件错误![%s]", filename );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P157");
		goto ErrExit;
	}

	ret = Mdm_unprt_hst_Dec_Sel( g_pub_tx.reply , \
			"ac_id = %ld order by tx_date,trace_no", \
			sMdm_ac_rel.ac_id );
	if ( ret )
	{
        sprintf( acErrMsg, "------>定义未登折明细游标错[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	while (1)
	{
		memset( &sMdm_unprt_hst, 0x00, sizeof(sMdm_unprt_hst) );
		ret = Mdm_unprt_hst_Fet_Sel( &sMdm_unprt_hst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			if ( num == 0 )
			{
        		sprintf( acErrMsg, "------>无未登折明细[%d]!!!", ret );
        		WRITEMSG
				strcpy( g_pub_tx.reply, "O190");
        		goto ErrExit;
			}

			break;
		}
		else if ( ret )
		{
        	sprintf( acErrMsg, "------>查询未登折明细错[%d]!!!", ret );
        	WRITEMSG
        	goto ErrExit;
		}

		pub_base_strpack( sMdm_unprt_hst.brf );
		fprintf( fp, "%ld,%s,%s,%.2f,%.2f,%s\n", sMdm_unprt_hst.tx_date, sMdm_unprt_hst.brf, sMdm_unprt_hst.add_ind,sMdm_unprt_hst.tx_amt, sMdm_unprt_hst.bal, sMdm_unprt_hst.tel );

		ret = sql_execute( "DELETE FROM mdm_unprt_hst where \
			ac_id = %ld and tx_date = %ld and trace_no = %ld", \
			sMdm_ac_rel.ac_id, sMdm_unprt_hst.tx_date, \
			sMdm_unprt_hst.trace_no );
		if ( ret )
		{
        	sprintf( acErrMsg, "------>删除未打印明细错[%d]!!!", ret );
        	WRITEMSG
			strcpy( g_pub_tx.reply, "P161" );
        	goto ErrExit;
		}

		num = num +1;	/*	本次记录数  */
		icount = num + sMdm_ac_rel.prt_line;	/* 打印起始行数 */

		if ( icount % BANK_BOOK_LENG == 0 )
		{
			break;		/* 送到存折满页就OK */
		}
	}

	Mdm_unprt_hst_Clo_Sel();
	fclose( fp );

	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", num );
	set_zd_data( "0220", tmpstr );

	num = sMdm_ac_rel.prt_line + 1;
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", num );
	set_zd_data( "0640", tmpstr );

	set_zd_data( "0370", cAcno );

	set_zd_data( "0110", "1" );

	/*	更改介质账号对照表中的打印行数 */
	sMdm_ac_rel.prt_line = icount;

	ret = Mdm_ac_rel_Upd_Upd( sMdm_ac_rel, g_pub_tx.reply  );
	if ( ret )
	{
        sprintf( acErrMsg, "------>修改介质账号对照表错[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	ret = Mdm_ac_rel_Clo_Upd( );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf( acErrMsg, "----->补登存折成功!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
	Mdm_unprt_hst_Clo_Sel();
	Mdm_ac_rel_Clo_Upd();
	fclose( fp );
	sprintf(acErrMsg,"----->补登存折失败[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

