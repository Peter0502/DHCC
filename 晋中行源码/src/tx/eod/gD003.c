/***************************************************
* �� �� ��:  spD003.c
* ���������� ��ͷ��ʧ�Զ����
*
* ��    ��:  jack
* ������ڣ� 2003��3��9��
*
* �޸ļ�¼��
* ��   ��:  2003.03.14
* �� �� ��: rob
* �޸�����:
*************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "mo_loss_c.h"
#include "com_eod_exec_c.h"
#include "com_sys_parm_c.h"

struct com_sys_parm_c com_sys_parm;
extern struct com_eod_exec_c grz_com_eod_exec;/*ȫ��,�������ӳ��򴫵ݿ�������*/

gD003()
{
	int ret = 0;

	int dates = 0;		/*--�ڹ�����--*/
	int jglx = 0;
	long gsdate = 0;
	long gsdate1 = 0;

	struct mo_loss_c sMo_loss;
	struct com_parm_c sCom_parm;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));

	ret=sql_begin(); /*������*/
	MYSQLERR

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	ret=pub_base_GetSysparm(&com_sys_parm);
	MYERR

	if( grz_com_eod_exec.do_type[0]=='0' ) /*����ǰ*/
		g_pub_tx.tx_date=com_sys_parm.sys_date;
	else
		g_pub_tx.tx_date=com_sys_parm.lst_date;

	/*-------- ��ѯ�������� --------*/
	ret=Com_parm_Sel(g_pub_tx.reply,&sCom_parm,\
			"parm_code='KGTS' and parm_seqn = 1");
	MYSQLERR
	dates = atoi(sCom_parm.val);

	/*--------- �����ʧ��¼���ڵĹ�ʧ���� ---------*/
	gsdate = pub_base_daynumSUB( g_pub_tx.tx_date, dates );
	
	sprintf( acErrMsg, "--------->���� [%ld] ��ʧ�Ŀڹ�", gsdate );
	WRITEMSG

	/**��Ϊ������ԭ�� ����ֻ������**/
	ret=Mo_loss_Dec_Sel(g_pub_tx.reply,
		"loss_ind='1' and loss_date=%ld and unloss_ind='0' ",gsdate );
	MYSQLERR
 	
	while(1)
	{
		memset(&sMo_loss,0x00,sizeof(struct mo_loss_c));

		ret=Mo_loss_Fet_Sel( &sMo_loss, g_pub_tx.reply );
		if( ret == 100 ) break;
		MYSQLERR

		memset( g_pub_tx.ac_no, 0x0 ,sizeof(g_pub_tx.ac_no) );
		strcpy( g_pub_tx.ac_no, sMo_loss.ac_no );
		strcpy( g_pub_tx.opn_br_no, sMo_loss.opn_br_no );
		strcpy( g_pub_tx.tx_br_no, sMo_loss.opn_br_no );

		sprintf(acErrMsg,"�Զ�����ڹ�[%s][%ld]",
				sMo_loss.ac_no,sMo_loss.loss_date );
		WRITEMSG;

		/*-------------------- ��ʼ��� ----------------*/
		jglx = 21;
		ret=pub_acct_loss( jglx, 1 );
		if(ret)
        {
           	sprintf(acErrMsg,"��ʧ�������[%s]����!", g_pub_tx.reply );
            WRITEMSG
            goto ErrExit;
        }

		pub_base_prom_inf( g_pub_tx.opn_br_no,"1",
			"%s�Ѿ������ڹ����ޣ����Զ�����ڹ�",
			g_pub_tx.ac_no );
	}


OkExit:
	sql_commit();	/*--�ύ����--*/
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"�Զ���ҳɹ�![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 0;
ErrExit:
	sql_rollback(); /*--����ع�--*/
        sprintf(acErrMsg,"�Զ����ʧ��![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;
}
