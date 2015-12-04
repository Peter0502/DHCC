/***************************************************************
* 文 件 名: gD111.c
* 功能描述: 特约商户的处理
*
* 作    者: rob
* 完成日期: 2003年6月24日
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
#include "pos_fee_deal_c.h"
#include "march_mst_c.h"
#include "march_mst_mx_c.h"
#include "pos_fee_mst_c.h"
#include "march_info_c.h"
#include "com_branch_c.h"
#include "com_parm_c.h"

int page = 1;
int 	total_line=60;
struct march_mst_c g_sMarch_mst;
struct march_mst_mx_c g_sMarch_mst_mx;
char  def_br_no[6];
char  def_tel[5];

int get_ratio_posmhd(int bli,int bll,char str[100]);
gD111()
{
    int ret;

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	memset(def_br_no, 0x0, sizeof(def_br_no));
	memset(def_tel, 0x0, sizeof(def_tel));

	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");

	/* 自动从前置机获取数据 */
	ret = mch_txt_to_table();
	if (ret != 0)
	{
		sprintf(acErrMsg, "获取数据错误!! ");
		WRITEMSG
		goto ErrExit;
	}

	/* 上特约商户基本帐 */
	ret = march_mst_acct();
	if (ret != 0)
	{
		sprintf(acErrMsg, "给特约商户记帐错误!!");
		WRITEMSG
		goto ErrExit;
	}
	
	/* POS手续费上账 */
	ret = pos_fee_mst_acct();
	if (ret != 0)
	{
		sprintf(acErrMsg, "POS手续费上帐错误!!");
		WRITEMSG
		goto ErrExit;
	}

	/* 特约商户进帐回单 */
	ret  = march_pos_hd();
	if (ret != 0)
	{
		sprintf(acErrMsg, "生成特约商户进帐回单错误!!");
		WRITEMSG
		goto ErrExit;
	}

	/* 特约商户交易清单 */
	ret = march_pos_list();
	if (ret != 0)
	{
		sprintf(acErrMsg, "生成特约商户交易清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

	/* 特约商户资金转入清单 */
	ret = march_pos_hzqd();
	if (ret != 0)
	{
		sprintf(acErrMsg, "生成特约商户资金转入清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

	/* 商户资金转入流水清单 */
	ret = march_pos_trqd();
	if (ret != 0)
	{
		sprintf(acErrMsg, "生成商户资金转入流水清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

	/* 特约商户转入114清算 */
	ret = march_acct_to_114();
	if (ret != 0)
	{
		sprintf(acErrMsg, "特约商户和114清算记账错误!!");
		WRITEMSG
		goto ErrExit;
	}
	
	/* pos手续费转入114清算 */
	ret = pos_acct_to_114();
	if (ret != 0)
	{
		sprintf(acErrMsg, "特约商户和114清算记账错误!!");
		WRITEMSG
		goto ErrExit;
	}

	/* 记录转历史 */
	ret = rec_to_history();
	if (ret != 0)
	{
		sprintf(acErrMsg, "记录转入历史表中错误!!");
		WRITEMSG
		goto ErrExit;
	}
OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"POS特约商户处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"POS特约商户处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int mch_txt_to_table()
{
	char tmp_buf[1025];
	char tmp_str[256];
	char *p;
	int i=0;
	char mch_name[40];
	char mch_usr[40];
	char mch_pass[40];
	char cmd_line[2049];
	char filename[80];
	FILE *fp;
	int ret;

	struct pos_fee_deal_c sPos_fee_deal;
	struct march_mst_c sMarch_mst;
	struct march_mst_mx_c sMarch_mst_mx;
	struct pos_fee_mst_c sPos_fee_mst;
	struct march_info_c sMarch_info;

	memset(&sPos_fee_deal, 0x0, sizeof(struct pos_fee_deal_c));
	memset(&sMarch_mst, 0x0, sizeof(struct march_mst_c));
	memset(&sMarch_mst_mx, 0x0, sizeof(struct march_mst_mx_c));
	memset(&sPos_fee_mst, 0x0, sizeof(struct pos_fee_mst_c));
	memset(&sMarch_info, 0x0, sizeof(struct march_info_c));

	/* 插入请求纪录 */
	ret = insert_pos_fee_deal("shhz");
	if (ret != 0)
	{
		sprintf(acErrMsg, "向POS消费处理记录表插入shhz记录错误!!");
		WRITEMSG
		return ret;
	}
	ret = insert_pos_fee_deal("shmx");
	if (ret != 0)
	{
		sprintf(acErrMsg, "向POS消费处理记录表插入shmx记录错误!!");
		WRITEMSG
		return ret;
	}
	ret = insert_pos_fee_deal("sxf");
	if (ret != 0)
	{
		sprintf(acErrMsg, "向POS消费处理记录表插入sxf记录错误!!");
		WRITEMSG
		return ret;
	}

	sql_commit();
	sql_begin();

	memset(tmp_buf, 0x0, sizeof(tmp_buf));
	memset(tmp_str, 0x0, sizeof(tmp_str));

	ret = Pos_fee_deal_Dec_Sel(g_pub_tx.reply, "deal_flag != 'Y' and \
								pos_date <= %ld", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE ERROR !! [%d]", ret);
		WRITEMSG
		return ret;
	}
		vtcp_log("22222221");

	while(1)
	{
		ret = Pos_fee_deal_Fet_Sel(&sPos_fee_deal, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH ERROR !! [%d]", ret);
			WRITEMSG
			return ret;
		}
		else if (ret == 100)
		{
			break;
		}
		pub_base_strpack(sPos_fee_deal.fname);
		memset(tmp_str, 0x0, sizeof(tmp_str));
		sprintf(tmp_str,  "get %s%ld\n", sPos_fee_deal.fname, 
					sPos_fee_deal.pos_date );
		strcat(tmp_buf, tmp_str);
	}
    Pos_fee_deal_Clo_Sel();

	sprintf(acErrMsg, "-------> \n [%s] \n<----------", tmp_buf);
	WRITEMSG

	if ( (p = (char *)getenv("CSADDR")) == NULL )
	{
		sprintf(acErrMsg, "ftp 地址环境变量中没有定义");
		WRITEMSG
		return 1;
	}

	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(mch_name, 0x0, sizeof(mch_name));
	strcpy(tmp_str, p);
	strcpy(mch_name, &tmp_str[2]);
	i = 0;
	while(mch_name[i]!=':') i++;
	mch_name[i] = 0;

	sprintf(acErrMsg, "ftp 地址: [%s] [%s]", mch_name, tmp_str);
	WRITEMSG

	if ((p = (char *)getenv("CSUSR")) == NULL)
	{
		sprintf(acErrMsg, "ftp 用户名和密码环境变量中没有定义!!");
		WRITEMSG
		return 1;
	}

	i = 0;
	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(mch_usr, 0x0, sizeof(mch_usr));
	memset(mch_pass, 0x0, sizeof(mch_pass));
	strcpy(tmp_str, p);
	strcpy(mch_usr, &tmp_str[2]);
	while(mch_usr[i]!=':') i++;
	mch_usr[i] = 0;
	strcpy(mch_pass, &tmp_str[2+i+1]);

	sprintf(acErrMsg, "-----> ftp : 地址: [%s] ,用户: [%s],密码: [%s]",
			mch_name, mch_usr, mch_pass);
	WRITEMSG

	chdir("/home/dhccb/tmp");
	memset(cmd_line, 0x0, sizeof(cmd_line));
	sprintf(cmd_line,"ftp -vin %s 1>/home/dhccb/log/get_pos_file.err 2>&1 <<!\nverbose off\nuser %s %s\ncd file\n%s\nbye\n!", mch_name, mch_usr, mch_pass, tmp_buf);

	vtcp_log("F[%s]",cmd_line);
	system(cmd_line);

	ret = Pos_fee_deal_Dec_Sel( g_pub_tx.reply, "deal_flag != 'Y' and \
								pos_date <= %ld", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE ERROR !! [%d]", ret);
		WRITEMSG
		return ret;
	}

	while(1)
	{
		ret = Pos_fee_deal_Fet_Sel(&sPos_fee_deal, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH ERROR !! [%d]", ret);
			WRITEMSG
			return ret;
		}
		else if (ret == 100)
		{
			break;
		}

		pub_base_strpack(sPos_fee_deal.fname);
		memset(filename, 0x0, sizeof(filename));
		sprintf(filename,"%s%ld",sPos_fee_deal.fname,sPos_fee_deal.pos_date);

		fp = fopen(filename, "r");

		if (fp != NULL)
		{
			ret = sql_execute("update pos_fee_deal set deal_flag='Y' \
				where fname='%s' and pos_date=%ld", sPos_fee_deal.fname,
				sPos_fee_deal.pos_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "更新POS消费处理记录表错误!! [%d]", ret);
				WRITEMSG
				return ret;
			}
		}
		else
		{
			sPos_fee_deal.req_cnt += 1;
			ret = sql_execute("update pos_fee_deal set req_cnt=%ld \
				where fname='%s' and pos_date=%ld", sPos_fee_deal.req_cnt,
				sPos_fee_deal.fname, sPos_fee_deal.pos_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "更新POS消费处理记录表错误!! [%d]", ret);
				WRITEMSG
				return ret;
			}
		}

		/* 处理商户资金明细 */
		if ( !memcmp(sPos_fee_deal.fname, "shhz", 4) )
		{
			while(fp != NULL)
			{
				memset(tmp_buf, 0x0, sizeof(tmp_buf));
				memset(&sMarch_mst, 0x0, sizeof(struct march_mst_c));
				fgets(tmp_buf, 1024, fp);
				if (strlen(tmp_buf) < 1) break;
				ret = txt_to_marchmst_struct(tmp_buf, &sMarch_mst);
				if (ret != 0)
				{
					sprintf(acErrMsg, "文本文件转化成结构错误!! ");
					WRITEMSG
					return ret;
				}

				if ( memcmp(sMarch_mst.area_code, CITY, 3) )
					continue;  /* 跳过非本地区数据 */

				pub_base_old_acno( sMarch_mst.ac_no );
				ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, 
					"ac_no='%s'", sMarch_mst.ac_no);
				if (ret == 100)
				{
					sprintf(acErrMsg, "介质帐号关系表中没有该帐号!! [%d]", ret);
					WRITEMSG
					strcpy(sMarch_mst.opn_br_no, "");
					goto NOBRNO;
				}
				else if ( ret )
				{
					sprintf(acErrMsg, "查询介质关系表错误!! [%d]", ret);
					WRITEMSG
					return ret;
				}
				strcpy(sMarch_mst.opn_br_no, g_mdm_ac_rel.opn_br_no);

				ret = March_info_Sel(g_pub_tx.reply, &sMarch_info, \
					"mrchno='%s' and mrchstat='0'", sMarch_mst.march_code);
				if (ret != 0 && ret != 100)
				{
					sprintf(acErrMsg, "查询商户信息错误!! [%d]", ret);
					WRITEMSG
					return ret;
				} 
				else if (ret == 100)
				{
					memset(&sMarch_info, 0x0, sizeof(struct march_info_c));
					strcpy(sMarch_info.mrchno, sMarch_mst.march_code);
					strcpy(sMarch_info.mrchnm, sMarch_mst.march_name);
					strcpy(sMarch_info.br_no, sMarch_mst.opn_br_no);
					strcpy(sMarch_info.mrchtype, "01");
					strcpy(sMarch_info.mrchstat, "0");
					strcpy(sMarch_info.mrchacno, sMarch_mst.ac_no);
					sMarch_info.chrg = 1.00;
					sMarch_info.reg_date = g_pub_tx.tx_date;
					sMarch_info.beg_date = g_pub_tx.tx_date;
					sMarch_info.end_date = 99999999;
					ret = March_info_Ins(sMarch_info, g_pub_tx.reply);
					if (ret != 0)
					{
						sprintf(acErrMsg, "向商户信息表插入记录错误[%d]", ret);
						WRITEMSG
						return ret;
					}
				}
				
				if (strcmp(sMarch_info.br_no, sMarch_mst.opn_br_no) != 0)
				{
					ret = sql_execute("update march_info set br_no='%s' \
						where mrchno='%s'", sMarch_mst.opn_br_no, 
						sMarch_mst.march_code);
					if (ret != 0)
					{
						sprintf(acErrMsg, "更新商户开户机构错误!! [%d]", ret);
						WRITEMSG
						strcpy(g_pub_tx.reply, "D101");
						return ret;
					}
				}
			NOBRNO:
				strcpy(sMarch_mst.deal_sign, "N");
				sprintf(acErrMsg, "------------->code [%s] [%ld]",
					sMarch_mst.march_code, sMarch_mst.tx_date);
				WRITEMSG
				sprintf(acErrMsg, "[%ld][%ld][%lf][%lf][%lf][%lf][%ld]",
				sMarch_mst.tx_date, sMarch_mst.tx_cnt, sMarch_mst.tx_amt,
				sMarch_mst.fee_amt, sMarch_mst.fee_rate, sMarch_mst.real_amt,
				sMarch_mst.acct_date);
				WRITEMSG
					

				ret = March_mst_Ins(sMarch_mst, g_pub_tx.reply);
				if (ret != 0)
				{
					sprintf(acErrMsg, "向 MARCH_MST插入数据错误!! [%d]", ret);
					WRITEMSG
					return ret;
				}
			}
		}

		/* 处理开户行商户POS交易明细 */
		if ( !memcmp(sPos_fee_deal.fname, "shmx", 4) )
		{
			while(fp != NULL)
			{
				memset(tmp_buf, 0x0, sizeof(tmp_buf));
				memset(&sMarch_mst_mx, 0x0, sizeof(struct march_mst_mx_c));
				fgets(tmp_buf, 1024, fp);

				if (strlen(tmp_buf) < 1) break;

				ret = txt_to_marchmstmx(tmp_buf, &sMarch_mst_mx);
				if (ret != 0)
				{
					sprintf(acErrMsg, "文本文件转化成结构错误!! ");
					WRITEMSG
					return ret;
				}

				if (memcmp(sMarch_mst_mx.area_code, CITY, 3))
					continue; /* 跳过非本地区数据 */
	
				ret = March_mst_mx_Ins(sMarch_mst_mx, g_pub_tx.reply);
				if (ret != 0)
				{
					sprintf(acErrMsg,"向商户消费记录明细表中插入记录错误!! \
							[%d]", ret);
					WRITEMSG
					return ret;
				}
			}
		}

		/* 处理手续费记录 */
		if ( !memcmp(sPos_fee_deal.fname, "sxf", 3) )
		{
			while( fp != NULL )
			{
				memset(tmp_buf, 0x0, sizeof(tmp_buf));
				memset(&sPos_fee_mst, 0x0, sizeof(struct pos_fee_mst_c));
				fgets(tmp_buf, 1024, fp);

				if (strlen(tmp_buf) < 1) break;
				ret = txt_to_posfeemst(tmp_buf, &sPos_fee_mst);
				if (ret != 0)
				{
					sprintf(acErrMsg, "文本文件转化成结构错误!! ");
					WRITEMSG
					return ret;
				}

				if( memcmp(sPos_fee_mst.area_code, CITY, 3) )
					continue; /* 跳过非本地区数据 */

				strcpy(sPos_fee_mst.deal_sign, "N");
				ret = Pos_fee_mst_Ins(sPos_fee_mst, g_pub_tx.reply);
				if (ret != 0)
				{
					sprintf(acErrMsg,"向POS消费手续费表中增加记录错误!! [%d]",
							ret);
					WRITEMSG
					return ret;
				}
			}
		}
		fclose(fp);
	}
	Pos_fee_deal_Clo_Sel();
	sql_commit();
	sql_begin();

	return 0;
}

int insert_pos_fee_deal(char *fname)
{
	int ret;
	struct pos_fee_deal_c sPos_fee_deal;

	memset(&sPos_fee_deal, 0x0, sizeof(struct pos_fee_deal_c));

	ret = sql_count("pos_fee_deal", "fname='%s' and pos_date=%ld",
			fname, g_pub_tx.tx_date);
	if (ret > 0)
	{
		return 0;
	}
	else if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count error !! [%d]", ret);
		WRITEMSG
		return ret;
	}
	
	strcpy(sPos_fee_deal.fname, fname);
	sPos_fee_deal.pos_date = g_pub_tx.tx_date;
	strcpy(sPos_fee_deal.deal_flag, "N");
	sPos_fee_deal.req_cnt = 0;

	ret = Pos_fee_deal_Ins(sPos_fee_deal, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "向POS消费处理记录表插入记录错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	return 0;
}
int txt_to_marchmst_struct(char *tmp_buf, struct march_mst_c *sMarch_mst)
{
	char long_buf[80];

	pub_base_readfield(sMarch_mst->area_code, tmp_buf, '|');
	pub_base_readfield(sMarch_mst->march_code, tmp_buf, '|');
	pub_base_readfield(sMarch_mst->ac_no, tmp_buf, '|');
	pub_base_readfield(sMarch_mst->march_name, tmp_buf, '|');

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst->tx_date = atol(long_buf);

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst->tx_cnt = atol(long_buf);

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst->tx_amt = atof(long_buf);

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst->fee_amt = atof(long_buf);

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst->fee_rate = atof(long_buf);

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst->real_amt = atof( long_buf );

	return 0;
}
int txt_to_marchmstmx(char *tmp_buf, struct march_mst_mx_c *sMarch_mst_mx)
{
	char long_buf[80];

	pub_base_readfield(sMarch_mst_mx->area_code, tmp_buf, '|');
	pub_base_readfield(sMarch_mst_mx->march_code, tmp_buf, '|');
	pub_base_readfield(sMarch_mst_mx->tty_no, tmp_buf, '|');

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst_mx->tx_time = atol(long_buf);

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst_mx->tx_date = atol(long_buf);

	pub_base_readfield(sMarch_mst_mx->brf, tmp_buf, '|');

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst_mx->trace_no = atol(long_buf);

	pub_base_readfield(sMarch_mst_mx->card_no, tmp_buf, '|');
	pub_base_readfield(sMarch_mst_mx->opn_tx_br_name, tmp_buf, '|');

	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sMarch_mst_mx->tx_amt = atof(long_buf);

	return 0;
}

int txt_to_posfeemst(char *tmp_buf, struct pos_fee_mst_c *sPos_fee_mst)
{
	char long_buf[80];

	/* 地区码 */
	pub_base_readfield(sPos_fee_mst->area_code, tmp_buf, '|');

	/* 交易笔数 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->tx_cnt = atol(long_buf);

	/* 交易日期 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->tx_date = atol(long_buf);

	/* 交易金额 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->tx_amt = atof(long_buf);

	/* 实时手续费借方 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->dr_amt = atof(long_buf);

	/* 实时手续费贷方 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->cr_amt = atof(long_buf);

	/* 非实时手续费贷方 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->n_cr_amt = atof(long_buf);

	/* 调整手续费借方 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->adjust_dr_amt = atof(long_buf);

	/* 调整手续费贷方 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->adjust_cr_amt = atof(long_buf);

	/* 成功打单笔数 */
	memset(long_buf, 0x0,sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->succ_num = atol(long_buf);

	/* 耗材费 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->mater_amt = atof(long_buf);

	/* 清算金额 */
	memset(long_buf, 0x0, sizeof(long_buf));
	pub_base_readfield(long_buf, tmp_buf, '|');
	sPos_fee_mst->clr_amt = atof(long_buf);

	return 0;
}

int march_mst_acct()
{
	int ret=0;
	char deal_sign;
	char deal_msg[41];
	int flag=0; /* 1 存款 0 取款 */

	struct march_mst_c sMarch_mst;

	memset(&sMarch_mst, 0x0, sizeof(struct march_mst_c));
	memset(deal_msg, 0x0, sizeof(deal_msg));

	ret = sql_count("march_mst", "deal_sign='N'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count error !! [%d]", ret);
		WRITEMSG
		return ret;
	}
	if (ret < 1) return 0;

	ret = March_mst_Dec_Sel_W(g_pub_tx.reply, "deal_sign='N'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret);
		WRITEMSG
		return ret;
	}

	while(1)
	{
		ret = March_mst_Fet_Sel(&sMarch_mst, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR [%d]", ret);
			WRITEMSG
			return ret;
		}
		else if (ret == 100)
		{
			break;
		}

		sql_commit();
		sql_begin();

		memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
		pub_base_sysinit();

		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
		memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
		memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
		memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		/* 给特约商户账号存款 */
		strcpy(g_pub_tx.tx_br_no, def_br_no);
		strcpy(g_pub_tx.tel, def_tel);
		strcpy(g_pub_tx.ac_no, sMarch_mst.ac_no);
		pub_base_old_acno( g_pub_tx.ac_no );
		strcpy(g_pub_tx.cur_no, "01");
		strcpy(g_pub_tx.ct_ind, "2");
		if (pub_base_CompDblVal(sMarch_mst.real_amt, 0.00) == -1)
		{
			g_pub_tx.tx_amt1 = -sMarch_mst.real_amt; /* 取款 */
			flag = 0;
		}
		else
		{
			g_pub_tx.tx_amt1 = sMarch_mst.real_amt;
			flag = 1;
		}
		strcpy(g_pub_tx.brf, "商户资金清算转入");
		strcpy(g_pub_tx.prt_ind,"0");
		if (flag == 1)
		{
			strcpy(g_pub_tx.add_ind, "1");
			strcpy(g_pub_tx.ac_wrk_ind, "00000");
		}
		else
		{
			strcpy(g_pub_tx.add_ind, "0");
			strcpy(g_pub_tx.ac_wrk_ind, "0000000011");
		}
		strcpy(g_pub_tx.hst_ind, "1");
		g_pub_tx.svc_ind = 1001;

		ret = pub_acct_trance();
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用存取款主控失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "调用存取款主控失败[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		/* 记会计帐 */
		if (flag == 1)
		{
			set_zd_data("101A","01");
			set_zd_data("101B", "2");
			set_zd_double("1013", g_pub_tx.tx_amt1);
			ret = pubf_acct_proc("D099");
		}
		else
		{
			set_zd_data("102J","01");
			set_zd_data("102K", "2");
			set_zd_double("102F", g_pub_tx.tx_amt1);
			ret = pubf_acct_proc("D003");
		}
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用记会计帐函数失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记商户会计账错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		/* 记挂账科目 40710 */
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
		memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
		memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
		memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
		pub_base_sysinit();

		strcpy(g_pub_tx.tx_br_no, def_br_no);
		strcpy(g_pub_tx.tel, def_tel);

		strcpy(g_pub_tx.ac_no, "40710");
		strcpy(g_pub_tx.cur_no, "01");
		strcpy(g_pub_tx.ct_ind, "2");
		if (flag == 1)
		{
			g_pub_tx.tx_amt1 = sMarch_mst.real_amt;
			strcpy(g_pub_tx.add_ind, "0");
		}
		else
		{
			g_pub_tx.tx_amt1 = -sMarch_mst.real_amt;
			strcpy(g_pub_tx.add_ind, "1");
		}
		strcpy(g_pub_tx.brf, "商户资金清算转入");
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.ac_wrk_ind, "00000");
		strcpy(g_pub_tx.hst_ind, "1");
		g_pub_tx.svc_ind = 1001;

		ret = pub_acct_trance();
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用存取款主控失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记40710流水错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		/* 记会计帐 */
		if (flag == 1)
		{
			set_zd_double("0400", g_pub_tx.tx_amt1);
			set_zd_data("0210", "01");
			set_zd_data("0152", "407A0");
			set_zd_data("0670", "1");
			ret = pubf_acct_proc("Z111");
		}
		else
		{
			set_zd_double("0400", g_pub_tx.tx_amt1);
			set_zd_data("0210", "01");
			set_zd_data("0152", "407A0");
			set_zd_data("0670", "2");
			ret = pubf_acct_proc("Z111");
		}
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用记会计帐函数失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记40710会计帐错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		deal_sign='Y';
		strcpy(deal_msg,"商户资金清算转入");
		goto WRT_DEAL_MSG;

ROLL_BACK:
		sql_rollback();
		sql_begin();

WRT_DEAL_MSG:
		ret = sql_execute("update march_mst set deal_sign='%c',\
			acct_date=%ld, deal_msg='%s' where march_code='%s' and \
			tx_date=%ld", deal_sign, g_pub_tx.tx_date, deal_msg,
			sMarch_mst.march_code, sMarch_mst.tx_date);
		if (ret != 0)
		{
			sprintf(acErrMsg, "更新商户上账主文件错误!! [%d]", ret);
			WRITEMSG
			return ret;
		}
	}
	March_mst_Clo_Sel();

	sql_commit();
	sql_begin();

	return 0;
}

int march_acct_to_114()
{
   	int ret=0;
	double tx_amt=0.00;
	char dc_ind;
	char tmp_str[20];

	struct com_parm_c sCom_parm;
	struct pos_fee_deal_c sPos_fee_deal;

ret = Pos_fee_deal_Dec_Sel(g_pub_tx.reply, "fname='shhz' and \
	deal_flag='Y' order by pos_date");
if (ret != 0)
{
	sprintf(acErrMsg, "Declare for select error ! [%d]", ret);
	WRITEMSG
	goto ErrExit;
}

while(1)
{
	memset(&sPos_fee_deal, 0x0, sizeof(struct pos_fee_deal_c));
	ret = Pos_fee_deal_Fet_Sel(&sPos_fee_deal, g_pub_tx.reply);
	if (ret == 100)
	{
		break;
	}
	else if ( ret )
	{
		sprintf(acErrMsg, "Fetch pos_fee_deal error [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	memset(def_br_no, 0x0, sizeof(def_br_no));
	memset(def_tel, 0x0, sizeof(def_tel));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c)); 
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}
	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);

	sprintf(acErrMsg, "-------->交易日期为:[%ld]", g_pub_tx.tx_date);
	WRITEMSG

	/* 汇总借方发生额 
	ret = sql_sum_double("march_mst_hst", "real_amt", &tx_amt, 
		"deal_sign='Y' and acct_date=%ld ", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总商户清算金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}*/

	tx_amt = 0.00;
	ret = sql_sum_double("march_mst", "real_amt", &tx_amt, 
		"tx_date=%ld ", sPos_fee_deal.pos_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总商户清算金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	sprintf(acErrMsg, "汇总[%ld]商户清算金额金额为:[%lf]",
		sPos_fee_deal.pos_date, tx_amt);
	WRITEMSG

	memset(tmp_str, 0x0, sizeof(tmp_str));
	sprintf(tmp_str, "%ld", sPos_fee_deal.pos_date);
	sprintf(acErrMsg, " tmp_str = [%s]", tmp_str);
	WRITEMSG
	strcpy(tmp_str, tmp_str+4);
	sprintf(acErrMsg, " tmp_str = [%s]", tmp_str);
	WRITEMSG

	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	sprintf(g_pub_tx.brf, "商清算%s", tmp_str);
	sprintf(acErrMsg, " g_pub_tx.brf = [%s]", g_pub_tx.brf);
	WRITEMSG
	if (pub_base_CompDblVal(tx_amt, 0.00) == 1) 
	{
		/* 借方多 */
		strcpy(g_pub_tx.add_ind, "1"); /* 40710减少 */
		g_pub_tx.tx_amt1 = tx_amt;
		dc_ind = '1';
	}
	else if (pub_base_CompDblVal(tx_amt, 0.00) == -1)
	{
		/* 贷方多 */
		strcpy(g_pub_tx.add_ind, "0"); /* 借40710增加 */
		g_pub_tx.tx_amt1 = -tx_amt;
		dc_ind = '0';
	}
	else
	{
		sprintf(acErrMsg, "本日40710的总发生额为零!!");
		WRITEMSG
		goto OkExit;
	}

	strcpy(g_pub_tx.ac_no, "40710"); 
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	/* 记40710会计帐 */
	if (dc_ind == '0')
	{
		/* 内部借40710 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '1')
	{
		/* 内部贷 A017 40710*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "记40710会计帐错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg, "*******114 ************");
	WRITEMSG

	/* 记114账号 */
	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c)); 
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}
	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);

	strcpy(g_pub_tx.cur_no, "01"); 
	strcpy(g_pub_tx.ct_ind, "2"); 
	sprintf(g_pub_tx.brf, "商清算%s", tmp_str);
	if (dc_ind == '0') /* 借 */
	{
		strcpy(g_pub_tx.add_ind, "1");
		g_pub_tx.tx_amt1 = -tx_amt;
	}
	else /* 贷 */
	{
		strcpy(g_pub_tx.add_ind, "0");
		g_pub_tx.tx_amt1 = tx_amt;
	}
	sprintf(acErrMsg, "------------>[%lf]", g_pub_tx.tx_amt1);
	WRITEMSG

	ret= Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='YDTD' and \
		parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sCom_parm.val);
	strcpy(g_pub_tx.ac_no, sCom_parm.val);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	sprintf(acErrMsg, "--------->[%s]", g_pub_tx.add_ind);
	WRITEMSG
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	if (dc_ind == '1') /* 40710贷 */
	{
		/* 内部借114 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '0')
	{
		/* 内部贷114 A017*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "记114账号会计帐错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

} /* end for while */
	/* 汇总借方发生额 *
	c_bal = 0.00;
	c_bal = 0.00;
	ret = sql_sum_double("dc_log", "amt", &c_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='1' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总40710的借方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log", "amt", &d_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='2' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总40710的贷方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	sprintf(acErrMsg, " --------> c_bal=[%lf], d_bal=[%lf]", c_bal, d_bal);
	WRITEMSG
	if (pub_base_CompDblVal(c_bal, d_bal) != 0)
	{
		sprintf(acErrMsg, "同城通兑的记账不平!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D251");
		goto ErrExit;
	}
	**/
	
OkExit:
    return 0;
ErrExit:
    return 1;
}

