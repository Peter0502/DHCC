/*************************************************************
* �� �� ��: spL888.c
* ������������ĩ�����׼����
**************************************************************/
#define EXTERN
#include "public.h"
#include "gl_mst_c.h"
spL888()
{
	  int ret=0;
	  double HTOTAMT;
		char   HBR_NO[6];
		double HBADAMT;
		double HCAMT;
	  HTOTAMT=0.0;
	  HBADAMT=0.0;
	  HCAMT=0.0;
    memset(HBR_NO,0x00,sizeof(HBR_NO)); 
    ret=sql_begin(); /*������*/
    /**------- ��ʼ��ȫ�ֱ��� --------**/		
		pub_base_sysinit();
		ret=pub_eod_dzzbj();
		if( ret ) 
		{
			goto ErrExit;
		}		
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
                                                                  