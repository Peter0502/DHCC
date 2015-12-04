/*************************************************************
* �� �� ��: rpt014.c
* �������������������嵥
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mo_opn_cls_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct mo_opn_cls_c	s_mo_opn_cls;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt014()
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
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_mo_opn_cls , 0x00 , sizeof(struct mo_opn_cls_c));
			
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
/*	
	strcpy( filename , "BMcxxhqd" );	
	strcpy(vpfm, "cxxhqd" );
*/	
	strcpy( filename , "RPT014");
	strcpy(vpfm, "RPT014");
	/* ����: ��������Ϊ������������ */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	tmp_date = s_com_sys_parm.lst_date/100*100 + 1;
	sprintf( tmpstr," tx_date<=%ld and tx_date>=%ld and " ,s_com_sys_parm.lst_date, 
			 tmp_date );
	strcat( wherelist,tmpstr );

	/* ����: �����˻� */
	strcpy( tmpstr," cif_type in('1','5') and " );
	strcat( wherelist,tmpstr );
	
	/* ����: ���� */
	strcpy( tmpstr," wrk_sts='2' and " );
	strcat( wherelist,tmpstr );
		
	/**��ɲ�ѯ**/	
	strcat(wherelist," 1=1 ORDER BY opn_br_no,tx_brno,prdt_no,ac_no,ac_id");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Mo_opn_cls_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Mo_opn_cls_Fet_Sel( &s_mo_opn_cls, g_pub_tx.reply );
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
		pub_base_strpack(s_mo_opn_cls.opn_br_no );
		if( strcmp( s_mo_opn_cls.opn_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_mo_opn_cls.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_mo_opn_cls.opn_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_mo_opn_cls.opn_br_no , filename );
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
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Mo_opn_cls_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ������������嵥�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɴ���������ˮ�嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxxhqd);
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
int get_ratio_cxxhqd( bli,bll,str )
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
			sprintf( t_str , "%4d��%2d��%2d��" , s_com_sys_parm.lst_date/10000 , s_com_sys_parm.lst_date%10000/100 , s_com_sys_parm.lst_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_mo_opn_cls.opn_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_mo_opn_cls.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_mo_opn_cls.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ��     �� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_mo_opn_cls.ac_no); 
			break; 
		case 'F': /* �ͻ����� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.name);
			break;
		case 'G': /* ��  ��  ��  �� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_mo_opn_cls.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* ��Ʒ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.prdt_no);
			break;
		case 'I': /* ���׻��� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.tx_brno);
			break;
		case 'J': /* ���׹�Ա */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.tel);
			break;
		case 'K': /* �������� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, s_mo_opn_cls.tx_date ); 
			break;
		case 'R': /* ��� */
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt, s_mo_opn_cls.ac_seqn);
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
