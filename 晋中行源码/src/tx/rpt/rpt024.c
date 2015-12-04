/***************************************************************
* �� �� ��: rpt024.c
* ��������: �������ͨ�ұ�����������ˮ��
*
* ��    ��: jack
* �������: 2003��8��23��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "chnl_self_trade_c.h"
#include "four_tradelog_c.h"

struct chnl_self_trade_c sChnl_self_trade;
struct four_tradelog_c sFour_tradelog;
struct com_branch_c sCom_branch;

int page = 0;
int total_line = 55;

double dD_amt = 0.00;	/*�跽�ܽ��*/
double dC_amt = 0.00;	/*�����ܽ��*/
int iD_count = 0;
int iC_count = 0; 

int get_ratio_bdt(int bli,int bll,char str[100]);
int get_ratio_bdt1(int bli,int bll,char str[100]);

rpt024()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Com_sys_parm_Sel(g_pub_tx.reply, &sCom_sys_parm, "1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯϵͳ���������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.tx_date = sCom_sys_parm.lst_date;

	/* ��ӡ�������ͨ�ұ�����������ˮ�嵥 */
	ret = cxyd_bdt_list();
	if (ret != 0)
	{
		sprintf(acErrMsg, "���ɴ������ͨ�ұ�����������ˮ��!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ������ͨ�ұ�����������ˮ�ʴ���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    strcpy(g_pub_tx.reply,"G009");
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"���ɴ������ͨ�ұ�����������ˮ�ʴ���ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int cxyd_bdt_list()
{
	int ret = 0; 
	int opncnt = 0; 
	FILE    *fp; 
	int     line;
	char fname[100]; 
	char filename[100];
	char vpfm[21];

	memset( vpfm, 0x0, sizeof(vpfm) );
	strcpy(vpfm, "cxydbdt");

	ret = Com_branch_Dec_Sel( g_pub_tx.reply, "wrk_sts != '*'" );
	if ( ret )
	{
		sprintf(acErrMsg, "ִ��Com_branch_Dec_Sel��[%d]", ret); 
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		line = 0;	/*ÿ������һ�ű���*/
		page = 1;
		
		memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
		ret = Com_branch_Fet_Sel( &sCom_branch, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf(acErrMsg, "ִ��Com_branch_Fet_Sel��[%d]", ret); 
			WRITEMSG
			goto ErrExit;
		}
		
		/*-----ATM,POS���ױ�����������¼-----*/
		ret = Chnl_self_trade_Dec_Sel( g_pub_tx.reply, "tx_date=%ld and \
			tx_br_no = '%s' and tx_code = '6203' and \
			info_source = '2' and stat = '0'", g_pub_tx.tx_date, \
			sCom_branch.br_no );
		if ( ret ) 
		{
			sprintf(acErrMsg, "ִ��Chnl_self_trade_Dec_Sel��[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		
		while(1)
		{
			memset( &sChnl_self_trade, 0x0, sizeof(sChnl_self_trade) );
			ret = Chnl_self_trade_Fet_Sel( &sChnl_self_trade, g_pub_tx.reply );
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
			
			dD_amt = dD_amt + sChnl_self_trade.tx_amt;
			iD_count = iD_count + 1;
			
			if ( line > total_line )	/*---��ҳ---*/
			{
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "DD", &opncnt,
					get_ratio_bdt) )
				goto ErrExit;
				
				fprintf(fp,"@HY@");
				page = page + 1;
				line = 0;
			}
			
			if ( line == 0 )
			{
				sprintf( filename, "BR%s", "cxydbdt" );
				
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename );
				
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt,
						get_ratio_bdt) )
				goto ErrExit;
				
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt,
						get_ratio_bdt) )
				goto ErrExit;
					
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt,
						get_ratio_bdt) )
				goto ErrExit;  
			}
			
			if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt,
					get_ratio_bdt) )
			goto ErrExit;
		}		
		
		Chnl_self_trade_Clo_Sel();
		
		/*----------����ҵ�񱾴���---------*/
		ret = Four_tradelog_Dec_Sel( g_pub_tx.reply, "tx_date=%ld and \
			sl_bank = '%s' and tx_code in ('7701','7702') and tx_stat = '0'", \
			g_pub_tx.tx_date, sCom_branch.br_no );
		if ( ret ) 
		{
			sprintf(acErrMsg, "ִ��Four_tradelog_Dec_Sel��[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		
		while(1)
		{
			memset( &sFour_tradelog, 0x0, sizeof(sFour_tradelog) );
			ret = Four_tradelog_Fet_Sel( &sFour_tradelog, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{ 
				sprintf(acErrMsg,"Four_tradelog_Fet_Sel ERROR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			
			pub_base_strpack( sFour_tradelog.tx_code );
			if ( strcmp( sFour_tradelog.tx_code, "7701" ) == 0 )
			{
				dC_amt = dC_amt + sFour_tradelog.tx_amt;
				iC_count = iC_count + 1;
			}
			else
			{
				dD_amt = dD_amt + sFour_tradelog.tx_amt;
				iD_count = iD_count + 1;
			}			
			
			if ( line > total_line )	/*---��ҳ---*/
			{
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "DD", &opncnt,
					get_ratio_bdt1) )
				goto ErrExit;
				
				fprintf(fp,"@HY@");
				page = page + 1;
				line = 0;
			}
			
			if ( line == 0 )
			{
				sprintf( filename, "BR%s", "cxydbdt" );
				
				pub_rpt_openfile( &fp, sCom_branch.br_no, filename );
				
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "HH", &opncnt,
						get_ratio_bdt1) )
				goto ErrExit;
				
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "AA", &opncnt,
						get_ratio_bdt1) )
				goto ErrExit;
					
				if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "BB", &opncnt,
						get_ratio_bdt1) )
				goto ErrExit;  
			}
			
			if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "CC", &opncnt,
					get_ratio_bdt1) )
			goto ErrExit;
		}		
		
		Four_tradelog_Clo_Sel(); 
		
		if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "DD", &opncnt,
					get_ratio_bdt1) )
		goto ErrExit;
				
		if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "EE", &opncnt,
					get_ratio_bdt1) )
		goto ErrExit;
		
		if( pub_rpt_read_pfm_qd( fp, &line, vpfm, "0001", "FF", &opncnt,
					get_ratio_bdt1) )
		goto ErrExit;
		
		fprintf(fp, "\n\n\n");
		fclose( fp );
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

