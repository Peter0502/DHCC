/*************************************************
* �� �� ��: spF401.c
* �����������ж����ֽ���ת��
*
* ��    ��: mike
* �������: 2004��02��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"

spF401()  
{ 	
 	char sign[2];

    if (strcmp(g_pub_tx.tx_code,"6401") == 0)
    {
        /* ��У������м�� */
    }

    get_zd_data("0710",g_pub_tx.ct_ind);
    get_zd_double("0420",&g_pub_tx.tx_amt1);

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

    sprintf(acErrMsg,"��ת��־Ϊ:[%s]",g_pub_tx.ct_ind);
	WRITEMSG


	get_zd_data("0680",sign);

	if (sign[0] == '1')
	{
		/* ��˰֧�� �����˻�ȡ�� */ 
		sprintf(acErrMsg,"AAAAAAAAAAAAAAAAAAAAAAA"); 
		WRITEMSG 
		set_zd_data("0191","1"); 
		set_zd_data("0192","00000000"); 
		set_zd_data("0193","1001"); 
	}
	else 
	{
		set_zd_data("0940","0");
	}


    if (g_pub_tx.ct_ind[0] == '1')
    {
        sprintf(acErrMsg,"ת�ֽ��ӽ���!!!");
        WRITEMSG
		set_zd_data("0670", g_pub_tx.ct_ind);
        goto OkExit1;
    }
	else
	{
		strcpy(g_pub_tx.ct_ind, "2");
	}
	set_zd_data("0670", g_pub_tx.ct_ind);

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"תת�ʴ���OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"ת�ֽ���OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
