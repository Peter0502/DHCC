/*************************************************
* �� �� ��:  sp6672.c
* ���������� �˻�����ѯ
*
* ��    ��:  ���
* ������ڣ� 20110721
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����:
* ˵����tx_code='6672' sub_tx_code='6672'
        insert into tx_def values ('6672', '�Ŵ��˻�����ѯ', '10000000000000000000000000100100000000100000000000000000000000000010000000000000000000000010000000000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('6672','0','6672','#$');
        insert into tx_sub_def values ('6672', '�Ŵ��˻�����ѯ', 'sp6672', '0', '0');
        ���룺0300�����˺š�
        �����0300���˺Ų��䣬0270������0390�������0670���˻����0910����������
              
*************************************************/
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "td_mst_c.h"
int sp6672(){
	int ret=0;
	char   cName[61];
	double m_bal=0.0;
	struct dd_mst_c m_dd_mst;
	struct td_mst_c m_td_mst;
	struct mdm_ac_rel_c m_mdm_ac_rel;
	struct prdt_ac_id_c m_prdt_ac_id;
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&m_td_mst,0,sizeof(m_td_mst) );
	memset(cName,0x00,sizeof(cName)-1);
	memset(&m_dd_mst,0,sizeof(m_dd_mst));
	memset(&m_mdm_ac_rel,0,sizeof(m_mdm_ac_rel));
	memset(&m_prdt_ac_id,0,sizeof(m_prdt_ac_id));
	pub_base_sysinit();
	get_zd_data("0300",m_mdm_ac_rel.ac_no);
	sprintf(acErrMsg," at line 37 ac_no[%s]",m_mdm_ac_rel.ac_no);
	WRITEMSG
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_no='%s' ",m_mdm_ac_rel.ac_no);
	if(ret){
		if(ret==100){
			sprintf(acErrMsg," at line 42 mdm_ac_rel�޽�� ac_no[%s]",m_mdm_ac_rel.ac_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
			goto ErrExit;
		}
		else{
			sprintf(acErrMsg,"sql error");
			WRITEMSG
			set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
	}
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&m_prdt_ac_id,"ac_id=%ld and ac_seqn=%d",m_mdm_ac_rel.ac_id,m_mdm_ac_rel.ac_seqn);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
			goto ErrExit;
		}
		else{
			sprintf(acErrMsg,"sql error");
			WRITEMSG
			set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
			goto ErrExit;
		}
	}
	if(m_prdt_ac_id.ac_id_type[0] == '1')
	{
		ret=Dd_mst_Sel(g_pub_tx.reply,&m_dd_mst,"ac_id=%ld and ac_seqn=%d",m_mdm_ac_rel.ac_id,m_mdm_ac_rel.ac_seqn);
		if(ret){
			if(ret==100){
				strcpy( g_pub_tx.reply, "XD00" );
				set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
				goto ErrExit;
			}
			else{
				sprintf(acErrMsg,"sql error");
				WRITEMSG
				set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
				goto ErrExit;
			}
		}
		m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*�������-������-Ԥ��Ȩ���*/
		memcpy(cName,m_dd_mst.name,sizeof(cName)-1);
	}else if(m_prdt_ac_id.ac_id_type[0] == '2')
	{
		ret=Td_mst_Sel(g_pub_tx.reply,&m_td_mst,"ac_id=%ld and ac_seqn=%d",m_mdm_ac_rel.ac_id,m_mdm_ac_rel.ac_seqn);
		if(ret){
			if(ret==100){
				strcpy( g_pub_tx.reply, "XD00" );
				set_zd_data("0130","XD00");/*��ʾû�в鵽���*/
				goto ErrExit;
			}
			else{
				sprintf(acErrMsg,"sql error");
				WRITEMSG
				set_zd_data("0130","ERROR[SQLִ��ʧ��]");/*��ʾ����ִ��ʧ��*/
				goto ErrExit;
			}
		}
		m_bal=m_td_mst.bal-m_td_mst.hold_amt-m_td_mst.ctl_amt;/*�������-������-Ԥ��Ȩ���*/
		memcpy(cName,m_td_mst.name,sizeof(cName)-1);
	}
	set_zd_data("0270",cName);
	set_zd_double("0390",m_bal);
	set_zd_data("0670",m_prdt_ac_id.ac_id_type);
	set_zd_data("0910",m_mdm_ac_rel.opn_br_no);
	
	set_zd_data("0130","0000");
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}