int get_ratio_bdt( bli, bll,str )
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
			sprintf( fmt, "%%-%ds", bll ); 
			sprintf( str, fmt, sCom_branch.br_no ); 
			break; 
		case 'B': 
			sprintf( fmt, "%%-%ds", bll ); 
			sprintf( str, fmt, sCom_branch.br_name ); 
			break;
		case 'C': 
			sprintf( fmt, "%%%dld", bll );
			sprintf( vstr, fmt,  g_pub_tx.tx_date );
			strcpy( str, vstr ); 
			break; 
		case 'P': 
			sprintf(fmt,"%%-%dld",bll);
			sprintf( str, fmt, page ); 
			break; 
		case 'D': 
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, sChnl_self_trade.trace_no );
			break;	
		case 'E': 
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, sChnl_self_trade.ac_no ); 
			break; 
		case 'F':
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, "4301" ); 
			break; 
		case 'G': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, sChnl_self_trade.tx_time ); 
			break; 
		case 'H': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, sChnl_self_trade.tx_code ); 
			break; 
		case 'I': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, "��" );
			break; 
		case 'J': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, sChnl_self_trade.tx_amt ); 
			break; 		
		case 'K': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, sChnl_self_trade.ttyn ); 
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

int get_ratio_bdt1( bli, bll, str )
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
			sprintf( fmt, "%%-%ds", bll ); 
			sprintf( str, fmt, sCom_branch.br_no ); 
			break; 
		case 'B': 
			sprintf( fmt, "%%-%ds", bll ); 
			sprintf( str, fmt, sCom_branch.br_name ); 
			break;
		case 'C': 
			sprintf( fmt, "%%%dld", bll );
			sprintf( vstr, fmt,  g_pub_tx.tx_date );
			strcpy( str, vstr ); 
			break; 
		case 'P': 
			sprintf(fmt,"%%-%dld",bll);
			sprintf( str, fmt, page ); 
			break; 
		case 'D': 
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, sFour_tradelog.trace_no );
			break;	
		case 'E': 
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, sFour_tradelog.ac_no ); 
			break; 
		case 'F':
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, "4301" ); 
			break; 
		case 'G': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, sFour_tradelog.tx_time ); 
			break; 
		case 'H': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, sFour_tradelog.tx_code ); 
			break; 
		case 'I': 
			sprintf(fmt,"%%%ds",bll); 
			pub_base_strpack( sFour_tradelog.tx_code );
			if ( strcmp( sFour_tradelog.tx_code, "7701" ) == 0 )
			{
				sprintf( str, fmt, "��" );
			}
			else
			{
				sprintf( str, fmt, "��" );
			}
			break; 
		case 'J': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, sFour_tradelog.tx_amt ); 
			break; 		
		case 'K': 
			memset( str, ' ', bll ); 
			break; 			
		case 'L': 
			sprintf(fmt,"%%-%dd",bll); 
			sprintf( str, fmt, iD_count ); 
			break; 
		case 'M': 
			sprintf(fmt,"%%-%d.2lf",bll); 
			sprintf( str, fmt, dD_amt ); 
			break; 
		case 'N': 
			sprintf(fmt,"%%-%dd",bll); 
			sprintf( str, fmt, iC_count ); 
			break; 
		case 'O': 
			sprintf(fmt,"%%-%d.2lf",bll); 
			sprintf( str, fmt, dC_amt ); 
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
