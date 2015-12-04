/*********************************
文件名称：spW006
功能：    对公账户的信息概览
完成时间:2009年3月27日
作者:  chengbo 
修改记录：
  1、修改时间
  2、修改人
  3、修改内容
  
*********************************/
#include "stdio.h"
#define  EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_c.h"
#include "cif_cop_inf_c.h"
#include "prdt_ac_id_c.h"
#include "ln_mst_c.h"
#include "cif_mger_rel_c.h"
#include "cif_mger_inf_c.h"
#include "com_branch_c.h"
#include "cif_email_inf_c.h"

spW006()
{
	int             ret,iRc;
	char            Ac_no[21];
	char            Act_type[2];
	char            Ac_type[2];
	char            filename[300];
	char            pcfilename[100];
	long            Cif_no;
	char            ac_sts[2];
	char            tx_date[8+1];
	char            sw_traceno[10+1];
	char            cFtpfile[200];
	char            cAddflag[2];
	FILE           *fp;

	struct dd_mst_c g_dd_mst;
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct cif_cop_inf_c g_cif_cop_inf;
	struct prdt_ac_id_c g_prdt_ac_id;
	struct td_mst_c g_td_mst;
	struct ln_mst_c g_ln_mst;
	struct cif_mger_rel_c g_cif_mger_rel;
	struct cif_mger_inf_c g_cif_mger_inf;
	struct com_branch_c	g_com_branch;
	struct cif_email_inf_c cif_email_inf;

	memset(filename, 0x00, sizeof(filename));
	memset(pcfilename, 0x00, sizeof(pcfilename));
	memset(Ac_no, 0x00, sizeof(Ac_no));
	memset(Act_type, 0x00, sizeof(Act_type));
	memset(Ac_type, 0x00, sizeof(Ac_type));
	memset(ac_sts, 0x00, sizeof(ac_sts));
	memset(tx_date, 0, sizeof(tx_date));
	memset(sw_traceno, 0, sizeof(sw_traceno));
	memset(cFtpfile, 0, sizeof(cFtpfile));
	memset(cAddflag, 0, sizeof(cAddflag));
	memset(&g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset(&g_cif_mger_rel, 0x00, sizeof(struct cif_mger_rel_c));
	memset(&g_cif_mger_inf, 0x00, sizeof(struct cif_mger_inf_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
	

	/***取值、赋值 ***/
	pub_base_sysinit();

	get_zd_data("0300", Ac_no);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);

	/* 生成下传全路经文件名(批量) */
	/**pub_base_AllDwnFilName(filename);**/
	sprintf( filename,"netbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("[%s],[%d],下载全路径文件名为[%s],文件名为[%s]", __FILE__, __LINE__, pcfilename,filename);

	fp = fopen(pcfilename, "w");
	if (fp == NULL) {
		sprintf(acErrMsg, "open file error [%s]", pcfilename);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	/**fprintf(fp, "~账户类型|账号|户名|余额|可用余额|冻结余额|上日余额|币种|起息日|存期|利率|通知种类|累计欠额|\n");**/
	fprintf(fp, "~核心客户号|法人代表|账户名称|账户开户网点|账户开户网点名称|联系电话|客户经理代码|客户经理名称|开户日期|账户状态|账户类型|可否下挂\n");

	vtcp_log("%s,%d开始查询对公帐号信息", __FILE__, __LINE__);
	/**这里先对能否下挂标志赋初值,若不能下挂则在下面具体分支中会改变**/
	strncpy(cAddflag,"0",1);
	
	/*根据显示账号,序号取得账户id*/
	memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no ='%s'", Ac_no);
	if ( ret == 100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
		WRITEMSG
		    strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
		WRITEMSG
		    strcpy (g_pub_tx.reply,"W011");
		return 1;
	}
	set_zd_data("0630",g_mdm_ac_rel.id_no);   /*证件号码*/
	set_zd_data("0680",g_mdm_ac_rel.id_type);   /*证件类型*/
	/**获取开户行行名行号**/
	ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
	if( 100==ret ) {
		sprintf(acErrMsg,"该机构号不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if( ret ) {
		sprintf(acErrMsg,"查询公共机构码表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	vtcp_log("%s,%d 开户行号=[%s],开户行名=[%s]\n",__FILE__,__LINE__,g_com_branch.br_no,g_com_branch.br_name);
	/*根据账户id和账户序号取得帐号类型*/
	memset( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id,
	    "ac_id=%ld and ac_seqn=1",g_mdm_ac_rel.ac_id);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表中不存在该记录[%d]",ret);
		WRITEMSG
		    strcpy (g_pub_tx.reply,"W012");
		return 1;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查询产品帐号对照表异常[%d]",ret);
		WRITEMSG
		    strcpy (g_pub_tx.reply,"W013");
		return 1;
	}

	/*返回帐号类型*/
	memset(Act_type, 0x00, sizeof(Act_type));
	strcpy(Act_type , g_prdt_ac_id.ac_id_type);
	vtcp_log("Jarod:ac_id_type=[%s] [%s][%d]",g_prdt_ac_id.ac_id_type,__FILE__,__LINE__);
	strcpy(g_pub_tx.ac_id_type,Act_type);
	strcpy(g_pub_tx.prdt_code,g_prdt_ac_id.prdt_no);

	sprintf(acErrMsg,"g_pub_tx.prdt_code=[%s]!!",g_pub_tx.prdt_code);
	WRITEMSG

	    if (pub_base_get_prdtparm(g_pub_tx.prdt_code))
	{
		strcpy(acErrMsg,"取产品参数出错!!");
		WRITEMSG
		    return 1;
	}

	switch ( atoi(Act_type) )
	{
	case 1:									/*1-活期*/
		memset( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );
		memset( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );

		/*主文件*/
		ret = Dd_mst_Sel(&g_pub_tx.reply, &g_dd_mst,
		    "ac_id=%ld and ac_seqn=1",
		    g_mdm_ac_rel.ac_id);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
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


		vtcp_log("Jarod2:g_dd_mst.ac_sts=[%s] [%s][%d]",g_dd_mst.ac_sts,__FILE__,__LINE__);
		/*查询活期参数表*/
		ret = Dd_parm_Sel(&g_pub_tx.reply, &g_dd_parm,
		    "prdt_no='%s'", g_dd_mst.prdt_no);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
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
		/**添加能否下挂判断 目前判断是保证金账户,协定存款等账户的话，不能下挂**/
		/**委托承兑汇票保证金132,活期承兑保证金131,委托存款133,定期承兑保证金134,担保保证金142,担保保证金144,财政集中支付清算12D,预算单位零余额存款12E,财政局零余额存款12F**/
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
	case 2:									/*2-定期*/

		memset( &g_td_mst, 0x00, sizeof(struct td_mst_c) );
		memset( &g_td_parm, 0x00, sizeof(struct td_parm_c) );

		ret = Td_mst_Sel(&g_pub_tx.reply, &g_td_mst,
		    "ac_id=%ld and ac_seqn=1",
		    g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W016");
			return 1;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W017");
			return 1;
		}

		/*查询定期参数表*/
		ret = Td_parm_Sel(&g_pub_tx.reply, &g_td_parm,
		    "prdt_no='%s'", g_td_mst.prdt_no);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W014");
			return 1;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W015");
			return 1;
		}
		break;
	case 3:									/*3-贷款*/
		memset( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );

		ret = Ln_mst_Sel(&g_pub_tx.reply, &g_ln_mst,
		    "ac_id=%ld and ac_seqn=1",
		    g_mdm_ac_rel.ac_id);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"贷款主文件中不存在该记录[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W018");
			return 1;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询贷款主文件异常[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W019");
			return 1;
		}
		break;
	case 4:									/*4-透支*/
		memset( &g_od_mst, 0x00, sizeof(struct od_mst_c) );

		ret = Od_mst_Sel(&g_pub_tx.reply, &g_od_mst,
		    "ac_id=%ld and ac_seqn=1",
		    g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"透支主文件中不存在该记录[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W020");
			return 1;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询透支主文件异常[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W021");
			return 1;
		}
		break;
	case 9:									/*5-内部产品*/
		memset( &g_in_mst, 0x00, sizeof(struct in_mst_c) );
		ret = In_mst_Sel(&g_pub_tx.reply, &g_in_mst,
		    "ac_id=%ld and ac_seqn=1",
		    g_mdm_ac_rel.ac_id);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"内部产品主文件中不存在该记录[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W022");
			return 1;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询内部主文件异常[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W023");
			return 1;
		}
		break;
	default:
		sprintf( acErrMsg, "帐户类型错" );
		WRITEMSG
		    return 1;
	}
	sprintf(acErrMsg,"g_mdm_ac_rel.opn_br_no=[%s],g_mdm_ac_rel.ac_id=[%ld]", g_mdm_ac_rel.opn_br_no, g_mdm_ac_rel.ac_id);
	WRITEMSG
		memset(ac_sts, 0x00, sizeof(ac_sts));
		vtcp_log("Jarod3:g_dd_mst.ac_sts=[%s] [%s][%d]",g_dd_mst.ac_sts,__FILE__,__LINE__);
		if(Act_type[0]=='1')
		{
			Cif_no=g_dd_mst.cif_no;	
			Ac_type[0]='2';
			if (g_dd_mst.ac_sts[0] == '1') {
				ac_sts[0]='0';   /**正常**/
			} else if (g_dd_mst.ac_sts[0] == '0') {
				ac_sts[0]='1';   /**开户待确认**/
			} else if (g_dd_mst.ac_sts[0] == '*') {
				ac_sts[0]='*';   /**销户**/
			}
		}else if(Act_type[0]=='2')
		{
			Cif_no=g_td_mst.cif_no;
			Ac_type[0]='4';
			if (g_td_mst.ac_sts[0] == '1') {
				ac_sts[0]='0';   /**正常**/
			} else if (g_td_mst.ac_sts[0] == '3') {
				ac_sts[0]='1';   /**挂失**/
			} else if (g_td_mst.ac_sts[0] == '*') {
				ac_sts[0]='2';   /**销户**/
			} else
			{
				ac_sts[0]='3';   /**异常**/
			}
		}else if(Act_type[0]=='3')
		{
			Cif_no=g_ln_mst.cif_no;
			Ac_type[0]='1';
			if (g_ln_mst.ac_sts[0] == '1') {
				ac_sts[0]='0';   /**正常**/
			} else if (g_ln_mst.ac_sts[0] == '3') {
				ac_sts[0]='1';   /**挂失**/
			} else if (g_ln_mst.ac_sts[0] == '*') {
				ac_sts[0]='2';   /**销户**/
			} else
			{
				ac_sts[0]='3';   /**异常**/
			}
		}
		else
		{
		  vtcp_log("%s,%d,账户异常!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P104");
			goto ErrExit;
		}
	ret = Cif_cop_inf_Sel(g_pub_tx.reply, &g_cif_cop_inf, "cif_no=%ld", Cif_no);
	if (ret!=0 && ret!=100 && ret!=1403) {
		sprintf(acErrMsg, "读取cif_cop_inf表异常![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "W011");
		goto ErrExit;
	}
	ret = Cif_mger_rel_Sel(g_pub_tx.reply, &g_cif_mger_rel, "cif_no=%ld", Cif_no);
	if (ret!=0 && ret!=100 && ret!=1403) {
		sprintf(acErrMsg, "读取cif_mger_rel表异常![%d]", ret);
		WRITEMSG
			strcpy(g_pub_tx.reply, "W011");
		goto ErrExit;
	}
	if(ret == 0)
		{
	    iRc = Cif_mger_inf_Sel(g_pub_tx.reply, &g_cif_mger_inf, "mang='%s'", g_cif_mger_rel.mang);
	    if (iRc!=0 && iRc!=100 && iRc!=1403) {
		      sprintf(acErrMsg, "读取cif_mger_inf表异常![%d]", iRc);
		      WRITEMSG
			    strcpy(g_pub_tx.reply, "W011");
		      goto ErrExit;
	    }
    }
/* 根据客户号查询客户联系电话*/
	memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
	ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf,
	" cif_no = %ld and addr_type = '1'", Cif_no);
	if (ret != 100 && ret != 0)
		{
			sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
	sprintf(acErrMsg, "[%s],[%d]信息查询完毕!", __FILE__,__LINE__);
	WRITEMSG
	vtcp_log("%s,%d ac_sts=[%s]\n",__FILE__,__LINE__,ac_sts);
	if(Act_type[0]=='1')
		{
			fprintf(fp, "%ld|%30s|%60s|%6s|%50s|%20s|%5s|%50s|%ld|%s|%s|%s|\n", Cif_no,g_cif_cop_inf.artificial_person,g_dd_mst.name,g_mdm_ac_rel.opn_br_no,g_com_branch.br_name,cif_email_inf.email,g_cif_mger_inf.mang,g_cif_mger_inf.mang_name,g_dd_mst.opn_date,ac_sts,Act_type,cAddflag );  /**按照和宇信新商定，账户类型返回字段变为Act_type 1.活期 2.定期 3.贷款**/
	}else if(Act_type[0]=='2')
		{
			fprintf(fp, "%ld|%30s|%60s|%6s|%50s|%20s|%5s|%50s|%ld|%s|%s|%s|\n", Cif_no,g_cif_cop_inf.artificial_person,g_td_mst.name,g_mdm_ac_rel.opn_br_no,g_com_branch.br_name,cif_email_inf.email,g_cif_mger_inf.mang,g_cif_mger_inf.mang_name,g_td_mst.opn_date,ac_sts,Act_type,cAddflag );
	}else if(Act_type[0]=='3')
		{
			fprintf(fp, "%ld|%30s|%60s|%6s|%50s|%20s|%5s|%50s|%ld|%s|%s|%s|\n", Cif_no,g_cif_cop_inf.artificial_person,g_ln_mst.name,g_mdm_ac_rel.opn_br_no,g_com_branch.br_name,cif_email_inf.email,g_cif_mger_inf.mang,g_cif_mger_inf.mang_name,g_ln_mst.opn_date,ac_sts,Act_type,cAddflag );
	}

	fclose(fp);
	/**set_zd_data(DC_FILE_SND, "1");**/
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


OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
