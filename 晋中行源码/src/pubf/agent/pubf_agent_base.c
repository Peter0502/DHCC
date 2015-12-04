/***************************************************************
* �� �� ��: pubf_agent_base.c
* ��������: ����ҵ���ú���
* ��ڲ�����
* ��    ��: jack
* ������ڣ� 
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"
#include "pos_mobile_pay_c.h"

/***************************************************************
* �� �� ��: pubf_agent_perno
* ��������: ���ɸ��˱��
* ��ڲ�����---------------���к�����1234567����
* ��    ��: jack
* ������ڣ�2003��1��26�� 
***************************************************************/
pub_agent_perno(char *Per_no)
{
	int ret = 0;
	int iPer_no = 0;
	char Per_tmp[6];

	memset(Per_tmp,0,sizeof(Per_tmp));

	ret = sql_count("ag_peracctinfo","1=1");
	if (ret == 0)
	{
		strcpy(Per_no,"123456700001");
	}
	if (ret > 0)
	{
		ret = 0;
		ret = sql_max_string( "ag_peracctinfo","per_no", Per_no, 12,"1=1");
		if (ret != 0)
		{
			sprintf(acErrMsg,"ȡ�����˱�Ŵ�![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H002");
			goto ErrExit;
		}
		memcpy(Per_tmp,Per_no+7,5);
		iPer_no = atoi(Per_tmp);
		iPer_no = iPer_no + 1;

		memset(Per_tmp,0,sizeof(Per_tmp));
		memset(Per_no,0x0,sizeof(Per_no));
		sprintf(Per_tmp,"%05d",iPer_no);
		strcpy(Per_no,"1234567");
		strcat(Per_no,Per_tmp);

	sprintf(acErrMsg,"ת����ĸ��˱��Ϊ![%s]",Per_no);
	WRITEMSG

	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɸ��˱�ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ɸ��˱��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

/***************************************************************
* �� �� ��: pubf_agent_unitno
* ��������: ���ɵ�λ���
* ��ڲ�����---------------
* ��    ��: jack
* ������ڣ�2003��1��30�� 
***************************************************************/
pub_agent_unitno(char *Unit_no)
{
	int ret = 0;
	int iUnit_no = 0;

	ret = sql_count("ag_unitinfo","1=1");
	if (ret == 0)
	{
		strcpy(Unit_no,"1001");
	}
	if (ret > 0)
	{
		ret = 0;
		ret = sql_max_string( "ag_unitinfo","unit_no", Unit_no, 4,"1=1");
		if (ret != 0)
		{
			sprintf(acErrMsg,"ȡ���λ��Ŵ�![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H002");
			goto ErrExit;
		}
		iUnit_no = atoi(Unit_no);
		iUnit_no = iUnit_no + 1;

		memset(Unit_no,0x0,sizeof(Unit_no));
		sprintf(Unit_no,"%4d",iUnit_no);

	sprintf(acErrMsg,"ת����ĵ�λ���Ϊ![%s]",Unit_no);
	WRITEMSG

	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɵ�λ��ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ɵ�λ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}


/***************************************************************
* �� �� ��: pubf_mobile_pay()
* ��������: �ж��Ƿ�����ƶ�֧��
* ��ڲ�����
* ��    ��: jack
* ������ڣ�2003��09��02�� 
***************************************************************/
int pub_mobile_pay( char *cIn_type, char *cTtyn, char *cMarch_no )
{
	int ret = 0;
	char cTrcode[5];

	struct pos_mobile_pay_c sPos_mobile_pay;

	memset( cTrcode, 0x0, sizeof(cTrcode) );
	get_zd_data( "0160", cTrcode );

	sprintf(acErrMsg, "------tty_no=[%s], march_no=[%s]", cMarch_no, cTtyn);
	WRITEMSG

	if ( strncmp( cIn_type, "012", 3 ) == 0 && \
		( strncmp( cTrcode, "6301", 4 ) == 0 || \
		strncmp( cTrcode, "6314", 4 ) == 0 ) ) 
	{
		ret = Pos_mobile_pay_Sel( g_pub_tx.reply, &sPos_mobile_pay, \
			"march_no = '%s' and tty_no = '%s'", \
			cMarch_no, cTtyn );
		if ( ret == 0 )
		{
			goto OkExit;
		}
		else
		{
			goto NoOkExit;
		}
	}
	else
	{
		goto NoOkExit;
	}

OkExit:
	sprintf( acErrMsg, "�����ƶ�֧��!" );
	WRITEMSG
	return 0;
NoOkExit:
	sprintf( acErrMsg, "�����ƶ�֧��!" );
	WRITEMSG
	return 1;
}
