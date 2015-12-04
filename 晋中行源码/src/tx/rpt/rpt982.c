/*************************************************************
* �� �� ��: rpt982.c
* ����������ȫ��һ����Ŀ���˱�
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
		sprintf( acErrMsg, "sql error[%d]" ,ret); \
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
#include "com_sys_parm_c.h"
#include "gl_mst_c.h"
#include "gl_mst_hst_c.h"
#include "com_item_c.h"

struct gl_mst_c 		s_gl_mst;
struct gl_mst_hst_c		s_gl_mst_hst;
struct com_sys_parm_c	s_com_sys_parm;
struct com_item_c		s_com_item;
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
double mon_dr_cal=0.00;
double mon_cr_cal=0.00;
long tmp_date;
int vpage=0;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt982( )
{
	int ttlnum;
	char   filename[100];
	long   tmp_date;
	int first=0;

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

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
	tmp_date = g_pub_tx.tx_date/100*100 + 1 ; 

	strcpy( filename , "BMyjkmzzb" );	
	strcpy( vpfm, "yjkmzzb" );

	ttlnum=0;

	ret = Com_item_Dec_Sel( g_pub_tx.reply, " acc_lvl='1' " );
	ERR_DEAL

	while(1)
	{
		ret = Com_item_Fet_Sel( &s_com_item,g_pub_tx.reply );
		if( ret==100 ) break ;
		ERR_DEAL

		vtcp_log("INSPECT-------------------->>>>>>");

		if( !ttlnum )
		{
		vtcp_log("INSPECT++++++++++++++++++++>>>>>>");
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( TOT_BR_NO , filename , 0 );
			ERR_DEAL

		vtcp_log("INSPECT--------->>>>>>");
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, TOT_BR_NO , filename );
			ERR_DEAL
		}

		ret = Gl_mst_Sel( g_pub_tx.reply,&s_gl_mst,"acc_hrt='%s'", \
							s_com_item.acc_hrt );
		ERR_DEAL

		vtcp_log("AAAAAAAAAAAAAAAAAA");
		mon_dr_cal=0.00;
		mon_cr_cal=0.00;
		first=0;

		/*** ȡÿ��������ϸ ***/
		ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"acc_hrt='%s' and \"date\">=%ld \
								 and \"date\"<=%ld order by \"date\"", 
								 s_com_item.acc_hrt,tmp_date,
								 s_com_sys_parm.lst_date);
		ERR_DEAL

		vtcp_log("======== acc_hrt=[%s]",s_com_item.acc_hrt);
		while(1)
		{
			ret = Gl_mst_hst_Fet_Sel( &s_gl_mst_hst, g_pub_tx.reply );
			if( ret==100 ) break ;
			ERR_DEAL

			if( first==0 && s_gl_mst_hst.date>tmp_date )
			{
				break;
			}
			else if( first==0 )
			{
				/* ��ӡҳ�� */
				ret = print_head( );
				ERR_DEAL
				first=1;
			}

			/* ��ӡ��Ϣ */
			ret = print_body( );
			ERR_DEAL

			mon_dr_cal = mon_dr_cal + s_gl_mst_hst.dr_bal ;
			mon_cr_cal = mon_cr_cal + s_gl_mst_hst.cr_bal ;
		}

		Gl_mst_hst_Clo_Sel();

		/* ��ҳβ */
		if(  first )
		{
			ret = print_tail();
			ERR_DEAL

		}
			ttlnum=1;
	}

	Com_item_Clo_Sel( );
	fclose(fp);
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ȫ��һ����Ŀ���˱���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"ȫ��һ����Ŀ���˱���ʧ��!!!!![%s]",g_pub_tx.reply);
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
	int day_cnt;
	long amt;

	switch( bli ) 
	{ 
		case 'Z': /* ����page */
			sprintf(fmt,"%%%dd",bll); 
			vpage++;
			sprintf( str , fmt , vpage ); 
			break;
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , s_com_sys_parm.lst_date/10000 , s_com_sys_parm.lst_date%10000/100 , s_com_sys_parm.lst_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* ��Ŀ�� */
			pub_base_strpack( s_com_item.acc_no ); 
			sprintf( t_str , "%s" ,s_com_item.acc_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* ��Ŀ���� */
			pub_base_strpack( s_com_item.acc_name );
			sprintf( t_str , "%s" ,s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* �������� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.ydr_bal );
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break; 
		case 'E': /* ��跢���� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.ydr_amt );
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'F': /* ���½���� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.mdr_amt );
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break; 
		case 'G': /* �������� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.ycr_bal );
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'H': /* ��������� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.ycr_amt );
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'I': /* ���´���� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.mcr_amt );
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'J': /* ���� */ 
			sprintf( amt_tmp, "%ld",s_gl_mst_hst.date );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* ����� */
			sprintf( amt_tmp, "%ld",s_gl_mst_hst.rdd_cnt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* �跢���� */
			sprintf( amt_tmp, "%.2lf",s_gl_mst_hst.rdd_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'M': /* ������ */
			sprintf( amt_tmp, "%ld",s_gl_mst_hst.rcd_cnt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'N': /* �������� */
			sprintf( amt_tmp, "%.2lf",s_gl_mst_hst.rcd_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* ����� */
			sprintf( amt_tmp, "%.2lf",s_gl_mst_hst.dr_bal );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* ����� */
			sprintf( amt_tmp, "%.2lf",s_gl_mst_hst.cr_bal );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Q': /* �¼Ʒ����� �跽 */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.mdr_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'T': /* �¼Ʒ����� ���� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.mcr_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* ������ۼƷ����� �跽 */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.ydr_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* ������ۼƷ����� ���� */
			sprintf( amt_tmp, "%.2lf", s_gl_mst.ycr_amt );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'S': /* ���»��� �跽 */
			sprintf( amt_tmp, "%.2lf", mon_dr_cal );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V': /* ���»��� ���� */
			sprintf( amt_tmp, "%.2lf", mon_cr_cal );
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
