/*************************************************************
* 文 件 名: rpt193.c
* 功能描述：承兑汇票发生日报表, 帐户资金大进大出明细
*
* 作    者: rob
* 完成日期: 2003年09月13日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "mo_bank_acc_po_c.h"
#define _SQLTOOL_INCL
#include "sqltool.h"
struct trace_log_c	s_trace_log;
struct mo_bank_acc_po_c	s_mo_bank_acc_po;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

/* 承兑汇票发生日报表 */
rpt193()
{
	int flag=0;

	char fname[100];
	char filename[100];
	char SQL[1024];  /**查询**/
	char tmpstr[100];
	char tmp_str[60];
	double sum_1=0, sum_2=0;   /* 出票总额, 收回总额 */

	memset( SQL,0,sizeof(SQL) );
	memset( &s_trace_log , 0x00 , sizeof(struct trace_log_c));
	
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
/*
   sprintf( acErrMsg, "打开败!!!    " );
	  strrtrim(acErrMsg, acErrMsg); 
	  strcpy(g_pub_tx.reply,"0001");
	  WRITEMSG
	goto ErrExit ;
*/
	strcpy( filename , "BRcdhpfsrb" );	
	strcpy(vpfm, "cdhpfsrb" );
	
	/* 条件: 票据类型=111银行承兑汇票 */
	
	strcpy( SQL,"select A.tx_br_no _08[交易机构], A.ac_no _14[合同号], B.payer_name [承兑单位], B.payer_ac_no [付款帐号], \
	 a.amt*decode(add_ind,'1',1, -1)  _1692[发生额],  req_date [出票日], matr_date [到期日], bail_amt _1492[保证金] \
	 from trace_log A,  mo_bank_acc_po B where A.note_type='111' and A.sts='0' and A.amt<>0.0 and A.ac_no=B.pact_no ");

	/**排序**/	
	strcat(SQL," ORDER BY tx_br_no,opn_br_no,trace_no ");

	vtcp_log( "--*--SQL [%s]",SQL );
	
	page=1;
	line_file=1;
	
	/* 删除原文件 */
	ret = pub_rpt_rmfile("99999" , filename , 0 );
	ERR_DEAL
	/* 打开文件 */	
	ret = pub_rpt_openfile( &fp, "99999" , filename );
	ERR_DEAL
	/* 打印页首 */
        fprintf(fp, "%30s%s承兑汇票发生日报表\n"," ", BANK_CNAME);
        sprintf( tmp_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
        fprintf(fp,"    记帐日期:%s%60s    单位：元\n", tmp_str);
        /* 打印表内容 */ 
/*	wprt_txt(SQL, 0, fp, 0, 0," ",'|',0, NULL ); */
	 wprt_hzcell( SQL,  0, fp, 50, "", 5, 1,35065, NULL,NULL,NULL,NULL);
	fclose(fp);

     rpt193_dd1( SQL )  ;    /*  生成帐户资金大进大出明细  */
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成承兑汇票发生日报表等成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成承兑汇票发生日报表等失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*  帐户资金大进大出明细  
*    
*   作    者: rob
*   完成日期: 2003年09月13日
*/

int rpt193_dd1_ftop(FILE * fprt, int pg )     /*  打页头  */
{
    	page = atoi( sql_Field (0)->f_char ) ;     /* 保存管理支行 */
    	if (fprt != NULL) fclose(fprt);
	/* 删除原文件 */
	ret = pub_rpt_rmfile(sql_Field (0)->f_char , "BRdjdcmx" , 0 );
	if (ret) goto ErrExit1 ;
	/* 打开文件 */	
	ret = pub_rpt_openfile( &fprt, sql_Field (0)->f_char , "BRdjdcmx" );
	if (ret) goto ErrExit1 ;
  sprintf( acErrMsg, "====生成%d文件名BRdjdcmx !!!", page );
  WRITEMSG

        fprintf(fprt, "%20s%s帐户资金大进大出明细(%d)\n", " ", BANK_CNAME, page );   
  	return 0;      
  	ErrExit1:
  sprintf( acErrMsg, "====打开%d文件名错误!!!", page );
  WRITEMSG
  	return -1; 
}

int rpt193_dd1_fnewpg(int r, int i )     /*  换页  */
{
	return (atoi( sql_Field (0)->f_char ) != page) ;     /* 管理支行变化  */
	
}

int rpt193_dd1( char *SQL )
{
      /*----- (总行) 对公进出>=50万, 储蓄>=30万 */
  	/* 删除原文件 */
	ret = pub_rpt_rmfile("99999" , "BRdjdcmx" , 0 );
	if (ret) goto ErrExit1 ;
	/* 打开文件 */	
	ret = pub_rpt_openfile( &fp, "99999" , "BRdjdcmx" );
	if (ret) goto ErrExit1 ;
	/* 打印页首 */
        fprintf(fp, "%20s%s帐户资金大进大出明细(总行)\n"," ", BANK_CNAME);
        strcpy(SQL," select b.up_br_no [管理支行], opn_br_no [开户行],prdt_no [产品编号], name [帐户名], (bal - ys_bal)/10000 [比上日增减] \
  	   from dd_mst a  join  com_branch b on a.opn_br_no = b.br_no  \
  	   where abs(bal - ys_bal)>=500000 or (prdt_no<'109' and abs(bal - ys_bal)>=300000)  order by 1,2");
        wprt_hzcell( SQL,  0, fp, 50, "", 5, 1,85065, NULL,NULL,NULL,NULL);
        fclose(fp);
      /*----- (管理支行) 对公进出>=10万, 储蓄>=5万  */
       fp=NULL;  
       page=0;
       strcpy(SQL," select b.up_br_no [管理支行], opn_br_no [开户行],prdt_no [产品编号], name [帐户名], (bal - ys_bal)/10000 [比上日增减] \
  	   from dd_mst a  join  com_branch b on a.opn_br_no = b.br_no  \
  	   where abs(bal - ys_bal)>=100000 or (prdt_no<'109' and abs(bal - ys_bal)>=50000)  order by 1,2");
       wprt_hzcell( SQL,  0, fp, 50, "", 5, 1,85065, NULL, rpt193_dd1_fnewpg , rpt193_dd1_ftop ,NULL);
       fclose(fp);
  return 0;      
  ErrExit1:
  sprintf( acErrMsg, "====打开文件名错误!!!" );
  WRITEMSG
  return -1;
}

