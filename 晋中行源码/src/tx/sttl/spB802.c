/*************************************************
* �� �� ��:  spB802.c
* ���������� ����ٴ�����˹��ܡ��Ǽ��ٴ���ǼǱ���
*	         ���ٴ����ڲ��ʡ��ڲ��ʴ�������ӽ��ס�
*	         ������ɵǼ��ٴ���ǼǱ����ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��7��23��
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
#include "com_branch_c.h"
#include "mo_again_ln_c.h"

int spB802()
{
	struct	com_branch_c	sComBranch;	/* ����������� */
	struct	mo_again_ln_c	sMoAgainLn;	/* �ٴ���ǼǱ� */
	struct	in_parm_c		sInParm;	/* �ڲ���Ʒ������ */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ */
	int	ret;

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sInParm, 0x00, sizeof( struct in_parm_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoAgainLn.pact_no );		/* Э���� */
	get_zd_data( "0300", g_pub_tx.ac_no );			/* ���ɵ��ڲ��ʺ� */

	/* ����ٴ���ǼǱ����Ƿ���ڴ�Э���� */
	ret = Mo_again_ln_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoAgainLn.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_again_ln_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_again_ln_Fet_Upd( &sMoAgainLn, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ٴ���ǼǱ��в����ڴ�Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B107" );
		goto ErrExit;
	}else if( ret )
   	{
    	sprintf( acErrMsg, "ִ��Mo_again_ln_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
	if(sMoAgainLn.sts[0]!='0'){
		sprintf( acErrMsg, "�ñ��ٴ����¼��״̬�����ȳ������˽���!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "B285" );
		goto ErrExit;
	}

	/* �����ڲ��ʿ����ӽ������ɵ��ڲ��ʺŲ�ѯ�ʺ�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", g_pub_tx.ac_no );
	if( ret == 100 )
    {
		sprintf( acErrMsg, "�޴��ڲ��ʺ�![%s]", g_pub_tx.ac_no );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "A029" );
		goto ErrExit;
	}else if( ret )
	{
       	sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* �޸��ٴ���ǼǱ�����־Ϊ���� */
	strcpy( sMoAgainLn.sts, "1" );				/* ��־��1���� */
	sMoAgainLn.ac_id = sMdmAcRel.ac_id;			/* �˻�ID */

	ret = Mo_again_ln_Upd_Upd( sMoAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸��ٴ���ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_again_ln_Clo_Upd();

	set_zd_data( "0300", g_pub_tx.ac_no );
	/* ����ˮ��־ */
	/**----- xxx ---- @20030821@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	strcpy(g_pub_tx.no_show,"1");
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
