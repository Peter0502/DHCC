/*************************************************
* �� �� ��:  sp9987.c
* ���������� ��Ȩ���
*
* ��    ��:  jack
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "auth_def_c.h"
#include "tx_def_c.h"
#include "auth_code_c.h"
#include "com_tel_c.h"


int sp9987()
{
	char vact_flag[11]; /*���̱�־*/
	char vlvl_flag[11]; /*��Ա�����־*/
	char txcode[5];
	char auth[5];
	struct tx_def_c tx_def_v;
	struct com_tel_c vcom_tel;
	int ret=0;
	double amt;
	char mo[41];
	char passd[21];
	char vttl[21];

	memset( vact_flag,0,sizeof(vact_flag) );
	memset( vlvl_flag,0,sizeof(vlvl_flag) );

	pub_base_sysinit();

	get_zd_data(DC_REPLY,txcode);
	get_zd_data(DC_AUTH,auth);

	/*goto OkExit;*/

	ret=Tx_def_Sel( g_pub_tx.reply,&tx_def_v,"tx_code='%s'",txcode );
	if( ret )
		goto ErrExit;

	if( tx_def_v.tx_type[0]=='4' )
	{
		sprintf(acErrMsg,"��ݲ�ѯ���ؼ��[%s]",txcode);
		WRITEMSG
		goto OkExit;
	}

	sprintf(acErrMsg,"����Ȩ����[%s]",txcode);
	WRITEMSG

	if( strlen(auth) && !strncmp(auth,"@@@",3) )
	{
		ret = pub_comm_bef_chk_auth( txcode,vact_flag,vlvl_flag,mo ) ;
		if( ret<0 ) goto ErrExit;

		ret=pub_comm_chk_far( txcode,vact_flag,vlvl_flag );
		if( ret ) goto ErrExit;
	}
	else if( strlen(auth) )
	{
		/* ret=sub_chk_loa(); */
	}
	else
	{
		ret = pub_comm_bef_chk_auth( txcode,vact_flag,vlvl_flag,mo ) ;
		if( ret==0 )
		{
			/****
			ret=pub_auth_get_amt( txcode,&amt );
			if( ret ) goto ErrExit;
			ret=pub_com_tel_amt_auth_chk( g_pub_tx.tel,amt );
			strcpy( vact_flag,"300" ); *���̱�־*
			strcpy( vlvl_flag,"" ); *��Ա�����־*
			strcpy( mo,"����Ȩ��" );
			***/
			;
		}
		if( ret ) 
		{
			if( ret>0 ) 
			{
				if( sub_reg_wrt(txcode,vact_flag,vlvl_flag,mo) ) goto ErrExit;
			}
			goto ErrExit;
		}
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"��Ȩ AUTH ����Ҫ return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��Ȩ AUTH CHK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
sub_reg_wrt( char *txcode,char *vact_flag,char *vlvl_flag,char *mo)
{
	struct auth_code_c vauth_code;
	struct auth_def_c vauth_def;
	int ret=0;
	int needauth=0;

	memset( &vauth_code,0,sizeof(vauth_code) );
	vauth_code.sts[0]='0';

	/**����ˮ������Ȩ��**/
	ret=pub_comm_make_auth_code( vact_flag[0],&vauth_code.auth_no );
	if( ret ) goto ErrExit;

	pub_base_strpack( mo );

	if( vact_flag[0]=='2' ) /**����**/
	{
		sprintf(acErrMsg,"!!��Ҫ����  ��ˮ�� %07d  %s",
				vauth_code.auth_no,mo);
		set_zd_data(DC_GET_MSG,acErrMsg);
		set_zd_long(DC_AUTH_CODE,vauth_code.auth_no);
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z002" );
	}
	if( vact_flag[0]=='3' ) /**��Ȩ**/
	{
		sprintf(acErrMsg,"*!��Ҫ��Ȩ  ��ˮ�� %07d  %s",
				vauth_code.auth_no,mo);
		set_zd_data(DC_GET_MSG,acErrMsg);
		set_zd_long(DC_AUTH_CODE,vauth_code.auth_no);
		WRITEMSG
		strcpy( g_pub_tx.reply,"Z003" );
	}
	if( vact_flag[0]=='4' ) /**����**/
	{
		vauth_code.sts[0]='9';
		needauth=0;
	}
	if( vact_flag[0]=='5' ) /**����**/
	{
		vauth_code.sts[0]='9';
		vtcp_log("�˽�������[%s][%s]",g_pub_tx.tel,vauth_code.tel);
		TRACE
		strcpy( g_pub_tx.reply,"Z005" );
	}
	if( vact_flag[0]=='6' ) /**������־**/
	{
		vauth_code.sts[0]='9';
		needauth=0;
	}

	/**д����Ȩ�Ǽǲ�**/
	strcpy( vauth_code.tx_code,txcode );
	vauth_code.trace_no = g_pub_tx.trace_no;
	strcpy( vauth_code.br_no,g_pub_tx.tx_br_no );
	vauth_code.done_trace_no=0;
	vauth_code.auth_ind[0]=vact_flag[0];
	strcpy( vauth_code.tel,g_pub_tx.tel );
	vauth_code.cond_seqn=vauth_def.seqn;

	vtcp_log(" CON[%d]",vauth_code.cond_seqn );
	vtcp_log(" LSH[%d]",vauth_code.trace_no );
	vauth_code.cond_seqn=0;

	if( Auth_code_Ins( vauth_code,g_pub_tx.reply ) )
		goto ErrExit;

	return 0;
ErrExit:
    return -1;
}
