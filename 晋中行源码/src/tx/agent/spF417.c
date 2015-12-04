/*************************************************
* 文 件 名: spF417.c
* 功能描述: 交费信息查询
*
* 作    者: mike
* 完成日期: 2004年07月27日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "mo_finance_info_c.h"

spF417()  
{ 	
	int ret=0;
	int flag=0;
	char cFileName[50];
    FILE *fp;
	char wherelist[500];
	char tmpstr[80];
	char tel[5];
	char c_flag[2];
    
	struct mo_finance_info_c sMo_finance_info;

	memset(&sMo_finance_info, 0x0, sizeof(struct mo_finance_info_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(tel, 0x0, sizeof(tel));
	memset(c_flag, 0x0, sizeof(c_flag));

	/*** 初始化全局变量 ***/
	if ( pub_base_sysinit() )
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_data("0730", sMo_finance_info.pay_no);
	get_zd_data("0370", sMo_finance_info.unit_code);
	get_zd_long("0440", &sMo_finance_info.ticket_date);
	get_zd_long("0450", &sMo_finance_info.tx_date);
	get_zd_double("0400", &sMo_finance_info.amt);
	get_zd_data("0920", tel);
	get_zd_data("0670", c_flag);

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

	strcpy(wherelist, "1=1 ");

	if (strcmp(sMo_finance_info.pay_no, NULL) != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and pay_no='%s' ", sMo_finance_info.pay_no);
		strcat(wherelist, tmpstr);
	}

	if (strcmp(tel, NULL) != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and tel='%s' ", tel);
		strcat(wherelist, tmpstr);
	}

	if (c_flag[0] == '1')
	{
		/* 正常缴费 */
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and sts in ('1', '2') ");
		strcat(wherelist, tmpstr);
	}
	else if (c_flag[0] == '2')
	{
		/* 待查缴费 */
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and sts = '3' ");
		strcat(wherelist, tmpstr);
	}
	else 
	{
		/* 补录缴款书 */
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and sts = '4' ");
		strcat(wherelist, tmpstr);
	}

	if (strcmp(sMo_finance_info.unit_code, NULL) != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and unit_code='%s' ", sMo_finance_info.unit_code);
		strcat(wherelist, tmpstr);
	}

	if (sMo_finance_info.ticket_date != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and ticket_date=%ld ", sMo_finance_info.ticket_date);
		strcat(wherelist, tmpstr);
	}

	if (sMo_finance_info.tx_date != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and tx_date=%ld ", sMo_finance_info.tx_date);
		strcat(wherelist, tmpstr);
	}

	if (pub_base_CompDblVal(sMo_finance_info.amt, 0.00) == 1)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, "and amt = %lf ", sMo_finance_info.amt);
		strcat(wherelist, tmpstr);
	}

	strcat(wherelist, " order by sts, payer_sort");

	sprintf(acErrMsg, "----------->wherelist = [%s]", wherelist);
	WRITEMSG

    fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|%s|\n","摘要","票据号码",
			"执收单位", "付款人","账号", "金额", "网点", "柜员", "流水号");
	
	sprintf(acErrMsg, "-------->交易网点为: [%s]", g_pub_tx.tx_br_no);
	WRITEMSG

	ret = Mo_finance_info_Dec_Sel(g_pub_tx.reply, wherelist);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	flag = 0;
	while(1)
	{
		ret = Mo_finance_info_Fet_Sel(&sMo_finance_info, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			if (flag == 0)
			{
				sprintf(acErrMsg, "没有要查询的记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "H036");
				goto ErrExit;
			}
			break;
		}

		flag ++;

		if (sMo_finance_info.sts[0] == '1' || sMo_finance_info.sts[0] == '2')
		{
			if (sMo_finance_info.payer_sort[0] == '0')
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "正常收入", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
			else
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "正常支出", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
		}
		else if (sMo_finance_info.sts[0] == '3')
		{
			if (sMo_finance_info.payer_sort[0] == '0')
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "待查收入", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
			else
			{
				fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "待查支出", 
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
			}
		}
		else
		{
			fprintf(fp,"%s|%s|%s|%s|%s|%16.2lf|%s|%s|%ld|\n", "补录缴款书",
					sMo_finance_info.pay_no, sMo_finance_info.unit_code,
					sMo_finance_info.payer_name, sMo_finance_info.payer_acno,
					sMo_finance_info.amt, sMo_finance_info.tx_br_no,
					sMo_finance_info.tel, sMo_finance_info.trace_no);
		}
	}
	Mo_finance_info_Clo_Sel();

  	fclose(fp);
    set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"缴费信息打印处理成功OKExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"缴费信息打印处理失败ErrExit [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
