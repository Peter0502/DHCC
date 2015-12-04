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
insert into tx_flow_def values ('4713', 0, 'D120', '#=0001@5@#=0002@1@#=0003@7@#$');
insert into tx_flow_def values ('4713', 1, 'D201', '#=0000@2@#$');
insert into tx_flow_def values ('4713', 2, 'D200', '#=0000@3@#$');
insert into tx_flow_def values ('4713', 3, 'D099', '#=0000@4@#$');
insert into tx_flow_def values ('4713', 4, 'D214', '#$');
insert into tx_flow_def values ('4713', 5, 'D003', '#=0000@6@#$');
insert into tx_flow_def values ('4713', 6, 'd099', '#=0000@4@#$');
insert into tx_flow_def values ('4713', 7, 'D202', '#=0000@8@#$');
insert into tx_flow_def values ('4713', 8, 'D203', '#=0000@3@#$');
-------tx_sub_def �Ѿ��еľͲ���������-----------
insert into tx_sub_def values ('D120', '�ۻ�����', 'spD120', '0', '0');
-----�����⼸��tx_sub_defӦ������ D214 ����û�� ���ѯ���ݿ��ȷ��---
insert into tx_sub_def values ('D214','�ۻ�ҵ��д�Ǽǲ�','spD214','0','0');
insert into tx_sub_def values ('D201', '�����Ϣ', 'spD201', '1', '0');
insert into tx_sub_def values ('D200', '�����ӽ���', 'spD200', '1', '0');
insert into tx_sub_def values ('D099', '����ӽ���', 'spD099', '1', '0');
insert into tx_sub_def values ('D003', 'ȡ���ӽ���', 'spD003', '1', '0');
insert into tx_sub_def values ('d099', '��֤����תӦ����', 'spD099', '1', '0');

