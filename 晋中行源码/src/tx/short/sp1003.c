/**************************************************************
* �� �� ��:  sp1003.c
* ���������� ������ˮ�Ų��ҽ���ƾ֤��Ϣ
*
* ��    ��:  liuyong
* ������ڣ� 2009��12��22��
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

int sp1003()
{
	struct trace_note_inf_c trace_note_inf;	
	int ret = 0;
  	long trace_no=0;
        long txdate = 0;
	int ttlnum=0;			/**��ȡ��������**/
	FILE *fp;
	char filename[100];
		
        memset( &trace_note_inf,0x0,sizeof(struct trace_note_inf_c));
	  
	pub_base_sysinit();
	  
	get_zd_long("0880", &trace_no);
        get_zd_long("0440",&txdate);
        sprintf(acErrMsg,"������ˮ��:[%ld]",trace_no);
        WRITEMSG
        sprintf(acErrMsg,"��������:[%ld]",txdate);
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
