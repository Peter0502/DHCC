/***************************************************************
* 文 件 名: spD242.c
* 功能描述: 网点报单打印(自动提入)
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

spD242()
{
    int ret;
	int flag;
    char cFileName[50];
    FILE *fp;
    int beg_no,end_no;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

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

	/***** test *
	if (strcmp(com_tx_br_rel_c.up_br_no, g_pub_tx.tx_br_no) == 0)
	{
		sprintf(acErrMsg,"自己提给自己不用打印!!");
		WRITEMSG
		ret = sql_execute("update mo_city set sts='I' where sts='H'");
		if (ret != 0)
		{
			sprintf(acErrMsg,"更新同城登记簿错误!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D171");
			goto ErrExit;
		}
		else
		{
			set_zd_data("0130","自己提给自己则不用打印!!");
		}
		goto OkExit;
	}
	else
	{
    	set_zd_data( DC_FILE_SND, "1" );        * 批量传送标志 *
	}
	*****/


    get_zd_int("0480",&beg_no);
    get_zd_int("0490",&end_no);
    
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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","编号","付款人账号",
            "付款人名称", "付款人行名","收款人账号","收款人名称","收款人行名",
            "金额", "提出类型","打印次数","摘要","备注");

    if (beg_no != 0 && end_no == 0)
	{
		/* 打印单张 */
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"bat_no=%d  and (sts='H' or \
				sts='I')", beg_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
	}
	else if (beg_no == 0 && end_no == 0)
	{
		/* 打印全部 */
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"sts='H'");
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
	}
	else
	{
		ret = Mo_city_Dec_Sel(g_pub_tx.reply, "bat_no >= %ld and \
				bat_no <= %ld and (sts='H' or sts='I')", beg_no, end_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

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


    	if (strcmp(mo_city_c.payee_br_no,g_pub_tx.tx_br_no) != 0)
    	{
			if (beg_no != 0 && end_no == 0)
			{
        		sprintf(acErrMsg,"该票不允许在本网点打印!!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"D149");
        		goto ErrExit;
			}

			sprintf(acErrMsg,"收款人行号:[%s], 交易机构号:[%s]",
						mo_city_c.payee_br_no, g_pub_tx.tx_br_no);
			WRITEMSG
			continue;
    	}

		if ( flag == 0 )
		{
		}

		flag = flag + 1;
        mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;
 
		if (mo_city_c.bill_type[0] == '1')
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "同城", mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '2')
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "内转",mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '3')
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "退票",mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}
		else 
		{
    		fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
					mo_city_c.bat_no,
          			mo_city_c.payer_ac_no, mo_city_c.payer_name,
					mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
					mo_city_c.payee_name,mo_city_c.payee_br_name, 
					mo_city_c.bill_amt, "其他",mo_city_c.pr_cnt,
					mo_city_c.brf, mo_city_c.remark);
		}

    	strcpy(mo_city_c.sts,"I");
    	if (beg_no != 0 && end_no == 0)
		{
			/*更新单张 */
    		ret = sql_execute("update mo_city set sts='I', \
				pr_cnt = pr_cnt + 1 where bat_no=%d  and (sts='H' or \
				sts='I')", mo_city_c.bat_no);
    		if (ret != 0)
			{
			   	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
			   	WRITEMSG
			   	goto ErrExit;
			}
		}
		else if (beg_no == 0 && end_no == 0)
		{
			/* 更新全部 */
			ret = sql_execute("update mo_city set sts='I', \
				pr_cnt = pr_cnt + 1 where bat_no=%d and sts='H'",
				mo_city_c.bat_no);
			if (ret != 0)
			{
			   	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
			   	WRITEMSG
			   	goto ErrExit;
			}
		}
		else
		{
			ret = sql_execute("update mo_city set sts='I' ,\
				pr_cnt = pr_cnt + 1 where bat_no = %ld and \
				(sts='H' or sts='I')", mo_city_c.bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
  
		/**** 
    	ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		*****/
	}
	Mo_city_Clo_Sel();
    fclose(fp);

    set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */

	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "自动提入票据网点打印");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"总行报单打印(自动)处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"总行报单打印(自动)处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

