/***************************************************************
* �� �� ��:     pubf_acct_trance.c
* ����������
*              1������˺š��˺�ID���˺���ŵĺϷ��ԣ�
*              2���ͻ�֧Ʊ���Ŵ���;
*              3������Ʒ����������˺����ļ����޸����ļ�ȫ�ֱ�����
*              4���Ǽ���ϸ�ʣ�
*              5�����ɴ����ļ���
*              6��ͨ���޸�407��ϸ�ʣ�
*              7����������Ǽǿ������Ǽǲ���
*              8���Ǽǽ�����ˮ��
**��־�÷���
**��Ҫ�˶�ƾ֤��** g_pub_tx.ac_wrk_ind[0]='1'
**                 g_pub_tx.hst_ind[0]=='1'�ռ�����ϸ
**                 g_pub_tx.prt_ind[0]!='1'�Ƿ����
*
* ��    ��: jack    
* ������ڣ�    2003��12��27��
*
* �޸ļ�¼��
*    ��    ��:20110827
*    �� �� ��:gongliangliang
*    �޸�����:����Ҫ�󵣱���֤��Ҳ����
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_unprt_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "tx_dc_br_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "card_consum_lmt_c.h"
#include "remark_ctl_c.h"
#include "auth_ac_rec_c.h"
#include "mo_hold_c.h"
#include "draw_loan_hst_c.h"
#include "draw_loan_limit_c.h"
/*#include "mo_ic_ac_rel_c.h"*/
#include "chnl_premise_c.h"                             /* add by yanqq 20111213 ׷��Ԥ��Ȩ���� */
#include "intst_hst_c.h"

struct mdm_attr_c v_mdm_attr;
struct prdt_ac_id_c prdt_ac_id_c;
struct mdm_attr_c mdm_attr_c;
struct note_parm_c note_parm_c;
struct card_consum_lmt_c g_card_consum_lmt;
static  struct ln_mst_c ln_mst_acm;
static  struct ln_mst_c o_td_mst;
static int wdr_flag=0;
int partflag=0;

