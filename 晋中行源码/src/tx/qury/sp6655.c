/*************************************************************
**     文 件 名：sp6655.c
**     功能描述：营业状态查询
**
**     作    者：
**     完成日期：20060831
**     修改记录：
**     日    期：
**     修 改 人：
**     修改内容：
**************************************************************/
#define ERR_DEAL if(ret)\
                 {  sprintf(acErrMsg, "sql error");\
                    WRITEMSG\
                    goto ErrExit;\
                 }
#define EXTERN
#include <string.h>
#include "hvstat_c.h"
#include "public.h"

int sp6655()
{
  struct hvstat_c   s_hvstat;
  struct hvstat_c   d_hvstat;
  char filename[100];
  char wherelist[1024];
  char tmpstr[512];
  char txstat[41];
  int totcnt=0;
  int ret=0;
  FILE *fp;

  /****初始化全局变量*****/
  memset(&s_hvstat,0,sizeof(struct hvstat_c));
  memset(&d_hvstat,0,sizeof(struct hvstat_c));
  memset(filename,0,sizeof(filename));
  memset(wherelist,0,sizeof(wherelist));
  memset(tmpstr,0,sizeof(tmpstr));
  memset(&g_pub_tx,0,sizeof( g_pub_tx));
  memset(txstat,0,sizeof(txstat));
  pub_base_sysinit();

  /*****取前台传送值*****/
  get_zd_long("0440",&s_hvstat.tx_date);

  /****组合查询条件******/
  vtcp_log("[%s],[%d],s_hvstat.tx_date=[%l]",__FILE__,__LINE__,s_hvstat.tx_date);
  if(s_hvstat.tx_date)
  {
    sprintf(tmpstr,"tx_date=%d and ",s_hvstat.tx_date);
    strcat(wherelist,tmpstr);
  }
  strcat(wherelist,"1=1 ORDER BY tx_date");
  vtcp_log("[%s],[%d]wherelist=[%s]",wherelist,__FILE__,__LINE__);

  /******查询取值*******/
  ret=Hvstat_Dec_Sel(g_pub_tx.reply,wherelist);
  ERR_DEAL
  while(1)
  {  
    ret=Hvstat_Fet_Sel(&d_hvstat,g_pub_tx.reply);
    if(ret==100)
      break;
    ERR_DEAL
    if(!totcnt)
    {
      pub_base_AllDwnFilName(filename);
      fp=fopen(filename,"w");
      if(fp==NULL)
      {
        sprintf(acErrMsg,"open file error [%s]",filename);
        WRITEMSG
        strcpy(g_pub_tx.reply,"S047");
        goto ErrExit;
      }
      /****显示列标题*****/
      fprintf(fp,"~营业日期  |@营业状态            |@状态描述|\n");
    }
      vtcp_log("[%s],[%d],tx_date=[%d],txstat=[%s],message=[%s]",__FILE__,__LINE__,d_hvstat.tx_date,
d_hvstat.stat,d_hvstat.message);

    /****选择营业状态*********/
    switch (d_hvstat.stat[0])
     {
        case '0':
                memcpy(txstat, "营业准备" , sizeof(txstat));
                break;
        case '1':
                memcpy(txstat, "日间处理" , sizeof(txstat));
                break;
        case '2':
                memcpy(txstat, "截至警告" , sizeof(txstat));
                break;
        case '3':
                memcpy(txstat, "业务截至" , sizeof(txstat));
                break;
        case '4':
                memcpy(txstat, "清算窗口" , sizeof(txstat));
                break;
        case '5':
                memcpy(txstat, "日终处理" , sizeof(txstat));
                break;
        case '6':
                memcpy(txstat, "对帐不平" , sizeof(txstat));
                break;
        case '7':
                memcpy(txstat, "对帐以平" , sizeof(txstat));
                break;
        case 'F':
                sprintf(txstat,"未知状态" );
                break;
    }

    vtcp_log("[%s],[%d],tx_date=[%d],txstat=[%s],message=[%s]",__FILE__,__LINE__,d_hvstat.tx_date,txstat,d_hvstat.message);
    fprintf(fp,"%d|%s|%s|\n",d_hvstat.tx_date,txstat,d_hvstat.message);
    totcnt++;
  }

  if(!totcnt)
  {
    strcpy(g_pub_tx.reply,"S049");
    goto ErrExit;
  }
  else
  {
    fclose(fp);
    set_zd_data(DC_FILE_SND,"1");
  }

  /******当查询记录为一条时，输入以下变量******/
  if(totcnt==1)
  {
    pub_base_AllDwnFilName(filename);
    fp=fopen(filename,"w");
    if(fp==NULL)
    {
      sprintf(acErrMsg,"open file [%s] error",filename);
      WRITEMSG
      strcpy(g_pub_tx.reply,"S049");
      goto ErrExit;
    }
      vtcp_log("[%s],[%d],tx_date=[%d],txstat=[%s],message=[%s]",__FILE__,__LINE__,d_hvstat.tx_date,txstat,d_hvstat.message);
    fprintf(fp,"CKPT%d|%s|%s|\n",d_hvstat.tx_date,txstat,d_hvstat.message);
  }
  ret=Hvstat_Clo_Sel();
  ERR_DEAL
  fclose(fp);
  set_zd_data( DC_FILE_SND,"1" );
 /***程序出口***/
GoodExit:
       strcpy( g_pub_tx.reply, "0000" );
       sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,g_pub_tx.reply);
       return 0;
ErrExit:
       sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY ,g_pub_tx.reply);
       return 1;
}


