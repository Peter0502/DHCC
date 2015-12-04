/*************************************************************
* �� �� ��: rpt983.c
* �������������ڻ����ֽ���֧�±���
*
* ��    ��: 
* �������: 2003��08��15��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define UNIT_AMT 1000000
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "mo_xjszybb_c.h"
#include "correl_brf_c.h"
#define PAGE_CNT 43

struct com_sys_parm_c 		s_com_sys_parm;
struct mo_xjszybb_c	s_mo_xjszybb;
struct correl_brf_c			s_correl_brf;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
int cnt = 1;
int target_cnt=1;
double lst_amt=0.00;
double now_amt=0.00;
long lst_date;

int get_ratio_lsqd(int bli,int bll,char str[100]);

char CAL_BR_NO[6];

rpt983( )
{
	int ttlnum;
	char   filename[100];
	long   tmp_date;

	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
    
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	vtcp_log("AAAAAAAAAAAAAAAAAAAAAAA");
	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
	tmp_date = g_pub_tx.tx_date/100*100 + 1 ; 
	ret = pub_base_deadlineD( tmp_date, -1, &lst_date );
	ERR_DEAL
	lst_date = lst_date/100 ;

	tmp_date = g_pub_tx.tx_date/100;

	vtcp_log("CCCCCCCCCCCCCCCCCCCCCCC------------->INSPECT-->[%ld]",lst_date);
	vtcp_log("CCCCCCCCCCCCCCCCCCCCCCC------------->INSPECT-->[%ld]",tmp_date);
	strcpy( filename , "BMjrjjxjszybb" );	
	strcpy( vpfm, "jrjjxjszybb" );
	strcpy( CAL_BR_NO,"61000" );

	ttlnum=0;

	ret = Correl_brf_Dec_Sel( g_pub_tx.reply, " 1=1 " );
	ERR_DEAL

	while(1)
	{
		ret = Correl_brf_Fet_Sel( &s_correl_brf,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ�ļ�¼!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}

		/*** ȡ���½���� ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &lst_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, lst_date );
		ERR_DEAL

		/*** ȡ���½���� ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &now_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, tmp_date );
		ERR_DEAL

		if( !ttlnum )
		{
			page=1;
			line_file=1;

			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( CAL_BR_NO , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, CAL_BR_NO , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL
		}

		/* ��ӡ��Ϣ */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		if( line_file>PAGE_CNT )
		{
			line_file=0;
			page++;
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}
	Correl_brf_Clo_Sel( );
	fclose(fp);

	vtcp_log("CCCCCCCCCCCCCCCCCCCCCCC------------->INSPECT-->[%ld]",tmp_date);
	strcpy( filename , "BMjrjjxjszybb" );	
	strcpy( vpfm, "jrjjxjszybb" );
	strcpy( CAL_BR_NO,"61027" );

	ttlnum=0;

	ret = Correl_brf_Dec_Sel( g_pub_tx.reply, " 1=1 " );
	ERR_DEAL

	while(1)
	{
		ret = Correl_brf_Fet_Sel( &s_correl_brf,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ�ļ�¼!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}

		/*** ȡ���½���� ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &lst_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, lst_date );
		ERR_DEAL

		/*** ȡ���½���� ***/
		ret = sql_sum_double ( "mo_xjszybb", "amt", &now_amt, 
			"br_no='%s' and target_no='%s' and tx_date=%ld", 
			CAL_BR_NO,s_correl_brf.target_no, tmp_date );
		ERR_DEAL

		if( !ttlnum )
		{
			page=1;
			line_file=1;

			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( CAL_BR_NO , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, CAL_BR_NO , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL
		}

		/* ��ӡ��Ϣ */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		if( line_file>PAGE_CNT )
		{
			line_file=0;
			page++;
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}
	Correl_brf_Clo_Sel( );
	fclose(fp);
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ڻ����ֽ���֧�±�����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ڻ����ֽ���֧�±�����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ӡ���� */
int print_head( )
{
	char vrec[3];
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ��β */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ���� */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ֵ���� */
/* ˵����bli ��������ַ����� ... bll�������ռ�ĳ��� ... str�������ֵ */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	char br_name[71];
	int i=0; 
	long long_amt;

	struct correl_brf_c x_correl_brf;

	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , s_com_sys_parm.sys_date/10000 , s_com_sys_parm.sys_date%10000/100 , s_com_sys_parm.sys_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* �������� */
			ret = pub_base_getbrname( CAL_BR_NO, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" ,br_name,CAL_BR_NO ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'F': /* λ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, cnt++); 
			break; 
		case 'G': /* Ƶ�� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , "��");
			break;
		case 'H': /* ������ */
			vtcp_log("lst_amt---------------[%lf]",lst_amt);
			long_amt = pub_base_PubDround_1( lst_amt / UNIT_AMT ) * 100;
			sprintf( amt_tmp ,"%ld", long_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* ������ */
			vtcp_log("now_amt---------------[%lf]",now_amt);
			long_amt = pub_base_PubDround_1( now_amt / UNIT_AMT ) * 100;
			sprintf( amt_tmp ,"%ld", long_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* ָ����� */
			sprintf( amt_tmp, " %04d ",target_cnt++ );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'S': /* ҵ���� */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, "�ֽ�");
			break;
		case 'U': /* ָ������ */
			pub_base_strpack(s_correl_brf.target_no);

			ret = Correl_brf_Sel( g_pub_tx.reply, &x_correl_brf, \
						"target_no='%s'",  s_correl_brf.target_no );

			vtcp_log("RET------------.....?[%d]",ret);
			vtcp_log("INSPECT------>target_no=[%s],target_name=[%s]",
						s_correl_brf.target_no,x_correl_brf.brf );

			ERR_DEAL
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, x_correl_brf.brf);
			break;
		case 'X': /* ָ����� */
			pub_base_strpack(s_correl_brf.target_no);
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt,s_correl_brf.target_no);
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
