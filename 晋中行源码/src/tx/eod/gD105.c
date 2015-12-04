/*************************************************************
* �� �� ��: gD105.c
* ����������ͬ�ǵǼǲ���¼�����ͳ��
*
* ��    ��: rob
* �������: 2003��3��8��
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

gD105()
{
    int ret=0;
	double d_amt=0.00, c_amt=0.00;


	ret=sql_begin(); /*������*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	/* ͬ�ǲ������� add by rob at 20030816 */
	ret = sql_sum_double("mo_city", "bill_amt", &d_amt, 
		"sts in ('1', '6') and cd_ind='2'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ�ƽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	ret = sql_sum_double("mo_city", "bill_amt", &c_amt, 
		"sts in ('1', '6') and cd_ind='1'");
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ�ƽ�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
		
	/******
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='GZJE' and \
		parm_seqn=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "��ѯCOM_PARM�쳣!![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	******/
	ret = sql_execute("update com_parm set val='%.2lf' where parm_code='GZJE' \
		and parm_seqn=1", d_amt - c_amt);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����COM_PARM�쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute("update com_parm set val='%.2lf' where parm_code='GZJE' \
		and parm_seqn=2", d_amt - c_amt);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����COM_PARM�쳣 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	/* end at 20050816 **/


OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ͬ�ǵǼǲ���¼����ͳ�Ƴɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	if (strcmp(g_pub_tx.reply,"0000") == 0)
		strcpy(g_pub_tx.reply,"G108");
	sprintf(acErrMsg,"ͬ�ǵǼǲ���¼����ͳ��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
