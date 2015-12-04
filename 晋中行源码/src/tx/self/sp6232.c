/*************************************************
* �� �� ��: V1.0
* �� �� ��: sp6232.c
* ��������: �����˻���Ϣ��ѯ(�����Կ����������������ͻ��ţ��ͻ����ͣ��˻�/����״̬������ϸ�ȵĲ�ѯ)
* ����ģ��: ��ƽ̨������ϵ����
* ��������: �����˻���Ϣ��ѯ
* ��    ��: gong
* �������: 2011��7��15��
* �޸ļ�¼:
* ��    ��: 20110816
* �� �� ��: xueke
* �޸�����: pub_com_test.c()���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
insert into tx_def  values ('6232', '�����ʻ���Ϣ��ѯ', '11000000000010010001000011100110000110111100000000000010000011000111111111011000111000001110101001000000000000000000000000000100', '1', '4', '0', '3', null);
insert into tx_flow_def  values ('6232', 0, '6232', '#=0000@1@#$');
insert into tx_flow_def  values ('6232', 1, 'F000', '#$');
insert into tx_sub_def  values ('6232', '�����˻���Ϣ��ѯ', 'sp6232', '0', '0');
*************************************************/
#include <stdio.h>
#include "public.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "card_reg_c.h"
#include "dd_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "card.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "in_parm_c.h"
#include "cif_addr_inf_c.h"
#include "td_mst_c.h"
#include "td_parm_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#define EXTERN
int sp6232()
{
	int iFhcnt = 0;/*�ֻ�����*/
	int iRet = 0,i=0;
	long End_line=100;
	int totnum=0;
	char cHx_tx_code[5]; /*���״���*/
	char cAc_name[61];  /** ���� **/
	char cAc_no[25];  /** �˺� **/
	char cMsr2[38];  /** ���ŵ���Ϣ **/
	char cMsr3[105];  /** ���ŵ���Ϣ **/
	char cPwd[17];  /** �ͻ����� **/
	char cBrf[51];  /** ����ժҪ **/
	char fd95[483],brf6[13],to_str[13];
	char mx_tmp[45];
	char tmpstr[41];
	char cCert_no[21];  /**֤������/����**/
	char cCtrlcmd[11];  /**���Ʊ�־ **/
	char cGz_flag[2];
	char sts[51];
	char sFlag[2];
	char cFee_flag[2];/*�Ƿ��շ�*/
	char cMx_Fh_flag[2];/*������ϸ���߷ֻ�����Ϣ��־ 0 ��ϸ 1 �ֻ���*/
	double dFee=0.00;/**����������**/
	double account_amt=0.0,limit_amt=0.0;/**��������ƽ��**/
	double ky_bal=0.0;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct dd_mst_hst_c sDd_mst_hst;
	struct dd_parm_c sDd_parm;
	struct card_reg_c sCard_reg;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct prdt_ac_id_c sPrdt_ac_id_fh;
	struct in_mst_c sIn_mst;
	struct in_parm_c sIn_parm;
	struct cif_addr_inf_c sCif_addr_inf;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	struct mo_prdt_prot_c sMo_prdt_prot;
	struct mo_prot_ac_rel_c sMo_prot_ac_rel;
	struct dd_mst_c sDd_mst_virtual;
	/** ��ʼ�������� **/
	memset(sFlag, 0x00, sizeof(sFlag));
	memset(cHx_tx_code,0,sizeof(cHx_tx_code));
	memset(cPwd, 0x00, sizeof(cPwd));
	memset(cBrf, 0x00, sizeof(cBrf));
	memset(cMsr2, 0x00, sizeof(cMsr2));
	memset(cMsr3, 0x00, sizeof(cMsr3));
	memset(cAc_no, 0x00, sizeof(cAc_no));
	memset(cGz_flag,0x00,sizeof(cGz_flag));
	memset(sts,0x00,sizeof(sts));
	memset(cAc_name, 0x00, sizeof(cAc_name));
	memset(cCert_no, 0x00, sizeof(cCert_no));
	memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(fd95,'\0',sizeof(fd95));
	memset(mx_tmp,0,sizeof(mx_tmp));
	memset(brf6,0,sizeof(brf6));
	memset(to_str,0,sizeof(to_str));
	memset(cFee_flag,0x00,sizeof(cFee_flag));
	strcpy(g_pub_tx.cur_no,"01");
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sDd_mst_hst,0,sizeof(sDd_mst_hst));
	memset(&sCard_reg,0,sizeof(sCard_reg));
	memset(&sDd_parm,0x00,sizeof(sDd_parm));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&sPrdt_ac_id_fh,0x00,sizeof(sPrdt_ac_id_fh));
	memset(&sIn_mst,0x00,sizeof(sIn_mst));
	memset(&sIn_parm,0x00,sizeof(sIn_parm));
	memset(&sCif_addr_inf,0x00,sizeof(sCif_addr_inf));
	memset(cMx_Fh_flag,0x00,sizeof(cMx_Fh_flag));
	memset(&sTd_mst,0x00,sizeof(sTd_mst));
	memset(&sTd_parm,0x00,sizeof(sTd_parm));
	memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
	memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
	memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
	/*** ȡֵ **/
	get_zd_data("0160",cHx_tx_code);
	get_zd_data("0250", cAc_name );
	get_zd_data("0300", cAc_no );
	get_zd_double("0410", &dFee);
	get_zd_data("0620", cCert_no);
	get_zd_data("0730", cCtrlcmd);
	get_zd_data("0750", cMsr2);
	get_zd_data("0760", cMsr3);
	get_zd_data("0710",cFee_flag);
	vtcp_log("[%s][%d],�շѱ�־=[%s]",__FILE__,__LINE__,cFee_flag);
	get_zd_data("0680",cMx_Fh_flag);
	vtcp_log("[%s][%d],��ϸ�ֻ���־=[%s]",__FILE__,__LINE__,cMx_Fh_flag);
	/*�����ø�ֵ
	strcpy(cAc_no,"1000039600118");
	strcpy(cMx_Fh_flag,"1");
	*/
	/*������80�� 20111020*/
	/** ����������80��ȡֵ��
	get_zd_data("0800", cPwd);
	vtcp_log("%s,%d,ǰ̨��������Ϊ[%s]!",__FILE__,__LINE__,cPwd);
	**/
	get_zd_data("0810", cBrf);
	/** �¾��˺�ת�� **/
	pub_base_old_acno(cAc_no);
	/**modify by lw 20110911**
	* memcpy(g_pub_tx.draw_pwd,cPwd,strlen(g_pub_tx.draw_pwd));
	memcpy(g_pub_tx.draw_pwd,cPwd,sizeof(g_pub_tx.draw_pwd)-1);
	vtcp_log("%s,%d,cPwd=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);
	**/
	memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
	zip_space(g_pub_tx.ac_no);
	/**����ʼ����Ҫ�ء�**/
	iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no,"001","","2",cGz_flag);
	vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cGz_flag);
	if (iRet)
	{
		sprintf(acErrMsg,"%s,%d,������������ʧ��",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	if( strlen ( g_pub_tx.ac_no ) >10) /*** ���������,ֻ�����˺� ***/
	{
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if (iRet )
		{
			vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,g_pub_tx.ac_no);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		/*add by yangmeng 20140706 ���֧ȡ��ʽ  begin*/
		vtcp_log("%s,%d,draw_pwd_yn[%s]���Ҵ���!",__FILE__,__LINE__,sMdm_ac_rel.draw_pwd_yn);
		
		if (sMdm_ac_rel.draw_pwd_yn[0] == 'Y')
		{
			vtcp_log("%s,%d,draw_pwd_yn[%s]����֧ȡ��ʶ!",__FILE__,__LINE__,sMdm_ac_rel.draw_pwd_yn);
			set_zd_data("0660", "1");
		}
		if (sMdm_ac_rel.draw_pwd_yn[0] == 'N')
		{
			vtcp_log("%s,%d,draw_pwd_yn[%s]���ܻ���ʶ!",__FILE__,__LINE__,sMdm_ac_rel.draw_pwd_yn);
			set_zd_data("0660", "9");
		}
		if (sMdm_ac_rel.draw_id_yn[0] == 'Y')
		{
			vtcp_log("%s,%d,draw_id_yn[%s]֤��֧ȡ��ʶ!",__FILE__,__LINE__,sMdm_ac_rel.draw_id_yn);
			set_zd_data("0720", "2");
		}
		if (sMdm_ac_rel.draw_seal_yn[0] == 'Y')
		{
			vtcp_log("%s,%d,draw_id_yn[%s]ƾӡ��֧ȡ��ʶ!",__FILE__,__LINE__,sMdm_ac_rel.draw_seal_yn);
			set_zd_data("0660", "3");
		}
		if (sMdm_ac_rel.draw_uncon_yn[0] == 'Y')
		{
			vtcp_log("%s,%d,draw_id_yn[%s]����֧ȡ��ʶ!",__FILE__,__LINE__,sMdm_ac_rel.draw_uncon_yn);
			set_zd_data("0660", "4");
		}
		/*add by yangmeng 20140706 ���֧ȡ��ʽ  end*/
		/*�����ڲ��˻��Ĳ�ѯ*/
		iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id = %ld",sMdm_ac_rel.ac_id);
		if(iRet)
		{
			vtcp_log("%s,%d,��ѯprdt_ac_id���󣡣�",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		if(sPrdt_ac_id.ac_id_type[0] == '1') /*����*/
		{
			iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
			if (iRet)
			{
				vtcp_log("%s,%d,��ѯ�������ļ�����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm," prdt_no='%s' and 1=1",sDd_mst.prdt_no);
			if(iRet)
			{
				sprintf(acErrMsg,"%s,%d,Dd_mst_Sel���ݿ����ʧ��[%d]",__FILE__,__LINE__,iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P102");
				goto ErrExit;
			}
			/***
			iRet = Cif_addr_inf_Sel(g_pub_tx.reply,&sCif_addr_inf,"cif_no = %ld",sDd_mst.cif_no);
			if(iRet)
			{
				sprintf(acErrMsg,"%s,%d,cif_addr_inf���ݿ����ʧ��[%d]",__FILE__,__LINE__,iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "C067");
				goto ErrExit;
			}***/
			
			/*add fupengfei 2014/7/5 14:47:57 beg*/
			/**1.���.�Ƿ����������˻�.**/
			vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,sDd_mst.ac_id);
			iRet = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",sDd_mst.ac_id);
			if(iRet == 100)
			{
				vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
				strcpy( g_pub_tx.reply,"0000");
				/**��������ǩԼ.����ԭ����.**/
			}
			else if(iRet)
			{
				sprintf(acErrMsg,"Mo_prdt_prot_Sel error iRet=[%d]",iRet);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			else
			{
				vtcp_log("[%s][%d]���˻� zyl 20110415 [%17.2f]",__FILE__,__LINE__,sDd_mst.bal);
				vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,sDd_mst.ac_id);
				sFlag[0] = '1';		/*7�����˺�*/
				vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
				iRet = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",sDd_mst.ac_id);
				if(iRet == 100)
				{
					/**������ǩԼ��Ϣ����.�������˻����չ�ϵ������.**/
					sprintf(acErrMsg,"������ǩԼ��Ϣ����.�������˻����չ�ϵ������.");
					WRITEMSG
					strcpy( g_pub_tx.reply,"D104");
					goto ErrExit;
				}
				else if(iRet)
				{
					sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error iRet=[%d]",iRet);
					WRITEMSG
					strcpy( g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				
				/**2.�������˻���Ϣ..ǩԼ�˻�,�����˻���Ϣ��ȡ��.**/
				iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
				if(iRet == 100)
				{
					sprintf(acErrMsg,"dd_mst���������˻���Ϣ.");
					WRITEMSG
					strcpy( g_pub_tx.reply,"D104");
					goto ErrExit;
				}
				else if(iRet)
				{
					sprintf(acErrMsg,"Dd_mst_Sel error iRet=[%d]",iRet);
					WRITEMSG
					strcpy( g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				sDd_mst.bal=sDd_mst.bal+sDd_mst_virtual.bal;
				vtcp_log("[%s][%d]�����˻����bal=[%17.2f]",__LINE__,__FILE__,sDd_mst.bal);
				vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
			}
			/*add fupengfei 2014/7/5 14:47:57 end*/
			/**����ǰ̨��ֵ**/
			set_zd_data("0250",sDd_mst.name);
			set_zd_data("0020",sDd_mst.opn_br_no ); /**/
			vtcp_log("%s,%d,opn_br_no[%s]",__FILE__,__LINE__,sDd_mst.opn_br_no);
			set_zd_long("0260",sDd_mst.cif_no );    /*�ͻ���*/
			set_zd_data("0830",sDd_parm.title);
			/***
			set_zd_data("0270",sCif_addr_inf.addr);
			***/
			/*�����˻�����*/
			set_zd_data("0700",sDd_mst.ac_type);
			/*ƽ̨Ҫ�󷵻ؿͻ�����1���� 2 �Թ� 3�ڲ� */
			if(sDd_parm.cif_type[0] == '1' || sDd_parm.cif_type[0] == '5') /*���˺��ڲ�Ա������1*/
			{
				set_zd_data("0670","1");
			}
			else
			{
				set_zd_data("0670","2");
			}
			/*�����˻�����״̬���ز�ֵͬ*/
			vtcp_log("%s,%d,note_sts=[%c]",__FILE__,__LINE__,sMdm_ac_rel.note_sts[0]);
			if(sMdm_ac_rel.note_sts[0]=='1')/*�ѹ�ʧ*/
			set_zd_data("0690","5");
			if(sDd_mst.ac_sts[0] == '*')/*����*/
			set_zd_data("0690","*");
			vtcp_log("%s,%d,ac_sts=[%c]",__FILE__,__LINE__,sDd_mst.ac_sts[0]);
			vtcp_log("%s,%d,hold_sts=[%c]",__FILE__,__LINE__,sDd_mst.hold_sts[0]);
			if(sDd_mst.hold_sts[0] != '0')
			{
				if(sDd_mst.hold_sts[0] == '1')/*����ȫ����*/
				{
					set_zd_data("0690","2");
				}
				else if(sDd_mst.hold_sts[0] == '2')/*ֻ������*/
				{
					set_zd_data("0690","4");
				}
				else if(sDd_mst.hold_sts[0] == '3')/*���ֶ���*/
				{
					set_zd_data("0690","3");
				}
			}
			/***ȡ���***/
			account_amt = sDd_mst.bal; /* �������*/
			vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
			if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0) /*�Ǳ��еĿ� �鱾��ȡ�ֽ��*/
			{
				/******���Ǽǲ����ֵ��׼�ˣ��ŵ�ƽ̨����
				iRet = Card_reg_Sel(g_pub_tx.reply,&sCard_reg,"ac_no='%s' ",g_pub_tx.ac_no);
				if (iRet)
				{
					vtcp_log("%s,%d,���Ų�����!",__FILE__,__LINE__);
					sprintf(acErrMsg,"���Ų�����") ;
					WRITEMSG
					strcpy(g_pub_tx.reply,"CU14");
					goto ErrExit;
				}
				ky_bal = sCard_reg.atm_max_dr_cash_tx_amt - sCard_reg.atm_td_dr_cash_tx_amt;**���տ������= atm���ֽ�ȡ��������- atm�����ֽ�ȡ��׽��**
				vtcp_log("%s,%d,ky_bal:[%lf]",__FILE__,__LINE__,ky_bal);
				vtcp_log("%s,%d,sCard_reg.atm_max_dr_cash_tx_amt:[%lf]",__FILE__,__LINE__,sCard_reg.atm_max_dr_cash_tx_amt);
				vtcp_log("%s,%d,sCard_reg.atm_td_dr_cash_tx_amt:[%lf]",__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_amt);
				***/
				/*����ֱ�ӷ��ؿ��ý��*/
				ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
				vtcp_log("%s,%d,sDd_mst.bal:[%lf]",__FILE__,__LINE__,sDd_mst.bal);
				vtcp_log("%s,%d,ky_bal:[%lf]",__FILE__,__LINE__,ky_bal);
				/****
				if (pub_base_CompDblVal(ky_bal,limit_amt)>0)
				{
					ky_bal = limit_amt;
					vtcp_log("%s,%d,���ý��[%lf]",__FILE__,__LINE__,ky_bal);
				}
				****/
			}
			else /*���ǿ��û�*/
			{
				ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
			}
			vtcp_log("%s,%d,���ý��[%lf]",__FILE__,__LINE__,ky_bal);
			if ((pub_base_CompDblVal(sDd_mst.bal,ky_bal) < 0) && sFlag[0] != '1')
			{
				ky_bal = sDd_mst.bal;
			}
			/*������˺ű�ȫ�������ֻ�����������ý��Ϊ0*/
			if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2')
			{
				ky_bal = 0.00;
			}
			set_zd_double("0420",ky_bal);/*�������*/
			set_zd_double("0400",sDd_mst.bal);/*�������*/
		}
		else if (sPrdt_ac_id.ac_id_type[0] == '9') /*�ڲ��˺�*/
		{
			iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
			if(iRet)
			{
				vtcp_log("%s,%d,��ѯ�ڲ����ļ����󣡣�",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"A029");
				goto ErrExit;
			}
			iRet = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no ='%s'",sIn_mst.prdt_no);
			if(iRet)
			{
				vtcp_log("%s,%d,��ѯ�ڲ���Ʒ�������󣡣�",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"W023");
				goto ErrExit;
			}
			set_zd_data("0250",sIn_mst.name); /*����*/
			set_zd_data("0020",sIn_mst.opn_br_no );/*��������*/
			/*
			set_zd_long("0260","");�ڲ��˺ŷ��ؿ� �ͻ���*/
			set_zd_data("0270","");/*�ڲ��˺�û�е�ַ*/
			set_zd_data("0670","3");/*�ͻ�����*/
			set_zd_data("0700",sPrdt_ac_id.ac_id_type);/*�˺�����*/
			/*����������*/
			account_amt = sIn_mst.bal;/*������*/
			vtcp_log("[%s][%d],�ڲ��˺��������=��%lf��",__FILE__,__LINE__,account_amt);
			ky_bal = sIn_mst.bal - sIn_mst.hold_amt;
			set_zd_double("0420",ky_bal);/*�������*/
		}
		else
		{
			vtcp_log("%s,%d,�˺����ʹ���",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W003");
			goto ErrExit;
		}
		/***
		cmonid��֪�����ĸ������ģ���֪����η���
		set_zd_data("0370",sMdm_ac_rel.cmonid );
		****/
	}
	else
	{
		vtcp_log("%s,%d,ac_no[%s]�����Ϊ��Ŀ�ţ��������!",__FILE__,__LINE__,g_pub_tx.ac_no);
		strcpy(g_pub_tx.reply,"L017");
		goto ErrExit;
	}
	/* add by lw 2012-05-30 �۳������� */
	if(cFee_flag[0] =='1')	/*��������*/
	{
		char cTmp[128]    = {0};
		char cFd123[1024] = {0};
		char cFee_acno[32]= {0};
		char cDc_ind[2]   = {0};
		double dFee = 0.00;
		double dSumFee = 0.00;
		get_fd_data("1230",cFd123);
		memcpy(cTmp, cFd123+1, 2);	
		int iCnt = atoi(cTmp);
		for(int i=0; i<iCnt; i++)
		{
			memset(cFee_acno, 0, sizeof(cFee_acno));
			memcpy(cFee_acno, cFd123+3+48*i, 24);
			zip_space(cFee_acno);
			if(strcmp(cAc_no, cFee_acno) != 0) {
				continue;
			}
			memcpy(cDc_ind, cFd123+27+48*i, 1);
			memset(cTmp, 0, sizeof(cTmp));
			memcpy(cTmp, cFd123+29+48*i, 12);
			dFee = atof(cTmp) / 100.00;
			if(cDc_ind[0] == '1') {
				dFee=-dFee;
			}
			dSumFee += dFee;
		}
		vtcp_log("%s,%d,LW �������[%lf] ���[%lf] ������[%lf]",__FILE__,__LINE__,ky_bal,account_amt,dSumFee);
		account_amt += dSumFee;
		ky_bal += dSumFee;
		set_zd_double("0420",ky_bal);	/*�������*/
	}
	/* end by lw 2012-05-30 */
	vtcp_log("%s,%d,���ý��[%lf],���[%lf] ",__FILE__,__LINE__,ky_bal,account_amt);
	memcpy(tmpstr,"1001156C",8);
	sprintf(tmpstr+8,"%012.0f",account_amt*100);
	memcpy(tmpstr+20,"1002156C",8);
	sprintf(tmpstr+28,"%012.0f",ky_bal*100);
	vtcp_log("%s,%d,��ѯ���������:tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
	set_zd_data("0820",tmpstr);
	/*��ѯ��ϸ���߷ֻ�����Ϣ*/
	if(cMx_Fh_flag[0] == '0')
	{
		/*��ѯ������ϸ*/
		if(sPrdt_ac_id.ac_id_type[0] == '1')
		{
			iRet = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%d order by hst_cnt desc",sDd_mst.ac_id,sDd_mst.ac_seqn);
			if(iRet)
			{
				vtcp_log("%s,%d,������ϸ����",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}
			i=0;
			while(1)
			{
				vtcp_log("%s,%d,��ʼ��ѯ�ʻ���ϸ��¼!",__FILE__,__LINE__);
				if( i > 9)
				{
					vtcp_log("%s,%d,�Ѿ�ȡ��10����ϸ",__FILE__,__LINE__);
					break;
				}
				iRet = Dd_mst_hst_Fet_Sel(&sDd_mst_hst,g_pub_tx.reply);
				if(iRet && iRet !=100)
				{
					vtcp_log("%s,%d,������ϸ����",__FILE__,__LINE__);
					Dd_mst_hst_Clo_Sel();
					strcpy(g_pub_tx.reply,"CU14");
					goto ErrExit;
				}
				else if(iRet == 100)
				{
					vtcp_log("%s,%d,�ҵ��ļ�¼��Ϊ[%d]",__FILE__,__LINE__,i);
					break;
				}
				/**��ֹ��95���ֵ̫�࣬����ȡ���ļ�¼��Ϊ100**/
				totnum++;
				if(totnum>End_line)
				{
					/*��100����д������*/
					vtcp_log("%s,%d,������ϸ���࣡",__FILE__,__LINE__);
					Dd_mst_hst_Clo_Sel();
					strcpy(g_pub_tx.reply,"W050");
					goto ErrExit;
				}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				memset(to_str,0,sizeof(to_str));
				strncpy(brf6,sDd_mst_hst.brf,sizeof(brf6)-1);
				vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,brf6);
				Zip_halfChinese(brf6,to_str);
				vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,to_str);
				sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100);
				memcpy(fd95+2+i * 48,mx_tmp,48);
				i ++;
			}
			Dd_mst_hst_Clo_Sel();
			if(i==0)
			{
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				memcpy(to_str,"��",2);
				sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100);
				memcpy(fd95+2+i * 48,mx_tmp,48);
				sprintf(mx_tmp,"%02d",1);
			}
			else
			{
				sprintf(mx_tmp,"%02d",i);
			}
			memcpy(fd95,mx_tmp,2);
			set_zd_data("0950",fd95);
			vtcp_log("%s,%d,��ѯ�˻���ϸ����!fd95[%s]",__FILE__,__LINE__,fd95);
		}
	}
	else if(cMx_Fh_flag[0] == '1')
	{
		/*��ѯ�ֻ�����Ϣ*/
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if (iRet )
		{
			vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,g_pub_tx.ac_no);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		iRet = Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id =%ld order by ac_seqn",sMdm_ac_rel.ac_id);
		if (iRet )
		{
			vtcp_log("%s,%d,Prdt_ac_id_Dec_Sel����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"K106");
			goto ErrExit;
		}
		while(1)
		{
			memset(&sPrdt_ac_id_fh,0x00,sizeof(sPrdt_ac_id_fh));
			memset(&sDd_mst,0x00,sizeof(sDd_mst));
			memset(&sTd_mst,0x00,sizeof(sTd_mst));
			iRet=Prdt_ac_id_Fet_Sel(&sPrdt_ac_id_fh,g_pub_tx.reply);
			if(iRet !=0 && iRet != 100)
			{
				vtcp_log("%s,%d,Prdt_ac_id_Fet_Sel����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"K106");
				goto ErrExit;
			}
			else if(iRet == 100 && iFhcnt == 0)
			{
				vtcp_log("%s,%d,Prdt_ac_id_Fet_Sel����!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"K106");
				goto ErrExit;
			}
			else if(iRet == 100 && iFhcnt > 0 || iFhcnt == 29)
			{
				vtcp_log("[%s][%d],��ѯ�� ���߱���Ϊ29����95������29����¼�����˳���������===[%d]",__FILE__,__LINE__,iFhcnt);
				break;
			}
			if(sPrdt_ac_id_fh.ac_id_type[0] == '1') /*���ڷֻ�*/
			{
				iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn = %d ",sPrdt_ac_id_fh.ac_id,sPrdt_ac_id_fh.ac_seqn);
				if(iRet)
				{
					vtcp_log("%s,%d,Dd_mst_Sel����!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"K107");
					goto ErrExit;
				}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				strcpy(brf6,"����");
				Zip_halfChinese(brf6,to_str);
				sprintf(mx_tmp,"%03d %-5.5s%012.0lf",sDd_mst.ac_seqn,to_str,sDd_mst.bal*100);
				memcpy(fd95+2+iFhcnt * 21,mx_tmp,21);
			}
			else if(sPrdt_ac_id_fh.ac_id_type[0] == '2')/*���ڷֻ�*/
			{
				iRet = Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id=%ld and ac_seqn = %d ",sPrdt_ac_id_fh.ac_id,sPrdt_ac_id_fh.ac_seqn);
				if(iRet)
				{
					vtcp_log("%s,%d,Dd_mst_Sel����!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"K107");
					goto ErrExit;
				}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				strcpy(brf6,"����");
				Zip_halfChinese(brf6,to_str);
				sprintf(mx_tmp,"%03d %-5.5s%012.0lf",sTd_mst.ac_seqn,to_str,sTd_mst.bal*100);
				memcpy(fd95+2+iFhcnt * 21,mx_tmp,21);
			}
			else
			{
				vtcp_log("%s,%d,�����˻���ͳ��!  [%s]",__FILE__,__LINE__,sPrdt_ac_id_fh.ac_id_type);
				continue;
			}
			iFhcnt ++;
		}
		Prdt_ac_id_Clo_Sel();
		memset(mx_tmp,0x00,sizeof(mx_tmp));
		sprintf(mx_tmp,"%02d",iFhcnt);
		memcpy(fd95,mx_tmp,2);
		set_zd_data("0950",fd95);
		vtcp_log("%s,%d,��ѯ�ֻ����˻�����!fd95[%s]",__FILE__,__LINE__,fd95);
	}
	else if(cMx_Fh_flag[0] == 'N')
	{
		vtcp_log("%s,%d,��������ϸ���߷ֻ��˲�ѯ[%s]",__FILE__,__LINE__,cMx_Fh_flag);
	}
	else if(cMx_Fh_flag[0] == '3')
	{/**���������˻���ϸ��ѯ**/
		vtcp_log("%s,%d,��������312�����˻���ϸ��ѯ[%s]",__FILE__,__LINE__,cMx_Fh_flag);
		
		long begindate=0;
		long enddate=0;
    char cFilename[256]={0};
    char sw_date[12]={0};
    char pcfilename[100]={0};
		char sw_traceno[12]={0};
    char cFtpfile[256]={0};
    char numtemp[10]={0};
    FILE *fp = NULL;
    
		get_zd_long("0440", &begindate);
		get_zd_long("0450", &enddate);
		get_zd_data("0050", sw_date);
		get_zd_data("0520", sw_traceno);

		iRet = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id =%ld and tx_date>=%ld and tx_date<=%ld order by hst_cnt desc",sDd_mst.ac_id,begindate,enddate);
		if(iRet)
		{
			vtcp_log("%s,%d,������ϸ����",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
		
    sprintf( cFilename,"7312IBPS%s%s", sw_date,sw_traceno);
    sprintf( pcfilename,"%s/%s",getenv("FILDIR"),cFilename );
    fp=fopen(pcfilename,"w");
    if(fp == NULL)
    {
        vtcp_log("���ļ�ʧ��[%s]",pcfilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    
			vtcp_log("%s,%d,��ʼ��ѯ�ʻ���ϸ��¼!",__FILE__,__LINE__);		
			while(1)
			{
				iRet = Dd_mst_hst_Fet_Sel(&sDd_mst_hst,g_pub_tx.reply);
				if(iRet && iRet !=100)
				{
					vtcp_log("%s,%d,������ϸ����",__FILE__,__LINE__);
					Dd_mst_hst_Clo_Sel();
					strcpy(g_pub_tx.reply,"CU14");
					goto ErrExit;
				}
				else if(iRet == 100)
				{
					break;
				}
				/**312���������д200����ϸ������ȡ���ļ�¼��Ϊ200**/
				totnum++;
				if(totnum>200)
				{/*��200����д������*/
					vtcp_log("%s,%d,������ϸ����200����������",__FILE__,__LINE__);
					break;
				}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				memset(to_str,0,sizeof(to_str));
				strncpy(brf6,sDd_mst_hst.brf,sizeof(brf6)-1);
				/**vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,brf6);**/
				Zip_halfChinese(brf6,to_str);
				vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,to_str);
				
				fprintf(fp,"%08d%-12.12s%014.0lf%014.0lf%s\n",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100,sDd_mst_hst.add_ind);
				vtcp_log("%s %d��ʼ��ӡ��%08d%-12.12s%014.0lf%014.0lf%s\n",\
	 					__FILE__,__LINE__,sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100,sDd_mst_hst.add_ind);
			}
			
			Dd_mst_hst_Clo_Sel();
			fclose(fp);
			fp = NULL;  
			vtcp_log("%s,%d,�ҵ��ļ�¼��Ϊ[%d]",__FILE__,__LINE__,totnum);

			set_zd_data("0740",cFilename); /*�ļ���*/
			sprintf(numtemp,"%d",totnum);
			set_zd_data("0770",numtemp);  /*��¼��*/
			/*set_zd_data("0770","0");  *//* ������*/
			sprintf(cFtpfile,"%s/bin/ftp_to_ibpssw.sh %s",getenv("WORKDIR"),cFilename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			iRet=system(cFtpfile);
			if (iRet)
			{
				vtcp_log("[%s][%d] ftp�ļ�[%s]����������ƽ̨����[%d]", __FILE__, __LINE__,cFilename,iRet);
				strcpy(g_pub_tx.reply,"9999");
				goto ErrExit;
			}	
			vtcp_log("[%s][%d] ftp�ļ�[%s]����������ƽ̨�ɹ�", __FILE__, __LINE__,cFilename); 
	}
	else
	{
		vtcp_log("%s,%d,��ѯ��־����[%s]",__FILE__,__LINE__,cMx_Fh_flag);
		goto ErrExit;
	}
	if(cFee_flag[0] =='1')
	{
		strcpy(g_pub_tx.brf,"��ѯ������");
		if (pub_ins_trace_log())
		{
			sprintf(acErrMsg, "�Ǽǽ�����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}
GoodExit:
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	strcpy(g_pub_tx.reply,"0000");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4) == 0)
	{
		strcpy(g_pub_tx.reply, "H034");
	}
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int Zip_halfChinese(char *fstr,char *tstr)
{
	printf("%s,%d,��������[%s]�������[%s]\n",__FILE__,__LINE__,fstr,tstr);
	int len=0;
	char *strf;
	char *strt;
	len=strlen(fstr);
	if(len == 0)
	{
		return(0);
	}
	strt = tstr;
	strf = fstr;
	printf("strf[%s],fstr+len[%d]\n",strf,fstr+len);
	for(strf=fstr; strf <= fstr+len;)
	{
		if(*strf > 128)
		{
			if(*(strf+1) > 128)
			{
				*strt = *strf;
				*(strt+1) = *(strf+1);
			}
			else
			{
				*strt = 32;
				*(strt+1) = *(strf+1);
			}
			strf=strf+2;
			strt=strt+2;
			printf("tstr[%s]\n",tstr);
		}
		else
		{
			*strt = *strf;
			strf++;
			strt++;
			printf("tstr[%s]\n",tstr);
		}
	}
	printf("%s,%d,�������[%s]\n",__FILE__,__LINE__,tstr);
	return(0);
}
