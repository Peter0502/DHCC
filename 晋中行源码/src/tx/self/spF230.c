/*********************************************************************
* �� �� �� :  V1.0        
* �������� :  XUEKE          
* ��    �� :  2011-07-14   
* ����ģ�� :  ��ƽ̨�Ľӿڳ���            
* �������� :  spF230.c
* �������� :  ���Ժ���ϵͳ�Ƿ��ܹ���������ƽ̨����
* �����б� :                                   
* ʹ��ע�� :                                       
* �޸ļ�¼ :                                       
*     �޸����� :                           
*     �޸�ʱ�� :                                    
*     �޸����� :
insert into tx_def  values ('6230', '��ͨ�Բ��Խ���', '10000000000000010000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000100100', '1', '6', '0', '3', null);
insert into tx_flow_def  values ('6230', 0, 'F230', '#$');
insert into tx_sub_def  values ('F230','��ͨ�Բ��Խ���','spF230','0','0');
******************************************************************/

#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"

spF230()
{
    int iRet=0;
    char cHx_tx_code[5];    /*���״���*/
    char cHx_brf[50];       /*ժҪ*/
    
    /** ��ʼ�������� **/
    memset(cHx_tx_code,0,sizeof(cHx_tx_code));
    memset(cHx_brf,0,sizeof(cHx_brf));
    
    /*��ʼ���ṹ*/
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    
    /*** ȡֵ **/
    get_zd_data("0160",cHx_tx_code);
    get_zd_data("0810",cHx_brf);
    
    /*�ж�ƽ̨���ͱ��ĵ��Ƿ�ɹ�*/
    if(strcmp(cHx_tx_code,"6230") != 0 )
    {
        sprintf(acErrMsg, "���ķ���ʧ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "P064");
        goto ErrExit;
    }
    
    
    
OkExit:
       strcpy(g_pub_tx.reply, "0000");
       sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 0;
ErrExit:
       sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 1;
}
