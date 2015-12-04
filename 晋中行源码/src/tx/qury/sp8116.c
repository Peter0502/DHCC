/***************************************************************
* 文 件 名: sp8116.c
* 功能描述：账号关联账户查询
* 作    者: lance
* 完成日期：2003年06月23日
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

sp8116()
{
	char filename[100];
	char tmpname[512],tmpstr[150];
	char ac_no[20];
	char tmp_title[41];
	char opn_br_no[6];
	
	char flag[3],tmp_ind[21];
	int ret=0;
	int num=0;
	FILE *fp;
	
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(tmpname,0x00,sizeof(tmpname));
        memset(tmpstr,0x00,sizeof(tmpstr));
        memset(filename,0x00,sizeof(filename));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(tmp_title,0x00,sizeof(tmp_title));
	memset(opn_br_no,0x00,sizeof(opn_br_no));
	memset(tmp_ind,0x00,sizeof(tmp_ind));
	memset(flag,0x00,sizeof(flag));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0370",ac_no);					/* 账号 */
    pub_base_old_acno(ac_no);

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , "ac_no='%s'" , \
						  ac_no );
	if(ret==100)
	{
		sprintf(acErrMsg,"该账号不存在,请检查!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L161");
		goto ErrExit;
	}
	
	strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d");

	g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld order by ac_seqn ",
		 							g_mdm_ac_rel.ac_id);
	if(g_reply_int)
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
 	fprintf(fp,"~账户序号|@账//卡号|@产品名称|$账户余额|@账户状态|@客户名称|@开户机构|\n");

					vtcp_log("XXXXXXXXXXXXXX");
	while(1)
	{
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
		g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if (g_reply_int==100&&num==0)
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
		TRACE
	  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
				"ac_id=%ld and (ac_seqn=%d or ac_seqn=ALL_SEQN)",
				g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
		if(g_reply_int==100)
		{
			sprintf(acErrMsg,"无此帐号表请检查!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"N062");
			break;
		}
		TRACE
		strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);			/*客户账号*/

		switch(g_prdt_ac_id.ac_id_type[0])
		{
			case '1':
					TRACE
					g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"活期明细文件不存在该记录!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
					 	"prdt_no='%s'",g_prdt_ac_id.prdt_no);
					if(ret) goto ErrExit;
					
					/* 产品名称 */
					strcpy(tmp_title,g_dd_parm.title);
					sprintf(acErrMsg,"XXXXg_dd_parm.title=[%s]!!", \
								g_dd_parm.title);
					WRITEMSG					
	
					g_pub_tx.tx_amt1=g_dd_mst.bal;
					pub_base_dic_show_str(tmp_ind,"dd_ac_sts",g_dd_mst.ac_sts);
					strcpy(g_pub_tx.name,g_dd_mst.name);
					strcpy( opn_br_no , g_dd_mst.opn_br_no );
					
					strcpy(flag,"OK");
					break;
			case '2':
					TRACE
					g_reply_int=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"定期文件不存在该记录!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					/* 产品名称 */
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
					 	"prdt_no='%s'",g_prdt_ac_id.prdt_no);
					if(ret) goto ErrExit;					
					strcpy(tmp_title,g_td_parm.title);
					sprintf(acErrMsg,"XXXXg_td_parm.title=[%s]!!", \
								g_td_parm.title);
					WRITEMSG					
										
					g_pub_tx.tx_amt1=g_td_mst.bal;
					pub_base_dic_show_str(tmp_ind,"td_ac_sts",g_td_mst.ac_sts);
					strcpy(g_pub_tx.name,g_td_mst.name);
					strcpy( opn_br_no , g_td_mst.opn_br_no );					
					strcpy(flag,"OK");
					break;
			case '3':
					TRACE
					g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst, \
						tmpstr,g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn );
					if(g_reply_int==100)
					{
						sprintf(acErrMsg,"贷款文件不存在该记录!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"N062");
						break;
					}
					/* 产品名称 */
					ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
					 	"prdt_no='%s'",g_prdt_ac_id.prdt_no);
					if(ret) goto ErrExit;					
					strcpy(tmp_title,g_ln_parm.title);
					sprintf(acErrMsg,"XXXXg_ln_parm.title=[%s]!!", \
								g_ln_parm.title);
					WRITEMSG					
					g_pub_tx.tx_amt1=g_ln_mst.bal;
					pub_base_dic_show_str(tmp_ind,"ln_ac_sts",g_ln_mst.ac_sts);
					strcpy(g_pub_tx.name,g_ln_mst.name);
					strcpy( opn_br_no , g_ln_mst.opn_br_no );
					strcpy(flag,"OK");
					break;
			default:
					break;
		}/*switch结束*/

		if(strcmp(flag,"OK")==0)
		{
 			fprintf(fp,"%d|%s|%s|%.2lf|%s|%s|%s|\n", \
					g_prdt_ac_id.ac_seqn , g_pub_tx.ac_no , tmp_title , \
					g_pub_tx.tx_amt1 , tmp_ind , \
					g_pub_tx.name, opn_br_no );
			num++;
		}
	}/*while结束*/

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Prdt_ac_id_Clo_Sel();
	sprintf(acErrMsg,"num![%d]",num);
	WRITEMSG
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"账号关联账户信息查询成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"账号关联账户信息查询失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
