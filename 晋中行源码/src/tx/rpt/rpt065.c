/*************************************************************
* �� �� ��: rpt065.c
* ��������������ת���ڴ�Ʊ���ۿƱ�ۺϰ汾
*
* ��    ��: dadary
* �������: 2006��12��17��
*
* ע����ʽ�ļ�ΪRPT065.tbl
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
/**
#define TABLE_RECORD 3
**/
#define TABLE_RECORD 2
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
#include "com_sys_parm_c.h"
#include "ln_parm_c.h"
#include "com_item_c.h"
#include "ln_mst_c.h"
#include "dc_acc_rel_c.h"
#include "dc_log_bk_c.h"

#define   TABLENUM	3



struct trace_log_bk_c 	s_trace_log_bk;
struct ln_mst_hst_c 	s_ln_mst_hst;
struct com_sys_parm_c 	sComSysParm;
struct dc_log_bk_c	s_dc_log_bk;


int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

double bal_amt=0.00;
double in_amt=0.00;
double out_amt=0.00;
double cmpd_amt=0.00;


long qry_date=0;


char acc_hrt_desp[60];	/***�ı��Ʒ�¼��ӡ�ķ�ʽ***/

int get_ratio_mbfesd(int bli,int bll,char str[100]);

/***ע����:�����Ǵ�ӡһ������,ÿһҳ��4����*****/
rpt065()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];
	char amt_tmp[21];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	char acc_hrt_tmp[8];
	char form_acc_hrt_tmp[8];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset(fname,0x00,sizeof(fname));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(tmp_str,0x00,sizeof(tmp_str));
	memset(old_br_no,0x00,sizeof(old_br_no));
	
	
	memset( &s_trace_log_bk , 0x00 , sizeof(s_trace_log_bk));
	memset( &s_dc_log_bk 	, 0x00 , sizeof(s_dc_log_bk));
	
	
	strcpy( old_br_no , "*****" );
		

	vtcp_log("[%s][%d]��ʼ���ɴ��������Լ��ۿƱ!\n",__FILE__,__LINE__);   
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	ret  = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1" );
	if( ret  ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", ret  ); 
		WRITEMSG 
		goto ErrExit; 
	}
	
	
	qry_date=g_pub_tx.tx_date;
	
	
	
	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	strcpy( filename , "RPT065");
	strcpy(vpfm, "RPT065");
	/* ����:���״���Ϊ"G088" */
	strcpy( tmpstr,"tx_code='G088'and prdt_no[1]='3' and " );
	strcat( wherelist,tmpstr );	
	/**��ɲ�ѯ**/

	strcat(wherelist," no_show!='1' and 1=1  ORDER BY tx_br_no, prdt_no, trace_no, trace_cnt") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	if(ret)
	{
		vtcp_log("[%s][%d]����Trace_log_bkʧ��!\n",__FILE__,__LINE__);
		goto ErrExit;
		ttlnum = 0;
	}
	while(1)
	{
		ret=Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
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
			bal_amt=0.00;
			in_amt=0.00;
			out_amt=0.00;
			cmpd_amt=0.00;
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
				/**���ǵ�һ������ ��������һ�������ر��ļ�***/
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
			
		}
		
		
		/* ͳ����Ϣ */
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &bal_amt , "ac_id=%ld and tx_date=%ld and tx_code='G088' and ln_tx_type='0' and add_ind='0' " , 
					s_trace_log_bk.ac_id, s_trace_log_bk.tx_date );
		ERR_DEAL 
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &in_amt , "ac_id=%ld and  tx_date=%ld and tx_code='G088' and ln_tx_type='1' and add_ind='0' " , 
					s_trace_log_bk.ac_id, s_trace_log_bk.tx_date );
		ERR_DEAL
					
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &out_amt , "ac_id=%ld and  tx_date=%ld and tx_code='G088' and ln_tx_type='2' and add_ind='0' " , 
					s_trace_log_bk.ac_id,  s_trace_log_bk.tx_date );
		ERR_DEAL
		 
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &cmpd_amt , "ac_id=%ld and  tx_date=%ld and tx_code='G088' and ln_tx_type='3' and add_ind='0' " , 
					s_trace_log_bk.ac_id, s_trace_log_bk.tx_date );
		ERR_DEAL
		
		
		vtcp_log("[%s][%d]bal_amt==[%.2f],in_amt==[%.2f],out_amt==[%.2f],cmpd_amt=[%.2f]\n",__FILE__,__LINE__,
				bal_amt,in_amt,out_amt,cmpd_amt);
				
		/* ��ҳ */		
    if( page != 1 && line_file==1 )
		      {
		      	fprintf ( fp , "\f" );
		      }		
    /* ��ӡ��Ϣ */
		ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
		ERR_DEAL
		line_file++;
		
		vtcp_log("[%s][%d]������˵���Ѿ���ӡ������ͷ���� ����ʼ��ӡ��Ʒ�¼!\n",__FILE__,__LINE__);
		
		ret = Dc_log_bk_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld  order by amt,dc_ind ",
					s_trace_log_bk.tx_date,s_trace_log_bk.trace_no);
		if(ret)
		{
			vtcp_log("[%s][%d]����Dc_log_bkʧ��!\n",__FILE__,__LINE__);
			goto ErrExit;
		}
		while(1)
		{
			memset(acc_hrt_desp,0x00,sizeof(acc_hrt_desp));
			ret=Dc_log_bk_Fet_Sel( &s_dc_log_bk, g_pub_tx.reply );
			if( ret==100 ) 
			{
				ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","TT",&opncnt,get_ratio_mbfesd);
				ERR_DEAL
				break;	
			}
			else if( ret )
			{
				sprintf(acErrMsg,"��ȡdc_log_bk�������!");
				WRITEMSG			
				goto ErrExit;			
			}
			
			if(s_dc_log_bk.dc_ind[0]=='1' )
			{
				vtcp_log("[%s][%d]��ȡ���˽跽��Ŀ201===[%s]\n",__FILE__,__LINE__,s_dc_log_bk.acc_hrt);
				memset(amt_tmp,0x00,sizeof(amt_tmp));
				sprintf( acc_hrt_desp,"��Ŀ���裩:",12);
				strcat(  acc_hrt_desp,s_dc_log_bk.acc_hrt);
				strcat(  acc_hrt_desp,"    ");
				sprintf( amt_tmp, "%.2lf" , s_dc_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				strcat(  acc_hrt_desp,amt_tmp);
				vtcp_log("[%s][%d]201���Ŀ=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
			}
			else if(s_dc_log_bk.dc_ind[0]=='2' )
			{
				vtcp_log("[%s][%d]��ȡ���˽跽��Ŀ201===[%s]\n",__FILE__,__LINE__,s_dc_log_bk.acc_hrt);
				memset(amt_tmp,0x00,sizeof(amt_tmp));
				sprintf( acc_hrt_desp,"��Ŀ������:",12);
				strcat(  acc_hrt_desp,s_dc_log_bk.acc_hrt);
				strcat(  acc_hrt_desp,"    ");
				sprintf( amt_tmp, "%.2lf" , s_dc_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				strcat(  acc_hrt_desp,amt_tmp);
				vtcp_log("[%s][%d]201���Ŀ=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
			}
			
			ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","KK",&opncnt,get_ratio_mbfesd);
			ERR_DEAL
		}
		if( line_file>TABLE_RECORD )	/* ���ڻ�ҳ */
		{
			/**line_file=0;del by martin 070520**/
			line_file=1;
			page++;
			/*** fprintf(fp,"\f"); ***/
			/***  fprintf(fp,"\n"); del by martin 070517 ***/
			/***  fprintf(fp,"\n"); del by martin 070517 ***/
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

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];
	char cTel[7];
	char amt_tmp[22];
	
	memset(vstr,	0x00,	sizeof(vstr));
	memset(fmt,	0x00,	sizeof(fmt));
	memset(cDate,	0x00,	sizeof(cDate));
	memset(cTel,	0x00,	sizeof(cTel));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));
	memset(amt_tmp,0x00,sizeof(amt_tmp));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	memset(str,	0x00,	sizeof(str));
	
	
	switch( bli ) 
	{
		case 'A': 
			vtcp_log("[%s][%d]��ʼ��ӡ--����!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,g_ln_mst.name);
			break;
		case 'B': 
			vtcp_log("[%s][%d]��ʼ��ӡ--�ʺ�!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,g_mdm_ac_rel.ac_no);
			break;
		case 'D':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			ret = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", s_trace_log_bk.tx_br_no );
			if( ret )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", ret );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name);
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'E': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'Y': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'G': /* �������� */						
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , bal_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* �������� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , in_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* �������� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* �������� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'M': /* �ܼ� */
			/* �Ӷ��� */
			vtcp_log("[%s][%d]��ʼ��ӡ--���!\n",__FILE__,__LINE__);
			vtcp_log("[%s][%d]��ʼ��ӡ--���׽��!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf( amt_tmp, "%.2lf" , bal_amt+in_amt+out_amt+cmpd_amt ); 
			numtomoney( bal_amt+in_amt+out_amt+cmpd_amt, vstr);
			pub_rpt_flttomon( amt_tmp,amt_tmp ); 
			vtcp_log("[%s][%d]fmt=[%s]\n",__FILE__,__LINE__,fmt);
			
			vtcp_log("[%s][%d]fmt=[%s]\n",__FILE__,__LINE__,fmt);
			strcat(vstr," Сд:");
			strcat(vstr,amt_tmp);
			sprintf(str,fmt,vstr);
			vtcp_log("���1=[%lf],vstr==[%s]str==[%s]\n",g_ln_mst.bal,vstr,str);
			/*strcpy( str, vstr ); */
			break; 
		case 'X':
			vtcp_log("[%s][%d]��ʼ��ӡ--ת��ԭ��1!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
			switch(g_ln_mst.ac_sts[0])
			{
				case '1':
					strcpy(vstr,"������Զ�����");
					break;
				case '2':
					strcpy(vstr,"   ����ת����   ");
					break;
				case '3':
					strcpy(vstr,"   ����ת����   ");
					break;
				case '4':
					strcpy(vstr,"   ����ת����   ");
					break;
				case '5':
					strcpy(vstr,"  ����ת90����  ");
				default:
					strcpy(vstr,"        ");					
			}
			memcpy(str,vstr,bll);
			 break;
		case 'T':		/**��¼130**/
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,acc_hrt_desp);
			
			break;
			break;
		
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}


