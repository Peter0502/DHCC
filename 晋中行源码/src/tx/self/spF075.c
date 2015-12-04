/*************************************************
* �� �� ��:  spF075.c
* ���������� ����POS����ӿ�/ƽ̨������ӿ�
*
* ��	 ��:  lifei
* ������ڣ� 2009-8-2 10:35:29
*
* �޸ļ�¼�� 
*	��	��:
*	�� �� ��:
*	�޸�����:
�ϵ�
TX_CODE	FLOW_CODE	SUB_TX_CODE	SUB_TX_RSLT
1	6203	0	F003	#=0001@1@#=0002@15@#=0003@16@#=0004@17@#=0005@18@#=0006@19@#=0007@20@#=0008@21@#=0009@22@#$
2	6203	1	F070	#=0001@2@#=0002@3@#=0003@4@#=0004@5@#=0005@6@#=0006@40@#=0007@41@#=0008@42@#$
3	6203	2	D003	#=0000@7@#$
4	6203	3	M001	#=0000@8@#$
5	6203	4	D003	#=0000@9@#$
6	6203	5	A016	#=0000@8@#$
7	6203	6	A016	#=0000@7@#$
8	6203	7	M002	#=0000@10@#$
9	6203	8	D099	#=0000@10@#$
10	6203	9	A017	#=0000@10@#$
11	6203	10	F071	#=0000@13@#=0002@11@#$
12	6203	11	D003	#=0000@12@#$
13	6203	12	A017	#=0000@13@#$
14	6203	13	F072	#$
15	6203	15	F036	#$
16	6203	16	F037	#$
17	6203	17	F038	#$
18	6203	18	F039	#$
19	6203	19	F040	#=0000@23@#$
20	6203	20	F041	#=0000@23@#$
21	6203	21	F042	#=0000@24@#$
22	6203	22	F046	#=0000@24@#$
23	6203	23	A017	#$
24	6203	24	A016	#$
25	6203	40	M001	#=0000@9@#$
26	6203	41	A016	#=0000@9@#$
27	6203	42	D003	#=0000@8@#$
�޸ĵ�
update tx_flow_def set sub_tx_rslt = '#=0000@45@#$'
where tx_code = '6203' and flow_code = 23

update tx_flow_def set sub_tx_rslt = '#=0000@45@#$'
where tx_code = '6203' and flow_code = 24

insert into tx_flow_def values('6203',45,'F075','#=0001@46@#$');
insert into tx_flow_def values('6203',46,'D003','#=0000@47@#$');
insert into tx_flow_def values('6203',47,'A017','#$');
insert into tx_sub_def values('F075','����pos������','spF075','0','0');

*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradelimit_c.h"
#include "card.h" /* add by LiuHuafeng 20070127 */
#ifndef PARM_ATM_AC
#define PARM_ATM_AC 1
#endif
#ifndef PARM_POS_AC
#define PARM_POS_AC 2
#endif

