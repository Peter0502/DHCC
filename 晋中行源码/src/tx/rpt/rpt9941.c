/*************************************************************
* �� �� ��:  rpt9941.c
* ���������� �ձ����ӡ
* ��    ��:  
* �������:  2003��08��15��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "[%s][%d] sql error",__FILE__,__LINE__ ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  MAX_RECS    48    /** һҳ��ӡ���ٱʼ�¼ **/
#define  MAX_LINES   110   /** һҳ����ӡ������ **/
#define  AMT_LEN     16    /** �����н��ĳ��� **/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"

FILE *fp;            /** �����ļ�ָ�� **/
int totpage=0;       /** �ܹ���ҳ�� **/
int page=0;          /** ��ǰҳ�� **/
int ret = 0;         /** �����־: 0-��ȷ  ����-���� **/
int totcnt = 0;      /** ��¼�ܱ��� **/
int lines = 0;       /** ��ǰ��ӡ���ڼ��� **/
int opncnt = 0;
char vpfm[21];         /** �����ʽ�� **/
char   old_acno[8];    /** �����ϴ�ѭ���Ŀ�Ŀ������,�����жϿ�Ŀ�������Ƿ�仯 **/
char   old_brno[8];    /** �����ϴλ�������,���ڿ��Ʊ�������ʹ����µı����ļ� **/
char   filename[100];  /** �����ļ��� **/

struct gl_mst_c	s_gl_mst;
struct com_sys_parm_c s_com_sys_parm;
struct {
	char ac_no[8];
	char ac_name[31];
	char act_nums[7];
	char lst_dr_bal[17];
	char lst_cr_bal[17];
	char dr_txamt[17];
	char cr_txamt[17];
	char td_act_nums[7];
	char td_dr_bal[17];
	char td_cr_bal[17];
	char td_tx_nums[7];
} prt_data;
int get_ratio_lsqd(int bli,int bll,char str[100]);
void rpt9441_init();
int rpt_9441_prepare_hj_data();
int rpt_9441_prepare_data();
int rpt_9441_end();
int rpt_9441_print_tail();
int rpt_9441_page_nums();
int rpt_9441_print_head();

