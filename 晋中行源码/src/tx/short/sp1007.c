/**************************************************************
* �� �� ��:  sp1007.c
* ���������� ������ˮ�ź����ڲ���Զ����Ȩ�ļ���Ϣ
*
* ��    ��:  liuyong
* ������ڣ� 2010��4��20��
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "auth_remote_inf_c.h"

int sp1007()
{
	struct auth_remote_inf_c auth_remote_inf;	
	int ret = 0;
  	long trace_no=0;
        long txdate=0;
        long filelen = 0;
        FILE *fp;
	char filename[100];
        char picname[51];
	char tmpfile[100];
	unsigned char *buf;
		
        memset( &auth_remote_inf,0x0,sizeof(struct auth_remote_inf_c));
	memset(filename,0x0,sizeof(filename));
	memset(tmpfile,0x0,sizeof(tmpfile));
	memset(picname,0x0,sizeof(picname));      
	
        pub_base_sysinit();
	  
	get_zd_long("0880", &trace_no); /*��ˮ��*/
        get_zd_long("0440",&txdate);    /*��������*/
        get_zd_data("0250",picname);

        pub_base_strpack(picname);

        sprintf(acErrMsg,"������ˮ��:[%ld]",trace_no);
        WRITEMSG
        sprintf(acErrMsg,"��������:[%ld]",txdate);
        WRITEMSG
        
        sprintf(acErrMsg,"Ҫ��ȡ��ͼƬ����[%s]",picname);
        WRITEMSG
        if(trace_no < 0 || txdate < 0)
        {
            goto ErrExit;
        }
        ret = Auth_remote_inf_Sel(g_pub_tx.reply,&auth_remote_inf,"trace_no=%ld and tx_date=%ld",trace_no,txdate);
        if(ret == 100)
        {
            sprintf(acErrMsg,"û����ؼ�¼![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D104");
            goto ErrExit;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"�����ݿ�����[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"AT06");
            goto ErrExit;
        }
        
        pub_base_AllDwnFilName(tmpfile);
        
        /*���Ҷ�Ӧ��ͼƬ*/
        if(strlen(picname) != 0)
        {
            sprintf(filename,"%s/%s/%ld/%ld/%s",getenv("HOME"),"auth",auth_remote_inf.tx_date,auth_remote_inf.trace_no,picname);
            pub_base_strpack(filename);
            
            /*��ȡͼƬ������*/
            fp = fopen(filename,"r");
            if(fp == NULL)
            {
                sprintf(acErrMsg,"�ļ�%s�򿪴���",filename);
                WRITEMSG
                goto ErrExit;	
            }
            else
            {
                (void)fseek(fp,0L,SEEK_END);
	        filelen = (unsigned int)ftell(fp);
	        buf = (unsigned char *)malloc(sizeof(unsigned char) * filelen);
	        (void)fseek(fp,0L,SEEK_SET);
	        (void)fread(buf,filelen,1,fp);
	        fclose(fp);
	              
	        /*д��ͼƬ��Ϣ*/
	        fp = fopen(tmpfile,"w");
	        if(fp == NULL)
	        {
	            sprintf(acErrMsg,"�ļ�%s�򿪴���",tmpfile);	
	            WRITEMSG
                    goto ErrExit;
	        }
	        else
	        {
	            fwrite(buf,filelen,1,fp);
                    (void)fseek(fp,0L,SEEK_END);
                    filelen = 0;
                    filelen = (unsigned int)ftell(fp);
                    sprintf(acErrMsg,"д���ļ�����Ϣ���ȣ�[%ld]",filelen);
                    WRITEMSG
	            fclose(fp);
                    free(buf);	
                    
                    set_zd_data( DC_FILE_SND,"1" );	
	        } 
            }
        }
        else
          goto ErrExit;
        
OkExit:
       strcpy( g_pub_tx.reply, "0000" );
       sprintf(acErrMsg,"��ȡ�ļ��ɹ�: reply is[%s]",g_pub_tx.reply);
       WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ȡ�ļ�ʧ��: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
