/*************************************************
* 文 件 名: spF060.c
* 功能描述：自助终端查询活期余额
*
* 作    者: jack
* 完成日期：2003年6月8日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spF060()
{

        int ret;
	char cAcno[20];
	char cPass[7];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cPass, 0x0, sizeof(cPass) );

	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x00, sizeof(sDd_mst) );

	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	get_zd_data( "0790", cPass );

    if ( pub_base_sysinit() )
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    sprintf( acErrMsg,"-------------->被查询的账号为[%s]", cAcno );
    WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
			"ac_no = '%s'", cAcno );
	if ( ret )
	{
        sprintf( acErrMsg, "查询账号介质对照表出错[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	pub_base_strpack( sMdm_ac_rel.note_sts );

	if ( sMdm_ac_rel.note_sts[0] != '0' )
	{
		strcpy( g_pub_tx.reply, "D140" );
        sprintf( acErrMsg, "凭证状态不正常[%s]", sMdm_ac_rel.note_sts );
        WRITEMSG
        goto ErrExit;
	}

	pub_base_EnDes( g_pub_tx.tx_date, "", cPass );

	if ( strcmp( cPass, sMdm_ac_rel.draw_pwd ) )
	{
		strcpy( g_pub_tx.reply, "M005" );
        sprintf( acErrMsg, "------>密码输入错!!!" );
        WRITEMSG
        goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
	if ( ret )
	{
        sprintf( acErrMsg, "------>查询活期主文件错[%d]!!!", ret );
        WRITEMSG
        goto ErrExit;
	}

	set_zd_data( "0370", cAcno );
	set_zd_double( "0420", sDd_mst.bal );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"----->查询活期余额成功!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

