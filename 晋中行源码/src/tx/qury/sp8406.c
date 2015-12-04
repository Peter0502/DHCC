/***************************************************************
* 文 件 名: sp8406.c
* 功能描述：网点存款查询
* 作    者: xxxxx
* 完成日期：2003年09月23日
*
* 修改记录：
* 日    期:2003.12.8
* 修 改 人:xxxx
* 修改内容:加入权限及总行查询
*
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "Com_branch_Sel Error!!" ); \
		WRITEMSG \
		return 1; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "com_tel_c.h"

struct com_branch_c g_com_branch;
struct com_tel_c g_com_tel;
char filename[100];
FILE *fp;
double all_bal,p_bal,c_bal,ys_bal;
double tmp_b1,tmp_b2,tmp_b3,tmp_b4,tmp_b5;
int ret;

sp8406()
{
  /*** 初始化全局变量 ***/
  char br_no[5];
  int cnt,tmp_cnt;
  
  ret=cnt=tmp_cnt=0;
  memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
  memset(&g_com_branch,0x00,sizeof(struct com_branch_c));  
  pub_base_sysinit();
  get_zd_data("0910",g_pub_tx.tx_br_no);
  
  ret=Com_tel_Sel(g_pub_tx.reply ,&g_com_tel,"tel='%s'",g_pub_tx.tel);
  
  if((strcmp(g_com_tel.br_no,"61000"))&&(strcmp(g_com_tel.br_no,"99999"))&&(strcmp(g_com_tel.br_no,"61099"))&&(strcmp(g_com_tel.br_no,"61056"))&&(strcmp(g_com_tel.br_no,"61058")))
  {
   if(strcmp(g_com_tel.br_no,g_pub_tx.tx_br_no))
   {
   	 ret=Com_branch_Dec_Sel(g_pub_tx.reply ,"up_br_no='%s'",g_com_tel.br_no);
   	 if(ret)
     {
      sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
   	  WRITEMSG
      strcpy(g_pub_tx.reply,"D101");
      goto ErrExit;
     }
     
     while(1)
     {
      memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
      ret=Com_branch_Fet_Sel(&g_com_branch,g_pub_tx.reply);
      if (ret==100)
       break;
      else ERR_DEAL
      	
      cnt++;
      if(strcmp(g_com_branch.br_no,g_pub_tx.tx_br_no)) tmp_cnt++; 
     }
     
     if(cnt==tmp_cnt)
     	{
     		sprintf( acErrMsg, "该柜员无此权限查询该机构[%s]",g_pub_tx.tx_br_no); 
		    WRITEMSG 
		    strcpy(g_pub_tx.reply,"S073");
		    goto ErrExit;
     	} 
    }
  }
  
  pub_base_AllDwnFilName( filename );
  fp = fopen( filename,"w" );
  if( fp==NULL )
  {
    sprintf(acErrMsg," open file [%s] error ",filename );
    WRITEMSG
    strcpy( g_pub_tx.reply,"S047" );
    goto ErrExit;
  }
  /**显示打印标题**/
  fprintf(fp,"~开户机构名称|@开户机构码|$存款余额|$对公|$对私|$比昨日|\n");
 
 	if((!strcmp(g_pub_tx.tx_br_no,"61000"))||(!strcmp(g_pub_tx.tx_br_no,"99999"))||(!strcmp(g_pub_tx.tx_br_no,"61099"))||(!strcmp(g_pub_tx.tx_br_no,"61056"))||(!strcmp(g_pub_tx.tx_br_no,"61058")))
  	{
  		strcpy(br_no,"61002");
  		if(Deposit_query(br_no)) goto ErrExit;
  		strcpy(br_no,"61003");
  		if(Deposit_query(br_no)) goto ErrExit;
  		strcpy(br_no,"61004");
  		if(Deposit_query(br_no)) goto ErrExit;
  		strcpy(br_no,"61006");
  		if(Deposit_query(br_no)) goto ErrExit;
  		strcpy(br_no,"61007");
  		if(Deposit_query(br_no)) goto ErrExit;
  		strcpy(br_no,"61009");
  		if(Deposit_query(br_no)) goto ErrExit;
  	}
   else if(Deposit_query(g_pub_tx.tx_br_no)) goto ErrExit;
	
fclose(fp);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"存款实时查询成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_FILE_SND,"1");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"存款实时查询失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
} 
  
