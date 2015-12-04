/*************************************************
* ��  ����:  sp9667.c
* ��������: �ͻ�ͼƬ��Ϣת��
* ��    ��: liuyong
* �������: 2009��08��25��
* �޸ļ�¼:   
*     1.��    ��:
*       �� �� ��:
*       �޸�����:
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#define EXTERN
#include "public.h"
#include "pub_tx.h"
#include "sys/types.h"
#include "sys/stat.h"
#include"fcntl.h"
#include "cif_id_code_rel_c.h"

static struct cif_id_code_rel_c cif_id_code_rel;

sp9667()
{ 	
    int ret = 0;
    char flag[2];
    char fileflag[2];
    char filename[100];
    char brno[10];
    char ttyno[5];
    char tmpfile[100];
    char oldfile[100];
    char idno[21];
    long filelen;
    FILE *fp;
    unsigned char *buffer;
    struct stat stat;
 
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    /** ���ݳ�ʼ�� **/
    memset(&cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
    memset(filename,0x00,sizeof(filename));
    memset(tmpfile,0x00,sizeof(tmpfile));
    memset(oldfile,0x00,sizeof(oldfile));
    memset(brno,0x00,sizeof(brno));
    memset(ttyno,0x00,sizeof(ttyno));

    /** ȡֵ����ֵ **/
    get_zd_long("0280",&cif_id_code_rel.cif_no);
    get_zd_data("0720",flag);                     /*����ͼƬ��ʶ*/
    get_zd_data("0620",idno);
    get_zd_data("0700",fileflag);
    get_zd_data(DC_TX_BR_NO,brno);
    get_zd_data(DC_TTY,ttyno); 
  
    if(strlen(idno) ==0)
    {
        /*ȡ�ͻ�֤������*/ 
        ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
              "cif_no=%ld",cif_id_code_rel.cif_no); 
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
        memcpy(idno,cif_id_code_rel.id_no,strlen(cif_id_code_rel.id_no));
    }
    sprintf(acErrMsg,"��ʶ[%s]",flag);
    WRITEMSG
    sprintf(acErrMsg,"�ͻ�֤�����룺[%s]",idno);
    WRITEMSG
    
    if(strlen(idno)==0)
    {
        sprintf(acErrMsg,"�ͻ�֤���������");
        WRITEMSG
        strcpy(g_pub_tx.reply,"C045");
        ERR_DEAL;
    } 
    sprintf(acErrMsg,"�����´��ļ���");
    WRITEMSG
  
    pub_base_strpack(idno);
    pub_base_strpack (brno);
    pub_base_strpack (ttyno);
    strcpy(filename,idno);
    strcat(filename,brno);
    strcat(filename,ttyno);
    pub_base_strpack (filename);
  
    sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,flag);
  
    pub_base_strpack ( tmpfile );
	
    sprintf(acErrMsg,"��ͼƬ·��[%s]",tmpfile);
    WRITEMSG
  
    /* �����´�ȫ·���ļ���(����) 
    pub_base_AllDwnFilName(oldfile);*/
    memset(filename,0x00,sizeof(filename));
    strcpy(filename,brno);
    strcat(filename,ttyno);
    pub_base_strpack(filename);
    
    sprintf(oldfile,"%s/%s",getenv("FILDIR"),filename);
	
    sprintf(acErrMsg,"ԭ��ͼƬ·����[%s]",oldfile);
    WRITEMSG
	  
    fp = fopen(oldfile,"r");
    if(fp == NULL)
    {
       sprintf(acErrMsg,"open file [%s] failed!",oldfile);
       WRITEMSG
       strcpy(g_pub_tx.reply,"L206");
       ERR_DEAL;	
    }
    (void)fseek(fp,0L,SEEK_END);
    filelen = ftell(fp);
    buffer = (unsigned char *)malloc(sizeof(unsigned char) * filelen);
    (void)fseek(fp,0L,SEEK_SET);
    (void)fread(buffer,filelen,1,fp);
    fclose(fp);
    
    /*
    int fd;
    
    fd = open(oldfile,O_RDONLY);
    if(fd < 0)
    {
    	sprintf(acErrMsg,"open file [%s] failed!",oldfile);
      WRITEMSG
      strcpy(g_pub_tx.reply,"L206");
      ERR_DEAL;
    }
    filelen = lseek(fd,0,SEEK_END);
    buffer = (unsigned char *)malloc(sizeof(unsigned char) * filelen);
    lseek(fd,0,SEEK_SET);
		read(fd,buffer,filelen);
		*/
    sprintf(acErrMsg,"�ļ����ȣ�[%ld]",filelen);
    WRITEMSG
	  if(fileflag[0] =='1')
	  {
	  	fp = fopen(tmpfile,"wb");
      if(fp == NULL)
      {
        sprintf(acErrMsg,"create file [%s] failed!",tmpfile);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P157");
        ERR_DEAL;	
      }
	
      fwrite(buffer,filelen,1,fp);
      fclose(fp);
      free(buffer);
	  }
	  else if(fileflag[0] =='0')
	  {
	  	fp = fopen(tmpfile,"r");
	  	if(fp!=NULL)
	  	{
	  		fclose(fp);
	  		if(remove(tmpfile) ==0)
        {
          sprintf(acErrMsg,"ɾ���ļ�[%s]�ɹ�!",tmpfile);
          WRITEMSG
        }
        else
        {
          sprintf(acErrMsg,"ɾ���ļ�[%s]ʧ�ܣ�",tmpfile);
          WRITEMSG
        }
	  	}
	  }
    
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ͼƬת��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    sprintf(acErrMsg,"ͼƬת��ʧ�ܣ�[%s]",g_pub_tx.reply);
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
