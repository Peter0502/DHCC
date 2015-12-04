/*************************************************************
* �� �� ��: rpt970.c
* ���������������Ϣ�嵥
*
* ��    ��: rob
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "intst_hst_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct intst_hst_c	s_intst_hst;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt970()
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
	strcpy( filename , "BJcxjxqd" );	
	strcpy(vpfm, "cxjxqd" );
*/	
	strcpy( filename , "RPT970");
	strcpy(vpfm, "RPT970");
	ret = Intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='1' and pt_ind='1' and intst_date=%ld order by br_no ,prdt_no,ac_id",g_pub_tx.tx_date );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		ret = Intst_hst_Fet_Sel( &s_intst_hst , g_pub_tx.reply );
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
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack(s_intst_hst.br_no );
		if( strcmp( s_intst_hst.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			strcpy( old_br_no , s_intst_hst.br_no );
			page=1;
			line_file=1;
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_intst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_intst_hst.br_no , filename );
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
	Intst_hst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ����Ϣ�����嵥�ɹ�!![%s]",g_pub_tx.reply);
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
	long tmpdate=0;
	char tmpname[51];
	char tmpac_no[51];
	struct com_branch_c sCom_branch;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			sprintf( t_str , "%4d��%2d��%2d��" , tmpdate/10000 , tmpdate%10000/100 , tmpdate%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_intst_hst.br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",s_intst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_intst_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ��     �� */
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=1 ",s_intst_hst.ac_id);
			if( ret==100 )
			{
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=9999 ",s_intst_hst.ac_id);
			}
			if( ret )
			{
				sprintf(acErrMsg, "ȡ�˻�����! [%d][%d]", s_intst_hst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_mdm_ac_rel.ac_no); 
			break; 
		case 'M': /*����*/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ",s_intst_hst.ac_id,s_intst_hst.ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "ȡ��������! [%d][%d][%d]", s_intst_hst.ac_id,s_intst_hst.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , g_dd_mst.name); 
			break; 
		case 'O': /* ��Ϣǰ��� */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.bic_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /*��Ϣ����*/
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.bic_acm ); 
			/**pub_rpt_flttomon( amt_tmp,amt_tmp );**/
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* ��Ʒ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.prdt_no);
			break;
		case 'J': /* ˰ǰ��Ϣ */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.fct_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* ��Ϣ˰ */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.intst_tax ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* ˰����Ϣ */			
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.fct_intst-s_intst_hst.intst_tax ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		/***
		case 'R': * ��� *
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt, s_intst_hst.ac_seqn);
			break;		
		***/
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
