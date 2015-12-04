/*************************************************
* 文 件 名:  sp8601.c
* 功能描述： 历史总账查询
*
* 作    者:  rob
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
#include "gl_mst_hst_c.h"
#include "com_item_c.h"

int sp8601()
{
	struct gl_mst_hst_c vgl_mst_hst;
	struct gl_mst_hst_c f_gl_mst_hst;
	struct gl_mst_hst_c f_gl_mst_hsta;
	struct com_item_c sCom_item;
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

	get_zd_data("0910",f_gl_mst_hst.br_no );
	get_zd_data("0210",f_gl_mst_hst.cur_no );
/*
	get_zd_data("0640",f_gl_mst_hst.acc_hrt );
*/
	char acc_no[8];
	memset(acc_no,'\0',sizeof(acc_no));
	get_zd_data("0640",acc_no);
        ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_no='%s'",
            acc_no);
        if (ret)
        {
            sprintf(acErrMsg, "科目表没有该科目号!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "L129");
            goto ErrExit;
        }
	strcpy(f_gl_mst_hst.acc_hrt,sCom_item.acc_hrt);

	get_zd_long("0290",&f_gl_mst_hst.date );
	get_zd_long("0440",&f_gl_mst_hsta.date );

	/**组成查询条件**/
	pub_base_strpack( f_gl_mst_hst.br_no );
	if( strlen(f_gl_mst_hst.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_gl_mst_hst.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_gl_mst_hst.cur_no );
	if( strlen(f_gl_mst_hst.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_gl_mst_hst.cur_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_gl_mst_hst.acc_hrt );
	if( strlen(f_gl_mst_hst.acc_hrt) )
	{
		sprintf( tmpstr," acc_hrt='%s' and",f_gl_mst_hst.acc_hrt );
		strcat( wherelist,tmpstr );
	}
	if( f_gl_mst_hst.date )
	{
		sprintf( tmpstr," \"date\">=%ld and",f_gl_mst_hst.date );
		strcat( wherelist,tmpstr );
	}
	if( f_gl_mst_hsta.date )
	{
		sprintf( tmpstr," \"date\"<=%ld and",f_gl_mst_hsta.date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"gl_mst_hst" );

	/**组成查询**/
	strcat(wherelist," 1=1 order by \"date\" ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"br_no,cur_no,acc_hrt,\"date\",dr_bal,cr_bal,ldd_bal,lcd_bal,rdd_cnt,rcd_cnt,rdd_amt,rcd_amt,cdd_cnt,ccd_cnt,cdd_amt,ccd_amt");

	ret=Gl_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Gl_mst_hst_Fet_Sel( &vgl_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		memset(acc_no,'\0',sizeof(acc_no));
        ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_hrt='%s'",
            vgl_mst_hst.acc_hrt);
        if (ret)
        {
            sprintf(acErrMsg, "科目表没有该科目号!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "L129");
            goto ErrExit;
        }
	strcpy(acc_no,sCom_item.acc_no);

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
			fprintf( fp,"~@科目号|交易日期|$当前借方余额|$当前贷方余额|$昨日借方余额|$昨日贷方余额|$本日借笔数|$本日贷笔数|$本日借发生额|$本日贷发生额|本日现金借笔数|本日现金贷笔数|$本日现金借发生额|$本日现金贷发生额|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vgl_mst_hst.date<-2147000000) vgl_mst_hst.date=0;
		if(vgl_mst_hst.dr_bal<-2147000000) vgl_mst_hst.dr_bal=0.0;
		if(vgl_mst_hst.cr_bal<-2147000000) vgl_mst_hst.cr_bal=0.0;
		if(vgl_mst_hst.ldd_bal<-2147000000) vgl_mst_hst.ldd_bal=0.0;
		if(vgl_mst_hst.lcd_bal<-2147000000) vgl_mst_hst.lcd_bal=0.0;
		if(vgl_mst_hst.rdd_cnt<-2147000000) vgl_mst_hst.rdd_cnt=0.0;
		if(vgl_mst_hst.rcd_cnt<-2147000000) vgl_mst_hst.rcd_cnt=0.0;
		if(vgl_mst_hst.rdd_amt<-2147000000) vgl_mst_hst.rdd_amt=0.0;
		if(vgl_mst_hst.rcd_amt<-2147000000) vgl_mst_hst.rcd_amt=0.0;
		if(vgl_mst_hst.cdd_cnt<-2147000000) vgl_mst_hst.cdd_cnt=0;
		if(vgl_mst_hst.ccd_cnt<-2147000000) vgl_mst_hst.ccd_cnt=0;
		if(vgl_mst_hst.cdd_amt<-2147000000) vgl_mst_hst.cdd_amt=0.0;
		if(vgl_mst_hst.ccd_amt<-2147000000) vgl_mst_hst.ccd_amt=0.0;

		fprintf( fp, "%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%.2ld|%.2ld|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|\n",acc_no,vgl_mst_hst.date,vgl_mst_hst.dr_bal,vgl_mst_hst.cr_bal,vgl_mst_hst.ldd_bal,vgl_mst_hst.lcd_bal,vgl_mst_hst.rdd_cnt,vgl_mst_hst.rcd_cnt,vgl_mst_hst.rdd_amt,vgl_mst_hst.rcd_amt,vgl_mst_hst.cdd_cnt,vgl_mst_hst.ccd_cnt,vgl_mst_hst.cdd_amt,vgl_mst_hst.ccd_amt);
		ttlnum++;
	}

	ret=Gl_mst_hst_Clo_Sel( );
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
