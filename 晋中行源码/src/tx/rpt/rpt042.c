/*************************************************************
* �� �� ��: rpt042.c
* �������������Ҵ������տۿ��嵥(ÿ��)
*
* ��    ��: lance
* �������: 2003��09��15��
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
#include "ln_mst_hst_c.h"
#include "com_branch_c.h"

struct ln_mst_hst_c s_ln_mst_hst;
struct ln_mst_hst_c old_ln_mst_hst;
struct ln_mst_c s_ln_mst;
struct ln_parm_c s_ln_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
long min_trace_no=0;
FILE    *fp; 
int ret = 0;
double bal_amt=0.00;
double in_amt=0.00;
double out_amt=0.00;
double cmpd_amt=0.00;
double rate_tmp=0.00;
long   tol_jx_cnt=0;
double tol_bal_amt=0.00;
double tol_in_amt=0.00;
double tol_out_amt=0.00;
double tol_cmpd_amt=0.00;


long days=0;
long qry_date=0;
char intst_type_name[9];
		
int get_ratio_lnajkkqd(int bli,int bll,char str[100]);

rpt042()
{
	int flag=0;
	int ttlnum=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	char old_intst_type[2];
	long old_ac_id=0;
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_mst_hst , 0x00 , sizeof(struct ln_mst_hst_c));
	memset( &old_ln_mst_hst, 0x00 , sizeof( struct ln_mst_hst_c ));
		
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	
	/*���㽻������*/
	qry_date=g_pub_tx.tx_date;

	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
/*	
	strcpy( filename , "BJlnajkkqd" );	
	strcpy(vpfm, "lnajkkqd" );
*/	
	strcpy( filename , "RPT042");
	strcpy(vpfm, "RPT042");
	/* ���������Ϣ�嵥 */
	/* ����:tx_dateΪ����,tx_code='G085'��'G086',������־Ϊ���� */
	tol_jx_cnt=0;
	tol_bal_amt=0.00;
	tol_in_amt=0.00;	
	tol_out_amt=0.00;	
	tol_cmpd_amt=0.00;
	memset( &tmpstr,0x0,strlen(tmpstr));	
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," tx_code = 'G085' and tx_date=%ld and add_ind='0' and " ,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY opn_br_no, ac_id, ac_seqn, ln_tx_type ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	while(1)
	{
		ret=Ln_mst_hst_Fet_Sel( &s_ln_mst_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ӡ���� */
				ret = print_body();
				ERR_DEAL
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
			sprintf(acErrMsg,"��ȡ�����Ϣ�Ǽǲ��쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
			sprintf(acErrMsg,"ac_id=%ld and old_ac_id=%ld!",s_ln_mst_hst.ac_id,old_ac_id);
			WRITEMSG
		if( old_ac_id!=s_ln_mst_hst.ac_id )	
		{	
			sprintf(acErrMsg,"ac_id=%ld and old_ac_id=%ld!",s_ln_mst_hst.ac_id,old_ac_id);
			WRITEMSG
		    	/* �����任ʱ */
			pub_base_strpack( s_ln_mst_hst.opn_br_no );
			if( strcmp( s_ln_mst_hst.opn_br_no , old_br_no ) )
			{
				TRACE
				/* ��һ������ */
				if( strcmp( old_br_no , "*****" ) )
				{
					TRACE
					/* ��ӡ���� */
					ret = print_body();
					ERR_DEAL
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
				TRACE
				strcpy( old_br_no , s_ln_mst_hst.opn_br_no );
				
				page=1;
				line_file=0;
				tol_jx_cnt=0;
				tol_bal_amt=0.00;
				tol_in_amt=0.00;	
				tol_out_amt=0.00;	
				tol_cmpd_amt=0.00;
								
				/* ɾ��ԭ�ļ� */
				ret = pub_rpt_rmfile( s_ln_mst_hst.opn_br_no , filename , 0 );
				ERR_DEAL
				/* ���ļ� */	
				ret = pub_rpt_openfile( &fp , s_ln_mst_hst.opn_br_no , filename );
				ERR_DEAL
				/* ��ӡҳ�� */
				ret = print_head();
				ERR_DEAL
				old_ac_id=0;
			} 
	    		TRACE
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
			if( old_ac_id )
			{
				tol_jx_cnt++;	/* �ܼƻ��� */
				/* ��ӡ���� */
				ret = print_body();
				ERR_DEAL
			}			
			ttlnum++;	
			TRACE
			memset( &old_ln_mst_hst,0x00,sizeof(struct ln_mst_hst_c ));
			
			memcpy( &old_ln_mst_hst,&s_ln_mst_hst,sizeof(struct ln_mst_hst_c ));	
				
			old_ac_id=s_ln_mst_hst.ac_id;
			bal_amt=0.00;
			in_amt=0.00;
			out_amt=0.00;
			cmpd_amt=0.00;
TRACE
			if( s_ln_mst_hst.ln_tx_type[0]=='0' )
			{
				bal_amt=s_ln_mst_hst.tx_amt;
				tol_bal_amt+=bal_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='1' )
			{
				in_amt=s_ln_mst_hst.tx_amt;
				tol_in_amt+=in_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='2' )
			{
				out_amt=s_ln_mst_hst.tx_amt;
				tol_out_amt+=out_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='3' )
			{
				cmpd_amt=s_ln_mst_hst.tx_amt;
				tol_cmpd_amt+=cmpd_amt;
			}		
		}else{
			if( s_ln_mst_hst.ln_tx_type[0]=='0' )
			{
				bal_amt+=s_ln_mst_hst.tx_amt;
				tol_bal_amt+=bal_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='1' )
			{
				in_amt+=s_ln_mst_hst.tx_amt;
				tol_in_amt+=in_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='2' )
			{
				out_amt+=s_ln_mst_hst.tx_amt;
				tol_out_amt+=out_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='3' )
			{
				cmpd_amt+=s_ln_mst_hst.tx_amt;
				tol_cmpd_amt+=cmpd_amt;
			}
		}			
	}		
	Ln_mst_hst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɰ��Ҵ���ۿ��嵥�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɰ��Ҵ���ۿ��嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnajkkqd);
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
	/*ȡ�����Ϣ*/
	ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",old_ln_mst_hst.ac_id,old_ln_mst_hst.ac_seqn ); /*��ȡ�����Ϣ*/
	if( ret ) 
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		return ret;
	}
			
	ret = Ln_parm_Sel( g_pub_tx.reply , &s_ln_parm,"prdt_no='%s' ",s_ln_mst.prdt_no ); /*��ȡ�����Ϣ*/
	if( ret ) 
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		return ret;
	}
	
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ֵ���� */
int get_ratio_lnajkkqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char mtr_date[13];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));		
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , qry_date/10000 , qry_date%10000/100 , qry_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_ln_mst_hst.opn_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_mst_hst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_mst_hst.opn_br_no); 
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
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",old_ln_mst_hst.ac_id,s_ln_mst_hst.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'H': /* ������ */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , bal_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* ��Ϣ��� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , in_amt+out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* ��Ϣ��� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , cmpd_amt ); 
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
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, s_ln_parm.title ); 
			break;
		case 'Q': /* �ۿ��˺� */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld",s_ln_mst.repay_ac_id);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'R': /* �����ܼ� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt+1); 
			break; 
		case 'X': /* ����Ϣ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_bal_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y': /* ����Ϣ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_in_amt+tol_out_amt ); 
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
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
