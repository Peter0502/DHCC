/*************************************************
* �� �� ��: spF408.c
* �����������ж��Ǳ��л�������
*
* ��    ��: mike
* �������: 2004��03��28��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"

spF408()  
{ 	
    char flag[2];
	char sign[2];

    get_zd_data("0710",flag);

	get_zd_double("0420",&g_pub_tx.tx_amt1);
	sprintf(acErrMsg,"���Ϊ[%lf]",g_pub_tx.tx_amt1);
	WRITEMSG

	get_zd_data("0680",sign);
	sprintf(acErrMsg, "�����˷���: [%s], �˺�����: [%s]", sign, flag);
	WRITEMSG

	if (sign[0] == '1')
	{
		/* ��˰֧�� �����˻�ȡ�� */
		sprintf(acErrMsg,"AAAAAAAAAAAAAAAAAAAAAAA");
		WRITEMSG
		set_zd_data("0191","1");
		set_zd_data("0192","00000000");
		set_zd_data("0193","1001");
	}

    if (flag[0] == '2')
    {
        sprintf(acErrMsg,"ת���пͻ��Ĵ�����!!!");
        WRITEMSG
        goto OkExit1;
    }
	else if (flag[0] != '1')
	{
		/* ͬ�Ǵ�� */
		if (sign[0] == '0')
		{
			/* һ��ɿ��� ���� */
			if (flag[0] == '3')
			{
				/* ͬ�� */
			}
			else
			{
				/* ��� */
			}
			goto OkExit;
		}
		else
		{
			/* ��˰֧��  ��� */
			if (flag[0] == '3')
			{
				/* ͬ�� */
				goto OkExit2;
			}
			else
			{
				/* ��� */
				goto OkExit3;
			}
		}
	}
	sprintf(acErrMsg, "�������̴���Ӧ���ֽ�ɷ�����!!");
	WRITEMSG
	strcpy(g_pub_tx.reply, "H041");

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ת���пͻ��Ĵ���(����)OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"ת���пͻ��Ĵ���OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit2:
    strcpy(g_pub_tx.reply,"0002");
    sprintf(acErrMsg,"ת(ͬ�����)����OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"ת(�������)����OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
}
