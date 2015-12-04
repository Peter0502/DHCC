/***************************************************************************************
�ļ���: rptLCa.c
*���ߣ������
*ʱ�䣺20140917
*���ܣ�ͳ��ļ������Ʋ�Ʒ����֧�е������ܽ��
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
#include "fn_flt_radio_parm_c.h"


struct com_branch_c sCom_branch;
struct com_branch_c sCom_branch1;
struct fn_reg_c sFn_reg;
struct fn_parm_c sFn_parm;
struct fn_br_profit_c sFn_br_profit;
struct fn_flt_radio_parm_c sFn_flt_radio_parm;
int page = 1;
int line = 0;
char vpfm[21];
FILE *fp = NULL;
int ret = 0;
char t_str[41];
int ttlnum = 0;
double sum = 0.0;
int line_file = 0;
int g_count=0;
int opncnt = 0;

int iCount = 0;      /*�ܱ���*/
double dAmt1 = 0.00; /*�����Ϲ����*/
double dAmt2 = 0.00; /*���ճ������*/
double dAmt3 = 0.00; /*����ļ�����*/
double dAmt4 = 0.00; /*�ۼ�ļ�����*/

int iCount1 = 0;     /*�����ܱ���*/	
double dAmt5 = 0.00; /*�ܼƱ����Ϲ����*/ 
double dAmt6 = 0.00; /*�ܼƱ��ճ������*/
double dAmt7 = 0.00; /*�ܼƱ���ļ�����*/
double dAmt8 = 0.00; /*�ܼ��ۼ�ļ�����*/

int get_ratio_lcrb(int bli,int bll,char str[200]);

int rptLC2()
{
	char filename[50];
	char wherelist[1024];
	int ttlnum1 = 0;
	int i = 0;
	struct com_sys_parm_c sCom_sys_parm;

	
	memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
	memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sCom_branch1,0x00,sizeof(sCom_branch1));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset(filename,0x00,sizeof(filename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(vpfm,0x00,sizeof(vpfm));
	memset(t_str,0x00,sizeof(t_str));

	ret = sql_begin();
	if(ret != 0 )
  {
    sprintf(acErrMsg,"������ʧ�ܣ�");
    WRITEMSG
    goto ErrExit;
  }

	pub_base_sysinit();
	strcpy(filename,"lc_rb");
	strcpy(vpfm,"lc_rb");
	
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	ERR_DEAL
	g_pub_tx.tx_date = sCom_sys_parm.lst_date;
	/**��ɲ�ѯ**/
	sprintf(wherelist," br_type='4'  order by br_no ");
	vtcp_log( "WHERE [%s]",wherelist );

	/* ɾ��ԭ�ļ� */
	 ret = pub_rpt_rmfile( "10001", filename , 0 );
	ERR_DEAL

 	ret = pub_rpt_openfile(&fp,"10001",filename);
	ERR_DEAL	
	ret = Fn_parm_Dec_Sel(g_pub_tx.reply,"stat in('0','1','4','5')  and beg_date<=%d and end_date>=%d",g_pub_tx.tx_date,g_pub_tx.tx_date);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"��ѯ��Ʋ��������!![%d]",ret);
                WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		iCount1 = 0;
		dAmt5 = 0.00;
		dAmt6 = 0.00;
		dAmt7 = 0.00;
		dAmt8 = 0.00;
		
		ret = Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
		if(ret== 100)
		{    
			if(!ttlnum1){
				sprintf(acErrMsg,"û�д���ļ���ڵ���Ʋ�Ʒ!![%d]",ret);
				WRITEMSG
				break;
			}
			else
			{	
			
				break;	
			}
		}		
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ��Ʋ��������!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*��ӡ����*/
		ret = print_head();
		ERR_DEAL
		
		
		line_file=0;
		page++;
		ttlnum1++;
		
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
			
			
			ret = my_sel_func(sCom_branch.br_no,sFn_parm.prdt_code,sFn_parm.beg_date);
			if(ret)
			{
				vtcp_log("��ѯ��ϸ��fn_reg���� ");
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
			iCount1 += iCount;
			dAmt5 += dAmt1;
			dAmt6 += dAmt2;
			dAmt7 += dAmt3;
			dAmt8 += dAmt4;
			ttlnum++;
			g_count++;


		}	
		Com_branch_Clo_Sel();
	}
	Fn_parm_Clo_Sel();

OkExit:
        sql_commit();   /*--�ύ����--*/
        fclose(fp);
	fp = NULL;
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"������Ʋ�Ʒ�ձ���ɹ�!!!![%s]",g_pub_tx.reply);
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
        sprintf(acErrMsg,"������Ʋ�Ʒ�ձ���ʧ��!!!![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;

}

