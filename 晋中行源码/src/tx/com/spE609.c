/*************************************************
* 文 件 名:  spE609.c
* 功能描述： 本日轧账(凭证)
*
* 作    者:  andy
* 完成日期： 2004-07-06
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
#include "com_tel_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"


int spE609()
{
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		
		char tel[5];
		char old_note_type[4];
		char note_name[21];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;

		long in_cnt=0,out_cnt=0;
		long ye_cnt=0,now_cnt=0;
							
		FILE *fp;

	struct com_tel_c	v_com_tel;
	struct note_mst_hst_c	v_note_mst_hst;
		
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
	memset( &v_note_mst_hst , 0x00 , sizeof(struct note_mst_hst_c));
		
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );

	strcpy( old_note_type , "***" );
	
	pub_base_sysinit();

	get_zd_data("0910",tel );

	/**组成查询条件**/
	pub_base_strpack( tel );
	if( strlen(tel) )
	{
		/* 检查柜员号是否存在 */
		ret = Com_tel_Sel(g_pub_tx.reply,&v_com_tel,"tel='%s'",tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"此柜员号不存在![%s]",tel);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O120" );			
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"读取柜员信息表异常出错!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}									
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
	}
	
	/* 机构编码 */
	sprintf( tmpstr," br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	/* 交易日期 */
	sprintf( tmpstr," tx_date=%ld and",g_pub_tx.tx_date );
	strcat( wherelist,tmpstr );
	strcat( wherelist," (((sts_ind='2' or sts_ind='3') and br_no!=tw_br_no) or (sts_ind !='2' and sts_ind!='3')) and " );/*jinyw add为了不统计凭证柜员调剂*/
		
	/**组成查询**/
	strcat(wherelist," 1=1  ORDER BY note_type ") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Note_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
		
	while(1)
	{
		ret=Note_mst_hst_Fet_Sel( &v_note_mst_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 查出凭证名称 */					
				ret = pub_base_get_note_name( old_note_type, note_name );	
				ERR_DEAL
				sprintf(acErrMsg,"note_name=%s",note_name);
				WRITEMSG
				
				pub_base_strpack( note_name );	
		
				/* 统计该种凭证昨日数量和现数量 */
				pub_base_strpack( tel );
				if( strlen(tel) )	/* 柜员号非空 */
				{
					ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , 
                          "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , 
                          g_pub_tx.tx_br_no,old_note_type,tel);
					ERR_DEAL					
					ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , 
                          "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , 
                          g_pub_tx.tx_br_no,old_note_type,tel);
					ERR_DEAL							
				}else{			/* 柜员号为空 */
					ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , 
					      "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , 
                          g_pub_tx.tx_br_no,old_note_type);
					ERR_DEAL					
					ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , 
                          "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , 
                          g_pub_tx.tx_br_no,old_note_type);
					ERR_DEAL
                    vtcp_log(">>>>>>>>>>>>>>>>>>>>>>今日余额:[%d]",now_cnt);
				}
			
				/**显示列标题**/	
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`           凭证编码: 【%3s】\n",old_note_type);
				fprintf( fp,"`           凭证名称: 【%s】\n",note_name);
				fprintf( fp,"`           ┍━━━━━┯━━━━━┯━━━━━┯━━━━━┑\n" );
				fprintf( fp,"`           │ 昨日数目 │ 本日收入 │ 本日付出 │ 本日剩余 │\n" );			
				fprintf( fp,"`           ┝━━━━━┿━━━━━┿━━━━━┿━━━━━┥\n" );			
				fprintf( fp,"`           │%10ld│%10ld│%10ld│%10ld│\n" ,ye_cnt,in_cnt,out_cnt,now_cnt);			
				fprintf( fp,"`           ┕━━━━━┷━━━━━┷━━━━━┷━━━━━┙\n" );	
				fprintf( fp,"`\n" );					
				fprintf( fp,"`\n" );
								
				/* 数据清零 */
				in_cnt=0;
				out_cnt=0;
				ye_cnt=0;
				now_cnt=0;		
			}
			
			break;		
		}else if(ret){
			goto ErrExit;
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
		}
		TRACE
		
		/* 第一条记录 */
		if( strcmp( old_note_type , "***")==0 )
		{
			TRACE
			strcpy( old_note_type, v_note_mst_hst.note_type );
		}		
		
		
		if( strcmp( v_note_mst_hst.note_type , old_note_type ) )	/* 凭证类型变化时 */
		{
			/* 查出凭证名称 */					
			ret = pub_base_get_note_name( old_note_type, note_name );	
			ERR_DEAL
			sprintf(acErrMsg,"note_name=%s",note_name);
			WRITEMSG
			
			pub_base_strpack( note_name );	
	
			/* 统计该种凭证昨日数量和现数量 */
			pub_base_strpack( tel );
			if( strlen(tel) )	/* 柜员号非空 */
			{
				ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , g_pub_tx.tx_br_no,old_note_type,tel);
				ERR_DEAL					
				ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , g_pub_tx.tx_br_no,old_note_type,tel);
				ERR_DEAL			
			}else{			/* 柜员号为空 */
				ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , g_pub_tx.tx_br_no,old_note_type);
				ERR_DEAL					
				ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , g_pub_tx.tx_br_no,old_note_type);
				ERR_DEAL		
			}
		
			/**显示列标题**/	
			fprintf( fp,"@3v1\n");
			fprintf( fp,"`           凭证编码: 【%3s】\n",old_note_type);
			fprintf( fp,"`           凭证名称: 【%s】\n",note_name);
			fprintf( fp,"`           ┍━━━━━┯━━━━━┯━━━━━┯━━━━━┑\n" );
			fprintf( fp,"`           │ 昨日数目 │ 本日收入 │ 本日付出 │ 本日剩余 │\n" );			
			fprintf( fp,"`           ┝━━━━━┿━━━━━┿━━━━━┿━━━━━┥\n" );			
			fprintf( fp,"`           │%10ld│%10ld│%10ld│%10ld│\n" ,ye_cnt,in_cnt,out_cnt,now_cnt);			
			fprintf( fp,"`           ┕━━━━━┷━━━━━┷━━━━━┷━━━━━┙\n" );	
			fprintf( fp,"`\n" );					
			fprintf( fp,"`\n" );
						
			/* 数据清零 */
			in_cnt=0;
			out_cnt=0;
			ye_cnt=0;
			now_cnt=0;
			strcpy( old_note_type , v_note_mst_hst.note_type );	/* 凭证类型 */ 			
		} 
		
		
		/* 统计信息 */
		if( v_note_mst_hst.note_cnt )
		{
			/* 1-入库 3-领入 5-凭证转移转入 9-正常收缴 F-出售收回 */
			if( v_note_mst_hst.sts_ind[0]=='1' || v_note_mst_hst.sts_ind[0]=='3' || 
				v_note_mst_hst.sts_ind[0]=='5' || v_note_mst_hst.sts_ind[0]=='9' ||
					v_note_mst_hst.sts_ind[0]=='F')		
			{
				in_cnt+=v_note_mst_hst.note_cnt;	
			}
			
			/* 2-下发 4-凭证转移转出 6-出售 7-正常上缴 D-使用 E-作废 */
			if( v_note_mst_hst.sts_ind[0]=='2' || v_note_mst_hst.sts_ind[0]=='4' || 
				v_note_mst_hst.sts_ind[0]=='6' || v_note_mst_hst.sts_ind[0]=='7' ||
					v_note_mst_hst.sts_ind[0]=='D' || v_note_mst_hst.sts_ind[0]=='E' )			
			{
				out_cnt+=v_note_mst_hst.note_cnt;
			}
		}
		
		ttlnum++;
	}

	ret=Note_mst_hst_Clo_Sel( );
	ERR_DEAL
TRACE
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
