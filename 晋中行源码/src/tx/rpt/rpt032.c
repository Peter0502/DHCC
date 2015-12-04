/*************************************************************
* �� �� ��: rpt032.c
* ��������������ҵ����ˮ�嵥(��Ʊǩ����ע���ͻ�ƾ֤��ע����Աƾ֤��ƾ֤���ϡ�ת�浥����)
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
#include "trace_log_bk_c.h"
#include "com_branch_c.h"
#include "menu_c.h"
#include "note_parm_c.h"
#include "note_mst_hst_c.h"
#include "note_cheq_hst_c.h"

static	struct trace_log_bk_c s_trace_log_bk;
static	struct menu_c	s_menu;
static	struct note_parm_c	s_note_parm;
static	struct note_mst_hst_c	s_note_mst_hst;
static	struct note_cheq_hst_c	s_note_cheq_hst;

static	char end_note_no[17];
static	long note_cnt=0;
static	int page=0;
static	int line=0;
static	int line_file=0;
static	char vpfm[21];
static	char t_str[41];
static	int opncnt;
static	FILE    *fp; 
static	int ret=0;

static	int get_ratio_tsjylsqd(int bli,int bll,char str[100]);

rpt032()
{
	int flag=0;
	int ttlnum;
	long qry_date=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	
	char o_tx_code[5];/*ԭ���׺� 2009/12/17 16:23:51 add by martin */
	char o_note_type[4];/*ԭƾ֤�� 2009/12/17 16:23:51 add by martin */
	
	
	memset( wherelist,0,sizeof(wherelist) );
	memset(&o_tx_code, 0, sizeof(o_tx_code) );
	memset(&o_note_type, 0, sizeof(o_note_type) );
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	
	strcpy( old_br_no , "*****" );
	/*���㽻������*/
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
/*	
	strcpy( filename , "BRtsjylsqd" );	
	strcpy(vpfm, "tsjylsqd" );
*/	
	strcpy( filename , "RPT032");
	strcpy(vpfm, "RPT032");
	/* ����:���״���Ϊ2401,5106,5110,5111,4142 */
	
	sprintf( tmpstr," tx_code in('2401','5106','5110','5111','4142') and tx_date=%ld and trace_cnt=1 and " , qry_date );
	strcat( wherelist,tmpstr );	
	/**��ɲ�ѯ**/

	strcat(wherelist," 1=1 ORDER BY tx_br_no, tx_code, trace_no,tel ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
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
			sprintf(acErrMsg,"��ȡ��ˮ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}	
					
		/* �����任ʱ */
		pub_base_strpack( s_trace_log_bk.tx_br_no );
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
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		if( strcmp( s_trace_log_bk.tx_code,"4142")==0 )
		{
			TRACE
			if( s_trace_log_bk.add_ind[0]=='0' )
			{
				continue;
			}
		}
		
		{/*modify by martin 2009/12/18 20:19:31 �����ط�*/
			
			memset(&s_menu, 0x0, sizeof(struct menu_c));
			memset(&s_note_parm, 0x0, sizeof(struct note_parm_c));
			memset(&s_note_mst_hst, 0x0, sizeof(struct note_mst_hst_c));
			memset(&s_note_cheq_hst, 0x0, sizeof(struct note_cheq_hst_c));
					/*ȡ��������*/
			if( strcmp( o_tx_code,s_trace_log_bk.tx_code) )
			{
				ret = Menu_Sel( g_pub_tx.reply , &s_menu,"code='%s' ",s_trace_log_bk.tx_code ); 
				if( ret ) 
				{
					sprintf(acErrMsg,"��ȡ����������Ϣerror");
					WRITEMSG
					return ret;
				}
				strcpy(o_tx_code,s_trace_log_bk.tx_code);
			}
			/*ȡ��������*/
			if( strcmp( o_note_type,s_trace_log_bk.note_type) )
			{
				ret = Note_parm_Sel( g_pub_tx.reply , &s_note_parm,"note_type='%s' ",s_trace_log_bk.note_type ); 
				if( ret ) 
				{
					sprintf(acErrMsg,"��ȡ����������Ϣerror");
					WRITEMSG
					return ret;
				}
				strcpy(o_note_type,s_trace_log_bk.note_type);
			}
			if( strcmp( s_trace_log_bk.tx_code,"5111") ) 
			{
				ret= Note_mst_hst_Sel( g_pub_tx.reply,&s_note_mst_hst,"trace_no=%ld and tx_date=%ld",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date);
				if(ret)	
				{
					sprintf(acErrMsg,"��ƾ֤��ϸ�����ret=[%ld]!",ret);
					WRITEMSG					
					sprintf(acErrMsg,"trace_no=[%ld],tx_date=[%ld],note_no=[%s]!",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date,s_trace_log_bk.note_no);
					WRITEMSG
					return ret;
				}
				strcpy( end_note_no ,s_note_mst_hst.end_note_no);
				note_cnt=s_note_mst_hst.note_cnt;
			}else{
				ret= Note_cheq_hst_Sel( g_pub_tx.reply,&s_note_cheq_hst,"trace_no=%ld and tx_date=%ld",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date);
				if(ret)	
				{
					sprintf(acErrMsg,"���ͻ�ƾ֤��ϸ�����ret=[%ld]!",ret);
					WRITEMSG					
					sprintf(acErrMsg,"trace_no=[%ld],tx_date=[%ld],note_no=[%s]!",s_trace_log_bk.trace_no,s_trace_log_bk.tx_date,s_trace_log_bk.note_no);
					WRITEMSG
					return ret;
				}
				strcpy( end_note_no ,s_note_cheq_hst.end_note_no);
				note_cnt=s_note_cheq_hst.cnt;
			}	
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
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Trace_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ҵ����ˮ�嵥�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��������ҵ����ˮ�嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tsjylsqd);
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
int get_ratio_tsjylsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char amt_tmp[21]; 
	char fmt[10]; 
	double tx_amt=0.00;
	
	int i=0; 
	struct com_branch_c sCom_branch;
	
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
			
	
	/* ���׽�� */
	if( strcmp( s_trace_log_bk.tx_code,"4142") ) 
	{
		tx_amt= 0.00;
	}else{
		tx_amt= s_trace_log_bk.amt;
	}
							
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
			sprintf(str, fmt, s_trace_log_bk.trace_no); 
			break; 
		case 'F': /* ���״��� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.tx_code );
			break;
		case 'G': /* �������� */						
			sprintf(fmt, "%%-%ds", bll);		
			sprintf( str, fmt, s_menu.title ); 
			break;
		case 'H': /* ƾ֤���� */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf( str, fmt, s_note_parm.name ); 
			break;
		case 'I': /* ƾ֤���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.note_type);
			break;
		case 'J': /* ��ʼ���� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_trace_log_bk.note_no);
			break;
		case 'K': /* ��ֹ���� */		
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, end_note_no );		
			break;
		case 'M': /* ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, note_cnt); 			
			break;	
		case 'N': /* ���׽�� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , tx_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 			
			break;	
		case 'L': /* ��Ա */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_trace_log_bk.tel);
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
