/***************************************************************************************
�ļ���: rptLC6.c
*���ߣ����Ĳ�
*ʱ�䣺20150107
*���ܣ���ӡ����±���
*****************************************************************************************/
#define ERR_DEAL if( ret ){\
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
#include "com_branch_c.h"
#include "fn_br_profit_c.h"


static struct com_branch_c sCom_branch;
static struct com_branch_c sCom_branch1;
static struct fn_reg_c sFn_reg;
static struct fn_parm_c sFn_parm;
static struct fn_br_profit_c sFn_br_profit;
static int page = 1;
static int line = 0;
static char vpfm[21];
static FILE *fp = NULL;
static int ret = 0;
static char t_str[41];
static int ttlnum = 0;
static double sum = 0.0;
static int line_file = 0;
static int g_count=0;
static int opncnt = 0;

static int iCountA = 0;      
static double dAmtA = 0.0;   
static double dAmtA1 = 0.00; 
static double dAmtA11 = 0.00; 
static double dAmtA12 = 0.00; 
static double dAmtA13 = 0.00;
static double dAmtA14 = 0.00;
static double dAmtA15 = 0.00;
static int iCountA1 = 0; 
static char prdt_type[11] = {'a','A','B','C','D','E','F','G','H','I'};

   
static double dAmtA3 = 0.00;
static double dAmtA5 = 0.00;  
static double dAmtA6 = 0.00; 
static int iCountA2 = 0; 
static int iCountA3 = 0; 
static int iCountA4 = 0; 
static int iCountA5 = 0; 



static double dAmt1 = 0.00; 
static double dAmt2 = 0.00;
static double dAmt3 = 0.00; 
static double dAmt4 = 0.00; 
static double dAmt5 = 0.00; 
static double dAmt6 = 0.00;
static double dAmt7 = 0.00; 

static char str_mon2[30];
static char cStr[100];
static int i = -1;

int get_ratio_lcyb(int bli,int bll,char str[200]);

