/*************************************************
* �� �� ��: sp9613.c
* ������������Լ�̻�������
*
* ��    ��: rob
* ������ڣ�20030719
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "march_info_c.h"

int sp9613( )
{
    int ret;
	struct march_info_c sMarch_info;

	memset(&sMarch_info, 0x0, sizeof(struct march_info_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

	get_zd_data("0250", sMarch_info.mrchnm);
	get_zd_data("0910", sMarch_info.br_no);
	get_zd_data("0210", sMarch_info.mrchtype);
	get_zd_data("0670", sMarch_info.mrchstat);
	get_zd_data("0300", sMarch_info.mrchacno);
	get_zd_data("0260", sMarch_info.mrchaddr);
	get_zd_data("0590", sMarch_info.mrchtel);
	get_zd_data("0730", sMarch_info.mrchman);
	get_zd_double("0840", &sMarch_info.chrg);
	get_zd_long("0440", &sMarch_info.reg_date);
	get_zd_long("0450", &sMarch_info.beg_date);
	get_zd_long("0460", &sMarch_info.end_date);

	/* �����̻��� 0014302 + ... */

	ret = March_info_Ins(sMarch_info, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "���̻���Ϣ���в����¼����!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
    
	/****
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
	****/

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�����̻���Ϣ�ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����̻���Ϣʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
