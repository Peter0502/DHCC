/*************************************************
* 文 件 名:  sp8503.c
* 功能描述： 会计流水查询
*
* 作    者:  peter
* 完成日期： 
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
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "com_item_c.h"
#include "dc_log_rz_c.h"
#include "trace_log_rz_c.h"
#include "tx_def_c.h"

int sp8503()
{
	struct dc_log_c vdc_log;
	struct dc_log_c f_dc_log;
	struct dc_log_c f_dc_loga;
	struct com_item_c sCom_item;
	struct com_item_c tCom_item;
	struct trace_log_c sTrace_log;
	struct trace_log_rz_c sTrace_log_rz;
	struct tx_def_c sTx_def;
	char f_dc_ind9[41];
	char f_ct_ind9[41];
	char f_note_type10[41];
	char f_sts13[41];
	char cur_no[21];
	char cAcc_no[8];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;
        int read_rz=0;/*0:读dc_log,1:读dc_log_rz*/

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(cAcc_no,0,sizeof(cAcc_no));
	memset(&sCom_item, 0x0, sizeof(struct com_item_c));
	memset(&tCom_item, 0x00,sizeof(struct com_item_c));
	memset(&sTrace_log, 0x0, sizeof(struct trace_log_c));
	memset(&sTrace_log_rz, 0x00,sizeof(struct trace_log_rz_c));
	memset(&sTx_def, 0x00,sizeof(struct tx_def_c));
	memset( wherelist, 0x00,sizeof(wherelist));

	pub_base_sysinit();

	get_zd_long("0290",&f_dc_log.trace_no );
	get_zd_data("0910",f_dc_log.tx_tx_br_no );
	get_zd_data("0210",f_dc_log.cur_no );
	get_zd_data("0640",cAcc_no);
	get_zd_data("0700",f_dc_log.dc_ind );
	get_zd_data("0660",f_dc_log.ct_ind );
	get_zd_data("0240",f_dc_log.note_type );
	get_zd_data("0590",f_dc_log.note_no );
	get_zd_double("1001",&f_dc_log.amt );
	get_zd_double("1002",&f_dc_loga.amt );
	get_zd_data("0670",f_dc_log.sts );
	get_zd_data("0920",f_dc_log.tel );
	get_zd_long("0460",&f_dc_log.tx_date );

	/**组成查询条件**/
	
	memset( tmpstr, 0x0, sizeof(tmpstr) );/*只允许查本网点---dgf---*/
	
	vtcp_log( "new WHERE wherelist [%s]",wherelist );
	if( f_dc_log.trace_no )
	{
		sprintf( tmpstr," trace_no=%ld and",f_dc_log.trace_no );
		strcat( wherelist,tmpstr );
	}
	vtcp_log("[%s][%d] trace_no = [%ld]\n",__FILE__,__LINE__,f_dc_log.trace_no);
	if( f_dc_log.tx_date )
	{
		read_rz=1;
		sprintf( tmpstr," tx_date=%ld and",f_dc_log.tx_date );
		strcat( wherelist,tmpstr );
	}
	vtcp_log("[%s][%d] tx_date = [%ld]\n",__FILE__,__LINE__,f_dc_log.tx_date);

  /*
	pub_base_strpack( f_dc_log.acc_hrt );
	if( strlen(f_dc_log.acc_hrt) )
	{
		ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_no='%s'",
			f_dc_log.acc_hrt);
		if (ret == 100)
		{
			sprintf(acErrMsg, "科目表没有该科目号!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L129");
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg, "查询公共科目表异常! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "L128");
			goto ErrExit;
		}
		sprintf( tmpstr," acc_hrt='%s' and",sCom_item.acc_hrt );
		strcat( wherelist,tmpstr );
	}*/
	pub_base_strpack(cAcc_no);
	if(strlen(cAcc_no))
	{
	  ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_no='%s'",
			cAcc_no);
		if (ret == 100)
		{
			sprintf(acErrMsg, "科目表没有该科目号!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L129");
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg, "查询公共科目表异常! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "L128");
			goto ErrExit;
		}
		sprintf( tmpstr," acc_hrt='%s' and",sCom_item.acc_hrt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_dc_log.dc_ind );
	if( strlen(f_dc_log.dc_ind) )
	{
		sprintf( tmpstr," dc_ind='%s' and",f_dc_log.dc_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_dc_log.ct_ind );
	if( strlen(f_dc_log.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_dc_log.ct_ind );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_dc_log.tel );
	if( strlen(f_dc_log.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_dc_log.tel );
		strcat( wherelist,tmpstr );
	}
	if( f_dc_log.amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_dc_log.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_dc_loga.amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_dc_loga.amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_dc_log.sts );
	if( strlen(f_dc_log.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_dc_log.sts );
		strcat( wherelist,tmpstr );
	}
	if(read_rz == 0)
		strcpy( tablelist,"dc_log" );
	else if(read_rz == 1)
		strcpy( tablelist,"dc_log_rz" );


	pub_base_strpack( f_dc_log.opn_br_no );
	if( strlen(f_dc_log.tx_tx_br_no) )
	{
		sprintf( tmpstr," tx_tx_br_no='%s' and",f_dc_log.tx_tx_br_no );
		strcat( wherelist,tmpstr );
	}
	else
	{
		sprintf( tmpstr, " (tx_opn_br_no='%s' or tx_tx_br_no='%s') and", g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
		strcat( wherelist, tmpstr);
	}
	vtcp_log( "WHERE wherelist [%s]",wherelist );
	/**组成查询
	if(read_rz == 0)
		strcat(wherelist," 1=1 order by trace_no,trace_cnt,dc_seqn") ;
	else
	**/
	/*strcat(wherelist," tel not like '%%%%99' and 1=1 order by trace_no,trace_cnt,dc_seqn") ;*/
	strcat(wherelist," 1=1 order by trace_no,trace_cnt,dc_seqn") ;

	vtcp_log( "WHERE wherelist [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,dc_seqn,opn_br_no,tx_opn_br_no,tx_tx_br_no,cur_no,acc_hrt,dc_ind,ct_ind,note_type,note_no,amt,sts");
 /*如果输入日期就查询dc_log_rz,否则如果不输入日期就查询本日的会计流水dc_log*/
	if(read_rz == 0)
	{
vtcp_log("从dc_log中查找记录");
		ret=Dc_log_Dec_Sel( g_pub_tx.reply,wherelist );
		ERR_DEAL

			ttlnum=0;
		while(1)
		{
			ret=Dc_log_Fet_Sel( &vdc_log, g_pub_tx.reply );
			if( ret==100 ) break;
			ERR_DEAL

			if( !ttlnum )
			{
				pub_base_AllDwnFilName( filename );
				fp = fopen( filename,"w" );
				if( fp==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",filename );
					WRITEMSG
					strcpy( g_pub_tx.reply,"S047" );
					goto ErrExit;
				}

				fprintf( fp,"~流水号|@借贷|@科目号|@交易名称|$发生额|@现转|@凭证种类|@凭证号|@状态|@交易机构|@开户机构|@柜员|交易日期|\n" );
			}

			if(vdc_log.trace_no<-2147000000) vdc_log.trace_no=0;
			if(vdc_log.trace_cnt<-2147000000) vdc_log.trace_cnt=0;
			pub_base_dic_show_str(f_ct_ind9,"ct_ind",vdc_log.ct_ind);
			pub_base_dic_show_str(f_note_type10,"note_type",vdc_log.note_type);
			if(vdc_log.amt<-2147000000) vdc_log.amt=0.0;
			pub_base_dic_show_str(f_sts13,"log_sts",vdc_log.sts);
			pub_base_dic_show_str(cur_no,"cur_name",vdc_log.cur_no);
		
			/* 表外科目记"收付" */
			if( vdc_log.acc_hrt[0]=='6' || vdc_log.acc_hrt[0]=='7' )
			{
				pub_base_dic_show_str(f_dc_ind9,"out_dc_ind",vdc_log.dc_ind);
			}else{
				/* 其他记"借贷" */
				pub_base_dic_show_str(f_dc_ind9,"dc_ind",vdc_log.dc_ind);
			}
			/***得到相应的科目号 add by ligl 2006-9-17 9:50***/
			ret=Com_item_Sel(g_pub_tx.reply, &tCom_item, "acc_hrt='%s'",vdc_log.acc_hrt);
			if (ret == 100)
			{
				sprintf(acErrMsg, "科目表没有该科目控制字![%s][%d]",vdc_log.acc_hrt,vdc_log.trace_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "L129");
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "查询公共科目表异常! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "L128");
				goto ErrExit;
			}
			/******end*******/	
			/***得到交易名称 add by wanglei 20070607***/
			ret=Trace_log_Dec_Sel(g_pub_tx.reply, "trace_no=%ld and trace_cnt = %d ",vdc_log.trace_no,vdc_log.trace_cnt);
			ERR_DEAL
			while(1)
			{
				ret=Trace_log_Fet_Sel( &sTrace_log, g_pub_tx.reply );
				if( ret==100 ){
					vtcp_log("trace_log里没这流水\n");
					 break;
				}
				memset(&sTx_def, 0x00,sizeof(struct tx_def_c));
				ret = Tx_def_Sel(g_pub_tx.reply,&sTx_def,"tx_code = '%s'",sTrace_log.tx_code);
				if(ret == 100)
				{
				/*	sprintf(acErrMsg, "交易定义表中无此交易信息!tx_code=[%s]",sTrace_log.tx_code);
					WRITEMSG
					strcpy(g_pub_tx.reply, "S007");
					goto ErrExit;*/
			 	}
			 	else if ( ret )
			 	{
					sprintf(acErrMsg, "查询交易定义表异常! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D103");
					goto ErrExit;
			 	}
			}
			ret=Trace_log_Clo_Sel( );
				fprintf( fp, "%ld|%s|%s|%s|%.2lf|%s|%s|%s|%s|%s|%s|%s|%ld|\n",vdc_log.trace_no,f_dc_ind9,tCom_item.acc_no,sTx_def.tx_name,vdc_log.amt,f_ct_ind9,f_note_type10,vdc_log.note_no,f_sts13,vdc_log.tx_tx_br_no,vdc_log.tx_opn_br_no,vdc_log.tel,vdc_log.tx_date);
				ttlnum++;
			/******end  20070607*******/		
		}

		ret=Dc_log_Clo_Sel( );
		ERR_DEAL

	} /*end of if*/
	else if(read_rz == 1)
	{
vtcp_log("输入了日期所以从dc_log_rz中查找记录");
		ret=Dc_log_rz_Dec_Sel( g_pub_tx.reply,wherelist );
		ERR_DEAL

			ttlnum=0;

		while(1)
		{
			ret=Dc_log_rz_Fet_Sel( &vdc_log, g_pub_tx.reply );
			if( ret==100 ) break;
			ERR_DEAL

			if( !ttlnum )
			{
				pub_base_AllDwnFilName( filename );
				fp = fopen( filename,"w" );
				if( fp==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",filename );
					WRITEMSG
					strcpy( g_pub_tx.reply,"S047" );
					goto ErrExit;
				}

				/**显示列标题*/
				fprintf( fp,"~流水号|@借贷|@科目号|@交易名称|$发生额|@现转|@凭证种类|@凭证号|@状态|@交易机构|@开户机构|@柜员|交易日期|\n" );
			}

			if(vdc_log.trace_no<-2147000000) vdc_log.trace_no=0;
			if(vdc_log.trace_cnt<-2147000000) vdc_log.trace_cnt=0;
			pub_base_dic_show_str(f_ct_ind9,"ct_ind",vdc_log.ct_ind);
			pub_base_dic_show_str(f_note_type10,"note_type",vdc_log.note_type);
			if(vdc_log.amt<-2147000000) vdc_log.amt=0.0;
			pub_base_dic_show_str(f_sts13,"log_sts",vdc_log.sts);
			pub_base_dic_show_str(cur_no,"cur_name",vdc_log.cur_no);
		
			/* 表外科目记"收付" */
			if( vdc_log.acc_hrt[0]=='6' || vdc_log.acc_hrt[0]=='7' )
			{
				pub_base_dic_show_str(f_dc_ind9,"out_dc_ind",vdc_log.dc_ind);
			}else{
				/* 其他记"借贷" */
				pub_base_dic_show_str(f_dc_ind9,"dc_ind",vdc_log.dc_ind);
			}			
			/***得到相应的科目号 add by ligl 2006-9-17 9:50***/
			ret=Com_item_Sel(g_pub_tx.reply, &tCom_item, "acc_hrt='%s'",vdc_log.acc_hrt);
			if (ret == 100)
			{
				sprintf(acErrMsg, "科目表没有该科目控制字!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "L129");
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "查询公共科目表异常! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "L128");
				goto ErrExit;
			}
			/******end*******/	
			/***得到交易名称 add by wanglei 20070607***/
			ret=Trace_log_rz_Dec_Sel(g_pub_tx.reply, "trace_no=%ld and trace_cnt = %d  and tx_date = %ld ",\
				 vdc_log.trace_no,vdc_log.trace_cnt,vdc_log.tx_date);
			ERR_DEAL
			while(1)
			{
				ret=Trace_log_rz_Fet_Sel( &sTrace_log, g_pub_tx.reply );
				if( ret==100 ) break;

				memset(&sTx_def, 0x00,sizeof(struct tx_def_c));
				ret = Tx_def_Sel(g_pub_tx.reply,&sTx_def,"tx_code = '%s'",sTrace_log.tx_code);
				if(ret == 100)
				{
					/*sprintf(acErrMsg, "交易定义表中无此交易信息!tx_code=[%s]",sTrace_log.tx_code);
					WRITEMSG
					strcpy(g_pub_tx.reply, "S007");
					goto ErrExit;*/
				}
				else if ( ret )
				{
					sprintf(acErrMsg, "查询交易定义表异常! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D103");
					goto ErrExit;
				}
				/* added over */
			}
			ret=Trace_log_rz_Clo_Sel( );
				fprintf( fp, "%ld|%s|%s|%s|%.2lf|%s|%s|%s|%s|%s|%s|%s|%ld|\n",vdc_log.trace_no,f_dc_ind9,tCom_item.acc_no,sTx_def.tx_name,vdc_log.amt,f_ct_ind9,f_note_type10,vdc_log.note_no,f_sts13,vdc_log.tx_tx_br_no,vdc_log.tx_opn_br_no,vdc_log.tel,vdc_log.tx_date);
				ttlnum++;
		}
	
		ret=Dc_log_rz_Clo_Sel( );
		ERR_DEAL
	} /*end of elseif*/

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
