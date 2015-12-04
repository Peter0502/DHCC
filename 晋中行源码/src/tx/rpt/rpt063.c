/*************************************************************
* �� �� ��: rpt063.c
* ����������20�Ŵ����Ϣ�����Ŀ�Ľ�Ϣ��Ʊ
*
* ��    ��: dadary
* �������: 2006��11��21��
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
#include "ln_acm_reg_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"
#include "trace_log_c.h"
#include "dc_log_c.h"

#define TABLE_RECORD 2

struct ln_acm_reg_c 	s_ln_acm_reg;
struct ln_mst_c 	s_ln_mst;
struct ln_parm_c 	s_ln_parm;
struct com_sys_parm_c 	s_ComSysParm;
struct dd_mst_c 	s_dd_mst;
struct mdm_ac_rel_c 	s_mdm_ac_rel;
struct mdm_ac_rel_c 	w_mdm_ac_rel;
struct dd_parm_c 	s_dd_parm;
struct trace_log_c      s_trace_log;
struct dc_log_c		s_dc_log;


int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
long days=0;
long qry_date=0;
double rate_money = 0.0;/*ʵ�ʽ�Ϣ���*/
double rtn_money = 0.0; /*�������*/
double owe_money = 0.0; /*��Ƿ���*/
double save_money = 0.0;/*�����ʻ��Ĵ��*/

char acc_hrt_desp[60];	/***�ı��Ʒ�¼��ӡ�ķ�ʽ***/
		
int get_ratio_lnjshd(int bli,int bll,char str[100]);

