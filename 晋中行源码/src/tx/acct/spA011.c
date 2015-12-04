/***************************************************************
* �� �� ��:     spA011.c
* ����������	�ڲ��ʿ�����
*
* ��    ��:     jack
* ������ڣ�    2004��1��17��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define loc_t sys_loc_t
#define EXTERN
#include "public.h"

spA011()
{
	int ret = 0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	data_init_a011();

	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.brf,"�ڲ��ʿ���");
	strcpy(g_pub_tx.ac_wrk_ind,"000");
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.prt_ind,"0");

	/***������***/
	ret=pub_acct_opn_in_mdm();
	if( ret )
	{
		sprintf(acErrMsg,"�����ʴ���![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.ac_wrk_ind,"011");
	/***����***/
	ret=pub_acct_opn_ac();
	if( ret )
	{
		sprintf(acErrMsg,"�����ʴ���![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	/*** ������� ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);
        set_zd_data("0370",g_pub_tx.ac_no);
       
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
data_init_a011()
{
	get_zd_data("1121",g_pub_tx.prdt_code);	/**��Ʒ���**/
	get_zd_long("1122",&g_pub_tx.mach_date); /**��������**/
	get_zd_data("1125",g_pub_tx.tmp_ind); /**��Ϣ����**/
	get_zd_double("1123",&g_pub_tx.rate); /**����**/
	get_zd_double("1124",&g_pub_tx.over_rate); /**��������**/
	get_zd_data("1129",g_pub_tx.name); /**����**/
}
