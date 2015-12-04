/*************************************************
* 文 件 名:  sp1901.c
* 功能描述： 客户交易量查询
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
#include "cif_tranq_stat_c.h"
#include "cif_basic_inf_c.h"

int sp1901()
{
	struct cif_tranq_stat_c vcif_tranq_stat;
	struct cif_tranq_stat_c f_cif_tranq_stat;
	struct cif_basic_inf_c g_cif_basic_inf;
		char f_cur_name[41];
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

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_long("0540",&f_cif_tranq_stat.cif_no );

	/**组成查询条件**/
	if( f_cif_tranq_stat.cif_no )
	{
		ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
							"cif_no=%ld",f_cif_tranq_stat.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}

		sprintf( tmpstr," cif_no=%ld and",f_cif_tranq_stat.cif_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_tranq_stat" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,cur_no,t_cash_cnt,t_cash_amt,t_turn_cnt,t_turn_amt,x_cash_cnt,x_cash_amt,x_turn_cnt,x_turn_amt,m_cash_cnt,m_cash_amt,m_turn_cnt,m_turn_amt,q_cash_cnt,q_cash_amt,q_turn_cnt,q_turn_amt,hy_cash_cnt,hy_cash_amt,hy_turn_cnt,hy_turn_amt,y_cash_cnt,y_cash_amt,y_turn_cnt,y_turn_amt");

	ret=Cif_tranq_stat_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_tranq_stat_Fet_Sel( &vcif_tranq_stat, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@户名|@币种|本日现金交易笔数|$本日现金交易金额|本日转账笔数|$本日转帐金额|本旬现金交易笔数|$本旬现金交易金额|本旬转账笔数|$本旬转帐金额|本月现金交易笔数|$本月现金交易金额|本月转账笔数|$本月转帐金额|本季现金交易笔数|$本季现金交易金额|本季转账笔数|$本季转帐金额|本半年现金交易笔数|$半年现金交易金额|本半年转账笔数|$本半年转账金额|本年现金交易笔数|$本年现金交易金额|本年转账笔数|$本年转账金额|\n" );
		}

		ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
							  "cif_no=%ld",vcif_tranq_stat.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配记录!!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"O043" );
			break;
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_tranq_stat.cif_no<-2147000000) vcif_tranq_stat.cif_no=0;
		if(vcif_tranq_stat.t_cash_cnt<-2147000000) vcif_tranq_stat.t_cash_cnt=0;
		if(vcif_tranq_stat.t_cash_amt<-2147000000) vcif_tranq_stat.t_cash_amt=0.0;
		if(vcif_tranq_stat.t_turn_cnt<-2147000000) vcif_tranq_stat.t_turn_cnt=0;
		if(vcif_tranq_stat.t_turn_amt<-2147000000) vcif_tranq_stat.t_turn_amt=0.0;
		if(vcif_tranq_stat.x_cash_cnt<-2147000000) vcif_tranq_stat.x_cash_cnt=0;
		if(vcif_tranq_stat.x_cash_amt<-2147000000) vcif_tranq_stat.x_cash_amt=0.0;
		if(vcif_tranq_stat.x_turn_cnt<-2147000000) vcif_tranq_stat.x_turn_cnt=0;
		if(vcif_tranq_stat.x_turn_amt<-2147000000) vcif_tranq_stat.x_turn_amt=0.0;
		if(vcif_tranq_stat.m_cash_cnt<-2147000000) vcif_tranq_stat.m_cash_cnt=0;
		if(vcif_tranq_stat.m_cash_amt<-2147000000) vcif_tranq_stat.m_cash_amt=0.0;
		if(vcif_tranq_stat.m_turn_cnt<-2147000000) vcif_tranq_stat.m_turn_cnt=0;
		if(vcif_tranq_stat.m_turn_amt<-2147000000) vcif_tranq_stat.m_turn_amt=0.0;
		if(vcif_tranq_stat.q_cash_cnt<-2147000000) vcif_tranq_stat.q_cash_cnt=0;
		if(vcif_tranq_stat.q_cash_amt<-2147000000) vcif_tranq_stat.q_cash_amt=0.0;
		if(vcif_tranq_stat.q_turn_cnt<-2147000000) vcif_tranq_stat.q_turn_cnt=0;
		if(vcif_tranq_stat.q_turn_amt<-2147000000) vcif_tranq_stat.q_turn_amt=0.0;
		if(vcif_tranq_stat.hy_cash_cnt<-2147000000) vcif_tranq_stat.hy_cash_cnt=0;
		if(vcif_tranq_stat.hy_cash_amt<-2147000000) vcif_tranq_stat.hy_cash_amt=0.0;
		if(vcif_tranq_stat.hy_turn_cnt<-2147000000) vcif_tranq_stat.hy_turn_cnt=0;
		if(vcif_tranq_stat.hy_turn_amt<-2147000000) vcif_tranq_stat.hy_turn_amt=0.0;
		if(vcif_tranq_stat.y_cash_cnt<-2147000000) vcif_tranq_stat.y_cash_cnt=0;
		if(vcif_tranq_stat.y_cash_amt<-2147000000) vcif_tranq_stat.y_cash_amt=0.0;
		if(vcif_tranq_stat.y_turn_cnt<-2147000000) vcif_tranq_stat.y_turn_cnt=0;
		if(vcif_tranq_stat.y_turn_amt<-2147000000) vcif_tranq_stat.y_turn_amt=0.0;
		pub_base_dic_show_str(f_cur_name,"cur_name",vcif_tranq_stat.cur_no);

		fprintf( fp, "%ld|%s|%s|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|\n",
		vcif_tranq_stat.cif_no,g_cif_basic_inf.name,
		f_cur_name,vcif_tranq_stat.t_cash_cnt,
		vcif_tranq_stat.t_cash_amt,vcif_tranq_stat.t_turn_cnt,
		vcif_tranq_stat.t_turn_amt,vcif_tranq_stat.x_cash_cnt,
		vcif_tranq_stat.x_cash_amt,vcif_tranq_stat.x_turn_cnt,vcif_tranq_stat.x_turn_amt,vcif_tranq_stat.m_cash_cnt,vcif_tranq_stat.m_cash_amt,vcif_tranq_stat.m_turn_cnt,vcif_tranq_stat.m_turn_amt,vcif_tranq_stat.q_cash_cnt,vcif_tranq_stat.q_cash_amt,vcif_tranq_stat.q_turn_cnt,vcif_tranq_stat.q_turn_amt,vcif_tranq_stat.hy_cash_cnt,vcif_tranq_stat.hy_cash_amt,vcif_tranq_stat.hy_turn_cnt,vcif_tranq_stat.hy_turn_amt,vcif_tranq_stat.y_cash_cnt,vcif_tranq_stat.y_cash_amt,vcif_tranq_stat.y_turn_cnt,vcif_tranq_stat.y_turn_amt);
		ttlnum++;
	}

	ret=Cif_tranq_stat_Clo_Sel( );
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
