/***************************************************
* �� �� ��:  spE502.c
* ���������� ��Ա��������
*
* ��    ��:  andy
* ������ڣ� 2004��3��9��
*
* �޸ļ�¼�� 
*   ��   ��: 20070429
*   �� �� ��:wanglei
*   �޸�����:��Ϊ���п��ƹ�Ա����Ϊǩ��״̬����
*            ���Թ�Ա���ӱ������м�¼��������ǩ��
***************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "tel_control_c.h"

int spE502()
{
	int	ret;

	struct	com_tel_c		g_com_tel;
	struct  tel_control_c   sTel_control;


	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&sTel_control, 0x0, sizeof(struct tel_control_c));


	/* ��ʼ���ṹ */
	pub_base_sysinit();

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

	/**** added 20070429****/
	ret = sql_count("com_tel_connect","tel = '%s' and ind!='1'",g_pub_tx.tel);
	if(ret > 0)
	{
		sprintf( acErrMsg, "��Ա[%s]��[%d]�ʽ��Ӽ�¼!!", g_pub_tx.tel,ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O250");
		goto ErrExit;
	}

	/**** end 20070429****/
	/**** added 20070502****/
	ret = sql_count("com_tel_connect","tw_tel = '%s' and ind!='1'",g_pub_tx.tel);
	if(ret > 0)
	{
		sprintf( acErrMsg, "��Ա[%s]��[%d]�ʽ��Ӽ�¼!!", g_pub_tx.tel,ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P093");
		goto ErrExit;
	}

	/**** end 20070502****/

	/* ��ѯ��Աǩ��״̬ */
	ret=Com_tel_Dec_Upd(g_pub_tx.reply,"tel='%s'",g_pub_tx.tel);
	if(ret&&ret!=100)
	{
		sprintf( acErrMsg, "�����Ա���α����!!" );
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

	if(g_com_tel.csts[0]=='-')
	{
		sprintf( acErrMsg, "�ù�Ա����δ���깤�����Ѿ�������!!" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"MYQT");
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

	strcpy(g_com_tel.csts,"0");

	/*���Ĺ�Ա״̬Ϊǩ�� */
	ret=Com_tel_Upd_Upd(g_com_tel,g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "���Ĺ�Աǩ��״̬����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O061");
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
