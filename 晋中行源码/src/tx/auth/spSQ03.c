/*************************************************
* �� �� ����spSQ03.c
* ��������: ��¼Զ����Ȩ��Ϣ 
* ��    ��: liuyong
* �������: 2010��2��24��
* �޸ļ�¼:   
*     1.��    ��:
*       �� �� ��:
*       �޸�����:
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#define  EXTERN
#include "public.h"
#include "pub_tx.h"
#include "auth_remote_inf_c.h"

static struct auth_remote_inf_c auth_remote_inf;

spSQ03()
{
        int ret = 0;
        long trace_no = 0;
        char filename[200];
        char attachs[601];

        /** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&auth_remote_inf,0x00,sizeof(struct auth_remote_inf_c));
        memset(filename,0x00,sizeof(filename));
        memset(attachs,0x00,sizeof(attachs));
        
        sprintf(acErrMsg,"��ʼȡֵ:");
        WRITEMSG

        /** ȡֵ����ֵ **/
        get_zd_long("0530",&auth_remote_inf.trace_no);
        get_zd_data("0280",auth_remote_inf.br_no);
        get_zd_data("0340",auth_remote_inf.tel);
        get_zd_long("0440",&auth_remote_inf.tx_date);
        get_zd_data("0350",auth_remote_inf.tx_code);
        get_zd_data("0360",auth_remote_inf.auth_tel);
        get_zd_data("0760",filename);
        get_zd_data("0950",attachs);
        get_zd_data("0250",auth_remote_inf.title);
 
        sprintf(acErrMsg,"ȡֵ���:[%ld]--[%s]--[%s]--[%ld]--[%s]--[%s]--[%s]",auth_remote_inf.trace_no,auth_remote_inf.br_no,auth_remote_inf.tel,auth_remote_inf.tx_date,auth_remote_inf.tx_code,auth_remote_inf.auth_tel,auth_remote_inf.title);
        WRITEMSG

        pub_base_strpack(filename);
        strcpy(auth_remote_inf.filename,filename);
        pub_base_strpack(attachs);
        strcpy(auth_remote_inf.attachments,attachs);

        sprintf(acErrMsg,"�����ļ�[%s],����[%s]",filename,attachs);
        WRITEMSG
        if(auth_remote_inf.trace_no<0)
        {
            sprintf(acErrMsg,"������ˮ�Ŵ���");
            WRITEMSG
            goto ErrExit;
        }
 
	/** �������� **/
        ret = Auth_remote_inf_Ins(auth_remote_inf,g_pub_tx.reply);
        if(ret)
        {
            sprintf(acErrMsg,"�Ǽ�Զ����Ϣʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D107");
            goto ErrExit;
        }
        
        sprintf(acErrMsg,"�������");
        WRITEMSG 	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�Ǽ�Զ����Ȩ��Ϣ�ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�Ǽ�Զ����Ȩ��Ϣʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}



