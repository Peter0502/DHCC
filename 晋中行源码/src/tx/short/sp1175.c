/*********************************************************************
* �� �� �� :  V1.0
* �������� :  ������
* ��    �� :  2011-09-19
* ����ģ�� :  �жϸñʿۻ��Ƿ���ִ��
* �������� :  sp1175.c
* �������� :
* �����б� :
* ʹ��ע�� :
* �޸ļ�¼ :
*     �޸����� :
*     �޸�ʱ�� :
*     �޸����� :
�������ã�
insert into tx_def values ('1175', '�ۻ���ѯ', '10000000000000000001111010000001000000101110000000010000000001000011111110000000100000010011100101100100000000000000000100000100', '0', '4', '2', '3', null);
insert into tx_flow_def values ('1175', 0, '1175', '#$');
insert into tx_sub_def values ('1175', '�˺Ų�ѯ', 'sp1175', '0', '0');
***********************************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_ac_rel_c.h"
#include "mo_hold_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"

int sp1175()
{
        int  ret = 0;
        int  iAc_seqn = 0; /*�˺����*/
        int  iHold_seqn = 0;/*�������*/
        double dKh_amt =0.00;/*�ۻ����*/
        double dSy_kh_amt = 0.00;/*�ñʶ�������ѿۻ����*/
        double dSum_plan_hold_amt = 0.00;/*���ֶ���֮ǰ������֮��*/
        double dSum_kh_sum_amt = 0.00;/*���ֶ���֮ǰ�ѿۻ����֮��*/
        double dMax_ky_amt = 0.00;/*���ֶ���ۻ��Ŀ������*/
        double dKy_amt = 0.00;/*���ý��*/
        double dXh_amt = 0.00;/*������� �����������*/
        char cKh_type[2];/*�ۻ�����*/
        char cXh_flag[2];/*������־ 1 �� 0 ��*/
        struct prdt_ac_id_c sPrdt_ac_id;
        struct mdm_ac_rel_c sMdm_ac_rel;
        struct mo_hold_c    sMo_hold;
        struct mo_hold_c    sMo_hold_tj;
        struct dd_mst_c sDd_mst;
        struct td_mst_c sTd_mst;
        struct dd_parm_c sDd_parm;
        struct td_parm_c sTd_parm;
        memset(cKh_type,0x00,sizeof(cKh_type));
        memset(cXh_flag,0x00,sizeof(cXh_flag));
        memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
        memset(&sTd_mst, 0x00, sizeof(struct td_mst_c));
        memset(&sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
        memset(&sPrdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
        memset(&sDd_parm, 0x00, sizeof(struct dd_parm_c));
        memset(&sTd_parm, 0x00, sizeof(struct td_parm_c));        
        memset(&sMo_hold, 0x00, sizeof(struct mo_hold_c));
        memset(&sMo_hold_tj, 0x00, sizeof(struct mo_hold_c));
        /*��ʼ��*/
        ret=pub_base_sysinit();
        if(ret)
        {
             sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
             WRITEMSG
             goto ErrExit;
        }
        get_zd_data("0690",cXh_flag);
        get_zd_data("0700",cKh_type);
        get_zd_data("1021", sMdm_ac_rel.ac_no);
        get_zd_int("1022", &iAc_seqn);
        get_zd_double("102F",&dKh_amt);
        get_zd_int("0480",&iHold_seqn);
        pub_base_old_acno(sMdm_ac_rel.ac_no);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel,"ac_no='%s'",sMdm_ac_rel.ac_no);
        if (ret != 0 && ret != 100)
        {
             sprintf(acErrMsg, "��ѯmdm_ac_rel����!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W011");
             goto ErrExit;
        }
        else if(ret == 100)
        {
             sprintf(acErrMsg, "�޴��˺�!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W010");
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
            sprintf(acErrMsg,"�ý���״̬������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M013");
            goto ErrExit;
        }

        
        ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if (ret)
        {
             sprintf(acErrMsg, "��ѯprdt_ac_rel����!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W013");
             goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')/*����*/
        {
            ret = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
            if (ret)
            {
                sprintf(acErrMsg, "��ѯdd_mst����!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "K107");
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
            ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
            if (ret)
            {
                sprintf(acErrMsg, "��ѯdd_parm����!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W015");
                goto ErrExit;
            }
            dXh_amt = sDd_mst.bal;
            dKy_amt = sDd_mst.bal - sDd_mst.hold_amt  - sDd_mst.ctl_amt -sDd_parm.min_bal;
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '2')/*����*/
        {
            ret = Td_mst_Sel(g_pub_tx.reply, &sTd_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
            if (ret)
            {
                sprintf(acErrMsg, "��ѯtd_mst����!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "K108");
                goto ErrExit;
            }
            if(sTd_mst.ac_sts[0] != '1')
            {
                sprintf(acErrMsg,"�˻���״̬������������");
                WRITEMSG
                strcpy(g_pub_tx.reply,"L016");
                goto ErrExit;
            }
            /*����˺ų��ֲ���ֹ����ȫ�����ᡢֻ������ͬʱ���֣�����ȫ��ֹ������������ͬʱ���֣������������ǲ���������������
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
                sprintf(acErrMsg,"�˺ű�ȫ���������ֻ��������������ۻ������飡");
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH02");
                goto ErrExit;
            }            
            ret = Td_parm_Sel(g_pub_tx.reply, &sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
            if (ret)
            {
                sprintf(acErrMsg, "��ѯtd_parm����!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W017");
                goto ErrExit;
            }
            dXh_amt = sTd_mst.bal;
            dKy_amt = sTd_mst.bal - sTd_mst.hold_amt  - sTd_mst.ctl_amt -sTd_parm.min_bal; 
        }
        else
        {
            sprintf(acErrMsg, "�˻��Ƕ��ڻ� ���� !! [%c]",sPrdt_ac_id.ac_id_type[0] );
            WRITEMSG
            strcpy(g_pub_tx.reply, "O238");
            goto ErrExit;
        }
        if(cKh_type[0] == '1')
        {
            if(cXh_flag[0] == '1')
            {
                /*ѡ��ֱ�ӿۻ�������ʱ����������������Ķ��ᡢֹ��*/
                if(sPrdt_ac_id.ac_id_type[0] == '2')
                {
                    if(sTd_mst.hold_sts[0] != '0' )
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
                    if(sDd_mst.hold_sts[0] != '0' )
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
            }
            else
            {
                sprintf(acErrMsg,"������־����[%c]",cXh_flag[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH15");
                goto ErrExit;
            }
        }
        else if(cKh_type[0] == '2')
        {
            ret = Mo_hold_Sel(g_pub_tx.reply, &sMo_hold,"ac_id =%ld and ac_seqn=%d and hold_seqn= %d ",sMdm_ac_rel.ac_id,iAc_seqn,iHold_seqn);
            if (ret != 0 && ret != 100)
            {
                 sprintf(acErrMsg, "��ѯmo_hold����!! [%d]", ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "KH04");
                 goto ErrExit;
            }
            else if(ret == 100)
            {
                 sprintf(acErrMsg, "���˺�û�иñʶ������!! [%d]", ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "KH05");
                 goto ErrExit;
            }
            if(sMo_hold.hold_sts[0] != '0')
            {
                sprintf(acErrMsg,"�ñʶ����ѽ�������ܿۻ���sMo_hold.hold_sts=[%c]",sMo_hold.hold_sts[0]);
                WRITEMSG
                strcpy(g_pub_tx.reply,"KH06");
                goto ErrExit;
            }
            /*�ϸñʶ�������Ƿ���Ч*/
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
            }
            else if(cXh_flag[0] == '0')
            {
                /*�����*/
                if(sMo_hold.hold_typ[0] == '3')/*���ֶ���ۻ�*/
                {
                    /*�������ۼƿۻ����
                    �˱ʶ���ʣ��ɿۻ���� = �ƻ���� - �ѿۻ����
                    dSy_kh_amt = sMo_hold.plan_hold_amt - sMo_hold.kh_sum_amt;
                    */
                    if(pub_base_CompDblVal(dKh_amt,sMo_hold.plan_hold_amt) != 0)
                    {
                        sprintf(acErrMsg,"�ۻ����[%f]��ñʲ��ֶ�����[%f]����",dKh_amt,sMo_hold.plan_hold_amt);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH19");
                        goto ErrExit;
                    }
#if 0
                    /*�˱ʶ���ۻ����ý�� = bal - zf_amt - min_bal - ctl_amt - \
                    (�˱ʲ��ֶ���֮ǰ����δ����Ĳ��ֶ����� - �˱ʲ��ֶ���֮ǰ����δ����Ĳ��ֶ����ѿۻ����)*/
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
#endif
                    /*����˱ʶ���ۻ��Ŀ��ý����۵Ļ���ʼ�ۻ�*/
                    if(sPrdt_ac_id.ac_id_type[0] == '2')/*����*/
                    {
                        dMax_ky_amt =  dKy_amt + sMo_hold.plan_hold_amt;
                        if(pub_base_CompDblVal(dKh_amt,dMax_ky_amt) > 0)
                        {
                            sprintf(acErrMsg,"�ۻ����[%f]���ڴ˱ʶ�����ý��[%f]",dKh_amt,dMax_ky_amt);
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"KH10");
                            goto ErrExit;
                        }            
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
                    }
                    else
                    {
                        sprintf(acErrMsg,"�˻��Ƕ��ڻ��߻���[%c]",sPrdt_ac_id.ac_id_type[0]);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"O238");
                        goto ErrExit;
                    }
                   vtcp_log("[%s][%d],dKy_amt=[%lf] dMax_ky_amt=[%lf],plan_hold_amt=[%f]",__FILE__,__LINE__,dKy_amt,dMax_ky_amt,sMo_hold.plan_hold_amt);

                }
                else if(sMo_hold.hold_typ[0] == '1' || sMo_hold.hold_typ[0] == '2')/*ȫ���������ֻ�������ۻ�*/
                {
                    if(pub_base_CompDblVal(dKh_amt,dKy_amt) > 0)
                    {
                        sprintf(acErrMsg,"�ۻ����[%f]���ڿ��ý��[%f]",dKh_amt,dKy_amt);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"KH03");
                        goto ErrExit;
                    }         
                }
                else
                {
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
        }
        else
        {
            sprintf(acErrMsg,"�ۻ����ʹ�[%c]",cKh_type[0]);
            WRITEMSG
            strcpy(g_pub_tx.reply,"KH12");
            goto ErrExit;
        }
OkExit:
        strcpy(g_pub_tx.reply, "0000");
        sprintf(acErrMsg, "success !!: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg, "fail !!: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}


