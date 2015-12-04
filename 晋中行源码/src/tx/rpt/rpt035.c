/*************************************************************
* �� �� ��: rpt035.c
* ����������ÿ��20�Ŵ����Ϣ�嵥
*
* ��    ��: jane
* �������: 2005��09��15��
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
#include "ln_acm_reg_c.h"
#include "com_branch_c.h"

struct ln_acm_reg_c s_ln_acm_reg;
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

double tol_jx_amt=0.00;
long   tol_jx_cnt=0;

char pay_c_sts[7];
char clear_flg[2];
long days=0;
long qry_date=0;
char intst_type_name[9];
		
int get_ratio_lnjxqd(int bli,int bll,char str[100]);

rpt035()
{
	int flag=0;
	int del_ind=0;
	int ttlnum;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	char old_intst_type[2];
	long old_trace_no=0;
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_acm_reg , 0x00 , sizeof(struct ln_acm_reg_c));
	
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	
	/*���㽻������*/
	qry_date=g_pub_tx.tx_date;
	/***
	pub_base_deadlineD( g_pub_tx.tx_date, -1 ,&qry_date );	
	*****/
	
	/*ȷ��������С��ˮ��*/
	ret= sql_min_long( "trace_log_rz", "trace_no", &min_trace_no, "tx_date=%ld and tel='9999'",qry_date);
	if(ret)  goto ErrExit;

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
	strcpy( filename , "BJlnjxqd" );	
	strcpy(vpfm, "lnjxqd" );
*/	
	strcpy( filename , "RPT035");
	strcpy(vpfm, "RPT035");
	/* ���������Ϣ�嵥 */
	/* ����:��ˮ�Ŵ��ڵ���������ʼ��ˮ�� */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	memset( &tmpstr,0x0,strlen(tmpstr));	
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and cal_date=%ld and intst_type in('1') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "�������" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
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
			sprintf(acErrMsg,"��ȡ�����Ϣ�Ǽǲ��쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		del_ind++;	
		/*ȡ�����Ϣ*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*��ȡ�����Ϣ*/
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
									
		/* �����任ʱ */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
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
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
	
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		} 
					  
		/* �ܼ���Ϣ */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
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
	Ln_acm_reg_Clo_Sel();

	/* ���ڻ�����Ϣ�嵥 */
	/* ����:��ˮ�Ŵ��ڵ���������ʼ��ˮ�� */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	memset( &tmpstr,0x0, strlen( tmpstr ));
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and end_date=%ld and intst_type in ('2') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "���ڻ���" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
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
			sprintf(acErrMsg,"��ȡ�����Ϣ�Ǽǲ��쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		del_ind++;	
		/*ȡ�����Ϣ*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*��ȡ�����Ϣ*/
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
										
		/* �����任ʱ */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
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
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
						
			/* ɾ��ԭ�ļ� */
			if( del_ind==0 ) 
			{
				ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
				ERR_DEAL
			}
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		} 
					  
    		/* ��ӡ��Ϣ */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		
		/* �ܼ���Ϣ */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
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
	Ln_acm_reg_Clo_Sel();
	
	/* ���������Ϣ�嵥 */
	/* ����:��ˮ�Ŵ��ڵ���������ʼ��ˮ�� */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	memset( &tmpstr,0x0, strlen( tmpstr ));
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and end_date=%ld and intst_type in ('3') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "�������" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
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
			sprintf(acErrMsg,"��ȡ�����Ϣ�Ǽǲ��쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		del_ind++;	
		/*ȡ�����Ϣ*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*��ȡ�����Ϣ*/
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
								
		/* �����任ʱ */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
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
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
						
			/* ɾ��ԭ�ļ� */
			if( del_ind==0 ) 
			{
				ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
				ERR_DEAL
			}
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		} 
					  
    		/* ��ӡ��Ϣ */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		
		/* �ܼ���Ϣ */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
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
	Ln_acm_reg_Clo_Sel();	
	/* ����������Ϣ�嵥 */
	/* ����:��ˮ�Ŵ��ڵ���������ʼ��ˮ�� */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	memset( &tmpstr,0x0, strlen( tmpstr ));
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and end_date=%ld and intst_type in ('4') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "��������" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
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
			sprintf(acErrMsg,"��ȡ�����Ϣ�Ǽǲ��쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}	
		del_ind++;
		/*ȡ�����Ϣ*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*��ȡ�����Ϣ*/
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
									
		/* �����任ʱ */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
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
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
				
			if( del_ind==0 ) 
			{			
				/* ɾ��ԭ�ļ� */
				ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
				ERR_DEAL
			}
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		} 
					  
    		/* ��ӡ��Ϣ */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		
		/* �ܼ���Ϣ */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
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
	Ln_acm_reg_Clo_Sel();	
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ����Ϣ�嵥�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɴ����Ϣ�嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnjxqd);
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
int get_ratio_lnjxqd( bli,bll,str )
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
	days= pub_base_CalTrueDays( s_ln_acm_reg.beg_date,s_ln_acm_reg.end_date);	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , qry_date/10000 , qry_date%10000/100 , qry_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_ln_acm_reg.br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_acm_reg.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_acm_reg.br_no); 
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
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G': /* ���� */						
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* ��Ϣ��� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* ����� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);

			break;
		/* case 'J': ��Ʒ 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		*/
		case 'J': /* ��Ʒ���� */			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, s_ln_parm.title ); 
			break;
		case 'K': /* �������� */			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, intst_type_name ); 
			break;
		case 'L': /* ���� */
			if(ret)	return 1;		
			sprintf( str, "%8.5lf", s_ln_acm_reg.rate ); 
			break;
		case 'O': /* ״̬ */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, clear_flg);
			break;

		case 'X': /* ��ʼ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.beg_date); 
			break; 
		case 'Y': /* ��ֹ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.end_date); 
			break; 	
		case 'Z': /* ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, days); 
			break; 
		case 'M': /* �����ܼ� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt); 
			break; 
		case 'N': /* ����ܼ� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tol_jx_amt ); 
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
