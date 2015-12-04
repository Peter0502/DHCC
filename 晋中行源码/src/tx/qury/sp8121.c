/***************************************************************
* 文 件 名: sp8121.c
* 功能描述：每日存款实时查询
* 作    者: rob
* 完成日期：2003年09月04日
*
* 修改记录：
*    日    期: 2003年10月25日
*    修 改 人: lance
*    修改内容: 添加条件（按机构查询）
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "dc_acc_rel_c.h"

sp8121( )
{
	char filename[100];
	char tmpname[512],tmpstr[150];
	double tmp_bal,all_bal,bal=0.00 ;
	int ret=0;
	int cnt;
	FILE *fp;

	struct com_item_c	g_com_item;
	struct dc_acc_rel_c	g_dc_acc_rel;

	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&g_dc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*查询段开始*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

    sprintf(acErrMsg,"RRRRRR交易机构=[%s]!",g_pub_tx.tx_br_no);
	WRITEMSG

	/**显示列标题**/
 	fprintf(fp,"~科 目 号|@科目名称|$当前余额|\n");

	ret = Com_item_Dec_Sel( g_pub_tx.reply , "in_ind='D' and acc_lvl='1' order by acc_no" );
	if( ret )
	{
		sprintf(acErrMsg,"游标定义失败!");
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{	
		cnt=0;
		tmp_bal=0.00;
		all_bal=0.00;

		ret = Com_item_Fet_Sel( &g_com_item,g_pub_tx.reply );
		if( ret==100 ) 
		{
			fprintf(fp,"%s|%s|%.2lf|\n", "【 - - - - 】" , "【 汇    总 】" , bal );
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找失败!");
			WRITEMSG
			goto ErrExit;
		}

		ret = Dc_acc_rel_Dec_Sel( g_pub_tx.reply, "acc_hrt[1,3]='%s' order by acc_hrt",g_com_item.acc_no );
		if( ret )
		{
			sprintf(acErrMsg,"查找失败!");
			WRITEMSG
			goto ErrExit;
		}

		while(1)
		{
			ret = Dc_acc_rel_Fet_Sel( &g_dc_acc_rel,g_pub_tx.reply );
			if( ret==100 ) break;
			else if( ret )
			{
				sprintf(acErrMsg,"查找失败!");
				WRITEMSG
				goto ErrExit;
			}
	
			ret = Dd_parm_Dec_Sel( g_pub_tx.reply, " dc_code='%s' ", \
									g_dc_acc_rel.dc_code );
			if( ret )
			{
				sprintf(acErrMsg,"查找失败!");
				WRITEMSG
				goto ErrExit;
			}

vtcp_log("commmm--------dc_code='%s'",g_dc_acc_rel.dc_code);
			while(1)
			{
				ret = Dd_parm_Fet_Sel( &g_dd_parm, g_pub_tx.reply );
				if( ret==100 ) break ;
				else if( ret )
				{
					sprintf(acErrMsg,"查找失败!");
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_sum_double( "dd_mst","bal",&tmp_bal,"prdt_no='%s' and opn_br_no='%s' ", \
									  g_dd_parm.prdt_no,g_pub_tx.tx_br_no );
				if( ret<0 )
				{
					sprintf(acErrMsg,"查找失败!");
					WRITEMSG
					goto ErrExit;
				}	
				all_bal+=tmp_bal;
				bal+=tmp_bal;
				cnt++ ;
			}

			Dd_parm_Clo_Sel( );

			ret = Td_parm_Dec_Sel( g_pub_tx.reply, " dc_code='%s' ", g_dc_acc_rel.dc_code );
			if( ret )
			{
				sprintf(acErrMsg,"查找失败!");
				WRITEMSG
				goto ErrExit;
			}

			while(1)
			{
				ret = Td_parm_Fet_Sel( &g_td_parm, g_pub_tx.reply );
				if( ret==100 ) break ;
				else if( ret )
				{
					sprintf(acErrMsg,"查找失败!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				ret = sql_sum_double( "td_mst","bal",&tmp_bal,"prdt_no='%s' and opn_br_no='%s' ", \
									  g_td_parm.prdt_no ,g_pub_tx.tx_br_no );
				if( ret<0 )
				{
					sprintf(acErrMsg,"查找失败!");
					WRITEMSG
					goto ErrExit;
				}
				all_bal+=tmp_bal;
				bal+=tmp_bal;
				cnt++ ;
			}
			Td_parm_Clo_Sel( );
		}
		Dc_acc_rel_Clo_Sel( );

		if( cnt )
 			fprintf(fp,"%s|%s|%.2lf|\n", g_com_item.acc_no , 
						g_com_item.acc_name , all_bal );
	}
	Com_item_Clo_Sel( );

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"存款实时查询成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"存款实时查询失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
