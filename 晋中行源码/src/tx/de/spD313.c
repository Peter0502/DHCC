/***************************************************************
* �� �� ��:  spD313.c
* ��������:  ���Ĵ�ǩԼ	
* ��    ��:  nx

* ������ڣ�    
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_sxc_c.h"

int spD313()
{
     int iRet=0;
     
     char cFlag[2];
     struct mo_sxc_c     sMo_sxc;
     struct mdm_ac_rel_c sMdm_ac_rel;
     struct dd_mst_c     sDd_mst;
     
     memset(cFlag,       0x00, sizeof(cFlag));
     memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
     memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
     memset(&sDd_mst,     0x00, sizeof(sDd_mst));

     pub_base_sysinit();
     
     get_zd_data("0300", g_pub_tx.ac_no);
     pub_base_strpack(g_pub_tx.ac_no);
     get_zd_int("0490",  &g_pub_tx.ac_seqn);
     get_zd_data("0700", cFlag);     /**1Ϊ����ǩԼר�ã��������ײ��ɴ�ֵ1**/
     vtcp_log("ac_no[%s], ac_seqn[%d]", g_pub_tx.ac_no, g_pub_tx.ac_seqn);
        
 
     /*����ac_no�ӱ�mdm_ac_rel�л�ȡac_id*/
     iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
     if(iRet)
     {
     	vtcp_log("��ȡac_idʧ�ܣ�iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "D101");
     	goto ErrExit;	
     }
     
     /**���Ĵ�ֻ��Խ�̫�����͸��ϲ��Կ�2���ţ�����������ǩԼ���Ĵ� 20151128**/
     if(strcmp(sMdm_ac_rel.mdm_code,"0027")!=0 && strcmp(sMdm_ac_rel.mdm_code,"0032")!=0)
     	goto OkExit;
     
     if(cFlag[0] != '1')  /*����ʱ���ߴ˷�֧*/
     {
     	 iRet = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld and ac_seqn=%d", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
         if(iRet)
         {
     	     vtcp_log(" Dd_mst_Sel���� �˻�[%ld][%d]������iRet[%d]", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn, iRet);
     	     strcpy(g_pub_tx.reply, "P102");
     	     goto ErrExit;
         }
     	 if(sDd_mst.ac_type[0]!='5')
         {
             vtcp_log("�Թ����ڲ�����ǩԼ");
             strcpy(g_pub_tx.reply, "S035");
             goto ErrExit;
         }
         
         /*���ݵ�ǰ��ȡ��ac_id�жϵ�ǰ�˻��Ƿ�ǩԼ���Ĵ�*/
         iRet = sql_count("mo_sxc", "ac_id=%ld and ac_seqn=%d and sts='1'", sMdm_ac_rel.ac_id, g_pub_tx.ac_seqn);
         if(iRet > 0)
         {
       	     set_zd_data("0130", "��ǰ�˻���ǩԼ���Ĵ�");
       	     vtcp_log("��ǰ�û���ǩԼ��,iRet=[%d]", iRet);
       	     strcpy(g_pub_tx.reply, "S035");
       	     goto ErrExit;
         }
         
         /*****ǩԼ֮ǰ�ȹ������� ***/
         g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
         g_pub_tx.tx_amt1 = 0.00;
         strcpy( g_pub_tx.add_ind, "1" );  /**����**/;
         strcpy( g_pub_tx.cur_no, "01" );
         strcpy( g_pub_tx.ct_ind, "2" );
         strcpy( g_pub_tx.prt_ind, "0" );  /*�Ƿ���۱�־*/  
         g_pub_tx.svc_ind = 1001;          /**��ȡ��***/
         strcpy(g_pub_tx.hst_ind, "1");
         strcpy(g_pub_tx.brf, "���Ĵ�ǩԼ");
         strcpy(g_pub_tx.ac_wrk_ind, "000000000");
         g_reply_int = pub_acct_trance();
         if(g_reply_int != 0)
         {
             vtcp_log("��������ʧ��!");
             strcpy(g_pub_tx.reply, "S035");
             goto ErrExit;
         }
         
         sMo_sxc.qy_type[0] = '2'; /**ǩԼ���ױ�־**/
     }
     else
     {
     	sMo_sxc.qy_type[0] = '1';  /**����ǩԼ��־1**/
     }
     
     /*�Ǽ����Ĵ�ǼǱ�*/
     sMo_sxc.ac_id = sMdm_ac_rel.ac_id;
     sMo_sxc.ac_seqn = g_pub_tx.ac_seqn;
     if(sMo_sxc.ac_seqn != 1)
     {
     	/**������ʾ��Ŀǰ������Ŷ�Ϊ1������Ժ�����ӿ��������Ų�Ϊ1���˻�����ע���޸Ĳ�ѯ����sp8314,sp8315�е��˻����1**/
        vtcp_log("ac_seqn[%d]��Ų�Ϊ1�����������Ĵ�ǩԼ", sMo_sxc.ac_seqn);
        strcpy(g_pub_tx.reply, "S035");
        goto ErrExit;
     }
     sMo_sxc.bal = 0.00;
     sMo_sxc.intst = 0.00;
     sMo_sxc.un_intst = 0.00;
     sMo_sxc.tx_date   = g_pub_tx.tx_date;
     sMo_sxc.trace_no  = g_pub_tx.trace_no;
     strcpy(sMo_sxc.tx_br_no, g_pub_tx.tx_br_no);
     strcpy(sMo_sxc.tx_tel, g_pub_tx.tel);
     strcpy(sMo_sxc.opn_br_no, sMdm_ac_rel.opn_br_no);
     sMo_sxc.sts[0]  = '1';   /**1-ǩԼ**/
     sMo_sxc.flag[0] = '1';   /**1-���շ���ҵ�� ����ִ������**/
     
     iRet = Mo_sxc_Ins(sMo_sxc, g_pub_tx.reply);
     if(iRet)
     {
        vtcp_log("�Ǽ����Ĵ�ǼǱ�ʧ�ܣ�iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D102");
        goto ErrExit;	
     }
      
     /**** �ǼǼ���ˮ *******/
     g_pub_tx.ac_wrk_ind[1] = '1'; /**1-����ˮ**/
     g_pub_tx.ac_id = sMo_sxc.ac_id;
     strcpy(g_pub_tx.brf, "���Ĵ�ǩԼ");
     iRet = pub_ins_trace_log();
     if(iRet)
     {
     	vtcp_log("�Ǽ���ˮʧ�ܣ�iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "S035");
     	goto ErrExit;	
     }
     /**����ǩԼ����**/
     if(cFlag[0] != '1')
     {
         iRet = pub_mob_sendmail(g_pub_tx.brf, g_pub_tx.tx_code, g_pub_tx.ac_no, "2", 0, 0);	
     }
     
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "success!!");
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(strlen(g_pub_tx.reply)==0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply, "S035");	
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;

}
