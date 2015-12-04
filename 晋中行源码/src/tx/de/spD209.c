/***************************************************************
* 文 件 名: spD209.c
* 功能描述：销介质业务
* 作    者: jack
* 完成日期：2004年5月27日
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
#include "mdm_attr_c.h"
#include "mdm_unprt_hst_c.h"

spD209()
{
	int	ret=0;
	int	num_dd=0;
	int	num_td=0;
	char tmp_ac_no[20],tmp_tx_name[21];
	FILE *fp;
	char tmpname[100];
	
	struct prdt_ac_id_c s_prdt_ac_id;
	struct mdm_attr_c sMdm_attr;
	struct mdm_unprt_hst_c sMdm_unprt_hst;
			
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&s_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));	
	memset(&sMdm_attr, 0x0, sizeof(struct mdm_attr_c));
	memset(&sMdm_unprt_hst, 0x0, sizeof(struct mdm_unprt_hst_c));
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("1051",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_data("1054",g_pub_tx.draw_pwd_yn);	
	get_zd_data("1056",g_pub_tx.draw_pwd);
	get_zd_data("1057",g_pub_tx.draw_id_yn);	
	get_zd_data("105A",g_pub_tx.id_type);	
	get_zd_data("1058",g_pub_tx.id_no);	

 	ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						g_pub_tx.ac_no);
	if( ret==100 )
	{
	  	sprintf(acErrMsg,"介质帐号关系表不存在该记录!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg, "查询介质账号关系表异常! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

 	ret = Mdm_attr_Sel( g_pub_tx.reply,&sMdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
	  	sprintf(acErrMsg,"介质不存在![%s]",g_mdm_ac_rel.mdm_code );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg, "查询异常! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( sMdm_attr.ac_num_ind[0]=='N' )
	{
		/*** 一本通处理 ***/
		if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
		{
			ret = sql_count( "td_mst", "ac_id=%ld and ac_sts='1'", \
							 g_mdm_ac_rel.ac_id );
			if( ret < 0 )
			{		
   	 			sprintf(acErrMsg,"调用sql_count函数错误![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			else if( ret )
			{
    			sprintf(acErrMsg,"该一本通户存在未销户记录,不能作此交易!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D250");
 				goto ErrExit;
			}
		}
	}

	ret=sql_count("mdm_ac_rel", \
  			  "ac_id=%ld and note_sts not in('2','3','4','5','*' )", \
			  g_mdm_ac_rel.ac_id);
	if( ret < 0 )
	{
   	 	sprintf(acErrMsg,"调用sql_count函数错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if( ret==1 )
	{
		/*--------del by dgf 20051115 for 卡等也不能直接销介质---------*
		if( sMdm_attr.ac_num_ind[0]=='Y' )
		{
   	 		sprintf(acErrMsg,"该账户只存在唯一介质,不能作此交易![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O178");
 			goto ErrExit;
		}
		*------------------------------------------------------------*/
		
		num_dd=sql_count("dd_mst", \
  			  "ac_id=%ld and ac_sts not in('*')", \
			  g_mdm_ac_rel.ac_id);
		num_td=sql_count("td_mst", \
  			  "ac_id=%ld and ac_sts not in('*')", \
			  g_mdm_ac_rel.ac_id);				
		if( num_dd+num_td )
		{	
			sprintf(acErrMsg,"该账户只存在唯一介质,不能作此交易![%d][%d]",num_dd,num_td);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O178");
	 		goto ErrExit;
	 	}
	}
	else if( ret>1 )
	{
		if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) && \
			g_mdm_ac_rel.main_ind[0]=='1' )
		{
			ret=sql_count("mdm_ac_rel", \
				"ac_id=%ld and note_sts not in('2','3','4','5','*' )and main_ind='2'",g_mdm_ac_rel.ac_id);
			if( ret>0 )
			{
    			sprintf(acErrMsg,"该卡存在副卡，请先销副卡再作此交易![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O200");
 				goto ErrExit;
			}
			else if( ret<0 )
			{
    			sprintf(acErrMsg,"TRANSFER FUNCTION SQL_COUNT ERROR!![%d]",ret);
				WRITEMSG
 				goto ErrExit;
			}
		}
	}

	memset( &sMdm_attr, 0x0, sizeof(struct mdm_attr_c) );

	/*** 卡配折的情况，检查卡内是否包含定期户 ***/
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) && \
		g_mdm_ac_rel.main_ind[0]!='2' )
	{
		ret = sql_count("td_mst","ac_id=%d and ac_sts='1'",g_mdm_ac_rel.ac_id);
		if( ret>0 )
		{
    		sprintf(acErrMsg,"该卡存在账户![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P453");
 			goto ErrExit;
		}
	}

	/* add by rob at 20050726 ***/
	if( strlen(g_mdm_ac_rel.note_no) && \
		strcmp(g_pub_tx.beg_note_no,g_mdm_ac_rel.note_no) ) 
	{ 
		sprintf(acErrMsg,"凭证号不符![%s][%s]", g_pub_tx.beg_note_no, 
				g_mdm_ac_rel.note_no); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"P169"); 
		goto ErrExit; 
	}

	switch( g_mdm_ac_rel.note_sts[0] ) 
	{ 
		case '1': 
			sprintf(acErrMsg,"该介质已经被挂失![%s]",g_mdm_ac_rel.note_sts); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"P170"); 
			goto ErrExit; 
		case '2': 
			sprintf(acErrMsg,"该介质已经被挂失换证![%s]",
				g_mdm_ac_rel.note_sts); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"P171"); 
			goto ErrExit; 
		case '*': 
			sprintf(acErrMsg,"该介质已经销户![%s]",g_mdm_ac_rel.note_sts); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"P172"); 
			goto ErrExit; 
		default: 
			break; 
	}

	/*** 判断捡拾 ***/
	if( g_mdm_ac_rel.coll_sts[0]=='1' ) 
	{ 
		ret = pub_base_checkcollyn( g_mdm_ac_rel.ac_no );
		if( ret )
		{
			sprintf(acErrMsg,"调捡拾函数出错![%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		}
	}

	/*** 检查支取方式函数 ***/
	if( pub_base_check_draw("001") )
	{
	  	sprintf(acErrMsg,"检查支取方式错误!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	pub_base_strpack(g_mdm_ac_rel.mdm_code); 

	ret = Mdm_attr_Sel( g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", 
						g_mdm_ac_rel.mdm_code); 
	if( ret==100 ) 
	{ 
		sprintf(acErrMsg, "介质属性表中不存在该记录!! "); 
		WRITEMSG 
		strcpy(g_pub_tx.reply, "N055"); 
		goto ErrExit; 
	}
	else if( ret ) 
	{ 
		sprintf(acErrMsg, "查询介质属性表错误!! [%d]", ret); 
		WRITEMSG 
		strcpy(g_pub_tx.reply, "N056"); 
		goto ErrExit; 
	} 

	/*** 折类账号检查是否有未登明细 ***/
	if( sMdm_attr.prt_ind[0]=='Y' ) 
	{ 
		ret = sql_count("mdm_unprt_hst", "ac_id=%ld and ac_seqn=%d", 
						g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn); 
		if (ret < 0) 
		{ 
			sprintf(acErrMsg, "查询未登折明细表异常!! [%d]", ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "O239"); 
			goto ErrExit; 
		} 
		else if (ret > 0) 
		{ 
			sprintf(acErrMsg, "未登折明细表中还有记录!! [%d]", ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "O240"); 
			goto ErrExit; 
		} 
	}
	/** add end ****/

	ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"账号不存在!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"FETCH ERROR!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_mdm_ac_rel.note_sts,"*" );

	ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"UPDATE CURSOR ERROR!");
		WRITEMSG
		goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd( );

	g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply , &s_prdt_ac_id , \
								  "ac_id=%ld" , g_mdm_ac_rel.ac_id );
	if( g_reply_int==100 )
	{
	   	sprintf(acErrMsg,"账号不存在[%ld]错",g_mdm_ac_rel.ac_id );
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		return 1;
	}	
	else if( g_reply_int )
	{
	   	sprintf(acErrMsg,"查找出错[%d]错",g_reply_int );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		return 1;
	}	

	/**** add by lance using for print 注销申请 ****/
	strcpy( tmp_ac_no , "XXXXXXXXXXXXXXXXXXX" );
	strcpy( tmp_tx_name , "注销" );
	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"打开文件[%s]错",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	/*定期产品*/
	if(s_prdt_ac_id.prdt_no[0]=='2')
	{
		pub_base_strpack(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.name);
		pub_base_strpack(g_td_parm.title);

		fprintf(fp,
		"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|\n"
			,tmp_ac_no,g_pub_tx.name,g_pub_tx.tx_amt1
			,g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no
			,s_prdt_ac_id.cif_no,g_td_mst.mtr_date,"",""
			,g_td_mst.rate,g_pub_tx.tx_amt1,""
			,"","","",g_pub_tx.tx_code,""
			,g_pub_tx.ac_no1,0.00
			,"",g_pub_tx.ac_seqn,g_td_mst.ic_date
			,g_mdm_ac_rel.prt_line,g_td_mst.prdt_no,0,g_pub_tx.brf
			,g_pub_tx.ac_no,tmp_tx_name);
	}
	else if(s_prdt_ac_id.prdt_no[0]=='1')
	{
		pub_base_strpack(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.name);
		pub_base_strpack(g_dd_parm.title);

		fprintf(fp,
		"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|\n"
		,tmp_ac_no,g_pub_tx.name,g_pub_tx.tx_amt1
		,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no
		,s_prdt_ac_id.cif_no,0,"","",0.00,g_pub_tx.tx_amt1
		,"","","",""
		,g_pub_tx.tx_code,"",g_pub_tx.ac_no1,0.00,""
		,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_dd_mst.prdt_no,0,g_pub_tx.brf
		,g_pub_tx.ac_no,tmp_tx_name);
	}
	else
	{
		sprintf(acErrMsg,"s_prdt_ac_id.prdt_no[%s]错",s_prdt_ac_id.prdt_no);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/*** Add Complete ***/	
	strcpy(g_pub_tx.brf, "销介质");

	if( pub_ins_trace_log() )
	{
	    sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"销介质交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"销介质交易失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
