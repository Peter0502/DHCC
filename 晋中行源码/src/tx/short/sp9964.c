/*************************************************
* 文 件 名: sp9964.c
* 功能描述：根据账号查询出客户姓名(活期户或内部户)
*
* 作    者: jack
* 完成日期：2003年7月15日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9964()
{
	int ret=0;

	char cAcno[20];
	long lCif_no = 0;
	char cName[50];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct in_mst_c sIn_mst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cName, 0x0, sizeof(cName) );

	get_zd_data( "0300", cAcno );
  pub_base_old_acno(cAcno);
	if ( cAcno[0] != '9' )
	{
		ret = pub_base_acgetname( cAcno, &lCif_no, cName );
		if ( ret )
		{
			sprintf( acErrMsg, "查询户名错 !!!" );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data( "0250", cName );
	}
	else
	{
		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", cAcno );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sIn_mst, 0x0, sizeof(sIn_mst) );
		ret = In_mst_Sel( g_pub_tx.reply, &sIn_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "执行In_mst_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data( "0250", sIn_mst.name );
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK !!!! " );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
