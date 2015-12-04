/**************************************************************
* �� �� ��: spA016.c
* �����������������뷽�跽����
* ��    ��: cfq
* ������ڣ�2012��12��2��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA024()
{
	int ret=0;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("1201",g_pub_tx.ac_no);
	pub_base_old_acno(g_pub_tx.ac_no);
    vtcp_log(" [%s][%d] 1201 is [%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no);
    sprintf(acErrMsg,"g_pub_tx.acno= [%s]", g_pub_tx.ac_no);
    WRITEMSG
    /*add beg by ChengGX ���� 2010-1-26 10:52*/
    vtcp_log(" [%s][%d] g_pub_tx.opn_br_no [%s]\n",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    vtcp_log(" [%s][%d] g_pub_tx.tx_br_no [%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
    /*add end by ChengGX ���� 2010-1-26 10:52*/
	get_zd_double("1208",&g_pub_tx.tx_amt1);
	vtcp_log(" [%s][%d] 1208 is [%lf]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	/*{
		int i;double amt;char f120[512], fmt[5];
		for(i = 0; i<=8; i++){
			sprintf(fmt, "12%02ld", i);
			vtcp_log("fmt={%s}", fmt);
			if(3==i || 8==i ){
				get_zd_double(fmt, &amt);
				vtcp_log("%d@%s,    {%lf}\n", __LINE__, __FILE__, amt);
			}else{
				get_zd_data(fmt, f120);
				vtcp_log("%d@%s,    {%s}\n", __LINE__, __FILE__, f120);
			}	
		}
	}*/

	get_zd_data("1206",g_pub_tx.note_type);
    vtcp_log(" [%s][%d] 1206 is [%s]\n",__FILE__,__LINE__,g_pub_tx.note_type);


	get_zd_data("1207",g_pub_tx.beg_note_no);
	get_zd_data("120A",g_pub_tx.brf);
	get_zd_data("1205",g_pub_tx.ct_ind);
	get_zd_data("1204",g_pub_tx.cur_no);

	strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	strcpy(g_pub_tx.add_ind,"0");/*�跽��־*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
	g_pub_tx.svc_ind=1001;  /*��ȡ��*/
	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

	sprintf(acErrMsg,"add[%s]",g_pub_tx.add_ind);
	WRITEMSG

	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("1204",g_pub_tx.cur_no);
	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ڲ��ʽ跽���ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ڲ��ʽ跽����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
