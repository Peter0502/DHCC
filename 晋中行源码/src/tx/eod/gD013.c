/*************************************************************
* �� �� ��: gD013.c
* �������������޶����մ���
*
* ��    ��: rob
* �������: 2004.03.26
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "card_consum_lmt_c.h"

gD013()
{
	int ret = 0;
	long y,m,d,bz;
	struct	card_consum_lmt_c	card_consum_lmt_c;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();


    memset(&card_consum_lmt_c,0x0,sizeof(struct card_consum_lmt_c));

	ret = Card_consum_lmt_Dec_Upd(g_pub_tx.reply,"1=1");
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
		WRITEMSG
		return ret;
	}

	while(1)
	{
		ret = Card_consum_lmt_Fet_Upd(&card_consum_lmt_c,g_pub_tx.reply);
	 	if( ret==100 ) break;
	 	else if( ret )
		{
		 	sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
		 	WRITEMSG
			return ret;
		}
		
		/*** ÿ���ۼƽ����0 ***/
		card_consum_lmt_c.da_all_cash_lmt = 0.00;
		card_consum_lmt_c.da_all_amt_lmt = 0.00;
		card_consum_lmt_c.da_all_con_lmt = 0.00;

		ret = pub_base_get_ymd(&y,&m,&d,g_pub_tx.tx_date,&bz);
		if( ret )
		{
		 	sprintf(acErrMsg,"�����ж���ĩ��������!![%d]",ret);
		 	WRITEMSG
			return ret;
		}

		if( bz==4 )						/*��ĩ*/
		{
			card_consum_lmt_c.mo_all_cash_lmt=0.00;
			card_consum_lmt_c.mo_all_amt_lmt=0.00;
			card_consum_lmt_c.mo_all_con_lmt=0.00;
		}

		ret = Card_consum_lmt_Upd_Upd(card_consum_lmt_c,g_pub_tx.reply);
	 	if( ret )
		{
			sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
			WRITEMSG
		 	return ret;
		}
	}

	Card_consum_lmt_Clo_Upd();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
