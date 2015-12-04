/*************************************************
* 文 件 名:  sp8504.c
* 功能描述： 总账查询
*
* 作    者:  peter
* 完成日期： 
*
* 修改记录： xyy
* 日   期:   2006-10-29 22:38
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
#include "gl_mst_c.h"
#include "com_item_c.h"
int sp8504()
{
	struct gl_mst_c vgl_mst;
	struct gl_mst_c f_gl_mst;
	struct gl_mst_c f_gl_msta;
	struct com_item_c com_item;
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char cAcc_no[8];
	  char cAcc_hrt[6];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

  ttlnum=0;
  memset(cAcc_no   ,0,sizeof(cAcc_no));
  memset(cAcc_hrt  ,0,sizeof(cAcc_hrt));
  memset( &com_item,0,sizeof(com_item) );
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_gl_mst.br_no );
	get_zd_data("0210",f_gl_mst.cur_no );
	get_zd_data("0640",cAcc_no );
	get_zd_long("0290",&f_gl_mst.date );
	get_zd_long("0440",&f_gl_msta.date );
  vtcp_log("[%s][%d]cAcc_no=[%s]",__FILE__,__LINE__,cAcc_no);
  ret=Com_item_Dec_Sel( g_pub_tx.reply,"acc_no like '%s%%%%' order by acc_no",cAcc_no);
  ERR_DEAL
  while(1)
  {
  	  memset(cAcc_no,0,sizeof(cAcc_no));
  	  memset(cAcc_hrt,0,sizeof(cAcc_hrt));
  	  ret=Com_item_Fet_Sel( &com_item, g_pub_tx.reply );
	    if( ret==100 ) break;
	    ERR_DEAL
	    memcpy(cAcc_hrt,com_item.acc_hrt,sizeof(cAcc_hrt)-1);
	    memcpy(cAcc_no,com_item.acc_no,sizeof(cAcc_no)-1);
	    vtcp_log("[%s][%d]cAcc_hrt=[%s]",__FILE__,__LINE__,cAcc_hrt);
	    vtcp_log("[%s][%d]cAcc_no=[%s]",__FILE__,__LINE__,cAcc_no);
	    zip_space(cAcc_hrt);
	    memcpy(f_gl_mst.acc_hrt,cAcc_hrt,strlen(cAcc_hrt));
	    vtcp_log("[%s][%d]fcAcc_hrt==[%s]",__FILE__,__LINE__,cAcc_hrt);
	    /**组成查询条件**/
	    pub_base_strpack( f_gl_mst.br_no );
	    pub_base_strpack( f_gl_mst.cur_no );
	    pub_base_strpack( f_gl_mst.acc_hrt );
      if(f_gl_mst.date==0 && f_gl_msta.date==0)
      {
      	  ret=Gl_mst_Dec_Sel( g_pub_tx.reply,"br_no='%s' and cur_no='%s' and acc_hrt ='%s'  and  1=1",f_gl_mst.br_no,f_gl_mst.cur_no,cAcc_hrt);
	    }
	    else
	    {
	    	  ret=Gl_mst_Dec_Sel( g_pub_tx.reply,"br_no='%s' and cur_no='%s' and acc_hrt ='%s' and date >='%ld' and date<='%ld' and  1=1",f_gl_mst.br_no,f_gl_mst.cur_no,cAcc_hrt,f_gl_mst.date,f_gl_msta.date);
	    }	
	    ERR_DEAL

	    while(1)
	    {
	    	ret=Gl_mst_Fet_Sel( &vgl_mst, g_pub_tx.reply );
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
	    		fprintf( fp,"~@机构编码|@币种|@科    目 |交易日期|$当前借方余额|$当前贷方余额|$昨日借方余额|$昨日贷方余额|$本日借笔数|$本日贷笔数|$本日借发生额|$本日贷发生额|本日现金借笔数|本日现金贷笔数|$本日现金借发生额|$本日现金贷发生额|$旬初借方余额|$旬初贷方余额|$本旬借方笔数|$本旬贷方笔数|$本旬借方发生额|$本旬贷方发生额|$月初借余额|$月初贷余额|$本月借笔|$本月贷笔数|$本月借发生额|$本月贷发生额|$季初借余额|$季初贷余额|$本季借笔数|$本季贷笔数|$本季借发生额|$本季贷发生额|$年初借余额|$年初贷余额|$本年借笔数|$本年贷笔数|$本年借发生额|$本年贷发生额|\n" );
	    	}
      
	    	/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
      
	    	if(vgl_mst.date<-2147000000) vgl_mst.date=0;
	    	if(vgl_mst.dr_bal<-2147000000) vgl_mst.dr_bal=0.0;
	    	if(vgl_mst.cr_bal<-2147000000) vgl_mst.cr_bal=0.0;
	    	if(vgl_mst.ldd_bal<-2147000000) vgl_mst.ldd_bal=0.0;
	    	if(vgl_mst.lcd_bal<-2147000000) vgl_mst.lcd_bal=0.0;
	    	if(vgl_mst.rdd_cnt<-2147000000) vgl_mst.rdd_cnt=0.0;
	    	if(vgl_mst.rcd_cnt<-2147000000) vgl_mst.rcd_cnt=0.0;
	    	if(vgl_mst.rdd_amt<-2147000000) vgl_mst.rdd_amt=0.0;
	    	if(vgl_mst.rcd_amt<-2147000000) vgl_mst.rcd_amt=0.0;
	    	if(vgl_mst.cdd_cnt<-2147000000) vgl_mst.cdd_cnt=0;
	    	if(vgl_mst.ccd_cnt<-2147000000) vgl_mst.ccd_cnt=0;
	    	if(vgl_mst.cdd_amt<-2147000000) vgl_mst.cdd_amt=0.0;
	    	if(vgl_mst.ccd_amt<-2147000000) vgl_mst.ccd_amt=0.0;
	    	if(vgl_mst.tddr_bal<-2147000000) vgl_mst.tddr_bal=0.0;
	    	if(vgl_mst.tdcr_bal<-2147000000) vgl_mst.tdcr_bal=0.0;
	    	if(vgl_mst.tddr_cnt<-2147000000) vgl_mst.tddr_cnt=0.0;
	    	if(vgl_mst.tdcr_cnt<-2147000000) vgl_mst.tdcr_cnt=0.0;
	    	if(vgl_mst.tddr_amt<-2147000000) vgl_mst.tddr_amt=0.0;
	    	if(vgl_mst.tdcr_amt<-2147000000) vgl_mst.tdcr_amt=0.0;
	    	if(vgl_mst.mdr_bal<-2147000000) vgl_mst.mdr_bal=0.0;
	    	if(vgl_mst.mcr_bal<-2147000000) vgl_mst.mcr_bal=0.0;
	    	if(vgl_mst.mdr_cnt<-2147000000) vgl_mst.mdr_cnt=0.0;
	    	if(vgl_mst.mcr_cnt<-2147000000) vgl_mst.mcr_cnt=0.0;
	    	if(vgl_mst.mdr_amt<-2147000000) vgl_mst.mdr_amt=0.0;
	    	if(vgl_mst.mcr_amt<-2147000000) vgl_mst.mcr_amt=0.0;
	    	if(vgl_mst.qdr_bal<-2147000000) vgl_mst.qdr_bal=0.0;
	    	if(vgl_mst.qcr_bal<-2147000000) vgl_mst.qcr_bal=0.0;
	    	if(vgl_mst.qdr_cnt<-2147000000) vgl_mst.qdr_cnt=0.0;
	    	if(vgl_mst.qcr_cnt<-2147000000) vgl_mst.qcr_cnt=0.0;
	    	if(vgl_mst.qdr_amt<-2147000000) vgl_mst.qdr_amt=0.0;
	    	if(vgl_mst.qcr_amt<-2147000000) vgl_mst.qcr_amt=0.0;
	    	if(vgl_mst.ydr_bal<-2147000000) vgl_mst.ydr_bal=0.0;
	    	if(vgl_mst.ycr_bal<-2147000000) vgl_mst.ycr_bal=0.0;
	    	if(vgl_mst.ydr_cnt<-2147000000) vgl_mst.ydr_cnt=0.0;
	    	if(vgl_mst.ycr_cnt<-2147000000) vgl_mst.ycr_cnt=0.0;
	    	if(vgl_mst.ydr_amt<-2147000000) vgl_mst.ydr_amt=0.0;
	    	if(vgl_mst.ycr_amt<-2147000000) vgl_mst.ycr_amt=0.0;
      
	    	fprintf( fp, "%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|\n",vgl_mst.br_no,vgl_mst.cur_no,cAcc_no,vgl_mst.date,vgl_mst.dr_bal,vgl_mst.cr_bal,vgl_mst.ldd_bal,vgl_mst.lcd_bal,vgl_mst.rdd_cnt,vgl_mst.rcd_cnt,vgl_mst.rdd_amt,vgl_mst.rcd_amt,vgl_mst.cdd_cnt,vgl_mst.ccd_cnt,vgl_mst.cdd_amt,vgl_mst.ccd_amt,vgl_mst.tddr_bal,vgl_mst.tdcr_bal,vgl_mst.tddr_cnt,vgl_mst.tdcr_cnt,vgl_mst.tddr_amt,vgl_mst.tdcr_amt,vgl_mst.mdr_bal,vgl_mst.mcr_bal,vgl_mst.mdr_cnt,vgl_mst.mcr_cnt,vgl_mst.mdr_amt,vgl_mst.mcr_amt,vgl_mst.qdr_bal,vgl_mst.qcr_bal,vgl_mst.qdr_cnt,vgl_mst.qcr_cnt,vgl_mst.qdr_amt,vgl_mst.qcr_amt,vgl_mst.ydr_bal,vgl_mst.ycr_bal,vgl_mst.ydr_cnt,vgl_mst.ycr_cnt,vgl_mst.ydr_amt,vgl_mst.ycr_amt);
	    	ttlnum++;
	    }
	    ret=Gl_mst_Clo_Sel( );
	    ERR_DEAL
  }
  ret=Com_item_Clo_Sel( );
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
