/***************************************************************************************
�ļ���: rptLC4.c
*���ߣ����Ĳ�
*ʱ�䣺20141217
*���ܣ�ͳ��ļ������Ʋ�Ʒ����֧�е������ܽ��,ļ�������һ���ӡ
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
double dAmt = 0.0;    /*�ܽ��*/
double dAmt1 = 0.00; /*�������*/
double dAmt2 = 0.00; /*������*/
double dAmt7 = 0.00; /*LJ20151116�ֻ����н��*/
int iCount1 = 0; /*��������*/
int iCount2 = 0; /*�������*/
int iCount7 = 0; /*LJ20151116�ֻ����б���*/

int iCount3 = 0;     /*�����ܱ���*/	
double dAmt3 = 0.00; /*�����ܽ��*/
double dAmt5 = 0.00; /*�ܼ��������*/ 
double dAmt6 = 0.00; /*�ܼƹ�����*/
double dAmt8 = 0.00; /*LJ20151116�ܼ��ֻ����н��*/
int iCount4 = 0; /*�ܼ���������*/
int iCount5 = 0; /*�ܼƹ������*/
int iCount8 = 0; /*LJ20151116�ܼ��ֻ����б���*/

int get_ratio_lcrbhz(int bli,int bll,char str[200]);

int rptLC4()
{
	char filename[50];
	char wherelist[1024];
	int ttlnum1 = 0;
	int i = 0;
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

	ret = sql_begin();
	if(ret != 0 )
	{
		sprintf(acErrMsg,"������ʧ�ܣ�");
		WRITEMSG
		goto ErrExit;
	}

	pub_base_sysinit();
	strcpy(filename,"lc_rb_hz");
	strcpy(vpfm,"lc_rb_hz");
	
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
	ret = Fn_parm_Dec_Sel(g_pub_tx.reply,"stat ='1'  and  end_date>=%ld",g_pub_tx.tx_date);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"��ѯ��Ʋ��������!![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		iCount3 = 0;     
		dAmt3 = 0.00; 
		dAmt5 = 0.00; 
		dAmt6 = 0.00; 
		iCount4 = 0; 
		iCount5 = 0; 
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
			
			
		/*	ret = my_sel_func(sCom_branch.br_no,sFn_parm.prdt_code,sFn_parm.beg_date);
			if(ret)
			{
				vtcp_log("��ѯ��ϸ��fn_reg���� ");
				goto ErrExit;
			}
			*/
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



/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{
        ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lcrbhz);
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
int get_ratio_lcrbhz( bli,bll,str )
int bli,bll;
char str[200];
{
	 char fmt[10];
	 struct com_sys_parm_c sCom_sys_parm;
	 struct mdm_ac_rel_c sMdm_ac_rel;
	 struct fn_flt_radio_parm_c sFn_flt_radio_parm;
	 struct dd_mst_c sDd_mst;
	 int ret1=0,ret2=0;
	 char str_mon[30];
	 double dTmp_amt1 = 0.00;
	 double dTmp_amt2 = 0.00;
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

 	 memset(str_mon,'\0',sizeof(str_mon));
	 memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	 memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	 memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));
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
			ret = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code='%s' ",sFn_parm.prdt_code);
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
				rate=sFn_parm.rate*(1+sFn_flt_radio_parm.flt_radio24);
		  if(rate)
			sprintf( t_str , "%.2lf~%.2lf%%" , sFn_parm.rate,rate); 
			else
			sprintf( t_str , "%.2lf%%" , sFn_parm.rate); 
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
    	case 'P':/*��Ȩ����*/
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
		case 'G':/*�ܱ���*/
			iCount  = sql_count("fn_reg"," prdt_code = '%s' and tx_br_no='%s' and sts='1'",sFn_parm.prdt_code,sCom_branch.br_no);
		  iCount3+=iCount;
			sprintf( t_str , "%d" , iCount);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'X':/*�ܽ��*/
			ret = sql_sum_double("fn_reg","tx_amt",&dAmt," prdt_code = '%s' and tx_br_no='%s' and sts='1'",sFn_parm.prdt_code,sCom_branch.br_no);
			/*dAmt /= 10000;*/
			dAmt3 += dAmt;
			sprintf( t_str , "%.2lf" , dAmt);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'H': /*��������*/
		  iCount1  = sql_count("fn_reg"," prdt_code = '%s' and tx_br_no='%s' and sts='1' and chnl_type ='0400'",sFn_parm.prdt_code,sCom_branch.br_no);
			iCount4 += iCount1;
			sprintf( t_str , "%d" , iCount1);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'J': /*�������*/
			ret = sql_sum_double("fn_reg","tx_amt",&dAmt1," prdt_code = '%s' and tx_br_no='%s' and sts='1' and chnl_type ='0400' ",sFn_parm.prdt_code,sCom_branch.br_no);
			/*dAmt1 /=10000; */
			dAmt5 += dAmt1;
			sprintf( t_str , "%.2lf" , dAmt1);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'K': /*�������*/
		/*	ret = sql_sum_double("fn_reg","tx_amt",&dTmp_amt1," prdt_code = '%s' and tx_br_no='%s' and tx_date>=%d",sFn_parm.prdt_code,sCom_branch.br_no,sFn_parm.beg_date);
			ret = sql_sum_double("fn_reg","redempt_amt",&dTmp_amt2," prdt_code = '%s' and tx_br_no='%s' and tx_date>=%d",sFn_parm.prdt_code,sCom_branch.br_no,sFn_parm.beg_date);
			dAmt4 = dTmp_amt1 - dTmp_amt2;*/

			iCount2  = sql_count("fn_reg"," prdt_code = '%s' and tx_br_no='%s' and sts='1' and chnl_type='0100' ",sFn_parm.prdt_code,sCom_branch.br_no);
			iCount5 += iCount2;
			sprintf( t_str , "%d" , iCount2);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'L' :/*������*/
			ret = sql_sum_double("fn_reg","tx_amt",&dAmt2," prdt_code = '%s' and tx_br_no='%s' and sts='1' and chnl_type ='0100' ",sFn_parm.prdt_code,sCom_branch.br_no);
			/*dAmt2 /=10000;*/
      dAmt6 +=dAmt2;
			sprintf( t_str , "%.2lf" , dAmt2);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /*LJ2015116�ֻ����б���*/
		  iCount7  = sql_count("fn_reg"," prdt_code = '%s' and tx_br_no='%s' and sts='1' and chnl_type ='0300'",sFn_parm.prdt_code,sCom_branch.br_no);
			iCount8 += iCount7;
			sprintf( t_str , "%d" , iCount7);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'U': /*LJ2015116�ֻ����н��*/
			ret = sql_sum_double("fn_reg","tx_amt",&dAmt7," prdt_code = '%s' and tx_br_no='%s' and sts='1' and chnl_type ='0300' ",sFn_parm.prdt_code,sCom_branch.br_no);
			/*dAmt1 /=10000; */
			dAmt8 += dAmt7;
			sprintf( t_str , "%.2lf" , dAmt7);
      sprintf(fmt, "%%-%ds", bll);
      sprintf(str, fmt, t_str);
			break;
		case 'N' : /*�����ܱ���*/
			sprintf( t_str , "%d" , iCount3);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'V': /*�����ܽ��*/
			sprintf( t_str , "%.2lf" , dAmt3);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Y': /*������������*/
			sprintf( t_str , "%d" , iCount4);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Z' : /*�����������*/
			sprintf( t_str , "%.2lf" , dAmt5);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'I' : /*���ܹ������*/
			sprintf( t_str , "%d" , iCount5);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'O' : /*���ܹ�����*/
			sprintf( t_str , "%.2lf" , dAmt6);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'c': /*LJ2015116�����ֻ����б���*/
			sprintf( t_str , "%d" , iCount8);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'u' : /*LJ2015116�����ֻ����н��*/
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