int pos_fee_mst_acct()
{
	int ret;
	char deal_sign;
	char deal_msg[41];

	struct pos_fee_mst_c sPos_fee_mst;

	memset(&sPos_fee_mst, 0x0, sizeof(struct pos_fee_mst_c));

	ret = sql_count("pos_fee_mst", "deal_sign = 'N'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count pos_fee_mst error [%d]",ret);
		WRITEMSG
		return ret;
	}

	if (ret < 1)
	{
		return 0;
	}

	ret = Pos_fee_mst_Dec_Sel_W(g_pub_tx.reply, "deal_sign='N'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]",ret);
		WRITEMSG
		return ret;
	}

	while(1)
	{
		ret = Pos_fee_mst_Fet_Sel(&sPos_fee_mst, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR UPDATE ERROR [%d]", ret);
			WRITEMSG
			return ret;
		}
		else if (ret == 100)
		{
			break;
		}

		sql_commit();
		sql_begin();

		/* 记POS手续费科目 51110 贷 */
		memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
		pub_base_sysinit();

		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
		memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
		memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
		memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		strcpy(g_pub_tx.tx_br_no, def_br_no);
		strcpy(g_pub_tx.tel, def_tel);

		strcpy(g_pub_tx.ac_no, "51110");
		strcpy(g_pub_tx.cur_no, "01");
		strcpy(g_pub_tx.ct_ind, "2");
		g_pub_tx.tx_amt1 = sPos_fee_mst.dr_amt - sPos_fee_mst.cr_amt - \
			sPos_fee_mst.n_cr_amt - sPos_fee_mst.mater_amt;
		strcpy(g_pub_tx.brf, "POS手续费清算转入");
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.add_ind, "1");
		strcpy(g_pub_tx.ac_wrk_ind, "00000");
		strcpy(g_pub_tx.hst_ind, "1");
		g_pub_tx.svc_ind = 1001;

		ret = pub_acct_trance();
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用存取款主控失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记51110流水错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		/* 记会计帐(手续费) */
		set_zd_double("1218", g_pub_tx.tx_amt1);
		set_zd_data("1214", "01");
		set_zd_data("1211", "51110");
		set_zd_data("1215","2");
		ret = pubf_acct_proc("A017");
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用记会计帐函数失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记40710会计帐错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		/* 记挂账科目 40710 借*/
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
		memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
		memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
		memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
		pub_base_sysinit();

		strcpy(g_pub_tx.tx_br_no, def_br_no);
		strcpy(g_pub_tx.tel, def_tel);

		strcpy(g_pub_tx.ac_no, "40710");
		strcpy(g_pub_tx.cur_no, "01");
		strcpy(g_pub_tx.ct_ind, "2");
		strcpy(g_pub_tx.brf, "POS手续费清算转入");
		strcpy(g_pub_tx.prt_ind,"0");
		g_pub_tx.tx_amt1 = sPos_fee_mst.dr_amt - sPos_fee_mst.cr_amt - \
			sPos_fee_mst.n_cr_amt - sPos_fee_mst.mater_amt;
		strcpy(g_pub_tx.add_ind, "0");
		strcpy(g_pub_tx.ac_wrk_ind, "00000");
		strcpy(g_pub_tx.hst_ind, "1");
		g_pub_tx.svc_ind = 1001;

		ret = pub_acct_trance();
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用存取款主控失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记40710流水错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		/* 记会计帐 */
		set_zd_double("0400", g_pub_tx.tx_amt1);
		set_zd_data("0210", "01");
		set_zd_data("0152", "407A0");
		set_zd_data("0670", "1");
		ret = pubf_acct_proc("Z111");
		if (ret != 0)
		{
			sprintf(acErrMsg, "调用记会计帐函数失败!!");
			WRITEMSG
			deal_sign='N';
			sprintf(deal_msg, "记40710会计帐错误[%s]",g_pub_tx.reply);
			goto ROLL_BACK;
		}

		deal_sign='Y';
		strcpy(deal_msg,"POS手续费清算转入");
		goto WRT_DEAL_MSG;

