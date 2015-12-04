/**************************************************************
* 文 件 名:  sp1004.c
* 功能描述： 根据流水号和凭证编号查找凭证信息
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
	  
	get_zd_long("0880", &trace_no); /*流水号*/
        get_zd_long("0480",&note_no);   /*凭证编号*/
        get_zd_long("0440",&txdate);    /*交易日期*/
        sprintf(acErrMsg,"交易流水号:[%ld]",trace_no);
        WRITEMSG
        sprintf(acErrMsg,"凭证编号:[%ld]",note_no);
        WRITEMSG
        sprintf(acErrMsg,"交易日期:[%ld]",txdate);
        WRITEMSG

        ret=Trace_note_inf_Sel(g_pub_tx.reply,&trace_note_inf,"trace_no=%ld and note_no=%d and tx_date=%ld",trace_no,note_no,txdate);
        if(ret == 100)
        {
            sprintf(acErrMsg,"没有相关记录![%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"D104");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读数据库表trace_note_inf错误！[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"AT06");
            ERR_DEAL;*/
        }
        
        pub_base_AllDwnFilName(tmpfile);
        
        /*查找对应的图片*/
        if(strlen(trace_note_inf.note_picname) != 0)
        {
            sprintf(filename,"%s/%s/%s",getenv("HOME"),"notedir",trace_note_inf.note_picname);
            pub_base_strpack(filename);
            
            /*读取图片的内容*/
            fp = fopen(filename,"r");
            if(fp == NULL)
            {
                sprintf(acErrMsg,"文件%s打开错误！",filename);
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
	              
	        /*写入图片信息*/
	        fp = fopen(tmpfile,"w");
	        if(fp == NULL)
	        {
	            sprintf(acErrMsg,"文件%s打开错误！",tmpfile);	
	            WRITEMSG
	        }
	        else
	        {
	            fwrite(buf,filelen,1,fp);
                    (void)fseek(fp,0L,SEEK_END);
                    filelen = 0;
                    filelen = (unsigned int)ftell(fp);
                    sprintf(acErrMsg,"写入文件的信息长度：[%ld]",filelen);
                    WRITEMSG
	            fclose(fp);
                    free(buf);	
                    
                    set_zd_data( DC_FILE_SND,"1" );	
	        } 
            }
        }
        
        /*返回与凭证相关的信息*/
        set_zd_data("0250",trace_note_inf.note_name); /*凭证名称*/
        set_zd_data("0260",trace_note_inf.note_picname); /*凭证图片名称*/
        
		  
OkExit:
       strcpy( g_pub_tx.reply, "0000" );
       sprintf(acErrMsg,"获取凭证信息成功: reply is[%s]",g_pub_tx.reply);
       WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"获取凭证信息失败: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
