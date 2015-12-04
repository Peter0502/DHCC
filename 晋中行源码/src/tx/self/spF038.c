/*************************************************
* �� �� ��:  spF038.c
* ���������� POS Ԥ��Ȩ����
*
* ��    ��:  rob
* ������ڣ� 2003��2��21��
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
#include "chnl_premise_c.h"

int spF038()
{

    int ret;
    char flag;
    long old_date;
    long premise_no;
    long old_trace_no=0;
    char dc_ind[2],ct_ind[2],fee_ind[2];
    char msr_str[38];
    char old_crd_no[20];    /*������ʱ��ƽ̨�����˻�*/
    char crd_no[20];
    struct chnl_marchinfo_c chnl_marchinfo;
    struct chnl_self_trade_c chnl_self_trade;
    struct chnl_premise_c    sChnl_premise;
    struct chnl_premise_c    mChnl_premise;
    
    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(&sChnl_premise,0x0,sizeof(struct chnl_premise_c));
    memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));
    memset(dc_ind,0,sizeof(dc_ind));
    memset(ct_ind,0,sizeof(ct_ind));
    memset(fee_ind,0,sizeof(fee_ind));
    memset(msr_str,0,sizeof(msr_str));
    memset(old_crd_no,0,sizeof(old_crd_no));
    memset(crd_no,0,sizeof(crd_no));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    get_zd_data("0750",msr_str); /*2�ŵ���Ϣ*/
    get_zd_data("0300",g_pub_tx.crd_no); /* ���� */
    strcpy(crd_no,g_pub_tx.crd_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */

    get_zd_data("0660",dc_ind);
    get_zd_data("0680",ct_ind);
    get_zd_data("0690",fee_ind);
    get_zd_long("0440",&sChnl_premise.pt_date); 
    get_zd_long("0520",&sChnl_premise.pt_trace_no); 
    get_zd_long("0780",&old_trace_no); /* ԭ�ն���ˮ�� */
    get_zd_long("0450",&old_date);              /* ԭ�ն�����*/	
    get_zd_long("0870",&premise_no);  /* Ԥ��Ȩ�� */
     
    vtcp_log(" ԭƽ̨��ˮ��======[%d]  ԭ����=====[%d]", g_pub_tx.old_trace_no, old_date);
	
    get_zd_data("0730",chnl_marchinfo.mrchno); /* �̻��� */
    vtcp_log("Ԥ��Ȩ��===premise_no===[%d]", premise_no);
    
    get_zd_data("0330",old_crd_no);
    pub_base_old_acno(old_crd_no);
    vtcp_log("%s%d,�ɿ���Ϊ[%s]",__FILE__,__LINE__,old_crd_no);

    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        goto ErrExit;
    }
    if(strlen(msr_str) != 0)
    {
	g_reply_int = pubf_card_chk_msr(3,g_pub_tx.crd_no);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,cvnУ�����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
	}
    }
    /* ��鿨��֧ȡ��ʽ *
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        goto ErrExit;
    }

    * ���֧ȡ��ʽ�ĺϷ��� 
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����֧ȡ�������!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU55");
        goto ErrExit;
    } �������ײ���֤����*/
    /*���������������Ӧ���ɿ��Ų�ѯ*/
    if(memcmp(crd_no,old_crd_no,19) && strlen(old_crd_no)!=0)
    {
    	strncpy(crd_no,old_crd_no,19);
    }
    ret = Chnl_premise_Sel(g_pub_tx.reply,&mChnl_premise,"ac_no='%s' and premise_no=%ld",crd_no,premise_no); 
    if(ret)
    {
        sprintf(acErrMsg,"ԭ��¼������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    /* ���������ͽ��
    if(pub_base_CompDblVal(mChnl_premise.premise_amt,g_pub_tx.tx_amt1) != 0)
    {
        sprintf(acErrMsg,"���������Ԥ��Ȩ������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    */
    char tmpstr[51];
    memset(tmpstr,0,sizeof(tmpstr));
    get_zd_data("0810",tmpstr);
    memcpy(mChnl_premise.filler,tmpstr,30); 
    ret = sql_execute2("update chnl_premise set sts='1', pt_date=%ld, pt_trace_no=%ld, filler='%s', ac_no='%s' where ac_no='%s' and premise_no=%ld",sChnl_premise.pt_date,sChnl_premise.pt_trace_no,mChnl_premise.filler,g_pub_tx.crd_no,crd_no,premise_no); 
    if(ret)
    {
        sprintf(acErrMsg,"���³�����¼����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    
   vtcp_log("%s,%d,chnl_marchinfo.mrchno====[%s],chnl_self_trade.spcd====[%s]",
	__FILE__,__LINE__,chnl_marchinfo.mrchno,chnl_self_trade.spcd);

    /* �޸��ʻ��Ŀ��ƽ����ʻ��ļ�� */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
                g_pub_tx.cur_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
    }
    g_pub_tx.ac_id = 0;
    g_pub_tx.tx_amt1 =  mChnl_premise.premise_amt; /*��ɽ��*/
    vtcp_log("%s,%d,�����־[%s]",__FILE__,__LINE__,dc_ind);
    if(dc_ind[0] =='1')
    {
    	g_pub_tx.add_ind[0] = '0'; /* ���ٿ��ƽ�� */
    }else if(dc_ind[0]=='2'){
    	g_pub_tx.add_ind[0] = '1'; /* ���ٿ��ƽ�� */
    }
    g_pub_tx.svc_ind = 1006;   /*  Ԥ��Ȩ  */
    g_pub_tx.hst_ind[0] = '0'; /* ������ϸ */
    memcpy(g_pub_tx.brf,tmpstr,20);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos Ԥ��Ȩ--����!!!");
        WRITEMSG
        goto ErrExit;
    }

    /* �������豸�����ռǽṹ��ֵ
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));	
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
    * �����׶���������ˮ�� *

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.premise_date = g_pub_tx.tx_date;
    chnl_self_trade.premise_no = premise_no;
    chnl_self_trade.premise_amt = g_pub_tx.tx_amt1;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;
    chnl_self_trade.dz_sxamt = 0.00;
    
    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    get_zd_long("0520",&chnl_self_trade.stra);
    get_zd_long("0440",&chnl_self_trade.dz_date);
    get_zd_long("0780",&chnl_self_trade.strb);	
	
    strcpy(chnl_self_trade.dz_flag,"0");
    strcpy(chnl_self_trade.info_source,&flag);
    strcpy(chnl_self_trade.deal_flag,"0");
    strcpy(chnl_self_trade.brf_no,"PSSC");
    strcpy(chnl_self_trade.add_ind,"0");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0"); */

    /* �Ǽ������豸�����ռ�
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
        WRITEMSG
        goto ErrExit;
    } */

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;


ErrExit:
	if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
        {
                strcpy(g_pub_tx.reply,"CU06");
        }
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