ROLL_BACK:
		sql_rollback();
		sql_begin();

WRT_DEAL_MSG:
		ret = sql_execute("update pos_fee_mst set deal_sign='%c',\
			settle_date=%ld, deal_msg='%s' where area_code='%s' and \
			tx_date=%ld and clr_amt=%lf", deal_sign, g_pub_tx.tx_date, 
			deal_msg, sPos_fee_mst.area_code, sPos_fee_mst.tx_date,
			sPos_fee_mst.clr_amt);
		if (ret != 0)
		{
			sprintf(acErrMsg, "更新pos_fee_mst失败!! [%d]", ret);
			WRITEMSG
			return ret;
		}
	}
	Pos_fee_mst_Clo_Sel();

	sql_commit();
	sql_begin();

	return 0;
}


int pos_acct_to_114()
{
    int ret=0;
	double tx_amt=0.00,dr_amt=0.00, cr_amt=0.00, n_cr_amt=0.00, mater_amt=0.00;
	char dc_ind;
	char tmp_str[20];

	struct com_parm_c sCom_parm;
	struct pos_fee_deal_c sPos_fee_deal;
	struct pos_fee_mst_c sPos_fee_mst;

ret = Pos_fee_deal_Dec_Sel(g_pub_tx.reply, "fname='sxf' and deal_flag='Y' \
	order by pos_date");
if ( ret )
{
	sprintf(acErrMsg, "Declare for select error [%d]", ret);
	WRITEMSG
	goto ErrExit;
}

while(1)
{
	memset(&sPos_fee_deal, 0x0, sizeof(struct pos_fee_deal_c));
	ret = Pos_fee_deal_Fet_Sel(&sPos_fee_deal, g_pub_tx.reply);
	if ( ret == 100 )
	{
		break;
	}
	else if ( ret )
	{
		sprintf(acErrMsg, "Fetch for select error  [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c)); 
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));

	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	memset(def_br_no, 0x0, sizeof(def_br_no));
	memset(def_tel, 0x0, sizeof(def_tel));

	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);

	sprintf(acErrMsg, "-------->pos手续费交易日期:[%ld]", 
		sPos_fee_deal.pos_date);
	WRITEMSG

	/* 汇总借方发生额 *

	ret = sql_sum_double("pos_fee_mst_hst", "dr_amt", &dr_amt, 
		"deal_sign='Y' and settle_date=%ld ", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总pos手续费清算金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	
	ret = sql_sum_double("pos_fee_mst_hst", "cr_amt", &cr_amt, 
		"deal_sign='Y' and settle_date=%ld ", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总pos手续费清算金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	
	ret = sql_sum_double("pos_fee_mst_hst", "n_cr_amt", &n_cr_amt, 
		"deal_sign='Y' and settle_date=%ld ", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总pos手续费清算金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	
	ret = sql_sum_double("pos_fee_mst_hst", "mater_amt", &mater_amt, 
		"deal_sign='Y' and settle_date=%ld ", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总pos手续费清算金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	delete by rob at 20031005 

	tx_amt = dr_amt - cr_amt - n_cr_amt - mater_amt;*/

	/************
	ret = sql_count("pos_fee_mst", "tx_date=%ld and area_code='ZZS'",
		sPos_fee_deal.pos_date);
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count pos_fee_mst error [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 1 )
	{
		sprintf(acErrMsg, "请检查sxf文件数据是否正确[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "");
	}
	**********/

	memset(&sPos_fee_mst, 0x0, sizeof(struct pos_fee_mst_c));
	ret = Pos_fee_mst_Sel(g_pub_tx.reply, &sPos_fee_mst, "tx_date=%ld and \
		area_code='ZZS'", sPos_fee_deal.pos_date);
	if ( ret )
	{
		sprintf(acErrMsg, "查询POS 手续费表错误!![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	tx_amt = sPos_fee_mst.dr_amt - sPos_fee_mst.cr_amt \
		- sPos_fee_mst.n_cr_amt - sPos_fee_mst.mater_amt;

	sprintf(acErrMsg, "汇总 [%ld] pos手续费清算金额金额为:[%lf]",
		sPos_fee_mst.tx_date, tx_amt);
	WRITEMSG

	memset(tmp_str, 0x0, sizeof(tmp_str)); 
	sprintf(tmp_str, "%ld", sPos_fee_deal.pos_date); 
	sprintf(acErrMsg, " tmp_str = [%s]", tmp_str); 
	WRITEMSG 
	strcpy(tmp_str, tmp_str+4); 
	sprintf(acErrMsg, " tmp_str = [%s]", tmp_str); 
	WRITEMSG

	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	sprintf(g_pub_tx.brf, "手续费%s", tmp_str);
	if (pub_base_CompDblVal(tx_amt, 0.00) == 1) 
	{
		/* 借方多 */
		strcpy(g_pub_tx.add_ind, "1"); /* 40710减少 */
		g_pub_tx.tx_amt1 = tx_amt;
		dc_ind = '1';
	}
	else if (pub_base_CompDblVal(tx_amt, 0.00) == -1)
	{
		/* 贷方多 */
		strcpy(g_pub_tx.add_ind, "0"); /* 借40710增加 */
		g_pub_tx.tx_amt1 = -tx_amt;
		dc_ind = '0';
	}
	else
	{
		sprintf(acErrMsg, "本日40710的总发生额为零!!");
		WRITEMSG
		goto OkExit;
	}

	strcpy(g_pub_tx.ac_no, "40710"); 
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	/* 记40710会计帐 */
	if (dc_ind == '0')
	{
		/* 内部借40710 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '1')
	{
		/* 内部贷 A017 40710*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "记40710会计帐错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg, "*******114 ************");
	WRITEMSG

	/* 记114账号 */
	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c)); 
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c)); 
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c)); 
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c)); 
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c)); 
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); 
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c)); 
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c)); 
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}
	strcpy(def_br_no, "61010");
	strcpy(def_tel, "0008");
	strcpy(g_pub_tx.tx_br_no, def_br_no);
	strcpy(g_pub_tx.tel, def_tel);

	strcpy(g_pub_tx.cur_no, "01"); 
	strcpy(g_pub_tx.ct_ind, "2"); 
	sprintf(g_pub_tx.brf, "手续费%s", tmp_str);
	if (dc_ind == '0') /* 借 */
	{
		strcpy(g_pub_tx.add_ind, "1");
		g_pub_tx.tx_amt1 = -tx_amt;
	}
	else /* 贷 */
	{
		strcpy(g_pub_tx.add_ind, "0");
		g_pub_tx.tx_amt1 = tx_amt;
	}
	sprintf(acErrMsg, "------------>[%lf]", g_pub_tx.tx_amt1);
	WRITEMSG

	ret= Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='YDTD' and \
		parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sCom_parm.val);
	strcpy(g_pub_tx.ac_no, sCom_parm.val);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	sprintf(acErrMsg, "--------->[%s]", g_pub_tx.add_ind);
	WRITEMSG
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	if (dc_ind == '1') /* 40710贷 */
	{
		/* 内部借114 A016*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
	}
	else if (dc_ind == '0')
	{
		/* 内部贷114 A017*/
		sprintf(acErrMsg, "---------------------------------->111111");
		WRITEMSG
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}
	if (ret != 0)
	{
		sprintf(acErrMsg, "记114账号会计帐错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
} /* end for while */
Pos_fee_deal_Clo_Sel();

	/* 汇总借方发生额 *
	c_bal = 0.00;
	c_bal = 0.00;
	ret = sql_sum_double("dc_log", "amt", &c_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='1' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总40710的借方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log", "amt", &d_bal, "acc_hrt='407A0' and \
		tx_date=%ld and dc_ind='2' and sts='0'", g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "汇总40710的贷方金额错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	sprintf(acErrMsg, " --------> c_bal=[%lf], d_bal=[%lf]", c_bal, d_bal);
	WRITEMSG
	if (pub_base_CompDblVal(c_bal, d_bal) != 0)
	{
		sprintf(acErrMsg, "同城通兑的记账不平!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D251");
		goto ErrExit;
	}
	**/
	
OkExit:
    return 0;
ErrExit:
    return 1;
}
int rec_to_history()
{
	int ret;

	ret = sql_count("march_mst", "deal_sign='Y'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count error!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	if (ret > 0)
	{
		ret = sql_execute("insert into march_mst_hst select * from \
			march_mst where deal_sign = 'Y'");
		if (ret != 0)
		{
			sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
			WRITEMSG
			return ret;
		}

		ret = sql_execute("delete from march_mst where deal_sign = 'Y'");
		if (ret != 0)
		{
			sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
			WRITEMSG
			return ret;
		}
	}

	ret = sql_count("march_mst_mx", "1=1");
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count error!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	if (ret > 0)
	{
		ret = sql_execute("insert into march_mst_mx_hst select * from \
			march_mst_mx where 1=1");
		if (ret != 0)
		{
			sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
			WRITEMSG
			return ret;
		}

		ret = sql_execute("delete from march_mst_mx where 1=1");
		if (ret != 0)
		{
			sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
			WRITEMSG
			return ret;
		}
	}

	ret = sql_count("pos_fee_mst", "deal_sign='Y'");
	if (ret < 0)
	{
		sprintf(acErrMsg, "sql_count error!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	if (ret > 0)
	{
		ret = sql_execute("insert into pos_fee_mst_hst select * from \
			pos_fee_mst where deal_sign='Y'");
		if (ret != 0)
		{
			sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
			WRITEMSG
			return ret;
		}

		ret = sql_execute("delete from pos_fee_mst where deal_sign='Y'");
		if (ret != 0)
		{
			sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
			WRITEMSG
			return ret;
		}
	}

	ret = sql_execute("delete from pos_fee_deal where deal_flag='Y'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "sql_execute error !! [%d]", ret);
		WRITEMSG
		return ret;
	}
	return 0;
}

int march_pos_hd()
{
	int ret = 0;
	int opncnt=0;
	FILE    *fp;
	int     line=0;
	char fname[100];
	char filename[100];
	char vpfm[21]; 
	char old_br_no[6];
	char new_br_no[6];
	
	memset(&g_sMarch_mst, 0x0, sizeof(struct march_mst_c));
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));
	memset(old_br_no, 0x0, sizeof(old_br_no));
	memset(new_br_no, 0x0, sizeof(new_br_no));

	/****
	sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"),
			"POSM","posmhd",g_pub_tx.tx_date);
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret); 
		WRITEMSG 
		return ret;
	} 
	****/
	
	strcpy(vpfm, "posmhd");

	ret = March_mst_Dec_Sel(g_pub_tx.reply, "deal_sign='Y' order by \
		opn_br_no, tx_date");
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret = March_mst_Fet_Sel(&g_sMarch_mst, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			break; 
		}

		sprintf( filename,"%s", "BRposmhd");
		pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 

		sprintf(acErrMsg, "************CGH********************");
		WRITEMSG

		strcpy(new_br_no, g_sMarch_mst.opn_br_no);
		if (strcmp(old_br_no, new_br_no) != 0)
		{
			strcpy(old_br_no, new_br_no);
			line = 0;
			page = 1;
		}

		if (line + 15 > total_line)
		{
			fprintf(fp, "@HY@\n"); 
			line = 0;
			page = page + 1;
		}
		
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
				get_ratio_posmhd) ) 
			goto ErrExit; 
		
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
				get_ratio_posmhd) ) 
			goto ErrExit; 
			
		fprintf(fp, "\n\n\n"); 
		line = line + 3;
		fclose(fp);
	} 
	
	March_mst_Clo_Sel();

	return 0;
