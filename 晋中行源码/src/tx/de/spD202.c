/***************************************************************
* �� �� ��: spD202.c
* ����������������Ϣ
* ��    ��: jack
* ������ڣ�2004��7��6��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "dc_acc_rel_c.h"
#include "com_rate_c.h"
#include "com_chrg_rate_c.h"
#include "com_intst_tax_rate_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_put_intst_c.h"
#include "com_parm_c.h"
#include "card.h"
#include "mo_loss_c.h"

spD202()
{
    int ret=0;

    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
    memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
    memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
    memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
    memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
    memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
    g_reply_int=0;

TRACE
    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();
    set_zd_long("0460",g_pub_tx.tx_date);/**add by spj 20140110**/
    sub_get_data(); /*��ȡ�ϴ�����*/
    strcpy(g_pub_tx.cur_no, "01");

    /*���Ŵ���*/
    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_old_acno( g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
         strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);

    sprintf(acErrMsg, "CGH******����֤��Ϊ:[%s]", g_pub_intst.edu_ind);
    WRITEMSG
    ret=sub_ac();   /* �����״̬ */
    ERR_DEAL

    ret=sub_mst();
    ERR_DEAL


    /************************************************************
    g_pub_tx.tx_amt1 = g_pub_intst.dealval + g_pub_intst.keepval;
    if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
    {
        strcpy(g_pub_tx.ac_wrk_ind, "000000");
    }
    strcpy(g_pub_tx.brf, "��Ϣ");
    strcpy(g_pub_tx.add_ind,"1");
    strcpy(g_pub_tx.ct_ind, "2");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg, "CGH ********* [%s]", g_pub_tx.reply);
    WRITEMSG

    g_pub_tx.tx_amt1 = g_pub_intst.val;
    if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
    {
        strcpy(g_pub_tx.ac_wrk_ind, "000000");
    }
    strcpy(g_pub_tx.brf, "��Ϣ˰");
    strcpy(g_pub_tx.add_ind,"0");
    strcpy(g_pub_tx.ct_ind, "2");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }
    **************************************************/

    get_zd_double("102I", &g_pub_tx.tx_amt1);
    if( sub_wrt_file() )
        goto ErrExit;

    /* set_zd_double("102I",g_pub_tx.tx_amt1);  */
    sprintf(acErrMsg, "********�˻����:[%lf]", g_pub_tx.tx_amt1);
    WRITEMSG
    /* set_zd_double("102I", 
    g_pub_tx.tx_amt1+g_pub_intst.factval+g_pub_intst.keepval); */
    set_zd_data(DC_CODE,g_pub_tx.dc_code);  
    set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);  /*��Ϣ�ϼ�*/
    set_zd_double("102Q",g_pub_intst.val);                          /*�� Ϣ ˰*/
    set_zd_double("102R",g_pub_intst.keepval);                      /*��ֵ��Ϣ*/
    set_zd_double("102S",
    g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
                                                                    /*Ӧ���ܶ�*/
    sprintf(acErrMsg, "[%s][%d]���׽��:[%f]", __FILE__,__LINE__,g_pub_tx.tx_amt1);
    WRITEMSG

    if (g_pub_tx.prdt_code[0]=='1')
    {
        sprintf(acErrMsg, "----------------> �˻�״̬:[%s]", g_dd_mst.ac_sts);
        WRITEMSG
    }
    else if (g_pub_tx.prdt_code[0] == '2')
    {
        sprintf(acErrMsg, "----------------> �˻�״̬:[%s]", g_td_mst.ac_sts);
        WRITEMSG
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������Ϣ�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    /*
    strcpy(g_pub_tx.reply,"A033");
    */
    sprintf(acErrMsg,"������Ϣʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
sub_get_data()
{
    get_zd_data("1021",g_pub_tx.ac_no);
    get_zd_int("1022",&g_pub_tx.ac_seqn);
    vtcp_log("AKKKING---------------------AC_SEQN=[%d]",g_pub_tx.ac_seqn);
    get_zd_data("1023",g_pub_tx.note_type);
    strcpy( g_pub_tx.note_type,"" );
    get_zd_data("1024",g_pub_tx.beg_note_no);
    strcpy( g_pub_tx.beg_note_no,"" );
    get_zd_data("1024",g_pub_tx.end_note_no);
    get_zd_data("1025",g_pub_tx.draw_uncon_yn); 
    get_zd_data("1026",g_pub_tx.draw_pwd_yn);   
    get_zd_data("1027",g_pub_tx.qry_pwd);
TRACE
    get_zd_data("1028",g_pub_tx.draw_pwd);
    get_zd_data("1029",g_pub_tx.draw_id_yn);    
    get_zd_data("102G",g_pub_tx.id_type);
    get_zd_data("102A",g_pub_tx.id_no); 
    get_zd_data("102B",g_pub_tx.draw_seal_yn);  
    get_zd_data("102C",g_pub_tx.pwd_mach_yn);   
    get_zd_data("102D",g_pub_tx.draw_mach); 
    get_zd_long("102E",&g_pub_tx.mach_date);
    get_zd_double("102I",&g_pub_tx.tx_amt1);
    get_zd_data("102X",g_pub_intst.edu_ind); /**����֤�� 1��**/
    sprintf(acErrMsg, "CGH******����֤��Ϊ:[%s]", g_pub_intst.edu_ind);
    WRITEMSG
        get_zd_data("102J",g_pub_tx.cur_no);
        /*get_zd_data("102K",g_pub_tx.ct_ind); */
    g_pub_tx.ct_ind[0] = '2';
    get_zd_data("102O",g_pub_tx.prt_ind);   /*�Ƿ���۱�־*/
    strcpy(g_pub_tx.prt_ind, "0");
        /*get_zd_data("102Y",g_pub_tx.brf);         *ժҪ����*/

    strcpy( g_pub_tx.brf,"��Ϣ" );

    strcpy(g_pub_tx.add_ind,"0");
        g_pub_tx.svc_ind = 1012;
        strcpy(g_pub_tx.hst_ind,"1"); /* �ռ�����ϸ */

    get_zd_data("0191",g_pub_tx.using_ind);
    if( g_pub_tx.using_ind[0]=='1' )
    {
        get_zd_data("0192",g_pub_tx.ac_wrk_ind);
        get_zd_int("0193",&g_pub_tx.svc_ind);
    }
}
int sub_wrt_file()
{
    char tmpname[100],intst_type[17],ct_name[5],term[3],memo[5];
    FILE *fp;
    char ct_ind[2];
    int ret = 0;
    double Td_rate=0.00;
    double Tax_rate=0.00;
    struct dc_acc_rel_c g_dc_acc_rel;
    struct com_rate_c sCom_rate;
    
    struct com_intst_tax_rate_c sCom_intst_tax_rate; /**Add by hxc 091101 ���ڷ���˰��**/
    
  memset(&sCom_rate,0x00,sizeof(struct com_rate_c));
    memset(&sCom_intst_tax_rate,0x00,sizeof(struct com_intst_tax_rate_c));
    memset(ct_ind, 0x0, sizeof(ct_ind));
    memset(&g_dc_acc_rel, 0x0, sizeof(struct dc_acc_rel_c));
  memset(term,0x00,sizeof(term));
    strcpy(ct_ind, g_pub_tx.ct_ind);
    get_zd_data("102K",g_pub_tx.ct_ind); 
        sprintf(acErrMsg, "hehe:[%s]", g_pub_tx.ct_ind);
    WRITEMSG
    if (g_pub_tx.ct_ind[0]!='1' && g_pub_tx.ct_ind[0]!='2' 
        && g_pub_tx.ct_ind[0]!='3' && g_pub_tx.ct_ind[0]!='4' )
    {
        vtcp_log("CCCCC[%s]",g_pub_tx.ct_ind);
        TRACE
        strcpy( g_pub_tx.reply,"P156" );
        goto ErrExit;
    }

    if(g_pub_tx.ct_ind[0]=='2')
        strcpy( memo,"ת��" );
    else
        strcpy( memo,"����" );

    strcpy(g_pub_tx.ct_ind, ct_ind);
    /***�õ���Ϣ��Ŀ 20060926***/
    ret=Dc_acc_rel_Sel(g_pub_tx.reply,&g_dc_acc_rel,"dc_code='%s'and data_code='0153'",g_pub_tx.dc_code);
        if( ret )
        {   
            sprintf(acErrMsg,"TRANSFER DC_ACC_REL ERROR,CODE=[%d]!",ret);
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d] acc_hrt=[%s]\n",__FILE__,__LINE__,g_dc_acc_rel.acc_hrt);
    
    /***Add By Azz.Part For Print***/
    pub_base_AllDwnFilName( tmpname );
    fp=fopen(tmpname,"a");

    /**Add by hxc 091101 ����ȡ��˰��(��ӡ��10λΪ˰��)**/
      ret=Com_intst_tax_rate_Sel(g_pub_tx.reply,&sCom_intst_tax_rate,"cur_no='01' and tax_rate_code='01' and end_date=99999999");
        if( ret)
        {
            sprintf(acErrMsg,"ȡ˰�ʴ���!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        Tax_rate=sCom_intst_tax_rate.tax_rate_val*100;
        vtcp_log("[%s][%d]ȡ��˰��tax_rate_val=[%.21f],Tax_rate=[%.21f]",__FILE__,__LINE__,sCom_intst_tax_rate.tax_rate_val,Tax_rate);
        /**End by hxc 091101 ����ȡ��˰��(��ӡ��10λΪ˰��)**/

    pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
    if(g_pub_tx.prdt_code[0]=='1')
    {
        g_reply_int = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", \
                                  g_pub_tx.prdt_code);
        if( g_reply_int )
        {   
            sprintf(acErrMsg,"TRANSFER DD_PARM ERROR,CODE=[%d]!",g_reply_int);
            WRITEMSG
            goto ErrExit;
        }
        
        /***ȡ������***/
      ret=Com_rate_Sel(g_pub_tx.reply,&sCom_rate,"cur_no='01' and end_date=99999999 and rate_code='%s'",g_dd_parm.rate_no);
        if( ret)
        {
            sprintf(acErrMsg,"ȡ���ʴ���!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
				
				Td_rate=sCom_rate.rate_val;
        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_dd_mst.name);
        pub_base_strpack(g_dd_parm.title);
        pub_base_dic_show_str(intst_type,"intst_knd",g_dd_mst.intst_type);
        
    

        /***���ӷ�����Ϣ��Ŀ(g_dc_acc_rel.acc_hrt) add by wanglei 20060926***/
        /***���ӷ���˰��(Tax_rate) add by hxc 20091101***/
        if( pub_base_CompDblVal(g_pub_intst.tax_intst, \
                                g_pub_intst.dealval+g_pub_intst.keepval)>=0 )
        {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
         g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.val,Tax_rate,g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_dd_mst.intst_acm,intst_type,memo,g_pub_tx.ac_seqn,
       "",g_mdm_ac_rel.prt_line,g_pub_tx.brf,"",0,0,0,Td_rate,g_dd_parm.prdt_no,g_dc_acc_rel.acc_hrt);
       }
       else
       {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
         g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.val,Tax_rate,g_pub_intst.tax_intst,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_dd_mst.intst_acm,intst_type,memo,g_pub_tx.ac_seqn,
       "",g_mdm_ac_rel.prt_line,g_pub_tx.brf,"",0,0,0,Td_rate,g_dd_parm.prdt_no,g_dc_acc_rel.acc_hrt);
       }
    }
    else if(g_pub_tx.prdt_code[0]=='2')
    {
        sprintf(term,"%02d",g_td_parm.term);
        g_reply_int = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", \
                                  g_pub_tx.prdt_code);
        if( g_reply_int )
        {   
            sprintf(acErrMsg,"TRANSFER DD_PARM ERROR,CODE=[%d]!",g_reply_int);
            WRITEMSG
            goto ErrExit;
        }

        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_td_mst.name);
        pub_base_strpack(g_td_parm.title);
        pub_base_dic_show_str(intst_type,"intst_knd",g_td_mst.intst_type);
        /**���Ӷ�����������ʴ�ӡ���޸� add by martin 2009/3/27 14:09:59**/
        if(g_td_parm.td_type[0]=='6')
            {
                g_reply_int=pub_base_CalRateDHLB(&Td_rate);
                if( g_reply_int )
             {  
                sprintf(acErrMsg," pub_base_CalRateDHLB ERROR,CODE=[%d]!",g_reply_int);
                WRITEMSG
                goto ErrExit;
             }
             sprintf(acErrMsg," pub_base_CalRateDHLB OK������������,Td_rate=[%lf]!",Td_rate);
                WRITEMSG
            }else
            {
                Td_rate=g_td_mst.rate;
            }

        /***���ӷ�����Ϣ��Ŀ(g_dc_acc_rel.acc_hrt) add by wanglei 20060926***/
        /***���ӷ���˰��(Tax_rate) add by hxc 20091101***/
        vtcp_log("%s,%d g_pub_intst.tax_intst=[%lf],g_pub_intst.dealval+g_pub_intst.keepval=[%lf]\n",__FILE__,__LINE__,g_pub_intst.tax_intst,g_pub_intst.dealval+g_pub_intst.keepval);
        if( pub_base_CompDblVal(g_pub_intst.tax_intst,g_pub_intst.dealval+g_pub_intst.keepval)>=0 )
        {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
         g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,g_pub_intst.val,
         Tax_rate,g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
         g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_td_mst.intst_acm,intst_type,"����",g_pub_tx.ac_seqn,
        term,g_mdm_ac_rel.prt_line,g_pub_tx.brf,g_pub_tx.ac_no1,
        g_td_mst.book_line,g_td_mst.mtr_date,0,Td_rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);
        /*g_td_mst.book_line,g_td_mst.mtr_date,0,g_td_mst.rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);*/
        }
        else
        {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
         g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,g_pub_intst.val,
         Tax_rate,
         g_pub_intst.tax_intst,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_td_mst.intst_acm,intst_type,"����",g_pub_tx.ac_seqn,
        term,g_mdm_ac_rel.prt_line,g_pub_tx.brf,g_pub_tx.ac_no1,
        g_td_mst.book_line,g_td_mst.mtr_date,0,Td_rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);
        /*g_td_mst.book_line,g_td_mst.mtr_date,0,g_td_mst.rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);*/
        }
    }
    fclose(fp);
    set_zd_data(DC_FILE_SND,"1");

    return 0;
ErrExit:
    return 1;
}
sub_mst()
{
    int ret=0 ;

    /*** ��鴦��ֻ���,��ϸ�� ***/
    switch( g_pub_tx.ac_id_type[0] )
    {
        case '1':   
            ret=Intst_dd_mst();
            ERR_DEAL
            break;
        case '2':   
            ret=Intst_td_mst();
            ERR_DEAL
            break;
        case '3':   
            sprintf(acErrMsg,"������⴦��![%s]",g_pub_tx.ac_type);
            WRITEMSG
            break;
        case '9':   
            ret=Intst_in_mst();
            ERR_DEAL
            break;
        default:    
            sprintf(acErrMsg,"�޴��˺�����![%s]",g_pub_tx.ac_type);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P104");
            goto ErrExit;
    }   

    return 0;
ErrExit:
    return 1;
}
int Intst_dd_mst()
{
    int ret;
    double tmpje=0.00;
    double tmpbal=0.00;
    double    dIntst_sxc = 0.00;
    char      cSxc_flag[3];
    
    struct dd_mst_c TmpDd_mst;

    /*** ���û��ڲ�Ʒ���������麯�� ***/
    g_pub_tx.ac_seqn=1;

    /*** �������ļ���� ***/
    ret=Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id='%ld' and ac_seqn='%d' ", \
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d][%d]", \
                g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    /**��ȡ��Ʒ����**/
    ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm," prdt_no='%s' ",g_dd_mst.prdt_no );
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);

    strcpy(g_pub_tx.name,g_dd_mst.name);
    g_pub_tx.cif_no=g_dd_mst.cif_no;
    /**����״̬�ж�**/
    /**��У��֧ȡ��ʽ**/
    /**Э����������**/
    vtcp_log("��ǰlst_date[%ld],ic_date[%ld]\n",g_dd_mst.lst_date,g_dd_mst.ic_date);

        /*** ��������Ϣ�����ĺ��� ***/
    vtcp_log("��ǰlst_date[%ld],acm=[%.2lf]\n",g_dd_mst.lst_date,g_dd_mst.intst_acm);
        if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
            g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
            g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
            g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }
    vtcp_log("���lst_date[%ld],acm=[%.2lf]\n",g_dd_mst.lst_date,g_dd_mst.intst_acm);
         /* ���ü�Ϣ���� */
         if( g_dd_mst.intst_type[0]!='0' )
         {
            g_pub_intst.mode = 1;  /* ������Ϣ */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
               sprintf(acErrMsg,"������Ϣ����!!");
               WRITEMSG
               strcpy(g_pub_tx.reply,"A032");
               goto ErrExit;
            }
         }
         else
         {
            g_pub_intst.dealval=0.00;
            g_pub_intst.tax_intst=0.00;
            g_pub_intst.keepval=0.00;
            g_pub_intst.factval=0.00;
            g_pub_intst.val=0.00;
         }
        sprintf(acErrMsg,"����֤��[%s]",g_pub_intst.edu_ind);
        WRITEMSG
        sprintf(acErrMsg,"Ӧ����Ϣ[%.2lf] Ӧ˰��Ϣ[%.2lf] ��ֵ��Ϣ[%.2lf]",
            g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval );
        WRITEMSG
        sprintf(acErrMsg,"ʵ����Ϣ[%.2lf] ��Ϣ˰[%.2lf] ",
            g_pub_intst.factval, g_pub_intst.val );
        WRITEMSG
        
        /***������Ĵ���Ϣ add by zqbo 20141125***/
		    memset(cSxc_flag, 0x00, sizeof(cSxc_flag));
		    get_zd_data("100A", cSxc_flag);
		    vtcp_log("%s, %d, bbbbbbbb�����ߵ�����û20150917[%s] ",__FILE__,__LINE__,cSxc_flag);
		    if( strcmp(cSxc_flag, "sx") == 0 )
		    {
		        get_zd_double("100E", &dIntst_sxc);
		        if(pub_base_CompDblVal(dIntst_sxc, 0.00) > 0)
		        {
		            g_pub_intst.dealval += dIntst_sxc;
		        }
		        vtcp_log("%s, %d, aaaaaaaaaa�����ߵ�����û20150917[%lf][%lf] ",__FILE__,__LINE__,g_pub_intst.dealval,dIntst_sxc);
		    }
		    vtcp_log(" %s, %d, edu_ind[%s] ",__FILE__,__LINE__,g_pub_intst.edu_ind);
		    sprintf(acErrMsg, "Ӧ����Ϣ[%.2lf] Ӧ˰��Ϣ[%.2lf] ��ֵ��Ϣ[%.2lf]",g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval);
		    WRITEMSG
		    sprintf(acErrMsg, "ʵ����Ϣ[%.2lf] ��Ϣ˰[%.2lf] ",g_pub_intst.factval, g_pub_intst.val);
		    WRITEMSG

    /****��Ϣ����****
    if( pub_base_CompDblVal(g_pub_intst.dealval,0.00) ) 
    {
        g_pub_tx.tx_amt1=g_pub_intst.dealval;

        if( g_pub_tx.hst_ind[0]=='1' )      *�ռ�����ϸ*
        {
            g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
        }
    }
    **
    ****��Ϣ˰�۳�****
    if( pub_base_CompDblVal(g_pub_intst.val,0.00) ) 
    {
        g_pub_tx.tx_amt1=g_pub_intst.val;
    }
    **/


    /*** ��������Ϣ�����ĺ��� ***/
    g_dd_mst.lst_date=g_pub_tx.tx_date;

    /* 
    if( g_pub_tx.hst_ind[0]=='1' )      *�ռ�����ϸ*
    {
        ret=pub_ins_dd_hst();
        ERR_DEAL
    }
    *
    *******/
    vtcp_log("���lst_date[%ld],acm=[%.2lf]\n",g_dd_mst.lst_date,g_dd_mst.intst_acm);

    ret=Dd_mst_Dec_Upd(g_pub_tx.reply,
        "ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    ret=Dd_mst_Fet_Upd(&TmpDd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]���lst_date[%ld],TmpDd_mst.acm1=[%.2lf],afteracm=[%.2lf]\n",__FILE__,__LINE__,g_dd_mst.lst_date,TmpDd_mst.intst_acm,g_dd_mst.intst_acm);

      /*add by zgf 20140919 �������Ϊ2409����������  �򲻴���dd_mst״̬*/
		vtcp_log("[%s][%d]aaaaaatx_code[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
    if(!strcmp(g_pub_tx.tx_code,"2409"))
    {
			;
    }
    else
    {
    	strcpy(TmpDd_mst.ac_sts, "#");
    	strcpy(g_dd_mst.ac_sts, "#");
    }
	/*add end*/

    
    /***modified by ligl 2006-12-22 23:12 ��Ϊacct_trance ���˺����Ի��������� ����Ͳ��ټ���***/
    /**ԭ�����ǽ����������� ���� ��������,������ ������Ϣ����**/
    /*TmpDd_mst.intst_acm = g_dd_mst.intst_acm;*/
    /***end***/
    vtcp_log("[%s][%d]���lst_date[%ld],TmpDd_mst.acm1=[%.2lf],afteracm=[%.2lf]\n",__FILE__,__LINE__,g_dd_mst.lst_date,TmpDd_mst.intst_acm,g_dd_mst.intst_acm);
    ret=Dd_mst_Upd_Upd(TmpDd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    return 1;
}

int Intst_td_mst()
{
    int ret;
    struct td_mst_c TmpTd_mst;
    memset(&TmpTd_mst, 0x0, sizeof(struct td_mst_c));

    ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and \
        ac_seqn=%d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg, "��ѯ�������ļ�����!! [%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    
    ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'",
        g_td_mst.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg, "��ѯ���ڲ�Ʒ�����!! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    /**add by ligl 2006-9-15 20:55**/
    if(g_td_parm.td_type[0]=='5')/**���ΪЭ�����ʻ�,����ͬʱ�������*/
    {
     ret=deldb("ac_ac_rel","ac_id='%ld'",g_pub_tx.ac_id);
    }
    strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);

    strcpy(g_pub_tx.name,g_td_mst.name);
    g_pub_tx.cif_no=g_td_mst.cif_no;

    g_pub_tx.mach_date = g_td_mst.mtr_date;
    sprintf(acErrMsg, "CGH******����֤��Ϊ:[%s]", g_pub_intst.edu_ind);
    WRITEMSG

    /*** ���ü�����Ϣ�����ĺ��� **** ������ȡ �������� ��������*/ 
    if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' || 
        g_td_parm.td_type[0] == '7' ) 
    { 
        if( pub_base_CalAcm(g_td_mst.lst_date, g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date, 
            g_td_parm.intst_term_type,g_td_parm.acm_calc_mode, 
            g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm, 
            g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) ) 
        { 
            sprintf(acErrMsg,"������Ϣ��������!"); 
            WRITEMSG 
            goto ErrExit; 
        } 
    }
    sprintf(acErrMsg, "CGH *********����Ϊ:[%lf]", g_td_mst.intst_acm);
    WRITEMSG

    /* ���ü�Ϣ���� */ 
    /* ���ж������Ϣ��ʽ�ǲ���Ϣ ��ȥ������Ϣ wudawei 20140126 */
    if( g_td_mst.intst_type[0]!='0' )
    {
	    g_pub_intst.mode = 1;  /* ������Ϣ */ 
	    ret = pub_base_CalIntst(&g_pub_intst); 
	    if (ret != 0) 
	    { 
	        sprintf(acErrMsg,"������Ϣ����!!"); 
	        WRITEMSG 
	        strcpy(g_pub_tx.reply,"A032"); 
	        goto ErrExit; 
	    }
 		}
 		else
         {
            g_pub_intst.dealval=0.00;
            g_pub_intst.tax_intst=0.00;
            g_pub_intst.keepval=0.00;
            g_pub_intst.factval=0.00;
            g_pub_intst.val=0.00;
         }
         /* ���ж������Ϣ��ʽ�ǲ���Ϣ ��ȥ������Ϣ wudawei 20140126  end */
    sprintf(acErrMsg,"����֤��[%s]",g_pub_intst.edu_ind);
    WRITEMSG
    sprintf(acErrMsg,"Ӧ����Ϣ[%.2lf] Ӧ˰��Ϣ[%.2lf] ��ֵ��Ϣ[%.2lf]",
    g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval );
    WRITEMSG
    sprintf(acErrMsg,"ʵ����Ϣ[%.2lf] ��Ϣ˰[%.2lf] ",
    g_pub_intst.factval, g_pub_intst.val );
    WRITEMSG
    sprintf(acErrMsg, "CGH ---------->:[%lf]", g_pub_intst.tax_intst);
    WRITEMSG

    /****��Ϣ����**** 
    if( pub_base_CompDblVal(g_pub_intst.dealval,0.00) ) 
    { 
        g_pub_tx.tx_amt1=g_pub_intst.dealval; 
        
        if( g_pub_tx.hst_ind[0]=='1' )      *�ռ�����ϸ*
        { 
            g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1; 
        } 
    }
    ***/

    ret = Td_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and \
        ac_seqn = %d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg, "Declare for select error !! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Td_mst_Fet_Upd(&TmpTd_mst, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg, "Fetch for select error !! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(TmpTd_mst.ac_sts, "#");
    /* ������������������� modified by liuxuan 2007-7-30
    TmpTd_mst.intst_acm = g_td_mst.intst_acm;
    */
    ret = Td_mst_Upd_Upd(TmpTd_mst, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg, "UPDATE error !! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Td_mst_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    return 1;
}