pub_acct_trance()
{
    int ret=0;
    int ass_ind=0;
    int factday,lastday;
    struct pub_tx v_pub_tx;

    memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
    memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
    memset(&note_parm_c,0x00,sizeof(struct note_parm_c));
    /**��ʼ��***/
    partflag=0;
    wdr_flag=0;
    sprintf(acErrMsg,"[%s][%d]����pub_acct_trance ����svc_ind=[%d],get_ind[%s]",__FILE__,__LINE__,g_pub_tx.svc_ind,g_pub_tx.ac_get_ind);
    vtcp_log("zzzzzzzzzzzzz====g_pub_tx.ac_id_type=[%s],hst_ind[%s]ac_no=[%s]",g_pub_tx.ac_id_type,g_pub_tx.hst_ind,g_pub_tx.ac_no);
    WRITEMSG
    /**��ʼ�����ļ�����**/

    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
    {
         strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
    }
    if( g_pub_tx.ct_ind[0]!='1' ) g_pub_tx.ct_ind[0]='2';

    sprintf(acErrMsg,"g_pub_tx.acno= [%s] id=[%ld] seqn=[%d] g_pub_tx.opn_br_no [%s] g_pub_tx.tx_br_no [%s]", \
            g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.opn_br_no,g_pub_tx.tx_br_no);
    WRITEMSG
    sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
    ret=strlen(g_pub_tx.ac_no);
    if(ret==3||ret==5||ret==7)
    {
        /**�Կ�Ŀ����**/
        ret=Chg_acchrt_mst(&ass_ind);
        ERR_DEAL
    }
    else
    {
        /**��ʼ�����ļ�����**/
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d],[%s]",__FILE__,__LINE__,g_pub_tx.svc_ind,g_pub_tx.ac_get_ind);
    WRITEMSG
        ret=pub_sub_ac();
        if( ret==100 ) return 0; /***TEMP�����˺�***/
        ERR_DEAL

        /**����Ƿ���Э��**/
        vtcp_log("-==========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        ret=pub_sub_prdt_chk();
        ERR_DEAL

        /**ƾ֤����**/
         /**   add by shiweina   200711217  **/

  if(strcmp(g_pub_tx.tx_code,"2430")!=0)
   {
        vtcp_log("-========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        ret=pub_sub_note();
        ERR_DEAL
     }
             /*** add���˽��� **/

        /***��鴦��ֻ���***/
        vtcp_log("-========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        ret=pub_sub_mst();
        ERR_DEAL

        /***�����۴���***/
        vtcp_log("-=========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
        ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
        ERR_DEAL

        /***�����۴���***/
        ret=pub_sub_prt_book();
        ERR_DEAL
    }

    /***ͨ��ͨ�Ҵ���***/
    /**
    ret=pub_sub_liqu();
    ERR_DEAL
    **/

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG

    vtcp_log("[%s][%d]lovexier g_pub_tx.ac_id_type[0]==[%c],ass_ind===[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_id_type[0],ass_ind);
    vtcp_log("[%s][%d]lovexier g_pub_tx.ac_id_type[0]==[%c],ass_ind===[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_id_type[0],ass_ind);
    /***�Ǽǽ�����ˮ***/
    if( g_pub_tx.ac_id_type[0]!='3' )
    {
        /***�����ر���***/
        ret=pub_sub_spe_do();
        ERR_DEAL
    /**modified by ligl 2006-12-23 0:07 ���ܿ��˲����˶�����ˮ**/
        /*if( ass_ind==0 )      *** �ǿ��˿�Ŀ ***
        {*/
            vtcp_log("[%s][%d]���������￪ʼд��ˮ��!\n",__FILE__,__LINE__);
            if( pub_ins_trace_log() )
            {
                sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
                WRITEMSG
                goto ErrExit;
            }
        /*}*/
    }
sprintf(acErrMsg,"WDR����%d",wdr_flag);
WRITEMSG
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
    if( wdr_flag )
    {
        memcpy( &v_pub_tx,&g_pub_tx,sizeof( v_pub_tx ) );

        strcpy( g_pub_tx.beg_note_no,g_pub_tx.new_note_no );
        strcpy( g_pub_tx.end_note_no,g_pub_tx.new_note_no );
        strcpy( g_pub_tx.ac_wrk_ind,"0110000" );
        strcpy( g_pub_tx.brf,"���ỻ֤" );
        if( pub_acct_chg_mdm(10) )
        {
            sprintf(acErrMsg,"��������ʺŴ�" );
            WRITEMSG
            goto ErrExit;
        }

        strcpy( v_pub_tx.ac_no1,g_pub_tx.ac_no );
        strcpy( v_pub_tx.note_type,g_pub_tx.note_type);
sprintf(acErrMsg,"ac_no1[%s]",v_pub_tx.ac_no1 );
WRITEMSG
        memcpy( &g_pub_tx,&v_pub_tx,sizeof( v_pub_tx ) );
    }

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
     sprintf(acErrMsg,"dc_code!!!!!!=[%s]",g_pub_tx.dc_code );
     WRITEMSG

    set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ҵ����ϵͳ���˴���ɹ�!");
    WRITEMSG
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"ҵ����ϵͳ���˴���ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    return 1;
}


int Chg_dd_mst()
{
    int ret;
    double tmpje=0.00;
    double tmpbal=0.00;
    struct dd_mst_c TmpDd_mst;
    char msg[51];
    struct  remark_ctl_c rem;
    struct mo_hold_c   g_mo_hold;
    int chk_ctl_flag=0;
    char  Brf[21];
    char  cCmtno[4];
    char kh_type[2];
    char  minholdtyp[2];
    long   hold_seqn;
    long   ac_seqn;
    double ysqwc_rate=0.0; /*Ԥ��Ȩ���ʱ�������ɱ��� 20091117*/
    double zg_amt=0.0;

    memset(Brf,0,sizeof(Brf));
    memset(cCmtno,0,sizeof(cCmtno));
    memset(minholdtyp,0,sizeof(minholdtyp));
    memset(&rem,0x00,sizeof(struct remark_ctl_c));
    memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
    /*** ���û��ڲ�Ʒ���������麯�� ***/
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
    strcpy(Brf,g_pub_tx.brf);/**�漰������ժҪ����20140325**/
    vtcp_log("[%s][%d] ssssssBrf =[%s]",__FILE__,__LINE__,Brf);
    vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
    vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
    /*** �������ļ���� ***/
    if( g_pub_tx.ac_get_ind[1]=='1' )
    {
sprintf(acErrMsg,"dd_mst�Ѿ����˲���������[%s]",g_pub_tx.tx_code);
WRITEMSG
        ret=0;
    }
    else
    {
sprintf(acErrMsg,"dd_mst��ʼ����[%s]",g_pub_tx.tx_code);
WRITEMSG
        /*** xxxx Ϊ������ **
        ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
            "ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        ****************/
        ret=Dd_mst_Dec_Upd(g_pub_tx.reply,
            "ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    }
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"bal[%lf][%lf][%lf][%lf]",
        g_dd_mst.ctl_amt,g_dd_mst.hold_amt,g_pub_tx.tx_amt1,g_dd_mst.bal);
    WRITEMSG
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

    vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
    if( (!strcmp(g_dd_mst.prdt_no,"131")||!strcmp(g_dd_mst.prdt_no,"142")) /** add by chenchao 2011/8/27 18:50:07 ����Ҫ�󵣱���֤��Ҳ����***/
        && strcmp(g_pub_tx.tx_code,"4335") && strcmp(g_pub_tx.tx_code,"9001") 
        && strcmp(g_pub_tx.tx_code,"4334") && strcmp(g_pub_tx.tx_code,"G090") 
        && strcmp(g_pub_tx.tx_code,"4373") && strcmp(g_pub_tx.tx_code,"5801")
        && strcmp(g_pub_tx.tx_code,"5839") && strcmp(g_pub_tx.tx_code,"G037")
        && strcmp(g_pub_tx.tx_code,"2208") 
        && strcmp(g_pub_tx.tx_code,"4380") /*4380�жһ�Ʊ������������*/
        && strcmp(g_pub_tx.tx_code,"4964") /*4964��Ʊ֪ͨ*/
        && g_pub_tx.svc_ind != 1005  /*��Ϣ������**/
    )
    {
		/** �����3201 ����� 3202 ���Ϣ ���� ֧�ֻ�����ʱ�ۻ���֤�� ���Ϊ 20150305 **/
		if(strcmp(g_pub_tx.tx_code,"3201")!=0 && strcmp(g_pub_tx.tx_code,"3202")!=0&& strcmp(g_pub_tx.tx_code,"5111")!=0)
		{
			vtcp_log("�жұ�֤���ȥ�жұ�֤������ȥ��\n",__FILE__,__LINE__);
			vtcp_log("%s,%d txcode===[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
			strcpy(g_pub_tx.reply,"P471");
			goto ErrExit;
		}
    }

    /**��ȡ��Ʒ����**/
    if( g_pub_tx.ac_get_ind[2]=='1' )
        ret=0;
    else
        ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
            " prdt_no='%s' ",g_dd_mst.prdt_no );
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);

    strcpy(g_pub_tx.name,g_dd_mst.name);
    strcpy(g_pub_tx.opn_br_no,g_dd_mst.opn_br_no);
    g_pub_tx.cif_no=g_dd_mst.cif_no;

    /** yud ��Ӷ��ڽ��㻧����ǿ�пۿ��־ac_wrk_ind ***
    *** ��10λ����1ʱ���ڷǽ��㻧Ҳ���Կۿ� 20030819 **/
    /****NEWTCTD9:TODO:С��ͨ��ͨ������ǽ��㻧****/
    if( g_pub_tx.ct_ind[0]!='1' && g_dd_mst.sttl_type[0]=='S' &&
         g_pub_tx.ac_wrk_ind[7]!='J' && g_pub_tx.svc_ind != 1012 &&
         g_pub_tx.ac_wrk_ind[9]!='1' &&
        memcmp(g_pub_tx.tx_code,"5840",4)!=0)
    {
        sprintf(acErrMsg,"�ǽ��㻧������ת��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P105");
        goto ErrExit;
    }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

  /*Begin   Modified by ChenMing   725����ʱ�������Լ�����������  
    �յ�725ʱ���ͻ��ʺ�״̬���ܷ����˸ı䣬���Բ�����ʺŵ�״̬*/
    
  get_zd_data("0510",cCmtno);
  vtcp_log("[%s][%d]  cCmtno======[%s]",__FILE__,__LINE__,cCmtno );
  vtcp_log("[%s][%d]  725���߿ͻ��ʣ����Բ�����ʺŵ�״̬  cCmtno======[%s]",__FILE__,__LINE__,cCmtno );
    if(memcmp(cCmtno,"725",3))
    {
        if( g_dd_mst.ac_sts[0]=='3' )
        {
            sprintf(acErrMsg,"���˻��ѹ�ʧ����!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O174");
            goto ErrExit;
        }
        else if( g_dd_mst.ac_sts[0]=='5' )
        {
            sprintf(acErrMsg,"���˻�������ʱ����״̬!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O173");
            goto ErrExit;
        }
        else if( g_dd_mst.ac_sts[0]=='*' )
        {
            sprintf(acErrMsg,"���˻�������������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P245");
            goto ErrExit;
        }
        sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
        WRITEMSG

        if( g_dd_mst.hold_sts[0]=='1' )
        {
            if( g_pub_tx.ac_wrk_ind[7]=='J' )
                vtcp_log("��ȫ������Ҫ��Ϣ[%s]",g_pub_tx.ac_wrk_ind );
                /** �޸Ķ���ۻ� **/
                /**   add by shiweina 20071217  **/
            else if (strcmp(g_pub_tx.tx_code,"2430")==0)
            {
                sprintf(acErrMsg,"[%s][%d]tx_code=%ld",__FILE__,__LINE__,g_pub_tx.tx_code);
                WRITEMSG
             
            }
            else if(memcmp(g_pub_tx.tx_code,"4713",4) == 0)/*���ӿۻ����⴦�� 2011/11/13 14:00:48 */
            {
                vtcp_log("[%s][%d] ȫ�������Կ��Կۻ�[%s]",__FILE__,__LINE__,g_pub_tx.tx_code );
            }
            else
            {
                sprintf(acErrMsg,"���˺��ѱ���ȫ����!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P110");
                goto ErrExit;
            }
        }

    }
  /*End    Modified by ChenMing  725���߿ͻ��ʣ����Բ�����ʺŵ�״̬   */
  
  sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
  WRITEMSG

    vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
    /*ȡ�������Э������*/
    if( g_pub_tx.add_ind[0]=='0')
    {
        /** add by chenchao ������ί��֧ȡЭ�� 2011-7-31 17:51:47 begin **/
        if( (memcmp(g_pub_tx.tx_code,"2430",4)!=0)  /** �ۻ������  **/
            && (memcmp(g_pub_tx.tx_code,"G083",4)!=0) /** �����Ϣ ��Ϣ����� **/
            && (memcmp(g_pub_tx.tx_code,"G085",4)!=0) /** ����ƻ������ղ���� **/
            && (memcmp(g_pub_tx.tx_code,"G086",4)!=0) /** ����Ƿ���ÿ�ղ����  **/
            && (memcmp(g_pub_tx.tx_code,"G088",4)!=0) /** ���մ���� ����� **/
            && (memcmp(g_pub_tx.tx_code,"4713",4)!=0) /*�ۻ����⴦�� 2011/11/13 14:05:41*/
            && (dd_limit()))
            
        {
            vtcp_log("[%s][%d]���֧ȡ���ƴ���",__FILE__,__LINE__);
            goto ErrExit;
        }
        /** add by chenchao ������ί��֧ȡЭ�� 2011-7-31 17:51:47 end **/
        if( g_dd_mst.ac_sts[0]=='0' )
        {   
            sprintf(acErrMsg,"���˻����ڿ�����ȷ��״̬������������ȷ��!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O172");
            goto ErrExit;
        }
        sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
        WRITEMSG

        /*** �жұ�֤�𻧲�����ȡ�� ***/
        if( !strncmp(g_dd_parm.prdt_no,"131",3) && g_pub_tx.ac_wrk_ind[8]!='1' && strcmp(g_pub_tx.tx_code,"5801")  && strcmp(g_pub_tx.tx_code,"2208"))
        {
            sprintf(acErrMsg,"�жұ�֤�𻧲�����ȡ��!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D212");
            goto ErrExit;
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

        if( g_pub_tx.svc_ind!=1003 && g_pub_tx.svc_ind!=1004 )
        {
            if( g_dd_parm.draw_ind[0]=='N' )
            if ( g_dd_parm.draw_ind[0]=='N' && memcmp(g_pub_tx.tx_code,"4713",4 )!=0)/* �ۻ����⴦�� 2011/11/13 14:06:51*/
            {
                sprintf(acErrMsg,"������ȡ[%s]!",g_dd_parm.draw_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A023");
                goto ErrExit;
            }
            if( g_dd_parm.thr_draw_ind[0]=='N' 
            && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) &&
            g_pub_tx.ac_wrk_ind[8]!='1' && g_pub_tx.ac_wrk_ind[9]!='1' )
            {
                sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_draw_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A011");
                goto ErrExit;
            }
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d],ct_ind[%c],cash_ind[%c]",__FILE__,__LINE__,g_pub_tx.svc_ind,g_pub_tx.ct_ind[0],g_dd_mst.cash_ind[0]);
    WRITEMSG

        if( g_pub_tx.ct_ind[0]=='1' ) /**ȡ��**/
        {
            /*��ʱ����ר�û��������ֱ�־ һ�㻧Ҳ���������� 20070116*/
            if( g_dd_mst.ac_type[0]!='1' && g_dd_mst.ac_type[0]!='4' && \
                g_dd_mst.ac_type[0]!='5' && g_dd_mst.ac_type[0]!='7') 
                
            {
                if(g_dd_mst.cash_ind[0]=='N')
                {
                    sprintf(acErrMsg,"���˻�������ȡ��!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P107");
                    goto ErrExit;
                }
            }

            if( g_dd_mst.odtt_ind[0]=='1' )
            {
                sprintf(acErrMsg,"�㻧������ȡ��!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P108");
                goto ErrExit;
            }

            if( g_dd_parm.cash_ind[0]=='N' && g_pub_tx.tx_amt1>0.001)/*0���ľͲ����� 2006-12-11 */
            {
                sprintf(acErrMsg,"��Ʒ������ȡ��!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"A012");
                goto ErrExit;
            }
        }
        else if( g_pub_tx.ct_ind[0]=='2' )
        {
            /****NEWTCTD9:TODO:С��ͨ��ͨ������ǽ��㻧****/
            if( g_dd_mst.sttl_type[0]=='S' && g_pub_tx.ac_wrk_ind[7]!='J' && \
                g_pub_tx.svc_ind != 1012 && g_pub_tx.ac_wrk_ind[9]!='1' &&
                memcmp(g_pub_tx.tx_code,"5840",4)!=0)
            {
                sprintf(acErrMsg,"�ǽ��㻧������ת��![%s][%d]",
                    g_pub_tx.svc_ind,g_pub_tx.ac_wrk_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O170");
                goto ErrExit;
            }
        }

        sprintf(acErrMsg,"OKOKOKOKOKOKac_wrk_ind=[%s]!", g_pub_tx.ac_wrk_ind);
        WRITEMSG
        /*** ����У��֧ȡ��ʽ��У�����뺯�� ***/
        if(g_pub_tx.ac_wrk_ind[3]=='1' )
        {
            sprintf(acErrMsg,"%s,%d,draw_pwd_yn=[%c],draw_id_yn=[%c]",
                __FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0],g_mdm_ac_rel.draw_id_yn[0]);
            WRITEMSG
            vtcp_log("%s,%d,g_pub_tx.ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
            /*add by hxc 20091012*/
                if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0){/**20091012 ������������memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0**/
                    g_mdm_ac_rel.draw_pwd_yn[0] = 'N';
                    vtcp_log("%s,%d,���п����Ĳ��������draw_pwd_yn[%s]g_pub_tx.tx_amt1[%f]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn,g_pub_tx.tx_amt1);
                }
                if(memcmp(g_pub_tx.tx_code,"6789",sizeof(g_pub_tx.tx_code)-1)==0){
                    g_mdm_ac_rel.draw_pwd_yn[0] = 'N';
                    vtcp_log("%s,%d,���в�˰������˰���������draw_pwd_yn[%s]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn);
                }
            /*end by hxc 20091012*/
            {/*add by wyw 20080316*/
                vtcp_log("%s,%d,draw_pwd_yn[%s]g_pub_tx.tx_amt1[%f]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn,g_pub_tx.tx_amt1);
                if(memcmp(g_pub_tx.tx_code,"6203",4)==0/**&& g_pub_tx.tx_amt1<0.001***/){/*20090729����ȥ������&& g_pub_tx.tx_amt1<0.001*/
                    g_mdm_ac_rel.draw_pwd_yn[0] = g_pub_tx.draw_pwd_yn[0];
                    vtcp_log("%s,%d,draw_pwd_yn[%s]g_pub_tx.tx_amt1[%f]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn,g_pub_tx.tx_amt1);
                }
                
            }/*add by wyw 20080316*/
            ret = pub_base_check_draw("001");
            ERR_DEAL
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

        /*** Э������� ***
        if( g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_mst.hold_amt
        || g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_parm.min_bal )
        if( g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_dd_parm.min_bal && g_dd_mst.ac_type[0]!='6' )
        **********************/
        /****�޸�Ϊ��pub_base_CompDblVal�������Ƚϴ�С modify at 20120108****/
       if( pub_base_CompDblVal(g_dd_mst.bal-g_pub_tx.tx_amt1,g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_dd_parm.min_bal)<0 && g_dd_mst.ac_type[0]!='6' )
        {
            ret=pub_base_CompDblVal(g_dd_mst.hold_amt,g_dd_parm.min_bal);
            if( ret>=0 ) 
        tmpje=g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_pub_tx.tx_amt1-g_dd_mst.bal;
            else 
        tmpje=g_dd_mst.ctl_amt+g_dd_parm.min_bal+g_pub_tx.tx_amt1-g_dd_mst.bal;
            
            if( pub_sub_prot(tmpje) )
            {
                sprintf(acErrMsg,"Э����������!");
                WRITEMSG
                goto ErrExit;
            }
        }
    vtcp_log("[%s][%d] 222222g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        /*** ��������Ϣ�����ĺ��� ***/
        if( g_pub_tx.svc_ind!=1006 && g_pub_tx.svc_ind!=1012 )
        {
            if( g_dd_mst.lst_date<g_dd_mst.ic_date && g_dd_mst.ic_date>19000000 )
            {
                if( pub_base_CalAcm(g_dd_mst.ic_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"������Ϣ��������!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
            else
            {
                if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"������Ϣ��������!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
    vtcp_log("[%s][%d] 333333g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);


        switch( g_pub_tx.svc_ind )
        {
            case 1001:      /*��ȡ��˴�Ϊȡ��*/   /* ������Ҫ���Э���˻�ȡ������� wudawei 20130717 */
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
               /* if( g_dd_mst.ac_type[0]=='6' || g_dd_mst.ac_type[0]=='7' ) */
                if( g_dd_mst.ac_type[0]=='7' )
                {
                    sprintf(acErrMsg,"�˻�ΪЭ��������˺ţ�����ֱ�Ӵ�ȡ��!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P293");
                    goto ErrExit;
                }
    sprintf(acErrMsg,"[%s][%d]g_dd_mst.ac_type[0]=[%c]",__FILE__,__LINE__,g_dd_mst.ac_type[0]);
    WRITEMSG
            case 1009:      /*** ������ȡ�� ***/
                if( g_pub_tx.svc_ind==1009 )
                {
                    g_pub_tx.tx_amt1=g_dd_mst.bal;
                    if( strcmp(g_dd_mst.prdt_no,"102") )
                    {
                        sprintf(acErrMsg,"�ǹ�����![%s]",g_dd_mst.prdt_no);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P416");
                        goto ErrExit;
                    }
                }
        sprintf(acErrMsg,"[%s][%d]g_dd_mst.ac_type[0]=[%c]",__FILE__,__LINE__,g_dd_mst.ac_type[0]);
        WRITEMSG
            case 1008:      /*Э������ȡ��˴�Ϊȡ��*/
                /*** ȡ��ʱ��鿨�޶�ǼǱ������� ***/
                /*** ADD COMMENCE BY Azz.05.03.26 ***/
                ret = Card_consum_lmt_Dec_Upd( g_pub_tx.reply,"ac_no='%s'", \
                                                g_pub_tx.ac_no); 
        sprintf(acErrMsg,"[%s][%d]ע�� ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
        WRITEMSG
                if( ret )
                {
                    sprintf(acErrMsg,"DECLARE ���޶�ǼǱ��쳣!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P121");
                    goto ErrExit;
                }

                ret = Card_consum_lmt_Fet_Upd( &g_card_consum_lmt, \
                                                g_pub_tx.reply);
                if( ret&&ret!=100 )
                {
                    sprintf(acErrMsg,"FETCH ���޶�ǼǱ��쳣!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P120");
                    goto ErrExit;
                }
                else if( ret==0 )
                {
                    /*** ȡ�� ***/
                    vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                    if( g_pub_tx.ct_ind[0]=='1' && \
                    pub_base_CompDblVal(g_card_consum_lmt.da_cash_lmt,0.00)>0 )
                    {
                        /*** ȡ�ּ�鿨�޶�� ****/
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_cash_lmt,g_card_consum_lmt.da_cash_lmt) > 0 )
                        {
                            sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P231");
                            goto ErrExit;
                        }
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_cash_lmt,g_card_consum_lmt.mo_cash_lmt) > 0 )
                        {
                            sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P232");
                            goto ErrExit;
                        }

                        g_card_consum_lmt.da_all_cash_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_cash_lmt;

                        g_card_consum_lmt.mo_all_cash_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_cash_lmt;
                    }
                    else if( pub_base_CompDblVal(g_card_consum_lmt.da_amt_lmt, \
                                                 0.00)>0 )
                    {
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_amt_lmt,g_card_consum_lmt.da_amt_lmt ) > 0 )
                        {
                            sprintf(acErrMsg,"����ÿ�����ȡ���޶�![%.2lf] \
                            [%.2lf][%.2lf]",g_pub_tx.tx_amt1,g_card_consum_lmt.da_all_amt_lmt,g_card_consum_lmt.da_amt_lmt);
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P233");
                            goto ErrExit;
                        }
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_amt_lmt,g_card_consum_lmt.mo_amt_lmt) > 0 )
                        {
                            sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P234");
                            goto ErrExit;
                        }

                        g_card_consum_lmt.da_all_amt_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_amt_lmt;

                        g_card_consum_lmt.mo_all_amt_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_amt_lmt;
                    }

                    ret = Card_consum_lmt_Upd_Upd( g_card_consum_lmt, \
                                                    g_pub_tx.reply);
                    if( ret )
                    {
                        sprintf(acErrMsg,"UPDATE ERROR CODE=[%d]!",ret);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"D105");
                        goto ErrExit;
                    }
                }
                Card_consum_lmt_Clo_Upd();
                /*** ADD COMPLETE ***/
                vtcp_log("[%s][%d] 5555555555g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);

        sprintf(acErrMsg,"!!![%s][%d] �������仯ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                vtcp_log("[%s][%d] 666666666g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                break;
            case 1006:      /*Ԥ��Ȩ*/
                g_dd_mst.ctl_amt=g_dd_mst.ctl_amt+g_pub_tx.tx_amt1;
                vtcp_log("%s,%d,POSԤ��Ȩ g_pub_tx.tx_amt1[%lf],ctl[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_dd_mst.ctl_amt);
                break;
            case 1007:      /*Ԥ��Ȩ���*/
                /*BEGIN:del by hxc 091117*/
                /**if( pub_base_CompDblVal(g_dd_mst.ctl_amt,g_pub_tx.tx_amt1)<0 )
                {
                    sprintf(acErrMsg,"Ԥ��Ȩ��ɽ���[%.2lf]<[%.2lf]!",
                        g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"A031" );
                    goto ErrExit;
                }
                g_dd_mst.ctl_amt=0.00;
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                break;**/
                /*End:del by hxc 091117*/
                /*BEGIN:add by hxc 091117 Ԥ��Ȩ��ɽ������ǿ��ƽ���115% */
                ret = pub_base_GetParm_double("YSQWC",1,&ysqwc_rate);
                if(ret)
                {
                    strcpy(g_pub_tx.reply,"CD06");
                    vtcp_log("%s,%d,ȡԤ��Ȩ����ϸ�����ʧ��!",__FILE__,__LINE__);
                    goto ErrExit;
                }
                vtcp_log("%s,%d,�ϸ�����[%lf]",__FILE__,__LINE__,ysqwc_rate);
                zg_amt = g_dd_mst.ctl_amt * ( 1 + ysqwc_rate);
                vtcp_log("%s,%d,�ϸ�����[%.2f],��߽��[%.2f]",__FILE__,__LINE__,ysqwc_rate,zg_amt);
                
                if ( pub_base_CompDblVal(g_pub_tx.tx_amt1,zg_amt)==1 )
                {
                    sprintf(acErrMsg,"Ԥ��Ȩ��ɽ�����[%.2lf]>[%.2lf]!",
                        g_pub_tx.tx_amt1,zg_amt);
                    WRITEMSG
                        strcpy( g_pub_tx.reply,"A031" );
                    goto ErrExit;
                }
                /*g_dd_mst.ctl_amt=0.00;*/
                            /*Ԥ��Ȩ���ҵ���г����ͳ���,���Ҫ�ָ�ԭԤ��Ȩ���*/
                vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
                g_dd_mst.ctl_amt = g_dd_mst.ctl_amt - g_pub_tx.tx_amt2;
                vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
                if(pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00)<0)
                {
                    vtcp_log("%s,%d,���ƽ��С��Ԥ��Ȩ���!ctl[%lf],tx_amt2[%lf]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
                    strcpy(g_pub_tx.reply,"CU14");
                    goto ErrExit;
                }
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                /*END: add by hxc 091117*/
                break;
            case 1010:      /*�������������*/
                g_pub_tx.tx_amt1=g_dd_mst.bal;
                if( strcmp(g_dd_mst.prdt_no,"102") )
                {
                    sprintf(acErrMsg,"�ǹ�����![%s]",g_dd_mst.prdt_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P416");
                    goto ErrExit;
                }
            case 1003:      /*����*/

                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( g_dd_parm.thr_cls_ind[0]=='N' 
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_draw_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A007");
                    goto ErrExit;
                }
                if( pub_base_CompDblVal(g_dd_mst.bal,g_pub_tx.tx_amt1) )
                {
                    sprintf(acErrMsg,"��������Ϊ��![.%2lf][%.2lf]", \
                            g_dd_mst.bal,g_pub_tx.tx_amt1);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P116");
                    goto ErrExit;
                }

                /*����ʻ��Ƿ��н���δ��*/
                ret = sql_count("mdm_ac_rel","ac_id=%ld and note_sts not in('2','3','4','5','*')", \
                                g_mdm_ac_rel.ac_id );
                if( ret<0 )
                {
                    sprintf(acErrMsg,"TRANSFER FUNCTION SQL_COUNT ERROR!");
                    WRITEMSG
                    goto ErrExit;
                }
                else if( ret>1 )
                {
                    sprintf(acErrMsg,"���ʻ�������������δ����������������!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O199");
                    goto ErrExit;
                }

                /* add by xxx at 20030805 ��δ������ϸ�Ĳ������� �Ȳ��� */
                sprintf(acErrMsg, "CGH *******trace_no=[%ld], tx_date=[%ld]", 
                    g_pub_tx.trace_no, g_pub_tx.tx_date);
                WRITEMSG
                sprintf(acErrMsg, "ac_id= [%ld], ac_seqn=[%d]", 
                    g_dd_mst.ac_id, g_dd_mst.ac_seqn);
                WRITEMSG
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                ret = sql_count("mdm_unprt_hst", "(trace_no!=%ld or \
                    tx_date != %ld) and ac_id=%ld and ac_seqn=%d",
                    g_pub_tx.trace_no, g_pub_tx.tx_date, g_dd_mst.ac_id,
                    g_dd_mst.ac_seqn);
                if (ret < 0)
                {
                    sprintf(acErrMsg,"TRANSFER FUNCTION SQL_COUNT ERROR!");
                    WRITEMSG
                    goto ErrExit;
                }
                else if (ret>0)
                {
                    /*���ӿۻ����⴦�� 2011/11/13 14:09:26*/
                    if(memcmp(g_pub_tx.tx_code,"4713",sizeof(g_pub_tx.tx_code)-1)==0 && memcmp(g_pub_tx.brf,"�ۻ�ҵ��",8)==0)
                    {
                        vtcp_log("[%s][%d]�ۻ������δ������ϸ",__FILE__,__LINE__);
                    }
                    /* ��������ʱ ��ʧ���������δ������ϸ wudawei 20130716 */
                   	else if(memcmp(g_pub_tx.tx_code,"2204",sizeof(g_pub_tx.tx_code)-1)==0 && memcmp(g_mdm_ac_rel.note_sts,"1",1)==0)
                   	{
                   			vtcp_log("[%s][%d]�����ʧ���������δ������ϸ",__FILE__,__LINE__);
                   	}
                   	/* add end 20130716 */
                    else 
                    {
                        sprintf(acErrMsg, "����δ������ϸ����������!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "D217");
                        goto ErrExit;
                    }
                }
                sprintf(acErrMsg, "CGH***********ret =[%d]", ret);
                WRITEMSG

                
                /* ���ü�Ϣ���� */
                memset(&g_pub_intst, 0x0, sizeof(struct S_pub_intst));
                if (g_dd_mst.ac_sts[0] != '#')
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
                    sprintf(acErrMsg,"CCCC**************��ϢΪ = [%lf]",
                    g_pub_intst.factval);
                    WRITEMSG
                }

                strcpy(g_dd_mst.ac_sts,"*");

                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                strcpy(g_pub_tx.brf, "����");
                break;
            case 1004:      /*Э������*/
                if( g_dd_parm.thr_cls_ind[0]=='N' 
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_draw_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A007");
                    goto ErrExit;
                }
                /* �ж��ȼ�Ϣ������ added by liuxuan 20071010*/
                if(g_dd_mst.lst_date-g_pub_tx.tx_date!=0 && g_dd_mst.intst_acm>0.005)
                {
                    sprintf(acErrMsg,"�ȼ�Ϣ������!!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P098");
                    goto ErrExit;
                }
                /* added over */
                g_pub_tx.tx_amt1=g_dd_mst.bal;
                strcpy(g_dd_mst.ac_sts,"*");
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                strcpy(g_pub_tx.brf, "Э������");
                break;
            default :
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                sprintf( acErrMsg,"��������[%d]��ͬȡ��",g_pub_tx.svc_ind );
                WRITEMSG
                break;
        }
        
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
        }
        /** �ۻ�  **/
    /***    add by  shiweina   **/
    if(strcmp(g_pub_tx.tx_code,"2430")==0)
    {   get_zd_data("0700",kh_type);
        vtcp_log("%s,%d,Gethere7!!!!notice!FD70kh_type=[%s]\n",__FILE__,__LINE__,kh_type);
        if(kh_type[0]=='2')
        {
      if(g_dd_mst.hold_sts[0]=='3' )  
      {
         vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
       get_zd_long("0350",&hold_seqn);
       get_zd_long("0340",&ac_seqn);
       vtcp_log("[%s][%d] hold_seqn=%ld ",__FILE__,__LINE__,hold_seqn);
       ret = Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn=%ld  and ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
        
        if(ret==100)
        {
           sprintf(acErrMsg," ���ݿ��޴˼�¼[%s]",g_pub_tx.reply);
           WRITEMSG
           goto ErrExit;
        }
        if(ret!=0)
        {
          sprintf(acErrMsg,"��ѯ����Ǽǲ�����[%s]",g_pub_tx.reply);
          WRITEMSG
          goto ErrExit;
        } 
          vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
        if( fabs(g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)<0.005)
        {  
            vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
          vtcp_log("[%s][%d] jinjinjinjinjin=%.2f ",__FILE__,__LINE__,g_mo_hold.plan_hold_amt);
          vtcp_log("[%s][%d] eeeeeeeeeeeeee=%.2f ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
          ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and  ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
          vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
          if(ret)
          {

             sprintf(acErrMsg,"ִ��Mo_hold_Dec_Upd����[%d]",ret);
             WRITEMSG
             goto ErrExit;
          }
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           if(ret==100)
           {
              sprintf( acErrMsg,"��ѯ�Ǽǲ���,�޼�¼");
              WRITEMSG
              goto ErrExit;
           }
           else if (ret)
           {
              sprintf( acErrMsg,"ִ��Mo_hold_Fet_Upd����[%d]",ret);
              WRITEMSG
              goto ErrExit;
           }   
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);  
           strcpy(g_mo_hold.hold_sts,"1");
           g_mo_hold.fct_unhold_date = g_pub_tx.tx_date;
           strcpy(g_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
           strcpy(g_mo_hold.unhold_tel,g_pub_tx.tel);
           strcpy(g_mo_hold.unhold_brf,g_pub_tx.brf);
           g_mo_hold.wrk_date = g_pub_tx.tx_date;
           g_mo_hold.trace_no = g_pub_tx.trace_no;
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           if( ret )
           {
               sprintf(acErrMsg," �޸ļ�¼����[%d]",ret);
               WRITEMSG
               goto ErrExit;
           }
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
           Mo_hold_Clo_Upd();
           }
           else if (fabs( g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)>0.005)
           {
              vtcp_log("[%s][%d],oooooooooo=%.2f",__FILE__,__LINE__,g_mo_hold.plan_hold_amt);
              vtcp_log("[%s][%d],qqqqqqqqqqq=%.2f",__FILE__,__LINE__,g_pub_tx.tx_amt1); 
                vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
              ret = Mo_hold_Dec_Upd ( g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);

              if(ret)
              {
                 sprintf(acErrMsg,"ִ��Mo_hold_Dec_Upd����[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
              }
              ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
              if(ret==100)
              {
                 sprintf( acErrMsg,"��ѯ�Ǽǲ���,�޼�¼");
                 WRITEMSG
                 goto ErrExit;
              }
              else if(ret)
              {
                 sprintf( acErrMsg,"ִ��Mo_hold_Fet_Upd����[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
              } 
              g_mo_hold.plan_hold_amt=g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1;
              g_mo_hold.wrk_date = g_pub_tx.tx_date;
              g_mo_hold.trace_no = g_pub_tx.trace_no;
              ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
              if(ret)
              {
                  sprintf(acErrMsg," �޸ļ�¼����[%d]",ret);
                  WRITEMSG
                  goto ErrExit;
              }
              Mo_hold_Clo_Upd();
            } 
            else if ( fabs(g_pub_tx.tx_amt1-g_mo_hold.plan_hold_amt)>0.005)
            {
                sprintf(acErrMsg,"�ۻ������ڶ�����");
                WRITEMSG
                goto ErrExit;
            }   
       }
       else if (g_dd_mst.hold_sts[0]=='2' || g_dd_mst.hold_sts[0]=='1' )
          {
            get_zd_long("0350",&hold_seqn);
            get_zd_long("0340",&ac_seqn);
              vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
            ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld and  ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn );
            vtcp_log("[%s][%d] retretret======%d",__FILE__,__LINE__,ret);
            if(ret)
            {
                 sprintf(acErrMsg,"ִ��Mo_hold_Dec_Upd����[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
            }
            ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
            vtcp_log("[%s][%d] retretret======%d",__FILE__,__LINE__,ret);
            if(ret==100)
            {
                 sprintf( acErrMsg,"��ѯ�Ǽǲ���,�޼�¼");
                 WRITEMSG
                 goto ErrExit;
            }
            else if (ret)
            {
                 sprintf( acErrMsg,"ִ��Mo_hold_Fet_Upd����[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
            }
            g_mo_hold.wrk_date = g_pub_tx.tx_date;
            g_mo_hold.trace_no = g_pub_tx.trace_no;
            ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
            if(ret)
            {
                 sprintf(acErrMsg," �޸ļ�¼����[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
            }
               Mo_hold_Clo_Upd();
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
         }
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
           double dj_dbal=0.00;
     /**        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no=%ld ",g_mdm_ac_rel.ac_no);
           if(ret==100)
           {
                sprintf(acErrMsg,"���ݿ��޴˼�¼[%s]",g_pub_tx.reply);
                WRITEMSG
                goto ErrExit;
           }
           if(ret)
           { 
                sprintf(acErrMsg,"��ѯMdm_ac_rel����[%s]",g_pub_tx.reply);
                WRITEMSG
                goto ErrExit;
           }   ** /
           ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld  and ac_seqn=%ld",g_mdm_ac_rel.ac_id,ac_seqn);
           if(ret)
           {
                sprintf(acErrMsg,"ִ��Dd_mst_Dec_Upd����[%s]",g_pub_tx.reply);
                WRITEMSG
                goto ErrExit;
           }
           ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
           if(ret==100)
           {
               sprintf(acErrMsg,"��ѯ����������,�޼�¼");
               WRITEMSG
               goto ErrExit;
           }
           if(ret)
           {
              sprintf(acErrMsg,"��ѯ�����������쳣[%s]".g_pub_tx.reply);
              WRITEMSG
              goto ErrExit;
           }    
          ** g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;  **/
           ret=sql_sum_double("mo_hold","plan_hold_amt",&dj_dbal,"hold_sts='0' and ac_id=%ld  and ac_seqn=%ld ",g_pub_tx.ac_id,ac_seqn);
           vtcp_log("[%s][%d] rettttttttt=%d",__FILE__,__LINE__,ret);
           vtcp_log("[%s][%d] dj_dbal=%.2f",__FILE__,__LINE__,dj_dbal);
           if(ret)
           {
              sprintf(acErrMsg,"��������쳣[%s]",g_pub_tx.reply);
              WRITEMSG
              goto ErrExit;
           }
           vtcp_log("[%s][%d], nanananananana=%.2f ",__FILE__,__LINE__,g_dd_mst.bal);  
           g_dd_mst.hold_amt=dj_dbal;
           vtcp_log("[%s][%d] hold_amt=%d",__FILE__,__LINE__,g_dd_mst.hold_amt);
           vtcp_log("[%s][%d] kouhuakouhuakouhua=%.2f ",__FILE__,__LINE__,g_pub_tx.tx_amt1);  
           ret=sql_min_string("mo_hold","hold_typ",minholdtyp,1,"hold_sts='0' and ac_id=%ld and ac_seqn=%ld",g_pub_tx.ac_id,ac_seqn);    
           vtcp_log("[%s][%d] shishishishi=%d ",__FILE__,__LINE__,ret);
           vtcp_log("[%s][%d] shishishishi='%s' ",__FILE__,__LINE__,minholdtyp); 
           if(ret)
           {
              sprintf(acErrMsg,"��������쳣[%s]",g_pub_tx.reply);
              WRITEMSG
              goto ErrExit;
           }
           strcpy(g_dd_mst.hold_sts,minholdtyp);
          vtcp_log("[%s][%d] hold_sts='%s' ",__FILE__,__LINE__,g_dd_mst.hold_sts);
          ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
          if(ret)
          {
               sprintf(acErrMsg,"Dd_mst_Upd_Upd����=[%d]",ret);
               WRITEMSG
               goto ErrExit;
          }
         
           Dd_mst_Clo_Upd();
         }
       
         
  }                       /**  ���˽���   **/
  else 
  {
        
        /*���ӿۻ����⴦�� 2011/11/13 14:16:53*/
        if ( g_dd_mst.hold_sts[0]=='1' && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        {
            sprintf(acErrMsg,"���˻�������ȫ����״̬!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P111");
            goto ErrExit;
        }
        /*���ӿۻ����⴦�� 2011/11/13 14:16:53*/
        if( g_dd_mst.hold_sts[0]=='2' && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        {
            sprintf(acErrMsg,"���˻�����ֻ����������״̬!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P111");
            goto ErrExit;
        }
        /*���ӿۻ����⴦�� 2011/11/13 14:16:53*/
        /**** �޸�Ϊ��pub_base_CompDblVal�����ȽϽ��  modify at 20120108
        if( g_dd_mst.hold_sts[0]=='3' && g_dd_mst.bal<g_dd_mst.hold_amt && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        ****/
        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d] g_pub_tx.tx_code=[%s]",__FILE__,__LINE__,g_dd_mst.ac_id,g_pub_tx.tx_code);
        if( g_dd_mst.hold_sts[0]=='3' && pub_base_CompDblVal(g_dd_mst.bal,g_dd_mst.hold_amt)<0 && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        {
            sprintf(acErrMsg,"���˺��ѱ����ֶ���!g_dd_mst.bal===[%.2lf], \
            g_dd_mst.hold_amt======[%.2lf]",g_dd_mst.bal,g_dd_mst.hold_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P112");
            goto ErrExit;
        }
    }
        /**
        if ( g_pub_tx.add_ind[0]=='0' )  
        {
            if( g_dd_mst.bal<g_dd_mst.ctl_amt+g_dd_mst.hold_amt 
                && ( g_dd_mst.ctl_amt>0.005 || g_dd_mst.hold_amt>0.005) )
            {
                sprintf(acErrMsg,"���˺������ڿ��ƽ��!");
                WRITEMSG
                if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
                {
                    Remark_ctl_Sel(g_pub_tx.reply,&rem,"ac_id=%d AND rem_sts!='2'",g_dd_mst.ac_id);
                    strcpy( g_pub_tx.reply,"D247");
                    rem.rem[50]='\0';
                    sprintf(msg,"%-50s",rem.rem);
                    set_zd_data("0130",msg);
                    vtcp_log(" MSG=[%s]",msg);
                }
                else strcpy(g_pub_tx.reply,"A030");
                goto ErrExit;
            }
        }
        **/

        if( g_dd_mst.bal<-0.005 && g_dd_mst.od_ind[0]=='Y' )
        {
            /*
            tmpbal=-g_dd_mst.bal;
            ret=pubf_od_proc(g_dd_mst.ac_id,g_dd_mst.ac_seqn,
                g_dd_mst.od_ind,g_pub_tx.add_ind,g_dd_mst.prdt_no,&tmpbal);
            if( ret )
            {
                sprintf(acErrMsg,"͸֧�������");
                WRITEMSG
                goto ErrExit;
            }
            */
            ;
        }

        if( (g_pub_tx.svc_ind==1001 || g_pub_tx.svc_ind==1003) && g_pub_tx.ac_wrk_ind[8]!='1' )
        {
            if( pub_sub_chkadd(g_dd_mst.ys_bal,g_pub_tx.tx_amt1) )
            {
                goto ErrExit;
            }
            
        }
    }
    else if( g_pub_tx.add_ind[0]=='1' )     /*���*/
    {
        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
        /* add begin */
        if( g_dd_mst.ac_sts[0]=='0' )
        {   
            sprintf(acErrMsg,"���˻����ڿ�����ȷ��״̬������������ȷ��!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O172");
            goto ErrExit;
        }
        /* add over liuxuan 2006-10-18 */
        if( g_dd_parm.dpst_ind[0]=='N' )
        {
            sprintf(acErrMsg,"��������[%s]!",g_dd_parm.dpst_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A009");
            goto ErrExit;
        }
        if( g_dd_parm.thr_dpst_ind[0]=='N' 
            && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no))
        {
            sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_dpst_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A010");
            goto ErrExit;
        }
        if( g_pub_tx.svc_ind!=1006 && g_pub_tx.svc_ind!=1012 )
        {
            if( g_dd_mst.lst_date<g_dd_mst.ic_date && g_dd_mst.ic_date>19000000 )
            {
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( pub_base_CalAcm(g_dd_mst.ic_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"������Ϣ��������!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
            else
            {
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"������Ϣ��������!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
        }

        if( g_dd_mst.od_ind[0]=='Y' )
        {
            /*
            ret=pubf_od_proc(g_dd_mst.ac_id,g_dd_mst.ac_seqn,
                g_dd_mst.od_ind,g_pub_tx.add_ind,g_dd_mst.prdt_no,&g_pub_tx.tx_amt1) ;
            if( ret )
            {
                sprintf(acErrMsg,"͸֧�������");
                WRITEMSG
                goto ErrExit;
            }
            */
            ;
        }

        switch( g_pub_tx.svc_ind )
        {
            case 1001:      /*���*/
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d],g_pub_tx.tx_code=[%s]",__FILE__,__LINE__,g_dd_mst.ac_id,g_pub_tx.tx_code);
                /* ������Ҫ�� ��Ʒת�����Թ� wudawei 20131010
                if( g_dd_mst.ac_type[0]=='6' )
                end 20131010*/ 
                if( g_dd_mst.ac_type[0]=='6' && memcmp(g_pub_tx.tx_code,"3319",4) != 0)
                {
                    sprintf(acErrMsg,"�˻�ΪЭ��������˺ţ�����ֱ�Ӵ��!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P293");
                    goto ErrExit;
                }
            case 1005:
            case 1008:      /*���*/
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
                break;
            case 1006:      /*Ԥ��Ȩ*/
                g_dd_mst.ctl_amt=g_dd_mst.ctl_amt-g_pub_tx.tx_amt1;
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00)<0 )
                {
                    sprintf(acErrMsg,"Ԥ��Ȩ��ɽ���[%.2lf]<[%.2lf]!",
                            g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"A031" );
                    goto ErrExit;
                }
                break;
            case 1007: /*BEGIN:add by wyw 20080319 Ԥ��Ȩ��ɳ���ʱ�� add by hxc 091117*/
            /*����ʱ����*/
            vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
            vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
            g_dd_mst.ctl_amt = g_dd_mst.ctl_amt + g_pub_tx.tx_amt2;
            vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
            g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
            g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
            break;
            /*END:add by wyw 20080319 Ԥ��Ȩ��ɳ���ʱ�� add by hxc 091117*/
            default :
                sprintf( acErrMsg,"��������[%d]��ͬ���",g_pub_tx.svc_ind );
                WRITEMSG
                g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
                break;
        }
        /*** ��������Ϣ�����ĺ��� ***/

        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
        }
    }
    else
    {
        sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P106");
        goto ErrExit;
    }
    vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);

    g_dd_mst.lst_date=g_pub_tx.tx_date;

    if( g_pub_tx.ac_wrk_ind[7]=='J' )
        ;
    else if( g_dd_mst.od_ind[0]=='Y' )
    {
        ;
    }
    else if( 
        pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt,g_dd_parm.min_bal)<0 
        && g_dd_mst.ac_sts[0]=='1' && g_dd_mst.ac_type[0]!='6' )
    {
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
        if ( g_pub_tx.add_ind[0]=='0' )  /* ȡ����ж� YUDONG 20030901*/
        {
            
             vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
             vtcp_log("%s,%d ctl_amt=%lf",__FILE__,__LINE__,g_dd_mst.ctl_amt);
             vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
             vtcp_log("%s,%d ac_id=%ld",__FILE__,__LINE__,g_pub_tx.ac_id);
             /**add by zyl 20110413 �������ճ�����**/
             /**0.��������**/
             char cAc_no_tmp[25];       /**���� 3.���浱ǰg_pub_tx����. **/
             long lAc_id_tmp = 0;       /**���� 3.���浱ǰg_pub_tx����. **/
             int  iAc_seqn_tmp = 0;     /**���� 3.���浱ǰg_pub_tx����. **/
             double dTx_amt_tmp = 0.0;  /**���� 3.���浱ǰg_pub_tx����. **/
             char cAdd_ind_tmp[2];      /**���� 3.���浱ǰg_pub_tx����. **/
             char cHst_ind_tmp[2];      /**���� 3.���浱ǰg_pub_tx����. **/
             char cAc_wrk_ind_tmp[11];  /**���� 3.���浱ǰg_pub_tx����. **/
             int  iSvc_ind_tmp = 0;     /**���� 3.���浱ǰg_pub_tx����. **/
             char cCt_ind_tmp[2];       /**���� 3.���浱ǰg_pub_tx����. **/
             char cAc_id_type_tmp[2];   /**���� 3.���浱ǰg_pub_tx����. **/
             char cNote_type_tmp[4];    /**���� 3.���浱ǰg_pub_tx����. **/
             char cBeg_note_no_tmp[17]; /**���� 3.���浱ǰg_pub_tx����. **/
             double b=0.0 ;
             struct mo_prdt_prot_c sMo_prdt_prot;
             struct mo_prot_ac_rel_c sMo_prot_ac_rel;
             struct dd_mst_c sDd_mst_virtual;
             struct dd_parm_c sDd_parm_virtual;/*add by slg 2013-02-05 11:02:16*/
             struct dd_mst_c sMain_dd_mst;
             struct dd_parm_c sDd_parm_main;   /*add by slg 2013-02-05 11:02:16*/
             struct S_pub_intst   sum_intst;
             memset(cAc_no_tmp,0x00,sizeof(cAc_no_tmp));
             memset(cAdd_ind_tmp,0x00,sizeof(cAdd_ind_tmp));
             memset(cHst_ind_tmp,0x00,sizeof(cHst_ind_tmp));
             memset(cAc_wrk_ind_tmp,0x00,sizeof(cAc_wrk_ind_tmp));
             memset(cCt_ind_tmp,0x00,sizeof(cCt_ind_tmp));
             memset(cAc_id_type_tmp,0x00,sizeof(cAc_id_type_tmp));
             memset(cNote_type_tmp,0x00,sizeof(cNote_type_tmp));
             memset(cBeg_note_no_tmp,0x00,sizeof(cBeg_note_no_tmp));
             
             memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
             memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
             memset(&sDd_parm_virtual,0x00,sizeof(struct dd_parm_c));
             memset(&sDd_parm_main,0x00,sizeof(struct dd_parm_c));
             memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
             memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
             
             /**1.���.�Ƿ����������˻�.**/
             vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
             ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_pub_tx.ac_id);
             if(ret == 100)
             {
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 /**��������ǩԼ.����ԭ����.**/
             }
             else if(ret)
             {
                 sprintf(acErrMsg,"Mo_prdt_prot_Sel error ret=[%d]",ret);
                 WRITEMSG
                 strcpy( g_pub_tx.reply,"D103");
                 goto ErrExit;
             }
             else
             {
                 vtcp_log("[%s][%d]���˻� zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);

                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_pub_tx.ac_id);
                 if(ret == 100)
                 {
                   /**������ǩԼ��Ϣ����.�������˻����չ�ϵ������.**/
                     sprintf(acErrMsg,"������ǩԼ��Ϣ����.�������˻����չ�ϵ������.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 
                 /**2.�������˻���Ϣ..ǩԼ�˻�,�����˻���Ϣ��ȡ��.**/
                 ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
                 if(ret == 100)
                 {
                     sprintf(acErrMsg,"dd_mst���������˻���Ϣ.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Dd_mst_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 /*BEGIN ---add  by slg 2013-02-02 */
                 vtcp_log("[%s][%d]�����˻����=[%17.2f]",__LINE__,__FILE__,sDd_mst_virtual.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 vtcp_log("[%s][%d] g_pub_tx.tx_amt1 =[%17.2f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
                 vtcp_log("[%s][%d] g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ctl_amt =[%17.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt);
                 vtcp_log("[%s][%d] g_dd_parm.min_bal =[%d]",__FILE__,__LINE__,g_dd_parm.min_bal);
                 vtcp_log("[%s][%d] aaaaaaaaaam.min_bal =[%17.2f]",__FILE__,__LINE__,g_dd_mst.bal+sDd_mst_virtual.bal+g_pub_tx.tx_amt1-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
                 if (pub_base_CompDblVal(g_pub_tx.tx_amt1,
                    g_dd_mst.bal+sDd_mst_virtual.bal+g_pub_tx.tx_amt1-g_dd_mst.ctl_amt-g_dd_parm.min_bal) > 0)/*by wjl 20130311 �ж������˺������˺�֮��*/
                 {   
                     sprintf(acErrMsg,"ȡ����������˻��������������֮��!!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"O214");
                     goto ErrExit;
                 }
                 
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm_virtual, "prdt_no='%s'",sDd_mst_virtual.prdt_no);
                 if (ret)
                 {
                     sprintf(acErrMsg, "��ѯ���ڲ�Ʒ�����ret=[%d]����prdt_no=[%s]!!!",ret,sDd_mst_virtual.prdt_no);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "W015");
                     goto ErrExit;
                 }
                
                 /*���������ʻ�����Ϣ*/
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret=app_cal_intst(sMo_prdt_prot.sts,sDd_mst_virtual,sDd_parm_virtual);
                 if (ret)
                 {
                     sprintf(acErrMsg, "���������ʻ���Ϣ����ret=[%d]!!!", ret);
                     WRITEMSG
                     goto ErrExit;
                 }
                 sum_intst.dealval=g_pub_intst.dealval;/*Ӧ����Ϣ*/
                 sum_intst.val=g_pub_intst.val;/*��Ϣ˰*/
                 
                 vtcp_log("[%s][%d]���˻���Ʒ����[%s]",__FILE__,__LINE__,g_dd_mst.prdt_no);
                 /*��ѯ���ڲ�Ʒ��*/
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 memset(&sDd_parm_main ,0x00,sizeof(struct dd_parm_c));
                 ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm_main, "prdt_no='%s'",g_dd_mst.prdt_no);
                 if (ret)
                 {
                     sprintf(acErrMsg, "��ѯ���ڲ�Ʒ�����ret=[%d]prdt_no=[%s]!!!",ret,g_dd_mst.prdt_no);
                     WRITEMSG
                     goto ErrExit;
                 }
                 
                  /**3.���浱ǰg_pub_tx����.**/
                 strcpy(cAc_no_tmp,g_pub_tx.ac_no);
                 
                 lAc_id_tmp = g_pub_tx.ac_id;
                 iAc_seqn_tmp = g_pub_tx.ac_seqn;
                 dTx_amt_tmp = g_pub_tx.tx_amt1;
                 strcpy(cAdd_ind_tmp,g_pub_tx.add_ind);
                 strcpy(cHst_ind_tmp,g_pub_tx.hst_ind);
                 strcpy(cAc_wrk_ind_tmp,g_pub_tx.ac_wrk_ind);
                 iSvc_ind_tmp = g_pub_tx.svc_ind;
                 strcpy(cCt_ind_tmp,g_pub_tx.ct_ind);
                 strcpy(cAc_id_type_tmp,g_pub_tx.ac_id_type);
                 strcpy(cNote_type_tmp,g_pub_tx.note_type);
                 strcpy(cBeg_note_no_tmp,g_pub_tx.beg_note_no);
                 
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 
                 /****��g_pub_tx.ac_no��ֵΪ��****/
                 strcpy(g_pub_tx.ac_no,"");
                 
                 ret=main_intst_acct(sDd_mst_virtual,sDd_parm_virtual,sum_intst);
                 if (ret)
                 {
                     sprintf(acErrMsg, "���˺ż��˴���ʧ��ret=[%d]!",ret);
                     WRITEMSG
                     goto ErrExit;
                 }
                 /*END ---add  by  slg   2013-02-02 09:39:50*/
                 
                 /**4.׼���ݹ�.pub_acct_trance()����.**/
                 /**5.pub_acct_trance().**/
                 /**a �����˻�����**/
                 strcpy(g_pub_tx.ac_no,"");
                 g_pub_tx.ac_id=sMo_prot_ac_rel.app_ac_id;
                 g_pub_tx.ac_seqn=1;
                 /****����Ӧ������Ϣ������� modify at 20130219
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal;
                 modify at next line****/
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;
                 /****end 20130218***/
                 strcpy(g_pub_tx.add_ind,"0"); 
                 strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
                 strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                 g_pub_tx.svc_ind=1001; /**�������***/
                 strcpy(g_pub_tx.ct_ind,"2");  /*** ת�� ***/
                 strcpy(g_pub_tx.ac_id_type,"");
                 strcpy(g_pub_tx.note_type,"");
                 strcpy(g_pub_tx.beg_note_no,"");
                 
                 if( pub_acct_trance() )
                 {
                   sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
                   WRITEMSG
                   return 1;
                 }
                 set_zd_data("102K",g_pub_tx.ct_ind);
                 /****����100���1����  modify at 20130218
                 set_zd_double("102I",sDd_mst_virtual.bal);
                 ****/
                 set_zd_double("1002",sDd_mst_virtual.bal);
                 vtcp_log("%s,%dddddddd=[%f]",__FILE__,__LINE__,sDd_mst_virtual.bal);
                 if ( pubf_acct_proc("LC03") )
                 {
                   sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
                   WRITEMSG
                   return 1;
                 }
                 /**b ǩԼ�˻���� ���⻧.���н��.**/
                 strcpy(g_pub_tx.ac_no,"");
                 g_pub_tx.ac_id=sMo_prdt_prot.main_ac_id;
                 g_pub_tx.ac_seqn=1;
                 /****����Ӧ������Ϣ������� modify at 20130219
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal;
                 modify at next line****/
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;
                 vtcp_log("zyl 201104277 see bal=[%17.2f]",sDd_mst_virtual.bal);
                 strcpy(g_pub_tx.add_ind,"1"); 
                 strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
                 strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                 g_pub_tx.svc_ind=1001; /**�������***/
                 strcpy(g_pub_tx.ct_ind,"2");  /*** ת�� ***/
                 strcpy(g_pub_tx.ac_id_type,"");
                 strcpy(g_pub_tx.note_type,"");
                 strcpy(g_pub_tx.beg_note_no,"");
                 
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 if( pub_acct_trance() )
                 {
                   sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
                   WRITEMSG
                   return 1;
                 }
                 set_zd_data("102K",g_pub_tx.ct_ind);
                 /****����ת�����Ϣ�������ʻ�,����100���1����  modify at 20130218****/
                 set_zd_double("1002",sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
                  vtcp_log("wjllll see bal=[%17.2f]",sDd_mst_virtual.bal);
                 vtcp_log("wjllll see bal=[%17.2f]",sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
                 /****end 20130218***/
                 if ( pubf_acct_proc("LC02") )
                 {
                   sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
                   WRITEMSG
                   return 1;
                 }
                 vtcp_log("[%s][%d] zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 /**6.��ԭ�ݹ�ǰg_pub_tx��Ϣ.**/
                 strcpy(g_pub_tx.ac_no,cAc_no_tmp);
                 g_pub_tx.ac_id = lAc_id_tmp;
                 g_pub_tx.ac_seqn = iAc_seqn_tmp;
                 g_pub_tx.tx_amt1 = dTx_amt_tmp;
                 strcpy(g_pub_tx.add_ind,cAdd_ind_tmp);
                 strcpy(g_pub_tx.hst_ind,cHst_ind_tmp);
                 strcpy(g_pub_tx.ac_wrk_ind,cAc_wrk_ind_tmp);
                 g_pub_tx.svc_ind = iSvc_ind_tmp;
                 strcpy(g_pub_tx.ct_ind,cCt_ind_tmp);
                 strcpy(g_pub_tx.ac_id_type,cAc_id_type_tmp);
                 strcpy(g_pub_tx.note_type,cNote_type_tmp);
                 strcpy(g_pub_tx.beg_note_no,cBeg_note_no_tmp);
                 
                 set_zd_data("102K",g_pub_tx.ct_ind);/**��������..**/
                 
                 /****�ڴ˴���ȥD003�Ľ��׶�  20130222****/
                 g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
                 /****����Ϣֵ��գ�����D003���յ� add at 20130218****/
                 g_pub_intst.dealval = 0.00;
                 g_pub_intst.keepval = 0.00;
                 g_pub_intst.val     = 0.00;
                 /****end 20130218****/
                 
                 /**7.�������ճ�����.��������.�����˻�״̬.ǩԼ���ռ�¼״̬.**/
                 memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
                 memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
                 ret = Mo_prdt_prot_Dec_Upd(g_pub_tx.reply," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_pub_tx.ac_id);
                 if(ret)
                 {
                       sprintf(acErrMsg, "�����α����!");
                       WRITEMSG
                       strcpy(g_pub_tx.reply, "P162");
                       goto ErrExit;
                 }
                 ret = Mo_prdt_prot_Fet_Upd(&sMo_prdt_prot,g_pub_tx.reply);
                 if(ret == 100)
                 {
                     sprintf(acErrMsg,"�޼�¼..");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Mo_prdt_prot_Fet_Upd error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 sMo_prdt_prot.sts[0] = '2';/**��ʱ��Լ.**/
                 ret = Mo_prdt_prot_Upd_Upd(sMo_prdt_prot,g_pub_tx.reply);
                 if(ret)
                 {
                    sprintf(acErrMsg, "�޸ļ�¼����!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "P162");
                    goto ErrExit;
                 }
                 Mo_prdt_prot_Clo_Upd();
                 vtcp_log("[%s][%d] ssssssBrf =[%s]",__FILE__,__LINE__,Brf);
                 strcpy(g_pub_tx.brf, Brf);/** 20140325**/
                 vtcp_log("[%s][%d] g_pub_tx.brf =[%s]",__FILE__,__LINE__,g_pub_tx.brf);
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 /**8.������ǰ.trance����.**/
                 vtcp_log("[%s][%d] zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 /**a.���仯.��ǰ��������仯����.δ���ĵ����ݿ�.ǩԼ�˻����ʱ trance()��.�����ĵ����ݿ�.**/
                 sprintf(acErrMsg,"!!![%s][%d] �������仯ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
                 WRITEMSG
                 vtcp_log("�������׽��1=[%lf] bal=[%lf] ac_id=[%ld] ac_seqn=[%d]",g_pub_tx.tx_amt1,g_dd_mst.bal,g_dd_mst.ac_id,g_dd_mst.ac_seqn);
                 g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                     vtcp_log("�������׽��2=[%lf]",g_dd_mst.bal);
                 g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;     
                 sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
                 WRITEMSG
                 /*** ��������Ϣ�����ĺ��� ***/

             }
            if(pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt,g_dd_parm.min_bal)<0 && g_dd_mst.ac_sts[0]=='1' && g_dd_mst.ac_type[0]!='6')
             { 
                if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
                {
                    Remark_ctl_Sel(g_pub_tx.reply,&rem,"ac_id=%d AND rem_sts!='2'",g_dd_mst.ac_id);
                    strcpy( g_pub_tx.reply,"D247");
                    rem.rem[50]='\0';
                    sprintf(msg,"%-50s",rem.rem);
                    set_zd_data("0130",msg);
                    vtcp_log(" MSG=[%s]",msg);
                    goto ErrExit;
                }
                sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",g_dd_mst.bal,g_dd_parm.min_bal );
                WRITEMSG
                strcpy(g_pub_tx.reply,"A022" );
                goto ErrExit;
             }
        }
    }
    else if( g_pub_tx.add_ind[0]=='0' )
    {
        if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_mst.ctl_amt)<0 )
        {
            if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
            {
                if( Remark_ctl_Sel(g_pub_tx.reply,&rem,
                    "ac_id=%d AND rem_sts!='2'", g_dd_mst.ac_id) )
                {
                    strcpy( rem.rem,"" );
                }
                pub_base_strpack( rem.rem );
                rem.rem[50]='\0';

                strcpy( g_pub_tx.reply,"D247");
                sprintf(msg,"%-50s",rem.rem);
                set_zd_data("0130",msg);
                vtcp_log(" MSG=[%s]",msg);
                goto ErrExit;
            }
        }
    }
vtcp_log("XXXXXXX[%.2lf]",g_dd_mst.bal);
    /******��������������ҵ�񼫶�������жϲ�׼ȷ   by jk 20110515
    if( g_dd_mst.bal<-0.005 && g_pub_tx.add_ind[0]!='1' )
    ****/
    if( g_dd_mst.bal<-0.005 && g_dd_mst.od_ind[0]!='Y')
    {
        strcpy(g_pub_tx.reply,"A022" );
        goto ErrExit;
    }

    if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_bal)>0 
        && pub_base_CompDblVal(g_dd_parm.max_bal,0.00) 
        && g_dd_mst.ac_type[0]!='6' )
    {
        sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
                g_dd_mst.bal,g_dd_parm.max_bal );
        WRITEMSG
        strcpy(g_pub_tx.reply,"A008" );
        goto ErrExit;
    }

    if( g_pub_tx.svc_ind==1003 )        /*����*/
    {
        /**��Ϣ����**/
        /**��Ϣ˰֧ȡ**/
        /**����֧ȡ���**/

        /* add by xxx at 20030726 ***/
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            ret=pub_ins_dd_hst();
            ERR_DEAL
        }
    }
    else if( g_pub_tx.hst_ind[0]=='1' )     /*�ռ�����ϸ*/
    {
        ret=pub_ins_dd_hst();
        ERR_DEAL
    }
    if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) != 0)  
    {
      vtcp_log("%s,%d,���������֪ͨ[%.2f]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
      pub_mob_sendmail( g_pub_tx.brf, g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_dd_mst.bal );
		}
		
    vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
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
    
    sprintf(acErrMsg,"===chenming:  g_pub_tx.tx_amt1=[%.2lf]  g_dd_mst.bal=[%.2lf] ===",g_pub_tx.tx_amt1,g_dd_mst.bal);
        WRITEMSG 
    ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();

    ret=pub_acct_ac_rec( g_dd_mst.ac_id,g_dd_mst.ac_seqn,
        g_pub_tx.add_ind,g_pub_tx.ct_ind,g_pub_tx.tx_code,g_pub_tx.tx_amt1,"0000" );
    if( ret ) goto ErrExit;

OkExit:
    return 0;
ErrExit:
    return 1;
}

int Chg_td_mst()
{
    struct mdm_ac_rel_c o_mdm_ac_rel;
    int ret;
    long l_date;
    long num;
    char msg[51];
    char kh_type[2];
    struct td_mst_c TmpTd_mst;
    struct  remark_ctl_c rem;
    struct  mo_hold_c  g_mo_hold;
    char  minholdtyp[2];
  long   hold_seqn;
  long   ac_seqn;
  memset(minholdtyp,0,sizeof(minholdtyp));
    memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));


    /*** ���ö��ڲ�Ʒ���������麯�� ***/
    partflag=0;
    wdr_flag=0;

    /*** �������ļ���� ***/
    ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id='%ld' and ac_seqn='%d' ", \
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    memcpy( &o_td_mst,&g_td_mst,sizeof(g_td_mst) );
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    if( g_td_mst.ac_sts[0]=='*' )
    {
        sprintf(acErrMsg,"�˻�����!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P245");
        goto ErrExit;
    }

    if( g_td_mst.ac_sts[0]!='1' && g_td_mst.ac_sts[0] != '#')
    {
        sprintf(acErrMsg,"�˺�״̬������״̬![%s]",g_td_mst.ac_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }
    /**  add by  shiweina  20071217  **/
  if(strcmp(g_pub_tx.tx_code,"2430")==0)
  {
    sprintf(acErrMsg,"[%s][%d]tx_code=%ld",__FILE__,__LINE__,g_pub_tx.tx_code);
   

  }                /**  ���˽���   **/
  else
  {
    /*���ӿۻ����⴦�� 2011/11/13 14:25:13*/
    if( g_td_mst.hold_sts[0]=='1' && memcmp(g_pub_tx.tx_code,"4713",4)!=0)
    {
        sprintf(acErrMsg,"���˺��ѱ���ȫ����!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P110");
        goto ErrExit;
    }
  }
    /* �����ڲ�Ʒ�ṹ��ֵ */
    ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",g_td_mst.prdt_no);
    ERR_DEAL  

    strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);

    strcpy(g_pub_tx.name,g_td_mst.name);
    strcpy(g_pub_tx.opn_br_no,g_td_mst.opn_br_no);
    g_pub_tx.cif_no=g_td_mst.cif_no;

    /***** add by xxx **/
    g_pub_tx.mach_date = g_td_mst.mtr_date;
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

    /*ȡ�������Э������*/
    if( g_pub_tx.add_ind[0]=='0' )
    {
        if (g_pub_tx.ct_ind[0]=='1' && g_td_parm.cash_ind[0] == 'N')
        {
            sprintf(acErrMsg,"�ò�Ʒ����������!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P223");
            goto ErrExit;
        }

        if( g_pub_tx.ct_ind[0]=='1' && g_td_mst.odtt_ind[0]=='1' )
        {
            sprintf(acErrMsg,"�㻧������ȡ��!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P108");
            goto ErrExit;
        }

        /**
        if( g_td_parm.thr_draw_ind[0]=='N'
              && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
        {
            sprintf(acErrMsg,"����ͨ��[%s]!",g_td_parm.thr_draw_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A011");
            goto ErrExit;
        }
        **/

        /*** ����У��֧ȡ��ʽ��У�����뺯�� ***/
        /*���ӿۻ����⴦��  �ۻ���У������2011/11/13 15:35:51*/
        if(memcmp(g_pub_tx.tx_code,"4713",4) != 0)
        {
            ret = pub_base_check_draw("001");
            ERR_DEAL
        }
        
        /**   add  by shiweina  20071217  **/
        if(strcmp(g_pub_tx.tx_code,"2430")==0)
        {
             sprintf(acErrMsg,"[%s][%d]tx_code=%ld",__FILE__,__LINE__,g_pub_tx.tx_code);
          }      /**   ���˽���  **/
          else 
        {
                /*���ӿۻ����⴦��  2011/11/13 14:26:11*/
              if( g_td_mst.hold_sts[0]=='2' && memcmp(g_pub_tx.tx_code,"4713",4)!=0 )
              {
                sprintf(acErrMsg,"���˺��ѱ�ֻ����������!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P111");
                goto ErrExit;
              }
        }

sprintf(acErrMsg, "--------> ��������:[%ld]", g_pub_tx.mach_date);
WRITEMSG

        /*** ���ü�����Ϣ�����ĺ��� **** ������ȡ �������� ��������*/
        if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' ||
            g_td_parm.td_type[0] == '7' )
        {
            if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date,
                g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
                g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
                g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                goto ErrExit;
            }
        }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        if( g_pub_tx.svc_ind==1004 )
            g_pub_tx.tx_amt1=g_td_mst.bal;

        if( g_td_parm.td_type[0]=='3' && g_pub_tx.svc_ind==1001 )
        {
            ;
        }
        else
        {
            g_td_mst.bal=g_td_mst.bal-g_pub_tx.tx_amt1;
            g_td_mst.acbo_bal=g_td_mst.acbo_bal-g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
            {
                g_td_mst.hst_bal=g_td_mst.hst_bal-g_pub_tx.tx_amt1;
            }
        }
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

        switch(g_pub_tx.svc_ind)
        {
            case 1012:
            case 1001:  /*ȡ��*/
                sprintf(acErrMsg, "CCCCCCCC*********��������:[%ld]", 
                    g_pub_tx.tx_date);
                WRITEMSG
                if( g_td_parm.thr_draw_ind[0]=='N'
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"����ͨ��[%s]!",g_td_parm.thr_draw_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A011");
                    goto ErrExit;
                }

                /* ����Ƿ������ȡ��� ** �������� �������� */
                if (g_pub_tx.svc_ind != 1012)
                {
                    if ( g_td_parm.td_type[0] == '1' || 
                         g_td_parm.td_type[0] == '6' || 
                         g_td_parm.td_type[0] == '7' )
                    {
                        /*���ӿۻ����⴦�� 2011/11/13 14:27:10*/
                        if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                        {
                            sprintf(acErrMsg,"�ò�Ʒ��������ȡ���!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P219");
                            goto ErrExit;   
                        }
                        
                    }
                }

                /* ����Ǽ���Ƿ������� */
                if (g_td_parm.td_type[0] == '0' && g_pub_tx.svc_ind != 1012)
                /* ��ͨ������ȡ */
                {
                    /* ������Ҫ�� �Զ�ת�治������ add by wudawei 20131108
			if (g_pub_tx.tx_date >= g_td_mst.mtr_date)
                    {
                        sprintf(acErrMsg,"���˻��Ѿ�����,������������!!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"A034");
                        goto ErrExit;
                    }
			add end 20131108 */
                    if (g_td_parm.wdr_ind[0] == 'N')
                    {
                        sprintf(acErrMsg,"��ͨ������ȡ��������!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P215");
                        goto ErrExit;
                    }

                    g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1; /* ������� */

                    if ( g_td_parm.wdr_lmt_cnt != 0 
                        && g_td_mst.tx_cnt > g_td_parm.wdr_lmt_cnt)
                    {
                        sprintf(acErrMsg,"������������涨�Ĵ���!!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P216");
                        goto ErrExit;
                    }
                    vtcp_log("AC_SEQN[%d][%s]",g_pub_tx.ac_seqn,g_pub_tx.ac_no);
                    if( !g_pub_tx.ac_seqn )
                    {
                        /*�ۻ����⴦�� 2011/11/13 14:31:56 �ۻ�����ƾ֤*/
                        if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                        {
                            wdr_flag=1;
                        }
                        vtcp_log("[%s][%d] ���Ը���wdr_flag = %d\n",__FILE__,__LINE__,wdr_flag);
                    }
                    /**--- xxxx --- ����Ҫ�ڴ˴��� ----**
                    else
                    {
                        **һ��ͨҪ���һ��**
                        g_mdm_ac_rel.prt_line++;
                    }
                    ***------------------------------***/

                    partflag=1;
    sprintf(acErrMsg,"WDR����%d",wdr_flag);
    WRITEMSG
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
                }
            if ( (g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0]=='1') && g_pub_tx.svc_ind != 1012 )
                /* ������ȡ ����ȡϢ */
                {
                    if( pub_base_CompDblVal(g_pub_tx.tx_amt1,
                                     g_td_mst.td_amt)!=0 )
                    {
                        /*���ӿۻ����⴦�� 2011/11/13 14:33:52*/
                        if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                        {
                            sprintf(acErrMsg,"ȡ�����!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P115");
                            goto ErrExit;
                        }
                    }
 
                    g_td_mst.tx_cnt++;
                    if( g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
                    {
                        sprintf(acErrMsg,"ȡ����ܴ���!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P117");
                        goto ErrExit;
                    }
                    /**********/
                    if (g_td_mst.ttl_cnt == g_td_mst.tx_cnt)
                    {
                        sprintf(acErrMsg,"���������һ�ν���Ӧ����������!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P229");
                        goto ErrExit;
                    }
                    /***********/
                    switch(g_td_parm.intvl_type[0])
                    {
                        case 'Y':
                            break;
                        case 'Q':
                            break;
                        case 'M':
                            num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                            ret = pub_base_deadlineM(g_td_mst.ic_date,num,
                                  &l_date);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������ȡ��ʱ�����!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P228");
                                goto ErrExit;
                            }
                            if (g_pub_tx.tx_date < l_date)
                            {
                                sprintf(acErrMsg,"��û�е�ÿ��ȡ���ʱ��!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P218");
                                goto ErrExit;
                            }
                            break;
                        case 'D':
                            num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                            ret = pub_base_deadlineD(g_td_mst.ic_date,num,
                                  &l_date);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"������ȡ��ʱ�����!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P228");
                                goto ErrExit;
                            }
                            if (g_pub_tx.tx_date < l_date)
                            {
                                sprintf(acErrMsg,"��û�е�ÿ��ȡ���ʱ��!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P218");
                                goto ErrExit;
                            }
                            break;
                        default:
                            break;
                    }
                }

sprintf(acErrMsg,"*****WDR����%d",wdr_flag);
WRITEMSG
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

sprintf(acErrMsg, "td_type=[%s], svc_ind=[%ld], tx_amt1=[%lf], bal=[%lf],\
    ac_seqn=[%d]", g_td_parm.td_type, g_pub_tx.svc_ind, g_pub_tx.tx_amt1,
    g_td_mst.bal, g_pub_tx.ac_seqn);
WRITEMSG
                /* ֪ͨȡ��ļ�� delete by xxx at 20030822
                if ( g_td_parm.td_type[0]=='4' && g_pub_tx.svc_ind != 1012)
                {
                    if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.bal)<0 )
                    {
                        if( !g_pub_tx.ac_seqn )
                            wdr_flag=1;
                    }

                    partflag=1;
                }*/

                /* ֪ͨȡ��ļ�� add by xxx */
                if ( g_td_parm.td_type[0]=='4' && g_pub_tx.svc_ind != 1012)
                {
                    if( pub_base_CompDblVal(g_td_mst.bal) > 0 )
                    {
                        if( !g_pub_tx.ac_seqn )
                        {
                            /*���ӿۻ����⴦�� �ۻ�����ƾ֤*/
                            if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                            {   
                                wdr_flag=1;
                            }
                            else
                            {
                                wdr_flag=0;
                            }
                            vtcp_log("[%s][%d] ֪ͨ wdr_flag = %d",__FILE__,__LINE__,wdr_flag);
                        }    
                    }

                    partflag=1;
                }
sprintf(acErrMsg,"*****WDR����%d, partflag=[%d]",wdr_flag, partflag);
WRITEMSG
                /* ������Ϣ ** ������ȡ �汾ȡϢ ���ü�����Ϣ */
                if ( g_td_parm.td_type[0] != '2' && \
                     g_td_parm.td_type[0] != '3' && \
                     g_pub_tx.svc_ind != 1012 )
                {
                sprintf(acErrMsg, "CCCCCCCC*********��������:[%ld]", 
                    g_pub_tx.tx_date);
                WRITEMSG
                    ret = pub_base_CalIntst(&g_pub_intst);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P220");
                        goto ErrExit;
                    }
                    sprintf(acErrMsg,"ȡ����ϢΪ:,[%lf]",g_pub_intst.factval);
                    WRITEMSG
                }

                /* ������ȡ���� */
                if (g_pub_tx.svc_ind != 1012)
                {
                    if (pub_base_CompDblVal(g_pub_tx.tx_amt1,
                                        g_td_parm.draw_min_amt) == -1)
                    {   
                        sprintf(acErrMsg,"ȡ����С�����ȡ����!!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P217");
                        goto ErrExit;
                    }

                    if( partflag ) g_pub_tx.svc_ind=1011; /**����ı�־**/
                }

                break;
            case 1004:              /*�������*/
                strcpy(g_pub_tx.brf, "�������");
            case 1003:                  /*����*/
                if( g_pub_tx.svc_ind==1003 )
                    strcpy(g_pub_tx.brf, "����");
                /* ����Ƿ���ǰ֧ȡ */
                if (g_td_parm.td_type[0] != '6' \
                         && g_pub_tx.tx_date < g_td_mst.mtr_date)
                {
                    /* ��������û����ǰ֧ȡ */
                    ret = pub_base_checktxdate(g_pub_tx.tx_date,
                          g_td_mst.mtr_date);
                    if (ret != 0 && ret != 100) 
                    {
                        sprintf(acErrMsg,"�ж��Ƿ���ǰ֧ȡ����!!!");
                        WRITEMSG
                        goto ErrExit;
                    }
                    else if (ret == 100)
                    {
                        if (g_td_parm.fwd_draw_ind[0] == 'N')
                        {
                            sprintf(acErrMsg,"�ò�Ʒ��������ǰ֧ȡ!!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P222");
                            goto ErrExit;
                        }
                    }
                }

                if( g_td_parm.thr_cls_ind[0]=='N'
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"�ò�Ʒ������ͨ������!!!![%s]",
                            g_td_parm.thr_cls_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P224");
                    goto ErrExit;
                }
                
                /*** ���ִ浥������ͨ������ ***/
                if( (!strncmp(g_mdm_ac_rel.mdm_code,"0014",4) ||
                     !strncmp(g_pub_tx.mdm_code,"0014",4) ) 
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"���ִ浥�˻�������ͨ������!!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"D219");
                    goto ErrExit;
                }

                if( pub_base_CompDblVal(g_td_mst.bal,0.00) 
                   || pub_base_CompDblVal(g_td_mst.acbo_bal,0.00) )
                {
                    sprintf(acErrMsg,"��������Ϊ��![%lf][%lf]", \
                            g_td_mst.bal,g_td_mst.acbo_bal);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P116");
                    goto ErrExit;
                }
                /* ����������������� */
                /**---- xxxx ---- һ��ͨ�������ܼ� ----**/
            /***����һ��ͨ����Ҫ��������    
                if( g_pub_tx.ac_seqn )
                {
                    g_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line-2;
                }
                ���ι�����***/
                /**-----------------------------**/

                /* ������Ϣ */
                if (g_td_mst.ac_sts[0] != '#')
                {
                    ret = pub_base_CalIntst(&g_pub_intst);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P220");
                        goto ErrExit;
                    }
                    sprintf(acErrMsg,"CCCCCCC************ȡ����ϢΪ:,[%lf]",
                        g_pub_intst.factval);
                    WRITEMSG
                }

                strcpy(g_td_mst.ac_sts,"*");
                break;
            default:
                sprintf( acErrMsg,"�������ʹ���[%d]",g_pub_tx.svc_ind );
                WRITEMSG
                strcpy( g_pub_tx.reply,"D999" );
                goto ErrExit;
        }
        /**   add by  shiweina **/
    if(strcmp(g_pub_tx.tx_code,"2430")==0)     
    {  get_zd_data("0700",kh_type);
        vtcp_log("%s,%d,Gethere7!!!!notice!FD70kh_type=[%s]\n",__FILE__,__LINE__,kh_type);
        if(kh_type[0]=='2')
        {
            vtcp_log("%s,%d,Gethere7!!!!notice!FD70kh_type=[%s]\n",__FILE__,__LINE__,kh_type);
      if(g_td_mst.hold_sts[0]=='3' )/*���ֶ���*/
      {
        get_zd_long("0350",&hold_seqn);
        get_zd_long("0340",&ac_seqn);
        ret = Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn=%ld  and ac_id=%ld  and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
        if(ret==100)
        {
           sprintf(acErrMsg," ���ݿ��޴˼�¼[%s]",g_pub_tx.reply);
           WRITEMSG
           goto ErrExit;
        }
        if(ret!=0)
        {
           sprintf(acErrMsg,"��ѯ����Ǽǲ�����[%s]",g_pub_tx.reply);
           WRITEMSG
           goto ErrExit;
        }
        if(fabs( g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)<0.005)
        {
           ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld  and  ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
           if(ret)
           {
              sprintf(acErrMsg,"ִ��Mo_hold_Dec_Upd����[%d]",ret);
              WRITEMSG
              goto ErrExit;
           }
           ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
           if(ret==100)
           {
              sprintf( acErrMsg,"��ѯ�Ǽǲ���,�޼�¼");
               WRITEMSG
              goto ErrExit;
           }
           else if (ret)
           {
              sprintf( acErrMsg,"ִ��Mo_hold_Fet_Upd����[%d]",ret);
              WRITEMSG
              goto ErrExit;
            }
           strcpy(g_mo_hold.hold_sts,"1");
           g_mo_hold.fct_unhold_date = g_pub_tx.tx_date;
           strcpy(g_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
           strcpy(g_mo_hold.unhold_tel,g_pub_tx.tel);
           strcpy(g_mo_hold.unhold_brf,g_pub_tx.brf);
           g_mo_hold.wrk_date = g_pub_tx.tx_date;
           g_mo_hold.trace_no = g_pub_tx.trace_no;
           ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
           if(ret)
           {
               sprintf(acErrMsg," �޸ļ�¼����[%d]",ret);
               WRITEMSG
               goto ErrExit;
           }
               Mo_hold_Clo_Upd();
        } 
        else if (fabs( g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)>0.005)
        {
          ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and  ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
          if (ret)
          {
              sprintf(acErrMsg,"ִ��Mo_hold_Dec_Upd����[%d]",ret);
              WRITEMSG
              goto ErrExit;
          }
          ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
          if(ret==100)
          {
              sprintf( acErrMsg,"��ѯ�Ǽǲ���,�޼�¼");
              WRITEMSG
              goto ErrExit;
          }
          else if(ret)
          {
              sprintf( acErrMsg,"ִ��Mo_hold_Fet_Upd����[%d]",ret);                           WRITEMSG
              goto ErrExit;
          } 
  
          g_mo_hold.plan_hold_amt=g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1;
         g_mo_hold.wrk_date = g_pub_tx.tx_date;
         g_mo_hold.trace_no = g_pub_tx.trace_no;
         ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
         if(ret)
         {
             sprintf(acErrMsg," �޸ļ�¼����[%d]",ret);
             WRITEMSG
             goto ErrExit;
         }
            Mo_hold_Clo_Upd();
        }  else if(fabs(g_pub_tx.tx_amt1-g_mo_hold.plan_hold_amt)>0.005)                   
             {
               sprintf(acErrMsg,"�ۻ������ڶ�����");
               WRITEMSG
               goto ErrExit;
           }
    }
    else if(g_td_mst.hold_sts[0]=='2' || g_td_mst.hold_sts[0]=='1' )/*�ǲ��ֶ���*/
    {
      get_zd_long("0350",&hold_seqn);
      get_zd_long("0340",&ac_seqn);
      ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld  and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
      if(ret)
      {
         sprintf(acErrMsg,"ִ��Mo_hold_Dec_Upd����[%d]",ret);
         WRITEMSG
         goto ErrExit;
      }
      ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
      if(ret==100)
      {
        sprintf( acErrMsg,"��ѯ�Ǽǲ���,�޼�¼");
        WRITEMSG
        goto ErrExit;
      }
      else if (ret)
      {
        sprintf( acErrMsg,"ִ��Mo_hold_Fet_Upd����[%d]",ret);
        WRITEMSG
        goto ErrExit;
      }
       g_mo_hold.wrk_date = g_pub_tx.tx_date;
       g_mo_hold.trace_no = g_pub_tx.trace_no;
      ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
      if(ret)
      {
         sprintf(acErrMsg," �޸ļ�¼����[%d]",ret);
         WRITEMSG
         goto ErrExit;
      }
         Mo_hold_Clo_Upd();
    } 
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    double dj_dbal=0.00;
    ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld  and ac_seqn=%ld",g_mdm_ac_rel.ac_id,ac_seqn);
    if(ret)
    {
       sprintf(acErrMsg,"ִ��Td_mst_Dec_Upd����[%s]",g_pub_tx.reply);
       WRITEMSG
       goto ErrExit;
    } 
    ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
    if(ret==100)
    {
       sprintf(acErrMsg,"��ѯ����������,�޼�¼");
       WRITEMSG
       goto ErrExit;
    }
    if(ret)
    {
       sprintf(acErrMsg,"��ѯ�����������쳣[%s]",g_pub_tx.reply);
       WRITEMSG
       goto ErrExit;
    }
    ret=sql_sum_double("mo_hold","plan_hold_amt",&dj_dbal,"hold_sts='0' and ac_id=%ld and ac_seqn=%ld",g_pub_tx.ac_id,ac_seqn);
    if(ret)
    {
         sprintf(acErrMsg,"��������쳣[%s]",g_pub_tx.reply);
         WRITEMSG
          goto ErrExit;
    }
    g_td_mst.hold_amt=dj_dbal; 
    ret=sql_min_string("mo_hold","hold_typ",minholdtyp,1,"hold_sts='0' and ac_id=%ld  and ac_seqn",g_pub_tx.ac_id,ac_seqn);
    if(ret)
    {
        sprintf(acErrMsg,"��������쳣[%s]",g_pub_tx.reply);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    strcpy(g_td_mst.hold_sts,minholdtyp);
    ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    if(ret)
    {
       sprintf(acErrMsg,"Td_mst_Upd_Upd����=[%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
       Td_mst_Clo_Upd();
    }
    
    }      /**     ���˽���   **/
    else
    {
	if( g_td_mst.hold_sts[0]=='3' && g_td_mst.bal<g_td_mst.hold_amt && memcmp(g_pub_tx.tx_code,"4713",4)!=0)
          {
            sprintf(acErrMsg,"���˺��ѱ����ֶ���!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P111");
            goto ErrExit;
          }
      }
        /* ����ʻ���� */
        if ( pub_base_CompDblVal(g_td_mst.bal,0.00)<0 )
        {
            sprintf(acErrMsg,"ȡ�������ʻ����!!bal=[%lf] amt1=[%lf]",
                g_td_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P214");
            goto ErrExit;
        }
vtcp_log("CDBZ[%s][%s]",g_pub_tx.tx_code,g_pub_tx.tel);
        if( !strcmp(g_pub_tx.tx_code,"G090") && 
            !strcmp(g_pub_tx.tel,"9999") )
        {
            vtcp_log("�жҿۿ�ܱ�עCDKKBGBZ");
        }
        else if( pub_base_CompDblVal(g_td_mst.bal,g_td_mst.ctl_amt)<0 )
        {
            if( pub_base_CompDblVal(g_td_mst.ctl_amt,0.00) )
            {
                if( Remark_ctl_Sel(g_pub_tx.reply,&rem,
                    "ac_id=%d AND rem_sts!='2'", g_td_mst.ac_id) )
                {
                    strcpy( rem.rem,"" );
                }
                pub_base_strpack( rem.rem );
                rem.rem[50]='\0';

                strcpy( g_pub_tx.reply,"D247");
                sprintf(msg,"%-50s",rem.rem);
                set_zd_data("0130",msg);
                vtcp_log(" MSG=[%s]",msg);
                TRACE
                goto ErrExit;
            }
        }
    }
    else if( g_pub_tx.add_ind[0]=='1' )     /*���*/
    {
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        if( g_pub_tx.svc_ind!=1001 && g_pub_tx.svc_ind!=1000 \
            && g_pub_tx.svc_ind!=1008 && g_pub_tx.svc_ind!= 1012)
        {
            sprintf( acErrMsg,"�������ʹ���[%d]",g_pub_tx.svc_ind );
            WRITEMSG
            strcpy( g_pub_tx.reply,"D999" );
            goto ErrExit;
        }

        /** 1-�������棬7-���������� **/
        if (g_pub_tx.svc_ind != 1012)
        {
            if (g_td_parm.td_type[0] != '1' && g_td_parm.td_type[0] != '7')
            {
                sprintf(acErrMsg,"�ò�Ʒ�Ķ��ڴ�����Ͳ�����������!!! \
                    [%s]",g_td_parm.td_type);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P225");
                goto ErrExit;
            }
            if (g_td_parm.dpst_ind[0] == 'N')
            {
                sprintf(acErrMsg,"�ò�Ʒ����������!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P225");
                goto ErrExit;
            }
        }

        if ( strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
              && g_td_parm.thr_dpst_ind[0] == 'N' && g_pub_tx.svc_ind!=1012 )
        {
            sprintf(acErrMsg,"�ò�Ʒ������ͨ��!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P226");
            goto ErrExit;
        }

        /*** ���ü�����Ϣ�����ĺ��� **** �������� */
        if (g_pub_tx.svc_ind != 1012)
        {
            if ( g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '7' )
            {
                if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
                    g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
                    g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
                    g_td_mst.intst_type,g_td_parm.intst_mon,
                    g_td_parm.intst_day) )
                {
                    sprintf(acErrMsg,"������Ϣ��������!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
        }

        g_td_mst.bal=g_td_mst.bal+g_pub_tx.tx_amt1;
        g_td_mst.acbo_bal=g_td_mst.acbo_bal+g_pub_tx.tx_amt1;

    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
            g_td_mst.hst_cnt++;
        }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

        if (g_pub_tx.svc_ind != 1012)
        {
            if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.td_amt)!=0 
                && g_td_parm.td_type[0]=='1' )
            {
                sprintf(acErrMsg,"������!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P118");
                goto ErrExit;
            }

            /*** ����������� ***/
            if( g_td_parm.td_type[0]=='1' && pub_chk_td_hst(1)!=0 )
            {
                sprintf(acErrMsg,"������������!");
                WRITEMSG
                goto ErrExit;
            }

            /*** ����������� ***/
            if( g_td_parm.td_type[0]=='7' && pub_chk_td_hst(2)!=0 )
            {
                sprintf(acErrMsg,"�������������!");
                WRITEMSG
                goto ErrExit;
            }

            /** add by xxx at 20030727 ***/
            if (g_td_parm.td_type[0] == '7' && pub_chk_JYCX() != 0)
            {
                sprintf(acErrMsg, "һ�����ڲ��ܴ�����!!");
                WRITEMSG
                goto ErrExit;
            }

            if (pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_parm.dpst_min_amt) == -1)
            {
                sprintf(acErrMsg,"�����С�����������!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P227");
                goto ErrExit;
            }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

            g_td_mst.tx_cnt++;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
            if( g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
            {
                sprintf(acErrMsg,"�����ܴ���!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P119");
                goto ErrExit;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
            }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    }
    else
    {
        sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P106");
        goto ErrExit;
    }

    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
sprintf(acErrMsg,"WDR����%d",wdr_flag);
WRITEMSG
    g_td_mst.lst_date=g_pub_tx.tx_date;

    /* add by xxx at 20030728 ** �������ü�� */
    sprintf(acErrMsg, "******** svc_ind = [%ld]", g_pub_tx.svc_ind);
    WRITEMSG
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG

    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_bal)<0 
        && g_td_mst.ac_sts[0]=='1' && memcmp(g_pub_tx.tx_code,"4713",4)!=0) /*���ӿۻ����⴦�� 2011/11/13 14:43:45*/
    {
        sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
            g_td_mst.bal,g_td_parm.min_bal );
        WRITEMSG
        strcpy(g_pub_tx.reply,"A022" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.max_bal)>0 
        && pub_base_CompDblVal(g_td_parm.max_bal,0.00) 
        && g_pub_tx.svc_ind != 1012)
    {
        sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
            g_td_mst.bal,g_td_parm.max_bal );
        WRITEMSG
        strcpy(g_pub_tx.reply,"A008" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

    sprintf(acErrMsg, "******�ռ�����ϸ��־:[%s]", g_pub_tx.hst_ind);
    WRITEMSG
    if (g_pub_tx.hst_ind[0]== '1') /*�ռ�����ϸ*/
    {
        strcpy(g_pub_tx.name,g_td_mst.name);
        g_pub_tx.cif_no=g_td_mst.cif_no;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        ret=pub_ins_td_hst();
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        ERR_DEAL
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

    ret=Td_mst_Dec_Upd( g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d'", \
                        g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) != 0)  
    {
      vtcp_log("%s,%d,���������֪ͨ[%.2f]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
      pub_mob_sendmail(  g_pub_tx.brf, g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_td_mst.bal );
		}

    ret=Td_mst_Fet_Upd(&TmpTd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    Td_mst_Clo_Upd();

    ret=pub_acct_ac_rec( g_td_mst.ac_id,g_td_mst.ac_seqn,
        g_pub_tx.add_ind,g_pub_tx.ct_ind,g_pub_tx.tx_code,g_pub_tx.tx_amt1,"0000" );
    if( ret ) goto ErrExit;
OkExit:
    return 0;
ErrExit:
    return 1;
}

int Chg_ln_mst()
{
    int ret;
    int do_flag=0;
    double yqlx=0.00;
    struct ln_mst_c tmp_ln_mst;
    TRACE
    sprintf(acErrMsg,"ǰ(�������)PASSΪ [%lf]",g_ln_mst.intst_acm );
    WRITEMSG
    memset( &tmp_ln_mst , 0x00 , sizeof( struct ln_mst_c ));

    /*** ���ô����Ʒ���������麯�� ***/
    sprintf(acErrMsg,"������ʹ������,�ѷ�JJJJ[%.2lf][%.2lf][%.2lf][%s]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt,g_pub_tx.ac_get_ind);
    WRITEMSG
    sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    if( g_pub_tx.ac_get_ind[1]!='1' ) /**δ��ȡln_mst**/
    {
        TRACE
    sprintf(acErrMsg,"������ʹ������,�ѷ�[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
        memset( &g_ln_mst , 0x00 , sizeof( struct ln_mst_c ));
    sprintf(acErrMsg,"������ʹ������,�ѷ�[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
        ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
            g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    WRITEMSG
        ERR_DEAL
    sprintf(acErrMsg,"������ʹ������,�ѷ�[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG

        ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
            WRITEMSG
            if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    sprintf(acErrMsg,"������ʹ������,�ѷ�[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
    }else{
         TRACE
         ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn);
         ERR_DEAL

         ret=Ln_mst_Fet_Upd(&tmp_ln_mst,g_pub_tx.reply);
         if( ret )
         {
             sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
             WRITEMSG
             if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
             goto ErrExit;
         }
    }

    sprintf(acErrMsg,"ǰ(�������)PASSΪ [%lf]",g_ln_mst.intst_acm );
    WRITEMSG

    sprintf(acErrMsg,"������ʹ������,�ѷ�[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
    TRACE
    ret = Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,
            " prdt_no='%s' ",g_ln_mst.prdt_no );
    ERR_DEAL
    sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );
    strcpy( g_pub_tx.opn_br_no,g_ln_mst.opn_br_no );
    set_zd_data("0020", g_pub_tx.opn_br_no);
    strcpy( g_pub_tx.name,g_ln_mst.name );
    g_pub_tx.cif_no=g_ln_mst.cif_no;
    strcpy(g_pub_tx.prdt_code,g_ln_parm.prdt_no);
    sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    /**�˻�״̬���**/
    if( g_ln_mst.ac_sts[0]=='*' )
    {
        sprintf(acErrMsg,"�����˺�������![%s]",g_ln_mst.ac_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P120");
        goto ErrExit;
    }
    TRACE
    if( g_ln_mst.ac_sts[0]=='9' )
    {
        sprintf(acErrMsg,"�����˺��ѳ���![%s]",g_ln_mst.ac_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"L036");
        goto ErrExit;
    }
    /*lpj20151129֮��û�д�����������dc_code����ln_parm.dc_code*/
		strcpy(g_pub_tx.dc_code,g_ln_parm.dc_code); 
		TRACE
   /* if( g_ln_mst.ac_sts[0]=='1' )* ���� *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.dc_code); 
    }
    if( g_ln_mst.ac_sts[0]=='2' )* ����90�� *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.over_dc_code);    
    }
    **add by ligl2006-9-25 17:31*
    if(g_ln_mst.ac_sts[0]=='5')**����90��*
    {
    TRACE
       strcpy(g_pub_tx.dc_code,g_ln_parm.over90_dc_code);
    }
    **end*
    if( g_ln_mst.ac_sts[0]=='3' )* ���� *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.sla_dc_code); 
    }
    if( g_ln_mst.ac_sts[0]=='4' )* ���� *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.bad_dc_code); 
    }lpj20151129end*/
    sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG

    sprintf(acErrMsg,"ǰ(�������)PASSΪ [%lf]",g_ln_mst.intst_acm );
    WRITEMSG
    /*** ���ü�����Ϣ�����ĺ���(״̬Ϊ�����İ��Ҵ������) ***/
    if(!((g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
                                            &&g_ln_mst.ac_sts[0]=='1') )
    {
        TRACE
        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2",g_ln_mst.bal,g_pub_tx.tx_amt1 , 
            &g_ln_mst.intst_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.intst_acm = g_ln_mst.intst_acm;
        
        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2",g_ln_mst.in_lo_intst,g_pub_tx.tx_amt2, 
            &g_ln_mst.in_lo_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.in_lo_acm = g_ln_mst.in_lo_acm;
                
        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2" , g_ln_mst.out_lo_intst , g_pub_tx.tx_amt3,
            &g_ln_mst.out_lo_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.out_lo_acm = g_ln_mst.out_lo_acm;

        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2" , g_ln_mst.cmpd_lo_intst , g_pub_tx.tx_amt4,
            &g_ln_mst.cmpd_lo_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.cmpd_lo_acm = g_ln_mst.cmpd_lo_acm;

    }
    g_ln_mst.lst_date=g_pub_tx.tx_date;
    sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                        g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG

    sprintf(acErrMsg,"ǰ(�������)PASSΪ [%lf]",g_ln_mst.intst_acm );
    WRITEMSG

    /**����ǻ�����Ϣ�����������ע���ֶλ����ɸ������Լ���**/
    if( g_pub_tx.svc_ind==3011 ||g_pub_tx.svc_ind==3012 || 
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {   
        g_ln_mst.intst_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;  
    }   
    if( g_pub_tx.svc_ind==3021||g_pub_tx.svc_ind==3051||
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {
        g_ln_mst.in_lo_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;
    }   
    if( g_pub_tx.svc_ind==3031||g_pub_tx.svc_ind==3051||
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {
        g_ln_mst.out_lo_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;  
    }
    if( g_pub_tx.svc_ind==3041||g_pub_tx.svc_ind==3051||
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {
        g_ln_mst.cmpd_lo_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;  
    }
    do_flag=0;
     		
    TRACE
    if( g_pub_tx.add_ind[0]=='1' ) /*����*/
    {
        if( g_pub_tx.svc_ind==3010 ) /*��������ӣ����ſ�*/
        {
            TRACE
            sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            if( g_ln_mst.ac_sts[0]!='1' && g_ln_parm.ac_sts[0]=='1' )
            {
                TRACE
                sprintf(acErrMsg, "״̬����[%s]", g_ln_mst.ac_sts);
                WRITEMSG
                strcpy( g_pub_tx.reply,"L067" );
                goto ErrExit;
            }
            TRACE
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,0.00 ) )
            {
                if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                    g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
                {
                    sprintf(acErrMsg,
                    "���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"P122" );
                    goto ErrExit;
                }
            }
            g_ln_mst.bal+=g_pub_tx.tx_amt1;
            g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        
        if(g_pub_tx.svc_ind==3065)/*����չ��*/
        {
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
                
        if( g_pub_tx.svc_ind==3091 ) /*���Ǵ����*/
        {
            TRACE
            sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
            {
                sprintf(acErrMsg,
                    "���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }
            g_ln_mst.bal+=g_pub_tx.tx_amt1;
            g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                g_ln_mst.bal)<0 )
            {
                sprintf(acErrMsg,
                    "���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*****ligl*******   
        if(g_pub_tx.svc_ind==3019)
        {
            TRACE
            sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
            {
                sprintf(acErrMsg,
                    "���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }
            g_ln_mst.bal+=-g_pub_tx.tx_amt1;
            g_ln_mst.ttl_prvd_amt+=-g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }   
      **************/
        if( g_pub_tx.svc_ind==3092 ) /*���Ǵ����ѺƷ���*/
        {
            g_ln_mst.gage_amt+=g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }               
        if(g_pub_tx.svc_ind==3011||g_pub_tx.svc_ind==3012)/*���������������Ϣ*/
        {
            ret=pub_ins_ln_hst( 4 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*���������Ϣ���ӣ�����Ϣ*/
        if(g_pub_tx.svc_ind==3020||g_pub_tx.svc_ind==3050||
                                                g_pub_tx.svc_ind==3052) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
            {
                strcpy(g_pub_tx.brf,"������ڹ�Ϣ");
                g_ln_mst.in_lo_intst+=g_pub_tx.tx_amt2;
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*������ڻ�����������Ϣ*/
        if( g_pub_tx.svc_ind==3021 || g_pub_tx.svc_ind==3051 ) 
        {
            strcpy(g_pub_tx.brf,"������ڻ�����Ϣ");
            ret=pub_ins_ln_hst( 5 , 0);
            ERR_DEAL
            do_flag=1;
        }
        /*���������Ϣ���ӣ�����Ϣ*/
        if( g_pub_tx.svc_ind==3030 || g_pub_tx.svc_ind==3050 ||
                                            g_pub_tx.svc_ind==3052) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt3 , 0.00 ) )
            {
                strcpy(g_pub_tx.brf,"��������Ϣ");
                g_ln_mst.out_lo_intst+=g_pub_tx.tx_amt3;
                ret=pub_ins_ln_hst( 2 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*������������������Ϣ*/
        if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 ) 
        {
            strcpy(g_pub_tx.brf,"������������Ϣ");
            ret=pub_ins_ln_hst( 6 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*��������ӣ�����Ϣ*/
        if( g_pub_tx.svc_ind==3040 || g_pub_tx.svc_ind==3050||
                                                g_pub_tx.svc_ind==3052 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
            {
                strcpy(g_pub_tx.brf,"�������Ϣ");
                g_ln_mst.cmpd_lo_intst+=g_pub_tx.tx_amt4;
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*�����������������Ϣ*/
        if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 ) 
        {
            strcpy(g_pub_tx.brf,"���㸴��������Ϣ");
            ret=pub_ins_ln_hst( 7 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /* �����弶�϶� */
        if( g_pub_tx.svc_ind==3060 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }
        /* �����ת--ת�� */
        if( g_pub_tx.svc_ind==3063 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }           
        /* ���ɰ��һ���ƻ� */
        if( g_pub_tx.svc_ind==3062 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }       
        if( g_pub_tx.svc_ind==3059 ) 
        {
            ret=pub_ins_ln_hst( 1 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /* ��ѺƷ��� */
        if( g_pub_tx.svc_ind==3061 )
        {
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            if( g_pub_tx.svc_ind==3010 ) 
            {
                g_ln_mst.hst_bal=g_ln_mst.hst_bal+g_pub_tx.tx_amt1;
            }
        }
        if( g_pub_tx.svc_ind==3052 )
        {
            g_ln_mst.ic_date = g_pub_tx.tx_date ;   /* �޸���Ϣ���� */
        }   
    }
    else if( g_pub_tx.add_ind[0]=='0' ) /*����*/
    {
        if( g_pub_tx.svc_ind==3019 ) /*�������٣�������*/
        {
            if( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)>=0 )
            {
                g_ln_mst.bal-=g_pub_tx.tx_amt1;
            }else{
                g_pub_tx.tx_amt1=g_ln_mst.bal;
                g_ln_mst.bal=0.00;
            }           

            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /****ligl****/
        if(g_pub_tx.svc_ind==3091)
        {
            if( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)>=0 )
            {
                g_ln_mst.bal-=g_pub_tx.tx_amt1;
            }else{
                /*
                g_pub_tx.tx_amt1-=g_ln_mst.bal;
                g_ln_mst.bal=0.00;*/
                sprintf(acErrMsg,
                    "���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }           

            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*********/
        if( g_pub_tx.svc_ind==3092 ) /*���������ѺƷ���*/
        {
            g_ln_mst.gage_amt-=g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        /*���������Ϣ���٣�����ǷϢ*/
        if( g_pub_tx.svc_ind==3059 ) 
        {
            ret=pub_ins_ln_hst( 1 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        if( g_pub_tx.svc_ind==3071 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
            {               
                g_ln_mst.in_lo_intst-=g_pub_tx.tx_amt2;
                if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"��Ϣ������[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.in_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                strcpy ( g_pub_tx.brf , "�����ұ�����Ϣ����(��)" );
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            do_flag=1;
        }       
        if( g_pub_tx.svc_ind==3072 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
            {               
                g_ln_mst.out_lo_intst-=g_pub_tx.tx_amt4;
                if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"��Ϣ������[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.out_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                strcpy ( g_pub_tx.brf , "�����ұ�����Ϣ����(��)" );
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            do_flag=1;
        }
        if( g_pub_tx.svc_ind==3068 ) /*���Ҵ����Ϣ*/
        {
            if( pub_base_CompDblVal(g_pub_tx.tx_amt2 , 0.00 ) )
            {
                strcpy ( g_pub_tx.brf,"���һ�����Ϣ����" );
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            
        }
        if( g_pub_tx.svc_ind==3069 ) /*���Ҵ����Ϣ*/
        {
            if( pub_base_CompDblVal(g_pub_tx.tx_amt4 , 0.00 ) )
            {
                strcpy ( g_pub_tx.brf,"���һ��Ϣ����" );
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            
        }
        if(g_pub_tx.svc_ind==3011||g_pub_tx.svc_ind==3012)/*���������������Ϣ*/
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy( g_pub_tx.brf,"���������Ϣ" );          
            ret=pub_ins_ln_hst( 11 , 0 );
            ERR_DEAL
            
            strcpy( g_pub_tx.add_ind , "0" );           
            strcpy( g_pub_tx.brf,"���������" );            
            ret=pub_ins_ln_hst( 15 , 0 );
            ERR_DEAL
            
            do_flag=1;
        }               
        if( g_pub_tx.svc_ind==3010 ) /*�������٣�������*/
        {
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            WRITEMSG            
            g_ln_mst.bal=g_ln_mst.bal-g_pub_tx.tx_amt1;
            if( pub_base_CompDblVal(g_ln_mst.bal,0.00)<0 )
            {
                TRACE
                sprintf(acErrMsg,"����������[%.2lf]>[%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
                WRITEMSG
                strcpy( g_pub_tx.reply,"L039" );
                goto ErrExit;
            }
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            WRITEMSG
            ret=pub_ins_ln_hst( 0 , 0 );
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            ERR_DEAL
            do_flag=1;
        }
        if( g_pub_tx.svc_ind==3018 ) /*����ƻ����*/
        {
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            g_ln_mst.amt_lmt = g_ln_mst.amt_lmt - g_pub_tx.tx_amt1;
            /****
            g_ln_mst.bal = g_ln_mst.bal - g_pub_tx.tx_amt1;
            ****/
            WRITEMSG            
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        /*���������Ϣ���٣�����ǷϢ*/
        if( g_pub_tx.svc_ind==3020 || g_pub_tx.svc_ind==3050 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
            {
                g_ln_mst.in_lo_intst-=g_pub_tx.tx_amt2;
            
                if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"��Ϣ������[%d][%.2lf]>[%.2lf]",
                        g_ln_mst.ac_id,g_pub_tx.tx_amt2,g_ln_mst.in_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                if( strlen(g_pub_tx.brf)==0 )
                {
                    strcpy ( g_pub_tx.brf , "������ǷϢ" );
                }
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*���������Ϣ���٣�����ǷϢ*/
        if( g_pub_tx.svc_ind==3030 || g_pub_tx.svc_ind==3050 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt3 , 0.00 ) )
            {
                g_ln_mst.out_lo_intst-=g_pub_tx.tx_amt3;
                if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"��Ϣ������[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.out_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                if( strlen(g_pub_tx.brf)==0 )
                {
                    strcpy ( g_pub_tx.brf , "������ǷϢ" );         
                }
                ret=pub_ins_ln_hst( 2 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*��������٣�����ǷϢ*/
        if( g_pub_tx.svc_ind==3040 || g_pub_tx.svc_ind==3050 ) 
        {
            TRACE
            sprintf(acErrMsg,"���[%.2lf]",
                    g_pub_tx.tx_amt4,g_ln_mst.cmpd_lo_intst );
            WRITEMSG

            if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
            {
                TRACE
                sprintf(acErrMsg,"!!!!���[%.2lf]",
                            g_pub_tx.tx_amt4,g_ln_mst.cmpd_lo_intst );
                WRITEMSG

                g_ln_mst.cmpd_lo_intst-=g_pub_tx.tx_amt4;

                if( pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"��Ϣ������[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.cmpd_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                if( strlen(g_pub_tx.brf)==0 )
                {
                    strcpy ( g_pub_tx.brf , "������ǷϢ" );         
                }
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*������ڻ�����������Ϣ*/
        if( g_pub_tx.svc_ind==3051 ) 
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy ( g_pub_tx.brf,"���ڻ�����Ϣ" );
            ret=pub_ins_ln_hst( 12 , 0 );
            ERR_DEAL            
            
            strcpy( g_pub_tx.add_ind , "0" );
            strcpy ( g_pub_tx.brf,"�����ڻ�����Ϣ" );
            ret=pub_ins_ln_hst( 5 , 0 );
            ERR_DEAL
            do_flag=1;

        }
        /*������������������Ϣ*/
        if( g_pub_tx.svc_ind==3051 ) 
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy ( g_pub_tx.brf,"���������Ϣ" );
            ret=pub_ins_ln_hst( 13 , 0 );
            ERR_DEAL
            
            strcpy( g_pub_tx.add_ind , "0" );
            strcpy ( g_pub_tx.brf,"�����������Ϣ" );
            ret=pub_ins_ln_hst( 6 , 0 );
            ERR_DEAL
            do_flag=1;

        }
        /*�����������������Ϣ*/
        if( g_pub_tx.svc_ind==3051 ) 
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy ( g_pub_tx.brf,"����������Ϣ" );
            ret=pub_ins_ln_hst( 14 , 0 );
            ERR_DEAL

            strcpy( g_pub_tx.add_ind , "0" );
            strcpy ( g_pub_tx.brf,"������������Ϣ" );
            ret=pub_ins_ln_hst( 7 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        /* �����弶�϶� */
        if( g_pub_tx.svc_ind==3060 )
        {
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /* �����ת--ת�� */
        if( g_pub_tx.svc_ind==3063 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }       
        /* ��ѺƷ���� */
        if( g_pub_tx.svc_ind==3061 )
        {
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            if( g_pub_tx.svc_ind==3019 || g_pub_tx.svc_ind==3010 ||
                                                g_pub_tx.svc_ind==3018)
            {
                g_ln_mst.hst_bal=g_ln_mst.hst_bal-g_pub_tx.tx_amt1;
            }
        }
        /***3013��������  ί�д��Ϣ���ף����Ӵ�����ϸ������******/
        if( g_pub_tx.svc_ind==3013 ) 
        {       		
        		get_zd_double("102F",&g_pub_tx.tx_amt1);  /*���׽�� */
						sprintf(acErrMsg,"ί�д��Ϣȡ����![%.2lf]",g_pub_tx.tx_amt1);
						WRITEMSG
        		ret=pub_ins_ln_hst( 1 , 1 );
            ERR_DEAL
            do_flag=1;
        }
    }
    else if( g_pub_tx.svc_ind==2 )
    {
        do_flag=1;
    }
    else
    {
        sprintf(acErrMsg,"������־����[%s]",g_pub_tx.add_ind);
        WRITEMSG
        strcpy( g_pub_tx.reply,"S048" );
        goto ErrExit;
    }

    if( do_flag==0 )
    {
        sprintf(acErrMsg,"��־����[%s][%d]",g_pub_tx.add_ind,g_pub_tx.svc_ind);
        WRITEMSG
        strcpy( g_pub_tx.reply,"S048" );
        goto ErrExit;
    }

    /**����������0�������**/
    if( g_pub_tx.svc_ind==2 )
    {
        /*  modified by liuxuan 2006-11-28 ���������������������Ѿ�������
        if( pub_base_CompDblVal(g_ln_mst.bal,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.intst_acm,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.in_lo_acm,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.out_lo_acm,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.cmpd_lo_acm,0.00) )
        {
            sprintf(acErrMsg,"����δ����");
            WRITEMSG
            strcpy( g_pub_tx.reply,"L068" );
            goto ErrExit;
        }
        */
            if(g_pub_tx.add_ind[0]=='2')    /* �������������������־���������Ļ��� */
                g_pub_tx.add_ind[0]='0';
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
    }
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    ret=Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    Ln_mst_Clo_Upd();
            sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
OkExit:
    return 0;
ErrExit:
    return 1;
}

int Chg_in_mst()
{
    struct com_item_c v_com_item;
    int ret;
    /*** �����ڲ���Ʒ���������麯�� ***/

    /*** �ڲ����ļ���� ***/
    ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"A029");
        goto ErrExit;
    }

    strcpy( g_pub_tx.opn_br_no,g_in_mst.opn_br_no );
vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    set_zd_data("0020", g_pub_tx.opn_br_no);
    strcpy(g_pub_tx.name,g_in_mst.name);
    /**��ȡ��Ʒ����**/
    ret = In_parm_Sel( g_pub_tx.reply,&g_in_parm,
            " prdt_no='%s' ",g_in_mst.prdt_no );
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_in_parm.dc_code);             /*��ƴ���*/
    strcpy(g_pub_tx.prdt_code,g_in_mst.prdt_no);            /*��ƴ���*/

    /**��ȡ��Ŀ����**/
    ret = Com_item_Sel( g_pub_tx.reply,&v_com_item,
            " acc_hrt='%s' ",g_in_parm.acc_hrt );
    ERR_DEAL

sprintf(acErrMsg,"yefx[%s][%s]",v_com_item.dc_ind,g_in_parm.acc_hrt);
WRITEMSG

    if( g_in_mst.sts[0]!='1' && g_in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]",g_in_mst.ac_id,g_in_mst.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }

    g_in_mst.lst_date=g_pub_tx.tx_date;

sprintf(acErrMsg, "�ڲ�: add_ind =[%s]", g_pub_tx.add_ind);
WRITEMSG

    if( g_pub_tx.add_ind[0]=='0' )  /*��*/
    {
        if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='2' )
        {
            sprintf(acErrMsg,"���˻��Ѿ����򶳽�!sts[%s],add_ind[%s],dc[%s]",
                g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P125");
            goto ErrExit;
        }
        if( v_com_item.amt_dc_ind[0]=='2' )
        {
            sprintf(acErrMsg,"��Ŀ����������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A025");
            goto ErrExit;
        }
        if( g_in_parm.amt_dc[0]=='2' )
        {
            sprintf(acErrMsg,"��Ʒ����������");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A027");
            goto ErrExit;
        }

        if( v_com_item.dc_ind[0]=='2' )
        {
            /*** ���ü�����Ϣ�������� ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                goto ErrExit;
            }

            g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
                g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

            strcpy(g_pub_tx.add_ind,"0");
        }
        else
        {
            /*** ���ü�����Ϣ�������� ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                goto ErrExit;
            }

            g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
                g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
            strcpy(g_pub_tx.add_ind,"1");
        }
    }
    else if ( g_pub_tx.add_ind[0]=='1' )    /*��*/
    {
        if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='1' )
        {
            sprintf(acErrMsg,"���˻��Ѿ����򶳽�!sts[%s],add_ind[%s],dc[%s]",
                g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P125");
            goto ErrExit;
        }
        if( v_com_item.amt_dc_ind[0]=='1' )
        {
            sprintf(acErrMsg,"��Ŀ�������跽");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A024");
            goto ErrExit;
        }
        if( g_in_parm.amt_dc[0]=='1' )
        {
            sprintf(acErrMsg,"��Ʒ�������跽");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A026");
            goto ErrExit;
        }
        if( v_com_item.dc_ind[0]=='2' )
        {
            /*** ���ü�����Ϣ�������� ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                goto ErrExit;
            }
            g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
                g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
            strcpy(g_pub_tx.add_ind,"1");
        }
        else
        {
            /*** ���ü�����Ϣ�������� ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                goto ErrExit;
            }
            g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
                g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;
            strcpy(g_pub_tx.add_ind,"0");
        }
    }
    else
    {
        sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P106");
        goto ErrExit;
    }

sprintf(acErrMsg, "�ڲ�: add_ind =[%s]", g_pub_tx.add_ind);
WRITEMSG
        if( g_pub_tx.svc_ind==9002 ) /**����**/
        {
            strcpy(g_in_mst.sts,"*");
            if( pub_base_CompDblVal(g_in_mst.bal,0.00)  )
            {
                sprintf(acErrMsg,"��������Ϊ��!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P116");
                goto ErrExit;
            }
            strcpy(g_pub_tx.add_ind,"0");
        }

        if( pub_base_CompDblVal(g_in_mst.bal,0.00)<0 
            && g_in_mst.od_ind[0]=='N')
        {
            sprintf(acErrMsg,"���˻�[%ld]������͸֧![%.2f][%.2f]",
                g_in_mst.ac_id,g_in_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P124");
            goto ErrExit;
        }
        if( g_in_mst.od_ind[0]=='Y' 
            && pub_base_CompDblVal(g_in_parm.od_amt,0.00)
            && pub_base_CompDblVal(-g_in_mst.bal,g_in_parm.od_amt)>0 )
        {
            sprintf(acErrMsg,"͸֧����![%.2f][%.2f]",
                g_in_mst.bal,g_in_parm.od_amt );
            WRITEMSG
            strcpy(g_pub_tx.reply,"T004");
            goto ErrExit;
        }
        vtcp_log("%s,%d ׼��д����ʷ��ϸ��� ===%f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
        if( g_pub_tx.hst_ind[0]=='1' )      /*�ռ�����ϸ*/
        {
            ret=pub_ins_in_hst();
            ERR_DEAL
        }

    ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    In_mst_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    return 1;
}
/**�Կ�Ŀ����**/
int Chg_acchrt_mst(int *ass_ind)
{
    int ret=0; 
    double amt;
    char wherelist[200];
    int i=0;
    char TmpAdd_ind[2];
    char brno[6];

    struct gl_mst_c v_gl_mst;
    struct com_item_c v_com_item;
    struct tx_dc_br_rel_c v_tx_dc_br_rel;

    memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
    memset(&v_com_item,0x00,sizeof(struct com_item_c));
    memset(brno,0x00,sizeof(brno));

    sprintf(acErrMsg,"��ȡ��־=[%s]!",g_pub_tx.add_ind);
    WRITEMSG

    if(memcmp(g_pub_tx.tx_code,"3206",4)!=0
        &&memcmp(g_pub_tx.tx_code,"3207",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5402",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5404",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5202",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5840",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5842",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5844",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5846",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5848",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5849",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5855",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5856",4)!=0
        &&memcmp(g_pub_tx.tx_code,"D304",4)!=0
        &&memcmp(g_pub_tx.tx_code,"D302",4)!=0
        &&memcmp(g_pub_tx.tx_code,"D131",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5901",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5709",4)!=0
        &&memcmp(g_pub_tx.tx_code,"F000",4)!=0/*add fupengfei  2014/6/30 ����֧�����ʳ��ˣ��������ˡ��������ˡ���������ʱ����Ҫ���Ŀ�������Ϊ���׻���*/
        &&memcmp(g_pub_tx.tx_code,"F059",4)!=0
        &&memcmp(g_pub_tx.tx_code,"7023",4)!=0
        &&memcmp(g_pub_tx.tx_code,"6254",4)!=0
        &&memcmp(g_pub_tx.tx_code,"6247",4)!=0
        &&memcmp(g_pub_tx.tx_code,"7022",4)!=0
        &&memcmp(g_pub_tx.tx_code,"7024",4)!=0/*add fupengfei  2014/6/30*/
        &&memcmp(g_pub_tx.tx_code,"7028",4)!=0
        &&memcmp(g_pub_tx.tx_code,"6238",4)!=0
        &&memcmp(g_pub_tx.tx_code,"2225",4)!=0   /*add by lwb 20150420*/
        )  /****�����Բ���ۻ����ʴ��� 20070624****/
        strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );
        vtcp_log("%s,%d tx_code=[%s] opn_br_no===[%s]",__FILE__,__LINE__, g_pub_tx.tx_code, g_pub_tx.opn_br_no);
    
    /*add beg by ChengGX ���� 2010-1-26 11:09 */
   /** if(memcmp(g_pub_tx.tx_code,"6203",4)==0 && memcmp(g_pub_tx.ac_no,"41404",5)==0 && memcmp(g_pub_tx.sub_tx_code,"A016",4)==0) **/
   vtcp_log("%s,%d g_pub_tx.ac_no===[%s]g_pub_tx.sub_tx_code[%s]",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.sub_tx_code);
   vtcp_log("%s,%d g_pub_tx.tx_code[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
   if(memcmp(g_pub_tx.ac_no,"41404",5)==0 && (memcmp(g_pub_tx.sub_tx_code,"A016",4)==0 || memcmp(g_pub_tx.sub_tx_code,"A017",4)==0 || memcmp(g_pub_tx.tx_code,"Z129",4)==0 || memcmp(g_pub_tx.tx_code,"CFEE",4)==0 )) 
    {
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    memcpy(g_pub_tx.opn_br_no,"10001",5);/*���е�41404��Ŀ���˶��ǵ���������*/
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    
    }
    /*add beg by ChengGX ���� 2010-1-26 11:09 */
    /*Add by hxc���� 20100203  */
    /**Mod by hxc 101218 �޸ļ��˿�Ŀ5111203->5111401**/
    if((memcmp(g_pub_tx.ac_no,"52704",5)==0 || memcmp(g_pub_tx.ac_no,"5111401",7)==0 || memcmp(g_pub_tx.ac_no,"5111601",7)==0 ) && (memcmp(g_pub_tx.sub_tx_code,"F071",4)==0 || memcmp(g_pub_tx.sub_tx_code,"F000",4)==0) ) 
    {
            memset(brno,0x00,sizeof(brno));
            get_zd_data("0770",brno);
            vtcp_log("%s,%d brno===[%s]",__FILE__,__LINE__,brno);
            memcpy(g_pub_tx.opn_br_no,brno,5);/*���е���������ATM����֧����Ŀ���˶��ǵ���֧��*/
    }
    /*End by hxc���� 20100203*/
    /**��������С��ҵ��41403��Ŀ���˼ǵ��������� 20130922 hzh start**/
   if(memcmp(g_pub_tx.ac_no,"41403",5)==0 && memcmp(g_pub_tx.tx_code,"6388",4)==0) 
    {
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
        memcpy(g_pub_tx.opn_br_no,"10001",5);/*���е�41403��Ŀ���˶��ǵ���������*/
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    
    }
    /**��������С��ҵ��41403��Ŀ���˼ǵ��������� 20130922 hzh   end**/
   /**����֧��С��ҵ��41403��Ŀ���˼ǵ��������� 2014/7/11 fupengfei start**/
   vtcp_log("%s,%d !!!ac_no===[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
   vtcp_log("%s,%d !!!tx_code===[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
   if(memcmp(g_pub_tx.ac_no,"41403",5)==0 && (memcmp(g_pub_tx.tx_code,"6236",4)==0 || 
   	memcmp(g_pub_tx.tx_code,"7023",4)==0 || memcmp(g_pub_tx.tx_code,"7021",4)==0 || 
   	memcmp(g_pub_tx.tx_code,"7022",4)==0 || memcmp(g_pub_tx.tx_code,"7024",4)==0))
    {
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
        memcpy(g_pub_tx.opn_br_no,"10001",5);/*���е�41403��Ŀ���˶��ǵ���������*/
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    
    }
    /**����֧��С��ҵ��41403��Ŀ���˼ǵ��������� 2014/7/11 fupengfei   end**/
	
	/*** ͬ������ҵ��41401��Ŀ���˵��������� ***/    
	if (memcmp(g_pub_tx.ac_no, "41401", 5) == 0 && 
		(memcmp(g_pub_tx.tx_code, "6238", 4) == 0 || 
		memcmp(g_pub_tx.tx_code, "7028", 4) == 0 ||
		memcmp(g_pub_tx.tx_code, "6253", 4) == 0 ||
		memcmp(g_pub_tx.tx_code, "6254", 4) == 0))
	{
		vtcp_log("[%s][%d] opn_br_no===[%s]", __FILE__, __LINE__, g_pub_tx.opn_br_no);
		memcpy(g_pub_tx.opn_br_no, "10001", 5);
		vtcp_log("[%s][%d] opn_br_no===[%s]", __FILE__, __LINE__, g_pub_tx.opn_br_no);
	}

    set_zd_data("0020", g_pub_tx.opn_br_no);
    vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    strcpy( g_pub_tx.prdt_code,"" );

    pub_base_strpack(g_pub_tx.cur_no);
    if( !strlen(g_pub_tx.cur_no) ) /**Ĭ��Ϊ�����**/
        strcpy( g_pub_tx.cur_no,"01" );

    /*����Ŀ����*/
    pub_base_strpack(g_pub_tx.ac_no);
    ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_no='%s'",g_pub_tx.ac_no);
    if( ret==100 )
    {
        sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����[%s]",g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    else if( ret )
    {
        sprintf(acErrMsg,"��ѯ������Ŀ���쳣[%s]",g_pub_tx.ac_no);
        WRITEMSG
        goto ErrExit;
    }
    strcpy(g_pub_tx.name,v_com_item.acc_name);

    /*��Ŀ���ͱ���Ϊ���ʻ�����*/
    if(v_com_item.acc_knd[0]!='3')
    {
        sprintf(acErrMsg,"�ÿ�Ŀ���Ͳ������ʻ���Ŀ!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }
    /**�������¼���Ŀ**/
    if(v_com_item.sub_acc_yn[0]=='Y')
    {
        sprintf(acErrMsg,"�ÿ�Ŀ�����¼���Ŀ!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }

    strcpy( g_pub_tx.dc_code,"ACHR" );
    set_zd_data("0152",v_com_item.acc_hrt );
    sprintf(acErrMsg, "CGH********acc_hrt[%s]", v_com_item.acc_hrt);
    WRITEMSG

    switch(g_pub_tx.add_ind[0])                 /*���׽������*/
    {   
        case '0': /**�跽����**/
            if(v_com_item.amt_dc_ind[0]=='2')
            {
                if(!memcmp(g_pub_tx.tx_code,"5839",4) && g_pub_tx.tx_amt1==0)
                {
                /** add by LiuHuafeng �������ҵ��������Ϊ0���Բ����� **/
                    vtcp_log("%s,%d txcode=[%s],tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.tx_amt1);
                }
                else
                {
                sprintf(acErrMsg,"�˿�Ŀֻ�ܼǴ�����![%s]",g_pub_tx.ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A025");
                goto ErrExit;
                }
            }
            if(v_com_item.dc_ind[0]=='1' || v_com_item.dc_ind[0]=='0')
                strcpy(TmpAdd_ind,"1");     /** ���� **/
            else
                strcpy(TmpAdd_ind,"0");     /** ���� **/
            break;
        case '1': /*��������*/
            if(v_com_item.amt_dc_ind[0]=='1')
            {
                if(!memcmp(g_pub_tx.tx_code,"5839",4) && g_pub_tx.tx_amt1==0)
                {
                    vtcp_log("%s,%d txcode=[%s],tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.tx_amt1);
                /** add by LiuHuafeng �������ҵ��������Ϊ0���Բ����� **/
                }
                else
                {
                sprintf(acErrMsg,"�˿�Ŀֻ�ܼǽ跽��![%s]",g_pub_tx.ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A024");
                goto ErrExit;
                }
            }
            if(v_com_item.dc_ind[0]=='2')
                strcpy(TmpAdd_ind,"1");     /** ���� **/
            else
                strcpy(TmpAdd_ind,"0");     /** ���� **/
            break;
        default:
            sprintf(acErrMsg,"��ȡ��־����=[%s]!",g_pub_tx.add_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P106");
            goto ErrExit;
            break;
    }

    if( v_com_item.in_ind[0]=='A' )     /*** �ÿ�Ŀ�ǿ��˿�Ŀ***/
        *ass_ind=1;
    else
    {
        *ass_ind=0;
        /*** �������־ת��Ϊ������־ ***/
        strcpy(g_pub_tx.add_ind,TmpAdd_ind);
    }

    sprintf(acErrMsg, "������־Ϊ: [%s]", g_pub_tx.add_ind);
    WRITEMSG

OkExit:
    return 0;
ErrExit:
    return 1;
}
/**�Կ�Ŀ����  �ɵ�ʵʱ������**/
int Chg_acchrt_mst_old()
{
    int ret=0; 
    double amt;
    char wherelist[200];
    int i=0;

    struct gl_mst_c v_gl_mst;
    struct com_item_c v_com_item;
    struct com_item_c u_com_item;
    struct tx_dc_br_rel_c v_tx_dc_br_rel;

    memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
    memset(&v_com_item,0x00,sizeof(struct com_item_c));

                sprintf(acErrMsg,"��ȡ��־����=[%s]!",g_pub_tx.add_ind);
                WRITEMSG

    pub_base_strpack(g_pub_tx.cur_no);
    if( !strlen(g_pub_tx.cur_no) ) /**Ĭ��Ϊ�����**/
        strcpy( g_pub_tx.cur_no,"01" );
    /*����Ŀ����*/
    pub_base_strpack(g_pub_tx.ac_no);
    ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_no='%s'",g_pub_tx.ac_no);
    if( ret==100 )
    {
        sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����[%s]",g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    else if( ret )
    {
        sprintf(acErrMsg,"��ѯ������Ŀ���쳣[%s]",g_pub_tx.ac_no);
        WRITEMSG
        goto ErrExit;
    }

    /*��Ŀ���ͱ���Ϊ���ʻ�����*/
    if(v_com_item.acc_knd[0]!='3')
    {
        sprintf(acErrMsg,"�ÿ�Ŀ���Ͳ������ʻ���Ŀ!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }
    /**�������¼���Ŀ**/
    if(v_com_item.sub_acc_yn[0]=='Y')
    {
        sprintf(acErrMsg,"�ÿ�Ŀ�����¼���Ŀ!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }

    /**ȡ�ö�Ӧ�Ļ�ƻ���**/
    ret=Tx_dc_br_rel_Sel(g_pub_tx.reply,&v_tx_dc_br_rel,"tx_br_no='%s'",
        g_pub_tx.tx_br_no );
    if( ret )
    {
        sprintf(acErrMsg,"���׻����޶�Ӧ��ƻ���[%s]",g_pub_tx.tx_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"S029");
        goto ErrExit;
    }

    memset( wherelist,0,sizeof(wherelist) );
    memcpy( &u_com_item,&v_com_item,sizeof(u_com_item) );
    pub_base_strpack( u_com_item.acc_hrt );
    pub_base_strpack( u_com_item.up_acc_hrt );

    strcpy( g_pub_tx.dc_code,"ACHR" );
    set_zd_data("0152",v_com_item.acc_hrt );
    sprintf(acErrMsg, "CGH********acc_hrt[%s]", v_com_item.acc_hrt);
    WRITEMSG

    sprintf( wherelist,"br_no='%s' and cur_no='%s' and acc_hrt in('%s'",
        v_tx_dc_br_rel.dc_br_no,g_pub_tx.cur_no, u_com_item.acc_hrt );
    /**ȡ���ϼ���Ŀ**/
    for ( i=0;i<2;i++ )
    {
        if( !strcmp(u_com_item.acc_hrt,u_com_item.up_acc_hrt) )
            break;

        ret=Com_item_Sel(g_pub_tx.reply,&u_com_item,"acc_hrt='%s'",
            u_com_item.up_acc_hrt);
        if( ret )
        {
            sprintf(acErrMsg,"�ÿ�Ŀ�����ֲ�����[%s]",u_com_item.up_acc_hrt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        pub_base_strpack( u_com_item.acc_hrt );
        pub_base_strpack( u_com_item.up_acc_hrt );
        strcat( wherelist,",'" );
        strcat( wherelist,u_com_item.acc_hrt );
        strcat( wherelist,"'" );
    }

    strcat( wherelist,")" );

    sprintf( acErrMsg,"GL_MST where [%s]",wherelist );
    WRITEMSG

    /*�޸�����*/
    ret=Gl_mst_Dec_Upd(g_pub_tx.reply,wherelist);
    if( ret )
    {
        sprintf(acErrMsg,"��ѯ�����쳣!![%s]",v_com_item.acc_hrt);
        WRITEMSG
        goto ErrExit;
    }

    i=0;
    while( 1 )
    {
        ret=Gl_mst_Fet_Upd(&v_gl_mst,g_pub_tx.reply);
        if( ret==100 ) break;
        if( ret )
        {
            sprintf(acErrMsg,"��ѯ�����쳣!![%s]",v_com_item.acc_hrt);
            WRITEMSG
            goto ErrExit;
        }
        i++;

        switch(g_pub_tx.add_ind[0])                 /*���׽������*/
        {   
            case '0': /**�跽����**/
                if(v_com_item.amt_dc_ind[0]=='2')
                {
                    sprintf(acErrMsg,"�˿�Ŀֻ�ܼǴ�����![%s]",g_pub_tx.ac_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A025");
                    goto ErrExit;
                }

                if(v_com_item.dc_ind[0]=='1')               /*�账��*/
                {
                    v_gl_mst.dr_bal+=g_pub_tx.tx_amt1;
                }
                else if(v_com_item.dc_ind[0]=='2')          /*������*/
                {
                    v_gl_mst.cr_bal-=g_pub_tx.tx_amt1;
                }   
                else if(v_com_item.dc_ind[0]=='0')          /*˫����*/
                {
                    amt=v_gl_mst.dr_bal-v_gl_mst.cr_bal+g_pub_tx.tx_amt1;
                    ret=pub_base_CompDblVal(amt,0.00);
                    if(ret==1)
                    {
                        v_gl_mst.dr_bal=amt;        /*��ǰ�跽���*/
                        v_gl_mst.cr_bal=0.00;       /*��ǰ�������*/
                    }
                    else if(ret==-1)
                    {
                        v_gl_mst.dr_bal=0.00;       /*��ǰ�跽���*/
                        v_gl_mst.cr_bal=-amt;       /*��ǰ�������*/
                    }
                    else
                    {
                        v_gl_mst.dr_bal=0.00;       /*��ǰ�跽���*/
                        v_gl_mst.cr_bal=0.00;       /*��ǰ�������*/
                    }
                }
                v_gl_mst.rdd_cnt+=1;                    /*���ս����*/
                v_gl_mst.rdd_amt+=g_pub_tx.tx_amt1;     /*���ս跢����*/
                v_gl_mst.tddr_cnt+=1;                   /*��Ѯ�����*/
                v_gl_mst.tddr_amt+=g_pub_tx.tx_amt1;    /*��Ѯ�跢����*/
                v_gl_mst.mdr_cnt+=1;                    /*���½����*/
                v_gl_mst.mdr_amt+=g_pub_tx.tx_amt1;     /*���½跢����*/
                v_gl_mst.qdr_cnt+=1;                    /*���������*/
                v_gl_mst.qdr_amt+=g_pub_tx.tx_amt1;     /*�����跢����*/
                v_gl_mst.ydr_cnt+=1;                    /*��������*/
                v_gl_mst.ydr_amt+=g_pub_tx.tx_amt1;     /*����跢����*/
                break;
            case '1': /*��������*/
                if(v_com_item.amt_dc_ind[0]=='1')
                {
                    sprintf(acErrMsg,"�˿�Ŀֻ�ܼǽ跽��![%s]",g_pub_tx.ac_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A024");
                    goto ErrExit;
                }

                if(v_com_item.dc_ind[0]=='1')               /*�账��*/
                {
                    v_gl_mst.dr_bal-=g_pub_tx.tx_amt1;
                }
                else if(v_com_item.dc_ind[0]=='2')          /*������*/
                {
                    v_gl_mst.cr_bal+=g_pub_tx.tx_amt1;
                }   
                else if(v_com_item.dc_ind[0]=='0')          /*˫����*/
                {
                    amt=v_gl_mst.dr_bal-v_gl_mst.cr_bal-g_pub_tx.tx_amt1;
                    ret=pub_base_CompDblVal(amt,0.00);
                    if(ret==1)
                    {
                        v_gl_mst.dr_bal=amt;        /*��ǰ�跽���*/
                        v_gl_mst.cr_bal=0.00;       /*��ǰ�������*/
                    }
                    else if(ret==-1)
                    {
                        v_gl_mst.dr_bal=0.00;       /*��ǰ�跽���*/
                        v_gl_mst.cr_bal=-amt;       /*��ǰ�������*/
                    }
                    else
                    {
                        v_gl_mst.dr_bal=0.00;       /*��ǰ�跽���*/
                        v_gl_mst.cr_bal=0.00;       /*��ǰ�������*/
                    }
                }
                v_gl_mst.rcd_cnt+=1;                    /*���մ�����*/
                v_gl_mst.rcd_amt+=g_pub_tx.tx_amt1;     /*���մ�������*/
                v_gl_mst.tdcr_cnt+=1;                   /*��Ѯ������*/
                v_gl_mst.tdcr_amt+=g_pub_tx.tx_amt1;    /*��Ѯ��������*/
                v_gl_mst.mcr_cnt+=1;                    /*���´�����*/
                v_gl_mst.mcr_amt+=g_pub_tx.tx_amt1;     /*���´�������*/
                v_gl_mst.qcr_cnt+=1;                    /*����������*/
                v_gl_mst.qcr_amt+=g_pub_tx.tx_amt1;     /*������������*/
                v_gl_mst.ycr_cnt+=1;                    /*���������*/
                v_gl_mst.ycr_amt+=g_pub_tx.tx_amt1;     /*�����������*/
                break;
            default:
                sprintf(acErrMsg,"��ȡ��־����=[%s]!",g_pub_tx.add_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P106");
                goto ErrExit;
                break;
        }

        ret=Gl_mst_Upd_Upd(v_gl_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"Update ���ʳ���!![%s]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    }
    Gl_mst_Clo_Upd();

    if( i==0 )
    {
        sprintf(acErrMsg,"�����в����ڸÿ�Ŀv_gl_mst.acc_hrt=[%s]", \
            v_gl_mst.acc_hrt);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O075");
        goto ErrExit;
    }


OkExit:
    return 0;
ErrExit:
    return 1;
}

int pub_ins_ln_trace_log( double amt )
{
    double v_amt;
    int ret=0;

    v_amt=g_pub_tx.tx_amt1;
    g_pub_tx.tx_amt1=amt;
    ret=pub_ins_trace_log();
    TRACE
    g_pub_tx.tx_amt1=v_amt;
    ERR_DEAL

OkExit:
    return 0;
ErrExit:
    return 1;
}
int pub_ins_trace_log()
{
    struct trace_log_c  trace_log_c;
    int ret;

    memset(&trace_log_c,0x00,sizeof(struct trace_log_c));
    
    trace_log_c.trace_no=g_pub_tx.trace_no;
    trace_log_c.tx_date=g_pub_tx.tx_date;
    trace_log_c.tx_time=g_pub_tx.tx_time;

    strcpy(trace_log_c.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(trace_log_c.opn_br_no,g_pub_tx.opn_br_no);
    if(memcmp(g_pub_tx.tx_code,"6203",4)==0){/**2010/6/10 11:53*ADD BY ZHGF**/
        char fd60[17]; /* ��Ա���� */
        char fd24[4]; /* ҵ������ */
        get_zd_data("0240",fd24);
        if(memcmp(fd24,"gmt",3)==0)
        {
            memset(fd60,0,sizeof(fd60));
            get_zd_data("0600",fd60);
            vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
            memcpy(trace_log_c.tx_br_no,fd60,5);
        }
    }
    strcpy(trace_log_c.tx_code,g_pub_tx.tx_code);
    strcpy(trace_log_c.sub_tx_code,g_pub_tx.sub_tx_code);
    trace_log_c.svc_ind=g_pub_tx.svc_ind;
    vtcp_log("CT IND[%s],svc_ind[%d]",g_pub_tx.ct_ind,g_pub_tx.svc_ind);
    trace_log_c.ct_ind[0]=g_pub_tx.ct_ind[0];

    strcpy(trace_log_c.cur_no,g_pub_tx.cur_no);
    strcpy(trace_log_c.prdt_no,g_pub_tx.prdt_code);
    /***
    strcpy(trace_log_c.ac_no,g_pub_tx.ac_no);
    ***/
    /**modify by xyy 2006-11-20 12:59 **/
    /*YW 20111216 ���ΪIC��ʱ���˻�ת��Ϊ���˻� ��Ŀǰ���ֻ��ͨ��prdt_code����*/
/**    ret=pub_get_ic_main(g_pub_tx.ac_no,trace_log_c.ac_no,trace_log_c.ac_id);
    if(ret!=0){
       strncpy(trace_log_c.ac_no,g_pub_tx.ac_no,19);
       trace_log_c.ac_id=g_pub_tx.ac_id;
       strcpy(g_pub_tx.reply,"0000");���������� ��Ӱ�����ǩԼ
    }
    vtcp_log("[%s][%d] ret =[%d][%s][%ld]\n",__FILE__,__LINE__,ret,trace_log_c.ac_no,trace_log_c.ac_id);
    trace_log_c.ac_seqn=g_pub_tx.ac_seqn;**/

		strncpy(trace_log_c.ac_no,g_pub_tx.ac_no,19);
    trace_log_c.ac_seqn=g_pub_tx.ac_seqn;
    trace_log_c.ac_id=g_pub_tx.ac_id;

    strcpy(trace_log_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify
    strcpy(trace_log_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(trace_log_c.note_no,g_pub_tx.beg_note_no,16);

    if( g_pub_tx.add_ind[0]=='1' )
        strcpy(trace_log_c.add_ind,g_pub_tx.add_ind);
    else
        strcpy(trace_log_c.add_ind,"0");
    trace_log_c.amt=g_pub_tx.tx_amt1;

    strcpy(trace_log_c.brf,g_pub_tx.brf);
    strcpy(trace_log_c.hst_ind,g_pub_tx.hst_ind);
    strcpy(trace_log_c.tel,g_pub_tx.tel);
    strcpy(trace_log_c.chk,g_pub_tx.chk);
    strcpy(trace_log_c.aut,g_pub_tx.auth);
sprintf(acErrMsg," TEL [%s] CHK [%s]",g_pub_tx.tel,g_pub_tx.chk);
WRITEMSG
    strcpy(trace_log_c.saving_notarize,"0");
    strcpy(trace_log_c.sts,"0");
    strcpy(trace_log_c.no_show,g_pub_tx.no_show);
    if( trace_log_c.no_show[0]!='1' )
        trace_log_c.no_show[0]='0';
    TRACE

    if( g_pub_tx.ac_wrk_ind[1]=='2' )
    {
        get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
        g_pub_tx.trace_cnt++;
        set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
        sprintf(acErrMsg,"ǿ�Ʋ�����ˮ");
        WRITEMSG
    }
    else if( !pub_base_CompDblVal(trace_log_c.amt,0.00) 
        && g_pub_tx.ac_wrk_ind[1]=='0' )
    {
        sprintf(acErrMsg,"0������ˮ");
        WRITEMSG
    }
    else
    {
            get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
        g_pub_tx.trace_cnt++;
        trace_log_c.trace_cnt=g_pub_tx.trace_cnt;
            set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
        ret=Trace_log_Ins(trace_log_c,g_pub_tx.reply);
        if( ret )
        {
            Trace_log_Debug(&trace_log_c);
            vtcp_log("=====[%.2lf]",trace_log_c.amt);
            sprintf(acErrMsg,"�Ǽǽ�����ˮ����[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P126");
            goto ErrExit;
        }
    }
    TRACE
    return 0;
ErrExit:
    return 1;
}

int pub_ins_dd_hst()
{
    int ret;

    struct dd_mst_hst_c dd_mst_hst_c;
    memset(&dd_mst_hst_c,0x00,sizeof(struct dd_mst_hst_c));


    dd_mst_hst_c.trace_no=g_pub_tx.trace_no;
    dd_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(dd_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);  
    strcpy(dd_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);    
    dd_mst_hst_c.ac_id=g_pub_tx.ac_id;
    dd_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    strcpy(dd_mst_hst_c.tx_code,g_pub_tx.tx_code);  
    strcpy(dd_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);  

    if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' 
        || g_pub_tx.add_ind[0]=='3' )
        strcpy(dd_mst_hst_c.add_ind,"0");   
    else
        strcpy(dd_mst_hst_c.add_ind,"1");   
    strcpy(dd_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
    dd_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
    dd_mst_hst_c.bal=g_dd_mst.hst_bal;
    dd_mst_hst_c.acbo_bal=g_dd_mst.acbo_bal;
    dd_mst_hst_c.intst_acm=g_dd_mst.intst_acm;
    dd_mst_hst_c.tx_date=g_pub_tx.tx_date;
    dd_mst_hst_c.tx_time=g_pub_tx.tx_time;
    strcpy(dd_mst_hst_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify
    strcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);
    
    /*--------------by xxx--20030729----------------
    strncpy(dd_mst_hst_c.brf,g_pub_tx.brf,10);
    ----------------------------------------------*/
    pub_base_unprt_brf( dd_mst_hst_c.brf );
    vtcp_log("-----ժ-----Ҫ-----g_pub_tx.brf=[%s],dd_mst_hst_c.brf=[%s]",g_pub_tx.brf,dd_mst_hst_c.brf);
    /*mod beg by zzz20121010 ��ƽ̨Լ��ר�б�־��"JZFSSR�ɿ�"*/
    const char sFssrStr[]="JZFSSR�ɿ�";
    char sFssrTag[128]={'\0'};
    get_zd_data("0830",sFssrTag);
    zip_space(sFssrTag);
    vtcp_log("[%s][%d][%s] ",__FILE__,__LINE__,sFssrTag);
    if (0==strcmp(sFssrTag,sFssrStr))
    {
        strcpy(dd_mst_hst_c.brf,sFssrStr);
    }
    /*mod end by zzz20121010*/
    strcpy(dd_mst_hst_c.tel,g_pub_tx.tel);  
    strcpy(dd_mst_hst_c.chk,g_pub_tx.chk);  
    strcpy(dd_mst_hst_c.auth,g_pub_tx.auth);    
    get_zd_long( DC_TRACE_CNT , &dd_mst_hst_c.trace_cnt);
    dd_mst_hst_c.trace_cnt++;

    if( ( pub_base_CompDblVal(dd_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
        g_dd_mst.hst_cnt++;
        dd_mst_hst_c.hst_cnt=g_dd_mst.hst_cnt;
        if(memcmp(g_pub_tx.tx_code,"6203",4) ==0)
        {/**2010/6/10 11:53*ADD BY ZHGF**/
        
            char fd60[17]; /* ��Ա���� */
            char fd24[4]; /* ҵ������ */
            get_zd_data("0240",fd24);
            if(memcmp(fd24,"gmt",3)==0)
            {
                memset(fd60,0,sizeof(fd60));
                get_zd_data("0600",fd60);
                vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
                memcpy(dd_mst_hst_c.tx_br_no,fd60,5);
                memcpy(dd_mst_hst_c.tel,fd60+5,6);
            }
        }

        ret=Dd_mst_hst_Ins(dd_mst_hst_c,g_pub_tx.reply);
        Dd_mst_hst_Debug(&dd_mst_hst_c);
        if( ret )
        {
            sprintf(acErrMsg,"�Ǽǻ�����ϸ�ʴ���[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P127");
            goto ErrExit;
        }
    }

OkExit: 
    return 0;
ErrExit:
    return 1;
}

int pub_ins_td_hst()
{
    int ret;
    struct td_mst_hst_c td_mst_hst_c;
    memset(&td_mst_hst_c,0x00,sizeof(struct td_mst_hst_c));

    td_mst_hst_c.trace_no=g_pub_tx.trace_no;
    td_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(td_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);  
    strcpy(td_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);    
    td_mst_hst_c.ac_id=g_pub_tx.ac_id;
    td_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    strcpy(td_mst_hst_c.tx_code,g_pub_tx.tx_code);  
    strcpy(td_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);  
TRACE
    if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
        strcpy(td_mst_hst_c.add_ind,"0");   
    else
        strcpy(td_mst_hst_c.add_ind,"1");   
    strcpy(td_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
    td_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
    td_mst_hst_c.bal=g_td_mst.hst_bal;
    td_mst_hst_c.acbo_bal=g_td_mst.acbo_bal;
    td_mst_hst_c.intst_acm=g_td_mst.intst_acm;
    td_mst_hst_c.intst=g_pub_intst.factval;         /*** ȡ��Ϣ ***/
    td_mst_hst_c.tx_date=g_pub_tx.tx_date;
    td_mst_hst_c.tx_time=g_pub_tx.tx_time;
TRACE
    strcpy(td_mst_hst_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify
    strcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);
    
    /*--------------by xxx--20030729----------------
    strcpy(td_mst_hst_c.brf,g_pub_tx.brf);  
    ---------------------------------------------*/
    pub_base_unprt_brf( td_mst_hst_c.brf );
    
    strcpy(td_mst_hst_c.tel,g_pub_tx.tel);  
    strcpy(td_mst_hst_c.chk,g_pub_tx.chk);  
    strcpy(td_mst_hst_c.auth,g_pub_tx.auth);    
    get_zd_long( DC_TRACE_CNT , &td_mst_hst_c.trace_cnt);
    td_mst_hst_c.trace_cnt++;

    vtcp_log("td_mst_hst_c.tx_amt===[%.2lf], g_pub_tx.ac_wrk_ind[2]===[%c], g_pub_tx.hst_ind====[%s]", td_mst_hst_c.tx_amt, g_pub_tx.ac_wrk_ind[2], g_pub_tx.hst_ind);
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

    if( ( pub_base_CompDblVal(td_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
        if( partflag==1 )
        {
            g_td_mst.hst_cnt++;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
            td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
    vtcp_log("[%s][%d] *********td_mst_hst.hst_cnt =[%d]\n",__FILE__,__LINE__,td_mst_hst_c.hst_cnt);
        /*  td_mst_hst_c.tx_amt=o_td_mst.bal;��ǰ���ַ�ʽ������ϸ�����*/
            td_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
            td_mst_hst_c.bal=0.00;
            strcpy(td_mst_hst_c.add_ind,"0");   
            ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
            if( ret )
            {
                sprintf(acErrMsg,"�ǼǶ�����ϸ�ʴ���[%d]!",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P128");
                goto ErrExit;
            }
            td_mst_hst_c.tx_amt=o_td_mst.bal-g_pub_tx.tx_amt1;
            td_mst_hst_c.bal=o_td_mst.bal-g_pub_tx.tx_amt1;
            strcpy(td_mst_hst_c.add_ind,"1");   
            strcpy(td_mst_hst_c.note_no,g_pub_tx.new_note_no);  
            strcpy( td_mst_hst_c.brf,"���Ὺ" );
            partflag = 0; /**** ���Ʋ����¿���һ����ϸ������ add at 20120108****/
        }
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
        g_td_mst.hst_cnt++;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
    vtcp_log("[%s][%d] *********td_mst_hst.hst_cnt =[%d]\n",__FILE__,__LINE__,td_mst_hst_c.hst_cnt);
        ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�ǼǶ�����ϸ�ʴ���[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P128");
            goto ErrExit;
        }
    }
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

    sprintf(acErrMsg,"�ǼǶ����ʻ���ϸ�ɹ�!!!");
    WRITEMSG
    return 0;
ErrExit:
    sprintf(acErrMsg,"�ǼǶ����ʻ���ϸʧ��!!!");
    WRITEMSG
    return 1;
}
/* mode 1���� 2���� 3���� 4���� 9-���һ���Ϣ*/
/* log_ind  0-����ˮ    1-������ˮ */
int pub_ins_ln_hst( int mode , int log_ind )
{
    struct ln_mst_hst_c ln_mst_hst_c;
    int ret=0;
    sprintf(acErrMsg,"�������ϸ����, ������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    memset(&ln_mst_hst_c,0x00,sizeof(ln_mst_hst_c));

    ln_mst_hst_c.trace_no=g_pub_tx.trace_no;

    strcpy(ln_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(ln_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
    ln_mst_hst_c.ac_id=g_pub_tx.ac_id;
    ln_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;

    strcpy(ln_mst_hst_c.tx_code,g_pub_tx.tx_code);
    strcpy(ln_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);
    strcpy(ln_mst_hst_c.add_ind,g_pub_tx.add_ind);
    /**
    if(g_pub_tx.svc_ind==3091)
    ***/
    if(memcmp(g_pub_tx.tx_code,"3315",sizeof(g_pub_tx.tx_code)-1)==0)
    {
        if(g_pub_tx.add_ind[0]=='1')
            strcpy(ln_mst_hst_c.add_ind,"0");
        else
            strcpy(ln_mst_hst_c.add_ind,"1");
    }
    strcpy(ln_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
    sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
    WRITEMSG    
    ln_mst_hst_c.tx_date=g_pub_tx.tx_date;
    ln_mst_hst_c.tx_time=g_pub_tx.tx_time;
    strcpy(ln_mst_hst_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify 
    strcpy(ln_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(ln_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);

    strncpy(ln_mst_hst_c.brf,g_pub_tx.brf,20);
    strcpy(ln_mst_hst_c.tel,g_pub_tx.tel);
    strcpy(ln_mst_hst_c.chk,g_pub_tx.chk);
    strcpy(ln_mst_hst_c.auth,g_pub_tx.auth);

    if( mode==0 )
    {
        TRACE
        strcpy(ln_mst_hst_c.ln_tx_type,"0");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.bal=g_ln_mst.bal;
        if(g_pub_tx.svc_ind==3063)  /* ��������ת����ת�����ͳ�0 */
            ln_mst_hst_c.bal=0;
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /*****
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }
    else if( mode==1 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"1");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;
        ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
        
        ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.in_lo_acm;
        ***/
    }
    else if( mode==2 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"2");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
        ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
        
        ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
        /****
        ln_mst_hst_c.intst_acm=ln_mst_acm.out_lo_acm;
        ***/
    }
    else if( mode==3 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
    
        ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.cmpd_lo_acm;
        ***/
    }
    else if( mode==4 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"4");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.bal=g_ln_mst.bal;
        
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }
    else if( mode==5 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;

        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
        }
                    
        ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.in_lo_acm;
        ***/
    }
    else if( mode==6 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
        }
        
        ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
        /****
        ln_mst_hst_c.intst_acm=ln_mst_acm.out_lo_acm;
        ***/
    }
    else if( mode==7 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
        }   
        
        ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
        /***    
        ln_mst_hst_c.intst_acm=ln_mst_acm.cmpd_lo_acm;
        ***/
    }
    else if( mode==8 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"8");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else if( g_pub_tx.svc_ind==3092 ){
            ln_mst_hst_c.bal=g_ln_mst.gage_amt;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.bal;
        }
        
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }
    else if( mode==9 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"9");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4+g_pub_tx.tx_amt2;
        ln_mst_hst_c.bal=0.00;
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==10 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"A");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.bal=0.00;
        ln_mst_hst_c.intst_acm=0.00;
    }           
    else if( mode==11 )
    {
        if( g_pub_tx.svc_ind==3011 )
        {
            strcpy(ln_mst_hst_c.ln_tx_type,"1");
            ln_mst_hst_c.bal=g_ln_mst.in_lo_intst+g_pub_tx.tx_amt1;
        }else if( g_pub_tx.svc_ind==3012 )
        {
            strcpy(ln_mst_hst_c.ln_tx_type,"2");
            ln_mst_hst_c.bal=g_ln_mst.out_lo_intst+g_pub_tx.tx_amt1;
        }
        
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==12 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;

        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst+g_pub_tx.tx_amt2;           
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==13 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;

        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst+g_pub_tx.tx_amt3;           
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==14 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;

        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst+g_pub_tx.tx_amt4;           
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==15 )
    {
        if( g_pub_tx.svc_ind==3011 )
        {
            strcpy(ln_mst_hst_c.ln_tx_type,"1");
            ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
        }else if( g_pub_tx.svc_ind==3012 ){
            strcpy(ln_mst_hst_c.ln_tx_type,"2");
            ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
        }           
        
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }                           
    else
    {
        sprintf(acErrMsg,"�ǼǴ�����ϸ��mode����[%d]!",mode);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P129");
        goto ErrExit;
    }

    ln_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    get_zd_long( DC_TRACE_CNT , &ln_mst_hst_c.trace_cnt);
    ln_mst_hst_c.trace_cnt++;

    if( ( pub_base_CompDblVal(ln_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
        g_ln_mst.hst_cnt++;
        ln_mst_hst_c.hst_cnt=g_ln_mst.hst_cnt;

        sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
        WRITEMSG        

        ret=Ln_mst_hst_Ins(ln_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�ǼǴ�����ϸ�ʴ���[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P129");
            goto ErrExit;
        }
    }
    if( log_ind==0 )
    {
        TRACE
        ret=pub_ins_ln_trace_log(ln_mst_hst_c.tx_amt);
        ERR_DEAL
        TRACE
    }
    
GoodExit:
    return 0;
ErrExit:
    return 1;
}

int pub_ins_in_hst()
{
    int ret;
    struct in_mst_hst_c in_mst_hst_c;
    memset(&in_mst_hst_c,0x00,sizeof(struct in_mst_hst_c));

    in_mst_hst_c.trace_no=g_pub_tx.trace_no;
    in_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(in_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);  
    strcpy(in_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);    
    in_mst_hst_c.tx_date=g_pub_tx.tx_date;
    in_mst_hst_c.tx_time=g_pub_tx.tx_time;
    in_mst_hst_c.ac_id=g_pub_tx.ac_id;
    in_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
        strcpy(in_mst_hst_c.add_ind,"0");   
    else
        strcpy(in_mst_hst_c.add_ind,"1");   
    vtcp_log("%s,%d ׼��д����ʷ��ϸ��� ===%f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    in_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
    vtcp_log("%s,%d ��ʷ��ϸ��� ===%f",__FILE__,__LINE__,in_mst_hst_c.tx_amt);
    in_mst_hst_c.bal=g_in_mst.hst_bal;
    in_mst_hst_c.intst_acm=g_in_mst.intst_acm;
    strcpy(in_mst_hst_c.note_type,g_pub_tx.note_type);
    strcpy(in_mst_hst_c.ct_ind,g_pub_tx.ct_ind); /*add by lwb 20150508*/
    /**** 20110521 modify
    strcpy(in_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(in_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);

    strncpy(in_mst_hst_c.brf,g_pub_tx.brf,20);  
    strcpy(in_mst_hst_c.tel,g_pub_tx.tel);  
    strcpy(in_mst_hst_c.chk,g_pub_tx.chk);  
    strcpy(in_mst_hst_c.auth,g_pub_tx.auth);    
    get_zd_long( DC_TRACE_CNT , &in_mst_hst_c.trace_cnt);
    in_mst_hst_c.trace_cnt++;

    if( ( pub_base_CompDblVal(in_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
        g_in_mst.hst_cnt++;
        in_mst_hst_c.hst_cnt=g_in_mst.hst_cnt;
        ret=In_mst_hst_Ins(in_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�Ǽ��ڲ���ϸ�ʴ���[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P128");
            goto ErrExit;
        }
    }
    
    return 0;
ErrExit:
    return 1;
}

int pub_unprt_hst_ins(char ac_type[2])
{
    int ret;
    char Cbrf[21];
    struct mdm_unprt_hst_c  mdm_unprt_hst_c;

    memset(Cbrf,0x00,sizeof(Cbrf));
    memset(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));

        vtcp_log("dddddddddd---");
        ret=sql_count("mdm_ac_rel","ac_id=%ld and (ac_seqn=%d or ac_seqn=%d) and (note_sts='0' or note_sts='1') and mdm_code in (select mdm_code from mdm_attr where prt_ind='Y' ) ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn,ALL_SEQN);
    if( ret==0 )
    {
        sprintf(acErrMsg,"����Ҫ��ӡ[%d][%d][%d]!",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        goto OkExit;
    }
    else if( ret>0 )
    {
        mdm_unprt_hst_c.trace_no=g_pub_tx.trace_no;
        mdm_unprt_hst_c.trace_cnt=g_pub_tx.trace_cnt;
        strcpy(mdm_unprt_hst_c.tx_code,g_pub_tx.tx_code);
        strcpy(mdm_unprt_hst_c.tx_br_no,g_pub_tx.tx_br_no);
        mdm_unprt_hst_c.ac_id=g_pub_tx.ac_id;
        mdm_unprt_hst_c.ac_seqn=g_pub_tx.ac_seqn;
        strcpy(mdm_unprt_hst_c.add_ind,g_pub_tx.add_ind);
        mdm_unprt_hst_c.tx_amt=g_pub_tx.tx_amt1;
        if( ac_type[0]=='1' )
            mdm_unprt_hst_c.bal=g_dd_mst.hst_bal;
        else if( ac_type[0]=='2' )
            {
            mdm_unprt_hst_c.bal=g_td_mst.hst_bal;
            if((!strncmp(g_pub_tx.tx_code,"2203",4))&&(!strncmp(g_pub_tx.mdm_code,"0016",4)))
        {   
            mdm_unprt_hst_c.hst_cnt=g_td_mst.book_line+1;
            vtcp_log("1111111111[%d][%d]\n",mdm_unprt_hst_c.hst_cnt,g_td_mst.book_line);
            }
            else if((!strncmp(g_pub_tx.tx_code,"2204",4))&&(!strncmp(g_pub_tx.mdm_code,"0016",4)))
            {
            mdm_unprt_hst_c.hst_cnt=g_td_mst.book_line+2;
            vtcp_log("222222222[%d][%d]\n",mdm_unprt_hst_c.hst_cnt,g_td_mst.book_line);
            }
            else
            {
            mdm_unprt_hst_c.hst_cnt=g_td_mst.book_line;
    vtcp_log("333333333[%d][%d]\n",mdm_unprt_hst_c.hst_cnt,g_td_mst.book_line);
    }   
            }
        else if( ac_type[0]=='3' )
            mdm_unprt_hst_c.bal=g_ln_mst.hst_bal;
        else if( ac_type[0]=='9' )
            mdm_unprt_hst_c.bal=g_in_mst.hst_bal;
        else
        {
            sprintf(acErrMsg,"�Ǽ�δ��ӡ��ϸ���˺����ʹ���[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P131");
            goto ErrExit;
        }
        mdm_unprt_hst_c.tx_date=g_pub_tx.tx_date;
        strcpy(mdm_unprt_hst_c.tel,g_pub_tx.tel);
        
        /*pub_base_unprt_brf(mdm_unprt_hst_c.brf);  del by hxc 100302*/
        /*strncpy(mdm_unprt_hst_c.brf,g_pub_tx.brf,10);*/
        /**Add by hxc 100302**/
        memset(Cbrf,0x00,sizeof(Cbrf));
        pub_base_unprt_brf(Cbrf);
        sprintf(acErrMsg,"[%s],[%d],Cbrf=[%s]!",__FILE__,__LINE__,Cbrf);
            WRITEMSG
        if(strlen(Cbrf)>10)
            {
                strncpy(mdm_unprt_hst_c.brf,Cbrf,10);
            }
        else
            {
                strcpy(mdm_unprt_hst_c.brf,Cbrf);
            }
        sprintf(acErrMsg,"[%s],[%d],�Ǽ�δ��ӡ��ϸ���˺�ժҪΪ[%s]!",__FILE__,__LINE__,mdm_unprt_hst_c.brf);
            WRITEMSG
        /**End by hxc 100302**/


        if ( ( pub_base_CompDblVal(mdm_unprt_hst_c.tx_amt,0.00) || \
                g_pub_tx.ac_wrk_ind[2]=='1') )
        {
            if(memcmp(g_pub_tx.tx_code,"6203",4) ==0)
            {/**2010/6/10 11:53*ADD BY ZHGF**/
            
                char fd60[17]; /* ��Ա���� */
                char fd24[4]; /* ҵ������ */
                get_zd_data("0240",fd24);
                if(memcmp(fd24,"gmt",3)==0)
                {
                    memset(fd60,0,sizeof(fd60));
                    get_zd_data("0600",fd60);
                    vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
                    memcpy(mdm_unprt_hst_c.tx_br_no,fd60,5);
                    memcpy(mdm_unprt_hst_c.tel,fd60+5,6);
                }
            }
                
            ret=Mdm_unprt_hst_Ins(mdm_unprt_hst_c,g_pub_tx.reply);
            if( ret )
            {
                sprintf(acErrMsg,"�Ǽ�δ��ӡ��ϸ�ʴ���[%d]!,traceno=[%ld],[%ld],[%ld][%ld]",ret,
                mdm_unprt_hst_c.trace_no,mdm_unprt_hst_c.ac_id,mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.trace_cnt);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P131");
                goto ErrExit;
            }
        }
    }
    else
    {
        sprintf(acErrMsg,"�ж��Ƿ��ӡ����[%d]!",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P130");
        goto ErrExit;
    }

OkExit:
    return 0;
ErrExit:
    return 1;
}
static pub_sub_ac()
{
    struct mdm_ac_rel_c v_mdm_ac_rel;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct prdt_ac_id_c s_prdt_ac_id;
    struct mdm_attr_c s_mdm_attr;
/*    struct mo_ic_ac_rel_c sMo_ic_ac_rel;*/
    char v_ac_get_ind[2];
    int ret=0;
    
    

    v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];
    vtcp_log("++++++++[%d]ac_id_type[%s],ac_id[%ld],mdm_ac_id[%ld],get_ind[%s]+++++++\n",__LINE__,g_pub_tx.ac_id_type,g_pub_tx.ac_id,g_mdm_ac_rel.ac_id,g_pub_tx.ac_get_ind);

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
sprintf(acErrMsg,"ac_seqn = [%d],ac_no[%s] ",g_pub_tx.ac_seqn,g_pub_tx.ac_no);
WRITEMSG
        /*** ����˺š��˺�ID���˺���� ***/
        if( g_pub_tx.ac_no[0]!=NULL )
        {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
            ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
                "ac_no='%s' ",g_pub_tx.ac_no);
            vtcp_log("%s,%d  notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
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
                " ac_id=%ld order by note_sts",g_pub_tx.ac_id );
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
    set_zd_data("0020", g_pub_tx.opn_br_no);

    /*** Add commence By Azz ***/
    if( g_mdm_ac_rel.coll_sts[0]=='1' && g_pub_tx.ac_wrk_ind[8]!='1')
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

sprintf(acErrMsg,"ac_seqn= [%d] cur_no[%s],ac_id[%ld] ",g_pub_tx.ac_seqn,g_pub_tx.cur_no,g_pub_tx.ac_id);
WRITEMSG

    vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
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
        vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
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
        else
        {
            g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
        }
    }
    vtcp_log("============[%d]ac_seqn[%d],ac_id[%ld]=======\n",__LINE__,g_pub_tx.ac_seqn,g_pub_tx.ac_id);
        vtcp_log("%s,%d notests=[%c],ac_wrk_ind=[%s],ac_get_ind=[%s]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0],g_pub_tx.ac_wrk_ind,g_pub_tx.ac_get_ind);
    /**����״̬���**/
    if( g_pub_tx.ac_wrk_ind[7]=='J' )
    {
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
    }
    else if( g_pub_tx.ac_get_ind[0]=='1' )
    {
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
        /* ������ƼҪ��ȼ���˷� ֻҪ��ʧ״̬�Ϳ�ס wudawei 20140620 */
				vtcp_log("==����ȼ����ժҪ��ʲô===[%d]brf=[%s],g_pub_tx.tx_code=[%s]\n",__LINE__,g_pub_tx.brf,g_pub_tx.tx_code);
				if( g_mdm_ac_rel.note_sts[0]=='1' && strcmp(g_pub_tx.tx_code,"6203")==0 && strcmp(g_pub_tx.brf,"ȼ���˷�")==0)
				{
						vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
            sprintf(acErrMsg,"�ý��ʴ��ڹ�ʧ״̬,������ȼ���˷ѽ��ף���ס�����Ϊ��20140620��!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M009");
            goto ErrExit;
				}
				/* 20140620 wudawei end*/
        if( (g_mdm_ac_rel.note_sts[0]=='1' &&  strcmp(g_pub_tx.tx_code,"2204")!=0 )
            && strcmp(g_pub_tx.tx_code,"Z016") 
            && strcmp(g_pub_tx.tx_code,"9001") 
            && g_pub_tx.add_ind[0]=='0' 
            && g_pub_tx.ac_wrk_ind[8]!='1'
            && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)!=0 
            && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0 )  /*���д�������IC����ʧ������20121222 wzw*/
        {
            vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
            sprintf(acErrMsg,"�ý��ʴ��ڹ�ʧ״̬,������Ҵ���!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M009");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='2' )
        {
            sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P171");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='3' &&  strcmp(g_pub_tx.tx_code,"5840")!=0 ) /*С�����ʻ�֤���� wudawei 20140721*/
        {
            sprintf(acErrMsg,"�ý����ѻ�֤!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M011");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='4' 
            && g_pub_tx.svc_ind != 1012 )
        {
            sprintf(acErrMsg,"���˻���������!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M017");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='5' )
        {
            sprintf(acErrMsg,"���˻���������!!");
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
            if( g_mdm_ac_rel.coll_sts[0]=='2' && g_pub_tx.ac_wrk_ind[8]!='1' )
            {
                sprintf(acErrMsg,"�ý��ʴ����̿�״̬,�������黹!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M015");
                goto ErrExit;
            }
            else if( g_mdm_ac_rel.coll_sts[0]!='0' && g_pub_tx.ac_wrk_ind[8]!='1' )
            {
                sprintf(acErrMsg,"�ý��ʼ�ʰ״̬������!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M016");
                goto ErrExit;
            }
        }
    }
    vtcp_log("++++++++[%d]ac_id_type[%s],ac_id[%ld]+++++++\n",__LINE__,g_pub_tx.ac_id_type,g_pub_tx.ac_id);
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
    else if( g_pub_tx.ac_id_type[0]=='3' )
    {
        g_pub_tx.ac_seqn=0 ;

        ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",
                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
    }
    else if( g_pub_tx.ac_id_type[0]=='1' )
    {
        g_pub_tx.ac_seqn=1 ;

        ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret )
        {
            sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",
                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
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
    
    /*******ic�����˻�ͨ��ͨ�Ҵ��� ����20111220*******/
/*    memset(&sMo_ic_ac_rel,0x00,sizeof(sMo_ic_ac_rel));
    ret=Mo_ic_ac_rel_Sel(g_pub_tx.reply,&sMo_ic_ac_rel,"app_ac_no='%s'",g_pub_tx.ac_no);
    if(ret!=100&&ret)
    {
    	vtcp_log("[%s][%d] ��ѯmo_ic_ac_rel���� %d",__FILE__,__LINE__,ret);
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }*/
    /*******ic�����˻�ͨ��ͨ�Ҵ��� ����20111220*******/

    /*** û�����벻����ͨ��,�ж�ǩ�����ܼ�� ***/
    if( strcmp(g_pub_tx.tx_br_no,QS_BR_NO) && g_pub_tx.ac_wrk_ind[8]!='1'
    && strcmp(g_pub_tx.tx_code,"4333")
    && strcmp(g_pub_tx.tx_code,"4334")
    && strcmp(g_pub_tx.tx_code,"4335")
    /**&& strcmp(g_pub_tx.tx_code,"3201") ����������ҵ�û�����������޸� lzy 20141210**/
    /**&& strcmp(g_pub_tx.tx_code,"3202") ����������ҵ�û����Ϣ�����޸� lzy 20141210**/
    && strcmp(g_pub_tx.tx_code,"6388")/**����������ҵ�û�С����޸� hzh 20130602**/
    && strcmp(g_pub_tx.tx_code,"6389")/**����������ҵ�û������޸� hzh 20130602**/
    && strcmp(g_pub_tx.tx_code,"7102")/*lpj20151028����֧����������ͬ��**/
    /*&& strcmp(g_pub_tx.tx_code,"6236")*fupengfei 2014/7/14 11:50:39 ���л�Ʊ**/
/*    && (ret==100)    */                  /**ic�����˻�û�����룬��Ӧ���ܹ�ͨ��ͨ�� ����20111220**/
    && g_pub_tx.ac_id_type[0]!='9' 
    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
    && g_mdm_ac_rel.draw_pwd_yn[0]!='Y' && g_mdm_ac_rel.pwd_mach_yn[0]!='Y'
        && (g_pub_tx.ac_id_type[0]=='1' || g_pub_tx.ac_id_type[0]=='2') 
        && g_pub_tx.add_ind[0]=='0' )
    {
        sprintf(acErrMsg,"�����ܻ�������ͨ��![%s]",g_pub_tx.tx_code);
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

int pub_sub_prdt_chk()
{
    int ret=0;

    if( g_pub_tx.svc_ind!=1003 && g_pub_tx.svc_ind!=1004 )  /** ���� **/
        goto GoodExit;

    if( g_pub_tx.svc_ind==1003 )
    {
        /* �ж��д������˺Ų������� modify by martin 2009/7/28 17:11:58 */
        /* ret=sql_count("mo_prot_ac_rel"," (main_ac_id=%ld and main_ac_seqn=%d) or (app_ac_id=%ld and app_ac_seqn=%d) and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id,g_pub_tx.ac_seqn);*/
        ret=sql_count("mo_prot_ac_rel"," ((main_ac_id=%ld and main_ac_seqn=%d) or (app_ac_id=%ld and app_ac_seqn=%d)) and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret<0 )
        {
            sprintf(acErrMsg,"����Ƿ�ΪЭ����ƻ�����![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P280");
            goto ErrExit;
        }

        if( ret>0 )
        {
            sprintf(acErrMsg,"�û�ΪЭ����ƻ�,����ֱ������![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P281");
            goto ErrExit;
        }
    }

    if( g_pub_tx.svc_ind==1004 )
    {
        ret=sql_count("mo_prot_ac_rel"," app_ac_id=%ld and app_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret<0 )
        {
            sprintf(acErrMsg,"����Ƿ�ΪЭ����ƻ�����![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P280");
            goto ErrExit;
        }
        if( ret==0 )
        {
            sprintf(acErrMsg,"�û���Э����ƻ�,���������!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P282");
            goto ErrExit;
        }
    }
    
GoodExit:
    return 0;
ErrExit:
    return 1;
}

pub_sub_note()
{
    int ret=0;
    vtcp_log("mdm_code is [%s],ac_no[%s],ac_id[%ld]\n",g_mdm_ac_rel.mdm_code,g_pub_tx.ac_no,g_pub_tx.ac_id);
    /**��֧ȡ����**/
    if(!strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) || !memcmp(g_pub_tx.tx_code,"6236",4) || !memcmp(g_pub_tx.tx_code,"6238",4))
    {
        /*** ƾ֤���Ŵ��� ***/
        if( strlen(g_pub_tx.note_type) && strlen(g_pub_tx.beg_note_no) )
        {
            ret=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,
                g_pub_tx.note_type,g_pub_tx.beg_note_no,
                g_pub_tx.end_note_no,g_pub_tx.tel);
            if( ret )
            {
                sprintf(acErrMsg,"����ƾ֤���Ŵ���!");
                WRITEMSG
                goto ErrExit;
            }
        }
    }
    else
    {
        ret=Mdm_attr_Sel(g_pub_tx.reply,&v_mdm_attr," mdm_code='%s' ", \
                        g_mdm_ac_rel.mdm_code );
        if( ret )
        {
            sprintf(acErrMsg,"ȡmdm_attrʧ��![%s]",g_mdm_ac_rel.mdm_code);
            WRITEMSG
            strcpy( g_pub_tx.reply,"N055" );
            goto ErrExit;
        }

        /**��Ҫ�˶�ƾ֤��**/
        if( g_pub_tx.ac_wrk_ind[0]=='1' && memcmp(g_pub_tx.tx_code,"6789",sizeof(g_pub_tx.tx_code)-1)!=0)
        {
            if( strcmp(v_mdm_attr.note_type,g_pub_tx.note_type) ||
                strcmp(g_mdm_ac_rel.note_no,g_pub_tx.beg_note_no) )
            {
                sprintf(acErrMsg,"ƾ֤�Ų���![%s][%s],mdm[%s][%s]",
                    g_pub_tx.note_type,g_pub_tx.beg_note_no,
                    v_mdm_attr.note_type,g_mdm_ac_rel.note_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P169");
                goto ErrExit;
            }
        }
    }

    return 0;
ErrExit:
    return 1;
}
/*** �����۴��� ***/
pub_sub_prt_book()
{
    int ret=0;
    int totalnum = 0;

    totalnum=v_mdm_attr.pg_num*v_mdm_attr.pg_line;

    sprintf(acErrMsg,"XXXXXXXXXX! g_mdm_ac_rel.mdm_code=[%s]", \
             g_mdm_ac_rel.mdm_code);
        WRITEMSG
    /**��֧ȡ����**/
    if( !strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) )
        goto GoodExit;

    sprintf(acErrMsg,"XXXXXXXXXX! v_mdm_attr.prt_ind=[%s]", \
             v_mdm_attr.prt_ind);
        WRITEMSG
    if( v_mdm_attr.prt_ind[0]!='Y' )
        goto GoodExit;

    sprintf(acErrMsg,"XXXXXXXXXX! g_pub_tx.prt_ind=[%s],ac_no=[%s]", \
                 g_pub_tx.prt_ind,g_mdm_ac_rel.ac_no);
        WRITEMSG
    if( g_pub_tx.prt_ind[0]!='1' )
        goto GoodExit;

    vtcp_log("<<<<<<<<<<<<<<PRT_LINE====[%d]",g_mdm_ac_rel.prt_line);
    /*add by xx �жϴ�������ʾ����*/
    if(strncmp(v_mdm_attr.mdm_code,"0016",4))
    {
    if( g_mdm_ac_rel.prt_line % totalnum == 0 )
    {
        strcpy( g_pub_tx.reply, "P509" );
        sprintf(acErrMsg,"���������Ȼ�����������![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    }

    ret=pub_base_patch_book(v_mdm_attr);
    if( ret==0 )    set_zd_data("0110","0");
    else if( ret>0 )    set_zd_data("0110","1");
    else
    {
        sprintf(acErrMsg,"�����۴���![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    vtcp_log("<<<<<<<<<<<<<<PRT_LINE2====[%d]",g_mdm_ac_rel.prt_line);
    vtcp_log("������PASSPASSPASSPASSPASSPASS!!");   

GoodExit:
    return 0;
ErrExit:
    return 1;
}
pub_sub_mst()
{
    int ret=0 ;
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

    vtcp_log("[%s][%d] HST_BAL==[%.2lf] ACBO_BAL==[%.2lf] ", __FILE__,__LINE__, g_dd_mst.hst_bal,g_dd_mst.acbo_bal);
    vtcp_log("[%s][%d] TX_AMT1==[%.2lf] TX_AMT2==[%.2lf] ", __FILE__,__LINE__, g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
    /*** ��鴦��ֻ���,��ϸ�� ***/
    switch( g_pub_tx.ac_id_type[0] )
    {
        case '1':   
            ret=Chg_dd_mst();
            ERR_DEAL
            break;
        case '2':   
            ret=Chg_td_mst();
            ERR_DEAL
            break;
        case '3':   
            ret=Chg_ln_mst();
            ERR_DEAL
            break;
        case '9':   
            ret=Chg_in_mst();
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
pub_sub_spe_do()
{
    int ret;
    int cnt1,cnt2;
    struct pub_tx v_pub_tx;

    /** �������ú��Ӽ���(���߼��˽��׳���) **/
    vtcp_log("[%s],[%d] g_pub_tx.tx_code=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
    if( strcmp(g_pub_tx.tel,"9999") &&
        strcmp(g_pub_tx.tx_code,"2502") && strcmp(g_pub_tx.tx_code,"LNRE") &&
        strcmp(g_pub_tx.tx_code,"ZTSC") && strcmp(g_pub_tx.tx_code,"7711") &&
        strcmp(g_pub_tx.tx_code,"5839") &&
        strcmp(g_pub_tx.tx_code,"4162") &&
        strcmp(g_pub_tx.tx_code,"4334") &&
        strcmp(g_pub_tx.tx_code,"5840") &&
        strcmp(g_pub_tx.tx_code,"5380") &&
        strcmp(g_pub_tx.tx_code,"3206") &&
        strcmp(g_pub_tx.tx_code,"6203") &&
        strcmp(g_pub_tx.tx_code,"D131") &&
        strcmp(g_pub_tx.tx_code,"3319") &&
        strcmp(g_pub_tx.tx_code,"D129") &&
        strcmp(g_pub_tx.tx_code,"8619") &&
        strcmp(g_pub_tx.tx_code,"CFEE") &&
        /*BEGIN ADD BY XJ 20101021 */
        strcmp(g_pub_tx.tx_code,"6789") &&
        /*END ADD BY XJ 20101021 */
	/***add zgf 20131031 �������ʳ�����֤���***/
        strcmp(g_pub_tx.tx_code,"6235") &&/*���ʳ��˽ӿڷ���������Ǹ��� Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"6247") && /*�������˽ӿڷ���������Ǹ��� Added by YangMeng 2014/7/12 17:22:09*/
         strcmp(g_pub_tx.tx_code,"7023") &&/*�������˳��˽ӿڷ���������Ǹ��� Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"7024") &&/*�������˳��˽ӿڷ���������Ǹ��� Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"7028") &&/*�������˳��˽ӿڷ���������Ǹ��� Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"6254") &&/*�������˳��˽ӿڷ���������Ǹ��� Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"5111") &&/*�ͻ�ƾ֤ע���˷� ���� Added by wudawei 2014/8/6 15:20:09*/
        strcmp(g_pub_tx.tx_code,"6234") &&/*����ת�˽ӿڷ��������Ϊ�� Added by guanjianfeng 2015/8/20 15:20:09*/
        strcmp(g_pub_tx.tx_code,"7102") &&/*���񵥱�֧���տ���˽ӿڣ���������7102 lipeijin 20151027*/
        strcmp(g_pub_tx.tx_code,"9014") &&/*ֱ��/���ʽת����ת��Ʊ����Ϣ��ѯ�����ӿ�,ֱ��Ʊ����Ϣ��ѯ9014  chenfuqiang 20151029*/
        /***add zgf end***/
	strcmp(g_pub_tx.tel,"9999") &&
         (pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)<0 
        || pub_base_CompDblVal(g_pub_tx.tx_amt2,0.00)<0 
        || pub_base_CompDblVal(g_pub_tx.tx_amt3,0.00)<0 
        || pub_base_CompDblVal(g_pub_tx.tx_amt4,0.00)<0) )
    {
        sprintf(acErrMsg,"�������Ϊ����![%lf][%lf][%lf][%lf]",g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P121");
        goto ErrExit;
    }

    /*** ����Ƿ�����Ʋ�Ʒ **** ǰ������� ***
    if( g_pub_tx.add_ind[0]=='2' )
    {
        ret=sql_count("mo_prdt_prot"," main_ac_id=%ld and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_id);
        if( ret!=0 )
        {
            sprintf(acErrMsg,"�û�����ƻ�Э����Ʒ��������![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P182");
            goto ErrExit;
       }
    }
    ****/

    if( g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004 
        || g_pub_tx.svc_ind==1010 || g_pub_tx.svc_ind==9002 )
    {
        ret=pub_base_InsMo_opn_cls(g_pub_tx.name,"","2");
        if( ret )
        {
            sprintf(acErrMsg,"�Ǽǿ������Ǽǲ�����!");
            WRITEMSG
            goto ErrExit;
        }
    }

    /*** ����Ǵ���(��һ��ͨ)���浥������ ***/
    if( g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004
            || g_pub_tx.svc_ind==1009 || g_pub_tx.svc_ind==1010 )
    {
        if( !strcmp(g_mdm_ac_rel.mdm_code,"0016") )
        {
vtcp_log("CNT һ��ͨ��������[%s][%s]",v_mdm_attr.mdm_code,v_mdm_attr.ac_num_ind);
        }
        else if( !strcmp(v_mdm_attr.mdm_code,NULL_MDM_CODE) 
            || v_mdm_attr.ac_num_ind[0]=='Y' )
        {
vtcp_log("CNT [%s][%s]",v_mdm_attr.mdm_code,v_mdm_attr.ac_num_ind);
            strcpy( v_pub_tx.opn_br_no,g_pub_tx.opn_br_no );
            ret=pub_acct_cls_mdm();
            if( ret )
            {
                sprintf(acErrMsg,"����������ģ���!");
                WRITEMSG
                goto ErrExit;
            }
            strcpy( g_pub_tx.opn_br_no,v_pub_tx.opn_br_no );
        }
        else
        {
            /*
            cnt1=sql_count("dd_mst","ac_id=%ld and ac_sts='1' ",g_mdm_ac_rel.ac_id );
            if( cnt1<0 )
            {
                strcpy( g_pub_tx.reply,"D103" );
                goto ErrExit;
            }
            cnt2=sql_count("td_mst","ac_id=%ld and ac_sts='1' ",g_mdm_ac_rel.ac_id );
            if( cnt2<0 )
            {
                strcpy( g_pub_tx.reply,"D103" );
                goto ErrExit;
            }
vtcp_log("CNT [%d][%d]",cnt1,cnt2);
            if( cnt1+cnt2==0 )
            {
                ret=pub_acct_cls_mdm();
                if( ret )
                {
                    sprintf(acErrMsg,"����������ģ���!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
            */
        }
    }
    return 0;
ErrExit:
    return 1;
}
pub_sub_liqu()
{
    int ret;

    /*** ͨ��ͨ���޸�407̨�� ***/
    if( strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
    {
        /*
        ret=pubf_ass_liqu();
        if( ret )
        {
            sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
            WRITEMSG
            goto ErrExit;
        }
        */
    }
    return 0;
ErrExit:
    return 1;
}

int pub_sub_prot(double tmpje)
{
    struct mo_prdt_prot_c   mo_prdt_prot_c;
    int ret=0;
    memset(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));
    
    vtcp_log("���Э�����ʼ!");
    ret=Mo_prdt_prot_Sel(g_pub_tx.reply,&mo_prdt_prot_c,"main_ac_id=%ld and main_ac_seqn=%d and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret==100 )  
    {
        sprintf(acErrMsg,"��Э��������ƻ�![%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        WRITEMSG
        return 0;
    }
    else if( ret )
    {
        sprintf(acErrMsg,"����Ƿ�ΪЭ����ƻ�����![%ld][%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P280");
        goto ErrExit;
    }
    
    switch(mo_prdt_prot_c.prot_type[0])
    {
        case 'X':
                if( pub_sub_prot_x(tmpje,mo_prdt_prot_c) )
                {
                    sprintf(acErrMsg,"Э������ش���!");
                    WRITEMSG
                    goto ErrExit;
                }
                break;
        case 'T':
                if( pub_sub_prot_t(tmpje,mo_prdt_prot_c) )
                {
                    sprintf(acErrMsg,"��ƻ�����ش���!");
                    WRITEMSG
                    goto ErrExit;
                }
                break;
        default:
            sprintf(acErrMsg,"Э������ǿ��ռ仯����!");
            WRITEMSG
            break;
    }
    vtcp_log("���Э������ɹ�!");
    
    return 0;
ErrExit:
    return 1;
}

int pub_sub_prot_x(double tmpje,struct mo_prdt_prot_c mo_prdt_prot_c)
{
    int ret=0;
    struct mo_prot_ac_rel_c     mo_prot_ac_rel_c;
    struct dd_mst_c             prot_dd_mst;

    struct pub_tx           tmp_pub_tx;
    struct mdm_ac_rel_c     tmp_mdm_ac_rel;
    struct dd_mst_c         tmp_dd_mst_c;
    struct prdt_ac_id_c     tmp_prdt_ac_id_c;
    struct mdm_attr_c       tmp_mdm_attr_c;
    struct note_parm_c      tmp_note_parm_c;
    char   main_dc_code[5];
    char   T_ct_ind[2];
    double T_tx_amt=0.00;

    memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));
    memset(&prot_dd_mst,0x00,sizeof(struct dd_mst_c));

    memset(&tmp_pub_tx,0x00,sizeof(struct pub_tx));
    memset(&tmp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&tmp_dd_mst_c,0x00,sizeof(struct dd_mst_c));
    memset(&tmp_prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
    memset(&tmp_mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
    memset(&tmp_note_parm_c,0x00,sizeof(struct note_parm_c));
    get_zd_data("102K",T_ct_ind);
    get_zd_double("102I",&T_tx_amt);

    ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&mo_prot_ac_rel_c,"main_ac_id=%ld and main_ac_seqn=%d and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"ȡЭ���˺Ŵ���![%d][%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        return 0;
    }

    ret=pub_base_currentid_file(mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn,&prot_dd_mst);
    if( ret )
    {
        sprintf(acErrMsg,"ȡЭ���˺����ļ�����!");
        WRITEMSG
        return 0;
    }

    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)<0 )
    {
        sprintf(acErrMsg,"Э��������!");
        WRITEMSG
        return 0;
    }
    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)>0 )/* ת�ز���add by martin 2009/6/16 17:30:28*/
        {
        }else
            {
          tmpje=prot_dd_mst.bal;    /** ���㻧���㣬һ�ʻ��� **/
        }
        vtcp_log("%s,%d g_pub_tx.tx_amt1=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

    memcpy(&tmp_pub_tx,&g_pub_tx,sizeof(struct pub_tx));
    memcpy(&tmp_mdm_ac_rel,&g_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&tmp_dd_mst_c,&g_dd_mst,sizeof(struct dd_mst_c));
    memcpy(&tmp_prdt_ac_id_c,&prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&tmp_mdm_attr_c,&mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&tmp_note_parm_c,&note_parm_c,sizeof(struct note_parm_c));

    /*** ���㻧��� ***/
    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)>0 )/*ת�ز���add by martin 2009/6/16 17:30:28*/
        {
                g_pub_tx.tx_amt1=tmpje;
        }else
            {
            g_pub_tx.tx_amt1=prot_dd_mst.bal;
        }
 vtcp_log("%s,%d g_pub_tx.tx_amt1=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
  /**g_pub_tx.tx_amt1=prot_dd_mst.bal; del by martin 2009/6/16 17:30:17**/
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.add_ind,"1");
    strcpy(g_pub_tx.hst_ind,"1");
    strcpy(g_pub_tx.ac_wrk_ind,"0000");
    g_pub_tx.svc_ind=1001;
    strcpy(g_pub_tx.ac_id_type,"");
    strcpy(g_pub_tx.note_type,"");
    strcpy(g_pub_tx.beg_note_no,"");
    strcpy(g_pub_tx.end_note_no,"");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("102K",g_pub_tx.ct_ind);
    /**set_zd_double("102I",g_pub_tx.tx_amt1);**/
    set_zd_double("1002",g_pub_tx.tx_amt1);
    strcpy(main_dc_code,g_pub_tx.dc_code);
    vtcp_log("%s,%bbbbbbbb=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    g_reply_int=pubf_acct_proc("LC02");
    if( g_reply_int )
    {
        sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

    tmp_dd_mst_c.bal=g_dd_mst.bal;
    tmp_dd_mst_c.acbo_bal=g_dd_mst.acbo_bal;
    tmp_dd_mst_c.hst_bal=g_dd_mst.hst_bal;
    tmp_dd_mst_c.intst_acm=g_dd_mst.intst_acm;
    tmp_dd_mst_c.hst_cnt=g_dd_mst.hst_cnt;
    tmp_dd_mst_c.lst_date=g_dd_mst.lst_date;
    strcpy(tmp_dd_mst_c.mac,g_dd_mst.mac);
    vtcp_log("[%s][%d] HST_BAL==[%.2lf] ACBO_BAL==[%.2lf] ", __FILE__,__LINE__,g_dd_mst.hst_bal,g_dd_mst.acbo_bal);

    /*** Э����ȡ�� ***/
    strcpy(g_pub_tx.ac_no,"");
    g_pub_tx.ac_id=mo_prot_ac_rel_c.app_ac_id;
    g_pub_tx.ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
    strcpy(g_pub_tx.draw_uncon_yn,"Y"); 
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.add_ind,"0"); /**����**/
    strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
    strcpy(g_pub_tx.ac_wrk_ind,"0000");
    g_pub_tx.svc_ind=1008; /**��ȡ��***/
    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)>0 )/*ת�ز���add by martin 2009/6/16 17:30:28*/
        {
                g_pub_tx.tx_amt1=tmpje;
        }else
            {
            g_pub_tx.tx_amt1=prot_dd_mst.bal;
        }
vtcp_log("%s,%d g_pub_tx.tx_amt1=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    /*g_pub_tx.tx_amt1=prot_dd_mst.bal;*/
    strcpy(g_pub_tx.ac_id_type,"");
    strcpy(g_pub_tx.note_type,"");
    strcpy(g_pub_tx.beg_note_no,"");
    strcpy(g_pub_tx.end_note_no,"");
    ret=pub_acct_trance();
    if( ret )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("102K",g_pub_tx.ct_ind);
    /****����100���1����  modify at 20130218
    set_zd_double("102I",g_pub_tx.tx_amt1);
    ****/
    set_zd_double("1002",g_pub_tx.tx_amt1);
    strcpy(main_dc_code,g_pub_tx.dc_code); /* ����20301��λЭ������  ������˴��� ���� add by wjl 20131022*/
    set_zd_data(DC_CODE,main_dc_code);
    vtcp_log("%s,%eeeeeeee=[%f],main_dc_code=[%s]",__FILE__,__LINE__,g_pub_tx.tx_amt1,main_dc_code);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    g_reply_int=pubf_acct_proc("LC03");
    if( g_reply_int )
    {
        sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

    set_zd_double("102I",T_tx_amt);
    memcpy(&g_pub_tx,&tmp_pub_tx,sizeof(struct pub_tx));
    memcpy(&g_mdm_ac_rel,&tmp_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&g_dd_mst,&tmp_dd_mst_c,sizeof(struct dd_mst_c));
    memcpy(&prdt_ac_id_c,&tmp_prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&mdm_attr_c,&tmp_mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&note_parm_c,&tmp_note_parm_c,sizeof(struct note_parm_c));
    set_zd_data("102K",T_ct_ind);

    return 0;
ErrExit:
    return 1;
}

int pub_sub_prot_t(double tmpje,struct mo_prdt_prot_c mo_prdt_prot_c)
{
    int ret=0;
    double turn_amt=0.00;
    struct mo_prot_ac_rel_c     mo_prot_ac_rel_c;
    struct td_mst_c             prot_td_mst;
    char T_ct_ind[2];
    double T_tx_amt=0.00;

    struct pub_tx tmp_pub_tx;
    struct mdm_ac_rel_c tmp_mdm_ac_rel;
    struct dd_mst_c tmp_dd_mst_c;
    struct prdt_ac_id_c tmp_prdt_ac_id_c;
    struct mdm_attr_c tmp_mdm_attr_c;
    struct note_parm_c tmp_note_parm_c;

    memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));
    memset(&prot_td_mst,0x00,sizeof(struct td_mst_c));

    memset(&tmp_pub_tx,0x00,sizeof(struct pub_tx));
    memset(&tmp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&tmp_dd_mst_c,0x00,sizeof(struct dd_mst_c));
    memset(&tmp_prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
    memset(&tmp_mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
    memset(&tmp_note_parm_c,0x00,sizeof(struct note_parm_c));

    memcpy(&tmp_pub_tx,&g_pub_tx,sizeof(struct pub_tx));
    memcpy(&tmp_mdm_ac_rel,&g_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&tmp_dd_mst_c,&g_dd_mst,sizeof(struct dd_mst_c));
    memcpy(&tmp_prdt_ac_id_c,&prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&tmp_mdm_attr_c,&mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&tmp_note_parm_c,&note_parm_c,sizeof(struct note_parm_c));
    get_zd_data("102K",T_ct_ind);
    get_zd_double("102I",&T_tx_amt);

    ret=Mo_prot_ac_rel_Dec_Upd(g_pub_tx.reply,"main_ac_id=%ld and main_ac_seqn=%d and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"ȡЭ���˺Ŵ���!");
        WRITEMSG
        return 0;
    }

    while(1)
    {
        ret=Mo_prot_ac_rel_Fet_Upd(&mo_prot_ac_rel_c,g_pub_tx.reply);
        if( ret==100 ) break;
        if( ret )
        {
            sprintf(acErrMsg,"ȡЭ���˺Ŵ���!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P181");
            return 1;
        }

        ret=pub_base_termiid_file(mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn,&prot_td_mst);
        if( ret )
        {
            sprintf(acErrMsg,"ȡЭ���˺����ļ�����!");
            WRITEMSG
            return 0;
        }

        /*** ��ƻ����� ***/
        strcpy(g_pub_tx.ac_no,"");
        g_pub_tx.ac_id=mo_prot_ac_rel_c.app_ac_id;
        g_pub_tx.ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
        strcpy(g_pub_tx.draw_uncon_yn,"Y"); 
        strcpy(g_pub_tx.ct_ind,"2");
        strcpy(g_pub_tx.add_ind,"0");
        strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
        strcpy(g_pub_tx.ac_wrk_ind,"0000");
        g_pub_tx.svc_ind = 1004;
        g_pub_tx.tx_amt1=prot_td_mst.bal;
        strcpy(g_pub_tx.ac_id_type,"");
        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
            WRITEMSG
            goto ErrExit;
        }
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("102I",g_pub_tx.tx_amt1);
        set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);
        set_zd_double("102Q",g_pub_intst.val);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
        g_reply_int=pubf_acct_proc("LC01");
        if( g_reply_int )
        {
            sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
            WRITEMSG
            goto ErrExit;
        }

        strcpy(mo_prot_ac_rel_c.sts,"*");
        ret=Mo_prot_ac_rel_Upd_Upd(mo_prot_ac_rel_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"mo_prot_ac_rel_upd_upd erro ![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P283");
            goto ErrExit;
        }

        turn_amt=turn_amt+g_pub_tx.tx_amt1+g_pub_intst.factval+g_pub_intst.keepval;
        if( tmpje<turn_amt )
            break;
    }
    Mo_prot_ac_rel_Clo_Upd();

    /*** ���㻧��� ***/
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.add_ind,"1");
    strcpy(g_pub_tx.hst_ind,"1");
    strcpy(g_pub_tx.ac_wrk_ind,"0000");
    g_pub_tx.svc_ind=1001;
    g_pub_tx.tx_amt1=turn_amt;
    strcpy(g_pub_tx.ac_id_type,"");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("102K",g_pub_tx.ct_ind);
    /**set_zd_double("102I",g_pub_tx.tx_amt1);**/
    set_zd_double("1002",g_pub_tx.tx_amt1);
    vtcp_log("%s,%cccccccc=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    g_reply_int=pubf_acct_proc("LC02");
    if( g_reply_int )
    {
        sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

    tmp_dd_mst_c.bal=g_dd_mst.bal;
    tmp_dd_mst_c.acbo_bal=g_dd_mst.acbo_bal;
    tmp_dd_mst_c.hst_bal=g_dd_mst.hst_bal;
    tmp_dd_mst_c.intst_acm=g_dd_mst.intst_acm;
    tmp_dd_mst_c.hst_cnt=g_dd_mst.hst_cnt;
    tmp_dd_mst_c.lst_date=g_dd_mst.lst_date;
    strcpy(tmp_dd_mst_c.mac,g_dd_mst.mac);
    vtcp_log("[%s][%d] HST_BAL==[%.2lf] ACBO_BAL==[%.2lf] ",  __FILE__,__LINE__,g_dd_mst.hst_bal,g_dd_mst.acbo_bal);

    memcpy(&g_pub_tx,&tmp_pub_tx,sizeof(struct pub_tx));
    memcpy(&g_mdm_ac_rel,&tmp_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&g_dd_mst,&tmp_dd_mst_c,sizeof(struct dd_mst_c));
    memcpy(&prdt_ac_id_c,&tmp_prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&mdm_attr_c,&tmp_mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&note_parm_c,&tmp_note_parm_c,sizeof(struct note_parm_c));
    set_zd_data("102K",T_ct_ind);
    set_zd_double("102I",T_tx_amt);
    set_zd_double("102P",0.00);
    set_zd_double("102Q",0.00);

    return 0;
ErrExit:
    return 1;
}

int pub_sub_chkadd(double ys_bal,double tx_amt1)
{
    int ret=0;
    double tmpamt=0.00;
    double tmpamt1=0.00;
    double tmpamt2=0.00;

    /***Ŀǰ����Ҫ�ж���ش�ȡ��ȡ���***/
    return(0);

    /***��ʱTEMP**/
    if( !strncmp(g_pub_tx.tx_code,"9001",4) ) 
        return 0;

    ret=sql_sum_double("trace_log","amt",&tmpamt, 
        "sts='0' and (sub_tx_code='D099' or sub_tx_code='A017') and tx_code!='6604' and ac_id=%d and ac_seqn=%d and tx_br_no!='%s' and saving_notarize='0' ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.tx_br_no);
    if( ret )
    {
        sprintf(acErrMsg,"���㱾�����ȡ���ʧ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P295");
        goto ErrExit;
    }
    if( pub_base_CompDblVal(tmpamt,0.00)==0 )
    {
        sprintf(acErrMsg,"û����Ҫȷ�ϵĴ��!");
        WRITEMSG
        return 0;
    }

    ret=sql_sum_double("trace_log","amt",&tmpamt1,
    "sts='0' and ac_id=%d and ac_seqn=%d and (((sub_tx_code='D099' or sub_tx_code='A017') and (tx_br_no='%s' or saving_notarize='1')) or sub_tx_code='D002' or tx_code='6604' ) ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.tx_br_no); 
    if( ret )
    {
        sprintf(acErrMsg,"���㱾�����ȡ���ʧ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P295");
        goto ErrExit;
    }

    ret=sql_sum_double("trace_log","amt",&tmpamt2,
        "sts='0' and (sub_tx_code='D003' or sub_tx_code='A016') and ac_id=%ld and ac_seqn=%d ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn); 
    if( ret )
    {
        sprintf(acErrMsg,"���㱾�����ȡ���ʧ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P295");
        goto ErrExit;
    }

    vtcp_log("ys_bal=[%lf],tmpamt1=[%lf],tx_amt1=[%lf],tmpamt2=[%f]",
        ys_bal,tmpamt1,tx_amt1,tmpamt2);

    if( pub_base_CompDblVal(ys_bal+tmpamt1,tx_amt1+tmpamt2)<0 )
    {
        sprintf(acErrMsg,"�������ȡ���㣬����������ȷ�ϴ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P296");
        goto ErrExit;
    }

    return 0;
ErrExit:
    return 1;
}

pub_acct_NoteUse_log( ac_id, ac_seqn, note_type, beg_no, end_no, cnt )
 long ac_id,ac_seqn;
 char *note_type;
 char *beg_no;
 char *end_no;
 long cnt;
{
    struct pub_tx v_pub_tx;
    int ret=0;

    memcpy( &v_pub_tx,&g_pub_tx,sizeof(g_pub_tx) );
    
    g_pub_tx.ct_ind[0]=' ';
    strcpy(g_pub_tx.ac_no,"702");
    g_pub_tx.ac_seqn=0;
    /*g_pub_tx.ac_id=ac_id;*/
    g_pub_tx.ac_id=0;
    g_pub_tx.svc_ind=7021;
    strcpy( g_pub_tx.note_type,note_type );
    strcpy( g_pub_tx.beg_note_no,beg_no );
    strcpy( g_pub_tx.end_note_no,end_no );
    strcpy( g_pub_tx.prdt_code,"" );
    strcpy( g_pub_tx.cur_no,"01" );
    g_pub_tx.add_ind[0]='0';
    g_pub_tx.hst_ind[0]='1';
    g_pub_tx.tx_amt1=(double)cnt;

    ret=pub_ins_trace_log();
    if( ret ) 
    {
        TRACE
        goto ErrExit;
    }
TRACE
    get_zd_double("0520",&g_pub_tx.tx_amt2);
    set_zd_double("0520",g_pub_tx.tx_amt1);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    ret=pubf_acct_proc( "N001" );
    if( ret )
    {
        goto ErrExit;
    }
    set_zd_double("0520",g_pub_tx.tx_amt2);

    memcpy( &g_pub_tx,&v_pub_tx,sizeof(g_pub_tx) );
TRACE   
    return 0;
ErrExit:
    return 1;
}

/* add by xxx at 20030727 **/
int pub_chk_JYCX()
{
    double t_amt=0.00;
    int ret=0;
    struct td_mst_hst_c sTd_mst_hst;

    memset(&sTd_mst_hst, 0x0, sizeof(struct td_mst_hst_c));

    /* ��������һ�����ڲ��ܴ�2�� */
    sprintf(acErrMsg, "CGH1*** ����������!!");
    WRITEMSG

    if (g_pub_tx.tx_date / 100 == g_td_mst.opn_date / 100)
    {
        sprintf(acErrMsg, "CGH2*** ����������!!");
        WRITEMSG

        ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and \
            ac_seqn=%d order by tx_date", g_td_mst.ac_id, g_td_mst.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg, "DECLARE FOR SELECT ERROR !! [%d]", ret);
            WRITEMSG
            goto ErrExit;
        }

        while(1)
        {
            memset(&sTd_mst_hst, 0x0, sizeof(struct td_mst_hst_c));
            ret = Td_mst_hst_Fet_Sel(&sTd_mst_hst, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg, "Fetch for select error !! [%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 100)
            {
                break;
            }

            if (sTd_mst_hst.tx_date / 100 == g_pub_tx.tx_date / 100)
            {
                 t_amt += sTd_mst_hst.tx_amt;
            }
            else
            {
                break;
            }
        }
        Td_mst_hst_Clo_Sel();

        t_amt += g_pub_tx.tx_amt1;
        if (pub_base_CompDblVal(t_amt, 20000.00) >= 0)
        {
            sprintf(acErrMsg, "һ�����ڲ��ܴ泬������!! [%lf]", t_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply, "O241");
            goto ErrExit;
        }
    }
    return 0;
ErrExit:
    return 1;
}
int pub_acct_ac_rec( ac_id,ac_seqn,add,ct,tx_code,amt,ind )
 long ac_id;
 int ac_seqn;
 char *add;
 char *ct;
 char *tx_code;
 double amt;
 char *ind;
{
    int ret=0;
    struct auth_ac_rec_c auth_ac_rec;
    char dc[2];

    dc[0]=add[0]+1;
    dc[1]='\0';

    if( !strcmp(g_pub_tx.tel,"9999") ) return 0;
    if( !strcmp(tx_code,"5702") ) return 0;

    ret=Auth_ac_rec_Dec_Upd( g_pub_tx.reply,
        "ac_id=%ld and ac_seqn=%d and dc='%s' and ct='%s' ",
        ac_id,ac_seqn,dc,ct );
    ERR_DEAL

    ret=Auth_ac_rec_Fet_Upd( &auth_ac_rec,g_pub_tx.reply );
    if( ret==100 )
    {
        memset( &auth_ac_rec,0,sizeof(auth_ac_rec) );
        auth_ac_rec.ac_id=ac_id;
        auth_ac_rec.ac_seqn=ac_seqn;
        memcpy( auth_ac_rec.dc,dc,sizeof(auth_ac_rec.dc)-1);
        memcpy( auth_ac_rec.ct,ct,sizeof(auth_ac_rec.ct)-1);
        auth_ac_rec.amt=amt;
        memcpy( auth_ac_rec.tx_code,tx_code,sizeof(auth_ac_rec.tx_code)-1);
        ret=Auth_ac_rec_Ins( auth_ac_rec,g_pub_tx.reply );
        if( ret ) goto ErrExit;
    }
    else if( ret )
    {
        Auth_ac_rec_Clo_Upd( );
        goto ErrExit;
    }
    else
    {
        auth_ac_rec.amt+=amt;
        ret=Auth_ac_rec_Upd_Upd( auth_ac_rec,g_pub_tx.reply  );
        if( ret ) goto ErrExit;
    }

    Auth_ac_rec_Clo_Upd( );

    return 0;
ErrExit:
    sprintf(acErrMsg, "�����ʻ���Ȩ�ۼƽ������!! [%d]",ret);
    WRITEMSG
    Auth_ac_rec_Debug(&auth_ac_rec);
    strcpy(g_pub_tx.reply, "D103");
    return 1;
}
/*******************************
add by chenchao 2011-7-31 17:21:40 
1.��������֧��Э���Ϊ�գ���ȡ���ڵ������ʽ𲿷�,������(����ſ��-������֧ȡ-��ί��֧ȡ)֮��Ĳ���
2.�������֧��Э��ŷǿգ���ȡ����ֵĽ��,��(����ſ��-������֧ȡ-��ί��֧ȡ-֧����ɽ��)֮��Ĳ���
add/modify by yanqq 2011-12-13 11:20
1.�����ǰ��Ԥ��Ȩ����ʱȡ�õ�ǰԤ��Ȩ���ƽ�tmpֵ�ӵ�ǰԤ��Ȩ���ƽ��
*******************************/
int dd_limit()
{
    int    ret;
    char   cDraw_no[61];
    double dRest_amt=0.0,temp;
    double dBal_virtul = 0.0;                     
    long premise_no;                                /* add by yanqq 20111213 ׷��Ԥ��Ȩ���� */
    char premise_flg[2];                            /* add by yanqq 20111213 Ԥ��Ȩ�жϱ�־ */
    char chongzheng_flg[2];                         /* add by yanqq 20111213 �����жϱ�־ */

    struct draw_loan_hst_c   sDraw_hst;
    struct dd_mst_c          sDd_mst;
    struct chnl_premise_c mChnl_premise;            /* add by yanqq 20111213 ׷��Ԥ��Ȩ���� */

    memset(cDraw_no,0x00,sizeof(cDraw_no));
    memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));   /* add by yanqq 20111213 ׷��Ԥ��Ȩ���� */
    memset(premise_flg,0x00,sizeof(premise_flg));               /* add by yanqq 20111213 Ԥ��Ȩ�жϱ�־ */
    memset(chongzheng_flg,0x00,sizeof(chongzheng_flg));         /* add by yanqq 20111213 �����жϱ�־ */

    if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)==0)
        return(0);

    /* add beg by yanqq 20111213 ȡ�õ�ǰԤ��Ȩ�Ŷ�Ӧ�Ŀ��ƽ�� */
    if(strcmp(g_pub_tx.tx_code,"6203")==0)
    {
        /* 6203�����С�70����ֵΪ2,3,4ʱ��ΪԤ��Ȩ�ཻ�� */
        get_zd_data("0700",premise_flg);
        /* 6203�����С�67����ֵΪ0ʱ��Ϊ�ǳ����ཻ�� */
        get_zd_data("0670",chongzheng_flg);
        if((premise_flg[0] == '2'|| premise_flg[0] == '3'|| premise_flg[0] == '4') && chongzheng_flg[0] == '0')
	    {
	        get_zd_long("0870",&premise_no); 
            ret = Chnl_premise_Sel(g_pub_tx.reply,&mChnl_premise,"ac_no='%s' and premise_no=%ld",g_pub_tx.crd_no,premise_no);
            vtcp_log("%s,%d,��ǰ�ʿ���=[%s],��ǰԤ��Ȩ��=[%ld],���ƽ��=[%lf]",__FILE__,__LINE__,g_pub_tx.crd_no,premise_no,mChnl_premise.premise_amt);
        }
    }
    /* add end by yanqq 20111213 */

    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%d",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if(ret)
    {
        vtcp_log("[%s][%d]���ڱ��ȡ����[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W014");
        return 1;
    }

    if(sDd_mst.od_ind[0]=='Y')  /* ��͸֧���Ͳ����� */
        return (0);

    if(strcmp(g_pub_tx.tx_code,"5802")==0 ||strcmp(g_pub_tx.tx_code,"5842")==0)
    {
        get_zd_data("1188",cDraw_no);
    }
    else
    {
        get_zd_data("1126",cDraw_no);
    }
    zip_space(cDraw_no);
    vtcp_log("[%s][%d]֧ȡЭ�飺%s",__FILE__,__LINE__,cDraw_no);
    if(strlen(cDraw_no)==0)/**û�д���֧ȡЭ���**/
    {
        ret=limit_draw_no_less(&dRest_amt);
        if(ret)
        {
            vtcp_log("[%s][%d] branch 1 ��ɿ��ý�%.2f",__FILE__,__LINE__,dRest_amt);
            return 1;
        }
    }
    else                   /**����֧ȡЭ���**/
    {
        /*  by Wang Yongwei ������֧������ŵģ�һ�����������������(�շ�)״̬�ͱ�Ϊ��֧ȡ�� 20101201*/
        if(memcmp(g_pub_tx.sub_tx_code,"SF",2)==0)
        {
            vtcp_log("%s,%d,�շѲ�����Ƿ�ί��֧��!",__FILE__,__LINE__);
        }else{
            ret=limit_draw_no_yes(&dRest_amt,cDraw_no);
            if(ret)
            {
                vtcp_log("[%s][%d]branch 2 ��ɿ��ý�%.2f",__FILE__,__LINE__,dRest_amt);
                return 1;
            }
        }
    }
		/**add by zyl 20121211 7���ֲ�Ʒ���,���޸�temp�ļ���**/
    ret = iGet7DaysVtlBal(g_pub_tx.ac_id,&dBal_virtul);
    if(ret)
    {
        vtcp_log("[%s][%d] ��鷢������...",__FILE__,__LINE__);
        return 1;
    }
    /**end by zyl 20121211**/
    /* modify beg by yanqq 20111213 ׷��Ԥ��Ȩ���� */
    /* temp=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt-dRest_amt-g_pub_tx.tx_amt1; */
    /* vtcp_log("[%s][%d]sDd_mst.bal=%.2f sDd_mst.hold_amt=%.2f sDd_mst.ctl_amt=%.2f dRest_amt=%.2f g_pub_tx.tx_amt1=%.2f  temp=%.2f ",__FILE__,__LINE__,sDd_mst.bal,sDd_mst.hold_amt,sDd_mst.ctl_amt,dRest_amt,g_pub_tx.tx_amt1,temp); */
    temp=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt-dRest_amt-g_pub_tx.tx_amt1+mChnl_premise.premise_amt+dBal_virtul;/**temp����dBal_virtul**/
    vtcp_log("[%s][%d]sDd_mst.bal=%.2f sDd_mst.hold_amt=%.2f sDd_mst.ctl_amt=%.2f mChnl_premise.premise_amt=%.2f dRest_amt=%.2f g_pub_tx.tx_amt1=%.2f  temp=%.2f ",__FILE__,__LINE__,sDd_mst.bal,sDd_mst.hold_amt,sDd_mst.ctl_amt,mChnl_premise.premise_amt,dRest_amt,g_pub_tx.tx_amt1,temp);
    /* modify end by yanqq 20111213 */
    if(pub_base_CompDblVal(temp,0.00)>=0)
    {
        return (0);
    }
    else 
    {
	if(pub_base_CompDblVal(dRest_amt,0.00)>0) /*add by lwb 20150508*/
	{
        	sprintf(acErrMsg,"����ί��֧��δ��Ȩ  "); 
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
       		return 1;
		
	}
	else {
        vtcp_log("[%s][%d]����Ľ����Ŵ�����֧ȡ������,������ѱ��ۻ�",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"LS51");
        return 1;
	}
    }
}
int limit_draw_no_less(double *amt)
{
    int    ret;
    struct draw_loan_limit_c sDraw_limit;
    vtcp_log("[%s][%d]����limit_draw_no_less",__FILE__,__LINE__);
    ret=Draw_loan_limit_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and sts='2'",g_pub_tx.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit�α궨�����[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    (*amt)=0.0;
    while(1)
    {
        memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
        ret=Draw_loan_limit_Fet_Sel(&sDraw_limit,g_pub_tx.reply);
        if(ret==100)
        {
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]Draw_loan_limitȡ�α����[%d]",__FILE__,__LINE__,ret);
            strcpy(g_pub_tx.reply,"D102");
            goto ErrExit;
        }
        /* ��ʱֻ����ί��֧��,����������֧�� 2011-7-31 17:23:34
        (*amt)+=sDraw_limit.save_amt-sDraw_limit.trust_draw_amt-sDraw_limit.self_draw_amt;
        ***********/

        *amt += sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt;
    }
    Draw_loan_limit_Clo_Sel();
    vtcp_log("[%s][%d] �������� %.2f",__FILE__,__LINE__,*amt);
OkExit:
    return 0;
ErrExit:
    return 1;
}
int limit_draw_no_yes(double *amt,char *cDraw_no)
{
    int    ret;
    double approvel_amt;
    double dTmp1,dTmp2;
    struct draw_loan_limit_c sDraw_limit;
    struct draw_loan_hst_c   sDraw_hst;
    approvel_amt=0.0;
    vtcp_log("[%s][%d]����limit_draw_no_yes amt=%.2f",__FILE__,__LINE__,amt);
    memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
    memset(&sDraw_hst,0x00,sizeof(sDraw_hst));
    ret=limit_draw_no_less(&approvel_amt);
    if(ret)
    {  
        vtcp_log("[%s][%d] ret=%d approvel_amt=%.2f",__FILE__,__LINE__,ret,approvel_amt);
        strcpy(g_pub_tx.reply,"B384");
        goto ErrExit;
    }
    if(pub_base_CompDblVal(approvel_amt,0.0)<0)
    {
        vtcp_log("[%s][%d] approvel_amt=%.2f",__FILE__,__LINE__,amt);
        strcpy(g_pub_tx.reply,"B384");
        goto ErrExit;
    }

    /* by Wang Yongwei ����ֻ�ǿ���ί��֧������֧�����ڿ��� 20101215*/
    ret=Draw_loan_hst_Dec_Upd(g_pub_tx.reply,"draw_no='%s' and ac_id=%ld and sts='1' and st_flag='2'",cDraw_no,g_pub_tx.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_hst�����α����[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    ret=Draw_loan_hst_Fet_Upd(&sDraw_hst,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_hstȡ�α����",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    sDraw_hst.sts[0]='2';
    sDraw_hst.trace_no=g_pub_tx.trace_no;
    sDraw_hst.tx_date=g_pub_tx.tx_date; /* by Wang Yongwei 20110131 */
    strcpy(sDraw_hst.tel,g_pub_tx.tel);
    approvel_amt-=sDraw_hst.draw_amt;
    *amt=approvel_amt;
    vtcp_log("[%s][%d] ������֧ȡ����ŵ�����¿���֧ȡ�Ĵ������:  %.2f",__FILE__,__LINE__,*amt);
    ret=Draw_loan_hst_Upd_Upd(sDraw_hst,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_hst���´���[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D105");
        goto ErrExit;
    }
    Draw_loan_hst_Clo_Upd();

    pub_base_strpack(sDraw_hst.pact_no);

    ret=Draw_loan_limit_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and ac_id=%ld and sts='2'",sDraw_hst.pact_no,sDraw_hst.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit�����α����[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    ret=Draw_loan_limit_Fet_Upd(&sDraw_limit,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limitȡ�α����[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D102");
        goto ErrExit;
    }
    if(sDraw_hst.st_flag[0]=='1')
    {
        dTmp2=sDraw_limit.self_amt-sDraw_limit.self_draw_amt-sDraw_hst.draw_amt;
        if(pub_base_CompDblVal(dTmp2,0.0)<0)
        {
            vtcp_log("[%s][%d]����������֧�������ڿ�ȡ������֧�����",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"SN41");
            goto ErrExit;
        }
        sDraw_limit.self_draw_amt+=sDraw_hst.draw_amt;
    }
    else
    {
        dTmp1=sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt-sDraw_hst.draw_amt;
        if(pub_base_CompDblVal(dTmp1,0.0)<0)
        {
            vtcp_log("[%s][%d]������ί��֧�������ڿ�ȡ������ί��֧�����",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"SN41");
            goto ErrExit;
        }
        sDraw_limit.trust_draw_amt+=sDraw_hst.draw_amt;
    }
    dTmp1=sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt;
    dTmp2=sDraw_limit.self_amt-sDraw_limit.self_draw_amt;

    /* by Wang Yongwei ����ֻ�Ǽ��ί��֧���Ϳ����� 20101215
    if( pub_base_CompDblVal(dTmp1,0.0)==0 && pub_base_CompDblVal(dTmp2,0.0)==0 )
    {
        sDraw_limit.sts[0]='*';
    }
    **/
    if(pub_base_CompDblVal(dTmp1,0.0)==0 )
    {
        sDraw_limit.sts[0]='*';
        sDraw_limit.self_draw_amt=sDraw_limit.self_amt; 
    }

    ret=Draw_loan_limit_Upd_Upd(sDraw_limit,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit�������ݿ����[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D105");
        goto ErrExit;
    }
    Draw_loan_limit_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
/**�������˻�ac_id,�õ�7���������˻����.��δǩԼ,�����Ϊ0 add by zyl 20121211**/
int iGet7DaysVtlBal(long ac_id,double *bal)
{
    struct mo_prdt_prot_c sMo_prdt_prot;
    struct mo_prot_ac_rel_c sMo_prot_ac_rel;
    struct dd_mst_c sDd_mst_virtual;
    memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
    memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
    memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    int ret = 0;
    ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",ac_id);
    if(ret == 100)
    {
        *bal = 0.0;
    }
    else if(ret)
    {
        sprintf(acErrMsg,"Mo_prdt_prot_Sel error ret=[%d]",ret);
            WRITEMSG
        strcpy( g_pub_tx.reply,"D103");
        return 1;
    }
    else
    {        
        ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",ac_id);
        if(ret == 100)
        {
          /**������ǩԼ��Ϣ����.�������˻����չ�ϵ������.**/
            sprintf(acErrMsg,"������ǩԼ��Ϣ����.�������˻����չ�ϵ������.");
                WRITEMSG
            strcpy( g_pub_tx.reply,"D104");
            return 1;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error ret=[%d]",ret);
                WRITEMSG
            strcpy( g_pub_tx.reply,"D103");
            return 1;
        }
        
        /**2.�������˻���Ϣ..ǩԼ�˻�,�����˻���Ϣ��ȡ��.**/
        ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
        if(ret == 100)
        {
            sprintf(acErrMsg,"dd_mst���������˻���Ϣ.");
                WRITEMSG
            strcpy( g_pub_tx.reply,"D104");
            return 1;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"Dd_mst_Sel error ret=[%d]",ret);
                WRITEMSG
            strcpy( g_pub_tx.reply,"D103");
            return 1;
        }
        *bal = sDd_mst_virtual.bal;
        
    }
    return 0;
}
/*�����������˻�������Ϣ*/
int app_cal_intst(cSts, dd_mst_c, dd_parm_c)
char   *cSts;
struct dd_mst_c  dd_mst_c;
struct dd_parm_c dd_parm_c;
{
    vtcp_log("[%s][%d],ǩԼ״̬cSts=[%s]\n",__FILE__,__LINE__,cSts);
    int    ret=0;
    long   lDays=0;
    double dLx=0.00;       /*Ӧ����Ϣ*/
    double dLxacm=0.00;    /*��Ϣ����*/
    double dHq_rate=0.00; /*����*/
    double dLxs=0.00;      /*��Ϣ˰*/
    double dLxsl=0.00;     /*��Ϣ˰˰��*/
    double dSflx=0.00;     /*ʵ����Ϣ*/
    
    struct intst_hst_c T_intst_hst;
    struct dd_mst_c tmp_dd_mst;
    
    memset(&T_intst_hst, 0x00, sizeof(struct intst_hst_c));
    memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
    memset(&tmp_dd_mst, 0x00, sizeof(struct dd_mst_c));
    
    
    memcpy(&tmp_dd_mst, &dd_mst_c, sizeof(tmp_dd_mst));
    memcpy(&g_dd_parm, &dd_parm_c, sizeof(g_dd_parm));
    
    if(cSts[0]=='1')  /*״̬Ϊ1��ʱ��ſ�����ʱ��Լ*/
    {
        /*��ʱ��Լ��Ҫȡ��ǰ�Ļ�������*/
        ret = pub_base_getllz("100","01",g_pub_tx.tx_date,&dHq_rate);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�ӹ������ʱ���ȡ����ֵ���� [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W110");
            goto ErrExit;
        }
        vtcp_log("[%s][%d],��������dHq_rate=[%.2lf]\n",__FILE__,__LINE__,&dHq_rate);
        /**��������Ϣ**û��������������������Ϣ��add at 20120207****/
        if( pub_base_CalAcm(tmp_dd_mst.lst_date,g_pub_tx.tx_date, \
          g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type, \
          tmp_dd_mst.bal,g_pub_tx.tx_amt1,&tmp_dd_mst.intst_acm, \
          tmp_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
          {
              sprintf(acErrMsg,"������Ϣ��������!");
              WRITEMSG
              goto ErrExit;
          }
        /****end at 20120207****/
        
        vtcp_log("[%s][%d],cSts=[%s],dHq_rate=[%.2lf] ��Ϣ����=[%.2lf]\n",__FILE__,__LINE__,cSts,dHq_rate,tmp_dd_mst.intst_acm);
        dLx = (tmp_dd_mst.intst_acm * dHq_rate)/L360/L100;/**Ӧ����Ϣ**/
        vtcp_log("[%s][%d],dLx=[%.2lf]\n",__FILE__,__LINE__,dLx);
        dLx = pub_base_PubDround_1(dLx);
        /*�������Ϣ˰���ǰ�������Ϣ˰�ջ��ǰ����������ֲ�Ʒ����Ϣ˰����?*/
        if(strcmp(g_dd_parm.tax_no,SLBH) == 0)
        {
            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &dLxsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"��Ϣ˰˰�ʲ����� ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"W110");
                goto ErrExit;
            }
            dLxs=dLx*dLxsl;
            dLxs=pub_base_PubDround_1(dLxs) ;/**��Ϣ˰**/
            vtcp_log("[%s][%d]���ܼ���dLx=[%.2lf]dLxs=[%.2lf]dLxsl=[%.2lf]",__FILE__,__LINE__,dLx,dLxs,dLxsl);
            dSflx=dLx-dLxs;
        }
        g_pub_intst.dealval=dLx;
        g_pub_intst.val=dLxs;
        vtcp_log("[%s][%d] sts=[%s],dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);     
    }
    else
    {
       sprintf(acErrMsg, "���ʹ���!");
       WRITEMSG
       strcpy(g_pub_tx.reply, "C108");
       goto ErrExit;
    }
    vtcp_log("[%s][%d],׼���Ǽǽ�Ϣ�Ǽǲ�\n",__FILE__,__LINE__);
    strcpy(T_intst_hst.br_no, tmp_dd_mst.opn_br_no);
    T_intst_hst.ac_id = tmp_dd_mst.ac_id;
    T_intst_hst.ac_seqn = tmp_dd_mst.ac_seqn;
    strcpy(T_intst_hst.prdt_no, tmp_dd_mst.prdt_no);
    strcpy(T_intst_hst.cur_no, g_dd_parm.cur_no);
    T_intst_hst.bic_bal = tmp_dd_mst.bal;
    T_intst_hst.bic_acm = tmp_dd_mst.intst_acm;
    T_intst_hst.fct_intst = g_pub_intst.dealval;
    T_intst_hst.intst_tax = g_pub_intst.val;
    T_intst_hst.aic_bal = tmp_dd_mst.bal;
    T_intst_hst.aic_acm = 0.00;
    T_intst_hst.ic_date = tmp_dd_mst.ic_date;    
    T_intst_hst.intst_date = g_pub_tx.tx_date;
    strcpy(T_intst_hst.ac_ind, "1");
    strcpy(T_intst_hst.pt_ind, "1");
    T_intst_hst.rate = dHq_rate;
    T_intst_hst.intst_date=g_pub_tx.tx_date;
    ret = Intst_hst_Ins(T_intst_hst, g_pub_tx.reply);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    /*���»������ļ�*/
    ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%d and ac_seqn=%d",tmp_dd_mst.ac_id, tmp_dd_mst.ac_seqn);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    ret = Dd_mst_Fet_Upd(&tmp_dd_mst, g_pub_tx.reply);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    tmp_dd_mst.intst_acm = 0.00;/**Ӧ����Ϣ����**/
    tmp_dd_mst.lst_date=g_pub_tx.tx_date;
    tmp_dd_mst.ic_date=g_pub_tx.tx_date;
    
    ret = Dd_mst_Upd_Upd(tmp_dd_mst, g_pub_tx.reply);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    vtcp_log("[%s][%d],���»����������\n",__FILE__,__LINE__);
    Dd_mst_Clo_Upd();
    return 0;
ErrExit:
    return 1;
} 
/*��������ʱ��Լ--��Ϣ����*/
   int main_intst_acct(dd_mst_c, dd_parm_c, sum_intst)
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
    struct S_pub_intst sum_intst;
    {
        int  ret = 0;
        char tmp_ac_no[25];
        struct dd_mst_c v_dd_mst;
        struct dd_parm_c v_dd_parm;
        
        memset(&v_dd_mst,0x00,sizeof(struct dd_mst_c));
        memset(&v_dd_parm,0x00,sizeof(struct dd_parm_c));
        memset(tmp_ac_no,0x00,sizeof(tmp_ac_no));
        
        memcpy(&v_dd_mst, &dd_mst_c, sizeof(v_dd_mst));
        memcpy(&v_dd_parm, &dd_parm_c, sizeof(v_dd_parm));
        
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        /****����Ҫ����ֵ 20130220
        strcpy(g_pub_tx.tx_br_no, v_dd_mst.opn_br_no);
        ****/
        strcpy(g_pub_tx.opn_br_no, v_dd_mst.opn_br_no);
        strcpy(g_pub_tx.cur_no, v_dd_parm.cur_no);
        strcpy(g_pub_tx.ac_wrk_ind, "0000010011");
        g_pub_tx.svc_ind = 1005;
        strcpy(g_pub_tx.ct_ind, "2");
        strcpy(g_pub_tx.add_ind, "1");
        strcpy(g_pub_tx.prdt_code, v_dd_parm.prdt_no);
        g_pub_tx.ac_id = v_dd_mst.ac_id;
        g_pub_tx.ac_seqn = v_dd_mst.ac_seqn;
        strcpy(g_pub_tx.note_type, "");
        strcpy(g_pub_tx.beg_note_no, "");
        g_pub_tx.tx_amt1 = sum_intst.dealval;
        strcpy(g_pub_tx.brf, "����֪ͨ�����Ϣ");
        /* ������Ϣ�Ľ��׼��ʴ��� */
        vtcp_log("[%s][%d] v_dd_mst�ʻ�ID=[%d] ",__FILE__,__LINE__,v_dd_mst.ac_id);
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        ret = pub_acct_trance();
        if (ret)
        {
            sprintf(acErrMsg, "���˴���!!");
            WRITEMSG
            goto ErrExit1;
        }    
        /* ������Ϣ˰�Ľ��׼��ʴ��� */
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        strcpy(g_pub_tx.brf, "����֪ͨ�����Ϣ");
        strcpy(g_pub_tx.no_show, "0");
        strcpy(g_pub_tx.add_ind, "0");
        strcpy(g_pub_tx.ac_wrk_ind, "0000010011");
        strcpy(g_pub_tx.hst_ind, "1");
        g_pub_tx.tx_amt1 = sum_intst.val;
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        ret = pub_acct_trance();
        if (ret)
        {
            sprintf(acErrMsg, "���˴���!!");
            WRITEMSG
            goto ErrExit1;
        }   
        /* ���л�Ƽ��� */
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        set_zd_data(DC_CODE, v_dd_parm.dc_code);
        set_zd_double("102P", sum_intst.dealval-sum_intst.val);
        set_zd_double("102Q", sum_intst.val);
        set_zd_data("102J", v_dd_parm.cur_no);
        set_zd_data("102K", g_pub_tx.ct_ind);/****����ת�ʱ�־****/
        strcpy(g_pub_tx.prdt_code, v_dd_mst.prdt_no);
        /****�ô���������Ϣ�ӽ��״���****/
        strcpy(g_pub_tx.sub_tx_code,"D202");
        vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg, "��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit1;
        }
        
        return 0;
    ErrExit1:
        return 1;
    }
