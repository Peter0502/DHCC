/*************************************************
* �� �� ��:     gD0033.c
* ����������    �������˻���Ϣ
* ��    ��:
* ������ڣ�    2012��02��29��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:

insert into com_eod_exec values('1039','Z037','gD0033','�������˻���Ϣ','1111111111','5','3','21','21','2','Y','Y','N','2','Y');
*************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include<stdio.h>
#define EXTERN
#include "public.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "sub_com_sect_acm_c.h"
#include "sub_intst_hst_c.h"
#include "intst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#define  INTST_TERM_TYPE_2  "0"
#define  INTST_TERM_TYPE_2  "0"
#define  ACM_CALC_TYPE_2    "2"
#define  INTST_MOON    3
#define  INTST_DAY     20

gD0033()
{
	int ret = -1 ;
	vtcp_log("�������˻���Ϣ��ʼ------------");

	ret=sql_begin(); /* ������ */
	if ( ret )
	{
		vtcp_log("������ʧ��! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT03");
		goto ErrExit;
	}

	pub_base_sysinit(); /* ��ʼ��ȫ�ֱ��� */

	if(pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		vtcp_log("ȡ��������ˮ���� [%d] [%s]",g_pub_tx.trace_no,g_pub_tx.reply);
		strcpy(g_pub_tx.reply,"P298");
		goto ErrExit;
	}

	ret=sub_dd_mst_cal();
	if(ret)
	{
		vtcp_log("�������˻���Ϣʧ��! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D158");
		goto ErrExit;
	}

	vtcp_log("�������˻���Ϣ����------------");

OkExit:
	sql_commit(); /* �ύ���� */
	strcpy(g_pub_tx.reply,"0000");
  vtcp_log("�������˻���Ϣ����ɹ�!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(0);
ErrExit:
	if(strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0] == '\0')
	{
		strcpy(g_pub_tx.reply, "G010");
	}
	sql_rollback();		/* ����ع� */
	vtcp_log("�������˻���Ϣ����ʧ��!!!!![%s]", g_pub_tx.reply);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return(1);
}

