/***************************************************************
* 文 件 名: spD999.c
* 功能描述：打贷款对帐单
* 作    者: 
* 完成日期：2003年8月14日
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
#include "ln_mst_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"

#define RECK_PAGE_LENG   50

spL999()
{
	int ret=0,i=0,count=0;
	int total_cnt=0,page_cnt=1,tmp_page_cnt=0;
	char ac_no[25],sub_ac_no[25];
	long date1,date2;
	int	 ac_seqn=0;
	int	 hst_cnt=0;					/***明细笔次***/
	FILE *fp;
	char tmpname[101];
	char add_ind[5];
	char cur_no[3];
	char term[5];
	char ac_type[2];
	char dc_flag[2];
	char ln_tx_type[9];
	double d_amt=0.00,c_amt=0.00;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct ln_mst_hst_c	g_ln_mst_hst;
	struct mdm_attr_c	g_mdm_attr;
	struct note_parm_c	g_note_parm;
	struct dic_data_c	g_dic_data;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
	memset(&g_dic_data,0x00,sizeof(struct dic_data_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit( );

	get_zd_data("0670",ac_type);				/*交易类型*/
	get_zd_data("0210",cur_no);					/*币种*/
	get_zd_data("0310",ac_no);					/*取帐号*/
	get_zd_long("0440",&date1);					/*取起始日期*/
	get_zd_long("0450",&date2);					/*取终止日期*/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该介质类型定义ret==[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, " ac_id=%ld ", 
							  	  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "该账号不存在!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查找错误[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_prdt_ac_id.prdt_no[0]!='3' )
	{
		sprintf( acErrMsg, "非贷款账号" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L114");
		goto ErrExit;
	}

	g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst, " ac_id=%ld ", 
							  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "贷款账号不存在!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查找错误[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_strpack( g_mdm_attr.mdm_name );

	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"打开文件[%s]错",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	/*** 日期相同打印对帐回单 ***/
	if( date1==date2 ) 
	{
		if( g_pub_tx.tx_date==date1 ) 
			g_ln_mst_hst.bal = g_ln_mst.bal;
		else
		{
			ret = Ln_mst_hst_Sel( g_pub_tx.reply,&g_ln_mst_hst, \
			  					"ac_id=%ld and ac_seqn=0 and tx_date<=%ld \
								 order by tx_date desc,tx_time desc", \
								 g_mdm_ac_rel.ac_id, date1 );
			if( ret==100 )
			{
				sprintf(acErrMsg,"未找到符合条件的贷款账户信息!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L224");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找记录出错[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
		}

		fprintf(fp,"DZHD%s|%s|%.2lf|%ld|\n",ac_no,g_ln_mst.name, \
					g_ln_mst_hst.bal,date1);
	}
	else
	{
		total_cnt=sql_count("ln_mst_hst", "ac_id=%ld and ac_seqn=0 and \
							tx_date>=%ld and tx_date<=%ld", \
							g_mdm_ac_rel.ac_id,date1,date2);
		if( total_cnt<=0 )
		{
			sprintf(acErrMsg,"transfer sql_count function error!!");
			WRITEMSG
			goto ErrExit;
		}

		if( ac_type[0]=='*' )
		{
			ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, \
									"ac_id=%ld and ac_seqn=0 and tx_date>=%ld \
									and tx_date<=%ld order by hst_cnt", \
									g_mdm_ac_rel.ac_id,date1,date2 );
		}
		else 
		{
			ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, \
									"ac_id=%ld and ac_seqn=0 and tx_date>=%ld \
									and tx_date<=%ld and ln_tx_type='%s' order \
									by hst_cnt", g_mdm_ac_rel.ac_id,date1, \
									date2,ac_type );
		}
		if( ret )
		{
			sprintf(acErrMsg,"Prepare Open Ln_mst_hst_Dec_Sel error!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		while(1)
		{
			ret=Ln_mst_hst_Fet_Sel(&g_ln_mst_hst,g_pub_tx.reply);
			if( ret==100 )
			{
				if(hst_cnt==0)
				{
					sprintf(acErrMsg,"该帐户没有要打印的记录!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O218");
					goto ErrExit;
				}
				else break;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"取活期明细帐出错!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"W050");
				goto ErrExit;
			}
			
			/*借方发生额*/
			if(g_ln_mst_hst.ln_tx_type[0]=='0' && g_ln_mst_hst.add_ind[0]=='1' )
			{
				d_amt=g_ln_mst_hst.tx_amt;
				c_amt=0.00;
				strcpy(dc_flag,"0");
			}else{
				/*贷方发生额*/
				c_amt=g_ln_mst_hst.tx_amt;
				d_amt=0.00;
				strcpy(dc_flag,"1");
			}				
			
			while(1)
			{
				if(i==RECK_PAGE_LENG)
				{
					page_cnt++;
					i=0;
				}
				else
					i++;
				break;
			}				

			if( hst_cnt>0 && g_ln_parm.cif_type[0]=='1' )
			{
				strcpy(g_mdm_ac_rel.note_no,"----------------");
				strcpy(g_mdm_attr.mdm_name,"------------");
			}
			else if(g_ln_parm.cif_type[0]!='1')
			{
				strcpy(g_mdm_ac_rel.note_no,g_ln_mst_hst.note_no);
				if( strlen(g_ln_mst_hst.note_type) )
				{
					ret = Note_parm_Sel(g_pub_tx.reply,&g_note_parm, \
					 	 "note_type='%s'",g_ln_mst_hst.note_type);
					if( ret==100 )
					{
						sprintf(acErrMsg,"取凭证种类不存在!!note_type=[%s]",
								g_ln_mst_hst.note_type);
						WRITEMSG
						strcpy(g_mdm_attr.mdm_name, g_ln_mst_hst.note_type);
					}
					else if( ret )
					{
						sprintf(acErrMsg,"查找记录出错[%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					else 
						strcpy(g_mdm_attr.mdm_name,g_note_parm.name);
				}
				else 
					strcpy(g_mdm_attr.mdm_name,g_note_parm.name);
			}
			if(g_ln_mst_hst.ln_tx_type [0]== '8' )
			{
				   continue;
			}

			pub_base_strpack(g_ln_mst.name);
			pub_base_dic_show_str(add_ind,"deta_type",g_ln_mst_hst.add_ind);
			pub_base_dic_show_str(ln_tx_type,"ln_tx_type_hst",g_ln_mst_hst.ln_tx_type);

			fprintf(fp,"DDZD%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|%s|%s|%.2lf|%.2lf|%s|\n",
			ac_no,g_ln_mst_hst.tx_br_no,add_ind,g_ln_mst_hst.tx_amt,g_ln_mst_hst.tx_date,
			g_ln_mst_hst.tx_time,g_ln_mst_hst.hst_cnt,g_ln_mst_hst.brf,g_ln_mst_hst.tel,g_ln_mst_hst.bal,
			g_ln_mst_hst.add_ind,g_ln_mst.name,page_cnt,g_mdm_ac_rel.note_no,g_mdm_attr.mdm_name,
			g_ln_parm.cif_type,ln_tx_type,d_amt,c_amt,dc_flag);
			hst_cnt++;
		}

		Ln_mst_hst_Clo_Sel();
	}

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"打印对帐单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"打印对帐单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
