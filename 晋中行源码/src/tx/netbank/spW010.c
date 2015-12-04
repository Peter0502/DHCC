/*************************************************
* 文 件 名:  spW010.c
* 功能描述： 银企明细对账单详细信息查询 
*
* 作    者:  hzh 
* 完成日期： 2013年6月19日 
*
* 修改记录： 
* 日   期:	
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"
#include "mdm_unprt_hst_c.h"
#include "dd_mst_c.h"
#include "chnl_self_trade_c.h"
extern char *get_env_info(char *infoname);
spW010()
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_hst_c vdd_mst_hst;
	struct td_mst_hst_c vtd_mst_hst;
	struct td_mst_hst_c f_td_mst_hst;
	struct td_mst_hst_c b_td_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c b_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	struct com_branch_c sCom_branch;
	struct mdm_unprt_hst_c sMdm_unprt_hst;
	struct dd_mst_c g_dd_mst;
	struct dd_mst_c f_dd_mst;
	struct td_mst_c f_td_mst;
	struct td_mst_c g_td_mst;
	struct chnl_self_trade_c g_chnl_self_trade;

	char            add_ind[3];
	char            ct_ind[5];
	char            ac_no3[25];
	char            f_note_type9[41];
	char            f_acno0[25];
	char            filename[100];
	char            pcfilename[100];
	char            wherelist[1024];	/**查询条件**/
	char            tmpstr[512];
	char            tmpmo[512];
	char			sPc_flag[2];   /* 对公对私标志 */
	char            fieldlist[1024];	/**要查询的字段列表**/
	char            tablelist[128];	/**要查询的表名**/
	char            titstr[1024];
	int             ttlnum = 0;	/**读取的总条数**/
	int             ttlnum2 = 0;  /**从要求的起始记录号开始读取的总条数**/
	int             i = 0;
	int             ret = 0, dPage = 0;
	double					tmpbal = 0.00;
	FILE           *fp;
	char            tmp_date[9];
	char            tmp_time[7];
	int             currnum = 0;
	int 						count = 0;
	int							sqlcnt = 0;
	long            qs_date;
	long            jz_date;
	char            bz[4];
	char            flag[3];
	char            td_mdm[5];
	char            tx_date[8+1];
	char            sw_traceno[10+1];
	char            cFtpfile[200];
	char            sDf_ac[30];
	char            sDf_ac_name[100];

	memset(&f_td_mst_hst, 0x00, sizeof(f_td_mst_hst));
	memset(&b_td_mst_hst, 0x00, sizeof(b_td_mst_hst));
	memset(add_ind, 0x00, sizeof(add_ind));
	memset(sPc_flag,0,sizeof(sPc_flag));
	memset(ct_ind, 0x00, sizeof(ct_ind));
	memset(ac_no3, 0x00, sizeof(ac_no3));
	memset(f_note_type9, 0x00, sizeof(f_note_type9));
	memset(f_acno0, 0x00, sizeof(f_acno0));
	memset(&vdd_mst_hst, 0x00, sizeof(vdd_mst_hst));
		memset(&vtd_mst_hst, 0x00, sizeof(vtd_mst_hst));
	memset(&f_dd_mst_hst, 0x00, sizeof(f_dd_mst_hst));
	memset(&b_dd_mst_hst, 0x00, sizeof(b_dd_mst_hst));
	memset(&f_dd_mst_hsta, 0x00, sizeof(f_dd_mst_hsta));
	memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
	memset(&sCom_branch, 0x00, sizeof(sCom_branch));
	memset(&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&g_td_mst, 0x00, sizeof(struct td_mst_c));
	memset(&f_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&f_td_mst, 0x00, sizeof(struct td_mst_c));
	memset(&g_chnl_self_trade, 0x00, sizeof(struct chnl_self_trade_c));
	memset(tmpstr, 0x00, sizeof(tmpstr));
	memset(filename, 0x00, sizeof(filename));
	memset(pcfilename, 0x00, sizeof(pcfilename));
	memset(tmpmo, 0x00, sizeof(tmpmo));
	memset(titstr, 0x00, sizeof(titstr));
	memset(tmp_date, 0x00, sizeof(tmp_date));
	memset(tmp_time, 0x00, sizeof(tmp_time));
	memset(&g_pub_tx, 0, sizeof(g_pub_tx));
	memset(wherelist, 0, sizeof(wherelist));
	memset(fieldlist, 0, sizeof(fieldlist));
	memset(tablelist, 0, sizeof(tablelist));
	memset(bz, 0, sizeof(bz));
	memset(td_mdm, 0, sizeof(td_mdm));
	memset(tx_date, 0, sizeof(tx_date));
	memset(sw_traceno, 0, sizeof(sw_traceno));
	memset(cFtpfile, 0, sizeof(cFtpfile));
	memset(sDf_ac, 0, sizeof(sDf_ac));
	memset(sDf_ac_name, 0, sizeof(sDf_ac_name));
	pub_base_sysinit();

	get_zd_data("0300", f_acno0);
	pub_base_old_acno(f_acno0);	/** 处理老账号**/
	get_zd_long("0440", &qs_date);
	get_zd_long("0450", &jz_date);

	get_zd_data("0710", flag);	/* 查询标志 */
	get_zd_data("0660",sPc_flag);
	vtcp_log("LINE %d FILE %s  flag[%.3s]", __LINE__, __FILE__, flag);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);

	/**查询**/
	pub_base_strpack(f_acno0);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", f_acno0);
	ERR_DEAL
	   
	memcpy( td_mdm ,sMdm_ac_rel.mdm_code, 4);
	vtcp_log("[%s],[%d],leijianhui=[%s]=[%s]", __FILE__, __LINE__, td_mdm,sMdm_ac_rel.mdm_code);
	
	ret = Dd_mst_Sel(&g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%ld", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
	if ( ret==100 )
	{
			sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W014");
			return 1;
	}
	else if(ret)
	{
			sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W015");
			return 1;
	}
	else
	{
		set_zd_data("0250",g_dd_mst.name);
		vtcp_log("[%s],[%d],账户名称为[%s]", __FILE__, __LINE__, g_dd_mst.name);

		/*	set_zd_double("0390",g_dd_mst.bal); */
	}
		
	vtcp_log("[%s],[%d],活期账户余额为[%.2f]", __FILE__, __LINE__, g_dd_mst.bal);

 		count=sql_count("dd_mst_hst","ac_id=%ld and ac_seqn=%ld and tx_date>=%ld and tx_date<=%ld ", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn,qs_date,jz_date);
	  vtcp_log("[%s],[%d],11112222count=[%d]", __FILE__, __LINE__,count);
	  if(count == 0)
	  {
	  	/*承后余额begin*/
			ret = Dd_mst_hst_Sel(&g_pub_tx.reply, &b_dd_mst_hst,
			   "ac_id=%ld and ac_seqn=%ld and  tx_date>%ld  order by hst_cnt asc",g_dd_mst.ac_id, g_dd_mst.ac_seqn, jz_date);
			vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
			if ( ret==100 )
			{
				set_zd_double("0430",g_dd_mst.bal); /*若明细表没有则说明承后余额要从活期定期文件表取*/
				set_zd_double("0390",g_dd_mst.bal); /*若明细表没有则说明承前余额要从活期定期文件表取*/
				vtcp_log("[%s],[%d],若明细表没有则说明余额要从活期定期文件表取g_dd_mst.bal=[%f]", __FILE__, __LINE__,g_dd_mst.bal);
			}
			else if(ret)
			{
				sprintf(acErrMsg,"查询活期明细表异常[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W050");
				return 1;
			}
			else
			{
				if(memcmp(b_dd_mst_hst.add_ind,"1",1)==0)
					{
						b_dd_mst_hst.bal=b_dd_mst_hst.bal-b_dd_mst_hst.tx_amt;
						set_zd_double("0430",b_dd_mst_hst.bal); /*承后余额*/
						set_zd_double("0390",b_dd_mst_hst.bal); /*承前余额*/
					}
				else
					{
						b_dd_mst_hst.bal=b_dd_mst_hst.bal+b_dd_mst_hst.tx_amt;
						set_zd_double("0430",b_dd_mst_hst.bal); /*承后余额*/
						set_zd_double("0390",b_dd_mst_hst.bal); /*承前余额*/
					}
				vtcp_log("[%s],[%d],承后账户余额tmpbal为[%.2f]", __FILE__, __LINE__, b_dd_mst_hst.bal);
			
		  }		
			/*承后余额end*/
	  }
		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and tx_date>=%ld and tx_date<=%d order by hst_cnt asc", sMdm_ac_rel.ac_id, qs_date, jz_date);
		vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
		ERR_DEAL

		ttlnum = 0;
		sprintf( filename,"6013netbank%s%s.txt",tx_date,sw_traceno );
		sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
		/**pub_base_AllDwnFilName(filename);**/
		fp = fopen(pcfilename, "w");
		if (fp == NULL)
	  {
			sprintf(acErrMsg, " open file [%s] error ", pcfilename);
			WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
			vtcp_log("[%s],[%d]",__LINE__,__FILE__);
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],下载全路径文件名为[%s],文件名为[%s]", __FILE__, __LINE__, pcfilename,filename);
		
						vtcp_log("[%s],[%d],f_acno0=[%s]", __FILE__, __LINE__, f_acno0);
				/**显示列标题**/
				/**if('2'==sPc_flag[0])**/
				if(memcmp(f_acno0,"5",1))
				{
					vtcp_log("[%s],[%d],qqqqq", __FILE__, __LINE__);
				  fprintf(fp, "交易流水号|交易日期|借贷方向|交易金额|账户余额|对方账号|对方账户名|摘要|\n");
				}
				else
				{	
					vtcp_log("[%s],[%d],wwwww", __FILE__, __LINE__);
					fprintf(fp, "交易流水号|交易日期|借贷方向|交易金额|账户余额|对方账号|对方账户名|摘要\n");
				}

				
		while (ttlnum <= count) 
		{
		ret = Dd_mst_hst_Fet_Sel(&vdd_mst_hst, g_pub_tx.reply);
		vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
		if (ret == 100)
		{
			ttlnum++;
			break;
		}
			vtcp_log("[%s],[%d],2222222", __FILE__, __LINE__);
			ERR_DEAL
			ttlnum++;
			vtcp_log("[%s],[%d],2222222ttlnum=[%d]", __FILE__, __LINE__,ttlnum);
			/*第一条计算承前余额*/
			
			if(ttlnum == 1)
		{
			if(memcmp(vdd_mst_hst.add_ind,"1",1)==0)
			{
				tmpbal=vdd_mst_hst.bal-vdd_mst_hst.tx_amt;
				set_zd_double("0390",tmpbal); /*承前余额*/
			}
			else
			{
				tmpbal=vdd_mst_hst.bal+vdd_mst_hst.tx_amt;
				set_zd_double("0390",tmpbal); /*承前余额*/
			}
	  }

	  /*第一条计算承前余额*/
	  if(ttlnum == count)
	  {
	  	set_zd_double("0430",vdd_mst_hst.bal); /*承后余额*/
	  }
	 
		sprintf(tmp_date, "%ld", vdd_mst_hst.tx_date);
		sprintf(tmp_time, "%d", vdd_mst_hst.tx_time);
		strcpy(bz, "CNY");
	/*	ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'", vdd_mst_hst.tx_br_no);
		if (100 != ret)
			ERR_DEAL
		*/

		/**查询对方账号和对方账户名称**/
	/*	if(memcmp(vdd_mst_hst.tx_code,"6203",4) == 0)
				{
					ret = Chnl_self_trade_Sel(&g_pub_tx.reply, &g_chnl_self_trade,
					"tx_date=%ld and trace_no=%ld",
					vdd_mst_hst.tx_date,vdd_mst_hst.trace_no);
					if ( ret==100 )
					{
					sprintf(acErrMsg,"查询自助设备交易流水记录中无此流水，无法取到对方账户信息[%d]",ret);
					WRITEMSG
					}
					else if(ret)
					{
					sprintf(acErrMsg,"查询自助设备交易流水异常[%d]",ret);
					WRITEMSG
					strcpy (g_pub_tx.reply,"W015");
					return 1;
					}
					strcpy(sDf_ac_name,"暂无");
					strcpy(sDf_ac,"暂无");
				}
			else 
				{
					strcpy(sDf_ac_name,"暂无");
					strcpy(sDf_ac,"暂无");
				}*/
			/**查询对方账号和对方账户名称**/
			
			strcpy(sDf_ac_name,"暂无");
			strcpy(sDf_ac,"暂无");
				if(memcmp(f_acno0,"5",1))
				{
					vtcp_log("[%s],[%d],aaaaa", __FILE__, __LINE__);
					fprintf(fp, "%20d|%6d|%1s|%17.2f|%17.2f|%20s|%50s|%60s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal,sDf_ac, sDf_ac_name, vdd_mst_hst.brf);
	        vtcp_log("开始打印》%d|%d|%06d|%s|%17.2f|%17.2f|%50s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.tx_time, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, vdd_mst_hst.brf);
					}
				else{
					vtcp_log("[%s],[%d],sssss", __FILE__, __LINE__);
		      fprintf(fp, "%20d|%6d|%s|%17.2f|%17.2f|%20s|%30s|%30s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, sDf_ac, sDf_ac_name,vdd_mst_hst.brf);
	        vtcp_log("开始打印》%d|%d|%s|%17.2f|%17.2f|%s|%20s|%50s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, sDf_ac, sDf_ac_name, vdd_mst_hst.brf);
				}
		}

		

		ret = Dd_mst_hst_Clo_Sel();
		vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
		ERR_DEAL
		vtcp_log("[%s],[%d],fp=[%s]", __FILE__, __LINE__,fp);
			fclose(fp);
		vtcp_log("[%s],[%d],6666", __FILE__, __LINE__);

			/**set_zd_data(DC_FILE_SND, "1");**/
			set_zd_int("0480", count);
			set_zd_data("0260",filename);
			sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
			{
			sprintf(acErrMsg,"ftp文件[%s]到平台出错[%d]",filename,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P157");
			goto ErrExit;
			}
			sprintf(acErrMsg,"ftp文件[%s]到平台成功",filename);
			WRITEMSG
		
	



GoodExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

