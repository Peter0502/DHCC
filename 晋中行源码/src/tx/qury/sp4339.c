/*************************************************
* 文 件 名:  sp4339.c
* 功能描述： 银行承兑汇票协议查询
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
#include "mo_bank_acc_po_c.h"
#include "impa_dep_reg_c.h"

int sp4339()
{
	struct mo_bank_acc_po_c vmo_bank_acc_po;
	struct mo_bank_acc_po_c f_mo_bank_acc_po;
	struct mo_bank_acc_po_c f_mo_bank_acc_poa;
	struct	impa_dep_reg_c	vimpa_dep_reg;
	char f_acc_po_sts1[41];
	char f_acc_po_ind2[41];
	char f_acc_type1[41];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char bzact[25];
		char dbact[25];
		char cBr_no[6];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &vmo_bank_acc_po,0,sizeof(struct mo_bank_acc_po_c) );
	memset( &f_mo_bank_acc_po,0,sizeof(struct mo_bank_acc_po_c) );
	memset( &f_mo_bank_acc_poa,0,sizeof(struct mo_bank_acc_po_c) );
	memset( &vimpa_dep_reg,0,sizeof(struct impa_dep_reg_c) );
	memset( cBr_no,0x00,sizeof(cBr_no) );
	pub_base_sysinit();

	get_zd_data("0630",f_mo_bank_acc_po.pact_no );
	get_zd_double("0400",&f_mo_bank_acc_po.sign_amt );
	get_zd_double("0410",&f_mo_bank_acc_poa.sign_amt );
	get_zd_long("0290",&f_mo_bank_acc_po.matr_date );
	get_zd_long("0440",&f_mo_bank_acc_poa.matr_date );
	get_zd_double("0420",&f_mo_bank_acc_po.bail_amt );
	get_zd_double("0390",&f_mo_bank_acc_poa.bail_amt );
	get_zd_double("0430",&f_mo_bank_acc_po.tot_mort_amt );
	get_zd_double("100E",&f_mo_bank_acc_poa.tot_mort_amt );
	get_zd_data("0670",f_mo_bank_acc_po.acc_po_sts );
	get_zd_data("0700",f_mo_bank_acc_po.acc_po_ind );
	get_zd_data("0030",cBr_no);
	
	/**组成查询条件**/
	pub_base_strpack( f_mo_bank_acc_po.pact_no );
	if( strlen(f_mo_bank_acc_po.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_bank_acc_po.pact_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_po.sign_amt )
	{
		sprintf( tmpstr," sign_amt>=%lf and",f_mo_bank_acc_po.sign_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_poa.sign_amt )
	{
		sprintf( tmpstr," sign_amt<=%lf and",f_mo_bank_acc_poa.sign_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_po.matr_date )
	{
		sprintf( tmpstr," matr_date>=%ld and",f_mo_bank_acc_po.matr_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_poa.matr_date )
	{
		sprintf( tmpstr," matr_date<=%ld and",f_mo_bank_acc_poa.matr_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_po.bail_amt )
	{
		sprintf( tmpstr," bail_amt>=%lf and",f_mo_bank_acc_po.bail_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_poa.bail_amt )
	{
		sprintf( tmpstr," bail_amt<=%lf and",f_mo_bank_acc_poa.bail_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_po.tot_mort_amt )
	{
		sprintf( tmpstr," tot_mort_amt>=%lf and",f_mo_bank_acc_po.tot_mort_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_bank_acc_poa.tot_mort_amt )
	{
		sprintf( tmpstr," tot_mort_amt<=%lf and",f_mo_bank_acc_poa.tot_mort_amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_bank_acc_po.acc_po_sts );
	if( strlen(f_mo_bank_acc_po.acc_po_sts) )
	{
		sprintf( tmpstr," acc_po_sts='%s' and",f_mo_bank_acc_po.acc_po_sts );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_bank_acc_po.acc_po_ind );
	if( strlen(f_mo_bank_acc_po.acc_po_ind) )
	{
		sprintf( tmpstr," acc_po_ind='%s' and",f_mo_bank_acc_po.acc_po_ind );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_bank_acc_po" );

	/**组成查询**/
	strcat(wherelist," 1=1 and tx_br_no='%s'") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply,wherelist,cBr_no );
	ERR_DEAL

		ttlnum=0;
TRACE
	while(1)
	{
		ret=Mo_bank_acc_po_Fet_Sel( &vmo_bank_acc_po, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
vtcp_log( "xxxxxx   tot_mort_amt[%lf], pact_num[%s]", vmo_bank_acc_po.tot_mort_amt, vmo_bank_acc_po.pact_num );
		ret = pub_base_CompDblVal( vmo_bank_acc_po.tot_mort_amt, 0.00 );
		if( ret == 1 )
		{
			ret=Impa_dep_reg_Dec_Sel( g_pub_tx.reply, "pact_no='%s'",\
						vmo_bank_acc_po.pact_num );
			ERR_DEAL
TRACE
			while(1)
			{
				ret=Impa_dep_reg_Fet_Sel( &vimpa_dep_reg, g_pub_tx.reply );
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
					fprintf( fp,"~@承兑汇票协议编号|@承兑类型|@承兑协议状态|@承兑协议到期处理标志|@付款人全称|@付款人账号|@付款人开户行行号|@付款人开户行行名|@收款人全称|@收款人账号|@收款人开户行行名|$签发票面金额|到期日期|@担保帐号|@保证金账号|$保证金金额|$保证金比例|@质押编号|$抵质押总金额|$银行垫款额|$罚息利率|@逾期贷款账号|\n" );
				}
	
				pub_base_dic_show_str(f_acc_po_sts1,"accpo_sts",vmo_bank_acc_po.acc_po_sts);
				pub_base_dic_show_str(f_acc_po_ind2,"accpo_ind",vmo_bank_acc_po.acc_po_ind);
				pub_base_dic_show_str(f_acc_type1,"acc_type",vmo_bank_acc_po.acc_type);
				if(vimpa_dep_reg.mort_amt<-2147000000) vimpa_dep_reg.mort_amt=0.0;
				if(vmo_bank_acc_po.sign_amt<-2147000000) vmo_bank_acc_po.sign_amt=0.0;
				if(vmo_bank_acc_po.matr_date<-2147000000) vmo_bank_acc_po.matr_date=0;
				if(vmo_bank_acc_po.bail_amt<-2147000000) vmo_bank_acc_po.bail_amt=0.0;
				if(vmo_bank_acc_po.bail_amt_ratio<-2147000000) vmo_bank_acc_po.bail_amt_ratio=0.0;
				if(vmo_bank_acc_po.tot_mort_amt<-2147000000) vmo_bank_acc_po.tot_mort_amt=0.0;
				if(vmo_bank_acc_po.advance_amt<-2147000000) vmo_bank_acc_po.advance_amt=0.0;
				if(vmo_bank_acc_po.over_rate<-2147000000) vmo_bank_acc_po.over_rate=0.0;

				memset(bzact,0,sizeof(bzact));
				memset(dbact,0,sizeof(dbact));
				if(vmo_bank_acc_po.acc_type[0] == '4')	/** 担保承兑 **/
				{
					memcpy(dbact,vmo_bank_acc_po.bail_ac_no,sizeof(dbact)-1);
				}else{
					memcpy(bzact,vmo_bank_acc_po.bail_ac_no,sizeof(bzact)-1);
				}
	
				fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%.2lf|%ld|%s|%s|%.2lf|%.2lf|%s|%.2lf|%.2lf|%.2lf|%s|\n",vmo_bank_acc_po.pact_no,f_acc_type1,f_acc_po_sts1,f_acc_po_ind2,vmo_bank_acc_po.payer_name,vmo_bank_acc_po.payer_ac_no,vmo_bank_acc_po.payer_br_no,vmo_bank_acc_po.payer_br_name,vmo_bank_acc_po.payee_name,vmo_bank_acc_po.payee_ac_no,vmo_bank_acc_po.payee_br_name,vmo_bank_acc_po.sign_amt,vmo_bank_acc_po.matr_date,dbact,bzact,vmo_bank_acc_po.bail_amt,vmo_bank_acc_po.bail_amt_ratio,vmo_bank_acc_po.pact_num,vmo_bank_acc_po.tot_mort_amt,vmo_bank_acc_po.advance_amt,vmo_bank_acc_po.over_rate,vmo_bank_acc_po.over_ac_no);
				ttlnum++;
			}
		ret=Impa_dep_reg_Clo_Sel( );
		ERR_DEAL
		}
		else if( ret == -1 )
		{
			sprintf( acErrMsg, "质押存单金额为负，请查询数据库!" );
			WRITEMSG
			goto ErrExit;
		}
		else
		{
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
					fprintf( fp,"~@承兑汇票协议编号|@承兑类型|@承兑协议状态|@承兑协议到期处理标志|@付款人全称|@付款人账号|@付款人开户行行号|@付款人开户行行名|@收款人全称|@收款人账号|@收款人开户行行名|$签发票面金额|到期日期|@担保帐号|@保证金账号|$保证金金额|$保证金比例|$抵质押总金额|@质押账号|$质押存单余额|$银行垫款额|$罚息利率|@逾期贷款账号|\n" );
				}
	
				pub_base_dic_show_str(f_acc_po_sts1,"accpo_sts",vmo_bank_acc_po.acc_po_sts);
				pub_base_dic_show_str(f_acc_po_ind2,"accpo_ind",vmo_bank_acc_po.acc_po_ind);
				pub_base_dic_show_str(f_acc_type1,"acc_type",vmo_bank_acc_po.acc_type);
				if(vimpa_dep_reg.mort_amt<-2147000000) vimpa_dep_reg.mort_amt=0.0;
				if(vmo_bank_acc_po.sign_amt<-2147000000) vmo_bank_acc_po.sign_amt=0.0;
				if(vmo_bank_acc_po.matr_date<-2147000000) vmo_bank_acc_po.matr_date=0;
				if(vmo_bank_acc_po.bail_amt<-2147000000) vmo_bank_acc_po.bail_amt=0.0;
				if(vmo_bank_acc_po.bail_amt_ratio<-2147000000) vmo_bank_acc_po.bail_amt_ratio=0.0;
				if(vmo_bank_acc_po.tot_mort_amt<-2147000000) vmo_bank_acc_po.tot_mort_amt=0.0;
				if(vmo_bank_acc_po.advance_amt<-2147000000) vmo_bank_acc_po.advance_amt=0.0;
				if(vmo_bank_acc_po.over_rate<-2147000000) vmo_bank_acc_po.over_rate=0.0;
				memset(bzact,0,sizeof(bzact));
				memset(dbact,0,sizeof(dbact));
				if(vmo_bank_acc_po.acc_type[0] == '4')	/** 担保承兑 **/
				{
					memcpy(dbact,vmo_bank_acc_po.bail_ac_no,sizeof(dbact)-1);
				}else{
					memcpy(bzact,vmo_bank_acc_po.bail_ac_no,sizeof(bzact)-1);
				}
	
				fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%.2lf|%ld|%s|%s|%.2lf|%.2lf|%.2lf|%s|%.2lf|%.2lf|%.2lf|%s|\n",vmo_bank_acc_po.pact_no,f_acc_type1,f_acc_po_sts1,f_acc_po_ind2,vmo_bank_acc_po.payer_name,vmo_bank_acc_po.payer_ac_no,vmo_bank_acc_po.payer_br_no,vmo_bank_acc_po.payer_br_name,vmo_bank_acc_po.payee_name,vmo_bank_acc_po.payee_ac_no,vmo_bank_acc_po.payee_br_name,vmo_bank_acc_po.sign_amt,vmo_bank_acc_po.matr_date,dbact,bzact,vmo_bank_acc_po.bail_amt,vmo_bank_acc_po.bail_amt_ratio,vmo_bank_acc_po.tot_mort_amt,vimpa_dep_reg.mort_ac_no,vimpa_dep_reg.mort_amt,vmo_bank_acc_po.advance_amt,vmo_bank_acc_po.over_rate,vmo_bank_acc_po.over_ac_no);
				ttlnum++;
		
		}
	}
	ret=Mo_bank_acc_po_Clo_Sel( );
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
