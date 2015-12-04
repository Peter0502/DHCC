/*************************************************************
* �� �� ��: rpt995.c
* ������������Ʒ�ձ����ӡ
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
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "rpt_note_inf_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "gl_prdt_dyn_c.h"
#define PAGE_CNT 43

struct gl_prdt_dyn_c	s_gl_prdt_dyn;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char prdt_name[31];
int opncnt;
FILE *fp; 
int ret = 0;

double dGHZY = 0.00;
double dGHJJ = 0.00;
double dGHJZ = 0.00;
double dGHJY = 0.00;

double dGDZY = 0.00;
double dGDJJ = 0.00;
double dGDJZ = 0.00;
double dGDJY = 0.00;

double dDHZY = 0.00;
double dDHJJ = 0.00;
double dDHJZ = 0.00;
double dDHJY = 0.00;

double dDDZY = 0.00;
double dDDJJ = 0.00;
double dDDJZ = 0.00;
double dDDJY = 0.00;

double dNBZY = 0.00;
double dNBJJ = 0.00;
double dNBJZ = 0.00;
double dNBJY = 0.00;

int get_ratio_cprbb(int bli,int bll,char str[100]);
int check_prdt( char *cPrdt );

rpt995()
{
	int    ttlnum;
	char   old_br_no[8];
	char   filename[100];

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
/*
	strcpy( filename , "BRcprbb" );
 	strcpy( vpfm, "cprbb" );
*/
	strcpy( filename , "RPT995");
 	strcpy( vpfm, "RPT995");
	ret = Gl_prdt_dyn_Dec_Sel( g_pub_tx.reply, "br_no in(select br_no from com_branch where br_type !='0') ORDER BY br_no,prdt_no " );
	ERR_DEAL

	while(1)
	{
		memset( &s_gl_prdt_dyn, 0x0, sizeof(s_gl_prdt_dyn) );
		ret = Gl_prdt_dyn_Fet_Sel( &s_gl_prdt_dyn,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail1();
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
		else if( ret ){
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG
			goto ErrExit;
		}

		/* �����任ʱ */
		pub_base_strpack( s_gl_prdt_dyn.br_no );
		if( strcmp( s_gl_prdt_dyn.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail1();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_gl_prdt_dyn.br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_gl_prdt_dyn.br_no , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_gl_prdt_dyn.br_no , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL			
			
			dGHZY = 0.00;
			dGHJJ = 0.00;
			dGHJZ = 0.00;
			dGHJY = 0.00;
			
			dGDZY = 0.00;
			dGDJJ = 0.00;
			dGDJZ = 0.00;
			dGDJY = 0.00;
			
			dDHZY = 0.00;
			dDHJJ = 0.00;
			dDHJZ = 0.00;
			dDHJY = 0.00;
			
			dDDZY = 0.00;
			dDDJJ = 0.00;
			dDDJZ = 0.00;
			dDDJY = 0.00;
			
			dNBZY = 0.00;
			dNBJJ = 0.00;
			dNBJZ = 0.00;
			dNBJY = 0.00;
		}
		
		ret = 0;
		pub_base_strpack( s_gl_prdt_dyn.prdt_no );
		ret = check_prdt( s_gl_prdt_dyn.prdt_no );
		if ( ret == 1 )
		{
			dGHZY = dGHZY + s_gl_prdt_dyn.lcd_bal;
			dGHJJ = dGHJJ + s_gl_prdt_dyn.rdd_amt;
			dGHJZ = dGHJZ + s_gl_prdt_dyn.rcd_amt;
			dGHJY = dGHJY + s_gl_prdt_dyn.cr_bal;
		}
		else if ( ret == 2 )
		{
			dGDZY = dGDZY + s_gl_prdt_dyn.lcd_bal;
			dGDJJ = dGDJJ + s_gl_prdt_dyn.rdd_amt;
			dGDJZ = dGDJZ + s_gl_prdt_dyn.rcd_amt;
			dGDJY = dGDJY + s_gl_prdt_dyn.cr_bal;
		}
		else if ( ret == 3 )
		{
			dDHZY = dDHZY + s_gl_prdt_dyn.lcd_bal;
			dDHJJ = dDHJJ + s_gl_prdt_dyn.rdd_amt;
			dDHJZ = dDHJZ + s_gl_prdt_dyn.rcd_amt;
			dDHJY = dDHJY + s_gl_prdt_dyn.cr_bal;
		}
		else if ( ret == 4 )
		{
			dDDZY = dDDZY + s_gl_prdt_dyn.lcd_bal;
			dDDJJ = dDDJJ + s_gl_prdt_dyn.rdd_amt;
			dDDJZ = dDDJZ + s_gl_prdt_dyn.rcd_amt;
			dDDJY = dDDJY + s_gl_prdt_dyn.cr_bal;
		}
		else if ( ret == 5 )
		{
			dNBZY = dNBZY + s_gl_prdt_dyn.lcd_bal;
			dNBJJ = dNBJJ + s_gl_prdt_dyn.rdd_amt;
			dNBJZ = dNBJZ + s_gl_prdt_dyn.rcd_amt;
			dNBJY = dNBJY + s_gl_prdt_dyn.cr_bal;
		}

		ret = print_body();
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

	Gl_prdt_dyn_Clo_Sel( );

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������ˮ�嵥�������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������ˮ�嵥�������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cprbb);
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

/* ��ӡ��β */
int print_tail1( )
{
	char vrec[3];

	strcpy( vrec,"DD" );
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
int get_ratio_cprbb( bli,bll,str )
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

	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , s_gl_prdt_dyn.date/10000 , s_gl_prdt_dyn.date%10000/100 , s_gl_prdt_dyn.date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_gl_prdt_dyn.br_no ); 
			break; 
		case 'C': /* �������� */
			ret = pub_base_getbrname( s_gl_prdt_dyn.br_no, br_name );
			if( ret )
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_gl_prdt_dyn.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'L': /* ��Ʒ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_gl_prdt_dyn.prdt_no);
			break;
		case 'P': /* �������� */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_gl_prdt_dyn.date);
			break;
		case 'Q': /* ������� */ 			
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* ������� */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn.lcd_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'S': /* ���ռ��ٽ�� */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn.rdd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'T': /* �������ӽ�� */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* ��Ʒ���� */
			ret = pub_base_prdt_name( s_gl_prdt_dyn.prdt_no, prdt_name );
			if( ret )
			{
				sprintf(acErrMsg, "��ѯ��Ʒ���ƴ���! [%d][%s]", \
						ret,s_gl_prdt_dyn.prdt_no);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , prdt_name );
			break;
		case 'E':  			
			sprintf( amt_tmp, "%.2lf" , dGHZY ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': 			
			sprintf( amt_tmp, "%.2lf" , dGHJJ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G':
			sprintf( amt_tmp, "%.2lf" , dGHJZ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H':			
			sprintf( amt_tmp, "%.2lf" , dGHJY );
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I':  			
			sprintf( amt_tmp, "%.2lf" , dGDZY ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'J': 			
			sprintf( amt_tmp, "%.2lf" , dGDJJ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K':
			sprintf( amt_tmp, "%.2lf" , dGDJZ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '8':			
			sprintf( amt_tmp, "%.2lf" , dGDJY );
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V':  			
			sprintf( amt_tmp, "%.2lf" , dDHZY ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'W': 			
			sprintf( amt_tmp, "%.2lf" , dDHJJ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'X':
			sprintf( amt_tmp, "%.2lf" , dDHJZ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y':			
			sprintf( amt_tmp, "%.2lf" , dDHJY );
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z':  			
			sprintf( amt_tmp, "%.2lf" , dDDZY ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '1': 			
			sprintf( amt_tmp, "%.2lf" , dDDJJ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '2':
			sprintf( amt_tmp, "%.2lf" , dDDJZ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '3':
			sprintf( amt_tmp, "%.2lf" , dDDJY );
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '4':  			
			sprintf( amt_tmp, "%.2lf" , dNBZY ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '5': 			
			sprintf( amt_tmp, "%.2lf" , dNBJJ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll);
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '6':
			sprintf( amt_tmp, "%.2lf" , dNBJZ ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case '7':
			sprintf( amt_tmp, "%.2lf" , dNBJY );
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

int check_prdt( char *cPrdt )
{
	int ret = 0;
	
	struct dd_parm_c sDd_parm;
	struct td_parm_c sTd_parm;
	
	if ( cPrdt[0] == '1' )	/*����*/
	{
		memset( &sDd_parm, 0x0, sizeof(sDd_parm) );
		ret = Dd_parm_Sel( g_pub_tx.reply, &sDd_parm, "prdt_no = '%s'", cPrdt );
		if ( ret )
		{
			sprintf(acErrMsg, "��ѯ��Ʒ��������! [%s]", cPrdt );
			WRITEMSG
			return -1;
		}
		if ( sDd_parm.cif_type[0] == '1' )	/*���˻���*/
		{
			return 1;
		}
		else
		{
			return 3;
		}
	}
	else if ( cPrdt[0] == '2' )	/*����*/
	{
		memset( &sTd_parm, 0x0, sizeof(sTd_parm) );
		ret = Td_parm_Sel( g_pub_tx.reply, &sTd_parm, "prdt_no = '%s'", cPrdt );
		if ( ret )
		{
			sprintf(acErrMsg, "��ѯ��Ʒ��������! [%s]", cPrdt );
			WRITEMSG
			return -1;
		}
		if ( sTd_parm.cif_type[0] == '1' )	/*���˶���*/
		{
			return 2;
		}
		else
		{
			return 4;
		}
	}
	else if ( cPrdt[0] == '2' )	/*�ڲ���*/
	{
		return 5;
	}
	else
	{
		return -1;
	}
}
