/***************************************************************
* 文 件 名: spD412.c
* 功能描述：吞卡处理
*
* 作    者: jack
* 完成日期：2004年03月23日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"

spD412()
{
	int ret = 0;
	char cFlag[2];
	char cAc_no[20];
	char cId_type[2];
	char cId_no[21];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct cif_id_code_rel_c sCif_id_code_rel;

	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
	memset( &sCif_id_code_rel, 0x00, sizeof(sCif_id_code_rel) );
	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cAc_no, 0x0, sizeof(cAc_no) );
	memset( cId_type, 0x0, sizeof(cId_type) );
	memset( cId_no, 0x0, sizeof(cId_no) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data( "0700", cFlag );
	get_zd_data( "0370", cAc_no );

	if ( cFlag[0] == '0' )
	{
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply, "ac_no='%s'", cAc_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(g_mdm_ac_rel.coll_sts,"2");

		g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}
	else if ( cFlag[0] == '1' )
	{
		get_zd_data( "0670", cId_type );
		get_zd_data( "0620", cId_no );

		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no = '%s'", cAc_no );
		if ( ret )
		{
			sprintf(acErrMsg,"查询介质账号对照表出错[%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sMdm_ac_rel.coll_sts );
		if ( sMdm_ac_rel.coll_sts[0] != '2' )
		{
			strcpy( g_pub_tx.reply, "A036" );
			sprintf( acErrMsg,"没有您要处理的吞卡记录" );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( cId_type );
		pub_base_strpack( cId_no );
		
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf(acErrMsg,"查询活期主文件表出错[%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply, &sCif_id_code_rel, "cif_no = %ld", sDd_mst.cif_no );
		if ( ret )
		{
			sprintf( acErrMsg, "查询客户号和证件对应关系表出错[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		
		pub_base_strpack( sCif_id_code_rel.id_type );
		pub_base_strpack( sCif_id_code_rel.id_no );
		
		if ( ( strcmp( sCif_id_code_rel.id_type, cId_type) != 0) \
			|| ( strcmp( sCif_id_code_rel.id_no, cId_no) != 0) )
		{
			sprintf(acErrMsg,"证件类型或证件号码错[%s][%s][%s][%s]!!!",cId_type,sCif_id_code_rel.id_type,cId_no,sCif_id_code_rel.id_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "M004" );
			goto ErrExit;
		}

		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply, "ac_no='%s'", cAc_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(g_mdm_ac_rel.coll_sts,"0");

		g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}
	else
	{
			sprintf(acErrMsg,"处理类型错!!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "D999" );
			goto ErrExit;
	}

	/*** 输出变量 ***/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"吞卡处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"吞卡处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