rpt063()
{
	int flag=0;
	int del_ind=0;
	int ttlnum;
	int BB_num=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	char old_intst_type[2];
	long ac_id_old=0;
	char amt_tmp[21];
	
	
	int  dc_log_flag = 0;/**һ����ˮ�ж�ʻ�Ʒ�¼,����������ֻ�ڵ�һ����¼�ĵط���ӡ��ͷ***/
	
	double old_trace_no = 0;
	
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_acm_reg , 0x00 , sizeof(struct ln_acm_reg_c));
	
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	
	/*���㽻������*/
	qry_date=g_pub_tx.tx_date;
	
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	memset( &s_ComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_ComSysParm , "1=1");
	if( ret ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", ret ); 
		WRITEMSG 
		goto ErrExit; 
	}
	
	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	
	strcpy( filename , "RPT063");
	strcpy(vpfm, "RPT063");
	/* ���������Ϣ�嵥 */
	/* ����:��ˮ�Ŵ��ڵ���������ʼ��ˮ�� */
	memset( &tmpstr,0x0,strlen(tmpstr));	
	memset( &wherelist,0x0,strlen(wherelist));
	/*sprintf( tmpstr," cal_date=%ld and intst_type in('1','2','3','4') and  ",qry_date ); modify by martin 0719*/
	sprintf( tmpstr," cal_date=%ld and intst_type = '1' and  ",qry_date );
	strcat( wherelist,tmpstr );	
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY br_no, ac_id, ac_seqn ,beg_date, intst_type");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;	
	while(1)
	{
		memset(&s_ln_acm_reg,0x00,sizeof(s_ln_acm_reg));
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				fclose(fp);
			}
			break;	
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����Ϣ�Ǽǲ��쳣����!");
			WRITEMSG			
			goto ErrExit;			
		}	
		
		/*ȡ�ǼǱ���Ϣ*/
		ret =  pub_get_all_file(s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn, &s_mdm_ac_rel,&s_ln_mst,&s_ln_parm,&s_dd_mst,&s_dd_parm );
		if(ret)
		{
			return ret;
		}
		/***  ��Ϣ��̖��ԃ 0905***/
		/**** delete at 20110521
		������Ϊʲô������ټ�鸶Ϣ�ʺ���Ϣ�����Ҵ�Mdm_ac_rel���в��ң�����mdm_ac_rel���п���ac_seqnΪ9999������Ӧ��ac_id�ڻ��������е�
		ac_seqnΪ1�����Լ�鲻��ȥ 
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &w_mdm_ac_rel, "ac_id=%ld and ac_seqn=%d ", s_dd_mst.ac_id ,s_dd_mst.ac_seqn);
		if( ret==100 )
		{
			sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼[%d][%ld]",ret,s_dd_mst.ac_id);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			return 1;
		} 
		else if( ret )
		{
		sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣[%d][%ld]",ret,s_dd_mst.ac_id);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
		return -1;
		}
		end 20110521 ****/
		
		/****�����ˮ�仯�� ����Ҫ��ӡ��β����******/
	
		/* �����任ʱ */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			ttlnum=0;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
		} 
		
		
		/******************��ȡ�������*************/
		ret = Dc_log_Sel( g_pub_tx.reply , &s_dc_log , "trace_no=%ld and tx_date=%ld and acc_hrt like '2%%' ",s_ln_acm_reg.trace_no,s_ln_acm_reg.cal_date);
		if(ret==100)
		{
			vtcp_log("[%s][%d]�Ĵ��û�л���Ϣ!\n",__FILE__,__LINE__);
			rtn_money = 0.0;
		}
		else if (ret )
			ERR_DEAL
		if(ret==0)
		{
			
			rtn_money  = s_dc_log.amt;
			vtcp_log("[%s][%d]����Ϣ��!!!!!=[%lf]\n",__FILE__,__LINE__,rtn_money);
		}
		vtcp_log("[%s][%d]������Ϣ===[%lf] ret=[%d] \n",__FILE__,__LINE__,rtn_money,ret);	
		owe_money   = 0.0 ;
		owe_money   = s_ln_mst.in_lo_intst + s_ln_mst.out_lo_intst + s_ln_mst.cmpd_lo_intst;/*�Ӵ���������ȡ������**/
		vtcp_log("[%s][%d]��ˮ[%ld]��ʼ��ӡ��ͷhaha!\n",__FILE__,__LINE__,s_ln_acm_reg.trace_no);
		if( page != 1 && line_file==0 )
		      {
		      	fprintf ( fp , "\f" );
		      }
		else if ( page == 1 && line_file==0 )
			{
				fprintf(fp,"%c%c",0x1b,0x40);
			} 
		ret = Make_yeb_sub("HH");
		if(s_ln_acm_reg.intst_type[0]!='1')
			ret = Make_yeb_sub("BB");
		else
			ret = Make_yeb_sub("AA");
		ret = Make_yeb_sub("CC");
		printf("[%s][%d]���id[%ld],����!\n",__FILE__,__LINE__,s_ln_acm_reg.ac_id);
		ERR_DEAL
		
		
		
		/******ͨ����ln_acm_reg�����ȡ����ˮ������ȡ���Ӧ�Ļ�Ʒ�¼***/
		ret = Trace_log_Dec_Sel(g_pub_tx.reply,"trace_no=%ld and tx_date=%ld order by trace_cnt",s_ln_acm_reg.trace_no,s_ln_acm_reg.cal_date);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]������ˮ���� !ret===[%d]\n",__FILE__,__LINE__,ret);
			return ret;
		}
		while(1)
		{
			memset(&s_trace_log,0x00,sizeof(s_trace_log));
			/***ע���� �����ln_acm_reg�д�����ˮ,��ô��trace_log�п϶�Ҳ����***/
			ret = Trace_log_Fet_Sel(&s_trace_log,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ�����쳣 !ret==[%d]\n",__FILE__,__LINE__,ret);
				return ret;
			}
			else if(ret==100)
			{
				strcpy(acc_hrt_desp,"��Ʒ�¼Ƿȱ");
				ret = Make_yeb_sub("TT");
				ERR_DEAL
				break;
			}
			
			
			
			
			/***********������г�ʼ��*****************/
			
			/**������ˮ���Լ���ˮ�ֺ�����ȡ��Ʒ�¼***/
			ret = Dc_log_Dec_Sel(g_pub_tx.reply,"trace_no=%ld and trace_cnt=%d and tx_date=%ld order by amt,dc_ind"
				,s_trace_log.trace_no,s_trace_log.trace_cnt,s_trace_log.tx_date);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ��쳣 !ret=[%d]\n",__FILE__,__LINE__,ret);
				return ret;
			}
			rate_money = 0.0;/*ʵ�ʽ�Ϣ���*/
		
			save_money = 0.0;/*�����ʻ��Ĵ��*/
			
			dc_log_flag = 0;
			while(1)
			{
				memset(&s_dc_log,0x00,sizeof(s_dc_log));
				ret = Dc_log_Fet_Sel(&s_dc_log,g_pub_tx.reply);
				if(ret && ret!=100)
				{
					vtcp_log("[%s][%d]���ݿ�����쳣 !ret==[%d]\n",__FILE__,__LINE__,ret);
					return ret;
				}
				else if(ret==100)
				{
					break;
				}
				memset(acc_hrt_desp,0x00,sizeof(acc_hrt_desp));
				
				if(s_dc_log.dc_ind[0]=='1' )
				{
					vtcp_log("[%s][%d]��ȡ���˽跽��Ŀ201===[%s]\n",__FILE__,__LINE__,s_dc_log.acc_hrt);
					memset(amt_tmp,0x00,sizeof(amt_tmp));
					sprintf( acc_hrt_desp,"��Ŀ���裩:",12);
					strcat(  acc_hrt_desp,s_dc_log.acc_hrt);
					strcat(  acc_hrt_desp,"    ");
					sprintf( amt_tmp, "%.2lf" , s_dc_log.amt ); 
					pub_rpt_flttomon( amt_tmp,amt_tmp );
					strcat(  acc_hrt_desp,amt_tmp);
					vtcp_log("[%s][%d]201���Ŀ=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
				}
				else if(s_dc_log.dc_ind[0]=='2' )
				{
					vtcp_log("[%s][%d]��ȡ���˽跽��Ŀ201===[%s]\n",__FILE__,__LINE__,s_dc_log.acc_hrt);
					memset(amt_tmp,0x00,sizeof(amt_tmp));
					sprintf( acc_hrt_desp,"��Ŀ������:",12);
					strcat(  acc_hrt_desp,s_dc_log.acc_hrt);
					strcat(  acc_hrt_desp,"    ");
					sprintf( amt_tmp, "%.2lf" , s_dc_log.amt ); 
					pub_rpt_flttomon( amt_tmp,amt_tmp );
					strcat(  acc_hrt_desp,amt_tmp);
					vtcp_log("[%s][%d]201���Ŀ=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
				}
				
				
				
				/* ��ӡ��Ϣ ,�Ȱѳ���¼��Ĳ��ִ�ӡ����,��¼������һ��ѭ���д�ӡ */
				/****��ӡ��Ʒ�¼****/
				ret = Make_yeb_sub("JJ");
				ERR_DEAL
				
				dc_log_flag ++; /***��¼����+1*****/
				
				vtcp_log("[%s][%d]dc_ind==[%c],acc_hrt=[%s]\n",__FILE__,__LINE__,s_dc_log.dc_ind[0],s_dc_log.acc_hrt);
				
			}/**����dc_log���while***/
			
		}/**����trace_log���while**/
		
		/* ��ӡ��Ϣ */
		line_file++;
		if( line_file==TABLE_RECORD )	/* ���ڻ�ҳ */
		{
			line_file=0;
			page++;
			/*** fprintf(fp,"\f"); **/
			/***  fprintf(fp,"\n"); del by martin 070517 ***/
		}
		ttlnum++;
	}		
	Ln_acm_reg_Clo_Sel();

	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɴ����Ϣ��Ʊ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɴ����Ϣ��Ʊʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnjshd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ֵ���� */
