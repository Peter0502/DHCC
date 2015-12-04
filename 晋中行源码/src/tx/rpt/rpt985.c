/*************************************************************
* �� �� ��:  rpt985.c
* ���������� �жһ�Ʊ�����嵥
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
#include "mo_bank_acc_po_c.h"
#define  PAGE_CNT   43

struct mo_bank_acc_po_c	s_mo_bank_acc_po;
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

rpt985( )
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

	g_pub_tx.tx_date = s_com_sys_parm.sys_date;
/*
	strcpy( filename , "BRcdhpdqqd" );	
	strcpy( vpfm, "cdhpdqqd" );
*/
	strcpy( filename , "RPT985");
	strcpy( vpfm, "RPT985");
	ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, " matr_date=%ld  ORDER BY tx_br_no,req_date,payer_ac_no ", g_pub_tx.tx_date );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Mo_bank_acc_po_Fet_Sel( &s_mo_bank_acc_po,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ��ҳβ */
				ret = print_tail( );
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
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}

		pub_base_strpack( s_mo_bank_acc_po.tx_br_no );
		pub_base_strpack( old_br_no );

		/* �����任ʱ */
		if( strcmp( s_mo_bank_acc_po.tx_br_no,old_br_no ) )
		{
			if( strcmp(old_br_no,"AAAAA") )			
			{
				/* ��ӡҳβ */
				ret = print_tail( );
				ERR_DEAL
				fclose(fp);
			}

			strcpy( old_br_no,s_mo_bank_acc_po.tx_br_no );
			
			page = 1;
			line_file = 1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_mo_bank_acc_po.tx_br_no , filename , 0 );
			ERR_DEAL

			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_mo_bank_acc_po.tx_br_no , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head( );
			ERR_DEAL
		}

    	/* ��ӡ������Ϣ */
		ret = print_body( );
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

	Mo_bank_acc_po_Clo_Sel( );
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
	struct mdm_ac_rel_c s_mdm_ac_rel;

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
			if(strlen(s_mo_bank_acc_po.tx_br_no))	/** gyf.20060815 **/
			ret = pub_base_getbrname( s_mo_bank_acc_po.tx_br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_mo_bank_acc_po.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'L': /* Э���� */
			sprintf( t_str , "%s" , s_mo_bank_acc_po.pact_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'U': /* ���� */
			pub_base_strpack( s_mo_bank_acc_po.payer_name ); 
			sprintf( t_str , "%s" , s_mo_bank_acc_po.payer_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'R': /* Ʊ���� */
			sprintf( amt_tmp, "%.2lf" , s_mo_bank_acc_po.sign_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_mo_bank_acc_po.sign_amt ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'T': /* �������� */
			sprintf( t_str , "%4d%02d%02d" , s_mo_bank_acc_po.matr_date/10000 , s_mo_bank_acc_po.matr_date%10000/100 , s_mo_bank_acc_po.matr_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
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
