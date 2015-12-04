/***************************************************************
* �� �� ��: spD702.c
* ��������: ͸֧Э��ȡ��
* ��    ��: mike
* ������ڣ�2003��3��31��
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
#include "od_dd_rel_c.h"
#include "cif_basic_inf_c.h"

spD702()
{
	char tmp_ac[21];
	int ret=0;

	struct	od_dd_rel_c		g_od_dd_rel;
	struct	od_mst_c		f_od_mst;
	struct	cif_basic_inf_c	g_cif_basic_inf;

	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&f_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_dd_rel,0x00,sizeof(struct od_dd_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*** ȡֵ ***/
	get_zd_data("0300",g_pub_tx.ac_no);

	/*�ʺż��*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"���ʻ�������");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*��ѯ�����ļ�*/
	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"�ʺŲ������ڻ����ļ�");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*��ѯ͸֧�ʵ��ļ�*/
	ret = sql_count("od_bill","ac_id=%ld and ac_seqn=%d and pay_sts='N'", \
					g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 	if( ret>0 )
 	{
 		sprintf(acErrMsg,"���˻�����͸֧���δ����");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D153");
    	goto ErrExit;
 	}
	else if(ret<0)
 	{
 		sprintf(acErrMsg,"��ѯ͸֧�ʵ��쳣");
  		WRITEMSG
    	goto ErrExit;
 	}

	/*��ѯ�˻�͸֧�ļ�*/
	ret = Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"���˻�û��͸֧��Ʒ");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D148");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_od_mst.ac_sts[0]=='*'&&g_dd_mst.od_ind[0]=='N' )
 	{
 		sprintf(acErrMsg,"���˻���ȡ��͸֧�Ǽ�");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D150");
    	goto ErrExit;
 	}

	ret = pub_base_CompDblVal(g_od_mst.intst_acm,0.00);
	if( ret )
 	{
 		sprintf(acErrMsg,"���˻�������Ϣ����");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D147");
    	goto ErrExit;
 	}

	ret = pub_base_CompDblVal(g_od_mst.bal,0.00);
	if( ret )
 	{
 		sprintf(acErrMsg,"���˻�����͸֧���");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D146");
    	goto ErrExit;
 	}

	ret = pub_base_CompDblVal(g_od_mst.intst_bal,0.00);
	if( ret )
 	{
 		sprintf(acErrMsg,"���˻�����͸֧��Ϣ���");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D145");
    	goto ErrExit;
 	}

	/*�޸�͸֧�ļ�Ϊ����*/
	g_reply_int = Od_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
								 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"�����α����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int = Od_mst_Fet_Upd(&f_od_mst,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
	  	sprintf(acErrMsg,"ȡ��¼����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	strcpy(f_od_mst.ac_sts,"*");

	g_reply_int = Od_mst_Upd_Upd(f_od_mst,g_pub_tx.reply);
  	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"�����α����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Od_mst_Clo_Upd( );

	/*�޸Ļ����ļ�Ϊ��͸֧��Ʒ*/
	g_reply_int = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"�����α����!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int )
	{
	  	sprintf(acErrMsg,"ȡ��¼����!");
		strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}

	strcpy(g_dd_mst.od_ind,"N");

	/*--- g_reply_int=Dd_mst_Upd_Upd(g_od_mst,g_pub_tx.reply); ---*/
	g_reply_int = Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
  	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"�����α����!");
		strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}

	Dd_mst_Clo_Upd( );

	/*�Ǽ�͸֧��ʷ�ļ�
	ret = pubf_od_hst(&g_od_mst,0.00,"2","͸֧Э��ȡ��");
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�͸֧��ϸ�ļ�ʧ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"T028");
		goto ErrExit;
	}
	*/

	if( pub_ins_trace_log() )
	{
	  	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
	 	goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"͸֧Э��ȡ���ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"͸֧Э��ȡ��ʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
