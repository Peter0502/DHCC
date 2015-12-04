/*************************************************************
* �� �� ��: rpt009.c
* ����������ͨ��ͨ�ҽ�����ˮ�嵥--����������ҵ��
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
#include "tx_def_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "dc_log_c.h"

struct dc_log_c s_trace_log_bk,dc_log;struct trace_log_bk_c tmp_trace_log;
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
double cr_sum=0.0; /*�����ϼ�*/
double dr_sum=0.0; /*�跽�ϼ�*/
long dr_cnt=0;
long cr_cnt=0;

int get_ratio_tdtdb(int bli,int bll,char str[100]);

rpt009()
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
	
	strcpy( filename , "RPT009");
	strcpy(vpfm, "RPT009");
	
	/* ����:���׻���!=�������� */
	strcpy( tmpstr," tx_tx_br_no != tx_opn_br_no and acc_hrt!='70200' and ");
	strcat( wherelist,tmpstr );	
	/**��ɲ�ѯ**/
	
	strcat(wherelist,"  sts='0' and amt!=0  ORDER BY tx_opn_br_no , tx_tx_br_no , sts , trace_no , trace_cnt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Dc_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Dc_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
	vtcp_log("��ӡҳβ1:[%s]--[%d]",__FILE__,__LINE__);
				ret=print_sum();
				ERR_DEAL
				dr_sum=0.00;
				cr_sum=0.00;
				dr_cnt=0;
				cr_cnt=0;
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
		pub_base_strpack( s_trace_log_bk.tx_opn_br_no );
		if( strcmp( s_trace_log_bk.tx_opn_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
	vtcp_log("��ӡҳβ2:[%s]--[%d]",__FILE__,__LINE__);
				ret=print_sum();
				ERR_DEAL
				dr_sum=0.00;
				cr_sum=0.00;
				dr_cnt=0;
				cr_cnt=0;
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_opn_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_opn_br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_trace_log_bk.tx_opn_br_no , filename );
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
	Dc_log_bk_Clo_Sel();

	/*  fclose(fp);  */
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ͨ��ͨ����ˮ�嵥(������)�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ͨ��ͨ����ˮ�嵥(������)ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_tdtdb);
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
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/*��ӡ�ϼƽ��*/
int print_sum()
{
	char vrec[3];
	
	strcpy(vrec,"ZZ");
	ret=Make_yeb_sub(vrec);
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
int get_ratio_tdtdb( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; char ss_add_ind[3];
	int i=0; 
	struct com_branch_c sCom_branch;
	struct tx_def_c sTx_def;
	struct cif_id_code_rel_c sCif_id_code_rel;
	struct cif_basic_inf_c  sCif_basic_inf;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct in_mst_c sIn_mst;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct com_item_c sCom_item;
	struct com_sys_parm_c sCom_sys_parm;
	int len=0;

	memset(&sCom_item,0x00,sizeof(struct com_item_c));
	memset(&sIn_mst,0x0,sizeof(struct in_mst_c));
	memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
	memset(&sTd_mst,0x0,sizeof(struct td_mst_c));
	memset(&sCif_id_code_rel,0x0,sizeof(struct cif_id_code_rel_c));
	memset(&sCif_basic_inf,0x0,sizeof(struct cif_basic_inf_c));
	memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&sTx_def,0x0,sizeof(struct tx_def_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	
	switch( bli ) 
	{
		case 'A': /* �������� */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm," 1=1");			
			if(ret)
			{
				sprintf(acErrMsg,"ȡϵͳ���ڳ���!![%d]",ret);			
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%4d��%2d��%2d��" , sCom_sys_parm.lst_date/10000,sCom_sys_parm.lst_date%10000/100 , sCom_sys_parm.lst_date%100 );			
			sprintf(fmt,"%%-%ds",bll);
 			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_trace_log_bk.tx_opn_br_no ); 
			break; 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_trace_log_bk.tx_opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_trace_log_bk.tx_opn_br_no); 
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
		case 'F': /* ����Ա */ 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.tel);
			break;
		case 'I': /* ��      �� */
			sprintf(fmt, "%%-%ds", bll);
			memset(&tmp_trace_log,'\0',sizeof(tmp_trace_log));
			ret=Trace_log_bk_Sel(g_pub_tx.reply,&tmp_trace_log,"trace_no=%d and trace_cnt=%d",s_trace_log_bk.trace_no,s_trace_log_bk.trace_cnt);
			if(ret==0)
				sprintf( str, fmt, tmp_trace_log.ac_no);
			else
				sprintf( str, fmt, s_trace_log_bk.acc_hrt);
			break;
		case 'L':  /*��      �� */
			len=strlen(tmp_trace_log.ac_no);
			if((len==3) || (len==5) || (len==7))
			{
				ret=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",tmp_trace_log.ac_no);
				if(ret)
				{
					sprintf(acErrMsg,"��ѯ������Ŀ��Ȼ������!![%d]",ret);
					WRITEMSG
					goto ErrExit;	
				}	
				pub_base_strpack(sCom_item.acc_name);
				sprintf(t_str,"%s",sCom_item.acc_name);
				sprintf(fmt, "%%-%ds", bll);
				sprintf( str, fmt, t_str);
				break;
			}
			if(tmp_trace_log.ac_no[0]=='9')
			{
				ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%d",tmp_trace_log.ac_id);
				if(ret && ret != 100)
				{
					sprintf(acErrMsg,"��ѯ�ڲ�̨�����ļ�������haha!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}	
			pub_base_strpack(sIn_mst.name); 
			sprintf( t_str , "%s" , sIn_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
			}
			else
			{	
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",tmp_trace_log.ac_no);
				if(ret && ret!=100)
				{
					sprintf(acErrMsg,"��ѯ�������ʻ���ϵ�����!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s' and id_no='%s'",sMdm_ac_rel.id_type,sMdm_ac_rel.id_no);
				if(ret && ret!=100)
				{
					sprintf(acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ����!![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				ret=Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%d",sCif_id_code_rel.cif_no);
				if(ret && ret!=100)
				{
					sprintf(acErrMsg,"��ѯ�ͻ�������Ϣ����� !![%d]",ret);
					WRITEMSG	
					goto ErrExit;
				}
			pub_base_strpack(sCif_basic_inf.name); 
			sprintf( t_str , "%s" , sCif_basic_inf.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, t_str);
			break;
			}
		case 'K': /* ��  ��  �� */
			pub_base_dic_show_str(ss_add_ind,"trace_add_ind",s_trace_log_bk.dc_ind);

			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_trace_log_bk.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			/*strcat( tmp_inf , amt_tmp);*/
			sprintf(fmt,"%%%ds",bll-5);
     			sprintf( str, fmt, amt_tmp );
			strcpy( str+strlen(str), " (");			
			sprintf(str+strlen(str),"%s",ss_add_ind);
			sprintf(str+strlen(str),")");
		if(s_trace_log_bk.dc_ind[0]=='1'){
			dr_sum+=s_trace_log_bk.amt;
				/****���� 20101210****/
				dr_cnt=dr_cnt;
				dr_cnt++;
		}
		else{
			cr_sum+=s_trace_log_bk.amt;
				/****���� 20101210****/
				cr_cnt=cr_cnt;
				cr_cnt++;
		}
			break;
		case 'J': /*�跽�ϼƽ��*/
			sprintf(amt_tmp,"%.2lf",dr_sum);
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'G': /*�跽�ϼƱ���*/
			sprintf(amt_tmp,"%ld",dr_cnt);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'H': /*�����ϼƽ��*/
			sprintf(amt_tmp,"%.2lf",cr_sum);
			pub_rpt_flttomon(amt_tmp,amt_tmp);
			sprintf(fmt,"%%%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'R': /*�����ϼƱ���*/
			sprintf(amt_tmp,"%ld",cr_cnt);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,amt_tmp);
			break;
		case 'M': /* ��ת */
			pub_base_dic_show_str(tmp_inf,"ct_ind",s_trace_log_bk.ct_ind);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, tmp_inf);
			break;
		case 'Q': /* ������ */
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str, fmt, s_trace_log_bk.tx_tx_br_no);
			break;
		case 'O': /* ״̬ */
			pub_base_dic_show_str(tmp_inf,"rpt_log_sts",s_trace_log_bk.sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, tmp_inf);
			break;
		case 'P': /* �������� */
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt, s_trace_log_bk.tx_date);
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
