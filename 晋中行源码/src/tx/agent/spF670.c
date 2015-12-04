/*************************************************
* 文 件 名: spF670.c
* 功能描述：自动开户补开介质(暂时只允许补开介质)
*
* 作    者: jack
* 完成日期：2004年06月29日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"

int spF670()
{
	int ret = 0;

	char cAcno[21];
	char cRctp[5];
	char cRcno[17];
	char cur_name[11];  /**币种**/

	char filename[64];
	FILE *fp;

	char cDraw_type[21];
	char cSttl_type[11];
	char cBrf[16];

	struct mdm_ac_rel_c sMdm_ac_rel;
	
	struct dd_mst_c sDd_mst;
	struct dd_mst_hst_c sDd_mst_hst;
	struct dd_parm_c sDd_parm;
	
	struct td_mst_c sTd_mst;
	struct td_mst_hst_c sTd_mst_hst;
	struct td_parm_c sTd_parm;
	
	struct com_branch_c sCom_branch;

	/*** 初始化全局变量***/
	pub_base_sysinit();

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cRcno, 0x0, sizeof(cRcno) );
	memset( cRctp, 0x0, sizeof(cRctp) );
	memset( cur_name, 0x0, sizeof(cur_name) );

	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	get_zd_data( "1161", cRctp );
	get_zd_data( "1168", cRcno );
	
	memset( cBrf, 0x0, sizeof(cBrf) );
	strcpy( cBrf, "开户" );
	
	sprintf( acErrMsg,"----->输入的账号为[%s]!!", cAcno );
	WRITEMSG

	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
		"ac_no = '%s'", cAcno );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "L015" );
		sprintf( acErrMsg,"----->读取介质与账户关系信息异常[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	pub_base_strpack( sMdm_ac_rel.note_no );
	pub_base_strpack( sMdm_ac_rel.mdm_code );
	if ( strcmp( sMdm_ac_rel.mdm_code, "0000" ) ||\
		strlen(sMdm_ac_rel.note_no) ) 
	{
		strcpy( g_pub_tx.reply, "H030" );
		sprintf( acErrMsg,"----->该账户已经对应了该类介质,不能增开!!" );
		WRITEMSG
		goto ErrExit;
	}

	if ( strcmp( cRctp, "0010" ) == 0 )
	{
		strcpy( cRctp, "010" );	/*存折*/
	}
	else if ( strcmp( cRctp, "0011" ) == 0 )
	{
		strcpy( cRctp, "011" );	/*零整存折*/
	}
	else if ( strcmp( cRctp, "0013" ) == 0 )
	{
		strcpy( cRctp, "013" );	/*整整定期存单*/
	}
	else if ( strcmp( cRctp, "0015" ) == 0 )
	{
		strcpy( cRctp, "015" );	/*定活两便存单*/
	}
	else if ( strcmp( cRctp, "0016" ) == 0 )
	{
		strcpy( cRctp, "016" );	/*定期一本通*/
	}

	ret = pub_com_NoteUse( 0, 0, cRctp, cRcno, cRcno, g_pub_tx.tel);
	if ( ret )
	{
		sprintf(acErrMsg,"--------->销柜员凭证错误!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mdm_ac_rel_Dec_Upd( g_pub_tx.reply , \
			"ac_no = '%s'", cAcno );
	if ( ret )
	{
		sprintf(acErrMsg,"----->执行Mdm_ac_rel_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	ret = Mdm_ac_rel_Fet_Upd( &sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"----->执行Mdm_ac_rel_Fet_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	strcpy( sMdm_ac_rel.note_no, cRcno );
	strcpy( sMdm_ac_rel.mdm_code, "0010" );

	ret = Mdm_ac_rel_Upd_Upd( sMdm_ac_rel, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf(acErrMsg,"----->执行Mdm_ac_rel_Upd_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd( );
	
	memset( cDraw_type, 0x0, sizeof(cDraw_type) );
	if(sMdm_ac_rel.draw_pwd_yn[0]=='Y' && \
			sMdm_ac_rel.draw_id_yn[0]=='N')
	{
		strcpy(cDraw_type,"【凭密支取】");
	}
	else if(sMdm_ac_rel.draw_pwd_yn[0]=='N' && \
			sMdm_ac_rel.draw_id_yn[0]=='Y')
	{
		strcpy(cDraw_type,"【证件支取】");
	}
	else if( sMdm_ac_rel.draw_pwd_yn[0]=='Y' && \
			sMdm_ac_rel.draw_id_yn[0]=='Y')
	{
		strcpy(cDraw_type,"【密码兼证件支取】");
	}
	else 
	{
		strcpy(cDraw_type,"");
	}	
	
		
	memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
	ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, \
		"br_no = '%s'", sMdm_ac_rel.opn_br_no );
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		strcpy( g_pub_tx.reply, "P159" );
		sprintf( acErrMsg,"打开写存折信息文件错!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	if ( strcmp( cRctp, "010" ) == 0 )	/*活期存折*/
	{
		memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )      
		{               
			strcpy( g_pub_tx.reply, "B114" );
			sprintf( acErrMsg,"----->读取活期主文件信息异常[%d]", ret );
			WRITEMSG
			fclose( fp );
			goto ErrExit;
		}               

		memset( &sDd_mst_hst, 0x0, sizeof(sDd_mst_hst) );
		ret = Dd_mst_hst_Sel( g_pub_tx.reply, &sDd_mst_hst, \
			"ac_id = %ld and hst_cnt = 1", sMdm_ac_rel.ac_id );
		if ( ret )      
		{
			strcpy( g_pub_tx.reply, "B114" );
			sprintf( acErrMsg, "---->读取活期明细中开户信息异常[%d]", ret );
			WRITEMSG
			fclose( fp );
			goto ErrExit;
		}
		
		if ( sDd_mst.opn_date != sDd_mst_hst.tx_date )	/*--主文件开户日期与明细初笔交易日期--*/
		{
			sDd_mst_hst.tx_amt = 0.00;
			sDd_mst_hst.bal = sDd_mst.bal;
			memset( cBrf, 0x0, sizeof(cBrf) );
			strcpy( cBrf, "补发" );
			sDd_mst.opn_date = g_pub_tx.tx_date;
		}

		memset( &sDd_parm, 0x0, sizeof(sDd_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply, &sDd_parm, \
			"prdt_no = '%s'", sDd_mst.prdt_no );
		if ( ret )      
		{               
			strcpy( g_pub_tx.reply, "A020" );
			sprintf( acErrMsg, "---->读取活期明细中开户信息异常[%d]", ret );
			WRITEMSG
			fclose( fp );
			goto ErrExit;
		}

		pub_base_strpack( sDd_mst.name );
		pub_base_strpack( sDd_parm.title );		

		memset( cSttl_type, 0x0, sizeof(cSttl_type) );
		if ( sDd_mst.sttl_type[0] == 'D' )
		{               
			strcpy( cSttl_type, "个人结算户" );
		}               
		if ( sDd_mst.sttl_type[0] == 'S' )
		{               
			strcpy( cSttl_type, "个人储蓄户" );
		}               
		/***返回币种和利率  add by wanglei 20061023***/
		pub_base_dic_show(cur_name,"cur_name",sDd_parm.cur_no);

                fprintf( fp, "CZFY%s|%s|%ld|%s|%ld|%s|%s|%s|%lf|%s|%s|%lf|%s|%lf|\n", \
			cAcno, sDd_mst.name, sDd_mst.cif_no, cSttl_type, \
			sDd_mst.opn_date, cDraw_type, sDd_parm.title, \
			cBrf, sDd_mst_hst.tx_amt, sDd_mst_hst.tel, sCom_branch.br_tele, sDd_mst_hst.bal, cur_name, sDd_mst.rate );
	}

	fclose( fp );

	set_zd_data( DC_FILE_SND, "1" );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg,"批量开户补开介质成功!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	Mdm_ac_rel_Clo_Upd( );
	sprintf(acErrMsg,"批量开户补开介质失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
