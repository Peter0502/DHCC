/*************************************************
* �� �� ��:  sp9810.c
* ���������� ���ڲ���Ʒ��Ų�ѯ�ڲ���Ʒ������,
             ��������Ŀ�Ŀ������ת���ɿ�Ŀ��ǰ̨��ʾ	
*
* ��    ��:  jack
* ������ڣ� 2003��1��17�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

int sp9810()
{
	struct in_parm_c sInParm;	/* �ڲ���Ʒ��ṹ */
	struct com_item_c sComItem;	/* ������Ŀ��ṹ */
	int ret;
	double llz = 0.00;
	double fxllz = 0.00;

	pub_base_sysinit();
	get_zd_data( "1121", g_pub_tx.prdt_code ); /* ��Ʒ��� */

	ret=In_parm_Sel( g_pub_tx.reply , &sInParm ,
			 		"prdt_no='%s'" , g_pub_tx.prdt_code );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "��ѯ�ڲ��ʲ�Ʒ���������!\
			prdt_no=[%s]", g_pub_tx.prdt_code );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W058" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��In_parm_Sel����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret=Com_item_Sel( g_pub_tx.reply , &sComItem ,
			 		 "acc_hrt='%s'" , sInParm.acc_hrt );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "��ѯ������Ŀ�����!\
			acc_hrt=[%s]", sInParm.acc_hrt );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S033" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Com_item_Sel����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret = pub_base_getllz(sInParm.rate_no,sInParm.cur_no,g_pub_tx.tx_date,&llz);
	if( ret != 0 )
	{
		sprintf( acErrMsg, "ȡ����ֵʧ��!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret = pub_base_getllz(sInParm.fine_rate_no,sInParm.cur_no,
							g_pub_tx.tx_date,&fxllz);
	if( ret != 0 )
	{
		sprintf( acErrMsg, "ȡ��Ϣ����ֵʧ��!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

		sprintf( acErrMsg, "ȡ��Ϣ����ֵʧ��!sInParm.stop_date=[%ld]",
					sInParm.stop_date);
		WRITEMSG
	set_zd_data( "1128", sComItem.acc_no );     /*��Ŀ��*/
	set_zd_data( "1126", sInParm.prdt_mo );     /*��Ʒ����*/
	set_zd_data( "1127", sInParm.cur_no );      /*����*/
	set_zd_long( "1122", sInParm.stop_date );	/*��������*/
	set_zd_double( "1123", llz );				/*����ֵ*/
	set_zd_double( "1124", fxllz );				/*��Ϣ����*/
	set_zd_data( "1125", sInParm.intst_knd );   /*��Ϣ����*/

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
