/****************************************************
*��������:gD322.c
*��������:ʵ������֪ͨ����Ʒ�е�����ʱ��Լ�͵�����������ĩ��Ϣ�ͻ�ת���������£�
*         (1)����״̬Ϊ��ʱ��Լ�ģ�
*         a�����Ϊ���ܽ�Լ�ģ�����������Զ������Ϣ����Ϣ���������ʼ��㣬����
*            ��Ϣ��ת�����ý��ʻ���
*         b�����Ϊ��ǰ��ʱ��Լ�ģ��������������ʻ���û�л�����������
*         (2)����״̬Ϊ�����ģ�
*         a������ǩԼ�ģ������ʻ���û����������
*         b����ǰǩԼ�ģ�������Ϣ������������֪ͨ�������ʼ�����Ϣ��������Ϣ��ת�����ý��ʻ��ϣ� 
*         c. ����ǩԼ..ǩԼ��,Ǯ�����ڿɻ�ת���..δ��ת..���ܽ�Ϣʱ,������������ȷ��.
*��    ��:
*���ʱ��:
*�޸ļ�¼:
*    1���޸�ʱ��
*    2���޸���
*    3���޸�����
*<EG321  DESC="���ղ�Ϊ����!"/>
*<EG322  DESC="����֪ͨ�����ĩ������Ϣʧ��!" />
*
insert into com_eod_exec values( '846','D322','gD322', '����֪ͨ����Ϣ����','1111111111','0','0','00','00','0','Y','Y','N','2','Y');
******************************************************/
#define MYSQLERR if( ret ) {\
    strcpy( g_pub_tx.reply,"G322" );\
    sprintf(acErrMsg,"SQLERR [%d]",ret);\
    WRITEMSG\
    goto ErrExit;\
}
#define MYERR if( ret ) {\
    sprintf(acErrMsg,"SQLERR [%d]",ret);\
    WRITEMSG\
    goto ErrExit;\
}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "intst_hst_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_parm_c.h"
#include "dd_mst_c.h"
#include "prdt_ac_id_c.h"

static  double  dQt_rate=0.00;
static  double  dHq_rate=0.00;

