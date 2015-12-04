/*************************************************
* �� �� ��:  spF047.c
* ���������� POS ����ѯ(��ѯ���е����)
*
* ��    ��:  rob
* ������ڣ� 2003��2��22��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"

int spF047()
{

    int ret;
    char flag;
    long old_date;
    char tty[9];
    char note_no[21];
    char in_type[5];
    double ky_amt=0.0;

    struct chnl_marchinfo_c chnl_marchinfo;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no);   /* ���� */
    get_zd_data("0800",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_data("0380", tty);              /* �ն˺� */
    get_zd_data("0620", note_no);          /* �̻��� */
    get_zd_data("0910", in_type);          /* �̻�����ʽ */

    sprintf(acErrMsg, "����Ϊ: [%s]", g_pub_tx.crd_no);
	WRITEMSG
    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }

	if ( pub_mobile_pay(in_type, tty, note_no) )
	{
		/* �����ƶ�֧�� */
    	/* ��鿨��֧ȡ��ʽ */
    	if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    	{
        	sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT02");
        	goto ErrExit;
    	}

    	/* ���֧ȡ��ʽ�ĺϷ��� */
    	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no, g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"����֧ȡ�������!!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT04");
        	goto ErrExit;
    	}
	}

    /* ��ѯ�����ʻ����ļ� */
    strcpy(g_pub_tx.cur_no,"01");
    ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����ʻ����ļ�����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT06");
        goto ErrExit;
    }

    /*set_zd_double("0420",g_dd_mst.bal);    20080311�ʻ���� */
	ky_amt = g_dd_mst.bal - g_dd_mst.ctl_amt; /*����������ȥ���ƽ��*/
    set_zd_double("0390",ky_amt);   /* �������*/
    set_zd_double("0400",g_dd_mst.bal);   /* �ʻ���� */
	vtcp_log("%s,%d,���[%.2f],�������[%.2f]\n",__FILE__,__LINE__,g_dd_mst.bal,ky_amt);
    set_zd_data("0270",g_dd_mst.name);    /* ���� */
	
	/*begin add by LiuHuafeng 200701022 */
	ret=pubf_card_chk_msr(1,g_pub_tx.crd_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����Ǽǲ�����!!!");
        WRITEMSG
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
            strcpy(g_pub_tx.reply,"AT06");
        goto ErrExit;
    }
	/* end by LiuHuafeng 20070122 */

    /* ���� ������� �� ���տ������ ���� ʵ����� FD74 */
    ret = iSetAdditional_Amounts( g_dd_mst.bal, g_dd_mst.bal);  /* ���� FD74 ʵ�����*/
    if(ret)
    {
        return -1;
     }
	

    /* �������豸�����ռǽṹ��ֵ 
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

    strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
  

    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"2");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);
   
    get_zd_long("0520", &chnl_self_trade.stra);

    chnl_self_trade.tx_amt = 0.00;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    strcpy(chnl_self_trade.dz_flag,"0");
    * strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind); *
    strcpy(chnl_self_trade.info_source,&flag);
    strcpy(chnl_self_trade.deal_flag,"0");
    strcpy(chnl_self_trade.brf_no,"PSYC");
    strcpy(chnl_self_trade.add_ind,"");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0");*/


    /* �Ǽ������豸�����ռ� 
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
        WRITEMSG
        goto ErrExit;
    }*/

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;


ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