int Deposit_query(char* br_no)
{ 
 memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
 memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
 memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
 
 ret=0;
 ys_bal=p_bal=c_bal=all_bal=0.00;

 ret=Com_branch_Dec_Sel(g_pub_tx.reply ,"up_br_no='%s'",br_no); 
 /*update g_pub_tx.tx_br_no by xxxx */
 if(ret)
  {
   sprintf(acErrMsg,"[DECLARE CURSOR]异常!!");
   WRITEMSG
   strcpy(g_pub_tx.reply,"D101");
   return 1;
  }  
  
  while(1)
  {
   ret=Com_branch_Fet_Sel(&g_com_branch,g_pub_tx.reply);
   if (ret==100)
        break;
   else ERR_DEAL
      
  tmp_b1=tmp_b2=tmp_b3=tmp_b4=tmp_b5=0.00;
  
  ret=sql_sum_double("dd_mst","bal",&tmp_b1,"opn_br_no='%s' and cal_code='1'", \
		                 g_com_branch.br_no);
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   p_bal+=tmp_b1;
   all_bal+=tmp_b1;
      
  ret=sql_sum_double("td_mst","bal",&tmp_b2,"opn_br_no='%s' and cal_code='1'", \
		     g_com_branch.br_no);
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   p_bal+=tmp_b2;
   all_bal+=tmp_b2;
  
  ret=sql_sum_double("dd_mst","bal",&tmp_b3,"opn_br_no='%s' and (cal_code='2' or cal_code='3' or cal_code='4')", \
		     g_com_branch.br_no);
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   c_bal+=tmp_b3;
   all_bal+=tmp_b3;
      
  ret=sql_sum_double("td_mst","bal",&tmp_b4,"opn_br_no='%s' and (cal_code='2' or cal_code='3' or cal_code='4')", \
		     g_com_branch.br_no);
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   c_bal+=tmp_b4;
   all_bal+=tmp_b4;
  
  ret=sql_sum_double("dd_mst","ys_bal",&tmp_b5,"opn_br_no='%s'", \
		     g_com_branch.br_no);
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   ys_bal+=tmp_b5;
   
   tmp_b1+=tmp_b2;
   tmp_b3+=tmp_b4;
   tmp_b2=tmp_b1+tmp_b3;
   tmp_b4=tmp_b2-tmp_b5;
      
   fprintf(fp,"%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|\n",\
              g_com_branch.br_name,g_com_branch.br_no,tmp_b2,tmp_b3,tmp_b1,tmp_b4);
	}   
 
  tmp_b1=tmp_b2=tmp_b3=tmp_b4=tmp_b5=0.00;
  ret=Com_branch_Sel(g_pub_tx.reply ,&g_com_branch,"br_no='%s'" , \
           	         br_no );  /*update g_pub_tx.tx_br_no by xxxx */
  if(ret==100)
  {
    sprintf(acErrMsg,"没有该机构号!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"O116");
    return 1;	
  }else ERR_DEAL
  
  ret=sql_sum_double("dd_mst","bal",&tmp_b1,"opn_br_no='%s' and cal_code='1'", \
		     br_no);/*update g_pub_tx.tx_br_no by xxxx */
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   p_bal+=tmp_b1;
   all_bal+=tmp_b1;
      
  ret=sql_sum_double("td_mst","bal",&tmp_b2,"opn_br_no='%s' and cal_code='1'", \
		     br_no);/*update g_pub_tx.tx_br_no by xxxx */
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   p_bal+=tmp_b2;
   all_bal+=tmp_b2;
  
  ret=sql_sum_double("dd_mst","bal",&tmp_b3,"opn_br_no='%s' and (cal_code='2' or cal_code='3' or cal_code='4')", \
		     br_no);/*update g_pub_tx.tx_br_no by xxxx */
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   c_bal+=tmp_b3;
   all_bal+=tmp_b3;
      
  ret=sql_sum_double("td_mst","bal",&tmp_b4,"opn_br_no='%s' and (cal_code='2' or cal_code='3' or cal_code='4')", \
		     br_no);/*update g_pub_tx.tx_br_no by xxxx */
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
   c_bal+=tmp_b4;
   all_bal+=tmp_b4;
  
  ret=sql_sum_double("dd_mst","ys_bal",&tmp_b5,"opn_br_no='%s'", \
		     br_no);/*update g_pub_tx.tx_br_no by xxxx */
  if( ret<0 )
   {
    sprintf(acErrMsg,"查找失败!");
    WRITEMSG
    return 1;
   }
  ys_bal+=tmp_b5;
  tmp_b1=all_bal-ys_bal;
  Com_branch_Clo_Sel();
  fprintf(fp,"%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_com_branch.br_name,\
          g_com_branch.br_no,all_bal,c_bal,p_bal,tmp_b1);
  return 0;
}