/* �������˻���Ϣ by Wang Yongwei */
int sub_dd_mst_cal()
{
	int ret = 0;
	int days=0;
	long	lTrace_cnt=0;
	int sig_tot_num=0.00;
	int jx_seqn=0;
	long tot_num=0;

	double tot_intst=0.00;
	double sig_tot_intst=0.00;
	double rate_val=0.00;
	double sect_intst=0.00;
	double mst_intst=0.00;
	double sum_intst=0.00;
	double sig_tot_acm=0.00;
	double tmp_bal=0.00;
	char   main_ac_no[25];
	char   tmp_tx_br_no[6];
	struct sub_dd_mst_c sSub_dd_mst;
	struct sub_dd_mst_c mSub_dd_mst;
	struct sub_dd_mst_c lSub_dd_mst;
	struct sub_dd_mst_hst_c sSub_dd_mst_hst;
	struct sub_com_sect_acm_c sSub_com_sect_acm;
	struct sub_intst_hst_c    sSub_intst_hst;
	struct intst_hst_c        mintst_hst_c;
	struct mdm_ac_rel_c    sMdm_ac_rel;
	struct dd_mst_c    sDd_mst;
	struct dd_parm_c   sDd_parm;
	memset(main_ac_no,0,sizeof(main_ac_no));
	memset(tmp_tx_br_no,0x00,sizeof(tmp_tx_br_no));
	set_zd_long(DC_TRACE_CNT,lTrace_cnt);
	ret = pub_base_getllz(HQLLBH, "01", g_pub_tx.tx_date,&rate_val);
	if(ret)
	{
		vtcp_log(" %s, %d, ȡ���ʱ�Ŵ���!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O195");
		goto ErrExit;
	}
	/* ��δ������Ϣ����Ϊ���ա��¡�����Ϣ���˻����н�Ϣ���� */
	ret = Sub_dd_mst_Dec_Sel(g_pub_tx.reply,"ac_sts<>'*' and intst_type in('0','2','3','4') and ic_date<>%ld  order by ac_no",g_pub_tx.tx_date);
	if(ret)
	{
		vtcp_log(" %s, %d, ��ѯ�����˻�ʧ�ܣ�",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"SN12");
		goto ErrExit;
	}
	while(1)
	{
		sect_intst=0.00;
		mst_intst=0.00;
		memset(&sSub_dd_mst,0,sizeof(sSub_dd_mst));
		memset(&lSub_dd_mst,0,sizeof(lSub_dd_mst));
		memset(&sSub_dd_mst_hst,0,sizeof(sSub_dd_mst_hst));
		ret = Sub_dd_mst_Fet_Sel(&lSub_dd_mst,g_pub_tx.reply);
		if(ret==100)
		{
			vtcp_log(" %s, %d, �����˻���Ϣ��Ϲ���Ϣ���[%.2lf],����%d��",__FILE__,__LINE__,tot_intst,tot_num);
			break;
		}else if(ret)
		{
			vtcp_log(" %s, %d, ��ѯ�����˻�ʧ�ܣ�",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		ret = Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%d",lSub_dd_mst.ac_no,lSub_dd_mst.sub_ac_seqn);
		if(ret)
		{
			vtcp_log(" %s, %d, ��ѯ�����˻�ʧ�ܣ�",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN12");
			goto ErrExit;
		}
		ret = Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log(" %s, %d, ��ѯ�����˻�ʧ�ܣ�",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN12");
			goto ErrExit;
		}
		/* ���ǲ���Ϣ���˻�ֱ����ջ��� 20110319 by Wang Yongwei*/
		if(sSub_dd_mst.intst_type[0]=='0')
		{
			sSub_dd_mst.intst_acm=0.00;
			sSub_dd_mst.lst_date=g_pub_tx.tx_date;
			ret = Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
			if(ret)
			{
				Sub_intst_hst_Debug(&sSub_intst_hst);
				vtcp_log(" %s, %d, ��ѯsub_dd_mstʧ��!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			Sub_dd_mst_Clo_Upd();
			continue;
		}
		vtcp_log("[%s] [%d] main_ac_no[%s] sSub_dd_mst.ac[%s]",__FILE__,__LINE__,main_ac_no,sSub_dd_mst.ac_no);
		if(memcmp(main_ac_no,sSub_dd_mst.ac_no,strlen(sSub_dd_mst.ac_no)))
		{
			if(strlen(main_ac_no)>0)
			{

				memset(&sMdm_ac_rel,0,sizeof(sMdm_ac_rel));
				memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
				memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
				memset(&mintst_hst_c,0x00,sizeof(struct intst_hst_c));

				ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",main_ac_no);
				if(ret)
				{
					vtcp_log(" %s, %d, �����˻�������![%s]",__FILE__,__LINE__,main_ac_no);
					strcpy(g_pub_tx.reply,"P102");
					goto ErrExit;
				}

				vtcp_log("�������˻���Ϣ �ʺ�:[%s],�ܱ���[%ld]����Ϣ[%lf]���˻���Ϣ[%lf] [%s] [%d]"\
				,__FILE__,__LINE__,main_ac_no,sig_tot_num,sig_tot_intst,__FILE__,__LINE__);

				ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
				if(ret)
				{
					vtcp_log("�������ļ���ѯ ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P102");
					goto ErrExit;
				}

				ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
				if(ret)
				{
					vtcp_log("���ڲ�Ʒ�������ѯ ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"D157");
					goto ErrExit;
				}

				strncpy(mintst_hst_c.br_no,sDd_mst.opn_br_no,strlen(sDd_mst.opn_br_no));
				mintst_hst_c.ac_id=sDd_mst.ac_id;
				mintst_hst_c.ac_seqn=sDd_mst.ac_seqn;
				strncpy(mintst_hst_c.prdt_no,sDd_mst.prdt_no,strlen(sDd_mst.prdt_no));
				strncpy(mintst_hst_c.cur_no,sDd_parm.cur_no,strlen(sDd_parm.cur_no));
				mintst_hst_c.bic_bal=sDd_mst.bal;
				mintst_hst_c.bic_acm=sig_tot_acm;
				mintst_hst_c.fct_intst=sig_tot_intst;
				mintst_hst_c.intst_tax=0.00;
				mintst_hst_c.aic_bal=sDd_mst.bal+sig_tot_intst;
				mintst_hst_c.aic_acm=0.00;
				mintst_hst_c.ic_date=sDd_mst.ic_date;
				mintst_hst_c.intst_date=g_pub_tx.tx_date;
				mintst_hst_c.ac_ind[0]='0';
				mintst_hst_c.pt_ind[0]='1';
				mintst_hst_c.rate=sDd_mst.rate;

				ret=Intst_hst_Ins(mintst_hst_c,g_pub_tx.reply);
				if(ret)
				{
					vtcp_log("�����Ϣ�Ǽǲ� ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					Intst_hst_Debug(&mintst_hst_c);
					strcpy(g_pub_tx.reply,"AT03");
					goto ErrExit;
				}

				strcpy(tmp_tx_br_no,g_pub_tx.tx_br_no);
				strcpy(g_pub_tx.tx_br_no,sDd_mst.opn_br_no);
				strcpy(g_pub_tx.opn_br_no,sDd_mst.opn_br_no);
				strcpy(g_pub_tx.ac_wrk_ind,"00000000");
				g_pub_tx.svc_ind=1005;
				strcpy(g_pub_tx.ct_ind,"2");
				strcpy(g_pub_tx.add_ind,"1");
				strcpy(g_pub_tx.prdt_code,sDd_mst.prdt_no );
				strcpy(g_pub_tx.ac_no,main_ac_no);
				g_pub_tx.ac_id=sDd_mst.ac_id;
				g_pub_tx.ac_seqn=sDd_mst.ac_seqn;
				strcpy(g_pub_tx.note_type,"");
				strcpy(g_pub_tx.beg_note_no,"" );
				g_pub_tx.tx_amt1=sig_tot_intst;
				strcpy(g_pub_tx.no_show,"0");
				strcpy(g_pub_tx.cur_no,sDd_parm.cur_no);
				strcpy(g_pub_tx.ac_get_ind,"00");
				strcpy(g_pub_tx.ac_id_type,"1");
				strcpy(g_pub_tx.hst_ind,"1");
				strcpy(g_pub_tx.brf,"��Ϣ");
				strcpy(g_pub_tx.tx_code,"Z037");
				strcpy(g_pub_tx.sub_tx_code,"D099");

				ret=pub_acct_trance();
				if(ret)
				{
					vtcp_log("�Ǽǽ�����ˮʧ�� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P126");
					goto ErrExit;
				}


        set_zd_data("1011", g_pub_tx.ac_no);
        set_zd_int("1012", &g_pub_tx.ac_seqn);
        set_zd_data("101A", g_pub_tx.cur_no);
        set_zd_data("1014", g_pub_tx.note_type);
        set_zd_data("1015", g_pub_tx.beg_note_no);
        set_zd_data("1015", g_pub_tx.end_note_no);
        set_zd_data("101B", g_pub_tx.ct_ind);
        set_zd_data("1016", g_pub_tx.brf);
				set_zd_double("1013",sig_tot_intst);

        ret = pubf_acct_proc("D099");
				if(ret)
				{
					vtcp_log("��Ϣ�����˻�ʧ�� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P126");
					goto ErrExit;
				}

				get_zd_data("0153",g_pub_tx.ac_no);
				g_pub_tx.ac_id = 0;
       	g_pub_tx.ac_seqn = 0;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.ac_id_type,"9");
        strcpy(g_pub_tx.add_ind,"0");
    		strcpy(g_pub_tx.ac_wrk_ind,"00000000");
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=9001;
        g_pub_tx.tx_amt1 = sig_tot_intst;
				strcpy(g_pub_tx.brf,"�Թ���Ϣ");
				strcpy(g_pub_tx.sub_tx_code,"A016");

        ret=pub_acct_trance();
        if( ret )
        {
					vtcp_log("��Ϣ֧����Ŀ���˴���!! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
          strcpy(g_pub_tx.reply,"P126");
					goto ErrExit;
        }

        set_zd_data("1204",g_pub_tx.cur_no);
        set_zd_data("1205",g_pub_tx.ct_ind);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        ret = pubf_acct_proc("A016");
        if (ret != 0)
        {
            vtcp_log("��Ƽ��ʲ��ɹ�!! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"P126");
            goto ErrExit;
        }

				ret=sql_execute("update dd_mst set ic_date=%ld,lst_date=%ld,intst_acm=0 where ac_id=%ld",\
				g_pub_tx.tx_date,g_pub_tx.tx_date,sDd_mst.ac_id);
				if(ret)
				{
					vtcp_log("���»������ļ� ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"AT03");
					goto ErrExit;
				}
				strcpy(g_pub_tx.tx_br_no,tmp_tx_br_no);

			}else if(tot_num>0 && strlen(main_ac_no)==0){
				vtcp_log(" %s, %d, �������˻�������![%s]",__FILE__,__LINE__,main_ac_no);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			sig_tot_num=0;
			sig_tot_intst=0.00;
			sig_tot_acm=0.00;
		}
		memcpy(main_ac_no,sSub_dd_mst.ac_no,sizeof(main_ac_no)-1);
		/******��������˻��ֶλ���,��δ��Ϣ��¼���н�Ϣ*****/
		ret = Sub_com_sect_acm_Dec_Upd(g_pub_tx.reply,
		    "sub_ac_no='%s' and sts='0'",sSub_dd_mst.sub_ac_no);

		MYSQLERR
		while (1) {
			memset(&sSub_com_sect_acm,0,sizeof(sSub_com_sect_acm));
			ret = Sub_com_sect_acm_Fet_Upd(&sSub_com_sect_acm, g_pub_tx.reply);
			if (ret == 100)
				break;
			MYSQLERR
			memset(&sSub_intst_hst, 0, sizeof(sSub_intst_hst));
			sSub_intst_hst.bic_acm = sSub_com_sect_acm.intst_acm;
			sSub_intst_hst.ic_date = sSub_com_sect_acm.beg_date;
			sSub_intst_hst.intst_date = sSub_com_sect_acm.end_date;
			strcpy(sSub_intst_hst.br_no, sSub_dd_mst.br_no);
			strcpy(sSub_intst_hst.sub_ac_no, sSub_com_sect_acm.sub_ac_no);

			sSub_intst_hst.bic_bal = sSub_dd_mst.bal+sect_intst;
			sSub_intst_hst.bic_acm = sSub_com_sect_acm.intst_acm;
			/****Ҫ��Ϊ����****/

			sSub_intst_hst.fct_intst = pub_base_PubDround_1(sSub_com_sect_acm.intst_acm * sSub_com_sect_acm.rate /12/30/100.00);
			sSub_intst_hst.intst_tax=0.00;
			sect_intst +=(sSub_intst_hst.fct_intst-sSub_intst_hst.intst_tax);/* �ϼƷֶ���Ϣ */
			sSub_intst_hst.aic_bal = sSub_dd_mst.bal + sect_intst;
			sSub_intst_hst.aic_acm = 0.00;
			sSub_intst_hst.sub_ac_seqn = sSub_com_sect_acm.sub_ac_seqn;
			sSub_intst_hst.jx_date = g_pub_tx.tx_date;
			strcpy(sSub_intst_hst.ac_no, sSub_com_sect_acm.ac_no);

			strcpy(sSub_intst_hst.ac_ind, "A");
			strcpy(sSub_intst_hst.pt_ind, "1");
			sSub_intst_hst.intst_flag[0]='2'; /*�����Ƿֶλ���������Ϣ */
			sSub_intst_hst.intst_sts[0]='0';  /* ���� */
			sSub_intst_hst.rate = sSub_com_sect_acm.rate;


			if (pub_base_CompDblVal(sSub_intst_hst.fct_intst, 0.00) != 0) {
				ret = Sub_intst_hst_Ins(sSub_intst_hst, g_pub_tx.reply);
				MYSQLERR
			}
			sSub_com_sect_acm.sts[0] = '1'; /* �ֶλ����ó��ѽ�Ϣ״̬ */
			ret = Sub_com_sect_acm_Upd_Upd(sSub_com_sect_acm, g_pub_tx.reply);
			MYSQLERR
		}
		Sub_com_sect_acm_Clo_Upd();
		/******����ֶλ���over*****/
		/* �������ڵĻ������н�Ϣ */
		if(sSub_dd_mst.rate_type[0]=='1')
		{
			/* �Ż����� */
		}else{
			sSub_dd_mst.rate= rate_val;
		}
		/* ������Ϣ,�Թ���û����Ϣ˰���ô�����Ϣ˰�ͱ�ֵ�� */
		memset(&sSub_intst_hst, 0, sizeof(sSub_intst_hst));
		/***��������ĺ���������Ϣ
		days= pub_base_CalTrueDays(sSub_dd_mst.lst_date,g_pub_tx.tx_date);
		tmp_bal = pub_base_floor(sSub_dd_mst.bal);
		***/

		/* ������� */
		if (pub_base_CalAcm(sSub_dd_mst.lst_date, g_pub_tx.tx_date,\
			 INTST_TERM_TYPE_2, ACM_CALC_TYPE_2,\
				    sSub_dd_mst.bal, 0.00, &sSub_dd_mst.intst_acm,\
				    sSub_dd_mst.intst_type,INTST_MOON , INTST_DAY))
		{
			sprintf(acErrMsg, "������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}
		mst_intst=sSub_dd_mst.intst_acm * sSub_dd_mst.rate/360/100;
		mst_intst=pub_base_PubDround_1(mst_intst);
		/* ��ʼ�Ǽ�Sub_intst_hst */

		memcpy(sSub_intst_hst.ac_no,sSub_dd_mst.ac_no,sizeof(sSub_intst_hst.ac_no)-1);
		memcpy(sSub_intst_hst.sub_ac_no,sSub_dd_mst.sub_ac_no,sizeof(sSub_intst_hst.sub_ac_no)-1);
		sSub_intst_hst.sub_ac_seqn=sSub_dd_mst.sub_ac_seqn;
		memcpy(sSub_intst_hst.br_no,sSub_dd_mst.br_no,sizeof(sSub_intst_hst.br_no)-1);
		sSub_intst_hst.bic_bal = sSub_dd_mst.bal+sect_intst;
		sSub_intst_hst.bic_acm = sSub_dd_mst.intst_acm;
		sSub_intst_hst.fct_intst=mst_intst;
		sSub_intst_hst.intst_tax=0.00;
		sSub_intst_hst.aic_bal=sSub_dd_mst.bal+sect_intst+mst_intst;
		sSub_intst_hst.aic_acm=0.00;
		sSub_intst_hst.ic_date=sSub_dd_mst.ic_date;
		sSub_intst_hst.intst_date=g_pub_tx.tx_date;
		sSub_intst_hst.ac_ind[0]='A'; /* �����˻� */
		sSub_intst_hst.pt_ind[0]='1';
		sSub_intst_hst.rate=sSub_dd_mst.rate;
		sSub_intst_hst.jx_date=g_pub_tx.tx_date;
		sSub_intst_hst.intst_flag[0]='1'; /*����������Ļ���������Ϣ */
		sSub_intst_hst.intst_sts[0]='0';  /* ���� */

		if(0)
		{
			vtcp_log(" %s, %d, WangYongwei",__FILE__,__LINE__);
			Sub_intst_hst_Debug(&sSub_intst_hst);
		}
		ret = Sub_intst_hst_Ins(sSub_intst_hst, g_pub_tx.reply);
		MYSQLERR

		sSub_dd_mst.intst_acm = 0.00;
		sSub_dd_mst.ic_date=g_pub_tx.tx_date;
		sSub_dd_mst.lst_date=g_pub_tx.tx_date;
		sSub_dd_mst.bal+=sect_intst+mst_intst;
		/* ��ʼ�Ǽ����˻���ϸ */
		memset(&sMdm_ac_rel,0,sizeof(sMdm_ac_rel));
		memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",main_ac_no);
		if(ret)
		{
			vtcp_log(" %s, %d, �����˻�������![%s]",__FILE__,__LINE__,main_ac_no);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}

		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
		if(ret)
		{
			vtcp_log("�������ļ���ѯ ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}

		ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
		if(ret)
		{
			vtcp_log("���ڲ�Ʒ�������ѯ ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D157");
			goto ErrExit;
		}

		sSub_dd_mst.hst_cnt++;
		strcpy(g_pub_tx.tx_br_no,sSub_dd_mst.br_no);
		strcpy(g_pub_tx.opn_br_no,sSub_dd_mst.br_no);
		strcpy(g_pub_tx.brf,"�����˻���Ϣ");
		strcpy(g_pub_tx.add_ind,"1");
		g_pub_tx.ct_ind[0]='2';
		g_pub_tx.tx_amt1=sect_intst+mst_intst;
		ret=pub_base_ins_sub_ac_hst(sSub_dd_mst);
		if(ret)
		{
			vtcp_log(" %s, %d, �ǼǶ����˻���ϸʧ��!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"H034");
			goto ErrExit;
		}

		strcpy(g_pub_tx.ac_wrk_ind,"0100000J");
		g_pub_tx.svc_ind=1201;
		g_pub_tx.hst_ind[0]='1';
		strcpy(g_pub_tx.prdt_code,sDd_mst.prdt_no);
		strcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no);
		g_pub_tx.ac_id=sSub_dd_mst.sub_ac_seqn;
		g_pub_tx.ac_seqn=sMdm_ac_rel.ac_seqn;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"" );
		strcpy(g_pub_tx.no_show,"0");
		strcpy(g_pub_tx.cur_no,"01");
		strcpy(g_pub_tx.ac_get_ind,"00");
		strcpy(g_pub_tx.ac_id_type,"1");
		strcpy(g_pub_tx.brf,"�����˻���Ϣ");
		strcpy(g_pub_tx.tx_code,"Z037");
		strcpy(g_pub_tx.sub_tx_code,"Z037");

		ret=pub_ins_trace_log();
		if(ret)
		{
			vtcp_log(" %s, %d, �Ǽ�trace_logʧ��!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"H034");
			goto ErrExit;
		}

		sSub_dd_mst.lst_date=g_pub_tx.tx_date;
		sSub_dd_mst.ic_date=g_pub_tx.tx_date;
		ret= Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log(" %s, %d, ���¶����˻�����ʧ��!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		Sub_dd_mst_Clo_Upd();
		/*** add by chenchao ****
		get_zd_long(DC_TRACE_CNT,&lTrace_cnt);
		lTrace_cnt++;
		set_zd_long(DC_TRACE_CNT,lTrace_cnt);
		*** add by chenchao ****/

		sig_tot_num++;
		sig_tot_intst+=sect_intst+mst_intst;  /* ͳ��һ�������˻��ļ�Ϣ��� */
		sig_tot_acm+=sSub_intst_hst.bic_acm;
		tot_num++;
		tot_intst+=sect_intst+mst_intst; /* �ۼƽ�Ϣ��� */
	}
	/* ��������ʱ�������˻�����Ϣ�������Ҫ���� */
	if(sig_tot_intst>0.001 && sig_tot_num>0)
	{
		vtcp_log(" %s, %d, ����һ���������˻���Ϣ���,�ܱ���[%ld]����Ϣ[%lf]���˻���Ϣ[%lf]"\
		,__FILE__,__LINE__,sig_tot_num,sig_tot_intst,sum_intst);
		memset(&sMdm_ac_rel,0,sizeof(sMdm_ac_rel));
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",main_ac_no);
		if(ret)
		{
			vtcp_log(" %s, %d, �����˻�������![%s]",__FILE__,__LINE__,main_ac_no);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}

		vtcp_log("�������˻���Ϣ �ʺ�:[%s],�ܱ���[%ld]����Ϣ[%lf]���˻���Ϣ[%lf] [%s] [%d]"\
		,__FILE__,__LINE__,main_ac_no,sig_tot_num,sig_tot_intst,__FILE__,__LINE__);

		memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
		if(ret)
		{
			vtcp_log("�������ļ���ѯ ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}

		ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
		if(ret)
		{
			vtcp_log("���ڲ�Ʒ�������ѯ ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D157");
			goto ErrExit;
		}

		memset(&mintst_hst_c,0x00,sizeof(struct intst_hst_c));
		strncpy(mintst_hst_c.br_no,sDd_mst.opn_br_no,strlen(sDd_mst.opn_br_no));
		mintst_hst_c.ac_id=sDd_mst.ac_id;
		mintst_hst_c.ac_seqn=sDd_mst.ac_seqn;
		strncpy(mintst_hst_c.prdt_no,sDd_mst.prdt_no,strlen(sDd_mst.prdt_no));
		strncpy(mintst_hst_c.cur_no,sDd_parm.cur_no,strlen(sDd_parm.cur_no));
		mintst_hst_c.bic_bal=sDd_mst.bal;
		mintst_hst_c.bic_acm=sig_tot_acm;
		mintst_hst_c.fct_intst=sig_tot_intst;
		mintst_hst_c.intst_tax=0.00;
		mintst_hst_c.aic_bal=sDd_mst.bal+sig_tot_intst;
		mintst_hst_c.aic_acm=0.00;
		mintst_hst_c.ic_date=sDd_mst.ic_date;
		mintst_hst_c.intst_date=g_pub_tx.tx_date;
		mintst_hst_c.ac_ind[0]='0';
		mintst_hst_c.pt_ind[0]='1';
		mintst_hst_c.rate=sDd_mst.rate;

		ret=Intst_hst_Ins(mintst_hst_c,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("�����Ϣ�Ǽǲ� ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			Intst_hst_Debug(&mintst_hst_c);
			strcpy(g_pub_tx.reply,"AT03");
			goto ErrExit;
		}

		strcpy(tmp_tx_br_no,g_pub_tx.tx_br_no);
		strcpy(g_pub_tx.tx_br_no,sDd_mst.opn_br_no);
		strcpy(g_pub_tx.opn_br_no,sDd_mst.opn_br_no);
		strcpy(g_pub_tx.ac_wrk_ind,"00000000");
		g_pub_tx.svc_ind=1005;
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.prdt_code,sDd_mst.prdt_no );
		strcpy(g_pub_tx.ac_no,main_ac_no);
		g_pub_tx.ac_id=sDd_mst.ac_id;
		g_pub_tx.ac_seqn=sDd_mst.ac_seqn;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"" );
		g_pub_tx.tx_amt1=sig_tot_intst;
		strcpy(g_pub_tx.no_show,"0");
		strcpy(g_pub_tx.cur_no,sDd_parm.cur_no);
		strcpy(g_pub_tx.ac_get_ind,"00");
		strcpy(g_pub_tx.ac_id_type,"1");
		strcpy(g_pub_tx.hst_ind,"1");
		strcpy(g_pub_tx.brf,"��Ϣ");
		strcpy(g_pub_tx.tx_code,"Z037");
		strcpy(g_pub_tx.sub_tx_code,"D099");

		ret=pub_acct_trance();
		if(ret)
		{
			vtcp_log("�Ǽǽ�����ˮʧ�� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

    set_zd_data("1011", g_pub_tx.ac_no);
    set_zd_int("1012", &g_pub_tx.ac_seqn);
    set_zd_data("101A", g_pub_tx.cur_no);
    set_zd_data("1014", g_pub_tx.note_type);
    set_zd_data("1015", g_pub_tx.beg_note_no);
    set_zd_data("1015", g_pub_tx.end_note_no);
    set_zd_data("101B", g_pub_tx.ct_ind);
    set_zd_data("1016", g_pub_tx.brf);
		set_zd_double("1013",sig_tot_intst);

    ret = pubf_acct_proc("D099");
		if(ret)
		{
			vtcp_log("��Ϣ�����˻�ʧ�� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

		get_zd_data("0153",g_pub_tx.ac_no);
		g_pub_tx.ac_id = 0;
   	g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ac_get_ind,"00");
    strcpy(g_pub_tx.ac_id_type,"9");
    strcpy(g_pub_tx.add_ind,"0");
		strcpy(g_pub_tx.ac_wrk_ind,"00000000");
    strcpy(g_pub_tx.hst_ind,"1");
    g_pub_tx.svc_ind=9001;
    g_pub_tx.tx_amt1 = sig_tot_intst;
		strcpy(g_pub_tx.brf,"�Թ���Ϣ");
		strcpy(g_pub_tx.sub_tx_code,"A016");

    ret=pub_acct_trance();
    if( ret )
    {
			vtcp_log("��Ϣ֧����Ŀ���˴���!! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret = pubf_acct_proc("A016");
		if (ret != 0)
		{
			vtcp_log("��Ƽ��ʲ��ɹ�!! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}

		ret=sql_execute("update dd_mst set ic_date=%ld,lst_date=%ld,intst_acm=0.00 where ac_id=%ld",\
		g_pub_tx.tx_date,g_pub_tx.tx_date,sDd_mst.ac_id);
		if(ret)
		{
			vtcp_log("���»������ļ� ����! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT03");
			goto ErrExit;
		}

		strcpy(g_pub_tx.tx_br_no,tmp_tx_br_no);

		sig_tot_num=0;
		sig_tot_intst=0.00;
	}
	Sub_dd_mst_Clo_Sel();
OkExit:
	return(0);
ErrExit:
	return(-1);
}

