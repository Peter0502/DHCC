/*************************************************
* �� �� ��: spC115.c
* ����������������ҵ��Ϣ¼��
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
#include "join_cop_inf_c.h"
#include "cif_basic_inf_c.h"

int spC115()
{
		int iRet = 0;
		int ret = 0;
		struct join_cop_inf_c sJoin_cop_inf;
		struct cif_basic_inf_c sCif_basic_inf;
		
		pub_base_sysinit();
		
		memset(&sJoin_cop_inf,0x00,sizeof(sJoin_cop_inf));
		memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
			
		get_zd_data("0260",sJoin_cop_inf.name);
		get_zd_long("0280",&sJoin_cop_inf.cif_no);
		get_zd_data("0670",sJoin_cop_inf.type);
		get_zd_data("0690",sJoin_cop_inf.id_type);
		get_zd_data("0620",sJoin_cop_inf.id_no);
		get_zd_data("0370",sJoin_cop_inf.br_no);
		get_zd_data("0380",sJoin_cop_inf.ag_cr_no);
		
		strcpy(sJoin_cop_inf.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(sJoin_cop_inf.tel,g_pub_tx.tel);
		
		ret=Cif_basic_inf_Sel( g_pub_tx.reply,&sCif_basic_inf,
							"cif_basic_inf.cif_no=%ld",sJoin_cop_inf.cif_no);
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
	  
	  Join_cop_inf_Debug(&sJoin_cop_inf);
	
	/*���ͻ������Ǹ��˻��ǹ�˾*/
	if(sCif_basic_inf.type[0]=='1')
	{
 		sprintf(acErrMsg,"�ÿͻ�Ϊ���˻������õǼ�!![%s]",g_pub_tx.reply);
    WRITEMSG
    strcpy(g_pub_tx.reply,"O165");
    ERR_DEAL;
  }

	sJoin_cop_inf.tx_date = g_pub_tx.tx_date;
	sJoin_cop_inf.up_date = g_pub_tx.tx_date;

	ret=sql_count("join_cop_inf","cif_no=%ld ",sJoin_cop_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		sJoin_cop_inf.cop_no=1;

		/* �Ǽǿͻ��ɶ���Ϣ�� */
		iRet = Join_cop_inf_Ins(sJoin_cop_inf,g_pub_tx.reply);
		if(iRet)
		{
			sprintf(acErrMsg,"�Ǽǿͻ��ɶ���Ϣ��ʧ�ܣ�");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data(DC_GET_MSG,acErrMsg);
			ERR_DEAL
		}
	}
	else
	{
		ret=sql_max_int("join_cop_inf","cop_no",\
						"cif_no=%ld ",sJoin_cop_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			sJoin_cop_inf.cop_no=ret+1;

		
			iRet = Join_cop_inf_Ins(sJoin_cop_inf,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg,"�Ǽǿͻ��ɶ���Ϣ��ʧ�ܣ�");
				strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				ERR_DEAL
			}

		}
	}
	set_zd_long("0350",sJoin_cop_inf.cop_no);
		/*�Ǽǽ�����ˮ*/
	g_pub_tx.ac_id = sJoin_cop_inf.cif_no;
	strcpy(g_pub_tx.brf,"��˾�ɶ���Ϣ¼��");
	g_pub_tx.ac_seqn = sJoin_cop_inf.cop_no;
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }


		
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ��ɶ���Ϣ¼��ɹ�!!�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"�ͻ��ɶ���Ϣ¼��ʧ��!!�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;		
}

