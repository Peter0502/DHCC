/*************************************************
* 文 件 名: sp9619.c
* 功能描述：察看为签退柜员
*
* 作    者: rob 
* 完成日期：20031001
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "dc_log_c.h"
#include "com_tel_c.h"
#include "com_parm_c.h"
#include "mo_city_c.h"
#include "rsp_code_c.h"
#include "tel_control_c.h"

int sp9619( )
{
    int ret=0;
    int i=0;
	int err_flag=0;
    char br_no[6];
	char tel[5];
    char flag[2];
	char tmp_str[256];
	char filename[50];
	char memo[256];
	char reply[5];
	FILE *fp;
    double j_amt=0.00, c_amt=0.00, d_amt=0.00;
	double tmp_amt1=0.00, tmp_amt2=0.00, tmp_amt3=0.00, tmp_amt4=0.00;
 
    struct com_branch_c sCom_branch;
    struct dc_log_c sDc_log;
	struct com_tel_c sCom_tel;
	struct com_parm_c sCom_parm;
	struct mo_city_c sMo_city;
	struct rsp_code_c sRsp_code;
	struct tel_control_c sTel_control;

    memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
    memset(&sDc_log, 0x0, sizeof(struct dc_log_c));
	memset(&sCom_tel, 0x0, sizeof(struct com_tel_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
	memset(&sRsp_code, 0x0, sizeof(struct rsp_code_c));
	memset(&sTel_control, 0x0, sizeof(struct tel_control_c));
	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(flag, 0x0, sizeof(flag));
	memset(br_no, 0x0, sizeof(br_no));
	memset(filename, 0x0, sizeof(filename));
	memset(memo, 0x0, sizeof(memo));
	memset(reply, 0x0, sizeof(reply));
	memset(tel, 0x0, sizeof(tel));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0910", br_no);
	/* get_zd_data("0920", tel); */

	if ( getenv("LOGDIR") )
	{
		strcpy( filename, getenv("LOGDIR") );
	}
	else 
	{ 
		strcpy(filename,"/tmp"); 
	} 
	strcat( filename, "/tel_t.txt" );

	sprintf(tmp_str, "rm -f %s", filename);
	system( tmp_str );
	memset(tmp_str, 0x0, sizeof(tmp_str));

	sprintf(acErrMsg, "机构号为:[%s], filename=[%s]", br_no, filename); 
	WRITEMSG

    j_amt = 0.00;
    d_amt = 0.00;
    /* 判断柜员是否签退
    ret = sql_count("com_tel","(csts='0' or csts='1') and br_no='%s'",br_no);
    if (ret > 0)
    {
        sprintf(acErrMsg,"有柜员未签退!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O051");
        goto ErrExit;
    } */

	if (strcmp(br_no, "") != 0) 
	{
		ret = sql_count("com_tel","csts in ('0', '1') and br_no='%s' and \
			lvl != '6' and substr(tel,1,1) != '9'", br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "sql_count error  [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O262");
			goto ErrExit;
		}
	}
	else
	{
		ret = sql_count("com_tel","csts in ('0', '1') and \
			lvl != '6' and substr(tel,1,1) != '9'");
		if (ret < 0)
		{
			sprintf(acErrMsg, "sql_count error  [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O262");
			goto ErrExit;
		}
	}

	if (ret > 0)
	{
		fp = fopen(filename,"a");
		if ( fp )
		{
			if (strcmp(br_no, "") != 0)
			{
				fprintf(fp, "该机构 [%s] 还有 [%d] 个柜员未签退:\n",br_no, ret);
			}
			else
			{
				fprintf(fp, "还有 [%d] 个柜员未签退:\n", ret);
			}
			fprintf(fp, "----------------------------------------------------------\n");
			fprintf(fp, "柜员号 姓  名  机构号      原因\n");
			fclose(fp);
		}
	}

	/* 判断未签退柜员是否可以签退 */
	if (strcmp(br_no, "") != 0)
	{
		ret = Com_tel_Dec_Sel(g_pub_tx.reply, "csts in ('0', '1') and \
			br_no='%s' and lvl != '6' and substr(tel,1,1) != '9' order by tel", br_no);
		if (ret != 0)
		{
			sprintf(acErrMsg, "DECLARE FOR Select error [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		ret = Com_tel_Dec_Sel(g_pub_tx.reply, "csts in ('0', '1') and \
			lvl != '6' and substr(tel,1,1) != '9' order by br_no, tel");
		if (ret != 0)
		{
			sprintf(acErrMsg, "DECLARE FOR Select error [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	err_flag = 0;
	while(1)
	{
		ret = Com_tel_Fet_Sel(&sCom_tel, g_pub_tx.reply);
		if (ret == 100)
		{
			break;
		}
		else if ( ret )
		{
			sprintf(acErrMsg, "FETCH for select error [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		/****
		strcpy(g_pub_tx.tx_br_no, sCom_tel.br_no);
		memset(&sTel_control, 0x0, sizeof(struct tel_control_c));
		strcpy(sTel_control.br_no, sCom_tel.br_no);
		strcpy(sTel_control.tel, sCom_tel.tel);
		sTel_control.tx_date = g_pub_tx.tx_date; 
		sTel_control.sts[0] = '0'; 
		strcpy(sTel_control.msg, "该柜员由于昨天未签退而被锁定!!"); 
		ret = Tel_control_Ins(sTel_control, g_pub_tx.reply); 
		if ( ret ) 
		{ 
			sprintf(acErrMsg, "insert into tel_control error [%d]", ret); 
			WRITEMSG 
			goto ErrExit; 
		}
		****/
		
		/*检查柜员是否尚有付出但对方尚未收入的交接记录*/ 
		ret=sql_count("com_tel_connect", "tel='%s' and ind in('0','A')",
			sCom_tel.tel); 
		if( ret > 0 ) 
		{ 
			sprintf( acErrMsg, "柜员有付出的交接记录,但对方柜员尚未收入,\
				请先收入!!" ); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"O048"); 
			goto W_ErrExit; 
		}
		else if( ret < 0 )
		{ 
			sprintf(acErrMsg, "操作数据库出错!! [%d]", ret);
			WRITEMSG
			goto W_ErrExit; 
		} 
		
		/*检查柜员是否尚有应该收入但尚未收入的交接记录*/ 
		ret=sql_count("com_tel_connect", "tw_tel='%s' and ind in('0','A')",
			sCom_tel.tel); 
		if( ret > 0) 
		{ 
			sprintf( acErrMsg,"柜员有应该收入的交接记录但尚未收入,请先收入!!"); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"O047"); 
			goto W_ErrExit; 
		}
		else if( ret < 0 )
		{ 
			sprintf(acErrMsg, "操作数据库出错!! [%d]", ret);
			WRITEMSG
			goto W_ErrExit; 
		} 
		
		/* 检查是否有付出没有做收入的纪录 */ 
		ret = sql_count("note_mst", "tel='%s' and sts = '2'", sCom_tel.tel); 
		if (ret < 0) 
		{ 
			sprintf(acErrMsg, "统计note_mst表错误!! [%d]", ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "D101"); 
			goto W_ErrExit; 
		} 
		if (ret > 0) 
		{ 
			sprintf(acErrMsg, "有调出待收状态的凭证!![%d]", ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "N095"); 
			goto W_ErrExit; 
		} 
		
		/* 对同城的控制 add by rob at 20030711 */ 
		ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and \ 
			parm_seqn = 1"); 
		if (ret == 100) 
		{ 
			sprintf(acErrMsg, "公共参数表没有该挂账科目!!"); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "O228"); 
			goto W_ErrExit; 
		} 
		else if ( ret ) 
		{ 
			sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret); 
			WRITEMSG 
			goto W_ErrExit; 
		} 

		/* 检查会计流水 26102 科目余额是否为零 ***/ 
		d_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and \
			tel='%s' and sts='0' and dc_ind='1' and acc_hrt='%s'", 
			g_pub_tx.tx_date, sCom_tel.tel, sCom_parm.val); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg, "统计提出挂帐科目借方余额错误!! [%d]", ret); 
			WRITEMSG 
			goto W_ErrExit; 
		} 
		
		c_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &c_amt, "tx_date=%ld and \
			tel='%s' and sts='0' and dc_ind='2' and acc_hrt='%s'", 
			g_pub_tx.tx_date, sCom_tel.tel, sCom_parm.val); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg, "统计提出挂帐科目贷方余额错误!! [%d]", ret); 
			WRITEMSG 
			goto W_ErrExit; 
		} 
		
		if (pub_base_CompDblVal(d_amt, c_amt) != 0) 
		{ 
			sprintf(acErrMsg, "同城提出挂帐科目不平 [%lf] [%lf]",d_amt, c_amt); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "O230"); 
			goto W_ErrExit; 
		} 
		
		ret = sql_count("mo_city", "tx_br_no='%s' and tel='%s' and \
			sts='2'", sCom_tel.br_no, sCom_tel.tel); 
		if (ret < 0)
		{ 
			sprintf(acErrMsg, "统计同城登记簿错误!! [%d]", ret); 
			WRITEMSG 
			goto W_ErrExit; 
		} 
		else if (ret > 0) 
		{ 
			sprintf(acErrMsg, "该柜员还有提出打印票据没有核销!! "); 
			WRITEMSG 
			set_zd_data("0130", "该柜员还有提出打印票据没有核销!!"); 
			strcpy(g_pub_tx.reply, "D258"); 
			goto W_ErrExit; 
		} 
		
		
		/* 检查该柜员的会计流水是否平账 */ 
		/*取今日借方发生额总数*/ 
		tmp_amt1 = 0.00;
		tmp_amt3 = 0.00;
		ret=Dc_log_Dec_Sel(g_pub_tx.reply, "dc_ind='1' and tx_tx_br_no='%s' \
			and sts='0' and acc_hrt[1]<'6' and tel='%s'", sCom_tel.br_no, 
			sCom_tel.tel); 
		if ( ret ) 
		{ 
			sprintf( acErrMsg, "Declare dc_log error!!" ); 
			WRITEMSG 
			goto W_ErrExit; 
		} 

		while(1) 
		{ 
			memset(&sDc_log, 0x0, sizeof(struct dc_log_c));
			ret=Dc_log_Fet_Sel(&sDc_log, g_pub_tx.reply); 
			if ( ret == 100 )
			{
				break; 
			}
			else if ( ret ) 
			{ 
				sprintf( acErrMsg, "Fetch dc_log record error!!" ); 
				WRITEMSG 
				goto W_ErrExit; 
			} 
			
			if( sDc_log.ct_ind[0] == '1' ) 
			{
				tmp_amt3 += sDc_log.amt; 
			}
			tmp_amt1 += sDc_log.amt; 
		} 
		Dc_log_Clo_Sel(); 
		
		/*取今日贷方发生额总数*/ 
		tmp_amt2 = 0.00;
		tmp_amt4 = 0.00;
		ret=Dc_log_Dec_Sel(g_pub_tx.reply, "dc_ind='2' and tx_tx_br_no='%s' \
			and sts='0' and acc_hrt[1]<'6' and tel='%s'",sCom_tel.br_no, 
			sCom_tel.tel); 
		if ( ret ) 
		{ 
			sprintf( acErrMsg, "Declare dc_log error!!" ); 
			WRITEMSG 
			goto W_ErrExit; 
		}

		while(1) 
		{ 
			memset(&sDc_log, 0x0, sizeof(struct dc_log_c));
			ret=Dc_log_Fet_Sel(&sDc_log, g_pub_tx.reply); 
			if ( ret == 100 )
			{
				break; 
			}
			else if ( ret ) 
			{ 
				sprintf( acErrMsg, "Fetch dc_log record error!!" ); 
				WRITEMSG 
				goto W_ErrExit; 
			} 

			if ( sDc_log.ct_ind[0]=='1' ) 
			{
				tmp_amt4 += sDc_log.amt; 
			}
			tmp_amt2 += sDc_log.amt; 
		} 
		Dc_log_Clo_Sel();
		
		/*检查会计分录借贷是否平衡*/ 
		ret = pub_base_CompDblVal(tmp_amt1,tmp_amt2); 
		if ( ret == 1 ) 
		{ 
			memset(tmp_str, 0x0, sizeof(tmp_str));
			sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt1,tmp_amt2); 
			set_zd_data(DC_GET_MSG,tmp_str); 
			sprintf( acErrMsg, "本日借方发生额大于贷方发生额!!" ); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"O255"); 
			goto W_ErrExit; 
		} 
		else if ( ret == -1 ) 
		{ 
			memset(tmp_str, 0x0, sizeof(tmp_str));
			sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt1,tmp_amt2); 
			set_zd_data(DC_GET_MSG,tmp_str); 
			sprintf( acErrMsg, "本日借方发生额小于贷方发生额!!" ); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"O255"); 
			goto W_ErrExit; 
		}
		
		/*检查会计分录借贷是否平衡*/ 
		ret = pub_base_CompDblVal(tmp_amt3,tmp_amt4); 
		if ( ret == 1 ) 
		{ 
			memset(tmp_str, 0x0, sizeof(tmp_str));
			sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt3,tmp_amt4); 
			set_zd_data(DC_GET_MSG,tmp_str); 
			sprintf( acErrMsg, "本日现金借方发生额大于贷方发生额!!" ); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"O256"); 
			goto W_ErrExit; 
		} 
		else if ( ret == -1 ) 
		{ 
			memset(tmp_str, 0x0, sizeof(tmp_str));
			sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt3,tmp_amt4);
			set_zd_data(DC_GET_MSG,tmp_str); 
			sprintf( acErrMsg, "本日现金借方发生额小于贷方发生额!!" ); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"O256"); 
			goto W_ErrExit; 
		}

		W_OkExit:
			pub_base_strpack(sCom_tel.name);
			fp = fopen(filename,"a");
			if ( fp )
			{
				fprintf(fp, " %s  %s  %s  可以强行签退\n", 
					sCom_tel.tel, sCom_tel.name, sCom_tel.br_no);
				fclose(fp);
			}
			continue;
		W_ErrExit:
			memset(reply, 0x0, sizeof(reply));
			strcpy(reply, g_pub_tx.reply);
			ret = Rsp_code_Sel(g_pub_tx.reply, &sRsp_code, "code='%s'", reply);
			if ( ret == 100 )
			{
				sprintf(acErrMsg, "没有该响应码!! [%s]", reply);
				WRITEMSG
				memset(memo, 0x0, sizeof(memo));
				sprintf(memo, "没有该响应码[%s]", reply);
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "查询响应码异常!! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else
			{
				memset(memo, 0x0, sizeof(memo));
				strcpy(memo, sRsp_code.memo);
			}
			memset(tmp_str, 0x0, sizeof(tmp_str));
			get_zd_data(DC_GET_MSG,tmp_str); 
			if (strcmp(tmp_str, "") != 0)
			{
				memset(memo, 0x0, sizeof(memo));
				strcpy(memo, tmp_str);
			}

			pub_base_strpack(sCom_tel.name);
			fp = fopen(filename,"a");
			if ( fp )
			{
				fprintf(fp, " %s  %s  %s    %s\n", sCom_tel.tel, sCom_tel.name,
					sCom_tel.br_no, memo);
				fclose(fp);
			}
			err_flag += 1;
	}
	Com_tel_Clo_Sel();

	goto OkExit;

	if (err_flag > 0)
	{
		sprintf(acErrMsg, "有柜员未办完业务不能强行签退!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "O261");
		goto ErrExit;
	}

	/* add by rob at 20030808 对分中心的控制 *
	ret = sql_count("com_tx_br_rel", "rel_type=4 and br_no='%s'", br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计交易机构关系表异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "该机构是分中心，\
			需要做完支行对中心清算交易后才能签退");
		WRITEMSG
		ret = sql_count("trace_log", "tx_date=%ld and tx_br_no='%s' and \
			tx_code='4267'", g_pub_tx.tx_date, br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "统计流水表异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret == 0)
		{
			sprintf(acErrMsg, "请先做支行对中心清算交易[4267]");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D224");
			goto ErrExit;
		}
	}
	*/

	/* 对中心的控制(清算中心)的控制 
	ret = sql_count("com_tx_br_rel", "rel_type=4 and up_br_no='%s'", br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计交易机构关系表异常!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "该机构是总中心, 总中心必须做完清算核对才能签退");
		WRITEMSG
		ret = sql_count("trace_log", "tx_date=%ld and tx_br_no='%s' and \
			tx_code='4269'", g_pub_tx.tx_date, br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "统计流水表异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret == 0)
		{
			sprintf(acErrMsg, "请做完清算核对再签退[4269]");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D232");
			goto ErrExit;
		}
	}
	* add end ***/

	/*检查柜员交接登记簿是否存在"未取"和"现金调出代收"状态记录
	ret=Com_tel_connect_Dec_Sel(g_pub_tx.reply,"tel='%s'",g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "Declare出错!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		ret=Com_tel_connect_Fet_Sel(&sCom_tel_connect,g_pub_tx.reply);
		if(ret==100)break;
		else if(ret)
		{
			sprintf( acErrMsg, "Fetch柜员交接登记簿出错!!" );
			WRITEMSG
			goto ErrExit;
		}

		if(sCom_tel_connect.ind[0]=='0')
		{
			sprintf( acErrMsg, "柜员交接登记簿中还存在付出但未收入记录!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O250");
			goto ErrExit;
		}

		if(sCom_tel_connect.ind[0]=='A')
		{
			sprintf( acErrMsg, "柜员交接登记簿中还存在现金调出代收记录!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O251");
			goto ErrExit;
		}
	}

	Com_tel_connect_Clo_Sel();*/

	/*检查现金收付登记簿中是否存在未收入记录*
	ret=Cash_book_Dec_Sel(g_pub_tx.reply,"tw_br_no='%s'",br_no);
	if(ret)
	{
		sprintf( acErrMsg, "Declare出错!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		ret=Cash_book_Fet_Sel(&g_cash_book,g_pub_tx.reply);
		if(ret==100)break;
		else if(ret)
		{
			sprintf( acErrMsg, "Fetch现金收付登记簿出错!!" );
			WRITEMSG
			goto ErrExit;
		}

		if(g_cash_book.ind[0]=='0')
		{
			sprintf( acErrMsg, "现金收付登记簿中还有现金未收入记录!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O252");
			goto ErrExit;
		}
	}

	Cash_book_Clo_Sel();*/


	/*检查网点是否已签退
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",
						br_no);
	if(ret)
	{
		sprintf( acErrMsg, "查询公共机构表出错!!" );
		WRITEMSG
		goto ErrExit;
	}

	if(sCom_branch.wrk_sts[0]=='0')
	{
		sprintf( acErrMsg, "网点已经签退!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O050");
		goto ErrExit;
	}

	if(sCom_branch.wrk_sts[0]=='*')
	{
		sprintf( acErrMsg, "该网点已被关闭!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O068");
		goto ErrExit;
	}*/

	/*--- hao ---- 检查是否有现金付出没收，有现金应收没收*/
	/*	ret=sql_count("cash_book","tx_date=%ld and ind='0' and  br_no='%s'",
		g_pub_tx.tx_date,br_no);
	if( ret )
	{
		sprintf( acErrMsg, "该机构有付出但对方网点没有作收入交易的现金!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O248");
		goto ErrExit;
	}

	ret=sql_count("cash_book","tx_date=%ld and ind='0' and  tw_br_no='%s'",
		g_pub_tx.tx_date,br_no);
	if( ret )
	{
		sprintf( acErrMsg, "该机构有应该收入现金没有作现金收入交易!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O249");
		goto ErrExit;
	}
	**-----------------------------------------------**/

	/*取今日借方发生额总数*
	ret=Dc_log_Dec_Sel(g_pub_tx.reply,
		"dc_ind='1' and tx_tx_br_no='%s' and sts='0' and acc_hrt[1]<'6'",
		br_no);
	if(ret)
	{
		sprintf( acErrMsg, "Declare dc_log error!!" );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret=Dc_log_Fet_Sel(&sDc_log,g_pub_tx.reply);
		if(ret==100)break;
		else if(ret)
		{
			sprintf( acErrMsg, "Fetch dc_log record error!!" );
			WRITEMSG
			goto ErrExit;
		}

		if( sDc_log.ct_ind[0]=='1' ) tmp_amt3+=sDc_log.amt;
		tmp_amt1+=sDc_log.amt;
	}

	Dc_log_Clo_Sel();
	*/

	/*取今日贷方发生额总数
	ret=Dc_log_Dec_Sel(g_pub_tx.reply,
		"dc_ind='2' and tx_tx_br_no='%s' and sts='0' and acc_hrt[1]<'6'",br_no);
	if(ret)
	{
		sprintf( acErrMsg, "Declare dc_log error!!" );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret=Dc_log_Fet_Sel(&sDc_log,g_pub_tx.reply);
		if(ret==100)break;
		else if(ret)
		{
			sprintf( acErrMsg, "Fetch dc_log record error!!" );
			WRITEMSG
			goto ErrExit;
		}

		if( sDc_log.ct_ind[0]=='1' ) tmp_amt4+=sDc_log.amt;
		tmp_amt2+=sDc_log.amt;
	}

	Dc_log_Clo_Sel();*/

	/*检查会计分录借贷是否平衡
	ret=pub_base_CompDblVal(tmp_amt1,tmp_amt2);
	if(ret==1)
	{
		sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt1,tmp_amt2);
		set_zd_data(DC_GET_MSG,tmp_str);
		sprintf( acErrMsg, "本日借方发生额大于贷方发生额!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O255");
		goto ErrExit;
	}
	else if(ret==-1)
	{
		sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt1,tmp_amt2);
		set_zd_data(DC_GET_MSG,tmp_str);
		sprintf( acErrMsg, "本日借方发生额小于贷方发生额!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O255");
		goto ErrExit;
	}
	**/
	/*检查会计分录借贷是否平衡
	ret=pub_base_CompDblVal(tmp_amt3,tmp_amt4);
	if(ret==1)
	{
		sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt3,tmp_amt4);
		set_zd_data(DC_GET_MSG,tmp_str);
		sprintf( acErrMsg, "本日现金借方发生额大于贷方发生额!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O256");
		goto ErrExit;
	}
	else if(ret==-1)
	{
		sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt3,tmp_amt4);
		set_zd_data(DC_GET_MSG,tmp_str);
		sprintf( acErrMsg, "本日现金借方发生额小于贷方发生额!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O256");
		goto ErrExit;
	}*/

	/* 对同城的控制 add by rob at 20030714 */
	/* 提入票据 对网点的控制 */
	/* 提入贷方票据(手工) 
	ret = sql_count("mo_city", "payee_br_no='%s' and in_out_ind='2' and \
		sts='B' and trans_mode='1'", br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计查询登记簿错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 0)
	{
		sprintf(acErrMsg, "网点还有手工提入贷方票据已经打印未核销");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D194");
		goto ErrExit;
	}*/

	/* 提入借方票据(手工) 
	ret = sql_count("mo_city", "payer_br_no='%s' and in_out_ind='2' and \
		sts='B' and trans_mode='1'", br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计查询登记簿错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 0)
	{
		sprintf(acErrMsg, "网点还有手工提入借方票据已经打印未核销");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D229");
		goto ErrExit;
	}*/

	/* 提入借方票据(自动) 
	ret = sql_count("mo_city", "payee_br_no='%s' and in_out_ind='2' and \
		sts='H' and trans_mode='2'", br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计查询登记簿错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 0)
	{
		sprintf(acErrMsg, "网点还有自动提入票据没有打印!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D256");
		goto ErrExit;
	}*/

	/* 提出票据 对中心的控制 
	ret = Mo_city_Dec_Sel(g_pub_tx.reply, "in_out_ind='1' and sts='2' and \
		tx_date=%ld",g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Mo_city_Fet_Sel(&sMo_city, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		ret = sql_count("com_tx_br_rel", "br_no='%s' and up_br_no='%s' and \
			rel_type=3",
			sMo_city.tx_br_no, br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "sql_count com_tx_br_rel error !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret > 0)
		{
			sprintf(acErrMsg, "该中心还有票据没有核销!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D196");
			goto ErrExit;
		}
	}
	Mo_city_Clo_Sel();*/

	/* 对网点的控制 
	ret = sql_count("com_tx_br_rel", "br_no='%s' and \
		rel_type=3", br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count com_tx_br_rel error !! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	else if (ret > 0)
	{
		ret = sql_count("mo_city", "tx_br_no='%s' and sts='2'",
			br_no);
		if (ret < 0)
		{
			sprintf(acErrMsg, "统计同城登记簿错误!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret > 0)
		{
			sprintf(acErrMsg, "还有手工提出票据没有核销!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D194");
			goto ErrExit;

		}
	}*/

	/*检查所有柜员是否都已签退
	ret=Com_tel_Dec_Sel(g_pub_tx.reply,"br_no='%s' and substr(tel,1,1)!='9' \
		and lvl!='6'",br_no);
	if(ret)
	{
		sprintf( acErrMsg, "Declare出错!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset(&sCom_tel, 0x0, sizeof(struct com_tel_c));
		ret=Com_tel_Fet_Sel(&sCom_tel,g_pub_tx.reply);
		if(ret==100)break;
		else if(ret)
		{
			sprintf( acErrMsg, "Fetch柜员信息表出错!!" );
			WRITEMSG
			goto ErrExit;
		}

		if(sCom_tel.csts[0]=='0'||sCom_tel.csts[0]=='1')
		{
			sprintf( acErrMsg, "还有柜员未签退!!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"O051");
			goto ErrExit;
		}
	}

	Com_tel_Clo_Sel();*/

	/*修改网点状态为签退
	ret=sql_execute("update com_branch set wrk_sts='0' where br_no='%s'",br_no);
	if(ret)
	{
		sprintf( acErrMsg, "更新网点状态出错!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O049");
		goto ErrExit;
	}

	if( sCom_branch.spe_ind[9]=='1' )
	{
		TRACE
		strcpy(g_pub_tx.reply,"S100");
		goto ErrExit;
	}*/
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"察看未签退柜员成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"察看未签退柜员失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
