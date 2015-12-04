/***************************************************************
* �� �� ��:  spD317.c
* ��������:  ���ӹ���ǩԼ	
* ��    ��:  lq

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
#include "mo_qzgl_c.h"
#include "dd_mst_c.h"

int spD317()
{
     int iRet=0;
     long child_ac_id=0;
     long part_ac_id=0;
     double g_qy_amt=0;
     char cFlag[2];
     
     struct mo_qzgl_c    sMo_qzgl;
     struct mdm_ac_rel_c sMdm_ac_rel_1;
     struct mdm_ac_rel_c sMdm_ac_rel_2;
     struct dd_mst_c     sDd_mst;
     
     memset(&sMo_qzgl,    0x00, sizeof(sMo_qzgl));
     memset(&sMdm_ac_rel_1, 0x00, sizeof(sMdm_ac_rel_1));
     memset(&sMdm_ac_rel_2, 0x00, sizeof(sMdm_ac_rel_2));
     memset(&sDd_mst,     0x00, sizeof(sDd_mst));
     memset(cFlag,     0x00, sizeof(cFlag));

     pub_base_sysinit();
     
     get_zd_data("0300", g_pub_tx.ac_no);
     get_zd_data("0310", g_pub_tx.ac_no1);
     get_zd_data("0720", cFlag);
     get_zd_double("0490", &g_qy_amt);
     pub_base_strpack(g_pub_tx.ac_no);
     vtcp_log("child_ac_no[%s],part_ac_no[%s],g_qy_amt=[%lf]", g_pub_tx.ac_no, g_pub_tx.ac_no1,g_qy_amt);
     
     
		 if(cFlag[0]=='1')
		 	{
		 		sMo_qzgl.hz_date=8;
		 	}else if(cFlag[0]=='2')
		 	{
		 		sMo_qzgl.hz_date=18;
		 	}
		 	else if(cFlag[0]=='3')
		 	{
		 		sMo_qzgl.hz_date=28;
		 	}else{}
		 	
     /*���ݶ�ͯ��ac_no�ӱ�mdm_ac_rel�л�ȡ��ͯ��ac_id*/
     iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_1, "ac_no='%s'", g_pub_tx.ac_no);
     if(iRet)
     {
     	vtcp_log("��ȡ��ͯ��ac_idʧ�ܣ�iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "D101");
     	goto ErrExit;	
     }
     child_ac_id=sMdm_ac_rel_1.ac_id;
     
     
     /*���ݸ�ĸ��ac_no�ӱ�mdm_ac_rel�л�ȡ��ĸ��ac_id*/
     iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_no='%s'", g_pub_tx.ac_no1);
     if(iRet)
     {
     	vtcp_log("��ȡ��ĸ��ac_idʧ�ܣ�iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "D101");
     	goto ErrExit;	
     }
     part_ac_id=sMdm_ac_rel_2.ac_id;     
     
     vtcp_log("%s %d ��ͯ��ac_id=[%ld],��ĸ��ac_id=[%ld]", __FILE__,__LINE__,child_ac_id,part_ac_id);
     
     /*���ݵ�ǰ��ȡ��ac_id�жϵ�ǰ�˻��Ƿ�ǩԼ���Ĵ�*/
     iRet = sql_count("mo_qzgl", "child_ac_id=%ld and part_ac_id=%ld and sts='1'", child_ac_id,part_ac_id);
     if(iRet > 0)
     {
   	     set_zd_data("0130", "���ڵ�ǰ���ӹ�����ϵ");
   	     vtcp_log("��ǰ�����˺���ǩԼ��,iRet=[%d]", iRet);
   	     strcpy(g_pub_tx.reply, "S035");
   	     goto ErrExit;
     }
     
     /*�Ǽ����ӹ����ǼǱ�*/
     sMo_qzgl.child_ac_id = child_ac_id;
     sMo_qzgl.part_ac_id	 = part_ac_id;
     sMo_qzgl.tx_date   = g_pub_tx.tx_date;
     sMo_qzgl.trace_no  = g_pub_tx.trace_no;
     strcpy(sMo_qzgl.tx_br_no, g_pub_tx.tx_br_no);
     strcpy(sMo_qzgl.tx_tel, g_pub_tx.tel);
     sMo_qzgl.qy_amt = g_qy_amt;
     sMo_qzgl.sts[0]  = '1';   /**1-ǩԼ**/
     
     iRet = Mo_qzgl_Ins(sMo_qzgl, g_pub_tx.reply);
     if(iRet)
     {
        vtcp_log("�Ǽ����ӹ����ǼǱ�ʧ�ܣ�iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D102");
        goto ErrExit;	
     }
      
     /**** �ǼǼ���ˮ *******/
     g_pub_tx.ac_wrk_ind[1] = '1'; /**1-����ˮ**/
     g_pub_tx.ac_id = sMo_qzgl.part_ac_id;
     strcpy(g_pub_tx.brf, "���ӹ���ǩԼ");
     iRet = pub_ins_trace_log();
     if(iRet)
     {
     	vtcp_log("�Ǽ���ˮʧ�ܣ�iRet=[%d]", iRet);
     	strcpy(g_pub_tx.reply, "S035");
     	goto ErrExit;	
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
