/***************************************************************
* 文 件 名: spD223.c
* 功能描述: 网点提出清单打印(自动)
*
* 作    者: jack
* 完成日期: 2004年4月04日
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
#include "com_branch_c.h"

spD223()
{
    int ret=0;
    char cFileName[50];
    FILE *fp;
    int beg_no=0, end_no=0;
    int flag=0;
	int zx_flag=0;
	long total_cnt=0;
	double total_amt = 0.00;
	char tmp_amt[20];
	char tmp_cnt[10];

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
	struct com_branch_c sCom_branch;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(cFileName, 0x0, sizeof(cFileName));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_long("0440", &g_pub_tx.tx_date);
    strcpy(mo_city_c.in_out_ind,"1");
    strcpy(mo_city_c.sts,"6");

	sprintf(acErrMsg, "--------->[%ld]", g_pub_tx.tx_date);
	WRITEMSG
    
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

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","编号","交易机构",
			"交易机构名称", "付款人账号", "付款人名称", "付款人行名",
			"收款人账号","收款人名称","收款人行名", "金额", "提出类型",
			"打印次数","摘要");

	ret = sql_count("com_tx_br_rel", "up_br_no='%s' and rel_type=3", 
		g_pub_tx.tx_br_no);
	if ( ret < 0 )
	{
		sprintf(acErrMsg, "统计交易机构关系表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret == 0 )
	{
		sprintf(acErrMsg, "网点打印!!");
		WRITEMSG
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and tel='%s' and \
			in_out_ind='1' and trans_mode='2' order by bat_no", 
			g_pub_tx.tx_date, g_pub_tx.tel);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		zx_flag = 0;
  	}
	else if ( ret > 0)
	{
		sprintf(acErrMsg, "中心打印!!");
		WRITEMSG
    	ret = Mo_city_Dec_Sel(g_pub_tx.reply,"tc_date=%ld and in_out_ind='1' \
			and trans_mode='2' and tx_br_no in \
			(select br_no from com_tx_br_rel where up_br_no='%s' ) \
			order by bat_no", g_pub_tx.tx_date, g_pub_tx.tx_br_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
		zx_flag = 1;
	}

    flag = 0;
	total_cnt = 0;
	total_amt = 0.00;
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
                sprintf(acErrMsg,"没有记录!![%d]",ret);
                WRITEMSG
				strcpy(g_pub_tx.reply, "D160");
                goto ErrExit;
            }
            break;
        }

		ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
				mo_city_c.tx_br_no);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"查询机构编码表错误!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D172");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"该票据的交易机构不存在!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D173");
			goto ErrExit;
		}

        flag = flag + 1;
   		total_cnt = total_cnt + 1;
		total_amt = total_amt + mo_city_c.bill_amt;
		if (mo_city_c.bill_type[0] == '1')
		{
        	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name, 
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "同城",mo_city_c.pr_cnt,mo_city_c.brf,
			mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '2')
		{
        	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name,
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "内转",mo_city_c.pr_cnt,mo_city_c.brf,
			mo_city_c.remark);
		}
		else if (mo_city_c.bill_type[0] == '3')
		{
        	fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name,
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "退票",mo_city_c.pr_cnt,mo_city_c.brf,
			mo_city_c.remark);
		}
		else 
		{ 
			fprintf(fp,"%d|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
			mo_city_c.bat_no, mo_city_c.tx_br_no, sCom_branch.br_name,
			mo_city_c.payer_ac_no, mo_city_c.payer_name,
			mo_city_c.payer_br_name, mo_city_c.payee_ac_no, 
			mo_city_c.payee_name,mo_city_c.payee_br_name,
          	mo_city_c.bill_amt, "其他",mo_city_c.pr_cnt,mo_city_c.brf, 
			mo_city_c.remark);
		}
    }
    Mo_city_Clo_Sel();

	memset(tmp_amt, 0x0, sizeof(tmp_amt));
	memset(tmp_cnt, 0x0, sizeof(tmp_cnt));
	sprintf(tmp_amt, "%16.2lf", total_amt);
	sprintf(tmp_cnt, "%ld", total_cnt);
	fprintf(fp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%s|%d|%s|%s|\n",
     	"汇总", "", "", "", "", "", "", "", "金额", total_amt, 
		"张数", total_cnt, "", "");
    fclose(fp);
    set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */


	/*** 登记交易流水 
	strcpy(g_pub_tx.brf, "自动提出打印");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点提出清单打印处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"网点提出清单打印处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

