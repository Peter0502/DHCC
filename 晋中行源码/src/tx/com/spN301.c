/*************************************************
* �� �� ��:  spN301.c
* �����������������ֹ���

*
* ��    ��:  andy
* ������ڣ� 2004��3��3��
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
	static struct mdm_ac_rel_c		mdm_ac_rel;

spN301()
{
	int ret=0;
	int addpoint=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&point_point_mst,0x00,sizeof(struct point_point_mst_c));
	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/** ȡֵ����ֵ **/
	get_zd_data("1021",point_point_mst.ac_no);	/*��/�˺�*/	
	get_zd_int("0470",&addpoint);			/*��������*/

	/**�������� **/
	/* ����˺��Ƿ���� */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,
					"ac_no='%s'",point_point_mst.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�޴��˺ţ�����!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N062");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�˺ų���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply," N063");
		goto ErrExit;
	}

	
	/* �޸Ļ������� */
	ret=Point_point_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",point_point_mst.ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"ִ��Point_point_mst_Dec_Upd��[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	ret=Point_point_mst_Fet_Upd(&point_point_mst,g_pub_tx.reply);
	if(ret == 100)	/*---��¼������---*/
	{
		/*---�����¼���������Ƚ������---*/
		strcpy(point_point_mst.ac_no,point_point_mst.ac_no);
		strcpy(point_point_mst.cif_type,"1");
		point_point_mst.beg_date=g_pub_tx.tx_date;
		point_point_mst.point=addpoint;
		point_point_mst.yt_point=0;
		point_point_mst.used_point=0;

		ret=Point_point_mst_Ins(point_point_mst);
		if(ret)
		{
			sprintf(acErrMsg,"��ӻ����������![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N065");
			goto ErrExit;
		}
		goto AAA;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"ִ��Point_point_mst_Fet_Upd ��[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	point_point_mst.yt_point = point_point_mst.point;
	addpoint=point_point_mst.point+addpoint;
	point_point_mst.point=addpoint;
	ret=Point_point_mst_Upd_Upd(point_point_mst,g_pub_tx.reply);
	if(ret !=0)
	{
		sprintf(acErrMsg,"�޸Ļ����������!");
		WRITEMSG
		goto ErrExit;
	}
	
	Point_point_mst_Clo_Upd();
	
AAA:
	 /* �Ǽǽ�����ˮ */
     if( pub_ins_trace_log() )
     {
         sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
         WRITEMSG
         goto ErrExit;
     }


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�������ֹ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�������ֹ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}




