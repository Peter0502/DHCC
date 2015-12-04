/***************************************************************
* 文 件 名: sp8122.c
* 功能描述：科目信息查询
* 作    者: lance
***************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "账号信息统计出错" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <stdio.h>
#include "public.h"
#include "com_item_c.h"
#include "com_branch_c.h"

char tmp_str1[2],tmp_str2[2],tmpstr[20],tmpstr1[20];
char filename[100];
FILE *fp;   
int ttlnum=0;
char acc_no[11];
char sts[3];

sp8122()
{   
    int ret=0;
	struct com_item_c com_item;
	struct com_branch_c com_branch;
	char qsjetj[100];
	char zzjetj[100];
	char hqkmtj[200];
	char dqkmtj[200];
	char lnkmtj[200];
	char inkmtj[200];
	char jgtj[200];
	char wherelist[1024];
	char vacno[33];
	double sumbal;
	long cnt;

    memset (tmp_str1, 0, sizeof(tmp_str1));
    memset (tmp_str2, 0, sizeof(tmp_str2));
    memset (filename, 0, sizeof(filename));

	memset( qsjetj,0,sizeof(qsjetj) );
	memset( zzjetj,0,sizeof(zzjetj) );
	memset( hqkmtj,0,sizeof(hqkmtj) );
	memset( dqkmtj,0,sizeof(dqkmtj) );
	memset( lnkmtj,0,sizeof(lnkmtj) );
	memset( inkmtj,0,sizeof(inkmtj) );
	memset( jgtj,0,sizeof(jgtj) );
       
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
       
    /*** 初始化全局变量 ***/
     
     pub_base_sysinit();     
     
     get_zd_double("1001",&g_pub_tx.tx_amt1); /**金额**/
     get_zd_double("1002",&g_pub_tx.tx_amt2);

     get_zd_data("0700",sts); /**帐户状态**/
     get_zd_data("0910",g_pub_tx.opn_br_no); /*机构*/
	 if( !strlen(g_pub_tx.opn_br_no) )  /**无机构则只查本所的**/
		strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );

     get_zd_data("1039",acc_no); /**科目**/
	
	if( g_pub_tx.tx_amt1<-0.005 || g_pub_tx.tx_amt1>0.005 )
	{
		sprintf( qsjetj,"bal>=%.2lf",g_pub_tx.tx_amt1);
	}

	if( g_pub_tx.tx_amt2<-0.005 || g_pub_tx.tx_amt2>0.005 )
	{
		sprintf( zzjetj,"bal<=%.2lf",g_pub_tx.tx_amt2);
	}

	if( strlen(acc_no) )
	{
		ret=Com_item_Sel( g_pub_tx.reply,&com_item,"acc_no='%s'",
			acc_no );
		if( ret )
		{
			sprintf(acErrMsg,"acc_no=[%s]",acc_no );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S033" );
			goto ErrExit;
		}
		pub_base_strpack( com_item.acc_hrt );
		sprintf( hqkmtj," prdt_no in(select prdt_no from dd_parm where dc_code in (select dc_code from dc_acc_rel where acc_hrt='%s') ) ",com_item.acc_hrt );

		sprintf( dqkmtj," prdt_no in(select prdt_no from td_parm where dc_code in (select dc_code from dc_acc_rel where acc_hrt='%s') ) ",com_item.acc_hrt );

		sprintf( lnkmtj," prdt_no in(select prdt_no from ln_parm where dc_code in (select dc_code from dc_acc_rel where acc_hrt='%s') ) ",com_item.acc_hrt );

		sprintf( inkmtj," prdt_no in(select prdt_no from in_parm where dc_code in (select dc_code from dc_acc_rel where acc_hrt='%s') ) ",com_item.acc_hrt );
	}
	/*********机构条件***********************/
	ret=Com_branch_Sel( g_pub_tx.reply,&com_branch,"br_no='%s'",
		g_pub_tx.opn_br_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"br_no=[%s]",g_pub_tx.opn_br_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O116" );
		goto ErrExit;
	}
	else { ERR_DEAL }
	if( com_branch.br_type[0]=='7' ) /**支行**/
	{
		sprintf( jgtj,
	"opn_br_no in(select br_no from com_branch where up_br_no='%s' or br_no='%s')",
			g_pub_tx.opn_br_no, g_pub_tx.opn_br_no );
	}
	else if( com_branch.br_type[0]=='6' ) /**总行**/
	{
		sprintf( jgtj," " );
	}
	else /*普通网点*/
	{
		sprintf( jgtj,"opn_br_no='%s'",g_pub_tx.opn_br_no );
	}

	strcpy( wherelist,jgtj );
	if( strlen(qsjetj) ) 
	{
		strcat( wherelist," and " );
		strcat( wherelist,qsjetj );
	}
	if( strlen(zzjetj) ) 
	{
		strcat( wherelist," and " );
		strcat( wherelist,zzjetj );
	}
     
     pub_base_AllDwnFilName( filename );
     fp = fopen( filename,"w" );
     if( fp==NULL )
	 {
	  	sprintf(acErrMsg," open file [%s] error ",filename );
	  	WRITEMSG
	  	strcpy( g_pub_tx.reply,"S047" );
	  	goto ErrExit;
	 }
     
		fprintf(fp,"~@帐号|@户名|$余额|\n");

	sumbal=0.00;
	cnt=0;
	 /**hq**/
	 if( strlen(hqkmtj) )
	 {
		ret=Dd_mst_Dec_Sel( g_pub_tx.reply ,"%s and %s",wherelist,hqkmtj );
	 }
	 else
	 {
		ret=Dd_mst_Dec_Sel( g_pub_tx.reply ,"%s",wherelist );
	 }