ErrExit:
	return 1;
}

int march_pos_list()
{
	int ret = 0; 
	int flag=0; 
	int opncnt; 
	char vpfm[21]; 
	int     line=0; 
	char fname[100]; 
	char filename[100]; 
	FILE    *fp; 
	char new_br_no[6];
	char old_br_no[6];

	memset(vpfm, 0x0, sizeof(vpfm));
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(new_br_no, 0x0, sizeof(new_br_no));
	memset(old_br_no, 0x0, sizeof(old_br_no));
	
	/*****
	sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"), 
			"POSM","posmlist",g_pub_tx.tx_date); 
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret);
		WRITEMSG 
		return ret; 
	} 
	*****/
	
	strcpy(vpfm, "posmlist"); 
	
	ret = March_mst_Dec_Sel(g_pub_tx.reply, "deal_sign='Y' order by \
		opn_br_no, tx_date"); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret = March_mst_Fet_Sel(&g_sMarch_mst, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			break; 
		}
		
		sprintf( filename,"%s","BRposmlist");
		pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 

		pub_base_strpack(g_sMarch_mst.opn_br_no);
		strcpy(new_br_no, g_sMarch_mst.opn_br_no);

		if (strcmp(new_br_no, old_br_no) != 0)
		{
			strcpy(old_br_no, new_br_no);
			line = 0;
			page = 1;
		}

		if (line + 10 > total_line)
		{
			fprintf(fp, "@HY@\n"); 
			line = 0;
			page = page + 1;
		}

		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
			get_ratio_posmhd) ) 
			goto ErrExit; 

		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
			get_ratio_posmhd) ) 
			goto ErrExit; 
			
		ret = March_mst_mx_Dec_Sel(g_pub_tx.reply, "march_code = '%s' and \ 
				tx_date = %ld", g_sMarch_mst.march_code, g_sMarch_mst.tx_date); 
		if (ret != 0) 
		{ 
			sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret); 
			WRITEMSG 
			return ret; 
		} 
		
		while(1) 
		{ 
			ret = March_mst_mx_Fet_Sel(&g_sMarch_mst_mx, g_pub_tx.reply); 
			if (ret != 0 && ret != 100) 
			{ 
				sprintf(acErrMsg, "FETCH FOR SELECT ERROR [%d]", ret); 
				WRITEMSG 
				return ret; 
			} 
			else if (ret == 100) 
			{ 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 

				fprintf(fp, "\n\n\n"); 
				line = line + 3;
				fclose(fp);
				break; 
			} 

			if (line + 2 > total_line)
			{
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_posmhd) ) 
					goto ErrExit; 
				fprintf(fp, "@HY@\n"); 
				line = 0;
				page = page + 1;
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
			}
			
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_posmhd) ) 
				goto ErrExit;
		} 
		
		March_mst_mx_Clo_Sel(); 
		
	} 
	
	March_mst_Clo_Sel();

	return 0;
