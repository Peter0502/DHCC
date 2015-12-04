/**************************************************************
* 文 件 名:  sp1195.c
* 功能描述： 交易凭证处理
*
* 作    者:  liuyong
* 完成日期： 2009年12月23日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
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
	int ttlnum=0;			/**读取的总条数**/
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
    
        sprintf(acErrMsg,"交易流水号:[%ld]",trace_no);
        WRITEMSG
    
        sprintf(acErrMsg,"交易日期:[%ld]",txdate);
	WRITEMSG
        	 
        get_zd_data(DC_TX_BR_NO,brno);
        strcpy(tmpfile,brno);
        get_zd_data(DC_TTY,ttyno); 
        strcat(tmpfile,ttyno);
        pub_base_strpack (tmpfile);
 
        sprintf(filename,"%s/%s",getenv("FILDIR"),tmpfile);

        sprintf(acErrMsg,"文件名[%s]",filename);
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
        /*将trace_note_inf表中的对应信息删除*/
        ret = sql_count("trace_note_inf","trace_no=%ld and tx_date=%ld",trace_no,txdate);
        if(ret > 0)
        {
            ret = sql_execute("delete from trace_no_inf where trace_no=%ld and tx_date=%ld",trace_no,txdate);
            if(ret)
            {
                sprintf(acErrMsg,"删除trace_note_inf表记录出错!");
                WRITEMSG
                goto ErrExit; 
            } 
        }
        sprintf(acErrMsg,"添加数据库记录");
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
		   
	    /*检查流水号是否相符*/
            sprintf(tmpTraceNo,"%ld",trace_no);
            pub_base_strpack(tmpTraceNo);
	    /*if(strncmp(tmpTraceNo,fldstr[3],4) !=0)
	    {
		sprintf(acErrMsg,"交易流水号不符!");
		WRITEMSG
                goto ErrExit;
	    }*/
            /*检查交易日期是否相符*/
            sprintf(tmpTxDate,"%ld",txdate);
            pub_base_strpack(tmpTxDate);
	    if(strncmp(tmpTxDate,fldstr[1],8) !=0)
	    {
		sprintf(acErrMsg,"交易日期不符!");
		WRITEMSG
		goto ErrExit;	
	    }
            /*查找本条记录在数据库中是否存在*/
	    ret = sql_count("trace_note_inf","trace_no=%ld and note_no=%d and tx_date=%ld",trace_no,atoi(fldstr[0]),txdate);
            if(ret < 0)
            {
                sprintf(acErrMsg,"查询trace_note_inf表出错");
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
                /*插入一条新记录*/
                ret = Trace_note_inf_Ins(trace_note_inf);
                if(ret)
                {
                     sprintf(acErrMsg,"登记交易凭证信息表错误![%d]",ret);
	             WRITEMSG
	             strcpy(g_pub_tx.reply,"C021");
	             goto ErrExit;
                } 
            }
	  }
	  
	  fclose(fp);
	  
		  
OkExit:
	  strcpy( g_pub_tx.reply, "0000" );
	  sprintf(acErrMsg,"交易凭证处理成功: reply is[%s]",g_pub_tx.reply);
	  WRITEMSG
	  set_zd_data(DC_REPLY,g_pub_tx.reply);
	  return 0;
ErrExit:
	  sprintf(acErrMsg,"交易凭证处理失败: reply is[%s]",g_pub_tx.reply);
	  WRITEMSG
	  set_zd_data(DC_REPLY,g_pub_tx.reply);
	  return 1;
}
