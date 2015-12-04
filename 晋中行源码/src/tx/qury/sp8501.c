/*************************************************
* 文 件 名:  sp8501.c
* 功能描述： 内部账信息查询
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
#include "com_item_c.h"

int sp8501()
{
	struct in_mst_c vin_mst;
	struct in_mst_c f_in_mst;
	struct in_mst_c f_in_msta;
	char ac_no1[25];
	char f_sts7[41];
	char f_od_ind15[41];
	char f_acno1[25];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char tmpmo[512];
	char tablelist[128];   /**要查询的表名**/
	char titstr[1024];
	int ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	FILE *fp;
	int kmbz=0;   

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	get_zd_data("0910",f_in_mst.opn_br_no );
	get_zd_data("0380",f_acno1);
    	pub_base_old_acno(f_acno1);
    	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	get_zd_long("0510",&f_in_mst.ac_seqn );
	get_zd_data("0240",f_in_mst.prdt_no );
	get_zd_double("1001",&f_in_mst.bal );
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	get_zd_double("1002",&f_in_msta.bal );
	get_zd_long("0290",&f_in_mst.opn_date );
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	get_zd_long("0440",&f_in_mst.end_date );
	get_zd_data("0260",f_in_mst.name );
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	/**组成查询条件**/
	pub_base_strpack( f_acno1 );
	kmbz=0;
	/*
	if( strlen(f_acno1)==3 || strlen(f_acno1)==5 || strlen(f_acno1)==7 )
	{
		goto KMCX;
	}
	*/
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	if( strlen(f_acno1) )
	{
		if ( pub_base_acno_acid(&f_in_mst.ac_id,f_acno1) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_in_mst.ac_id );
		strcat( wherelist,tmpstr );
		kmbz=1;
	}
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	pub_base_strpack( f_in_mst.opn_br_no );
	if( strlen(f_in_mst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_in_mst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	/*******************in no need ac_seqn********
	if( f_in_mst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_in_mst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	********************************************/
	pub_base_strpack( f_in_mst.prdt_no );
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	if( strlen(f_in_mst.prdt_no) )
	{
		sprintf( tmpstr," prdt_no='%s' and",f_in_mst.prdt_no );
		strcat( wherelist,tmpstr );
		kmbz=1;
	}
	vtcp_log("%s,%d\n",__FILE__,__LINE__);
	/***查询中间过渡账户余额***走此分支*ADD BEGIN 2010/8/11 11:20*
	**
	**记得更新bitmap的24,39,40,41,42位成1
	**49域的内容和平台约定，用于区分是平台发过来的查询交易
	****/
	char cChnl[5];/**2010-8-11 10:10**/
	memset(cChnl,0x00,sizeof(cChnl));
	get_zd_data("0920",cChnl);
	vtcp_log("%s,%d 0920===【%s】\n",__FILE__,__LINE__,cChnl);
	vtcp_log("%s,%d cChnl===【%s】\n",__FILE__,__LINE__,cChnl);
	if(memcmp(cChnl,"ZFCX",4)==0)/**49域可以和平台约定具体内容**/
	{
		struct in_mst_c z_in_mst;
		struct in_parm_c z_in_parm;
		struct com_item_c z_com_item;
		
		memset(&z_in_mst,0x00,sizeof(z_in_mst));
		memset(&z_in_parm,0x00,sizeof(z_in_parm));
		memset(&z_com_item,0x00,sizeof(z_com_item));
		
		ret=In_mst_Sel(g_pub_tx.reply,&z_in_mst,"ac_id=%ld",f_in_mst.ac_id);
		ERR_DEAL
		vtcp_log("%s,%d 查到中间过度账户的信息了！\n",__FILE__,__LINE__);
		set_zd_double("0390",z_in_mst.bal);
		set_zd_double("0400",z_in_mst.ys_bal);
		set_zd_data("0720",z_in_mst.sts);
		set_zd_data("0590",z_in_mst.mac);
		/**判断账户科目借贷属性**/
		ret=In_parm_Sel(g_pub_tx.reply,&z_in_parm,"prdt_no='%s'",z_in_mst.prdt_no);
		ERR_DEAL
		ret=pub_base_item_inf(z_in_parm.acc_hrt,&z_com_item);
		ERR_DEAL
		double sum_dr_amt=0.00,sum_cr_amt=0.00;
		if(z_com_item.dc_ind[0]=='1'){/**借方**/
			/**统计当日借方发生额**/
			ret=sql_sum_double("in_mst_hst","tx_amt",&sum_dr_amt,"ac_id=%ld and add_ind='1' and tx_date=%ld",f_in_mst.ac_id,g_pub_tx.tx_date);
			/**统计当日贷方发生额**/
			ret=sql_sum_double("in_mst_hst","tx_amt",&sum_cr_amt,"ac_id=%ld and add_ind='0' and tx_date=%ld",f_in_mst.ac_id,g_pub_tx.tx_date);
		}else if(z_com_item.dc_ind[0]=='2'){/**贷方**/
			/**统计当日借方发生额**/
			ret=sql_sum_double("in_mst_hst","tx_amt",&sum_dr_amt,"ac_id=%ld and add_ind='0' and tx_date=%ld",f_in_mst.ac_id,g_pub_tx.tx_date);
			/**统计当日贷方发生额**/
			ret=sql_sum_double("in_mst_hst","tx_amt",&sum_cr_amt,"ac_id=%ld and add_ind='1' and tx_date=%ld",f_in_mst.ac_id,g_pub_tx.tx_date);
		}else {/**双性*这里不知道有没有问题？待定*/
			/**统计当日借方发生额**/
			ret=sql_sum_double("in_mst_hst","tx_amt",&sum_dr_amt,"ac_id=%ld and add_ind='0' and tx_date=%ld",f_in_mst.ac_id,g_pub_tx.tx_date);
			/**统计当日贷方发生额**/
			ret=sql_sum_double("in_mst_hst","tx_amt",&sum_cr_amt,"ac_id=%ld and add_ind='1' and tx_date=%ld",f_in_mst.ac_id,g_pub_tx.tx_date);
		}
		set_zd_double("0410",sum_dr_amt);/**当日借方发生额**/
		set_zd_double("0420",sum_cr_amt);/**当日贷方发生额**/
		vtcp_log("%s,%d 查询完毕喽，退出了!\n",__FILE__,__LINE__);
		goto GoodExit;
	}
	/***查询中间过渡账户余额***走此分支*ADD END 2010/8/11 11:20*/
	if( f_in_mst.bal )
	{
		sprintf( tmpstr," bal>=%lf and",f_in_mst.bal );
		strcat( wherelist,tmpstr );
	}
	if( f_in_msta.bal )
	{
		sprintf( tmpstr," bal<=%lf and",f_in_msta.bal );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst.opn_date )
	{
		sprintf( tmpstr," opn_date=%ld and",f_in_mst.opn_date );
		strcat( wherelist,tmpstr );
		kmbz=1;
	}
	if( f_in_mst.end_date )
	{
		sprintf( tmpstr," end_date=%ld and",f_in_mst.end_date );
		strcat( wherelist,tmpstr );
		kmbz=1;
	}
	pub_base_strpack( f_in_mst.name );
	if( strlen(f_in_mst.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_in_mst.name );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"in_mst" );

	/**组成查询**/
	strcat(wherelist," 1=1  order by prdt_no,ac_id") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=In_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=In_mst_Fet_Sel( &vin_mst, g_pub_tx.reply );
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
	 	 fprintf( fp,"~@账号|@户名|@产品|$余额|$昨日余额|开户行|@帐户状态|开户日期|上笔交易日|利率|逾期利率|\n" );
		}

		pub_base_acid_acno( vin_mst.ac_id,vin_mst.ac_seqn,ac_no1 );
		if(vin_mst.bal<-2147000000) vin_mst.bal=0.0;
		if(vin_mst.ys_bal<-2147000000) vin_mst.ys_bal=0.0;
		if(vin_mst.hst_bal<-2147000000) vin_mst.hst_bal=0.0;
		pub_base_dic_show_str(f_sts7,"in_ac_sts",vin_mst.sts);
		if(vin_mst.opn_date<-2147000000) vin_mst.opn_date=0;
		if(vin_mst.ic_date<-2147000000) vin_mst.ic_date=0;
		if(vin_mst.end_date<-2147000000) vin_mst.end_date=0;
		if(vin_mst.cls_date<-2147000000) vin_mst.cls_date=0;
		if(vin_mst.lst_date<-2147000000) vin_mst.lst_date=0;
		if(vin_mst.rate<-2147000000) vin_mst.rate=0.0;
		if(vin_mst.over_rate<-2147000000) vin_mst.over_rate=0.0;
		pub_base_dic_show_str(f_od_ind15,"yes_no",vin_mst.od_ind);
		ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
			vin_mst.prdt_no );
		if( ret ) strcpy( g_in_parm.prdt_mo,vin_mst.prdt_no);
		pub_base_strpack(g_in_parm.prdt_mo);
		char tmp_name[31];memset(tmp_name,'\0',sizeof(tmp_name));
		memcpy(tmp_name,vin_mst.name,30);
		fprintf( fp, 
		"%s|%s|%s %s|%.2lf|%.2lf|%s|%s|%ld|%ld|%.5lf|%.5lf|\n",
		ac_no1,tmp_name,vin_mst.prdt_no,g_in_parm.prdt_mo,
		vin_mst.bal,vin_mst.ys_bal,vin_mst.opn_br_no,f_sts7,
		vin_mst.opn_date,vin_mst.lst_date,vin_mst.rate,
		vin_mst.over_rate);
		ttlnum++;
	}

	ret=In_mst_Clo_Sel( );
	ERR_DEAL

	/******************************************************/
KMCX:
	
	/*****************************************************/

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
