/*************************************************
* �� �� ��:    sp9977.c
* ����������   �����ʺŻ��Ի���
*
* ��    ��:    mike
* ������ڣ�   2003��08��14��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"

sp9977()
{ 		
	int		ret;
	char	addr_tmp[61];

	/** ���ݳ�ʼ�� **/
	ret = 0;
	memset(addr_tmp,0x00,sizeof(addr_tmp));
	get_zd_data("0300",g_pub_tx.ac_no);

	if ( Pub_base_Getacname(g_pub_tx.ac_no,g_pub_tx.name,&g_pub_tx.ac_id, \
		&g_pub_tx.cif_no,&g_pub_tx.tx_amt1,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"ac_no to name erro [%s] ",g_pub_tx.ac_no,g_pub_tx.cif_no);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"CHK 9977 [%s] [%d] [%d]",g_pub_tx.name,g_pub_tx.ac_id,g_pub_tx.cif_no);
	WRITEMSG

	ret = distinct_sel("cif_addr_inf","addr",addr_tmp,"cif_no=%d",g_pub_tx.cif_no);
	if ( ret )
	{
		sprintf(acErrMsg,"select addr error cif_no=[%d] ",g_pub_tx.cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C067");
		goto ErrExit;
	}
	vtcp_log(" addr_tmp=[%s] ",addr_tmp);
	set_zd_data("0961",g_pub_tx.name);
	set_zd_long("0280",g_pub_tx.cif_no);
	vtcp_log(" addr_tmp=[%s] ",addr_tmp);
	set_zd_data("0962",addr_tmp);

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
