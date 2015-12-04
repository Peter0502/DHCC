/*************************************************
* ��  ����:  sp9669.c
* ��������: �ͻ�ͼƬ��Ϣ¼��
* ��    ��: liuyong
* �������: 2009��08��14��
* �޸ļ�¼:   
*     1.��    ��:
*       �� �� ��:
*       �޸�����:
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#define EXTERN
#include "public.h"
#include "pub_tx.h"
#include "cif_photo_inf_c.h"
#include "cif_cop_photo_c.h"
#include "cif_id_code_rel_c.h"

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;
static struct cif_id_code_rel_c cif_id_code_rel;

sp9669()
{ 	
  int ret = 0;
  char flag[2];
  char filename[100];
  char brno[10];
  char ttyno[5];
  char tmpfile[100];
  char picflag[2];
  char txcode[21];
  FILE *fp;
  
  /** ��ʼ��ȫ�ֱ��� **/
  pub_base_sysinit();

  /** ���ݳ�ʼ�� **/
  memset(filename,0x00,sizeof(filename));
  memset(tmpfile,0x00,sizeof(tmpfile));
  memset(brno,0x00,sizeof(brno));
  memset(ttyno,0x00,sizeof(ttyno));
  memset(txcode,0x00,sizeof(txcode));
  memset(picflag,0x00,sizeof(picflag));
  memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
  memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c)); 
  memset(&cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

  /** ȡֵ����ֵ **/
  get_zd_long("0280",&cif_photo_inf.cif_no);		/* �ͻ��� */
  get_zd_long("0280",&cif_id_code_rel.cif_no);
  get_zd_long("0280",&cif_cop_photo.cif_no);
  get_zd_data("0620",txcode);                    /*֤������*/
  get_zd_data(DC_TX_BR_NO,brno);
  get_zd_data(DC_TTY,ttyno); 
  get_zd_data("1270",flag); 

  sprintf(acErrMsg,"�ͻ���[%ld]",cif_photo_inf.cif_no);
  WRITEMSG
	
  sprintf(acErrMsg,"�Ƿ�����ͼƬ��ʶ[%s]",flag);
  WRITEMSG
  
  sprintf(acErrMsg,"ͼƬ���ͣ�[%s]",picflag);
  WRITEMSG
  
  /** �������� **/
  if(strlen(flag) == 0)
  {
     goto OkExit;	
  }
  if(flag[0] !='1')
  {
    goto OkExit;
  } 
  sprintf(acErrMsg,"�����´��ļ���");
  WRITEMSG

  /*if(strlen(txcode)==0)
  {*/
      /*ȡ�ͻ�֤������*/ 
      ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
        "cif_no=%ld and rowid in (select min(rowid) from cif_id_code_rel where cif_no =%ld)",cif_photo_inf.cif_no,cif_photo_inf.cif_no); 
      if(ret ==100)
      {
          sprintf(acErrMsg,"�ͻ��Ų����ڣ�[%s],ȡ�ͻ�֤���ų���",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"C007");
          ERR_DEAL;
      }
      else if(ret)
      {
          sprintf(acErrMsg,"��ȡ�ͻ�֤����Ϣ�쳣��[%s]",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"C006");
          ERR_DEAL;
      }
      memcpy(txcode,cif_id_code_rel.id_no,strlen(cif_id_code_rel.id_no));
  /*}*/

  /*pub_base_strpack(picflag);*/
  pub_base_strpack(txcode); 
  pub_base_strpack (brno);
  pub_base_strpack (ttyno);
  strcpy(filename,txcode);
  strcat(filename,brno);
  strcat(filename,ttyno);
  pub_base_strpack (filename);
  
  /*������Ƭ*/
  strcpy(picflag,"1");
  sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,picflag);
  pub_base_strpack ( tmpfile );
  memcpy(cif_photo_inf.photo,tmpfile,strlen(tmpfile));
  sprintf(acErrMsg,"��Ƭ·��[%s]",cif_photo_inf.photo);
  WRITEMSG
  memcpy(cif_photo_inf.photo_type,picflag,strlen(picflag));
  fp=fopen(tmpfile,"r");
  if(fp !=NULL)
  {
  	  fclose(fp);
      /*��ѯ�ͻ�ͼƬ��Ϣ�Ƿ����*/
      ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"��ѯ�ͻ�ͼƬ��Ϣ����![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {
      	cif_photo_inf.photo_seqn=1;
        sprintf(acErrMsg,"��ʼ������Ƭ![%d]",ret);
	      WRITEMSG
	      /* �Ǽǿͻ�ͼƬ��Ϣ�� */
	      ret=Cif_photo_inf_Ins(cif_photo_inf);
	      if(ret)
	      {
	        sprintf(acErrMsg,"�Ǽǿͻ���Ƭ��Ϣ�����![%d]",ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C021");
	        ERR_DEAL;
	      }
      }
      else 
      {
	      /*���¿ͻ�ͼƬ��Ϣ�� */
        ret=Cif_photo_inf_Upd(cif_photo_inf,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"���� Cif_photo_infʧ��![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  else
  {
  	  sprintf(acErrMsg,"ɾ����Ƭ��Ϣ��");
  	  WRITEMSG
  	  
      /*��ѯ�ͻ�ͼƬ��Ϣ�Ƿ����*/
      ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"��ѯ��Ƭ��Ϣ����![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {

      }
      else 
      {
	      /*ɾ���ͻ�ͼƬ��Ϣ�� */
        ret=Cif_photo_inf_Del_Upd(cif_photo_inf,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"ɾ�� Cif_photo_infʧ��![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  /*����ӡ��*/
  strcpy(picflag,"2");
  sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,picflag);
  pub_base_strpack ( tmpfile );
  memcpy(cif_cop_photo.photo,tmpfile,strlen(tmpfile));
  sprintf(acErrMsg,"ͼƬ·��[%s]",cif_cop_photo.photo);
  WRITEMSG
  memcpy(cif_cop_photo.photo_type,picflag,strlen(picflag));
  fp=fopen(tmpfile,"r");
  if(fp !=NULL)
  {
  	  fclose(fp);
  	  	
      /*��ѯ�ͻ�ͼƬ��Ϣ�Ƿ����*/
      ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"��ѯ�ͻ�ӡ����Ϣ����![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {
      	cif_cop_photo.photo_seqn=1;
        sprintf(acErrMsg,"��ʼ����ӡ��![%d]",ret);
	      WRITEMSG
	      /* �Ǽǿͻ�ͼƬ��Ϣ�� */
	      ret=Cif_cop_photo_Ins(cif_cop_photo);
	      if(ret)
	      {
	        sprintf(acErrMsg,"�Ǽǿͻ�ӡ����Ϣ�����![%d]",ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C021");
	        ERR_DEAL;
	      }
      }
      else 
      {
	      /*���¿ͻ�ͼƬ��Ϣ�� */
        ret = Cif_cop_photo_Upd(cif_cop_photo,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"���� Cif_cop_photoʧ��![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  else
  {
    sprintf(acErrMsg,"ɾ��ӡ����Ϣ��");
  	  WRITEMSG	
      /*��ѯ�ͻ�ͼƬ��Ϣ�Ƿ����*/
      ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"��ѯ�ͻ�ӡ����Ϣ����![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {
 
      }
      else 
      {
	      /*ɾ���ͻ�ӡ����Ϣ�� */
        ret = Cif_cop_photo_Del_Upd(cif_cop_photo,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"ɾ�� Cif_cop_photoʧ��![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ�ͼƬ��Ϣ¼��ɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
	sprintf(acErrMsg,"�ͻ�ͼƬ��Ϣ¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
	vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
