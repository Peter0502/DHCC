/*************************************************************
* �� �� ��: rpt018.c
* ���������������Ʒ�����嵥
*
* ��    ��: lance
* �������: 2003��07��15��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
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
#include "gl_prdt_dyn_hst_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"

struct gl_prdt_dyn_hst_c	s_gl_prdt_dyn_hst;
struct com_sys_parm_c	s_com_sys_parm;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char prdt_name[31];
int opncnt;
FILE  *fp; 
int ret = 0;

long	l_ac_cnt_tol=0;		/* ���ջ����ܼ� */
double	lcd_bal_tol=0;		/* ��������ܼ� */
long	rcd_cnt_tol=0;		/* ���ӱ����ܼ� */
double	rcd_amt_tol=0;		/* ���ӽ���ܼ� */
long	rdd_cnt_tol=0;		/* ���ٱ����ܼ� */
double	rdd_amt_tol=0;		/* ���ٽ���ܼ� */
long	d_opn_cnt_tol=0;	/* ���������ܼ� */
double	cr_bal_tol=0;		/* ��������ܼ� */
long	d_cls_cnt_tol=0;	/* ���������ܼ� */
long	ac_cnt_tol=0;		/* ���ջ����ܼ� */

int get_ratio_cxcpqd(int bli,int bll,char str[100]);

