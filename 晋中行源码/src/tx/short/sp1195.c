/**************************************************************
* �� �� ��:  sp1195.c
* ���������� ����ƾ֤����
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
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "trace_note_inf_c.h"

int sp1195()
{
	struct trace_note_inf_c trace_note_inf;	
	int ret = 0;
  	long trace_no=0;
  	long txdate =0;
	int ttlnum=0;			/**��ȡ��������**/
	FILE *fp;
	char filename[100];
	char fldstr[5][50];
        char tmpstr[1024];
        char tmpfile[100];
	char tmpTraceNo[10];
        char tmpTxDate[10];
        char brno[10];
        char ttyno[5];	
	memset( &trace_note_inf,0x0,sizeof(struct trace_note_inf_c));
	memset(filename,0x0,sizeof(filename));  
	memset(ttyno,'\0',sizeof(ttyno));
        memset(brno,'\0',sizeof(brno));
        memset(tmpfile,'\0',sizeof(tmpfile));
        pub_base_sysinit();
	  
	get_zd_long( "0880", &trace_no);
        get_zd_long("0440",&txdate);
    
        sprintf(acErrMsg,"������ˮ��:[%ld]",trace_no);
        WRITEMSG
    
        sprintf(acErrMsg,"��������:[%ld]",txdate);
	WRITEMSG
        	 
        get_zd_data(DC_TX_BR_NO,brno);
        strcpy(tmpfile,brno);
        get_zd_data(DC_TTY,ttyno); 
        strcat(tmpfile,ttyno);
        pub_base_strpack (tmpfile);
 
        sprintf(filename,"%s/%s",getenv("FILDIR"),tmpfile);

        sprintf(acErrMsg,"�ļ���[%s]",filename);
        WRITEMSG 
	/*pub_base_AllDwnFilName( filename );*/
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
	    sprintf( acErrMsg," open file [%s] error ", filename );
	    WRITEMSG 
            strcpy( g_pub_tx.reply, "S047" );
	    goto ErrExit;
	}
        /*��trace_note_inf���еĶ�Ӧ��Ϣɾ��*/
        ret = sql_count("trace_note_inf","trace_no=%ld and tx_date=%ld",trace_no,txdate);
        if(ret > 0)
        {
            ret = sql_execute("delete from trace_no_inf where trace_no=%ld and tx_date=%ld",trace_no,txdate);
            if(ret)
            {
                sprintf(acErrMsg,"ɾ��trace_note_inf���¼����!");
                WRITEMSG
                goto ErrExit; 
            } 
        }
        sprintf(acErrMsg,"������ݿ��¼");
        WRITEMSG 
	while (1)
	{
	    memset( tmpstr, 0x0, sizeof(tmpstr) );
	    fgets( tmpstr,1024,fp );
	    if( feof(fp) ) 
	    {
	        break;
	    }
	    memset(fldstr,0x0,sizeof(fldstr));
	    for(int i=0;i<4;i++ )
	    {
	        ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
	        vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
		pub_base_strpack( fldstr[i] );
	    }
		   
	    /*�����ˮ���Ƿ����*/
            sprintf(tmpTraceNo,"%ld",trace_no);
            pub_base_strpack(tmpTraceNo);
	    /*if(strncmp(tmpTraceNo,fldstr[3],4) !=0)
	    {
		sprintf(acErrMsg,"������ˮ�Ų���!");
		WRITEMSG
                goto ErrExit;
	    }*/
            /*��齻�������Ƿ����*/
            sprintf(tmpTxDate,"%ld",txdate);
            pub_base_strpack(tmpTxDate);
	    if(strncmp(tmpTxDate,fldstr[1],8) !=0)
	    {
		sprintf(acErrMsg,"�������ڲ���!");
		WRITEMSG
		goto ErrExit;	
	    }
            /*���ұ�����¼�����ݿ����Ƿ����*/
	    ret = sql_count("trace_note_inf","trace_no=%ld and note_no=%d and tx_date=%ld",trace_no,atoi(fldstr[0]),txdate);
            if(ret < 0)
            {
                sprintf(acErrMsg,"��ѯtrace_note_inf�����");
                WRITEMSG
                goto ErrExit; 
            }
            else if(ret == 0)
            {
                trace_note_inf.trace_no = trace_no;
                trace_note_inf.tx_date = txdate;
                trace_note_inf.note_no = atoi(fldstr[0]);
                strcpy(trace_note_inf.note_name,fldstr[2]);
                strcpy(trace_note_inf.note_picname,fldstr[3]);
                strcpy(trace_note_inf.tel,g_pub_tx.tel); 
                /*����һ���¼�¼*/
                ret = Trace_note_inf_Ins(trace_note_inf);
                if(ret)
                {
                     sprintf(acErrMsg,"�Ǽǽ���ƾ֤��Ϣ�����![%d]",ret);
	             WRITEMSG
	             strcpy(g_pub_tx.reply,"C021");
	             goto ErrExit;
                } 
            }
	  }
	  
	  fclose(fp);
	  
		  
OkExit:
	  strcpy( g_pub_tx.reply, "0000" );
	  sprintf(acErrMsg,"����ƾ֤����ɹ�: reply is[%s]",g_pub_tx.reply);
	  WRITEMSG
	  set_zd_data(DC_REPLY,g_pub_tx.reply);
	  return 0;
ErrExit:
	  sprintf(acErrMsg,"����ƾ֤����ʧ��: reply is[%s]",g_pub_tx.reply);
	  WRITEMSG
	  set_zd_data(DC_REPLY,g_pub_tx.reply);
	  return 1;
}
