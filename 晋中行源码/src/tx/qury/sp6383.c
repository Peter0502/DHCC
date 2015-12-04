/***********************************************************
*�ļ���: sp6383.c
*��������: �ж������˻��Ƿ�Ϊ�ÿͻ����һ��δ���������˻���
�������жϸÿͻ��Ƿ���ǩԼ���������ڿ��Ƶ��ÿͻ��������
һ���˻�ʱ��ȥ�������˻�,��������67�򷵻ء�0��,�����������ء�1��
*����: ������
*����: 20130811
***********************************************************/

#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "netbank_cif_ukey_rel_c.h"

int sp6383()
{
	int ret;
	char sAcno[30];
	struct mdm_ac_rel_c mdm_ac_rel;
	struct prdt_ac_id_c prdt_ac_id;
	struct dd_mst_c dd_mst;
	struct netbank_cif_ukey_rel_c netbank_cif_ukey_rel;
	
	ret=0;
	memset(sAcno,0x00,sizeof(sAcno));
	memset(&mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	memset(&dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&netbank_cif_ukey_rel, 0x00, sizeof(struct netbank_cif_ukey_rel_c));

	pub_base_sysinit();
	/*��ǰ̨ȡֵ*/
	get_zd_data("0300",sAcno);
	vtcp_log("ǰ̨�����˺�Ϊ:[%s] [%s]--[%d]",sAcno,__FILE__,__LINE__);
	pub_base_old_acno( sAcno );                 /**�Ծ��ʺŵĴ���**/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",sAcno );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",sAcno);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id,"ac_id=%ld", mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%ld]",mdm_ac_rel.ac_id);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�����ʺų���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(memcmp(prdt_ac_id.ac_id_type,"1",1)==0)
	{
		vtcp_log("���˺ſͻ���Ϊ:[%s] [%d]--[%d]",__FILE__,__LINE__,prdt_ac_id.cif_no);
		ret = sql_count("dd_mst","cif_no=%ld and ac_sts='1'",prdt_ac_id.cif_no);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==1 ) 
		{
		vtcp_log("���˺ſͻ���Ϊ:[%s] [%d]--[%d]",__FILE__,__LINE__,prdt_ac_id.cif_no);
			ret = Netbank_cif_ukey_rel_Sel( g_pub_tx.reply,&netbank_cif_ukey_rel,"cif_no=%ld and sts='0'",prdt_ac_id.cif_no );
			if( ret==100 )
			{
				vtcp_log("�ÿͻ�δǩԼ���������Ѿ���Լ!",prdt_ac_id.cif_no,__FILE__,__LINE__);
				set_zd_data("0670","0");
				goto OkExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"���ҳ���[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			vtcp_log("�ÿͻ�����ǩԼ��������ʾǰ̨������������������!",__FILE__,__LINE__);
			set_zd_data("0670","1");
			goto OkExit;
		}
		else if( ret>1 )
		{
		vtcp_log("�ÿͻ����ж������״̬�����˻�������Ҫ������!",prdt_ac_id.cif_no,__FILE__,__LINE__);
		set_zd_data("0670","0");
		}
	}
	else
	{
	vtcp_log("���˻���Ϊ�����˻���ֱ�������˳�!",prdt_ac_id.ac_id_type,__FILE__,__LINE__);
	set_zd_data("0670","0");
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply is [%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before ERROR return: reply is[%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
