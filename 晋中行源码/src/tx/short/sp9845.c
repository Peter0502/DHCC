/*************************************************
* 文 件 名:  sp9845.c
* 功能描述： 由委托收款协议编号下传委托清单
*
* 作    者:  jane
* 完成日期： 2003年2月2日 
*
* 修改记录:
* 日   期 :
* 修 改 人:
* 修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_trust_list_c.h"

int sp9845()
{
	struct	mo_trust_list_c	sMoTrustList;	/* 委托清单 */
    	char 	filename[100];
    	int 	ret;
	FILE 	*fp;
   
	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoTrustList.turst_pay_no );	/* 委托收款协议编号 */

	/* 检查委托收款协议是否存在 */
	ret = Mo_trust_list_Dec_Sel( g_pub_tx.reply, "turst_pay_no='%s'", sMoTrustList.turst_pay_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_trust_list_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	while(1)
	{
		ret = Mo_trust_list_Fet_Sel( &sMoTrustList, g_pub_tx.reply );
		if( ret==100 )
		{
			sprintf( acErrMsg, "查询委托清单错，无记录!" );
			WRITEMSG
			break;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Mo_trust_list_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%d|%s|%s|%s|%s|%.2f|\n", \
			sMoTrustList.turst_pay_no, sMoTrustList.payer_seqn, \
			sMoTrustList.payer_ac_no, sMoTrustList.payer_name, \
			sMoTrustList.payer_br_no, sMoTrustList.payer_br_name, \
			sMoTrustList.amt );
	}
	Mo_trust_list_Clo_Sel ();

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
