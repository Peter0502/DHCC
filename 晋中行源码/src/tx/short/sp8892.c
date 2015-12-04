/*************************************************
* �� �� ��:  sp8892.c
* ���������� �жϸÿͻ��Ƿ����;�������������б����ֻ����в�ѯ�˺ź��ֻ����Ƿ�ƥ��
* ��    ��:  lwb
* ������ڣ� 2015��4��15��
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "cif_basic_inf_c.h"
#include "mob_acct_type_c.h"
#include "mdm_ac_rel_c.h"

int sp8892()
{
	int ret = 0;
	char cAcno[21];	
	char mobile[15];
	
	struct mob_acct_type_c sMob_acct_type;	/*�������пͻ��˺������Ʒ�ֶ�Ӧ��*/
	struct cif_basic_inf_c sCif_basic_inf;	/*�ͻ�������Ϣ��*/
	struct dd_mst_c sDd_mst;								/*�����˻���Ϣ��*/
	struct mdm_ac_rel_c sMdm_ac_rel;				/*�������˻���ϵ��*/
	
	/*���*/
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));

	
	/*ǰ̨ȡֵ*/
	get_zd_data("0370",cAcno);
	sprintf(acErrMsg, "�˺�Ϊ��[%s]", cAcno);
	WRITEMSG
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",cAcno);
	if(ret == 100)
	{
   		vtcp_log("[%s],[%d] mdm_ac_rel �����޴��˺�!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"C115");
		goto ErrExit;
	}
	else if(ret)
	{
		vtcp_log("[%s][%d]��ѯ��mdm_ac_rel����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"L182");
		goto ErrExit;
	}
	
	if(sMdm_ac_rel.note_sts[0]=='*')
	{
		vtcp_log("[%s][%d]�ý���������!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
	if(ret == 100)
	{
		vtcp_log("[%s][%d]���˺ŷǻ����˺�!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
  	
	}	
	else if(ret)
	{
		vtcp_log("[%s][%d]��ѯdd_mst����![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		goto ErrExit;	
	}

	if(sDd_mst.ac_sts[0] == '*')
  	{
		vtcp_log("[%s][%d]���˺�������![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"P245");
		goto ErrExit;		
  	}
	
	/*��ѯ�������пͻ��˺������Ʒ�ֶ�Ӧ��*/
	ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no='%s'  ",cAcno);
	
	if(ret == 100)
	{
		vtcp_log("[%s],[%d] �˿ͻ�δ��������ǩԼ���ѽ�Լ!",__FILE__,__LINE__);
		goto ErrExit;
	}
	else if( ret )
	{
		vtcp_log("[%s][%d]��ѯ��Mob_acct_type_Sel����!",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"D104");
    goto ErrExit; 
	}

	sprintf(acErrMsg,"���������пͻ��˺������Ʒ�ֶ�Ӧ�� PASS!");
	strcpy(mobile,sMob_acct_type.mobile);   /*��¼�ֻ���*/

	 /*���ֻ��Ŵ���ǰ̨*/
	 set_zd_data("0810",mobile);

	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
}

