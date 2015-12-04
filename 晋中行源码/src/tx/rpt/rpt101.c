/*************************************************************
* �� �� ��: rpt101.c
* �����������ޱ�ǷϢ����嵥
*
* ��    ��: lance
* �������: 2003��10��26��
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mo_opn_cls_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct ln_mst_c	s_ln_mst;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
static long qry_date=0; /*add by martin 2009/4/17 9:27:43 */

long   tol_jx_cnt=0;
double tol_bal_amt=0.00;
double tol_in_amt=0.00;
double tol_out_amt=0.00;
double tol_cmpd_amt=0.00;

int get_ratio_lnwbqxqd(int bli,int bll,char str[100]);

rpt101()
{
	int flag=0;
	int ttlnum;
	long tmp_date;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	tol_jx_cnt=0;
	tol_bal_amt=0.00;
	tol_in_amt=0.00;	
	tol_out_amt=0.00;	
	tol_cmpd_amt=0.00;
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c));
			
	strcpy( old_br_no , "*****" );
	
	/*���㽻������ add by martin 2009/4/17 9:28:58*/
  pub_base_deadlineD( g_pub_tx.tx_date, -1 ,&qry_date );
	
  
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	
	strcpy( filename , "RPT101" );	
	strcpy(vpfm, "RPT101" );
		
	/**��ɲ�ѯ**/	
	strcpy(wherelist," (in_lo_intst>0.00 or out_lo_intst>0.00 or cmpd_lo_intst>0.00) and repay_type not in ('3','4') and prdt_no not in ('3Z1','3Z2','3Z3' ) and bal=0.00 ORDER BY opn_br_no,prdt_no,ac_id");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Ln_mst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Ln_mst_Fet_Sel( &s_ln_mst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				/* ���ܼ� */
				ret = print_zj();
				ERR_DEAL				
				/* ��ҳ */
				ret = print_NY();
				ERR_DEAL				
				fclose(fp);
			}
			break;
		}else if( ret ){
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack(s_ln_mst.opn_br_no );
		if( strcmp( s_ln_mst.opn_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				/* ���ܼ� */
				ret = print_zj();
				ERR_DEAL				
				/* ��ҳ */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_mst.opn_br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_bal_amt=0.00;
			tol_in_amt=0.00;	
			tol_out_amt=0.00;	
			tol_cmpd_amt=0.00;
						
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_ln_mst.opn_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_ln_mst.opn_br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ����ͳ�� */
    		tol_jx_cnt++;
    		tol_bal_amt+=s_ln_mst.bal;
    		tol_in_amt+=s_ln_mst.in_lo_intst;
    		tol_out_amt+=s_ln_mst.out_lo_intst;
    		tol_cmpd_amt+=s_ln_mst.cmpd_lo_intst;
    		  
    		/* ��ӡ��Ϣ */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>=50 )	/* ����50�л�ҳ */
		{
			line_file=0;
			page++;
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ҳ */
			ret = print_NY();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Ln_mst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������ͨ����Ƿ��嵥�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������ͨ����Ƿ��嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnwbqxqd);
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
		fprintf ( fp ,"\f" );
	}
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


/* ��ӡ��ҳ */
int print_NY( )
{
	char vrec[3];

	strcpy( vrec,"EE" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ӡ�ܼ� */
int print_zj( )
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
int get_ratio_lnwbqxqd( bli,bll,str )
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
			/*sprintf( t_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );*/
			sprintf( t_str , "%4d��%2d��%2d��" , qry_date/10000 , qry_date%10000/100 , qry_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_ln_mst.opn_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_mst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_mst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ��ݺ� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, s_ln_mst.pact_no); 
			break; 
		case 'F': /* �����˺� */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'H': /* Ƿ���ڽ�� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.in_lo_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* Ƿ������ */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.out_lo_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* Ƿ������� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.cmpd_lo_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;			
		case 'Q': /* ��Ƿ���� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;	
		case 'I': /* ����� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);

			break;
		case 'J': /* ��Ʒ */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		case 'M': /* ��Ʒ���� */			
			ret= Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",s_ln_mst.prdt_no);
			if(ret)	return 1;
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, g_ln_parm.title ); 
			break;
		case 'R': /* �����ܼ� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt); 
			break; 	
		case 'X': /* ����Ϣ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_in_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y': /* ����Ϣ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z': /* ����Ϣ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /* �������� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_mst.opn_date); 
			break; 
		case 'S': /* �������� */
			sprintf(fmt,"%%%dld",bll); 
			if( s_ln_mst.exp_ind[0]!='0' )
			{
				sprintf(str, fmt, s_ln_mst.exp_mtr_date); 
			}else{
				sprintf(str, fmt, s_ln_mst.mtr_date); 
			}				
			break; 			
		case 'W': /* չ�ڱ�־ */
			sprintf(fmt,"%%-%ds",bll); 
			if( s_ln_mst.exp_ind[0]!='0' )
			{
				strcpy(str,"չ"); 
			}else{
				strcpy(str,"  ");  
			}
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
