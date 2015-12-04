/*************************************************************
* �� �� ��: rpt996.c
* �����������Թ�����Ϣ�嵥��ӡ
*
* ��    ��: rob
* �������: 2003��08��15��
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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "rpt_note_inf_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "intst_hst_c.h"

#define EXTERN

static	char    g_brno[10];         	 

struct intst_hst_c	s_intst_hst;
int page=0;
int line=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_dglsqd(int bli,int bll,char str[100]);

rpt996()
{
	int    ret;
	int    ttlnum;
	char   flag ;
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

	ret = Trace_log_Dec_Sel( g_pub_tx.reply, "1=1 ORDER BY tx_br_no,opn_br_no,sts,trace_no,trace_cnt" );
	ERR_DEAL

	while(1)
	{
		ret = Trace_log_Fet_Sel( &s_intst_hst,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����ˮ!" );
				WRITEMSG
			}
			break;
		}
		else 
			ERR_DEAL

		/* �����任ʱ */
		pub_base_strpack(s_intst_hst.tx_br_no );
		if( strcmp( s_intst_hst.tx_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
			}
			
			strcpy( old_br_no , s_intst_hst.tx_br_no );
			
			page=1;
			line=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_intst_hst.tx_br_no , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_intst_hst.tx_br_no , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL
		}

    	/* ��ӡ��Ϣ */
		ret = print_body();
		ERR_DEAL

		line++;

		if( line>50 )	/* ����50�л�ҳ */
		{
			line=0;
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

	Trace_log_Clo_Sel( );

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
	fclose(fp);
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
	char br_name[71];
	int i=0; 

	switch( bli ) 
	{ 
		case 'A': /* �������� */
			sprintf( t_str , "%4d��%2d��%2d��" , s_intst_hst.tx_date/10000 , s_intst_hst.tx_date%10000/100 , s_intst_hst.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_intst_hst.tx_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = pub_base_getbrname( s_intst_hst.tx_br_no, br_name );
			if( ret )
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_intst_hst.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ��ˮ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_intst_hst.trace_no); 
			break; 
		case 'F': /* ����Ա */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.tel);
			break;
		case 'G': /* ����Ա,��Ȩ�� */
			if( strlen(s_intst_hst.chk) )
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt , s_intst_hst.chk );
			}
			else if( strlen(s_intst_hst.aut) )
			{
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt , s_intst_hst.aut );
			}
			else 
				strcpy( str,"" );
			break;
		case 'H': /* ������ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.tx_code);
			break;
		case 'I': /* ��      �� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.ac_no);
			break;
		case 'J': /* �˻���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_intst_hst.ac_seqn);
			break;
		case 'K': /* ��  ��  �� */
			/* �Ӹ��� */
			if( s_intst_hst.add_ind[0]=='0' )
			{
				/* �Ӷ��� */
				strcpy( tmp_inf , "-" );			
				sprintf( amt_tmp, "%.2lf" , s_intst_hst.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				/* �Ӹ��� */
				strcat( tmp_inf , amt_tmp); 							
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, tmp_inf ); 
			}
			else
			{							
				/* �Ӷ��� */
				sprintf( amt_tmp, "%.2lf" , s_intst_hst.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				
				sprintf(fmt,"%%%ds",bll); 		
				sprintf( str, fmt, amt_tmp ); 
			}
			break;
		case 'L': /* ��Ʒ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_intst_hst.prdt_no);
			break;
		case 'M': /* ��ת */
			pub_base_dic_show_str(tmp_inf,"rpt_ct_ind",s_intst_hst.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'N': /* ������ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_intst_hst.opn_br_no);
			break;
		case 'O': /* ״̬ */
			pub_base_dic_show_str(tmp_inf , "rpt_log_sts" , s_intst_hst.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt , tmp_inf);
			break;
		case 'P': /* �������� */ 
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_intst_hst.tx_date);
			break;
		case 'Q': /* ����ʱ�� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_intst_hst.tx_time);
			break;
		case 'R': /* ��ˮ�ʴ� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt , s_intst_hst.trace_cnt);
			break;		
		case 'S': /* ժҪ */
			sprintf(fmt, "%%-%ds", bll);	
			sprintf(str, fmt, s_intst_hst.brf);
			break;
		case 'T': /* ��� */
			pub_base_dic_show_str(tmp_inf,"accdc_ind",s_intst_hst.add_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
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
