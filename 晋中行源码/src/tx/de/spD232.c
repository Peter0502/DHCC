/***************************************************************
* 文 件 名: spD232.c
* 功能描述: 中心打印报单(手工提入)
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

spD232()
{
    int ret;
    int bat_no;
	int flag;
	char reprt[2];
    char br_no[6];
    char cFileName[50];
    FILE *fp;
	char wherelist[500];
	long beg_bat_no=0, end_bat_no=0;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
	struct com_branch_c com_branch_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(reprt, 0x0, sizeof(reprt));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0910",br_no);
    get_zd_data("0670",reprt);
    strcpy(mo_city_c.in_out_ind,"2");
    strcpy(mo_city_c.sts,"A");
    mo_city_c.tx_date = g_pub_tx.tx_date;


    if (strcmp(br_no, "") != 0)
	{
		sprintf(wherelist, "br_no='%s' and br_type in ('3','4','5')", br_no);
	}
	else
	{
		strcat(wherelist, " br_type in ('3','4','5') ");
	}

	strcat(wherelist, " order by br_no");

	sprintf(acErrMsg,"wherelist = [%s]\n", wherelist);
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

   	fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|\n","批次号","网点号码","网点名称",
			"付款人账号", "收款人账号","金额","提入类型","打印次数");

	ret = Com_branch_Dec_Sel(g_pub_tx.reply, wherelist);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECALRE FOR SELECT COM_BRANCH!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	flag = 0;

	while(1)
	{
		ret = Com_branch_Fet_Sel(&com_branch_c, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH FOR SELECT COM_BRANCH!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			/***
			if (strcmp(br_no, "") != 0)
			{
				sprintf(acErrMsg,"没有该机构号!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D173");
				goto ErrExit;
			}
			****/
			if (flag == 0)
			{
				sprintf(acErrMsg,"没有该票据!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
			break;
		}

		/** delete by rob at 20051013 for 中心和分中心的传递 
    	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
        	      and br_no='%s'", com_branch_c.br_no);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"查询交易机构关系表错误!! [%d] [%s]",
				ret, com_branch_c.br_no);
        	WRITEMSG
			goto ErrExit;
    	}
		else if (ret == 100)
		{
			sprintf(acErrMsg, "交易机构关系表中没有记录!!");
			WRITEMSG
			continue;
		}

    	if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    	{
        	* 该机构不是该笔业务的上级机构不能打印 *
        	sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",
					mo_city_c.bat_no, com_branch_c.br_no);
        	WRITEMSG
			if (strcmp(br_no,"") != 0)
			{
        		strcpy(g_pub_tx.reply,"D144");
        		goto ErrExit;
			}
			continue;
    	}
		*/

		/****** 
		if (strcmp(com_tx_br_rel_c.up_br_no,com_branch_c.br_no) == 0)
		{
			sprintf(acErrMsg,"自己网点就不用打印了!!");
			WRITEMSG
			* 更新这些纪录的状态 *
			ret = sql_execute("update mo_city set sts='B' where cd_ind='1' \
					and payer_br_no= '%s' and tx_date=%ld and sts='A'", 
					com_branch_c.br_no, g_pub_tx.tx_date); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"更新同城登记簿中的纪录错误!! [%d]",ret); 
				WRITEMSG 
				strcpy(g_pub_tx.reply,"D171"); 
				goto ErrExit;
			} 

			ret = sql_execute("update mo_city set sts='B' where cd_ind='2' \
					and payee_br_no= '%s' and tx_date=%ld and sts='A'", 
					com_branch_c.br_no, g_pub_tx.tx_date); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"更新同城登记簿中的纪录错误!! [%d]",ret); 
				WRITEMSG 
				strcpy(g_pub_tx.reply,"D171"); 
				goto ErrExit;
			} 
			else 
			{ 
				if (strcmp(br_no, "") != 0)
				{
					strcpy(g_pub_tx.reply,"D170"); 
					set_zd_data("0130","自己提给自己则不用打印!!");
					goto OkExit;
				}
			} 
        	continue;
		}
		****/

    	/* 生成批次号 */
    	ret = sql_max_int("mo_city","bat_no","trans_mode='1' and \
				in_out_ind='2'");
    	if (ret < 0)
    	{
        	sprintf(acErrMsg,"查询同城登记簿!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}

    	bat_no = ret + 1;

		/* if (strcmp(br_no, "") != 0) */
		if (reprt[0] == 'N')
		{
			/* 重打 */
			beg_bat_no = 0;
			get_zd_long("0530", &beg_bat_no);
			if ( beg_bat_no == 0 )
			{
   				ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_br_no='%s' and \
					sts='B'", g_pub_tx.tx_br_no);
   				if (ret != 0)
   				{
       				sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       				WRITEMSG
       				goto ErrExit;
   				}
			}
			else
			{
   				ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_br_no='%s' and \
					sts='B' and bat_no=%ld", g_pub_tx.tx_br_no, beg_bat_no);
   				if (ret != 0)
   				{
       				sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       				WRITEMSG
       				goto ErrExit;
   				}
			}
		}
		else
		{
			/* 第一次打印 */
   			ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_br_no='%s' and \
					sts='A'", g_pub_tx.tx_br_no);
   			if (ret != 0)
   			{
       			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       			WRITEMSG
       			goto ErrExit;
   			}
		}

    	while(1)
    	{
        	ret = Mo_city_Fet_Upd(&mo_city_c, g_pub_tx.reply);
        	if (ret != 0 && ret != 100)
        	{
            	sprintf(acErrMsg,"查询同城登记簿错误!! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
        	else if (ret == 100)
        	{
				/****
				if (flag == 0)
				{
					sprintf(acErrMsg,"没有该类票据!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D160");
					goto ErrExit;
				}
				*****/
            	break;
        	}
 
			if (strcmp(mo_city_c.sts, "A") == 0)
			{
        		mo_city_c.bat_no = bat_no;
				if (flag == 0)
				{
					beg_bat_no = bat_no;
					end_bat_no = bat_no;
				}
				else
				{
					end_bat_no = bat_no;
				}
			}
			else
			{
				beg_bat_no = end_bat_no = mo_city_c.bat_no;
			}

			mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;
        	if (mo_city_c.cd_ind[0] == '1')
        	{
				/* 提入借方 */
				if (strcmp(mo_city_c.payer_br_no,com_branch_c.br_no) != 0)
				{
					continue;
				}
				flag = flag + 1;
            	fprintf(fp,"%d|%s|%s|%s|%s|%lf|%s|%d|\n",mo_city_c.bat_no,
					com_branch_c.br_no,com_branch_c.br_name, 
					mo_city_c.payer_ac_no, mo_city_c.payee_ac_no, 
					mo_city_c.bill_amt, "提入借方", mo_city_c.pr_cnt);
        	}
        	else
        	{
				/* 提入贷方 */
				if (strcmp(mo_city_c.payee_br_no,com_branch_c.br_no) != 0)
				{
					continue;
				}
				flag = flag + 1;
            	fprintf(fp,"%d|%s|%s|%s|%s|%lf|%s|%d|\n",mo_city_c.bat_no,
					com_branch_c.br_no, com_branch_c.br_name, 
					mo_city_c.payer_ac_no, mo_city_c.payee_ac_no, 
					mo_city_c.bill_amt, "提入贷方", mo_city_c.pr_cnt);
        	}

        	strcpy(mo_city_c.sts,"B");
        	ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
    	Mo_city_Clo_Upd();
	}

    fclose(fp);
	
   	set_zd_data( DC_FILE_SND, "1" );        /* 批量传送标志 */


	/* set_zd_long("0480",flag); */

	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "手工提入中心打印");
	g_pub_tx.ac_seqn = beg_bat_no;
	g_pub_tx.svc_ind = end_bat_no;
	sprintf(acErrMsg, "beg=[%ld] , [%ld]", beg_bat_no, end_bat_no);
	WRITEMSG
	strcpy(g_pub_tx.no_show, "1");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"中心提入手工打印处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"中心提入手工打印处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

