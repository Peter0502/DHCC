/***************************************************************
* �� �� ��: spD440.c
* ������������������ҵ�������ֻ���Э��ǩԼ
* ��    ��: Ѧ��
* ������ڣ�2011��4��8��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
insert into tx_def values('0008','������ǩԼ','10100000000000000000100001000100000000100000110000000000000000000011100100000000000001000000000100000000000000000000000000100100','1','4','0');
insert into tx_flow_def values('0008',0,'D440','#=0000@1@#$');
insert into tx_flow_def values('0008',1,'D001','#=0000@2@#$');
insert into tx_flow_def values('0008',2,'D002','#=0000@3@#$');
insert into tx_flow_def values('0008',3,'D441','#$');
insert into tx_sub_def values('D440','�����ֻ���Э��ǩԼ','spD440','0','0');

insert into tx_sub_rel values('0008','D001','1','1',   '0191','����0190');
insert into tx_sub_rel values('0008','D002','0','1167','1039','�˻�');
insert into tx_sub_rel values('0008','D002','0','1166','1038','����');
insert into tx_sub_rel values('0008','D002','1','107', '1031','��Ʒ����');
�����������Ʒ
insert into dd_parm
values ('107', '�����������˻�', 19850101, 99999999, '1', '01', 'D', '5', 'C', 0, 0, 0, 0, 'N', '000', 'Y', 'Y', '000', '000', 'C', '0', 0, 0, '100', '0', 'F', '000', 0, 0, '1', '2', 0, 0, '001', 'N', '000', '000', 0, 0, '000', 1, 'Y', 'Y', 'Y', '000', 'N', '000', 5, 'Y', 'Y', 'Y', 1, 'Y', 'Y', 'Y', 'Y', 'Y', 'N', 'Y', 'Y', 'N', 'N', '000', 0, 0, '2111', 'N');


*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "prdt_prot_parm_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "mo_prot_ac_rel_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

spD440()
{
    int  ret=0;
    char ac_no[20];
    char id_no[20];    
    char cif_type[2];
    char id_type[2];
    char draw_pwd[7];
    char   cAddr[60];
    char   cEmail[20];
    double dLeave_amt = 0.0;
        
    struct dd_mst_c  sDd_mst;                /*�������Ż���*/
    struct dd_parm_c  sDd_parm;              /*���ڲ�Ʒ������*/
    struct mdm_ac_rel_c  sMdm_ac_rel;        /*�������˻���ϵ*/
    struct mo_prdt_prot_c  sMo_prdt_prot;    /*����Э��Ǽǲ�*/
    struct mo_prot_ac_rel_c  sMo_prot_ac_rel;/*����Э���˻����ձ�*/
    struct cif_addr_inf_c  sCif_addr_inf;
    struct cif_email_inf_c  sCif_email_inf;
    memset(cAddr,   0x00 , sizeof(cAddr));
    memset(cEmail,  0x00 , sizeof(cEmail));    
    memset(ac_no,   0x00 , sizeof(ac_no));
    memset(id_no,   0x00 , sizeof(id_no));
    memset(cif_type,0x00 , sizeof(cif_type));
    memset(id_type, 0x00 , sizeof(id_type));
    memset(draw_pwd,0x00 , sizeof(draw_pwd));
    
    memset(&sDd_parm,        0x00 , sizeof(struct dd_parm_c));
    memset(&sDd_mst,         0x00 , sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
    memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
    memset(&sMo_prot_ac_rel, 0x00 , sizeof(struct mo_prot_ac_rel_c));
    memset(&sCif_addr_inf,   0x00 , sizeof(struct cif_addr_inf_c));
    memset(&sCif_email_inf,  0x00 , sizeof(struct cif_email_inf_c));
    
    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();
     
     /*ȡֵ����ֵ*/
    get_zd_data("0300", ac_no);              /*�˺�*/
    get_zd_data("0380", id_no);              /*֤������*/
    get_zd_data("0260", cAddr);              /**��ַ**/
    get_zd_data("0330", cEmail);             /**�绰**/
    get_zd_double("0410",&dLeave_amt);       /**������**/
    pub_base_strpack(ac_no);
    pub_base_old_acno(ac_no); /* �Ծ��ʺŵĴ��� */
    
    get_zd_data("0680", cif_type); /*�ͻ�����*/
    get_zd_data("0890", id_type);  /*֤������*/
    get_zd_data("0790", draw_pwd); /*����*/
    
    /* �����ʺŲ�ѯ�˻���ź��˻�ID */
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",ac_no);
    if(ret)
      {
        sprintf(acErrMsg, "��ѯmdm_ac_rel����!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"W011");
          goto ErrExit;
      }
    /**����,֧�ֿ���������..mdm_ac_rel.ac_seqn=9999 dd_mst.ac_seqn=1 20110511 ȥ��ac_seqn����.**/
    ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
    if(ret == 100)
    {
        sprintf(acErrMsg, "�޴˼�¼!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D104");
          goto ErrExit;
    }
    else if(ret)
    {
        sprintf(acErrMsg, "���ݿ��¼�쳣!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
          goto ErrExit;
    }
    /**add by zyl 20110511 �����ֽ������**/
    vtcp_log("[%s][%d] dd_mst.bal=[%17.2f]",__FILE__,__LINE__,sDd_mst.bal);
    if( pub_base_CompDblVal(sDd_mst.bal-sDd_mst.ctl_amt-sDd_mst.hold_amt,50000.00+1000.00) < 0 )
    {
        sprintf(acErrMsg, "�˻�����!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"A030");
          goto ErrExit;
    }
    /**end by zyl 20110511**/
    memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
    strcpy(sMo_prdt_prot.prot_type,"TK7");           /*Э������*/
    sMo_prdt_prot.main_ac_id=sMdm_ac_rel.ac_id;      /*���˺�*/
    sMo_prdt_prot.main_ac_seqn=sMdm_ac_rel.ac_seqn;  /*���˺����*/
    strcpy(sMo_prdt_prot.tx_br_no,g_pub_tx.tx_br_no);/*�Ǽǻ�������*/
    sMo_prdt_prot.tx_date=g_pub_tx.tx_date;          /*�Ǽ�����*/
    sMo_prdt_prot.trace_no=g_pub_tx.trace_no;        /*������ˮ��*/
    strcpy(sMo_prdt_prot.tel,g_pub_tx.tel);          /*�Ǽǹ�Ա*/
    sMo_prdt_prot.sts[0]='1';                        /*״̬: 1��������*��ȡ��*/        
    sMo_prdt_prot.leave_amt=dLeave_amt;
    vtcp_log("[%s][%d] xk leave_amt=[%f]",__FILE__,__LINE__,sMo_prdt_prot.leave_amt);
    sMo_prdt_prot.turn_amt=50000;                    /*��ת���*/
    
    Mo_prdt_prot_Debug(&sMo_prdt_prot);
    
    ret = Mo_prdt_prot_Ins(sMo_prdt_prot,g_pub_tx.reply);
    if( ret )
     {
       sprintf(acErrMsg, "����mo_prdt_prot����[%d]!!",ret);
       WRITEMSG
       strcpy(g_pub_tx.reply,"D105");
         goto ErrExit;
     }
     
    /* pub_mob_sendmail("����֪ͨ����˻�ǩԼ", g_pub_tx.tx_code,sMdm_ac_rel.ac_no,g_pub_tx.add_ind,0.0, 0.0 ); mod by lwb 20150519*/
     pub_mob_sendmail("����֪ͨ����˻�ǩԼ", g_pub_tx.tx_code,sMdm_ac_rel.ac_no,"1",0.0, 0.0 ); 
     
    /**����.�ͻ�������Ϣ.�绰,��ַ. ��ԭ�������ڼ�¼.���..by zyl 20110701**/
    /**cif_addr_inf update**/
    ret = sql_count("cif_addr_inf","cif_no=%ld",sDd_mst.cif_no);
    if(ret<0)
    {
        sprintf(acErrMsg,"����sql_count����!");
        WRITEMSG
        goto ErrExit;
    }
    else if(ret == 0)
    {
        sCif_addr_inf.cif_no = sDd_mst.cif_no;
        sCif_addr_inf.addr_seqn = 1;
        strcpy(sCif_addr_inf.addr_type,"2");
        strcpy(sCif_addr_inf.country,"01");
        strcpy(sCif_addr_inf.addr,cAddr);
        strcpy(sCif_addr_inf.addr,"");
        ret = Cif_addr_inf_Ins(sCif_addr_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"�Ǽǵ�ַ��Ϣ�����![%d]",ret);
            WRITEMSG
            	strcpy(g_pub_tx.reply,"C023");
            goto ErrExit;
        }
    }
    else
    {
        ret = Cif_addr_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",sDd_mst.cif_no);
        if (ret)
        {
            sprintf(acErrMsg,"DECLARE Cif_addr_inf_rel�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
              goto ErrExit;
        }
        ret = Cif_addr_inf_Fet_Upd(&sCif_addr_inf,g_pub_tx.reply);
        if (ret)
        {
           	sprintf(acErrMsg,"FETCH Cif_addr_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D102");
              goto ErrExit;
        }
        
        strcpy(sCif_addr_inf.addr,cAddr);
        ret = Cif_addr_inf_Upd_Upd(sCif_addr_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"UPDATE Cif_addr_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
              goto ErrExit;
        }
        Cif_addr_inf_Clo_Upd( );
    }
    /**cif_email_inf update**/
    ret = sql_count("cif_email_inf","cif_no=%ld",sDd_mst.cif_no);
    if(ret<0)
    {
        sprintf(acErrMsg,"����sql_count����!");
        WRITEMSG
        goto ErrExit;
    }
    else if(ret == 0)
    {
        sCif_email_inf.cif_no = sDd_mst.cif_no;
        sCif_email_inf.addr_seqn = 1;
        strcpy(sCif_email_inf.addr_type,"1");
        strcpy(sCif_email_inf.email,cEmail);
        strcpy(sCif_email_inf.link_man,sDd_mst.name);
        ret = Cif_email_inf_Ins(sCif_email_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"�Ǽǵ��ӵ�ַ��Ϣ�����![%d]",ret);
            WRITEMSG
            	strcpy(g_pub_tx.reply,"C024");
            goto ErrExit;
        }
    }
    else
    {
        ret = Cif_email_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",sDd_mst.cif_no);
        if (ret)
        {
            sprintf(acErrMsg,"DECLARE Cif_email_inf_rel�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
              goto ErrExit;
        }
        ret = Cif_email_inf_Fet_Upd(&sCif_email_inf,g_pub_tx.reply);
        if (ret)
        {
           	sprintf(acErrMsg,"FETCH Cif_email_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D102");
              goto ErrExit;
        }
        strcpy(sCif_email_inf.email,cEmail);
        ret = Cif_email_inf_Upd_Upd(sCif_email_inf,g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"UPDATE Cif_email_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
              goto ErrExit;
        }
        Cif_email_inf_Clo_Upd( );
    }
    /**end update.**/
    set_zd_data("0192", "0");              /*���˶Խ��ʺ���*/
    get_zd_data("0192", g_pub_tx.ac_wrk_ind);
    vtcp_log("%s,%d  seeeee hs=[%s]",__FILE__,__LINE__,g_pub_tx.ac_wrk_ind);
    set_zd_int("0193",  "1000");                    /*svc_ind--1000����*/
    set_zd_data("1161", "0000");                    /*���ʴ���*/
    set_zd_data("1164", sMdm_ac_rel.id_type);       /*֤������*/
    set_zd_data("1165", sMdm_ac_rel.id_no);         /*֤������*/
   /**delete by jk 20130304�����д����
    set_zd_data("1168", "00000000BBBBBBBB");        ƾ֤����*/
    set_zd_data("1166", sDd_mst.name);              /*����*/
    set_zd_data("1031", "107");                     /*��Ʒ����*/
    set_zd_data("1033", "01");                      /*����*/
    set_zd_data("1034", "5");                       /*�˻�����*/
    set_zd_data("1036", "4");                       /*��Ϣ����*/
    set_zd_data("1039", sMdm_ac_rel.ac_no);         /*�˻�*/
    vtcp_log("[%s][%d] zyl see3 opn_br_no=[%s]",__FILE__,__LINE__,sDd_mst.opn_br_no);
    set_zd_data("0020",sDd_mst.opn_br_no);/**20110427 �����˻��Ŀ�������,���ǽ��׻���.��Ϊ���˻��Ŀ�������.**/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "7���ֻ���Э��ǩԼ�ɹ�![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "7���ֻ���Э��ǩԼʧ��![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
    
}
