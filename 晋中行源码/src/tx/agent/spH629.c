/**************************************************************
* 文 件 名: spH629.c
* 功能描述：代理编号信息管理
*
* 作    者: jack
* 完成日期：2004年3月28日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "cif_id_code_rel_c.h"
spH629()
{
	int ret = 0;
	char proc_flag[2];
	char tbwhere[128];

	char cUnit_no[5];
	char cItem_no[26];
	char cAc_no[20];
	char cRemark[51];
	char cBat_flag[2];
	char cPswd[7];
	char cName[51];

	long lCif_no = 0;

	struct ag_peritemmap_c sAg_peritemmap;	  /*代理编号信息控制表*/
	struct ag_unitinfo_c sUnitInfo;
	struct cif_id_code_rel_c sCif_id_code_rel;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
	memset( &sCif_id_code_rel, 0x00, sizeof(sCif_id_code_rel) );

	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cAc_no,0x0,sizeof(cAc_no));
	memset(cRemark,0x0,sizeof(cRemark));
	memset(cBat_flag,0x0,sizeof(cBat_flag));
	memset( cPswd, 0x0, sizeof(cPswd) );
	memset( cName, 0x0, sizeof(cName) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0630",cItem_no);
	pub_base_ltrim_zero( cItem_no );
	
	get_zd_data("0920",cUnit_no);
	get_zd_data("0310",cAc_no);
	pub_base_old_acno( cAc_no );
	get_zd_data("0720",cBat_flag);
	get_zd_data("0260",cRemark);
	get_zd_data( "0790", cPswd );
	get_zd_data( "0270", cName );

	memset( &sUnitInfo, 0x0, sizeof(sUnitInfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sUnitInfo, \
			"unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "查询单位信息错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	if ( proc_flag[0] != '2' && sUnitInfo.opr_attri[0] == '0' )	/*---代收---*/
	{
		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", cAc_no );
		if ( ret )
		{
			sprintf(acErrMsg,"查询[%s]表mdm_ac_rel错[%d]", cAc_no, ret);
			WRITEMSG
			goto ErrExit;
		}

		/*-----------------------------------*
		pub_base_EnDes( g_pub_tx.tx_date, "", cPswd );
		if ( strcmp( sMdm_ac_rel.draw_pwd, cPswd ) )
		{
			strcpy ( g_pub_tx.reply, "F004" );
			sprintf( acErrMsg, "个人支取密码密码错!!!" );
			WRITEMSG
			goto ErrExit;
		}
		*------------------------------------*/
	}

	if (proc_flag[0] == '2')	/*--- 删除 ---*/
	{
		ret=sql_count("pay_agent","unit_no='%s' and item_no='%s'",cUnit_no, cItem_no);
		 if(ret>0)
		 {
			sprintf(acErrMsg,"该个人帐号已经在代收付表里了,不允许修改删除[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T069");
			goto ErrExit;		   
		 }
		 else
		 {
				sprintf(tbwhere,"unit_no = '%s' and item_no = '%s'", \
				cUnit_no, cItem_no);
				ret = deldb("ag_peritemmap",tbwhere);
				if (ret != 0)
				{
					sprintf(acErrMsg,"删除代理编号信息记录错[%s]",cUnit_no);
					WRITEMSG
					strcpy(g_pub_tx.reply,"H008");
					goto ErrExit;
				}
		}
	}
	else if (proc_flag[0] == '1')	/*--- 修改 ---*/
	{
		 ret=sql_count("pay_agent","unit_no='%s' and item_no='%s'",cUnit_no, cItem_no);
		 if(ret>0)
		 {
			sprintf(acErrMsg,"该个人帐号已经在代收付表里了,不允许修改删除[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T069");
			goto ErrExit;		   
		 }
		 else
		 {
			ret = Ag_peritemmap_Dec_Upd( g_pub_tx.reply , "unit_no = '%s' \
				and item_no = '%s'",cUnit_no, cItem_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"修改代理编号信息错[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}

			ret = Ag_peritemmap_Fet_Upd( &sAg_peritemmap, g_pub_tx.reply );
			if (ret != 0)
			{
				sprintf(acErrMsg,"修改代理编号信息错2[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}

			strcpy(sAg_peritemmap.unit_no,cUnit_no);
			strcpy(sAg_peritemmap.ac_no,cAc_no);
			strcpy(sAg_peritemmap.remark,cRemark);
			strcpy(sAg_peritemmap.bat_flag,cBat_flag);
			strcpy(sAg_peritemmap.name, cName);

			ret = Ag_peritemmap_Upd_Upd( sAg_peritemmap, g_pub_tx.reply );
			if (ret != 0)
			{
				sprintf(acErrMsg,"修改代理编号信息错3[%s]",cUnit_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"H008");
				goto ErrExit;
			}
			ret = Ag_peritemmap_Clo_Upd( );
		}
	}
	else if (proc_flag[0] == '0')
	{
		strcpy(sAg_peritemmap.unit_no, cUnit_no);
		strcpy(sAg_peritemmap.item_no, cItem_no);
		strcpy(sAg_peritemmap.ac_no, cAc_no);
		strcpy(sAg_peritemmap.remark, cRemark);
		strcpy(sAg_peritemmap.bat_flag, cBat_flag);

		sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
		sAg_peritemmap.opn_date = g_pub_tx.tx_date;
		strcpy(sAg_peritemmap.opn_tel,g_pub_tx.tel);

		sAg_peritemmap.cls_date = 0;
		strcpy(sAg_peritemmap.cls_tel, "xxxx");

		strcpy(sAg_peritemmap.stat,"0");

		ret = pub_base_acgetname( cAc_no, &lCif_no, sAg_peritemmap.name);
		if ( ret )
		{
			sprintf( acErrMsg,"该账户[%s]对应的户名不存在", cAc_no );
			WRITEMSG
			goto ErrExit;
		}

		ret = Cif_id_code_rel_Sel( g_pub_tx.reply, &sCif_id_code_rel, \
			"cif_no = %ld", lCif_no );
		if (ret != 0)
		{
			sprintf( acErrMsg, "查找该客户[%ld]的证件与客户号对照 \
					关系出错",lCif_no );
			WRITEMSG
			goto ErrExit;
		}

		strcpy( sAg_peritemmap.id_type, sCif_id_code_rel.id_type );
		strcpy( sAg_peritemmap.id_no, sCif_id_code_rel.id_no );

		ret = Ag_peritemmap_Ins(sAg_peritemmap,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"插入代理编号信息表错[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"处理类型错[%s]",proc_flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H006");
		goto ErrExit;
	}

	set_zd_data( "0270", sAg_peritemmap.name );

	/*** 输出变量 ***/
	set_zd_data("0920",cUnit_no);
OkExit:
	ret = Ag_peritemmap_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"维护代理编号信息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_peritemmap_Clo_Upd( );
	sprintf(acErrMsg,"维护代理编号信息失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
