/***************************************************************
* 文 件 名: spD222.c
* 功能描述: 总行报单打印(自动)
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
#include "com_branch_c.h"

spD222()
{
    int ret;
    char cFileName[50];
    FILE *fp;
    int beg_no,end_no;
    int flag;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
	struct com_branch_c sCom_branch;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_int("0530",&beg_no);
    get_zd_int("0540",&end_no);
    strcpy(mo_city_c.in_out_ind,"1");
    strcpy(mo_city_c.sts,"6");
    
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

    if (end_no == 0 && beg_no != 0)
    {
        /* 打印单张 */
        sprintf(acErrMsg,"单张打印");
        WRITEMSG
        ret = Mo_city_Dec_Sel(g_pub_tx.reply,"trans_mode='2' and \
			in_out_ind='1' and bat_no=%d  and sts='7' order by bat_no", beg_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else if (end_no == 0 && beg_no == 0)
    {
        /* 打印全部 */
        sprintf(acErrMsg,"打印全部");
        WRITEMSG
        ret = Mo_city_Dec_Sel(g_pub_tx.reply,"trans_mode='2' and \
				in_out_ind='1' and sts='6' order by bat_no");
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else 
    {
        /* 打印一段 */
        sprintf(acErrMsg,"打印一段 [%d] [%d]", beg_no, end_no);
        WRITEMSG
        ret = Mo_city_Dec_Sel(g_pub_tx.reply,"bat_no >= %d and bat_no <= %d \
              and sts = '7' and trans_mode='2' and in_out_ind='1' order by \
			  bat_no", beg_no, end_no);
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
                sprintf(acErrMsg,"没有记录!![%d]",ret);
                WRITEMSG
				strcpy(g_pub_tx.reply, "D160");
                goto ErrExit;
            }
            break;
        }

        /* 判断是否是该交易机构的上级机构 */
        ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
              and br_no='%s'", mo_city_c.tx_br_no);
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

        if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
        {
            /* 该机构不是该笔业务的上级机构不能打印 */
			if (beg_no != 0 && end_no == 0)
			{
            	sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                	    mo_city_c.tx_br_no);
            	WRITEMSG
            	strcpy(g_pub_tx.reply,"D144");
            	goto ErrExit;
			}
			continue;
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
		mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;
   
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
        
        strcpy(mo_city_c.sts,"7");
       	if (end_no == 0 && beg_no != 0)
    	{
        	sprintf(acErrMsg,"更新单张打印");
        	WRITEMSG
        	ret = sql_execute("update mo_city set sts='7', \
				pr_cnt = pr_cnt + 1 , tc_date=%ld where \
				trans_mode='2' and in_out_ind='1' and bat_no=%d  and \
				sts='7'", g_pub_tx.tx_date, beg_no);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"sql_execute ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
    	else if (end_no == 0 && beg_no == 0)
    	{
        	sprintf(acErrMsg,"更新打印全部");
        	WRITEMSG
        	ret = sql_execute("update mo_city set sts='7', \
				pr_cnt = pr_cnt + 1 , tc_date=%ld where trans_mode='2' and \
				in_out_ind='1' and sts='6' and bat_no=%d", g_pub_tx.tx_date,
				mo_city_c.bat_no);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
            	WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
            	goto ErrExit;
        	}
    	}
    	else 
    	{
        	sprintf(acErrMsg,"更新一段");
        	WRITEMSG
        	ret = sql_execute("update mo_city set sts='7', \
				pr_cnt = pr_cnt + 1 , tc_date=%ld where \
				bat_no = %d and sts = '7' and \
				trans_mode='2' and in_out_ind='1'", g_pub_tx.tx_date,
				mo_city_c.bat_no);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"sql_execute  ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
		/*****
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
	strcpy(g_pub_tx.brf, "自动提出打印");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"中心提出自动打印处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"中心提出自动打印处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

