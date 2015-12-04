/***************************************************************
* 文 件 名: spD282.c
* 功能描述: 中心提入票据查询
*
* 作    者: jack
* 完成日期: 2004年5月23日
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

spD282()
{
    int ret=0;
    int bat_no=0;
	char flag[2];
	int flag1=0;
	char wherelist[1024];
	char wherelist1[1024];
	char stat[50];
	char tmp[500];
    char cFileName[50];
    FILE *fp;
	char br_no[6];
	long t_cnt=0;
	double t_amt=0.00;

	double amt=0.00;
	int i=0;

    struct mo_city_c mo_city_c;
	struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(br_no, 0x0, sizeof(br_no));
	memset(flag, 0x0, sizeof(flag));
	memset(stat, 0x0, sizeof(stat));
	memset(tmp, 0x0, sizeof(tmp));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(wherelist1, 0x0, sizeof(wherelist1));

	flag1 = 0;

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0670",flag);
	get_zd_data("0680",mo_city_c.trans_mode);
	get_zd_double("0390", &amt);
	get_zd_data("0910", br_no);
	get_zd_long("0440", &mo_city_c.tx_date);
    
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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","流水号", 
			"录入日期", "批次号", "网点号",
			"操作员","付款人账号","金额","票据种类","收款人账号","提出类型",
			"打印次数","提出状态","票据传递方式");

	sprintf(wherelist,"in_out_ind='2'");

	if (strcmp(mo_city_c.trans_mode,"") != 0)
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp," and trans_mode='%s'", mo_city_c.trans_mode);
		strcat(wherelist, tmp);
	}

	if (mo_city_c.tx_date != 0)
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp, " and tx_date=%ld", mo_city_c.tx_date);
		strcat(wherelist, tmp);
	}

	if (strcmp(flag,"") != 0)
	{
		if (flag[0] == '2')
		{
			/* 已打印 */
			if (mo_city_c.trans_mode[0] == '1')
			{
				strcat(wherelist," and sts='B'");
			}
			else if (mo_city_c.trans_mode[0] == '2')
			{
				strcat(wherelist," and sts='I'");
			}
			else
			{
				strcat(wherelist," and (sts='B' or sts='I')");
			}
		}
		else if (flag[0] == '1')
		{
			/* 未打印 */
			if (mo_city_c.trans_mode[0] == '1')
			{
				strcat(wherelist," and sts='A'");
			}
			else if (mo_city_c.trans_mode[0] == '2')
			{
				strcat(wherelist," and sts='H'");
			}
			else
			{
				strcat(wherelist," and (sts='A' or sts='H')");
			}
		}
		else if (flag[0] == '3')
		{
			/* 未核销 */
			strcat(wherelist," and (sts='A' or sts='B')");
		}
		else if (flag[0] == '4')
		{
			/* 已核销 */
			strcat(wherelist, " and sts='C'");
		}
		else if (flag[0] == '5')
		{
			/* 已记帐 */
			strcat(wherelist," and (sts='O' or sts='P')");
		}
		else if (flag[0] == '6')
		{
			/* 已退票 */
			strcat(wherelist," and sts='Q'");
		}
	}

	if ( pub_base_CompDblVal(amt, 0.00) > 0 )
	{
		memset(tmp, 0x0, sizeof(tmp));
		sprintf(tmp," and bill_amt = %lf",amt);
		strcat(wherelist,tmp);
	}

	ret = sql_count("com_tx_br_rel", "rel_type=3 and up_br_no='%s'",  
		g_pub_tx.tx_br_no);
	if (ret < 0) 
	{ 
		sprintf(acErrMsg, "查询交易机构关系表错误!!"); 
		WRITEMSG 
		goto ErrExit; 
	} 
	else if (ret > 0) 
	{ 
		memset(tmp, 0x0, sizeof(tmp)); 
		sprintf(tmp, " and tx_br_no = '%s'", g_pub_tx.tx_br_no); 
		strcat(wherelist, tmp); 
	} 
	else 
	{
	   	ret = sql_count("com_tx_br_rel", "rel_type=3 and br_no='%s'",  
			g_pub_tx.tx_br_no); 
		if (ret < 0) 
		{ 
			sprintf(acErrMsg, "查询交易机构关系表错误!!"); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret > 0) 
		{ 
			memset(tmp, 0x0, sizeof(tmp)); 
			sprintf(tmp, " and (payee_br_no = '%s' or payer_br_no = '%s')", 
				g_pub_tx.tx_br_no, g_pub_tx.tx_br_no); 
			strcat(wherelist, tmp); 
		} 
		else 
		{ 
			sprintf(acErrMsg, "交易机构关系表没有没有定义该机构的票据传递关系");
			WRITEMSG 
			strcpy(g_pub_tx.reply, "D238"); 
			goto ErrExit; 
		} 
	}

	strcpy(wherelist1, wherelist);
	strcat(wherelist," order by payee_br_no, bat_no, tx_date, tx_no, trans_mode, cd_ind");

	sprintf(acErrMsg,"wherelist = [%s]", wherelist);
	WRITEMSG

    ret = Mo_city_Dec_Sel(g_pub_tx.reply,wherelist);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	t_cnt = 0;
	t_amt = 0.00;
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
			if (flag1 == 0)
			{
				sprintf(acErrMsg,"没有该类票据!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
            break;
        }

		if (strcmp(g_pub_tx.tx_br_no, mo_city_c.tx_br_no) != 0)
		{
			/* 网点查询 */

			ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,
				"rel_type=3 and br_no='%s'",g_pub_tx.tx_br_no);
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

			if (strcmp(g_pub_tx.tx_br_no, com_tx_br_rel_c.br_no) != 0)
			{
				sprintf(acErrMsg,"该机构不是该中心的下级机构!![%s] [%s]",
					g_pub_tx.tx_br_no, com_tx_br_rel_c.up_br_no);
				WRITEMSG
				continue;
			}
		}
		else
		{
			/* 中心查询 */
			sprintf(acErrMsg, "br_no=[%s]", br_no);
			WRITEMSG
			if (mo_city_c.cd_ind[0] == '1')
			{
				if (strcmp(br_no, NULL) != 0)
				{
					if (strncmp(mo_city_c.payer_br_no, br_no, 5) != 0)
					{
						continue;
					}
				}
			}
			else if (mo_city_c.cd_ind[0] == '2') 
			{ 
				if (strcmp(br_no, NULL) != 0)
				{
					if (strncmp(mo_city_c.payee_br_no, br_no, 5) != 0)
					{
						continue;
					}
				}
			}
			else
			{
				sprintf(acErrMsg, "同城登记簿中的借贷标志错误!! [%s]", 
					mo_city_c.cd_ind);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D201");
				goto ErrExit;
			}
		}

		flag1 = flag1 + 1;
 if (mo_city_c.trans_mode[0] == '1')
 {
		if (mo_city_c.sts[0] == 'A')
		{
			strcpy(stat,"手工提入中心录入");
		}
		else if (mo_city_c.sts[0] == 'B')
		{
			strcpy(stat,"手工提入中心已经打印");
		}
		else if (mo_city_c.sts[0] == 'C')
		{
			strcpy(stat,"手工提入网点已经核对");
		}
		else if (mo_city_c.sts[0] == 'O')
		{
			strcpy(stat,"网点已经提入记帐");
		}
		else if (mo_city_c.sts[0] == 'P')
		{
			strcpy(stat,"中心已经提入记帐");
		}
		else if (mo_city_c.sts[0] == 'Q')
		{
			strcpy(stat,"网点提入退票");
		}
		else
		{
			sprintf(acErrMsg,"提出票据的状态错误!! [%s]",mo_city_c.sts);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D176");
			goto ErrExit;
		}

		t_cnt = t_cnt + 1;
		t_amt = t_amt + mo_city_c.bill_amt;

		if (mo_city_c.cd_ind[0] == '2')
		{
        	fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
				mo_city_c.tx_no, mo_city_c.tx_date,
				mo_city_c.bat_no, mo_city_c.payee_br_no, mo_city_c.tel,
				mo_city_c.payer_ac_no,mo_city_c.bill_amt, "",
				mo_city_c.payee_ac_no,"提入贷方",mo_city_c.pr_cnt,
				stat,"手工");
		}
		else
		{
        	fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
				mo_city_c.tx_no, mo_city_c.tx_date,
				mo_city_c.bat_no, mo_city_c.payer_br_no, mo_city_c.tel,
				mo_city_c.payer_ac_no,mo_city_c.bill_amt, "",
				mo_city_c.payee_ac_no,"提入借方",mo_city_c.pr_cnt,
				stat,"手工");
		}
 }
 		else if (mo_city_c.trans_mode[0] == '2')
 		{
			if (mo_city_c.sts[0] == 'H')
			{
				strcpy(stat,"自动提入中心录入");
			}
			else if (mo_city_c.sts[0] == 'I')
			{
				strcpy(stat,"自动提入网点已经打印");
			}
			else if (mo_city_c.sts[0] == 'O')
			{
				strcpy(stat,"网点已经提入记帐");
			}
			else if (mo_city_c.sts[0] == 'P')
			{
				strcpy(stat,"中心已经提入记帐");
			}
			else if (mo_city_c.sts[0] == 'Q')
			{
				strcpy(stat,"网点提入退票");
			}
			else
			{
				sprintf(acErrMsg,"提出票据的状态错误!! [%s]",mo_city_c.sts);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D176");
				goto ErrExit;
			}

			t_cnt = t_cnt + 1;
			t_amt = t_amt + mo_city_c.bill_amt;

			switch(mo_city_c.bill_type[0])
			{
				case '1':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.payee_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "同城",
						mo_city_c.payee_ac_no,"提入贷方",mo_city_c.pr_cnt,
						stat,"自动");
					break;
				case '2':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.payee_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "内转",
						mo_city_c.payee_ac_no,"提入贷方",mo_city_c.pr_cnt,
						stat,"自动");
					break;
				case '3':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.payee_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "退票",
						mo_city_c.payee_ac_no,"提入贷方",mo_city_c.pr_cnt,
						stat,"自动");
					break;
				case '4':
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.payee_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "其他",
						mo_city_c.payee_ac_no,"提入贷方",mo_city_c.pr_cnt,
						stat,"自动");
					break;
				default:
					sprintf(acErrMsg,"票据种类错误!![%s]",mo_city_c.bill_type);
					WRITEMSG
           		fprintf(fp,"%ld|%ld|%d|%s|%s|%s|%16.2lf|%s|%s|%s|%d|%s|%s|\n",
						mo_city_c.tx_no, mo_city_c.tx_date,
						mo_city_c.bat_no, mo_city_c.payee_br_no, mo_city_c.tel,
						mo_city_c.payer_ac_no,mo_city_c.bill_amt, "",
						mo_city_c.payee_ac_no,"提入贷方",mo_city_c.pr_cnt,
						stat,"自动");
					break;
			}
 		}
 		else
 		{
			sprintf(acErrMsg,"该票据的传递方式错误!! [%s]",
					mo_city_c.trans_mode);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D177");
			goto ErrExit;
 		}
    }
	/* 汇总 *
	ret = sql_sum_double("mo_city", "bill_amt", &t_amt, wherelist1);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总金额错误! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	ret = sql_count("mo_city", wherelist1);
	if (ret < 0)
	{
		sprintf(acErrMsg, "汇总张数错误! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	t_cnt = ret;
	****/
    fprintf(fp,"%s|%s|%s|%s|%ld|%s|%16.2lf|%s|%s|%s|%s|%s|%s|\n", 
		"汇总:", "", "", "张数:", t_cnt, "金额:", t_amt,"", "", "", "", "", "");
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

