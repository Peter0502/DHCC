/***************************************************************
* 文 件 名: spD283.c
* 功能描述: 同城查询打印
*
* 作    者: jack
* 完成日期: 2005年6月20日
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
#include "com_tx_br_rel_c.h"
#include "com_branch_c.h"

spD283()
{
    int ret;
	int flag;
	char br_no[6];
	char wherelist[500], wherelist1[500];
	char tmp[50];
    char cFileName[50];
    FILE *fp;

	long beg_date,end_date, beg_date1;
	double amt1=0.00, amt2=0.00, amt3=0.00, amt4=0.00, amt5=0.00;
	double dr_amt=0.00, cr_amt=0.00, sum1=0.00, sum2=0.00;

	double t_amt1=0.00, t_amt2=0.00, t_amt3=0.00, t_amt4=0.00;
	double t_sum1=0.00, t_sum2=0.00;

	int num1=0, num2=0, num3=0, num4=0;
	int t_num1=0, t_num2=0, t_num3=0, t_num4=0;


	struct com_tx_br_rel_c sCom_tx_br_rel;
	struct com_branch_c sCom_branch;

	memset(&sCom_tx_br_rel, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&wherelist, 0x0, sizeof(wherelist));
	memset(&wherelist1, 0x0, sizeof(wherelist1));


	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_data("0910", br_no);
	get_zd_long("0440", &beg_date);
	get_zd_long("0450", &end_date);
    
	if (beg_date == 0)
	{
		beg_date = g_pub_tx.tx_date;
		if (end_date == 0)
		{
			end_date = g_pub_tx.tx_date;
		}
	}
	else
	{
		if (end_date == 0)
		{
			end_date = beg_date;
		}
	}
	beg_date1 = beg_date;

	sprintf(acErrMsg, "起始日期: [%ld], 终止日期: [%ld]", beg_date, end_date);
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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
			"网点号", "日期",
			"手工提出金额", "手工提出张数", "自动提出金额",
			"自动提出张数", "提出汇总金额", "提出汇总张数", 
			"手工提入金额", "手工提入张数", "自动提入金额",
			"自动提入张数", "提入汇总金额", "提入汇总张数");

	sprintf(wherelist,"1=1 ");
	if (strcmp(br_no, "") != 0)
	{
		sprintf(tmp, "and br_no = '%s'", br_no);
		strcat(wherelist, tmp);
	}

	strcat(wherelist," order by br_no");

	sprintf(acErrMsg,"wherelist = [%s]", wherelist);
	WRITEMSG

	while(1)
	{
    	ret = Com_branch_Dec_Sel(g_pub_tx.reply,wherelist);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}

		flag = 0;

		t_amt1 = 0.00;
		t_amt2 = 0.00;
		t_amt3 = 0.00;
		t_amt4 = 0.00;
		t_sum1 = 0.00;
		t_sum2 = 0.00;
		t_num1 = 0;
		t_num2 = 0;
		t_num3 = 0;
		t_num4 = 0;

   	 	while(1)
    	{
        	ret = Com_branch_Fet_Sel(&sCom_branch, g_pub_tx.reply);
        	if (ret != 0 && ret != 100)
        	{
            	sprintf(acErrMsg,"查询同城登记簿错误!! [%d]",ret);
            	WRITEMSG
				strcpy(g_pub_tx.reply,"D172");
            	goto ErrExit;
        	}
        	else if (ret == 100)
        	{
				if (flag == 0) 
				{
					sprintf(acErrMsg,"没有该机构号!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D173");
					goto ErrExit;
				}
            	break;
        	}
 

			if (strcmp(br_no, "") != 0)
			{
				ret = Com_tx_br_rel_Sel(g_pub_tx.reply, &sCom_tx_br_rel,
					"rel_type = 3 and br_no = '%s' and up_br_no = '%s'",
					br_no, g_pub_tx.tx_br_no);
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
					if (strcmp(br_no, g_pub_tx.tx_br_no) != 0)
					{
						sprintf(acErrMsg,"网点只能打印自己网点的票据或中\
									心打印其下级机构的票据");
						WRITEMSG
						strcpy(g_pub_tx.reply, "D185");
						goto ErrExit;
					}
				}
			}
			else
			{
				ret = Com_tx_br_rel_Sel(g_pub_tx.reply, &sCom_tx_br_rel,
					"rel_type = 3 and br_no = '%s' and up_br_no = '%s'",
					sCom_branch.br_no, g_pub_tx.tx_br_no);
    			if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg,"查询交易机构关系表错误!! [%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D167");
					goto ErrExit;
				}
				else if (ret == 100)
				{
					sprintf(acErrMsg,"网点 [%s] 不是 网点 [%s] 的上级机构!!",
						g_pub_tx.tx_br_no, sCom_branch.br_no);
					WRITEMSG
					continue;
				}
			}

			flag++;
			sum1 = 0.00;
			sum2 = 0.00;
			amt1 = 0.00;
			amt2 = 0.00;
			amt3 = 0.00;
			amt4 = 0.00;
			cr_amt = 0.00;
			dr_amt = 0.00;

			num1 = 0;
			num2 = 0;
			num3 = 0;
			num4 = 0;

			/* 手工提出贷方金额统计 */
			ret = sql_sum_double("mo_city", "bill_amt", &cr_amt,
				"in_out_ind = '1' and cd_ind = '2' and trans_mode = '1' and \
				sts = '3' and tx_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg," 统计手工提出贷方金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			/**
			ret = sql_count("mo_city", "in_out_ind = '1' and cd_ind = '2' \
				and trans_mode = '1' and sts = '3' and tx_br_no = '%s' and \
				tx_date = %ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计提出张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			num1 = ret;
			***/

			/* 手工提出借方金额统计 */
			ret = sql_sum_double("mo_city", "bill_amt", &dr_amt,
				"in_out_ind = '1' and cd_ind = '1' and trans_mode = '1' and \
				sts = '3' and tx_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg," 统计手工提出贷方金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			amt1 = (cr_amt - dr_amt);
			sprintf(acErrMsg, "手工提出统计金额为: [%lf]", amt1);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind = '1' \
				and trans_mode = '1' and sts = '3' and tx_br_no = '%s' and \
				tx_date = %ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计提出张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			num1 = ret;

			sprintf(acErrMsg, "手工提出统计金额为: [%ld]", num1);
			WRITEMSG

			/* 自动提出金额统计 */
			ret = sql_sum_double("mo_city", "bill_amt", &amt2, 
				"in_out_ind = '1' and cd_ind = '2' and trans_mode = '2' and \
				sts = '7' and tx_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "统计自动提出金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "自动提出统计金额为: [%lf]", amt2);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind = '1' and cd_ind = '2' \
				and trans_mode = '2' and sts = '7' and tx_br_no = '%s' and \
				tx_date = %ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计提出张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num2 = ret;
			sprintf(acErrMsg, "提出张数为: [%ld]", num2);
			WRITEMSG

			/* 手工提入借方金额统计 */
			ret = sql_sum_double("mo_city", "bill_amt", &dr_amt,
				"in_out_ind = '2' and cd_ind = '1' and trans_mode = '1' and \
				sts != 'A' and payer_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "统计手工提入借方金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "手工提入借方金额为: [%lf]", dr_amt);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind='2' and cd_ind='1' and \
				trans_mode='1' and sts != 'A' and payer_br_no='%s' and \
				tx_date=%ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计手工提入借方张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num3 = ret;

			/* 手工提入贷方金额统计 */
			ret = sql_sum_double("mo_city", "bill_amt", &cr_amt,
				"in_out_ind = '2' and cd_ind = '2' and trans_mode = '1' and \
				sts != 'A' and payee_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "统计手工提入借方金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "手工提入贷方金额为: [%lf]", cr_amt);
			WRITEMSG

			amt3 = (cr_amt - dr_amt);

			sprintf(acErrMsg, "统计手工提入金额为: [%lf]", amt3);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind='2' and trans_mode='1' and \
				cd_ind='2' and sts != 'A' and payee_br_no='%s' and \
				tx_date=%ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计手工贷方笔数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			num3 += ret;

			/* 统计自动提入金额 */
			ret = sql_sum_double("mo_city", "bill_amt", &amt4,
				"in_out_ind = '2' and cd_ind = '2' and trans_mode = '2' and \
				sts != 'H' and payee_br_no = '%s' and tx_date = %ld",
				sCom_branch.br_no, beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "统计自动提入金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			sprintf(acErrMsg, "统计自动提入金额为: [%lf]", amt4);
			WRITEMSG

			ret = sql_count("mo_city", "in_out_ind='2' and cd_ind='2' and \
				trans_mode='2' and sts != 'H' and payee_br_no='%s' and \
				tx_date=%ld", sCom_branch.br_no, beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计自动提入张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num4 = ret;

			/* 统计中心通存金额 *
			ret = sql_sum_double("mo_city", "bill_amt", &amt5, 
				"sts = 'P' and tx_date = %ld", beg_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "统计中心通存金额错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			ret = sql_count("mo_city", "sts='P' and tx_date=%ld", beg_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计中心通存张数错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}

			num2 += ret;
			***/

			sum1 += amt1 + amt2;
			sum2 += amt3 + amt4;

			/* 汇总 */
			t_num1 += num1;
			t_num2 += num2;
			t_num3 += num3;
			t_num4 += num4;
			t_amt1 += amt1;
			t_amt2 += amt2;
			t_amt3 += amt3;
			t_amt4 += amt4;

			amt1 = amt1 / 10000;
			amt2 = amt2 / 10000;
			amt3 = amt3 / 10000;
			amt4 = amt4 / 10000;
			sum1 = sum1 / 10000;
			sum2 = sum2 / 10000;


           	fprintf(fp,"%s|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|\n", 
			sCom_branch.br_no, beg_date, amt1, num1, amt2, num2, 
			sum1, num1+num2, amt3, num3, amt4, num4, sum2, num3+num4);

    	}

		t_sum1 = t_amt1 + t_amt2;
		t_sum2 = t_amt3 + t_amt4;

		t_sum1 = t_sum1 / 10000;
		t_sum2 = t_sum2 / 10000;
		t_amt1 = t_amt1 / 10000;
		t_amt2 = t_amt2 / 10000;
		t_amt3 = t_amt3 / 10000;
		t_amt4 = t_amt4 / 10000;

        fprintf(fp,"%s|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|%10.6lf|%ld|\n", 
			"汇总", beg_date, t_amt1, t_num1, t_amt2, t_num2, 
			t_sum1, t_num1 + t_num2, t_amt3, t_num3, t_amt4, t_num4, 
			t_sum2, t_num3 + t_num4);

		ret = pub_base_deadlineD(beg_date, 1, &beg_date);
		if (ret != 0)
		{
			sprintf(acErrMsg,"取下一天日期错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D186");
			goto ErrExit;
		}
		if (beg_date > end_date)
		{
			/******
			t_sum1 += sum1;
			t_sum2 += sum2;
			t_num1 += total_num1;
			t_num2 += total_num2;
			sum1 = sum1 / 10000;
			sum2 = sum2 / 10000;
			fprintf(fp, "%s|%s|%s|%lf|%s|%ld|%s|%lf|%s|%ld|\n",
					"网点汇总", "", "提出金额", sum1, "提出张数", 
					total_num1, "提入金额", sum2, "提入张数", total_num2);

			beg_date = beg_date1;
			******/
			break;
		}

	}

    fclose(fp);

   	set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */

	/*** 登记交易流水 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
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

