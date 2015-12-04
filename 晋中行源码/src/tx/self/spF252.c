/***************************
*�������ƣ�spF252.c
*������: ��ѯ�����˻����ڲ��ʵ�
           ĳһ���ڵĽ跽�����
           ���������������
           �������(���������ڳе�֧����ҵ�������ط�Ҫ����ʵ������޸�)
*����ˣ�CHENGBO
*���ʱ�䣺 2011-11-23
insert into tx_def values('6252','����ѯ','10000000000000000000000000000000000000011110000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','4','0','3',null);
insert into tx_flow_def  values ('6252', 0, 'F252', '#$');
insert into tx_sub_def  values ('F252', '����ѯ', 'spF252', '0', '0');
*
<EO099 DESC="��ѯ���ڲ��ô��ڵ�ǰ����!!"/>
***************************/
#include <stdio.h>
#include "public.h"
#include "in_mst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_hst_c.h"
#include "prdt_ac_id_c.h"

spF252()
{
    int iRet=0;
    long lTx_date=0;
    long lMax_date=0;          /*���������-20120618*/
    long lBr_max_trace=0;
    long lBr_min_trace=0;
    long lAc_max_trace=0;      /*�ʻ���Ӧ�������ˮ��*/
    long lAc_min_trace=0;      /*�˻���С��ˮ��*/
    char cAc_no[25];
    double dTot_dr_amt=0.00;   /*�跽�ܽ��*/
    double dTot_cr_amt=0.00;   /*�����ܽ��*/
    double dBr_bal=0.00;       /*�������*/
    double dZr_bal=0.00;       /*�������*/

    struct in_mst_hst_c sIn_mst_hst;
    struct dd_mst_hst_c sDd_mst_hst;
    struct in_mst_c sIn_mst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;

    memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
    memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(cAc_no,0x00,sizeof(cAc_no));

    pub_base_sysinit();

    get_zd_data("0300",cAc_no);
    get_zd_long("0460",&lTx_date);

    pub_base_old_acno(cAc_no);  /*�¾��˺�ת��*/
    
    /*���ݳе����������������ڴ��ڵ�ǰ����ֱ�ӱ���  20120618 */
    if( g_pub_tx.tx_date < lTx_date )
    {
        sprintf(acErrMsg,"��ѯ���ڴ��ڵ�ǰϵͳ����!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D244");
        goto ErrExit;
    }
    /*END BY 20120618*/
    iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no = '%s' ",cAc_no);
    if(iRet)
    {
         sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ����!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"W011");
         goto ErrExit;
    }

    iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",sMdm_ac_rel.ac_id);
    if(iRet)
    {
         sprintf(acErrMsg,"��ѯ��Ʒ�˺Ŷ��ձ����!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"W013");
         goto ErrExit;
    }
    if( sPrdt_ac_id.ac_id_type[0] == '1' )
    {
         iRet=sql_max_long("dd_mst_hst","trace_no",&lBr_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ȡ�����ˮ�Ŵ���iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
         iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lBr_max_trace,lTx_date);
         if( iRet == 100 )
         {
             memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
             /*���û����ˮ��ȥ���ҽ�����ϸ������ڵ������ˮ�Ŷ�Ӧ����������-20120618*/
             iRet=sql_max_long("dd_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"ȡ��ϸ����������ڴ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_max_long("dd_mst_hst","trace_no",&lAc_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"ȡ��ϸ�������ˮ�Ŵ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_max_trace,lMax_date);
             if( iRet == 100 )
             {
                 dBr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 dBr_bal=sDd_mst_hst.bal;
             }
             else
             {
                 sprintf(acErrMsg,"��ѯ������ϸ�����!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W050");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             dBr_bal=sDd_mst_hst.bal;
         }
         else
         {
             sprintf(acErrMsg,"��ѯ������ϸ�����!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W050");
             goto ErrExit;
         }

         iRet=sql_min_long("dd_mst_hst","trace_no",&lBr_min_trace,"ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ȡ��С��ˮ�Ŵ���iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
         iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst,"ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lBr_min_trace,lTx_date);
         if( iRet == 100 )
         {
             /*
             sDd_mst_hst.tx_amt=0.00;
             memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
             iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld ",sMdm_ac_rel.ac_id);
             if( iRet )
             {
                 sprintf(acErrMsg,"��ѯ�����������!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"K107");
                 goto ErrExit;
             }
             dZr_bal= sDd_mst.bal; ���û�н�����ϸ�Ͱ���������ȥ
             */
             memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
             /*���û����ˮ��ȥ���ҽ�����ϸ������ڵ���С��ˮ�Ŷ�Ӧ�Ľ��׶������-20120618*/
             iRet=sql_max_long("dd_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"ȡ��ϸ����������ڴ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_min_long("dd_mst_hst","trace_no",&lAc_min_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"ȡ��ϸ����С��ˮ�Ŵ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_min_trace,lMax_date);
             if( iRet == 100 )
             {
                 dZr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 if( sDd_mst_hst.add_ind[0] == '0')
                 {
                    dZr_bal=sDd_mst_hst.bal + sDd_mst_hst.tx_amt;
                 }
                 else if(sDd_mst_hst.add_ind[0] == '1')
                 {
                    dZr_bal=sDd_mst_hst.bal - sDd_mst_hst.tx_amt;
                 }
             }
             else
             {
                 sprintf(acErrMsg,"��ѯ������ϸ�����!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W050");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             if( sDd_mst_hst.add_ind[0] == '0')
             {
                dZr_bal=sDd_mst_hst.bal + sDd_mst_hst.tx_amt;
             }
             else if(sDd_mst_hst.add_ind[0] == '1')
             {
                dZr_bal=sDd_mst_hst.bal - sDd_mst_hst.tx_amt;
             }
         }
         else
         {
             sprintf(acErrMsg,"��ѯ������ϸ�����!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W050");
             goto ErrExit;
         }
         iRet=sql_sum_double("dd_mst_hst","tx_amt",&dTot_dr_amt,"ac_id=%ld and tx_date=%ld and add_ind='0' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ͳ�ƽ跽���������iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         iRet=sql_sum_double("dd_mst_hst","tx_amt",&dTot_cr_amt,"ac_id=%ld and tx_date=%ld and add_ind='1' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ͳ�ƴ������������iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
    }
    else if( sPrdt_ac_id.ac_id_type[0] == '9' )
    {
         iRet=sql_max_long("in_mst_hst","trace_no",&lBr_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ȡ�����ˮ�Ŵ���iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
         iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lBr_max_trace,lTx_date);
         if( iRet == 100 )
         {
             memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
             /*���û����ˮ��ȥ���ҽ�����ϸ������ڵ������ˮ�Ŷ�Ӧ����������-20120618*/
             iRet=sql_max_long("in_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"ȡ��ϸ����������ڴ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_max_long("in_mst_hst","trace_no",&lAc_max_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"ȡ��ϸ�������ˮ�Ŵ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_max_trace,lMax_date);
             if( iRet == 100 )
             {
                 dBr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 dBr_bal=sIn_mst_hst.bal;
             }
             else
             {
                 sprintf(acErrMsg,"��ѯ�ڲ�����ϸ�����!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W054");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             dBr_bal=sIn_mst_hst.bal;
         }
         else
         {
             sprintf(acErrMsg,"��ѯ�ڲ�����ϸ�����!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W054");
             goto ErrExit;
         }

         iRet=sql_min_long("in_mst_hst","trace_no",&lBr_min_trace,"ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ȡ��С��ˮ�Ŵ���iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         memset(&sIn_mst_hst,0x00,sizeof(struct in_mst_hst_c));
         iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst,"ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lBr_min_trace,lTx_date);
         if( iRet == 100 )
         {
             /*
             sIn_mst_hst.tx_amt=0.00;
             memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
             iRet=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
             if( iRet )
             {
                 sprintf(acErrMsg,"��ѯ�ڲ����������!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"K110");
                 goto ErrExit;
             }
             dZr_bal=sIn_mst.bal; ���û�н�����ϸ�Ͱ��������ȡ����
             */
             memset(&sIn_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
             /*���û����ˮ��ȥ���ҽ�����ϸ������ڵ���С��ˮ�Ŷ�Ӧ�Ľ��׶������-20120618*/
             iRet=sql_max_long("in_mst_hst","tx_date",&lMax_date," ac_id=%ld and tx_date <= %ld ",sMdm_ac_rel.ac_id,lTx_date);
             if( iRet )
             {
                 sprintf(acErrMsg,"ȡ��ϸ����������ڴ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=sql_min_long("in_mst_hst","trace_no",&lAc_min_trace," ac_id=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id,lMax_date);
             if(iRet)
             {
                 sprintf(acErrMsg,"ȡ��ϸ����С��ˮ�Ŵ���iRet=[%d]",iRet);
                 WRITEMSG
                 goto ErrExit;
             }
             iRet=In_mst_hst_Sel(g_pub_tx.reply,&sIn_mst_hst," ac_id=%ld and trace_no=%ld and tx_date=%ld ",sMdm_ac_rel.ac_id ,lAc_min_trace,lMax_date);
             if( iRet == 100 )
             {
                 dZr_bal=0.00;
             }
             else if( iRet == 0 )
             {
                 if( sIn_mst_hst.add_ind[0] == '0')
                 {
                    dZr_bal=sIn_mst_hst.bal + sIn_mst_hst.tx_amt;
                 }
                 else if(sIn_mst_hst.add_ind[0] == '1')
                 {
                    dZr_bal=sIn_mst_hst.bal - sIn_mst_hst.tx_amt;
                 }
             }
             else
             {
                 sprintf(acErrMsg,"��ѯ�ڲ�����ϸ�����!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"W050");
                 goto ErrExit;
             }
             /*ADD END 20120618*/
         }
         else if( iRet == 0 )
         {
             if( sIn_mst_hst.add_ind[0] == '0')
             {
                 dZr_bal=sIn_mst_hst.bal + sIn_mst_hst.tx_amt;
             }
             else if(sIn_mst_hst.add_ind[0] == '1')
             {
                 dZr_bal=sIn_mst_hst.bal - sIn_mst_hst.tx_amt;
             }
         }
         else
         {
             sprintf(acErrMsg,"��ѯ�ڲ�����ϸ�����!");
             WRITEMSG
             strcpy(g_pub_tx.reply,"W054");
             goto ErrExit;
         }

         iRet=sql_sum_double("in_mst_hst","tx_amt",&dTot_dr_amt,"ac_id=%ld and tx_date=%ld and add_ind='0' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ͳ�ƽ跽���������iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
         iRet=sql_sum_double("in_mst_hst","tx_amt",&dTot_cr_amt,"ac_id=%ld and tx_date=%ld and add_ind='1' ",sMdm_ac_rel.ac_id,lTx_date);
         if(iRet)
         {
             sprintf(acErrMsg,"ͳ�ƴ������������iRet=[%d]",iRet);
             WRITEMSG
             goto ErrExit;
         }
    }
    else
    {
         vtcp_log("[%s][%d],ac_id_type=[%c] ���ڲ��ʺͻ����˻�",__FILE__,__LINE__,sPrdt_ac_id.ac_id_type[0]);
         sprintf(acErrMsg,"�ǻ��ں��ڲ����˻�");
         WRITEMSG
         goto ErrExit;
    }
    set_zd_double("0400",dBr_bal);
    set_zd_double("0410",dZr_bal);
    set_zd_double("0420",dTot_dr_amt);
    set_zd_double("0430",dTot_cr_amt);
    /*�е�Ҫ���ѯ�Ǽ���ˮ  20120516 chengbo*/
    strcpy(g_pub_tx.ac_no,cAc_no);
    g_pub_tx.ac_id=sMdm_ac_rel.ac_id;
    g_pub_tx.ac_seqn=1;
    strcpy(g_pub_tx.brf,"�˻�����ѯ");
    if (pub_ins_trace_log())
    {
       sprintf(acErrMsg, "�Ǽǽ�����ˮ����!");
       WRITEMSG
       goto ErrExit;
    }
    /*���غ�����ˮ��ƽ̨*/
    set_zd_long("0040",g_pub_tx.trace_no);

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"��ѯ����״̬�ɹ�!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(strcmp(g_pub_tx.reply,"0000")==0 || strcmp(g_pub_tx.reply,"")==0)
    {
        strcpy(g_pub_tx.reply,"D104");
    }
    sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
