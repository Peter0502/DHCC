/*************************************************
* 文 件 名:  sp6688.c
* 功能描述： 保证金审核信息查询
*
* 作    者:  gongliangliang
* 完成日期： 20110922
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容: 
* 说明：tx_code='6688' sub_tx_code='6688'
        insert into tx_def values ('6688', '保证金利率审批查询', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('6688','0','6688','#$');
        insert into tx_sub_def values ('6688', '保证金利率审批查询', 'sp6688', '0', '0');
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

int sp6688()
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
	get_zd_double("0400",&amt_min );
	get_zd_double("0410",&amt_max );
	get_zd_double("0840",&rate_min );
	get_zd_double("0850",&rate_max );
	get_zd_data("0380",f_ln_auth.pay_ac_no );
	get_zd_data("0030",kinbr1);

	/**组成查询条件**/
	if(flag[0]=='0')
		{
			strcpy(tmpsts,"X");
		}else if(flag[0]=='1')
		{
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
	strcat(wherelist," 1=1 and substr(pact_no,1,1)='C' and substr(pact_no,2,5)='%s' order by sts , pact_no") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"sts,pact_no,name,time_lmt,amt,rate,pay_ac_no");

	ret=Ln_auth_Dec_Sel( g_pub_tx.reply,wherelist ,kinbr1);
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		memset(&vln_auth,0x00,sizeof(vln_auth));
		ret=Ln_auth_Fet_Sel( &vln_auth, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		memset(cBr_no,0x00,sizeof(cBr_no));
		memset(cPact_no,0x00,sizeof(cPact_no));
		memcpy(cPact_no,vln_auth.pact_no,sizeof(vln_auth.pact_no));
		

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
			fprintf( fp,"~@状态|@承兑协议编号|@客户名称|@到期日期|$审批金额|利率(月)|@保证金账户|\n" );
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
			strcpy(f_sts0,"保证金利率已维护");
		}
		
		fprintf( fp, "%s|%s|%s|%8d|%.2lf|%.5lf|%s|\n",f_sts0,cPact_no,vln_auth.name,vln_auth.mtr_date,vln_auth.amt,vln_auth.rate,vln_auth.pay_ac_no);
		ttlnum++;


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
