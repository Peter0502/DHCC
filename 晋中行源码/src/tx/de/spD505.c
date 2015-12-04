/***************************************************************
* �� �� ��: spD505.c
* ����������0������ʻ�����(�ڲ��ʡ��Թ�����)
* ��    ��: mike
* ������ڣ�2003��3��1��
*
*    �޸ļ�¼��liuxuan
*    ��    ��: 20070713
*    �� �� ��:
*    �޸�����:
*    ������Ҫ�󶳽�״̬�������� 82������ modify by wudawei 20130425 
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "note_cheq_mst_c.h"
#include "mo_prdt_prot_c.h" /****20130715 wudawei Э������ж�****/
#include "mob_acct_type_c.h" /*add by zgf 2013-07-25*/
spD505()
{
	int ret=0;
	char ac_no[20];
	char tmp_ind1[2],tmp_ind2[2],flag[2];
	char tmp_name[50],tmp_phone[20],tmp_id_knd[1],tmp_id_no[20];
	long tmp_date1,tmp_date2;
	double tmp_amt;

	struct mo_infrm_c		g_mo_infrm;
	struct in_mst_c			f_in_mst;
	struct mdm_ac_rel_c		f_mdm_ac_rel;
	struct dd_mst_c			f_dd_mst;
	struct note_cheq_mst_c		g_note_cheq_mst;
	struct mo_prdt_prot_c g_mo_prdt_prot; /****20130715 wudawei Э������ж�****/
	struct mob_acct_type_c st_mob_acct_type;/*add by zgf 2013-07-25*/

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,'\0',sizeof(g_dd_mst)); memset(&g_in_mst,'\0',sizeof(g_in_mst));
	memset(&f_dd_mst,'\0',sizeof(f_dd_mst)); memset(&f_in_mst,'\0',sizeof(f_in_mst));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	memset(&g_note_cheq_mst,0x00,sizeof(struct note_cheq_mst_c));
	memset(&g_mo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));/****20130715 wudawei Э������ж�****/

	g_reply_int=0;
	flag[0]='0';

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* �ʺ� */
	get_zd_data("0250",tmp_name);         		/* �˻����� */
	get_zd_double("0410",&g_pub_tx.tx_amt1);     	/* �˻���� */

	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
		"ac_no='%s'",g_pub_tx.ac_no);
  	if( g_reply_int==100 )
  	{
	   sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}
	else if(g_reply_int && g_reply_int!=100)
	{
	   sprintf(acErrMsg,"������ʾ�ʺŲ�ѯ�����˻����ձ�ʧ��!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"P147");
	   goto ErrExit;
	}
	
	/****���ӹ���Э������ʻ�����ʱ���жϣ���δ��Լ����������������ȥ��Լ  add at 20130715 wudawei****/
		ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&g_mo_prdt_prot,"main_ac_id = %ld and prot_type ='XD1' and sts != '*' ", \
				g_mdm_ac_rel.ac_id);
		if(ret==0)
		{
			sprintf(acErrMsg,"���ʻ�ΪЭ������ʻ������Ƚ�Լ������!![%s] ret=[%d]",g_mdm_ac_rel.ac_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P182");
			goto ErrExit;
		}else if(ret==100){/****����ǩԼ�ʻ�������������****/
			vtcp_log("[%s][%d] ���ʻ�����Э������ʻ�!![%s] ret=[%d]",__FILE__,__LINE__,g_mdm_ac_rel.ac_no,ret);
		}else if(ret){
			sprintf(acErrMsg,"��ѯ���Э��Ǽǲ��쳣!!ret=[%d] �ʺ�=[%s]",ret,g_mdm_ac_rel.ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"X013");
			goto ErrExit;
		}
	/****end at 20130715****/
	
	  /*���ӿ��ƶ��Ŵ���ǩԼ��Ϣ���ܽ����˻�����,�����Ƚ��н�Լ----add by zgf 2013-07-25*/
	  memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
	  ret=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"ac_no='%s' and mob_sts='1' ",g_pub_tx.ac_no);
	  if(ret==100){
	          vtcp_log("[%s][%d]�˺�[%s]δ��ѯ������ǩԼ��¼,��������!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	  }else if(ret==0){
	          vtcp_log("[%s][%d]�˺�[%s]���ڶ���ǩԼ��¼!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	          sprintf(acErrMsg, "���˺Ŵ��ڶ���֪ͨҵ����δ��Լ!!");
	          strcpy(g_pub_tx.reply,"F037");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	
	  }else{
	          vtcp_log("[%s][%d]��ѯ��������ǩԼ�����!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	          sprintf(acErrMsg, "��ѯ��������ǩԼ�����!!");
	          strcpy(g_pub_tx.reply,"XXXX");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	  }
	  /*end add by zgf*/
	
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
	g_pub_tx.ct_ind[0]='2';
	if(g_pub_tx.ac_id<900000000){
		/**sprintf(acErrMsg,"�Թ�,���ڲ��ʺ�!");
		WRITEMSG
	  strcpy(g_pub_tx.reply, "A029");
		goto ErrExit;*/
		g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%d", g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if( g_reply_int )
		{
		   sprintf(acErrMsg,"�������в����ڸü�¼");
		   WRITEMSG
		   strcpy(g_pub_tx.reply,"P102");
		   goto ErrExit;
		}
		/** ������Ҫ�󶳽�״̬�������� add by wudawei 20130425 **/
		if( g_dd_mst.hold_sts[0]!='0')
		{
				sprintf(acErrMsg,"���˻����ڶ���״̬,��������!!");
		  	WRITEMSG
		    strcpy(g_pub_tx.reply,"O084");
		    goto ErrExit;
		}
		/** add end **/
			/*����Ƿ�����Ϣ����*/
		if( g_dd_mst.intst_type[0]!='0')
		if(pub_base_CompDblVal(g_dd_mst.intst_acm,0.00))
		{
		 	sprintf(acErrMsg,"���˻��д���Ϣ,��������!!");
		  	WRITEMSG
		    strcpy(g_pub_tx.reply,"O076");
		    goto ErrExit;
		}
		/* ����ƾ֤,���ڻ�ƾ֤����Ϊ�ղ������� add by martin 2008/12/22 11:56*/
		g_reply_int=Note_cheq_mst_Sel(g_pub_tx.reply,&g_note_cheq_mst, "ac_id=%ld and cnt>0 ", g_mdm_ac_rel.ac_id);
		if( g_reply_int==0 )
		{
		   sprintf(acErrMsg,"�������д���ƾ֤��¼");
		   WRITEMSG
		   strcpy(g_pub_tx.reply,"N037");
		   goto ErrExit;
		}
	}
	else{
			g_reply_int=In_mst_Sel(g_pub_tx.reply,&g_in_mst, \
				"ac_id=%ld", g_mdm_ac_rel.ac_id);
			if( g_reply_int )
			{
			   sprintf(acErrMsg,"�ڲ����в����ڸü�¼");
			   WRITEMSG
			   strcpy(g_pub_tx.reply,"W010");
			   goto ErrExit;
			}
				/*����Ƿ�����Ϣ����*/
		if(g_in_mst.intst_knd[0]!='0')
		if(pub_base_CompDblVal(g_in_mst.intst_acm,0.00))
		{
		 	sprintf(acErrMsg,"���˻��д���Ϣ,��������!!");
		  	WRITEMSG
		    strcpy(g_pub_tx.reply,"O076");
		    goto ErrExit;
		}			
	}
	/*����˻�״̬*/
	if(g_mdm_ac_rel.note_sts[0]=='*' || g_in_mst.sts[0]=='*' || g_dd_mst.ac_sts[0]=='*')
	{
	 	sprintf(acErrMsg,"���˻�������!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"P172");
	    goto ErrExit;
	}

	/*����Ƿ������*/
	g_reply_int=pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00);
	if(g_reply_int)
	{
	 	sprintf(acErrMsg,"���˻��д����,��������!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"O077");
	    goto ErrExit;
	}


	if(g_pub_tx.ac_id<900000000){	
		g_reply_int=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Dd_mst_Fet_Upd(&f_dd_mst,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_dd_mst.ac_sts,"*");
		/*****add by liuyue  20070208�����ϴν�������***/
		f_dd_mst.lst_date = g_pub_tx.tx_date;
		/****add end****/

		g_reply_int=Dd_mst_Upd_Upd(f_dd_mst,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Dd_mst_Clo_Upd( );
		/*** xxx ---- @20040907@ ---- Ϊ������ ---**/
		g_pub_tx.svc_ind=1003;
	}
	else{
		/*�޸��ڲ�̨��Ϊ����*/
		g_reply_int=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=In_mst_Fet_Upd(&f_in_mst,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_in_mst.sts,"*");
		/*****add by liuyue  20070208�����ϴν�������***/
		f_in_mst.lst_date = g_pub_tx.tx_date;
		/****add end****/

		g_reply_int=In_mst_Upd_Upd(f_in_mst,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		In_mst_Clo_Upd( );
		/*** xxx ---- @20040907@ ---- Ϊ������ ---**/
		g_pub_tx.svc_ind=9002;
	}

			/*�޸Ľ����ʺű�Ϊ����*/
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
									g_mdm_ac_rel.ac_no);
    if(g_reply_int)
   	{
   		sprintf(acErrMsg,"�����α����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"ȡ��¼����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	strcpy(f_mdm_ac_rel.note_sts,"*");

	g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
	 	sprintf(acErrMsg,"�޸ļ�¼����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Mdm_ac_rel_Clo_Upd( );

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"0����ʻ��������׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"0����ʻ���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
