/*************************************************
* 文 件 名:  sp8103.c
* 功能描述： 
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:2004.12.12
* 修 改 人:
* 修改内容:非本网点或本网点下属网点帐户信息不能查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int sp8103()
{
	struct td_mst_c vtd_mst;
	struct td_mst_c f_td_mst;
	struct td_mst_c f_td_msta;
	struct com_branch_c g_com_branch;
	char ac_no0[25];
	char f_intst_type10[41];
	char f_odtt_ind19[41];
	char f_ac_sts20[41];
	char f_hold_sts21[41];
	char f_tfr_ind24[41];
	char f_acno0[25];
	char prdt_name[100];
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
	char note_sts[10];
	int f_acno=0;
	int f_name=0;
	int f_cif=0;
	int f_amt=0;
	int f_brno=0;
	int f_prdt=0;
	int f_opn=0;
  int cnt,tmp_cnt;
  	char cTypename[21];
	memset(cTypename,0x0,sizeof(cTypename));

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
    pub_base_old_acno( f_acno0 );
	get_zd_long("0510",&f_td_mst.ac_seqn );
	get_zd_long("0540",&f_td_mst.cif_no );
	get_zd_data("0700",f_td_mst.ac_sts );
	get_zd_data("0260",f_td_mst.name );
	get_zd_data("0910",f_td_mst.opn_br_no );
	get_zd_data("0230",f_td_mst.prdt_no );
	get_zd_double("1001",&f_td_mst.bal );
	get_zd_double("1002",&f_td_msta.bal );
	get_zd_long("0290",&f_td_mst.opn_date );
	get_zd_long("0440",&f_td_mst.mtr_date );
	get_zd_double("1004",&f_td_mst.td_amt );

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	pub_base_strpack( f_td_mst.name );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_td_mst.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_td_mst.ac_id );
		strcat( wherelist,tmpstr );
		f_acno=1;
	}
	/***XXXX
	else if( !strlen(f_td_mst.name) )
	{
		sprintf( tmpstr," opn_br_no='%s' and ",g_pub_tx.tx_br_no );
		strcat( wherelist,tmpstr );
	}
	*******/

	if( f_td_mst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_td_mst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	if( f_td_mst.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_td_mst.cif_no );
		strcat( wherelist,tmpstr );
		f_cif=1;
	}
	pub_base_strpack( f_td_mst.name );
	if( strlen(f_td_mst.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_td_mst.name );
		strcat( wherelist,tmpstr );
		f_name=1;
	}
	pub_base_strpack( f_td_mst.opn_br_no );
	if( strlen(f_td_mst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_td_mst.opn_br_no );
		strcat( wherelist,tmpstr );
		f_brno=1;
	}
	pub_base_strpack( f_td_mst.prdt_no );
	if( strlen(f_td_mst.prdt_no) )
	{
		sprintf( tmpstr," prdt_no='%s' and",f_td_mst.prdt_no );
		strcat( wherelist,tmpstr );
		f_prdt=1;
	}
	pub_base_strpack( f_td_mst.ac_sts );
	if( strlen(f_td_mst.ac_sts))
	{
		sprintf( tmpstr," ac_sts='%s' and",f_td_mst.ac_sts );
		strcat( wherelist,tmpstr );
	}
	if( f_td_mst.bal )
	{
		sprintf( tmpstr," bal>=%lf and",f_td_mst.bal );
		strcat( wherelist,tmpstr );
		f_amt++;
	}
	if( f_td_msta.bal )
	{
		sprintf( tmpstr," bal<=%lf and",f_td_msta.bal );
		strcat( wherelist,tmpstr );
		f_amt++;
	}
	if( f_td_mst.opn_date )
	{
		sprintf( tmpstr," opn_date>=%ld and",f_td_mst.opn_date );
		strcat( wherelist,tmpstr );
		f_opn++;
	}
	if( f_td_mst.mtr_date )
	{
		sprintf( tmpstr," mtr_date<=%ld and",f_td_mst.mtr_date );
		strcat( wherelist,tmpstr );
		f_opn++;
	}
	if( f_td_mst.td_amt )
	{
		sprintf( tmpstr," td_amt=%lf and",f_td_mst.td_amt );
		strcat( wherelist,tmpstr );
	}
       
	/*
	if( f_acno )
	{
		TRACE
	}
	else if( f_name )
	{
		TRACE
	}
	else if( f_cif )
	{
		TRACE
	}
	else if( f_brno && f_opn )
	{
		TRACE
	}
	else if( f_brno && f_amt )
	{
		TRACE
	}
	else if( f_brno && f_prdt )
	{
		TRACE
	}
	else if( f_prdt && f_opn )
	{
		TRACE
	}
	else if( f_prdt && f_amt )
	{
		TRACE
	}
	else
	{
		sprintf(acErrMsg,"请输入更多的查询条件");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply,"BBCL" );
		goto ErrExit;
	}
   */    
	strcpy( tablelist,"td_mst" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY opn_date,ac_id,ac_seqn ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,cif_no,name,opn_br_no,prdt_no,bal,ys_bal,hst_bal,rate,intst_type,opn_date,ic_date,mtr_date,lst_date,td_amt,ttl_cnt,tx_cnt,hst_cnt,odtt_ind,ac_sts,hold_sts,hold_amt,ctl_amt,tfr_ind");

	ret=Td_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Td_mst_Fet_Sel( &vtd_mst, g_pub_tx.reply );
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

			/**显示列标题**
			fprintf( fp,"~账号|序号|@户      名|@产品名称|@开户行|$清算余额|$昨日余额|$明细余额|利率|@计息类型|开户日期|起息日期|到期日期|上笔日期|$特殊定期交易金额|总次数|已发生次数|明细笔数|@钞汇|@帐户状态|@冻结状态|@介质状态|$冻结金额|$控制金额|@预约转存|客户号|\n" );
			***/
			/**显示列标题**
			fprintf( fp,"~账号|序号|@户名|@产品名称|@开户行|$账户余额|$昨日余额|利率|@计息类型|开户日期|起息日期|到期日期|上笔日期|$特殊定期交易金额|总次数|已发生次数|@帐户状态|@冻结|@介质状态|$冻结金额|$控制金额|@预约转存|客户号|@凭证号|@证件类型|@证件号码|\n" );
		  ***/
		  /** fprintf( fp,"~账号|@户名|@产品名称|@开户行|$账户余额|开户日期|@帐户状态|序号|利率|@计息类型|$昨日余额|起息日期|到期日期|上笔日期|$特殊定期交易金额|总次数|已发生次数|@冻结|@介质状态|$冻结金额|$控制金额|@预约转存|客户号|@凭证号|@证件类型|@证件号码|\n" );
		  **/
		  fprintf( fp,"~账号|@户名|@产品名称|@开户行|$账户余额|开户日期|@帐户状态|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vtd_mst.ac_id,vtd_mst.ac_seqn,ac_no0 );
		if(vtd_mst.cif_no<-2147000000) vtd_mst.cif_no=0;
		if(vtd_mst.bal<-2147000000) vtd_mst.bal=0.0;
		if(vtd_mst.ys_bal<-2147000000) vtd_mst.ys_bal=0.0;
		if(vtd_mst.hst_bal<-2147000000) vtd_mst.hst_bal=0.0;
		if(vtd_mst.rate<-2147000000) vtd_mst.rate=0.0;
		pub_base_dic_show_str(f_intst_type10,"intst_knd",vtd_mst.intst_type);
		if(vtd_mst.opn_date<-2147000000) vtd_mst.opn_date=0;
		if(vtd_mst.ic_date<-2147000000) vtd_mst.ic_date=0;
		if(vtd_mst.mtr_date<-2147000000) vtd_mst.mtr_date=0;
		if(vtd_mst.lst_date<-2147000000) vtd_mst.lst_date=0;
		if(vtd_mst.td_amt<-2147000000) vtd_mst.td_amt=0.0;
		if(vtd_mst.hst_cnt<-2147000000) vtd_mst.hst_cnt=0;
		pub_base_dic_show_str(f_odtt_ind19,"odtt_ind",vtd_mst.odtt_ind);
		pub_base_dic_show_str(f_ac_sts20,"td_ac_sts",vtd_mst.ac_sts);
		pub_base_dic_show_str(f_hold_sts21,"hold_sts",vtd_mst.hold_sts);
		if(vtd_mst.hold_amt<-2147000000) vtd_mst.hold_amt=0.0;
		if(vtd_mst.ctl_amt<-2147000000) vtd_mst.ctl_amt=0.0;
		pub_base_dic_show_str(f_tfr_ind24,"yes_no",vtd_mst.tfr_ind);
		ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",vtd_mst.prdt_no);
		strcpy(prdt_name,g_td_parm.title);

		/**-----------hao--------冻结状态----------**/
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no0);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"取介质帐号对照表错![%d][%s]",g_reply_int,ac_no0);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L026");
		}
		pub_base_strpack(g_mdm_ac_rel.note_no);
		memset(cTypename,0x0,sizeof(cTypename));
		pub_base_dic_show_str(cTypename,"id_type",g_mdm_ac_rel.id_type);

		pub_base_dic_show_str(note_sts,"mdm_sts",g_mdm_ac_rel.note_sts);
		fprintf( fp, "%s|%.20s|%s|%s|%.2lf|%ld|%s|%d|%.5lf|%s|%.2lf|%ld|%ld|%ld|%.2lf|%d|%d|%s|%s|%.2lf|%.2lf|%s|%ld|%16s|%s|%s|%s|\n",
			       ac_no0,vtd_mst.name,prdt_name,
			       vtd_mst.opn_br_no,vtd_mst.bal,vtd_mst.opn_date,
			       f_ac_sts20,vtd_mst.ac_seqn,
			       vtd_mst.rate,f_intst_type10,vtd_mst.ys_bal,
			       vtd_mst.ic_date,vtd_mst.mtr_date,vtd_mst.lst_date,
			       vtd_mst.td_amt,vtd_mst.ttl_cnt,vtd_mst.tx_cnt,
			       f_hold_sts21,note_sts,vtd_mst.hold_amt,vtd_mst.ctl_amt,
			       f_tfr_ind24,vtd_mst.cif_no,g_mdm_ac_rel.note_no,cTypename,g_mdm_ac_rel.id_no,vtd_mst.name);
		/*-----------------------------------update by guohua begin-------------------------------*/
