/***************************************************************
* 文 件 名: sp1907.c
* 功能描述：VTM查询客户所有卡
* 作    者: 孙朋君
* 完成日期：2014年7月24日
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

sp1907()
{
	char filename[100];
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[21];
	char sNoteSts[31]={0};
	long open_date = 0; /**add zgf 20131031 增加开户日期**/
	int ret=0;
	FILE *fp;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct cif_basic_inf_c	g_cif_basic_inf;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_long("0280",&g_pub_tx.cif_no);				/*客户号*/
	get_zd_data("0670",flag);							/*证件类型*/
	get_zd_data("0620",g_pub_tx.id_no);					/*证件号码*/

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
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**显示列标题**/
 	fprintf(fp,"~客户ID|@客户名称|@账//卡号|$账户余额|@账户状态|@开户日期|介质状态|\n");/**add zgf 20131031 增加开户日期**/

					vtcp_log("XXXXXXXXXXXXXX");
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
		/** 换折子 换卡的 部提的 只能查到旧介质 查不到新的 改了后 直接查新的 modify by wudawei 20130923 
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld",g_prdt_ac_id.ac_id);
		modify by wudawei 20130923 **/
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
									"ac_id=%ld and note_sts not in ('2','3','4')",g_prdt_ac_id.ac_id);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"无此帐号表请检查!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");
			/*wudawei 20140212 
			break;
			*/
			continue;
		}

		strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);			/*客户账号*/

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
	
					g_pub_tx.tx_amt1=g_dd_mst.bal;
					pub_base_dic_show_str(tmp_ind,"dd_ac_sts",g_dd_mst.ac_sts);
					strcpy(g_pub_tx.name,g_dd_mst.name);
					open_date = g_dd_mst.opn_date;/**add zgf 20131031 增加开户日期**/
					strcpy(flag,"OK");
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
					open_date = g_td_mst.opn_date;/**add zgf 20131031 增加开户日期**/
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
					open_date = g_ln_mst.opn_date;/**add zgf 20131031 增加开户日期**/
					strcpy(flag,"OK");
					break;
			default:
					break;
		}/*switch结束*/

		if(!strcmp(flag,"OK"))
		{/**add zgf 20131031 增加开户日期**/
                        memset(sNoteSts,0,sizeof(sNoteSts));
                        pub_base_dic_show_str(sNoteSts,"mdm_sts",g_mdm_ac_rel.note_sts);
 			fprintf(fp,"%ld|%s|%s|%.2lf|%s|%ld|%s|\n", \
					g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.ac_no, \
					g_pub_tx.tx_amt1,tmp_ind,open_date,sNoteSts);
			ret++;
		}
	}/*while结束*/

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

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
