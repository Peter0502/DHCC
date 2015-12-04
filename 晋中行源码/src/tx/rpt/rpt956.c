/*************************************************************
* 文 件 名: rpt956.c
* 功能描述：晋中二级子账户满页帐
*
* 作    者: z.s.wang
* 完成日期: 2012年03月09日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
insert into com_eod_exec values ('6005','R956','rpt956','二级子账户满页帐','1111111111','1','01','01','2','Y','Y','N','2','Y');
**************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define  EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "xqdz_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"

#define MAX_CNT 50  /* 设置满页的条目数 */

FILE   *fp_full;
FILE   *fp_not_full;
long   tmp_bgn_date;
long   page_cnt;
long   tmp_flag;
double dD_amt;
double dJ_amt;
char   page_title[64+1];
struct com_sys_parm_c s_com_sys_parm;
struct sub_dd_mst_c   s_sub_dd_mst;
struct xqdz_c         s_xqdz;

rpt956()
{
	fp_full=NULL;
	fp_not_full=NULL;
	int ret=-1;
	int  i=0;
	int  tmp_cnt=0;
	int  tmp_page=0;
	long tmp_line=0;
	char filename[20+1];
	char filename_not_full[20+1];
	char tmp_filename_full[128+1];
	char tmp_filename_not_full[128+1];
	char old_br_no[5+1];
	char old_ac_no[20];

	memset(filename,0x00,sizeof(filename));
	memset(filename_not_full,0x00,sizeof(filename_not_full));
	memset(tmp_filename_full,0x00,sizeof(tmp_filename_full));
	memset(tmp_filename_not_full,0x00,sizeof(tmp_filename_not_full));
	memset(old_br_no,0x00,sizeof(old_br_no));
	memset(page_title,0x00,sizeof(page_title));
	memset(&s_com_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&s_xqdz,0x00,sizeof(struct xqdz_c));

	strcpy( filename , "RPT956");
	strcpy( filename_not_full,"RPT956_NOT_FULL");

	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	vtcp_log("---生成报表rpt956(二级子账户满页帐)开始---");
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
	if(ret)
	{
		vtcp_log("查询系统日期出错! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		goto ErrExit;
	}

	tmp_bgn_date=s_com_sys_parm.lst_date/100*100+1;
	vtcp_log("得到的系统日期 sys_date:[%ld] tmp_bgn_date:[%ld] [%s] [%d]",\
		s_com_sys_parm.lst_date,tmp_bgn_date,__FILE__,__LINE__);

	ret=Sub_dd_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by br_no,ac_no,sub_ac_no");
	if(ret)
	{
		vtcp_log("声明Sub_dd_mst查询条件错误 [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		goto ErrExit;
	}

	while(1)
	{
		tmp_flag=0;
		dD_amt=0.0;
		dJ_amt=0.0;
		memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		memset(page_title,0x00,sizeof(page_title));
		ret=Sub_dd_mst_Fet_Sel(&s_sub_dd_mst,g_pub_tx.reply);
		if(ret==100)
		{
			vtcp_log("提取数据结束.[%s] [%d]",__FILE__,__LINE__);
			break;
		}
		else if(ret)
		{
			vtcp_log("提取数据异常.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
			goto ErrExit;
		}
		page_cnt=s_sub_dd_mst.hst_pg;
		if(tmp_bgn_date%10000==101)
		{
			page_cnt=0;
		}
		if(strcmp(old_br_no,s_sub_dd_mst.br_no))
		{
			if(fp_full != NULL) fclose(fp_full);
			if(fp_not_full != NULL) fclose(fp_not_full);
			memset(tmp_filename_full,0x00,sizeof(tmp_filename_full));
			memset(tmp_filename_not_full,0x00,sizeof(tmp_filename_not_full));
			sprintf(tmp_filename_full,"%s/report/%s/%s.txt",getenv("HOME"),s_sub_dd_mst.br_no,filename);
			sprintf(tmp_filename_not_full,"%s/report/%s/%s.txt",getenv("HOME"),s_sub_dd_mst.br_no,filename_not_full);
			vtcp_log("filename_full is:[%s] [%s] [%d]",tmp_filename_full,__FILE__,__LINE__);
			vtcp_log("filename_not_full is:[%s] [%s] [%d]",tmp_filename_not_full,__FILE__,__LINE__);
			pub_rpt_rmfile(s_sub_dd_mst.br_no,filename,0 );
			pub_rpt_rmfile(s_sub_dd_mst.br_no,filename_not_full,0 );
			strcpy(old_br_no,s_sub_dd_mst.br_no);
		}

		if(strcmp(old_ac_no,s_sub_dd_mst.ac_no))
		{
			memset(&s_xqdz,0x00,sizeof(struct xqdz_c));
			ret=Xqdz_Sel(g_pub_tx.reply,&s_xqdz,"xqdz100='%s'",s_sub_dd_mst.ac_no);
			if(ret)
			{
				vtcp_log("查询小区信息异常! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				goto ErrExit;
			}
		}

		tmp_cnt=sql_count("sub_dd_mst_hst","sub_ac_no='%s' and tx_date>=%ld  and tx_date <=%ld",\
			s_sub_dd_mst.sub_ac_no,tmp_bgn_date,s_com_sys_parm.lst_date);

		if(tmp_cnt==0)
		{
			vtcp_log("没有满足条件的记录,[%s] [%s] [%d]",s_sub_dd_mst.sub_ac_no,__FILE__,__LINE__);
			continue;
		}
		else if(tmp_cnt<MAX_CNT)
		{
			if(fp_not_full == NULL)
			{
				fp_not_full=fopen(tmp_filename_not_full,"w+");
			}

			strcpy(page_title,"                            晋中银行未满页账\n");
			/* 不足的直接打印至not_full文件里面 */
			ret=rpt956_print(fp_not_full,s_sub_dd_mst.sub_ac_no,0,&tmp_line);
			if(ret)
			{
				vtcp_log("打印不足条数的当月满月帐出错! [%s] [%d]",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}
		}
		else
		{
			tmp_line=0;
			tmp_page=tmp_cnt/MAX_CNT;

			if(fp_full == NULL)
			{
				fp_full=fopen(tmp_filename_full,"w+");
			}
			if(tmp_cnt%MAX_CNT != 0 && fp_not_full == NULL)
			{
				fp_not_full=fopen(tmp_filename_not_full,"w+");
			}

			strcpy(page_title,"                            晋中银行满页账\n");

			for(i=0;i<tmp_page;i++)
			{
				/* 数目等于MAX_CNT 就打印一页 */
				ret=rpt956_print(fp_full,s_sub_dd_mst.sub_ac_no,tmp_line,&tmp_line);
				if(ret)
				{
					vtcp_log("打印不足条数的当月满月帐出错! [%s] [%d]",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"L207");
					goto ErrExit;
				}
			}

			if(tmp_cnt%MAX_CNT != 0)
			{
				memset(page_title,0x00,sizeof(page_title));
				strcpy(page_title,"                            晋中银行未满页账\n");
				/* 不足的直接打印至not_full文件里面 */
				ret=rpt956_print(fp_not_full,s_sub_dd_mst.sub_ac_no,tmp_line,&tmp_line);
				if(ret)
				{
					vtcp_log("打印不足条数的当月满月帐出错! [%s] [%d]",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"L207");
					goto ErrExit;
				}
			}
		}

		if(page_cnt != s_sub_dd_mst.hst_pg)
		{
			ret=sql_execute("update sub_dd_mst set hst_pg=%ld where sub_ac_no='%s'",page_cnt,s_sub_dd_mst.sub_ac_no);
			if(ret)
			{
				vtcp_log("更新sub_dd_mst的hst_pg失败![%d] [%s] [%d]",ret,__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P434");
				goto ErrExit;
			}
		}
	}
	Sub_dd_mst_Clo_Sel();
	vtcp_log("---生成报表rpt956(二级子账户满页帐)结束---");

OkExit:
	vtcp_log("[%s][%d]提交数据库!\n",__FILE__,__LINE__);
	sql_commit();   /*--提交事务--*/
	vtcp_log("---生成二级子账户满页帐 成功 ---,[%s] [%d]",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return (0);

ErrExit:
	Sub_dd_mst_Clo_Sel();
	if(fp_full != NULL) fclose(fp_full);
	if(fp_not_full != NULL) fclose(fp_not_full);
	vtcp_log("---生成二级子账户满页帐 失败 !!! ---,[%s] [%d]",__FILE__,__LINE__);
	if( strlen(g_pub_tx.reply)==0 || !strcmp(g_pub_tx.reply,"0000"))
	{
		strcpy(g_pub_tx.reply,"L207");
	}
	set_zd_data("0120",g_pub_tx.reply);
	return (1);
}

int rpt956_print(FILE *fp,char* sub_ac_no,long icnt_bgn,long* ocnt_end)
{
	int ret=-1;
	int cnt=0;
	double dBal=0.0;
	char j_str[13+1];
	char d_str[13+1];
	struct sub_dd_mst_hst_c s_sub_dd_mst_hst;
	memset(j_str,0x00,sizeof(j_str));
	memset(d_str,0x00,sizeof(d_str));
	memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));

	ret=Sub_dd_mst_hst_Dec_Sel(g_pub_tx.reply,"sub_ac_no='%s' and tx_date>=%ld \
		and tx_date <=%ld and hst_cnt>=%ld",sub_ac_no,tmp_bgn_date,s_com_sys_parm.lst_date,icnt_bgn);
	if(ret)
	{
		vtcp_log("声明Sub_dd_mst_hst错误. [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	while(1)
	{
		char ct_flag[2+1];
		char brf[64+1];
		memset(j_str,0x00,sizeof(j_str));
		memset(d_str,0x00,sizeof(d_str));
		memset(ct_flag,0x00,sizeof(ct_flag));
		memset(brf,0x00,sizeof(brf));
		struct sub_dd_mst_hst_c s_sub_dd_mst_hst;
		memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
		ret=Sub_dd_mst_hst_Fet_Sel(&s_sub_dd_mst_hst,g_pub_tx.reply);
		if(ret==100 || cnt == MAX_CNT)
		{
			fprintf(fp,"└────┴──────────┴────┴────────┴───────┴───────┴───────┴───┘\n");
			fprintf(fp,"本页借方小计:%-16.2f      本页贷方小计:%-16.2f    \n",dJ_amt,dD_amt);
			fprintf(fp,"%c\n",'\f');
			*ocnt_end=s_sub_dd_mst_hst.hst_cnt;
			break;
		}
		else if(ret)
		{
			vtcp_log("提取Sub_dd_mst_hst错误. [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		if(cnt==0)
		{
			page_cnt++;
			fprintf(fp,"\n");
			fprintf(fp,page_title);
			fprintf(fp,"\n");
			fprintf(fp,"小区名称:%-50s\n",s_xqdz.xqdz15);
			fprintf(fp,"主账户:%-20.20s     子账户:%-20.20s   户名:%-30.30s   第%3d页\n",s_sub_dd_mst.ac_no,s_sub_dd_mst.sub_ac_no,s_sub_dd_mst.name,page_cnt);
			fprintf(fp,"┌────┬──────────┬────┬────────┬───────┬───────┬───────┬───┐\n");
			fprintf(fp,"│交易日期│    摘        要    │凭证种类│    凭证号码    │  借方发生额  │  贷方发生额  │   余    额   │柜  员│\n");
			fprintf(fp,"├────┼──────────┼────┼────────┼───────┼───────┼───────┼───┤\n");

			if(s_sub_dd_mst_hst.add_ind[0]=='0')
			{
				dBal=s_sub_dd_mst_hst.bal+s_sub_dd_mst_hst.tx_amt;
			}
			else
			{
				dBal=s_sub_dd_mst_hst.bal-s_sub_dd_mst_hst.tx_amt;
			}

			if(s_sub_dd_mst_hst.hst_cnt==1)
			{
			}
			else if(tmp_bgn_date%10000==101 && page_cnt==1 )
			{
				fprintf(fp,"│        │承上年              │        │                │              │              │%14.2f│      │\n",dBal);
			}
			else
			{
				if(tmp_flag==0)
				{
					fprintf(fp,"│        │承上页              │        │                │              │              │%14.2f│      │\n",dBal);
				}
				else
				{
					fprintf(fp,"│%8d│承上页              │        │                │%14.2f│%14.2f│%14.2f│      │\n",s_sub_dd_mst_hst.tx_date,dJ_amt,dD_amt,dBal);
				}
			}
		}
		if(s_sub_dd_mst_hst.ct_ind[0]=='1')
		{
			strcpy(ct_flag,"现");
		}
		else if(s_sub_dd_mst_hst.ct_ind[0]=='2')
		{
			strcpy(ct_flag,"转");
		}

		if(s_sub_dd_mst_hst.add_ind[0]=='0')
		{
			sprintf(j_str,"%s%12.2f",ct_flag,s_sub_dd_mst_hst.tx_amt);
			dJ_amt+=s_sub_dd_mst_hst.tx_amt;
		}
		else
		{
			sprintf(d_str,"%s%12.2f",ct_flag,s_sub_dd_mst_hst.tx_amt);
			dD_amt+=s_sub_dd_mst_hst.tx_amt;
		}

		Del_half_wrod(s_sub_dd_mst_hst.brf,brf);
		fprintf(fp,"│%8d│%-20.20s│%8.8s│%16.16s│%14.14s│%14.14s│%14.2f│%6.6s│\n",
		s_sub_dd_mst_hst.tx_date,brf,s_sub_dd_mst_hst.note_type,s_sub_dd_mst_hst.note_no,j_str,d_str,s_sub_dd_mst_hst.bal,s_sub_dd_mst_hst.tel);
		cnt++;
	}
	tmp_flag++;
	Sub_dd_mst_hst_Clo_Sel();

OkExit:
	return(0);
ErrExit:
	Sub_dd_mst_hst_Clo_Sel();
	return(-1);
}
