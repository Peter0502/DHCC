/*************************************************
* �� �� ��:     pubf_eod_crd.c
* ����������    ���տ��ദ��
* ��    ��:     jack
* ������ڣ�    2003��07��30��
* �޸ļ�¼��
*
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "com_chrg_rate_c.h"
#include "public.h"
#include "mdm_attr_c.h"
#include "com_chrg_rate_c.h"
#include "com_parm_c.h"
#include "dc_acc_rel_c.h"
#include "com_chrg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "com_item_c.h"
#include "card_reg_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "card.h"


/**********************************************************************
 * �� �� ����   pub_eod_crd_chrg
 * �������ܣ�   ���տ��շѴ��� 
 * ����/ʱ�䣺  
 *
 * ������
 *     ����: �շѴ���
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_crd_chrg(char chrg_rate_code[4])
{
	int ret=0;
	struct com_chrg_rate_c  sCom_chrg_rate;
	memset(&sCom_chrg_rate,0x00,sizeof(struct com_chrg_rate_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));

	ret=Com_chrg_rate_Sel(g_pub_tx.reply,&sCom_chrg_rate,"chrg_rate_code='%s' ",chrg_rate_code);
	if( ret )
	{
		printf(acErrMsg, "ȡ���ʴ������!![%d][%s]",ret,chrg_rate_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W117");
		return -1;
	}
	/*** �������Ƿ� ***/
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=1 ",g_mdm_ac_rel.ac_id);
	if( ret )
	{
		printf(acErrMsg, "ȡ�������ļ�����!![%d][%s]",ret,g_mdm_ac_rel.ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W117");
		return 1;
	}
	ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",g_dd_mst.prdt_no);
	if( ret )
	{
		printf(acErrMsg, "ȡ���ڲ�Ʒ������!![%d][%s]",ret,g_dd_mst.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W024");
		return 1;
	}
	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	if( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' )
		return 1;
	else if( g_dd_mst.hold_sts[0]=='3' )
	{
		ret=pub_base_CompDblVal(sCom_chrg_rate.val,g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
		if( ret>0 )		
			return 1;
	}
	else	/*** �Ƕ��� ***/
	{
		ret=pub_base_CompDblVal(sCom_chrg_rate.val,g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
		if( ret>0 )		
			return 1;
	}
	strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=1;
	strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
	strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);
	g_pub_tx.tx_amt1=sCom_chrg_rate.val;
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"���");
	strcpy(g_pub_tx.add_ind,"0"); /**����**/
	strcpy(g_pub_tx.prt_ind,"0"); /*�Ƿ���۱�־*/
	strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
	strcpy(g_pub_tx.ac_wrk_ind,"000000001");
	strcpy(g_pub_tx.ac_get_ind,"111");
	g_pub_tx.svc_ind=1001; /**��ȡ��***/
	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		return 1;
	}
	return 0;
}

/**********************************************************************
* �� �� ����   pub_eod_crd_chrg_chk
* �������ܣ�   ���տ��շѴ��� 
* ����/ʱ�䣺  
*
* ������
    double *amt        Ϊ�շѽ��,�������ͬʱ���ؿ�ʹ����С���
    char    chrg_typ    '1' ���˻�������С��'0' ���㲻����
    char    *fld_brf    �۷�����
    char    *succmsg    Ϊ��������Ϣ�������ش���ԭ��
    
*     ����: �շѴ���
*     ���: 
*     ����: <>0ʧ��
             =0�ɹ�
*
* �޸���ʷ��
*/
int pub_eod_crd_chrg_chk(double *amt, char chrg_typ, char* fld_brf, char *succmsg)
{
    int ret=0;
    memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
    strcpy(g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no);
    strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);
    vtcp_log("[%s][%d],����ֻ������,%lf, bal[%lf]",__FILE__,__LINE__,*amt, g_dd_mst.bal);
    /*** �������Ƿ� ***/
    ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",g_dd_mst.prdt_no);
    if ( ret )
    {
        printf(acErrMsg, "ȡ���ڲ�Ʒ������!![%d][%s]",ret,g_dd_mst.prdt_no);
        WRITEMSG
        strcpy(succmsg, "��Ʒ����");
        return 1;
    }
    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
    vtcp_log("%s,%d,����ֻ������,%lf, bal[%lf]",__FILE__,__LINE__,*amt, g_dd_mst.bal);
    Dd_mst_Debug(&g_dd_mst);
    if ( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' ){
        *amt=0;
        strcpy(succmsg, "�˻�״̬������");
        return 1;
    }
    else if ( g_dd_mst.hold_sts[0]=='3' )
    {
        vtcp_log("%s,%d,����ֻ������,%lf",__FILE__,__LINE__,*amt);
        ret=pub_base_CompDblVal(*amt,g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
        if(ret>0  && '1' == chrg_typ )
        {
            *amt=g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt-g_dd_parm.min_bal; 
            if(*amt<0.00001)
            {
                *amt=0;
                strcpy(succmsg, "����");
                return 1;
            }
            else
            {
                strcpy(succmsg, "�۷ѳɹ�");
            }
        }else if ( ret>0 ) /* by Wang Yongwei 20100414 Ӧ����elseû�м� */
        {
            strcpy(succmsg, "�˻�״̬������");
            return 1;
        }
        vtcp_log("%s,%d,����ֻ������,%lf",__FILE__,__LINE__,*amt);
    }
    else    /*** �Ƕ��� ***/
    {
        vtcp_log("%s,%d,����ֻ������,%lf",__FILE__,__LINE__,*amt);
        ret=pub_base_CompDblVal(*amt,g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
        if(ret>0  && '1' == chrg_typ )
        {
            vtcp_log("%s,%d,����ֻ������,%lf ,���� %lf %lf %lf %lf ",__FILE__,__LINE__,*amt, g_dd_mst.bal, g_dd_mst.ctl_amt, g_dd_parm.min_bal, g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal); 
            *amt=g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;
            vtcp_log("%s,%d,����ֻ������,%lf",__FILE__,__LINE__,*amt); 
            if(*amt<0.00001)
            {
                *amt=0;
                strcpy(succmsg, "����");
                return 1;
            }
            else
            {
                strcpy(succmsg, "�۷ѳɹ�");
            }
            ret=0;
        }
        if ( ret>0 )
        {
            strcpy(succmsg, "�˻�״̬������");
            return 1;
        }
    }
    vtcp_log("%s,%d,����ֻ������,%lf",__FILE__,__LINE__,*amt);
    strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
    g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
    g_pub_tx.ac_seqn=1;
    strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
    strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);
    g_pub_tx.tx_amt1=*amt;
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.brf,fld_brf);
    strcpy(g_pub_tx.add_ind,"0"); /**����**/
    strcpy(g_pub_tx.ac_id_type, "1");    /* �����˻� */
    strcpy(g_pub_tx.prt_ind,"0"); /*�Ƿ���۱�־*/
    strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
    strcpy(g_pub_tx.ac_wrk_ind,"000000001");
    strcpy(g_pub_tx.ac_get_ind,"111");
    g_pub_tx.svc_ind=1001; /**��ȡ��***/
    ret=pub_acct_trance();
    if ( ret )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        strcpy(succmsg, "����ʧ��");
        return 1;
    }
    set_zd_data("102O", "0");
    set_zd_data("102J", "01");
    set_zd_data("102K", "2");
    set_zd_double("102F", g_pub_tx.tx_amt1);
    set_zd_double("102P", 0.00);
    set_zd_double("102Q", 0.00);
    ret = pubf_acct_proc("D003");
    if (ret){
        sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
        WRITEMSG
        strcpy(succmsg, "����ʧ��");
        return 1;
    }
    vtcp_log("%s,%d,�ֻ��ɹ�,%lf",__FILE__,__LINE__,*amt);
    return 0;
}
/**********************************************************************
* �� �� ����   pub_eod_crd_fee
* �������ܣ�   ���շ��ô���
* ����/ʱ�䣺   gujy
*
* ������
*     ����:  feetyp Ϊһ��������һλ������ѣ��ڶ�λΪС�����ѣ��������Ϊ1
            �������Ϊ'10',��С������Ϊ'01',����Ϊ'11'
        fee_br_no    �շѻ�����Ϊ����Ϊ���л���
        fee_ac_no    �շѿ���, Ϊ����Ϊ���з��������Ŀ�
        fee_hst_typ    '1'Ϊֻ��Ƿ�ѣ�ֻ��Com_chrg_hst�в��ɹ���¼
                 '2' Ϊ�����Com_chrg_hst��¼
                '0' �ȴ���Ƿ�����շ�
        succmsg        �������������򷵻ش���ԭ��
        fee_fact    �������0��۷ѽ�����ȡ������
*     ���: 
*     ����: <>0ʧ��
=0�ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_eod_crd_fee(char *feetyp, char *fee_br_no, char *fee_ac_no , char fee_hst_typ, char *succmsg, double *fee_fact)
{
    
    struct mdm_attr_c sMdm_attr;
    struct com_chrg_rate_c sCom_chrg_rate;
    struct com_parm_c sCom_parm;
    struct dc_acc_rel_c sDc_acc_rel;
    struct com_chrg_hst_c com_chrg_hst;
    struct mdm_ac_rel_c   mdm_ac_rel;
    struct dd_mst_c       dd_mst;
    struct dd_mst_hst_c   dd_mst_hst;
    struct com_item_c      com_item;
    struct card_reg_c      card_reg;
    struct com_branch_c    tmp_com_branch_c;
    char        fld_br_no[10];
    char        fld_ac_no[20];
    char        fld[20];
    char        fld_item[20];
    char        cMsg[51];
    char        fld_brf[40];
    char        kou_typ[2]; /*�ۻ���־��'0' ���� '1' �ж��ٿ۶���*/
    double      kou_amt=0.00; /*�ɹ��ۻ����*/
    int         times;
    char        chrg_rate_code[4];
    char        tmp_char[2];
    char        opn_br_no[6];
    double      AMT=0.00;
    double      dFee_amt=0.00;
    char        cAcc_hrt[6];
    long        kk_date=0;
    char        tx_date[9];
    long        dq_date=0;
    long        Ldate=0;
    long        sf_date = 0, tmp_date = 0;
    int         ret = 0;
    int         i = 0;
    int         ALL_FLAG = 0;
    int         VAL = 0;
    int         cnt1 = 0;
    int         cnt2 = 0;
    int         a=0;
    int         iCount=0;
    int     iJzcnt=0;
    memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&com_chrg_hst, 0x00, sizeof(struct com_chrg_hst_c));
    memset(&sCom_chrg_rate, 0x00, sizeof(struct com_chrg_rate_c));
    memset(&dd_mst, 0x00, sizeof(struct dd_mst_c));
    memset(&dd_mst_hst, 0x00, sizeof(struct dd_mst_hst_c));
    memset(&com_item, 0x00, sizeof(struct com_item_c));
    memset(&card_reg,0x00,sizeof(struct card_reg_c));    
    memset(&tmp_com_branch_c,0x00,sizeof(struct com_branch_c));    
    memset(cAcc_hrt,0x00,sizeof(cAcc_hrt));
    memset(tx_date,0x00,sizeof(tx_date));
    
    memset(cMsg    , 0 , sizeof(cMsg));
    memset(kou_typ , 0 , sizeof(kou_typ));
    memset(opn_br_no,0x00,sizeof(opn_br_no));
    memset(chrg_rate_code,0x00,sizeof(chrg_rate_code));
    memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
    memset(&com_chrg_hst,0x00,sizeof(com_chrg_hst));
    memset(&sMdm_attr, 0x00, sizeof(struct mdm_attr_c));
    memset(&sCom_parm, 0x00, sizeof(struct com_parm_c));
    /*** ��ʼ��chrg_dc�ṹ ***/
    
    /**------- ��ʼ��ȫ�ֱ��� --------**/
    strcpy(fld_br_no, "%%%%");
    strcpy(fld_ac_no, "%%%%");
    if(strlen(fee_br_no)>0)
        strcpy(fld_br_no, fee_br_no);
    if(strlen(fee_ac_no)>0)
        strcpy(fld_ac_no, fee_ac_no);
    vtcp_log("[%s][%d] tx_date [%d] /10 [%d]", __FILE__, __LINE__, g_pub_tx.tx_date, g_pub_tx.tx_date%100);
    ret = pub_base_GetParm_Str("K00TY",1,kou_typ);/**����ѱ�־**/
    if (ret){
        sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"EO004");
        goto ErrExit;
    }
    for(times=0; times<1; times++){ /*times=0 ��������� times=1 ����С������ ��ʱû��С������ times<2 �޸�Ϊ times<1*/
        vtcp_log("[%s][%d] feetyp[%s] times=[%d] [%f] ",__FILE__,__LINE__,feetyp,times,*fee_fact);
        if(0 == times)
        {
            if('1' != feetyp[0])continue;
            vtcp_log("[%s][%d] ��ʼ�տ����feetyp[%s][%f]",__FILE__,__LINE__, feetyp,*fee_fact);
            
            /****����û����Ѵ�����û����ѿ�Ŀ��û����ѽ��õľ������뷢�����Ľ��******
            ret = pub_base_GetParm_int("POSCS",1,&VAL);/ **pos��Ѵ���** /
            if (ret){
                sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            
            ret = pub_base_GetParm_Str("K00",1,cAcc_hrt);/ **��ѿ�Ŀ** /
            if (ret){
                sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            
            ret = pub_base_GetParm_double("SNFJE",1,&dFee_amt);/ **��ѽ��** /
            if (ret){
                sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            ***********************************/
            /* ���п���Ѽǵ���Ŀ�� ChengGX 2010-3-25 11:19 */
            ret = pub_base_GetParm_Str("JZCAR",3,cAcc_hrt);/**����ѿ�Ŀ**/
            if (ret){
                sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            strcpy(g_pub_tx.ac_no,cAcc_hrt);
            /* ���п���Ѽǵ���Ŀ�� ChengGX 2010-3-25 11:19 */
            if(*fee_fact > 0.0001) dFee_amt=*fee_fact;
            vtcp_log("[%s][%d] dFee_amt =[%f]",__FILE__,__LINE__,dFee_amt);
            strcpy(fld_item, "K00");
            strcpy(fld_brf, "�������");
        }
        else
            break;
/*********** ChengGX ��ʱû��С������2010-3-26 9:54
        {
            if('1' != feetyp[1]&&'A' != feetyp[1])
                continue;
            vtcp_log("[%s][%d] ��ʼ�տ�С������feetyp[%s]",__FILE__,__LINE__,feetyp);
            ret = pub_base_GetParm_Str("K01",1,cAcc_hrt);**�����Ŀ**
            if (ret){
                sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            ret = pub_base_GetParm_double("JEK01",1,&dFee_amt);**����ѽ��**
            if (ret){
                sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            if(*fee_fact > 0.0001) 
                dFee_amt=*fee_fact;
            strcpy(fld_item, "K01");
            strcpy(fld_brf, "��С������");
        }
***********/
        if(((g_pub_tx.tx_date/10000)%4==1) && ((g_pub_tx.tx_date%10000)==228) )
        {
            Ldate=(g_pub_tx.tx_date/10000 - 1)*10000+g_pub_tx.tx_date%10000+1;   /** �õ�һ��ǰ�������2��29�� **/
        }
        else
        {
            Ldate=(g_pub_tx.tx_date/10000 - 1)*10000+g_pub_tx.tx_date%10000;/***�õ�һ��ǰ����**/
        }
        sprintf(tx_date,"%ld",g_pub_tx.tx_date);
        ret=Com_branch_Dec_Sel1(g_pub_tx.reply,"br_no like '%s' and  br_type ='4' and wrk_sts!='*' order by br_no", fld_br_no);
        if(ret)
        {
            sprintf(acErrMsg,"��ѯ�����������쳣[%d']",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"EO092");
            goto ErrExit;
        }
        while(1)
        {
            memset(&tmp_com_branch_c,0x00,sizeof(struct com_branch_c));
            AMT=0.00;/**ÿ��������ʼһ��**/
            ret=Com_branch_Fet_Sel1(&tmp_com_branch_c,g_pub_tx.reply);
            if(ret==100)
            {
                break;
            }
            else if(ret)
            {
                sprintf(acErrMsg,"��ѯ�����������쳣[%d']",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO092");
                goto ErrExit;
            }
            char filler2[9];
            memset(filler2,0x00,sizeof(filler2));
            sprintf(filler2,"%ld",Ldate);
            vtcp_log("[%s][%d]  filler2=[%s]",__FILE__,__LINE__,filler2);
            /**0��ʾ�¿���һ�ηѶ�û�۹�����ȡ�ϴ�ͳ��pos�����Ľ�ֹ���� ���Ϊһ��**/
            
            /**modify by xyy ���Ӷ��˻����жϣ�ֻ���˻���621223&&621780��ͷ�ĺͽ���״̬Ϊ�����͹�ʧ��**/
            if(0 == strcmp("K00", fld_item)){
                if(0 != memcmp("%%%%", fld_ac_no, 4)){
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no = '%s'", fld_ac_no);
                }
                else
                {
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no in (select ac_no from mdm_ac_rel where opn_br_no = '%s') and ( filler2= %s or (filler2 is null and ac_no in(select ac_no from mdm_ac_rel where opn_br_no='%s' and beg_date=%ld and substr(ac_no,0,6) in ('%s','%s') and  note_sts in ('0','1')) ))",tmp_com_branch_c.br_no, filler2,tmp_com_branch_c.br_no,Ldate,CARDHEADCODE,ICCARDHEADCODE);
                }
            }
            else
            {
                if('A'==feetyp[1])
                { /*���ļ���ȡ*/
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no = '%s'", fld_ac_no);
                }
                else
                {
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no in (select a.ac_no from dynamic_acm a, mdm_ac_rel b   where a.ac_no=b.ac_no and opn_br_no='%s' and y_acm/(to_date('%ld', 'yyyymmdd')-to_date('%ld0101', 'yyyymmdd')) < 200) ", tmp_com_branch_c.br_no, g_pub_tx.tx_date, g_pub_tx.tx_date/10000);
                }
            }
            if(ret){
                 sprintf(acErrMsg,"��ѯ���Ǽǲ�����[%d]",ret);
                 WRITEMSG
                 memcpy(cMsg,"�޴˿���",8);
                 *fee_fact=0.00;
                 goto OkExit;
            }
            iCount=0;
            iJzcnt=0;
            while(1) 
            {
                 memset(&card_reg,0x00,sizeof(card_reg));
                 ret = Card_reg_Fet_Upd1(&card_reg,g_pub_tx.reply);
                 if(ret)
                 {
                     vtcp_log("[%s][%d] ret=[%d] iCount=[%d]",__FILE__,__LINE__,ret,iCount);
                     if(ret == 100 && iCount >0)
                     {
                         if(iJzcnt >0)
                         {
                             sprintf(acErrMsg, "cardreg��ѯ���,�ټǿ�Ŀ!![%d]", ret);
                             WRITEMSG
                             break;
                         }
                         else
                         {
                             sprintf(acErrMsg, "cardreg��ѯ���,û�м���,����!![%d]", ret);
                             WRITEMSG
                             memcpy(cMsg,"�ۿ�ʧ��",8);/** ��֪��Ϊʲô������������ ChengGX 2010-3-26 17:13 **/
                             *fee_fact=0.00;
                             goto OkExit;
                         }
                     }
                     else
                     {
                         sprintf(acErrMsg, "cardreg�޴˿���!![%d]", ret);
                         WRITEMSG
                         memcpy(cMsg,"�޴˿���",8);
                         *fee_fact=0.00;
                         goto OkExit;
                     }
                 }
                 vtcp_log("[%s][%d]  �����=[%s]",__FILE__,__LINE__,card_reg.ac_no);
                 memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
                 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",card_reg.ac_no);
                 if (ret)
                 {
                      sprintf(acErrMsg, "ȡ�����ʺŶ��ձ����!![%d]", ret);
                      WRITEMSG
                      memcpy(cMsg,"���ʴ���",8);
                      *fee_fact=0.00;
                      goto OkExit;
                 }
                 memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,strlen(card_reg.ac_no));
                 vtcp_log("[%s][%d],�����,%lf",__FILE__,__LINE__,dFee_amt);
                 memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
                 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=1 ",g_mdm_ac_rel.ac_id);
                 if ( ret )
                 {
                     sprintf(acErrMsg, "ȡ�������ļ�����!![%d][%ld]",ret,g_mdm_ac_rel.ac_id);
                     WRITEMSG
                     memcpy(cMsg,"�޴��˻�",8);
                     *fee_fact=0.00;
                     goto OkExit;
                 }
                 if(g_dd_mst.ac_sts[0]=='*')
                 {
                     vtcp_log("[%s][%d] ���˻��Ѿ�����!!!",__FILE__,__LINE__);
                     memcpy(cMsg,"������",8);
                     *fee_fact=0.00;
                     goto OkExit;
                 }
                 kou_amt=dFee_amt;
                 vtcp_log("[%s][%d] dd_mst.bal[%lf] name[%s]%ld",__FILE__,__LINE__, g_dd_mst.bal, g_dd_mst.name, g_dd_mst.ac_id);
                 vtcp_log("[%s][%d]  WYW--ret[%d] kou_amt[%lf] dFee_amt[%lf],kou_typ[%s]",__FILE__,__LINE__,ret, kou_amt, dFee_amt,kou_typ);
                 ret = pub_eod_crd_chrg_chk(&kou_amt, kou_typ[0], fld_brf, cMsg);
                 vtcp_log("[%s][%d]  ret[%d] kou_amt[%lf]",__FILE__,__LINE__, ret, kou_amt);
            
                 if (ret) 
                 {    /*** �ۿ�ɹ� ***/
                     vtcp_log("[%s][%d] �ۿ�ɹ�!!!!!",__FILE__,__LINE__);
                     *fee_fact=0;
                     strcpy(g_pub_tx.brf, fld_brf);
                     ret = pub_base_ins_chrg_mst(fld_item, "N", dFee_amt);
                     if (ret)
                     {
                           sprintf(acErrMsg, "�Ǽ��շ���ϸ�ʴ���!!");
                           WRITEMSG
                           goto ErrExit;
                     }
                 }
                 else 
                 {   /*** �ۿ�ɹ� ***/
                     vtcp_log("[%s][%d] �ۿ�ɹ�!!!!!",__FILE__,__LINE__);
                     strcpy(g_pub_tx.brf, fld_brf);
                     AMT= AMT + kou_amt;/***��������Ƿ�ѱ���ۼ�ֵ**/
                     vtcp_log("[%s][%d] AMT=[%lf]",__FILE__,__LINE__,AMT);
                     *fee_fact=kou_amt;
                     if(1 == pub_base_CompDblVal( dFee_amt , kou_amt))
                     {
                         ret = pub_base_ins_chrg_mst(fld_item, "N", dFee_amt-kou_amt);
                         vtcp_log( "[%s][%d]pub_base_ins_chrg_mst(%s, 'N', %lf)",__FILE__,__LINE__,fld_item, kou_amt);
                     }
                     else
                     {
                         vtcp_log( "[%s][%d]pub_base_ins_chrg_mst(%s, 'Y', %lf)",__FILE__,__LINE__,fld_item, dFee_amt);
                         ret = pub_base_ins_chrg_mst(fld_item, "Y", dFee_amt);
                     }
                     if (ret)
                     {
                          sprintf(acErrMsg, "�Ѿ�������,���ǵ�����ȴ����!!ret=[%d]", ret);
                          WRITEMSG
                          strcpy(g_pub_tx.reply, "O169");
                          goto ErrExit;
                     }
                     memcpy(cMsg,"�۷ѳɹ�",8);
                     iJzcnt++;
                 }
                 vtcp_log("tx_date %ld trace_no %ld , ac_no %s, tx_code %s, rate_code %s brf %s", \
                 g_pub_tx.tx_date,g_pub_tx.trace_no,g_pub_tx.ac_no,g_pub_tx.tx_code,fld_item,g_pub_tx.brf);
                 strcpy(card_reg.filler1,"0"); /***��POS��������***/
                 strcpy(card_reg.filler2,tx_date);/****���浱ǰ����***/
                 /*Card_reg_Debug(&card_reg);*/
                 ret=Card_reg_Upd_Upd1(card_reg,g_pub_tx.reply);
                 if(ret)
                 {
                      sprintf(acErrMsg,"���¿��Ǽǲ�����[%d] ",ret);
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "O169");
                      Card_reg_Clo_Upd();
                      goto ErrExit;
                 }
                 iCount++;
                 vtcp_log("[%s][%d] iCount=[%d]����=[%d]",__FILE__,__LINE__,iCount,iJzcnt);
            }
            Card_reg_Clo_Upd1();
KMJZ:
            vtcp_log("[%s][%d] ��Ŀ����!!----%lf------",__FILE__,__LINE__,AMT);
            /***��ʼ�ǿ�Ŀ��**/
            if(AMT>0.0001) /*���Ϊ0������*/
            {
                 g_pub_tx.tx_amt1 = AMT;
                 /** ����ֱ��ʹ�ÿ�Ŀ ����Ҫ�ڲ�ѯ�ˡ� ChengGX 2010-3-25 11:33
                 if(strlen(cAcc_hrt)>0)�����Ŀ�����ֲ�Ϊ��
                 {
                     ret = Com_item_Sel(g_pub_tx.reply, &com_item, " acc_hrt='%s' ",cAcc_hrt);
                     if (ret){
                         sprintf(acErrMsg, "ȡ��ƿ�Ŀ����![%d][%s]", ret,sDc_acc_rel.acc_hrt );
                         WRITEMSG
                         strcpy(g_pub_tx.reply, "O169");
                         goto ErrExit;
                     }
                     strcpy(g_pub_tx.ac_no, com_item.acc_no);
                 }
                 else
                 {
                     char cAc_no[25];
                     memset(cAc_no , 0 , sizeof(cAc_no));
                     
                     ret=pub_base_GetParm_Str(tmp_com_branch_c.br_no,PARM_CARD_AC,cAc_no);
                     if (ret){
                         sprintf(acErrMsg,"���������޶�Ӧ��Ϣ[%d]",ret);
                         WRITEMSG
                         strcpy(g_pub_tx.reply,"EO004");
                         goto ErrExit;
                     }
                     vtcp_log("[%s][%d] cAc_no=[%s]",__FILE__,__LINE__,cAc_no);
                     strcpy(g_pub_tx.ac_no, cAc_no);
                 }
                 ����ֱ��ʹ�ÿ�Ŀ ����Ҫ�ڲ�ѯ�ˡ� ChengGX 2010-3-25 11:33 **/
                 strcpy(g_pub_tx.ac_no, cAcc_hrt);
                 strcpy(g_pub_tx.tx_br_no,tmp_com_branch_c.br_no);
                 strcpy(g_pub_tx.opn_br_no,tmp_com_branch_c.br_no);
                 vtcp_log("%s,%d,tmp_com_branch_c.br_no='%s'",__FILE__,__LINE__,tmp_com_branch_c.br_no);
                 vtcp_log("%s,%d,tmp_com_branch_c.br_no='%s'",__FILE__,__LINE__,tmp_com_branch_c.br_no);
                 
                 g_pub_tx.ac_id = 0;
                 g_pub_tx.ac_seqn = 0;
                 strcpy(g_pub_tx.ac_id_type, "9");    /* �˻�����Ϊ�ڲ� */
                 strcpy(g_pub_tx.ac_get_ind, "00");    /* ������δ��ȡ�ֻ� */
                 strcpy(g_pub_tx.ac_wrk_ind, "000");    /* ���˶�ƾ֤��,���?                                  * ����?��ϸ */
                 strcpy(g_pub_tx.add_ind, "1");    /* ������־ */
                 strcpy(g_pub_tx.prt_ind, "0");    /* ������ */
                 g_pub_tx.svc_ind = 9001;    /* �ڲ��ʴ�ȡ */
                 strcpy(g_pub_tx.hst_ind, "1");    /* �ռ�����ϸ */
                 ret = pub_acct_trance();
                 if (ret)
                 {
                     sprintf(acErrMsg, "���ô�ȡ������ʧ��!");
                     WRITEMSG
                     goto ErrExit;
                 }
                 set_zd_data("1214", g_pub_tx.cur_no);
                 set_zd_data("1215", g_pub_tx.ct_ind);
                 set_zd_double("1218", g_pub_tx.tx_amt1);
                 ret = pubf_acct_proc("A017");
                 if (ret)
                 {
                     sprintf(acErrMsg, "������Ƽ��ʲ��ɹ�!!");
                     WRITEMSG
                     goto ErrExit;
                 }
                 set_zd_data("1214", "");
                 set_zd_data("1215", "");
                 set_zd_double("1218",0.0);
            }
        }
        Com_branch_Clo_Sel1();
    }
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    memcpy(succmsg,cMsg,strlen(cMsg));
    sprintf(acErrMsg, "[%s]���״���ɹ�!!!!![%s]",__FILE__, g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    if(0 == strcmp("0000", g_pub_tx.reply)) 
        strcpy(g_pub_tx.reply, "E000");
    sprintf(acErrMsg, "!!!!![%s]", g_pub_tx.reply);
    sprintf(succmsg, "����ʧ��,������%s", g_pub_tx.reply);
    *fee_fact=AMT;
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}
