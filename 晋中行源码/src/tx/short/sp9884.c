/*************************************************
* �� �� ��:  sp9884.c
* ���������� ����Ʊ�������Ʊ�ݺ����ж�ͬ�ǵǼǲ����Ƿ��Ѿ�����
*
* ��    ��:  rob
* ������ڣ� 2003��4��21�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_city_c.h"

int sp9884()
{
    int ret;
	struct mo_city_c mo_city_c;
   
	memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));

    get_zd_data("0670",mo_city_c.bill_type);
	get_zd_data("0580",mo_city_c.bill_no);

	sprintf(acErrMsg,"bill_type=[%s], bill_no=[%s]",mo_city_c.bill_type,
			mo_city_c.bill_no);
    WRITEMSG

	ret = Mo_city_Sel(g_pub_tx.reply,&mo_city_c,"bill_type='%s' and \
		  bill_no = '%s'",mo_city_c.bill_type,mo_city_c.bill_no);
    if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�����!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		sprintf(acErrMsg,"��Ʊ�ݼ�������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D161");
		goto ErrExit;
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
