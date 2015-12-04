/*************************************************************
* �� �� ��: rpt017.c
* ����������ƾ֤��Ŀ�սᵥ
* ��    ��: lance
* �������: 2003��07��14��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: Azz
* �޸�����: �˶�����
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
#include "note_mst_hst_c.h"
#include "note_mst_c.h"
#include "note_parm_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct note_mst_c	s_note_mst;
struct com_branch_c	s_jg;
struct note_parm_c	s_zl;
struct note_mst_hst_c s_note_hst;


int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char note_name[21];
char t_str[41];

int opncnt;
FILE    *fp; 
int ret = 0;

long ys_cnt=0;			/* ���տ�� */
long in_cnt=0;			/* �������� */
long out_cnt=0;			/* ���ս��� */
long use_cnt=0;			/* ����ʹ�� */
long buy_cnt=0;			/* �ͻ����� */
long dsy_cnt=0;			/* �������� */
long cls_cnt=0;			/* �������� */
long lose_cnt=0;		/* ���չ�ʧ */
long de_cnt=0;			/*��ȷ��*/
long unlose_cnt=0;		/* ���ս�� */
long dd_cnt=0;			/* �������� */
long now_cnt=0;			/* ���ս�� */

long ys_cnt_tol=0;		/* ���տ���ܼ� */
long in_cnt_tol=0;		/* ���������ܼ� */
long out_cnt_tol=0;		/* ���ս����ܼ� */
long use_cnt_tol=0;		/* ����ʹ���ܼ� */
long buy_cnt_tol=0;		/* �ͻ������ܼ� */
long dsy_cnt_tol=0;		/* ���������ܼ� */
long cls_cnt_tol=0;		/* ���������ܼ� */
long lose_cnt_tol=0;	/* ���չ�ʧ�ܼ� */
long de_cnt_tol=0;		/* ��ȷ���ܼ�  */
long unlose_cnt_tol=0;	/* ���ս���ܼ� */
long dd_cnt_tol=0;		/* ���������ܼ� */	
long now_cnt_tol=0;		/* ���ս���ܼ� */
	
int get_ratio_pzrjd(int bli,int bll,char str[100]);