int rpt9941()
{
	/** ������,�������ݿ����� **/
	ret=sql_begin();
	if( ret )
	{
	  sprintf( acErrMsg, "[%s][%d] ������ʧ��!!!",__FILE__,__LINE__ );
		WRITEMSG
		goto ErrExit;
	}
	
	/** ��ʼ��ȫ�ֱ��� **/
	rpt9441_init();
	
		
	/* ȡ�����ӡ���� */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
	
	
	/** �����ʽ���������� **/
	strcpy( filename , "BHDRPT101" );	
	strcpy( vpfm, "BHDRPT101" );
	

	/** ��ѯ����(�����) **/
	ret = Gl_mst_Dec_Sel( g_pub_tx.reply, "cur_no='01' ORDER BY acc_hrt" );
	ERR_DEAL
	
	while(1)
	{
	   ret = Gl_mst_Fet_Sel( &s_gl_mst,g_pub_tx.reply );
	   
	   /** �жϲ�ѯ�Ƿ�������� **/
	   rpt_9441_end();
	   if( ret ) 
	   {
      sprintf(acErrMsg,"[%s][%d]��ȡ��ˮ��Ϣ���쳣����!",__FILE__,__LINE__);
      WRITEMSG	
      goto ErrExit;			
		 }
		
		 /** �����ı�ʱ,������һ������,��ʼ������һ�����������ļ� **/
		 if( memcmp(old_brno,s_gl_mst.br_no,sizeof(s_gl_mst.br_no)) )	
		 {      
         lines = 0;
        	       
         /** ɾ��ԭ�ļ� **/
	       ret = pub_rpt_rmfile( s_gl_mst.br_no , filename , 0 );
	       ERR_DEAL

	       /** ���ļ� **/	
	       ret = pub_rpt_openfile( &fp, s_gl_mst.br_no , filename );
	       ERR_DEAL
	       
	       /** ����ҳ���� **/
	       ret = rpt_9441_page_nums();
	       ERR_DEAL
	       
	       /** ��ӡ��ͷ **/
	       ret = rpt_9441_print_head();
	       ERR_DEAL
		 }
		 
		 /** �жϣ���ҳ��ҳ **/
		 if ( lines >= MAX_LINES )
		 {
		 	   rpt_9441_print_tail();
		 	   ret = rpt_9441_print_head();
	       ERR_DEAL
		 }
		
		 /** �жϿ�Ŀ�����ı�ʱ��ӡ�ϼ� **/
		 if( s_gl_mst.acc_hrt[0] != old_acno[0] )
	 	 {
	 	 	 /** ������ڿ�Ŀ�����������Ŀ��ʼ������ʽ **/
	 	 	 if(old_acno[0] == '5' && s_gl_mst.acc_hrt[0] == '6')
	 	 	 {
	 	 	    ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","CC",&opncnt,get_ratio_lsqd);
	 	 	 }
	 	 	 
	 	 	 /** ��ӡ�ϼƺ����ж�һ�飬��ҳ��ҳ **/
		   if ( lines >= MAX_LINES )
		   {
		 	     rpt_9441_print_tail();
		 	     ret = rpt_9441_print_head();
	         ERR_DEAL
		   }
	 	 	 
		   ret = rpt_9441_prepare_hj_data();
		   if( ret ) {
         sprintf(acErrMsg,"[%s][%d]�����Ŀ�ϼƳ���!",__FILE__,__LINE__);
         WRITEMSG			
         goto ErrExit;			
		   }
		   
		   /** ��ӡ�ϼƺ����ж�һ�飬��ҳ��ҳ **/
		   if ( lines >= MAX_LINES )
		   {
		 	     rpt_9441_print_tail();
		 	     ret = rpt_9441_print_head();
	         ERR_DEAL
		   }
		 }
		 memcpy(old_acno,s_gl_mst.acc_hrt,sizeof(s_gl_mst.acc_hrt)-1);
		
		 /** ����ӡ�ṹ��ֵ **/
		 ret = rpt_9441_prepare_data();
		 if( ret ) 
		 {
         sprintf(acErrMsg,"[%s][%d]�����ӡ����!",__FILE__,__LINE__);
         WRITEMSG			
         goto ErrExit;			
		 }
		
		 /** ��¼�������� **/
     totcnt++;
		
	} /** while ѭ������ **/
	
	
	OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ձ������ɴ���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�ձ������ɴ���ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

void rpt9441_init()
{
	memset(vpfm,0,sizeof(vpfm));
	memset(filename,0,sizeof(filename));
	memset(old_acno,0,sizeof(old_acno));
	memset(old_brno,0,sizeof(old_brno));
	pub_base_sysinit();
}	

int rpt_9441_prepare_hj_data()
{
	  char   amt_tmp[31];
	  double tmp_amt;
	  memset(&prt_data,0,sizeof(prt_data));
	
	  /** 6 - 7 ���Ŀ���鵽���� **/
	  if(old_acno[0] == '6' && s_gl_mst.acc_hrt[0] =='7')
	  	return(0);
	
    /** �ϼ����� **/
    switch(s_gl_mst.acc_hrt[0])
    {
    	case '1':
    		sprintf(prt_data.ac_name,"һ���ʲ���С��");
    		break;
    	case '2':
    		sprintf(prt_data.ac_name,"������ծ��С��");
    		break;
    	case '3':
    		sprintf(prt_data.ac_name,"����������Ȩ����С��");
    		break;
    	case '4':
    		sprintf(prt_data.ac_name,"�ġ��ʲ���ծ��ͬ��С��");
    		break;
    	case '5':
    		sprintf(prt_data.ac_name,"�塢������С��");
    		break;
    	case '6':
    		sprintf(prt_data.ac_name,"�����Ŀ�ϼ�");
    		break;   		
    	default:
    		return(-1);
    }
    
    /** ���ս跽��� **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","ldd_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.lst_dr_bal,amt_tmp,sizeof(prt_data.lst_dr_bal)-1);
	  
	  /** ���մ������ **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","lcd_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.lst_cr_bal,amt_tmp,sizeof(prt_data.lst_cr_bal)-1);
	  
	  /** ���ս跽������ **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","rdd_amt",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.dr_txamt,amt_tmp,sizeof(prt_data.dr_txamt)-1);
	  
	  /** ���մ��������� **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","rcd_amt",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.cr_txamt,amt_tmp,sizeof(prt_data.cr_txamt)-1);
	  
	  /** ���ս跽��� **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","dr_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.td_dr_bal,amt_tmp,sizeof(prt_data.td_dr_bal)-1);
	  
	  /** ���մ������ **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","cr_bal",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
	  pub_rpt_flttomon( amt_tmp,amt_tmp );
	  if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt );
	  strncp(prt_data.td_cr_bal,amt_tmp,sizeof(prt_data.td_cr_bal)-1);
	  
	  /** ���շ����� **/
    tmp_amt = 0;
    memset(amt_tmp,0,sizeof(amt_tmp));
    ret = sql_sum_double( "gl_mst","rdd_cnt+rcdcnt",&tmp_amt,"acc_hrt[1]='%c' \
          and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) ", s_gl_mst.acc_hrt[0] );
    if(ret)
    	return(ret);
    	
    sprintf( amt_tmp, "%6.0lf" , tmp_amt ); 
	  strncp(prt_data.td_tx_nums,amt_tmp,sizeof(prt_data.td_tx_nums)-1);
		
    if(s_gl_mst.acc_hrt[0] < '6')
	     ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","AA",&opncnt,get_ratio_lsqd);
	  else
	 	   ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","DD",&opncnt,get_ratio_lsqd);	 
  
    return(ret);
}

int rpt_9441_prepare_data()
{
	 char   amt_tmp[31];
	 struct com_item_c s_com_item;
	
   memset(&prt_data,0,sizeof(prt_data));
  	
   /** ��Ŀ **/
   pub_base_strpack(s_gl_mst.acc_hrt);
	 strcpy(prt_data.ac_no,s_gl_mst.acc_hrt);

   /** ��Ŀ���� **/
   ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								prt_data.ac_no );
   if(ret)
      return(ret);
   pub_base_strpack(s_com_item.acc_name);
	 strcpy(prt_data.ac_name,s_com_item.acc_name);
	 
	 /** ���ջ��� **/
	 
	 /** ���ս跽��� **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.ldd_bal );
	 strncp(prt_data.lst_dr_bal,amt_tmp,sizeof(prt_data.lst_dr_bal)-1);
	 
	 /** ���մ������ **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.lcd_bal );
	 strncp(prt_data.lst_cr_bal,amt_tmp,sizeof(prt_data.lst_cr_bal)-1);
	 
	 /** ���ս跽������ **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rdd_amt );
	 strncp(prt_data.dr_txamt,amt_tmp,sizeof(prt_data.dr_txamt)-1);
	 
	  /** ���մ��������� **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.rcd_amt );
	 strncp(prt_data.cr_txamt,amt_tmp,sizeof(prt_data.cr_txamt)-1);
	 
	 /**���ջ��� **/
	 
	 /** ���ս跽��� **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.dr_bal );
	 strncp(prt_data.td_dr_bal,amt_tmp,sizeof(prt_data.td_dr_bal)-1);
	 
	 /** ���մ������ **/
	 memset(amt_tmp,0,sizeof(amt_tmp));
	 sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal ); 
	 pub_rpt_flttomon( amt_tmp,amt_tmp );
	 if( strlen(amt_tmp)>AMT_LEN ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_mst.cr_bal );
	 strncp(prt_data.td_cr_bal,amt_tmp,sizeof(prt_data.td_cr_bal)-1);
	 
	 /** ҵ���� **/
	 sprintf(prt_data.td_tx_nums,"%06d",s_gl_mst.rdd_cnt+s_gl_mst.rcd_cnt);
	 
	 if(old_acno[0] < '6')
	     ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","AA",&opncnt,get_ratio_lsqd);
	 else
	 	   ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","DD",&opncnt,get_ratio_lsqd);	 
  
   return(ret);
}