vtcp_log("PPPP[%s %s]",wherelist,hqkmtj );
	 while( 1 )
	 {
	 	ret=Dd_mst_Fet_Sel( &g_dd_mst , g_pub_tx.reply );
		if( ret==100 ) break;
	 	ERR_DEAL

		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%d",g_dd_mst.ac_id );
		if( ret ) strcpy( vacno," " );
		else strcpy( vacno,g_mdm_ac_rel.ac_no );
		pub_base_strpack(g_dd_mst.name);
		fprintf( fp,"%s|%s|%.2lf|\n",vacno,g_dd_mst.name,g_dd_mst.bal );
		sumbal+=g_dd_mst.bal;
		cnt++;
	 }
	 Dd_mst_Clo_Sel();
      
	 /**dq**/
	 if( strlen(dqkmtj) )
	 {
		ret=Td_mst_Dec_Sel( g_pub_tx.reply ,"%s and %s",wherelist,dqkmtj );
	 }
	 else
	 {
		ret=Td_mst_Dec_Sel( g_pub_tx.reply ,"%s",wherelist );
	 }
vtcp_log("PPPP[%s %s]",wherelist,dqkmtj );
	 while( 1 )
	 {
	 	ret=Td_mst_Fet_Sel( &g_td_mst , g_pub_tx.reply );
		if( ret==100 ) break;
	 	ERR_DEAL

		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%d",g_td_mst.ac_id );
		if( ret ) strcpy( vacno," " );
		else strcpy( vacno,g_mdm_ac_rel.ac_no );
		pub_base_strpack(g_td_mst.name);
		fprintf( fp,"%s|%s|%.2lf|\n",vacno,g_td_mst.name,g_td_mst.bal );
		sumbal+=g_td_mst.bal;
		cnt++;
	 }
	 Td_mst_Clo_Sel();

	 /**ln**/
	 if( strlen(lnkmtj) )
	 {
		ret=Ln_mst_Dec_Sel( g_pub_tx.reply ,"%s and %s",wherelist,lnkmtj );
	 }
	 else
	 {
		ret=Ln_mst_Dec_Sel( g_pub_tx.reply ,"%s",wherelist );
	 }
vtcp_log("PPPP[%s %s]",wherelist,lnkmtj );
	 while( 1 )
	 {
	 	ret=Ln_mst_Fet_Sel( &g_ln_mst , g_pub_tx.reply );
		if( ret==100 ) break;
	 	ERR_DEAL

		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%d",g_ln_mst.ac_id );
		if( ret ) strcpy( vacno," " );
		else strcpy( vacno,g_mdm_ac_rel.ac_no );
		pub_base_strpack(g_ln_mst.name);
		fprintf( fp,"%s|%s|%.2lf|\n",vacno,g_ln_mst.name,g_ln_mst.bal );
		sumbal+=g_ln_mst.bal;
		cnt++;
	 }
	 Ln_mst_Clo_Sel();

	 /**in**/
	 if( strlen(inkmtj) )
	 {
		ret=In_mst_Dec_Sel( g_pub_tx.reply ,"%s and %s",wherelist,inkmtj );
	 }
	 else
	 {
		ret=In_mst_Dec_Sel( g_pub_tx.reply ,"%s",wherelist );
	 }
vtcp_log("PPPP[%s %s]",wherelist,inkmtj );
	 while( 1 )
	 {
	 	ret=In_mst_Fet_Sel( &g_in_mst , g_pub_tx.reply );
		if( ret==100 ) break;
	 	ERR_DEAL

		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%d",g_in_mst.ac_id );
		if( ret ) strcpy( vacno," " );
		else strcpy( vacno,g_mdm_ac_rel.ac_no );
		pub_base_strpack(g_in_mst.name);
		fprintf( fp,"%s|%s|%.2lf|\n",vacno,g_in_mst.name,g_in_mst.bal );
		sumbal+=g_in_mst.bal;
		cnt++;
	 }
	 In_mst_Clo_Sel();

		fprintf( fp,"%s|共%d户|%.2lf|\n","合计",cnt,sumbal );

	  fclose(fp);

	set_zd_data( DC_FILE_SND,"1" );
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
