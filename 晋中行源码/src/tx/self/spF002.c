/*************************************************
* �� �� ��:  spF002.c
* ���������� ATM ��ѯ���
*
* ��	��:  rob
* ������ڣ� 2003��2��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "dd_mst_hst_c.h"
#include "card_reg_c.h"
#include "com_branch_c.h"
#include "td_parm_c.h"
#include "hv_brno_orno_c.h"
#include "card.h"
#include "lv_define.h"
#define  JWQURY_FEE   2.00 /*�����ѯ�����շѽ��*/
#define  KHJW_FEE    4.00 /*�����ѯ�����շѽ��*/
#define BH_SQATMFEE_KM   "5111601"   /*������ȡATM�����ѿ�Ŀ Mod by hxc 101218 �޸���������ѯ�����������Ŀ 5111203->5111601*/
#define BH_DFFEE_KM   "52704"     /*���е渶�����ѿ�Ŀ add by lzy 150109 Ϊ����ATM��ѯ�渶����*/

int spF002()
{

	int ret=0,i=0,j=0;
	long pass_cnt=0;
	int totnum=0;
	long Beg_line=0;
	double limit_amt =0.0, today_amt = 0.0, account_amt = 0.0, available_amt=0.0;
	double ky_amt=0.0;
	double dJw_fee=0.0;
	char s_txamt[17],prdt_name[6],flag[2],mx_tmp[45];
	char fd95[483],brf6[13],to_str[13];
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct dd_mst_hst_c sDd_mst_hst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;	
	struct com_branch_c zcom_branch;
	char cAcno_qs[25];
	char zAc_no[25];
	char fee_mode[2];
	char tmpstr[41];
	char str_brf[51];
	char sPrdt_no_dd[4];/****���ڲ�Ʒ��****/

	memset(prdt_name,0,sizeof(prdt_name));
	memset(sPrdt_no_dd,0,sizeof(sPrdt_no_dd));
	memset(s_txamt,0,sizeof(s_txamt));
	memset(&sDd_mst_hst,0,sizeof(struct dd_mst_hst_c));
	memset(&sTd_mst,0,sizeof(struct td_mst_c));
	memset(&sTd_parm,0,sizeof(struct td_parm_c));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&zcom_branch,0,sizeof(struct com_branch_c));
	memset(flag,0,sizeof(flag));
	memset(fd95,'\0',sizeof(fd95));
	memset(mx_tmp,0,sizeof(mx_tmp));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(str_brf,0,sizeof(str_brf));
	memset(brf6,0,sizeof(brf6));
	memset(to_str,0,sizeof(to_str));
	/*	
	account_amt   ==  �������
	available_amt ==  ���տ������
	*/
	struct card_reg_c sCard_reg;
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(zAc_no, 0x00, sizeof(zAc_no));
	memset(fee_mode, 0 , sizeof(fee_mode));
	memset(&sCard_reg,0,sizeof(sCard_reg));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	vtcp_log("%s,%d,Gethere1\n",__FILE__,__LINE__);
	get_zd_data("0300",g_pub_tx.crd_no);   /* ���� */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ����  */
	get_zd_data("0690",fee_mode);	       /*����ģʽ*/
	get_zd_data("0810",str_brf);           /*ժҪ*/
	get_zd_data("0710",flag);              /*ͨ��71�������ֲ�ѯ����:0,��������ѯ;1.������ϸ��ѯ;2.���˻���ѯ*/
	sprintf(acErrMsg,"tx_date=[%ld],ac_no=[%s]", g_pub_tx.tx_date, g_pub_tx.crd_no);
	WRITEMSG
	vtcp_log("%s,%d,CEHSI,fee_mode[%s]",__FILE__,__LINE__,fee_mode);
	vtcp_log("%s,%d,g_pub_tx.crd_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.crd_no);
	/**Add by SSH,20100427,�򻯶Թ���ѯ����**/
	if(g_pub_tx.crd_no[0]=='5'){
		struct hv_brno_orno_c hv_brno_orno;
		memset(&hv_brno_orno,'\0',sizeof(hv_brno_orno));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s' and ac_seqn=1",	g_pub_tx.crd_no);
		if(ret){
			sprintf(acErrMsg,"δ�ҵ��ʻ�[%s]",g_pub_tx.crd_no);
			strncpy(g_pub_tx.reply,"CU14",4);
			WRITEMSG
			vtcp_log("%s,%d δ�ҵ��ʻ�[%s][%d]",__FILE__,__LINE__,g_pub_tx.crd_no,g_reply_int);
			goto ErrExit;
			
		}
		/***ADD BY ZHGF 20100610 ���ӷ��ػ�������**/
		/****Ϊ����������˴�������Com_branch_Sel��
		ret=pub_base_get_br_parm(sMdm_ac_rel.opn_br_no,&zcom_branch);
		if(ret){
			sprintf(acErrMsg,"ȡ����������Ϣ����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		****/
		ret=Com_branch_Sel(g_pub_tx.reply,&zcom_branch,"br_no = '%s' ",sMdm_ac_rel.opn_br_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"�������������޴˻���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O127");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"ȡ����������Ϣ����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		set_zd_data("0760",zcom_branch.br_name);
		/***ADD BY ZHGF 20100610 ���ӷ��ػ�������**/
		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=1",	sMdm_ac_rel.ac_id);
		if(ret){
			sprintf(acErrMsg,"δ�ҵ��ʻ�[%s]",g_pub_tx.crd_no);
			strncpy(g_pub_tx.reply,"CU14",4);
			WRITEMSG
			vtcp_log("%s,%d δ�ҵ��ʻ�[%s][%d]",__FILE__,__LINE__,g_pub_tx.crd_no,g_reply_int);
			goto ErrExit;
			
		}
		/****�������Ӳ�Ʒ�ŵĿ���,��Ϊ����ͨȡ��,������ӱ�֤��ȡ�� 20100703 begin****/
		memcpy(sPrdt_no_dd,sDd_mst.prdt_no,sizeof(sPrdt_no_dd));
		if(memcmp(sPrdt_no_dd,"131",3)==0||memcmp(sPrdt_no_dd,"132",3)==0||memcmp(sPrdt_no_dd,"134",3)==0||memcmp(sPrdt_no_dd,"142",3)==0||memcmp(sPrdt_no_dd,"144",3)==0){
			sprintf(acErrMsg,"��Ϊ���б�֤��,�������ù���ͨȡ��!��Ʒ��==[%s]",sPrdt_no_dd);
			strncpy(g_pub_tx.reply,"P471",4);
			WRITEMSG
			goto ErrExit;
		}
		/****20100703 end****/
		set_zd_data("0250",sDd_mst.name);	
		set_zd_long("0260",sDd_mst.cif_no);	
		set_zd_data("0270",sMdm_ac_rel.id_no);	
		ret=Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",sDd_mst.opn_br_no);
		if(ret){
			set_zd_data("0320",LV_QSBRNO);
		}else{
			set_zd_data("0320",hv_brno_orno.or_br_no);
		}
		set_zd_data("0330",sDd_mst.opn_br_no);
		set_zd_data("0910",sDd_mst.opn_br_no);/**20100610**/
		/****��Ϊ���Ŵ�������Ҫ�����ȡ��������������С�����㻧����� ���Է���֮ begin 20100713****/
		available_amt=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt;
		account_amt=sDd_mst.bal;
		
		/*��82��*/
		memcpy(tmpstr, "1001156C", 8);
		sprintf(tmpstr+8, "%012.0f", account_amt*100);
		memcpy(tmpstr+20, "1002156C", 8);
		sprintf(tmpstr+28, "%012.0f", available_amt*100);
		vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
		set_zd_data("0820", tmpstr);
		/****��Ϊ���Ŵ�������Ҫ�����ȡ��������������С�����㻧����� ���Է���֮ end 20100713****/
		goto OkExit;
	}
	
	/***********add by luowei at 20090815**********/
	/****���Ӷ��۵�֧��****/
	/****/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_no='%s' ", g_pub_tx.crd_no);
		if(ret)
		{
			sprintf(acErrMsg,"%s,%d,Mdm_ac_rel_Sel���ݿ����ʧ��[%d]",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
			goto ErrExit;
		}
		/***ADD BY ZHGF 20100610 ���ӷ��ػ�������**/
		/****
		ret=pub_base_get_br_parm(sMdm_ac_rel.opn_br_no,&zcom_branch);
		if(ret){
			sprintf(acErrMsg,"ȡ����������Ϣ����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		****/
		ret=Com_branch_Sel(g_pub_tx.reply,&zcom_branch,"br_no = '%s' ",sMdm_ac_rel.opn_br_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"�������������޴˻���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O127");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"ȡ����������Ϣ����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		set_zd_data("0760",zcom_branch.br_name);
               /***ADD BY ZHGF 20100610 ���ӷ��ػ�������**/

		vtcp_log("%s,%d br_name=[%s],opn_br_no=[%s]\n",__FILE__,__LINE__,zcom_branch.br_name,sMdm_ac_rel.opn_br_no);
		set_zd_data("0760",zcom_branch.br_name);
		set_zd_data("0910",sMdm_ac_rel.opn_br_no);
		/***ADD BY ZHGF 20100610 ���ӷ��ػ�������**/
		set_zd_data("0920",sMdm_ac_rel.note_sts); /**���ӷ����˻�ƾ֤״̬ add by hxc 100813**/
		vtcp_log("%s,%d,�˻�ƾ֤״̬[%s]!",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
		if((memcmp(g_pub_tx.crd_no, CARDHEADCODE, 6) != 0) && (memcmp(g_pub_tx.crd_no, ICCARDHEADCODE, 6) != 0) )
		{
		ret=Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id='%d'", sMdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"%s,%d,Dd_mst_Sel���ݿ����ʧ��[%d]",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
			goto ErrExit;
		}
		/************������֤***20100527 �ǿ�ʱУ������ MODIFY BY ZHGF***********/
		if((memcmp(g_pub_tx.crd_no, CARDHEADCODE, 6) != 0) && (memcmp(g_pub_tx.crd_no, ICCARDHEADCODE, 6) != 0))
		{
			/***20100618 ��֤����Ҫд�����ļ��� RM BY ZHGF
			ret=pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,g_pub_tx.draw_pwd,sMdm_ac_rel.draw_pwd);
			if(ret)
			{
				sprintf(acErrMsg,"%s,%d,�������",__FILE__,__LINE__);
				WRITEMSG
				strcpy(g_pub_tx.reply,"AT04");
				goto ErrExit;
			}
			***********/
			/**2010/7/7 14:22 MODIFY BY ZHGF**/
			/*vtcp_log("%s,%d g_pub_tx.draw_pwd=[%s]\n",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
			if(strlen(g_pub_tx.draw_pwd)){
				strcpy(g_pub_tx.id_type,sMdm_ac_rel.id_type);
				strcpy(g_pub_tx.id_no,sMdm_ac_rel.id_no);
				strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
				ret = pub_base_check_draw("101");
				if(ret){
					goto ErrExit;
				}
			}
			/**2010/7/7 14:22 MODIFY BY ZHGF**/
		}
		/*********************************/
		available_amt=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt;
		account_amt=sDd_mst.bal;
		/*��82��*/
		vtcp_log("%s,%d,����[%s]!",__FILE__,__LINE__,sDd_mst.name);
		set_zd_data("0250",sDd_mst.name);
		memcpy(tmpstr, "1001156C", 8);
		sprintf(tmpstr+8, "%012.0f", account_amt*100);
		memcpy(tmpstr+20, "1002156C", 8);
		sprintf(tmpstr+28, "%012.0f", available_amt*100);
		vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
		set_zd_data("0820", tmpstr);
		strcpy(g_pub_tx.reply, "0000");
		goto OkExit;
	}
	/***********end by luowei at 20090815**********/
	/* ��鿨��״̬�Ϸ���:�Ƿ��ʧ�����ڵ� 
	if ( pub_base_CheckCrdNo() )
	{
		sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
		WRITEMSG
		if(strlen(g_pub_tx.reply) ==0 || memcmp(g_pub_tx.reply,"0000",4)==0){
			strcpy(g_pub_tx.reply,"CU14");
		}
		strcpy(g_pub_tx.reply,"P019");
		goto ErrExit;
	}
*********************/
	/* ���ƾ֤��״̬  
	if (g_mdm_ac_rel.note_sts[0] != '0') 
	{ 
		sprintf(acErrMsg, "�ÿ�Ϊ��Ч��!! "); 
		WRITEMSG 
		if (g_mdm_ac_rel.note_sts[0] == '1' || g_mdm_ac_rel.note_sts[0] == '2') 
		{ 
			strcpy(g_pub_tx.reply, "CU41"); 
		} 
		else 
		{ 
			strcpy(g_pub_tx.reply, "CU41"); 
		} 
		goto ErrExit; 
	}****/
	g_reply_int=pub_base_currentdis_file(g_pub_tx.crd_no,1,&sDd_mst);
	if(g_reply_int)
	{
	       vtcp_log("%s,%d,�˺Ų�����!",__FILE__,__LINE__);
	       goto ErrExit;
	}
	/** ���˻�״̬�������������ѯ **/
	if(sDd_mst.hold_sts[0] != '0' && sDd_mst.hold_sts[0] !='3')
	{
	       vtcp_log("%s,%d,�˻�������״̬!",__FILE__,__LINE__);
	       strcpy(g_pub_tx.reply,"CU62");
	       set_zd_data(DC_GET_MSG,"�˻�����״̬���ܲ�ѯ!");
	       goto ErrExit;
	}
	set_zd_data("0930",sDd_mst.hold_sts); /**���ӷ����˻�����״̬ add by hxc 100813**/
	vtcp_log("%s,%d,����״̬[%s]!",__FILE__,__LINE__,sDd_mst.hold_sts);
	char cTradetype[4];/* ������� */
	int iSelflag=0;  /*������֯��־ */
	int atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */
	int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
	int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
	int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
	int iDscpt=0;/* ��ת��ѯ��־ */
	iSelflag=0;  /*������֯��־ */						
	atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */  
	pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */ 
	pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */ 
	atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */ 
	iDscpt=0;/* ��ת��ѯ��־ */						   
	memset(cTradetype, 0 , sizeof(cTradetype));
	/*modified by wyw 20080312 
		�����շѱ�׼���жϽ�����Դ,��������, �����б�־*/
	iDscpt = CARD_TRADE_QURY;
	if(fee_mode[0] == '0')/*����ģʽ*/
	{
		memcpy(cTradetype ,"POS",3);
	}else if(fee_mode[0] == '6')
	{
		memcpy(cTradetype ,"GMM",3);
	}else{
		memcpy(cTradetype,"ATM",3);
	}
	if(fee_mode[0] == '1'||fee_mode[0] =='2')
	{
		iSelflag = 0; /*����ATM,����POS*/
	}else{
		iSelflag = 1; /*����ATM������POS*/
	}	
	g_reply_int=pub_card_reg(g_pub_tx.tx_amt1,cTradetype,'0',&atm_qy_free_flag,
		&pos_qy_free_flag,&pos_tx_free_flag,&atm_tx_free_flag,iSelflag,iDscpt);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"�Ǽǿ��Ǽǲ�����");
		strncpy(g_pub_tx.reply,"AT06",4);
		WRITEMSG
		vtcp_log("%s,%d �������׵Ǽǲ����� ret=[%d]",__FILE__,__LINE__,g_reply_int);
		goto ErrExit;
	}
	/* ��ѯ�������������� */
	/*ret = pub_base_GetParm_long("ATMMM",1,&pass_cnt); */
	/* ���֧ȡ��ʽ�ĺϷ��� 
	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
					  g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
	*/
	strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
     /****/  ret = pub_base_GetParm_long("KFCAR",2,&pass_cnt);
       if(ret)
       {
               pass_cnt = 3;
               vtcp_log("%s,%d,�������������!",__FILE__,__LINE__);
               vtcp_log("%s,%d,�������������,ȡĬ�ϴ���[%d]!",__FILE__,__LINE__,pass_cnt);
       }