int Intst_in_mst()
{
OkExit:
    return 0;
ErrExit:
    return 1;
}

static sub_ac()
{
    struct mdm_ac_rel_c v_mdm_ac_rel;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct prdt_ac_id_c s_prdt_ac_id;
    struct mdm_attr_c s_mdm_attr;
    struct mo_loss_c sMo_loss;
    char v_ac_get_ind[2];
    int ret=0;

    v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];

    vtcp_log("AKKKING2------------------------AC_SEQN==[%d]",g_pub_tx.ac_seqn);
    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
    {
        strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
    }

sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
    if( g_pub_tx.ac_get_ind[0]=='1' ) /**�Ѿ���ȡ�˽����˺�**/
    {
        if( g_mdm_ac_rel.ac_id<=0 ) 
        {
            sprintf(acErrMsg,"��ʶac_get_ind ����[%s]",
                g_pub_tx.ac_get_ind,g_mdm_ac_rel.ac_id);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
        strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
    }
    else
    {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
        /*** ����˺š��˺�ID���˺���� ***/
        if( g_pub_tx.ac_no[0]!=NULL )
        {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
            ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
                "ac_no='%s' ",g_pub_tx.ac_no);
            if( ret )
            {
                sprintf(acErrMsg,"��ʾ�˺Ų�����[%s][%s]",g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind);
                WRITEMSG
                if( ret==100 ) 
                {
                    if( g_pub_tx.ac_wrk_ind[6]=='1' )
                    {
                        strcpy(g_pub_tx.reply,"P102");
                        goto ErrExit2;
                    }   
                    strcpy(g_pub_tx.reply,"P102");
                }
                goto ErrExit;
            }
            if( g_pub_tx.ac_id && g_pub_tx.ac_id!=g_mdm_ac_rel.ac_id )
            {
                sprintf(acErrMsg,"�˺�[%s]���˺�ID[%ld]����һ����",
                    g_pub_tx.ac_no,g_pub_tx.ac_id);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
            strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
            g_pub_tx.ac_get_ind[0]='1';
        }
        else if( g_pub_tx.ac_id )
        {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
            /**û����ȡmdm_ac_rel,����������һ��**/
            g_pub_tx.ac_get_ind[0]='0';
            ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply,
                " ac_id=%ld ",g_pub_tx.ac_id );
            ERR_DEAL
            while(1)
            {
                ret=Mdm_ac_rel_Fet_Sel( &v_mdm_ac_rel,g_pub_tx.reply );
                if( ret==100 ) break;
                ERR_DEAL

                if( v_mdm_ac_rel.ac_seqn==g_pub_tx.ac_seqn )
                {
                    memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
                    g_pub_tx.ac_get_ind[0]='1';
                    break;
                }
                else if( v_mdm_ac_rel.ac_seqn==ALL_SEQN && 
                    (v_mdm_ac_rel.note_sts[0]=='0' || 
                    v_mdm_ac_rel.note_sts[0]=='1') 
                    && g_mdm_ac_rel.ac_id!=g_pub_tx.ac_id )
                {
                    g_pub_tx.ac_get_ind[0]='1';
                    memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
                }
            }
            if( g_pub_tx.ac_get_ind[0]=='0' )
            {
                sprintf(acErrMsg,"mdm_ac_rel����ƥ���¼acid=[%d]ac_seqn=[%d]",
                    g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn );
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
            strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
        }
        else
        {
            sprintf(acErrMsg,"�˺�[%s]���˺�ID[%ld]��Ϊ��",
                g_pub_tx.ac_no,g_pub_tx.ac_id);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P101");
            goto ErrExit;
        }
    }
    strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);

    /*** Add commence By Azz ***/
    if( g_mdm_ac_rel.coll_sts[0]=='1' )
    {
        if( pub_base_checkcollyn( g_mdm_ac_rel.ac_no ) )
        {
            sprintf(acErrMsg,"���ü�ʰ��麯������");
            WRITEMSG
            goto ErrExit;
        }
    }
    /*** complete ***/

    /**����Ѿ������������ҵ��ıȽ�һ��**/
