/*************************************************
* �� �� ��:  spN113.c
* ���������� ����ƾ֤�����ֽ���
*
* ��    ��:  andy
* ������ڣ� 2004��1��22��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "trace_log_c.h"
int spN113()
{
	char	cHstFlag[2];		/* �Ƿ�ɾ����ϸ��־Yɾ��N��ɾ��*/
								/* �����ɾ����������һ����ϸ */
	long	trace_no;			/* ��ˮ�� */
	int	ret;

	/* ��ʼ��g_pub_tx�ṹ */
	pub_base_sysinit();
	get_zd_long( "0520" , &trace_no );
	
	/* ���ó�����������ϸĬ�ϲ�ɾ�� */
	ret = pub_com_NoteCX( trace_no, 0, g_pub_tx.tx_date, "N" );
	if ( ret )
	{
		sprintf( acErrMsg, "����ƾ֤�����״�" );
		WRITEMSG
		goto ErrExit;
	}
	/*if ( strlen( cAc_no ) != 0 )	
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld \
				and ac_no = '%s' and amt = %lf and sts = '0'", \
				trace_no, g_pub_tx.tx_date, cAc_no, dTram );
	}
	else
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld and sts = '0'", \
				trace_no, g_pub_tx.tx_date );
	}
	if ( ret == 100 )
	{
		strcpy( g_pub_tx.reply, "E001" );
    	sprintf( acErrMsg, "Ҫ�������Ľ��׼�¼������" );
    	WRITEMSG
		goto ErrExit;
	}
	else if ( ret )
	{
		strcpy( g_pub_tx.reply, "D103" );
    	sprintf( acErrMsg, "��ѯ������ˮ��¼��!!!" );
    	WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack( sTrace_log.tel );
	vtcp_log("ԭ����Ա��[%s],��ȡ�����׵Ĳ���Ա��[%s]\n",sTrace_log.tel,g_pub_tx.tel);
	if ( strcmp( sTrace_log.tel, g_pub_tx.tel ) )
	{
		strcpy( g_pub_tx.reply, "E002" );
    	sprintf( acErrMsg, "�Ǳ���Ա����,���ܳ���" );
    	WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"��ʼ���ú���[%ld]!!",trace_no);
  WRITEMSG
ret=pub_acct_rollback(trace_no);
	if(ret)
	{
    	sprintf(acErrMsg,"���ú�������[%s]!!",g_pub_tx.reply);
    	WRITEMSG
		goto ErrExit;
	}
*/
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