int get_ratio_lnjshd( bli,bll,str )
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
	char ln_intst_type[11];
	char acm_intst_type[21];
	int i=0; 
	int k=0;
	struct com_branch_c s_Com_branch;
	char acc_hrt_print[8];
	memset(acc_hrt_print,0x00,sizeof(acc_hrt_print));
	memcpy(acc_hrt_print,"0000000",7);
	
	memset(&s_Com_branch, 0x0, sizeof(struct com_branch_c));
	days= pub_base_CalTrueDays( s_ln_acm_reg.beg_date,s_ln_acm_reg.end_date);	
	switch( bli ) 
	{ 
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &s_Com_branch, "br_no='%s'",
				s_ln_acm_reg.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( s_Com_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , s_Com_branch.br_name , s_ln_acm_reg.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'E':/*��ǰ ��*/
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, s_ComSysParm.lst_date/10000 ); 
			break; 
			break;
		case 'D':/*��ǰ��*/
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, s_ComSysParm.lst_date/100%100 ); 
			break; 
			break;		
		case 'F': /* �����˺� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_mdm_ac_rel.ac_no);
			memset( &s_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G':/*��ǰ��*/
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, s_ComSysParm.lst_date%100 ); 
			break; 
			break;		
		case 'J': /* ��Ϣ���� */
			pub_base_dic_show_str(acm_intst_type,"ln_acm_type",s_ln_acm_reg.intst_type);	/* ��Ϣ���� */
			pub_base_strpack(acm_intst_type);
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, acm_intst_type); 
			break;	
		case 'N': /* ���� */						
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* ��Ϣ��� */
			/* �Ӷ��� */
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* ����� */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);
			break;
		case 'I': /* ��Ʒ */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_parm.title);
			break;
		case 'O': /* ���� */	
			sprintf( amt_tmp, "%8.5lf" , s_ln_acm_reg.rate ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf(acErrMsg,"!![%s]",amt_tmp);
			WRITEMSG	
			sprintf( str, "%8.5lf", s_ln_acm_reg.rate ); 
			break;	
		case 'K': /* ��ʼ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.beg_date); 
			break; 
		case 'L': /* ��ֹ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.end_date); 
			break; 	
		case 'M': /* ���� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, days); 
			break; 
		case 'R':/*������Ϣ*/
			sprintf( amt_tmp, "%.2lf" , rtn_money ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf(acErrMsg,"!![%s]",amt_tmp);
			WRITEMSG
			break;
		case 'S':/*��Ƿ��Ϣ*/
			sprintf( amt_tmp, "%.2lf" , owe_money ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf(acErrMsg,"!![%s]",amt_tmp);
			WRITEMSG
			break;
		case 'U':/*�� 201���Ŀ*/
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,acc_hrt_desp);
			break;
		case 'a': /*��������*/
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'b': /*������*/
			sprintf( amt_tmp, "%8.5lf" , s_ln_acm_reg.rate ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf( str, "%8.5lf", s_ln_acm_reg.rate ); 
			break; 
		case 'c': /**/
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'Q':/*��Ϣ�ʺ�*/
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, w_mdm_ac_rel.ac_no);
			memset( &w_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
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
/*****************************************************
 *���ݽ�Ϣ�ļ�¼��ȡ���ڱ�,���ڱ�,�ʺŽ��ʱ��е����� *
 *****************************************************/
 
int pub_get_all_file(int ac_id,int ac_seqn , struct mdm_ac_rel_c *mdm_ac_rel,struct ln_mst_c *ln_mst,struct ln_parm_c *ln_parm
			,struct dd_mst_c *dd_mst,struct dd_parm_c *dd_parm )
{
    int ret = 0 ;
    
    /* ������ʾ�˺ţ���ƾ֤(����)���˻���ϵmdm_ac_rel���в�ѯ��ȡ���˻�ID */
	sprintf( acErrMsg,"!!!![%ld]",ac_id);
	WRITEMSG
	sprintf( acErrMsg,"!!!![%ld]",ac_id);
	WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , mdm_ac_rel, "ac_id=%ld and ac_seqn=%d ", ac_id ,ac_seqn);
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼[%d][%ld]",ret,ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣[%d][%ld]",ret,ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
        }
    	        

   	/* ȡ�������ļ���Ϣ */
	ret = Ln_mst_Sel(g_pub_tx.reply, ln_mst , "ac_id=%ld and ac_seqn=%d" ,
				        mdm_ac_rel->ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",mdm_ac_rel->ac_id,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;

	}
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}

   	/* ȡ���������Ϣ */
	ret = Ln_parm_Sel(g_pub_tx.reply, ln_parm , " prdt_no='%s' " ,
				        ln_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��������ļ����޴˼�¼ [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ��������ļ��쳣 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		return -1;
   	}
   	
   	/* ȡ�����Ӧ�Ļ������ļ���Ϣ */
	ret = Dd_mst_Sel(g_pub_tx.reply, dd_mst , "ac_id=%ld and ac_seqn=1" ,
				        ln_mst->repay_ac_id );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",ln_mst->repay_ac_id,ac_seqn);
		WRITEMSG

                return(0); /* modified by liuxuan */
         
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;

	}
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}

   	/* ȡ���������Ϣ */
	ret = Dd_parm_Sel(g_pub_tx.reply, dd_parm , " prdt_no='%s' " ,
				        dd_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���� �����ļ����޴˼�¼ [%s][%d]",dd_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ���ڲ����ļ��쳣 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		return -1;
   	}
  return 0 ;
}
