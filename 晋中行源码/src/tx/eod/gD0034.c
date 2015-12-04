/*************************************************
* 文 件 名:     gD0034.c
* 功能描述：    二级子账户总分检查
* 作    者:
* 完成日期：    2012年03月12日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"

int gD0034()
{
	int ret=-1;
	int  t_num=0;
	double sum_sub_bal=0.0;
	double sum_jamt=0.0;
	double sum_damt=0.0;
	double sum_sub_jamt=0.0;
	double sum_sub_damt=0.0;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct dd_mst_c s_dd_mst;
	struct sub_dd_mst_c s_sub_dd_mst;

	memset(&s_mdm_ac_rel ,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	vtcp_log("---二级子账户总分检查开始---");

	ret=sql_begin(); /* 打开事物 */
	if ( ret )
	{
		vtcp_log("打开事物失败! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT03");
		goto ErrExit;
	}

	pub_base_sysinit(); /* 初始化全局变量 */

	if(pub_base_PubQlsh( &g_pub_tx.trace_no, g_pub_tx.reply))
	{
		vtcp_log("取主机号流水出错 [%d] [%s]",g_pub_tx.trace_no,g_pub_tx.reply);
		strcpy(g_pub_tx.reply,"P298");
		goto ErrExit;
	}

	ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply, "draw_pwd_chg='1' and substr(ac_no,1,1)='5' and note_sts='0'");
	if (ret)
	{
		vtcp_log("从mdm_ac_rel查询帐号出错 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D169");
	}

	while(1)
	{
		sum_sub_bal=0.0;
		memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
		ret=Mdm_ac_rel_Fet_Sel(&s_mdm_ac_rel,g_pub_tx.reply);
		if(ret==100)
		{
			vtcp_log("提取记录结束 总共:[%d] [%s] [%d]",t_num,__FILE__,__LINE__);
			break;
		}
		else if(ret)
		{
			vtcp_log("提取数据出错! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			goto ErrExit;
		}

		/* 比较余额是否相等 */
		ret=Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst, "ac_id=%ld", s_mdm_ac_rel.ac_id);
		if(ret)
		{
			vtcp_log("查询活期主文件出错! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		vtcp_log("开始查询sub_dd_mst中该对公账户是否设立子账户");
		/**BEGIN 查询sub_dd_mst中该对公账户是否设立子账户，如果无子账户则跳过不做总分检查 BY LZY 20140310**/
		ret=Sub_dd_mst_Sel( g_pub_tx.reply, &s_sub_dd_mst, "ac_no='%s' and ac_sts='1'", s_mdm_ac_rel.ac_no);
		vtcp_log("ret:[%d]",ret);
		if( ret && ret!=100)
		{
			vtcp_log("查询Sub_dd_mst出错 帐号:[%s] [%d] [%s] [%d]", s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}
		else if(ret==100 )
		{
			vtcp_log("查询Sub_dd_mst无此账号,账号:[%s] [%s] [%d]",s_mdm_ac_rel.ac_no,__FILE__,__LINE__);
			continue;
		}
		/**END 查询sub_dd_mst中该对公账户是否设立子账户，如果无子账户则跳过不做总分检查 BY LZY 20140310**/
		vtcp_log("结束查询sub_dd_mst中该对公账户是否设立子账户");

		ret=sql_sum_double( "sub_dd_mst","bal", &sum_sub_bal, "ac_no='%s' and ac_sts='1'", s_mdm_ac_rel.ac_no);
		if( ret )
		{
			vtcp_log("查询子账户金额合计出错 帐号:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		vtcp_log("主帐号:[%s] 主帐号金额:[%lf] 子账户合计金额:[%lf]",s_mdm_ac_rel.ac_no,s_dd_mst.bal,sum_sub_bal);
		if(s_dd_mst.bal != sum_sub_bal)
		{
			vtcp_log("主账户余额 与 子账号合计金额 不符 [%s] [%d]",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"H019");
			goto ErrExit;
		}

		/* 比较当天流水的借贷金额 */
		ret=sql_sum_double( "dd_mst_hst","tx_amt",&sum_jamt, "ac_id=%ld and ac_seqn=%ld and tx_date=%ld and add_ind='0'",\
			s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("查询当日主账户交易明细出错! 帐号:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		ret=sql_sum_double( "dd_mst_hst","tx_amt",&sum_damt, "ac_id=%ld and ac_seqn=%ld and tx_date=%ld and add_ind='1'",\
			s_mdm_ac_rel.ac_id,s_mdm_ac_rel.ac_seqn,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("查询当日主账户交易明细出错! 帐号:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		ret=sql_sum_double( "sub_dd_mst_hst","tx_amt",&sum_sub_jamt, "ac_no=%s and tx_date=%ld and add_ind='0'",\
			s_mdm_ac_rel.ac_no,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("查询当日子账户交易明细出错! 帐号:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		ret=sql_sum_double( "sub_dd_mst_hst","tx_amt",&sum_sub_damt, "ac_no=%s and tx_date=%ld and add_ind='1'",\
			s_mdm_ac_rel.ac_no,g_pub_tx.tx_date);
		if( ret )
		{
			vtcp_log("查询当日子账户交易明细出错! 帐号:[%s] [%d] [%s] [%d]",s_mdm_ac_rel.ac_no,ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D169");
			goto ErrExit;
		}

		vtcp_log("主账户当日 借方金额:[%lf] 贷方金额:[%lf] 子账户当日 借方金额:[%lf] 贷方金额:[%lf]",\
			sum_jamt,sum_damt,sum_sub_jamt,sum_sub_damt);

		if( (sum_jamt - sum_damt) != (sum_sub_jamt - sum_sub_damt))
		{
			vtcp_log("主账户与子账户 当日明细借贷金额不符 请检查! 主账户:[%lf] 子账户[%lf] [%s] [%d]",\
				(sum_jamt - sum_damt),(sum_sub_jamt - sum_sub_damt),__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"H019");
			goto ErrExit;
		}

		t_num++;
	}

	vtcp_log("---二级子账户总分检查结束---");

OkExit:
	strcpy(g_pub_tx.reply,"0000");
  vtcp_log("二级子账户总分检查成功!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(0);
ErrExit:
	if(strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0] == '\0')
	{
		strcpy(g_pub_tx.reply, "G010");
	}
	sql_rollback();		/* 事务回滚 */
	vtcp_log("二级子账户总分检查失败!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(1);
}