ErrExit:
	return 1;
}

int march_pos_hzqd()
{
	int ret = 0; 
	int flag=0; 
	int opncnt=0; 
	char vpfm[21]; 
	int  line=0; 
	char fname[100]; 
	char filename[100]; 
	FILE    *fp; 
	char tmp_str[60];
	char new_br_no[6];
	char old_br_no[6];
	char ac_no[20];
	char new_ac_no[20];

	memset(vpfm, 0x0, sizeof(vpfm));
	memset(filename, 0x0, sizeof(filename));
	memset(fname, 0x0, sizeof(fname));
	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(new_br_no, 0x0, sizeof(new_br_no));
	memset(old_br_no, 0x0, sizeof(old_br_no));
	memset(ac_no, 0x0, sizeof(ac_no));
	memset(new_ac_no, 0x0, sizeof(new_ac_no));

	/*** 	
	sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"), 
		"POSM","posmhzqd",g_pub_tx.tx_date); 
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret); 
		WRITEMSG 
		return ret; 
	} 
	*****/
	
	strcpy(vpfm, "posmhzqd"); 
	page = 1;
	
	ret = March_mst_Dec_Sel(g_pub_tx.reply, "1=1 order by opn_br_no, ac_no"); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret = March_mst_Fet_Sel(&g_sMarch_mst, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			if (strcmp(old_br_no, "") != 0)
			{
				pub_rpt_openfile( &fp, old_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
				fclose(fp);
			}
			break; 
		} 

		sprintf( filename,"%s","BRposmhzqd");

		pub_base_strpack(g_sMarch_mst.opn_br_no);
		strcpy(new_br_no, g_sMarch_mst.opn_br_no);
		strcpy(new_ac_no, g_sMarch_mst.ac_no);
		if (strcmp(new_br_no, old_br_no) != 0)
		{
			if (strcmp(old_br_no, "") != 0)
			{
				pub_rpt_openfile( &fp, old_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
				fclose(fp);
			}
			strcpy(old_br_no, new_br_no);
			strcpy(ac_no, new_ac_no);
			line = 0;
			page = 1;

			pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
	
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_posmhd) ) 
				goto ErrExit; 
		
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_posmhd) ) 
				goto ErrExit; 
			fclose(fp);
		}
		else
		{
			if (strcmp(new_ac_no, ac_no) != 0)
			{
				pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
		
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
				fclose(fp);
				strcpy(ac_no, new_ac_no);
			}
		}

		pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
		if (g_sMarch_mst.deal_sign[0] == 'Y') 
		{ 
			strcpy(g_sMarch_mst.deal_msg, "已记帐"); 
		} 
		else 
		{ 
			strcpy(tmp_str, "未记帐"); 
			strcat(tmp_str, g_sMarch_mst.deal_msg); 
			strcpy(g_sMarch_mst.deal_msg, tmp_str); 
		} 


		
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
			get_ratio_posmhd) ) 
			goto ErrExit; 
		fclose(fp);
	} 
	
	March_mst_Clo_Sel(); 
	

	return 0;
