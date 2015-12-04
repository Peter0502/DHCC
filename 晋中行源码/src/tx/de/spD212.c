/***************************************************************
* 文 件 名: spD212.c
* 功能描述: 网点打印报单(手工)
*
* 作    者: jack
* 完成日期: 2004年3月30日
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

spD212()
{
    int ret=0;
    int bat_no=0;
	int flag=0;
    char cFileName[50];
    FILE *fp;

	char bill_name[20];
	char dc_ind[14];
	double amt=0.00, d_amt=0.00, c_amt=0.00;
	double totalamt=0.00;
	int sum=0;
	int totalsum=0;
	int i=0;

	long i_bat_no=0;

    struct mo_city_c mo_city_c;
	struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(bill_name, 0x0, sizeof(bill_name));
	memset(dc_ind, 0x0, sizeof(dc_ind));

	flag = 0;

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    /* get_zd_data("0670",mo_city_c.bill_type); */
	get_zd_long("0530", &i_bat_no);

    strcpy(mo_city_c.in_out_ind,"1");
    strcpy(mo_city_c.tel,g_pub_tx.tel);
    strcpy(mo_city_c.sts,"1");
    mo_city_c.tx_date = g_pub_tx.tx_date;
    
	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 and \
			br_no='%s'",g_pub_tx.tx_br_no);
    if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"查询交易机构关系表错误!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg,"交易机构关系表中没有该纪录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D168");
		goto ErrExit;
	}

	/*********
	if (strcmp(g_pub_tx.tx_br_no,com_tx_br_rel_c.up_br_no) == 0)
	{
		sprintf(acErrMsg,"自己提给自己不用打印!!");
		WRITEMSG
		* 更新这些纪录的状态 *
		ret = sql_execute("update mo_city set sts='2' where tel='%s' and \
			tx_date=%ld and sts='1'", mo_city_c.bill_type, g_pub_tx.tel, 
			g_pub_tx.tx_date);
		if (ret != 0)
		{
			sprintf(acErrMsg,"更新同城登记簿中的纪录错误!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D171");
			goto ErrExit;
		}
		else
		{
			* strcpy(g_pub_tx.reply,"ZZZZ"); *
			set_zd_data("0130","自己提给自己不用打印!!");
		}
		goto OkExit;
	}
	else
	{
	}
	*****/

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

    /* 生成批次号 */
    ret = sql_max_int("mo_city","bat_no","trans_mode='1' and in_out_ind='1'");
    if (ret < 0)
    {
        sprintf(acErrMsg,"查询同城登记簿!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    bat_no = ret + 1;

    /* 检查该批次号是否已存在并且状态为2的 delete by rob at 20050712
    while(1)
    {
        ret = sql_count("mo_city","trans_mode='1' and bat_no=%d and sts='2'",
			bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"统计同城登记簿错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret > 0)
        {
            bat_no += 1;
            continue;
        }
        else 
        {
            break;
        }
    }*/

 	sum = 0;
fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|\n","批次号","付款人账号","金额",
           "票据种类","收款人账号","提出类型","借贷方向","打印次数");

	i = 1;
	flag = 0;
	while(1)
	/* for(i=1;i<=5;i++) */
	{
		if (i_bat_no != 0)
		{
			/* 重打 */
			TRACE
			ret = Mo_city_Dec_Upd(g_pub_tx.reply, "bat_no=%d and sts='2' and \
					bill_type='%d' and in_out_ind='1' and trans_mode='1' and \
					tel='%s'", i_bat_no, i, g_pub_tx.tel);
			if (ret != 0)
			{
				sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			TRACE
    		ret = Mo_city_Dec_Upd(g_pub_tx.reply,"bill_type='%d' and \
					tel='%s' and sts='1' and trans_mode='1'", i, g_pub_tx.tel);
    		if (ret != 0)
    		{
        		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        		WRITEMSG
        		goto ErrExit;
    		}	
		}

    	while(1)
    	{
        	ret = Mo_city_Fet_Upd(&mo_city_c,g_pub_tx.reply);
        	if (ret != 0 && ret != 100)
        	{
            	sprintf(acErrMsg,"查询同城登记簿错误!! [%d]",ret);
            	WRITEMSG
           	 	goto ErrExit;
        	}
        	else if (ret == 100)
        	{
				/**
				if (flag == 0 && i == 6) 
				{
					sprintf(acErrMsg,"没有该类票据!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D160");
					goto ErrExit;
				}
				*****/
            	break;
        	}
 
			flag = flag + 1;
			if (i_bat_no == 0)
			{
        		mo_city_c.bat_no = bat_no;
			}
        	mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;

TRACE
			switch(mo_city_c.bill_type[0])
			{
				case '1':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no,mo_city_c.bill_amt, "同城",
						mo_city_c.payee_ac_no,"提出贷方", "2",mo_city_c.pr_cnt);
					break;
				case '2':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, "市国库税票",
						mo_city_c.payee_ac_no,"提出贷方", "2",mo_city_c.pr_cnt);
					break;
				case '3':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, "县国库税票",
						mo_city_c.payee_ac_no,"提出贷方", "2",mo_city_c.pr_cnt);
					break;
				case '4':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"大同城进帐单", mo_city_c.payee_ac_no,"提出贷方", "2",
						mo_city_c.pr_cnt);
					break;
				case '5':
            		fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"委托收款", mo_city_c.payee_ac_no,"提出贷方","2",
						mo_city_c.pr_cnt);
					break;
				case '6':
					if (mo_city_c.cd_ind[0] == '2')
					{
            			fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",
						mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"其他票据", mo_city_c.payee_ac_no,"提出贷方","2",
						mo_city_c.pr_cnt);
					}
					else
					{
            			fprintf(fp,"%d|%s|%lf|%s|%s|%s|%s|%d|\n",
						mo_city_c.bat_no,
              			mo_city_c.payer_ac_no, mo_city_c.bill_amt, 
						"其他票据", mo_city_c.payee_ac_no,"提出借方","1",
						mo_city_c.pr_cnt);
					}

					break;
				default :
					sprintf(acErrMsg,"票据种类错误!![%s]",mo_city_c.bill_type);
					WRITEMSG
					break;
			}

			sum = sum + 1;
        	strcpy(mo_city_c.sts,"2");
        	ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
    	Mo_city_Clo_Upd();
		i = i + 1;
		if (i > 6)
		{
			if (flag == 0) 
			{
				sprintf(acErrMsg,"没有该类票据!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
			break;
		}
	}


	/* 汇总报单 */

   	fprintf(fp,"%s|\n","汇总");
   	fprintf(fp,"%s|%s|%s|\n","票据种类","张数","金额");
				
	totalamt = 0.00;
	totalsum = 0;
	for(i=1;i<=6;i++)
	{
		/*** delete by rob at 20050711
		if (i_bat_no == 0)
		{
		***/

		if (i_bat_no != 0)
		{
			bat_no = i_bat_no;
		}

			/* add by rob at 20050818** for 提出借方退票 */
			amt = 0.00;
			d_amt = 0.00;
			c_amt = 0.00;
			ret = sql_sum_double("mo_city","bill_amt",&d_amt,"bill_type='%d' \
				and tel='%s' and bat_no=%d  and in_out_ind ='1' and \
				trans_mode='1' and cd_ind='2'", i, g_pub_tx.tel,bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"汇总票据金额错误!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D179");
				goto ErrExit;
			}

			ret = sql_sum_double("mo_city","bill_amt",&c_amt,"bill_type='%d' \
				and tel='%s' and bat_no=%d  and in_out_ind ='1' and \
				trans_mode='1' and cd_ind='1'", i, g_pub_tx.tel,bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"汇总票据金额错误!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D179");
				goto ErrExit;
			}
			amt = d_amt - c_amt;

		/***** delete by rob at 20050711 
			ret = sql_sum_double("mo_city","bill_amt",&amt,"bill_type='%d' \
				and tel='%s' and bat_no=%d  and in_out_ind ='1' and \
				trans_mode='1'", i, g_pub_tx.tel,bat_no);
			if (ret != 0)
			{
				sprintf(acErrMsg,"汇总票据金额错误!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D179");
				goto ErrExit;
			}

		}
		else
		{
			ret = sql_sum_double("mo_city", "bill_amt", &amt, "bat_no=%d and \
				bill_type='%d' and in_out_ind='1' and trans_mode='1'", 
				i_bat_no, i);
			if (ret != 0)
			{
				sprintf(acErrMsg,"汇总票据金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D179");
				goto ErrExit;
			}
		}

		if (i_bat_no == 0)
		{
		*****/

			ret = sql_count("mo_city","bill_type='%d' and tel='%s' and \
					bat_no = %d and in_out_ind='1' and trans_mode='1'", 
					i, g_pub_tx.tel, bat_no);
			if (ret < 0)
			{
				sprintf(acErrMsg,"汇总票据张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D180");
				goto ErrExit;
			}

		/*** delete by rob at 20050711
		}
		else
		{
			ret = sql_count("mo_city", "bill_type='%d' and bat_no=%d and \
					in_out_ind = '1' and trans_mode='1'", i, bat_no);
			if (ret < 0)
			{
				sprintf(acErrMsg,"汇总票据张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D180");
				goto ErrExit;
			}
		}
		********/

		totalsum = ret;

		/***** delete by rob**
		if (pub_base_CompDblVal(amt,0.00) != 1)
		{
			continue;
		}
		****/

		switch(i)
		{
			case 1:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","同城",totalsum,amt);
				break;
			case 2:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","市国库税票",totalsum,amt);
				break;
			case 3:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","县国库税票",totalsum,amt);
				break;
			case 4:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","大同城进帐单",totalsum,amt);
				break;
			case 5:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","委托收款",totalsum,amt);
				break;
			case 6:
				totalamt = totalamt + amt;
           		fprintf(fp,"%s|%ld|%lf|\n","其他票据",totalsum,amt);
				break;
			default:
				sprintf(acErrMsg,"票据种类错误!!");
				WRITEMSG
				goto ErrExit;
		}
	}


		if (pub_base_CompDblVal(totalamt,0.00) == -1)
		{
           	fprintf(fp,"%s|%s|%lf|\n","汇总","提出借方",-totalamt);
		}
		else if (pub_base_CompDblVal(totalamt,0.00) == 1)
		{
           	fprintf(fp,"%s|%s|%lf|\n","汇总","提出贷方",totalamt);
		}

    fclose(fp);

   	set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */
	if (i_bat_no == 0)
	{
		set_zd_long("0530",bat_no);
	}
	else
	{
		set_zd_long("0530",i_bat_no);
	}
	set_zd_long("0480",sum);

	/*** 登记交易流水 ***/
	if (i_bat_no == 0)
	{
		strcpy(g_pub_tx.brf, "手工提出打印");
	}
	else
	{
		strcpy(g_pub_tx.brf, "手工提出重打");
	}
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.ac_seqn = bat_no;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点提出手工打印处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
   	set_zd_data( DC_FILE_SND, "0" );
	sprintf(acErrMsg,"网点提出手工打印处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

