/*************************************************
* �� �� ��:    sp9949.c
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
#include "string.h" 
#define EXTERN
#include "public.h"
#include "sub_dd_mst_c.h"

sp9949()  
{ 		
	int ret=0;
	int i=0;
	struct sub_dd_mst_c	sSub_dd_mst;
	struct dic_data_c vdic_data;
	char filename[100],tmpname[100];
	char tmp_memo[100],prdt_name[31];
	FILE *fp;

	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",g_pub_tx.ac_no1);			/*ĸ�ʺ�*/
	pub_base_old_acno( g_pub_tx.ac_no1 );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0300",g_pub_tx.ac_no1);	/* ������ʺ� */
	get_zd_data("0310",g_pub_tx.ac_no);				/*���ʺ�*/
		
	vtcp_log("ac_no1=[%s],ac_no=[%s]",g_pub_tx.ac_no1,g_pub_tx.ac_no);	
	ret=Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst,"ac_no='%s' and sub_ac_no='%s' ",g_pub_tx.ac_no1,g_pub_tx.ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"ac_no=[%s],sub_ac_no=[%s]!!ret=[%d]",g_pub_tx.ac_no1,g_pub_tx.ac_no,ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ������� */
	pub_base_strpack(sSub_dd_mst.name);
	vtcp_log("name=[%s]",sSub_dd_mst.name);
	set_zd_data("0250",sSub_dd_mst.name);
	set_zd_double("0400",sSub_dd_mst.bal);

	/*��ʼ���ز˵�*/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	/***ժҪѡ���б�***/
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='tran_memo'");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);
		if(i==0)
		strcpy(tmp_memo,vdic_data.pr_seq);

		fprintf(fp,"0890|%-10s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
   	set_zd_data("0890",tmp_memo);			 	/*ժҪ*/

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
   
