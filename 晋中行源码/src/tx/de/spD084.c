/***************************************************************
* �� �� ��: spD084.c
* ��������������ά����Ʋ�ƷǩԼ�˺�
            ��ȷ���˻���Ϊ�������˻�����
* ��    ��: shangyongchao
* ������ڣ�2013/5/20 15:54:58
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "mdm_attr_c.h"
int spD084()
{
    int iRet = 0;
    int iCnt  = 0;/*������*/
    int iLen = 0;
    int iAc_seqn_new = 0;
    int iAc_seqn_old =0;
    char cAc_no_old[25];/*ԭ�˺�*/
    char cAc_no_new[25];/*���˺�*/
    char cPrdt_code[21];/*��Ʒ����*/
    char cAgent_id_type[2];/*������֤������*/
    char cAgent_id[21];/*������֤������*/
    char cAgent_name[61];/*����������*/
    char cTx_type[2];/*0ֻ�����������Ĳ�Ʒ�����Ӧ���˺ţ�1���˺ŵĶ�����*/
    char cAc_no_old_tmp[5];
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct mdm_ac_rel_c sMdm_ac_rel_old;
    struct dd_mst_c sDd_mst;
    struct fn_reg_hst_c sFn_reg_hst;
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    
    memset(cAc_no_old_tmp,0x00,sizeof(cAc_no_old_tmp));
    memset(cAc_no_old,0x00,sizeof(cAc_no_old));
    memset(cAc_no_new,0x00,sizeof(cAc_no_new));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cTx_type,0x00,sizeof(cTx_type));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sMdm_ac_rel_old,0x00,sizeof(sMdm_ac_rel_old));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));

    pub_base_sysinit();

    get_zd_data("0300",cAc_no_old);
    pub_base_old_acno(cAc_no_old);
    get_zd_data("0310",cAc_no_new);
    pub_base_old_acno(cAc_no_new);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0660",cTx_type);
    get_zd_int("0480",&iAc_seqn_old);
    get_zd_int("0490",&iAc_seqn_new);
    get_zd_data("0690",cAgent_id_type);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0250",cAgent_name);
    iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no_new);
    if( iRet==100 )
    {
        vtcp_log("���˺Ų�����,����! ");
        strcpy(g_pub_tx.reply,"C115");
        goto ErrExit;
    }
    else if( iRet )
    {
        vtcp_log(" �������α����,�������Ļ�����ϵ ",iRet);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    if(sMdm_ac_rel.note_sts[0] != '0')
    {
        vtcp_log("���˺ŵĽ���״̬������! note_sts[%s] ",sMdm_ac_rel.note_sts);
        strcpy(g_pub_tx.reply,"D140");
        goto ErrExit;
    }
    iRet=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn = %ld",sMdm_ac_rel.ac_id,iAc_seqn_new);
    if( iRet==100 )
    {
        vtcp_log(" fatal error! table no record ");
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    else if( iRet )
    {
        vtcp_log("database error! ret[%d] ",iRet);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_log(" ���˺ŵ��˻�״̬������! ac_sts[%s] ",sDd_mst.ac_sts);
        strcpy(g_pub_tx.reply,"O085");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' )
            
    {
        vtcp_log("���˻���ȫ�������� [%s] ",sDd_mst.hold_sts);
        strcpy(g_pub_tx.reply,"KH01");
        goto ErrExit;
    }
    /*��ѯ�¾��˺�*/
    iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_old,"ac_no='%s'",cAc_no_old);
    if( iRet==100 )
    {
        vtcp_log("fatal error! table no record ");
        strcpy(g_pub_tx.reply,"P088");
        goto ErrExit;
    }
    else if( iRet )
    {
        vtcp_log(" database error! ret[%d] ",iRet);
        strcpy(g_pub_tx.reply,"P084");
        goto ErrExit;
    }
    if(strcmp(sMdm_ac_rel_old.id_no,sMdm_ac_rel.id_no)) 
    {  vtcp_log(" ����������˻���ͬһ�������˺� ");
    	 vtcp_log("old.id_no= [%s] new.id_no=[%s]  ",sMdm_ac_rel_old.id_no,sMdm_ac_rel.id_no);
        strcpy(g_pub_tx.reply,"S093");
        goto ErrExit;
    	}
    /*���ݲ�ͬ�Ĳ�����־ ���α�*/
    if(cTx_type[0] == '0')/*ֻ����һ����Ʒ���տ��˺�*/
    {
        iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
        if(iRet == 100)
        {
            vtcp_log("û�������Ʋ�Ʒ! prdt_code[%s] ",cPrdt_code);
            strcpy(g_pub_tx.reply,"D104");
            goto ErrExit;
        }
        else if(iRet)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ���������! prdt_code[%s] iRet [%d] ",cPrdt_code,iRet);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        /*���ֲ�ͬ�Ĳ��ܸ���*/
        /*if(strcmp(sDd_mst.cur_no,sFn_parm.cur_no))
        {
            vtcp_log("���ܸ���Ϊ�������ֵ��˻�parm.cur_no[%s] dd.cur_no[%s]",sFn_parm.cur_no,sDd_mst.cur_no);
            strcpy(g_pub_tx.reply,"D337");
            goto ErrExit;
        }*/
        /*ֻ�����Ϲ����ѿۿ�(���ŷ�������)���깺��*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code ='%s' and ac_id=%ld and ac_seqn=%d and sts in('0','1','4')",cPrdt_code,sMdm_ac_rel_old.ac_id,iAc_seqn_old);
        if(iRet)
        {
            vtcp_log("���������α����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333");
            goto ErrExit;
        }
        while(1)
        {
            memset(&sFn_reg,0x00,sizeof(sFn_reg));
            memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            if(iRet != 0 && iRet != 100)
            {
                vtcp_log(" ȡ�����α����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            else if(iRet == 100)
            {
                if(iCnt  == 0)
                {
                    vtcp_log("����˻�û���Ϲ�����Ʋ�Ʒ iRet[%d] ",iRet);
                    strcpy(g_pub_tx.reply,"D104");
                    goto ErrExit;
                }
                break;
            }
            strcpy(sFn_reg.filler,"");
            sprintf(sFn_reg.filler,"%s|%d",sFn_reg.ac_no,sFn_reg.ac_seqn);
            strcpy(sFn_reg.ac_no,cAc_no_new);
            sFn_reg.ac_id  = sDd_mst.ac_id;
            sFn_reg.ac_seqn  = sDd_mst.ac_seqn;
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("����fn_reg����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D332");
                goto ErrExit;
            }
            /*������Ϲ��ģ����뽫���˺Ž�����ס��ͬʱ��ԭ�˺ſ��ƽ���� Ϊʲô����������*/
            if(sFn_reg.sts[0] == '0')
            {
                /*�����Ϲ��˻��Ŀ��ƽ��*/
                iRet  = upd_dd_mst(0,sMdm_ac_rel_old.ac_id,iAc_seqn_old,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("���»��ڴ�����ļ����� iRet[%d] ",iRet);
                    goto ErrExit;
                }
                /*�����滻�˺ŵĿ��ƽ��*/
                iRet  = upd_dd_mst(1,sFn_reg.ac_id,sFn_reg.ac_seqn,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("���»��ڴ�����ļ����� iRet[%d] ",iRet);
                    goto ErrExit;
                }
            }
            iCnt ++;
            /*�Ǽ���ϸ*/
            sFn_reg_hst.tx_date = g_pub_tx.tx_date;
            sFn_reg_hst.trace_no = g_pub_tx.trace_no;
            strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
            strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
            strcpy(sFn_reg_hst.type,"7");
            sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
            strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
            strcpy(sFn_reg_hst.agent_id,cAgent_id);
            strcpy(sFn_reg_hst.agent_name,cAgent_name);
            sFn_reg_hst.buy_date=sFn_reg.tx_date;
            sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
            strcpy(sFn_reg_hst.brf,"����ǩԼ�˺�");
            sprintf(sFn_reg_hst.filler,"%s|%d|%s|%d",cAc_no_old,iAc_seqn_old,cAc_no_new,iAc_seqn_new);
            iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
            if(iRet)
            {
                Fn_reg_hst_Debug(&sFn_reg_hst);
                vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }

       }
       Fn_reg_Clo_Upd();
    }
    else if(cTx_type[0] == '1')
    {
        /*����˺� �Ϲ��� �Ѿ��ۿ�ȷ���� �깺��ȷ�ϵ�*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn =%d and sts in('0','1','4') order by prdt_code",sMdm_ac_rel_old.ac_id,iAc_seqn_old);
        if(iRet)
        {
            vtcp_log("���������α����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333");
            goto ErrExit;
        }
        while(1)
        {
            memset(&sFn_reg,0x00,sizeof(sFn_reg));
            memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
            memset(&sFn_parm,0x00,sizeof(sFn_parm));
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            if(iRet != 0 && iRet != 100)
            {
                vtcp_log("ȡ�����α����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            else if(iRet == 100)
            {
                if(iCnt  == 0)
                {
                    vtcp_log("����˻�û���Ϲ��κ���Ʋ�Ʒ iRet[%d] ",iRet);
                    strcpy(g_pub_tx.reply,"D104");
                    goto ErrExit;
                }
                break;
            }
            iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
            if(iRet)
            {
                vtcp_log("û�������Ʋ�Ʒ! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
                strcpy(g_pub_tx.reply,"D326");
                goto ErrExit;
            }
            /*���ֲ�ͬ�Ĳ��ܸ���*/
            /*if(strcmp(sDd_mst.cur_no,sFn_parm.cur_no))
            {                 
                vtcp_log("���ܸ���Ϊ�������ֵ��˻�parm.cur_no[%s] dd.cur_no[%s]",sFn_parm.cur_no,sDd_mst.cur_no);
                strcpy(g_pub_tx.reply,"D337");
                goto ErrExit;
            }*/
            strcpy(sFn_reg.filler,"");
            sprintf(sFn_reg.filler,"%s|%d",sFn_reg.ac_no,sFn_reg.ac_seqn);
            strcpy(sFn_reg.ac_no,cAc_no_new);
            sFn_reg.ac_id  = sDd_mst.ac_id;
            sFn_reg.ac_seqn  = sDd_mst.ac_seqn;
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("����fn_reg����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            /*������Ϲ��ģ����뽫���˺Ž�����ס��ͬʱ��ԭ�˺ſ��ƽ���� Ϊʲô����������*/
            if(sFn_reg.sts[0] == '0')
            {
                /*�����Ϲ��˻��Ŀ��ƽ��*/
                iRet  = upd_dd_mst(0,sMdm_ac_rel_old.ac_id,iAc_seqn_old,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("���»��ڴ�����ļ����� iRet[%d] ",iRet);
                    goto ErrExit;
                }
                /*�����滻�˺ŵĿ��ƽ��*/
                iRet  = upd_dd_mst(1,sFn_reg.ac_id,sFn_reg.ac_seqn,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("���»��ڴ�����ļ����� iRet[%d] ",iRet);
                    goto ErrExit;
                }
            }
            iCnt ++;
            sFn_reg_hst.tx_date = g_pub_tx.tx_date;
            sFn_reg_hst.trace_no = g_pub_tx.trace_no;
            strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
            strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
            strcpy(sFn_reg_hst.type,"7");
            sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
            strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
            strcpy(sFn_reg_hst.agent_id,cAgent_id);
            strcpy(sFn_reg_hst.agent_name,cAgent_name);
            sFn_reg_hst.buy_date=sFn_reg.tx_date;
            sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
            strcpy(sFn_reg_hst.brf,"����ǩԼ�˺�");
            sprintf(sFn_reg_hst.filler,"%s|%d|%s|%d",cAc_no_old,iAc_seqn_old,cAc_no_new,iAc_seqn_new);
            iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
            if(iRet)
            {
                Fn_reg_hst_Debug(&sFn_reg_hst);
                vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
        }
        Fn_reg_Clo_Upd();
    }
    else
    {
        vtcp_log( "�������ʹ���!");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    iLen=strlen(cAc_no_old);
    cAc_no_old_tmp[0]=cAc_no_old[iLen-4];
    cAc_no_old_tmp[1]=cAc_no_old[iLen-3];
    cAc_no_old_tmp[2]=cAc_no_old[iLen-2];
    cAc_no_old_tmp[3]=cAc_no_old[iLen-1];
    cAc_no_old_tmp[4]=0x00;

		pub_mob_sendmail( cAc_no_old_tmp, "D084",cAc_no_new,"2",0, 0.00 );

    memcpy(g_pub_tx.ac_no,cAc_no_old,sizeof(g_pub_tx.ac_no)-1);
    strcpy(g_pub_tx.brf,"�������ǩԼ�˺�");
    vtcp_log(" insert trace_log record ");
    if( pub_ins_trace_log() )
    {
        vtcp_log("�Ǽǽ�����ˮ����! ");
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/*���»�������Ŀ��ƽ��*/
int upd_dd_mst(int mode,long ac_id,int ac_seqn,double tx_amt)
{
    int iRet = 0;
    double dAvbal = 0.00;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;

    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    iRet = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id = %ld and ac_seqn = %d",ac_id,ac_seqn);
    if(iRet)
    {
        vtcp_log("������»������ļ��α����");
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
    if(iRet)
    {
        vtcp_log("ȡ���»������ļ��α����");
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(iRet)
    {
        vtcp_log("��ѯ���ڲ�Ʒ��������");
        strcpy(g_pub_tx.reply,"015");
        goto ErrExit;
    }
    /*dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.zf_bal - sDd_mst.ctl_amt - sDd_parm.min_bal;*/
    dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.ctl_amt - sDd_parm.min_bal;
    if(mode  == 0)
    {
        sDd_mst.ctl_amt  -= tx_amt;
        if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) < 0)
        {
            vtcp_log("���ƽ��С��0�˰�[%lf]",sDd_mst.ctl_amt);
            strcpy(g_pub_tx.reply,"D229");
            goto ErrExit;
        }
    }
    else if(mode == 1)
    {
        /*����¿�������������*/
        if(pub_base_CompDblVal(tx_amt,dAvbal) > 0)
        {
            vtcp_log("���ý����� tx_amt[%lf] dAvbal[%lf]",tx_amt,dAvbal);
            strcpy(g_pub_tx.reply,"A030");
            goto ErrExit;
        }
        sDd_mst.ctl_amt += tx_amt;
    }
    else
    {
        vtcp_log("������־����[%d]",mode);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    iRet = Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
    if(iRet)
    {
        vtcp_log("���»������ļ�ʧ��");
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
OkExit:
    vtcp_log("���»������ļ��ɹ�");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("���»������ļ�ʧ��");
    return 1;
}