ErrExit:
	return 1;
}

int march_pos_trqd()
{
	int ret = 0; 
	int flag=0; 
	int opncnt=0; 
	char vpfm[21]; 
	int     line=0; 
	char fname[100]; 
	char filename[100]; 
	FILE    *fp; 
	char tmp_str[60]; 
	char new_br_no[6];
	char old_br_no[6];
	char new_mrchno[21];
	char old_mrchno[21];

	memset(vpfm, 0x0, sizeof(vpfm));
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(new_br_no, 0x0, sizeof(new_br_no));
	memset(old_br_no, 0x0, sizeof(old_br_no));
	memset(new_mrchno, 0x0, sizeof(new_mrchno));
	memset(old_mrchno, 0x0, sizeof(old_mrchno));
		
	/***
	sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"), 
		"POSM","posmtrqd",g_pub_tx.tx_date); 
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret); 
		WRITEMSG 
		return ret; 
	} 
	***/
	
	strcpy(vpfm, "posmtrqd"); 
	
	ret = March_mst_Dec_Sel(g_pub_tx.reply, "1=1 order by opn_br_no, tx_date"); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret = March_mst_Fet_Sel(&g_sMarch_mst, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			if (strcmp(old_br_no, "") != 0)
			{
				pub_rpt_openfile( &fp, old_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_posmhd) ) 
					goto ErrExit; 
				fclose(fp);
			}
			break; 
		} 
		
		sprintf( filename,"%s","BRposmtrqd");

		pub_base_strpack(g_sMarch_mst.opn_br_no); 
		strcpy(new_br_no, g_sMarch_mst.opn_br_no);

		if (strcmp(new_br_no, old_br_no) != 0) 
		{ 
			if (strcmp(old_br_no,"") != 0) 
			{ 
				pub_rpt_openfile( &fp, old_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_posmhd) ) 
					goto ErrExit; 
				fclose(fp);
			} 
			strcpy(old_br_no, new_br_no); 
			pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_posmhd) ) 
				goto ErrExit; 

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_posmhd) ) 
				goto ErrExit; 
			fclose(fp);
		} 

		/***
		pub_base_strpack(g_sMarch_mst.march_code);
		strcpy(new_mrchno, g_sMarch_mst.march_code);
		if (strcmp(new_mrchno, old_mrchno) != 0)
		{
			if (strcmp(old_mrchno, "") != 0)
			{
				pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_posmhd) ) 
					goto ErrExit; 
				fclose(fp);
			}
			pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_posmhd) ) 
				goto ErrExit; 

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_posmhd) ) 
				goto ErrExit; 
			fclose(fp);
		}
		****/
		
		pub_rpt_openfile( &fp, g_sMarch_mst.opn_br_no, filename); 
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_posmhd) ) 
			goto ErrExit; 
		fclose(fp);
	} 

	March_mst_Clo_Sel(); 
	

	return 0;
