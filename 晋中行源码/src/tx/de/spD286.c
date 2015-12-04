/***************************************************************
* 文 件 名: spD286.c
* 功能描述: 清算统计查询
*
* 作    者: jack
* 完成日期: 2004年4月13日
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
#include "trace_log_c.h"
#include "com_parm_c.h"

spD286()
{
    int ret=0;
    FILE *fp;
	char cFileName[50];
	double amt=0.00, c_amt=0.00, d_amt=0.00, amt1=0.00;
	int cnt=0;

	struct trace_log_c sTrace_log;
	struct com_parm_c sCom_parm;

	memset(&sTrace_log, 0x0, sizeof(struct trace_log_c));
	memset(cFileName, 0x0, sizeof(cFileName));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

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

    fprintf(fp,"~%s|%s|%s|%s|\n","类型", "笔数", "金额", "40801发生方向");

	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4231' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计手工提入借方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4231' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计手工提入借方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;

    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "手工提入借方", cnt, amt, "贷方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4231' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计手工提入贷方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4231' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计手工提入贷方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "手工提入贷方", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4241' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计自动提入贷方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4241' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计手工提入借方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "自动提入贷方", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4252' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计中心通存贷方金额错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4252' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计中心通存贷方票数错误![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "中心通存贷方", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4261' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "中心挂账(提出借方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4261' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "中心挂账(提出借方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "中心提出借挂账", cnt, amt, "贷方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4261' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "中心挂账(提出贷方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4261' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "中心挂账(提出贷方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "中心提出贷挂账", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4262' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "中心销账(提出借方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4262' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "中心销账(提出借方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "中心提出借销账", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4262' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "中心销账(提出贷方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4262' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "中心销账(提出贷方)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "中心提出贷销账", cnt, amt, "贷方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4265' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "同城清算(提出)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4265' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "同城清算(提出)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "同城清算(提出)", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4265' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "同城清算(提入)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4265' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "同城清算(提入)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "同城清算(提入)", cnt, amt, "贷方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4268' and sts='0' and ac_no='40801' and add_ind='1'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "大同城挂账(提出)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4268' and \
		sts='0' and ac_no='40801' and add_ind='1'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "大同城挂账(提出)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "大同城挂账(提出)", cnt, amt, "借方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("trace_log", "amt", &amt, "tx_date=%ld and \
		tx_code='4268' and sts='0' and ac_no='40801' and add_ind='0'", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "大同城挂账(提入)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log","tx_date=%ld and tx_code='4268' and \
		sts='0' and ac_no='40801' and add_ind='0'", g_pub_tx.tx_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "大同城挂账(提入)金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	cnt = ret;
    fprintf(fp,"%s|%ld|%16.2lf|%s|\n", "大同城挂账(提入)", cnt, amt, "贷方");

	cnt = 0;
	amt = 0.00;
	ret = sql_sum_double("mo_city", "bill_amt", &amt, "sts in ('1',  '6')", 
		g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "今天网点总提出金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_count("trace_log", "tx_code='4269' and sts='0'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "查询是否已经核对成功错误 [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	else if (ret > 0)
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn = 2");
		if (ret != 0)
		{
			sprintf(acErrMsg, "查询COM_PARM异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else 
	{
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
			parm_seqn = 1");
		if (ret != 0)
		{
			sprintf(acErrMsg, "查询COM_PARM异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	amt1 = amt;

    fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801余额应为", "", amt,"贷方");

	cnt = 0;
	amt = 0.00;
	c_amt = 0.00;
	d_amt = 0.00;
	ret = sql_sum_double("dc_log", "amt", &c_amt, "tx_date=%ld and \
		acc_hrt='40801' and sts='0' and dc_ind='1'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "今天网点总未提出金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and \
		acc_hrt='40801' and sts='0' and dc_ind='2'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "今天网点总未提出金额错误[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	pub_base_strpack(sCom_parm.val);
	amt = d_amt - c_amt + atof(sCom_parm.val);

	if (pub_base_CompDblVal(amt, 0.00) == 1)
	{
    	fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801实际余额为:", "", amt, "贷方");
	}
	else
	{
    	fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801实际余额为:", "", -amt, "借方");
	}
   	fprintf(fp,"%s|%s|%16.2lf|%s|\n", "40801相差金额为:", "", amt1-amt, "贷方");

    fclose(fp);

   	set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"打印核销流水查询处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
   	set_zd_data( DC_FILE_SND, "0" );
	sprintf(acErrMsg,"打印核销流水查询处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

