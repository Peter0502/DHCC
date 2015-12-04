/***************************************************************
* 文 件 名: spD273.c
* 功能描述: 查询书查询
*
* 作    者: jack
* 完成日期: 2004年7月28日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_city_query_c.h"

spD273()
{
    int ret=0;
	char cFileName[50];
	FILE *fp;
	int cnt=0;
	char wherelist[500];
	char tmpstr[80];
	char Note_Type[40];
	char Type[10];

    struct mo_city_query_c sMo_city_query;

    memset(&sMo_city_query, 0x0, sizeof(struct mo_city_query_c));

	memset(cFileName, 0x0, sizeof(cFileName));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(Type, 0x0, sizeof(Type));
	memset(Note_Type, 0x0, sizeof(Note_Type));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    /* get_zd_data("0910", sMo_city_query.rec_br_no); */
	get_zd_data("0670", sMo_city_query.type);
	get_zd_long("0440", &sMo_city_query.tx_date);

	/*******
	sprintf(wherelist, "tx_br_no='%s' ", g_pub_tx.tx_br_no);

	if (strcmp(sMo_city_query.rec_br_no, NULL) != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and rec_br_no='%s' ", sMo_city_query.rec_br_no);
		strcat(wherelist, tmpstr);
	}

	if (strcmp(sMo_city_query.type, NULL) != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and type='%s' ", sMo_city_query.type);
		strcat(wherelist, tmpstr);
	}
	******/


	if (sMo_city_query.type[0] == '1')
	{
		sprintf(wherelist, "rec_br_no = '%s'", g_pub_tx.tx_br_no);
	}
	else
	{
		sprintf(wherelist, "tx_br_no = '%s' ", g_pub_tx.tx_br_no);
	}

	if (sMo_city_query.tx_date > 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and tx_date=%ld ", sMo_city_query.tx_date);
		strcat(wherelist, tmpstr);
	}

	sprintf(acErrMsg, "------->wherelsit = [%s]", wherelist);
	WRITEMSG

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

	sprintf(acErrMsg, "交易机构:[%s]", g_pub_tx.tx_br_no); 
	WRITEMSG

	ret = Mo_city_query_Dec_Sel(g_pub_tx.reply, wherelist);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	fprintf(fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
			"录入日期","流水号","接受机构", "查询类型", "凭证日期",
			"凭证类型", "金额", "付款人账号", "付款人名称", "付款人行名",
			"收款人账号", "收款人名称", "收款人行名", "查询内容");

	while(1)
	{
		ret = Mo_city_query_Fet_Sel(&sMo_city_query, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			if (cnt == 0)
			{
				sprintf(acErrMsg, "没有要打印的查询查复书");
				WRITEMSG
				strcpy(g_pub_tx.reply, "D206");
				goto ErrExit;
			}
			break;
		}

		if (sMo_city_query.type[0] == '1')
			strcpy(Type, "查询");
		else if (sMo_city_query.type[0] == '2')
			strcpy(Type, "查复");
		else if (sMo_city_query.type[0] == '3')
			strcpy(Type, "更正");
		else if (sMo_city_query.type[0] == '4')
			strcpy(Type, "退汇");

		if (sMo_city_query.note_type[0] == '1')
			strcpy(Note_Type, "手工提出");
		else if (sMo_city_query.note_type[0] == '2')
			strcpy(Note_Type, "自动提出");
		else if (sMo_city_query.note_type[0] == '3')
			strcpy(Note_Type, "手工提入");
		else if (sMo_city_query.note_type[0] == '4')
			strcpy(Note_Type, "自动提入");
		
		cnt++;
		fprintf(fp, "%ld|%ld|%s|%s|%ld|%s|%lf|%s|%s|%s|%s|%s|%s|%s|\n",
			sMo_city_query.tx_date, sMo_city_query.tx_no,
			sMo_city_query.rec_br_no, Type, sMo_city_query.note_date,
			Note_Type, sMo_city_query.tx_amt, sMo_city_query.payer_ac_no, 
			sMo_city_query.payer_name, sMo_city_query.payer_br_name,
			sMo_city_query.payee_ac_no, sMo_city_query.payee_name,
			sMo_city_query.payee_br_name, sMo_city_query.content);
	}
	Mo_city_query_Clo_Sel();

	fclose(fp);

	set_zd_data( DC_FILE_SND, "1" );/* 批量传送标志 */


	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "查询书查询");
	strcpy(g_pub_tx.no_show, "1");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询查复台帐管理处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询查复台帐管理处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

