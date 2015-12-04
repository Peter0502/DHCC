
/*************************************************
* �� �� ��: spC113.c
* ������������˾�߹���Ϣ¼��
*
* ��    ��: lwb
* ������ڣ�2014��10��14��
*
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_manage_inf_c.h"
#include "cif_basic_inf_c.h"

int spC113()
{
		int iRet = 0;
		int ret = 0;
		struct cif_manage_inf_c sCif_manage_inf;
		struct cif_basic_inf_c sCif_basic_inf;
		
		pub_base_sysinit();
		
		memset(&sCif_manage_inf,0x00,sizeof(sCif_manage_inf));
		memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
			
		get_zd_data("0260",sCif_manage_inf.name);
		get_zd_long("0280",&sCif_manage_inf.cif_no);
		get_zd_long("0290",&sCif_manage_inf.manage_no);
		get_zd_data("0670",sCif_manage_inf.type);
		get_zd_data("0680",sCif_manage_inf.sex);
		get_zd_data("0690",sCif_manage_inf.id_type);
		get_zd_data("0620",sCif_manage_inf.id_no);
		
		strcpy(sCif_manage_inf.br_no,g_pub_tx.tx_br_no);
    strcpy(sCif_manage_inf.tel,g_pub_tx.tel);
		
		ret=Cif_basic_inf_Sel( g_pub_tx.reply,&sCif_basic_inf,
							"cif_basic_inf.cif_no=%ld",sCif_manage_inf.cif_no);
		if( ret==100)
		{
			sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C007");
			ERR_DEAL;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;
	  }
	
	/*���ͻ������Ǹ��˻��ǹ�˾*/
	if(sCif_basic_inf.type[0]=='1')
	{
 		sprintf(acErrMsg,"�ÿͻ�Ϊ���˻������õǼ�!![%s]",g_pub_tx.reply);
    WRITEMSG
    strcpy(g_pub_tx.reply,"O165");
    ERR_DEAL;
  }

	sCif_manage_inf.tx_date = g_pub_tx.tx_date;
	sCif_manage_inf.up_date = g_pub_tx.tx_date;
	
	ret=sql_count("cif_manage_inf","cif_no=%ld ",sCif_manage_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		sCif_manage_inf.manage_no=1;

		/* �Ǽǿͻ��߹���Ϣ�� */
		iRet = Cif_manage_inf_Ins(sCif_manage_inf,g_pub_tx.reply);
		if(iRet)
		{
			sprintf(acErrMsg,"�Ǽǿͻ��߹���Ϣ��ʧ�ܣ�");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data(DC_GET_MSG,acErrMsg);
			ERR_DEAL
		}
	}
	else
	{
		ret=sql_max_int("cif_manage_inf","manage_no",\
						"cif_no=%ld ",sCif_manage_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			sCif_manage_inf.manage_no=ret+1;

		
			iRet = Cif_manage_inf_Ins(sCif_manage_inf,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg,"�Ǽǿͻ��߹���Ϣ��ʧ�ܣ�");
				strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				ERR_DEAL
			}

		}
	}
	set_zd_int("0350",sCif_manage_inf.manage_no);
		/*�Ǽǽ�����ˮ*/
	g_pub_tx.ac_id = sCif_manage_inf.cif_no;
	strcpy(g_pub_tx.brf,"��˾�߹���Ϣ¼��");
	g_pub_tx.ac_seqn = sCif_manage_inf.manage_no;
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }


		
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ��߹ܹ�����Ϣ¼��ɹ�!!�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"�ͻ������߹�¼��ʧ��!!�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;		
}
