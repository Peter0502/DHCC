/*************************************************************
* �� �� ��: rpt901.c
* ������������Ŀ��ˮ������ѯ��ӡ
*
* ��    ��: martin
* �������: 2007��07��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) \
	  {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "tx_def_c.h"
#include "com_sys_parm_c.h"
#include "dc_log_rz_c.h"

#define PAGE_CNT 42

int page=0;
int line=0;
int line_file=0;

FILE    *fp; 
int     ret = 0;
int     opncnt;
char    vpfm[21];
char    t_str[41];

struct dc_log_rz_c	s_dc_log_rz;
struct tx_def_c s_tx_def;


int get_ratio_kmls(int bli,int bll,char str[100]);

rpt901()
{
	int    ttlnum;
	char   filename[100];
	char   old_br_no[6];
	
	struct com_sys_parm_c s_com_sys_parm;
	
	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c));
	
	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	strcpy( old_br_no , "*****" );
	
	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
    
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	strcpy( filename , "RPT901" );
	strcpy( vpfm, "RPT901");
	
	ret = Dc_log_rz_Dec_Sel( g_pub_tx.reply, "tx_date='%08d' and 1=1 ORDER BY tx_tx_br_no,acc_hrt,amt,trace_no",g_pub_tx.tx_date );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Dc_log_rz_Fet_Sel( &s_dc_log_rz,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL

				fclose(fp);
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����ˮ!" );
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
		
		/* �����任ʱ */
		pub_base_strpack( s_dc_log_rz.tx_tx_br_no );
		if( strcmp( s_dc_log_rz.tx_tx_br_no , old_br_no ) )
		{
			printf("[%s][%d]����[%s]����!\n",__FILE__,__LINE__,s_dc_log_rz.tx_tx_br_no);
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL

				fclose(fp);
			}
			
			strcpy( old_br_no , s_dc_log_rz.tx_tx_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_dc_log_rz.tx_tx_br_no , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_dc_log_rz.tx_tx_br_no , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL
		}
		
			/* ��ӡ��Ϣ */
		ret = print_body( );
		ERR_DEAL

		line_file++;
		sprintf( acErrMsg, "��Ŀ��ˮ=��[%d]��!",line );
		WRITEMSG
		if( line_file>PAGE_CNT )
		{
			line_file=0;
			page++;
			sprintf( acErrMsg, "��Ŀ��ˮ=��[%d]ҳ!",page );
			WRITEMSG			
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			
			
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}

	Dc_log_rz_Clo_Sel( );
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��Ŀ��ˮ�嵥�������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��Ŀ��ˮ�嵥�������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_kmls);
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
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
	strcpy( vrec,"KK" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
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
int get_ratio_kmls( bli,bll,str )
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
	memset(&s_tx_def,0x00,sizeof(struct tx_def_c));

	vtcp_log("DC_LOG_RZ.TX_TX_BR_NO==============[%s]",s_dc_log_rz.tx_tx_br_no);

	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , s_dc_log_rz.tx_date/10000 , s_dc_log_rz.tx_date%10000/100 , s_dc_log_rz.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_dc_log_rz.tx_tx_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = pub_base_getbrname( s_dc_log_rz.tx_br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" ,br_name,s_dc_log_rz.tx_tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'E': /* ��ˮ�� add by martin ȡ����ˮǰ�ӣ�*�� */
		  if( strcmp( s_dc_log_rz.sts , "1" ) )
		  {
				sprintf(fmt,"%%%dld",bll); 
				sprintf(str, fmt , s_dc_log_rz.trace_no); 
				break; 
			}else
			{
				sprintf(fmt,"*%%%dld",bll); 
				sprintf(str, fmt , s_dc_log_rz.trace_no); 
				break;
		  } 
		case 'J': /* ��Ŀ�� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt , s_dc_log_rz.acc_hrt);
			break;
		case 'K': /* ��  ��  �� */
			sprintf( amt_tmp, "%.2lf" , s_dc_log_rz.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
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