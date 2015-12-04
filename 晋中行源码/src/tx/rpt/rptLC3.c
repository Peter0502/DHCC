/**************************************************************************************
*�ļ���: rptLC3.c
*���ߣ��Ž�
*ʱ�䣺20141125
*���ܣ���ӡ��Ʋ�Ʒ�����������ϸ����
*****************************************************************************************/
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
#include "fn_reg_c.h"
#include "com_sys_parm_c.h"
#include "fn_parm_c.h"
#include "fn_flt_radio_parm_c.h"

static struct fn_reg_c sFn_reg;
static struct fn_parm_c sFn_parm;
static int page = 0;
static int line = 0;
static char vpfm[21];
static FILE *fp = NULL;
static int ret = 0;
static char t_str[41];
static int ttlnum = 0;
static double sum = 0.0;
static double sum2= 0.0;
static int line_file = 0;
static int g_count=0;
static int opncnt = 0;
static int i=0;

int get_ratio_lcmx(int bli,int bll,char str[200]);
int rptLC3()
{
	char filename[50];
	char wherelist[1024];
	char old_br_no[6];
	char old_prdt_code[15];
	struct com_sys_parm_c sCom_sys_parm;
	
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset(old_prdt_code,0x00,sizeof(old_prdt_code));
	memset(old_br_no,0x00,sizeof(old_br_no));
	memset(filename,0x00,sizeof(filename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(vpfm,0x00,sizeof(vpfm));
	memset(t_str,0x00,sizeof(t_str));
	strcpy( old_br_no , "*****" );
	strcpy( old_prdt_code , "*****" );
	
	ret = sql_begin();
	if(ret != 0 )
	{
		sprintf(acErrMsg,"������ʧ�ܣ�");
		WRITEMSG
		goto ErrExit;
	}
	
	pub_base_sysinit();
	strcpy(filename,"lc_sy_mx");
	strcpy(vpfm,"lc_sy");
	
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = sCom_sys_parm.lst_date;
	/**��ɲ�ѯ**/
	sprintf(wherelist,"return_date=%ld and sts in ('2','6')  order by tx_br_no,prdt_code,pact_no,sts ",g_pub_tx.tx_date);
	
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL
	
	while(1)
	{
		ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(ret == 100)
		{
			if(ttlnum)
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));	
				memset(&sum2,0x00,sizeof(double));
				vtcp_log("��ӡҳβ1:[%s]--[%d]",__FILE__,__LINE__);
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);				
				
			}	
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����Ʋ�Ʒ����������ϸ!" );
				WRITEMSG
			}	
			break;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ȡ��Ʋ�Ʒ�ǼǱ����!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		
			/* �����任ʱ */
		pub_base_strpack( sFn_reg.tx_br_no );
		if( strcmp( sFn_reg.tx_br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
				strcpy( old_prdt_code , "*****" );
			}
			
			strcpy( old_br_no , sFn_reg.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile( sFn_reg.tx_br_no, filename , 0 );
			ERR_DEAL
			/* ���ļ� */	
			ret = pub_rpt_openfile( &fp , sFn_reg.tx_br_no, filename );
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		/*����Ʒ��ʱ*/
		if(strcmp(sFn_reg.prdt_code,old_prdt_code))
		{
			/* ��һ����Ʒ�� */
			if( strcmp( old_prdt_code , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
			
				/*��ӡҳ��*/
				page++;
				ret = print_head();
				ERR_DEAL
			}
			
			strcpy( old_prdt_code, sFn_reg.prdt_code);
			
		}
		
    		/* ��ӡ��Ϣ */
    if(sFn_reg.sts[0] == '*')
	  {
		   sFn_reg.tx_amt = -sFn_reg.tx_amt;
	  }		    		
		sum+=sFn_reg.tx_amt;
		sum2+=sFn_reg.fct_intst;
		ret = print_body();
		ERR_DEAL
		line_file++;
		/*if( line_file>50 )	 ����50�л�ҳ */
		if( line_file>42 )	/* ����42�л�ҳ */
		{
			line_file=0;
			page++;	
			/*memset(&sum,0x00,sizeof(double));	
			memset(&sum2,0x00,sizeof(double));*/
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
		g_count++;
	}
	Fn_reg_Clo_Sel();
	
	/*�������Ĵ�ӡȫ�е�*/
	g_count = 0;
	i=1;
	strcpy( old_br_no , "*****" );
	strcpy( old_prdt_code , "*****" );
	/* ɾ��ԭ�ļ� */
	ret = pub_rpt_rmfile( "10001", filename , 0 );
	ERR_DEAL

 	ret = pub_rpt_openfile(&fp,"10001",filename);
	ERR_DEAL	
		
	sprintf(wherelist," sts in ('2','6') and return_date = %ld order by prdt_code,tx_br_no,tx_date,pact_no ",g_pub_tx.tx_date);
	
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL
	
	while(1)
	{
		ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(ret == 100)
		{
			if(ttlnum)
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				vtcp_log("��ӡҳβ1:[%s]--[%d]",__FILE__,__LINE__);
				/* ��ҳβ */
				ret = print_tail();
				ERR_DEAL
							
				
			}	
			else 
			{
				sprintf( acErrMsg, "����û��Ҫ��ӡ����Ʋ�Ʒ������ϸ!" );
				WRITEMSG
			}	
			break;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ȡ��Ʋ�Ʒ�ǼǱ����!![%d]",ret);
			WRITEMSG			
			goto ErrExit;			
		}
		

		/*����Ʒ��ʱ*/
		if(strcmp(sFn_reg.prdt_code,old_prdt_code))
		{
			/* ��һ����Ʒ�� */
			if( strcmp( old_prdt_code , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* ��ӡҳβ */
				ret = print_tail();
				ERR_DEAL
			

			}
							/*��ӡҳ��*/
			page = 1;
			line_file=1;
			ret = print_head();
			ERR_DEAL
	
			strcpy( old_prdt_code, sFn_reg.prdt_code);
			strcpy( old_br_no , sFn_reg.tx_br_no );
		}
		else
		{
					/* �����任ʱ */
		/*pub_base_strpack( sFn_reg.tx_br_no );
		if( strcmp( sFn_reg.tx_br_no , old_br_no ) )
		{
			/ ��һ������ /
			if( strcmp( old_br_no , "*****" ) )
			{
				ret=print_ZZ();
				ERR_DEAL
				memset(&sum,0x00,sizeof(double));
				memset(&sum2,0x00,sizeof(double));	
				/* ��ӡҳβ /
				ret = print_tail();
				ERR_DEAL
			
				/*strcpy( old_prdt_code , "*****" );/
			}
			
			strcpy( old_br_no , sFn_reg.tx_br_no );
			
			page++;
			line_file=1;
			
		
			/* ��ӡҳ�� /
			ret = print_head();
			ERR_DEAL
		}*/
		}		
		sum+=sFn_reg.tx_amt;
		sum2+=sFn_reg.fct_intst;
		ret = print_body();
		ERR_DEAL
		
		line_file++;
		/*if( line_file>50 )	 ����50�л�ҳ */
		if( line_file>42 )	/* ����42�л�ҳ */
		{
			line_file=0;
			page++;	
			/*memset(&sum,0x00,sizeof(double));	
			memset(&sum2,0x00,sizeof(double));	*/
			/* ��ӡҳβ */
			ret = print_tail();
			ERR_DEAL
			/* ��ӡҳ�� */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
		g_count++;
	}
	Fn_reg_Clo_Sel();


OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������Ʋ�Ʒ����������ϸ��ɹ�!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
printf("\n������[%d]����¼.\n",g_count);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������Ʒ���������ϸ��ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lcmx);
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
	
/*	if( page != 1 )*/
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

int print_ZZ()
{
	char vrec[3];
	strcpy(vrec,"ZZ");
	ret=Make_yeb_sub(vrec);
	ERR_DEAL
OkExit:
	return 0;
ErrExit:
	return 1;
}

/* ��ֵ���� */
int get_ratio_lcmx( bli,bll,str )
int bli,bll;
char str[200];
{
	char fmt[10]; 
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_ac_rel_c sMdm_ac_rel1;
	struct dd_mst_c sDd_mst;
	struct fn_flt_radio_parm_c sFn_flt_radio_parm;
	int ret1=0,ret2=0;
	char str_mon[30];
	char str_mon2[30];
	char cAc_id_list[1024];
	char cAc_id_list_tmp[1024];
	int cnt = 0;
	int iCount = 0;
	double rate = 0.00;
	  double n = 0.0;
  double rate1 = 0.0;
  double amt = 0.0;
  double dSum_amt1 = 0.0;
  double dSum_amt2 = 0.0;
  double dSum_amt3 = 0.0;
  double dSum_amt4 = 0.0;
  double dSum_amt5 = 0.0;
  double dSum_amt6 = 0.0;
	
	memset(cAc_id_list,0x00,sizeof(cAc_id_list));
	memset(cAc_id_list_tmp,0x00,sizeof(cAc_id_list_tmp));
  memset(str_mon,'\0',sizeof(str_mon));
  memset(str_mon2,'\0',sizeof(str_mon2));
  memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));

	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_ac_rel1, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst, 0x0, sizeof(struct dd_mst_c));

	
	switch( bli ) 
	{ 
		case 'A': /* �������� */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ����ϵͳ��������!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf( t_str , "%4d��%2d��%2d��" , sCom_sys_parm.lst_date/10000 , sCom_sys_parm.lst_date%10000/100 , sCom_sys_parm.lst_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /*��Ʒ���*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%5s    " , sFn_reg.prdt_code); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* �������� */
	
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",sFn_reg.tx_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			if(i==1)	
			strcpy(sCom_branch.br_name," ��������");
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s" , sCom_branch.br_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break;
		case 'E':/*ļ����ʼ����*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%d" , sFn_parm.beg_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'F':/*ļ����������*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%d" , sFn_parm.end_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;		
		case 'S':/*��Ϣ����*/
    	sprintf( t_str , "%d" , sFn_reg.ic_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	  case 'T':/*��������*/
    	sprintf( t_str , "%d" , sFn_reg.end_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	  case 'Y':/*��Ȩ����*/
			    /*add by lwb 20141204  ����һ����ʵʱ��Ȩ���ʵĲ���*/
        ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code = '%s'",sFn_parm.prdt_code);
        if(ret &&ret != 100)
        {
        	vtcp_log("Fn_flt_radio_parm_Sel error! ret[%d] ",ret);
          strcpy( g_pub_tx.reply,"D324" );
          goto ErrExit;	
        }
        else if(ret ==0){
        
       	if(sFn_flt_radio_parm.buy_amt){
        sql_sum_double("fn_reg","tx_amt",&dSum_amt1,"prdt_code='%s' and tx_amt >=0 and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt);
        amt = sFn_flt_radio_parm.buy_amt;
        rate = sFn_flt_radio_parm.flt_radio2;
        }
        if(sFn_flt_radio_parm.buy_amt1){
        sql_sum_double("fn_reg","tx_amt",&dSum_amt2,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt,sFn_flt_radio_parm.buy_amt1);
         amt = sFn_flt_radio_parm.buy_amt1;
         rate = sFn_flt_radio_parm.flt_radio21;
        }
        if(sFn_flt_radio_parm.buy_amt2){
         sql_sum_double("fn_reg","tx_amt",&dSum_amt3,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt1,sFn_flt_radio_parm.buy_amt2);
         amt = sFn_flt_radio_parm.buy_amt2;
         rate = sFn_flt_radio_parm.flt_radio22;
        }
         if(sFn_flt_radio_parm.buy_amt3){
         sql_sum_double("fn_reg","tx_amt",&dSum_amt4,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt2,sFn_flt_radio_parm.buy_amt3);
         amt = sFn_flt_radio_parm.buy_amt3;
         rate = sFn_flt_radio_parm.flt_radio23;
         } 
         if(sFn_flt_radio_parm.buy_amt4){
         sql_sum_double("fn_reg","tx_amt",&dSum_amt5,"prdt_code='%s' and tx_amt >=%lf and tx_amt<%lf and sts!='*'",sFn_parm.prdt_code,sFn_flt_radio_parm.buy_amt3,sFn_flt_radio_parm.buy_amt4);
         amt = sFn_flt_radio_parm.buy_amt4;
         rate = sFn_flt_radio_parm.flt_radio24;
         }
         sql_sum_double("fn_reg","tx_amt",&dSum_amt6,"prdt_code='%s' and tx_amt >=%lf  and sts!='*'",sFn_parm.prdt_code,amt);


         n =  ( sFn_flt_radio_parm.flt_radio2*dSum_amt2 +sFn_flt_radio_parm.flt_radio21*dSum_amt3 +sFn_flt_radio_parm.flt_radio22*dSum_amt4+sFn_flt_radio_parm.flt_radio23*dSum_amt5 +rate*dSum_amt6)/(dSum_amt1+dSum_amt2+dSum_amt3+dSum_amt4+dSum_amt5+dSum_amt6);
         rate1 = sFn_parm.fct_rate*(1+n);
         vtcp_log("n = %.6lf ",n);
         vtcp_log("rate1 = %.6lf ",rate1);
        }
        /*add end */
      sprintf( t_str , "%.2lf%%" , rate1); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
      break;
		case 'H':/*������*/
			sprintf( t_str , "%s" ,sFn_reg.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'J':/*���*/
			sprintf( t_str , "%d" , line_file); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'I':/*Э����*/
			sprintf( t_str , "%s" , sFn_reg.pact_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'L':/*Ͷ��������*/

			ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id='%d' ",sFn_reg.ac_id);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			vtcp_log(" [%s]--[%d]   name = %s",__FILE__,__LINE__,sDd_mst.name );
      pub_base_strpack( sDd_mst.name ); 
			sprintf( t_str , "%s" , sDd_mst.name); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	
		case 'N':/*����ʽ��˺�*/
			sprintf( t_str , "%s" , sFn_reg.ac_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;	
		case 'U':/*������*/
			sprintf( t_str , "%.2lf" , sFn_reg.tx_amt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;	
		case 'V':/*��λ����*/
			sprintf( t_str , "%s" , sFn_reg.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'P':/*�������*/
	
			cnt = sql_count("fn_reg","prdt_code = '%s' and tx_br_no='%s' and ac_no = '%s' and sts != '*'",sFn_reg.prdt_code,sFn_reg.tx_br_no,sFn_reg.ac_no);
			sprintf( t_str , "%d" , cnt); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Q':/*��������*/
			sprintf( t_str , "%d" , sFn_reg.return_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Z':/*֤������*/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id='%d' ",sFn_reg.ac_id);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			sprintf( t_str , "%s" , sMdm_ac_rel.id_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'W':/*��Ʒ������*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			/*ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
				if(sFn_flt_radio_parm.flt_radio2)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio2);
			if(sFn_flt_radio_parm.flt_radio21)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio21);
			if(sFn_flt_radio_parm.flt_radio22)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio22);
			if(sFn_flt_radio_parm.flt_radio23)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio23);
			if(sFn_flt_radio_parm.flt_radio24)
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio24);*/
			sprintf( t_str , "%.2lf%%" , sFn_parm.fct_rate); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'M': /*��Ʒ����*/
			ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s' ",sFn_reg.prdt_code);
			if(ret !=0)
			{
				sprintf(acErrMsg, "��ѯ��Ʋ�Ʒ���������! [%d]", ret);
				WRITEMSG
				goto ErrExit;	
			}
			pub_base_strpack( sFn_parm.title ); 
			sprintf( t_str , "%s" , sFn_parm.title); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'O': /*ÿҳС��*/
			sprintf(t_str,"%.2lf",sum);
			pub_rpt_flttomon(t_str,str_mon);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon);
			break;
		case 'R': /*������*/
			sprintf( t_str , "%.2lf" , sFn_reg.fct_intst); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'G': /*������С��*/
			sprintf(t_str,"%.2lf",sum2);
			pub_rpt_flttomon(t_str,str_mon2);
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,str_mon2);
			break;
		case 'K': /*�Ϲ�����*/
			if(!strcmp(sFn_reg.chnl_type,"0100"))
				strcpy(t_str,"����");
			if(!strcmp(sFn_reg.chnl_type,"0400"))
				strcpy(t_str,"����");
			if(!strcmp(sFn_reg.chnl_type,"0300"))
				strcpy(t_str,"�ֻ�����");
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'X':/*״̬*/	
			if(!strcmp(sFn_reg.sts,"2"))
				strcpy(t_str,"����");
			if(!strcmp(sFn_reg.sts,"6"))
				strcpy(t_str,"δ����");
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

