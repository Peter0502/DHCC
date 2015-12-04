/*************************************************
* �� �� ��:  spN302.c
* �������������ֶһ�
*
* ��    ��:  andy
* ������ڣ� 2004��3��5��
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
#include "point_mst_hst_c.h"

	static struct point_point_mst_c		point_point_mst;
	static struct point_mst_hst_c		point_mst_hst;
	static struct mdm_ac_rel_c              mdm_ac_rel;

spN302()
{
	int ret=0;
	int spoint=0;
	char memo[40];
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&point_point_mst,0x00,sizeof(struct point_point_mst_c));
	memset(&point_mst_hst,0x00,sizeof(struct point_mst_hst_c));
	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	
	/** ȡֵ����ֵ **/
	get_zd_data("1021",point_point_mst.ac_no);	/*��/�˺�*/
	get_zd_int("0460",&spoint);			/*����ʹ�û���*/
	get_zd_data("0810",memo);			/*���ܷ�������*/

	/** �������� **/
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
	if(ret==100)
	{
		sprintf(acErrMsg,"��ѯ�����������![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N064");
		goto ErrExit;
	}
	 else if(ret)
        {
                sprintf(acErrMsg,"ִ��Point_point_mst_Fet_Upd ��[%d]",ret);
                WRITEMSG
                goto ErrExit;
        }

	point_point_mst.point=point_point_mst.point-spoint;
	point_point_mst.used_point=point_point_mst.used_point+spoint;

	ret=Point_point_mst_Upd_Upd(point_point_mst,g_pub_tx.reply);
	if(ret!=0)
	{
		sprintf(acErrMsg,"�޸Ļ����������!");
		WRITEMSG
		goto ErrExit;
	}
	
	ret=Point_point_mst_Clo_Upd();

	/*�Ǽǻ���ʹ����ϸ��*/
	strcpy(point_mst_hst.ac_no,point_point_mst.ac_no);
	strcpy(point_mst_hst.cif_type,point_point_mst.cif_type);
	point_mst_hst.used_point=spoint;
	point_mst_hst.used_date=g_pub_tx.tx_date;
	strcpy(point_mst_hst.brf,memo);
	
	ret=Point_mst_hst_Ins(point_mst_hst,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǻ���ʹ����ϸ�����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N066");
		goto ErrExit;
	}
	/* ������� */
	set_zd_long("0470",point_point_mst.point);

	

	/*�Ǽǽ�����ˮ*/
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ֶһ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"���ֶһ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}