-----tx_sub_rel-----
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
insert into tx_sub_rel values ('4713', 'D202', '1', '2', '102K', 'ת��');
insert into tx_sub_rel values ('4713', 'D003', '1', '2', '102K', 'ת��');
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
    double dXh_amt = 0.00;/*������� ���� �������*/
    char cKh_type[2];/*�ۻ����� 1ֱ�ӿۻ� 2����ۻ�*/
    char cOut_ac_no[25];/*�ۿ��˺�*/
    char cIn_ac_no[25];/*�տ��˺�*/
    char cXh_flag[2];/*������־ 1 �� 0 ��*/
    struct mo_hold_c sMo_hold;
    struct mo_hold_c sMo_hold_tj;
    struct dd_mst_c sDd_mst;
    struct td_mst_c sTd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct td_parm_c sTd_parm;
    struct dd_parm_c sDd_parm;
    memset(cXh_flag,0,sizeof(cXh_flag));
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
    get_zd_data("1011",cIn_ac_no);
    vtcp_log("[%s][%d] �տ��˺ţ�[%s]",__FILE__,__LINE__,cIn_ac_no);
    get_zd_data("0700",cKh_type);
    vtcp_log("[%s][%d] �ۻ����ͣ�[%s]",__FILE__,__LINE__,cKh_type);
    get_zd_double("102F",&dKh_amt);
    vtcp_log("[%s][%d] �ۻ���[%f]",__FILE__,__LINE__,dKh_amt);
    get_zd_int("0480",&iHold_seqn);
    vtcp_log("[%s][%d] ������ţ�[%d]",__FILE__,__LINE__,iHold_seqn);
    get_zd_data("0690",cXh_flag);
    vtcp_log("[%s][%d] ������־��[%s]",__FILE__,__LINE__,cXh_flag);
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
    if(memcmp(sMdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1)!=0) /***ֻ�ܿ������ۻ�***/
    {
        sprintf(acErrMsg,"���������뽻�׻�������[%s]", sMdm_ac_rel.opn_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D245");
        goto ErrExit;
    }
    if(sMdm_ac_rel.note_sts[0] != '0' && sMdm_ac_rel.note_sts[0] != '1')
    {
        sprintf(acErrMsg,"�ý���״̬������������");
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
        /*����˺ų��ֲ���ֹ����ȫ�����ᡢֻ������ͬʱ���֣�����ȫ��ֹ������������ͬʱ���֣������������ǲ���������������
        ����û��ֹ������������������
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
        */
        if((sTd_mst.hold_sts[0] == '1' || sTd_mst.hold_sts[0] == '2') && cKh_type[0] == '1')
        {
            sprintf(acErrMsg,"�˺ű�ȫ���������ֻ��������������ֱ�ӿۻ������飡");
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
        dXh_amt = sTd_mst.bal;
        /**
        dKy_amt = sTd_mst.bal - sTd_mst.hold_amt - sTd_mst.zf_amt - sTd_mst.ctl_amt -sTd_parm.min_bal; 
        **/
        dKy_amt = sTd_mst.bal - sTd_mst.hold_amt  - sTd_mst.ctl_amt -sTd_parm.min_bal;          
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
        /*����˺ų��ֲ���ֹ����ȫ�����ᡢֻ������ͬʱ���֣�����ȫ��ֹ������������ͬʱ���֣������������ǲ���������������
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
        */
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
        dXh_amt = sDd_mst.bal;
        /**
        dKy_amt = sDd_mst.bal - sDd_mst.hold_amt - sDd_mst.zf_amt - sDd_mst.ctl_amt -sDd_parm.min_bal;
        **/
        dKy_amt = sDd_mst.bal - sDd_mst.hold_amt  - sDd_mst.ctl_amt -sDd_parm.min_bal;
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
        if(cXh_flag[0] == '1')
        {
            /*ѡ��ֱ�ӿۻ�������ʱ����������������Ķ��ᡢֹ��*/
            if(sPrdt_ac_id.ac_id_type[0] == '2')
            {
                /*������ֹ��
                if(sTd_mst.hold_sts[0] != '0' || sTd_mst.zf_sts[0] != '0')
                */
                if(sTd_mst.hold_sts[0] != '0')
                {
                    sprintf(acErrMsg,"���˻��Ѿ����������ֹ��������ֱ�ӿۻ�����������");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH16");
                    goto ErrExit;
                }
                if(pub_base_CompDblVal(sTd_mst.ctl_amt,0.005) > 0)
                {
                    sprintf(acErrMsg,"���˻����ڿ��ƽ�������ۻ�����������������");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH18");
                    goto ErrExit;
                }
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '1')
            {
                /*������ֹ��
                if(sDd_mst.hold_sts[0] != '0' || sDd_mst.zf_sts[0] != '0')
                */
                if(sDd_mst.hold_sts[0] != '0')
                {
                    sprintf(acErrMsg,"���˻��Ѿ����������ֹ��������ֱ�ӿۻ�����������");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH16");
                    goto ErrExit;
                }
                if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.005) > 0)
                {
                    sprintf(acErrMsg,"���˻����ڿ��ƽ�������ۻ�����������������");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH18");
                    goto ErrExit;
                }
            }
            else
            {
                sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O238");
                goto ErrExit;
            }
            strcpy(g_pub_tx.reply,"0003");
            vtcp_log("[%s][%d] ��ʼ������g_pub_tx.reply = [%s]",__FILE__,__LINE__,g_pub_tx.reply);
        }
        else if(cXh_flag[0] == '0')
        {
            if(pub_base_CompDblVal(dKh_amt,dKy_amt) > 0)
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
        else
        {
            sprintf(acErrMsg,"������־����[%c]",cXh_flag[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH15");
            goto ErrExit;
        }
        
    }
    else if(cKh_type[0] == '2')/*����ۻ�*/
    {
        ret=Mo_hold_Dec_Upd(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%d and hold_seqn= %d ",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
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
        /*���Ҫ����������������������Ķ����ֹ��*/
        if(cXh_flag[0] == '1')
        {
            ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn =%d and hold_seqn <> %d and hold_sts='0'",\
                            sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if(ret > 0)
            {
                sprintf(acErrMsg,"���˻��������������ֹ����������ۻ�����������");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH16");
                goto ErrExit;
            }
            else if(ret<0)
            {
                sprintf(acErrMsg,"���Ҷ����¼ʧ�ܣ�");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH04");
                goto ErrExit;
            }
            /*ѡ������ʱ����������ڿ��ƽ��*/
            if(sPrdt_ac_id.ac_id_type[0] == '2')
            {
                if(pub_base_CompDblVal(sTd_mst.ctl_amt,0.005) > 0)
                {
                    sprintf(acErrMsg,"���˻����ڿ��ƽ�������ۻ�����������������");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH18");
                    goto ErrExit;
                }
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '1')
            {
                if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.005) > 0)
                {
                    sprintf(acErrMsg,"���˻����ڿ��ƽ�������ۻ�����������������");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH18");
                    goto ErrExit;
                }
            }
            else
            {
                sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O238");
                goto ErrExit;
            }
            /*�޸Ķ����¼    
            sMo_hold.kh_sum_amt += dXh_amt;
            sMo_hold.kh_sum_cnt ++;
            */
            sMo_hold.hold_sts[0] == '1'; /*������ �ͽ���ñʶ��� ����Ҳû����*/
            strcpy(g_pub_tx.reply,"0003");
        }
        else if(cXh_flag[0] == '0')
        {
            if(sMo_hold.hold_typ[0] == '3')/*���ֶ���ۻ�*/
            {
                /*�˱ʶ���ʣ��ɿۻ���� = �ƻ���� - �ѿۻ����
                dSy_kh_amt = sMo_hold.plan_hold_amt - sMo_hold.kh_sum_amt;
                
                if(pub_base_CompDblVal(dKh_amt,dSy_kh_amt) > 0 )
                */
                if(pub_base_CompDblVal(dKh_amt,sMo_hold.plan_hold_amt) != 0 )
                {
                    Mo_hold_Clo_Upd();
                    sprintf(acErrMsg,"�ۻ����[%f]��ñʲ��ֶ�����[%f]����",dKh_amt,dSy_kh_amt);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"KH19");
                    goto ErrExit;
                }
#if 0
                ret = Mo_hold_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and hold_seqn < %d and \
                hold_typ ='3'  and hold_sts ='0'",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
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
                     dSum_plan_hold_amt += sMo_hold_tj.plan_hold_amt;
                 }
                vtcp_log("[%s][%d],dSum_plan_hold_amt=[%lf] dSum_kh_sum_amt=[%lf]",__FILE__,__LINE__,dSum_plan_hold_amt,dSum_kh_sum_amt);
#endif 0
                /*����˱ʶ���ۻ��Ŀ��ý����۵Ļ���ʼ�ۻ�*/
                if(sPrdt_ac_id.ac_id_type[0] == '2')/*����*/
                {
                    dMax_ky_amt =  dKy_amt + sMo_hold.plan_hold_amt;
                    if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) > 0)
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
                    dMax_ky_amt =  dKy_amt + sMo_hold.plan_hold_amt;
                    if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) > 0)
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
                vtcp_log("[%s][%d],dKy_amt=[%lf] dMax_ky_amt=[%lf],plan_hold_amt=[%f]",\
                            __FILE__,__LINE__,dKy_amt,dMax_ky_amt,sMo_hold.plan_hold_amt);
                /*�޸Ķ����¼   
                sMo_hold.kh_sum_amt += dKh_amt;
                sMo_hold.kh_sum_cnt ++;
                */
                /*�ѿۻ�����Ϊ�Զ��ⶳ�� ���ⶳ�ᴦ��*/
                if(sMo_hold.auto_unhold_ind[0] == 'Y')
                {
                    sMo_hold.hold_sts[0] = '1';/*������*/
                    /*������˺�ֻ����һ�ʲ��ֶ��ᣬ���������״̬hold_sts = 0 ����*/
                    ret = sql_count("mo_hold", "ac_id= %ld and ac_seqn=%d and hold_seqn <> %d  and hold_sts='0'",\
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
                if(pub_base_CompDblVal(dKh_amt,dKy_amt) > 0)
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
                /*�޸Ķ����¼   
                sMo_hold.kh_sum_amt += dKh_amt;
                sMo_hold.kh_sum_cnt ++;  
                */         
            }
            else
            {
                Mo_hold_Clo_Upd();
                sprintf(acErrMsg,"�������ʹ�[%c]",sMo_hold.hold_typ[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH11");
                goto ErrExit;
            }
        }
        else
        {
            sprintf(acErrMsg,"������־����[%c]",cXh_flag[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH15");
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
    /*�Ǽǽ�����ˮ*/
    strcpy(g_pub_tx.ac_no,cOut_ac_no);
    strcpy(g_pub_tx.brf,"�ۻ�ҵ��");/*�˱�ע����Ķ�*/
    ret=pub_ins_trace_log();
    if(ret)
    {
        vtcp_log("[%s][%d]�Ǽǽ�����ˮ����[%d]",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"[%d]�Ǽǽ�����ˮ����",__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"D103");
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
