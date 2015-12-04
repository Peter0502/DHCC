/*************************************************************
* �� �� ��: rpt012.c
* ����������ƾ֤ʹ����ˮ�嵥
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
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "note_parm_c.h"

struct note_mst_hst_c	s_note_mst_hst;
struct com_sys_parm_c	s_com_sys_parm;
struct note_parm_c	s_note_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_pzsyls(int bli,int bll,char str[100]);

rpt012()
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
	memset( &s_note_mst_hst , 0x00 , sizeof(struct note_mst_hst_c));
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_note_parm , 0x00 , sizeof(struct note_parm_c));
			
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
	strcpy( filename , "BRpzsyls" );	
	strcpy(vpfm, "pzsyls" );
	*/	
	strcpy( filename , "RPT012");
	strcpy(vpfm, "RPT012");
	/* ����: ��������Ϊ��һ�������� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	sprintf( tmpstr," tx_date=%ld and " , s_com_sys_parm.lst_date );
	strcat( wherelist,tmpstr );

	/**��ɲ�ѯ**/	
	strcat(wherelist," 1=1 ORDER BY br_no,note_type,beg_note_no,end_note_no,tel");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Note_mst_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Note_mst_hst_Fet_Sel( &s_note_mst_hst, g_pub_tx.reply );
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
			sprintf(acErrMsg,"��ȡ ��ˮ��Ϣ���쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* �����任ʱ */
		pub_base_strpack(s_note_mst_hst.br_no );
		if( strcmp( s_note_mst_hst.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_note_mst_hst.br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(s_note_mst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_note_mst_hst.br_no , filename );
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
	Note_mst_hst_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ƾ֤ʹ�ý����嵥�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ƾ֤ʹ����ˮ�嵥ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_pzsyls);
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
int get_ratio_pzsyls( bli,bll,str )
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
			sprintf( t_str , "%4d��%2d��%2d��" , s_note_mst_hst.tx_date/10000 , s_note_mst_hst.tx_date%10000/100 , s_note_mst_hst.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_note_mst_hst.br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_note_mst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_note_mst_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ƾ֤���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_note_mst_hst.note_type); 
			break; 
		case 'F': /* ƾ֤��ʼ���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.beg_note_no);
			break;
		case 'G': /* ƾ֤��ֹ���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.end_note_no);
			break;
		case 'H': /* ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt , s_note_mst_hst.note_cnt);
			break;
		case 'I': /* ���׻��� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.br_no);
			break;
		case 'J': /* ���׹�Ա */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_note_mst_hst.tel);
			break;
		case 'K': /* �Է����� */			
			sprintf(fmt, "%%-%ds", bll);		
			sprintf( str, fmt, s_note_mst_hst.tw_br_no ); 
			break;
		case 'L': /* �Է���Ա */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_note_mst_hst.tw_tel);
			break;
		case 'M': /* �������� */
			pub_base_dic_show_str(tmp_inf,"note_act",s_note_mst_hst.sts_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'Q': /* ����ʱ�� */ 			
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt , s_note_mst_hst.tx_time);
			break;
		case 'R': /* ƾ֤���� */
			ret = Note_parm_Sel(g_pub_tx.reply, &s_note_parm, "note_type='%s'",
		s_note_mst_hst.note_type);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯƾ֤���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, s_note_parm.name);
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
