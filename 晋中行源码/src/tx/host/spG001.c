/*************************************************
* �� �� ��: spG001.c
* ���������������������������س��� 
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
#include "data_dic_c.h"

struct com_eod_exec_c grz_com_eod_exec; /**ȫ�֣��������ӳ��򴫵ݿ�������**/
char	cCnt[21];

int spG001( )
{
	int	ret;
	
	memset( cCnt, 0x00, sizeof( cCnt ) );
	
	get_zd_data("0670", cCnt );
	if( cCnt[0]=='S' )
	{
		pub_eod_WriJk("%s",
				"�� ��----------------------------------�� ��");
   		pub_eod_WriJk("%-40s",
				"�� ��            �����Խ�ת            �� ��");
		pub_eod_WriJk("%s",
				"�� ��----------------------------------�� ��");
		sprintf( acErrMsg, "------------�����Խ�ת��ʼ!!!!");
		WRITEMSG
	}
	else
	{
		sprintf( acErrMsg, "------------���մ���ʼ[%s]!!!!",cCnt);
		WRITEMSG
	}
	
	ret = pub_eod_deal();
	if ( ret ) 
	{
		sprintf( acErrMsg, "�������մ������س����[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"����һ�����гɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����һ������ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * ��  �� ����  pub_eod_deal
********************************************************************/
int pub_eod_deal()
{
	struct com_sys_parm_c 	sSysParm;	/* ����ϵͳ������ */
	struct com_eod_exec_c	sEodExec;	/* ���մ���˳��� */
	struct pub_tx v_pub_tx;
	char	cDoType[2];			/* ִ��ʱ�� 0����ǰ 1���� 2���ƺ� */
	int	ret, i;
	
	memset( &sEodExec, 0x00, sizeof( sEodExec ) );
	memset( &sSysParm, 0x00, sizeof( sSysParm ) );
	memset( &v_pub_tx, 0x00, sizeof( v_pub_tx ) );
			
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	memcpy( &v_pub_tx, &g_pub_tx, sizeof( v_pub_tx ) );

	/* ��¼������־��ʼ */
	pub_eod_WriJk("%s","========�� ��==============�� ��========");
   	pub_eod_WriJk("%-40s","���մ���ʼ");

	/**�������ص�����**/
	ret=sql_commit();
	TRACE

	/* �ȴ�����ǰ�ĳ��� 0����ǰ 1���� 2���ƺ� */
	for ( i=0; i<=2; i++ )
	{
		sprintf( cDoType, "%d", i );

		/* ���ݹ���ϵͳ�������ж�״̬�Ƿ���1 */
		if ( pub_base_GetSysparm( &sSysParm ) )
		{
			sprintf( acErrMsg, "ȡ����ϵͳ�������!" );
			WRITEMSG
			goto ErrExit;			
		}
	
		/*** rob *** 20030701 *******/
		set_zd_long( DC_DATE , sSysParm.sys_date );        /* �������� */
		set_zd_data( DC_TEL , "9999" );

		ret = pub_eod_ExecProg( cDoType, sSysParm.sys_date );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ�����մ����ret=[%d]",ret );
			WRITEMSG
			pub_eod_WriJk("%-40s","���մ���ʧ��");
		    pub_eod_WriJk("========�� �� =============�� ��========");
			goto ErrExit;
		}
	}
    pub_eod_WriJk("%-40s","���մ���ɹ�");

	/**����������**/
	sql_begin();	
	TRACE

	/* �޸�˳������м�¼��ɣ���־ΪN */
	/* ����������գ�ֱ���޸�ΪN */
	/*���������,��һ�β��޸ı�־���ڶ����޸� */


	/*--------------------------------------------------------*/
	if( cCnt[0]!='S' )
	{
		ret = sql_execute("update com_eod_exec set finish='N'" );
		if ( ret )
		{
    		pub_eod_WriJk("%-40s","�ָ�����˳����״̬����");
			sprintf( acErrMsg, "�޸����մ���˳����" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "G005" );	
			goto ErrExit;
		}
	}
	else
	{
		ret = sql_execute(
			"update com_eod_exec set finish='N' where eoy_num in('0','2','4')" );
		if ( ret )
		{
    		pub_eod_WriJk("%-40s","�ָ�����˳����״̬����");
			sprintf( acErrMsg, "�޸����մ���˳����" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "G005" );	
			goto ErrExit;
		}
	}
	/*--------------------------------------------------------*/
	
	/* ��¼���ս��� */
    	pub_eod_WriJk("%-40s","�ָ�����˳����״̬�ɹ�");
    pub_eod_WriJk("========�� ��==============�� ��========");
	
OkExit:
	set_zd_data( DC_TEL , v_pub_tx.tel );
	return 0;
ErrExit:
	set_zd_data( DC_TEL , v_pub_tx.tel );
	return -1;
}
/**********************************************************************
 * ��  �� ����  exec_eod_prog
 * ���� ����:   �������մ������ִ�в���¼������־
 * ��  ����
 * ���룺  
 *		char	*do_type	����ִ��ʱ�� 0����ǰ 1���� 2���ƺ�		
 *		long sys_date,		ϵͳ��������
 *
 * �����  
 *
 * ����ֵ��  0:�ɹ�  -1��ʧ��
 *           
 * �޸���ʷ�� 
 *
********************************************************************/
int	pub_eod_ExecProg( char *do_type, long sys_date)
{
	struct com_eod_exec_c sEodExec;
	struct com_eod_exec_c cExec[512];
	int iSeq[256];
	int	ret = 0;
	int	i = 0;
	int	j = 0;

	void *handle;
	void (* execFun)();
	char libsoname[64];

		/**����������**/
	sql_begin();	
	TRACE

	memset(cExec, 0x0, sizeof(cExec));
	
	/* �������α�  */
	ret = Com_eod_exec_Dec_Sel ( g_pub_tx.reply, 
		"do_type = '%s' and using = 'Y' and finish = 'N' order by do_seq", 
		do_type );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Com_eod_exec_Dec_Sel��,[%d]", ret );
		WRITEMSG
		return(-1);
	}
		
	while ( 1 )
	{
		memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );
	
		/*  ������˳�����ȡ��һ����¼  */
		ret = Com_eod_exec_Fet_Sel( &sEodExec, g_pub_tx.reply );
		if ( ret == 100 ) break;
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��	Com_eod_exec_Fet_Sel��[%d]", ret );
			WRITEMSG
			return(-1);
		}
	
		/* �ж��Ƿ��ڽڼ��գ��ڼ����Ƿ�ִ�� */
		/* ������ǽڼ��գ�ִ�У�����ǽڼ��գ����ǽڼ��ձ�־Ϊִ�У�ִ�� */
		ret = pub_eod_chkHoliday( sys_date );
		if ( ret == -1 )	/* ִ�д��� */
		{
			sprintf( acErrMsg, "���ڼ��մ���" );
			WRITEMSG
			return(-1);
		}
		else if ( ret == 1 )
		{
			/* ���ڼ����Ƿ����ִ�� */
			if ( sEodExec.holiday[0] == 'N' )
			{
				goto CONTINUE;
			}	
		}
		
		if( cCnt[0]=='S' ) /**�����***/
		{
			if( sEodExec.eoy_num[0]=='2' ) /**����ʽ����ִ��**/
				continue;
		}
		else /**��ʽ����**/
		{
			if( sEodExec.eoy_num[0]=='1' ) /**�������ִ��**/
				goto CONTINUE;
			if( sEodExec.eoy_num[0]=='4' ) /**�������ִ��**/
				goto CONTINUE;
		}

		/* ����ִ�����ڡ������ڡ���ʼִ�����ںͽ���ִ������ */
		/* �ж��Ƿ���ִ������ */	
		ret = pub_eod_chkSysdate( sys_date, sEodExec.exec_pd,
					sEodExec.pd_month, sEodExec.exec_beg,
					sEodExec.exec_end );
		if ( ret )
		{
			goto CONTINUE;
		}

		pub_base_strpack( sEodExec.prog );
		/*strcpy( cExecProg[i],sEodExec.prog ); */
		memcpy( &cExec[i],&sEodExec,sizeof(sEodExec) );
		iSeq[i] = sEodExec.do_seq;

		i++;

		continue;

	 CONTINUE:
		/* �޸�finish��־ */
		ret = sql_execute("update com_eod_exec set finish = 'Y' \
				where do_seq=%d and prog='%s' ",sEodExec.do_seq,sEodExec.prog );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸�����˳����־��!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"G005");
			return(-1);
		}
	}

	Com_eod_exec_Clo_Sel ();

		/**��������**/
		ret=sql_commit();
		TRACE

	for(j=0; j<i; j++)
	{
		init_data_dic();

		/* д���ռ����־��ʼ */		
		sprintf( acErrMsg, "%04d %s ��ʼ %s",cExec[j].do_seq,cExec[j].prog_name, cExec[j].prog );
		pub_eod_WriJk( "%-40s", acErrMsg );

		memset(libsoname,0x0,sizeof(libsoname));

		sprintf( libsoname,"%s/lib/",getenv("HOME") );
		pub_base_strpack( cExec[j].prog );
		strcat( libsoname, cExec[j].prog );
		strcat( libsoname,".so" );
		pub_base_strpack( libsoname );

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
			pub_eod_WriJk( "%-40s", acErrMsg );

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
			pub_eod_WriJk( "%-40s", acErrMsg );

			return(-1);
		}
		else
		{
            sprintf(acErrMsg,"cExecProg[j]=[%s]",cExec[j].prog);
            WRITEMSG
			( *execFun )();		/*---ִ�г���---*/
		}

		vtcp_log("prog=[%s][%s][%d]",cExec[j].prog,__FILE__,__LINE__);
		if ( dlclose(handle) )
		{
			sprintf( acErrMsg, "�رմ���dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "%s�����˳�", cExec[j].prog );
			pub_eod_WriJk( "%-40s", acErrMsg );

			return(-1);
		}

		vtcp_log("prog=[%s][%s][%d]",cExec[j].prog,__FILE__,__LINE__);
		if (strcmp(g_pub_tx.reply,"0000"))
		{
			vtcp_log("g_pub_tx.reply=[%s][%s][%d]",g_pub_tx.reply,__FILE__,__LINE__);
			return (-1);
		}

		/* д���ռ����־���� */
		pub_base_strpack( cExec[j].prog );
		sprintf( acErrMsg, "%04d %s ���� %s",cExec[j].do_seq,cExec[j].prog_name, cExec[j].prog);
		pub_eod_WriJk( "%-40s", acErrMsg );
		
		/* �޸�����˳����Ƿ����finish��־ */
		/* ���������,���ǵ�ǰִ��˳���һ�ε�,�ж������õĳ��������ζ�ִ��,*/
		/*���ҵ�һ�β�Ҫ��ı��־finish��,���޸�finish��־,ֱ�Ӽ�����һ������*/
		
	vtcp_log("CHANGE IND[%s] [%s]-[%s]",cCnt,cExec[j].eoy_num,cExec[j].prog );
		
		sprintf( acErrMsg, "[%s]�޸�����˳����־!" ,cExec[j].prog );
		WRITEMSG

		/* �޸�finish��־ */
		ret = sql_execute("update com_eod_exec set finish = 'Y' \
				where do_seq = %d and prog = '%s' ", iSeq[j], cExec[j].prog );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸�����˳����־��!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"G005");
			return(-1);
		}
		sql_commit();/*JYW�޸��ύ�ɹ���־*/

		sprintf( acErrMsg, "[%s]�޸�����˳����־�ɹ����ύ111!", cExec[j].prog );
		WRITEMSG
	}/* end for */
	
	return(0);
}
init_data_dic()
{
	struct data_dic_c data_dic;
	int ret;

	ret = Data_dic_Dec_Sel( g_pub_tx.reply , 
		"data_code>'0220' or data_code[1,3]='015'" );
	if (ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret = Data_dic_Fet_Sel( &data_dic , g_pub_tx.reply );
		if( ret==100 ) break;
		if (ret)
		{
			WRITEMSG
			goto ErrExit;
		}
		
		if( data_dic.data_leng==0 ) continue;

		switch( data_dic.data_type[0] )
		{
			case '0':
				set_zd_data( data_dic.data_code,"" );
				break;
			case '1':
				set_zd_long( data_dic.data_code,0 );
				break;
			case '4':
			case '2':
				set_zd_double( data_dic.data_code,0.00 );
				break;
		}
	}
	Data_dic_Clo_Sel();

	vtcp_log("���������ֵ�ok");
	return 0;
ErrExit:
	vtcp_log("���������ֵ�error");
	return 1;
}
