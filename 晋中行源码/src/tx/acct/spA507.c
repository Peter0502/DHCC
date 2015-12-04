/***************************************************************
* 文 件 名: spD507.c
* 功能描述：打印内部户对帐单
* 作    者: jack
* 完成日期：2004年09月07日
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
#include "note_parm_c.h"
#include "com_item_c.h"
#include "in_mst_hst_c.h"

#define RECK_PAGE_LENG   50

spA507()
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
	char br_name[51];
	long tmpdate;
	int first=0;
	double tpbal=0.00;
	double jfse,dfse;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct note_parm_c	g_note_parm;
	struct in_mst_hst_c	g_in_mst_hst;
	struct in_mst_hst_c o_in_mst_hst;
	struct dic_data_c	g_dic_data;
	struct com_item_c	com_item;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_dic_data,0x00,sizeof(struct dic_data_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

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

	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"打开文件[%s]错",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

/*	fprintf(fp,"~@内部帐号|@交易机构|@增减标志|交易金额|交易日期|交易时间|明细笔数|@摘    要|@柜 员 号|@余    额|@户    名|@凭证名称|@凭 证 号|@交易机构|\n");
*/

	ret = In_mst_Sel( g_pub_tx.reply,&g_in_mst,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"未找到此账户信息!!");
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
	ret = In_parm_Sel( g_pub_tx.reply,&g_in_parm,"prdt_no='%s'", \
				  		g_in_mst.prdt_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"内部产品参数表无此记录!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	ret = Com_item_Sel( g_pub_tx.reply,&com_item,"acc_hrt='%s'", \
				  		g_in_parm.acc_hrt);
	if( ret ) com_item.dc_ind[0]=g_in_parm.acc_hrt[0];

	total_cnt=sql_count("in_mst_hst", "ac_id=%ld and tx_date>=%ld and \
						tx_date<=%ld", g_mdm_ac_rel.ac_id,date1,date2);
	if( total_cnt<0 )
	{
		sprintf(acErrMsg,"transfer sql_count function error!!");
		WRITEMSG
		goto ErrExit;
	}

	ret = In_mst_hst_Dec_Sel( g_pub_tx.reply, "ac_id=%ld and tx_date>=%ld \
							  and tx_date<=%ld order by tx_date,hst_cnt, \
							  trace_no,tx_time", g_mdm_ac_rel.ac_id, \
							  date1,date2 );
	if( ret )
	{
		sprintf(acErrMsg,"Prepare Open Dd_mst_hst_Dec_Sel error!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	while(1)
	{
		ret = In_mst_hst_Fet_Sel( &g_in_mst_hst,g_pub_tx.reply );
		if( ret==100 )
		{
			if( hst_cnt==0 )
			{
				sprintf(acErrMsg,"无符合条件的记录!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"S049");
				goto ErrExit;
			}
			else break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"取内部帐户明细帐出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W050");
			goto ErrExit;
		}

		if( first==0 )
		{
			if( date1%100==1 && g_in_mst.opn_date<date1 )
			{
				tpbal=0.00;
				tmpdate=date1;
				if( g_in_mst_hst.add_ind[0]=='0' )
					tpbal = g_in_mst_hst.bal + g_in_mst_hst.tx_amt;
				if( g_in_mst_hst.add_ind[0]=='1' )
					tpbal = g_in_mst_hst.bal - g_in_mst_hst.tx_amt;

				pub_base_getbrname(g_in_mst_hst.opn_br_no,br_name);
				fprintf( fp,"%d|%s|%s|%s|%s|%.2lf|%.2lf|%.2lf|%s|%s|%s\n",
					tmpdate,"上月结转","","","",0.0,0.0,tpbal,
					ac_no,g_in_mst.name,br_name);

				hst_cnt++;
			}
			first=1;
			memcpy( &o_in_mst_hst,&g_in_mst_hst,sizeof(g_in_mst_hst));
		}
		else if( o_in_mst_hst.tx_date/100!=g_in_mst_hst.tx_date/100 )
		{
			if( i>RECK_PAGE_LENG )
			{
				page_cnt++;
				i = 1;
			}
			else i++;

			pub_base_getbrname(g_in_mst_hst.opn_br_no,br_name);
			tmpdate = g_in_mst_hst.tx_date/100*100+1;

			fprintf( fp,"%d|%s|%s|%s|%s|%.2lf|%.2lf|%.2lf|%s|%s|%s\n",
				tmpdate,"上月结转","","","",0.0,0.0,o_in_mst_hst.bal,
				ac_no,g_in_mst.name,br_name);
			hst_cnt++;
		}

		if( i>RECK_PAGE_LENG )
		{
			page_cnt++;
			i = 1;
		}
		else i++;

		pub_base_strpack(g_in_mst_hst.note_no);
		if( strlen(g_in_mst_hst.note_no)>8 )
			strcpy( g_mdm_ac_rel.note_no,g_in_mst_hst.note_no+strlen(g_in_mst_hst.note_no)-8 );
		strcpy(g_mdm_ac_rel.note_no,g_in_mst_hst.note_no);

		if( strlen(g_in_mst_hst.note_type) )
		{
			ret = Note_parm_Sel( g_pub_tx.reply,&g_note_parm, \
			 					 "note_type='%s'",g_in_mst_hst.note_type);
			if( ret==100 )
			{
				sprintf(acErrMsg,"取凭证种类不存在!!note_type=[%s]",
						g_in_mst_hst.note_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
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
		else strcpy( g_note_parm.name, "----------" );

		pub_base_strpack(g_in_mst.name);
		pub_base_dic_show_str(add_ind,"add_ind",g_in_mst_hst.add_ind);
		pub_base_getbrname(g_in_mst_hst.opn_br_no,br_name);

		jfse=dfse=0.00;
		if( g_in_mst_hst.add_ind[0]=='1' )
		{
			if( com_item.dc_ind[0]=='1' )
			{
				jfse=g_in_mst_hst.tx_amt;
			}
			else if( com_item.dc_ind[0]=='2' )
			{
				dfse=g_in_mst_hst.tx_amt;
			}
			else
			{
				jfse=g_in_mst_hst.tx_amt;
			}
		}
		else
		{
			if( com_item.dc_ind[0]=='1' )
			{
				dfse=g_in_mst_hst.tx_amt;
			}
			else if( com_item.dc_ind[0]=='2' )
			{
				jfse=g_in_mst_hst.tx_amt;
			}
			else
			{
				dfse=g_in_mst_hst.tx_amt;
			}
		}

		fprintf( fp,"%d|%s|%s|%-s|%-s|%.2lf|%.2lf|%.2lf|%s|%s|%s|\n",
				g_in_mst_hst.tx_date, g_in_mst_hst.brf, g_mdm_ac_rel.note_no,
				g_note_parm.name, g_in_mst_hst.tel, jfse,dfse,
				g_in_mst_hst.bal ,
				ac_no,g_in_mst.name,br_name);

		memcpy( &o_in_mst_hst,&g_in_mst_hst,sizeof(g_in_mst_hst));
		hst_cnt++;
	}

	In_mst_hst_Clo_Sel();

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
	sprintf(acErrMsg,"打印内部帐户对帐单成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"打印内部帐户对帐单失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
