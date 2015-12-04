/*************************************************
* 文 件 名:  sp8208.c
* 功能描述： 贷款审核信息查询
*
* 作    者:  lance
* 完成日期： 2003-07-01
*
* 修改记录：
* 日   期:20110903
* 修 改 人:gongliangliang
* 修改内容:配合信贷系统对贷款审批进行查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"
#include "dd_mst_c.h"

int sp8208()
{
	struct ln_auth_c vln_auth;
	struct ln_auth_c f_ln_auth;
	struct dd_mst_c f_dd_mst;
	char f_sts0[41];
	char kinbr1[BRNO_LEN+1];
	char filename[100];
	char wherelist[1024];  /**查询条件**/  
	char cBr_no[6];
	char tmpstr[512];
	char tmpmo[512];
	char fieldlist[1024];  /**要查询的字段列表**/
	char tablelist[128];   /**要查询的表名**/
	char titstr[1024];
	char tmpsts[2];
	char flag[2]; 
	char cPact_no[21];
	int ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	int time_lmt_min=0,time_lmt_max=0;
	double amt_min=0.00,amt_max=0.00;
	double rate_min=0.00,rate_max=0.00;
	FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(kinbr1,0,sizeof(kinbr1));
	pub_base_sysinit();
	get_zd_data("0670",flag );
	get_zd_data("0630",f_ln_auth.pact_no );
	get_zd_data("0260",f_ln_auth.name );
	get_zd_int("0340",&time_lmt_min );
	get_zd_int("0350",&time_lmt_max );
	get_zd_double("0400",&amt_min );
	get_zd_double("0410",&amt_max );
	get_zd_double("0840",&rate_min );
	get_zd_double("0850",&rate_max );
	get_zd_data("0380",f_ln_auth.pay_ac_no );
	get_zd_data("0370",f_ln_auth.save_ac_no );
	get_zd_data("0030",kinbr1);

	/**组成查询条件**/
	if(flag[0]=='0')
		{
			strcpy(tmpsts,"X");
		}else if(flag[0]=='1'){
			strcpy(tmpsts,"1");
		}
		
	pub_base_strpack( tmpsts );
	if( strlen(tmpsts) )
	{
		sprintf( tmpstr," sts='%s' and",tmpsts );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_auth.pact_no );
	if( strlen(f_ln_auth.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_ln_auth.pact_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_auth.name );
	if( strlen(f_ln_auth.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_ln_auth.name );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_auth.pay_ac_no );
	if( strlen(f_ln_auth.pay_ac_no) )
	{
		sprintf( tmpstr," pay_ac_no='%s' and",f_ln_auth.pay_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_auth.save_ac_no );
	if( strlen(f_ln_auth.save_ac_no) )
	{
		sprintf( tmpstr," save_ac_no='%s' and",f_ln_auth.save_ac_no );
		strcat( wherelist,tmpstr );
	}
	if( time_lmt_min )
	{
		sprintf( tmpstr," time_lmt>=%d and",time_lmt_min );
		strcat( wherelist,tmpstr );
	}
	if( time_lmt_max )
	{
		sprintf( tmpstr," time_lmt<=%d and",time_lmt_max );
		strcat( wherelist,tmpstr );
	}
	if( amt_min )
	{
		sprintf( tmpstr," amt>=%lf and",amt_min );
		strcat( wherelist,tmpstr );
	}
	if( amt_max )
	{
		sprintf( tmpstr," amt<=%lf and",amt_max );
		strcat( wherelist,tmpstr );
	}
	if( rate_min )
	{
		sprintf( tmpstr," rate>=%lf and",rate_min );
		strcat( wherelist,tmpstr );
	}
	if( rate_max )
	{
		sprintf( tmpstr," rate<=%lf and",rate_max );
		strcat( wherelist,tmpstr );
	}
	

	strcpy( tablelist,"ln_auth" );

	/**组成查询**/
	strcat(wherelist," 1=1 and cif_no>0  order by sts , pact_no") ;
	/**strcat(wherelist," 1=1 and cif_no>0 and substr(pact_no,2,5)='%s' order by sts , pact_no") ;jkjk20111206**/

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"sts,pact_no,name,time_lmt,amt,rate,pay_ac_no,save_ac_no");

	ret=Ln_auth_Dec_Sel( g_pub_tx.reply,wherelist,kinbr1);
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		memset(&vln_auth,0x00,sizeof(vln_auth));
		ret=Ln_auth_Fet_Sel( &vln_auth, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		memset(cBr_no,0x00,sizeof(cBr_no));
		memcpy(cBr_no,vln_auth.pact_no+1,5);
		
		memset(cPact_no,0x00,sizeof(cPact_no));
		memcpy(cPact_no,vln_auth.pact_no,sizeof(vln_auth.pact_no));
		
		ret=Dd_mst_Sel(g_pub_tx.reply,&f_dd_mst," cif_no=%ld",vln_auth.cif_no);
		if(ret)
		{
			sprintf(acErrMsg," 查询dd_mst [%d] error ",ret );
			WRITEMSG
			strcpy( g_pub_tx.reply,"H034" );
			goto ErrExit;
		}else if (!ret){
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
				fprintf( fp,"~@状态|@借据号|@客户号|@客户名称|期限(月)|@还款日期|$贷款金额|利率(月)|@还款账户|@存款账户|\n" );
			}
 	
			/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
 	
			pub_base_dic_show_str(f_sts0,"ln_auth_sts",vln_auth.sts);
			if(vln_auth.amt<-2147000000) vln_auth.amt=0.0;
			if(vln_auth.rate<-2147000000) vln_auth.rate=0.0;
			pub_base_dic_show_str(f_sts0,"ln_auth_sts",vln_auth.sts);	
			
			vtcp_log("[%s][%d] vln_auth.sts===[%s]\n",__FILE__,__LINE__,vln_auth.sts);
			if(vln_auth.sts[0]=='X')
			{
				memset(f_sts0,0x00,sizeof(f_sts0));
				strcpy(f_sts0,"信贷已审批");
			}else if(vln_auth.sts[0]=='1'){
				memset(f_sts0,0x00,sizeof(f_sts0));
				strcpy(f_sts0,"贷款已开户");
			}
			
			fprintf( fp, "%s|%s|%8d|%s|%4d|%8d|%.2lf|%.5lf|%s|%s|\n",f_sts0,cPact_no,vln_auth.cif_no,vln_auth.name,vln_auth.time_lmt,vln_auth.mtr_date,vln_auth.amt,vln_auth.rate,vln_auth.pay_ac_no,vln_auth.save_ac_no);
			ttlnum++;
		}
		else
		continue;

	}

	ret=Ln_auth_Clo_Sel( );
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