/*************20090729*****/
	ret = Card_reg_Sel(g_pub_tx.reply,&sCard_reg,"ac_no='%s' and l_tx_date=%ld ",g_pub_tx.crd_no,g_pub_tx.tx_date);	
	if(ret)
	{
		vtcp_log("%s,%d,���Ų�����",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
	}
	vtcp_log("%s,%d,fee_ind[%s],type[%s]",__FILE__,__LINE__,fee_mode,cTradetype);
	if(memcmp(cTradetype,"ATM",3)==0)
	{
		if( g_pub_tx.tx_date == sCard_reg.l_tx_date && sCard_reg.add_pswd_cnt+1 >pass_cnt)
		{
			vtcp_log("%s,%d,����ATM�ĵ��տ�ȡ�����!,pass_cnt:[%d]",__FILE__,__LINE__,pass_cnt);
			strcpy(g_pub_tx.reply,"CU75");
			/*  ����return (-1)  12����0000.ATM���淵�ص������Ϊ0      090602 LOONG
			return(-1);
			*/
			goto ErrExit;
		}
	}
/*****************20090728
	ret=pub_base_check_draw("101");
	if (ret != 0)
	{
		sprintf(acErrMsg,"����֧ȡ�������!!!");
		WRITEMSG
		ret = Card_reg_Dec_Upd(g_pub_tx.reply,"ac_no='%s' ",g_pub_tx.crd_no); 
		if (ret != 0) 
		{
			sprintf(acErrMsg,"Declare error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"tx_date=[%ld],   ac_no=[%s]", g_pub_tx.tx_date,
				g_pub_tx.crd_no);
		WRITEMSG

		ret = Card_reg_Fet_Upd(&sCard_reg,g_pub_tx.reply);
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"Fetch error !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		* ���½��׼�¼�ۼƿ��Ʊ� *
		if(sCard_reg.l_tx_date==g_pub_tx.tx_date)
		{
			sCard_reg.add_pswd_cnt = sCard_reg.add_pswd_cnt + 1;
		}else{
			sCard_reg.add_pswd_cnt=1;
		}
		sCard_reg.l_tx_date = g_pub_tx.tx_date;
		if (sCard_reg.add_pswd_cnt > pass_cnt)
		{
			* �޸Ľ����˺Ŷ��ձ� 
			* atm�̿���Ӧ�� *
			ret  = sql_execute2("update mdm_ac_rel set coll_sts='2' where \
				ac_no = '%s' ", g_pub_tx.crd_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "����mdm_ac_rel�����!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "AT08");
				goto ErrExit;
			}
			chnl_tradeadd.add_pswd_cnt = 0;*
	
			strcpy(g_pub_tx.reply,"CU75");
		}
		else
		{
			* ������� *
			strcpy(g_pub_tx.reply,"CU55");
		}
		* ���¿��Ʊ� *
		vtcp_log("%s,%d,��ʼ���¿��Ǽǲ�!",__FILE__,__LINE__);
		ret = Card_reg_Upd_Upd(sCard_reg,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���½��׼�¼�ۼƿ��Ʊ����!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		Card_reg_Clo_Upd();

		ret = sql_commit();
		if (ret != 0)
		{
			sprintf(acErrMsg,"�ύ�������!!");
			WRITEMSG
		}
		vtcp_log("%s,%d,�ύ���´����������!pwd_cnt[%d]",__FILE__,__LINE__,sCard_reg.add_pswd_cnt);
		ret = sql_begin();
		if (ret != 0)
		{
			sprintf(acErrMsg,"��ʼ�������!!");
			WRITEMSG
		}

		goto ErrExit;
	}
**************************/
	/* ���½��׼�¼�ۼƿ��Ʊ�����������������Ϊ0 */
	if(sCard_reg.add_pswd_cnt > pass_cnt && memcmp(cTradetype,"ATM",3)==0)
	{
		vtcp_log("%s,%d,���Ų�����",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU38");
		goto ErrExit;
	}
	sCard_reg.add_pswd_cnt = 0;
	ret = sql_execute2("update card_reg set add_pswd_cnt=0 where \
			l_tx_date=%ld and ac_no = '%s'", 
			g_pub_tx.tx_date, g_pub_tx.crd_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"���½��׼�¼�ۼƿ��Ʊ����!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.cur_no,"01");
	/*begin add by LiuHuafeng 20070203 */
	/* ���2�ŵ�cvn��Ϣ20090728 
	ret=pubf_card_chk_msr(3,g_pub_tx.crd_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯ���ʻ����ļ�����!!!");
		WRITEMSG
		goto ErrExit;
	}*******************/
	/* end by LiuHuafeng 20070203 */
	/*ͨ��71���������ǻ��ڣ���ϸ�����˻���ѯ*/
	/* ��ѯ�����ʻ����ļ� */
	ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯ�����ʻ����ļ�����!!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	/** ���˻�״̬�������������ѯ **/
	if(g_dd_mst.hold_sts[0] != '0' && g_dd_mst.hold_sts[0] !='3')
	{
		vtcp_log("%s,%d,�˻�������״̬!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU62");
		set_zd_data(DC_GET_MSG,"�˻�����״̬���ܲ�ѯ!");
		goto ErrExit;
	}
	vtcp_log("%s,%d,����[%s]!",__FILE__,__LINE__,g_dd_mst.name);
	set_zd_data("0250",g_dd_mst.name);

	if(flag[0]=='1')
	{
		/*��ѯ������ϸ*/	
		get_zd_long("0540",&Beg_line); /*��ʼ���� */
		if(Beg_line=1)
		{
			int Count=0;
			Count=sql_count("dd_mst","ac_id=%ld and ac_seqn=%ld",g_dd_mst.ac_id,g_dd_mst.ac_seqn);
			set_zd_long("0550",Count);
		}			
		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%ld order by hst_cnt desc",g_dd_mst.ac_id,g_dd_mst.ac_seqn);    
		if(ret)
		{
			vtcp_log("%s,%d,������ϸ����",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
		i=0,totnum=0;
		while(1)
		{
			vtcp_log("%s,%d,��ʼ��ѯ�ʻ���ϸ��¼!",__FILE__,__LINE__);
			if( i > 9)
			{
				vtcp_log("%s,%d,�Ѿ�ȡ��10����ϸ",__FILE__,__LINE__);
				break;		
			}
			ret = Dd_mst_hst_Fet_Sel(&sDd_mst_hst,g_pub_tx.reply);
			if(ret && ret !=100)
			{
				vtcp_log("%s,%d,������ϸ����",__FILE__,__LINE__);
				Dd_mst_hst_Clo_Sel();
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}else if(ret == 100)
			{
				vtcp_log("%s,%d,�ҵ��ļ�¼��Ϊ[%d]",__FILE__,__LINE__,i);
				set_zd_data("0660","1"); /* ������־ */
				break;
			}
			totnum++;	
			if(totnum<Beg_line)
			{
				continue; /*����������д�ļ�*/
			}
			memset(mx_tmp,0,sizeof(mx_tmp));
			memset(brf6,0,sizeof(brf6));
			memcpy(brf6,sDd_mst_hst.brf,12);
			Zip_halfChinese(brf6,to_str);
								
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
		}else{
			sprintf(mx_tmp,"%02d",i);
		}
		memcpy(fd95,mx_tmp,2);
		set_zd_data("0950",fd95);
		strcpy(g_pub_tx.reply,"0000");
		vtcp_log("%s,%d,��ѯ�˻���ϸ����!fd95[%s]",__FILE__,__LINE__,fd95);
		goto OkExit;
	}else if(flag[0]=='2'){
		/*��ѯ���˻����*/
		ret = Td_mst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_dd_mst.ac_id);
		if(ret)
		{
			vtcp_log("%s,%d,�ʺŲ�����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
		memcpy(prdt_name,"����",4);
		sprintf(fd95+2,"%03d %-5s%012.0lf",g_dd_mst.ac_seqn,prdt_name,g_dd_mst.bal*100);
		j=1;
		while(1)
		{
			if(j>9)
			{
				vtcp_log("%s,%d,�����ѯ10�����ڻ�[%d]",__FILE__,__LINE__,j);
				break;
			}
			vtcp_log("%s,%d,��ʼ��ѯ�����ʻ�!",__FILE__,__LINE__);
			ret = Td_mst_Fet_Sel(&sTd_mst,g_pub_tx.reply);
			if(ret && ret !=100)
			{
				vtcp_log("%s,%d,�ʺŲ�����!",__FILE__,__LINE__);
				Td_mst_Clo_Sel();
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}else if(ret){
				vtcp_log("%s,%d,���ڻ���Ϊ[%d]",__FILE__,__LINE__,j);
				break;
			}
			ret = Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
			if(ret)
			{
				vtcp_log("%s,%d,�ò�Ʒ������!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}
			Get_Tdprdt_name(sTd_parm.prdt_no,prdt_name);
			memset(mx_tmp,0,sizeof(mx_tmp));
			sprintf(mx_tmp,"%03d %-5s%012.0lf",sTd_mst.ac_seqn,prdt_name,sTd_mst.bal*100);
			memcpy(fd95+2+j*21,mx_tmp,21);
			j++;
		}
		Td_mst_Clo_Sel();
		sprintf(mx_tmp,"%02d",j);
		memcpy(fd95,mx_tmp,2);
		set_zd_data("0950",fd95);
		vtcp_log("%s,%d,���˻���ϸ[%s]",__FILE__,__LINE__,fd95);
		strcpy(g_pub_tx.reply,"0000");
		goto OkExit;
	}
	account_amt = g_dd_mst.bal; /* �������*/
	
	sprintf(acErrMsg,"g_dd_mst.bal=[%lf] !!", g_dd_mst.bal);
	WRITEMSG
	
	sprintf(acErrMsg,"-------->account_amt===[%lf]", account_amt);
	WRITEMSG
	limit_amt = sCard_reg.atm_max_dr_cash_tx_amt - sCard_reg.atm_td_dr_cash_tx_amt;
	sprintf(acErrMsg,"-------->  ÿ��Ľ����޶�Ϊ:[%lf]", limit_amt);
	WRITEMSG
	sprintf(acErrMsg,"-------->�ÿ�������ȡ���Ϊ:[%lf]",sCard_reg.atm_td_dr_cash_tx_amt );
	WRITEMSG

	available_amt = g_dd_mst.bal - g_dd_mst.hold_amt - g_dd_mst.ctl_amt;
	sprintf(acErrMsg,"���տ������Ϊ: [%lf]  ",available_amt);
	WRITEMSG
	if(fee_mode[0]!='0' && fee_mode[0] != '6') /* ATM���ص��տ�ȡ�ֽ�� */
	{
		if (pub_base_CompDblVal(available_amt, limit_amt) == 1)
		{
			  available_amt = limit_amt;   /* ������� */		  
		}
	}else {  /*POS���ص������-Ԥ��Ȩ���-��������*/
	}
	
	sprintf(acErrMsg,"-------->available_amt===[%lf]", available_amt);
	WRITEMSG

	/* �Կ��Ǽǲ����д��� add by LiuHuafeng 2007-2-8 11:07*/
	/*�����ѯҵ��,��ȡ�ͻ�4Ԫ��2Ԫ������������,������ѯ���*/
	if(fee_mode[0]=='5')/*�����ѯ*/
	{
		vtcp_log("%s,%d ����ATM��ѯ��ȡ�ͻ�����",__FILE__,__LINE__);
		get_zd_double("0410",&dJw_fee);  /**ȡ����ATM��ѯ������**/
		memcpy(g_pub_tx.opn_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1);
    /**memcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1);**/
	}
	else
	{
		/* ��ѯ��� **/
		vtcp_log("%s,%d �Ǿ����ѯ���շ�",__FILE__,__LINE__);
		/*��82��*/
		memcpy(tmpstr,"1001156C",8);
		sprintf(tmpstr+8,"%012.0f",account_amt*100);
		memcpy(tmpstr+20,"1002156C",8);
		sprintf(tmpstr+28,"%012.0f",available_amt*100);	
		vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);	
		set_zd_data("0820",tmpstr);
		strcpy(g_pub_tx.reply,"0000");
		goto OkExit;
	}
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	/** ���⿨�������յ�һ������Ӧ������˻���  wyw 20090504
	ret =pub_base_GetParm_Str("CUPS",1,cAcno_qs);
	if(ret)
	{
		*�Ҳ�����������������������Ĭ���ʻ�TESTCARD*
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"3001231",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������������������������Ĭ���ʻ�=%s",__FILE__,__LINE__,cAcno_qs);
	}
	*****/
	/** ���⿨�������յ�һ������Ӧ������˻���  wyw 20090504 
	ret =pub_base_GetParm_Str("KFCAR",22,cAcno_qs);
	if(ret)
	{
		vtcp_log("%s,%d ���������ѷŵ�һ������Ӧ�����=%s",__FILE__,__LINE__,cAcno_qs);
		goto ErrExit;
	}**/
	ret =pub_base_GetParm_Str("CUPS",1,cAcno_qs);
	if(ret)
	{
		/*�Ҳ�����������������������Ĭ���ʻ�TESTCARD*/
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"41404",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������������������������Ĭ���ʻ�=%s",__FILE__,__LINE__,cAcno_qs);
	}
	/*�е���ȡ�ͻ��ʲ�ѯ����Ϊ4��,����������  del by hxc ���п��շѰ������봫�͹�����ֵ����
	ret = pub_base_GetParm_double("KFCAR",10,&dJw_fee);
	if (ret)
	{
		dJw_fee=2.00;
		vtcp_log("%s,%d �����ѯ��ȡ�ͻ��ķ���=%lf",__FILE__,__LINE__,dJw_fee);
	}*/
	char sbrf[21];
	memset(sbrf,0,sizeof(sbrf));
	memcpy(sbrf,"�����ѯ�շ�",12);
	set_zd_data("1211",cAcno_qs);
	set_zd_data("1215",CR_FLAG);
	/*set_zd_double("1218",KHJW_FEE - JWQURY_FEE); ������ȡ2��Ǯ������ 20090504*/
	set_zd_double("1218",JWQURY_FEE);   /**���п�������������2Ԫ**/
	set_zd_data("121A",sbrf);

	set_zd_data("1021",g_pub_tx.crd_no);	
	set_zd_data("1022","1");	/*�ʻ����Ϊ1,��ʾ���ڻ�*/
	set_zd_data("1028",g_pub_tx.draw_pwd);	
	set_zd_double("102F",dJw_fee);	
	set_zd_data("102Y",sbrf);
	set_zd_data("102K","2"); /*ת��*/	
	set_zd_data("102J","01");
	set_zd_data("0910",g_mdm_ac_rel.opn_br_no); /*��ΪҪ���ͨ��ͨ�ҽ��*/

	/*�����ѯ������������ת�͵�Ӧ����*/
	/*����Ӧ����������Ŀ*/
	memset(cAcno_qs,0,sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str("YLCSR",3,cAcno_qs); /**Mod by hxc 101218 For ���������ѯ���������˿�Ŀ5111203->5111601**/
	if (ret)
	{
		/*���鲻�����õĿ���������ȡ��Ŀ����5111601��ȡ*/
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,BH_SQATMFEE_KM,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������֧����Ŀ����������Ĭ���ʻ�=%s",__FILE__,__LINE__,cAcno_qs);
	}
	
	/** �����ѵ渶�˺� add by lzy 150109 For ����ATM��ѯ�����ѵ渶**/
	memset(zAc_no, 0 , sizeof(zAc_no));
	ret = pub_base_GetParm_Str("YLCDF",1,zAc_no);
	if(ret)
	{
		memset(zAc_no, 0 , sizeof(zAc_no));
		strncpy(zAc_no,BH_DFFEE_KM,sizeof(zAc_no)-1);
		vtcp_log("%s,%d û���ҵ��渶��Ŀ��������Ĭ�Ͽ�Ŀ=%s",__FILE__,__LINE__,zAc_no);
	}
	
	if ( pub_base_CompDblVal(dJw_fee,JWQURY_FEE) < 0 )
	{
		g_pub_tx.tx_amt1=JWQURY_FEE-dJw_fee;
		strcpy(g_pub_tx.add_ind,"0");/*�跽��־*/
		strcpy(g_pub_tx.ac_no,zAc_no);/*֧���õ渶��Ŀ*/
		memcpy(g_pub_tx.sub_tx_code,"F002",4);
	}
	else
	{
		g_pub_tx.tx_amt1=dJw_fee-JWQURY_FEE;
		strcpy(g_pub_tx.add_ind,"1");/*������־*/
		strcpy(g_pub_tx.ac_no,cAcno_qs);/*���������������˿�Ŀ*/
		memcpy(g_pub_tx.sub_tx_code,"F000",4);
	}
	
	memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*��ת��־ ��Ϊ2ת��*/
	memcpy(g_pub_tx.brf,sbrf,20);
	memcmp(g_pub_tx.cur_no,"01",2); /*����*/
	
	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/

	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
	set_zd_data("0770",sMdm_ac_rel.opn_br_no);
	vtcp_log("%s,%d,[0770]sMdm_ac_rel.opn_br_no[%s]",__FILE__,__LINE__,sMdm_ac_rel.opn_br_no);
	set_zd_data("1225","2");	 /*��ת��־*/
	set_zd_double("1226",g_pub_tx.tx_amt1);
	memcpy(g_pub_tx.tx_code,"6202",4);

	ret = pub_acct_trance();
	if(ret)
	{
		vtcp_log("%s,%d,���ô������ʧ��!",__FILE__,__LINE__);
		goto ErrExit;
	}
	/**Ϊ�渶��Ŀ����ר������,Ϊ�˽�50704�ǵ���֧�� begin... BY LZY 20150119**/
	memcpy(g_pub_tx.opn_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1); 
	/**Ϊ�渶��Ŀ����ר������,Ϊ�˽�50704�ǵ���֧�� END ... BY LZY 20150119**/
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if(ret)
	{
		vtcp_log("%s,%d,���û�ƽӿڳ���ʧ��!",__FILE__,__LINE__);
		goto ErrExit;
	}
	memcpy(g_pub_tx.reply,"0001",4);/*�����ѯ���շѷ�֧*/

	/*��82��*/
	account_amt=account_amt-dJw_fee;           /*���ؿ۳�������֮����˻������Ϣ*/
	/*available_amt=available_amt-dJw_fee;       ���ؿ۳�������֮����˻����������Ϣ*/
	
	/**Add by hxc ���¼����˻�������� 110303**/
	available_amt =0.00;
	available_amt = g_dd_mst.bal - g_dd_mst.hold_amt - g_dd_mst.ctl_amt-dJw_fee;
	sprintf(acErrMsg,"���տ������Ϊ: [%lf]  ",available_amt);
	WRITEMSG
	if(fee_mode[0]!='0' && fee_mode[0] != '6') /* ATM���ص��տ�ȡ�ֽ�� */
	{
		if (pub_base_CompDblVal(available_amt, limit_amt) == 1)
		{
			  available_amt = limit_amt;   /* ������� */		  
		}
	}else {  /*POS���ص������-Ԥ��Ȩ���-��������*/
	}
	sprintf(acErrMsg,"-------->available_amt===[%lf]", available_amt);
	WRITEMSG
	
	set_zd_double("0390", available_amt); 
	memcpy(tmpstr,"1001156C",8);
	sprintf(tmpstr+8, "%012.0f", account_amt*100);
	memcpy(tmpstr+20,"1002156C",8);
	sprintf(tmpstr+28, "%012.0f", available_amt*100);
	vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);	
	set_zd_data("0820",tmpstr);
	/*end by LiuHuafeng 2007-2-8 11:07 **/
	vtcp_log("%s,%d,�����ѯ��ȡ����",__FILE__,__LINE__);
	goto OkExit1;	 
	/* �������豸�����ռǽṹ��ֵ
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(chnl_self_trade.tel,g_pub_tx.tel);
	strcpy(chnl_self_trade.ttyp,"1");
	strcpy(chnl_self_trade.sctp, "AT");

	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	* �����׶���������ˮ�� *
	strcpy(chnl_self_trade.brf_no,"ATMG");
	strcpy(chnl_self_trade.batch_flag,"0");
	strcpy(chnl_self_trade.stat,"0");
	strcpy(chnl_self_trade.info_source,"0");
	* �Ǽ������豸�����ռ� *
	ret = pub_base_Insertselftrade(chnl_self_trade);
	if (ret != 0)
	{
		sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
		WRITEMSG
		goto ErrExit;
	}

	*  �Ǽǽ�����ˮ *
	strcpy(g_pub_tx.brf,"ATM��ѯ���!!");

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
		WRITEMSG
		goto ErrExit;
	} 
	  */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	sprintf(acErrMsg,"success!!,�����ѯ������ȡ������");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(strlen(g_pub_tx.reply)==0 || strcmp(g_pub_tx.reply,"0000")==0)
	{
		vtcp_log("%s,%d,ATM��ѯʧ��!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P102");
	}
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int Get_Tdprdt_name(char *prdt_no,char *prdt_name)
{
	vtcp_log("%s,%d,�õ����ڲ�Ʒ����!prdt_no[%s]",__FILE__,__LINE__,prdt_no);
	if(!memcmp(prdt_no,"204",3) || !memcmp(prdt_no,"205",3) || !memcmp(prdt_no,"206",3) || !memcmp(prdt_no,"207",3) || !memcmp(prdt_no,"201",3) || !memcmp(prdt_no,"202",3))
	{
		memcpy(prdt_name,"����",4);
	}else if(!memcmp(prdt_no,"209",3) || !memcmp(prdt_no,"209",3) || !memcmp(prdt_no,"209",3))
	{
		memcpy(prdt_name,"����",4);	
	}else if(!memcmp(prdt_no,"211",3) || !memcmp(prdt_no,"212",3) ||!memcmp(prdt_no,"213",3))
	{
		memcpy(prdt_name,"����",4);
	}else if(!memcmp(prdt_no,"221",3) || !memcmp(prdt_no,"222",3) || !memcmp(prdt_no,"223",3))
	{
		memcpy(prdt_name,"����",4);
	}else if(!memcmp(prdt_no,"231",3) || !memcmp(prdt_no,"232",3) || !memcmp(prdt_no,"233",3))
	{
		memcpy(prdt_name,"�汾",4);
	}else if(!memcmp(prdt_no,"241",3))
	{
		memcpy(prdt_name,"����",4);
	}else if(!memcmp(prdt_no,"242",4) || !memcmp(prdt_no,"243",3))
	{
		memcpy(prdt_name,"֪ͨ",4);
	}
	vtcp_log("%s,%d,�õ����ڲ�Ʒ����!prdt_name[%s]",__FILE__,__LINE__,prdt_name);
	return(0);
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
			}else{
				*strt = 32;
				*(strt+1) = *(strf+1);
			}
			strf=strf+2;
			strt=strt+2;
			printf("tstr[%s]\n",tstr);
		}else{
			*strt = *strf;
			strf++;
			strt++;
			printf("tstr[%s]\n",tstr);
		}
	}
	printf("%s,%d,�������[%s]\n",__FILE__,__LINE__,tstr);
	return(0);
}
