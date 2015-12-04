/*************************************************************
* �� �� ��:  rpt988.c
* ���������� ����ֽ�֧ȡ�����ӡ
* ��    ��:  
* �������:  2003��08��15��
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
#include "com_sys_parm_c.h"
#include "trace_log_rz_c.h"
#include "tx_def_c.h"
#define  AMT_LMT     100000.00
#define  PAGE_CNT    43

struct trace_log_rz_c	s_trace_log_rz;
int flag=0;								/*** 1-ҳĩ ***/
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE *fp; 
int ret = 0;
double  amt=0.00;
double  all_amt=0.00;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt988( )
{
	int    ttlnum;
	char   filename[100];
	char   old_br_no[6];
	long   tmp_date;

	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy( old_br_no, "AAAAA" );

	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
    
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
/*
	strcpy( filename , "BMdexjzqbb" );	
	strcpy( vpfm, "dexjzqbb" );
*/
	strcpy( filename , "RPT988");
	strcpy( vpfm, "RPT988");
	tmp_date = g_pub_tx.tx_date/100*100+1;

	vtcp_log("tmp_date======[%ld], \
				g_pub_tx.tx_date=====[%ld], \
				AMT_LMT=========[%.2lf]",
				tmp_date,g_pub_tx.tx_date,AMT_LMT);

	ret = Trace_log_rz_Dec_Sel( g_pub_tx.reply, "tx_date>=%ld and tx_date<=%ld and cur_no='01' and ct_ind='1' and add_ind='0' and amt>%lf ORDER BY tx_br_no,tx_date,ac_no",tmp_date,g_pub_tx.tx_date,AMT_LMT );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Trace_log_rz_Fet_Sel( &s_trace_log_rz,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				flag = 2;
				ret = print_tail( );
				ERR_DEAL
			}
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����ˮ!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}

		if( s_trace_log_rz.prdt_no[0]!='1' && s_trace_log_rz.prdt_no[0]!='2' )
		continue;

		pub_base_strpack( s_trace_log_rz.tx_br_no );
		pub_base_strpack( old_br_no );

		/* �����任ʱ */
		if( strcmp( s_trace_log_rz.tx_br_no,old_br_no ) )
		{
			if( strcmp(old_br_no,"AAAAA") )			
			{
				/* ��ӡҳβ */
				flag = 2;
				ret = print_tail( );
				flag = 0;
				all_amt = 0.00 ;
				ERR_DEAL
			}

			strcpy( old_br_no,s_trace_log_rz.tx_br_no );
			
			page = 1;
			line_file = 1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_trace_log_rz.tx_br_no , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_trace_log_rz.tx_br_no , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL
		}

		all_amt = all_amt + s_trace_log_rz.amt ;
		amt = amt + s_trace_log_rz.amt ;

    	/* ��ӡ������Ϣ */
		ret = print_body( );
		ERR_DEAL

		line_file++;

		if( line_file>PAGE_CNT )			
		{
			line_file=0;
			page++;

			/* ��ӡҳβ */
			flag = 1;
			ret = print_tail();
			flag = 0;
			amt = 0.00;
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}

	Trace_log_rz_Clo_Sel( );
	fclose(fp);
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ձ������ɴ���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�ձ������ɴ���ʧ��!!!!![%s]",g_pub_tx.reply);
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

/* ���ڻ�ҳ��ӡ���� */
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

/* ҳĩ��ӡ��β */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"NN" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/* ���ڴ�ӡ���� */
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
	struct tx_def_c s_tx_def;

	long cif_no;
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[41];
	char amt_tmp[41]; 
	char tmp_inf[41];
	char tmp_str[41];
	char fmt[20]; 
	char br_name[71];
	char prdt_name[31];
	char ac_name[51];
	double tmp_amt;
	int i=0; 

	switch( bli ) 
	{ 
		case 'A': /* �������� */ 
			sprintf( t_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* �������� */
			ret = pub_base_getbrname( s_trace_log_rz.tx_br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_trace_log_rz.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'S': /* �˺� */
			pub_base_strpack( s_trace_log_rz.ac_no ); 
			sprintf( t_str , "%s" , s_trace_log_rz.ac_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'T': /* ���� */
			ret = pub_base_acgetname( s_trace_log_rz.ac_no,&cif_no,ac_name );
			ERR_DEAL
			pub_base_strpack( ac_name ); 
			sprintf( t_str , "%s" , ac_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Q': /* ���׽�� */
			sprintf( amt_tmp, "%.2lf" , s_trace_log_rz.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_trace_log_rz.amt ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* ժҪ */
			pub_base_strpack( s_trace_log_rz.brf ); 
			sprintf( t_str , "%s" , s_trace_log_rz.brf ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'X': /* �������� */
			ret = Tx_def_Sel( g_pub_tx.reply, &s_tx_def, "tx_code='%s'", \
							  s_trace_log_rz.tx_code );
			ERR_DEAL

			pub_base_strpack( s_tx_def.tx_name ); 
			sprintf( t_str , "%s" , s_tx_def.tx_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'L': /* �������� */
			sprintf( t_str , "%4d%02d%02d" , s_trace_log_rz.tx_date/10000 , s_trace_log_rz.tx_date%10000/100 , s_trace_log_rz.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'U': /* ��Ʒ���� */
			pub_base_strpack( s_trace_log_rz.prdt_no ); 
			ret = pub_base_prdt_name( s_trace_log_rz.prdt_no, prdt_name );
			ERR_DEAL

			pub_base_strpack( prdt_name ); 
			sprintf( t_str , "%s" , prdt_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'V': /* ����� */
			pub_base_strpack( s_trace_log_rz.tx_br_no ); 
			sprintf( t_str , "%s" , s_trace_log_rz.tx_br_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'K':	/* ÿҳ���� */
			if( flag==1 )
			{
				strcpy( tmp_str, "С  ��");
				sprintf( t_str , "%s" , tmp_str ); 
			}
			else if( flag==2 )
			{
				strcpy( tmp_str, "��  ��");
				sprintf( t_str , "%s" , tmp_str ); 
			}

			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'P':   /* С�ƽ�� */
			vtcp_log("flag ===============>>>>>>[%d], \
						amt===============>>>>>>[%.2lf], \
						all_amt===========>>>>>>[%.2lf]", \
					 	flag,amt, all_amt );
			if( flag==1 )
			{
				sprintf( amt_tmp, "%.2lf" , amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				if( strlen(amt_tmp)>bll ) 
					sprintf( amt_tmp, "%.2lf" , amt ); 
				sprintf(fmt,"%%%ds",bll);
				sprintf( str, fmt, amt_tmp ); 
			}
			else if( flag==2  )
			{
				sprintf( amt_tmp, "%.2lf" , all_amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				if( strlen(amt_tmp)>bll ) 
					sprintf( amt_tmp, "%.2lf" , all_amt ); 
				sprintf(fmt,"%%%ds",bll);
				sprintf( str, fmt, amt_tmp ); 
			}
			break;
		   case 'Z': /* ��ת��־ */
				   if(s_trace_log_rz.ct_ind[0]=='1')   /**�ֽ�**/
						   strcpy(t_str,"�ֽ�");
				   else if(s_trace_log_rz.ct_ind[0]=='2')  /**ת��**/
						   strcpy(t_str,"ת��");
				   sprintf(fmt, "%%-%ds", bll);
				   sprintf(str, fmt, t_str);
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
