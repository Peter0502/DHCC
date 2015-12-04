/**************************************************************
* 文 件 名:  sp1003.c
* 功能描述： 根据流水号查找交易凭证信息
*
* 作    者:  liuyong
* 完成日期： 2009年12月22日
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

int sp1003()
{
	struct trace_note_inf_c trace_note_inf;	
	int ret = 0;
  	long trace_no=0;
        long txdate = 0;
	int ttlnum=0;			/**读取的总条数**/
	FILE *fp;
	char filename[100];
		
        memset( &trace_note_inf,0x0,sizeof(struct trace_note_inf_c));
	  
	pub_base_sysinit();
	  
	get_zd_long("0880", &trace_no);
        get_zd_long("0440",&txdate);
        sprintf(acErrMsg,"交易流水号:[%ld]",trace_no);
        WRITEMSG
        sprintf(acErrMsg,"交易日期:[%ld]",txdate);
        WRITEMSG

	ret=Trace_note_inf_Dec_Sel(g_pub_tx.reply,"trace_no=%ld and tx_date=%ld order by to_number(note_no)",trace_no,txdate);
	ERR_DEAL
        ttlnum=0;
        while(1)
        {
	    memset( &trace_note_inf,0x0,sizeof(struct trace_note_inf_c));
	    ret=Trace_note_inf_Fet_Sel(&trace_note_inf, g_pub_tx.reply );
	    if( ret==100 ) break;
	    ERR_DEAL
	    if( !ttlnum )
	    {
	        pub_base_AllDwnFilName( filename );
	        fp = fopen( filename,"w" );
	        if( fp==NULL )
		{
		    sprintf(acErrMsg," open file [%s] error ",filename );
		    WRITEMSG
		    strcpy( g_pub_tx.reply,"S047" );
		    goto ErrExit;
		}
		
	    }
	    fprintf( fp, "%ld|%ld|%d|%s|%s|\n",trace_note_inf.trace_no,trace_note_inf.tx_date,trace_note_inf.note_no,trace_note_inf.note_name,trace_note_inf.note_picname);
	    ttlnum++;
	}
	ret=Trace_note_inf_Clo_Sel();
	ERR_DEAL
        if(ttlnum ==0)
        {
            pub_base_AllDwnFilName(filename);
            fp = fopen(filename,"w");
            if(fp==NULL)
            {
                strcpy(g_pub_tx.reply,"S047");
                goto ErrExit;
            }
        }
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );
		  
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
