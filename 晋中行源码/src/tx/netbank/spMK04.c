/*************************************************
* �� �� �� :  
* �������� :  �⺣��
* ��    �� :  2015/01/08
* ����ģ�� :  �տ��˻���ϢУ��
* �������� :  spMK04.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5024', '�տ��˻���ϢУ��', '10000000000000000000001111110100000111100000110000010000001000001111111100001010010001000000100000011000000000000000000000000000', '0','4', '0');
insert into tx_flow_def values ('5024', 0, 'MK04', '#$');
insert into tx_sub_def values ('MK04', '�տ��˻���ϢУ��', 'spMK04', '0', '0');
*************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "dd_mst_c.h"			/*���ڴ�����ļ�*/
#include "td_mst_c.h"			/*���ڴ�����ļ�*/
#include "mdm_ac_rel_c.h"		/*�������˻���ϵ��*/
#include "prdt_ac_id_c.h"		/*��Ʒ�˺Ŷ��ձ�*/

int spMK04()
{
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dd_mst_c g_dd_mst;
	struct td_mst_c g_td_mst;	
	
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	
	int ret=-1;
	char ac_id_str[20]={0};
	char Ac_id_str[200]={0};
	char sTemp[10]={0};
	
	char accountNo[32+1]; /*�˺�*/
	char custName[60+1];  /*�ͻ�����*/
	
	memset(accountNo,0x00,sizeof(accountNo));
	memset(custName,0x00,sizeof(custName));
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0300",accountNo);
	get_zd_data("0370",custName);
	
	/*����˻�*/
	pub_base_strpack(accountNo);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",accountNo);
	if( 100==ret || 1403==ret ) 
	{
		vtcp_log("%s %d::���˻�������!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","���˻�������");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::��ѯ���ʺ��ʻ���ϵ�����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","��ѯ���ʺ��ʻ���ϵ�����");
		goto ErrExit;
	}
	set_zd_data("0670", g_mdm_ac_rel.note_sts); /*����״̬*/	
	 
	/**���ڶ�������**/
	ret = -1;
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( 100==ret || 1403 == ret ) 
	{
		vtcp_log("%s %d::���˻��Ų�����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","���˻��Ų�����");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::��ѯ��Ʒ�˺Ŷ��ձ����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","��ѯ��Ʒ�˺Ŷ��ձ����");
		goto ErrExit;
	}
	
	/**��ѯ���ڲ�Ʒ!***/
	if (sPrdt_ac_id.ac_id_type[0] == '1')
	{/**��ȡ�˻�״̬**/
		ret = -1;
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if( 100==ret || 1403==ret) 
		{
			vtcp_log("%s %d::���˻��Ų�����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data("0130","���˻��Ų�����");
			goto ErrExit;
		}
		else if( ret ) 
		{
			vtcp_log("%s %d::��ѯ���ڴ�����ļ������!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT06");
			set_zd_data("0130","��ѯ���ڴ�����ļ������");
			goto ErrExit;
		}
		
		set_zd_data("0660", g_dd_mst.ac_sts);/*�˻�״̬*/
		set_zd_data("0690", g_dd_mst.hold_sts);/*����״̬*/
	}
	/**��ѯ���ڲ�Ʒ!***/
	else if (sPrdt_ac_id.ac_id_type[0] == '2')               
	{/**��ȡ�˻�״̬**/
		ret = -1;
		ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if( 100==ret || 1403==ret) 
		{
			vtcp_log("%s %d::���˻��Ų�����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data("0130","���˻��Ų�����");
			goto ErrExit;
		}
		else if( ret ) 
		{
			vtcp_log("%s %d::��ѯ���ڴ�����ļ������!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT06");
			set_zd_data("0130","��ѯ���ڴ�����ļ������");
			goto ErrExit;
		}
		
		set_zd_data("0660", g_td_mst.ac_sts);/*�˻�״̬*/
		set_zd_data("0690", g_td_mst.hold_sts);/*����״̬*/
	}
	
	memset(sTemp,0x00,sizeof(sTemp));
	sprintf(sTemp,"%ld",sPrdt_ac_id.cif_no);
	set_zd_data("0860",sTemp);  /**�ͻ���**/
	set_zd_data("0240", sPrdt_ac_id.ac_id_type);  /*�˻�����*/
	
	
OkExit:
        strcpy(g_pub_tx.reply, "0000");
        vtcp_log("[%s],[%d]::Before OK return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        vtcp_log("[%s],[%d]::Before return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}











