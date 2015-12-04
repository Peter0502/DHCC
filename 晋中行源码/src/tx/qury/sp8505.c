/*************************************************
* 文 件 名:  sp8505.c
* 功能描述： 科目余额查询
*
* 作    者:  rob
* 完成日期： 20031219
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define EXTERN
#include "public.h"
#include "gl_mst_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"

int sp8505()
{
	int ret=0;
	int ttlnum=0;
	char br_no[6], acc_hrt[8];
	FILE *fp;
	char cFileName[15];
	char wherelist[1000];
	char tmpstr[500];
	char gl_sub;
	double dr_amt=0.00, cr_amt=0.00;

	struct gl_mst_c sGl_mst;
	struct gl_sub_c sGl_sub;
	struct com_item_c sCom_item;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( br_no, 0x0, sizeof(br_no));
	memset( acc_hrt, 0x0, sizeof(acc_hrt));
	memset( cFileName, 0x0, sizeof(cFileName));
	memset( wherelist, 0x0, sizeof(wherelist));
	memset( tmpstr, 0x0, sizeof(tmpstr));
	memset( &sGl_mst, 0x0, sizeof(sGl_mst));
	memset( &sGl_sub, 0x0, sizeof(sGl_sub));
	memset( &sCom_item, 0x0, sizeof(sCom_item));

	pub_base_sysinit();

	get_zd_data("0910", br_no);
	get_zd_data("0640", acc_hrt);

	ret = pub_base_AllDwnFilName( cFileName ); 
	if (ret != 0) 
	{ 
		sprintf( acErrMsg, "生成文件名错错误!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"D126"); 
		goto ErrExit; 
	} 
	
	fp = fopen( cFileName, "w" ); 
	if( fp==NULL ) 
	{ 
		sprintf( acErrMsg, "临时文件错误!" ); 
		WRITEMSG 
		strcpy( g_pub_tx.reply, "B080" ); 
		goto ErrExit; 
	}

	fprintf(fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
		"机构号", "科目号", "借方余额", "贷方余额", "本日借方发生额",
		"本日贷方发生额", "月初借方余额", "月初贷方余额", 
		"本月借方发生额", "本月贷方发生额", "年初借方余额",
		"年初贷方余额", "本年借方发生额", "本年贷方发生额");

	sprintf(wherelist, "br_no='%s'", br_no);

	if (strcmp(acc_hrt, "") != 0 )
	{
		/***
		strcat(wherelist, "and acc_hrt in (");
		ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_no='%s'",
			acc_hrt);
		if ( ret )
		{
			sprintf(acErrMsg, "-------查询科目表异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		if ( sCom_item.sub_acc_yn[0] == 'N' )
		{
			strcat(wherelist, "'%s')", sCom_item.acc_hrt);
		}
		else
		{
			ret = Com_item_Dec_Sel(g_pub_tx.reply, 
		}
		***/
		sprintf(tmpstr, " and acc_hrt in ( select acc_hrt from \
			com_item where ( up_acc_hrt in (select acc_hrt from \
			com_item where acc_hrt in (select acc_hrt from com_item \
			where up_acc_hrt in (select acc_hrt from com_item where \
			acc_no='%s' ))) or ( up_acc_hrt in ( select acc_hrt \
		   	from com_item where acc_no='%s' )) or ( acc_no='%s' ) ) )", 
			acc_hrt, acc_hrt, acc_hrt);
		strcat(wherelist, tmpstr);
	}
	strcat(wherelist, " order by acc_hrt");

	sprintf(acErrMsg, "-------wherelist = [%s]", wherelist);
	WRITEMSG

	if ( strcmp(br_no, "61000") == 0 )
	{
		gl_sub = 'N';
	}

	if ( gl_sub == 'N' )
	{
		ret = Gl_mst_Dec_Sel(g_pub_tx.reply, wherelist);
		if ( ret )
		{
			sprintf(acErrMsg, "DECARE FOR gl_mst ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		ret = Gl_sub_Dec_Sel(g_pub_tx.reply, wherelist);
		if ( ret )
		{
			sprintf(acErrMsg, "DECARE FOR gl_sub ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	while(1)
	{
		memset(&sGl_mst, 0x0, sizeof(sGl_mst));
		memset(&sGl_sub, 0x0, sizeof(sGl_sub));
		if ( gl_sub == 'N' )
		{
			ret = Gl_mst_Fet_Sel(&sGl_mst, g_pub_tx.reply);
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "FETCH FOR gl_mst ERROR [%d]", ret);
				WRITEMSG
				Gl_mst_Clo_Sel();
				goto ErrExit;
			}
		}
		else
		{
			ret = Gl_sub_Fet_Sel(&sGl_sub, g_pub_tx.reply);
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "FETCH FOR gl_sub ERROR [%d]", ret);
				WRITEMSG
				Gl_sub_Clo_Sel();
				goto ErrExit;
			}
		}

		if ( gl_sub == 'N' )
		{
			strcpy(acc_hrt, sGl_mst.acc_hrt);
		}
		else
		{
			strcpy(acc_hrt, sGl_sub.acc_hrt);
		}

		memset(&sCom_item, 0x0, sizeof(sCom_item));
		ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_hrt='%s'", 
			acc_hrt);
		if ( ret )
		{
			sprintf(acErrMsg, "查询公共科目表异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		/* 本日借方发生额 */
		if ( gl_sub == 'N' )
		{
		dr_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &dr_amt, "sts='0' and \
			dc_ind='1' and acc_hrt in ( select acc_hrt from \
			com_item where ( up_acc_hrt in (select acc_hrt from \
			com_item where acc_hrt in (select acc_hrt from com_item \
			where up_acc_hrt in (select acc_hrt from com_item where \
			acc_no='%s' ))) or ( up_acc_hrt in ( select acc_hrt \
			from com_item where acc_no='%s' )) or ( acc_no='%s' )))",
			sCom_item.acc_no, sCom_item.acc_no, sCom_item.acc_no);
		if ( ret )
		{
			sprintf(acErrMsg, "统计本日借方发生额异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		/* 本日贷方发生额 */
		cr_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &cr_amt, "sts='0' and \
			dc_ind='2' and acc_hrt in ( select acc_hrt from \
			com_item where ( up_acc_hrt in (select acc_hrt from \
			com_item where acc_hrt in (select acc_hrt from com_item \
			where up_acc_hrt in (select acc_hrt from com_item where \
			acc_no='%s' ))) or ( up_acc_hrt in ( select acc_hrt \
			from com_item where acc_no='%s' )) or ( acc_no='%s' )))",
			sCom_item.acc_no, sCom_item.acc_no, sCom_item.acc_no);
		if ( ret )
		{
			sprintf(acErrMsg, "统计本日贷方发生额异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		}
		else
		{
		dr_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &dr_amt, "sts='0' and \
			dc_ind='1' and acc_hrt in ( select acc_hrt from \
			com_item where ( up_acc_hrt in (select acc_hrt from \
			com_item where acc_hrt in (select acc_hrt from com_item \
			where up_acc_hrt in (select acc_hrt from com_item where \
			acc_no='%s' ))) or ( up_acc_hrt in ( select acc_hrt \
			from com_item where acc_no='%s' )) or ( acc_no='%s' ))) \
			and tx_tx_br_no in (select br_no from com_branch where \
			up_br_no='%s' or br_no='%s')",
			sCom_item.acc_no, sCom_item.acc_no, sCom_item.acc_no,
			br_no, br_no);
		if ( ret )
		{
			sprintf(acErrMsg, "统计本日借方发生额异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		/* 本日贷方发生额 */
		cr_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &cr_amt, "sts='0' and \
			dc_ind='2' and acc_hrt in ( select acc_hrt from \
			com_item where ( up_acc_hrt in (select acc_hrt from \
			com_item where acc_hrt in (select acc_hrt from com_item \
			where up_acc_hrt in (select acc_hrt from com_item where \
			acc_no='%s' ))) or ( up_acc_hrt in ( select acc_hrt \
			from com_item where acc_no='%s' )) or ( acc_no='%s' ))) \
			and tx_tx_br_no in (select br_no from com_branch where \
			up_br_no='%s' or br_no='%s')",
			sCom_item.acc_no, sCom_item.acc_no, sCom_item.acc_no,
			br_no, br_no);
		if ( ret )
		{
			sprintf(acErrMsg, "统计本日贷方发生额异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		}

		ttlnum++;
		if ( gl_sub == 'N' )
		{
			fprintf(fp, "%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", 
			sGl_mst.br_no, sCom_item.acc_no, sGl_mst.dr_bal + dr_amt,
			sGl_mst.cr_bal + cr_amt, dr_amt, cr_amt, sGl_mst.mdr_bal,
			sGl_mst.mcr_bal, sGl_mst.mdr_bal + dr_amt,
			sGl_mst.mcr_bal + cr_amt, sGl_mst.ydr_bal, sGl_mst.ycr_bal,
			sGl_mst.ydr_amt + dr_amt, sGl_mst.ycr_amt + cr_amt);
		}
		else
		{
			fprintf(fp, "%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", 
			sGl_sub.br_no, sCom_item.acc_no, sGl_sub.dr_bal + dr_amt,
			sGl_sub.cr_bal + cr_amt, dr_amt, cr_amt, sGl_sub.mdr_bal,
			sGl_sub.mcr_bal, sGl_sub.mdr_bal + dr_amt,
			sGl_sub.mcr_bal + cr_amt, sGl_sub.ydr_bal, sGl_sub.ycr_bal,
			sGl_sub.ydr_amt + dr_amt, sGl_sub.ycr_amt + cr_amt);
		}
	}

	if ( gl_sub == 'N' )
	{
		Gl_mst_Clo_Sel();
	}
	else
	{
		Gl_sub_Clo_Sel();
	}

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