static int my_sel_func(char *br_no,char *prdt_code, int beg_date)
{
	int iRet = 0;
	int iRet1 = 0;
	struct fn_reg_c sFn_reg;
	struct fn_br_profit_c sFn_br_profit;
	memset(&sFn_reg,0x0,sizeof(sFn_reg));
	memset(&sFn_br_profit,0x0,sizeof(sFn_br_profit));
	dAmt1 = 0.00;
	dAmt2 = 0.00;
	dAmt3 = 0.00;
	dAmt4 = 0.00;
	iCount = 0;
	/*ͳ�Ʊ����Ϲ����*/
	iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"tx_br_no='%s' and prdt_code='%s' and tx_date = %d  ",\
	br_no,prdt_code,g_pub_tx.tx_date);
		
	if(iRet)
	{
		vtcp_log("�����ѯ�α���� iRet[%d] ",iRet);
		strcpy(g_pub_tx.reply,"D330");
		return 1;
	}
	while(1)
	{
		memset(&sFn_reg,0x00,sizeof(sFn_reg));
		iRet = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(iRet != 0 && iRet != 100)
       		{
			vtcp_log("ȡ��ѯ�α���� iRet[%d] ",iRet);
			strcpy(g_pub_tx.reply,"D330");
			return 1;
        	}
        	else if(iRet == 100)
        	{
			break;
        	}
            
        dAmt1 += sFn_reg.tx_amt;        /*�����Ϲ����*/
        if(sFn_reg.sts[0] != '*')
        iCount ++;                       /*ͳ�Ʊ���*/
   	}
    Fn_reg_Clo_Sel();
    
    /*ͳ�Ʊ��ճ������*/
    iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"tx_br_no='%s' and prdt_code='%s' and date1 = %d and sts = '*' ",\
	br_no,prdt_code,g_pub_tx.tx_date);
		
	if(iRet)
	{
		vtcp_log("�����ѯ�α���� iRet[%d] ",iRet);
		strcpy(g_pub_tx.reply,"D330");
		return 1;
	}
	while(1)
	{
		memset(&sFn_reg,0x00,sizeof(sFn_reg));
		iRet = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(iRet != 0 && iRet != 100)
        	{
			vtcp_log("ȡ��ѯ�α���� iRet[%d] ",iRet);
			strcpy(g_pub_tx.reply,"D330");
			return 1;
        	}
      		else if(iRet == 100)
        	{
			break;
        	}
            
        	dAmt2 += sFn_reg.tx_amt;        /*���ճ������*/
   	}
 	  Fn_reg_Clo_Sel();
   
		/*����fn_br_profit ���л���ļ���ܶ�*/
   		iRet = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code = '%s' and br_no = '%s'",prdt_code,br_no);
   		if(iRet)
   		{
			vtcp_log("�����ѯ�α���� iRet[%d] ",iRet);
			strcpy(g_pub_tx.reply,"D330");
			return 1;  		
 		}
 		iRet = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
 		if(iRet && iRet != 100)
 		{
 			vtcp_log("����Fn_br_profit��ѯ�α���� iRet[%d] ",iRet);
			strcpy(g_pub_tx.reply,"D330");
			return 1;  	
 		}	
 		else if(iRet == 100)
 		{
 			strcpy(sFn_br_profit.prdt_code,prdt_code);
 			strcpy(sFn_br_profit.br_no,br_no);
 			sFn_br_profit.sts[0] = '0';
 			sFn_br_profit.tx_amt = (dAmt1 - dAmt2);
 
			iRet1 = Fn_br_profit_Ins(sFn_br_profit,g_pub_tx.reply);
 			if(iRet1)
 			{
 				vtcp_log("����fn_br_profit���� iRet1[%d] ",iRet1);
				strcpy(g_pub_tx.reply,"D343");
				return 1;  		
 			}	
 		}
 		else
 		{
	 		sFn_br_profit.tx_amt += (dAmt1 - dAmt2);
	 		iRet = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
	 		if(iRet)
	 		{
	 			vtcp_log("����fn_br_profit�α���� iRet[%d] ",iRet);
				strcpy(g_pub_tx.reply,"D344");
				return 1;  
	   	}
  	}
   	iRet = Fn_br_profit_Clo_Upd();
    return 0;
}