rpt018()
{
	int flag=0;
	int ttlnum;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_gl_prdt_dyn_hst , 0x00 , sizeof(struct gl_prdt_dyn_hst_c));
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
		
	strcpy( old_br_no , "*****" );
		 
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	
	strcpy( filename , "BRcxcpqd" );	
	strcpy(vpfm, "cxcpqd" );
	
	/* ����: ��������Ϊ��ǰ����ǰһ�� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	sprintf( tmpstr," date=%ld and " , s_com_sys_parm.lst_date );
	strcat( wherelist,tmpstr );	

	/* ����: ����Ʒ */
	strcpy( tmpstr," prdt_cod[1]='1' or prdt_cod[1]='2' and ");
	strcat( wherelist,tmpstr );

	/**��ɲ�ѯ**/	
	strcat(wherelist," 1=1 ORDER BY br_no,prdt_cod");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Gl_prdt_dyn_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Gl_prdt_dyn_hst_Fet_Sel( &s_gl_prdt_dyn_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;
		}else if( ret ){
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �ж��Ƿ��Ƕ�˽�ͻ�*/
		if( s_gl_prdt_dyn_hst.prdt_cod[0]=='1' )	/* ���� */
		{
			ret = Dd_parm_Sel( g_pub_tx.reply , &g_dd_parm , "prdt_no='%s'" , s_gl_prdt_dyn_hst.prdt_cod );
			ERR_DEAL
			/* ���Ƕ�˽���û��ڲ�Ա�� */
			if( g_dd_parm.cif_type[0]!='1' && g_dd_parm.cif_type[0]!='0' && g_dd_parm.cif_type[0]!='5' )
			{
				continue;
			}
			strcpy( prdt_name , g_dd_parm.title );
		}
				
		/* �����任ʱ */
		pub_base_strpack(s_gl_prdt_dyn_hst.br_no );
		if( strcmp( s_gl_prdt_dyn_hst.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_gl_prdt_dyn_hst.br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_gl_prdt_dyn_hst.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_gl_prdt_dyn_hst.br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
			/* �ܼ��������� */
			l_ac_cnt_tol=0;		/* ���ջ����ܼ� */
			lcd_bal_tol=0;		/* ��������ܼ� */
			rcd_cnt_tol=0;		/* ���ӱ����ܼ� */
			rcd_amt_tol=0;		/* ���ӽ���ܼ� */
			rdd_cnt_tol=0;		/* ���ٱ����ܼ� */
			rdd_amt_tol=0;		/* ���ٽ���ܼ� */
			d_opn_cnt_tol=0;	/* ���������ܼ� */
			cr_bal_tol=0;		/* ��������ܼ� */
			d_cls_cnt_tol=0;	/* ���������ܼ� */
			ac_cnt_tol=0;		/* ���ջ����ܼ� */						
		}
		
    		/* ��ӡ��Ϣ */
		/* ͳ����Ϣ */
		l_ac_cnt_tol+=s_gl_prdt_dyn_hst.ac_cnt;		/* ���ջ����ܼ� */
		lcd_bal_tol+=s_gl_prdt_dyn_hst.cr_bal;		/* ��������ܼ� */
		rcd_cnt_tol+=s_gl_prdt_dyn_hst.rcd_cnt;		/* ���ӱ����ܼ� */
		rcd_amt_tol+=s_gl_prdt_dyn_hst.rcd_amt;		/* ���ӽ���ܼ� */
		rdd_cnt_tol+=s_gl_prdt_dyn_hst.rdd_cnt;		/* ���ٱ����ܼ� */
		rdd_amt_tol+=s_gl_prdt_dyn_hst.rdd_amt;		/* ���ٽ���ܼ� */
		d_opn_cnt_tol+=s_gl_prdt_dyn_hst.d_opn_cnt;	/* ���������ܼ� */
		cr_bal_tol+=s_gl_prdt_dyn_hst.cr_bal;		/* ��������ܼ� */
		d_cls_cnt_tol+=s_gl_prdt_dyn_hst.d_cls_cnt;	/* ���������ܼ� */
		ac_cnt_tol+=s_gl_prdt_dyn_hst.ac_cnt;		/* ���ջ����ܼ� */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* ����50�л�ҳ */
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
	Gl_prdt_dyn_hst_Clo_Sel();
	
OkExit:
	
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ����Ʒ�����嵥�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɴ����Ʒ�����嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxcpqd);
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
int get_ratio_cxcpqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , s_gl_prdt_dyn_hst.date/10000 , s_gl_prdt_dyn_hst.date%10000/100 , s_gl_prdt_dyn_hst.date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_gl_prdt_dyn_hst.br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_gl_prdt_dyn_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				strcpy( sCom_branch.br_name," " );
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_gl_prdt_dyn_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 		
		case 'E': /* ��Ʒ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_gl_prdt_dyn_hst.prdt_cod); 
			break; 
		case 'F': /* ��Ʒ���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , prdt_name);
			break;
		case 'G': /* ���ջ��� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.ac_cnt);
			break;		
		case 'H': /* ������� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;			
		case 'I': /* ���ӱ��� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.rcd_cnt);
			break;
		case 'J': /* ���ӽ�� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.rcd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;					
		case 'K': /* ���ٱ��� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.rdd_cnt);
			break;
		case 'L': /* ���ٽ�� */
			/* �Ӷ��� */
			strcpy( tmp_inf , "-" );			
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.rdd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			/* �Ӹ��� */
			strcat( tmp_inf , amt_tmp); 							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, tmp_inf ); 
			break;
		case 'M': /* �������� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.d_opn_cnt);
			break;
		case 'N': /* ������� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_gl_prdt_dyn_hst.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;	
		case 'O': /* �������� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_gl_prdt_dyn_hst.d_cls_cnt);
			break;
		case 'P': /* ���ջ��� */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_gl_prdt_dyn_hst.ac_cnt);
			break;									
		case 'Q': /* ���ջ����ܼ� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , l_ac_cnt_tol);
			break;
		case 'R': /* ��������ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , lcd_bal_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;		
		case 'S': /* ���ӱ����ܼ� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , rcd_cnt_tol);
			break;
		case 'T': /* ���ӽ���ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , rcd_amt_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* ���ٱ����ܼ� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , rdd_cnt_tol);
			break;
		case 'V': /* ���ٽ���ܼ� */
			/* �Ӷ��� */
			strcpy( tmp_inf , "-" );			
			sprintf( amt_tmp, "%.2lf" , rdd_amt_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			/* �Ӹ��� */
			strcat( tmp_inf , amt_tmp); 							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, tmp_inf ); 
			break;
		case 'W': /* ���������ܼ� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , d_opn_cnt_tol);
			break;
		case 'X': /* ��������ܼ� */ 			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , cr_bal_tol ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y': /* ���������ܼ� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , d_cls_cnt_tol);
			break;
		case 'Z': /* ���ջ����ܼ� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , ac_cnt_tol);
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