int spF075()
{

	int ret=0;
	char flag;
	char crd_no[25],crd2_no[25],sAc_Fee[25];
	char brf10[11],tmp_brf[51],Tmp_str[101];
	char brf20[21];
	char dc_ind[2], ct_ind[2], cz_ind[2],fee_ind[2],ysq_ind[2],sYw_type[4];
	char acc_opnbr[6],acc_no[ACC_NO_LVL3_LEN+1];  /** 20090405 wyw ��Ŀ�����õ� **/
	long cx_ind=0; /*������־*/
	char auth_no[8];
	double tx_amt=0.0;
	double dFee_amt=0.0;
	
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	memset(crd_no, 0x0, sizeof(crd_no));
	memset(crd2_no, 0x0, sizeof(crd2_no));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(ysq_ind, 0x0, sizeof(ysq_ind));
	memset(auth_no, 0x0, sizeof(auth_no));
	memset(tmp_brf, 0x0, sizeof(tmp_brf));
	memset(brf10, 0x0, sizeof(brf10));
	memset(brf20, 0x0, sizeof(brf20));
	memset(sYw_type,0,sizeof(sYw_type));
	memset(Tmp_str,0,sizeof(Tmp_str));
	memset(sAc_Fee,0,sizeof(sAc_Fee));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst,0,sizeof(struct dd_mst_c));
	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		goto ErrExit;
	}

	get_zd_data("0660",dc_ind); /*�����־*/ 
	get_zd_data("0670",cz_ind); /*������־*/ 
	get_zd_data("0680",ct_ind); /*��ת��־*/ 
	get_zd_data("0690",fee_ind); /*�շ�ģʽ*/
	get_zd_data("0700",ysq_ind); /*Ԥ��Ȩ��־ */

	get_zd_data("0300", crd_no); /*�����־��Է�*/
	get_zd_data("0300", g_pub_tx.crd_no); 
	get_zd_data("0310", crd2_no);
	
	get_zd_data("0790",g_pub_tx.draw_pwd);	/* ���� */
	get_zd_double("0400",&tx_amt);/*���׽��*/
	get_zd_double("0410",&dFee_amt);/*������*/
	get_zd_data("0810",tmp_brf);  /*ժҪ*/
	get_zd_long("0500",&cx_ind); /*������־*/
	get_zd_data("0890",g_pub_tx.note_type);/*ƾ֤����*/
	get_zd_data("0590",g_pub_tx.beg_note_no);/*ƾ֤����*/
	get_zd_data("0240",sYw_type);
	vtcp_log("%s,%d,ƾ֤����[%s],ƾ֤����[%s]",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.beg_note_no);
	memcpy(brf10,tmp_brf,10);
	memcpy(brf20,tmp_brf,20);
	strcpy(g_pub_tx.cur_no, "01");
	g_pub_tx.ct_ind[0]=ct_ind[0];
	vtcp_log("%s,%d,���[%s],��ת[%s],�շ�ģʽ[%s],����[%s],���[%.2f]",__FILE__,__LINE__,dc_ind,ct_ind,fee_ind,crd_no,tx_amt);
	vtcp_log("%s,%d,����[%s],����2[%s]ժҪ[%s]",__FILE__,__LINE__,crd_no,crd2_no,tmp_brf);
	sprintf(acErrMsg,"��������= ysq_ind[%s] ,cz_ind[%s],cx_ind[%d]",ysq_ind,cz_ind,cx_ind);
	WRITEMSG
	pub_base_old_acno(g_pub_tx.crd_no);
	strcpy(crd_no,g_pub_tx.crd_no);
	pub_base_old_acno(crd2_no);

	if (fee_ind[0]!='7')/*�Ǳ���pos���շ�*/ 
	{
			strcpy(g_pub_tx.reply,"0000");	
			goto OkExit;
	}
	else
	{
			vtcp_log("%s %d crd2_no=[%s]",__FILE__,__LINE__,crd2_no);
			set_zd_data("1021",crd2_no);
			set_zd_data("1022","0");/*�������*/
			set_zd_data("1028",g_pub_tx.draw_pwd);
			set_zd_data("102K","2");/*��ת��־*/
			set_zd_data("102J","01");
			
			if(cz_ind[0] == '1')
			{
				if (dc_ind[0]=='1')
					dFee_amt = -1 * dFee_amt;
				else
					dFee_amt = dFee_amt;
				set_zd_data("121A","����pos�����ѳ���");
				set_zd_data("102Y","����pos�����ѳ���");
				set_zd_data("1026","N"); /*�������ײ��������*/
			}
			else if(cz_ind[0]=='0')
			{
				if (dc_ind[0]=='1')
					dFee_amt = dFee_amt;
				else
					dFee_amt = -1 * dFee_amt;
				set_zd_data("121A","����pos������");
				set_zd_data("102Y","����pos������");
			}		
			
			set_zd_double("102F",dFee_amt);			
			
			memset(sAc_Fee,0x00,sizeof(sAc_Fee));
			ret = pub_base_GetParm_Str("JZCAR",PARM_POS_AC,sAc_Fee);
			if(ret)
			{
				vtcp_log("%s,%d û���ҵ��շѿ�Ŀ��������Ĭ���ʻ�=%s",__FILE__,__LINE__,sAc_Fee);
				goto ErrExit;
			}			
			
			set_zd_data("1211",sAc_Fee);  /* �����������Ŀ */
			set_zd_data("1215","2");    /*ת��*/
			set_zd_data("1214",g_pub_tx.cur_no);
			set_zd_double("1218",dFee_amt);	
			strcpy( g_pub_tx.reply, "0001" );
			goto OkExit;	
	}
	

OkExit:
	sprintf(acErrMsg,"����pos�����Ѵ���!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"����pos�����Ѵ���ʧ��!! ");
	WRITEMSG
	if( !memcmp(g_pub_tx.reply,"0000",4) || strlen(g_pub_tx.reply) ==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