/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{
        ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lcrb);
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
int get_ratio_lcrb( bli,bll,str )
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
	 
	 	double rate = 0.00;
	
	  double n = 0.00;
   
    double rate1 = 0.00;
    double amt = 0.00;
    double dSum_amt1 = 0.00;
    double dSum_amt2 = 0.00;
    double dSum_amt3 = 0.00;
    double dSum_amt4 = 0.00;
    double dSum_amt5 = 0.00;
    double dSum_amt6 = 0.00;

 	 memset(str_mon,'\0',sizeof(str_mon));
	 memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	 memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
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
      sprintf( t_str , "%4d��%2d��%2d��" , sCom_sys_parm.lst_date/10000 , sCom_sys_parm.lst_date%10000
/100 , sCom_sys_parm.lst_date%100 );
      sprintf(fmt,"%%-%ds",bll);
      sprintf( str , fmt , t_str );
      break;
			case 'B': /*��Ʒ���*/
			    sprintf( t_str , "%5s    " , sFn_parm.prdt_code);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
			    break;
			case 'D': /* ҳ�� */
			    sprintf(fmt,"%%%dld",bll);
			    sprintf(str, fmt, page);
			    break;
			case 'M': /*��Ʒ����*/
					sprintf( t_str , "%s" , sFn_parm.title);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
					 break;
			case 'E':/*ļ����ʼ����*/
			    sprintf( t_str , "%d" , sFn_parm.beg_date);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
					 break;
			case 'F':/*ļ����������*/
			    sprintf( t_str , "%d" , sFn_parm.end_date);
			    sprintf(fmt, "%%-%ds", bll);
			    sprintf(str, fmt, t_str);
			    break;
		case 'R':/*��Ϣ����*/
    	sprintf( t_str , "%d" , sFn_parm.ic_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
	  case 'T':/*��������*/
    	sprintf( t_str , "%d" , sFn_parm.mtr_date); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;

		case 'W':/*��Ʒ������*/
			 sprintf( t_str , "%.2lf" , sFn_parm.rate);
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
         rate1 = sFn_parm.rate*(1+n);
         vtcp_log("n = %.6lf ",n);
         vtcp_log("rate1 = %.6lf ",rate1);
        }
        /*add end */
      sprintf( t_str , "%.2lf%%" , rate1); 
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
		case 'G':/*�ܱ���*/
			sprintf( t_str , "%d" , iCount);
                         sprintf(fmt, "%%-%ds", bll);
                         sprintf(str, fmt, t_str);
			break;
		case 'H':/*�����Ϲ����*/
			/*dAmt1 /= 10000;*/
			sprintf( t_str , "%.2lf" , dAmt1);
                         sprintf(fmt, "%%-%ds", bll);
                         sprintf(str, fmt, t_str);
			break;
		case 'J': /*���ճ������*/
		     /* dAmt2 /= 10000;*/
			sprintf( t_str , "%.2lf" , dAmt2);
                         sprintf(fmt, "%%-%ds", bll);
                         sprintf(str, fmt, t_str);
			break;
		case 'K': /*����ļ�����*/
			dAmt3 = dAmt1 - dAmt2;
			sprintf( t_str , "%.2lf" , dAmt3);
                         sprintf(fmt, "%%-%ds", bll);
                         sprintf(str, fmt, t_str);
			break;
		case 'L': /*�ۼ�ļ�����*/
		/*	ret = sql_sum_double("fn_reg","tx_amt",&dTmp_amt1," prdt_code = '%s' and tx_br_no='%s' and tx_date>=%d",sFn_parm.prdt_code,sCom_branch.br_no,sFn_parm.beg_date);
			ret = sql_sum_double("fn_reg","redempt_amt",&dTmp_amt2," prdt_code = '%s' and tx_br_no='%s' and tx_date>=%d",sFn_parm.prdt_code,sCom_branch.br_no,sFn_parm.beg_date);
			dAmt4 = dTmp_amt1 - dTmp_amt2;*/

			memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
			ret = Fn_br_profit_Sel(g_pub_tx.reply,&sFn_br_profit,"prdt_code = '%s' and br_no='%s'",sFn_parm.prdt_code,sCom_branch.br_no);
			if(ret)
			{
				vtcp_log("��ѯfn_br_profit�α���� ret[%d] ",ret);
				strcpy(g_pub_tx.reply,"D343");
				return 1;  	
			}
			dAmt4 = sFn_br_profit.tx_amt;
			/*dAmt4 /= 10000;*/
			sprintf( t_str , "%.2lf" , dAmt4);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'V' :/*�����ܱ���*/
			sprintf( t_str , "%d" , iCount1);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'X' : /*���ܽ����˹��ܽ��*/
			sprintf( t_str , "%.2lf" , dAmt5);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Z': /*���ܽ��ճ����ܽ��*/
			sprintf( t_str , "%.2lf" , dAmt6);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'I': /*���ܽ���ļ���ܽ��*/
			sprintf( t_str , "%.2lf" , dAmt7);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'O' : /*�����ۼ�ļ���ܽ��*/
			sprintf( t_str , "%.2lf" , dAmt8);
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
