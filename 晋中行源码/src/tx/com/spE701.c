/*************************************************
* �� �� ��:  spE701.c
* ���������� ����
*
* ��    ��:  andy
* ������ڣ� 2004��3��9��
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
#include "trace_log_c.h"
#include "com_parm_c.h"

int spE701()
{
	long vtrace_no;

	int ret = 0;
	char cAc_no[25];
	double dTram = 0.00;
	
	char cTrcode[3];	/*ȡ�������ǰ��λ*/

	struct trace_log_c sTrace_log;
	struct com_parm_c  sCom_parm;

	/* ��ʼ���ṹ */
	pub_base_sysinit();

	get_zd_long("0440",&vtrace_no);
 	sprintf(acErrMsg," q[%ld]!!",vtrace_no);
    	WRITEMSG
	/*---------add by jack 20040729------begin----*/
	memset( cAc_no, 0x0, sizeof(cAc_no));
	get_zd_data( "0300", cAc_no );
	get_zd_double( "0400", &dTram );

	sprintf(acErrMsg,"-------------> ������ˮ��Ϊ[%ld]�˺�[%s]���[%lf]", vtrace_no, cAc_no, dTram );
	WRITEMSG

	pub_base_strpack( cAc_no );
	memset( &sTrace_log, 0x0, sizeof(sTrace_log) );

	if ( strlen( cAc_no ) != 0 )	/*--��Χ����������ҵ��--*/
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld \
				and ac_no = '%s' and amt = %lf and sts = '0'", \
				vtrace_no, g_pub_tx.tx_date, cAc_no, dTram );
	}
	else
	{
		ret = Trace_log_Sel( g_pub_tx.reply, &sTrace_log, \
				"trace_no = %ld and tx_date = %ld and sts = '0'", \
				vtrace_no, g_pub_tx.tx_date );
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
	if ( !strncmp(sTrace_log.tx_code,"41",2) &&
		strcmp(sTrace_log.tx_code,"4137") && 
		strcmp(sTrace_log.tx_code,"4141") )
	{
		strcpy( g_pub_tx.reply, "S053" );
    	sprintf( acErrMsg, "���ܳ���" );
    	WRITEMSG
		goto ErrExit;
	}
	
	sprintf(acErrMsg,"-------------> ������ˮ��Ϊ[%ld]�˺�[%s]���[%lf],����tx_code[%s]", vtrace_no, cAc_no, dTram,sTrace_log.tx_code );
	WRITEMSG
	
	if( !strcmp(sTrace_log.tx_code,"5110") )/*** 2008-11-20 17:06 add by martin ȡ��������Աƾ֤ע�� ***/
		{
		strcpy( g_pub_tx.reply, "S053" );
    	sprintf( acErrMsg, "���ܳ���" );
    	WRITEMSG
		goto ErrExit;
	  }
	  
	/*** 2008/12/9 14:16 add by martin ����com_parm ����Ҳ��ܳ����Ľ��� ***/
	memset( &sCom_parm, 0x0, sizeof( struct com_parm_c ) );
	ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm,
                        "parm_code='REVOC' and val='%s'",sTrace_log.tx_code );
                if( ret==0 )
                {
                        sprintf(acErrMsg,"%s���ܳ���![%d]",sCom_parm.parm_name,ret);
                        WRITEMSG
                        
                        switch(sCom_parm.val_type[0])
                        {
                        	case '1':
                             strcpy( g_pub_tx.reply, "S081" );
                             break;
                          default:
			                       strcpy( g_pub_tx.reply, "S053" );
			                       break;
                        }
                        goto ErrExit;
                }

	
	pub_base_strpack( sTrace_log.tx_code );
	memset( cTrcode, 0x00, sizeof(cTrcode) );
	memcpy( cTrcode, sTrace_log.tx_code, 2 );
	if ( !strcmp( g_pub_tx.tx_code, "5701" ) )
	{
		if ( !strcmp( cTrcode, "62" ) )
		{
			strcpy( g_pub_tx.reply, "E031" );
    			sprintf( acErrMsg, "[%s]��ר�Ž��׳���", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
		if ( !strcmp( cTrcode, "64" ) )
		{
			strcpy( g_pub_tx.reply, "E032" );
    			sprintf( acErrMsg, "[%s]��ר�Ž��׳���", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
		/*-----------------------------------------------*
		if ( !strcmp( cTrcode, "67" ) )
		{
			strcpy( g_pub_tx.reply, "E033" );
    			sprintf( acErrMsg, "[%s]��ר�Ž��׳���", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
		*-----------------------------------------------*/
		if ( !strcmp( cTrcode, "77" ) )
		{
			strcpy( g_pub_tx.reply, "E034" );
    			sprintf( acErrMsg, "[%s]��ר�Ž��׳���", sTrace_log.tx_code );
    			WRITEMSG
			goto ErrExit;
		}
	}
	/*---------add by jack 20040729------end------*/
        sprintf(acErrMsg,"��ʼ���ú���[%ld]!!",vtrace_no);
    	WRITEMSG
	/*���ú���*/
	ret=pub_acct_rollback(vtrace_no);
	if(ret)
	{
    	sprintf(acErrMsg,"���ú�������[%s]!!",g_pub_tx.reply);
    	WRITEMSG
		goto ErrExit;
	}

    /*** �Ǽǽ�����ˮ ***/
    if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }


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
