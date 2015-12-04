/*************************************************
* 文 件 名:  sp9338.c
* 功能描述： 活期账户明细查询(用于晋中电话银行查卡交易明细)
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
#include "dd_mst_hst_c.h"

int sp9338()
{
	struct dd_mst_hst_c vdd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	
	char add_ind[3];
	char ct_ind[5];
	char ac_no3[25];
	char f_note_type9[41];
	char f_acno0[25];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char current_page[26];
		int ttlnum=0;			/**读取的总条数**/
		int page_counter=0;
		int ret=0;
		FILE *fp;
		char tmp_date[9];
		char tmp_time[7];
	memset(current_page,0x00,sizeof(current_page));
	memset(tmp_date,0x00,sizeof(tmp_date));
	memset(tmp_time,0x00,sizeof(tmp_time));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
	memset( &f_dd_mst_hst,0,sizeof(f_dd_mst_hst));
	memset( &f_dd_mst_hsta,0,sizeof(f_dd_mst_hsta));
    
    
	get_zd_data("0380",f_acno0);
  pub_base_old_acno(f_acno0);
	get_zd_long("0290",&f_dd_mst_hst.tx_date ); 
	vtcp_log( "%s,%d,begin_date [%ld]",__FILE__,__LINE__,f_dd_mst_hst.tx_date); 
	get_zd_long("0450",&f_dd_mst_hsta.tx_date );
	vtcp_log( "%s,%d,end_date [%ld]",__FILE__,__LINE__,f_dd_mst_hsta.tx_date ); 
  get_zd_data("0300",current_page);
  	vtcp_log( "%s,%d,current_page [%s]",__FILE__,__LINE__,current_page); 

/**添加密码交易-wangwk-20101229**/
 /*  ret=iCheckPasswd();
   if(ret)
   	{
   		goto ErrExit;
   	}*/
   /**添加结束**/
	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_dd_mst_hst.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_dd_mst_hst.ac_id );
		strcat( wherelist,tmpstr );
	}
	

	if( f_dd_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_dd_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_dd_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"dd_mst_hst" );

	strcat(wherelist," 1=1 ORDER BY tx_date desc,trace_no desc,trace_cnt,tx_time desc");/*modfiy by ChengGX 20100111*/
  vtcp_log( "WHERE [%s]",wherelist );
  ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
  
  ttlnum=0;
  
	while(1)
	{
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log( "%s,%d,FILENAME [%s]",__FILE__,__LINE__,filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
  					vtcp_log( "第一次进入！"); 

		}
		  ttlnum++;
		if(((ttlnum/5+1)==atoi(current_page))&&(ttlnum%5!=0)||((ttlnum%5==0)&&ttlnum/5==atoi(current_page)))
			{
        vtcp_log( "%s,%d, 第%d条记录brf[%s]",__FILE__,__LINE__,ttlnum,vdd_mst_hst.brf);
        vtcp_log( "%s,%d, 第%d条记录trace_no[%ld]",__FILE__,__LINE__,ttlnum,vdd_mst_hst.trace_no);
        fprintf(fp, "%ld|%.2lf|%.2lf|%s|%ld|%ld|\n",vdd_mst_hst.tx_date,vdd_mst_hst.tx_amt,vdd_mst_hst.bal,vdd_mst_hst.brf,vdd_mst_hst.tx_time,vdd_mst_hst.trace_no);
       }   
	}
	page_counter=ttlnum/5;
	if(ttlnum%5!=0)
	{
		page_counter+=1;
	}

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fprintf(fp, "%s|%d|\n","总页数",page_counter);
  	vtcp_log( "%s,%d, 总记录数[%d]",__FILE__,__LINE__,ttlnum);
		vtcp_log( "%s,%d, 总页数[%d]",__FILE__,__LINE__,page_counter);
	}
	if(page_counter<atoi(current_page))
	{		
		sprintf(acErrMsg,"无当前页");
		WRITEMSG
		strcpy( g_pub_tx.reply,"DH01" );
		goto ErrExit;
	}
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
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
int iCheckPasswd()
{
	int ret = 0;
	struct mdm_ac_rel_c st_mdm_ac_rel;
	memset(&st_mdm_ac_rel, 0, sizeof(st_mdm_ac_rel));
	get_zd_data("0380",st_mdm_ac_rel.ac_no );		/*折子号码*/
	vtcp_log( "%s,%d===折子号码[%s]",__FILE__,__LINE__,st_mdm_ac_rel.ac_no);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		return -1;
	}
	get_zd_data("0790",g_pub_tx.draw_pwd);
	/*vtcp_log( "%s,%d===g_pub_tx.draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd_yn[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
	if(g_pub_tx.draw_pwd_yn[0] == 'Y' && 
		 strcmp(st_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
		{
          sprintf(acErrMsg, "密码错误!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "O123");
          return -1;
    }
   return 0;
}
