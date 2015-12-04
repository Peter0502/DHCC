/*************************************************
* �� �� ��:  spE503.c
* ���������� �������ƽ���
*
* ��    ��:  andy
* ������ڣ� 2004��3��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "tel_control_c.h"

int spE503()
{
	int	ret;

	struct	com_tel_c		g_com_tel;
	struct tel_control_c    sTel_control;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&sTel_control, 0x0, sizeof(struct tel_control_c));

	/* ��ʼ���ṹ */
	pub_base_sysinit();
	

	/******
	ret = Tel_control_Sel(g_pub_tx.reply, &sTel_control, "tel='%s' and \
		sts='0'", g_pub_tx.tel);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "select tel_control error !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		sprintf(acErrMsg, "�ù�Ա�Ѿ�������!!");
		WRITEMSG
		set_zd_data("0130", sTel_control.msg);
		strcpy(g_pub_tx.reply, "O999");
		goto ErrExit;
	}
	*****/

	/* ��ѯ��Աǩ��״̬ */
	ret=Com_tel_Dec_Upd(g_pub_tx.reply,"tel='%s'",g_pub_tx.tel);
	if(ret&&ret!=100)
	{
		sprintf( acErrMsg, "��ѯ��Ա��Ϣ�����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	ret=Com_tel_Fet_Upd(&g_com_tel,g_pub_tx.reply);
	if(ret&&ret!=100)
	{
		sprintf( acErrMsg, "��ѯ��Ա��Ϣ�����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='0')
	{
		sprintf( acErrMsg, "�ù�Ա�Ѿ�ǩ��!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O064");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='3')
	{
		sprintf( acErrMsg, "�ù�Ա�Ѿ�������,����ǩ��!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O063");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='4')
	{
		sprintf( acErrMsg, "�ù�Ա�Ѿ���ɾ��!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O062");
		goto ErrExit;
	}

	/*���Ĺ�Ա״̬Ϊǩ�� */
	strcpy(g_com_tel.csts,"0");

	ret=Com_tel_Upd_Upd(&g_com_tel,g_pub_tx.reply);
	if(ret)
	{
		sprintf( acErrMsg, "�޸Ĺ�Ա��Ϣ�����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	Com_tel_Clo_Upd( );

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
