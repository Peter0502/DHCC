/***************************************************************
* �� �� ��: spD095.c
* ��������������ӽ��� for nbz
* ��    ��: jack
* ������ڣ�2004��3��29��
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
#include "com_parm_c.h"
#include "mo_city_c.h"

spD095()
{
    int ret;
    char flag[2];
    struct com_parm_c com_parm_c;
    struct mo_city_c mo_city_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
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

	get_zd_double("1131",&g_pub_tx.tx_amt1);
    get_zd_data("1132",flag); /* ��������־ */
	if( flag[0]=='1' ) flag[0]='2';
	else flag[0]='1';
    set_zd_data("1132",flag); /* ��������־ */
    get_zd_data("1133",g_pub_tx.cur_no);
	get_zd_data("1135",g_pub_tx.brf);
	pub_base_strpack(g_pub_tx.brf);


  	if( flag[0] == '2')
    {
	   	strcpy(g_pub_tx.add_ind,"0"); /* ����� ���� */
		if( strcmp(g_pub_tx.brf,"")==0 )
        	strcpy(g_pub_tx.brf,"ͬ�������");
    }
   	else
    {
       	strcpy(g_pub_tx.add_ind,"1"); /* ����� ���� */
		if( strcmp(g_pub_tx.brf,"")==0 )
        	strcpy(g_pub_tx.brf,"ͬ�������");
    }

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
          parm_seqn = 2");
    if( ret!=0 )
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
 
    pub_base_strpack(com_parm_c.val);
    strcpy(g_pub_tx.ac_no,com_parm_c.val);
    pub_base_old_acno(g_pub_tx.ac_no);
    sprintf(acErrMsg,"ac_no = [%s]",g_pub_tx.ac_no);

    strcpy(g_pub_tx.ct_ind,"2");

	strcpy(g_pub_tx.ac_wrk_ind,"100");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=1001;
	get_zd_data("0191",g_pub_tx.using_ind);
    g_pub_tx.ac_seqn = 0;

	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}

	sprintf(acErrMsg,"BEG D095[%s][%s][%s][%d]",g_pub_tx.ac_no,
		g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
	WRITEMSG
	/** ���� **/

	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����40801��Ŀ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����40801��Ŀʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
