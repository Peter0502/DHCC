/*************************************************
* �� �� ��:    sp9684.c
* ����������   
*              ������ĸ�ʺŷ����˻����
*
* ��    ��:    rob
* ������ڣ�   2003��07��11��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "sub_dd_mst_c.h"

sp9684()  
{ 		
	int ret=0;
	int i=0;
	struct sub_dd_mst_c	sSub_dd_mst;
	struct dic_data_c vdic_data;
	char filename[100],tmpname[100];
	char tmp_memo[100],prdt_name[31];
	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	char flag[11];

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",g_pub_tx.ac_no1);			/*ĸ�ʺ�*/
	pub_base_old_acno( g_pub_tx.ac_no1 );  	/** �Ծ��ʺŵĴ��� **/
	get_zd_data("0320",g_pub_tx.ac_no);				/*���ʺ�*/
		
	vtcp_log("ac_no1=[%s],ac_no=[%s]",g_pub_tx.ac_no1,g_pub_tx.ac_no);	
	ret=Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst,"ac_no='%s' and sub_ac_no='%s' ",g_pub_tx.ac_no1,g_pub_tx.ac_no);
	if ( ret )
	{
		sprintf(acErrMsg,"ac_no=[%s],sub_ac_no=[%s]!!ret=[%d]",g_pub_tx.ac_no1,g_pub_tx.ac_no,ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ������� */
	pub_base_strpack(sSub_dd_mst.name);
	vtcp_log("name=[%s]",sSub_dd_mst.name);
	set_zd_data("0260",sSub_dd_mst.name);
	set_zd_data("0330",g_pub_tx.ac_no1);			/*ĸ�ʺ�*/
	/* set_zd_double("0400",sSub_dd_mst.bal); */

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
   
