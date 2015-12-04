/*************************************************
* �� �� ��: spG002.c
* �����������������մ������س������
*
* ��    ��:  
* ������ڣ�
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <dlfcn.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_eod_exec_c.h"

struct com_eod_exec_c grz_com_eod_exec; /**ȫ�֣��������ӳ��򴫵ݿ�������**/
char pgm[100];

int spG002( )
{
	int	ret;
	
	get_zd_data("0330",pgm);

	sprintf( acErrMsg, "------------����pgm test begin!!!![%s]",pgm);
	WRITEMSG
	
	ret = cs_eod_deal( );
	if ( ret ) 
	{
		sprintf( acErrMsg, "����pgm test ERROR !!!!");
		WRITEMSG
		goto ErrExit;
	}	
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg, "------------����pgm test OK!!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "------------����pgm test BAD[%s]!!!!",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * ��  �� ����  pub_eod_deal
 * ���� ����:   
 *		�������մ������,���ⲿ���׵���
 * ��  ����
 *     ���룺  
 *		char	*c_cnt		���յڼ���ִ�� 1 ��һ�Σ�2 �ڶ���
 *		char	*do_type	����ִ��ʱ�� 
 *					0����ǰ 1���� 2���ƺ�		
 *		long sys_date,		ϵͳ��������
 *
********************************************************************/
int cs_eod_deal( )
{
	struct com_sys_parm_c 	sSysParm;	/* ����ϵͳ������ */
	struct com_eod_exec_c	sEodExec;	/* ���մ���˳��� */
	char	cDoType[2];			/* ִ��ʱ�� 0����ǰ 1���� 2���ƺ� */

	int	ret, i;
	
	memset( &sSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
			
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��¼������־��ʼ */
	/*pub_eod_WriJk("%s","========�� ��==============�� ��========");
   	pub_eod_WriJk("����TEST[%s]��ʼ",pgm);*/


		/**�������ص�����**/
		ret=sql_commit();

		if ( pub_base_GetSysparm( &sSysParm ) )
		{
			sprintf( acErrMsg, "ȡ����ϵͳ�������!" );
			WRITEMSG
			goto ErrExit;			
		}
	
		set_zd_long( DC_DATE , sSysParm.sys_date );        /* �������� */
		set_zd_data( DC_TEL , "9999" );

		ret = cs_eod_ExecProg( 0, "", sSysParm.sys_date );
		if ( ret )
		{
			sprintf( acErrMsg,"����TEST[%s]ʧ��",pgm);
			WRITEMSG
		/*	pub_eod_WriJk("����TEST[%s]ʧ��",pgm);
		        pub_eod_WriJk("========�� �� =============�� ��========");*/
			goto ErrExit;
		}
 /*   pub_eod_WriJk("����TEST[%s]�ɹ�",pgm);*/

		/**����������**/
		sql_begin();	

   /* pub_eod_WriJk("========�� ��==============�� ��========");*/
	
OkExit:
	return 0;
ErrExit:
	return -1;
}
/**********************************************************************
 * ��  �� ����  exec_eod_prog
 * ���� ����:   
 *		�������մ������ִ�в���¼������־
 *		            
 * ����/ʱ�䣺  jack//2003��1��16��
 * 
 * ��  ����
 * ���룺  
 *		char	*c_cnt		���յڼ���ִ�� 1 ��һ�Σ�2 �ڶ���
 *		char	*do_type	����ִ��ʱ�� 
 *					0����ǰ 1���� 2���ƺ�		
 *		long sys_date,		ϵͳ��������
 *
 * �����  
 *
 * ����ֵ��  0:�ɹ�  -1��ʧ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *		jack--*--20030305--*--��system���ø�Ϊdlopen�Ķ�̬�����
 *
********************************************************************/
int	cs_eod_ExecProg( char *fc_cnt, char *do_type, long sys_date)
{
	struct com_eod_exec_c sEodExec;
	struct com_eod_exec_c cExec[256];
	int iSeq[256];
	int	ret = 0;
	int	i = 0;
	int	j = 0;

	void *handle;
	void (* execFun)();
	char libsoname[64];

		/**����������**/
		sql_begin();	

	memset(cExec, 0x0, sizeof(cExec));
	
	/* �������α�  */
	ret = Com_eod_exec_Dec_Sel ( g_pub_tx.reply, "prog='%s'",pgm ); 
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Com_eod_exec_Dec_Sel��,[%d]", ret );
		WRITEMSG
		return(-1);
	}
		
		memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );		
	
		/*  ������˳�����ȡ��һ����¼  */
		ret = Com_eod_exec_Fet_Sel( &sEodExec, g_pub_tx.reply );
		if ( ret == 100 )
		{
			strcpy( sEodExec.sub_tx_code,"RZCS" );
			sEodExec.do_seq=0;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��	Com_eod_exec_Fet_Sel��[%d]", ret );
			WRITEMSG
			return(-1);
		}
	
		pub_base_strpack( sEodExec.prog );
vtcp_log("=========[%s]--",sEodExec.prog );
		/*strcpy( cExecProg[i],sEodExec.prog ); */
		memcpy( &cExec[i],&sEodExec,sizeof(sEodExec) );
vtcp_log("=========[%s]-=",cExec[i].prog );
		iSeq[i] = sEodExec.do_seq;

	Com_eod_exec_Clo_Sel ();

		/**��������**/
		ret=sql_commit();

		memset(libsoname,0x0,sizeof(libsoname));

		sprintf( libsoname,"%s/lib/",getenv("HOME") );
		pub_base_strpack( cExec[j].prog );
		strcat( libsoname, cExec[j].prog );
		strcat( libsoname,".so" );
		pub_base_strpack( libsoname );
vtcp_log("=========[%s]==[%d-%d]",libsoname,i,j );

		memcpy( &grz_com_eod_exec,&cExec[j],sizeof(grz_com_eod_exec) );
		memset(&g_mdm_ac_rel,0x0,sizeof(g_mdm_ac_rel));
		memset(&g_dd_mst,0x0,sizeof(g_dd_mst));
		memset(&g_td_mst,0x0,sizeof(g_td_mst));
		memset(&g_ln_mst,0x0,sizeof(g_ln_mst));
		memset(&g_in_mst,0x0,sizeof(g_in_mst));
		memset(&g_od_mst,0x0,sizeof(g_od_mst));

		memset(&g_dd_parm,0x0,sizeof(g_dd_parm));
		memset(&g_td_parm,0x0,sizeof(g_td_parm));
		memset(&g_ln_parm,0x0,sizeof(g_ln_parm));
		memset(&g_in_parm,0x0,sizeof(g_in_parm));
		memset(&g_od_parm,0x0,sizeof(g_od_parm));

		memset(&g_pub_intst,0x0,sizeof(g_pub_intst));
		set_zd_data(DC_SUB_TX_CODE,cExec[j].sub_tx_code);

		handle = (void *)dlopen(libsoname, RTLD_LAZY|RTLD_GLOBAL);
		if ( handle == NULL )
		{
			sprintf( acErrMsg, "�򿪶�̬�����dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "[%s]�����˳�", cExec[j].prog );
			/*pub_eod_WriJk( "%-40s", acErrMsg );*/
			WRITEMSG

			return(-1);
		}

		execFun = (void(*)(int))dlsym( handle,cExec[j].prog );
		if ( execFun == NULL )
		{
			sprintf( acErrMsg, "����ִ�г������dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "[%s]�����˳�", cExec[j].prog );
	/*		pub_eod_WriJk( "%-40s", acErrMsg );*/

			return(-1);
		}
		else
		{
            sprintf(acErrMsg,"cExecProg[j]=[%s]",cExec[j].prog);
            WRITEMSG
			( *execFun )();		/*---ִ�г���---*/
		}

		if ( dlclose(handle) )
		{
			sprintf( acErrMsg, "�رմ���dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "%s�����˳�", cExec[j].prog );
	/*		pub_eod_WriJk( "%-40s", acErrMsg );*/

			return(-1);
		}

		if (strcmp(g_pub_tx.reply,"0000"))
		{
			return (-1);
		}

		/* д���ռ����־���� */
		pub_base_strpack( cExec[j].prog );
		sprintf( acErrMsg, "%04d %s ���� %s",cExec[j].do_seq,cExec[j].prog_name, cExec[j].prog);
	/*	pub_eod_WriJk( "%-40s", acErrMsg );*/
		WRITEMSG
		

	return(0);
}

