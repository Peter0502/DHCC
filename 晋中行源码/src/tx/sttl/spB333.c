/*************************************************
* �� �� ��:  spB333.c
* ���������� ������гжһ�Ʊǩ������
*
* ��    ��:  jane
* ������ڣ� 2003��5��26��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_hold_c.h"
#include "com_parm_c.h"
#include "mo_bank_po_fee_c.h"

int spB333()
{
    struct    mo_bank_acc_po_c    sMoBankAccPo;    /* ���гжһ�Ʊ������Ϣ�ǼǱ� */
    struct    mo_bank_po_c         sMoBankPo;        /* ���гжһ�Ʊ�ǼǱ� */
    struct    mdm_ac_rel_c         sMdmAcRel;        /* �����˺Ŷ��ձ� */
    struct    dd_mst_c             sDdMst;            /* �������ļ� */
    struct    impa_dep_reg_c         sImpaDepReg;    /* ��Ѻ�浥�ǼǱ� */
    struct    mo_hold_c             sMoHold;        /* ����Ǽǲ� */
    struct    td_mst_c             sTdMst;            /* �������ļ� */
    struct    com_parm_c             sComParm;        /* ���������� */
    struct    mo_bank_po_fee_c      sMo_bank_po_fee;   /** �жһ�Ʊ�շ� **/
    struct    com_parm_c             wd_com_parm;    /* ����������:ȡ�жҿ�Ŀ */
    char    cPactNo[21];                        /* �ж�Э���� */
    char    cPoNo1[31];/**�ĳ�30**/                            /* �жһ�Ʊ������Ļȡֵ */
    char    cPoNo[9];                            /* �жһ�Ʊ������Чλ */
    char    cPoNoHead[9];                        /* �жһ�Ʊ�������ͷ */
    char    cPoNo2[31];                            /* �жһ�Ʊ�������ͷ+��Чλ */
    double    dPoAmt;                                /* ��ǩ����Ʊ�ܽ�� */
    double    dSumPoAmt;                            /* ��ǩ���Ļ�Ʊ����ܶ��뱾��ǩ����Ʊ���֮�� */
    long    lHoldSeqn;                            /* ������� */
    int        iPactNoNum;                            /* ��� */
    int        ret, flag;
    int     parm_ind = 0;
    double  dTallAmt=0.00; 
    double  dKqAmt =0.00;
    /****************/
    /*****���ӵ�Ʊ������� ****/
    char ctype[2];
    /**************************/
    char cInd[2];
    char cPayee_acno[33];
    char cPayee_name[61];
    char cPayee_brname[61];
    char cCt_ind[2];/** add by chenchao 2011-7-28 10:48:10 ��������ת��־ **/
    double dFee_amt= 0.00;/** add by chenchao �����ѽ�� **/
    double dPrm_amt = 0.00;/** add by chenchao ��ŵ��� **/
    
    memset(cInd,0x00,sizeof(cInd));
    memset(cPayee_acno,0x00,sizeof(cPayee_acno));
    memset(cPayee_name,0x00,sizeof(cPayee_name));
    memset(cPayee_brname,0x00,sizeof(cPayee_brname));
    /******************/
    memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
    memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
    memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
    memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
    memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
    memset( &sMoHold, 0x00, sizeof( struct mo_hold_c ) );
    memset( &sTdMst, 0x00, sizeof( struct td_mst_c ) );
    memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
    memset( &wd_com_parm, 0x00, sizeof( struct com_parm_c ) );
    memset( cPactNo, 0x00, sizeof( cPactNo ) );
    memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
    memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
    memset( cPoNo, 0x00, sizeof( cPoNo ) );
    memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );
    memset( cCt_ind,0x00,sizeof(cCt_ind));

    /* ���ݳ�ʼ�� */
    pub_base_sysinit();

    /* ��ǰ̨��Ļȡֵ */
    get_zd_data( "0330", cPactNo );                        /* �ж�Э���� */
    get_zd_int( "0500", &iPactNoNum );                    /* ��� */
    get_zd_data( "0580", cPoNo1 );                        /* �жһ�Ʊ���� */
    vtcp_log( "zxxxyu111      cPoNo1[%s]", cPoNo1 );
    get_zd_double( "100E", &sMoBankPo.po_amt );            /* Ʊ���� */
    get_zd_data("0930",ctype);    /*��ƱֽƱ��־* add by chenchao  20100419��PֽƱ��E��Ʊ*/
    if(ctype[0]=='E')
    get_zd_data("1243",cPoNo1);    
    get_zd_data("0670",cCt_ind);

    /**********************/
    get_zd_data("0930",cInd);
    get_zd_data("0270",cPayee_name);
    get_zd_data("0830",cPayee_acno);
    get_zd_data("0810",cPayee_brname);
    get_zd_double("1004",&dFee_amt);
    get_zd_double("1131",&dPrm_amt);
    /**********************/
    /* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
    /* Ʊ���ܽ��ܴ��ڵ���1100��Ԫ����� */
    ret = pub_base_GetParm_double( "CDZGE", 1, &dTallAmt );
    if( ret )
    {
        sprintf( acErrMsg, "ȡ�жһ�Ʊ����޶��Ʒ��Ŵ�!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B269" );
        goto ErrExit;
    }

    ret = pub_base_CompDblVal( sMoBankPo.po_amt, dTallAmt );
    if( ret != -1 )
    {
        sprintf( acErrMsg, "�жһ�ƱƱ���ܽ�������1100��Ԫ�����![%lf]",sMoBankPo.po_amt);
        WRITEMSG
        strcpy( g_pub_tx.reply, "B270" );
        goto ErrExit;
    }

    strcpy( sMoBankPo.sign_br_no, g_pub_tx.tx_br_no );    /* ǩ�����к� */
    sMoBankPo.sign_date = g_pub_tx.tx_date;                /* ǩ������ */
    strcpy( sMoBankPo.at_term_ind, "N" );                /* Ʊ�ݵ��ڱ�־:Y ���ڣ�N δ���� */
    strcpy( sMoBankPo.po_sts, "1" );                    /* �жһ�Ʊ״̬:1 ǩ�� */
    strcpy( sMoBankPo.po_ind, "1" );                    /* �жһ�Ʊ��־:1 ���� */
    if(ctype[0]=='E')/***��Ʊ�Ļ������ƾ֤�Ų�����**/
    {
    }
    else
    {
        /* �������жһ�Ʊ�����Ƿ������ͷ */
        ret = pub_com_ChkNoteHead( "111", cPoNo1, g_pub_tx.tel);
        if( ret )
        {
            sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
        
        vtcp_log( "zxxxyu222      cPoNo1[%s]", cPoNo1 );
        /* ��ѯ�жһ�Ʊ�Ƿ��ѱ����� */
        ret = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CDHP'" );
        if( ret )
        {
            sprintf( acErrMsg, "ִ��Com_parm_Sel����![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
        
        pub_base_strpack( sComParm.val );
        
        vtcp_log("pono[%s]notetype[%s]brno[%s]tel[%s]",cPoNo1, sComParm.val,g_pub_tx.tx_br_no, g_pub_tx.tel);
        /* �жһ�Ʊ���Ŵ��� */
        g_pub_tx.svc_ind=7021;
        ret=pub_com_NoteUse( 0,0,sComParm.val,cPoNo1,cPoNo1,g_pub_tx.tel);
        if( ret) 
        {
            sprintf( acErrMsg, "���гжһ�Ʊ���ų���!" );
            WRITEMSG
            goto ErrExit;
        }
    }
    /* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
    ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", cPactNo );
    if( ret == 100 )
    {
        sprintf( acErrMsg, "�޴˳ж�Э����!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B187" );
        goto ErrExit;
    }
    else if( ret )
    {
        sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

/**
    if( sMoBankAccPo.acc_po_sts[0] == '2'&& sMoBankAccPo.acc_po_sts[0] == '3' )
**/
    if( sMoBankAccPo.acc_po_sts[0] == '2' )
    {
        sprintf( acErrMsg, "�˳ж�Э������ǩ��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B188" );
        goto ErrExit;
    }
    else if( sMoBankAccPo.acc_po_sts[0] == 'A' )
    {
        sprintf( acErrMsg, "�˳ж�δ��¼��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B274" );
        goto ErrExit;
    }else if( sMoBankAccPo.acc_po_sts[0] == '0' )
    {
        sprintf( acErrMsg, "�˳ж�δ�����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B275" );
        goto ErrExit;
    }

    set_zd_data( "0810", sMoBankAccPo.payee_br_name );    /* �տ��˿���������(Ϊ��ӡ׼��) */

    /* ���ݳж�Э���źͳж�Э��״̬Ϊ"����"�޸����гжһ�Ʊ������Ϣ�ǼǱ� */
    ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply , "pact_no='%s' \
                and acc_po_sts in ('1','3')", cPactNo );
    if ( ret )
    {
        sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "�޴˳ж�Э����!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B187" );
        goto ErrExit;
    }
    else if ( ret )
    {
        sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    /* �ж����гжһ�Ʊ�Ǽǲ��ж���ͬһ��Э���ŵĻ�Ʊ���֮����¼��ʱ��Ʊ���ܽ���Ƿ���� */
    ret = sql_sum_double( "mo_bank_po", "po_amt", &dPoAmt, "pact_no='%s' and po_sts='1'", cPactNo );
    if( ret )
    {
        sprintf( acErrMsg, "ִ��sql_sum_double��[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }
    sprintf( acErrMsg, "��ǩ���Ļ�Ʊ����ܶ�=[%lf],sign_amt=[%lf]", dPoAmt + sMoBankPo.po_amt,sMoBankAccPo.sign_amt );
    WRITEMSG
    dSumPoAmt = dPoAmt + sMoBankPo.po_amt;    /* ��ǩ���Ļ�Ʊ����ܶ��뱾��ǩ����Ʊ���֮�� */
    
    /*************add by xyy 2010-6-24 15:46:57 �����֤������0 *************/
    if( pub_base_CompDblVal ( sMoBankAccPo.bail_amt,0.00 ) > 0 )
    {
        double dSumKyamt = 0.00;
        
        dSumKyamt = sMoBankAccPo.bail_amt / sMoBankAccPo.bail_amt_ratio*100;
        if( pub_base_CompDblVal( dSumPoAmt, dSumKyamt) > 0)
        {
            sprintf( acErrMsg, "��֤����!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B194" );
            goto ErrExit;
        }
        
        dKqAmt = dSumKyamt;
    }
    else
    {
        dKqAmt = sMoBankAccPo.sign_amt;
    }
    vtcp_log("[%s][%d] ��ǩ���ܽ��=[%f]",__FILE__,__LINE__,dKqAmt);
    /*************end by xyy 2010-6-24 15:46:57 *************/
    
    ret = pub_base_CompDblVal( dSumPoAmt, dKqAmt );
    if( ret == 1 )
    {
        sprintf( acErrMsg, "��ǩ���Ļ�Ʊ����ܶ����¼��ʱ��Ʊ���ܽ��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B194" );
        goto ErrExit;
    }
    else if( ret == 0 )
    {
        sprintf( acErrMsg, "��ǩ���Ļ�Ʊ����ܶ�=[%lf],sign_amt=[%lf]", dPoAmt + sMoBankPo.po_amt,sMoBankAccPo.sign_amt );
        WRITEMSG
        /* �޸����гжһ�Ʊ������Ϣ�ǼǱ� */
        strcpy( sMoBankAccPo.acc_po_sts, "2" );                /* ��Ʊ״̬: 2 ���� */

        ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
            if ( ret )
        {
            sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Upd_Upd��[%d]", ret );
            WRITEMSG
            goto ErrExit;    
        }
    }
    sprintf( acErrMsg, "��ǩ���Ļ�Ʊ����ܶ�=[%lf],sign_amt=[%lf]", dPoAmt + sMoBankPo.po_amt,sMoBankAccPo.sign_amt );
    WRITEMSG
    Mo_bank_acc_po_Clo_Upd( );
    if(ctype[0]=='E')
    {
        get_zd_data("1243",cPoNo2);
    }
    else
    {
        /* ȡ�жһ�Ʊ����ͷ */
        strncpy( cPoNoHead, cPoNo1, 8 );
        cPoNoHead[8]='\0';
        pub_base_strpack( cPoNoHead );
    
        /* ȡ�жһ�Ʊ��Чλ */
        strncpy( cPoNo, cPoNo1+8, 8 );
        cPoNo[8] = '\0';
        sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
        WRITEMSG
    
        /* �жһ�Ʊ�������ͷ+��Чλ */
        sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
    }
    sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
    WRITEMSG
    pub_base_strpack_all( cPoNo2 );

    /* ���ݳжһ�Ʊ����������гжһ�Ʊ�ǼǱ� */
    ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo2 );
    TRACE
    if( ret == 0 )
    {
        sprintf( acErrMsg, "�жһ�Ʊ�����Ѵ���!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B209" );
        goto ErrExit;
    }
    else if( ret != 100 )
    {
        TRACE
        sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    /* ���ݳжһ�Ʊ��Ų������гжһ�Ʊ�ǼǱ� */
    ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "pact_no='%s' and pact_no_num=%d",cPactNo, iPactNoNum );
    if( ret == 0 )
    {
        sprintf( acErrMsg, "����ظ�!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B208" );
        goto ErrExit;
    }
    else if( ret != 100 )
    {
        TRACE
        sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    /* �Ǽ����гжһ�Ʊ�ǼǱ� */
    vtcp_log("[%s][%d] pactno=[%s] pono=[%s] ",__FILE__,__LINE__,cPactNo,cPoNo2);
    strcpy( sMoBankPo.pact_no, cPactNo );
    sMoBankPo.pact_no_num = iPactNoNum;
    strcpy( sMoBankPo.po_no, cPoNo2 );

    ret = Mo_bank_po_Ins( sMoBankPo, g_pub_tx.reply );
    if( ret )
    {
        Mo_bank_po_Debug(&sMoBankPo);
        sprintf( acErrMsg, "ִ��Mo_bank_po_Ins��[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

TRACE
    /* ������Ѻ�浥 */
    ret = Impa_dep_reg_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_num );
    if( ret )
    {
        sprintf( acErrMsg, "ִ��Impa_dep_reg_Dec_Upd��[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

TRACE
    flag = 0;
    while(1)
    {
        ret = Impa_dep_reg_Fet_Upd( &sImpaDepReg, g_pub_tx.reply );
        if ( ret == 100 )
        {
            break;
        }else if ( ret )
        {
            sprintf( acErrMsg, "ִ��Impa_dep_reg_Fet_Upd��[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
TRACE
        /* ������Ѻ�浥���޸Ķ������ļ� */
        ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
                        sImpaDepReg.mort_ac_no );
        if( ret == 100 )
        {
            sprintf( acErrMsg, "��Ѻ�浥�Ų�����![%s]", sImpaDepReg.mort_ac_no );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B183" );
            goto ErrExit;
        }
        else if( ret )
        {
            sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        ret = Td_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld", sMdmAcRel.ac_id );
        if ( ret )
        {
            sprintf( acErrMsg, "ִ��Td_mst_Dec_Upd��[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        ret = Td_mst_Fet_Upd( &sTdMst, g_pub_tx.reply );
        if ( ret == 100 )
        {
            sprintf( acErrMsg, "��Ѻ�浥�Ų�����![%s]", sImpaDepReg.mort_ac_no );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B183" );
            goto ErrExit;
        }else if ( ret )
        {
            sprintf( acErrMsg, "ִ��Td_mst_Fet_Upd��[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

        /**--- xxx ----- @20030906@ ---- ������˾Ͳ����ٶ����� -----**/
        if( sTdMst.hold_sts[0]=='1' )
        {
            sprintf( acErrMsg, "�Ѿ�������ˣ������ٶ�����[%d][%ld]",ret,sTdMst.ac_id );
            WRITEMSG
            flag++;
            continue;
        }
        strcpy( sTdMst.hold_sts, "1" );        /* ����״̬: 1 ��ȫ����*/
TRACE
        ret = Td_mst_Upd_Upd( sTdMst, g_pub_tx.reply );
            if ( ret )
        {
            sprintf( acErrMsg, "ִ��Td_mst_Upd_Upd��[%d]", ret );
            WRITEMSG
            goto ErrExit;    
        }
        Td_mst_Clo_Upd( );

TRACE
        /* ��ѯ����Ǽǲ�����󶳽���� */
        ret = sql_max_long( "mo_hold", "hold_seqn", &lHoldSeqn , "1=1" );
        if( ret )
        {
            sprintf( acErrMsg, "ִ��sql_max_long��[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        /* �ǼǶ���Ǽǲ� */
        memset(&sMoHold,0x00,sizeof(struct mo_hold_c));
        sMoHold.hold_seqn = lHoldSeqn + 1;    /* ������� */
        sMoHold.ac_id = sMdmAcRel.ac_id;    /* �����˻�ID */
        sMoHold.ac_seqn = sMdmAcRel.ac_seqn;    /* �����˻���� */
        strcpy( sMoHold.hold_typ, "1" );    /* ����״̬: 1 ȫ������*/
        sMoHold.plan_hold_amt = 0.00;        /* �ƻ������� */
        strcpy( sMoHold.auto_unhold_ind, "Y" );    /* �Զ��ⶳ��־: Y ���Զ��ⶳ*/
        strcpy( sMoHold.hold_sts, "0" );    /* ����״̬���: 0 δ���*/
        sMoHold.beg_hold_date = g_pub_tx.tx_date;/* ������ʼ���� */
        sMoHold.plan_unhold_date = 0;        /* �ƻ��ⶳ���� */
        sMoHold.fct_unhold_date = 0;        /* ʵ�ʽⶳ���� */
        strcpy( sMoHold.hold_br_no, g_pub_tx.tx_br_no );/* ��������� */
        strcpy( sMoHold.hold_tel, g_pub_tx.tel );/* �������Ա*/
        strcpy( sMoHold.unhold_br_no, "0" );    /* �ⶳ������*/
        strcpy( sMoHold.unhold_tel, "0" );    /* �ⶳ����Ա*/
        sMoHold.wrk_date = g_pub_tx.tx_date;    /* �������� */
        sMoHold.trace_no = g_pub_tx.trace_no;    /* ��ˮ�� */
        strcpy( sMoHold.hold_brf,"ǩ���жһ�Ʊ�Զ�����");

TRACE
        ret = Mo_hold_Ins( sMoHold, g_pub_tx.reply );
        if( ret )
        {
            sprintf( acErrMsg, "ִ��Mo_hold_Ins��[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        ret = Impa_dep_reg_Upd_Upd( sImpaDepReg, g_pub_tx.reply );
            if ( ret )
        {
            sprintf( acErrMsg, "ִ��Impa_dep_reg_Upd_Upd��[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

        flag++;
    }

TRACE
    Impa_dep_reg_Clo_Upd( );

    if(sMoBankAccPo.acc_po_sts[0] == '9')    /** ���л�Ʊ **/
            parm_ind = 6;
    else{
            switch(sMoBankAccPo.acc_type[0])
            {
                case '1':    /** ���ж� **/
                    parm_ind = 1;
                    break;
                case '2':    /** ��ѹ�浥 **/
                    parm_ind = 2;
                    break;
                case '3':    /** 100%��֤�� **/
                    parm_ind = 3;
                    break;
                case '4':    /** �����ж� **/
                    parm_ind = 4;
                    break;
                case '5':    /** ��Ʊ��СƱ **/
                    parm_ind = 5;
                    break;
                default:
                    sprintf( acErrMsg, "[%s][%d]��Ʊ���ʹ���![%s]",__FILE__,__LINE__,sMoBankAccPo.acc_type);
                    WRITEMSG
                    goto ErrExit;
                
            }
    }
    /** �Ӳ�����com_parmȡ�ò�ͬ���ͳжһ�Ʊ�Ŀ�Ŀ **/
    ret = Com_parm_Sel(g_pub_tx.reply,&wd_com_parm,"parm_code='CDHPR' and parm_seqn=%d",parm_ind);   
    if(ret)
    {
            sprintf( acErrMsg, "[%s][%d]��ѯ���������![%d]",__FILE__,__LINE__, ret );
            WRITEMSG
            goto ErrExit;
    }

    /** Ϊ����ˮ��׼�� * xxx --- ��Ϊ���ñ�������ӽ��� ---**
    g_pub_tx.svc_ind = 0;  
    strcpy( g_pub_tx.cur_no, "01" );     * ���� *
    strcpy( g_pub_tx.ac_no, "60201" );     * ���⣺�жһ�Ʊ *
    g_pub_tx.ac_seqn = 0;                  * �˻���� *
    g_pub_tx.ac_id = 0;                  * �˻�ID *
    g_pub_tx.tx_amt1 = sMoBankPo.po_amt;    * �жһ�ƱƱ���� *
    strcpy( g_pub_tx.note_type, sComParm.val );
    strcpy( g_pub_tx.beg_note_no, cPoNo2 );
    strcpy( g_pub_tx.add_ind, "1" );    * ����:0��1�� *
    strcpy( g_pub_tx.ct_ind, "2" );        * ��ת:1��2ת *
    strcpy( g_pub_tx.brf, "����жһ�Ʊ" );* ��ע *
    strcpy(g_pub_tx.hst_ind,"1");         * �ռ�����ϸ *
    **----------------------------------------------------**/

    /* ����ˮ��־ */
    /**----- xxx ---- 20030819 ---- ��������Ҫ��Э���� ----**/
    strcpy(g_pub_tx.ac_no,cPactNo);
    g_pub_tx.tx_amt1 = sMoBankPo.po_amt;    /* �жһ�ƱƱ���� */
    strcpy( g_pub_tx.note_type, sComParm.val );
    memcpy( g_pub_tx.beg_note_no, cPoNo2,16 );
    g_pub_tx.svc_ind=0;
    strcpy(g_pub_tx.no_show,"1");
    ret = pub_ins_trace_log();
    if ( ret )
    {
        sprintf( acErrMsg, "����ˮ��־��" );
        WRITEMSG
        goto ErrExit;
    }

    sprintf( acErrMsg, "�жҿ�ĿΪ:[%s]",wd_com_parm.val );
    WRITEMSG

    /* �жһ�Ʊ����Ҫ��ӡ�Ĳ��̶ֹ����� */
    set_zd_data( "0640", wd_com_parm.val );        /* �жһ�Ʊ���ʿ�Ŀ */
    set_zd_data( "0250", "��������" );        /* ������ȫ�� */
    set_zd_data( "0300", "401175056393");    /* �������к� */
    set_zd_data( "0820", "ɽ��ʡ�������ܴ�����˳�ǽ�129��");    /* �����е�ַ */
    
    /**�жһ�Ʊ��Ʊ�������ѡ�add by chenchao 2011-7-28 10:13:51 begin **/
    ret = sql_execute("update mo_bank_po_fee set sts='0' where pact_no='%s' and num=%ld ",cPactNo,iPactNoNum);
    if(ret)
    {
        sprintf( acErrMsg, "�޸ĳжҵǼǱ�ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /** Ϊ����ˮ��׼��**/
    /** ��ʼ���� ��������  **/
    set_zd_double("0390",0.00);
    set_zd_double("0400",0.00);
    cCt_ind[0] = '2';
    vtcp_log("[%s][%d] dFee_amt=[%.2lf]dPrm_amt[%.2lf] ",__FILE__,__LINE__,dFee_amt,dPrm_amt);
    if(cCt_ind[0] == '1')/** �ֽ� **/
    {
         vtcp_log("[%s][%d] ct_ind[%s] ",__FILE__,__LINE__,cCt_ind);
         set_zd_data("0680","1");/** �����ֽ�����  spM001 **/
         strcpy( g_pub_tx.reply, "0001" );
    }else if(cCt_ind[0] == '2')
    {
         vtcp_log("[%s][%d] ct_ind[%s] ",__FILE__,__LINE__,cCt_ind);
    	 strcpy( g_pub_tx.reply, "0002" );
         set_zd_data("0680","2");/** ����ת������  spD003 **/
         set_zd_data("0370",sMoBankAccPo.payer_ac_no);
    }else{
         sprintf( acErrMsg, "��ת��־����!ct_ind=[%s]", cCt_ind);
         WRITEMSG
         strcpy( g_pub_tx.reply, "B099" );
         goto ErrExit;
    }
    ret = sql_count("ln_gage_reg","pact_no='%s' and sts='X' ",cPactNo);
    if(ret)
    {
         sprintf( acErrMsg, "����Ѻ�ﻹû�����!ret=[%d]", ret);
         WRITEMSG
         strcpy( g_pub_tx.reply, "SN06" );
         goto ErrExit;
    }
OkExit:
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