gD322()
{
	  int  ret = 0;
    int  iRet = 0;
    int  iSunday = 0;
    
    struct mo_prot_ac_rel_c sMo_prot_ac_rel;
    struct mo_prdt_prot_c sMo_prdt_prot;
    struct com_sys_parm_c sCom_sys_parm;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct S_pub_intst sum_intst;
    struct dd_parm_c sApp_dd_parm,sMain_dd_parm;
    struct dd_mst_c sApp_dd_mst,sMain_dd_mst;
        
    memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
    memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sum_intst, 0x00, sizeof(sum_intst));
    memset(&sApp_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&sMain_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&sApp_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
    
    /* ������ */
    iRet = sql_begin();  
    if (iRet != 0)
    {
        sprintf(acErrMsg, "������ʧ��!!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();
    
    iRet = Com_sys_parm_Sel(g_pub_tx.reply, &sCom_sys_parm, "1=1");
    if (iRet) 
    {
        sprintf(acErrMsg, "ȡϵͳ���ڴ���![%d]", iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"CUD2");
        goto ErrExit;
    }
    g_pub_tx.tx_date = sCom_sys_parm.sys_date;
    
    /*������ڵĺϷ���*/
    iRet=pub_base_dayvalicheck(g_pub_tx.tx_date);
    if(iRet)
    {
        sprintf(acErrMsg, "ϵͳ���ڲ��Ϸ�![%d]", iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"CUD2");
        goto ErrExit;
    }
    
    /*��鵱���Ƿ�������*/
    iSunday=pub_base_day_week(g_pub_tx.tx_date);
    vtcp_log("zylsee11122 date=[%ld]",g_pub_tx.tx_date);
    vtcp_log("zylsee11122 dat=[%d]",iSunday);
    /**����ʱ pub_base_day_week����99 by zyl 20110511**/
    if(iSunday != 99)
    {
        sprintf(acErrMsg, "���첻Ϊ������,����������Ϣ![%d] [%d]",__LINE__, iSunday);
        WRITEMSG
        goto OkExit;
    }
    /*��ʱ��Լ��Ҫȡ��ǰ�Ļ�������*/
    iRet = pub_base_getllz("100","01",g_pub_tx.tx_date,&dHq_rate);
    if (iRet != 0)
    {
        sprintf(acErrMsg,"�ӹ������ʱ���ȡ����ֵ���� [%d]",iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W110");
        goto ErrExit;
    }
    vtcp_log("[%s][%d],dHq_rate=[%.2lf]\n",__FILE__,__LINE__,dHq_rate);
    /*��ʽǩԼ��ȡ����֪ͨ������� ����֪ͨ������� B00*/
    iRet = pub_base_getllz("B00","01",g_pub_tx.tx_date,&dQt_rate);
    if (iRet != 0)
    {
        sprintf(acErrMsg,"�ӹ������ʱ���ȡ����ֵ���� [%d]",iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W110");
        goto ErrExit;
    }
    vtcp_log("[%s][%d],dQt_rate=[%.2lf]\n",__FILE__,__LINE__,dQt_rate);
    /*ȡ������ˮ��*/
    if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply))
    {
        sprintf(acErrMsg, "ȡ������ˮ�ų���! trace_no=[%ld]",g_pub_tx.trace_no);
        WRITEMSG
        goto ErrExit;
    }
    iRet = Mo_prdt_prot_Dec_Sel1( g_pub_tx.reply , "  prot_type='TK7' and sts in('1','2') order by sts,main_ac_id" );
    if (iRet)
    {
        sprintf(acErrMsg, "�趨mo_prdt_prot�α����![%d] [%d]",__LINE__,iRet);
        WRITEMSG
        goto ErrExit;
    }
    
    while(1)
    {
    	  strcpy(g_pub_tx.ac_no,"");
        memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
        iRet=Mo_prdt_prot_Fet_Sel1( &sMo_prdt_prot , g_pub_tx.reply );
        if(iRet==100)
        {
            break;
        }
        else if (iRet)
        {
            sprintf(acErrMsg, "��ȡmo_prdt_prot�α����!");
            WRITEMSG
            goto ErrExit;
        }
        /*��ѯ����Э���˻����ձ�*/
        memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
        iRet=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and main_ac_id=%ld and main_ac_seqn=%d and sts in('1','2')",sMo_prdt_prot.main_ac_id,sMo_prdt_prot.main_ac_seqn);
        if(iRet)
        {
            sprintf(acErrMsg, "��ȡ����Э���ʻ����ձ����!iRet=[%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        /*��ѯ��������*/
        memset(&sApp_dd_mst,0x00,sizeof(struct dd_mst_c));
        iRet=Dd_mst_Sel(g_pub_tx.reply,&sApp_dd_mst," ac_id=%ld ",sMo_prot_ac_rel.app_ac_id);
        if(iRet)
        {
            sprintf(acErrMsg, "��ȡ�������ļ�����!iRet=[%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        /*��ѯ���ڲ�Ʒ��*/
        memset(&sApp_dd_parm,0x00,sizeof(struct dd_parm_c));
        iRet = Dd_parm_Sel(g_pub_tx.reply, &sApp_dd_parm, "prdt_no='%s'",sApp_dd_mst.prdt_no);
        if (iRet)
        {
            sprintf(acErrMsg, "��ѯ���ڲ�Ʒ�����iRet=[%d]prdt_no=[%s]!",iRet,sApp_dd_mst.prdt_no);
            WRITEMSG
            goto ErrExit;
        }
        /*���������ʻ�����Ϣ*/
        memset(&sum_intst, 0x00, sizeof(sum_intst));
        iRet=app_cal_intst(sMo_prdt_prot.sts,sApp_dd_mst,sApp_dd_parm);
        if (iRet)
        {
            sprintf(acErrMsg, "���������ʻ���Ϣ����ret=[%d]!!!", iRet);
            WRITEMSG
            goto ErrExit;
        }
        sum_intst.dealval=g_pub_intst.dealval;
        sum_intst.val=g_pub_intst.val;
        
        vtcp_log("[%s][%d]g_pub_intst.dealval=[%.2lf],g_pub_intst.val=[%.2lf]",__FILE__,__LINE__,g_pub_intst.dealval,g_pub_intst.val);
        vtcp_log("[%s][%d]  sum_intst.dealval=[%.2lf],  sum_intst.val=[%.2lf]",__FILE__,__LINE__,sum_intst.dealval,sum_intst.val);
        
        memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
        iRet=Dd_mst_Sel(g_pub_tx.reply,&sMain_dd_mst," ac_id=%ld ",sMo_prot_ac_rel.main_ac_id);
        if(iRet)
        {
            sprintf(acErrMsg, "��ȡ�������ļ�����!iRet=[%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        /*��ѯ���ڲ�Ʒ��*/
        memset(&sMain_dd_parm ,0x00,sizeof(struct dd_parm_c));
        iRet = Dd_parm_Sel(g_pub_tx.reply, &sMain_dd_parm, "prdt_no='%s'",sMain_dd_mst.prdt_no);
        if (iRet)
        {
            sprintf(acErrMsg, "��ѯ���ڲ�Ʒ�����iRet=[%d]prdt_no=[%s]!!!",iRet,sApp_dd_mst.prdt_no);
            WRITEMSG
            goto ErrExit;
        }
        /****/
        vtcp_log("[%s][%d] see143 ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
        vtcp_log("[%s][%d] see143 ac_id=[%ld]",__FILE__,__LINE__,g_pub_tx.ac_id);
        vtcp_log("[%s][%d] see143 sMo_prdt_prot.sts=[%s]",__FILE__,__LINE__,sMo_prdt_prot.sts);
        iRet=main_intst_acct(sMain_dd_mst,sMain_dd_parm,sum_intst,sMo_prdt_prot.sts);
        if (iRet)
        {
            sprintf(acErrMsg, "���˺ż��˴���ʧ��iRet=[%d]!",iRet);
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("One of the fet end.");
        
        /**add by zyl 20110516 ��Ϣ��,�����⻧��Ǯ�������˻�..�Ա����ƺ�,���¿�ʼ����֪ͨ��ת.**/
        /**1.����֪ͨ��������˻�ȡ��.**/
        ret=acct_Z016(sApp_dd_mst.ac_id,1,1001,"0",sApp_dd_mst.bal,"LC03");
        if(ret)
        {
            sprintf(acErrMsg,"����֪ͨ��������˻�ȡ�� ����. [%ld][%d]",
            sApp_dd_mst.ac_id,ret);
            WRITEMSG
            goto ErrExit;
        }
        /**2.����֪ͨ������˻����.**/
        ret=acct_Z016(sMain_dd_mst.ac_id,1,1001,"1",sApp_dd_mst.bal,"LC02");
        if(ret)
        {
            sprintf(acErrMsg,"����֪ͨ������˻��������. [%ld][%d]",
            sMain_dd_mst.ac_id,ret);
            WRITEMSG
            goto ErrExit;
        }
        /**end by zyl 20110516**/

    }
    Mo_prdt_prot_Clo_Sel();
    vtcp_log("vvvvvvvvvvvvvvvvvvv[%d]",g_reply_int);
    /***zgf add 20130725**/  
    pub_mob_sendmail(  g_pub_tx.brf, g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, sApp_dd_mst.bal );
    /***zgf add end**/   
        
OkExit:
    sql_commit();        /*--�ύ����--*/
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "����֪ͨ������ռ�Ϣ����ɹ�!!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0')
    {
        strcpy(g_pub_tx.reply, "G322");
    }
    sql_rollback();        /*--����ع�--*/
    sprintf(acErrMsg, "����֪ͨ������ռ�Ϣ����ʧ��!!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}



int app_cal_intst(cSts, dd_mst_c, dd_parm_c)
    char   *cSts;
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
{
    vtcp_log("[%s][%d],ǩԼ״̬cSts=[%s]\n",__FILE__,__LINE__,cSts);
    int    ret=0;
    long   lDays=0;
    double dLx=0.00;       /*Ӧ����Ϣ*/
    double dLxacm=0.00;    /*��Ϣ����*/
    double dLxs=0.00;      /*��Ϣ˰*/
    double dLxsl=0.00;     /*��Ϣ˰˰��*/
    double dSflx=0.00;     /*ʵ����Ϣ*/
    
    struct intst_hst_c T_intst_hst;
    struct dd_mst_c tmp_dd_mst;
    
    memset(&T_intst_hst, 0x00, sizeof(struct intst_hst_c));
    memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
    
    memcpy(&g_dd_mst, &dd_mst_c, sizeof(g_dd_mst));
    memcpy(&g_dd_parm, &dd_parm_c, sizeof(g_dd_parm));
    
    /*���»������ļ�*/
    ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%d and ac_seqn=%d",g_dd_mst.ac_id, g_dd_mst.ac_seqn);
    MYERR
    
    if(cSts[0]=='2')   /*��ʱ��Լ*/
    {        
        vtcp_log("[%s][%d],cSts=[%s],dHq_rate=[%.2lf]\n",__FILE__,__LINE__,cSts,dHq_rate);
        dLx = (g_dd_mst.intst_acm * dHq_rate)/L360/L100;/**Ӧ����Ϣ**/
        vtcp_log("[%s][%d],dLx=[%.2lf]\n",__FILE__,__LINE__,dLx);
        dLx = pub_base_PubDround_1(dLx);
        /*�������Ϣ˰���ǰ�������Ϣ˰�ջ��ǰ�����֪ͨ������ֲ�Ʒ����Ϣ˰����*/
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
        T_intst_hst.rate = dHq_rate;
        vtcp_log("[%s][%d] sts=[%s],dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);
    }
    else if(cSts[0]=='1')  /*��ʽǩԼ*/
    {
        vtcp_log("[%s][%d],dQt_rate=[%.2lf]\n",__FILE__,__LINE__,dQt_rate);        
        /**��������Ϣ**/
        vtcp_log("zylsee145 g_dd_mst.lst_date=[%ld]",g_dd_mst.lst_date);
        vtcp_log("zylsee145 g_pub_tx.tx_date=[%ld]",g_pub_tx.tx_date);
        ret=pub_base_CalIntstDays(g_dd_mst.lst_date,g_pub_tx.tx_date,"0",&lDays);
        lDays++;
        lDays = 7;
        if (ret !=0)
        {
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
            goto ErrExit;
        }
        if (lDays!=0 && lDays % 7 == 0) 
        {
            dLx = (lDays * dQt_rate * g_dd_mst.bal)/L360/L100;/**Ӧ����Ϣ**/
            dLx = pub_base_PubDround_1(dLx);
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
                vtcp_log("[%s][%d],dLx=[%.2lf],dLxs=[%.2lf],dLxsl=[%.2lf]",__FILE__,__LINE__,dLx,dLxs,dLxsl);
                dSflx=dLx-dLxs;
            }
            g_pub_intst.dealval=dLx;
            g_pub_intst.val=dLxs;
            g_dd_mst.intst_acm=g_dd_mst.bal * lDays;  /*��Ϣ����*/
            T_intst_hst.rate = dQt_rate;
            vtcp_log("[%s][%d] sts=[%s]����dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);
        }
        else if( lDays < 7)
        {
            vtcp_log("[%s][%d] lDays=[%ld] ǩԼ�����ڱ����Ҳ�������",__FILE__,__LINE__,lDays);
        }
        else
        {
        	  if(pub_base_CompDblVal(g_dd_mst.bal,0.00) == 0)
        	  {
        	      vtcp_log("[%s][%d] lDays=[%ld] ǩԼ����������,��������,δ����.",__FILE__,__LINE__,lDays);
        	  }
        	  else
        	  {
                sprintf(acErrMsg,"����֪ͨ�����������! lDays=[%ld]",lDays);
                WRITEMSG
                goto ErrExit;
            }
        }
    }
    vtcp_log("[%s][%d],׼���Ǽǽ�Ϣ�Ǽǲ�\n",__FILE__,__LINE__);
    strcpy(T_intst_hst.br_no, g_dd_mst.opn_br_no);
    T_intst_hst.ac_id = g_dd_mst.ac_id;
    T_intst_hst.ac_seqn = g_dd_mst.ac_seqn;
    strcpy(T_intst_hst.prdt_no, g_dd_mst.prdt_no);
    strcpy(T_intst_hst.cur_no, g_dd_parm.cur_no);
    T_intst_hst.bic_bal = g_dd_mst.bal;
    T_intst_hst.bic_acm = g_dd_mst.intst_acm;
    T_intst_hst.fct_intst = g_pub_intst.dealval;
    T_intst_hst.intst_tax = g_pub_intst.val;
    T_intst_hst.aic_bal = g_dd_mst.bal;
    T_intst_hst.aic_acm = 0.00;
    T_intst_hst.ic_date = g_dd_mst.ic_date;    
    T_intst_hst.intst_date = g_pub_tx.tx_date;
    strcpy(T_intst_hst.ac_ind, "1");
    strcpy(T_intst_hst.pt_ind, "1");
    T_intst_hst.intst_date=g_pub_tx.tx_date;
    ret = Intst_hst_Ins(T_intst_hst, g_pub_tx.reply);
    MYERR
    
    ret = Dd_mst_Fet_Upd(&tmp_dd_mst, g_pub_tx.reply);
    MYERR
    g_dd_mst.intst_acm = 0.00;/**Ӧ����Ϣ����**/
    g_dd_mst.lst_date=g_pub_tx.tx_date;
    g_dd_mst.ic_date=g_pub_tx.tx_date;
    ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply);
    MYERR
    vtcp_log("[%s][%d],���»����������\n",__FILE__,__LINE__);
    Dd_mst_Clo_Upd();
    
    

    return 0;
ErrExit:
    return 1;
}

int main_intst_acct(dd_mst_c, dd_parm_c, sum_intst,cSts)
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
    struct S_pub_intst sum_intst;
    char   *cSts;
{
    int  ret = 0;
    /**add by zyl 20110511**/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id = %ld ",dd_mst_c.ac_id);
    if(ret)
    {
        sprintf(acErrMsg, "Mdm_ac_rel_Sel error! ret=[%ld]",ret);
        WRITEMSG
        goto ErrExit;
    }
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",dd_mst_c.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",
				g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		strcpy(g_pub_tx.prdt_code,sPrdt_ac_id.prdt_no);
		strcpy(g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type);
		vtcp_log("zylsee134  ac_id_type=[%s]",sPrdt_ac_id.ac_id_type);
    /**end by zyl 20110511**/    
    strcpy(g_pub_tx.tx_br_no, dd_mst_c.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, dd_mst_c.opn_br_no);
    strcpy(g_pub_tx.cur_no, dd_parm_c.cur_no);
    strcpy(g_pub_tx.ac_wrk_ind, "0000000");
    g_pub_tx.svc_ind = 1005;
    strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.add_ind, "1");
    strcpy(g_pub_tx.prdt_code, dd_parm_c.prdt_no);
    g_pub_tx.ac_id = dd_mst_c.ac_id;
    vtcp_log("zylsee12331 g_pub_tx.ac_id=[%ld]",g_pub_tx.ac_id);    
    g_pub_tx.ac_seqn = sMdm_ac_rel.ac_seqn;/**mod by zyl 20110512**/
    strcpy(g_pub_tx.note_type, "");
    strcpy(g_pub_tx.beg_note_no, "");
    g_pub_tx.tx_amt1 = sum_intst.dealval;
    strcpy(g_pub_tx.brf, "����֪ͨ����Ϣ");
    strcpy(g_pub_tx.hst_ind, "1"); /*��һ����ϸ20121210 by jk*/
    /* ������Ϣ�Ľ��׼��ʴ��� */
    ret = pub_acct_trance();
    if (ret)
    {
        sprintf(acErrMsg, "���˴���!!");
        WRITEMSG
        goto ErrExit;
    }    
    /* ������Ϣ˰�Ľ��׼��ʴ��� */
    strcpy(g_pub_tx.brf, "��Ϣ˰");
    strcpy(g_pub_tx.no_show, "0");
    strcpy(g_pub_tx.add_ind, "0");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000");
    strcpy(g_pub_tx.hst_ind, "1");
    g_pub_tx.tx_amt1 = sum_intst.val;
    ret = pub_acct_trance();
    if (ret)
    {
        sprintf(acErrMsg, "���˴���!!");
        WRITEMSG
        goto ErrExit;
    }   
    /* ���л�Ƽ��� */
    set_zd_data(DC_CODE, dd_parm_c.dc_code);
    set_zd_double("102P", sum_intst.dealval-sum_intst.val);
    set_zd_double("102Q", sum_intst.val);
    set_zd_data("102J", dd_parm_c.cur_no);
    strcpy(g_pub_tx.prdt_code, dd_mst_c.prdt_no);
    /**mod by zyl 20110522 todo test**/
    if(cSts[0]=='1')
    {
    	strcpy(g_pub_tx.sub_tx_code,"Z027");/**ǩԼ״̬.**/
    }
    else if(cSts[0]=='2')
    {
    	strcpy(g_pub_tx.sub_tx_code,"Z032");/**��ʱ��Լ״̬.**/
    }
    else
    	goto ErrExit;
    /**mod end by zyl 20110522**/
    ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
    if (ret != 0)
    {
        sprintf(acErrMsg, "��Ƽ��ʲ��ɹ�!!");
        WRITEMSG
        goto ErrExit;
    }
    /****/
    char cTmpbuf[256];
    memset(cTmpbuf,0x00,sizeof(cTmpbuf));
    get_zd_data("0152",cTmpbuf);
    vtcp_log("zylsee109 0152=[%s]",cTmpbuf);
    memset(cTmpbuf,0x00,sizeof(cTmpbuf));
    get_zd_data("0153",cTmpbuf);
    vtcp_log("zylsee109 0153=[%s]",cTmpbuf);
    /****/
    return 0;
ErrExit:
    return 1;
}


int acct_Z016(long ac_id,int ac_seqn,long deal_code,char *add_ind,double amt,char *sub_tx_code)
{
	strcpy(g_pub_tx.ac_no,"");
	g_pub_tx.ac_id=ac_id;
	g_pub_tx.ac_seqn=ac_seqn;
	g_pub_tx.tx_amt1=amt;
	strcpy(g_pub_tx.add_ind,add_ind); 
	strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
	g_pub_tx.svc_ind=deal_code; /**�������***/
	strcpy(g_pub_tx.ct_ind,"2");	/*** ת�� ***/
	strcpy(g_pub_tx.ac_id_type,"");
	strcpy(g_pub_tx.note_type,"");
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.brf, "����֪ͨ����");
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		return 1;
	}
	set_zd_data("102K",g_pub_tx.ct_ind);
	/***�޸�Ϊʹ��100���2���� modify at 20130517
	set_zd_double("102I",amt);
	****/
	set_zd_double("1002",amt);
	if ( pubf_acct_proc(sub_tx_code) )
	{
		sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
		WRITEMSG
		return 1;
	}
	return 0;	
}
