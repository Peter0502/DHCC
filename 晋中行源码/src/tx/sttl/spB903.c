/*************************************************
* 文 件 名:  spB903.c
* 功能描述： 同城明细查询
*
* 作    者:  liuxuan 
* 完成日期： 2006-8-19
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
#include "in_mst_hst_c.h"
#include "accountreg_c.h"
	struct in_mst_hst_c f_in_mst_hst;
	struct in_mst_hst_c f_in_mst_hsta;
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char fieldlist[1024];  /**要查询的字段列表**/
		char titstr[1024];
		char tmpstr[512];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;
struct accountreg_c accountreg;

int spB903()
{
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&f_in_mst_hst,0,sizeof(f_in_mst_hst));
	memset(&f_in_mst_hsta,0,sizeof(f_in_mst_hsta));
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset(&accountreg,'\0',sizeof(accountreg));	

	pub_base_sysinit();

	get_zd_data("0910",f_in_mst_hst.opn_br_no );	/* 交易机构 */
	get_zd_long("0290",&f_in_mst_hst.tx_date );
	get_zd_long("0440",&f_in_mst_hsta.tx_date );
	get_zd_data("0700",f_in_mst_hst.add_ind );	/* 1提出 2提入 3全部 */
	get_zd_double("1001",&f_in_mst_hst.tx_amt );
	get_zd_double("1002",&f_in_mst_hsta.tx_amt );
	/*get_zd_data("0590",f_in_mst_hst.note_no );	* 交换号 */
	get_zd_data("0590",f_in_mst_hst.tx_br_no);	/* 交易机构号 */
	get_zd_data("0380",accountreg.pay_ac_no);/* 输入帐号,可能是付款也可能是收款暂时放到付款里 */

	ret=get_ac_hst();
	if( ret ) goto ErrExit;

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
get_ac_hst()
{
	char tmp_no[64],sstat[5];

	/**组成查询条件**/

	memset( wherelist,0,sizeof(wherelist)); 
	/*
	pub_base_strpack( f_in_mst_hst.opn_br_no );
	if( strlen(f_in_mst_hst.opn_br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_in_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	*/
	if( f_in_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_in_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_in_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.add_ind );
	if( f_in_mst_hst.add_ind[0]!='3')
	{
		sprintf( tmpstr," put_flag='%c' and",f_in_mst_hst.add_ind[0]);
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hst.tx_amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_in_mst_hst.tx_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hsta.tx_amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_in_mst_hsta.tx_amt );
		strcat( wherelist,tmpstr );
	}
	/*
	pub_base_strpack( f_in_mst_hst.note_no );
	if( strlen(f_in_mst_hst.note_no) )
	{
		if( f_in_mst_hst.ct_ind[0]=='1'){
			sprintf( tmpstr," change_no='%s' and ",f_in_mst_hst.note_no);
			strcat( wherelist,tmpstr );
		}
		else if( f_in_mst_hst.ct_ind[0]=='2'){
			sprintf( tmpstr," cash_ac_no='%s' and ",f_in_mst_hst.note_no);
			strcat( wherelist,tmpstr );
		}
		else{
			sprintf( tmpstr," (change_no='%s' or cash_ac_no='%s') and ",f_in_mst_hst.note_no,f_in_mst_hst.note_no);
			strcat( wherelist,tmpstr );
		}
	}
	*/
	pub_base_strpack( f_in_mst_hst.tx_br_no );
	if( strlen(f_in_mst_hst.tx_br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_in_mst_hst.tx_br_no);
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( accountreg.pay_ac_no);
	if( strlen( accountreg.pay_ac_no))
	{
		if( f_in_mst_hst.ct_ind[0]=='1'){
			sprintf( tmpstr," pay_ac_no='%s' and ",accountreg.pay_ac_no);
			strcat( wherelist,tmpstr );
		}
		else if( f_in_mst_hst.ct_ind[0]=='2'){
			sprintf( tmpstr," cash_ac_no='%s' and ",accountreg.pay_ac_no);
			strcat( wherelist,tmpstr );
		}
		else{
			sprintf( tmpstr," (cash_ac_no='%s' or pay_ac_no='%s') and ",accountreg.pay_ac_no,accountreg.pay_ac_no);
			strcat( wherelist,tmpstr );
		}
	}

	/**组成查询**/
	vtcp_log("查询条件 wherelist =[%s]",wherelist);
	ret=Accountreg_Dec_Sel( g_pub_tx.reply, " %s  stat!='9' order by br_no",wherelist);
	ERR_DEAL

	memset(&accountreg,'\0',sizeof(accountreg));
	ttlnum=0;
	double tot_amt=0;
	while( 1 )
	{
		ret=Accountreg_Fet_Sel( &accountreg, g_pub_tx.reply );
		if( ret==100 ){
		fprintf( fp, "%s|%d%s|%.2lf|\n",
			"合  计",ttlnum,"  笔 ",tot_amt);
			break;
		}
		ERR_DEAL

		switch(accountreg.stat[0]){
			case '1':
				strcpy(sstat,"登记");
				break;
			case '2':
				strcpy(sstat,"记帐");
				break;
			case '3':
				strcpy(sstat,"复核");
				break;
			case '9':
				strcpy(sstat,"取消");
				break;
			default:
				strcpy(sstat,"未知");
				break;
		}

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
			fprintf( fp,
			"~@付款人帐号|付款人名称|$发生额|交换号|收款人帐号|收款人名称|状态|\n");
		}
		fprintf( fp, "%.20s|%.24s|%.2lf|%s|%.20s|%.24s|%s|\n",
			accountreg.pay_ac_no,accountreg.pay_name,accountreg.amt,\
			accountreg.change_no,accountreg.cash_ac_no,accountreg.cash_name,sstat);
		ttlnum++;
		tot_amt +=accountreg.amt;
	}
	Accountreg_Clo_Sel();

	return 0;
ErrExit:
	return -1;
}
