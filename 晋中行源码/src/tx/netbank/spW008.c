/***************************************************************
* 文 件 名: spW008.c
* 功能描述：客户账户详细信息
* 作    者: dlc
* 完成日期：2013年6月3日
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
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_mger_inf_c.h"
#include "cif_mger_rel_c.h"

spW008()
{
	char filename[100];
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[21];
	char cFtpfile[200];
	char tx_date[8+1];
	char sw_traceno[10+1];
	char cAddflag[2];
	int ret=0;
	FILE *fp;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct cif_basic_inf_c	g_cif_basic_inf;
	struct cif_mger_inf_c g_cif_mger_inf;
	struct cif_mger_rel_c g_cif_mger_rel;
	struct com_branch_c g_com_branch;

	memset( &g_com_branch,0,sizeof(g_com_branch) );
	memset( &g_cif_mger_inf,0,sizeof(g_cif_mger_inf) );
	memset( &g_cif_mger_rel,0,sizeof(g_cif_mger_rel) );
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(cFtpfile, 0, sizeof(cFtpfile));
	memset(cAddflag, 0, sizeof(cAddflag));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(tx_date, 0, sizeof(tx_date));
	memset(sw_traceno, 0, sizeof(sw_traceno));

	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_long("0280",&g_pub_tx.cif_no);				/*客户号*/
	get_zd_data("0670",flag);							/*证件类型*/
	get_zd_data("0620",g_pub_tx.id_no);					/*证件号码*/
	
			get_zd_data("0050",tx_date);
		get_zd_data("0520",sw_traceno);

	if(g_pub_tx.cif_no==0&&strlen(flag)!=0)				/*按照证件类型查找*/
	{
		if (strlen(g_pub_tx.id_no)==0)
		{
			sprintf(acErrMsg,"请输入客户证件号码!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C079");
			goto ErrExit;
		}

		g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
					"id_type='%s' and id_no='%s'",flag,g_pub_tx.id_no); 
	 	if (g_reply_int==100)
	 	{
	 		sprintf(acErrMsg,"客户基本信息表中无此记录!!");
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"C007");
	 		goto ErrExit;
		}
		else if(g_reply_int)
	 	{
	 		sprintf(acErrMsg,"查询客户基本信息表异常!!g_reply_int=[%ld]", \
					g_reply_int);
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"D101");
	 		goto ErrExit;
		}
		g_pub_tx.cif_no=g_cif_id_code_rel.cif_no;
	}
	else if(g_pub_tx.cif_no==0&&strlen(flag)==0)
	{
		sprintf(acErrMsg,"客户号和证件栏不能同时为空");
	 	WRITEMSG
	 	strcpy(g_pub_tx.reply,"C076");
	 	goto ErrExit;
	}

	/*检查客户号是否存在*/
	g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
								  "cif_no=%ld", g_pub_tx.cif_no); 
 	if (g_reply_int==100)
 	{
 		sprintf(acErrMsg,"客户基本信息表中未找到此客户号!!");
 		WRITEMSG
 		strcpy(g_pub_tx.reply,"C007");
 		goto ErrExit;
	}

	strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d");

	g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", \
								   g_pub_tx.cif_no);
	if (g_reply_int)
	{
 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]异常!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
 		goto ErrExit;
	}

	/*打印开始*/
  sprintf( filename,"0303netbank%s%s",tx_date,sw_traceno );
	sprintf( tmpname,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("[%s][%d] 要写的文件名tmpname[%s]",__FILE__,__LINE__,tmpname);
	fp=fopen(tmpname,"w");

	/**显示列标题**/
 	fprintf(fp,"~客户ID|@客户名称|@账//卡号|$账户余额|@账户状态|账号类型|账号核心开户网点|账号核心开户网点名称|联系电话|客户经理姓名|客户经理编号|能否下挂\n");

					vtcp_log("XXXXXXXXXXXXXX");
	int iNum = 0;
	while(1)
	{
		g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if (g_reply_int==100&&ret==0)
		{
			sprintf(acErrMsg,"该客户没有相关的帐务信息!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C077");
			goto ErrExit;
		}
		else if(g_reply_int==100)break;
		else if(g_reply_int)
		{
			sprintf(acErrMsg,"FETCT 产品帐号对照表异常!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld",g_prdt_ac_id.ac_id);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"无此帐号表请检查!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");
			break;
		}

		strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);			/*客户账号*/

		g_reply_int=Cif_mger_rel_Sel(g_pub_tx.reply,&g_cif_mger_rel, \
									"cif_no=%ld",g_pub_tx.cif_no);
		if (g_reply_int==100)
		{
			sprintf(acErrMsg,"无客户经理!!g_reply_int=[%ld]", \
					g_reply_int);
			strcpy(g_cif_mger_inf.mang,"");
			strcpy(g_cif_mger_inf.mang_name,"");
		}
		else if(g_reply_int)
	 	{
	 		sprintf(acErrMsg,"无此客户请检查!!g_reply_int=[%ld]", \
					g_reply_int);
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"D101");
	 		goto ErrExit;
		}

		else
		{	g_reply_int=Cif_mger_inf_Sel(g_pub_tx.reply,&g_cif_mger_inf, \
										"mang=%s",g_cif_mger_rel.mang);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"无此客户经理请检查!!");
				strcpy(g_cif_mger_inf.mang_name,"");
				
			}
			else if(g_reply_int)
		 	{
		 		sprintf(acErrMsg,"无此客户经理请检查!!g_reply_int=[%ld]", \
						g_reply_int);
		 		WRITEMSG
		 		strcpy(g_pub_tx.reply,"D101");
		 		goto ErrExit;
			}
		}
			
		g_reply_int=Com_branch_Sel(g_pub_tx.reply,&g_com_branch, \
									"br_no=%s",g_mdm_ac_rel.opn_br_no);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"无此机构请检查!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");
			break;
		}
		/**这里先对能否下挂标志赋初值,若不能下挂则在下面具体分支中会改变**/
		strncpy(cAddflag,"0",1);
		switch(g_prdt_ac_id.ac_id_type[0])
		{
			case '1':
					g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"活期明细文件不存在该记录!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					vtcp_log("%s,%d 1234g_dd_mst.ac_sts=[%s]\n",__FILE__,__LINE__,g_dd_mst.ac_sts);
					/*为了和宇信账户状态对应，账户状态转码0,1互换*/
					if(memcmp(g_dd_mst.ac_sts,"1",1) == 0)
					{
						memcpy(g_dd_mst.ac_sts,"0",1);
					}
					else if(memcmp(g_dd_mst.ac_sts,"0",1) == 0)
					{
					memcpy(g_dd_mst.ac_sts,"1",1);
					}
					/*为了和宇信账户状态对应，账户状态转码*/
					vtcp_log("%s,%d 123g_dd_mst.ac_sts=[%s]\n",__FILE__,__LINE__,g_dd_mst.ac_sts);
					g_pub_tx.tx_amt1=g_dd_mst.bal;
					pub_base_dic_show_str(tmp_ind,"dd_ac_sts",g_dd_mst.ac_sts);
					strcpy(g_pub_tx.name,g_dd_mst.name);
					strcpy(flag,"OK");
					
					/**添加能否下挂判断 目前判断是保证金账户,协定存款等账户的话，不能下挂**/
					/**委托承兑汇票保证金132,活期承兑保证金131,委托存款13,定期承兑保证金134,担保保证金142,担保保证金144,财政集中支付清算12D,预算单位零余额存款12E,财政局零余额存款12F**/
					sprintf(acErrMsg, "[%s],[%d] 能否下挂判定条件：g_dd_mst.prdt_no=[%s],g_dd_mst.ac_type=[%s]", __FILE__,__LINE__,g_dd_mst.prdt_no,g_dd_mst.ac_type);
					WRITEMSG
					if(memcmp(g_dd_mst.prdt_no,"131",3)==0||memcmp(g_dd_mst.prdt_no,"132",3)==0||memcmp(g_dd_mst.prdt_no,"133",3)==0||memcmp(g_dd_mst.prdt_no,"134",3)==0||memcmp(g_dd_mst.prdt_no,"142",3)==0||memcmp(g_dd_mst.prdt_no,"144",3)==0||memcmp(g_dd_mst.prdt_no,"12D",3)==0||memcmp(g_dd_mst.prdt_no,"12E",3)==0||memcmp(g_dd_mst.prdt_no,"12F",3)==0)
					{
						strncpy(cAddflag,"1",1);/**0：可以下挂 1：不能下挂**/
					}
					/**协定存款 ac_type：6**/
					if(memcmp(g_dd_mst.ac_type,"6",1)==0)
					{
						strncpy(cAddflag,"1",1);
					}
					sprintf(acErrMsg, "[%s],[%d] 0：可以下挂 1：不能下挂,判定的能否下挂结果为：[%s]", __FILE__,__LINE__,cAddflag);
					WRITEMSG
					
					break;
			case '2':
					g_reply_int=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"定期文件不存在该记录!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					g_pub_tx.tx_amt1=g_td_mst.bal;
					pub_base_dic_show_str(tmp_ind,"td_ac_sts",g_td_mst.ac_sts);
					strcpy(g_pub_tx.name,g_td_mst.name);
					strcpy(flag,"OK");
					break;
			case '3':
					g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"贷款文件不存在该记录!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					g_pub_tx.tx_amt1=g_ln_mst.bal;
					pub_base_dic_show_str(tmp_ind,"ln_ac_sts",g_ln_mst.ac_sts);
					strcpy(g_pub_tx.name,g_ln_mst.name);
					strcpy(flag,"OK");
					break;
			default:
					break;
		}/*switch结束*/

		if(!strcmp(flag,"OK"))
		{
 			fprintf(fp,"%ld|%50s|%20s|%2s|%1s|%5s|%30s|%20s|%20s|%4s|%1s|\n", \
					g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.ac_no, \
					g_dd_mst.ac_sts,g_prdt_ac_id.ac_id_type,g_com_branch.br_no,g_com_branch.br_name,g_cif_mger_inf.phone,g_cif_mger_inf.mang_name,g_cif_mger_inf.mang,cAddflag);
			ret++;
		}
		iNum++;
	}/*while结束*/

	fclose(fp);
	/*set_zd_data(DC_FILE_SND,"1");*/
			set_zd_int("0490",iNum);
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


	Prdt_ac_id_Clo_Sel();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户账户详细信息查询成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"客户账户详细信息查询失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

