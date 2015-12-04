/***************************************************************
* �� �� ��:    spB592.c 
* ����������   ͬ�����뽻����Ϣά�� 
*
* ��    ��:    Amelia 
* ������ڣ�   2006-10-15
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ctl_accountreg_c.h"

int spB592()
{ 
    struct ctl_accountreg_c sCtl_accountreg_c, sCtl_accountreg_c1;
    char flag[2];
    int ret;

    memset(&sCtl_accountreg_c, 0x0,sizeof(struct ctl_accountreg_c));
    memset(&sCtl_accountreg_c1,0x0,sizeof(struct ctl_accountreg_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
   
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg," trce_no is [%ld]",g_pub_tx.trace_no);
    WRITEMSG
    vtcp_log("!!! seqno is [%s]\n",sCtl_accountreg_c.seqno );
    get_zd_data("0300",sCtl_accountreg_c.seqno);	/* ���׻���+�������� */
    get_zd_double("0400",&sCtl_accountreg_c.amt);       /*�����*/
    get_zd_data("0700",flag);
    vtcp_log("!!! seqno is [%s]\n",sCtl_accountreg_c.seqno );
	
/************************�����ǲ��ǻ�Ӧ�üӵ�ʲôѽ?********************/

switch( atoi(flag) )
    {
        case 0 :
            /* ¼�� */
            sprintf(acErrMsg,"¼��!!");
            WRITEMSG

            /* ������Ϣ�Ƿ��Ѿ����� */
            ret = Ctl_accountreg_Sel(g_pub_tx.reply,&sCtl_accountreg_c1,"seqno='%s'",sCtl_accountreg_c.seqno);
          if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"select ctl_accountreg error [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"����Ϣ�Ѿ�����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"T048");
                goto ErrExit;
            }

            ret = Ctl_accountreg_Ins(sCtl_accountreg_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��Ǽǲ��������ݴ���",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D107");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"ͬ�����뽻����Ϣά��");
            sprintf(acErrMsg,"��ͬ�����뽻����Ϣά���Ǽǲ�����pass");
            WRITEMSG

            break;
        case 2 :
            /* �޸� */
            ret = Ctl_accountreg_Dec_Upd(g_pub_tx.reply,"seqno = '%s'",sCtl_accountreg_c.seqno);
            vtcp_log("&&&&&&&&&&&&**************************[%s][%d][%ld]\n",__FILE__,__LINE__,ret);
            vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,sCtl_accountreg_c.seqno);
            vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,sCtl_accountreg_c.amt);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
  
            ret = Ctl_accountreg_Fet_Upd(&sCtl_accountreg_c1,g_pub_tx.reply);
            vtcp_log("+++++++++++++++++++++++++++++++++++++[%s][%d][%ld]\n",__FILE__,__LINE__,ret);
            vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,sCtl_accountreg_c.seqno);
            vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,sCtl_accountreg_c.amt);
            if (ret != 0)
            {
                sprintf(acErrMsg,"û�и�����Ϣ����Ϣ���!!ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "H031");
               /** Ctl_accountreg_Clo_Upd(); **/
                goto ErrExit;
            }
/*add*/     strcpy(sCtl_accountreg_c.rowid,sCtl_accountreg_c1.rowid);
            ret = Ctl_accountreg_Upd_Upd(sCtl_accountreg_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"���¼�¼�쳣���������Ļ�����ϵ!!ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D105");
                Ctl_accountreg_Clo_Upd();
                goto ErrExit;
            }
/*addover*/
            break;
        default :
            sprintf(acErrMsg,"�������ͳ���!! type=[%c] 0¼��1��ѯ2�޸�",flag);
            goto ErrExit;
            break;
    }	
   
    g_pub_tx.tx_amt1 = sCtl_accountreg_c.amt;

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
 
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
