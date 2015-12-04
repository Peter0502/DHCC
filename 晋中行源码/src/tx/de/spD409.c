/***************************************************************
* �� �� ��: spD409.c
* ����������������������
* ��    ��: mike
* ������ڣ�2004��1��14��
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
#include "prdt_ac_id_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "od_mst_hst_c.h"
#include "in_mst_hst_c.h"

spD409()
{
	char ind[2];

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_hst_c	g_dd_mst_hst;
	struct td_mst_hst_c	g_td_mst_hst;
	struct ln_mst_hst_c	g_ln_mst_hst;
	struct od_mst_hst_c	g_od_mst_hst;
	struct in_mst_hst_c	g_in_mst_hst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0670",ind);					/*����ʽ*/
	get_zd_data("0680",g_pub_tx.ct_ind);				/* ��ת��־ */
	get_zd_data("0310",g_pub_tx.ac_no);				/*�ʺ�*/
    pub_base_old_acno( g_pub_tx.ac_no );
	get_zd_int("0340",&g_pub_tx.ac_seqn);				/*�˻����*/
	get_zd_data("0210",g_pub_tx.cur_no);				/*����*/
	get_zd_double("1001",&g_pub_tx.tx_amt1);			/*�˻����*/
	get_zd_data("0810",g_pub_tx.brf);					/*ժҪ*/

	/* ��Ϣ�ӽ�����Ҫ�ĸ�ֵ */
	set_zd_data("1021",g_pub_tx.ac_no);
	set_zd_int("1022",g_pub_tx.ac_seqn);
	set_zd_data("1023",g_pub_tx.note_type);
	strcpy( g_pub_tx.note_type,"" );
	set_zd_data("102B",g_pub_tx.draw_seal_yn);	
	set_zd_data("102C",g_pub_tx.pwd_mach_yn);	
	set_zd_data("102D",g_pub_tx.draw_mach);	
	set_zd_double("102I",g_pub_tx.tx_amt1);
   	set_zd_data("102J",g_pub_tx.cur_no);
   	set_zd_data("102K",g_pub_tx.ct_ind);
	strcpy(g_pub_tx.prt_ind, "0");
	/*strcpy( g_pub_tx.brf,"��Ϣ" );*/

	strcpy(g_pub_tx.add_ind,"0");
   	g_pub_tx.svc_ind = 1012;
   	strcpy(g_pub_tx.hst_ind,"1"); /* �ռ�����ϸ */

	get_zd_data("0191",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}

	if(ind[0]=='1')										/*1-��������*/
	  	goto ErrExit1;
	else if(ind[0]=='2')								/*2-��������ȡ*/
		goto ErrExit2;
	else
	{
		sprintf(acErrMsg,"����������ʽ����![%s]",ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P450");
		return 1;
	}

ErrExit1:
	set_zd_data("102Y",g_pub_tx.brf);
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�жϲ���������ʽ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�жϲ���������ʽ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}

