/*************************************************
* 文 件 名:  sp8114.c
* 功能描述： 产品动态表查询
*
* 作    者:  
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
#include "gl_prdt_dyn_c.h"

int sp8114()
{
	struct gl_prdt_dyn_c vgl_prdt_dyn;
	struct gl_prdt_dyn_c f_gl_prdt_dyn;
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

	get_zd_data("0240",f_gl_prdt_dyn.prdt_no );
	get_zd_data("0910",f_gl_prdt_dyn.br_no );
	get_zd_double("1001",&f_gl_prdt_dyn.cr_bal );
	get_zd_double("1002",&f_gl_prdt_dyn.lcd_bal );

	/**组成查询条件**/
	pub_base_strpack( f_gl_prdt_dyn.prdt_no );
	if( strlen(f_gl_prdt_dyn.prdt_no) )
	{
		sprintf( tmpstr," prdt_no='%s' and",f_gl_prdt_dyn.prdt_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_gl_prdt_dyn.br_no );
	if( strlen(f_gl_prdt_dyn.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_gl_prdt_dyn.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_gl_prdt_dyn.cr_bal )
	{
		sprintf( tmpstr," cr_bal=%lf and",f_gl_prdt_dyn.cr_bal );
		strcat( wherelist,tmpstr );
	}
	if( f_gl_prdt_dyn.lcd_bal )
	{
		sprintf( tmpstr," lcd_bal=%lf and",f_gl_prdt_dyn.lcd_bal );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"gl_prdt_dyn" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"prdt_no,br_no,cr_bal,ac_cnt,lcd_bal,l_ac_cnt,d_opn_cnt,d_cls_cnt,rdd_cnt,rcd_cnt,rdd_amt,rcd_amt,cdd_cnt,ccd_cnt,cdd_amt,ccd_amt,tdcr_bal,td_ac_cnt,td_opn_cnt,td_cls_cnt,td_dr_cnt,td_cr_cnt,td_dr_amt,td_cr_amt,m_cr_bal,m_ac_cnt,m_opn_cnt,m_cls_cnt,m_dr_cnt,m_cr_cnt,m_dr_amt,m_cr_amt,q_cr_bal,q_ac_cnt,q_opn_cnt,q_cls_cnt,q_dr_cnt,q_cr_cnt,q_dr_amt,q_cr_amt,y_cr_bal,y_ac_cnt,y_opn_cnt,y_cls_cnt,y_dr_cnt,y_cr_cnt,y_dr_amt,y_cr_amt,spe_ind");

	ret=Gl_prdt_dyn_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel( &vgl_prdt_dyn, g_pub_tx.reply );
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
			fprintf( fp,"~产品代号|机构编码|当前余额|当前结余户数|$昨日余额|昨日结余户数|本日开户数|本日销户数|本日减少笔数|本日增加笔数|$本日减少发生额|$本日增加发生额|本日现金减少笔数|本日现金增加笔数|$本日现金减少发生额|$本日现金增加发生额|$旬初余额|旬初结余户数|本旬开户数|本旬销户数|本旬减少笔数|本旬增加笔数|$本旬减少发生额|$本旬增加发生额|$月初余额|月初结余户数|本月开户数|本月销户数|本月减少笔数|本月增加笔数|$本月减少发生额|$本月增加发生额|$季初余额|季初结余户数|本季开户数|本季销户数|本季减少笔数|本季增加笔数|$本季减少发生额|$本季增加发生额|$年初余额|年初结余户数|本年开户数|本年销户数|本年减少笔数|本年增加笔数|本年减少发生额|本年增加发生额|标志位|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vgl_prdt_dyn.cr_bal<-2147000000) vgl_prdt_dyn.cr_bal=0.0;
		if(vgl_prdt_dyn.ac_cnt<-2147000000) vgl_prdt_dyn.ac_cnt=0;
		if(vgl_prdt_dyn.lcd_bal<-2147000000) vgl_prdt_dyn.lcd_bal=0.0;
		if(vgl_prdt_dyn.l_ac_cnt<-2147000000) vgl_prdt_dyn.l_ac_cnt=0;
		if(vgl_prdt_dyn.d_opn_cnt<-2147000000) vgl_prdt_dyn.d_opn_cnt=0;
		if(vgl_prdt_dyn.d_cls_cnt<-2147000000) vgl_prdt_dyn.d_cls_cnt=0;
		if(vgl_prdt_dyn.rdd_cnt<-2147000000) vgl_prdt_dyn.rdd_cnt=0;
		if(vgl_prdt_dyn.rcd_cnt<-2147000000) vgl_prdt_dyn.rcd_cnt=0;
		if(vgl_prdt_dyn.rdd_amt<-2147000000) vgl_prdt_dyn.rdd_amt=0.0;
		if(vgl_prdt_dyn.rcd_amt<-2147000000) vgl_prdt_dyn.rcd_amt=0.0;
		if(vgl_prdt_dyn.cdd_cnt<-2147000000) vgl_prdt_dyn.cdd_cnt=0;
		if(vgl_prdt_dyn.ccd_cnt<-2147000000) vgl_prdt_dyn.ccd_cnt=0;
		if(vgl_prdt_dyn.cdd_amt<-2147000000) vgl_prdt_dyn.cdd_amt=0.0;
		if(vgl_prdt_dyn.ccd_amt<-2147000000) vgl_prdt_dyn.ccd_amt=0.0;
		if(vgl_prdt_dyn.tdcr_bal<-2147000000) vgl_prdt_dyn.tdcr_bal=0.0;
		if(vgl_prdt_dyn.td_ac_cnt<-2147000000) vgl_prdt_dyn.td_ac_cnt=0;
		if(vgl_prdt_dyn.td_opn_cnt<-2147000000) vgl_prdt_dyn.td_opn_cnt=0;
		if(vgl_prdt_dyn.td_cls_cnt<-2147000000) vgl_prdt_dyn.td_cls_cnt=0;
		if(vgl_prdt_dyn.td_dr_cnt<-2147000000) vgl_prdt_dyn.td_dr_cnt=0;
		if(vgl_prdt_dyn.td_cr_cnt<-2147000000) vgl_prdt_dyn.td_cr_cnt=0;
		if(vgl_prdt_dyn.td_dr_amt<-2147000000) vgl_prdt_dyn.td_dr_amt=0.0;
		if(vgl_prdt_dyn.td_cr_amt<-2147000000) vgl_prdt_dyn.td_cr_amt=0.0;
		if(vgl_prdt_dyn.m_cr_bal<-2147000000) vgl_prdt_dyn.m_cr_bal=0.0;
		if(vgl_prdt_dyn.m_ac_cnt<-2147000000) vgl_prdt_dyn.m_ac_cnt=0;
		if(vgl_prdt_dyn.m_opn_cnt<-2147000000) vgl_prdt_dyn.m_opn_cnt=0;
		if(vgl_prdt_dyn.m_cls_cnt<-2147000000) vgl_prdt_dyn.m_cls_cnt=0;
		if(vgl_prdt_dyn.m_dr_cnt<-2147000000) vgl_prdt_dyn.m_dr_cnt=0;
		if(vgl_prdt_dyn.m_cr_cnt<-2147000000) vgl_prdt_dyn.m_cr_cnt=0;
		if(vgl_prdt_dyn.m_dr_amt<-2147000000) vgl_prdt_dyn.m_dr_amt=0.0;
		if(vgl_prdt_dyn.m_cr_amt<-2147000000) vgl_prdt_dyn.m_cr_amt=0.0;
		if(vgl_prdt_dyn.q_cr_bal<-2147000000) vgl_prdt_dyn.q_cr_bal=0.0;
		if(vgl_prdt_dyn.q_ac_cnt<-2147000000) vgl_prdt_dyn.q_ac_cnt=0;
		if(vgl_prdt_dyn.q_opn_cnt<-2147000000) vgl_prdt_dyn.q_opn_cnt=0;
		if(vgl_prdt_dyn.q_cls_cnt<-2147000000) vgl_prdt_dyn.q_cls_cnt=0;
		if(vgl_prdt_dyn.q_dr_cnt<-2147000000) vgl_prdt_dyn.q_dr_cnt=0;
		if(vgl_prdt_dyn.q_cr_cnt<-2147000000) vgl_prdt_dyn.q_cr_cnt=0;
		if(vgl_prdt_dyn.q_dr_amt<-2147000000) vgl_prdt_dyn.q_dr_amt=0.0;
		if(vgl_prdt_dyn.q_cr_amt<-2147000000) vgl_prdt_dyn.q_cr_amt=0.0;
		if(vgl_prdt_dyn.y_cr_bal<-2147000000) vgl_prdt_dyn.y_cr_bal=0.0;
		if(vgl_prdt_dyn.y_ac_cnt<-2147000000) vgl_prdt_dyn.y_ac_cnt=0;
		if(vgl_prdt_dyn.y_opn_cnt<-2147000000) vgl_prdt_dyn.y_opn_cnt=0;
		if(vgl_prdt_dyn.y_cls_cnt<-2147000000) vgl_prdt_dyn.y_cls_cnt=0;
		if(vgl_prdt_dyn.y_dr_cnt<-2147000000) vgl_prdt_dyn.y_dr_cnt=0;
		if(vgl_prdt_dyn.y_cr_cnt<-2147000000) vgl_prdt_dyn.y_cr_cnt=0;
		if(vgl_prdt_dyn.y_dr_amt<-2147000000) vgl_prdt_dyn.y_dr_amt=0.0;
		if(vgl_prdt_dyn.y_cr_amt<-2147000000) vgl_prdt_dyn.y_cr_amt=0.0;

		fprintf( fp, "%s|%s|%.2lf|%ld|%.2lf|%ld|%ld|%ld|%ld|%ld|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%ld|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%ld|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%ld|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%ld|%.2lf|%.2lf|%s|\n",
		vgl_prdt_dyn.prdt_no,vgl_prdt_dyn.br_no,vgl_prdt_dyn.cr_bal,vgl_prdt_dyn.ac_cnt,vgl_prdt_dyn.lcd_bal,vgl_prdt_dyn.l_ac_cnt,vgl_prdt_dyn.d_opn_cnt,vgl_prdt_dyn.d_cls_cnt,vgl_prdt_dyn.rdd_cnt,vgl_prdt_dyn.rcd_cnt,vgl_prdt_dyn.rdd_amt,vgl_prdt_dyn.rcd_amt,vgl_prdt_dyn.cdd_cnt,vgl_prdt_dyn.ccd_cnt,vgl_prdt_dyn.cdd_amt,vgl_prdt_dyn.ccd_amt,vgl_prdt_dyn.tdcr_bal,vgl_prdt_dyn.td_ac_cnt,vgl_prdt_dyn.td_opn_cnt,vgl_prdt_dyn.td_cls_cnt,vgl_prdt_dyn.td_dr_cnt,vgl_prdt_dyn.td_cr_cnt,vgl_prdt_dyn.td_dr_amt,vgl_prdt_dyn.td_cr_amt,vgl_prdt_dyn.m_cr_bal,vgl_prdt_dyn.m_ac_cnt,vgl_prdt_dyn.m_opn_cnt,vgl_prdt_dyn.m_cls_cnt,vgl_prdt_dyn.m_dr_cnt,vgl_prdt_dyn.m_cr_cnt,vgl_prdt_dyn.m_dr_amt,vgl_prdt_dyn.m_cr_amt,vgl_prdt_dyn.q_cr_bal,vgl_prdt_dyn.q_ac_cnt,vgl_prdt_dyn.q_opn_cnt,vgl_prdt_dyn.q_cls_cnt,vgl_prdt_dyn.q_dr_cnt,vgl_prdt_dyn.q_cr_cnt,vgl_prdt_dyn.q_dr_amt,vgl_prdt_dyn.q_cr_amt,vgl_prdt_dyn.y_cr_bal,vgl_prdt_dyn.y_ac_cnt,vgl_prdt_dyn.y_opn_cnt,vgl_prdt_dyn.y_cls_cnt,vgl_prdt_dyn.y_dr_cnt,vgl_prdt_dyn.y_cr_cnt,vgl_prdt_dyn.y_dr_amt,vgl_prdt_dyn.y_cr_amt,vgl_prdt_dyn.spe_ind);
		ttlnum++;
	}

	ret=Gl_prdt_dyn_Clo_Sel( );
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
