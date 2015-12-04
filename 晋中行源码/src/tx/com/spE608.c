/*************************************************
* 文 件 名:  spE608.c
* 功能描述： 本日轧账(现金)
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
#include "cash_mst_c.h"

int spE608()
{

	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	
	char tel[5];
	int ret=0;
	
	double in_amt=0.00,out_amt=0.00,tot_amt=0.00,ys_tot_amt=0.00;
						
	FILE *fp;
	
	struct com_tel_c	v_com_tel;
	struct cash_mst_c	v_cash_mst;
	
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));			
	memset( &v_cash_mst , 0x00 , sizeof(struct cash_mst_c));	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );

	
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
		
	/* 机构编码 */
	sprintf( tmpstr," br_no='%s' and cur_no='01' and ",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	
	/**组成查询**/
	strcat(wherelist," sts='0' ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Cash_mst_Sel( g_pub_tx.reply , &v_cash_mst , wherelist );
	if( ret ==100 )
	{
		sprintf(acErrMsg,"此柜员无可用钱箱!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D189" );			
		goto ErrExit;		
	}else if( ret ){
		sprintf(acErrMsg,"查询现金主文件异常出错,请与中心机房联系!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D190" );			
		goto ErrExit;	
	}
	tot_amt    = v_cash_mst.bal;	
	ys_tot_amt = v_cash_mst.ys_bal;	
}
else{/*如果柜员号为空,则取所有柜员钱箱余额为机构的现金余额JYW*/
	sprintf( tmpstr," br_no='%s' and cur_no='01' and  ",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );
  strcat( wherelist,"  sts='0' ");
	
	/**组成查询**/
	
	vtcp_log( "WHERE [%s]",wherelist );	
	ret=sql_sum_double( "cash_mst","bal",&tot_amt,wherelist);
	ret=sql_sum_double( "cash_mst","ys_bal",&ys_tot_amt,wherelist);
	vtcp_log("[%s][%d], wherelist = [%s] ,ys_bal=[%.2f]\n",__FILE__,__LINE__,wherelist,ys_tot_amt);
  ERR_DEAL
}	
	
	/* 统计本日现金收入付出金额 */
	pub_base_strpack( tel );
	if( strlen(tel) )	/* 柜员号非空 */
	{	
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &in_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='1' and tel='%s'" , g_pub_tx.tx_date,g_pub_tx.tx_br_no,tel);
		ERR_DEAL				
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &out_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='0' and tel='%s'" , g_pub_tx.tx_date,g_pub_tx.tx_br_no,tel);			
		ERR_DEAL			
	}else{			/* 柜员号为空 */
	
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &in_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='1' " , g_pub_tx.tx_date,g_pub_tx.tx_br_no);
		ERR_DEAL				
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &out_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='0' " , g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
		ERR_DEAL
	}	
	
	/* 写显示文件 */
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fprintf( fp,"@3v1");
	fprintf( fp,"`\n" );	
	fprintf( fp,"`\n" );
	fprintf( fp,"`                            【本日现金轧差信息】\n" );
	fprintf( fp,"`\n" );	
	fprintf( fp,"` ┍━━━━━━━━┯━━━━━━━━┯━━━━━━━━┯━━━━━━━━┑\n" );
	fprintf( fp,"` │   昨 日 余 额  │   本 日 收 入  │   本 日 付 出  │   本 日 余 额  │\n" );			
	fprintf( fp,"` ┝━━━━━━━━┿━━━━━━━━┿━━━━━━━━┿━━━━━━━━┥\n" );			
	fprintf( fp,"` │%16.2lf│%16.2lf│%16.2lf│%16.2lf│\n" ,ys_tot_amt,in_amt,out_amt,tot_amt);			
	fprintf( fp,"` ┕━━━━━━━━┷━━━━━━━━┷━━━━━━━━┷━━━━━━━━┙\n" );	
	fprintf( fp,"`\n" );		
	fprintf( fp,"`\n" );
	
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
