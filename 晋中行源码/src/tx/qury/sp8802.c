/*************************************************
* 文 件 名:  sp8802.c
* 功能描述： 当日/历史流水帐查询(改)
*
* 作    者:   gyf
* 完成日期： 
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error [%s][%d][%d]",__FILE__,__LINE__,ret ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "trace_parm_c.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "trace_log_bk_c.h"
#include "dc_log_c.h"

int sp8802()
{
	struct trace_log_c vtrace_log;
	struct trace_log_c strace_log;
	struct trace_log_c Strace_log;
	struct trace_log_c f_trace_log;
	struct trace_log_c f_trace_loga;
	struct dd_mst_c s_dd_mst;
  struct ln_mst_c s_ln_mst;
  struct td_mst_c s_td_mst;
  struct in_mst_c s_in_mst;
  struct dc_log_c dc_log;

	char f_note_type9[41];
	char f_add_ind18[41];
	char f_ct_ind18[41];
	char f_sts18[41];
	char cur_no[21];
	char log_sts[2];
	char ac_name[50];
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
	  int c_old_date=0;
	  int c_old_wssrno=0;
		FILE *fp;
	long rq1=0,rq2=0;
	int read_bk=0;

	memset( &vtrace_log,0,sizeof(vtrace_log));
	memset( &strace_log,0,sizeof(strace_log));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(ac_name, 0x0, sizeof(ac_name));
	memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&s_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&s_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&s_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&dc_log, 0x00,sizeof(struct dc_log_c));
	
	pub_base_sysinit();
	
	get_zd_data("0710",log_sts );	
	get_zd_long("0290",&f_trace_log.trace_no );
	get_zd_data("0910",f_trace_log.tx_br_no );
	get_zd_data("0380",f_trace_log.ac_no );
	get_zd_long("0510",&f_trace_log.ac_seqn );
	get_zd_data("0240",f_trace_log.note_type );
	get_zd_data("0590",f_trace_log.note_no );
	get_zd_data("0700",f_trace_log.add_ind );
	get_zd_double("1001",&f_trace_log.amt );
	get_zd_double("1002",&f_trace_loga.amt );
	get_zd_data("0660",f_trace_log.ct_ind );
	get_zd_data("0630",f_trace_log.brf );
	get_zd_data("0640",f_trace_log.tel );
	get_zd_data("0650",f_trace_log.chk );
	get_zd_data("0870",f_trace_log.aut );
	get_zd_long("0440",&rq1);
	get_zd_long("0450",&rq2);

	/**组成查询条件**/
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	if(rq1==0 && rq2==0)
		read_bk=0;
	else
		read_bk=1;
	
	if( log_sts[0]=='0' )
	{
		sprintf( tmpstr," sts='%s' and",log_sts );
		strcat( wherelist,tmpstr );
	}else if( log_sts[0]=='1' ){
		sprintf( tmpstr," sts='%s' and",log_sts );
		strcat( wherelist,tmpstr );
	}else if( log_sts[0]=='2' ){
		strcpy( tmpstr," sts in ('0' ,'1')  and" );
		strcat( wherelist,tmpstr );
	}
		
	if( f_trace_log.trace_no )
	{
		sprintf( tmpstr," trace_no=%ld and",f_trace_log.trace_no );
		strcat( wherelist,tmpstr );
	}
	
	/*------ 只允许查本网点的话,则输入机构就无意义了 ------------
	----------------------------------------------------------*/
	pub_base_strpack( f_trace_log.tx_br_no );
	if( strlen(f_trace_log.tx_br_no) )
	{
		sprintf( tmpstr," tx_br_no='%s' and",f_trace_log.tx_br_no );
		strcat( wherelist,tmpstr );
	}
	else
	{
		sprintf( tmpstr," (tx_br_no='%s' or opn_br_no='%s') and",g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.ac_no );
	if( strlen(f_trace_log.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_trace_log.ac_no );
		strcat( wherelist,tmpstr );
	}
	if( f_trace_log.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_trace_log.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.note_type );
	if( strlen(f_trace_log.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_trace_log.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.note_no );
	if( strlen(f_trace_log.note_no) )
	{
		sprintf( tmpstr," note_no='%s' and",f_trace_log.note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.add_ind );
	if( strlen(f_trace_log.add_ind) )
	{
		sprintf( tmpstr," add_ind='%s' and",f_trace_log.add_ind );
		strcat( wherelist,tmpstr );
	}
	if( pub_base_CompDblVal(f_trace_log.amt,0.00) )
	{
		sprintf( tmpstr," amt>=%lf and",f_trace_log.amt );
		strcat( wherelist,tmpstr );
	}
	if( pub_base_CompDblVal(f_trace_loga.amt,0.00) )
	{
		sprintf( tmpstr," amt<=%lf and",f_trace_loga.amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.ct_ind );
	if( strlen(f_trace_log.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_trace_log.ct_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.brf );
	if( strlen(f_trace_log.brf) )
	{
		sprintf( tmpstr," brf like '%%%%%s%%%%' and",f_trace_log.brf );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.tel );
	if( strlen(f_trace_log.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_trace_log.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.chk );
	if( strlen(f_trace_log.chk) )
	{
		sprintf( tmpstr," chk='%s' and",f_trace_log.chk );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log.aut );
	if( strlen(f_trace_log.aut) )
	{
		sprintf( tmpstr," aut='%s' and",f_trace_log.aut );
		strcat( wherelist,tmpstr );
	}

	/* 限制只能取当日交易记录 改为可以查昨天的交易记录,根据输入的日期判断
	sprintf( tmpstr," tx_date=%ld and ",g_pub_tx.tx_date );
	strcat( wherelist , tmpstr );*/

	if(read_bk==0)
	{
		sprintf(tmpstr," tx_date=%ld and ",g_pub_tx.tx_date);
		strcat(wherelist,tmpstr);
		strcpy(tablelist,"trace_log");
	}
	else if(read_bk==1)
	{
		if( rq2>0 )
			sprintf(tmpstr," tx_date>=%ld and tx_date<=%ld and ",rq1,rq2);
		else
			sprintf(tmpstr," tx_date=%ld and ",rq1);
		strcat(wherelist,tmpstr);
		strcpy(tablelist,"trace_log_bk");
	}

	/**组成查询**/
	strcat(wherelist," no_show!='1' order by trace_no,trace_cnt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,tx_time,tx_br_no,opn_br_no,svc_ind,cur_no,ac_no,ac_seqn,note_type,note_no,add_ind,amt,ct_ind,brf,tel,chk,aut,sts");
	
	if(read_bk==0)
	{
		ret=Trace_log_Dec_Sel( g_pub_tx.reply,wherelist );
		ERR_DEAL

		ttlnum=0;

		while(1)
		{
			memset(ac_name, 0x0, sizeof(ac_name));
			ret=Trace_log_Fet_Sel( &strace_log, g_pub_tx.reply );
			if( ret==100 )
			{
				if(ttlnum)
				{
					fprintf( fp, 
					"%s|%ld|%ld|%s|%d|%s|%s|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|\n",
					f_sts18,vtrace_log.trace_no,vtrace_log.trace_cnt,vtrace_log.ac_no,
					vtrace_log.ac_seqn,ac_name,f_add_ind18,vtrace_log.amt,f_ct_ind18,
					vtrace_log.brf,f_note_type9,vtrace_log.note_no,vtrace_log.opn_br_no,
					vtrace_log.tx_time,vtrace_log.tx_br_no,vtrace_log.tel,
					vtrace_log.chk,vtrace_log.aut );
				}
				break;
			}
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

			/**显示列标题**/
				fprintf( fp,"~@状态|流水号|笔次|@账号|序号|户名|@借贷|$发生额|@现转|@摘要|@凭证种类|@凭证号|@开户行|交易时间|@交易行|@操作员|@复核员|@授权人|\n" );
			}

		/* add by martin 20061031*/
		/*in_mst*/ 
     	ret=In_mst_Sel( g_pub_tx.reply, &s_in_mst , "ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
     	if(ret ==0)
     	{
     		pub_base_strpack( s_in_mst.name );
	   		strcpy(ac_name, s_in_mst.name);
     	}
      /*td_mst*/
      	ret=Td_mst_Sel( g_pub_tx.reply, &s_td_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
    	if(ret==0)
     	{
			pub_base_strpack( s_td_mst.name );
			strcpy(ac_name, s_td_mst.name);
     	}
      /*dd_mst*/
      	ret=Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
    	if(ret==0)
     	{
     	  	pub_base_strpack( s_dd_mst.name );
	       	strcpy(ac_name, s_dd_mst.name);
     	}
      /*ln_mst*/
      	ret=Ln_mst_Sel( g_pub_tx.reply, &s_ln_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
   		if(ret==0)
     	{
			pub_base_strpack( s_ln_mst.name );
	        strcpy(ac_name, s_ln_mst.name);
		}
		/* 行内现金收付交易将对方机构号放在了凭证号字段 */
	vtcp_log("1111111111vtrace [%s]",vtrace_log.sub_tx_code);
		if((memcmp(vtrace_log.tx_code,"5202",4)==0 && memcmp(vtrace_log.sub_tx_code,"M202",4)==0))
			strcpy(ac_name,vtrace_log.note_no);

			/*  Del by martin 20070403
			if(c_old_date!=strace_log.tx_date || c_old_wssrno!=strace_log.trace_no)
			{
			*/
		    	if(ttlnum)
		    	{
					fprintf( fp, 
				"%s|%ld|%ld|%s|%d|%s|%s|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|\n",
				f_sts18,vtrace_log.trace_no,vtrace_log.trace_cnt,vtrace_log.ac_no,
				vtrace_log.ac_seqn,ac_name,f_add_ind18,vtrace_log.amt,f_ct_ind18,
				vtrace_log.brf,f_note_type9,vtrace_log.note_no,vtrace_log.opn_br_no,
				vtrace_log.tx_time,vtrace_log.tx_br_no,vtrace_log.tel,
				vtrace_log.chk,vtrace_log.aut );
				
                 }

		   	 memcpy(&vtrace_log,&strace_log,sizeof(vtrace_log));
		  /*
			}
			*/
				
			ret = iput_all_fld(&vtrace_log,&strace_log);
			ERR_DEAL

			ret=Dc_log_Sel( g_pub_tx.reply,&dc_log,"trace_no=%d and trace_cnt=%d",vtrace_log.trace_no,vtrace_log.trace_cnt);
			strcpy(vtrace_log.add_ind,dc_log.dc_ind);
			vtcp_log("dclog_ind[%s]\n",dc_log.dc_ind);

			pub_base_dic_show_str(f_note_type9,"note_type",vtrace_log.note_type);
			pub_base_dic_show_str(f_add_ind18,"trace_add_ind",vtrace_log.add_ind);
			pub_base_dic_show_str(f_ct_ind18,"ct_ind",vtrace_log.ct_ind);
			pub_base_dic_show_str(f_sts18,"log_sts",vtrace_log.sts);
			pub_base_dic_show_str(cur_no,"cur_name",vtrace_log.cur_no);


			c_old_date=vtrace_log.tx_date;
			c_old_wssrno=vtrace_log.trace_no;

			ttlnum++;
		}

		ret=Trace_log_Clo_Sel( );
		ERR_DEAL
	}
	else if(read_bk==1)
	{
		ret=Trace_log_rz_Dec_Sel( g_pub_tx.reply,wherelist );
		ERR_DEAL

		ttlnum=0;

		while(1)
		{
			memset(ac_name, 0x0, sizeof(ac_name));
			ret=Trace_log_rz_Fet_Sel( &strace_log, g_pub_tx.reply );
			if( ret==100 )
			{
				if(ttlnum)
				{
					fprintf( fp, 
					"%s|%ld|%ld|%s|%d|%s|%s|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|\n",
					f_sts18,vtrace_log.trace_no,vtrace_log.trace_cnt,vtrace_log.ac_no,
					vtrace_log.ac_seqn,ac_name,f_add_ind18,vtrace_log.amt,f_ct_ind18,
					vtrace_log.brf,f_note_type9,vtrace_log.note_no,vtrace_log.opn_br_no,
					vtrace_log.tx_time,vtrace_log.tx_br_no,vtrace_log.tel,
					vtrace_log.chk,vtrace_log.aut );
				}
				break;
			}
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

			/**显示列标题**/
				fprintf( fp,"~@状态|流水号|笔次|@账号|序号|户名|@借贷|$发生额|@现转|@摘要|@凭证种类|@凭证号|@开户行|交易时间|@交易行|@操作员|@复核员|@授权人|\n" );
			}


		/* add by martin 20061031*/
			/*in_mst*/ 
     		ret=In_mst_Sel( g_pub_tx.reply, &s_in_mst , "ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
     			if(ret ==0)
     			{
     		  				pub_base_strpack( s_in_mst.name );
	        				strcpy(ac_name, s_in_mst.name);
     	  		}
      /*td_mst*/
      		ret=Td_mst_Sel( g_pub_tx.reply, &s_td_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
    	 	if(ret==0)
     		{
     			pub_base_strpack( s_td_mst.name );
	        	strcpy(ac_name, s_td_mst.name);
     	  	}
      /*dd_mst*/
      	ret=Dd_mst_Sel( g_pub_tx.reply, &s_dd_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
     			if(ret==0)
     			{
     		  	pub_base_strpack( s_dd_mst.name );
	        	strcpy(ac_name, s_dd_mst.name);
     	  		}
      /*ln_mst*/
      	ret=Ln_mst_Sel( g_pub_tx.reply, &s_ln_mst , "1=1 and ac_id='%ld' and ac_seqn='%ld' ",vtrace_log.ac_id,vtrace_log.ac_seqn);
    		if(ret==0)
     		{
				pub_base_strpack( s_ln_mst.name );
				strcpy(ac_name, s_ln_mst.name);
			}

		/* 行内现金收付交易将对方机构号放在了凭证号字段 */
		if((memcmp(vtrace_log.tx_code,"5202",4)==0 && memcmp(vtrace_log.sub_tx_code,"M202",4)==0))
			strcpy(ac_name,vtrace_log.note_no);
	
			if(c_old_date!=strace_log.tx_date || c_old_wssrno!=strace_log.trace_no)
			{
		    	if(ttlnum)
		    	{
					fprintf( fp, 
				"%s|%ld|%ld|%s|%d|%s|%s|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|\n",
				f_sts18,vtrace_log.trace_no,vtrace_log.trace_cnt,vtrace_log.ac_no,
				vtrace_log.ac_seqn,ac_name,f_add_ind18,vtrace_log.amt,f_ct_ind18,
				vtrace_log.brf,f_note_type9,vtrace_log.note_no,vtrace_log.opn_br_no,
				vtrace_log.tx_time,vtrace_log.tx_br_no,vtrace_log.tel,
				vtrace_log.chk,vtrace_log.aut );
                    }

		   	 memcpy(&vtrace_log,&strace_log,sizeof(vtrace_log));
			}

				
			ret = iput_all_fld(&vtrace_log,&strace_log);
			ERR_DEAL

			ret=Dc_log_bk_Sel( g_pub_tx.reply,&dc_log,"trace_no=%d and trace_cnt=%d",vtrace_log.trace_no,vtrace_log.trace_cnt);
			strcpy(vtrace_log.add_ind,dc_log.dc_ind);

			pub_base_dic_show_str(f_note_type9,"note_type",vtrace_log.note_type);
			pub_base_dic_show_str(f_add_ind18,"trace_add_ind",vtrace_log.add_ind);
			pub_base_dic_show_str(f_ct_ind18,"ct_ind",vtrace_log.ct_ind);
			pub_base_dic_show_str(f_sts18,"log_sts",vtrace_log.sts);
			pub_base_dic_show_str(cur_no,"cur_name",vtrace_log.cur_no);


			c_old_date=vtrace_log.tx_date;
			c_old_wssrno=vtrace_log.trace_no;

			ttlnum++;
		}

		ret=Trace_log_rz_Clo_Sel( );
		ERR_DEAL
	}

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
int iput_all_fld(vtrace_log,strace_log)
struct trace_log_c *vtrace_log;
struct trace_log_c *strace_log;
{
	int ret = 0;
	struct trace_parm_c trace_parm;

	ret=Trace_parm_Dec_Sel( g_pub_tx.reply," tx_code='%s' and list='%s' ",  \
			    strace_log->tx_code,strace_log->sub_tx_code);
	if (ret == 100 )
	{
		/*
		   sprintf( acErrMsg, "该子交易没有执行[%s] [%s][%d][%d]",strace_log->sub_tx_code,__FILE__,__LINE__,ret ); 
		   WRITEMSG 
		*/
		   return(0);
	}else if( ret ) {
		   sprintf( acErrMsg, "sql error [%s][%d][%d]",__FILE__,__LINE__,ret ); 
		   WRITEMSG 
		   return(ret);
	}

	while(1)
	{
		ret=Trace_parm_Fet_Sel( &trace_parm, g_pub_tx.reply );
		if( ret==100 ) break;
		if(ret)
		{
		   sprintf( acErrMsg, "sql error [%s][%d][%d]",__FILE__,__LINE__,ret ); 
		   WRITEMSG 
		   return(ret);
		}

		pub_base_strpack( trace_parm.field );

		ret = iput_fld(vtrace_log,strace_log,trace_parm.field);
		if( ret ) {
		   sprintf( acErrMsg, "sql error [%s][%d][%d]",__FILE__,__LINE__,ret ); 
		   WRITEMSG 
		   return(ret);
		}
	}

	ret=Trace_parm_Clo_Sel( );

	return(0);
}

int iput_fld(vtrace_log,strace_log,fld)
struct trace_log_c *vtrace_log;
struct trace_log_c *strace_log;
char   *fld;
{
	if(!strcmp(fld,"trace_no"))
	{
		vtrace_log->trace_no = strace_log->trace_no;
	}else if(!strcmp(fld,"trace_cnt")){
		vtrace_log->trace_cnt = strace_log->trace_cnt;
	}else if(!strcmp(fld,"tx_date")){
		vtrace_log->tx_date = strace_log->tx_date;
	}else if(!strcmp(fld,"tx_time")){
		vtrace_log->tx_time = strace_log->tx_time;
	}else if(!strcmp(fld,"tx_br_no")){
		strcpy(vtrace_log->tx_br_no , strace_log->tx_br_no);
	}else if(!strcmp(fld,"opn_br_no")){
		strcpy(vtrace_log->opn_br_no , strace_log->opn_br_no);
	}else if(!strcmp(fld,"tx_code")){
		strcpy(vtrace_log->tx_code , strace_log->tx_code);
	}else if(!strcmp(fld,"sub_tx_code")){
		strcpy(vtrace_log->sub_tx_code , strace_log->sub_tx_code);
	}else if(!strcmp(fld,"svc_ind")){
		vtrace_log->svc_ind = strace_log->svc_ind;
	}else if(!strcmp(fld,"cur_no")){
		strcpy(vtrace_log->cur_no , strace_log->cur_no);
	}else if(!strcmp(fld,"prdt_no")){
		strcpy(vtrace_log->prdt_no , strace_log->prdt_no);
	}else if(!strcmp(fld,"ac_no")){
		strcpy(vtrace_log->ac_no , strace_log->ac_no);
	}else if(!strcmp(fld,"ac_seqn")){
		vtrace_log->ac_seqn = strace_log->ac_seqn;
	}else if(!strcmp(fld,"ac_id")){
		vtrace_log->ac_id = strace_log->ac_id;
	}else if(!strcmp(fld,"note_type")){
		strcpy(vtrace_log->note_type , strace_log->note_type);
	}else if(!strcmp(fld,"note_no")){
		strcpy(vtrace_log->note_no , strace_log->note_no);
	}else if(!strcmp(fld,"add_ind")){
		strcpy(vtrace_log->add_ind , strace_log->add_ind);
	}else if(!strcmp(fld,"amt")){
		vtrace_log->amt = strace_log->amt;
	}else if(!strcmp(fld,"ct_ind")){
		strcpy(vtrace_log->ct_ind , strace_log->ct_ind);
	}else if(!strcmp(fld,"brf")){
		strcpy(vtrace_log->brf , strace_log->brf);
	}else if(!strcmp(fld,"tel")){
		strcpy(vtrace_log->tel , strace_log->tel);
	}else if(!strcmp(fld,"chk")){
		strcpy(vtrace_log->chk , strace_log->chk);
	}else if(!strcmp(fld,"aut")){
		strcpy(vtrace_log->aut , strace_log->aut);
	}else if(!strcmp(fld,"hst_ind")){
		strcpy(vtrace_log->hst_ind , strace_log->hst_ind);
	}else if(!strcmp(fld,"saving_notarize")){
		strcpy(vtrace_log->saving_notarize , strace_log->saving_notarize);
	}else if(!strcmp(fld,"no_show")){
		strcpy(vtrace_log->no_show , strace_log->no_show);
	}else if(!strcmp(fld,"sts")){
		strcpy(vtrace_log->sts , strace_log->sts);
	}

	return(0);
}