/*		cnt=tmp_cnt=0;
    if((!strcmp(g_pub_tx.tx_br_no,KJ_BR_NO))||(!strcmp(g_pub_tx.tx_br_no,"99999"))||(!strcmp(g_pub_tx.tx_br_no,"61099"))||(!strcmp(g_pub_tx.tx_br_no,"61056"))||(!strcmp(g_pub_tx.tx_br_no,"61058"))||(!strcmp(g_mdm_ac_rel.mdm_code,"0016")))
		 fprintf( fp, "%s|%d|%s|%s|%s|%.2lf|%.2lf|%.5lf|%s|%ld|%ld|%ld|%ld|%.2lf|%d|%d|%s|%s|%s|%.2lf|%.2lf|%s|%ld|\n",
			       ac_no0,vtd_mst.ac_seqn,vtd_mst.name,prdt_name,
			       vtd_mst.opn_br_no,vtd_mst.bal,vtd_mst.ys_bal,
			       vtd_mst.rate,f_intst_type10,vtd_mst.opn_date,
			       vtd_mst.ic_date,vtd_mst.mtr_date,vtd_mst.lst_date,
			       vtd_mst.td_amt,vtd_mst.ttl_cnt,vtd_mst.tx_cnt,
			       f_ac_sts20,
			       f_hold_sts21,note_sts,vtd_mst.hold_amt,vtd_mst.ctl_amt,
			       f_tfr_ind24,vtd_mst.cif_no);
		else 
			{
			 ret=Com_branch_Dec_Sel(g_pub_tx.reply ,"up_br_no='%s'",g_pub_tx.tx_br_no);
       if(ret)
       {
        sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
       }
       
       while(1)
       {
        memset( &g_com_branch,0x00,sizeof(struct com_branch_c) );
        ret=Com_branch_Fet_Sel(&g_com_branch,g_pub_tx.reply);
        if (ret==100)
           break;
        else ERR_DEAL
        cnt++;
       
        if(strcmp(g_com_branch.br_no,vtd_mst.opn_br_no))
        tmp_cnt++;
			 }
			 
			 if((cnt!=tmp_cnt)||(!strcmp(g_pub_tx.tx_br_no,vtd_mst.opn_br_no)))
			 	fprintf( fp, "%s|%d|%s|%s|%s|%.2lf|%.2lf|%.5lf|%s|%ld|%ld|%ld|%ld|%.2lf|%d|%d|%s|%s|%s|%.2lf|%.2lf|%s|%ld|\n",
			       ac_no0,vtd_mst.ac_seqn,vtd_mst.name,prdt_name,
			       vtd_mst.opn_br_no,vtd_mst.bal,vtd_mst.ys_bal,
			       vtd_mst.rate,f_intst_type10,vtd_mst.opn_date,
			       vtd_mst.ic_date,vtd_mst.mtr_date,vtd_mst.lst_date,
			       vtd_mst.td_amt,vtd_mst.ttl_cnt,vtd_mst.tx_cnt,
			       f_ac_sts20,
			       f_hold_sts21,note_sts,vtd_mst.hold_amt,vtd_mst.ctl_amt,
			       f_tfr_ind24,vtd_mst.cif_no);
			}*/
	  /*-----------------------------------update by guohua end-------------------------------*/		
		ttlnum++;
	}

	ret=Td_mst_Clo_Sel( );
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
