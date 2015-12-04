/*************************************************
* �� �� ��: sp9947.c
* ���������������ʺŻ����˻���š����ʵ����ס�
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_basic_inf_c.h"
#include "mdm_attr_c.h"
#include "sub_dd_mst_c.h"

sp9947()  
{ 		
	int ret=0,tag=0;
	long ac_seqn=0;
	char ac_no[25],sub_ac_no[25];
	char ac_type[2];
	double bal;
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_basic_inf_c g_cif_basic_inf;
	struct mdm_attr_c g_mdm_attr;
	struct sub_dd_mst_c sSub_dd_mst;
	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0670",ac_type);					/*�ʻ�����*/
	get_zd_data("0310",ac_no);						/*�ʺ�*/
	pub_base_old_acno( ac_no );                 	/*�Ծ��ʺŵĴ���*/
	get_zd_data("0320",sub_ac_no);					/*�������ʺ�*/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if( ac_type[0]=='2' && \
		( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) || \
		  !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) ) \
	  )
	set_zd_data("0680","1");

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
					     g_mdm_ac_rel.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') )
	{
		sprintf(acErrMsg,"�Ǵ�ȡ�����˺�! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
		goto ErrExit;
	}

	/*** ���˻����ʵ���� ***/
	if( ac_type[0]!='3' )
	{
		if(g_mdm_ac_rel.ac_seqn==9999)
		{
			ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
			if(ret<0)
			{
				sprintf(acErrMsg,"sql_count error!! [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			else if(ret==1) 
			{
				ac_seqn = prdt_ac_id_c.ac_seqn;
			}
			else if(ret>1)
			{
				/*** ��һ��ͨ ***/
				if( strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
				{
					ac_seqn = prdt_ac_id_c.ac_seqn;
					set_zd_long("0340",ac_seqn);
				}
			}
		}
		else
			ac_seqn = g_mdm_ac_rel.ac_seqn;

		ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
							prdt_ac_id_c.cif_no);
		if( ret )
		{
			sprintf(acErrMsg,"�ͻ��Ų�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
	}
	else
	{
		ret = Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst," ac_no='%s' and sub_ac_no='%s' ",ac_no,sub_ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"�������ʺŲ�����!![%d][%s][%s]",ret,ac_no,sub_ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P447");
			goto ErrExit;
		}
		strcpy(g_cif_basic_inf.name,sSub_dd_mst.name);
	}

	set_zd_data("0310",ac_no);
	set_zd_data("0250",g_cif_basic_inf.name);

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
