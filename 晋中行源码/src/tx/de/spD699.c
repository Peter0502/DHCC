/*************************************************************
* 文 件 名:     spD699.c
* 功能描述：    �
* 作    者:     xxxxxxx
* 完成日期：    2003年3月15日
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

spD699()
{
	int ret=0;
	int num=0;
	char ac_no[20],ac_type[2];
	char name[50],cur_no[2];
	double amt;
	FILE *fp;
	char filename[50];
	char tmpname[101];

	struct prdt_ac_id_c 		g_prdt_ac_id;
	struct dd_mst_c				g_dd_mst;
	struct td_mst_c				g_td_mst;
	struct mdm_ac_rel_c			g_mdm_ac_rel;
	struct dd_parm_c			g_dd_parm;
	struct td_parm_c			g_td_parm;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/
	get_zd_data("0300",g_pub_tx.ac_no);

	/** 处理流程 **/
	/* 根据账号取得账户ID和帐户序号 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,\
	                      "ac_no='%s'",g_pub_tx.ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"查询介质与账户关系表错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;

TRACE		
	if(g_pub_tx.ac_seqn!=9999)
	{
TRACE		
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, \
							"ac_id=%ld and  ac_seqn=%d", \
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"查询产品账号对照表失败![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

TRACE		
		switch(g_prdt_ac_id.ac_id_type[0])
		{
TRACE		
			 case '1':                                   /* 活期 */
					ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,\
						"ac_id=%ld and ac_seqn=%d",\
						 g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
TRACE		
					if(ret)
					{	
						sprintf(acErrMsg,"查询活期存款主文件失败![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
TRACE		
					strcpy(name,g_dd_mst.name);
					amt=g_dd_mst.bal;
					/*根据产品编号查找币种*/
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,\
									"prdt_no='%s'",g_dd_mst.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"查询活期产品参数表失败![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
TRACE		
					strcpy(cur_no,g_dd_parm.cur_no);
					break;
			case '2':
					ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,\
								"ac_id=%ld and ac_seqn=%d",\
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
					if(ret)
					{
						sprintf(acErrMsg,"查询定期主文件失败![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					strcpy(name,g_td_mst.name);
					amt=g_td_mst.bal;
				
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,\
									"prdt_no='%s'",g_td_mst.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"查询定期产品参数表错误![%d]");
						WRITEMSG
						goto ErrExit;
					}
					strcpy(cur_no,g_td_parm.cur_no);
					break;
			default:
					break;
			}
TRACE		
			/*打印显示部分*/
			pub_base_GenDwnFilName(filename);
TRACE		
			pub_base_strpack(filename);
			sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
TRACE		
			fp=fopen(tmpname,"w");

TRACE		
			fprintf(fp,"~@账号|账号序号|@客户名|@币种|$金额|\n");
			fprintf(fp,"%s|%d|%s|%s|2lf|\n",g_pub_tx.ac_no,g_pub_tx.ac_seqn,\
						name,cur_no,amt);
			fclose(fp);
TRACE		
			set_zd_data(DC_FILE_SND,"1");
		}
		else
		{
			ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",\
									g_mdm_ac_rel.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"执行Prdt_ac_id_Dec_Sel错![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			num=0;
			pub_base_GenDwnFilName(filename);
			pub_base_strpack(filename);
			sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
			fp=fopen(tmpname,"w");


			fprintf(fp,"~@账号|账号序号|@客户名|@币种|$金额|\n");
			while(1)
			{
				ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
				if(ret==100&& num==0) 
				{
					sprintf(acErrMsg,"查询产品账号对照表，无记录![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				else if ( ret == 100 && num > 0 ) break;

				else if(ret)
				{
					sprintf(acErrMsg,"执行Prdt_ac_id_Fet_Sel错![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				g_pub_tx.ac_seqn=g_prdt_ac_id.ac_seqn;


		
		switch(g_prdt_ac_id.ac_id_type[0])
		{
			case '1':									/* 活期 */
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d",\
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"查询活期存款主文件失败![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
TRACE
		sprintf(acErrMsg,"OKOKOKOOKOO");
		WRITEMSG
			strcpy(name,g_dd_mst.name);
			amt=g_dd_mst.bal;
			/*根据产品编号查找币种*/
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
							"prdt_no='%s'",g_dd_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"查询活期产品参数表失败![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(cur_no,g_dd_parm.cur_no);
TRACE
		sprintf(acErrMsg,"OKOKOKOOKOO");
		WRITEMSG
			break;
			
			case '2':								/* 定期 */
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%d",\
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"查询定期主文件失败![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_td_mst.name);
			amt=g_td_mst.bal;

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							"prdt_no='%s'",g_td_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"查询定期产品参数表失败![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(cur_no,g_td_parm.cur_no);
			break;
		default:
			break;
		}

TRACE

  	/*打印显示部分*/

	fprintf(fp,"%s|%d|%s|%s|2lf|\n",g_pub_tx.ac_no,g_pub_tx.ac_seqn,name,amt);
TRACE
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	}
	num++;
	/*while 结束*/

	Mdm_ac_rel_Clo_Sel();
	}
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询打印成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"查询打印失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
