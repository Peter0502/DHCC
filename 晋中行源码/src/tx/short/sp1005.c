/*************************************************
* ��  ����:  sp1005.c
* ��������: ƾ֤ͼƬ��Ϣת��
* ��    ��: liuyong
* �������: 2009��12��23��
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

int sp1005()
{ 	
    int ret = 0;
    char filename[100];
    char tmpfile[100];
    char oldfile[100];
    char tmpstr[100];
    char brno[10];
    char ttyno[5];
    long filelen;
    long trace_no =0;
    long note_no =0;
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

    /** ȡֵ����ֵ **/
    get_zd_long("0880", &trace_no);
    get_zd_long("0480",&note_no);
    get_zd_long("0440",&txdate);
        
    get_zd_data(DC_TX_BR_NO,brno);
    get_zd_data(DC_TTY,ttyno); 
    
    sprintf(acErrMsg,"������ˮ��:[%ld]",trace_no);
    WRITEMSG
    sprintf(acErrMsg,"ƾ֤���:[%ld]",note_no);
    WRITEMSG
 
    if(note_no <10)
    {
        sprintf(tmpstr,"%ld%s%d%ld",trace_no,"0",note_no,txdate);
    } 
    else
    {
        sprintf(tmpstr,"%ld%d%ld",trace_no,note_no,txdate);
    }
    pub_base_strpack(tmpstr);
    sprintf(tmpfile,"%s/%s/%s",getenv("HOME"),"notedir",tmpstr);
    pub_base_strpack ( tmpfile );
	
    sprintf(acErrMsg,"��ͼƬ·��[%s]",tmpfile);
    WRITEMSG
  
    /* �����´�ȫ·���ļ���(����) */
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
    sprintf(acErrMsg,"ƾ֤ͼƬת��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    sprintf(acErrMsg,"ƾ֤ͼƬת��ʧ�ܣ�[%s]",g_pub_tx.reply);
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
