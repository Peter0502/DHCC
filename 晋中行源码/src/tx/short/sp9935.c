/*************************************************
* �� �� ��:    sp9935.c
* ����������   
*              �����˺Ų�ѯ����
*
* ��    ��:    jane
* ������ڣ�   2003��6��23��
* �޸ļ�¼��   
*     ��    ��:
*     �� �� ��:
*     �޸�����:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9935()
{
	int ret=0,ac_seqn;
	char ac_no[20],name[51];

	/** ȡֵ����ֵ **/	
	get_zd_data("0310",ac_no);		/*�ʺ�*/

	pub_base_old_acno( ac_no );   		/* �Ծ��ʺŵĴ��� */

	/* �����ʺŲ�ѯ�˻���ź��˻�ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"ִ��Mdm_ac_rel_Sel����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

 	/*���ݳ������л�Ʋ�����ƼҪ������򲻶�����֧Ʊ*/
	if (ac_no[0]=='9')
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld and ac_seqn=%d",\
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if(ret==100)
		{
			sprintf(acErrMsg,"���˻��ǲ�����!ac_id=[%ld] and ac_seqn=[%d]",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M018");
			goto ErrExit;
		}
		else if(ret)
			{
             sprintf(acErrMsg,"ִ��Id_mst_Sel����!ret=[%d]",ret);
			 WRITEMSG
			 goto ErrExit;
			}
		set_zd_data("0250",g_in_mst.name);
	    set_zd_data("0310",ac_no);

	}
	else
{
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"���˻��ǻ��ڻ�!ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M018");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"ִ��Dd_mst_Sel����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0250",g_dd_mst.name);
    set_zd_data("0310",ac_no);
}

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
