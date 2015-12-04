/*************************************************
* �� �� ��:  spF020.c
* ���������� POS ����(�жϸÿ��Ǳ��п��������п�)
*            POS Ԥ��Ȩ���(�жϸÿ��Ǳ��п��������п�)
*            POS Ԥ��Ȩ��ɳ���(�жϸÿ��Ǳ��п��������п�)
*
* ��    ��:  rob
* ������ڣ� 2003��2��19��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
int spF020()
{
    int ret;

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    /**if (strncmp(g_pub_tx.crd_no,CARDHEADCODE,6) != 0 || strncmp(g_pub_tx.crd_no+6,CARD_UNION_AREA,2)!=0)**/
    if ((strncmp(g_pub_tx.crd_no,CARDHEADCODE,6) != 0 && strncmp(g_pub_tx.crd_no,ICCARDHEADCODE,6) != 0 ) || strncmp(g_pub_tx.crd_no+6,CARD_UNION_AREA,2)!=0)
    {
        vtcp_log("%s,%d ��ǰ����=[%s]�Ǳ��п���",__FILE__,__LINE__,g_pub_tx.crd_no);
        sprintf(acErrMsg,"���ò��Ǳ��п������ӽ���!!");
/* Modified by ChenMing 2007.08.21.
        goto OkExit1;
*/
    	strcpy( g_pub_tx.reply, "E099" );
        goto ErrExit; /* ���п�ֱ�ӱ��� */
    }

OkExit:
    /* ���п� */
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    /* ���п� */
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

