/*************************************************
* ��  ����:  sp9660.c
* ��������: ƾ֤ͼƬ��Ϣת��
* ��    ��: liuyong
* �������: 2010��3��1��
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

int sp9660()
{ 	
    int ret = 0;
    char filename[100];
    char tmpfile[100];
    char oldfile[100];
    char tmpstr[100];
    char brno[10];
    char ttyno[5];
    char filedir[100];
    char filedir1[100];
    long filelen;
    long trace_no =0;
    char note_no[100];
    long txdate =0;
    FILE *fp;
    unsigned char *buffer;
 
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    /** ���ݳ�ʼ�� **/
    memset(tmpfile,0x00,sizeof(tmpfile));
    memset(oldfile,0x00,sizeof(oldfile));
    memset(tmpstr,0x00,sizeof(tmpstr));
    memset(brno,0x00,sizeof(brno));
    memset(ttyno,0x00,sizeof(ttyno));
    memset(filedir,0x00,sizeof(filedir));
    memset(note_no,0x00,sizeof(note_no));

    /** ȡֵ����ֵ **/
    get_zd_long("0880", &trace_no);
    get_zd_data("0810",note_no);    /*�����ļ���*/
    get_zd_long("0440",&txdate);
        
    get_zd_data(DC_TX_BR_NO,brno);
    get_zd_data(DC_TTY,ttyno); 
    
    sprintf(acErrMsg,"������ˮ��:[%ld]",trace_no);
    WRITEMSG
    sprintf(acErrMsg,"�����ļ���:[%s]",note_no);
    WRITEMSG
    
    sprintf(acErrMsg,"����:[%ld]",txdate);
    WRITEMSG
    
    sprintf(filedir,"%s/%s/%ld",getenv("HOME"),"auth",txdate);
    pub_base_strpack(filedir);
    
    sprintf(acErrMsg,"����Ŀ¼:[%s]",filedir);
    WRITEMSG
 
    /*����Ŀ¼�����ڣ��򴴽�����Ŀ¼*/
    if(access(filedir,0) == -1)
    {
        if(mkdir(filedir,0766) != 0)
        {
            sprintf(acErrMsg,"��������Ŀ¼����");
            WRITEMSG
            goto ErrExit;
        }
    }
    
    memset(filedir1,0x00,sizeof(filedir1));
    sprintf(filedir1,"%s/%s/%ld/%ld",getenv("HOME"),"auth",txdate,trace_no); 
    pub_base_strpack(filedir1);

    sprintf(acErrMsg,"��ˮ��Ŀ¼:[%s]",filedir1);
    WRITEMSG

    if(access(filedir1,0) == -1)
    {
        if(mkdir(filedir1,0766) != 0)
        {
            sprintf(acErrMsg,"������ˮ���ļ�Ŀ¼����");
            WRITEMSG
            goto ErrExit;
        }
    }

    pub_base_strpack(note_no);
    if(strlen(note_no)==0 )
    {
        sprintf(acErrMsg,"�ļ�������Ϊ��");
        WRITEMSG
        goto ErrExit; 
    } 
    else
    {
        sprintf(tmpfile,"%s/%s/%ld/%ld/%s",getenv("HOME"),"auth",txdate,trace_no,note_no);
    }
   
    sprintf(acErrMsg,"�ļ�����·��:[%s]",tmpfile);
    WRITEMSG

    /* �����´�ȫ·���ļ���(����) */
    memset(filename,0x00,sizeof(filename));
    strcpy(filename,brno);
    strcat(filename,ttyno);
    pub_base_strpack(filename);
    
    sprintf(oldfile,"%s/%s",getenv("FILDIR"),filename);
	
    sprintf(acErrMsg,"�ϴ�����·����[%s]",oldfile);
    WRITEMSG
	  
    fp = fopen(oldfile,"r");
    if(fp == NULL)
    {
       sprintf(acErrMsg,"open file [%s] failed!",oldfile);
       WRITEMSG
       /*strcpy(g_pub_tx.reply,"L206");
       ERR_DEAL;	*/
    }
    else
    {
    	(void)fseek(fp,0L,SEEK_END);
      filelen = ftell(fp);
      buffer = (unsigned char *)malloc(sizeof(unsigned char) * filelen);
      (void)fseek(fp,0L,SEEK_SET);
      (void)fread(buffer,filelen,1,fp);
      fclose(fp);
      sprintf(acErrMsg,"�ļ����ȣ�[%ld]",filelen);
      WRITEMSG
      
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
    
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ļ�ת��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    sprintf(acErrMsg,"�ļ�ת��ʧ�ܣ�[%s]",g_pub_tx.reply);
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
