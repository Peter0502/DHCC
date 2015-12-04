/***************************************************************
* �� �� ��: spD107.c
* ��������������ȷ�Ͻ���
* ��    ��: jack
* ������ڣ�2004��1��18��
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
#include "dd_mst_c.h"
spD107()
{
	int ret=0;
	char	tmp_no[20],tmp_ind[2];
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_hst_c	g_dd_mst_hst;
	struct td_mst_hst_c	g_td_mst_hst;
	struct dd_mst_c g_dd_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0310",g_pub_tx.ac_no);					/*ȡ�ʺ�*/
	get_zd_data("0300",tmp_no);							/*ȡ������׼��*/
	get_zd_data("0680",tmp_ind);						/*ȡ�����ֱ�־*/

	/*������ʾ�ʺŵ������ʺű��ҳ��˻�id���˻���� */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*�����˻�id���˻���ŵ���Ʒ�ʺŶ��ձ��ҳ���Ʒ����*/
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						 g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�˻����ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W012");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/*�����˻�id���˻���ŵ����ڴ�����ļ��ҳ��ʻ�״̬
	g_mdm_ac_rel.ac_id  g_mdm_ac_rel.ac_seqn
	martin update 2006.9.14  g_dd_mst_c.ac_sts[0] =='0'
	*/
	ret = Dd_mst_Sel1( g_pub_tx.reply , &g_dd_mst , "ac_id=%ld and ac_seqn=%ld" , g_mdm_ac_rel.ac_id , g_mdm_ac_rel.ac_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ڴ�����ļ��в����ڴ��ʻ���Ϣ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"B114");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_prdt_ac_id.prdt_no[0]=='1'&&g_dd_mst.ac_sts[0] =='0')  				/* 1-���� && 0-������ȷ��*/
	{
		ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						  g_prdt_ac_id.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ò�Ʒ���Ͳ�����");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W024" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		ret = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"����������ļ��α����[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D101" );
			goto ErrExit;
		}

		ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�������ļ���δ�ҵ��ü�¼[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		/*ר�û�����ʱ���޸Ŀ����ֱ�־*/
		if(g_dd_mst.ac_type[0]=='3'||g_dd_mst.ac_type[0]=='4')
		{
			strcpy(g_dd_mst.cash_ind,tmp_ind);
		}

		if(g_dd_mst.ac_sts[0]=='0') 
		{
			g_dd_mst.ac_sts[0]='1';
		}

		ret = Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"UPDATE dd_mst error %d",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D105" );
			goto ErrExit;
		}

		Dd_mst_Clo_Upd( );

		ret = sql_execute("update mo_opn_cls set check_no='%s' where ac_no='%s'", tmp_no,g_pub_tx.ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"UPDATE mo_opn_cls error %d",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D106" );
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"���˻�����Ҫ����ȷ��!!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "O096" );
		goto ErrExit;
	}

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ȷ�ϳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ȷ��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