sprintf(acErrMsg,"ac_seqn = [%d][%s] ",g_pub_tx.ac_seqn,g_pub_tx.cur_no);
WRITEMSG
    vtcp_log("AKKKING3------------------------AC_SEQN==[%d]",g_pub_tx.ac_seqn);

    if( g_pub_tx.ac_seqn>0 )
    {
        if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && 
            g_mdm_ac_rel.ac_seqn!=g_pub_tx.ac_seqn )
        {
            sprintf(acErrMsg,"mdm_ac_rel��Ų�ͬacid=[%d]seqn=[%d]in[%d]",
                g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.ac_seqn );
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    }
    else if( g_pub_tx.ac_seqn==0 )
    {
        ret = Mdm_attr_Sel( g_pub_tx.reply, &s_mdm_attr, "mdm_code='%s'" , \
                            g_mdm_ac_rel.mdm_code );
        if( ret==100 )
        {
            sprintf(acErrMsg,"���ʴ��벻����=[%s]",g_mdm_ac_rel.mdm_code);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        else if( ret )
        {
            sprintf(acErrMsg,"���ҳ���=[%s][%d]",g_mdm_ac_rel.mdm_code,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D103");
            goto ErrExit;
        }

        vtcp_log("AKKKING4---------------------s_mdm_attr.ac_num_ind==[%s]", \
                 s_mdm_attr.ac_num_ind );
        if( s_mdm_attr.ac_num_ind[0]=='N' )
        {
            ret = Prdt_ac_id_Sel( g_pub_tx.reply,&s_prdt_ac_id,"ac_id=%ld", \
                                  g_pub_tx.ac_id );
            if( ret==100 )
            {
                sprintf(acErrMsg,"�˺Ų�����=[%ld]",g_pub_tx.ac_id );
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            else if( ret )
            {
                sprintf(acErrMsg,"���Ҵ�=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"D103");
                goto ErrExit;
            }

            if( s_prdt_ac_id.ac_id_type[0]=='2' )
               g_pub_tx.ac_seqn = 200;
            else
               g_pub_tx.ac_seqn = s_prdt_ac_id.ac_seqn;
        }
    }

    /**����״̬���**/
    if( g_pub_tx.ac_get_ind[0]=='1' )
    {
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
        /*���ӿۻ����⴦��  ��ʧ������2011/11/13 13:45:36
        if( g_mdm_ac_rel.note_sts[0]=='1' && strcmp(g_pub_tx.tx_code,"Z016") )
        */
        /*begin IC�����ʱ����ֱ������20120618 add by wzw*/
        /**������Ҫ�� ����Ҳ�ܹ�ʧ����
         if(memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6))
        {
        	if( g_mdm_ac_rel.note_sts[0]=='1' && strcmp(g_pub_tx.tx_code,"Z016") && memcmp(g_pub_tx.tx_code,"4713",4))
        	{
        	    sprintf(acErrMsg,"�ý��ʴ��ڹ�ʧ״̬,������Ҵ���!");
        	    WRITEMSG
        	    strcpy(g_pub_tx.reply,"M009");
        	    goto ErrExit;
        	}
        }
        else
        {
        **/
        	if(g_mdm_ac_rel.note_sts[0]=='1')
        	{
        		memset(&sMo_loss,0x00,sizeof(struct mo_loss_c));
        		ret=Mo_loss_Sel(g_pub_tx.reply, &sMo_loss, "ac_no=%s and unloss_ind='0'", g_pub_tx.ac_no);
        		if(ret)
        		{
        			sprintf(acErrMsg,"��ѯ����!");
        			WRITEMSG
        			strcpy(g_pub_tx.reply,"D104");
        			goto ErrExit;
        		}
        		if(sMo_loss.loss_ind[0]!='2')
        		{
        			sprintf(acErrMsg,"������Ҵ���!");
        			WRITEMSG
        		  strcpy(g_pub_tx.reply,"M009");
        		  goto ErrExit;
        		}else
        		{/****�޸Ĺ�ʧ�Ǽǲ��������״̬��Ϊ'7'  20130702****/
        			sMo_loss.unloss_ind[0]='7';/****��ʧ����״̬****/
        			sMo_loss.unloss_date=g_pub_tx.tx_date;/****��ʧ��������****/
        			memcpy(sMo_loss.unloss_tel,g_pub_tx.tel,sizeof(sMo_loss.unloss_tel));/****��ʧ������Ա****/
        			ret=Mo_loss_Upd_Upd(sMo_loss,g_pub_tx.reply);
        			if(ret)
        			{
        				sprintf(acErrMsg,"�޸Ĺ�ʧ�Ǽǲ�����!");
        				WRITEMSG
        				strcpy(g_pub_tx.reply,"D105");
        				goto ErrExit;
        			}
        			vtcp_log("[%s][%d]sMo_loss.unloss_ind=[%c]",__FILE__,__LINE__,sMo_loss.unloss_ind[0]);
        			vtcp_log("[%s][%d]sMo_loss.unloss_date=[%ld]",__FILE__,__LINE__,sMo_loss.unloss_date);
        			vtcp_log("[%s][%d]sMo_loss.unloss_tel=[%s]",__FILE__,__LINE__,sMo_loss.unloss_tel);
        		}
        	}
        /** 
        }
        **/
        /*end*/
        if( g_mdm_ac_rel.note_sts[0]=='2' )
        {
            sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P171");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='3' )
        {
            sprintf(acErrMsg,"�ý����ѻ�֤!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M011");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='4' )
        {
            sprintf(acErrMsg,"���˻���������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M017");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='5' )
        {
            sprintf(acErrMsg,"���˻���������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M019");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='*' )
        {
            sprintf(acErrMsg,"�ý���������������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M012");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]!='0' && g_mdm_ac_rel.note_sts[0]!='1')
        {
            sprintf(acErrMsg,"�ý���״̬������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M013");
            goto ErrExit;
        }

        /*** ������ ***/
        if( g_pub_tx.add_ind[0]=='0' )
        {
            if( g_mdm_ac_rel.coll_sts[0]=='2' )
            {
                sprintf(acErrMsg,"�ý��ʴ����̿�״̬,�������黹!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M015");
                goto ErrExit;
            }
            else if( g_mdm_ac_rel.coll_sts[0]!='0' )
            {
                sprintf(acErrMsg,"�ý��ʼ�ʰ״̬������!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M016");
                goto ErrExit;
            }
        }
    }

    /*** ȡ�˻�����,��Ʒ���� ����Ѿ�ȷ���˾Ͳ�����***/
    if( g_pub_tx.ac_id_type[0]=='0' || g_pub_tx.ac_id_type[0]==NULL )
    {
        if( g_pub_tx.ac_seqn>0 )
            ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        else
            ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld ",g_pub_tx.ac_id);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",
                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
        strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
    }
    else
    {
        /**�Ѿ�ȷ������,������ȡ�˻����**/
        if( g_pub_tx.ac_seqn<=0 )
        {
            ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_id_type='%s' ",
                g_pub_tx.ac_id,g_pub_tx.ac_id_type);
            if( ret )
            {
                sprintf(acErrMsg,"�˺Ų�����![%ld][%d][%s]",
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id_type);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
            strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
        }
    }

    if( g_pub_tx.ac_id_type[0]=='9' ) /**�ڲ��ʣ����������**/
    {
        if( g_pub_tx.ac_seqn==0 )
            g_pub_tx.ac_seqn=SINGLE_SEQN;
    }
    sprintf(acErrMsg,"ac_no[%s] ac_id[%d] ac_seqn[%d] type[%s]",
        g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id_type);
    WRITEMSG

    g_pub_tx.ac_get_ind[0]=v_ac_get_ind[0];

    /*** û�����벻����ͨ�� ***/
    if( strcmp(g_pub_tx.tx_br_no,"99999") && g_pub_tx.ac_wrk_ind[8]!='1'
    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
    && g_mdm_ac_rel.draw_pwd_yn[0]!='Y' && g_mdm_ac_rel.pwd_mach_yn[0]!='Y'
        && (g_pub_tx.ac_id_type[0]=='1' || g_pub_tx.ac_id_type[0]=='2') 
        && g_pub_tx.add_ind[0]=='0' )
    {
        sprintf(acErrMsg,"�����ܻ�������ͨ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P412");
        goto ErrExit;
    }

GoodExit:
    return 0;
ErrExit:
    return 1;
ErrExit2:
    return 100;
}
/**********************************************************************
* ��������    pub_base_CalRateDHLB()
* �������ܣ�  ���������������
* ����/ʱ�䣺 2009��03��27��martin
*
* ������
*     ���룺
*     ���: ����          double
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_CalRateDHLB(double *rate)
{
    char  intst_term_type[2] ;/* ��Ϣ�������� */
    long  dayterm,monterm;
    char llbh[4];  /* ���ʱ�� */
  char cur_no[3];   /* ���� */ 
  double rate_val,rate_val1; /* ����ֵ */
    long tx_date;    /* �������� */
  long ic_date;    /* ��Ϣ���� */
  long date99,date9303,date9201,date8910;
  int ret;
  
  memset(cur_no, 0x0, sizeof(cur_no));
  strcpy(cur_no, g_td_parm.cur_no);
  ic_date = g_td_mst.ic_date;
  tx_date = g_pub_tx.tx_date;
  
  date99 = 19991101;
  date9303 = 19930301;
  date9201 = 19920101;
  date8910 = 19891001;
  (*rate) = 0.00;
    
    if (ic_date == 0 || tx_date == 0)
    {
        sprintf(acErrMsg,"��Ϣ��[%ld]������[%ld]Ϊ���޷���Ϣ",
                ic_date, tx_date);
        WRITEMSG
        memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
        strcpy(g_pub_tx.reply, "W112");
        return 1; 
    }
    
    strcpy(intst_term_type,g_td_parm.intst_term_type);/***��ʵ��������Ϣ***/
    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
        sprintf(acErrMsg,"��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
                g_td_parm.intst_term_type );
        WRITEMSG
        memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
        strcpy(g_pub_tx.reply, "W107");
        return 1;
    }
    ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"������������!!");
        WRITEMSG
        return 1;
    }
    
    monterm = dayterm / 30;
    memset(llbh,0x0,sizeof(llbh));
    
