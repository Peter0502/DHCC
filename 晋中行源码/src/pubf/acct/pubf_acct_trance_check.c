/***************************************************************
* �� �� ��:     pubf_acct_trance_check.c
* ����������
*              1������˺š��˺�ID���˺���ŵĺϷ��ԣ�
*              2���ͻ�֧Ʊ���Ŵ���;
*              3������Ʒ����������˺����ļ����޸����ļ�ȫ�ֱ�����
**��־�÷���
**��Ҫ�˶�ƾ֤��** g_pub_tx.ac_wrk_ind[0]='1'
**				   g_pub_tx.hst_ind[0]=='1'�ռ�����ϸ
**				   g_pub_tx.prt_ind[0]!='1'�Ƿ����
*
* ��    ��:	jack	
* ������ڣ�    2003��12��27��
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

struct mdm_attr_c v_mdm_attr;
struct prdt_ac_id_c prdt_ac_id_c;
struct mdm_attr_c mdm_attr_c;
struct note_parm_c note_parm_c;
struct card_consum_lmt_c g_card_consum_lmt;
static	struct ln_mst_c ln_mst_acm;
static	struct ln_mst_c o_td_mst;
static int wdr_flag=0;

pub_acct_trance_check()
{
	int ret=0;
	int ass_ind=0;
	int factday,lastday;
	struct pub_tx v_pub_tx;
	memset(&v_pub_tx, 0  , sizeof(v_pub_tx)); /* add by LiuHuafeng */
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&note_parm_c,0x00,sizeof(struct note_parm_c));
	wdr_flag=0;

    sprintf(acErrMsg, "����pub_acct_trance_check ����date[%d],svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
	vtcp_log("%s,%d ���·ֻ�,����ĿS----, ac_no[%s]  ",__FILE__,__LINE__,g_pub_tx.ac_no);
    WRITEMSG
	/**��ʼ�����ļ�����**/
	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_strpack(g_pub_tx.crd_no);
	if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
	{
		strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
	}
	if( g_pub_tx.ct_ind[0]!='1' ) g_pub_tx.ct_ind[0]='2';

	sprintf(acErrMsg,"g_pub_tx.acno= [%s] id=[%ld] seqn=[%d]", \
			g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
	ret=strlen(g_pub_tx.ac_no);
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
vtcp_log("%s,%d ���·ֻ�,����ĿS----, crd_no[%s]  [%d]",__FILE__,__LINE__,g_pub_tx.ac_no, ret);
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
	if(ret==3||ret==5||ret==7)
	{
		/**�Կ�Ŀ����**/
		ret=Chg_acchrt_mst_check(&ass_ind);
		ERR_DEAL
	}
	else
	{
		/**��ʼ�����ļ�����**/
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
		ret=pub_sub_ac_check();
		if( ret==100 ) return 0; /***TEMP�����˺�***/
		ERR_DEAL

	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
		/**����Ƿ���Э��**/
		vtcp_log("-==========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
		ret=pub_sub_prdt_chk_check();
		ERR_DEAL
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);

		/***��鴦��ֻ���***/
		vtcp_log("-========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
		ret=pub_sub_mst_check();
		ERR_DEAL
	}

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
	/***�Ǽǽ�����ˮ***/
	if( g_pub_tx.ac_id_type[0]!='3' )
	{
		/***�����ر���***/
		ret=pub_sub_spe_do_check();
		ERR_DEAL

	}
sprintf(acErrMsg,"WDR����%d",wdr_flag);
WRITEMSG
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
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

int Chg_dd_mst_check()
{
	int ret;
	double tmpje=0.00;
	double tmpbal=0.00;
	struct dd_mst_c	TmpDd_mst;
	char msg[51];
	struct	remark_ctl_c rem;
	int chk_ctl_flag=0;
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG

	memset(&rem,0x00,sizeof(struct remark_ctl_c));
	/*** ���û��ڲ�Ʒ���������麯�� ***/
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);

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
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
			"ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	}
	/*beg ����������˻�by wjl 20130306*/
             
             struct mo_prdt_prot_c sMo_prdt_prot;
             struct mo_prot_ac_rel_c sMo_prot_ac_rel;
             struct dd_mst_c sDd_mst_virtual;
             struct dd_parm_c sDd_parm_virtual;/*add by slg 2013-02-05 11:02:16*/
             struct dd_mst_c sMain_dd_mst;
             struct dd_parm_c sDd_parm_main;   /*add by slg 2013-02-05 11:02:16*/
             memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
             memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
             memset(&sDd_parm_virtual,0x00,sizeof(struct dd_parm_c));

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
                  g_dd_mst.bal=g_dd_mst.bal+sDd_mst_virtual.bal;
             }
                 vtcp_log("[%s][%d]�����˻����=[%17.2f]",__LINE__,__FILE__,sDd_mst_virtual.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 /* end 20130306*/
	sprintf(acErrMsg,"bal[%lf][%lf][%lf][%lf]",
		g_dd_mst.ctl_amt,g_dd_mst.hold_amt,g_pub_tx.tx_amt1,g_dd_mst.bal);
	WRITEMSG
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);

	if( !strcmp(g_dd_mst.prdt_no,"131") 
		&& strcmp(g_pub_tx.tx_code,"4335") && strcmp(g_pub_tx.tx_code,"9001") 
		&& strcmp(g_pub_tx.tx_code,"4334") && strcmp(g_pub_tx.tx_code,"G090") 
		&& strcmp(g_pub_tx.tx_code,"4373") && strcmp(g_pub_tx.tx_code,"5801")
	)
	{
		vtcp_log("%s,%d txcode=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
		strcpy(g_pub_tx.reply,"P471");
		goto ErrExit;
	}
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG

	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
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
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);

	/** yud ��Ӷ��ڽ��㻧����ǿ�пۿ��־ac_wrk_ind ***
	*** ��10λ����1ʱ���ڷǽ��㻧Ҳ���Կۿ� 20030819 **/
	if( g_pub_tx.ct_ind[0]!='1' && g_dd_mst.sttl_type[0]=='S' &&
		 g_pub_tx.ac_wrk_ind[7]!='J' && g_pub_tx.svc_ind != 1012 &&
		 g_pub_tx.ac_wrk_ind[9]!='1' )
	{
		sprintf(acErrMsg,"�ǽ��㻧������ת��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P105");
		goto ErrExit;
	}

	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
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
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG

	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
	if( g_dd_mst.hold_sts[0]=='1' )
	{
		if( g_pub_tx.ac_wrk_ind[7]=='J' )
			vtcp_log("��ȫ������Ҫ��Ϣ[%s]",g_pub_tx.ac_wrk_ind );
		else
		{
			sprintf(acErrMsg,"���˺��ѱ���ȫ����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P110");
			goto ErrExit;
		}
	}
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG

	vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
	vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
	/*ȡ�������Э������*/
	if( g_pub_tx.add_ind[0]=='0' )
	{
		if( g_dd_mst.ac_sts[0]=='0' )
		{	
			sprintf(acErrMsg,"���˻����ڿ�����ȷ��״̬������������ȷ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O172");
			goto ErrExit;
		}
		vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);

		/*** �жұ�֤�𻧲�����ȡ�� ***/
		if( !strncmp(g_dd_parm.prdt_no,"131",3) && g_pub_tx.ac_wrk_ind[8]!='1' 
			&& strcmp(g_pub_tx.tx_code,"5801")
		)
		{
			sprintf(acErrMsg,"�жұ�֤�𻧲�����ȡ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D212");
			goto ErrExit;
		}

		vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
		if( g_pub_tx.svc_ind!=1003 && g_pub_tx.svc_ind!=1004 )
		{
			if( g_dd_parm.draw_ind[0]=='N' )
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
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
    
       vtcp_log("%s,%d ct_ind=[%lf] ac_id=[%ld]",__FILE__,__LINE__,g_dd_mst.bal,g_pub_tx.ac_id);
		vtcp_log("%s,%d ct_ind====[%c]",__FILE__,__LINE__,g_pub_tx.ct_ind[0]);
		if( g_pub_tx.ct_ind[0]=='1' ) /**ȡ��**/
		{
			/*��ʱ����ר�û��������ֱ�־*/
			if( g_dd_mst.ac_type[0]!='1' && g_dd_mst.ac_type[0]!='2' && \
				g_dd_mst.ac_type[0]!='5' && g_dd_mst.ac_type[0]!='7' && 
     			g_dd_mst.ac_type[0]!='4' )
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

			if( g_dd_parm.cash_ind[0]=='N' )
			{
				sprintf(acErrMsg,"��Ʒ������ȡ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"A012");
				goto ErrExit;
			}
		}
		else if( g_pub_tx.ct_ind[0]=='2' )
		{
			if( g_dd_mst.sttl_type[0]=='S' && g_pub_tx.ac_wrk_ind[7]!='J' && \
				g_pub_tx.svc_ind != 1012 && g_pub_tx.ac_wrk_ind[9]!='1' )
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
        	ret = pub_base_check_draw("001");
        	ERR_DEAL
		}
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
       vtcp_log("------44444444------[%lf]",g_dd_mst.bal);
		switch( g_pub_tx.svc_ind )
		{
			case 1001: 		/*��ȡ��˴�Ϊȡ��*/
				if( g_dd_mst.ac_type[0]=='6' || g_dd_mst.ac_type[0]=='7' )
				{
					sprintf(acErrMsg,"�˻�ΪЭ��������˺ţ�����ֱ�Ӵ�ȡ��!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P293");
					goto ErrExit;
				}
			case 1009:		/*** ������ȡ�� ***/
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
			case 1008: 		/*Э������ȡ��˴�Ϊȡ��*/
				/*** ȡ��ʱ��鿨�޶�ǼǱ������� ***/
				/*** ADD COMMENCE BY Azz.05.03.26 ***/
				ret = Card_consum_lmt_Sel( g_pub_tx.reply,&g_card_consum_lmt,"ac_no='%s'", \
												g_pub_tx.ac_no); 
		sprintf(acErrMsg,"[%s][%d]ע�� ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
		WRITEMSG
         vtcp_log("------33333333------[%lf]",g_dd_mst.bal);
				if( ret&&ret!=100 )
				{
					sprintf(acErrMsg,"DECLARE ���޶�ǼǱ��쳣!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P121");
					goto ErrExit;
				}

				/*** ȡ�� ***/
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
                    vtcp_log("------5555555555------[%lf]",g_dd_mst.bal);
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
                     vtcp_log("------66666666666------[%lf]",g_dd_mst.bal);
					g_card_consum_lmt.da_all_amt_lmt = \
					g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_amt_lmt;

					g_card_consum_lmt.mo_all_amt_lmt = \
					g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_amt_lmt;
				}
				/*** ADD COMPLETE ***/
                      vtcp_log("------77777777777------[%lf]",g_dd_mst.bal);
				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				break;
			case 1006: 		/*Ԥ��Ȩ*/
				g_dd_mst.ctl_amt=g_dd_mst.ctl_amt+g_pub_tx.tx_amt1;
				break;
			case 1007: 		/*Ԥ��Ȩ���*/
				if( pub_base_CompDblVal(g_dd_mst.ctl_amt,g_pub_tx.tx_amt1)<0 )
				{
					sprintf(acErrMsg,"Ԥ��Ȩ��ɽ���[%.2lf]<[%.2lf]!",
						g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
					WRITEMSG
					strcpy( g_pub_tx.reply,"A031" );
					goto ErrExit;
				}
				vtcp_log("------88888888888------[%lf]",g_dd_mst.bal);
				g_dd_mst.ctl_amt=0.00;
				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				vtcp_log("------9999999999------[%lf]",g_dd_mst.bal);
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				break;
			case 1010: 		/*�������������*/
				g_pub_tx.tx_amt1=g_dd_mst.bal;
				if( strcmp(g_dd_mst.prdt_no,"102") )
				{
					sprintf(acErrMsg,"�ǹ�����![%s]",g_dd_mst.prdt_no);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P416");
					goto ErrExit;
				}
			case 1003: 		/*����*/
			case 1004: 		/*Э������*/
				break;
			default :
				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				sprintf( acErrMsg,"��������[%d]��ͬȡ��",g_pub_tx.svc_ind );
				WRITEMSG
				break;
		}
		if( g_dd_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"���˻�����ֻ����������״̬!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
		if( g_dd_mst.hold_sts[0]=='3' && g_dd_mst.bal<g_dd_mst.hold_amt )
		{
			sprintf(acErrMsg,"���˺��ѱ����ֶ���!g_dd_mst.bal===[%.2lf], \
			g_dd_mst.hold_amt======[%.2lf]",g_dd_mst.bal,g_dd_mst.hold_amt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P112");
			goto ErrExit;
		}
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
	else if( g_pub_tx.add_ind[0]=='1' )		/*���*/
	{
		if( g_dd_parm.dpst_ind[0]=='N' )
		{
			sprintf(acErrMsg,"��������[%s]!",g_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A009");
			goto ErrExit;
		}
		if( g_dd_parm.thr_dpst_ind[0]=='N' 
			&& strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
		{
			sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A010");
			goto ErrExit;
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
			case 1001: 		/*���*/
				if( g_dd_mst.ac_type[0]=='6' )
				{
					sprintf(acErrMsg,"�˻�ΪЭ��������˺ţ�����ֱ�Ӵ�ȡ��!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P293");
					goto ErrExit;
				}
			case 1005:
			case 1008: 		/*���*/
				g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
				break;
			case 1006: 		/*Ԥ��Ȩ*/
				g_dd_mst.ctl_amt=g_dd_mst.ctl_amt-g_pub_tx.tx_amt1;
				if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00)<0 )
				{
					sprintf(acErrMsg,"Ԥ��Ȩ��ɽ���[%.2lf]<[%.2lf]!",
							g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
					WRITEMSG
					strcpy( g_pub_tx.reply,"A031" );
					goto ErrExit;
				}
				break;
			default :
				sprintf( acErrMsg,"��������[%d]��ͬ���",g_pub_tx.svc_ind );
				WRITEMSG
				g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
				break;
		}
		/*** ��������Ϣ�����ĺ��� ***/

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
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
	vtcp_log("------22222222------[%lf]",g_dd_mst.bal);

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
		if ( g_pub_tx.add_ind[0]=='0' )  /* ȡ����ж� YUDONG 20030901*/
		{
			if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
			{
				Remark_ctl_Sel(g_pub_tx.reply,&rem,
					"ac_id=%d AND rem_sts!='2'",g_dd_mst.ac_id);
				strcpy( g_pub_tx.reply,"D247");
				rem.rem[50]='\0';
				sprintf(msg,"%-50s",rem.rem);
				set_zd_data("0130",msg);
				vtcp_log(" MSG=[%s]",msg);
				goto ErrExit;
			}
			sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
					g_dd_mst.bal,g_dd_parm.min_bal );
			WRITEMSG
			strcpy(g_pub_tx.reply,"A022" );
			goto ErrExit;
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
	if( g_dd_mst.bal<-0.005 && g_pub_tx.add_ind[0]!='1' && g_dd_mst.od_ind[0]!='Y' )
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
	vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_in_mst_check()
{
	struct com_item_c v_com_item;
	int ret;
	/*** �����ڲ���Ʒ���������麯�� ***/

	/*** �ڲ����ļ���� ***/
	ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id='%ld' and ac_seqn='%d' ",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	strcpy( g_pub_tx.opn_br_no,g_in_mst.opn_br_no );
	set_zd_data("0020", g_pub_tx.opn_br_no);
	strcpy(g_pub_tx.name,g_in_mst.name);
	/**��ȡ��Ʒ����**/
	ret = In_parm_Sel( g_pub_tx.reply,&g_in_parm,
			" prdt_no='%s' ",g_in_mst.prdt_no );
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_in_parm.dc_code); 			/*��ƴ���*/
	strcpy(g_pub_tx.prdt_code,g_in_mst.prdt_no); 			/*��ƴ���*/

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

	if( g_pub_tx.add_ind[0]=='0' )	/*��*/
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
			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

			strcpy(g_pub_tx.add_ind,"0");
		}
		else
		{
			/*** ���ü�����Ϣ�������� ***/
			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
			strcpy(g_pub_tx.add_ind,"1");
		}
	}
	else if ( g_pub_tx.add_ind[0]=='1' )	/*��*/
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
			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
			strcpy(g_pub_tx.add_ind,"1");
		}
		else
		{
			/*** ���ü�����Ϣ�������� ***/
			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
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
			sprintf(acErrMsg,"���˻�������͸֧![%.2f][%.2f]",
				g_in_mst.bal,g_pub_tx.tx_amt1);
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

OkExit:
	return 0;
ErrExit:
	return 1;
}
/**�Կ�Ŀ����**/
int Chg_acchrt_mst_check(int *ass_ind)
{
	int ret=0; 
	double amt;
	char wherelist[200];
	int i=0;
	char TmpAdd_ind[2];

	struct gl_mst_c v_gl_mst;
	struct com_item_c v_com_item;
	struct tx_dc_br_rel_c v_tx_dc_br_rel;

	memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&v_com_item,0x00,sizeof(struct com_item_c));

	sprintf(acErrMsg,"��ȡ��־=[%s]!",g_pub_tx.add_ind);
	WRITEMSG

	strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );
	set_zd_data("0020", g_pub_tx.opn_br_no);
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

	switch(g_pub_tx.add_ind[0])					/*���׽������*/
	{	
		case '0': /**�跽����**/
			if(v_com_item.amt_dc_ind[0]=='2')
			{
				sprintf(acErrMsg,"�˿�Ŀֻ�ܼǴ�����![%s]",g_pub_tx.ac_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"A025");
				goto ErrExit;
			}
			if(v_com_item.dc_ind[0]=='1' || v_com_item.dc_ind[0]=='0')
				strcpy(TmpAdd_ind,"1");		/** ���� **/
			else
				strcpy(TmpAdd_ind,"0");		/** ���� **/
			break;
		case '1': /*��������*/
			if(v_com_item.amt_dc_ind[0]=='1')
			{
				sprintf(acErrMsg,"�˿�Ŀֻ�ܼǽ跽��![%s]",g_pub_tx.ac_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"A024");
				goto ErrExit;
			}
			if(v_com_item.dc_ind[0]=='2')
				strcpy(TmpAdd_ind,"1");		/** ���� **/
			else
				strcpy(TmpAdd_ind,"0");		/** ���� **/
			break;
		default:
			sprintf(acErrMsg,"��ȡ��־����=[%s]!",g_pub_tx.add_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P106");
			goto ErrExit;
			break;
	}

	if( v_com_item.in_ind[0]=='A' )		/*** �ÿ�Ŀ�ǿ��˿�Ŀ***/
		/**�����ǲ��ǿ��˿�Ŀ������ˮ modify by wanglei 20061226
		*ass_ind=1;
		**/
		*ass_ind=0;
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


static pub_sub_ac_check()
{
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct prdt_ac_id_c prdt_ac_id_c;
	struct prdt_ac_id_c s_prdt_ac_id;
	struct mdm_attr_c s_mdm_attr;
	char v_ac_get_ind[2];
	int ret=0;

	v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];

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

sprintf(acErrMsg,"ac_seqn = [%d][%s] ",g_pub_tx.ac_seqn,g_pub_tx.cur_no);
WRITEMSG

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

	/**����״̬���**/
	if( g_pub_tx.ac_wrk_ind[7]=='J' )
	{
		strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
	}
	else if( g_pub_tx.ac_get_ind[0]=='1' )
	{
		strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);

		if( g_mdm_ac_rel.note_sts[0]=='1' 
			&& strcmp(g_pub_tx.tx_code,"Z016") 
			&& strcmp(g_pub_tx.tx_code,"9001") 
			&& g_pub_tx.add_ind[0]=='0' 
			&& g_pub_tx.ac_wrk_ind[8]!='1')
		{
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
		else if( g_mdm_ac_rel.note_sts[0]=='3' )
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

	/*** û�����벻����ͨ��,�ж�ǩ�����ܼ�� ***/
	if( strcmp(g_pub_tx.tx_br_no,"99999") && g_pub_tx.ac_wrk_ind[8]!='1'
	&& strcmp(g_pub_tx.tx_code,"4333")
	&& strcmp(g_pub_tx.tx_code,"4334")
	&& strcmp(g_pub_tx.tx_code,"4335")
	&& strcmp(g_pub_tx.tx_code,"5807")
	&& strcmp(g_pub_tx.tx_code,"6388")/**����������ҵ�û�С����޸� 20130602**/
	&& strcmp(g_pub_tx.tx_code,"6389")/**����������ҵ�û������޸� 20130602**/
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

int pub_sub_prdt_chk_check()
{
	int ret=0;

	if( g_pub_tx.svc_ind!=1003 && g_pub_tx.svc_ind!=1004 )	/** ���� **/
		goto GoodExit;

	if( g_pub_tx.svc_ind==1003 )
	{
		ret=sql_count("mo_prot_ac_rel"," (main_ac_id=%ld and main_ac_seqn=%d) or (app_ac_id=%ld and app_ac_seqn=%d) and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
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


pub_sub_mst_check()
{
	int ret=0 ;
	/*** ��鴦��ֻ���,��ϸ�� ***/
    sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
    WRITEMSG
	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':	
			ret=Chg_dd_mst_check();
			ERR_DEAL
			break;
		case '9':	
			ret=Chg_in_mst_check();
			ERR_DEAL
			break;
		case '2':	
		case '3':	
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
pub_sub_spe_do_check()
{
	int ret;
	int cnt1,cnt2;
	struct pub_tx v_pub_tx;
	memset(&v_pub_tx, 0 , sizeof(v_pub_tx));/* add by LiuHuafeng 2006-8-10 19:03 */

	/** �������ú��ּ���(���߼��˽��׳���) **/
	vtcp_log("g_pub_tx.tx_code=[%s]",g_pub_tx.tx_code);
	if( strcmp(g_pub_tx.tel,"9999") &&
		strcmp(g_pub_tx.tx_code,"2502") && strcmp(g_pub_tx.tx_code,"LNRE") &&
		strcmp(g_pub_tx.tx_code,"ZTSC") && strcmp(g_pub_tx.tx_code,"7711") &&
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
			strcpy( g_pub_tx.opn_br_no,v_pub_tx.opn_br_no );
		}
		else
		{
		}
	}
	return 0;
ErrExit:
	return 1;
}

/*
 *int pub_sub_chkadd(double ys_bal,double tx_amt1)
 *{
 *	int ret=0;
 *	double tmpamt=0.00;
 *	double tmpamt1=0.00;
 *	double tmpamt2=0.00;
 *
 *	**��ʱTEMP*
 *	if( !strncmp(g_pub_tx.tx_code,"9001",4) ) 
 *		return 0;
 *
 *	ret=sql_sum_double("trace_log","amt",&tmpamt, 
 *		"sts='0' and (sub_tx_code='D099' or sub_tx_code='A017') and tx_code!='6604' and ac_id=%d and ac_seqn=%d and tx_br_no!='%s' and saving_notarize='0' ",
 *		g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.tx_br_no);
 *	if( ret )
 *	{
 *		sprintf(acErrMsg,"���㱾�����ȡ���ʧ��!");
 *		WRITEMSG
 *		strcpy(g_pub_tx.reply,"P295");
 *		goto ErrExit;
 *	}
 *	if( pub_base_CompDblVal(tmpamt,0.00)==0 )
 *	{
 *		sprintf(acErrMsg,"û����Ҫȷ�ϵĴ��!");
 *		WRITEMSG
 *		return 0;
 *	}
 *
 *	ret=sql_sum_double("trace_log","amt",&tmpamt1,
 *	"sts='0' and ac_id=%d and ac_seqn=%d and (((sub_tx_code='D099' or sub_tx_code='A017') and (tx_br_no='%s' or saving_notarize='1')) or sub_tx_code='D002' or tx_code='6604' ) ",
 *		g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.tx_br_no); 
 *	if( ret )
 *	{
 *		sprintf(acErrMsg,"���㱾�����ȡ���ʧ��!");
 *		WRITEMSG
 *		strcpy(g_pub_tx.reply,"P295");
 *		goto ErrExit;
 *	}
 *
 *	ret=sql_sum_double("trace_log","amt",&tmpamt2,
 *		"sts='0' and (sub_tx_code='D003' or sub_tx_code='A016') and ac_id=%ld and ac_seqn=%d ",
 *		g_pub_tx.ac_id,g_pub_tx.ac_seqn); 
 *	if( ret )
 *	{
 *		sprintf(acErrMsg,"���㱾�����ȡ���ʧ��!");
 *		WRITEMSG
 *		strcpy(g_pub_tx.reply,"P295");
 *		goto ErrExit;
 *	}
 *
 *	vtcp_log("ys_bal=[%lf],tmpamt1=[%lf],tx_amt1=[%lf],tmpamt2=[%f]",
 *		ys_bal,tmpamt1,tx_amt1,tmpamt2);
 *
 *	if( pub_base_CompDblVal(ys_bal+tmpamt1,tx_amt1+tmpamt2)<0 )
 *	{
 *		sprintf(acErrMsg,"�������ȡ���㣬����������ȷ�ϴ��!");
 *		WRITEMSG
 *		strcpy(g_pub_tx.reply,"P296");
 *		goto ErrExit;
 *	}
 *
 *	return 0;
 *ErrExit:
 *	return 1;
 *}
 */
