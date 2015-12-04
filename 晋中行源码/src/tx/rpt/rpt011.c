/*************************************************************
* �� �� ��: rpt011.c
* �����������Թ�������ˮ�嵥
*
* ��    ��: lance
* �������: 2003��07��13��
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
#include "trace_log_bk_c.h"
#include "com_branch_c.h"

struct trace_log_bk_c	s_trace_log_bk;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_dglsqd(int bli,int bll,char str[100]);

rpt011()
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
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	
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
	
	strcpy( filename , "BRdglsqd" );	
	strcpy(vpfm, "dglsqd" );
	
	/* ����: ���׻���=�������� */
	strcpy( tmpstr," tx_br_no=opn_br_no and ");
	strcat( wherelist,tmpstr );
		
	/* ����: �Թ��˻�*/
	strcpy( tmpstr," ac_no[1]='5' and ");
	strcat( wherelist,tmpstr );


	/**��ɲ�ѯ**/	
	strcat(wherelist," no_show!='1' and 1=1 ORDER BY tx_br_no,opn_br_no,sts,trace_no,trace_cnt");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
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
		
		/* �����任ʱ */
		pub_base_strpack(s_trace_log_bk.tx_br_no );
		if( strcmp( s_trace_log_bk.tx_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
    		/* ��ӡ��Ϣ */
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
			TRACE
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
			TRACE
		}
		ttlnum++;
	}		
	Trace_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɶԹ���ˮ�����嵥�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɶԹ�������ˮ�嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_dglsqd);
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
	TRACE
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	TRACE
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
int get_ratio_dglsqd( bli,bll,str )
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
			sprintf( t_str , "%4d��%2d��%2d��" , s_trace_log_bk.tx_date/10000 , s_trace_log_bk.tx_date%10000/100 , s_trace_log_bk.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_trace_log_bk.tx_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_trace_log_bk.tx_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_trace_log_bk.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ��ˮ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_no); 
			break; 
		case 'F': /* ����Ա */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.tel);
			break;
		case 'G': /* ����Ա */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.chk);
			break;
		case 'H': /* ������ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.tx_code);
			break;
		case 'I': /* ��      �� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_trace_log_bk.ac_no);
			break;
		case 'J': /* �˻���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_trace_log_bk.ac_seqn);
			break;
		case 'K': /* ��  ��  �� */
			/* �Ӹ��� */
			if( s_trace_log_bk.add_ind[0]=='0' )
			{
				/* �Ӷ��� */
				strcpy( tmp_inf , "-" );			
				sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				/* �Ӹ��� */
				strcat( tmp_inf , amt_tmp); 							
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, tmp_inf ); 

			}else{							
				/* �Ӷ��� */
				sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, amt_tmp ); 
			}
			break;

		case 'L': /* ��Ʒ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_trace_log_bk.prdt_no);
			break;
		case 'M': /* ��ת */
			pub_base_dic_show_str(tmp_inf,"rpt_ct_ind",s_trace_log_bk.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'N': /* ������ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_trace_log_bk.opn_br_no);
			break;
		case 'O': /* ״̬ */
			pub_base_dic_show_str(tmp_inf , "rpt_log_sts" , s_trace_log_bk.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt , tmp_inf);
			break;
		case 'P': /* �������� */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_trace_log_bk.tx_date);
			break;
		case 'Q': /* ����ʱ�� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_trace_log_bk.tx_time);
			break;
		case 'R': /* ��ˮ�ʴ� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_trace_log_bk.trace_cnt);
			break;		
		case 'S': /* ժҪ */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, s_trace_log_bk.brf);
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
