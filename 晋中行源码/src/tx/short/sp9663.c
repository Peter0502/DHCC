/*************************************************
* ��  ����: sp9663.c
* ��������: ����˺Ż򿨺��Ƿ����
* ��    ��: liuyong
* �������: 2009��10��11��
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
#include "mdm_ac_rel_c.h"

static struct mdm_ac_rel_c mdm_ac_rel;

sp9663()
{
        int ret = 0;

        /** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
        memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

        /** ȡֵ����ֵ **/
        get_zd_data("0300",mdm_ac_rel.ac_no);	/* �˺Ż򿨺� */	
        /** �������� **/
        sprintf(acErrMsg,"�˺ţ�[%s]",mdm_ac_rel.ac_no);
        WRITEMSG

        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,
              "ac_no='%s'",mdm_ac_rel.ac_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"�˺Ż򿨺Ų�����![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"M002");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"��ȡ�������˻���ϵ�����[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W011");
            ERR_DEAL;
        }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����˿��ųɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����˿���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}


