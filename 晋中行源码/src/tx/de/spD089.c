/*************************************************************
* �� �� ��: spD089.c
* ��������: ļ��ʧ�ܴ���...ǰ̨����ֱ�ӵ���.
*
* ��    ��: zyl
* �������: 20110825
*
* �޸ļ�¼: �ĳɽ����汾��ʧ�ܴ��� ����������
                ȫ������

* ��    ��:20130524
* �� �� ��:
* ��    ע:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"


int spD089()
{
    int        iRet = 0;
    int        iFlag = 0;     /**��־ �Ƿ���Ҫ���ļ�.**/
    char       cPrdt_code[21];   /**��Ʒ���**/
    char       cFilename[128];
    char       cTmpbuf[512];
    FILE      *fp = NULL;
    struct fn_parm_c      g_fn_parm;
    struct fn_reg_c       g_fn_reg;
    struct mdm_ac_rel_c   sMdm_ac_rel;
    struct dd_mst_c       sDd_mst;
    memset(cPrdt_code ,  0x00 , sizeof(cPrdt_code));
    memset(cFilename ,   0x00 , sizeof(cFilename));
    memset(cTmpbuf,      0x00 , sizeof(cTmpbuf));
    memset(&g_fn_parm,   0x00 , sizeof(struct fn_parm_c));
    memset(&g_fn_reg,    0x00 , sizeof(struct fn_reg_c));
    memset(&sMdm_ac_rel, 0x00 , sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,     0x00 , sizeof(struct dd_mst_c));

    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    /**�õ����׻���,��Ʒ����**/

    get_zd_data("0630", cPrdt_code);
    vtcp_log("prdt_code=[%s]", cPrdt_code);

    /*add by lwb 20140820 ���ӶԻ������ж�*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO))
    {
	sprintf(acErrMsg,"���������Ĳ��������˽��ף�");
	WRITEMSG
	strcpy(g_pub_tx.reply,"P047");
	goto ErrExit;
    }
    /**�����´�ȫ·���ļ���**/
    pub_base_AllDwnFilName(cFilename);
    fp = fopen(cFilename, "w");
    if (fp == NULL)
    {
        vtcp_log("open file error [%s]", cFilename);
        strcpy(g_pub_tx.reply, "S066");
        goto ErrExit;
    }
    /**��Ʋ�Ʒδ���۳ɹ���ϸ��**/
    /** ����|�˺�|��Ʒ����|���|��������|�������� */
    fprintf( fp, "\n\n");
    fprintf( fp, "             ���ļ��ʧ�ܴ���\n");
    fprintf( fp, "             �������[%s]  ��Ա��[%s] ����[%ld]\n",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.tx_date);
    fprintf( fp, "%-24s %-20s %-60s %-17s %-8s\n","�˺�","��Ʒ����","��Ʒ����","���","�Ϲ�����");

    iRet = Fn_parm_Dec_Upd(g_pub_tx.reply, " prdt_code ='%s' ", cPrdt_code);
    if(iRet)
    {
        vtcp_log("����fn_parm�α�ʧ��! iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D101");
        goto ErrExit;
    }
    iRet = Fn_parm_Fet_Upd(&g_fn_parm, g_pub_tx.reply);
    if(iRet == 100)
    {
        vtcp_log( "û�������Ʒ iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D104");
        goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log( "��fn_parm�α�ʧ��! iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D324");
        goto ErrExit;
    }
    /*ֻ��ļ��״̬����ͣ���۵� ����ļ��ʧ��*/
    if(g_fn_parm.stat[0] != '0' && g_fn_parm.stat[0] != '4')
    {
        vtcp_log( "��Ʋ�Ʒ״̬ ������ļ��ʧ��! stat=[%s]", g_fn_parm.stat);
        strcpy(g_pub_tx.reply, "D3334");
        goto ErrExit;
    }
    /**����Ʒ����״̬..�Ƿ�ļ��ʧ��.**/
    vtcp_log("beg_date=[%ld]", g_fn_parm.beg_date);
    vtcp_log("end_date=[%ld]", g_fn_parm.end_date);
    vtcp_log("tx_date =[%ld]", g_pub_tx.tx_date);
    vtcp_log("sale_amt=[%17.2f]", g_fn_parm.sale_amt);
    /**ļ��ʧ��,�ʽ����������.�����Ϲ��Ǽǲ� fn_reg.**/
    iRet = Fn_reg_Dec_Upd(g_pub_tx.reply, "prdt_code='%s' and sts='0'", g_fn_parm.prdt_code);
    if(iRet)
    {
        vtcp_log( "����fn_ret�α�ʧ��! iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D330");
        goto ErrExit;
    }
    while(1)
    {
        memset(&g_fn_reg,   0x00 , sizeof(struct fn_reg_c));
        iRet = Fn_reg_Fet_Upd(&g_fn_reg, g_pub_tx.reply);
        if(iRet == 100)
        {
            break;
        }
        else if(iRet)
        {
            vtcp_log("��fn_reg�α�ʧ��! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D330");
            goto ErrExit;
        }
        g_fn_reg.sts[0] = '3';
        iRet = Fn_reg_Upd_Upd(g_fn_reg, g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("����fn_regʧ��! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D332");
            goto ErrExit;
        }
        vtcp_log("ac_id =[%ld] ac_seqn[%ld]", g_fn_reg.ac_id,g_fn_reg.ac_seqn);
        vtcp_log("tx_amt=[%17.2f]", g_fn_reg.tx_amt);
        memset(&sDd_mst,   0x00,  sizeof(struct dd_mst_c));
        iRet = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld and ac_seqn=%d", g_fn_reg.ac_id,g_fn_reg.ac_seqn);
        if(iRet)
        {
            vtcp_log( "����dd_mst�α�ʧ��! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D226");
            goto ErrExit;
        }
        iRet = Dd_mst_Fet_Upd(&sDd_mst, g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("��dd_mst�α�ʧ��! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D230");
            goto ErrExit;
        }
        vtcp_log("ctl_amt=[%17.2f]", sDd_mst.ctl_amt);
        sDd_mst.ctl_amt -= g_fn_reg.tx_amt;
        if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) <0)
        {
            vtcp_log("�˺ſ��ƽ��С��0�ˣ�������ԭ��! ctl_amt=[%lf]",sDd_mst.ctl_amt);
            strcpy(g_pub_tx.reply,"P068");
            goto ErrExit;
        }
        iRet = Dd_mst_Upd_Upd(sDd_mst, g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("����dd_mstʧ��! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D226");
            goto ErrExit;
        }
        Dd_mst_Clo_Upd();
        /**��ѯac_no**
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",g_fn_reg.ac_no);
        if (iRet == 100)
        {
            vtcp_log( "�����ʺŹ�ϵ�����ڸ��ʺ�!");
            strcpy(g_pub_tx.reply, "P088");
            goto ErrExit;
        }
        if (iRet)
        {
            vtcp_log("ȡ�����ʺŹ�ϵ�����!");
            strcpy(g_pub_tx.reply, "P084");
            goto ErrExit;
        }
        */
        /**���ɴ�ӡ�ļ���Ϣ**/
        /** ����|�˺�|��Ʒ����|���|��������|�������� **/
        fprintf(fp, "%-24s %-20s %-60s %-17.2lf %-8d\n", \
                g_fn_reg.ac_no,g_fn_parm.prdt_code, g_fn_parm.title, g_fn_reg.tx_amt, g_fn_reg.tx_date);

        iFlag++;
    		 pub_mob_sendmail(  g_pub_tx.brf, "0089",g_fn_reg.ac_no,"1",g_fn_reg.tx_amt, 0.00 );
    
    }
    Fn_reg_Clo_Upd();
    g_fn_parm.stat[0] = '3';
    iRet = Fn_parm_Upd_Upd(g_fn_parm,g_pub_tx.reply);
    if (iRet)
    {
        vtcp_log("������Ʋ�Ʒ������ʧ��!");
        strcpy(g_pub_tx.reply, "D325");
        goto ErrExit;
    }
    Fn_parm_Clo_Upd();
    fclose(fp);
    if(iFlag)
    {
        /*set_zd_data(DC_FILE_SND, "1"); �������ļ���*/
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log( "����ɹ�![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 0;

ErrExit:
    vtcp_log("����ʧ��![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}


