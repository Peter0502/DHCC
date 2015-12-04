/****************************************************************************************
*程序名: spJ423.c
*功能:   退票理由查询交易
*
*日期:   2007-4-18 22:58
*作者:   xyy
**
****************************************************************************************/
#define ERR_DEAL if(ret){ \
	 sprintf(acErrMsg,"sql error.[%d]",ret);\
 	WRITEMSG \
 	goto ErrExit;\
}

#include <string.h>
#include <stdio.h>
#define EXTERN
#include "lv_define.h"
#include "hv_define.h"
#include "public.h"

#include "public.h"
#include "lv_param_c.h"

int spJ423()
{
	FILE *fp;
	char filename[100];
	int  ret=0;
	int  cnt=0;
	int  i  =0;
	int  iCount=0;
  char cTmpBuff [5];
  char cTxname[21];
	struct lv_param_c lv_param;
	
	memset(filename , 0x00, sizeof(filename));
	memset(&lv_param, 0x00, sizeof(struct lv_param_c));
  memset(cTmpBuff , 0x00, sizeof(cTmpBuff));
  memcpy(cTmpBuff , "0000" , sizeof(cTmpBuff)-1);
	pub_base_sysinit();

  iCount = sql_count("lv_param", "par_code like 'TPLY%%' order by par_code ");
  vtcp_log("[%s][%d]iCount=[%d]\n",__FILE__,__LINE__,iCount);

  ret=Lv_param_Dec_Sel(g_pub_tx.reply," par_code like 'TPLY%%' order by par_code ");
  ERR_DEAL
  while(1)
  {
  	  memset(&lv_param, 0x00 , sizeof(struct lv_param_c));
  	  ret=Lv_param_Fet_Sel(&lv_param,g_pub_tx.reply);
  	  if(ret == 100) break;
  	  else ERR_DEAL
  	  
  	  if(!cnt)
  	  {
  	  	  pub_base_AllDwnFilName( filename );
          fp = fopen( filename,"w" );
          if(!fp)
          {
           		sprintf(acErrMsg," open file [%s] error ",filename );
           		WRITEMSG
           		strcpy( g_pub_tx.reply,"S047" );
           		goto ErrExit;
          }
  	  }
  	  memset(cTxname , '\0',sizeof(cTxname));
  	  memcpy(cTxname,lv_param.par_name,sizeof(cTxname)-1);
  	  vtcp_log("[%s][%d]cnt=[%d]\n",__FILE__,__LINE__,cnt);
  	  for(i=0;i<20;i++)
  	  {
  	  	  if(cTxname[i]=='\0')
  	  	  {
  	  	  	 cTxname[i]=' ';
  	  	  }
  	  }
  	  if(cnt < iCount)
  	  {
  	      fprintf(fp,"%s|%s|%s|",cTmpBuff,lv_param.par_value,cTxname);
  	  }
  	  cnt++;
  }
  Lv_param_Clo_Sel();
  if(!cnt)
  {
  	strcpy( g_pub_tx.reply,"S049" );
  	goto ErrExit;
  }
  else
  {
  	fclose(fp);
  	set_zd_data( DC_FILE_SND,"1" );
  }
  vtcp_log("[%s][%d]共取得[%d]条记录!!\n",__FILE__,__LINE__,cnt);	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"OK exit!! reply is:[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail exit!! reply is:[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
