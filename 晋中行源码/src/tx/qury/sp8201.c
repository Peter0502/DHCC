/*************************************************
* 文 件 名:  sp8201.c
* 功能描述： 贷款账户查询
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
#include "cif_basic_inf_c.h"
#include "mo_merge_ac_c.h"
#include "cassreg_c.h"
int sp8201()
{
	struct ln_mst_c vln_mst;
	struct ln_mst_c f_ln_mst;
	struct ln_mst_c f_ln_msta;
	struct cassreg_c cassreg;
	struct cif_basic_inf_c cif_basic_inf;
	struct mo_merge_ac_c vmo_merge_ac;
	char ac_no0[25];
	char flag_tmp[3];
	char f_five_sts21[41];
	char f_intst_type22[41];
	char f_repay_type21[41];	
	char ac_no24[25];
	char f_ac_sts25[41];
	char f_acno0[25];
	char f_acno9[25];
	char tol_p[10];
	char rate_normal[9];
	char rate_over[9];
	char rate_fine[9];
	char prdt_name[31];
	char cmpd_sts[2];
	char cmpd_sts_str[9];
	char cif_type[2];
	char ln_type[2];
	
	long opn_date_end,mtr_date_end;
	double tol_bal=0.00,tol_ys_bal=0.00,tol_amt_lmt=0.00,tol_ttl_prvd_amt=0.00,tol_in_lo_intst=0.00,tol_out_lo_intst=0.00,tol_cmpd_lo_intst=0.00;
	double	tol_intst_acm=0.00, tol_in_lo_intst_acm=0.00, tol_out_lo_intst_acm=0.00, tol_cmpd_lo_intst_acm=0.00, tol_gage_amt=0.00;
	
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
	memset( &cif_basic_inf,0x00,sizeof( struct cif_basic_inf_c) );
	
	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
	get_zd_long("0510",&f_ln_mst.ac_seqn );
	get_zd_data("0910",f_ln_mst.opn_br_no );
	get_zd_data("0240",f_ln_mst.prdt_no );
	get_zd_double("1001",&f_ln_mst.bal );
	get_zd_double("1002",&f_ln_msta.bal );
	get_zd_long("0290",&f_ln_mst.opn_date );
	get_zd_long("0440",&f_ln_mst.mtr_date );
	get_zd_data("0700",f_ln_mst.intst_type );
	get_zd_data("0370",f_acno9);
	get_zd_long("0540",&f_ln_mst.cif_no );
	get_zd_data("0260",f_ln_mst.name );
	get_zd_data("0630",f_ln_mst.pact_no );

	/*0620存放要查询的担保账号gujy JinZ 0423*/
	get_zd_data("0620",cassreg.db_ac_no );
	
	get_zd_long("0450",&opn_date_end );
	get_zd_long("0460",&mtr_date_end );		
	get_zd_data("0710",f_ln_mst.ac_sts );
	get_zd_data("0720",f_ln_mst.five_sts );
	get_zd_data("0690",cmpd_sts );
	get_zd_data("0670",cif_type );
	sprintf(acErrMsg,"cif_type[%s]",cif_type);
	WRITEMSG
	get_zd_data("0680",ln_type );
	sprintf(acErrMsg,"ln_type[%s]",ln_type);
	WRITEMSG				
	strcpy( flag_tmp , "‰" );
	pub_base_old_acno(f_acno0);
	/**组成查询条件**/	
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_ln_mst.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_mst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_ln_mst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst.opn_br_no );
	if( strlen(f_ln_mst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_ln_mst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst.prdt_no );
	if( strlen(f_ln_mst.prdt_no) )
	{
		sprintf( tmpstr," prdt_no='%s' and",f_ln_mst.prdt_no );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst.bal )
	{
		sprintf( tmpstr," bal>=%lf and",f_ln_mst.bal );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_msta.bal )
	{
		sprintf( tmpstr," bal<=%lf and",f_ln_msta.bal );
		strcat( wherelist,tmpstr );
	}	
	if( f_ln_mst.opn_date )
	{
		sprintf( tmpstr," opn_date>=%ld and opn_date<=%ld and",f_ln_mst.opn_date, opn_date_end );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst.mtr_date )
	{
		sprintf( tmpstr," ((mtr_date>=%ld and mtr_date<=%ld) or (exp_mtr_date>=%ld and exp_mtr_date<=%ld)) and",f_ln_mst.mtr_date, mtr_date_end,f_ln_mst.mtr_date, mtr_date_end );
		strcat( wherelist,tmpstr );
		sprintf(acErrMsg,"日期条件[%s]",tmpstr);
		WRITEMSG		
	}
	
	pub_base_strpack( f_ln_mst.ac_sts );
	if( strlen(f_ln_mst.ac_sts) )
	{
		sprintf( tmpstr," ac_sts='%s' and",f_ln_mst.ac_sts );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst.five_sts );
	if( strlen(f_ln_mst.five_sts) )
	{
		sprintf( tmpstr," five_sts='%s' and",f_ln_mst.five_sts );
		strcat( wherelist,tmpstr );
	}
	
	pub_base_strpack( f_ln_mst.intst_type );
	if( strlen(f_ln_mst.intst_type) )
	{
		sprintf( tmpstr," intst_type='%s' and",f_ln_mst.intst_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_acno9 );
	if( strlen(f_acno9) )
	{
		if ( pub_base_acno_acid(&f_ln_mst.repay_ac_id,f_acno9) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," repay_ac_id=%ld and",f_ln_mst.repay_ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_ln_mst.cif_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst.name );
	if( strlen(f_ln_mst.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_ln_mst.name );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst.pact_no );
	if( strlen(f_ln_mst.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_ln_mst.pact_no );
		strcat( wherelist,tmpstr );
	}
	/*担保账号, gujy JinZ 0423*/
	pub_base_strpack( f_ln_mst.pact_no );
	if( strlen(cassreg.db_ac_no) )
	{
		sprintf( tmpstr," pact_no in (select pact_no from cassreg where db_ac_no ='%s') and",cassreg.db_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( cmpd_sts );
	if( strlen(cmpd_sts) )
	{
		sprintf( tmpstr," cmpd_intst_ind='%s' and", cmpd_sts );
		strcat( wherelist,tmpstr );
	}
	strcpy( tablelist,"ln_mst" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,opn_br_no,prdt_no,bal,ys_bal,hst_bal,amt_lmt,ttl_prvd_amt,rate,over_rate,in_lo_intst,out_lo_intst,cmpd_lo_intst,ic_date,opn_date,mtr_date,lst_date,exp_rate,exp_mtr_date,five_sts,intst_type,repay_type,repay_ac_id,ac_sts,cif_no,name,pact_no");

	ret=Ln_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
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
		/*** del by lance 2003-07-23
		fprintf( fp,"~账号|序号|@开户行|@产品名称|$余额|$昨日余额|$计划发放额|$累计发放额|正常利率|逾期利率|罚息利率|$表内欠息|$表外欠息|$欠复利利息|$抵押品金额|起息日期|开户日期|到期日期|上笔发生日期|展期利率|展期到期日|@五级分类|@计息类型|@还款方式|还款账号|@账户状态|客户号|@户名|@借据号|\n" );
		****************************/
		/***
		fprintf( fp,"~账号|序号|@户名|@开户行|@产品|$余额|$本金积数|$昨日余额|$计划发放额|$累计发放额|正常利率|逾期利率|罚息利率|$表内欠息|$表内积数|$表外欠息|$表外积数|$欠复利利息|$复利积数|$抵押品金额|起息日期|开户日期|到期日期|上笔发生日期|展期利率|展期到期日|@五级分类|@计息类型|@是否计复利|@还款方式|还款账号|@账户状态|客户号|@借据号|\n" );			
		****/
		/*** del by martin 2007323
		fprintf( fp,"~账号|序号|@产品|@开户行|@户名|$余额|开户日期|到期日期|正常利率|$本金积数|$昨日余额|$计划发放额|$累计发放额|逾期利率|罚息利率|$表内欠息|$表内积数|$表外欠息|$表外积数|$欠复利利息|$复利积数|$抵押品金额|起息日期|上笔日期|展期利率|展期到期日|@五级分类|@计息类型|@是否计复利|@还款方式|还款账号|@账户状态|客户号|@借据号|\n" );
		***/
		/*** for martin 20070323
		fprintf( fp,"~账号|@户名|@产品|$余额|$本金积数|正常利率|开户日期|到期日期|@开户行|序号|$昨日余额|$计划发放额|$累计发放额|逾期利率|罚息利率|$表内欠息|$表内积数|$表外欠息|$表外积数|$欠复利利息|$复利积数|$抵押品金额|起息日期|上笔日期|展期利率|展期到期日|@五级分类|@计息类型|@是否计复利|@还款方式|还款账号|@账户状态|客户号|@借据号|\n" );			
		***/
		/*** for martin***/
		fprintf( fp,"~账号|@户名|@产品|$余额|$本金积数|正常利率|开户日期|到期日期|@开户行|\n" );
		
	}
	while(1)
	{
		memset( &vln_mst,0x00,sizeof( struct ln_mst_c) );		
		ret=Ln_mst_Fet_Sel( &vln_mst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"pass break!!" );
			WRITEMSG
			break;
		}
		TRACE
		ERR_DEAL
		
		memset( &cif_basic_inf,0x00,sizeof( struct cif_basic_inf_c) );
		TRACE
		if( vln_mst.cif_no ) 
		{
			sprintf(acErrMsg,"客户号为[%ld]",vln_mst.cif_no);
			WRITEMSG
			ret= Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",vln_mst.cif_no);
			if( ret ) goto ErrExit;
			TRACE
		}
		sprintf(acErrMsg,"主文件客户类型判断[%s]",cif_basic_inf.type);
		WRITEMSG		
		/*客户类型判断*/
		pub_base_strpack( cif_type );	
		if( strlen(cif_type) )
		{
			if( cif_type[0]=='1' )	/*个人*/
			{
				if( !(cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5') )
				{
					continue;
				}
			}
			if( cif_type[0]=='2' )	/*公司*/
			{
				if( cif_basic_inf.type[0]!='2' )
				{
					continue;
				}
			}
			if( cif_type[0]=='3' )	/*机构*/
			{
				if( cif_basic_inf.type[0]!='3' )
				{
					continue;
				}
			}					
			if( cif_type[0]=='4' )	/*同业*/
			{
				if( cif_basic_inf.type[0]!='4' && cif_basic_inf.type[0]!='6' )
				{
					continue;
				}
			}
		}
		TRACE
		memset( &g_ln_parm,0x00,sizeof( struct ln_parm_c) );
		ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , " prdt_no='%s' " , vln_mst.prdt_no );		
		if( ret )
		{
			sprintf(acErrMsg,"读取贷款产品参数信息error");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}
		memset( &cassreg,0x00,sizeof( struct cassreg_c) );
		ret = Cassreg_Sel( g_pub_tx.reply , &cassreg , " pact_no='%s' " , vln_mst.pact_no );		
		if( ret  && 100 != ret)
		{
			sprintf(acErrMsg,"读取贷款担保参数信息error");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}
		sprintf(acErrMsg,"读取贷款产品参数信息prdt_name",g_ln_parm.title);
		WRITEMSG
		strcpy( prdt_name , g_ln_parm.title );
		/*贷款类型判断*/
		pub_base_strpack( ln_type );	
		if( strlen(ln_type) )
		{
			TRACE
			if( strcmp(ln_type , g_ln_parm.ln_type )!=0 )
			{
				TRACE
				continue;
			}	
		}		
		TRACE
		sprintf(acErrMsg," ttlnum is [%ld]  ",ttlnum );
		WRITEMSG

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vln_mst.ac_id,vln_mst.ac_seqn,ac_no0 );
/* rem by LiuHuafeng 不明白为何在这里找新旧账号表
		ret=Mo_merge_ac_Sel(g_pub_tx.reply , &vmo_merge_ac, "ac_no='%s'",ac_no0)
;
        if( ret==100 ) continue;
***************************************************************/
		if(vln_mst.bal<-2147000000) vln_mst.bal=0.0;
		if(vln_mst.ys_bal<-2147000000) vln_mst.ys_bal=0.0;
		if(vln_mst.amt_lmt<-2147000000) vln_mst.amt_lmt=0.0;
		if(vln_mst.ttl_prvd_amt<-2147000000) vln_mst.ttl_prvd_amt=0.0;
		if(vln_mst.rate<-2147000000) vln_mst.rate=0.0;
		if(vln_mst.over_rate<-2147000000) vln_mst.over_rate=0.0;
		if(vln_mst.in_lo_intst<-2147000000) vln_mst.in_lo_intst=0.0;
		if(vln_mst.out_lo_intst<-2147000000) vln_mst.out_lo_intst=0.0;
		if(vln_mst.cmpd_lo_intst<-2147000000) vln_mst.cmpd_lo_intst=0.0;
		if(vln_mst.ic_date<-2147000000) vln_mst.ic_date=0;
		if(vln_mst.opn_date<-2147000000) vln_mst.opn_date=0;
		if(vln_mst.mtr_date<-2147000000) vln_mst.mtr_date=0;
		if(vln_mst.lst_date<-2147000000) vln_mst.lst_date=0;
		if(vln_mst.exp_rate<-2147000000) vln_mst.exp_rate=0.0;
		if(vln_mst.exp_mtr_date<-2147000000) vln_mst.exp_mtr_date=0;
		pub_base_dic_show_str(f_five_sts21,"five_sts",vln_mst.five_sts);
		pub_base_dic_show_str(f_repay_type21,"repay_type",vln_mst.repay_type);				
		pub_base_dic_show_str(f_intst_type22,"ln_intst_type",vln_mst.intst_type);
		pub_base_acid_acno( vln_mst.repay_ac_id,vln_mst.ac_seqn,ac_no24 );
		pub_base_dic_show_str(f_ac_sts25,"ln_ac_sts",vln_mst.ac_sts);
		if(vln_mst.cif_no<-2147000000) vln_mst.cif_no=0;
		sprintf( rate_normal ,"%8.5lf%s" , vln_mst.rate, flag_tmp);
		sprintf( rate_over ,"%8.5lf‰" , vln_mst.over_rate );		
		pub_base_dic_show_str(cmpd_sts_str,"cmpd_sts",vln_mst.cmpd_intst_ind);
		if( pub_base_CompDblVal ( vln_mst.fine_rate , 0.00 )==0 )
		{
			sprintf( rate_fine ,"%8.5lf‰" , vln_mst.over_rate );
		}else{
			sprintf( rate_fine ,"%8.5lf‰" , vln_mst.fine_rate );
		}	
		/*** del by lance 2003-07-23		
		fprintf( fp, "%s|%d|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%ld|%s|%s|\n",ac_no0,vln_mst.ac_seqn,vln_mst.opn_br_no,vln_mst.prdt_no,vln_mst.bal,vln_mst.ys_bal,vln_mst.amt_lmt,vln_mst.ttl_prvd_amt,rate_normal,rate_over,rate_fine,vln_mst.in_lo_intst,vln_mst.out_lo_intst,vln_mst.cmpd_lo_intst,vln_mst.gage_amt,vln_mst.ic_date,vln_mst.opn_date,vln_mst.mtr_date,vln_mst.lst_date,vln_mst.exp_rate,vln_mst.exp_mtr_date,f_five_sts21,f_intst_type22,f_repay_type21,ac_no24,f_ac_sts25,vln_mst.cif_no,vln_mst.name,vln_mst.pact_no);
		****************************/
		/***
		fprintf( fp, "%s|%d|%s|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%s|%ld|%s|\n",ac_no0,vln_mst.ac_seqn,vln_mst.opn_br_no,prdt_name,vln_mst.name,vln_mst.bal,vln_mst.intst_acm,vln_mst.ys_bal,vln_mst.amt_lmt,vln_mst.ttl_prvd_amt,rate_normal,rate_over,rate_fine,vln_mst.in_lo_intst,vln_mst.in_lo_acm,vln_mst.out_lo_intst,vln_mst.out_lo_acm,vln_mst.cmpd_lo_intst,vln_mst.cmpd_lo_acm,vln_mst.gage_amt,vln_mst.ic_date,vln_mst.opn_date,vln_mst.mtr_date,vln_mst.lst_date,vln_mst.exp_rate,vln_mst.exp_mtr_date,f_five_sts21,f_intst_type22,cmpd_sts_str,f_repay_type21,ac_no24,f_ac_sts25,vln_mst.cif_no,vln_mst.pact_no);
		****/
		/*** del by martin 20070323
		fprintf( fp, "%s|%d|%s|%s|%s|%.2lf|%ld|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%s|%ld|%s|\n",ac_no0,vln_mst.ac_seqn,prdt_name,vln_mst.opn_br_no,vln_mst.name,vln_mst.bal,vln_mst.opn_date,vln_mst.mtr_date,rate_normal,vln_mst.intst_acm,vln_mst.ys_bal,vln_mst.amt_lmt,vln_mst.ttl_prvd_amt,rate_over,rate_fine,vln_mst.in_lo_intst,vln_mst.in_lo_acm,vln_mst.out_lo_intst,vln_mst.out_lo_acm,vln_mst.cmpd_lo_intst,vln_mst.cmpd_lo_acm,vln_mst.gage_amt,vln_mst.ic_date,vln_mst.lst_date,vln_mst.exp_rate,vln_mst.exp_mtr_date,f_five_sts21,f_intst_type22,cmpd_sts_str,f_repay_type21,ac_no24,f_ac_sts25,vln_mst.cif_no,vln_mst.pact_no);
		***/
		/*** for martin 20070323
		fprintf( fp, "%s|%s|%s|%s|%.2lf|%ld|%ld|%s|%.2lf|%.2lf      |%d|%.2lf|%.2lf|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%s|%ld|%s|\n",ac_no0,vln_mst.name,prdt_name,vln_mst.bal,vln_mst.intst_acm,rate_normal,vln_mst.opn_date,vln_mst.mtr_date,vln_mst.opn_br_no,vln_mst.ac_seqn,vln_mst.ys_bal,vln_mst.amt_lmt,vln_mst.ttl_prvd_amt,rate_over,rate_fine,vln_mst.in_lo_intst,vln_mst.in_lo_acm,vln_mst.out_lo_intst,vln_mst.out_lo_acm,vln_mst.cmpd_lo_intst,vln_mst.cmpd_lo_acm,vln_mst.gage_amt,vln_mst.ic_date,vln_mst.lst_date,vln_mst.exp_rate,vln_mst.exp_mtr_date,f_five_sts21,f_intst_type22,cmpd_sts_str,f_repay_type21,ac_no24,f_ac_sts25,vln_mst.cif_no,vln_mst.pact_no);
		***/
		fprintf( fp, "%s|%.20s|%.20s|%.2lf|%.2lf|%s|%ld|%ld|%s|%d|%.2lf|%.2lf|%.2lf|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|\n",ac_no0,vln_mst.name,prdt_name,vln_mst.bal,vln_mst.intst_acm,rate_normal,vln_mst.opn_date,vln_mst.mtr_date,vln_mst.opn_br_no,vln_mst.ac_seqn,vln_mst.ys_bal,vln_mst.amt_lmt,vln_mst.ttl_prvd_amt,rate_over,rate_fine,vln_mst.in_lo_intst,vln_mst.in_lo_acm,vln_mst.out_lo_intst,vln_mst.out_lo_acm,vln_mst.cmpd_lo_intst,vln_mst.cmpd_lo_acm,vln_mst.gage_amt,vln_mst.ic_date,vln_mst.lst_date,vln_mst.exp_rate,vln_mst.exp_mtr_date,f_five_sts21,f_intst_type22,cmpd_sts_str,f_repay_type21,ac_no24,f_ac_sts25,vln_mst.cif_no,vln_mst.pact_no,vln_mst.name,prdt_name, cassreg.db_ac_no);
		
		tol_bal+=vln_mst.bal;
		tol_ys_bal+=vln_mst.ys_bal;
		tol_amt_lmt+=vln_mst.amt_lmt;
		tol_ttl_prvd_amt+=vln_mst.ttl_prvd_amt;
		tol_in_lo_intst+=vln_mst.in_lo_intst;
		
		tol_out_lo_intst+=vln_mst.out_lo_intst;
		tol_cmpd_lo_intst+=vln_mst.cmpd_lo_intst;
		
		tol_intst_acm+=vln_mst.intst_acm;
		tol_in_lo_intst_acm+=vln_mst.in_lo_acm;
		tol_out_lo_intst_acm+=vln_mst.out_lo_acm;
		tol_cmpd_lo_intst_acm+=vln_mst.cmpd_lo_acm;
		tol_gage_amt+=vln_mst.gage_amt;
		
		ttlnum++;
	}

	ret=Ln_mst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		sprintf(acErrMsg,"[%lf][%lf][%lf][%lf][%lf][%lf][%lf]",tol_bal,tol_ys_bal,tol_amt_lmt,tol_ttl_prvd_amt,tol_in_lo_intst,tol_out_lo_intst,tol_cmpd_lo_intst);
	  WRITEMSG	
	  
		memset( &vln_mst ,0x00 ,sizeof(struct ln_mst_c) );
		strcpy( tol_p , "总计" );
		/****
		fprintf( fp, "%s|%d|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%s|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%ld|%s|%s|\n",tol_p,ttlnum,vln_mst.opn_br_no,vln_mst.prdt_no,tol_bal,tol_ys_bal,tol_amt_lmt,tol_ttl_prvd_amt,rate_normal,rate_over,rate_fine,tol_in_lo_intst,tol_out_lo_intst,tol_cmpd_lo_intst,vln_mst.ic_date,vln_mst.opn_date,vln_mst.mtr_date,vln_mst.lst_date,vln_mst.exp_rate,vln_mst.exp_mtr_date,vln_mst.opn_br_no,vln_mst.opn_br_no,vln_mst.opn_br_no,vln_mst.opn_br_no,vln_mst.opn_br_no,vln_mst.cif_no,vln_mst.name,vln_mst.pact_no);
		****/
		/****  del by martin 20070323	
		fprintf( fp, "%s|%d|%s|%s|%s|%.2lf|%ld|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%s|%ld|%s|\n",tol_p,ttlnum,"","","",tol_bal,0,0,"",tol_intst_acm,tol_ys_bal,tol_amt_lmt,tol_ttl_prvd_amt,"","",tol_in_lo_intst,tol_in_lo_intst_acm,tol_out_lo_intst,tol_out_lo_intst_acm,tol_cmpd_lo_intst,tol_cmpd_lo_intst_acm,tol_gage_amt,0,0,0.00,0,"","","","","","",0,"");
		****/
		fprintf( fp, "%s|%d项|%s|%.2lf|%.2lf|%s|%ld|%ld|%s|%s|%.2lf|%.2lf|%.2lf|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.5lf|%ld|%s|%s|%s|%s|%s|%s|%ld|%s|\n",tol_p,ttlnum,"",tol_bal,tol_intst_acm,"",0,0,"","",tol_ys_bal,tol_amt_lmt,tol_ttl_prvd_amt,"","",tol_in_lo_intst,tol_in_lo_intst_acm,tol_out_lo_intst,tol_out_lo_intst_acm,tol_cmpd_lo_intst,tol_cmpd_lo_intst_acm,tol_gage_amt,0,0,0.00,0,"","","","","","",0,"");		
	
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