int rpt_9441_end()
{
	   if( ret==100 ) 
     {
       if( totcnt )
       {
         /** ����,��ҳβ **/
         rpt_9441_print_tail();
         return(ret);
         
       } else {
         sprintf( acErrMsg, "[%s][%d]����û��Ҫ��ӡ����ˮ!" ,__FILE__,__LINE__);
         WRITEMSG
         return(ret);
       }
     } else if( ret ) {
        sprintf(acErrMsg,"[%s][%d]��ȡ��ˮ��Ϣ���쳣����!",__FILE__,__LINE__);
        WRITEMSG	
        return(ret);				
		 }
}

int rpt_9441_print_tail()
{
	  if( old_acno[0]<6)
        ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","BB",&opncnt,get_ratio_lsqd);
    else
    	  ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","DD",&opncnt,get_ratio_lsqd);
    return(ret);
}

int rpt_9441_page_nums()
{
	  long lpage = 0;
	  
	  /** ͳ�Ƶ�ǰ������Ŀ��¼�� **/
	  ret = sql_sum_long("gl_mst","acc_hrt",&lpage," where br_no = '%s' ",s_gl_mst.br_no);
	  if( ret ) 
	  {
      sprintf(acErrMsg,"[%s][%d]ͳ�Ƽ�¼������!",__FILE__,__LINE__);
      WRITEMSG	
      return(ret);
    }
    
    /** ����ҳ������ **/
    totpage = (int) (lpage / MAX_RECS);
    
    totpage++;
    
}

