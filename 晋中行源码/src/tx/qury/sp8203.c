/*************************************************
* 文 件 名:  sp8203.c
* 功能描述： 贷款放/还款计划查询
*
* 作    者:  lance
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
#include "ln_pay_pln_c.h"

int sp8203()
{
	struct ln_pay_pln_c vln_pay_pln;
	struct ln_pay_pln_c ln_pay_pln_p;
	struct ln_pay_pln_c ln_pay_pln_tmp;		
	struct ln_pay_pln_c f_ln_pay_pln;
	struct ln_pay_pln_c f_ln_pay_plna;
	char ac_no0[25];
	char ac_no2[25];
	char f_pay_ind12[41];
	char f_acno0[25];
	char f_acno2[25];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char tmpmo[512];
	char fieldlist[1024];  /**要查询的字段列表**/
	char tablelist[128];   /**要查询的表名**/
	char titstr[1024];
	int ttlnum=0;			/**读取的总条数**/
	int i=0,t=1;
	int ret=0;
	int cnt=0;
	FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
    pub_base_old_acno(f_acno0);
	get_zd_long("0510",&f_ln_pay_pln.ac_seqn );
	get_zd_data("0370",f_acno2);
	get_zd_long("0500",&f_ln_pay_pln.de_ac_seqn );
	get_zd_double("1001",&f_ln_pay_pln.ttl_amt );
	get_zd_double("1002",&f_ln_pay_pln.curr_amt );
	get_zd_long("0290",&f_ln_pay_pln.beg_date );
	get_zd_long("0440",&f_ln_pay_plna.beg_date );
	get_zd_long("0450",&f_ln_pay_pln.end_date );
	get_zd_data("0700",f_ln_pay_pln.pay_ind );		

	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(f_acno0,f_ln_pay_pln.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	ERR_DEAL
	/* 若是按揭贷款 */		
	if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{			
		/* 查询按揭欠款表中欠款期数 */
		cnt = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_pub_tx.ac_id,g_pub_tx.ac_seqn );
	
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &ln_pay_pln_p , " ac_id='%ld' and ac_seqn='%d' " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );		
		if( ret )
		{
			sprintf(acErrMsg,"读取还款计划表信息error");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}
		sprintf(acErrMsg,"读取还款计划表信息cnt[%d]ln_pay_pln_p.curr_cnt[%d]",cnt,ln_pay_pln_p.curr_cnt);
		WRITEMSG		
		/* 计算从哪期开始未还 */
		if(!cnt)
		{
			TRACE
			cnt = ln_pay_pln_p.curr_cnt;
		}else{
			TRACE
			cnt = ln_pay_pln_p.curr_cnt-cnt;
		}
		sprintf(acErrMsg,"cnt[%d]",cnt);
		WRITEMSG
		/* 生成全部还款计划 */
		pub_base_AllDwnFilName( filename );
		fp = fopen( filename,"w" );
		if( fp==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}
		TRACE
		/**显示列标题**/
		fprintf( fp,"~账号|账户序号|@账号|放款账号|放款账户序号|$总金额|总期数|当前期次|$本期金额|$当前期次应还利息|登记日期|开始归还日期|到期日期|@放款/还款状态|\n" );			
		while( t<=ln_pay_pln_p.ttl_cnt )
		{
			TRACE
			ret = pub_ln_PayPlan( g_ln_mst , g_ln_parm , t , &ln_pay_pln_tmp ,0);	
			if (ret)
			{
				sprintf(acErrMsg,"调用函数pub_ln_PayPlan错误!");
				WRITEMSG
				ERR_DEAL	
			}
			TRACE
			pub_base_dic_show_str(f_pay_ind12,"pay_sts",ln_pay_pln_tmp.pay_ind);
			pub_base_acid_acno( ln_pay_pln_tmp.ac_id,ln_pay_pln_tmp.ac_seqn,ac_no0 );
			pub_base_acid_acno( ln_pay_pln_tmp.de_ac_id,ln_pay_pln_tmp.ac_seqn,ac_no2 );
			if( ln_pay_pln_tmp.curr_cnt<cnt )
			{
				strcpy( ln_pay_pln_tmp.pay_ind , "1" );
				if( f_ln_pay_pln.pay_ind[0]=='0' )
				{
					break;
				} 
			}else{
				if( f_ln_pay_pln.pay_ind[0]=='1' )
				{
					break;
				} 
			}				
			TRACE

			fprintf( fp, "%s|%d|%s|%s|%d|%.2lf|%d|%d|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",ac_no0,ln_pay_pln_tmp.ac_seqn,g_ln_mst.name,ac_no2,ln_pay_pln_tmp.de_ac_seqn,ln_pay_pln_tmp.ttl_amt,ln_pay_pln_tmp.ttl_cnt,ln_pay_pln_tmp.curr_cnt,ln_pay_pln_tmp.curr_amt,ln_pay_pln_tmp.curr_intst,ln_pay_pln_tmp.wrk_date,ln_pay_pln_tmp.beg_date,ln_pay_pln_tmp.end_date,f_pay_ind12);			
			TRACE
			t++;
			sprintf(acErrMsg," t=[%d] ",t );
			WRITEMSG			
		}
		TRACE		
		fclose(fp);
		TRACE
		set_zd_data( DC_FILE_SND,"1" );	
			vtcp_log("44441\n");
											
	}else{	/* 普通贷款 */	
		/**组成查询条件**/
		TRACE
		pub_base_strpack( f_acno0 );
		if( strlen(f_acno0) )
		{
			if ( pub_base_acno_acid(&f_ln_pay_pln.ac_id,f_acno0) )
			{
			goto ErrExit;
			}
			sprintf( tmpstr," ac_id=%ld and",f_ln_pay_pln.ac_id );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_pln.ac_seqn )
		{
			sprintf( tmpstr," ac_seqn=%d and",f_ln_pay_pln.ac_seqn );
			strcat( wherelist,tmpstr );
		}
		/***
		pub_base_strpack( f_acno2 );
		if( strlen(f_acno2) )
		{
			if ( pub_base_acno_acid(&f_ln_pay_pln.de_ac_id,f_acno2) )
			{
			goto ErrExit;
			}
			sprintf( tmpstr," de_ac_id=%ld and",f_ln_pay_pln.de_ac_id );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_pln.de_ac_seqn )
		{
			sprintf( tmpstr," de_ac_seqn=%d and",f_ln_pay_pln.de_ac_seqn );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_pln.ttl_amt )
		{
			sprintf( tmpstr," ttl_amt=%lf and",f_ln_pay_pln.ttl_amt );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_pln.curr_amt )
		{
			sprintf( tmpstr," curr_amt=%lf and",f_ln_pay_pln.curr_amt );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_pln.beg_date )
		{
			sprintf( tmpstr," beg_date>=%ld and",f_ln_pay_pln.beg_date );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_plna.beg_date )
		{
			sprintf( tmpstr," beg_date<=%ld and",f_ln_pay_plna.beg_date );
			strcat( wherelist,tmpstr );
		}
		if( f_ln_pay_pln.end_date )
		{
			sprintf( tmpstr," end_date=%ld and",f_ln_pay_pln.end_date );
			strcat( wherelist,tmpstr );
		}
		****/
		pub_base_strpack( f_ln_pay_pln.pay_ind );
		if( strlen(f_ln_pay_pln.pay_ind) )
		{
			sprintf( tmpstr," pay_ind='%s' and",f_ln_pay_pln.pay_ind );
			strcat( wherelist,tmpstr );
		}

		strcpy( tablelist,"ln_pay_pln" );

		/**组成查询**/
		strcat(wherelist," 1=1") ;

		vtcp_log( "WHERE [%s]",wherelist );
		strcpy( fieldlist,"ac_id,ac_seqn,de_ac_id,de_ac_seqn,ttl_amt,ttl_cnt,curr_cnt,curr_amt,curr_intst,wrk_date,beg_date,end_date,pay_ind");

		ret=Ln_pay_pln_Dec_Sel( g_pub_tx.reply,wherelist );
		ERR_DEAL

		ttlnum=0;

		while(1)
		{
			ret=Ln_pay_pln_Fet_Sel( &vln_pay_pln, g_pub_tx.reply );
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

				/**显示列标题**/
				fprintf( fp,"~账号|账户序号|@户名|放款账号|放款账户序号|$总金额|总期数|当前期次|$本期金额|$当前期次应还利息|登记日期|开始归还日期|到期日期|@放款/还款状态|\n" );
			}

			/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

			pub_base_acid_acno( vln_pay_pln.ac_id,vln_pay_pln.ac_seqn,ac_no0 );
			pub_base_acid_acno( vln_pay_pln.de_ac_id,vln_pay_pln.ac_seqn,ac_no2 );
			if(vln_pay_pln.ttl_amt<-2147000000) vln_pay_pln.ttl_amt=0.0;
			if(vln_pay_pln.curr_amt<-2147000000) vln_pay_pln.curr_amt=0.0;
			if(vln_pay_pln.curr_intst<-2147000000) vln_pay_pln.curr_intst=0.0;
			if(vln_pay_pln.wrk_date<-2147000000) vln_pay_pln.wrk_date=0;
			if(vln_pay_pln.beg_date<-2147000000) vln_pay_pln.beg_date=0;
			if(vln_pay_pln.end_date<-2147000000) vln_pay_pln.end_date=0;
			pub_base_dic_show_str(f_pay_ind12,"pay_sts",vln_pay_pln.pay_ind);

			fprintf( fp, "%s|%d|%s|%s|%d|%.2lf|%d|%d|%.2lf|%.2lf|%ld|%ld|%ld|%s|\n",ac_no0,vln_pay_pln.ac_seqn,g_ln_mst.name,ac_no2,vln_pay_pln.de_ac_seqn,vln_pay_pln.ttl_amt,vln_pay_pln.ttl_cnt,vln_pay_pln.curr_cnt,vln_pay_pln.curr_amt,vln_pay_pln.curr_intst,vln_pay_pln.wrk_date,vln_pay_pln.beg_date,vln_pay_pln.end_date,f_pay_ind12);
			ttlnum++;
		}

		ret=Ln_pay_pln_Clo_Sel( );
		ERR_DEAL

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
