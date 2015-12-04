/***************************************************************
* 文 件 名: rpt025.c
* 功能描述: 同城提入未处理清单
*
* 作    者: rob
* 完成日期: 2003年9月04日
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
#include "mo_city_c.h"

int page = 1;
struct mo_city_c sMo_city;

int get_ratio_mocity(int bli,int bll,char str[100]);
rpt025()
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

	/* 打印atm交易流水清单 */
	ret = mo_city_undeallist();
	if (ret != 0)
	{
		sprintf(acErrMsg, "打印同城提入未处理清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"打印同城提入未处理清单处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"打印同城提入未处理清单处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int mo_city_undeallist()
{
	int ret = 0; 
	int opncnt=0; 
	FILE    *fp; 
	int     line=0; 
	int  	total_line=55;
	char fname[100]; 
	char filename[100];
	char vpfm[21]; 
	int first=0;
	struct com_branch_c sCom_branch;
	
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));

	strcpy(vpfm, "mocity");
	pub_rpt_rmfile( "" , "BRmocity" , 0 );

sprintf(acErrMsg, "tx_date=%ld", g_pub_tx.tx_date);
WRITEMSG

	ret = Com_branch_Dec_Sel(g_pub_tx.reply, "1=1 order by br_no");
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE COM_BRANCH ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
		ret = Com_branch_Fet_Sel(&sCom_branch, g_pub_tx.reply);
		if (ret == 100)
		{
			break;
		}
		else if ( ret )
		{
			sprintf(acErrMsg, "FETCH COM_BRANCH ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack(sCom_branch.br_no);
		/* 提入未处理清单 */
		ret = Mo_city_Dec_Sel(g_pub_tx.reply, "sts in ('I' , 'C') and \
			(payer_br_no='%s' or payee_br_no='%s')", sCom_branch.br_no,
			sCom_branch.br_no);
		if (ret != 0) 
		{ 
			sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
	
		first=0;
		line = 0;
		page = 0;
		while(1) 
		{ 
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Fet_Sel(&sMo_city, g_pub_tx.reply); 
			if (ret == 100) 
			{ 
				if (first != 0)
				{
					pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
					if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit;
					fclose(fp);
				}
				break; 
			}
			else if ( ret ) 
			{ 
				sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
				WRITEMSG 
				goto ErrExit; 
			} 

			if( first==0 )
			{
				page=1;
				sprintf( filename,"%s","BRmocity");
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 

				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit; 
		
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit; 
				fclose(fp);
			}
			first = first + 1;
		
			sprintf( filename,"%s","BRmocity");
	
			if (line > total_line)
			{
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit;
				fprintf(fp,"@HY@");
				page = page + 1;
				line = 0;
				fclose(fp);
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 

				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_mocity) ) 
				goto ErrExit; 
		
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit; 
				fclose(fp);
			}
			pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
					get_ratio_mocity) ) 
				goto ErrExit; 
			fclose(fp);
		} 
		Mo_city_Clo_Sel();
		
		if ( first != 0 )
		{
			pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
			fprintf(fp,"@HY@");
			fclose(fp);
		}

		page = 1;
		line = 0;

		/* 提出未处理清单 */
		ret = Mo_city_Dec_Sel(g_pub_tx.reply, "sts in ('6' , '1') and \
			tx_br_no='%s' order by tel", sCom_branch.br_no);
		if (ret != 0) 
		{ 
			sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
	
		first=0;
		while(1) 
		{ 
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Fet_Sel(&sMo_city, g_pub_tx.reply); 
			if (ret == 100) 
			{ 
				if (first != 0)
				{
					pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
					if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit;
					fclose(fp);
				}
				break; 
			}
			else if ( ret ) 
			{ 
				sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
				WRITEMSG 
				goto ErrExit; 
			} 

			if( first==0 )
			{
				sprintf( filename,"%s","BRmocity");
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 

				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","FF",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit; 
		
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit; 
				fclose(fp);
			}
			first = first + 1;
		
			sprintf( filename,"%s","BRmocity");
	
			if (line > total_line)
			{
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit;
				fprintf(fp,"@HY@");
				page = page + 1;
				line = 0;
				fclose(fp);
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 

				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","FF",&opncnt, 
					get_ratio_mocity) ) 
				goto ErrExit; 
		
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
						get_ratio_mocity) ) 
					goto ErrExit; 
				fclose(fp);
			}
			pub_rpt_openfile( &fp, sCom_branch.br_no, filename); 
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
					get_ratio_mocity) ) 
				goto ErrExit; 
			fclose(fp);
		} 
		Mo_city_Clo_Sel();
	}
	Com_branch_Clo_Sel();
	return 0;
ErrExit:
	return 1;
}

int get_ratio_mocity( bli,bll,str )
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
		case 'A': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( vstr,fmt, sMo_city.tx_date);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sMo_city.payer_ac_no);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sMo_city.payer_name);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sMo_city.payer_br_name); 
			break; 
		case 'E': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sMo_city.payee_ac_no); 
			break; 
		case 'F':
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sMo_city.payee_name); 
			break; 
		case 'G':
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sMo_city.payee_br_name); 
			break; 
		case 'H': 
			sprintf(fmt, "%%%d.2lf", bll); 
			sprintf(str, fmt, sMo_city.bill_amt); 
			break;
		case '1': 
			sprintf( str, "%08ld", g_pub_tx.tx_date); 
			break; 
		case '2': 
			sprintf( str, "%02ld", page); 
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