int rpt_9441_print_head()
{
   ret=pub_rpt_read_pfm_qd(fp,&lines,vpfm,"0001","HH",&opncnt,get_ratio_lsqd);
   return(ret);
}

/* ��ֵ���� */
/* ˵����bli ��������ַ����� ... bll�������ռ�ĳ��� ... str�������ֵ */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char t_str[101]; 
	char fmt[20]; 

	switch( bli ) 
	{ 
		case 'A': /* ������� */ 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_gl_mst.br_no ); 
			break;
		case 'B': /* �������� */
			ret = pub_base_getbrname( s_gl_mst.br_no, t_str );
			ERR_DEAL
			pub_base_strpack( t_str ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'P': /* �������� */ 
			sprintf( t_str , "%4d��%2d��%2d��" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
    case 'C': /* ҳ��ϼ� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, totpage); 
			break; 
		case 'D': /* ҳ�� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, page); 
			break;
    case 'E': /* ��Ŀ��� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.ac_no); 
			break;
		case 'F': /* ��Ŀ���� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.ac_name); 
			break;
		case 'G': /* ���ջ��� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.act_nums); 
			break;
		case 'H': /* ���ս跽��� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.lst_dr_bal); 
			break;
		case 'I': /* ���մ������ */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.lst_cr_bal); 
			break;
		case 'J': /* ���ս跽������ */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.dr_txamt); 
			break;
		case 'K': /* ���մ��������� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.dr_txamt); 
			break;
		case 'L': /* ���ջ��� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_act_nums); 
			break;
		case 'M': /* ���ս跽��� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_dr_bal); 
			break;
		case 'N': /* ���մ������ */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_cr_bal); 
			break;
		case 'O': /* ����ҵ���� */
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt,  prt_data.td_tx_nums); 
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
