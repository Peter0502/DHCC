/*************************************************
* �� �� ��:  spN303.c
* �������������ֲ�ѯ
*
* ��    ��:  andy
* ������ڣ� 2004��3��7��
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include "pubf_data_code.h"
#define EXTERN
#include "public.h"
#include "point_point_mst_c.h"

	static struct point_point_mst_c		point_point_mst;

spN303()
{
	int ret=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	 memset(&point_point_mst,0x00,sizeof(struct point_point_mst_c));

	/** ȡֵ����ֵ **/
	get_zd_data("1021",point_point_mst.ac_no);

	/** �������� **/
	/* ��ѯ�������� */
	ret=Point_point_mst_Sel(g_pub_tx.reply,&point_point_mst,
					"ac_no='%s'",point_point_mst.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�޴��˺ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N062");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�����������![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N064");
		goto ErrExit;
	}
	
	/*  ������� */
	set_zd_long("0460",point_point_mst.used_point);
	set_zd_long("0470",point_point_mst.point);

	/** �Ǽǽ�����ˮ **/
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ֲ�ѯ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"���ֲ�ѯʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	
