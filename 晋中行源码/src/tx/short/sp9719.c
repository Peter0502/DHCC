/*************************************************
* �� �� ��: sp9719.c
* �������������ݿͻ��źͺ�ͬ��Ż�����Ϣ
*
* ��    ��: rob
* ������ڣ�2003��03��31��
* �޸ļ�¼�� 
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_crdt_code_c.h"
#include "cif_crdt_inf_c.h"
#include "cif_basic_inf_c.h"

sp9719()  
{ 		
	int ret=0;
	char tmp_title[41];

	struct cif_basic_inf_c	g_cif_basic_inf;
	struct cif_crdt_code_c	g_cif_crdt_code;
	struct cif_crdt_inf_c	g_cif_crdt_inf;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_crdt_code,0x00,sizeof(struct cif_crdt_code_c));
	memset(&g_cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));

	/*ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ��*/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_long("0280",&g_cif_crdt_code.cif_no);			/*�ͻ���*/
	get_zd_data("0580",g_cif_crdt_code.credit_no);			/*��ͬ���*/


		sprintf(acErrMsg,"�ͻ��Ų�����[%ld]",g_cif_crdt_code.cif_no);
		WRITEMSG
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
							g_cif_crdt_code.cif_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�ͻ��Ų�����");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�ͻ�������Ϣ�����[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	ret=Cif_crdt_code_Sel(g_pub_tx.reply,&g_cif_crdt_code, \
							"cif_no=%ld and credit_no='%s'", \
							g_cif_crdt_code.cif_no, \
							g_cif_crdt_code.credit_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�ÿͻ������ڴ����ź�ͬ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C084");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�ͻ����Ŵ��Ŷ��ձ����[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	ret=Cif_crdt_inf_Sel(g_pub_tx.reply,&g_cif_crdt_inf, \
						"credit_no='%s'", g_cif_crdt_code.credit_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�����ڸ����ź�ͬ��Ϣ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C085");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�ͻ�������Ϣ�����[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	set_zd_data("0210",g_cif_crdt_inf.cur_no);
	set_zd_double("0400",g_cif_crdt_inf.bal);
	set_zd_data("0680",g_cif_crdt_inf.cyc_ind);

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
   
