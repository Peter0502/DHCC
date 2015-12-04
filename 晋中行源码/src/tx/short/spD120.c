/*********************************************************************
* �� �� �� :  V1.0
* �������� :  ������
* ��    �� :  2011-09-14
* ����ģ�� :  �ۻ�����
* �������� :  spD120.c
* �������� :  ����ǰ̨����Ŀۻ����ͣ�������ţ��ۻ��˺ţ��տ��˺ţ���ɿۻ�������
* �����б� :
* ʹ��ע�� : �������spD214.c����ʹ�á�
* �޸ļ�¼ :
*     �޸����� :
*     �޸�ʱ�� :
*     �޸����� :
�������ã�
insert into tx_def values ('4713', '�ۻ�����', '10000000000010000000000000000000000000111110000000000000000000000001000000000000000000000000000000000100000000000000000000000100', '1', '3', '2', '3', null);

--------tx_flow_def ----------
insert into tx_flow_def values ('4713', 0, 'D120', '#=0001@5@#=0002@1@#$');
insert into tx_flow_def values ('4713', 1, 'D201', '#=0000@2@#$');
insert into tx_flow_def values ('4713', 2, 'D200', '#=0000@3@#$');
insert into tx_flow_def values ('4713', 3, 'D099', '#=0000@4@#$');
insert into tx_flow_def values ('4713', 4, 'D214', '#$');
insert into tx_flow_def values ('4713', 5, 'D003', '#=0000@6@#$');
insert into tx_flow_def values ('4713', 6, 'd099', '#=0000@4@#$');

-------tx_sub_def �Ѿ��еľͲ���������-----------
insert into tx_sub_def values ('D120', '�ۻ�����', 'spD120', '0', '0');
-----�����⼸��tx_sub_defӦ������---
insert into tx_sub_def values ('D201', '�����Ϣ', 'spD201', '1', '0');
insert into tx_sub_def values ('D200', '�����ӽ���', 'spD200', '1', '0');
insert into tx_sub_def values ('D099', '����ӽ���', 'spD099', '1', '0');
insert into tx_sub_def values ('D003', 'ȡ���ӽ���', 'spD003', '1', '0');
insert into tx_sub_def values ('d099', '��֤����תӦ����', 'spD099', '1', '0');

-----tx_sub_rel-----
insert into tx_sub_rel values ('4713', 'D003', '1', '1', '0191', '����0190');
insert into tx_sub_rel values ('4713', 'D003', '1', '1001', '0192', '���Ʊ�־');
insert into tx_sub_rel values ('4713', 'D003', '1', '1001', '0193', '��ͨ��ȡ');
insert into tx_sub_rel values ('4713', 'D099', '0', '102J', '101A', '����');
insert into tx_sub_rel values ('4713', 'D099', '0', '102S+102P-102Q', '1013', '���');
insert into tx_sub_rel values ('4713', 'D099', '1', '1', '0191', '����0190');
insert into tx_sub_rel values ('4713', 'D099', '1', '1001', '0193', '��ͨ��ȡ');
insert into tx_sub_rel values ('4713', 'D099', '1', '0001', '0192', '���Ʊ�־');
insert into tx_sub_rel values ('4713', 'd099', '1', '1', '0191', '����0190');
insert into tx_sub_rel values ('4713', 'd099', '1', '0001', '0192', '���Ʊ�־');
insert into tx_sub_rel values ('4713', 'd099', '1', '1001', '0193', '��ͨ��ȡ');
insert into tx_sub_rel values ('4713', 'd099', '0', '102J', '101A', '����');
insert into tx_sub_rel values ('4713', 'd099', '0', '102S', '1013', '���');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_hold_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "td_parm_c.h"
#include "dd_parm_c.h"
int spD120()
 {
    int ret = 0;
    int iHold_seqn = 0;/*�������*/
    int iAc_seqn = 0;/*�˺����*/
    double dSum_plan_hold_amt = 0.00;/*���ֶ���֮ǰ������֮��*/
    double dSum_kh_sum_amt = 0.00;/*���ֶ���֮ǰ�ѿۻ����֮��*/
    double dSy_kh_amt = 0.00; /*���ֶ���ƻ�ʣ����*/
    double dMax_ky_amt = 0.00;/*���ֶ���ۻ��Ŀ������*/
    double dKh_amt = 0.00;/*�ۻ����*/
    double dKy_amt = 0.00;/*���ý��*/
    char cKh_type[2];/*�ۻ����� 1ֱ�ӿۻ� 2����ۻ�*/
    char cOut_ac_no[25];/*�ۿ��˺�*/
    char cIn_ac_no[25];/*�տ��˺�*/
    
    struct mo_hold_c sMo_hold;
    struct mo_hold_c sMo_hold_tj;
    struct dd_mst_c sDd_mst;
    struct td_mst_c sTd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct td_parm_c sTd_parm;
    struct dd_parm_c sDd_parm;
    memset(cKh_type,0,sizeof(cKh_type));
    memset(cOut_ac_no,0,sizeof(cOut_ac_no));
    memset(cIn_ac_no,0,sizeof(cIn_ac_no));
    memset(&sMo_hold,0x0,sizeof(struct mo_hold_c));
    memset(&sMo_hold_tj,0x0,sizeof(struct mo_hold_c));
    memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&sTd_mst,0x0,sizeof(struct td_mst_c));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x0,sizeof(struct prdt_ac_id_c));
    memset(&sTd_parm,0x0,sizeof(struct td_parm_c));
    memset(&sDd_parm,0x0,sizeof(struct dd_parm_c));
    /*��ʼ��*/
    ret=pub_base_sysinit();
    if(ret)
    {
         sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
         WRITEMSG
         goto ErrExit;
    }
    /*ȡֵ*/
    
    get_zd_data("1021",cOut_ac_no);
    vtcp_log("[%s][%d] �ۿ��˺ţ�[%s]",__FILE__,__LINE__,cOut_ac_no);
    get_zd_int("1022",&iAc_seqn);
    vtcp_log("[%s][%d] �˺���ţ�[%d]",__FILE__,__LINE__,iAc_seqn);
    get_zd_data("0300",cIn_ac_no);
    vtcp_log("[%s][%d] �տ��˺ţ�[%s]",__FILE__,__LINE__,cIn_ac_no);
    get_zd_data("0700",cKh_type);
    vtcp_log("[%s][%d] �ۻ����ͣ�[%s]",__FILE__,__LINE__,cKh_type);
    get_zd_double("102F",&dKh_amt);
    vtcp_log("[%s][%d] �ۻ���[%f]",__FILE__,__LINE__,dKh_amt);
    get_zd_int("0480",&iHold_seqn);
    vtcp_log("[%s][%d] ������ţ�[%d]",__FILE__,__LINE__,iHold_seqn);

    /*�����˺���ac_id*/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cOut_ac_no);
    if(ret !=0 && ret !=100)
    {
         sprintf(acErrMsg,"������˻���ϵ�����");
         WRITEMSG
         strcpy(g_pub_tx.reply,"L015");
         goto ErrExit;
    }
    else if(ret == 100)
    {
         sprintf(acErrMsg,"�޴��˺�");
         WRITEMSG
         strcpy(g_pub_tx.reply,"LS53");
         goto ErrExit;
    }
    if(sMdm_ac_rel.note_sts[0] != '0' && sMdm_ac_rel.note_sts[0] != '1')
    {
        sprintf(acErrMsg,"�ý���״̬������");
        WRITEMSG
        strcpy(g_pub_tx.reply,"M013");
        goto ErrExit;
    }
    /*����ac_id ��ac_seqn���˺�����*/
    ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
    if(ret)
    {
         sprintf(acErrMsg,"���Ҳ�Ʒ�˺Ŷ��ձ����");
         WRITEMSG
         strcpy(g_pub_tx.reply,"W013");
         goto ErrExit;
    }
    if(sPrdt_ac_id.ac_id_type[0] == '2') /*����*/
    {
        ret=Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id =%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if(ret)
        {
            sprintf(acErrMsg,"���Ҷ������ļ�ʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"X004");
            goto ErrExit;
        }
        /*����˺ŵ�״̬*/
        if(sTd_mst.ac_sts[0] != '1')
        {
            sprintf(acErrMsg,"�˻���״̬������������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L016");
            goto ErrExit;
        }
        /*����˺ų��ֲ���ֹ����ȫ�����ᡢֻ������ͬʱ���֣�����ȫ��ֹ������������ͬʱ���֣������������ǲ���������������*/
        if((sTd_mst.zf_sts[0] == '3' && (sTd_mst.hold_sts[0] == '1' || sTd_mst.hold_sts[0] == '2'))\
            || (sTd_mst.zf_sts[0] == '1' && sTd_mst.hold_sts[0] != '0' ))
        {
            sprintf(acErrMsg,"�˻��Ķ����ֹ��״̬�ǳ�������������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH01");
            goto ErrExit;
            
        }
        if(sTd_mst.zf_sts[0] == '1')
        {
            sprintf(acErrMsg,"�˻���ȫ��ֹ�������飡");
            WRITEMSG
            strcpy(g_pub_tx.reply,"SN32");
            goto ErrExit;
        }
        if((sTd_mst.hold_sts[0] == '1' || sTd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
        {
            sprintf(acErrMsg,"�˺ű�ȫ���������ֻ��������������ۻ������飡");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH02");
            goto ErrExit;
        }
        ret=Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
        if(ret)
        {
            sprintf(acErrMsg,"���Ҷ��ڲ�Ʒ������ʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"W027");
            goto ErrExit;
        }
        /*������ý��*/
        dKy_amt = sTd_mst.bal - sTd_mst.hold_amt - sTd_mst.zf_amt - sTd_mst.ctl_amt -sTd_parm.min_bal;          
    }
    else if(sPrdt_ac_id.ac_id_type[0] == '1') /*����*/
    {
        ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id =%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if(ret)
        {
            sprintf(acErrMsg,"���һ������ļ�ʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"X003");
            goto ErrExit;
        }
        /*����˺ŵ�״̬*/
        if(sDd_mst.ac_sts[0] != '1')
        {
            sprintf(acErrMsg,"�˻���״̬������������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L016");
            goto ErrExit;
        }
        /*����˺ų��ֲ���ֹ����ȫ�����ᡢֻ������ͬʱ���֣�����ȫ��ֹ������������ͬʱ���֣������������ǲ���������������*/
        if((sDd_mst.zf_sts[0] == '3' && (sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'))\
            || (sDd_mst.zf_sts[0] == '1' && sDd_mst.hold_sts[0] != '0' ))
        {
            sprintf(acErrMsg,"�˻��Ķ����ֹ��״̬�ǳ�������������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH01");
            goto ErrExit;
            
        }
        if(sDd_mst.zf_sts[0] == '1')
        {
            sprintf(acErrMsg,"�˻���ȫ��ֹ��������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"SN32");
            goto ErrExit;
        }
        if((sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
        {
            sprintf(acErrMsg,"�˻���ȫ���������ֻ��������������ۻ�������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH02");
            goto ErrExit;
        }
        ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
        if(ret)
        {
            sprintf(acErrMsg,"���һ��ڲ�Ʒ������ʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"W027");
            goto ErrExit;
        }
        /*������ý��*/
        dKy_amt = sDd_mst.bal - sDd_mst.hold_amt - sDd_mst.zf_amt - sDd_mst.ctl_amt -sDd_parm.min_bal;
    }
    else
    {
        sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O238");
        goto ErrExit;
    }
    /*�ۻ���Ϊֱ�ӿۻ��Ͷ���ۻ��������*/
    if(cKh_type[0] == '1')/*ֱ�ӿۻ�*/
    {
        if(pub_base_CompDblVal(dKh_amt,dKy_amt) == 1)
        {
            sprintf(acErrMsg,"�ۻ����[%f]���ڿ��ý��[%f]",dKh_amt,dKy_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH03");
            goto ErrExit;
        }
        /*��ʼ�ۻ�*/
        if(sPrdt_ac_id.ac_id_type[0] == '2')
        {
            strcpy(g_pub_tx.reply,"0002");
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            strcpy(g_pub_tx.reply,"0001");
        }
        else
        {
            sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"O238");
            goto ErrExit;
        }
    }
    else if(cKh_type[0] == '2')/*����ۻ�*/
    {
        ret=Mo_hold_Dec_Upd(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%d and hold_seqn= %d and hold_stop ='1'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
        if(ret)
        {
            sprintf(acErrMsg,"Mo_hold_dec_Upd����");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH04");
            goto ErrExit;
        }
        ret=Mo_hold_Fet_Upd(&sMo_hold,g_pub_tx.reply);
        if(ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"Mo_hold_Fet_Upd ����");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH04");
            goto ErrExit;
        }
        else if(ret == 100)
        {
            Mo_hold_Clo_Upd();
            sprintf(acErrMsg,"�޴˶������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH05");
            goto ErrExit;
        }
        if(sMo_hold.hold_sts[0] != '0')
        {
            Mo_hold_Clo_Upd();
            sprintf(acErrMsg,"�ñʶ����ѽ�������ܿۻ���sMo_hold.hold_sts=[%c]",sMo_hold.hold_sts[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH06");
            goto ErrExit;
        }
        
        /*�жϸñʲ��ֶ����Ƿ���Ч,���˱ʶ����¼֮ǰ�Ƿ����ȫ�����ᣬֻ��������ȫ��ֹ��*/
        ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn =%d and hold_seqn < %d and hold_typ in('1','2','4')\
         and hold_sts='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
        if(ret > 0)
        {
            sprintf(acErrMsg,"�ñʶ���δ��Ч��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH07");
            goto ErrExit;
        }
        else if(ret<0)
        {
            sprintf(acErrMsg,"���Ҷ����¼ʧ�ܣ�");
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH04");
            goto ErrExit;
        }
        /*�ñ���Ч����ʼ����,��Ϊ���ֶ���ۻ���ȫ�����ᡢֻ�������ۻ��������*/
        if(sMo_hold.hold_typ[0] == '3')/*���ֶ���ۻ�*/
        {
            /*�˱ʶ���ʣ��ɿۻ���� = �ƻ���� - �ѿۻ����*/
            dSy_kh_amt = sMo_hold.plan_hold_amt - sMo_hold.kh_sum_amt;
            if(pub_base_CompDblVal(dKh_amt,dSy_kh_amt) == 1)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"�ۻ����[%f]���ڴ˱ʶ���ʣ��ɿۻ����[%f]",dKh_amt,dSy_kh_amt);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH08");
                goto ErrExit;
            }
            /*�˱ʶ���ۻ����ý�� = bal - zf_amt - min_bal - ctl_amt - \
            (�˱ʲ��ֶ���֮ǰ����δ����Ĳ��ֶ����� - �˱ʲ��ֶ���֮ǰ����δ����Ĳ��ֶ����ѿۻ����)*/
            /*******
            ret=sql_sum_double( "mo_hold","plan_hold_amt",&dSum_plan_hold_amt,"ac_id=%ld and ac_seqn=%d and hold_seqn < %d and \
            hold_typ ='3'  and hold_stop ='1' and hold_sts ='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if(ret)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"���㶳�������");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH09");
                goto ErrExit;
            }
            ret=sql_sum_double( "mo_hold","kh_sum_amt",&dSum_kh_sum_amt,"ac_id=%ld and ac_seqn =%d and hold_seqn < %d \
            and hold_typ ='3' and hold_stop ='1'and hold_sts ='0' ",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if(ret)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"�����ѿۻ�������");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH09");
                goto ErrExit;
            }
            **************/
             ret = Mo_hold_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and hold_seqn < %d and \
            hold_typ ='3'  and hold_stop ='1' and hold_sts ='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
             if(ret)
             {
                 sprintf(acErrMsg,"��ѯ����Ǽǲ�����");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"KH04");
                 goto ErrExit;
             }
             while(1)
             {
                 memset(&sMo_hold_tj,0x00,sizeof(struct mo_hold_c));
                 ret = Mo_hold_Fet_Sel(&sMo_hold_tj,g_pub_tx.reply);
                 if(ret != 0 && ret != 100)
                 {
                     sprintf(acErrMsg,"��ѯmo_hold����!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"KH04");
                     goto ErrExit;
                 }
                 else if(ret == 100)
                 {
                     break;
                 }
                 /*����Ǽƻ��ۻ��ģ����ۼ�*/
                 if(memcmp(sMo_hold_tj.hold_knd,"JHKH",4) == 0)
                 {
                     vtcp_log("[%][%s] �ƻ��ۻ��Ķ��᲻�ۼ� �ƻ�������� �ѿۻ����",__FILE__,__LINE__);
                     continue;
                 }
                 dSum_plan_hold_amt += sMo_hold_tj.plan_hold_amt;
                 dSum_kh_sum_amt += sMo_hold_tj.kh_sum_amt;
             }
            vtcp_log("[%s][%d],dSum_plan_hold_amt=[%lf] dSum_kh_sum_amt=[%lf]",__FILE__,__LINE__,dSum_plan_hold_amt,dSum_kh_sum_amt);
            /*����˱ʶ���ۻ��Ŀ��ý����۵Ļ���ʼ�ۻ�*/
            if(sPrdt_ac_id.ac_id_type[0] == '2')/*����*/
            {
                dMax_ky_amt =  sTd_mst.bal - sTd_mst.zf_amt - sTd_mst.ctl_amt -sTd_parm.min_bal - (dSum_plan_hold_amt -dSum_kh_sum_amt);
                if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) == 1)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"�ۻ����[%f]���ڴ˱ʶ�����ý��[%f]",dKh_amt,dMax_ky_amt);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH10");
                    goto ErrExit;
                }
                
                ret=sql_execute("update td_mst set hold_amt =%f   where ac_id = %ld and ac_seqn=%d",sTd_mst.hold_amt-dKh_amt,sMdm_ac_rel.ac_id,iAc_seqn);
                if(ret)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"����td_mst ʧ�ܣ�");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O013");
                    goto ErrExit;
                }
                strcpy(g_pub_tx.reply,"0002");            
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '1')/*����*/
            {
                dMax_ky_amt =  sDd_mst.bal - sDd_mst.zf_amt - sDd_mst.ctl_amt -sDd_parm.min_bal - (dSum_plan_hold_amt -dSum_kh_sum_amt);
                if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) == 1)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"�ۻ����[%f]���ڴ˱ʶ�����ý��[%f]",dKh_amt,dMax_ky_amt);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH10");
                    goto ErrExit;
                }
                
                ret=sql_execute("update dd_mst set hold_amt = %f where ac_id = %ld and ac_seqn=%d",sDd_mst.hold_amt-dKh_amt,sMdm_ac_rel.ac_id,iAc_seqn);
                if(ret)
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"����dd_mst ʧ�ܣ�");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O012");
                    goto ErrExit;
                }
                strcpy(g_pub_tx.reply,"0001");            
            }
            else
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O238");
                goto ErrExit;
            }
            /*�޸Ķ����¼*/    
            sMo_hold.kh_sum_amt += dKh_amt;
            sMo_hold.kh_sum_cnt ++;
            /*�жϸñʶ����Ƿ���ȫ���ۻ���,�ѿۻ�����Ϊ�Զ��ⶳ�� ���ⶳ�ᴦ��*/
            if(pub_base_CompDblVal(sMo_hold.kh_sum_amt,sMo_hold.plan_hold_amt) == 0 && sMo_hold.auto_unhold_ind[0] == 'Y')
            {
                sMo_hold.hold_sts[0] = '1';/*������*/
                /*������˺�ֻ����һ�ʲ��ֶ��ᣬ���������״̬hold_sts = 0 ����*/
                ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn=%d and hold_seqn <> %d and hold_stop='1' and hold_sts='0'",\
                    sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
                if (ret == 0)
                {
                    if(sPrdt_ac_id.ac_id_type[0] == '2')/*����*/
                    {
                        ret=sql_execute("update td_mst set hold_sts ='0'  where ac_id = %ld and ac_seqn=%d ",\
                        sMdm_ac_rel.ac_id,iAc_seqn);
                        if(ret)
                        {
                            Mo_hold_Clo_Upd();
                            sprintf(acErrMsg,"����td_mst ʧ�ܣ�");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"O013");
                            goto ErrExit;
                        }            
                    }
                    else if(sPrdt_ac_id.ac_id_type[0] == '1')/*����*/
                    {
                        ret=sql_execute("update dd_mst set hold_sts ='0' where ac_id = %ld and ac_seqn=%d ",\
                        sMdm_ac_rel.ac_id,iAc_seqn);
                        if(ret)
                        {
                            Mo_hold_Clo_Upd();
                            sprintf(acErrMsg,"����dd_mst ʧ�ܣ�");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"O012");
                            goto ErrExit;
                        }      
                    }
                    else
                   {
                       Mo_hold_Clo_Upd();
                       sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
                       WRITEMSG
                       strcpy(g_pub_tx.reply,"O238");
                       goto ErrExit;
                   }
                } 
                else if (ret < 0) 
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg, "��ѯ����Ǽǲ�����");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "KH04");
                    goto ErrExit;
                }
                    
            }                               
        }
        else if(sMo_hold.hold_typ[0] == '1' || sMo_hold.hold_typ[0] == '2')/*ȫ���������ֻ�������ۻ�*/
        {
            if(pub_base_CompDblVal(dKh_amt,dKy_amt) == 1)
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"�ۻ����[%f]���ڿ��ý��[%f]",dKh_amt,dKy_amt);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH03");
                goto ErrExit;
            }
            /*��ʼ�ۻ�*/
            if(sPrdt_ac_id.ac_id_type[0] == '2')/*����*/
            {
                strcpy(g_pub_tx.reply,"0002");            
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '1')/*����*/
            {
                strcpy(g_pub_tx.reply,"0001");            
            }
            else
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O238");
                goto ErrExit;
            }
            /*�޸Ķ����¼*/    
            sMo_hold.kh_sum_amt += dKh_amt;
            sMo_hold.kh_sum_cnt ++;          
        }
        else
        {
            Mo_hold_Clo_Upd();
            sprintf(acErrMsg,"�������ʹ�[%c]",sMo_hold.hold_typ[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH11");
            goto ErrExit;
        }
        ret=Mo_hold_Upd_Upd(sMo_hold,g_pub_tx.reply);
        if(ret)
       {
           Mo_hold_Clo_Upd();
           sprintf(acErrMsg,"����mo_hold ʧ�ܣ�");
           WRITEMSG
           strcpy(g_pub_tx.reply,"O021");
           goto ErrExit;
       }
        Mo_hold_Clo_Upd();
    }
    else
    {
        sprintf(acErrMsg,"�ۻ����ʹ�[%c]",cKh_type[0]);
        WRITEMSG
        strcpy(g_pub_tx.reply,"KH12");
        goto ErrExit;
    }   
  OkExit:
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
