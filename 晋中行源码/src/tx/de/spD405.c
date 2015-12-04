/***************************************************************
* �� �� ��: spD405.c
* ������������ʧ����ӽ���
* ��    ��: jack
* ������ڣ�2004��1��21��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD405()
{
	int	mode=0;
	char	loss_type[3];
	char	loss_title[21];
	char	tmpname[51];
	char 	id_own[2];
	FILE *fp=NULL;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("0300",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );
	get_zd_data("0220",loss_type);
	mode=atoi(loss_type);

	get_zd_data("0690",id_own);		/** �Ƿ��� **/
	if( id_own[0]=='Y' )
		get_zd_data("0250",g_pub_tx.name);
	else if( id_own[0]=='N' )
		get_zd_data("0260",g_pub_tx.name);

	sprintf(acErrMsg,"id_own=[%s],����=[%s]",id_own,g_pub_tx.name);
	WRITEMSG
	
	/**ȡ���״���**/
	get_zd_data("0160",g_pub_tx.tx_code);	/** ���״��� **/
	/**����**/
	get_zd_data("0680",g_pub_tx.tmp_ind);	/** ������֤������ **/
	get_zd_data("0630",g_pub_tx.cal_code);	/** ������֤������ **/
	get_zd_data("1148",g_pub_tx.id_type);
	get_zd_data("1146",g_pub_tx.id_no);
	get_zd_data("1141",g_pub_tx.draw_uncon_yn);
	get_zd_data("1142",g_pub_tx.draw_pwd_yn);
	get_zd_data("1143",g_pub_tx.qry_pwd);
	get_zd_data("1144",g_pub_tx.draw_pwd);
	get_zd_data("1145",g_pub_tx.draw_id_yn);
	get_zd_data("1146",g_pub_tx.id_no);
	get_zd_data("1147",g_pub_tx.draw_seal_yn);
	get_zd_data("1149",g_pub_tx.qry_pwd_yn);
	/**Ϊ4405��������׼������-wangwk-20101221**/
	if(strcmp(g_pub_tx.tx_code,"4405") == 0)
	{
		get_zd_data("0790",g_pub_tx.draw_pwd);
		get_zd_data("0670",g_pub_tx.draw_pwd_yn);
		get_zd_data("0680",g_pub_tx.id_type);
		get_zd_data("0630",g_pub_tx.id_no);
	}
	/**����**/
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	vtcp_log("%s %d tx_code ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_code);
/*add by cgx 20091017 begin*/
	vtcp_log("%s %d �Ƿ���id_own ===[%s] ",__FILE__,__LINE__,id_own);
	vtcp_log("%s %d tmp_ind ===[%s] ",__FILE__,__LINE__,g_pub_tx.tmp_ind);
	vtcp_log("%s %d cal_code ===[%s] ",__FILE__,__LINE__,g_pub_tx.cal_code);
	vtcp_log("%s %d id_type ===[%s] ",__FILE__,__LINE__,g_pub_tx.id_type);
	vtcp_log("%s %d id_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.id_no);
	vtcp_log("%s %d draw_uncon_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_uncon_yn);
	vtcp_log("%s %d draw_pwd_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	/*vtcp_log("%s %d qry_pwd ===[%s] ",__FILE__,__LINE__,g_pub_tx.qry_pwd);
	vtcp_log("%s %d draw_pwd ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
	vtcp_log("%s %d draw_id_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_id_yn);
	vtcp_log("%s %d id_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.id_no);
	vtcp_log("%s %d draw_seal_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.draw_seal_yn);
	vtcp_log("%s %d qry_pwd_yn ===[%s] ",__FILE__,__LINE__,g_pub_tx.qry_pwd_yn);
/*add by cgx 20091017 begin*/
	g_reply_int=pub_acct_loss( mode, 0 );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��ʧ������س���!");
		WRITEMSG
		goto ErrExit;
	}

	/*** ��ӡ����� ***/
   	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	pub_base_dic_show_str(loss_title,"loss_type",loss_type);

	fprintf(fp,"%s|%s|%s|\n",g_pub_tx.ac_no,g_pub_tx.tel,loss_type);

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/*Add Complete*/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ʧ��ҳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ʧ��ҳɹ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
