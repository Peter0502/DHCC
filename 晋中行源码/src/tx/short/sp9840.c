/*************************************************
* �� �� ��:  sp9840.c
* ���������� ����Э���ŵ���֤ȯ�ع�Э������
*
* ��    ��:  rob
* ������ڣ� 2003��1��29�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"

int sp9840()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot;
    char flag;
    int ret;
   
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));

    get_zd_char("0670",&flag); /* �������� */
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no); /* ֤ȯ�ع�Э���� */

    if (flag == '0')
    {
        /* ¼�� */
        goto OkExit;
    }
   
    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
          "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��ǼǱ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    set_zd_char("0700",sMo_stok_rebuy_prot.tcht_type[0]); /* ֤ȯ���� */
    set_zd_double("0390",sMo_stok_rebuy_prot.par_value);  /* ֤ȯ��ֵ*/
    set_zd_double("0940",sMo_stok_rebuy_prot.pledge_rate);/* ��Ѻ��*/
    set_zd_char("0680",sMo_stok_rebuy_prot.buy_type[0]);  /* ���/�����־ */
    set_zd_double("0400",sMo_stok_rebuy_prot.bal);  /* ���/������ */
    set_zd_long("0440",sMo_stok_rebuy_prot.matr_date); /* Э�鵽���� */
    set_zd_long("0480",sMo_stok_rebuy_prot.term); /* ֤ȯ�ع����� */
    set_zd_double("0850",sMo_stok_rebuy_prot.rate); /* ������ */
    if (sMo_stok_rebuy_prot.opt_acno[0] != '\0')
    {
        set_zd_char("0690",'2');
        set_zd_data("0300",sMo_stok_rebuy_prot.opt_acno);  /* �Է��˺� */
        set_zd_data("0270",sMo_stok_rebuy_prot.opt_name);  /* �Է����� */
    }
    else 
    {
        set_zd_char("0690",'1');
    }
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
