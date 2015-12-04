/*************************************************
* �� �� ��:  spE704.c
* ���������� δ���˽���Ĩ�˽���
*
* ��    ��:  
* ������ڣ� 2004��3��25��
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
#include "auth_code_c.h"
#include "tx_log_c.h"

int spE704()
{
	int	ret;
	struct auth_code_c vauth_code;
	struct tx_log_c vtx_log;
	long vau_code;
	char tmpstr[21];

	/* ��ʼ���ṹ */
	pub_base_sysinit();

	get_zd_data("0250",tmpstr);		
	vau_code=atol(tmpstr);

	ret=Auth_code_Sel( g_pub_tx.reply, &vauth_code,"auth_no=%d",vau_code);
	if( ret )
	{
		vtcp_log("����Ȩ�벻����: ��Ȩ��: [%d]", vau_code);TRACE
		strcpy( g_pub_tx.reply,"Z101" );
		goto ErrExit;
	}

	if (vauth_code.sts[0] == '9')
	{
		sprintf(acErrMsg, "����Ȩ�����Ѿ����, ����Ĩ��!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "Z202");
		goto ErrExit;
	}


	/**
	ret = Tx_log_Sel(g_pub_tx.reply, &vtx_log," trace_no=%d ",
				vauth_code.trace_no );
	if( ret )
	{
		sprintf( acErrMsg,"���ײ�����[%d]",vauth_code.trace_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z106" );
		goto ErrExit;
	}
	***/

	ret = sql_execute("delete from auth_code where auth_no=%ld",
			vauth_code.auth_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ɾ����Ȩ���������!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "Z203");
		goto ErrExit;
	}

	set_zd_data("0640",vtx_log.tx_code );

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
