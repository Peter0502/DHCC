/*************************************************
* �� �� ��:  spB355.c
* ���������� ������гжһ�Ʊ��⹦��
*
* ��    ��:  Dongxy
* ������ڣ� 2006��9��14��
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
#include "mo_bank_po_c.h"
#include "mo_dep_reg_c.h"

int spB355()
{
	struct	mo_dep_reg_c 		sMoDepReg;	/* ��Ѻ�浥�Ǽǲ� */
	struct	mo_bank_po_c 		sMoBankPo;	/* ���гжһ�Ʊ�Ǽǲ� */

	char    cLoadName[50];       		   		/* �ϴ��ļ��� */
	int		ret, flag, num;
	double	dTotamt;		/** ��Ѻ�ܽ�� **/

	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sMoDepReg.pact_no );			/* ��Ѻ��Ʊ��� */

	/* ������гжһ�Ʊ���Ǽǲ����Ƿ��Ѵ��ڴ˳ж�Э���� */
	ret = Mo_dep_reg_Sel( g_pub_tx.reply, &sMoDepReg, " pact_no='%s' ", sMoDepReg.pact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "�˳ж�Э�����Ѵ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B307" );
		goto ErrExit;
	}
	else if( ret != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_dep_reg_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ���ļ�������ʱ��mo_dep_reg�� */
	if( strcmp( sMoDepReg.pact_no, "" ) )
	{
		pub_base_GenDwnFilName( cLoadName );
		ret = tab_load( cLoadName, "Mo_dep_reg", "pact_no, po_no, po_amt" );
		if( ret )
		{
			sprintf(acErrMsg,"RET [%d]",ret);
			WRITEMSG
	       		goto ErrExit;
		}
		vtcp_log("[%s][%d]-----�Ѿ�д�����ݿ�!",__FILE__,__LINE__);
		/** �����ܽ�� **/
		ret = sql_sum_double("mo_dep_reg","po_amt",&dTotamt," pact_no='%s' ",sMoDepReg.pact_no);
		if ( ret )
		{
		        sprintf( acErrMsg, "�����ܽ���[%d]", ret );
		        WRITEMSG
		        goto ErrExit;
		}

		/* Ŀǰ����һ�Ż�Ʊֻ������һ����Ѻ��Ʊ��ŵ�¼�� */
		ret = Mo_dep_reg_Dec_Sel( g_pub_tx.reply, " pact_no='%s' ", sMoDepReg.pact_no );
		if( ret!= 0 )
		{
			sprintf(acErrMsg,"ִ��Mo_dep_reg_Dec_Sel����![%d]",ret);
			WRITEMSG
       			goto ErrExit;
		}
		flag=0;
		while(1)
		{
			ret = Mo_dep_reg_Fet_Sel( &sMoDepReg, g_pub_tx.reply );
			if( ret == 100 && flag == 0 )
			{
				sprintf( acErrMsg, "����Ѻ��Ʊ��Ų�����![%s]", sMoDepReg.pact_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B308" );
				goto ErrExit;
			}
			else if( ret == 100 && flag > 0 )
			{
				break;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_dep_reg_Sel����![%d]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			num = 0;
			/* �����Ʊ�浥���ִ��� */
			num = sql_count( "mo_dep_reg", " po_no='%s' ", sMoDepReg.po_no );
			if( num <= 0 )
			{
				sprintf( acErrMsg, "������Ѻ��Ʊ���Ǽǲ�mo_dep_reg����!" );
				WRITEMSG
				goto ErrExit;
			}
			else if( num > 1 )
			{
				sprintf( acErrMsg, "һ�Ż�Ʊֻ�����Ӧһ����Ѻ��Ʊ���!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B309" );
				goto ErrExit;
			}
			flag++;

			/** �жϴ˻�Ʊ��״̬�ͽ�� **/
			ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "po_no='%s'",sMoDepReg.po_no);
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd����![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		        ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
		        if( ret == 100 )
		        {
		                sprintf( acErrMsg, "�޴˳жһ�Ʊ!" );
		                WRITEMSG
			       	sprintf( g_pub_tx.reply, "B217" );
		                goto ErrExit;
		    	}
		        else if( ret )
		    	{
		        	sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��![%d]", ret );
		        	WRITEMSG
		        	goto ErrExit;
		    	}
			/** �жϻ�Ʊ״̬�Ƿ���ȷ **/
			if( (sMoBankPo.po_sts[0] != '1' && sMoBankPo.po_sts[0] != '9') || sMoBankPo.po_ind[0] != '1')
		        {
		                sprintf( acErrMsg, "�жһ�Ʊ״̬������!po_sts=[%c],po_ind=[%c]",sMoBankPo.po_sts[0],sMoBankPo.po_ind[0] );
		                WRITEMSG
			            	sprintf( g_pub_tx.reply, "B062" );
		                goto ErrExit;
		    	}
			/** �ж�ǰ̨����Ļ�Ʊ����Ƿ�С�ڻ�Ʊʵ�ʵ�Ʊ���� **/
			ret = pub_base_CompDblVal(sMoBankPo.po_amt,sMoDepReg.po_amt);
			if ( ret )
			{
			        sprintf( acErrMsg, "ǰ̨����Ļ�Ʊ������Ʊʵ�ʵ�Ʊ�����[%d]", ret );
			        WRITEMSG
				      sprintf( g_pub_tx.reply, "B353" );
			        goto ErrExit;
			}
			sMoBankPo.po_sts[0] = '5';	/** ��Ʊ״̬��Ϊ���� **/
		        ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
		        if ( ret )
		        {
		                sprintf( acErrMsg, "�޸ĳжһ�Ʊ�ǼǱ���[%d]", ret );
		                WRITEMSG
			       	      sprintf( g_pub_tx.reply, "B068" );
		                goto ErrExit;
		        }
		        Mo_bank_po_Clo_Upd();
		}

		Mo_dep_reg_Clo_Sel ();
	}

	vtcp_log("[%s][%d]==========dTotamt=[%f]",__FILE__,__LINE__,dTotamt);

	set_zd_double( "0400", dTotamt);	/**  ��Ѻ�ܽ�� **/

	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "������Ѻ��Ʊ���" );	/* ��ע */
	/* ����ˮ��־ */
	/**----- hao ---- @20050819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoDepReg.pact_no);
	strcpy(g_pub_tx.beg_note_no,sMoDepReg.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");	/**/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
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
