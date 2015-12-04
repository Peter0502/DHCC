/*************************************************************
* �� �� ��: rpt026.c
* �����������������ˮ�嵥(����ת����ÿ��ִ��,����ˮ��ʷ��trace_log_bk��)
*
* ��    ��: lance
* �������: 2003��09��05��
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
#include "ln_mst_hst_c.h"
#include "ln_parm_c.h"                  /*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/
#include "dc_acc_rel_c.h"								/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/
#include "com_item_c.h"									/*Ҫ�󽫱����С���Ʒ������Ϊ����Ŀ�š���zhangpeng 20131125*/


struct trace_log_bk_c s_trace_log_bk;
struct ln_mst_hst_c s_ln_mst_hst;
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

int get_ratio_lndqqd(int bli,int bll,char str[100]);

rpt026()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	
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
/*	
	strcpy( filename , "BRlndqqd" );	
	strcpy(vpfm, "lndqqd" );
*/
	strcpy( filename , "RPT026");
	strcpy(vpfm, "RPT026");
	/* ����:���״���Ϊ"G088" */
	strcpy( tmpstr,"tx_code='G088'and prdt_no[1]='3' and " );
	strcat( wherelist,tmpstr );	
	/**��ɲ�ѯ**/

	strcat(wherelist," no_show!='1' and 1=1  ORDER BY tx_br_no, prdt_no, trace_no, trace_cnt") ;

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
			sprintf(acErrMsg,"��ȡ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		if( s_trace_log_bk.trace_no==old_trace_no )	/*��ˮ��δ�ı�*/
		{
			continue;
		}
		
		old_trace_no= s_trace_log_bk.trace_no;
		
		/*ȡ�����Ϣ*/
		ret = pub_ln_lndis_file(s_trace_log_bk.ac_no,s_trace_log_bk.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
		if( ret ) 
		{
			sprintf(acErrMsg,"��ȡ�����Ϣerror");
			WRITEMSG
			return ret;
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
	sprintf(acErrMsg,"���ɴ������ˮ�嵥(����ת����)�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɴ������ˮ�嵥(����ת����)ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lndqqd);
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
int get_ratio_lndqqd( bli,bll,str )
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
		case 'G': /* �����˺� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.ac_no);
			break;
		case 'H': /* �˺���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.ac_seqn);
			break;
		case 'I': /* ����� */
			sprintf(fmt, "%%-%ds", bll);
			char tmp_name[51];
			memset(tmp_name,'\0',sizeof(tmp_name));
			memcpy(tmp_name,g_ln_mst.name,50);
			sprintf( str, fmt, g_ln_mst.name);
			break;
		/**��Ʒ�Ͳ�Ʒ����ע�͵����ӿ�Ŀ�š�zhangpeng 20131125 start**
		case 'J': //��Ʒ
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, g_ln_mst.prdt_no);
			break;
		case 'K': //��Ʒ���� 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, g_ln_parm.title);
			break;
		*��Ʒ�Ͳ�Ʒ����ע�͵����ӿ�Ŀ�š�zhangpeng 20131125 end***/
		
		/**���ӿ�Ŀ�š�zhangpeng 20131125 start***/
		case 'J': /*��Ŀ��*/			
			ret=Ln_parm_Sel( g_pub_tx.reply ,&s_ln_parm,"prdt_no = '%s'",g_ln_mst.prdt_no);
			if( ret )
			{
				return 1;
			}
			ret=Dc_acc_rel_Sel( g_pub_tx.reply ,&s_dc_acc_rel,"dc_code = '%s'",s_ln_parm.dc_code);
			if( ret )
			{
				return 1;
			}
			ret=Com_item_Sel( g_pub_tx.reply ,&s_com_item,"acc_hrt = '%s'",s_dc_acc_rel.acc_hrt);
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
		case 'L': /* ������ */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, g_ln_mst.opn_br_no);
			break;
		case 'M': /* �������� */
			if( g_ln_mst.exp_ind[0]=='0' )
			{
				sprintf( mtr_date, "%ld", g_ln_mst.mtr_date );
			}else{					
				sprintf( mtr_date, "%ld(չ)", g_ln_mst.exp_mtr_date );
			}		
			sprintf(fmt,"%%-%ds",bll);				
			sprintf(str, fmt, mtr_date);
			break;	
		case 'N': /* ��Ƿ���� */						
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , g_ln_mst.bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* ״̬ */
			pub_base_dic_show_str(tmp_inf,"ln_ac_sts",g_ln_mst.ac_sts);
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
