/*************************************************
* �� �� ��:  spJLZ0.c
* ���������� MBFE���˳�ʼ������,�����ײ�ִ���κ�ҵ���߼���
*			       ��������Щ�������ʼ��һ�£���ֹ�±߳���ȡ
*            �������ݳ�������
*             1,fd52 =0 ,2,fd3==qsbrno,3fd7 5001
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-10-7 17:46
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"

int spJLZ0()
{
	long iSubpackIndex=0;
	pub_base_sysinit();
	set_zd_long("0520",iSubpackIndex); /* ��ǰ���� */
	set_zd_long("0530",iSubpackIndex); /* spJJJ1��Ҫ����ļ�¼��� ��ʼֵ=0 */	
	set_zd_data(DC_TX_BR_NO, QS_BR_NO );
	set_zd_data(DC_TEL,"5001");
	memcpy(g_pub_tx.reply,"0000",4);
OkExit:
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