int rptLC6()
{
	char filename[50];
	char wherelist[1024];

	int ttlnum1 = 0;

	struct com_sys_parm_c sCom_sys_parm;
	
	memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sCom_branch1,0x00,sizeof(sCom_branch1));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset(filename,0x00,sizeof(filename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(vpfm,0x00,sizeof(vpfm));
	memset(t_str,0x00,sizeof(t_str));
	memset(str_mon2,0x00,sizeof(str_mon2));
  memset(cStr,0x00,sizeof(cStr));

	ret = sql_begin();
	if(ret != 0 )
	{
		sprintf(acErrMsg,"������ʧ�ܣ�");
		WRITEMSG
		goto ErrExit;
	}

	pub_base_sysinit();
	strcpy(filename,"lc_yb");
	strcpy(vpfm,"lc_yb");
	
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	ERR_DEAL
	g_pub_tx.tx_date = sCom_sys_parm.lst_date;
	/**��ɲ�ѯ**/
	sprintf(wherelist," br_type='4' order by br_no ");
	vtcp_log( "WHERE [%s]",wherelist );

	/* ɾ��ԭ�ļ� */
	 ret = pub_rpt_rmfile( "10001", filename , 0 );
	ERR_DEAL

 	ret = pub_rpt_openfile(&fp,"10001",filename);
	ERR_DEAL	

	while(++i<6)
	{
	
	vtcp_log("iiiiiiiiiiii=%d",i);
  
  iCountA3 = 0;     
	dAmtA3 = 0.00; 
	dAmtA5 = 0.00;  
	iCountA4 = 0; 
	iCountA5 = 0;

	
	dAmt4 = 0.00; 
	dAmt2 = 0.00;
	dAmt6 = 0.00;	
	dAmt7 = 0.00;
	
	memset(cStr,0x00,sizeof(cStr));	
	line_file = 0;
		
	if(i)
	sprintf(cStr,"and prdt_code in (select prdt_code from fn_parm where title like '%%%c%%')",prdt_type[i]);
	else if(i==0)
	strcpy(cStr," ");
	
	ret=print_head();
	ERR_DEAL
	ret = Com_branch_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL

	while(1)
	{
			

		memset(&sCom_branch,0x00,sizeof(sCom_branch));
		ret = Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
		if(ret == 100)
		{
			ret = print_ZZ();
			ERR_DEAL
			ret = print_tail();
			ERR_DEAL
			break;		
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ȡ�������������!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}			
		
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>42 )      /* ����42�л�ҳ */
		{
			line_file=0;
			page++;
		}	
		ttlnum++;
		g_count++;


	}
	page ++;
}
	Com_branch_Clo_Sel();


OkExit:
        sql_commit();   /*--�ύ����--*/
        fclose(fp);
	      fp = NULL;
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"������Ʋ�Ʒ�±���ɹ�!!!![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
	      printf("\n������[%d]����¼.\n",g_count);

        return 0;
ErrExit:
        sql_rollback(); /*--����ع�--*/
        if(fp != NULL)
        {
        	fclose(fp);
        	fp=NULL;
      	}
        sprintf(acErrMsg,"������Ʋ�Ʒ�±���ʧ��!!!![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;

}



/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{
        ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lcyb);
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

       /* if( page != 1 )*/
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
int print_body()
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
int get_ratio_lcyb( bli,bll,str )
int bli,bll;
char str[200];
{
	 char fmt[10];
	 struct com_sys_parm_c sCom_sys_parm;
	 struct mdm_ac_rel_c sMdm_ac_rel;
	 struct dd_mst_c sDd_mst;
	 int ret1=0,ret2=0;
	 char str_mon[30];
	 double dTmp_amt1 = 0.00;
	 double dTmp_amt2 = 0.00;

 	 memset(str_mon,'\0',sizeof(str_mon));
	 memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	 memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	 memset(&sDd_mst, 0x0, sizeof(struct dd_mst_c));


		switch( bli )
		{
			
			case 'l': /* �������� */
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ����ϵͳ��������!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		      sprintf( t_str , "%4d��%2d��%2d��" , sCom_sys_parm.lst_date/10000 , sCom_sys_parm.lst_date%10000
/100 , sCom_sys_parm.lst_date%100 );
		      sprintf(fmt,"%%-%ds",bll);
		      sprintf( str , fmt , t_str );
		      break;

			case 'D': /* ҳ�� */
			    sprintf(fmt,"%%%dld",bll);
			    sprintf(str, fmt, page);
			    break;
			case 'u': /*��Ʒ����*/
				  if(i==0)
				  strcpy(t_str,"����");
				  else
			    sprintf(t_str,"%c��",prdt_type[i]);
			    sprintf(fmt,"%%-%ds",bll);
		      sprintf( str , fmt , t_str );
		      break; 
			case 'F':/*ͳ�Ƶ��·�*/
			ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ����ϵͳ��������!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		      sprintf( t_str , "%4d��%2d��" , sCom_sys_parm.lst_date/10000 , sCom_sys_parm.lst_date%10000
/100  );
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
			    break;

		case 'S':/*������*/
			 sprintf( t_str , "%s" , sCom_branch.br_no);
       sprintf(fmt, "%%-%ds", bll);
       sprintf(str, fmt, t_str);
       break;
		case 'Q':/*��������*/
       pub_base_strpack( sCom_branch.br_name );
			 sprintf( t_str , "%s" , sCom_branch.br_name);
       sprintf(fmt, "%%-%ds", bll);
       sprintf(str, fmt, t_str);
       break;
		case 'G':/*ļ������*/
			iCountA  = sql_count("fn_reg","   tx_br_no='%s' and sts not in ('3','*','0') and pay_date like '%ld%%' %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
		  iCountA3+=iCountA;
			sprintf( t_str , "%d" , iCountA);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'H':/*ļ���ܽ��*/
			ret = sql_sum_double("fn_reg","tx_amt",&dAmtA,"   tx_br_no ='%s' and sts not in ('3','*','0') and pay_date like '%ld%%' %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
			/*dAmtA /= 10000;*/
			dAmtA3 += dAmtA;
			sprintf( t_str , "%.2lf" , dAmtA);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'a': /*�Ҹ�����*/
		  iCountA1  = sql_count("fn_reg","   tx_br_no='%s' and sts ='2' and return_date like '%ld%%' %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
			iCountA4 += iCountA1;
			sprintf( t_str , "%d" , iCountA1);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'J': /*�Ҹ����*/
			ret = sql_sum_double("fn_reg","return_capital",&dAmtA1,"   tx_br_no='%s' and sts ='2' and return_date like '%ld%%' %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
			/*ret = sql_sum_double("fn_reg","fct_intst",&dAmtA12,"   tx_br_no='%s' and sts ='2' and return_date like '%ld%%' %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
		  ret = sql_sum_double("fn_br_profit","profit_amt",&dAmtA13," br_no = '%s' and pay_date like '%ld%%' and prdt_code in (select prdt_code from fn_parm where  stat='8') %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
			dAmtA1 = dAmtA11+dAmtA12+dAmtA13;*/
			/*dAmtA1 /=10000; */
			dAmtA5 += dAmtA1;
			sprintf( t_str , "%.2lf" , dAmtA1);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
    case 'g':/*�ܼ�ļ���ܱ���*/
    	sprintf( t_str , "%d" , iCountA3);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
      break;
    case 'X':/*�ܼ�ļ���ܽ��*/
    	sprintf( t_str , "%.2lf" , dAmtA3);
    	 pub_rpt_flttomon(t_str,str_mon2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, str_mon2);
      break;
    case 'b':/*�ܼƶҸ��ܱ���*/
    	sprintf( t_str , "%d" , iCountA4);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
      break;
    case 'Z':/*�ܼƶҸ��ܽ��*/
    	sprintf( t_str , "%.2lf" , dAmtA5);
    	 pub_rpt_flttomon(t_str,str_mon2);
      sprintf(fmt, "%%-%ds", bll);
     sprintf(str, fmt, str_mon2);
      break;
      
    case 'Y':/*���¾�ļ�����*/
    	dAmt1 = dAmtA - dAmtA1;
    	sprintf( t_str , "%.2lf" , dAmt1);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
      break;
    case 'C':/*�ܼƱ��¾�ļ�����*/
    	dAmt2 = dAmtA3 - dAmtA5 ;
    	sprintf( t_str , "%.2lf" , dAmt2);
    	pub_rpt_flttomon(t_str,str_mon2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, str_mon2);
      break;
    case 'R':/*���¿ͻ�������*/
    	ret = sql_sum_double("fn_reg","fct_intst",&dAmt3,"tx_br_no = '%s' and return_date like '%ld%%' %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
    	/*dAmt3 /= 10000;*/
    	dAmt4 += dAmt3;
    	sprintf( t_str , "%.2lf" , dAmt3);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
      break;
     case 'P':/*�ܼƿͻ�������*/
     	sprintf( t_str , "%.2lf" , dAmt4);
     	pub_rpt_flttomon(t_str,str_mon2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, str_mon2);
      break;
    case 'W':/*�������ж�����*/
      ret = sql_sum_double("fn_br_profit","profit_amt",&dAmtA13," br_no = '%s' and pay_date like '%ld%%' and prdt_code in (select prdt_code from fn_parm where  stat='8') %s",sCom_branch.br_no,g_pub_tx.tx_date/100,cStr);
      dAmt6 += dAmtA13;
    	sprintf( t_str , "%.2lf" , dAmtA13);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
		  break;
		case 'V':/*�ܼ����ж�����*/
			sprintf( t_str , "%.2lf" , dAmt6);
			pub_rpt_flttomon(t_str,str_mon2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, str_mon2);
		  break;
		case 'c':/*��ĩ������Ʒ��*/
			iCountA2  = sql_count("fn_parm"," stat ='1' %s",cStr);
			iCountA5 += iCountA2;
			sprintf( t_str , "%d" , iCountA2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'e':/*��ĩ�������*/
		 	ret = sql_sum_double("fn_reg","tx_amt",&dAmtA15,"tx_br_no = '%s' and sts ='1' %s",sCom_branch.br_no,cStr);
		  dAmt7 += dAmtA15;
    	sprintf( t_str , "%.2lf" , dAmtA15);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
      break;
     case 'f':/*�ܼ���ĩ�������*/
     	sprintf( t_str , "%.2lf" , dAmt7);
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