ErrExit:
	return 1;
}

int get_ratio_posmhd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 

	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': 
			strcpy(str, g_sMarch_mst.ac_no); 
			break; 
		case 'B': 
			strcpy(str, "特约商户资金清算转入"); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, g_sMarch_mst.tx_amt);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, g_sMarch_mst.march_name); 
			break; 
		case 'E': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, pub_rpt_pcask(g_sMarch_mst.tx_amt)); 
			break; 
		case 'F':
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, g_sMarch_mst_mx.card_no); 
			break; 
		case 'G': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tx_amt); 
			break; 
		case 'H': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tx_date); 
			break; 
		case 'I': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tx_time); 
			break; 
		case 'J': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.trace_no); 
			break; 
		case 'K': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tty_no); 
			break; 
		case 'L': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.opn_tx_br_name); 
			break; 
		case 'M': 
			sprintf(fmt, "%%%dld",bll); 
			sprintf(str, fmt, g_sMarch_mst.tx_date); 
			break; 
		case 'N': 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf(str, fmt, g_sMarch_mst.deal_msg); 
			break; 
		case 'O': 
			strcpy( str, "POS"); 
			break; 
		case 'P': 
			sprintf(fmt, "%%-%ds",bll); 
			sprintf(str, fmt, g_sMarch_mst.opn_br_no); 
			break; 
		case 'Q': 
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'", 
					g_sMarch_mst.opn_br_no); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d][%s]", 
					ret, g_sMarch_mst.opn_br_no); 
				WRITEMSG 
				goto ErrExit; 
			} 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf(str, fmt, sCom_branch.br_name); 
			break;
		case 'X': 
			sprintf( str, "%04ld", g_pub_tx.tx_date/10000 ); 
			break; 
		case 'W': 
			sprintf( str, "%02ld", g_pub_tx.tx_date/100%100 ); 
			break; 
		case 'V': 
			sprintf( str, "%02ld", g_pub_tx.tx_date%100 ); 
			break; 
		case '1': 
			sprintf( str, "%08ld", g_pub_tx.tx_date); 
			break; 
		case '2': 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf( str, fmt, g_sMarch_mst.march_code ); 
			break; 
		case '3': 
			sprintf( str, "%02ld", page); 
			break; 
		case '4': 
			sprintf(fmt, "%%-%d.6lf", bll); 
			sprintf( str, fmt, g_sMarch_mst.fee_rate); 
			break; 
		case '5': 
			sprintf( str, "%08ld", g_sMarch_mst.tx_cnt); 
			break; 
		case '6': 
			sprintf(fmt, "%%-%d.2lf", bll); 
			sprintf( str, fmt, g_sMarch_mst.tx_amt); 
			break; 
		case '7': 
			sprintf(fmt, "%%-%d.2lf", bll); 
			sprintf( str, fmt, g_sMarch_mst.fee_amt); 
			break; 
		case '8': 
			sprintf(fmt, "%%-%d.2lf", bll); 
			sprintf( str, fmt, g_sMarch_mst.real_amt); 
			break; 
		case 'm': 
			sprintf(fmt, "%%%dld", bll); 
			sprintf( str, fmt, g_pub_tx.tx_date); 
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
