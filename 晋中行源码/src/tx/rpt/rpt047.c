/*************************************************************
* �� �� ��: rpt047.c
* �������������Ҵ���Ƿ��嵥
*
* ��    ��: lance
* �������: 2003��09��21��
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
#include "ln_lo_c.h"
#include "ln_pay_pln_tmp_c.h"
#include "ln_parm_c.h"                  /*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/
#include "dc_acc_rel_c.h"								/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/
#include "com_item_c.h"									/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/

struct com_sys_parm_c	s_com_sys_parm;
struct ln_mst_c	s_ln_mst;
struct ln_lo_c	s_ln_lo;
struct ln_pay_pln_tmp_c	s_ln_pay_pln_tmp;
struct ln_parm_c s_ln_parm;							/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/
struct dc_acc_rel_c s_dc_acc_rel;				/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/
struct com_item_c s_com_item;						/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
double lo_bj=0.00;
double lo_lx=0.00;
double tol_bj=0.00;
double tol_lx=0.00;
double pay_bj=0.00;
double pay_lx=0.00;

double s_bj=0.00;
double s_lx=0.00;
double tol_s_bj=0.00;
double tol_s_lx=0.00;

long   tol_jx_cnt=0;
double tol_out_amt=0.00;
double tol_cmpd_amt=0.00;

int get_ratio_lnajloqd(int bli,int bll,char str[100]);

rpt047()
{
	int flag=0;
	int ttlnum;
	long tmp_date;
	long old_ac_id=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	tol_jx_cnt=0;	
	tol_out_amt=0.00;	
	tol_cmpd_amt=0.00;
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_ln_mst , 0x00 , sizeof(struct ln_mst_c));
	memset( &s_ln_lo , 0x00 , sizeof(struct ln_lo_c));
				
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
	
	strcpy( filename , "RPT047" );	
	strcpy(vpfm, "RPT047" );
		
	/**��ɲ�ѯ**/	
	strcpy( wherelist," repay_type in ('3','4') ORDER BY opn_br_no,prdt_no,opn_date,ac_id,ac_seqn");
	
	ret= Ln_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	
	ttlnum=0;	
	while(1)
	{
		ret= Ln_mst_Fet_Sel( &s_ln_mst,g_pub_tx.reply);
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
			sprintf(acErrMsg,"��ȡ�������ļ���Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		ret= Ln_lo_Sel( g_pub_tx.reply,&s_ln_lo,"ac_id=%ld and ac_seqn=%d and pay_type='0' ",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if( ret&&ret!=100 )
		{
			sprintf(acErrMsg,"��ȡ����Ƿ�����Ϣ�쳣����!");
			WRITEMSG			
			goto ErrExit;
		}else if( ret==100 )
		{
			continue;
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
		/* ͳ����Ϣ */	
		ret= sql_sum_double( "ln_lo","lo_amt",&lo_bj,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1; 
		ret= sql_sum_double( "ln_lo","lo_intst",&lo_lx,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1;
		ret= sql_sum_double( "ln_lo","pay_lo_amt",&pay_bj,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1;
		ret= sql_sum_double( "ln_lo","pay_lo_intst",&pay_lx,"ac_id=%ld and ac_seqn=%d",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if(ret)	return 1;
		
		
		pay_bj=	lo_bj-pay_bj;
		pay_lx= lo_lx-pay_lx;
		
    		/* ����ͳ�� */
    		tol_jx_cnt++;
    		tol_out_amt+=pay_bj;
    		tol_cmpd_amt+=pay_lx;
    																	
    		ret= Ln_pay_pln_tmp_Sel( g_pub_tx.reply,&s_ln_pay_pln_tmp,"ac_id=%ld and ac_seqn=%d ",s_ln_mst.ac_id,s_ln_mst.ac_seqn);
		if( ret&&ret!=100 )
		{
			sprintf(acErrMsg,"��ȡ����Ƿ�����Ϣ�쳣����!");
			WRITEMSG			
			goto ErrExit;
		}else if( ret==100 )
		{
			s_bj=0.00;
			s_lx=0.00;
		}
    		s_bj=s_ln_pay_pln_tmp.curr_amt;
    		s_lx=s_ln_pay_pln_tmp.curr_intst;
    		tol_s_bj+=s_bj;
    		tol_s_lx+=s_lx;
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
	sprintf(acErrMsg,"���ɰ��Ҵ���Ƿ��嵥�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɰ��Ҵ���Ƿ��嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnajloqd);
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
int get_ratio_lnajloqd( bli,bll,str )
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
			sprintf( t_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
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
		case 'H': /* Ƿ������ */			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , pay_bj ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* Ƿ��Ϣ��� */			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , pay_lx ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;		
		case 'G': /* ���ڱ��� */			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_bj ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* ������Ϣ */			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_lx ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;	
		case 'Q': /* �ܴ����� */
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
		/**��Ʒ�Ͳ�Ʒ����ע�͵����ӿ�Ŀ�š�zhangpeng 20131125 start**
		case 'J': //��Ʒ 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		case 'M': //��Ʒ����			
			ret= Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",s_ln_mst.prdt_no);
			if(ret)	return 1;
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, g_ln_parm.title ); 
			break;
		*��Ʒ�Ͳ�Ʒ����ע�͵����ӿ�Ŀ�š�zhangpeng 20131125 end***/
		
		/**���ӿ�Ŀ�š�zhangpeng 20131125 start***/
		case 'J': /*��Ŀ��*/			
			ret=Ln_parm_Sel( g_pub_tx.reply ,&s_ln_parm,"prdt_no='%s'",s_ln_mst.prdt_no);
			if( ret )
			{
				return 1;
			}
			ret=Dc_acc_rel_Sel( g_pub_tx.reply ,&s_dc_acc_rel,"dc_code='%s'",s_ln_parm.dc_code);
			if( ret )
			{
				return 1;
			}
			ret=Com_item_Sel( g_pub_tx.reply ,&s_com_item,"acc_hrt='%s'",s_dc_acc_rel.acc_hrt);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, s_com_item.acc_no ); 
			memset( &s_ln_parm, 0x00 , sizeof( struct ln_parm_c ));
			memset( &s_dc_acc_rel, 0x00 , sizeof( struct dc_acc_rel_c ));
			memset( &s_com_item, 0x00 , sizeof( struct com_item_c ));
			break;
		/**���ӿ�Ŀ�š�zhangpeng 20131125 end***/
		
		case 'X': /* �������� */
			sprintf( t_str , "%4d-%02d-%02d" , s_ln_mst.opn_date/10000 , s_ln_mst.opn_date%10000/100 , s_ln_mst.opn_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'Y': /* �������� */
			sprintf( t_str , "%4d-%02d-%02d" , s_ln_mst.mtr_date/10000 , s_ln_mst.mtr_date%10000/100 , s_ln_mst.mtr_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'R': /* �����ܼ� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt); 
			break; 
		case 'U': /* Ƿ���ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V': /* ǷϢ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'W': /* ���ڱ����ܼ� */			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_s_bj ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z': /* ������Ϣ�ܼ� */			
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_s_lx ); 
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
