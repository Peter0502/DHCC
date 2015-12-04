/***************************************************************
* �� �� ��: spD003.c
* ����������ȡ��
* ��    ��: jack
* ������ڣ�2004��1��6��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "dc_acc_rel_c.h"
#include "card.h"
#include "card_reg_c.h"
#include "com_rate_c.h"


spD003()
{
    int  ret=0;
    char tmpname[101],ct_name[5],tmp_ac_no[7],term[3],term_type[3],id_name[21];
    char intst_type[17],memo[51],draw_type[21],auto_depo[9];
    char br_name[31],br_tele[21];
    char tmp_brf[21];
    FILE *fp;
    int  tmp_tx_date=0,TmpTerm=0,TmpAcSeqn=0;
    double tmp_tx_amt=0.00;
    struct S_pub_intst              TmpIntst;
    struct td_mst_c                 sTd_mst;
    struct cif_id_code_rel_c        g_cif_id_code_rel;
    struct dc_acc_rel_c             g_dc_acc_rel;
    char acc_no[8]; 
    struct com_rate_c sCom_rate;
    memset(&sCom_rate,0x00,sizeof(struct com_rate_c));

    memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
    memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
    memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
    memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
    memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
    memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&g_dc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
    memset(&TmpIntst,0x00,sizeof(struct S_pub_intst));
    memset(&g_pub_intst,0x00,sizeof(g_pub_intst) );
    memset(tmp_brf,0x00,sizeof(tmp_brf));
    memset(term,0x00,sizeof(term)); /**�����ڳ�ʼ��***/
    g_reply_int=0;
    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();
    get_zd_data("1021",g_pub_tx.ac_no);
    get_zd_int("1022",&g_pub_tx.ac_seqn);
    sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
    WRITEMSG

    pub_base_old_acno( g_pub_tx.ac_no );

    get_zd_data("1023",g_pub_tx.note_type);
    get_zd_data("1024",g_pub_tx.beg_note_no);
    get_zd_data("1024",g_pub_tx.end_note_no);
    get_zd_data("102T",g_pub_tx.new_note_no);
    get_zd_data("1025",g_pub_tx.draw_uncon_yn); 
    get_zd_data("1026",g_pub_tx.draw_pwd_yn);   
    get_zd_data("1027",g_pub_tx.qry_pwd);
    get_zd_data("1028",g_pub_tx.draw_pwd);
    get_zd_data("1029",g_pub_tx.draw_id_yn);    
    get_zd_data("102G",g_pub_tx.id_type);   
    get_zd_data("102A",g_pub_tx.id_no); 
    get_zd_data("102B",g_pub_tx.draw_seal_yn);  
    get_zd_data("102C",g_pub_tx.pwd_mach_yn);   
    get_zd_data("102D",g_pub_tx.draw_mach); 
    get_zd_long("102E",&g_pub_tx.mach_date);
    get_zd_double("102F",&g_pub_tx.tx_amt1);
    vtcp_log("gxjgxj----[%.2lf]",g_pub_tx.tx_amt1); 
        get_zd_data("102J",g_pub_tx.cur_no);
    get_zd_data("102K",g_pub_tx.ct_ind);
    /*�жһ�Ʊ�����������������Ϊ0.00��ֱ������ ,20150505 wudawei*/
    if((g_pub_tx.tx_amt1==0.00) && memcmp(g_pub_tx.tx_code,"4380",4)==0)
    {
    	 vtcp_log("[%s][%d] �жһ�Ʊ�����������������Ϊ0.00��ֱ��������g_pub_tx.tx_code=[%s],g_pub_tx.tx_amt1=[%f]\n",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.tx_amt1);
    	 goto OkExit;
    }
    /** 
    get_zd_data("102Y",g_pub_tx.brf);**/
    get_zd_data("102Y",tmp_brf);
    vtcp_log("AAAAAAAAAAAAAAAAA brf=[%s]\n",tmp_brf);
    strcpy(g_pub_tx.brf,tmp_brf);   
    vtcp_log("AAAAAAAAAAAAAAAAA brf=[%s]\n",g_pub_tx.brf);
    sprintf(acErrMsg,"cur_no=[%s],ct_ind=[%s]",g_pub_tx.cur_no,g_pub_tx.ct_ind);
    WRITEMSG
    get_zd_data("102O",g_pub_tx.prt_ind); /*�Ƿ���۱�־*/
    vtcp_log("[%s][%d] ���۱�־=[%s]\n",__FILE__,__LINE__,g_pub_tx.prt_ind);
    vtcp_log("[%s][%d] �˶�ƾ֤��־��־=[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_wrk_ind[0]);
    
    /**Add by hxc091202 ATMת��ת��ʱ��������Ϊ����ֱ�ӷ��أ�������ȡ��׵Ĵ���**/
    vtcp_log("[%s][%d]  [%s]������=[%.2lf]\n",__FILE__,__LINE__,g_pub_tx.brf,g_pub_tx.tx_amt1);
    if((g_pub_tx.tx_amt1==0.00) && (!memcmp(g_pub_tx.brf,"ATM������",9)))
        {
            vtcp_log("[%s][%d]  [%s]������=[%.2lf],ֱ�ӷ���\n",__FILE__,__LINE__,g_pub_tx.brf,g_pub_tx.tx_amt1);
            strcpy(g_pub_tx.reply,"0000");
            sprintf(acErrMsg,"������Ϊ��,�ɹ�����![%s]",g_pub_tx.reply);
            WRITEMSG
            set_zd_data(DC_REPLY,g_pub_tx.reply);
            return 0; 
        }
    /**End by hxc091202 ATMת��ת��ʱ��������Ϊ����ֱ�ӷ��أ�������ȡ��׵Ĵ���**/

    if( !strlen(g_pub_tx.brf) )
    {
        if(!strncmp(g_pub_tx.tx_code,"2430",4))/*add by martin 2009/6/30 16:29:02 �ۻ���brf�޸�*/
            strcpy( g_pub_tx.brf,"�ۻ�" );
        else
            strcpy( g_pub_tx.brf,"ȡ��" );
    }

    if(g_pub_tx.ct_ind[0]=='1')
        strcpy( memo,"ȡ��" );
    else 
        strcpy( memo,"תȡ" );

    strcpy(g_pub_tx.add_ind,"0"); /**����**/
    strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
    
    if (g_pub_tx.ct_ind[0]!='1' && g_pub_tx.ct_ind[0]!='2' 
        && g_pub_tx.ct_ind[0]!='3' && g_pub_tx.ct_ind[0]!='4' )
    {
        vtcp_log("CCCCC[%s]",g_pub_tx.ct_ind);
        TRACE
        strcpy( g_pub_tx.reply,"P156" );
        goto ErrExit;
    }
    if (g_pub_tx.ct_ind[0] != '1')
    {
        strcpy(g_pub_tx.ct_ind,"2");
    }
    /**���ƾ֤���㲻����ˮ���㲻����ϸ�����֧ȡ��ʽ**/
    strcpy(g_pub_tx.ac_wrk_ind,"1001"); 
    g_pub_tx.svc_ind=1001; /**��ȡ��***/

    get_zd_data("0191",g_pub_tx.using_ind);
    if( g_pub_tx.using_ind[0]=='1' )
    {
        get_zd_data("0192",g_pub_tx.ac_wrk_ind);
        get_zd_int("0193",&g_pub_tx.svc_ind);
    }
    /**ADD BY ZHGF 20100607 ����ͨ������ôͬҵ�������벢��ͨ��,�������⴦��һ��***/
    if(memcmp(g_pub_tx.tx_code,"6203",4)==0
       && g_pub_tx.ac_no[0]=='5'){
        strcpy(g_pub_tx.ac_wrk_ind,"100010001");
    }
    /**ADD BY ZHGF 20100607 ����ͨ�������⴦��һ��***/
    sprintf(acErrMsg,"BEG D003[%s][%s][%s][%d]",g_pub_tx.ac_no,
        g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
    WRITEMSG
    
    /*���ӿۻ����⴦�� 2011/11/13 13:15:37*/
    pub_base_strpack(g_pub_tx.brf);
    if (memcmp(g_pub_tx.tx_code, "4713", 4) == 0) 
    {
        memcpy(g_pub_tx.ac_wrk_ind, "1000", 4);
        g_pub_tx.ac_wrk_ind[9]='1';
        sprintf(acErrMsg, "BEG D003LM2[%s][%s][%s][%d][%s]", g_pub_tx.ac_no,
        g_pub_tx.using_ind, g_pub_tx.ac_wrk_ind, g_pub_tx.svc_ind, g_pub_tx.brf);
    }
    
    /** ���� **/

    TmpAcSeqn = g_pub_tx.ac_seqn;
    
    sprintf( acErrMsg, "-------------->����ȥ��ƾ֤����Ϊ[%s]", g_pub_tx.note_type );
    WRITEMSG

    sprintf(acErrMsg, "*********��������[%ld]", g_pub_tx.tx_date);
    WRITEMSG
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,
        g_pub_tx.svc_ind);
    WRITEMSG

    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }

    vtcp_log("++++++++++++++++++++++++++++NEW_AC_NO====[%s]",g_pub_tx.ac_no1);

    /*** ��ӡ�ο�ʼ ***/
    pub_base_AllDwnFilName( tmpname );
    fp=fopen(tmpname,"a");

    /*** ����һ��ͨ���� ***/
    if(!strncmp(g_pub_tx.mdm_code,"0016",4))
        strcpy(g_pub_tx.ac_no1,g_pub_tx.ac_no);
    
    if(memcmp(g_pub_tx.tx_code, "4713", 4)) /* ���ӿۻ����⴦�� */
    {
        /* ������ begin add by LiuHuafeng 20070205*/
        if((memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) ==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) == 0)
        && !memcmp(g_pub_tx.ac_no+6,CARD_UNION_AREA,2))
        {
            struct card_reg_c sCard_reg;
            memset(&sCard_reg, 0 , sizeof(sCard_reg));
            memcpy(sCard_reg.ac_no,g_pub_tx.ac_no,CARD_NO_LEN);
            /* ���2�ŵ���Ϣ */ 
            g_reply_int = pubf_card_chk_msr(1,g_pub_tx.ac_no);
            if(g_reply_int)
            {
                sprintf(acErrMsg,"���2�ŵ���Ϣʧ��!");
                WRITEMSG
                if(memcmp(g_pub_tx.reply,"0000",4)==0)
                    strcpy( g_pub_tx.reply,"M003" );
                goto ErrExit;
            }
        }
        /* end by LiuHuafeng 20070205 */
    }
    

    if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && g_mdm_ac_rel.draw_id_yn[0]=='N')
        strcpy(draw_type,"�� ƾ��֧ȡ ��");
    else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N' && g_mdm_ac_rel.draw_id_yn[0]=='Y')
        strcpy(draw_type,"�� ֤��֧ȡ ��");
    else if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' &&g_mdm_ac_rel.draw_id_yn[0]=='Y')
        strcpy(draw_type,"�� �����֤��֧ȡ ��");
    else 
        strcpy(draw_type,"");

    pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);

    if(g_pub_tx.prdt_code[0]=='1')
    {
        /*** ȡ֤����Ϣ ***/
        ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
                                  "cif_no=%ld", g_dd_mst.cif_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"�ÿͻ�������֤����Ϣ!");
            WRITEMSG
            strcpy( g_pub_tx.reply,"M003" );
            goto ErrExit;
        }
        else if( ret )
        {
            sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ����");
            WRITEMSG
            strcpy( g_pub_tx.reply,"D103" );
            goto ErrExit;
        }

        pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

        g_pub_tx.tx_amt2=g_dd_mst.bal;

        set_zd_double("102I",g_dd_mst.bal);

        pub_base_getbrname(g_dd_mst.opn_br_no,br_name);
        pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
        pub_base_strpack(br_name);
        pub_base_strpack(br_tele);
        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_dd_mst.name);
        pub_base_strpack(g_dd_parm.title);
        if(  pub_base_prdt_accno( g_dd_mst.prdt_no,acc_no ) )
            strcpy( acc_no,"" );

        /***ȡ������***/
        ret=Com_rate_Sel(g_pub_tx.reply,&sCom_rate,"cur_no='01' and end_date=99999999 and rate_code='%s'",g_dd_parm.rate_no);
        if( ret)
        {
            sprintf(acErrMsg,"ȡ���ʴ���!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
        g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
        g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.cif_no,
        0,"","",sCom_rate.rate_val,g_dd_mst.bal,"","","","","",memo,"",0.00,
        ct_name,g_pub_tx.ac_seqn,0,0,g_dd_mst.prdt_no,0,g_pub_tx.brf,
        "","","",id_name,g_cif_id_code_rel.id_no,br_name,br_tele,
        g_mdm_ac_rel.note_no,"",acc_no);

        set_zd_double("102I",g_dd_mst.bal);
    }
    else if(g_pub_tx.prdt_code[0]=='2')
    {
        /*** ȡ֤����Ϣ ***/
        ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
                                  "cif_no=%ld", g_td_mst.cif_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"�ÿͻ�������֤����Ϣ!");
            WRITEMSG
            strcpy( g_pub_tx.reply,"M003" );
            goto ErrExit;
        }
        else if( ret )
        {
            sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ����");
            WRITEMSG
            strcpy( g_pub_tx.reply,"D103" );
            goto ErrExit;
        }
        /***�õ���Ϣ��Ŀ add by wanglei 20060927***/
        ret = Dc_acc_rel_Sel(g_pub_tx.reply,&g_dc_acc_rel,"dc_code='%s' and data_code='0153'",g_pub_tx.dc_code);
        if(ret)
        {
            sprintf(acErrMsg,"ȡ��Ʒ��Ϣ��Ŀ��!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"B345");
            goto ErrExit;
        }

        pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

        g_pub_tx.tx_amt2=g_td_mst.bal;

        if(g_td_parm.term_type[0]=='Y')
            TmpTerm=g_td_parm.term*12;
        else 
            TmpTerm=g_td_parm.term;

        /***��鲿���־***/
        if(g_td_parm.wdr_ind[0]=='Y')
        {
            strcpy(memo,"����");
            /*** �´���ƾ֤���� ***/
/*vtcp_log("11111111111111111113[%s]",g_pub_tx.note_type);*/
            set_zd_data("102L",g_pub_tx.note_type);
        }

        sprintf(term,"%02d",TmpTerm);
        /***�˴����Ӵ��ڵ�λ add by wanglei 20060921***/
        if(g_td_parm.term_type[0] == 'Y')
            strcpy(term_type,"��");
        else if(g_td_parm.term_type[0] == 'M')
            strcpy(term_type,"��");
        else if(g_td_parm.term_type[0] == 'D')
            strcpy(term_type,"��");

        pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
        pub_base_getbrname(g_td_mst.opn_br_no,br_name);
        pub_base_strpack(br_name);
        pub_base_strpack(br_tele);
        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_td_mst.name);
        pub_base_strpack(g_td_parm.title);
        pub_base_dic_show_str(intst_type,"intst_knd",g_td_mst.intst_type);

        /***������Ϣ��Ŀ add by wanglei 20060927***/
        if( pub_base_CompDblVal(g_pub_intst.tax_intst,g_pub_intst.dealval+g_pub_intst.keepval)>=0 )
        {
        fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|5.00|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%.2lf|%s|%s|\n",
        g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,
        g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no,
        g_td_mst.bal,g_pub_intst.dealval+g_pub_intst.keepval,
        g_pub_intst.val,g_pub_intst.dealval+g_pub_intst.keepval,
        g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_td_mst.intst_acm,intst_type,memo,g_pub_tx.ac_seqn,term,
        g_mdm_ac_rel.prt_line,g_pub_tx.brf,g_pub_tx.ac_no1,g_td_mst.book_line,
        g_td_mst.mtr_date,TmpAcSeqn,g_td_mst.rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);
        }
        else
        {
        fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|5.00|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%.2lf|%s|%s|\n",
        g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,
        g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no,
        g_td_mst.bal,g_pub_intst.dealval+g_pub_intst.keepval,
        g_pub_intst.val,g_pub_intst.tax_intst,
        g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_td_mst.intst_acm,intst_type,memo,g_pub_tx.ac_seqn,term,
        g_mdm_ac_rel.prt_line,g_pub_tx.brf,g_pub_tx.ac_no1,g_td_mst.book_line,
        g_td_mst.mtr_date,TmpAcSeqn,g_td_mst.rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);
        }
        if( g_pub_tx.ac_seqn )
        {
            ret=Td_mst_Dec_Upd( g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
                                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            if( ret )
            {
                sprintf(acErrMsg,"ȡ�������ļ�����[%ld][%d][%d]", \
                        g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
                WRITEMSG
                strcpy( g_pub_tx.reply,"O013" );
                goto ErrExit;
            }
            memset(&sTd_mst,0x00,sizeof(struct td_mst_c));
            ret=Td_mst_Fet_Upd(&sTd_mst,g_pub_tx.reply);
            if( ret )
            {
                sprintf(acErrMsg,"ȡ�������ļ�����[%ld][%d][%d]", \
                        g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
                WRITEMSG
                strcpy( g_pub_tx.reply,"O013" );
                goto ErrExit;
            }
            sTd_mst.book_line=g_mdm_ac_rel.prt_line;
            ret=Td_mst_Upd_Upd(sTd_mst,g_pub_tx.reply);
            if( ret )
            {
                sprintf(acErrMsg,"ȡ�������ļ�����[%ld][%d][%d]", \
                        g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
                WRITEMSG
                strcpy( g_pub_tx.reply,"O013" );
                goto ErrExit;
            }
            Td_mst_Clo_Upd();
        }

        if( g_td_parm.td_type[0]!='4' )
        {
            tmp_tx_date=g_pub_tx.tx_date;
            tmp_tx_amt=g_pub_tx.tx_amt1;
            g_pub_tx.tx_date=g_td_mst.mtr_date;
            g_pub_tx.tx_amt1=g_td_mst.bal;
vtcp_log("11111111111111111116[%lf][%ld][%ld]",g_td_mst.bal,g_td_mst.opn_date,g_td_mst.ic_date);
            g_reply_int = pub_base_CalIntstTd(&TmpIntst.dealval, \
                                      &TmpIntst.factval, \
                                          &TmpIntst.val, \
                                          &TmpIntst.keepval,g_pub_tx.reply);
            g_pub_tx.tx_date=tmp_tx_date;
            g_pub_tx.tx_amt1=tmp_tx_amt;
            if( g_reply_int ) {
            sprintf(acErrMsg,"ȡ������Ϣֵ��[%.2lf]��",TmpIntst.dealval);
            WRITEMSG
            strcpy( g_pub_tx.reply,"O194" );
            goto ErrExit;
            }
        }
        else
        {
            TmpIntst.dealval=0.00;
            TmpIntst.factval=0.00;
            TmpIntst.val=0.00;
            TmpIntst.keepval=0.00;
        }

        if( g_td_mst.tfr_ind[0]=='Y' )
            strcpy(auto_depo,"�� �Զ�ת�� ��");
        else
            strcpy(auto_depo,"");
        if(  pub_base_prdt_accno( g_td_mst.prdt_no,acc_no ) )
            strcpy( acc_no,"" );
        fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
        g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,
        g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no,
        g_td_mst.cif_no,g_td_mst.mtr_date,term,term_type,g_td_mst.rate,
        g_td_mst.bal,"",draw_type,"","","",memo,g_pub_tx.ac_no1,
        TmpIntst.dealval+TmpIntst.keepval,ct_name,g_pub_tx.ac_seqn,
        g_td_mst.ic_date,g_mdm_ac_rel.prt_line,g_td_mst.prdt_no,
        g_td_mst.book_line,g_pub_tx.brf,"","","",id_name,
        g_cif_id_code_rel.id_no,br_name,br_tele,g_mdm_ac_rel.note_no,
        auto_depo,acc_no);
    }
    fclose(fp);
    set_zd_data(DC_FILE_SND,"1");
    /*��ӡ����*/

    set_zd_data("102H",g_pub_tx.name);
    set_zd_double("102I",g_pub_intst.val);
    set_zd_data("102W",g_pub_tx.ac_no1);
    set_zd_data("102J",g_pub_tx.cur_no);
    set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);
    set_zd_double("102Q",g_pub_intst.val);
    set_zd_double("102R",g_pub_intst.keepval);
    set_zd_double("102S", g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
    /* set_zd_long("0430",g_pub_tx.tx_amt2); */
    set_zd_double("102I",g_pub_tx.tx_amt2);

    /*---------add by dgf 20050706--begin-------*/
    if ( g_pub_tx.prt_ind[0] == '1') /*�Ƿ���۱�־*/
    {
        Gl_file_first = 0;
    }
    /*---------add by dgf 20050706--end---------*/

    sprintf(acErrMsg,"Ӧ����Ϣ[%.2lf] ˰[%.2lf] �ܸ�[%.2lf]",
            g_pub_intst.dealval+g_pub_intst.keepval, g_pub_intst.val,
    g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
    WRITEMSG

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ȡ��ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"[%s][%d]ȡ��ʧ��![%s]",__FILE__,__LINE__,g_pub_tx.reply);
    WRITEMSG
        if(memcmp(g_pub_tx.reply,"0000",4)==0 || g_pub_tx.reply[0]=='\0')
                strcpy(g_pub_tx.reply,"D103");
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