rpt017()
{
	int flag=0;
	int ttlnum;
	long tmp_cnt=0;
									
	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	char old_note_type[4];
	int jgi=0;
	char tmp_tel[3];
	
	memset( tmp_tel,0,sizeof(tmp_tel) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_note_mst , 0x00 , sizeof(struct note_mst_c));
	memset( &s_note_hst , 0x00 , sizeof(struct note_mst_hst_c));
				
	strcpy( old_br_no , "*****" );
   
	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	/* ɾ��ԭ�ļ� */
/*
	strcpy( filename , "BRpzrjd" );	
	strcpy(vpfm, "pzrjd" );
*/
	strcpy( filename , "RPT017");
	strcpy(vpfm, "RPT017");
	ret = pub_rpt_rmfile("" , filename , 0 );
	ERR_DEAL
	
	/* ����: ��������Ϊ��һ�������� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , \
							"sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	sprintf( tmpstr," tx_date=%ld and " , s_com_sys_parm.lst_date );
	strcat( wherelist,tmpstr );
			
	/**��ɲ�ѯ**/	
	strcat(wherelist," 1=1 ORDER BY br_no,note_type,sts_ind");

	vtcp_log( "WHERE [%s]",wherelist );
	
	ttlnum=0;

			page=1;
			line_file=1;

	ret = Com_branch_Dec_Sel( g_pub_tx.reply, "1=1 and wrk_sts!='*' and br_no not in('%s','%s') order by br_no",TOT_BR_NO,KJ_BR_NO);
	ERR_DEAL

	jgi=1;
	while(1)
	{
		ret = Com_branch_Fet_Sel( &s_jg, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"��ȡƾ̨֤����ϸ�쳣!");
			WRITEMSG			
			goto ErrExit;			
		}

vtcp_log("JGJGJG [%d]",jgi++ );
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp, s_jg.br_no , filename );
			ERR_DEAL

			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL


	ret = Note_parm_Dec_Sel( g_pub_tx.reply, "ctl_no_ind='Y' order by note_type" );
	ERR_DEAL

	while(1)
	{
		ret = Note_parm_Fet_Sel( &s_zl, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"��ȡƾ̨֤����ϸ�쳣!");
			WRITEMSG
			goto ErrExit;			
		}

   		/* ƾ֤���ͱ仯 */
		pub_base_strpack( s_zl.note_type );

			/* �������� */
			ys_cnt=0;		/* ���տ���ܼ� */
			in_cnt=0;		/* ���������ܼ� */
			out_cnt=0;		/* ���ս����ܼ� */
			use_cnt=0;		/* ����ʹ���ܼ� */
			buy_cnt=0;		/* �ͻ������ܼ� */
			dsy_cnt=0;		/* ���������ܼ� */
			cls_cnt=0;		/* ���������ܼ� */
			lose_cnt=0;		/* ���չ�ʧ�ܼ� */
			unlose_cnt=0;	/* ���ս���ܼ� */
			de_cnt=0; /*��ȷ���ܼ�*/
			dd_cnt=0;		/* ���������ܼ� */			
			now_cnt=0;		/* ���ս���ܼ� */		
		
		
		  /** ����152���� 20080125 add by martin **/
		  if(memcmp(s_zl.note_type,"152",3)==0 )
    { 
    	memcpy(tmp_tel,s_jg.br_no+1,2);
      ret = sql_sum_long( "note_mst" , "cnt" , &tmp_cnt , \
					"tel like '%s%%' and note_type='%s' and sts in('0','1') " , \
					tmp_tel,s_zl.note_type);
			ERR_DEAL
    }else
    {
		  /** ����152���� end**/
     		/* ͳ�ƽ��ս�� */
			ret = sql_sum_long( "note_mst" , "cnt" , &tmp_cnt , \
					"br_no='%s' and note_type='%s' and sts in('0','1') " , \
					s_jg.br_no,s_zl.note_type);
			ERR_DEAL
		}

     		now_cnt = tmp_cnt;
   	    		
     		/* ƾ֤���� */					
			strcpy( note_name,s_zl.name );
			ERR_DEAL


		/* �������� */
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind in('1','3','5','9','A')" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL			

		in_cnt=tmp_cnt;

		/* ���ս��� */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt ,
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind in('2','4','7','8')" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		

		out_cnt=tmp_cnt;
		
		/**  152ʹ�ô���  20080125 add by martin **/
		if(memcmp(s_zl.note_type,"152",3)==0)
    {
    	if( memcmp(s_jg.br_no,"10001",5)!=0 )
    		{
          ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			      "br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='D'" , 
			      s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
			      ERR_DEAL
			  }
			
			ret = Note_mst_hst_Dec_Sel( g_pub_tx.reply, "br_no='10001' and tx_date=%ld \
			and note_type='%s' and sts_ind ='D'",s_com_sys_parm.lst_date,s_zl.note_type );
	    ERR_DEAL
			while(1)
			{
			    ret = Note_mst_hst_Fet_Sel( &s_note_hst, g_pub_tx.reply );
		       if( ret==100 ) break;
		       if( ret )
		       {
		       	sprintf(acErrMsg,"��ȡƾ̨֤����ϸ�쳣!");
		       	WRITEMSG
		       	goto ErrExit;			
		       }
		       ret = Note_mst_Sel(g_pub_tx.reply,&s_note_mst,
		       " beg_note_no<=%s and end_note_no>=%s and tel like '%s%%' and note_type='%s' ",
		       s_note_hst.beg_note_no,s_note_hst.end_note_no,tmp_tel,s_zl.note_type);
		       if( ret==100 ) continue;
		       if( ret )
		       {
		       	sprintf(acErrMsg,"��ȡƾ̨֤���쳣!");
		       	WRITEMSG
		       	goto ErrExit;			
		       }
		       else
		       {
		        tmp_cnt++;
		       }
		       
			}
    }else{
		/**  152ʹ�ô��� end **/
		/* ����ʹ�� */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='D'" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		
	  }

		use_cnt+=tmp_cnt;

		/* �ͻ����� */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='6'" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		

		buy_cnt+=tmp_cnt;

		/* �������� */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='E'" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		

		dsy_cnt+=tmp_cnt;
		/*��ȷ�� 20140305_wjl*/
		ret = sql_sum_long( "note_mst" , "cnt" , &tmp_cnt , 
			"br_no='%s'  and note_type='%s' and sts ='7'" , 
			s_jg.br_no,s_zl.note_type);
		ERR_DEAL	
		de_cnt+=tmp_cnt;
		/* �������� */
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='H'" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		

		cls_cnt+=tmp_cnt;

		/* ���չ�ʧ */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='B'" , 
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		

		lose_cnt+=tmp_cnt;

		/* ���ս�� */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			"br_no='%s' and tx_date=%ld and note_type='%s' and sts_ind ='C'" ,
			s_jg.br_no,s_com_sys_parm.lst_date,s_zl.note_type);
		ERR_DEAL		

		unlose_cnt+=tmp_cnt;	

		ttlnum++;

     		/* ͳ�����տ�� */
			ys_cnt = now_cnt + out_cnt + use_cnt + buy_cnt + 
					 dsy_cnt + cls_cnt + lose_cnt + unlose_cnt - in_cnt;

    		/* ��ӡ��Ϣ */
			ret = print_body();
			ERR_DEAL
	}
	Note_parm_Clo_Sel( );

				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
		fclose(fp);
	}

	Com_branch_Clo_Sel();
	
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ƾ֤��Ŀ�սᵥ�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"����ƾ֤��Ŀ�սᵥʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_pzrjd);
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
int get_ratio_pzrjd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	char br_name[51];
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
			sprintf( str , fmt , s_jg.br_no ); 
			break; 
		case 'C': /* �������� */
			strcpy( br_name,s_jg.br_name );
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_jg.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* ƾ֤��� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_zl.note_type); 
			break; 
		case 'F': /* ƾ֤���� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , note_name);
			break;
		case 'G': /* ���տ�� */							
			sprintf(fmt,"%%%dld",bll);			
			sprintf( str, fmt, ys_cnt ); 
			break;
		case 'H': /* �������� */
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt , in_cnt );
			break;
		case 'I': /* ���ս��� */
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt , out_cnt );
			break;	
		case 'J': /* ����ʹ�� */
			sprintf(fmt,"%%%dld",bll);
			sprintf( str, fmt , use_cnt );
			break;
		case 'K': /* �ͻ����� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, buy_cnt ); 
			break;	
		case 'L': /* �������� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, dsy_cnt ); 
			break;	
		case 'a': /* ���մ�ȷ�� 20140305����wjl */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, de_cnt ); 
			break;	
		case 'b': /* ���մ�ȷ���ܼ� 20140305����wjl */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, de_cnt_tol ); 
			break;	
		case 'M': /* �������� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, cls_cnt ); 
			break;	
		case 'N': /* ���չ�ʧ */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, lose_cnt ); 
			break;						
		case 'O': /* ���ս�� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, unlose_cnt ); 
			break;	
		case 'P': /* ���ս�� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, now_cnt ); 
			break;		
		case 'Q': /* ���տ���ܼ� */							
			sprintf(fmt,"%%%dld",bll);			
			sprintf( str, fmt, ys_cnt_tol ); 
			break;
		case 'R': /* ���������ܼ� */
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt , in_cnt_tol );
			break;
		case 'S': /* ���ս����ܼ� */
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt , out_cnt_tol );
			break;	
		case 'T': /* ����ʹ���ܼ� */
			sprintf(fmt,"%%%dld",bll);
			sprintf( str, fmt , use_cnt_tol );
			break;
		case 'U': /* �ͻ������ܼ� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, buy_cnt_tol ); 
			break;	
		case 'V': /* ���������ܼ� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, dsy_cnt_tol ); 
			break;	
		case 'W': /* ���������ܼ� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, cls_cnt_tol ); 
			break;	
		case 'X': /* ���չ�ʧ�ܼ� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, lose_cnt_tol ); 
			break;						
		case 'Y': /* ���ս���ܼ� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, unlose_cnt_tol ); 
			break;	
		case 'Z': /* ���ս���ܼ� */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, now_cnt_tol ); 
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