sprintf(acErrMsg,"��ʼ���㶨��������Ϣg_td_mst.ic_date==[%ld]",g_td_mst.ic_date);
WRITEMSG
  if (g_td_mst.ic_date >= date9303)
    {
        if (monterm < 3)
        {
            strcpy(llbh,HQLLBH);
        }      
        else if (monterm < 6)
        {
            strcpy(llbh,ZCZQ03);
            
        }
        else if (monterm < 12)
        {
            strcpy(llbh,ZCZQ06);
        }
        else 
        {
            strcpy(llbh,ZCZQ12);
        }
 sprintf(acErrMsg,"llbh =  [%s]",llbh);
 WRITEMSG

        ret = pub_base_getllz(HQLLBH,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
            WRITEMSG
            memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
            strcpy(g_pub_tx.reply,"W110");
            return ret;
        }

sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG

        if (strcmp(llbh,HQLLBH) != 0)
        {
            ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val1);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
                strcpy(g_pub_tx.reply,"W110");
                return ret;
            }
            rate_val1 = rate_val1 * 0.60; /*����ط�ԭ����ע�͵���*/
            if (rate_val <= rate_val1)
            {
                rate_val = rate_val1;
            }
        }
sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG
    }
    else if (g_td_mst.ic_date >= date9201)
    {
    }
    else if (g_td_mst.ic_date >= date8910)
    {
    }
    else 
    {
    }
    (*rate)=rate_val;
/* add over */
    return 0;
}
