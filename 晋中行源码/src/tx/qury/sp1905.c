/***************************************************************
* 文 件 名:     sp1905.c
* 功能描述：	客户已使用的银行服务
* 作    者:     peter
* 完成日期：    2003年3月19日
*
* 修改记录：
*    日    期:	2004年06月08日
*    修 改 人:	peter
*    修改内容:	添加介质类型和介质号码
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_opn_cls_c.h"
#include "cif_basic_inf_c.h"
#include "mdm_attr_c.h"

sp1905()
{
	char filename[100];
	char prdt_name[25];  
	char tmpname[512],tmpstr[150];
	char flag[3],tmp_ind[9];
	int i=0;
	int ret=0;
	FILE *fp;
	int cnt=0;
	int aaa=0;
	char freenull[20];

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct mo_opn_cls_c	g_mo_opn_cls;
	struct cif_basic_inf_c	g_cif_basic_inf;
	struct mdm_attr_c	mdm_attr_tmp;
	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_mo_opn_cls,0x00,sizeof(struct mo_opn_cls_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset( &g_cif_basic_inf,0,sizeof(struct cif_basic_inf_c));
	memset( &mdm_attr_tmp,0,sizeof(struct mdm_attr_c));
	
	g_reply_int=0;
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	
	strcpy(freenull,"");
	
	get_zd_long("0280",&g_pub_tx.cif_no);				/*客户号*/	
	get_zd_data("0620",g_pub_tx.id_no);					/*证件号码*/
	get_zd_data("0670",flag);							/*证件类型*/
	vtcp_log("g_pub_tx.cif_no=[%ld]",g_pub_tx.cif_no);
	vtcp_log("g_pub_tx.id_no=[%s]",g_pub_tx.id_no);
	vtcp_log("flag=[%s]",flag);
	if(g_pub_tx.cif_no==0&&strlen(flag)!=0)				/*按照证件类型查找*/
	{
		if(strlen(g_pub_tx.id_no)==0)
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
	 		sprintf(acErrMsg,"客户证件对照表中无此记录!!");
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"C007");
	 		goto ErrExit;
		}
		else if(g_reply_int)
	 	{
	 		sprintf(acErrMsg,"查询客户证件对照表异常!!g_reply_int=[%ld]", \
					g_reply_int);
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"D101");
	 		goto ErrExit;
		}

		g_pub_tx.cif_no=g_cif_id_code_rel.cif_no;		/*客户ID*/
	}
	else if(g_pub_tx.cif_no==0&&strlen(flag)==0)
	{
	 	sprintf(acErrMsg,"客户号和证件类型不能同时为空!!g_reply_int=[%ld]", \
				g_reply_int);
	 	WRITEMSG
	 	strcpy(g_pub_tx.reply,"C076");
	 	goto ErrExit;
	}

	/*检查该客户号是否存在*/
	g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
								  "cif_no=%ld", g_pub_tx.cif_no);
	if (g_reply_int&&g_reply_int!=100)
	{
 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]异常!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
 		goto ErrExit;
	}else if( g_reply_int==100 ){
 		sprintf(acErrMsg,"客户号不存在,请检查!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
 		goto ErrExit;
 	}		

	g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "cif_no=%ld", \
								   g_pub_tx.cif_no);
	if (g_reply_int)
	{
 		sprintf(acErrMsg,"[DECLARE SELECT Prdt_ac_id]异常!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
 		goto ErrExit;
	}

	strcpy(tmpstr,"ac_id=%ld and ac_seqn=%d");

	/*打印开始*/
	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);
	sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpname,"w");

	/**显示列标题**/
 	fprintf(fp,"~客户号|@客户名称|@产品|@产品名称|@凭证名称|@凭证号码|@账//卡号|@状态|开户时间|@网点|@柜员|\n");

	while(1)
	{
		g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if (g_reply_int==100&&ret==0)
		{
			sprintf(acErrMsg,"该客户没有使用的银行服务!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C078");
			goto ErrExit;;
		}else if( g_reply_int==100&&ret!=0){
			break;
		}else if(g_reply_int){
			sprintf(acErrMsg,"FETCT 产品帐号对照表异常!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}

		strcpy(g_pub_tx.prdt_code,g_prdt_ac_id.prdt_no);	/*产品编号*/

			TRACE
			/* 限制自己行只能查自己的 modify by wudawei 20140711*/
			g_reply_int=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and (ac_seqn=%d or ac_seqn=9999) and opn_br_no='%s'",g_prdt_ac_id.ac_id, g_prdt_ac_id.ac_seqn, g_pub_tx.tx_br_no);
			if(g_reply_int)
			{
				sprintf(acErrMsg,"Mdm_ac_rel_Dec_Sel失败!!");
				WRITEMSG
				goto ErrExit;
			}
			while(1)
			{
				TRACE
				g_reply_int=Mdm_ac_rel_Fet_Sel(&g_mdm_ac_rel,g_pub_tx.reply);
				if(g_reply_int&&g_reply_int!=100)
				{
					sprintf(acErrMsg,"Mdm_ac_rel_Fet_Sel异常出错!!");
					WRITEMSG
					goto ErrExit;
				}else if(g_reply_int==100){
					sprintf(acErrMsg,"介质账户对照表中不存在该记录!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"N062");	
					break;
				}			
				TRACE
				strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);			/*客户帐号*/

				g_reply_int=Mo_opn_cls_Sel(g_pub_tx.reply,&g_mo_opn_cls, \
						"ac_id=%ld and ac_seqn=%d", \
						g_mdm_ac_rel.ac_id,g_prdt_ac_id.ac_seqn );
				if(g_reply_int==100)
				{
					sprintf(acErrMsg,"开销户登记薄中不存在该记录!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P230");
					break;
				}
				TRACE
				/* 根据ac_id在介质账号对照表中查出介质类型和介质号码 */	
				/* 在mdm_attr表中查询介质名称 */
				g_reply_int = Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr_tmp,"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
				if (g_reply_int)	goto ErrExit;
				TRACE	
				g_pub_tx.tx_date=g_mo_opn_cls.tx_date;				/*开户日期*/
				strcpy(tmp_ind,g_mo_opn_cls.wrk_sts);				/*账户状态*/
				pub_base_dic_show_str(tmp_ind,"opn_cls_sts",g_mo_opn_cls.wrk_sts);
				strcpy(g_pub_tx.name,g_mo_opn_cls.name);			/*户名*/
				strcpy(prdt_name,g_dd_parm.title);					/*产品名称*/
				strcpy(g_pub_tx.tx_br_no,g_mo_opn_cls.opn_br_no);	/*开户点*/
				strcpy(g_pub_tx.tel,g_mo_opn_cls.tel);				/*操作员*/

				TRACE
				switch(g_prdt_ac_id.ac_id_type[0])
				{
					case '1':
						g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
									"prdt_no='%s'", g_pub_tx.prdt_code);
						if(g_reply_int==100)
						{
							sprintf(acErrMsg,"活期明细文件不存在该记录!!");
							WRITEMSG
							strcpy(g_pub_tx.reply,"N062");
							break;
						}
						strcpy(prdt_name,g_dd_parm.title);			/*产品名称*/
						strcpy(flag,"OK");							
						break;
					case '2':
						g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
									"prdt_no='%s'",g_pub_tx.prdt_code);
						if(g_reply_int==100)
						{
							sprintf(acErrMsg,"定期明细文件不存在该记录!!");
							WRITEMSG
							strcpy(g_pub_tx.reply,"N062");
							break;
						}
						strcpy(prdt_name,g_td_parm.title);			/*产品名称*/
						strcpy(flag,"OK");							
						break;
					case '3':
						g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
										"prdt_no='%s'",g_pub_tx.prdt_code);
						if(g_reply_int==100)
						{
							sprintf(acErrMsg,"贷款明细文件不存在该记录!!");
							WRITEMSG
							strcpy(g_pub_tx.reply,"N062");
							break;
						}
						strcpy(prdt_name,g_ln_parm.title);			/*产品名称*/
						strcpy(mdm_attr_tmp.mdm_name,"贷款账户无凭证");
						strcpy(flag,"OK");							
						break;
					default:
						break;
				}/*switch结束*/

				if(!strcmp(flag,"OK"))
				{
 					if(!cnt)
 					{
 						fprintf(fp,"%ld|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|\n", \
							g_pub_tx.cif_no,g_pub_tx.name,g_pub_tx.prdt_code, \
							prdt_name,mdm_attr_tmp.mdm_name,g_mdm_ac_rel.note_no,g_pub_tx.ac_no, \
							tmp_ind,g_pub_tx.tx_date,g_pub_tx.tx_br_no,g_pub_tx.tel);
					}else{
 						fprintf(fp,"%ld|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|\n", \
							aaa,freenull,freenull, \
							freenull,mdm_attr_tmp.mdm_name,g_mdm_ac_rel.note_no,g_pub_tx.ac_no, \
							tmp_ind,g_pub_tx.tx_date,g_pub_tx.tx_br_no,g_pub_tx.tel);
					}					
					ret++;
					cnt++;
				}
				TRACE
			}			
			Mdm_ac_rel_Clo_Sel();
			cnt=0;						
		
	}/*while结束*/

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

	Prdt_ac_id_Clo_Sel();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户已使用的银行服务信息查询成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"客户已使用的银行服务信息查询失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
