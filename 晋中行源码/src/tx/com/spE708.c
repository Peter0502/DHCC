/************************************************************
* �� �� ��:  spE708.c
* ��������: ����ũ�Ŵ����������
* ˵    ��: �ó���ֻ�漰���˱��𣬱��ڣ������������Ϣ�ĵ�����
            ���ڵ�ֵѺƷ��û���κδ���
* ��    ��:  xyy
* ������ڣ� 2009-3-3 17:15:44
* insert into tx_def values('3320','�����������','00000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','2','2','3','');
* insert into tx_flow_def values('3320','0','E708','#$');
* insert into tx_sub_def values('E708','�����������','spE708','0','0');
* insert into auth_item(tx_code,seqn,ind,mo) values('3320',1,'0000','�����������');
* insert into auth_cond values('S','0','3320',1,'','sq(347A)');
* �޸ļ�¼�� 
*   ��   ��:2009/8/13 11:36:13
*   �� �� ��:martin 
*   �޸�����:ȥ��dc_log�е�tx_code�ֶΣ���Ϊ��Щ����û������ֶ�
**************************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_parm_c.h"
#include "dc_log_c.h"
#include "dc_acc_rel_c.h"
#include "ln_mst_hst_c.h"
#include "intrtmpreg_c.h"

int spE708()
{
    struct ln_mst_c     sLn_mst;
    struct dc_log_c     sDc_log;
    struct ln_parm_c    sLn_parm;
    struct dc_acc_rel_c sDc_acc_rel;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct ln_mst_hst_c sLn_mst_hst;
    struct intrtmpreg_c sIntrtmpreg;
    int    ret=0,iCnt=0;
    int    iTrace_cnt=1;
    int    iCount=0;
    long   lAc_id=0;
    char   cAcno[25];
    char   cTmpBrno[3];
    char   cKinbr[BRNO_LEN+1];/**�Է�����**/
    char   cTxbrno[BRNO_LEN+1];/**��ǰ����**/
    char   cDc_code[5];
    char   cAcc_hrt1[8];/**�����Ŀ**/
    char   cAcc_hrt2[8];/**���ڿ�Ŀ**/
    char   cAcc_hrt3[8];/**�����Ŀ**/
    char   cAcc_hrt4[8];/**��Ϣ��Ŀ**/
    char   cAcc_hrt5[8];/**704��ѺƷ��Ŀ**/

    /* ��ʼ���ṹ */
    memset(&sLn_mst    , 0 , sizeof(sLn_mst));
    memset(&sDc_log    , 0 , sizeof(sDc_log));
    memset(&sLn_parm   , 0 , sizeof(sLn_parm));
    memset(&sDc_acc_rel, 0 , sizeof(sDc_acc_rel));
    memset(&sMdm_ac_rel, 0 , sizeof(sMdm_ac_rel));
    memset(&sLn_mst_hst, 0 , sizeof(sLn_mst_hst));
    memset(&sIntrtmpreg, 0 , sizeof(sIntrtmpreg));
    memset(cAcno       , 0 , sizeof(cAcno));
    memset(cKinbr      , 0 , sizeof(cKinbr));
    memset(cTxbrno     , 0 , sizeof(cTxbrno));
    memset(cTmpBrno    , 0 , sizeof(cTmpBrno));
    memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    memset(cAcc_hrt5   , 0 , sizeof(cAcc_hrt5));
    memset(cDc_code    , 0 , sizeof(cDc_code));

    pub_base_sysinit();

    get_zd_data("0300", cAcno);     /* �����˺� */
    get_zd_data("0910", cKinbr);    /* ת����� */
    get_zd_data("0030", cTxbrno);
 
    pub_base_strpack(cAcno);

    /*memcpy(g_pub_tx.ac_no , cAcno , sizeof(g_pub_tx.ac_no)-1);*/

    memcpy(cTmpBrno , cTxbrno,2);
	pub_base_old_acno(cAcno);
    ret = Mdm_ac_rel_Dec_Upd( g_pub_tx.reply, "ac_no='%s' ", cAcno);
    if(ret)
    {
        sprintf( acErrMsg,"��ѯ�˻���Ϣ����");
        WRITEMSG
        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
        goto ErrExit;
    }
    ret = Mdm_ac_rel_Fet_Upd(&sMdm_ac_rel, g_pub_tx.reply);
    if (ret)
    {
         sprintf(acErrMsg, "��ѯ�����˻���ϵ�����!");
         WRITEMSG
         vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
         goto ErrExit;
    }
    if(memcmp(sMdm_ac_rel.opn_br_no,cTxbrno,sizeof(sMdm_ac_rel.opn_br_no)-1)!=0)
    {
    	   sprintf(acErrMsg, "���˻����Ǳ������˻�!");
         WRITEMSG
	memcpy(g_pub_tx.reply,"L222",4);
         goto ErrExit;
    }

    memcpy(sMdm_ac_rel.opn_br_no,cKinbr,sizeof(sMdm_ac_rel.opn_br_no)-1);

    ret=Mdm_ac_rel_Upd_Upd(sMdm_ac_rel,g_pub_tx.reply);
    if(ret)
    {
    	vtcp_log("[%s][%d]���½����˻���ϵ�����[%d]",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"���½����˻���ϵ�����");
        WRITEMSG
        goto ErrExit;
    }

    Mdm_ac_rel_Clo_Upd();
    
    vtcp_log("[%s][%d] ����׼�����´������ļ�[%ld]\n",__FILE__,__LINE__,sMdm_ac_rel.ac_id);
    ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld'",sMdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf( acErrMsg, "Declare dc_log error!!" );
        WRITEMSG
        vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
        goto ErrExit;
    }
    while(1)
    {
    	  ret = Ln_mst_Fet_Upd(&sLn_mst, g_pub_tx.reply);
    	  if(ret==100 && iCount>0)
        	  	 break;
        if (ret)
        {
             sprintf(acErrMsg, "��ѯ�������ļ�����!");
             WRITEMSG
             vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
             goto ErrExit;
        }
        vtcp_log("[%s][%d] ״̬=[%s]  ����=[%f] \n",__FILE__,__LINE__,sLn_mst.ac_sts,sLn_mst.bal);
        if(sLn_mst.ac_sts[0]!='1' && sLn_mst.ac_sts[0]!='2' && sLn_mst.ac_sts[0]!='3' && sLn_mst.ac_sts[0]!='4' && sLn_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "���˻�״̬����[%s]!",sLn_mst.ac_sts);
             WRITEMSG
             vtcp_log("[%s][%d] ���˻�[%d]״̬[%s]����",__FILE__,__LINE__,sLn_mst.ac_id,sLn_mst.ac_sts);
             goto ErrExit;
        }
        lAc_id = sLn_mst.ac_id;
        vtcp_log("[%s][%d] ����Ϣ=[%f] ����Ϣ=[%f]\n",__FILE__,__LINE__,sLn_mst.in_lo_intst,sLn_mst.out_lo_intst);
        ret = Ln_parm_Sel( g_pub_tx.reply, &sLn_parm, "prdt_no='%s'", sLn_mst.prdt_no );
        if(ret)
        {
            sprintf( acErrMsg,"��ѯ�˻��Ĳ�Ʒ��Ϣ����");
            WRITEMSG
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
        if(sLn_mst.ac_sts[0]=='1')/**��������**/
        {
        	  memcpy(cDc_code,sLn_parm.dc_code,sizeof(sLn_parm.dc_code)-1);
        }
        else if(sLn_mst.ac_sts[0]=='2')/**����90����**/
        {
        	  memcpy(cDc_code,sLn_parm.over_dc_code,sizeof(sLn_parm.over_dc_code)-1);
        }
        else if(sLn_mst.ac_sts[0]=='3')/**����**/
        {
        	  memcpy(cDc_code,sLn_parm.sla_dc_code,sizeof(sLn_parm.sla_dc_code)-1);
        }
        else if(sLn_mst.ac_sts[0]=='4')/**����**/
        {
        	  memcpy(cDc_code,sLn_parm.bad_dc_code,sizeof(sLn_parm.bad_dc_code)-1);
        }
        else if(sLn_mst.ac_sts[0]=='5')/**����90����**/
        {
        	  memcpy(cDc_code,sLn_parm.over90_dc_code,sizeof(sLn_parm.over90_dc_code)-1);
        }
        else 
        {
        	  sprintf(acErrMsg, "���˻�״̬����[%s]!",sLn_mst.ac_sts);
            WRITEMSG
            vtcp_log("[%s][%d] ���˻�[%s]״̬[%s]����",__FILE__,__LINE__,sLn_mst.ac_id,sLn_mst.ac_sts);
            goto ErrExit;
        }
        vtcp_log("[%s][%d] ac_sts=[%s] dc_code=[%s] \n",__FILE__,__LINE__,sLn_mst.ac_sts,cDc_code);
        ret=Dc_acc_rel_Dec_Sel(g_pub_tx.reply,"dc_code='%s'",cDc_code);
        if(ret)
        {
            sprintf( acErrMsg,"��ѯ��ƹ�ϵ[%s]�����",cDc_code);
            WRITEMSG
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
        iCnt= 0 ;
        memcpy(g_pub_tx.ac_no , cAcno , sizeof(g_pub_tx.ac_no)-1);/*add by martin 2009/8/14 14:39:18 xyy������*/
        
        g_pub_tx.ac_id = sLn_mst.ac_id ;/*add by martin 2009/8/14 15:28:11 */
        while(1)
        {
        	  memset(&sDc_acc_rel, 0 , sizeof(sDc_acc_rel));
        	  ret=Dc_acc_rel_Fet_Sel(&sDc_acc_rel,g_pub_tx.reply);
        	  if(ret==100 && iCnt>0)
        	  {
        	  	 break;
        	  }
            if(ret)
            {
                sprintf( acErrMsg, "Fetch dc_acc_rel record error!!" );
                WRITEMSG
                vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
                goto ErrExit;
            }
            vtcp_log("[%s][%d] data_code=[%s]\n",__FILE__,__LINE__,sDc_acc_rel.data_code);
            if(memcmp(sDc_acc_rel.data_code,"0152",4)==0)/**����**/
            {
            	  memcpy(cAcc_hrt1,sDc_acc_rel.acc_hrt,sizeof(cAcc_hrt1)-1);
            	  vtcp_log("[%s][%d] cAcc_hrt1=[%s]",__FILE__,__LINE__,cAcc_hrt1);
            	  if(pub_base_CompDblVal(sLn_mst.bal,0.00)>0)
                {
                	  /*****ת����������ˮ****/
                	  memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 1;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt1,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"1",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"����ת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"����ת��");

                    sDc_log.amt = -sLn_mst.bal;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                    
                    /******ת���������ˮ*****/
                    memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 1;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cKinbr,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt1,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"1",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"����ת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"����ת��");

                    sDc_log.amt = sLn_mst.bal;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]������ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                    
                }
            }
            else if(memcmp(sDc_acc_rel.data_code,"0155",4)==0)/**����Ϣ**/
            {
            	  memcpy(cAcc_hrt2,sDc_acc_rel.acc_hrt,sizeof(cAcc_hrt2)-1);
            	  vtcp_log("[%s][%d] cAcc_hrt2=[%s]",__FILE__,__LINE__,cAcc_hrt2);
            	  if(pub_base_CompDblVal(sLn_mst.in_lo_intst,0.00)>0)
                {
                    vtcp_log("[%s][%d] sLn_mst.in_lo_intst=[%f]\n",__FILE__,__LINE__,sLn_mst.in_lo_intst);
                    /*****ת����������ˮ****/
                	  memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 2;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt2,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"1",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"����Ϣת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"����Ϣת��");

                    sDc_log.amt = -sLn_mst.in_lo_intst;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                    
                    /******ת���������ˮ*****/
                    memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 2;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cKinbr,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt2,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"1",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"����Ϣת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"����Ϣת��");

                    sDc_log.amt = sLn_mst.in_lo_intst;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                }
            }
            else if(memcmp(sDc_acc_rel.data_code,"0156",4)==0)/**����Ϣ**/
            {
            	  memcpy(cAcc_hrt3,sDc_acc_rel.acc_hrt,sizeof(cAcc_hrt3)-1);
            	  vtcp_log("[%s][%d] cAcc_hrt3=[%s]",__FILE__,__LINE__,cAcc_hrt3);
            	  if(pub_base_CompDblVal(sLn_mst.out_lo_intst,0.00)>0)
                {
                    vtcp_log("[%s][%d] sLn_mst.out_lo_intst=[%f]\n",__FILE__,__LINE__,sLn_mst.out_lo_intst);
                    /*****ת����������ˮ****/
                	  memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 3;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt3,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"2",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"����Ϣת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"����Ϣת��");

                    sDc_log.amt = -sLn_mst.out_lo_intst;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                    
                    /******ת���������ˮ*****/
                    memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 3;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cKinbr,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt3,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"2",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"����Ϣת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"����Ϣת��");

                    sDc_log.amt = sLn_mst.out_lo_intst;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                }
            }
            else if(memcmp(sDc_acc_rel.data_code,"0158",4)==0)/**��Ϣ**/
            {
            	  memcpy(cAcc_hrt4,sDc_acc_rel.acc_hrt,sizeof(cAcc_hrt4)-1);
            	  vtcp_log("[%s][%d] cAcc_hrt4=[%s]",__FILE__,__LINE__,cAcc_hrt4);
            	  if(pub_base_CompDblVal(sLn_mst.in_lo_intst,0.00)>0)
                {
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    /*****ת����������ˮ****/
                	  memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 4;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt4,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"2",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"��Ϣ����ת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"��Ϣ����ת��");

                    sDc_log.amt = -sLn_mst.in_lo_intst;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                    
                    /******ת���������ˮ*****/
                    memset(&sDc_log,0 ,sizeof(sDc_log));
                    vtcp_log("[%s][%d] sLn_mst.bal=[%f]\n",__FILE__,__LINE__,sLn_mst.bal);
                    sDc_log.tx_date = g_pub_tx.tx_date;
                    sDc_log.trace_no = g_pub_tx.trace_no;
                    sDc_log.trace_cnt = iTrace_cnt;
                    sDc_log.dc_seqn  = 4;
                    
                    memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
                    strcat(sDc_log.opn_br_no,"000");
                    memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
                    memcpy(sDc_log.tx_opn_br_no,cKinbr,BRNO_LEN);
                    memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
                    memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
                    memcpy(sDc_log.acc_hrt,cAcc_hrt4,sizeof(sDc_log.acc_hrt)-1);
                    memcpy(sDc_log.dc_ind,"2",sizeof(sDc_log.dc_ind)-1);
                    memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
                    memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
                    memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
                    /*memcpy(sDc_log.brf      ,"��Ϣ����ת��",sizeof(sDc_log.brf )-1);*/
                    strcpy(g_pub_tx.brf,"��Ϣ����ת��");

                    sDc_log.amt = sLn_mst.in_lo_intst;
                    
                    ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
                    if(ret)
                    {
                        Dc_log_Debug(&sDc_log);
                        vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"���뽻����ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
                    g_pub_tx.tx_amt1=sDc_log.amt;
                    ret = pub_ins_trace_log();  
                    if (ret) {
                        vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
                        sprintf(acErrMsg,"������ˮ�����");
                        WRITEMSG
                        goto ErrExit;
                    }
                    iTrace_cnt++;
                }
            }
            else
            {
            	  vtcp_log("[%s][%d] data_code=[%s]",__FILE__,__LINE__,sDc_acc_rel.data_code);
            	  continue;
            }
            iCnt ++;
            vtcp_log("[%s][%d] iCnt=[%d]\n",__FILE__,__LINE__,iCnt);
       }
       Dc_acc_rel_Clo_Sel();
       vtcp_log("[%s][%d] ��ˮ��4=[%d]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
       /*ret = pub_ins_trace_log();  �޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58
       if (ret) {
           vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
           sprintf(acErrMsg,"������ˮ�����");
           WRITEMSG
           goto ErrExit;
       }*/
       /*add by martin 2009/12/23 18:01:22 ���ݽ����������Ӷ�704 ��ѺƷ������*/
       if(pub_base_CompDblVal(sLn_mst.gage_amt,0.00)>0)
       {
           memcpy(cAcc_hrt5,"70400",sizeof(cAcc_hrt5)-1);
           vtcp_log("[%s][%d] sLn_mst.gage_amt=[%f]\n",__FILE__,__LINE__,sLn_mst.gage_amt);
           /*****ת����������ˮ****/
           memset(&sDc_log,0 ,sizeof(sDc_log));
           vtcp_log("[%s][%d] sLn_mst.gage_amt=[%f]\n",__FILE__,__LINE__,sLn_mst.gage_amt);
           sDc_log.tx_date = g_pub_tx.tx_date;
           sDc_log.trace_no = g_pub_tx.trace_no;
           sDc_log.trace_cnt = iTrace_cnt;
           sDc_log.dc_seqn  = 5;
           
           memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
           strcat(sDc_log.opn_br_no,"000");
           memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
           memcpy(sDc_log.tx_opn_br_no,cTxbrno,BRNO_LEN);
           memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
           memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
           memcpy(sDc_log.acc_hrt,cAcc_hrt5,sizeof(sDc_log.acc_hrt)-1);
           memcpy(sDc_log.dc_ind,"2",sizeof(sDc_log.dc_ind)-1);
           memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
           memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
           memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
           strcpy(g_pub_tx.brf,"��ѺƷת��");

           sDc_log.amt = -sLn_mst.gage_amt;
           
           ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
           if(ret)
           {
               Dc_log_Debug(&sDc_log);
               vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
               sprintf(acErrMsg,"���뽻����ˮ�����");
               WRITEMSG
               goto ErrExit;
           }
           /*�޸ĳɶ��ҵ����ˮ modify by martin 2009/8/14 14:43:58*/
           g_pub_tx.tx_amt1=sDc_log.amt;
           ret = pub_ins_trace_log();  
           if (ret) {
               vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
               sprintf(acErrMsg,"������ˮ�����");
               WRITEMSG
               goto ErrExit;
           }
           iTrace_cnt++;
           
           /******ת���������ˮ*****/
           memset(&sDc_log,0 ,sizeof(sDc_log));
           vtcp_log("[%s][%d] sLn_mst.gage_amt=[%f]\n",__FILE__,__LINE__,sLn_mst.gage_amt);
           sDc_log.tx_date = g_pub_tx.tx_date;
           sDc_log.trace_no = g_pub_tx.trace_no;
           sDc_log.trace_cnt = iTrace_cnt;
           sDc_log.dc_seqn  = 5;
           
           memcpy(sDc_log.opn_br_no,cTmpBrno,sizeof(cTmpBrno)-1);
           strcat(sDc_log.opn_br_no,"000");
           memcpy(sDc_log.tx_br_no ,sDc_log.opn_br_no,sizeof(sDc_log.opn_br_no)-1);
           memcpy(sDc_log.tx_opn_br_no,cKinbr,BRNO_LEN);
           memcpy(sDc_log.tx_tx_br_no,cTxbrno,BRNO_LEN);
           memcpy(sDc_log.cur_no,"01",sizeof(sDc_log.cur_no)-1);
           memcpy(sDc_log.acc_hrt,cAcc_hrt5,sizeof(sDc_log.acc_hrt)-1);
           memcpy(sDc_log.dc_ind,"2",sizeof(sDc_log.dc_ind)-1);
           memcpy(sDc_log.ct_ind,"2",sizeof(sDc_log.ct_ind)-1);
           memcpy(sDc_log.tel      ,g_pub_tx.tel,sizeof(sDc_log.tel)-1);
           memcpy(sDc_log.sts      ,"0",sizeof(sDc_log.sts)-1);
           strcpy(g_pub_tx.brf,"��ѺƷת��");

           sDc_log.amt = sLn_mst.gage_amt;
           
           ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
           if(ret)
           {
               Dc_log_Debug(&sDc_log);
               vtcp_log("[%s][%d]���뽻����ˮ�����[%d]",__FILE__,__LINE__,ret);
               sprintf(acErrMsg,"���뽻����ˮ�����");
               WRITEMSG
               goto ErrExit;
           }
           g_pub_tx.tx_amt1=sDc_log.amt;
           ret = pub_ins_trace_log();  
           if (ret) {
               vtcp_log("[%s][%d]�Ǽǽ�����ˮ�����[%d]",__FILE__,__LINE__,ret);
               sprintf(acErrMsg,"������ˮ�����");
               WRITEMSG
               goto ErrExit;
           }
           iTrace_cnt++;
       }
       /*end by martin 2009/12/23 18:02:02*/
       vtcp_log("[%s][%d] ��ˮ��5=[%d]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
       memcpy(sLn_mst.opn_br_no,cKinbr,sizeof(sLn_mst.opn_br_no)-1);       
       ret=Ln_mst_Upd_Upd(sLn_mst,g_pub_tx.reply);
       if(ret)
       {
       	   vtcp_log("[%s][%d]���´����������[%d]",__FILE__,__LINE__,ret);
           sprintf(acErrMsg,"���´����������");
           WRITEMSG
           goto ErrExit;
       }
       iCount ++;
    }
    Ln_mst_Clo_Upd();
    
    {/* ���ӽ����ϴ���� add by martin 2009/8/20 16:15:38 */
       	    ret=sql_execute2( "update intrtmpreg set kinbr='%s' where actno='%s' and kinbr='%s' ",cKinbr,cAcno,cTxbrno);
            if(ret)
            {
             	  vtcp_log("[%s][%d]�����ϴ�����ϸ�����[%d]",__FILE__,__LINE__,ret);
            }
            vtcp_log("[%s][%d] �����ϴ���ret=[%d]",__FILE__,__LINE__,ret);
     }
    /**���´�����ϸ��**/
    vtcp_log("[%s][%d] sLn_mst.ac_id=[%d]\n",__FILE__,__LINE__,lAc_id);
    ret=sql_execute2( "update ln_mst_hst set opn_br_no='%s' where ac_id='%ld' and opn_br_no='%s' ",cKinbr,lAc_id,cTxbrno);
    if(ret)
    {
     	  vtcp_log("[%s][%d]���´�����ϸ�����[%d]",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"���´�����ϸ�����");
        WRITEMSG
        /*goto ErrExit;����ϸʱ ���ױ��� del by martin 2009/8/14 14:28:03*/
    }
    vtcp_log("[%s][%d] ��[%s]���Ѿ��ɹ���[%s]ת��[%s]����\n",__FILE__,__LINE__,cAcno,cTxbrno,cKinbr);
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
	  if(memcmp(g_pub_tx.reply,"0000",4)==0)
	  	  memcpy(g_pub_tx.reply,"HV01",4);
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}
