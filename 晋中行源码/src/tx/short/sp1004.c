/**************************************************************
* �� �� ��:  sp1004.c
* ���������� ������ˮ�ź�ƾ֤��Ų���ƾ֤��Ϣ
*
* ��    ��:  liuyong
* ������ڣ� 2009��12��23��
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
#include "trace_note_inf_c.h"

int sp1004()
{
	struct trace_note_inf_c trace_note_inf;	
	int ret = 0;
  	long trace_no=0;
	long note_no = 0;
        long txdate=0;
        long filelen = 0;
        FILE *fp;
	char filename[100];
	char tmpfile[100];
	unsigned char *buf;
		
        memset( &trace_note_inf,0x0,sizeof(struct trace_note_inf_c));
	memset(filename,0x0,sizeof(filename));
	memset(tmpfile,0x0,sizeof(tmpfile));
	      
	pub_base_sysinit();
	  
	get_zd_long("0880", &trace_no); /*��ˮ��*/
        get_zd_long("0480",&note_no);   /*ƾ֤���*/
        get_zd_long("0440",&txdate);    /*��������*/
        sprintf(acErrMsg,"������ˮ��:[%ld]",trace_no);
        WRITEMSG
        sprintf(acErrMsg,"ƾ֤���:[%ld]",note_no);
        WRITEMSG
        sprintf(acErrMsg,"��������:[%ld]",txdate);
        WRITEMSG

        ret=Trace_note_inf_Sel(g_pub_tx.reply,&trace_note_inf,"trace_no=%ld and note_no=%d and tx_date=%ld",trace_no,note_no,txdate);
        if(ret == 100)
        {
            sprintf(acErrMsg,"û����ؼ�¼![%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"D104");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"�����ݿ��trace_note_inf����[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"AT06");
            ERR_DEAL;*/
        }
        
        pub_base_AllDwnFilName(tmpfile);
        
        /*���Ҷ�Ӧ��ͼƬ*/
        if(strlen(trace_note_inf.note_picname) != 0)
        {
            sprintf(filename,"%s/%s/%s",getenv("HOME"),"notedir",trace_note_inf.note_picname);
            pub_base_strpack(filename);
            
            /*��ȡͼƬ������*/
            fp = fopen(filename,"r");
            if(fp == NULL)
            {
                sprintf(acErrMsg,"�ļ�%s�򿪴���",filename);
                WRITEMSG	
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
        
        /*������ƾ֤��ص���Ϣ*/
        set_zd_data("0250",trace_note_inf.note_name); /*ƾ֤����*/
        set_zd_data("0260",trace_note_inf.note_picname); /*ƾ֤ͼƬ����*/
        
		  
OkExit:
       strcpy( g_pub_tx.reply, "0000" );
       sprintf(acErrMsg,"��ȡƾ֤��Ϣ�ɹ�: reply is[%s]",g_pub_tx.reply);
       WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ȡƾ֤��Ϣʧ��: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
