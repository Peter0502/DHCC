/***************************************************************
* 文 件 名: rpt030.c
* 功能描述: 电信缴费对帐文件
*
* 作    者: jack
* 完成日期: 2003年09月27日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "ag_paytlcs_c.h"

struct ag_paytlcs_c sAg_paytlcs;
struct com_branch_c sCom_branch;

int page = 0;
int total_line = 55;

double dTram = 0.00;	/*缴费总金额*/
long lCount = 0;	/*缴费总笔数*/

int get_ratio_dxtj(int bli,int bll,char str[100]);

rpt030()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Com_sys_parm_Sel(g_pub_tx.reply, &sCom_sys_parm, "1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询系统参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.tx_date = sCom_sys_parm.lst_date;

	/* 打印储蓄电信代缴话费*/
	ret = dxdz_list();
	if (ret != 0)
	{
		sprintf(acErrMsg, "生成电信代缴话费对帐清单!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成电信代缴话费对帐清单处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    strcpy(g_pub_tx.reply,"G009");
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"生成电信代缴话费对帐清单处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int dxdz_list()
{
	int ret = 0;
	int opncnt = 0; 
	FILE    *fp; 
	int     line=0;
	char fname[100]; 
	char filename[100];
	char vpfm[21];
	
	long lDate = 0;
	 
	int i = 0;
	char cOgcd[7];
	
	int iFlag = 0;		/*---记录是否处理完毕标志---*/
	
	memset( cOgcd, 0x0, sizeof(cOgcd) );

	memset( vpfm, 0x0, sizeof(vpfm) );
	strcpy(vpfm, "dxdzlist");
	
	sprintf( filename, "BR%s", "dxdzlist" );

	pub_rpt_openfile( &fp, "61080", filename );	/*报表放在61080国旺营业部*/

	ret = Ag_paytlcs_Dec_Sel( g_pub_tx.reply, "trn_date = %ld and unit_no = '1250' \
		and stat = '0' order by item_no ", g_pub_tx.tx_date );
	if ( ret ) 
	{
		sprintf( acErrMsg, "执行Ag_paytlcs_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		memset( &sAg_paytlcs, 0x0, sizeof(sAg_paytlcs) );
		ret = Ag_paytlcs_Fet_Sel( &sAg_paytlcs, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"FETCH FOR SEL self_trade ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		if ( line >= total_line )	/*---换页---*/
		{
			if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "DD", &opncnt,
				get_ratio_dxtj) )
			goto ErrExit;
			
			fprintf(fp,"@HY@");
			page = page + 1;
			line = 0;
		}
		
		if ( line == 0 )
		{			
			if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt,
					get_ratio_dxtj) )
			goto ErrExit;
			
			if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt,
					get_ratio_dxtj) )
			goto ErrExit;

			if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt,
					get_ratio_dxtj) )
			goto ErrExit;
		}
		
		if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt,
				get_ratio_dxtj) )
		goto ErrExit;
			
		if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "DD", &opncnt,
				get_ratio_dxtj) )
		goto ErrExit;
		
		lCount = lCount + 1;
		dTram = dTram + sAg_paytlcs.trn_amt;					
	}
	
	Ag_paytlcs_Clo_Sel();

	if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "EE", &opncnt,
				get_ratio_dxtj) )
	goto ErrExit;
	
	if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "FF", &opncnt,
				get_ratio_dxtj) )
	goto ErrExit;
	
	fprintf(fp, "\n\n\n");
	fclose( fp );

OkExit:
	return 0;
ErrExit:
	fclose( fp );
	return 1;
}

int get_ratio_dxtj( bli, bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0;
	
	switch( bli ) 
	{ 
		case '1':
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, g_pub_tx.tx_date );
			break;
		case 'A':
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, sAg_paytlcs.bank_tlcs );
			break;
		case 'B':
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str, fmt, sAg_paytlcs.item_no );
			break;
		case 'C':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, sAg_paytlcs.trn_amt );
			break;
		case 'D':
			sprintf(fmt,"%%-%dld",bll);
			sprintf( str, fmt, g_pub_tx.tx_date );
			break;
		case 'E':
			sprintf( fmt, "%%-%dld", bll );
			sprintf( str, fmt, lCount ); 
			break;
		case 'F':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, dTram ); 
			break;
		case 'G':
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str, fmt, sAg_paytlcs.br_no );
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
