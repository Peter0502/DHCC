/***************************************************************
* 文 件 名: spD243.c
* 功能描述: 自动提入票据清单打印
*
* 作    者: jack
* 完成日期: 2004年3月31日
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"

spD243()
{
    int ret=0;
	int flag=0;
    char cFileName[50];
	char stat[50];
    FILE *fp;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(stat, 0x0, sizeof(stat));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	/******
	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 and \
			br_no='%s'", g_pub_tx.tx_br_no);	
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"查询交易机构表错误!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg,"交易机构关系表中没有要查的纪录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D168");
		goto ErrExit;
	}
	*****/

	get_zd_data("0910", mo_city_c.tx_br_no);
	get_zd_long("0440", &mo_city_c.tx_date);

    ret = pub_base_AllDwnFilName( cFileName );
    if (ret != 0)
    {
        sprintf( acErrMsg, "生成文件名错错误!!" );
        WRITEMSG
        strcpy(g_pub_tx.reply,"D126");
        goto ErrExit;
    }
 
    sprintf(acErrMsg,"filename=[%s]",cFileName);
    WRITEMSG

    fp = fopen( cFileName, "w" );
    if( fp==NULL )
    {
        sprintf( acErrMsg, "临时文件错误!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B080" );
        goto ErrExit;
    }

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","编号","付款人账号",
            "付款人名称", "付款人行名","收款人账号","收款人名称","收款人行名",
            "金额", "票据状态");

	ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s'",  
		g_pub_tx.tx_br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计交易机构关系表异常!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	else if ( ret )
	{
		/* 本机构是中心 */
		ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s' \
			and br_no='%s'",  g_pub_tx.tx_br_no, mo_city_c.tx_br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "统计交易机构关系表异常!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D101");
			goto ErrExit;
		}
		else if (ret == 0)
		{
			sprintf(acErrMsg, "该网点不是本机构的下级机构!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D185");
			goto ErrExit;
		}

  		ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and \
			tx_br_no='%s' and in_out_ind='2' and \
			trans_mode='2'", mo_city_c.tx_date, mo_city_c.tx_br_no);
		TRACE
	}
	else 
	{
		/* 普通网点 */
		if (strcmp(g_pub_tx.tx_br_no, mo_city_c.tx_br_no) != 0)
		{
			sprintf(acErrMsg, "普通网点只能打印自己的票据!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D257");
			goto ErrExit;
		}
  		ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and \
			(payer_br_no='%s' or payee_br_no='%s') and in_out_ind='2' and \
			trans_mode='2'", mo_city_c.tx_date, mo_city_c.tx_br_no, 
			mo_city_c.tx_br_no);
		TRACE
	}

	/******
  	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and \
		(payer_br_no='%s' or payee_br_no='%s') and in_out_ind='2' and \
		trans_mode='2'", mo_city_c.tx_date, mo_city_c.tx_br_no, 
		mo_city_c.tx_br_no);
	******/
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       	WRITEMSG
       	goto ErrExit;
   	}
  
	flag = 0;
	while(1)
	{
    	ret = Mo_city_Fet_Sel(&mo_city_c,g_pub_tx.reply);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"查询同城登记簿错误!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		else if (ret == 100)
		{
			if (flag == 0)
			{
				sprintf(acErrMsg,"不存在此票据!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
			break;
		}



		flag = flag + 1;
		if (mo_city_c.sts[0] == 'H')
		{
			strcpy(stat, "中心已经录入");
		}
		else if (mo_city_c.sts[0] == 'I')
		{
			strcpy(stat, "网点已经打印");
		}
		else if (mo_city_c.sts[0] == 'O')
		{
			strcpy(stat, "网点已经记账");
		}
		else if (mo_city_c.sts[0] == 'P')
		{
			strcpy(stat, "中心已经通存");
		}
		else if (mo_city_c.sts[0] == 'Q')
		{
			strcpy(stat, "网点已经退票");
		}
		else
		{
			strcpy(stat, "状态不清");
		}
 
    	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|\n",
			mo_city_c.bat_no,
       		mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name, 
			mo_city_c.bill_amt, stat);
	}
	Mo_city_Clo_Sel();
    fclose(fp);

    set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */

	/*** 登记交易流水 
	strcpy(g_pub_tx.brf, "自动提入票据网点打印");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"自动提入票据清单打印处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"自动提入票据清单打印处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

