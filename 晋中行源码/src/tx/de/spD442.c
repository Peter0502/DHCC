/***************************************************************
* �� �� ��: spD442.c
* ��������: ��������������Э��ά��.
* ��    ��: zyl
* �������: 20110518
*
* �޸ļ�¼: 
* ��    ��: 
* �� �� ��: 
* �޸�����: 

insert into tx_def values('0042','������Э��ά��','
10000000000010000000000011000000100000001100000000000000001001000000000000000000100000001000000000000000000000000000000000000100','0','6','0');
insert into tx_flow_def values('0042',0,'0042','#$');
insert into tx_sub_def values('0042','������Э��ά��','spD442','0','0');

**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "dd_mst_c.h"
#include "mo_prot_ac_rel_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

spD442()
{
    int    iRet = 0;
    char   cAddr[60];
    char   cEmail[20];
    double dLeave_amt = 0.0;
    struct dd_mst_c  sDd_mst;
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct mo_prdt_prot_c  sMo_prdt_prot;
    struct mo_prot_ac_rel_c  sMo_prot_ac_rel;
    struct cif_addr_inf_c  sCif_addr_inf;
    struct cif_email_inf_c  sCif_email_inf;
    memset(cAddr,            0x00 , sizeof(cAddr));
    memset(cEmail,           0x00 , sizeof(cEmail));
    memset(&sDd_mst,         0x00 , sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
    memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
    memset(&sMo_prot_ac_rel, 0x00 , sizeof(struct mo_prot_ac_rel_c));
    memset(&sCif_addr_inf,   0x00 , sizeof(struct cif_addr_inf_c));
    memset(&sCif_email_inf,  0x00 , sizeof(struct cif_email_inf_c));
    
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    
    /**ȡֵ����ֵ**/
    get_zd_data("0300", g_pub_tx.ac_no);     /**�ʺ�**/
    get_zd_data("0260", cAddr);              /**��ַ**/
    get_zd_data("0330", cEmail);             /**�绰**/
    get_zd_double("0410",&dLeave_amt);
    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_old_acno(g_pub_tx.ac_no);       /**�Ծ��ʺŵĴ���**/
    
    vtcp_log("[%s][%d] ac_no =[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
    vtcp_log("[%s][%d] cAddr =[%s]",__FILE__,__LINE__,cAddr);
    vtcp_log("[%s][%d] cEmail=[%s]",__FILE__,__LINE__,cEmail);
    vtcp_log("[%s][%d] dLeave_amt=[%17.2f]",__FILE__,__LINE__,dLeave_amt);
    
    /**mdm_ac_rel**/
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",g_pub_tx.ac_no);
    if(iRet == 100)
    {
        sprintf(acErrMsg, "��¼������.ac_no=[%s]",g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D104");
          goto ErrExit;
    }
    else if(iRet)
    {
        sprintf(acErrMsg, "���ݿ��¼�쳣!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
          goto ErrExit;
    }
    /**dd_mst**/
    iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
    if(iRet == 100)
    {
        sprintf(acErrMsg, "��¼������..");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D104");
          goto ErrExit;
    }
    else if(iRet)
    {
        sprintf(acErrMsg, "���ݿ��¼�쳣!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
          goto ErrExit;
    }
    /**cif_addr_inf update**/
    iRet = sql_count("cif_addr_inf","cif_no=%ld",sDd_mst.cif_no);
    if(iRet<0)
    {
        sprintf(acErrMsg,"����sql_count����!");
        WRITEMSG
        goto ErrExit;
    }
    else if(iRet == 0)
    {
        sCif_addr_inf.cif_no = sDd_mst.cif_no;
        sCif_addr_inf.addr_seqn = 1;
        strcpy(sCif_addr_inf.addr_type,"2");
        strcpy(sCif_addr_inf.country,"01");
        strcpy(sCif_addr_inf.addr,cAddr);
        strcpy(sCif_addr_inf.addr,"");
        iRet = Cif_addr_inf_Ins(sCif_addr_inf,g_pub_tx.reply);
        if (iRet)
        {
            sprintf(acErrMsg,"�Ǽǵ�ַ��Ϣ�����![%d]",iRet);
            WRITEMSG
            	strcpy(g_pub_tx.reply,"C023");
            goto ErrExit;
        }
    }
    else
    {
        iRet = Cif_addr_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",sDd_mst.cif_no);
        if (iRet)
        {
            sprintf(acErrMsg,"DECLARE Cif_addr_inf_rel�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
              goto ErrExit;
        }
        iRet = Cif_addr_inf_Fet_Upd(&sCif_addr_inf,g_pub_tx.reply);
        if (iRet)
        {
           	sprintf(acErrMsg,"FETCH Cif_addr_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D102");
              goto ErrExit;
        }
        
        strcpy(sCif_addr_inf.addr,cAddr);
        iRet = Cif_addr_inf_Upd_Upd(sCif_addr_inf,g_pub_tx.reply);
        if (iRet)
        {
            sprintf(acErrMsg,"UPDATE Cif_addr_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
              goto ErrExit;
        }
        Cif_addr_inf_Clo_Upd( );
    }
    /**cif_email_inf update**/
    iRet = sql_count("cif_email_inf","cif_no=%ld",sDd_mst.cif_no);
    if(iRet<0)
    {
        sprintf(acErrMsg,"����sql_count����!");
        WRITEMSG
        goto ErrExit;
    }
    else if(iRet == 0)
    {
        sCif_email_inf.cif_no = sDd_mst.cif_no;
        sCif_email_inf.addr_seqn = 1;
        strcpy(sCif_email_inf.addr_type,"1");
        strcpy(sCif_email_inf.email,cEmail);
        strcpy(sCif_email_inf.link_man,sDd_mst.name);
        iRet = Cif_email_inf_Ins(sCif_email_inf,g_pub_tx.reply);
        if (iRet)
        {
            sprintf(acErrMsg,"�Ǽǵ��ӵ�ַ��Ϣ�����![%d]",iRet);
            WRITEMSG
            	strcpy(g_pub_tx.reply,"C024");
            goto ErrExit;
        }
    }
    else
    {
        iRet = Cif_email_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",sDd_mst.cif_no);
        if (iRet)
        {
            sprintf(acErrMsg,"DECLARE Cif_email_inf_rel�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D101");
              goto ErrExit;
        }
        iRet = Cif_email_inf_Fet_Upd(&sCif_email_inf,g_pub_tx.reply);
        if (iRet)
        {
           	sprintf(acErrMsg,"FETCH Cif_email_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D102");
              goto ErrExit;
        }
        strcpy(sCif_email_inf.email,cEmail);
        iRet = Cif_email_inf_Upd_Upd(sCif_email_inf,g_pub_tx.reply);
        if (iRet)
        {
            sprintf(acErrMsg,"UPDATE Cif_email_inf�α��쳣![%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
              goto ErrExit;
        }
        Cif_email_inf_Clo_Upd( );
    }
    /**end update.**/
    /**leave_amt update.**/
    iRet=Mo_prdt_prot_Dec_Upd(g_pub_tx.reply,"prot_type='TK7' and main_ac_id=%ld and sts in('1','2') ",sMdm_ac_rel.ac_id);
    if (iRet)
    {
        sprintf(acErrMsg, "�α궨�����!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
          goto ErrExit;
    }
    iRet=Mo_prdt_prot_Fet_Upd(&sMo_prdt_prot,g_pub_tx.reply);
    if (iRet == 100)
    {
        sprintf(acErrMsg, "��¼������!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D104");
          goto ErrExit;
    }
    else if (iRet)
    {
        sprintf(acErrMsg, "���Ҽ�¼����[%d]", iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D102");
        goto ErrExit;
    }
    sMo_prdt_prot.leave_amt = dLeave_amt;
    iRet=Mo_prdt_prot_Upd_Upd(sMo_prdt_prot,g_pub_tx.reply);
    if(iRet)
    {
        sprintf(acErrMsg, "���¼�¼����.[%d]", iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D105");
          goto ErrExit;
    }
    Mo_prdt_prot_Clo_Upd();
    
    /**end**/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "������Э��ά���ɹ�![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "������Э��ά��ʧ��![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}
