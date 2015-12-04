#include <stdio.h>
#define EXTERN
#include "public.h"
#include "xqdz_c.h"
#include "sub_dd_mst_c.h"
#include "xqdz_c.h"
#include "sub_dd_mst_hst_c.h"

sp8129()
{
	char   ac_no[25],xqno[11],filename[100],wherelist[512],list2[512],j_str[30],d_str[30],ct_flag[3];
	long   start_date=0,end_date=0,sub_ac_seqn=0,cnt=0,lDate=0,page;
	FILE   *fp=NULL;
	double dD_amt=0.0,dJ_amt=0.0,dBal=0.0;
	struct sub_dd_mst_c        sSub_dd_mst;
	struct sub_dd_mst_hst_c    sSub_dd_mst_hst;
	struct xqdz_c              sXqdz;
	char   brf[40];

	char   note_type[9],note_no[17];
	memset(note_type,0x00,sizeof(note_type));
	memset(note_no,  0x00,sizeof(note_no));

	memset(ac_no,0x00,sizeof(ac_no));
	memset(xqno,0x00,sizeof(xqno));
	memset(filename,0x00,sizeof(filename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(j_str,0x00,sizeof(j_str));
	memset(d_str,0x00,sizeof(d_str));
	memset(list2,0x00,sizeof(list2));
	memset(ct_flag,0x00,sizeof(ct_flag));
	memset(brf,0x00,sizeof(brf));

	pub_base_sysinit();
	get_zd_data("0300",ac_no);
	get_zd_data("0300",g_pub_tx.ac_no);
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_data("0860",xqno);
	get_zd_long("0280",&sub_ac_seqn);
	get_zd_long("0460",&start_date);
	get_zd_long("0470",&end_date);
	/* 生成下传全路经文件名 */
	pub_base_AllDwnFilName(filename);

	fp = fopen(filename, "w");
	if (fp == NULL) {
	    sprintf(acErrMsg, "open file error [%s]", filename);
	    WRITEMSG
	        strcpy(g_pub_tx.reply, "S047");
	    goto ErrExit;
	}

	sprintf(wherelist,"ac_no='%s' ",g_pub_tx.ac_no);
	if(sub_ac_seqn>0)
	{
		sprintf(wherelist+strlen(wherelist)," and sub_ac_seqn='%ld' ",sub_ac_seqn);
	}
	if(strlen(xqno)>0)
	{
		sprintf(wherelist+strlen(wherelist)," and xq_num='%s' ",xqno);
	}

	g_reply_int=Sub_dd_mst_Dec_Sel(g_pub_tx.reply,wherelist);
	if(g_reply_int)
	{
			vtcp_log("[%s][%d]Sub_dd_mst_Fet_Sel错误",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
	}
	while(1)
	{
		memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
		g_reply_int=Sub_dd_mst_Fet_Sel(&sSub_dd_mst,g_pub_tx.reply);
		if(g_reply_int==100)
		{
			break;
		}
		else if(g_reply_int)
		{
			vtcp_log("[%s][%d]Sub_dd_mst_Fet_Sel错误",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		memset(&sXqdz,0x00,sizeof(sXqdz));
		if(strlen(xqno)>0)
		{
			g_reply_int=Xqdz_Sel(g_pub_tx.reply,&sXqdz,"xqdz100='%s' and xqdz101='%s'",g_pub_tx.ac_no,xqno);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]Xqdz_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
		}
		memset(list2,0x00,sizeof(list2));
		sprintf(list2,"ac_no='%s' and sub_ac_seqn='%ld' ",sSub_dd_mst.ac_no,sSub_dd_mst.sub_ac_seqn);
		if(start_date>0)
		{
			sprintf(list2+strlen(list2)," and tx_date>=%ld ",start_date);
		}
		if(end_date>0)
		{
			sprintf(list2+strlen(list2)," and tx_date<=%ld ",end_date);
		}
		sprintf(list2+strlen(list2),"  order by sub_ac_seqn,tx_date,trace_no,trace_cnt");
		g_reply_int=Sub_dd_mst_hst_Dec_Sel(g_pub_tx.reply,list2);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]Sub_dd_mst_hst_Dec_Sel错误",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		dJ_amt=0.0;
		dD_amt=0.0;
		dBal=0.0;
		cnt=0;
		page=0;
		while(1)
		{
			memset(&sSub_dd_mst_hst,0x00,sizeof(sSub_dd_mst));
			g_reply_int=Sub_dd_mst_hst_Fet_Sel(&sSub_dd_mst_hst,g_pub_tx.reply);
			if(g_reply_int==100)
			{
				if(cnt%50!=0)
				{
					fprintf(fp,"└────┴──────────┴────┴────────┴───────┴───────┴───────┴───┘\n");
					fprintf(fp,"本页借方小计:%-16.2f      本页贷方小计:%-16.2f    %c\n",dJ_amt,dD_amt,'\f');
				}
				break;
			}
			else if(g_reply_int)
		 	{
				vtcp_log("[%s][%d]Sub_dd_mst_hst_Fet_Sel错误",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}
			if(sSub_dd_mst_hst.ct_ind[0]=='1')
			{
				strcpy(ct_flag,"现");
			}
			else if(sSub_dd_mst_hst.ct_ind[0]=='2')
			{
				strcpy(ct_flag,"转");
			}
			memset(j_str,0x00,sizeof(j_str));
			memset(d_str,0x00,sizeof(d_str));

			if(cnt%50==0)/**文件头**/
			{
				page++;
				fprintf(fp,"                                晋中银行对账单\n");
				if(strlen(xqno)>0)
				{
					fprintf(fp,"小区名称:%-50s\n",sXqdz.xqdz15);
				}
				fprintf(fp,"主账户:%s     子账户:%-24.24s   户名:%-30.30s   第%3d页\n",sSub_dd_mst.ac_no,sSub_dd_mst.sub_ac_no,sSub_dd_mst.name,page);
				fprintf(fp,"┌────┬──────────┬────┬────────┬───────┬───────┬───────┬───┐\n");
				fprintf(fp,"│交易日期│    摘        要    │凭证种类│    凭证号码    │  借方发生额  │  贷方发生额  │   余    额   │柜  员│\n");
				fprintf(fp,"├────┼──────────┼────┼────────┼───────┼───────┼───────┼───┤\n");
				if(sSub_dd_mst_hst.add_ind[0]=='0')
				{
					dBal=sSub_dd_mst_hst.bal+sSub_dd_mst_hst.tx_amt;
				}
				else
				{
					dBal=sSub_dd_mst_hst.bal-sSub_dd_mst_hst.tx_amt;
				}
				if( cnt==0&&((start_date%10000==101)||start_date==0) )
				{
					fprintf(fp,"│        │承上年              │        │                │              │              │%14.2f│      │\n",dBal);
				}
				else if(cnt%50==0)/**换页，打印承上页**/
				{
					if(page==1)
					{
						fprintf(fp,"│        │承上页              │        │                │              │              │%14.2f│      │\n",dBal);
					}
					else
					{
						fprintf(fp,"│%8d│承上页              │        │                │%14.2f│%14.2f│%14.2f│      │\n",sSub_dd_mst_hst.tx_date,dJ_amt,dD_amt,dBal);
					}
				}
				dJ_amt=0.0;
				dD_amt=0.0;
			  if(sSub_dd_mst_hst.add_ind[0]=='0')
			  {
					sprintf(j_str,"%s%12.2f",ct_flag,sSub_dd_mst_hst.tx_amt);
					dJ_amt+=sSub_dd_mst_hst.tx_amt;
			  }
			  else
			  {
					sprintf(d_str,"%s%12.2f",ct_flag,sSub_dd_mst_hst.tx_amt);
					dD_amt+=sSub_dd_mst_hst.tx_amt;
			  }
			  lDate=g_pub_tx.tx_date;
			}
			else
			{
				if(sSub_dd_mst_hst.add_ind[0]=='0')
				{
					sprintf(j_str,"%s%12.2f",ct_flag,sSub_dd_mst_hst.tx_amt);
					dJ_amt+=sSub_dd_mst_hst.tx_amt;
				}
				else
				{
					sprintf(d_str,"%s%12.2f",ct_flag,sSub_dd_mst_hst.tx_amt);
					dD_amt+=sSub_dd_mst_hst.tx_amt;
				}
			}
			Del_half_wrod(sSub_dd_mst_hst.brf,brf);
			fprintf(fp,"│%8d│%-20.20s│%8.8s│%16.16s│%14.14s│%14.14s│%14.2f│%6.6s│\n",
			sSub_dd_mst_hst.tx_date,brf,note_type,note_no,j_str,d_str,sSub_dd_mst_hst.bal,sSub_dd_mst_hst.tel);
			cnt++;
			if(cnt%50==0)
			{
				fprintf(fp,"└────┴──────────┴────┴────────┴───────┴───────┴───────┴───┘\n");
				fprintf(fp,"本页借方小计:%-16.2f      本页贷方小计:%-16.2f    %c\n",dJ_amt,dD_amt,'\f');
				lDate=sSub_dd_mst_hst.tx_date;
				dBal=sSub_dd_mst_hst.bal;
			}
		}
		Sub_dd_mst_hst_Clo_Sel();
	}
	Sub_dd_mst_Clo_Sel();
	fclose(fp);
OkExit:
	set_zd_data(DC_FILE_SND, "1");
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp!=NULL)
		fclose(fp);
	Sub_dd_mst_Clo_Sel();
	Sub_dd_mst_hst_Clo_Sel();
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
