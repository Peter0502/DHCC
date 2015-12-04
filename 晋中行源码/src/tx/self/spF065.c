/*************************************************
* 文 件 名:  spF065.c
* 功能描述： 自助终端转帐返回
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

int spF065()
{
	int ret = 0;
	char cAcno[20];
	char cAcno1[20];
	double dTram = 0.00;

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cAcno1, 0x0, sizeof(cAcno1) );
	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x0, sizeof(sDd_mst) );

	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	set_zd_data( "0370", cAcno );

	get_zd_data( "0300", cAcno1 );
	pub_base_old_acno( cAcno1 );
	set_zd_data( "0300", cAcno1 );

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
		"ac_no = '%s'", cAcno );
	if ( ret )
	{
		sprintf(acErrMsg,"----->查询介质账号对帐表出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id = %ld", \
			sMdm_ac_rel.ac_id );
	if ( ret )
	{
		sprintf(acErrMsg,"----->查询活期主文件出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	get_zd_double( "0400", &dTram );
	sDd_mst.bal = sDd_mst.bal - dTram;

	set_zd_double( "0420", sDd_mst.bal );
	set_zd_data( "0250", sDd_mst.name );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"----->填返回信息成功" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"----->填返回信息失败[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

