/*************************************************************
**     �� �� ����sp6655.c
**     ����������Ӫҵ״̬��ѯ
**
**     ��    �ߣ�
**     ������ڣ�20060831
**     �޸ļ�¼��
**     ��    �ڣ�
**     �� �� �ˣ�
**     �޸����ݣ�
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

  /****��ʼ��ȫ�ֱ���*****/
  memset(&s_hvstat,0,sizeof(struct hvstat_c));
  memset(&d_hvstat,0,sizeof(struct hvstat_c));
  memset(filename,0,sizeof(filename));
  memset(wherelist,0,sizeof(wherelist));
  memset(tmpstr,0,sizeof(tmpstr));
  memset(&g_pub_tx,0,sizeof( g_pub_tx));
  memset(txstat,0,sizeof(txstat));
  pub_base_sysinit();

  /*****ȡǰ̨����ֵ*****/
  get_zd_long("0440",&s_hvstat.tx_date);

  /****��ϲ�ѯ����******/
  vtcp_log("[%s],[%d],s_hvstat.tx_date=[%l]",__FILE__,__LINE__,s_hvstat.tx_date);
  if(s_hvstat.tx_date)
  {
    sprintf(tmpstr,"tx_date=%d and ",s_hvstat.tx_date);
    strcat(wherelist,tmpstr);
  }
  strcat(wherelist,"1=1 ORDER BY tx_date");
  vtcp_log("[%s],[%d]wherelist=[%s]",wherelist,__FILE__,__LINE__);

  /******��ѯȡֵ*******/
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
      /****��ʾ�б���*****/
      fprintf(fp,"~Ӫҵ����  |@Ӫҵ״̬            |@״̬����|\n");
    }
      vtcp_log("[%s],[%d],tx_date=[%d],txstat=[%s],message=[%s]",__FILE__,__LINE__,d_hvstat.tx_date,
d_hvstat.stat,d_hvstat.message);

    /****ѡ��Ӫҵ״̬*********/
    switch (d_hvstat.stat[0])
     {
        case '0':
                memcpy(txstat, "Ӫҵ׼��" , sizeof(txstat));
                break;
        case '1':
                memcpy(txstat, "�ռ䴦��" , sizeof(txstat));
                break;
        case '2':
                memcpy(txstat, "��������" , sizeof(txstat));
                break;
        case '3':
                memcpy(txstat, "ҵ�����" , sizeof(txstat));
                break;
        case '4':
                memcpy(txstat, "���㴰��" , sizeof(txstat));
                break;
        case '5':
                memcpy(txstat, "���մ���" , sizeof(txstat));
                break;
        case '6':
                memcpy(txstat, "���ʲ�ƽ" , sizeof(txstat));
                break;
        case '7':
                memcpy(txstat, "������ƽ" , sizeof(txstat));
                break;
        case 'F':
                sprintf(txstat,"δ֪״̬" );
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

  /******����ѯ��¼Ϊһ��ʱ���������±���******/
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
 /***�������***/
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


