/*************************************************
* ��  ����:  sp1006.c
* ��������: ������ˮ��ѯ
* ��    ��: liuyong
* �������: 2009��12��29��
* �޸ļ�¼:   
*     1.��    ��:
*       �� �� ��:
*       �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include "stdio.h" 
#include "stdlib.h" 
#define EXTERN
#include "public.h"
#include "pub_tx.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "dc_log_c.h"
#include "dc_log_rz_c.h"
#include "dc_log_bk_c.h"
#include "trace_note_inf_c.h"
#include "trace_log_c.h"
#include "trace_log_bk_c.h"
#include "trace_log_rz_c.h"
#include "tx_def_c.h"

int sp1006()
{ 	
    int ret = 0;
    long traceno =0;
    long txdate =0;
    int ttlnum = 0;
    char txcode[5];
    char filename[100];
    FILE * fp;
    struct trace_log_c trace_log;
    struct tx_def_c tx_def;
 
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    memset(&tx_def ,0x0,sizeof(struct tx_def_c));

    /** ȡֵ����ֵ **/
    get_zd_long("0880", &traceno);
    get_zd_long("0440",&txdate);
        
      
    sprintf(acErrMsg,"������ˮ��:[%ld]",traceno);
    WRITEMSG
    sprintf(acErrMsg,"��������:[%ld]",txdate);
    WRITEMSG
    
    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"trace_no=%ld and tx_date=%ld order by to_number(trace_cnt)",traceno,txdate);
    ERR_DEAL
    ttlnum = 0;
    while(1)
    {
        memset(&trace_log,0x0,sizeof(struct trace_log_c));
        ret = Trace_log_Fet_Sel(&trace_log,g_pub_tx.reply);
        if(ret == 100)
            break;
        ERR_DEAL
        if(!ttlnum)
        {
            pub_base_AllDwnFilName(filename);
            fp = fopen(filename,"w");
            if(fp == NULL)
            {
                sprintf(acErrMsg,"open file [%s] error",filename);
                WRITEMSG
                strcpy(g_pub_tx.reply,"S047");
                goto ErrExit;
            }
        }
        fprintf(fp,"%s|%.2lf|%s|\n",trace_log.ac_no,trace_log.amt,trace_log.brf);
        /*������ˮ������Ϣ*/
        if(!ttlnum)
        {
           /*���״���*/
           sprintf(txcode,"%s",trace_log.tx_code);
           set_zd_data("0810",txcode);
           sprintf(acErrMsg,"���״���[%s]",txcode);
           WRITEMSG
            /*��������*/
           if(strlen(trace_log.brf) != 0)
           {
               set_zd_data("0250",trace_log.brf);
           }
           else
           {
               ret = Tx_def_Sel(g_pub_tx.reply,&tx_def,"tx_code='%s'",trace_log.tx_code);
               if(ret)
               {
                   sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʧ��");
	           WRITEMSG
               } 
               else
               {
                   set_zd_data("0250",tx_def.tx_name);
               }
           }
           /*���׻���*/
           set_zd_data("0290",trace_log.tx_br_no);
           sprintf(acErrMsg,"������[%s]",trace_log.tx_br_no);
           WRITEMSG  
        } 
        ttlnum++;
    }  

    ret = Trace_log_Clo_Sel();
    ERR_DEAL
    if(ttlnum == 0)
    {
        sprintf(acErrMsg,"û����صļ�¼");
        WRITEMSG
        pub_base_AllDwnFilName(filename);
        fp = fopen(filename,"w");
        if(fp==NULL)
        {
            strcpy(g_pub_tx.reply,"S047");
            goto ErrExit;
        }
    }
    fclose(fp);
    set_zd_data(DC_FILE_SND,"1");
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������ˮ��Ϣ��ѯ�ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    sprintf(acErrMsg,"������ˮ��Ϣ��ѯʧ�ܣ�[%s]",g_pub_tx.reply);
    vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
